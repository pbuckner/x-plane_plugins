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
#      imgWindow.delete()
#
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
#
# The imgui context, io/key-map setup, frame loop and keyboard translation all
# live in host.ImguiHost, which AvionicsDevice shares. THIS file is only the
# XPLMCreateWindowEx half: window geometry, window callbacks, window focus, and
# the boxel/native coordinate chain that the OpenGL renderer needs.
#
# YOU SHOULD NOT NEED TO MAKE ANY CHANGES TO THIS FILE -- if you do, it's most
# likely you're fixing a bug I should know about, so tell me about it -- support@xppython3.org

from typing import Any, Optional, Self, Callable
from collections import namedtuple
import sys

from XPPython3 import xp
from XPPython3.xp_typing import XPLMWindowID, XPLMCursorStatus
from .host import ImguiHost, loge

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
                                               'handleRightClickFunc',
                                               'contentType'])


class Window(ImguiHost):
    def __init__(self: Self, left: int = 100, top: int = 200, right: int = 200, bottom: int = 100, visible: int = 0,
                 draw: Optional[Callable] = None, refCon: Optional[Any] = None, decoration: int = 1, layer: int = 1,
                 contentType: int = xp.WindowContentTypeOpenGL) -> None:
        self.createWindow_t = CreateWindow_t(left, top, right, bottom, visible, draw, None, None, None, None,
                                             refCon, decoration, layer, None, contentType)
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

        # Builds the imgui context and the renderer. Must run before createWindowEx:
        # the window can be drawn as soon as it exists.
        super().__init__(draw=draw, refCon=refCon, contentType=contentType)

        # Here, we create the XP window, but pass to it "our" callbacks (e.g., self.drawWindow, self.handleMouseClick)
        # "Our" callbacks will do whatever they need to do and then call the user-provided callbacks
        self.windowID = xp.createWindowEx(
            self.createWindow_t.left, self.createWindow_t.top, self.createWindow_t.right, self.createWindow_t.bottom,
            self.createWindow_t.visible,
            self.imguiDrawWindow, self.imguiHandleMouseClick, self.imguiHandleKey,
            self.imguiHandleCursor, self.imguiHandleMouseWheel, self.createWindow_t.refcon,
            self.createWindow_t.decorateAsFloatingWindow, self.createWindow_t.layer,
            self.imguiHandleRightClick,
            contentType=self.createWindow_t.contentType)
        xp.bringWindowToFront(self.windowID)

    def delete(self: Self) -> None:
        self.stop = True
        try:
            self.deleteHost()          # renderer + imgui context, in that order
            if self.windowID is not None:
                xp.destroyWindow(self.windowID)
                self.windowID = None
        except Exception:  # pylint: disable=broad-except
            loge("Exception while trying to delete window")

    def setTitle(self: Self, title: str) -> None:
        if self.windowID is not None:
            xp.setWindowTitle(self.windowID, title)

    # ---- ImguiHost hooks -----------------------------------------------------

    def displaySize(self: Self) -> tuple[float, float]:
        return self.mRight - self.mLeft, self.mTop - self.mBottom

    def callUserDraw(self: Self) -> None:
        if self.createWindow_t.drawWindowFunc:
            self.createWindow_t.drawWindowFunc(self.windowID, self.createWindow_t.refcon)

    def hasInputFocus(self: Self) -> bool:
        if self.windowID is None:
            return False
        return bool(xp.hasKeyboardFocus(self.windowID))

    def requestInputFocus(self: Self, req: bool) -> None:
        if self.windowID is not None:
            xp.takeKeyboardFocus(self.windowID if req else 0)

    def isFrontmost(self: Self) -> bool:
        return self.windowID is not None and bool(xp.isWindowInFront(self.windowID))

    # ---- X-Plane window callbacks -------------------------------------------

    def imguiDrawWindow(self: Self, windowID: XPLMWindowID, inRefCon: Any) -> None:
        # updateMatrices + geometry FIRST: the OpenGL renderer reads both, and
        # displaySize()/translateToImguiSpace() are built from the geometry.
        self.updateMatrices()
        geom = xp.getWindowGeometry(windowID)
        if geom is not None:
            (self.mLeft, self.mTop, self.mRight, self.mBottom) = geom
        self.drawFrame()

    def imguiHandleMouseClick(self: Self, _inWindowID: int, x: int, y: int, inMouse: int, _inRefCon: Any) -> int:
        imguiX, imguiY = self.translateToImguiSpace(x, y)
        self.setMouse(imguiX, imguiY, down=inMouse in (xp.MouseDown, xp.MouseDrag))
        # Return 1 to indicate we've handled the mouse click
        return 1

    def imguiHandleRightClick(self: Self, _inWindowID: XPLMWindowID, _x, _y, _inMouse, _inRefCon) -> int:
        return 1

    def imguiHandleCursor(self: Self, _inWindowID: XPLMWindowID, x: int, y: int, _inRefCon: Any) -> XPLMCursorStatus:
        """Hover tracking. Fires while the cursor is over the window with no button
        down, which is what imgui needs for hover states -- previously this returned
        without touching io, so mouse_pos only ever updated on a click."""
        imguiX, imguiY = self.translateToImguiSpace(x, y)
        self.setMouse(imguiX, imguiY)
        return xp.CursorDefault

    def imguiHandleMouseWheel(self: Self, _inWindowID: XPLMWindowID, x: int, y: int,
                              wheel: int, clicks: int, _inRefCon: Any) -> int:
        """Feed the wheel to imgui so scrollable regions work -- previously the
        wheel value was discarded and io.mouse_wheel was never set."""
        imguiX, imguiY = self.translateToImguiSpace(x, y)
        self.setMouse(imguiX, imguiY)
        self.addWheel(wheel, clicks)
        return 1

    def imguiHandleKey(self: Self, _inWindowID: XPLMWindowID, inKey: int, inFlags: int, inVirtualKey: int,
                       _inRefCon: Any, losingFocus: int) -> None:
        self.handleKey(inKey, inFlags, inVirtualKey, losingFocus)

    # ---- coordinates ---------------------------------------------------------

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

    def updateMatrices(self: Self) -> None:
        xp.getDatavf(self.modelviewMatrixRef, self.modelview, 0, 16)
        xp.getDatavf(self.projectionMatrixRef, self.projection, 0, 16)
        xp.getDatavi(self.viewportRef, self.viewport, 0, 4)
