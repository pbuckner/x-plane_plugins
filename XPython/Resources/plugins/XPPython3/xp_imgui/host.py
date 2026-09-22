#####################################
# YOU SHOULD NOT NEED TO MODIFY THIS FILE
#
# ImguiHost -- everything an ImGui integration needs that does NOT depend on
# WHERE the frame is displayed: the imgui context, the style/io/key-map setup,
# the per-frame new_frame/begin/render loop, keyboard translation, and teardown.
#
# Window (window.py) hosts it in an XPLMCreateWindowEx window.
# AvionicsDevice (avionics.py) hosts it in an XPLMCreateAvionicsEx device screen.
#
# Subclasses supply four things -- where the frame goes and how focus works:
#     displaySize()      -> (width, height) in imgui pixels
#     callUserDraw()     -> invoke the plugin's draw callback
#     hasInputFocus()    -> bool
#     requestInputFocus(bool)
# and may override isFrontmost() (default True) and makeRenderer().

from typing import Any, Callable, Optional, Self
import traceback

import imgui
from XPPython3 import xp

# NOTE: neither OpenGL nor .xprenderer is imported here. Both are pulled in by
# makeRenderer() only when contentType selects the OpenGL renderer, so a
# panel-graphics host has no PyOpenGL dependency at all. xprenderer.py carries its
# own "OpenGL not found" message, so that guidance is not lost.


def loge(s: str) -> None:
    """ log with exception traceback """
    xp.log(f'{s}\n{traceback.format_exc()}')


class ImguiHost:
    def __init__(self: Self, draw: Optional[Callable] = None, refCon: Optional[Any] = None,
                 contentType: int = xp.WindowContentTypeOpenGL) -> None:
        if contentType not in (xp.WindowContentTypeOpenGL, xp.WindowContentTypePanelGraphics):
            raise ValueError("xp_imgui contentType must be WindowContentTypeOpenGL or "
                             f"WindowContentTypePanelGraphics, not {contentType}")
        self.drawFunc = draw
        self.refCon = refCon
        self.contentType = contentType
        self.stop = False
        self._releasePending = False   # see setMouse(): latched mouse release
        self.imgui_context = None
        self.renderer = None   # set below; declared first so delete() is safe if __init__ raises

        self.imgui_context = imgui.create_context()
        imgui.set_current_context(self.imgui_context)
        imgui.get_style().window_rounding = 0
        self.io = imgui.get_io()
        self.configureIO()
        self.renderer = self.makeRenderer(contentType)

    # ---- setup ---------------------------------------------------------------

    def configureIO(self: Self) -> None:
        io = self.io
        io.config_mac_osx_behaviors = False
        try:
            io.config_resize_windows_from_edges = False
        except AttributeError:
            io.config_windows_resize_from_edges = False
        io.config_flags = imgui.CONFIG_NAV_NO_CAPTURE_KEYBOARD

        io.key_map[imgui.KEY_TAB] = xp.VK_TAB
        io.key_map[imgui.KEY_LEFT_ARROW] = xp.VK_LEFT
        io.key_map[imgui.KEY_RIGHT_ARROW] = xp.VK_RIGHT
        io.key_map[imgui.KEY_UP_ARROW] = xp.VK_UP
        io.key_map[imgui.KEY_DOWN_ARROW] = xp.VK_DOWN
        io.key_map[imgui.KEY_PAGE_UP] = xp.VK_PRIOR
        io.key_map[imgui.KEY_PAGE_DOWN] = xp.VK_NEXT
        io.key_map[imgui.KEY_HOME] = xp.VK_HOME
        io.key_map[imgui.KEY_END] = xp.VK_END
        io.key_map[imgui.KEY_INSERT] = xp.VK_INSERT
        io.key_map[imgui.KEY_DELETE] = xp.VK_DELETE
        io.key_map[imgui.KEY_BACKSPACE] = xp.VK_BACK
        io.key_map[imgui.KEY_SPACE] = xp.VK_SPACE
        io.key_map[imgui.KEY_ENTER] = xp.VK_ENTER
        io.key_map[imgui.KEY_ESCAPE] = xp.VK_ESCAPE
        io.key_map[imgui.KEY_A] = xp.VK_A
        io.key_map[imgui.KEY_C] = xp.VK_C
        io.key_map[imgui.KEY_V] = xp.VK_V
        io.key_map[imgui.KEY_X] = xp.VK_X
        io.key_map[imgui.KEY_Y] = xp.VK_Y
        io.key_map[imgui.KEY_Z] = xp.VK_Z

    def makeRenderer(self: Self, contentType: int) -> Any:
        """Pick the renderer for contentType. Imported lazily, see note above.

        Subclasses that cannot support a renderer should override and raise --
        XPRenderer, for instance, needs a windowID and the boxel/native matrix
        chain, which an avionics device does not have.
        """
        if contentType == xp.WindowContentTypePanelGraphics:
            try:
                from .pgrenderer import XPPanelGraphicsRenderer
            except ImportError as e:
                raise RuntimeError("WindowContentTypePanelGraphics needs xp_imgui/pgrenderer.py, "
                                   f"which is not available: {e}") from e
            return XPPanelGraphicsRenderer(self)
        from .xprenderer import XPRenderer
        return XPRenderer(self)

    # ---- to be supplied by the host ------------------------------------------

    def displaySize(self: Self) -> tuple[float, float]:
        raise NotImplementedError

    def callUserDraw(self: Self) -> None:
        raise NotImplementedError

    def hasInputFocus(self: Self) -> bool:
        raise NotImplementedError

    def requestInputFocus(self: Self, req: bool) -> None:
        raise NotImplementedError

    def isFrontmost(self: Self) -> bool:
        """Whether it is reasonable to grab keyboard focus. Windows check
        isWindowInFront; a device screen has no equivalent, so it defaults True."""
        return True

    # ---- the frame -----------------------------------------------------------

    def drawFrame(self: Self) -> None:
        """One imgui frame: set up io, run the user's draw callback between
        new_frame/render, and hand the result to the renderer. Hosts call this
        from their own draw callback, after updating whatever displaySize() reads.
        """
        if self.imgui_context is not None:
            imgui.set_current_context(self.imgui_context)
        if self.stop:
            return
        try:
            io = imgui.get_io()
            width, height = self.displaySize()
            if not width or not height:
                return

            hasFocus = self.hasInputFocus()
            if io.want_text_input and not hasFocus and self.isFrontmost():
                self.requestInputFocus(True)
            elif not io.want_text_input and hasFocus:
                self.requestInputFocus(False)
                # reset keysdown otherwise we'll think any keys used to defocus the
                # keyboard are still down!
                for i, _val in enumerate(io.keys_down):
                    io.keys_down[i] = False

            io.display_size = width, height
            io.display_fb_scale = 1.0, 1.0

            imgui.new_frame()
            imgui.set_next_window_position(0.0, 0.0, imgui.ALWAYS)
            imgui.set_next_window_size(width, height, imgui.ALWAYS)
            imgui.begin("X-Plane",
                        flags=imgui.WINDOW_NO_TITLE_BAR | imgui.WINDOW_NO_RESIZE | imgui.WINDOW_NO_COLLAPSE)
            self.callUserDraw()
            imgui.end()
            imgui.render()

            if self.imgui_context is not None:
                imgui.set_current_context(self.imgui_context)
            self.renderer.render(imgui.get_draw_data())

            # imgui has now consumed this frame's button state, so a pending
            # release can safely take effect: the NEXT frame sees the transition
            # and fires the click. See setMouse().
            if self._releasePending:
                io.mouse_down[0] = False
                self._releasePending = False
        except Exception:  # pylint: disable=broad-except
            loge('Exception drawing imgui frame; this host is now stopped')
            self.stop = True

    # ---- input ---------------------------------------------------------------

    def handleKey(self: Self, inKey: int, inFlags: int, inVirtualKey: int, losingFocus: int) -> None:
        if losingFocus:
            self.requestInputFocus(False)
            return
        if self.imgui_context is not None:
            imgui.set_current_context(self.imgui_context)
        io = imgui.get_io()
        if not io.want_text_input:
            return
        # if you press and hold a key, the flags will actually be down, 0, 0, ..., up
        # so the key always has to be considered as pressed unless the up flag is set
        try:
            io.keys_down[inVirtualKey & 0xff] = (inFlags & xp.UpFlag) != xp.UpFlag
        except IndexError:
            loge(f'IndexError for inKey: {inKey}, inVirtualKey: {inVirtualKey} / {inVirtualKey & 0xff}')
            return
        io.key_shift = (inFlags & xp.ShiftFlag) == xp.ShiftFlag
        io.key_alt = (inFlags & xp.OptionAltFlag) == xp.OptionAltFlag
        io.key_ctrl = (inFlags & xp.ControlFlag) == xp.ControlFlag
        try:
            if (inFlags & xp.UpFlag) != xp.UpFlag and not io.key_ctrl and not io.key_alt and chr(inKey).isprintable():
                io.add_input_character(inKey)
        except Exception:  # pylint: disable=broad-except
            loge(f'failed to add key: {inKey}, vk: {inVirtualKey}')

    def setMouse(self: Self, imguiX: float, imguiY: float, down: Optional[bool] = None) -> None:
        """Feed a mouse position (already in imgui space) and optionally button state.

        The release is LATCHED rather than applied immediately. ImGui only counts a
        click when it sees mouse_down true at one new_frame() and false at a later
        one; X-Plane can deliver press and release between two draws, so clearing
        the button here would make imgui miss the press entirely -- widgets
        highlight on hover but never activate. Instead we let the press stand for at
        least one frame and drop it in drawFrame() after render().
        """
        if self.imgui_context is not None:
            imgui.set_current_context(self.imgui_context)
        self.io.mouse_pos = imguiX, imguiY
        if down is not None:
            self.setMouseButton(down)

    def addWheel(self: Self, axis: int, clicks: int) -> None:
        """Accumulate a scroll-wheel event.

        axis is X-Plane's convention (XPLMHandleMouseWheel_f): 0 = vertical,
        1 = horizontal. imgui consumes and zeroes these itself during new_frame,
        so accumulating here is correct -- were that not so, a single scroll would
        run away.
        """
        if self.imgui_context is not None:
            imgui.set_current_context(self.imgui_context)
        if axis == 1:
            self.io.mouse_wheel_horizontal += clicks
        else:
            self.io.mouse_wheel += clicks

    def setMouseButton(self: Self, down: bool) -> None:
        """Change button state WITHOUT touching the position.

        Separate from setMouse() because a release must keep the cursor where it
        was: imgui only fires a widget if the release lands on it.
        """
        if self.imgui_context is not None:
            imgui.set_current_context(self.imgui_context)
        if down:
            self.io.mouse_down[0] = True
            self._releasePending = False
        elif self.io.mouse_down[0]:
            self._releasePending = True         # drop it after imgui has seen it
        else:
            self._releasePending = False

    # ---- teardown ------------------------------------------------------------

    def deleteHost(self: Self) -> None:
        """Release the renderer and the imgui context. Subclasses call this from
        their own delete(), and destroy their window/device separately.

        Order matters: the renderer owns the font texture and reaches through
        io.fonts, so it must shut down while the context is still alive.
        """
        if self.renderer is not None:
            self.renderer.shutdown()
            self.renderer = None
        if self.imgui_context is not None:
            # (Bug in older version of imgui.cpp destroys 'current context' rather
            #  than the one passed it, so we set_current just to be sure.)
            imgui.set_current_context(self.imgui_context)
            imgui.destroy_context(self.imgui_context)
            self.imgui_context = None
