##############################################
# To use:
# 1) create a draw callback which includes imgui code:
#      def drawCallback(windowID, refCon):
#          imgui.text('label')
#          if imgui.button("Press Me"):
#             do_something()
#
# 2) At some point (perhaps in response to menu selection)
#    Create an xp_imgui.Window() instance, passing in your drawCallback
#      imgWindow = Window([..., drawCallback, ...])
#      imgWindow.setTitle('<string>')
#
#    When the window is displayed, it will include imgui widgets
#
# 3) When you're done with the window, destroy it
#      ...
#      imgWindow.destroy()
#
# Create an IMGUI window by creating instance of Window() class
#   requires single parameter, which is 14 element list identical to
#   list you would otherwise provide to XPLMCreateWindowEx (that is:
#   position, reference constant, and callback functions)
# Once created, you will do your imgui drawing in your drawWindow callback
#   When you're called, we'll have already set up the environment:
#      set_current_context()
#      new_frame()
#      begin()
#        ... your callback here
#      end()
#      render()
# You callback will receive the XP windowID, and whatever you provided as
#   the reference constant
# You can set the window title using .setTitle(title=<string>)
# At program end, you should destroy the IMGUI window calling .destroy()
#
# YOU SHOULD NOT NEED TO MAKE ANY CHANGES TO THIS FILE -- if you do, it's most
# likely you're fixing a bug I should know about, so tell me about it -- support@xppython3.org

from typing import Any, Optional, Self, Callable
import traceback
from collections import namedtuple
import sys
try:
    import OpenGL.GL as GL
except ImportError:
    print("[XPPython3] OpenGL not found. Use XPPython3 Pip Package Installer to install 'pyopengl' package and restart.")
    raise


import imgui
from XPPython3 import xp
from XPPython3.xp_typing import XPLMWindowID, XPLMCursorStatus
from .xprenderer import XPRenderer


CreateWindow_t = namedtuple('CreateWindow_t', ['left', 'top', 'right', 'bottom',
                                               'visible',
                                               'drawWindowFunc',
                                               'handleMouseClickFunc',
                                               'handleKeyFunc',
                                               'handleCursorFunc',
                                               'handleMouseWheelFunc',
                                               'refcon',
                                               'decorateAsFloatingWindow',
                                               'layer',
                                               'handleRightClickFunc'])


def loge(s: str) -> None:
    """ log with exception traceback """
    xp.log(f'{s}\n{traceback.format_exc()}')


class Window:
    def __init__(self: Self, left: int = 100, top: int = 200, right: int = 200, bottom: int = 100, visible: int = 0,
                 draw: Optional[Callable] = None, refCon: Optional[Any] = None, decoration: int = 1, layer: int = 1) -> None:
        self.createWindow_t = CreateWindow_t(left, top, right, bottom, visible, draw, None, None, None, None,
                                             refCon, decoration, layer, None)
        self.stop = False
        self.imgui_context = None
        self.fontTextureId = None
        self.windowID = None
        self.modelviewMatrixRef = xp.findDataRef("sim/graphics/view/modelview_matrix")
        self.viewportRef = xp.findDataRef("sim/graphics/view/viewport")
        self.projectionMatrixRef = xp.findDataRef("sim/graphics/view/projection_matrix")
        self.modelview: list[float] = []
        self.projection: list[float] = []
        self.viewport: list[int] = []

        # Set in imguiDrawWindow by call from xp.getWindowGeometry()
        self.mLeft = 0
        self.mRight = 0
        self.mTop = 0
        self.mBottom = 0

        # Initialize
        self.imgui_context = imgui.create_context()
        imgui.set_current_context(self.imgui_context)
        imgui.get_style().window_rounding = 0

        self.io = imgui.get_io()
        # self.io.ini_file_name = '/dev/null'
        self.io.config_mac_osx_behaviors = False
        try:
            self.io.config_resize_windows_from_edges = False
        except AttributeError:
            self.io.config_windows_resize_from_edges = False

        # self.io.ini_file_name = None
        self.io.config_flags = imgui.CONFIG_NAV_NO_CAPTURE_KEYBOARD

        # self.io.KeyMap here
        self.io.key_map[imgui.KEY_TAB] = xp.VK_TAB
        self.io.key_map[imgui.KEY_LEFT_ARROW] = xp.VK_LEFT
        self.io.key_map[imgui.KEY_RIGHT_ARROW] = xp.VK_RIGHT
        self.io.key_map[imgui.KEY_UP_ARROW] = xp.VK_UP
        self.io.key_map[imgui.KEY_DOWN_ARROW] = xp.VK_DOWN
        self.io.key_map[imgui.KEY_PAGE_UP] = xp.VK_PRIOR
        self.io.key_map[imgui.KEY_PAGE_DOWN] = xp.VK_NEXT
        self.io.key_map[imgui.KEY_HOME] = xp.VK_HOME
        self.io.key_map[imgui.KEY_END] = xp.VK_END
        self.io.key_map[imgui.KEY_INSERT] = xp.VK_INSERT
        self.io.key_map[imgui.KEY_DELETE] = xp.VK_DELETE
        self.io.key_map[imgui.KEY_BACKSPACE] = xp.VK_BACK
        self.io.key_map[imgui.KEY_SPACE] = xp.VK_SPACE
        self.io.key_map[imgui.KEY_ENTER] = xp.VK_ENTER
        self.io.key_map[imgui.KEY_ESCAPE] = xp.VK_ESCAPE
        self.io.key_map[imgui.KEY_A] = xp.VK_A
        self.io.key_map[imgui.KEY_C] = xp.VK_C
        self.io.key_map[imgui.KEY_V] = xp.VK_V
        self.io.key_map[imgui.KEY_X] = xp.VK_X
        self.io.key_map[imgui.KEY_Y] = xp.VK_Y
        self.io.key_map[imgui.KEY_Z] = xp.VK_Z

        self.renderer = XPRenderer(self)

        # Here, we create the XP window, but pass to it "our" callbacks (e.g., self.drawWindow, self.handleMouseClick)
        # "Our" callbacks will do whatever they need to do and then call the user-provided callbacks
        self.windowID = xp.createWindowEx(
            self.createWindow_t.left, self.createWindow_t.top, self.createWindow_t.right, self.createWindow_t.bottom,
            self.createWindow_t.visible,
            self.imguiDrawWindow, self.imguiHandleMouseClick, self.imguiHandleKey,
            self.imguiHandleCursor, self.imguiHandleMouseWheel, self.createWindow_t.refcon,
            self.createWindow_t.decorateAsFloatingWindow, self.createWindow_t.layer,
            self.imguiHandleRightClick)
        xp.bringWindowToFront(self.windowID)

    def delete(self: Self) -> None:
        self.stop = True
        try:
            if self.imgui_context is not None:
                # (Bug in older version of imgui.cpp destroys 'current context' rather than the one passed it
                #  so we'll set_current just to be sure.)
                imgui.set_current_context(self.imgui_context)
                imgui.destroy_context(self.imgui_context)
                self.imgui_context = None
            if self.fontTextureId is not None:
                GL.glDeleteTextures(1, [self.fontTextureId])
                self.fontTextureId = None
            if self.windowID is not None:
                xp.destroyWindow(self.windowID)
                self.windowID = None
        except Exception:  # pylint: disable=broad-except
            loge("Exception while trying to delete window")

    def imguiHandleMouseClick(self: Self, _inWindowID: int, x: int, y: int, inMouse: int, _inRefCon: Any) -> int:
        # if not xp.hasKeyboardFocus(inWindowID):
        #     xp.takeKeyboardFocus(inWindowID)
        # First, handle imgui mouse click
        if self.imgui_context:
            imgui.set_current_context(self.imgui_context)
        imguiX, imguiY = self.translateToImguiSpace(x, y)
        self.io.mouse_pos = imguiX, imguiY
        if inMouse in (xp.MouseDown, xp.MouseDrag):
            self.io.mouse_down[0] = True
        else:
            self.io.mouse_down[0] = False

        # Finally, return 1 to indicate we've handled the mouse click
        return 1

    def setTitle(self: Self, title: str) -> None:
        if self.windowID is not None:
            xp.setWindowTitle(self.windowID, title)

    def imguiDrawWindow(self: Self, windowID: XPLMWindowID, inRefCon: Any):
        self.updateMatrices()
        if self.imgui_context is not None:
            imgui.set_current_context(self.imgui_context)
        if self.stop:
            return
        try:
            try:
                # Set site of the window into which we'll be writing imgui
                geom = xp.getWindowGeometry(windowID)
                if geom is not None:
                    (self.mLeft, self.mTop, self.mRight, self.mBottom) = geom
                io = imgui.get_io()

                hasKeyboardFocus = self.hasInputFocus()
                if io.want_text_input and not hasKeyboardFocus and xp.isWindowInFront(windowID):
                    self.requestInputFocus(True)
                elif not io.want_text_input and hasKeyboardFocus:
                    self.requestInputFocus(False)
                    # reset keysdown otherwise we'll think any keys used to defocus the keyboard are still down!
                    for i, _val in enumerate(io.keys_down):
                        io.keys_down[i] = False

                io.display_size = self.mRight - self.mLeft, self.mTop - self.mBottom
                io.display_fb_scale = 1.0, 1.0
                try:
                    imgui.new_frame()
                    imgui.set_next_window_position(0.0, 0.0, imgui.ALWAYS)
                    imgui.set_next_window_size(self.mRight - self.mLeft, self.mTop - self.mBottom, imgui.ALWAYS)
                except Exception as e:  # pylint: disable=broad-except
                    xp.log(f"Failure with window pos / size: {e}")
                    raise

                imgui.begin("X-Plane",
                            flags=imgui.WINDOW_NO_TITLE_BAR | imgui.WINDOW_NO_RESIZE | imgui.WINDOW_NO_COLLAPSE)
                if self.createWindow_t.drawWindowFunc:
                    self.createWindow_t.drawWindowFunc(windowID, inRefCon)
                imgui.end()
                imgui.render()

                if self.imgui_context is not None:
                    imgui.set_current_context(self.imgui_context)
                self.renderer.render(imgui.get_draw_data())
            except Exception:  # pylint: disable=broad-except
                loge('oops')
                self.stop = True
                return
        except Exception:
            loge("Exception in drawwindow")
            raise

    def hasInputFocus(self: Self) -> int:
        if self.windowID is None:
            return False
        return xp.hasKeyboardFocus(self.windowID)

    def requestInputFocus(self: Self, req: bool) -> None:
        if self.windowID is not None:
            xp.takeKeyboardFocus(self.windowID if req else 0)

    def imguiHandleRightClick(self: Self, _inWindowID: XPLMWindowID, _x, _y, _inMouse, _inRefCon) -> int:
        return 1

    def imguiHandleCursor(self: Self, _inWindowID: XPLMWindowID, _x: int, _y: int, _inRefCon: Any) -> XPLMCursorStatus:
        return xp.CursorDefault

    def imguiHandleMouseWheel(self: Self, _inWindowID: XPLMWindowID, _x, _y, _wheel, _clicks, _inRefCon: Any) -> int:
        return 1

    def imguiHandleKey(self: Self, _inWindowID: XPLMWindowID, inKey: int, inFlags: int, inVirtualKey: int, _inRefCon: Any, losingFocus: int) -> None:
        if losingFocus:
            self.requestInputFocus(False)
            return
        if self.imgui_context is not None:
            imgui.set_current_context(self.imgui_context)
        io = imgui.get_io()
        if io.want_text_input:
            # if you press and hold a key, the flags wiil actually be down, 0, 0, ..., up
            # so the key always has to be considered as pressed unless the up flag is set
            try:
                # print("inKey: {}, virtualKey: {}, Description: {}".format(inKey, inVirtualKey, xp.getVirtualKeyDescription(inVirtualKey & 0xff)))
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
        return

    def translateToImguiSpace(self: Self, x: float, y: float) -> tuple[float, float]:
        imguiX = x - self.mLeft
        if imguiX < 0 or imguiX > (self.mRight - self.mLeft):
            return -sys.float_info.max, -sys.float_info.max
        imguiY = self.mTop - y
        if imguiY < 0 or imguiY > (self.mTop - self.mBottom):
            return -sys.float_info.max, -sys.float_info.max
        return imguiX, imguiY

    def translateImguiToBoxel(self: Self, x: float, y: float) -> tuple[int, int]:
        return int(self.mLeft + x), int(self.mTop - y)

    def boxelsToNative(self: Self, x: float, y: float) -> tuple[int, int]:
        # vrEnabledRef = XPLMFindDataRef("sim/graphics/VR/enabled");
        try:
            boxelPos = (float(x), float(y), 0, 1)

            eye = self.multMatrixVec4f(self.modelview, boxelPos)
            ndc = self.multMatrixVec4f(self.projection, eye)
            outX = int((ndc[0] * 0.5 + 0.5) * self.viewport[2] + self.viewport[0])
            outY = int((ndc[1] * 0.5 + 0.5) * self.viewport[3] + self.viewport[1])
        except Exception as e:  # pylint: disable=broad-except
            xp.log(f"Exception in Boxesl to Native: {e}")
            raise
        return outX, outY

    @staticmethod
    def multMatrixVec4f(m: list[float], v: tuple[float, float, float, float]) -> tuple[float, float, float, float]:
        return (v[0] * m[0] + v[1] * m[4] + v[2] * m[8] + v[3] * m[12],
                v[0] * m[1] + v[1] * m[5] + v[2] * m[9] + v[3] * m[13],
                v[0] * m[2] + v[1] * m[6] + v[2] * m[10] + v[3] * m[14],
                v[0] * m[3] + v[1] * m[7] + v[2] * m[11] + v[3] * m[15])

    def updateMatrices(self: Self):
        xp.getDatavf(self.modelviewMatrixRef, self.modelview, 0, 16)
        xp.getDatavf(self.projectionMatrixRef, self.projection, 0, 16)
        xp.getDatavi(self.viewportRef, self.viewport, 0, 4)
