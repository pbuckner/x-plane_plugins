from typing import Tuple, Callable, Union, Any
from xp_hinting import XPWindowID
Phase_Modern3D = 31
Phase_FirstCockpit = 35
Phase_Panel = 40
Phase_Gauges = 45
Phase_Window = 50
Phase_LastCockpit = 55
MouseDown = 1
MouseDrag = 2
MouseUp = 3
CursorDefault = 0
CursorHidden = 1
CursorArrow = 2
CursorCustom = 3
WindowLayerFlightOverlay = 0
WindowLayerFloatingWindows = 1
WindowLayerModal = 2
WindowLayerGrowlNotifications = 3
WindowDecorationNone = 0
WindowDecorationRoundRectangle = 1
WindowDecorationSelfDecorated = 2
WindowDecorationSelfDecoratedResizable = 3
WindowPositionFree = 0
WindowCenterOnMonitor = 1
WindowFullScreenOnMonitor = 2
WindowFullScreenOnAllMonitors = 3
WindowPopOut = 4
WindowVR = 5
Device_GNS430_1 = 0
Device_GNS430_2 = 1
Device_GNS530_1 = 2
Device_GNS530_2 = 3
Device_CDU739_1 = 4
Device_CDU739_2 = 5
Device_G1000_PFD_1 = 6
Device_G1000_PFD_2 = 7
Device_G1000_MFD = 8
Device_CDU815_1 = 9
Device_CDU815_2 = 10
Device_Primus_PFD_1 = 11
Device_Primus_PFD_2 = 12
Device_Primus_MFD_1 = 13
Device_Primus_MFD_2 = 14
Device_Primus_MFD_3 = 15
Device_Primus_RMU_1 = 16
Device_Primus_RMU_2 = 17


def registerAvionicsCallbacksEx(deviceId: int,
                                before: Union[None, Callable[[int, int, Any], int]] = None,
                                after: Union[None, Callable[[int, int, Any], int]] = None,
                                refcon: Any = None) -> int:
    return int()  # avionicsID


def unregisterAvionicsCallbacks(avionicsID: int) -> None:
    return


def registerDrawCallback(draw: Callable[[int, int, Any], int],
                         phase: int = Phase_Window,
                         after: int = 1,
                         refCon: Any = None) -> int:
    return int()  # 0=phase does not exist; 1=registration successful


def unregisterDrawCallback(draw: Callable[[int, int, Any], int],
                           phase: int = Phase_Window,
                           after: int = 1,
                           refCon: Any = None) -> int:
    return int()  # 1=callback found and unregistered, 0=otherwise


def createWindowEx(left: int = 100,
                   top: int = 200,
                   right: int = 200,
                   bottom: int = 100,
                   visible: int = 0,
                   draw: Union[None, Callable[[XPWindowID, Any], None]] = None,
                   click: Union[None, Callable[[XPWindowID, int, int, int, Any], int]] = None,
                   key: Union[None, Callable[[XPWindowID, int, int, int, Any, int], None]] = None,
                   cursor: Union[None, Callable[[XPWindowID, int, int, Any], int]] = None,
                   wheel: Union[None, Callable[[XPWindowID, int, int, int, int, Any], int]] = None,
                   refCon: Any = None,
                   decoration: int = WindowDecorationRoundRectangle,
                   layer: int = WindowLayerFloatingWindows,
                   rightClick: Union[None, Callable[[XPWindowID, int, int, int, Any], int]] = None) -> XPWindowID:
    return XPWindowID(0)


def destroyWindow(windowID: XPWindowID) -> None:
    return


def getScreenSize() -> Tuple[int, int]:
    return (int(), int())  # width, height


def getScreenBoundsGlobal() -> Tuple[int, int, int, int]:
    return int(), int(), int(), int()  # left, top, right, bottom


def getAllMonitorBoundsGlobal(bounds: Callable[[int, int, int, int, int, Any], None], refCon: Any) -> None:
    return


def getAllMonitorBoundsOS(bounds: Callable[[int, int, int, int, int, Any], None], refCon: Any) -> None:
    return


def getMouseLocationGlobal() -> Tuple[int, int]:
    return int(), int()  # x, y


def getWindowGeometry(windowID: XPWindowID) -> Tuple[int, int, int, int]:
    return int(), int(), int(), int()  # left, top, right, bottom


def setWindowGeometry(windowID: XPWindowID, left: int, top: int, right: int, bottom: int) -> None:
    return


def getWindowGeometryOS(windowID: XPWindowID) -> Tuple[int, int, int, int]:
    return int(), int(), int(), int()  # left, top, right, bottm


def setWindowGeometryOS(windowID: XPWindowID, left: int, top: int, right: int, bottom: int) -> None:
    return


def getWindowGeometryVR(windowID: XPWindowID) -> Tuple[int, int]:
    return int(), int()  # widthBoxels, heightBoxels


def setWindowGeometryVR(windowID: XPWindowID, width: int, height: int) -> None:
    return


def getWindowIsVisible(windowID: XPWindowID) -> int:
    return int()  # 1=visible


def setWindowIsVisible(windowID: XPWindowID, visible: int = 1) -> None:
    return


def windowIsPoppedOut(windowID: XPWindowID) -> int:
    return int()  # 1=True


def windowIsInVR(windowID: XPWindowID) -> int:
    return int()  # 1=True


def setWindowGravity(windowID: XPWindowID, left: float, top: float, right: float, bottom: float) -> None:
    return


def setWindowResizingLimits(windowID: XPWindowID, minWidth: int = 0, minHeight: int = 0,
                            maxWidth: int = 10000, maxHeight: int = 10000) -> None:
    return


def setWindowPositioningMode(windowID: XPWindowID, mode: int, index: int = -1) -> None:
    return


def setWindowTitle(windowID: XPWindowID, title: str) -> None:
    return


def getWindowRefCon(windowID: XPWindowID) -> Any:
    return Any  # refcon


def setWindowRefCon(windowID: XPWindowID, refCon: Any) -> None:
    return


def takeKeyboardFocus(windowID: XPWindowID) -> None:
    return


def hasKeyboardFocus(windowID: XPWindowID) -> int:
    return int()  # 1=window has focus;


def bringWindowToFront(windowID: XPWindowID) -> None:
    return


def isWindowInFront(windowID: XPWindowID) -> int:
    return int()  # 1=True


def registerKeySniffer(sniffer: Callable[[int, int, int, Any], int], before: int = 0, refCon: Any = None) -> int:
    return int()  # 1=success


def unregisterKeySniffer(sniffer: Callable[[int, int, int, Any], int], before: int = 0, refCon: Any = None) -> int:
    return int()  # 1=success


def registerHotKey(vKey: int, flags: int = 0, description:str = '',
                   hotKey: Union[None, Callable[[Any], None]] = None, refCon: Any = None) -> int:
    return int()  # XMPLHotKeyID


def unregisterHotKey(hotKeyID: int) -> None:
    return


def countHotKeys() -> int:
    return int()


def getNthHotKey(index: int) -> int:
    return int()  # XPLMHotKeyID


class HotKeyInfo(object):
    virtualKey = None   # integer (XPLM_VK_*)
    flags = None        # integer (xplm_*Flag)
    description = None  # string
    plugin = None       # integer


def getHotKeyInfo(hotKeyId: int) -> HotKeyInfo:
    return HotKeyInfo()  # HotKeyInfo


def setHotKeyCombination(hotKeyID: int, vKey: int, flags: int = 0):
    return
