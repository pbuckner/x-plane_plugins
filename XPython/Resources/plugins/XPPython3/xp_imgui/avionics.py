##############################################
# To use:
# 1) create a draw callback which includes imgui code:
#      def drawCallback(avionicsID, refCon):
#          imgui.text('label')
#          if imgui.button("Press Me"):
#             do_something()
#
# 2) Create an xp_imgui.AvionicsDevice(), passing in your drawCallback
#      device = AvionicsDevice(screenWidth=400, screenHeight=300,
#                              draw=drawCallback, deviceID='myplugin.pfd')
#      device.popup(True)          # show the 2-D pop-up
#
# 3) When you're done with the device, destroy it
#      device.delete()
#
# This is the XPLMCreateAvionicsEx half of the imgui integration; the context,
# frame loop and keyboard translation are shared with Window via host.ImguiHost.
#
# PANEL GRAPHICS ONLY. The OpenGL renderer needs a windowID, window geometry and
# the boxel/native matrix chain, none of which a device has -- so contentType is
# forced to WindowContentTypePanelGraphics and anything else is rejected.
#
# Where your imgui appears: the device SCREEN. That screen shows up wherever the
# device does -- in the 2-D pop-up, and on any 3-D object bound with
# setObjectAvionics(). The BEZEL is not imgui: it is drawn by your own bezelDraw
# callback with plain panel-graphics calls, and only while the pop-up is visible.
#
# YOU SHOULD NOT NEED TO MAKE ANY CHANGES TO THIS FILE -- if you do, it's most
# likely you're fixing a bug I should know about, so tell me about it -- support@xppython3.org

from typing import Any, Callable, Optional, Self

from XPPython3 import xp
from XPPython3.xp_typing import XPLMCursorStatus, XPLMWindowContentType
from .host import ImguiHost, loge


class AvionicsDevice(ImguiHost):
    def __init__(self: Self,
                 screenWidth: int = 400, screenHeight: int = 300,
                 bezelWidth: Optional[int] = None, bezelHeight: Optional[int] = None,
                 screenOffsetX: Optional[int] = None, screenOffsetY: Optional[int] = None,
                 draw: Optional[Callable] = None, refCon: Optional[Any] = None,
                 bezelDraw: Optional[Callable] = None, brightness: Optional[Callable] = None,
                 deviceID: str = 'xppython3.imgui.device', deviceName: str = 'imgui device',
                 bezelPad: int = 20,
                 contentType: XPLMWindowContentType = xp.WindowContentTypePanelGraphics) -> None:
        if contentType != xp.WindowContentTypePanelGraphics:
            raise ValueError("xp_imgui.AvionicsDevice requires "
                             "contentType=WindowContentTypePanelGraphics; the OpenGL renderer "
                             "needs a window, which a device does not have")
        self.screenWidth = screenWidth
        self.screenHeight = screenHeight
        self.bezelWidth = bezelWidth if bezelWidth is not None else screenWidth + 2 * bezelPad
        self.bezelHeight = bezelHeight if bezelHeight is not None else screenHeight + 2 * bezelPad
        self.screenOffsetX = screenOffsetX if screenOffsetX is not None else bezelPad
        self.screenOffsetY = screenOffsetY if screenOffsetY is not None else bezelPad
        self.bezelDrawFunc = bezelDraw
        self.brightnessFunc = brightness
        self.deviceID = deviceID
        self.avionicsID = None
        self.lastRaw = (0, 0)     # last unconverted touch/cursor coords, for probes

        # Builds the imgui context and the renderer, before the device exists: the
        # device can be drawn as soon as it is created.
        super().__init__(draw=draw, refCon=refCon, contentType=contentType)

        self.avionicsID = xp.createAvionicsEx(
            screenWidth=screenWidth, screenHeight=screenHeight,
            bezelWidth=self.bezelWidth, bezelHeight=self.bezelHeight,
            screenOffsetX=self.screenOffsetX, screenOffsetY=self.screenOffsetY,
            drawOnDemand=0,                       # imgui needs a frame every frame
            screenDraw=self.imguiScreenDraw,
            bezelDraw=self.imguiBezelDraw,
            screenTouch=self.imguiScreenTouch,
            screenRightTouch=self.imguiScreenRightTouch,
            screenScroll=self.imguiScreenScroll,
            screenCursor=self.imguiScreenCursor,
            keyboard=self.imguiKeyboard,
            brightness=self.imguiBrightness,
            deviceID=deviceID, deviceName=deviceName,
            refCon=refCon,
            contentType=contentType)

    def delete(self: Self) -> None:
        self.stop = True
        try:
            self.deleteHost()          # renderer + imgui context, in that order
            if self.avionicsID is not None:
                xp.destroyAvionics(self.avionicsID)
                self.avionicsID = None
        except Exception:  # pylint: disable=broad-except
            loge("Exception while trying to delete avionics device")

    def popup(self: Self, visible: bool = True) -> None:
        if self.avionicsID is not None:
            xp.setAvionicsPopupVisible(self.avionicsID, 1 if visible else 0)

    def isPopupVisible(self: Self) -> bool:
        return self.avionicsID is not None and bool(xp.isAvionicsPopupVisible(self.avionicsID))

    # ---- ImguiHost hooks -----------------------------------------------------

    def displaySize(self: Self) -> tuple[float, float]:
        return self.screenWidth, self.screenHeight

    def callUserDraw(self: Self) -> None:
        if self.drawFunc:
            self.drawFunc(self.avionicsID, self.refCon)

    def hasInputFocus(self: Self) -> bool:
        if self.avionicsID is None:
            return False
        return bool(xp.hasAvionicsKeyboardFocus(self.avionicsID))

    def requestInputFocus(self: Self, req: bool) -> None:
        """Take keyboard focus for the pop-up.

        Asymmetric with the window case, and deliberately so: XPLMDisplay offers
        takeAvionicsKeyboardFocus(avionicsID) with no release form -- there is no
        avionics equivalent of takeKeyboardFocus(0). So releasing is a no-op and we
        let X-Plane drop focus when the pop-up is hidden or something else takes it.
        Focus also only exists for the POP-UP: typing is not routed to a device
        drawn on a 3-D object or in the panel.
        """
        if req and self.avionicsID is not None:
            xp.takeAvionicsKeyboardFocus(self.avionicsID)

    # ---- X-Plane device callbacks -------------------------------------------

    def imguiScreenDraw(self: Self, _refCon: Any) -> None:
        self.drawFrame()

    def imguiBezelDraw(self: Self, r: float, g: float, b: float, refCon: Any) -> None:
        """Bezel is NOT imgui -- pass it straight through to the plugin.

        Called only while the 2-D pop-up is visible. r/g/b are the ambient tint
        X-Plane wants the bezel modulated by.
        """
        if self.bezelDrawFunc:
            self.bezelDrawFunc(r, g, b, refCon)

    def imguiBrightness(self: Self, rheoValue: float, ambientBrightness: float,
                        busVoltsRatio: float, refCon: Any) -> float:
        """Screen brightness. Defaults to full: a device bound to an OBJECT reports
        busVoltsRatio == -1 (it is not on the aircraft bus), and X-Plane's default
        behaviour would leave such a screen dark."""
        if self.brightnessFunc:
            return self.brightnessFunc(rheoValue, ambientBrightness, busVoltsRatio, refCon)
        return 1.0

    def toImguiSpace(self: Self, x: int, y: int) -> tuple[float, float]:
        """Screen-touch coords are device-local with a BOTTOM-LEFT origin; imgui
        wants TOP-LEFT.

        UNVERIFIED: the SDK doc says touch coords are "relative lower-left corner
        of screen or bezel". We assume SCREEN. If they are bezel-relative, every
        position is off by (screenOffsetX, screenOffsetY) -- which shows up as
        hit-testing that misses widgets while io.mouse_pos still tracks. lastRaw
        keeps the unconverted value so a probe can display both.
        """
        self.lastRaw = (x, y)
        return float(x), float(self.screenHeight - y)

    def imguiScreenTouch(self: Self, x: int, y: int, mouseStatus: int, _refCon: Any) -> int:
        """Feed the touch to imgui.

        XPD-18371 (filed 2026-08-12, open): on avionics devices xplm_MouseUp
        always reports (0, 0), and xplm_MouseDrag is never delivered at all. So on
        release we must IGNORE the coordinates and change only the button state --
        moving the cursor to the corner at the moment of release makes imgui think
        the release landed away from the widget, and nothing ever activates
        (widgets still highlight on press, which is what makes it look like a
        hit-testing problem rather than a release problem).
        """
        if mouseStatus == xp.MouseUp:
            self.setMouseButton(False)
            return 1
        imguiX, imguiY = self.toImguiSpace(x, y)
        self.setMouse(imguiX, imguiY, down=True)
        return 1

    def imguiScreenRightTouch(self: Self, _x: int, _y: int, _mouseStatus: int, _refCon: Any) -> int:
        return 1

    def imguiScreenScroll(self: Self, x: int, y: int, wheel: int, clicks: int, _refCon: Any) -> int:
        """Feed the wheel to imgui so scrollable regions work.
        wheel is the axis: 0 = vertical, 1 = horizontal."""
        imguiX, imguiY = self.toImguiSpace(x, y)
        self.setMouse(imguiX, imguiY)
        self.addWheel(wheel, clicks)
        return 1

    def imguiScreenCursor(self: Self, x: int, y: int, _refCon: Any) -> XPLMCursorStatus:
        """Hover tracking: fires while the cursor is over the screen with no button
        down, which is what imgui needs for hover states."""
        imguiX, imguiY = self.toImguiSpace(x, y)
        self.setMouse(imguiX, imguiY)
        return xp.CursorDefault

    def imguiKeyboard(self: Self, key: int, flags: int, vKey: int, _refCon: Any, losingFocus: int) -> int:
        """Feed a keystroke to imgui and consume it.

        NOTE the asymmetry with windows: XPLMAvionicsKeyboard_f returns int
        ("return true to consume the event, or false to let X-Plane process it"),
        while the window key callback XPLMHandleKey_f returns void. Returning None
        here raises a wrong-type error from the wrapper.
        """
        self.handleKey(key, flags, vKey, losingFocus)
        return 1
