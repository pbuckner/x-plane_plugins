#####################################
# YOU SHOULD NOT NEED TO MODIFY THIS FILE
# Nor should you need to call it directly as it is normally called only
# by xp_imgui.Window(). You should be using xp_imgui.Window()
#
# This defines XPPanelGraphicsRenderer, the XPLM440 panel-graphics counterpart to
# XPRenderer: instead of issuing OpenGL calls it hands each ImGui draw list to
# xp.drawCalls(), which X-Plane renders natively. Used when a window is created
# with contentType=xp.WindowContentTypePanelGraphics.
#
# It touches NO OpenGL. A panel-graphics imgui window does not need PyOpenGL.
#
# Why this is so much shorter than xprenderer.py: XPLMDrawCalls consumes
# ImDrawData's own layout and coordinate space directly. Positions and scissors
# are window-LOCAL pixels with a TOP-LEFT origin -- exactly ImGui's convention --
# so the whole modelview/projection/boxel/native conversion chain the OpenGL path
# needs (updateMatrices, boxelsToNative, translateImguiToBoxel) is not used here.
# The host flips Y for us.

from typing import Any, Optional, Self
import ctypes

import imgui


from XPPython3 import xp
from XPPython3.imgui_typing import IMGUIDrawData
from .profiling import makeProfiler

# Set True to log per-frame CPU timings for this path. See profiling.py.
PROFILE = False

# ImDrawVert is 5 floats / 20 bytes, which is what XPLMDrawCalls requires.
_VERTEX_SIZE = 20


def _bufferAt(address: int, nbytes: int) -> Any:
    """Zero-copy buffer-protocol view onto memory ImGui already owns.

    xp.drawCalls() takes it via PyObject_GetBuffer, so nothing is copied. The view
    is only valid while ImGui's draw data is alive -- never retain one past the
    render call that made it.
    """
    return (ctypes.c_char * nbytes).from_address(address)


class XPPanelGraphicsRenderer:
    """Renders ImGui frames through xp.drawCalls().

    Implements the same three methods as pyimgui's renderers -- refresh_font_texture(),
    render() and shutdown() -- but deliberately does NOT subclass BaseOpenGLRenderer,
    so no OpenGL is imported.
    """

    def __init__(self: Self, window: Any) -> None:
        if not imgui.get_current_context():
            raise RuntimeError("No valid ImGui context. Use imgui.create_context() first "
                               "and/or imgui.set_current_context().")
        self.window = window
        self.io = imgui.get_io()
        self.io.delta_time = 1.0 / 60.0

        # ImGui identifies textures by an integer; xp.drawCalls wants a capsule.
        # Map one to the other. Keys start at 1: ImGui treats 0 as "no texture".
        self._textures: dict[int, Any] = {}
        self._nextTextureKey = 1
        self._fontTextureKey: Optional[int] = None

        self._warnedIndexRange = False
        self._warnedDisplayPos = False
        self._prof = makeProfiler(PROFILE, "panelgraphics")
        self._skippedThisFrame = 0

        if imgui.INDEX_SIZE not in (2, 4):
            raise RuntimeError(f"unsupported imgui.INDEX_SIZE {imgui.INDEX_SIZE}; expected 2 or 4")

        self.refresh_font_texture()

    # ---- textures ------------------------------------------------------------

    def registerTexture(self: Self, rgba: bytes, width: int, height: int) -> int:
        """Upload an RGBA8 image and return the integer key to use as an ImGui
        texture id. Call from OUTSIDE a draw callback -- xp.createTexture() is not
        legal inside one.
        """
        key = self._nextTextureKey
        self._nextTextureKey += 1
        self._textures[key] = xp.createTexture(rgba, width, height)
        return key

    def refresh_font_texture(self: Self) -> None:
        """Upload ImGui's font atlas.

        RGBA32, not the alpha8 the OpenGL path uses: xp.createTexture() accepts
        RGBA8 only. Costs 4x the atlas memory, and there is no way around it.

        MUST NOT run inside a draw callback (xp.createTexture()). It is called from
        __init__, i.e. from Window.__init__, which is safe. If you add fonts later
        you must re-call it from outside the draw callback too.
        """
        if self._fontTextureKey is not None:
            self.destroyTexture(self._fontTextureKey)
            self._fontTextureKey = None

        width, height, pixels = self.io.fonts.get_tex_data_as_rgba32()
        self._fontTextureKey = self.registerTexture(bytes(pixels), width, height)
        self.io.fonts.texture_id = self._fontTextureKey
        self.io.fonts.clear_tex_data()

    def destroyTexture(self: Self, key: int) -> None:
        tex = self._textures.pop(key, None)
        if tex is not None:
            xp.destroyTexture(tex)

    def shutdown(self: Self) -> None:
        """Release every texture. Called from Window.delete() while the imgui
        context is still alive (we touch self.io.fonts).
        """
        for key in list(self._textures):
            self.destroyTexture(key)
        self._fontTextureKey = None
        try:
            self.io.fonts.texture_id = 0
        except Exception:  # pylint: disable=broad-except
            pass   # context already gone; nothing left to detach the texture from

    # ---- rendering -----------------------------------------------------------

    def render(self: Self, draw_data: IMGUIDrawData) -> None:
        display_width, display_height = self.io.display_size
        if not display_width or not display_height:
            return

        # NOTE: unlike the OpenGL path we do NOT call draw_data.scale_clip_rects().
        # That scales logical coords into framebuffer pixels, which is what glScissor
        # wants; xp.drawCalls wants window-LOCAL LOGICAL pixels, so scaling here would
        # be wrong on any display where display_fb_scale is not 1.

        # ImGui vertex positions are relative to display_pos. window.py never sets it,
        # so it should be (0, 0) and positions are already window-local. Warn rather
        # than silently mis-place everything if that ever changes.
        if not self._warnedDisplayPos:
            pos = draw_data.display_pos
            if (pos[0], pos[1]) != (0.0, 0.0):
                xp.log(f"[xp_imgui] display_pos is {pos}, expected (0, 0); "
                       "panel-graphics positions will be offset")
                self._warnedDisplayPos = True

        prof = self._prof
        self._skippedThisFrame = 0
        frameStart = prof.now() if prof else 0.0

        for commands in draw_data.commands_lists:
            if not commands.vtx_buffer_size or not commands.idx_buffer_size:
                continue

            started = prof.now() if prof else 0.0
            # Zero-copy views onto ImGui's own buffers: a ctypes array supports the
            # buffer protocol, so xp.drawCalls() reads them in place. Valid only for
            # the duration of this call -- do not retain them. The 32->16 index
            # narrowing that used to happen here is now done in C via indexSize.
            vertices = _bufferAt(commands.vtx_buffer_data,
                                 commands.vtx_buffer_size * _VERTEX_SIZE)
            indices = _bufferAt(commands.idx_buffer_data,
                                commands.idx_buffer_size * imgui.INDEX_SIZE)
            if prof:
                prof.add('extract', started)

            started = prof.now() if prof else 0.0
            calls = []
            idx_offset = 0
            for command in commands.commands:
                scissor = self._clipRect(command.clip_rect, display_width, display_height)
                if scissor is not None and command.elem_count:
                    calls.append((self._textures.get(command.texture_id),
                                  scissor,
                                  idx_offset,
                                  command.elem_count,
                                  0))          # vtx_offset: see _indicesAsUint16
                idx_offset += command.elem_count
            if prof:
                prof.add('build', started)

            if calls:
                started = prof.now() if prof else 0.0
                try:
                    xp.drawCalls(vertices, indices, calls, indexSize=imgui.INDEX_SIZE)
                except ValueError:
                    # An index above 65535: the C side refuses to narrow it. Splitting
                    # on vtx_offset is not implemented, so drop this list.
                    self._warnIndexRange()
                    self._skippedThisFrame += 1
                if prof:
                    prof.add('submit', started)

        if prof:
            prof.add('total', frameStart)
            prof.frame(draw_data.total_vtx_count, draw_data.total_idx_count,
                       self._skippedThisFrame)

    def _warnIndexRange(self: Self) -> None:
        if not self._warnedIndexRange:
            xp.log("[xp_imgui] draw list has an index above 65535; XPLMDrawCalls is "
                   "uint16-only and vtx_offset splitting is not implemented. "
                   "Draw list skipped. Reduce what is drawn in one window.")
            self._warnedIndexRange = True

    @staticmethod
    def _clipRect(clip_rect: Any, display_width: float, display_height: float):
        """ImGui clip rect -> XPLMDrawCall_t scissor, or None if it clips everything.

        Both are (left, top, right, bottom) with a top-left origin, so this is only
        a clamp: ImGui uses very large values to mean 'no clipping', and the SDK
        expects a real rect. Zero-area rects produce no output, so skip them.
        """
        left = max(0.0, min(float(clip_rect[0]), display_width))
        top = max(0.0, min(float(clip_rect[1]), display_height))
        right = max(0.0, min(float(clip_rect[2]), display_width))
        bottom = max(0.0, min(float(clip_rect[3]), display_height))
        if right <= left or bottom <= top:
            return None
        return (left, top, right, bottom)
