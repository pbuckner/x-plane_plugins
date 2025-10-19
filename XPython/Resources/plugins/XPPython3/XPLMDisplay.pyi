from dataclasses import dataclass
from typing import Any, Callable, Generic, Optional, Type, TypeVar, NewType, Literal, Sequence
from XPPython3.xp_typing import *
NoFlag: XPLMKeyFlags
CursorDefault: XPLMCursorStatus
CursorHidden: XPLMCursorStatus
CursorArrow: XPLMCursorStatus
CursorCustom: XPLMCursorStatus
CursorRotateSmall: XPLMCursorStatus
CursorRotateSmallLeft: XPLMCursorStatus
CursorRotateSmallRight: XPLMCursorStatus
CursorRotateMedium: XPLMCursorStatus
CursorRotateMediumLeft: XPLMCursorStatus
CursorRotateMediumRight: XPLMCursorStatus
CursorRotateLarge: XPLMCursorStatus
CursorRotateLargeLeft: XPLMCursorStatus
CursorRotateLargeRight: XPLMCursorStatus
CursorUpDown: XPLMCursorStatus
CursorUp: XPLMCursorStatus
CursorDown: XPLMCursorStatus
CursorLeftRight: XPLMCursorStatus
CursorLeft: XPLMCursorStatus
CursorRight: XPLMCursorStatus
CursorButton: XPLMCursorStatus
CursorHandle: XPLMCursorStatus
CursorFourArrows: XPLMCursorStatus
CursorSplitterH: XPLMCursorStatus
CursorSplitterV: XPLMCursorStatus
CursorText: XPLMCursorStatus
MouseDown: XPLMMouseStatus
MouseDrag: XPLMMouseStatus
MouseUp: XPLMMouseStatus
Phase_Modern3D: XPLMDrawingPhase
Phase_FirstCockpit: XPLMDrawingPhase
Phase_Panel: XPLMDrawingPhase
Phase_Gauges: XPLMDrawingPhase
Phase_Window: XPLMDrawingPhase
Phase_LastCockpit: XPLMDrawingPhase
Phase_LocalMap3D: XPLMDrawingPhase
Phase_LocalMap2D: XPLMDrawingPhase
Phase_LocalMapProfile: XPLMDrawingPhase
WindowLayerFlightOverlay: XPLMWindowLayer
WindowLayerFloatingWindows: XPLMWindowLayer
WindowLayerModal: XPLMWindowLayer
WindowLayerGrowlNotifications: XPLMWindowLayer
WindowPositionFree: XPLMWindowPositioningMode
WindowCenterOnMonitor: XPLMWindowPositioningMode
WindowFullScreenOnMonitor: XPLMWindowPositioningMode
WindowFullScreenOnAllMonitors: XPLMWindowPositioningMode
WindowPopOut: XPLMWindowPositioningMode
WindowVR: XPLMWindowPositioningMode
WindowDecorationNone: XPLMWindowDecoration
WindowDecorationRoundRectangle: XPLMWindowDecoration
WindowDecorationSelfDecorated: XPLMWindowDecoration
WindowDecorationSelfDecoratedResizable: XPLMWindowDecoration
Device_GNS430_1: XPLMDeviceID
Device_GNS430_2: XPLMDeviceID
Device_GNS530_1: XPLMDeviceID
Device_GNS530_2: XPLMDeviceID
Device_CDU739_1: XPLMDeviceID
Device_CDU739_2: XPLMDeviceID
Device_G1000_PFD_1: XPLMDeviceID
Device_G1000_PFD_2: XPLMDeviceID
Device_G1000_MFD: XPLMDeviceID
Device_CDU815_1: XPLMDeviceID
Device_CDU815_2: XPLMDeviceID
Device_Primus_PFD_1: XPLMDeviceID
Device_Primus_PFD_2: XPLMDeviceID
Device_Primus_MFD_1: XPLMDeviceID
Device_Primus_MFD_2: XPLMDeviceID
Device_Primus_MFD_3: XPLMDeviceID
Device_Primus_RMU_1: XPLMDeviceID
Device_Primus_RMU_2: XPLMDeviceID
Device_MCDU_1: XPLMDeviceID
Device_MCDU_2: XPLMDeviceID
_Airport: int
def registerDrawCallback(draw:Callable[[XPLMDrawingPhase, int, Any], None | int], phase:XPLMDrawingPhase=Phase_Window, after:int=0, refCon:Any=None) -> int:
    """
    Registers  low-level drawing callback.
    
    The after parameter indicates you want to be called before (0) or after (1) phase.
    draw() callback function takes three parameters (phase, after, refCon), returning 
    0 to suppress further X-Plane drawing in the phase, or 1 to allow X-Plane to finish.
    (Callback's value is ignored if after=1).
    
    Registration returns 1 on success, 0 otherwise.
    """
    ...

def registerAvionicsCallbacksEx(deviceId:XPLMDeviceID, 
          before:Optional[Callable[[XPLMDeviceID, int, Any], int]]=None, 
          after:Optional[Callable[[XPLMDeviceID, int, Any], int]]=None, 
          refCon:Any=None,
          bezelClick:Optional[Callable[[int, int, XPLMMouseStatus, Any], int]]=None, 
          bezelRightClick:Optional[Callable[[int, int, XPLMMouseStatus, Any], int]]=None, 
          bezelScroll:Optional[Callable[[int, int, int, int, Any], int]]=None, 
          bezelCursor:Optional[Callable[[int, int, Any], XPLMCursorStatus]]=None, 
          screenTouch:Optional[Callable[[int, int, XPLMMouseStatus, Any], int]]=None, 
          screenRightTouch:Optional[Callable[[int, int, XPLMMouseStatus, Any], int]]=None, 
          screenScroll:Optional[Callable[[int, int, int, int, Any], int]]=None, 
          screenCursor:Optional[Callable[[int, int, Any], XPLMCursorStatus]]=None, 
          keyboard:Optional[Callable[[int, XPLMKeyFlags, int, Any, int], int]]=None) -> XPLMAvionicsID:
    """
    Registers draw callback for particular device.
    
    Registers drawing callback(s) to enhance or replace X-Plane drawing. For
    'before' callback, return 1 to let X-Plane draw or 0 to suppress X-Plane
    drawing. Return value for 'after' callback is ignored.
    
    Upon entry, OpenGL context will be correctly set in panel coordinates for 2d drawing.
    OpenGL state (texturing, etc.) will be unknown.
    
    Successful registration returns an AvionicsID.
    
    """
    ...

def unregisterAvionicsCallbacks(avionicsId:XPLMAvionicsID) -> None:
    """
    Unregisters avionics draw callback(s) associated with given avionicsId.
    
    Does not return a value.
    
    """
    ...

def getAvionicsHandle(deviceID:int) -> XPLMAvionicsID:
    """
    Registers no callbacks for a cockpit device, but returns a
    handle which allows you to interact using other Avionics Device
    API. Use this if you do not wish to intercept drawing, clicks, or
    touches but want to interact with its popup programmatically.
    
    Returns XPLMAvionicsID.
    
    """
    ...

def isAvionicsBound(avionicsID:XPLMAvionicsID) -> int:
    """
    Return 1 if cockpit device with given ID is used by the current aircraft.
    """
    ...

def isCursorOverAvionics(avionicsID:XPLMAvionicsID) -> tuple[int, int] | None:
    """
    Is the cursor over the device with given avionicsID
    
    Returns tuple (x, y) with position or None.
    """
    ...

def isAvionicsPopupVisible(avionicsID:XPLMAvionicsID) -> int:
    """
    Is the popup window for the device with given avionicsID visible?
    (It may or may not be popped out into an OS window.)
    
    Returns 1 if true.
    """
    ...

def isAvionicsPoppedOut(avionicsID:XPLMAvionicsID) -> int:
    """
    Returns 1 (true) if the popup window for the cockpit device is popped out
    into an OS window.
    """
    ...

def hasAvionicsKeyboardFocus(avionicsID:XPLMAvionicsID) -> int:
    """
    Returns 1 (true) if cockpit device has keyboard focus.
    """
    ...

def avionicsNeedsDrawing(avionicsID:XPLMAvionicsID) -> None:
    """
    Tells X-Plane that your device's screens needs to be re-drawn.
    If your device is marked for on-demand drawing, XP will call your screen
    drawing callback before drawing the next simulator frame. If your device
    is already drawn every frame, this has no effect.
    """
    ...

def popOutAvionics(avionicsID:XPLMAvionicsID) -> None:
    """
    Pops out OS window for cockpit device.
    """
    ...

def takeAvionicsKeyboardFocus(avionicsID:XPLMAvionicsID) -> None:
    """
    Sets keyboard focus to the (already) visible popup window of cockpit device.
    Does nothing if device is not visible.
    """
    ...

def destroyAvionics(avionicsID:XPLMAvionicsID) -> None:
    """
    Destroys the cockpit device and deallocates its framebuffer. You should
    only ever call this for devices that you created, not stock X-Plane devices
    you have customized.
    """
    ...

def getAvionicsBusVoltsRatio(avionicsID:XPLMAvionicsID) -> float:
    """
    Return ratio [0.0:1.0] of nominal voltage of electrical bus,
    for given avionics device. Returns -1 if device is not bound
    to the current aircraft.
    """
    ...

def getAvionicsBrightnessRheo(avionicsID:XPLMAvionicsID) -> float:
    """
    Returns brightness setting between 0 and 1 for the screen of
    this cockpit device.
    If the device is bound to current aircraft, this is equivalent
    to 'sim/cockpit2/switches/instrument_brightness_ratio[]' dataref
    with the correct array slot for the bound device.
    If the device is not bound, it returns brightness ratio for the
    device alone.
    """
    ...

def setAvionicsBrightnessRheo(avionicsID:XPLMAvionicsID, brightness:float=1.0) -> None:
    """
    Sets brightness setting between 0 and 1 for the screen of
    this cockpit device.
    
    If the device is bound to current aircraft, this is equivalent
    to 'sim/cockpit2/switches/instrument_brightness_ratio[]' dataref
    with the correct array slot for the bound device.
    
    If the device is not bound, it sets brightness rheostat for the
    device alone, even though not connected to the dataref.
    """
    ...

def setAvionicsPopupVisible(avionicsID:XPLMAvionicsID, visible:int=1) -> None:
    """
    Shows (visible=1) or Hides popup window for cockpit device.
    """
    ...

def registerKeySniffer(sniffer:Callable[[int, XPLMKeyFlags, int, Any], int], before:int=0, refCon:Any=None) -> int:
    """
    Registers a key sniffer callback function.
    
    sniffer() callback takes four parameters (key, flags, vKey, refCon) and
    should return 0 to consume the key, 1 to pass it to next sniffer or X-Plane.
    
    before=1 will intercept keys before windows (i.e., the user may be typing in
    input field), so generally, use before=0 to sniff keys not already consumed.
    
    refCon will be passed to your sniffer callback.
    """
    ...

def unregisterDrawCallback(draw:Callable[[XPLMDrawingPhase, int, Any], None], phase:XPLMDrawingPhase=Phase_Window, after:int=0, refCon:Any=None) -> int:
    """
    Unregisters low-level drawing callback.
    
    Parameters must match those provided  with registerDrawCallback().
    Returns 1 on success, 0 otherwise.
    """
    ...

def unregisterKeySniffer(sniffer:Callable[[int, XPLMKeyFlags, int, Any], int], before:int=0, refCon:Any=None) -> int:
    """
    Unregisters key sniffer.
    
    Parameters must match those provided with registerKeySniffer().
    Returns 1 on success, 0 otherwise.
    """
    ...

def createAvionicsEx(screenWidth:int=100, screenHeight:int=200, bezelWidth:int=140, bezelHeight:int=250, 
          screenOffsetX:int=20, screenOffsetY:int=25, drawOnDemand=0, 
          bezelDraw:Optional[Callable[[float, float, float, Any], None]]=None, 
          screenDraw:Optional[Callable[[Any], None]]=None, 
          bezelClick:Optional[Callable[[int, int, XPLMMouseStatus, Any], int]]=None, 
          bezelRightClick:Optional[Callable[[int, int, XPLMMouseStatus, Any], int]]=None, 
          bezelScroll:Optional[Callable[[int, int, int, int, Any], int]]=None, 
          bezelCursor:Optional[Callable[[int, int, Any], XPLMCursorStatus]]=None, 
          screenTouch:Optional[Callable[[int, int, XPLMMouseStatus, Any], int]]=None, 
          screenRightTouch:Optional[Callable[[int, int, XPLMMouseStatus, Any], int]]=None, 
          screenScroll:Optional[Callable[[int, int, int, int, Any], int]]=None, 
          screenCursor:Optional[Callable[[int, int, Any], XPLMCursorStatus]]=None, 
          keyboard:Optional[Callable[[int, XPLMKeyFlags, int, Any, int], int]]=None, 
          brightness:Optional[Callable[[float, float, float, float], float]]=None, 
          deviceID:str = "deviceID", deviceName:str = "deviceName",
          refCon:Any = None,) -> XPLMAvionicsID:
    """
    Creates glass cockpit device for 3D cockpit.
    With 12.0 you needed to call this within your XPluginStart callback
    to ensure your texture would be ready. Since 12.1, you may call this
    at anytime and X-Plane will retroactively map your display to it.
    
    Returns new avionicsID.
    """
    ...

def createWindowEx(left:int=100, top:int=200, right:int=200, bottom:int=100,
          visible:int=0, 
          draw:Optional[Callable[[XPLMWindowID, Any], None]]=None,
          click:Optional[Callable[[XPLMWindowID, int, int, XPLMMouseStatus, Any], int]]=None, 
          key:Optional[Callable[[XPLMWindowID, int, int, int, Any, int], None]]=None, 
          cursor:Optional[Callable[[XPLMWindowID, int, int, Any], XPLMCursorStatus]]=None, 
          wheel:Optional[Callable[[XPLMWindowID, int, int, int, int, Any], int]]=None, 
          refCon:Any=None, decoration:XPLMWindowDecoration=WindowDecorationRoundRectangle, 
          layer:XPLMWindowLayer=WindowLayerFloatingWindows, 
          rightClick:Optional[Callable[[XPLMWindowID, int, int, XPLMMouseStatus, Any], int]]=None, ) -> XPLMWindowID:
    """
    Creates modern window
    
    Callback functions are optional:
      draw(windowID, refCon) with no return value.
      key(windowID, key, flags, vKey, refCon, losingFocus) with no return value.
      cursor(windowID, x, y, refCon) returns updated CursorStatus
      click(windowID, x, y, mouseStatus, refCon) with 1= consume click, else 0.
      rightClick(windowID, x, y, mouseStatus, refCon) with 1= consume click, else 0.
      wheel(windowID, x, y, wheel, clicks, refCon) with 1= consume click, else 0.
    
    Returns new windowID.
    """
    ...

def createWindow(left:int=100, top:int=200, right:int=200, bottom:int=100, visible:int=0,
          draw:Optional[Callable[[XPLMWindowID, Any], None]]=None,
          key:Optional[Callable[[XPLMWindowID, int, int, int, Any, int], None]]=None, 
          mouse:Optional[Callable[[XPLMWindowID, int, int, XPLMMouseStatus, Any], int]]=None, 
          refCon:Any=None) -> XPLMWindowID:
    """
    (Deprecated do not use)
    """
    ...

def destroyWindow(windowID:XPLMWindowID) -> None:
    """
    Destroys window. Returns None.
    """
    ...

def getScreenSize() -> tuple[int, int]:
    """
    Returns (width, height) of screen.
    """
    ...

def getScreenBoundsGlobal() -> tuple[int, int, int, int]:
    """
    Returns (left, top, right, bottom) of screen bounds.
    """
    ...

def getAllMonitorBoundsGlobal(bounds:Callable[[int, int, int, int, int, Any], None], refCon:Any) -> None:
    """
    Immediately calls bounds() callback once for each monitor to retrieve
    bounds for each monitor running full-screen simulator.
    
    Callback function is
      bounds(index, left, top, right, bottom, refCon)
    """
    ...

def getAllMonitorBoundsOS(bounds:Callable[[int, int, int, int, int, Any], None], refCon:Any) -> None:
    """
    Immediately calls bounds() once for each monitor known to OS.
    
    Callback function is
      bounds(index, refCon, left, top, right, bottom, refCon)
    """
    ...

def getMouseLocation() -> None | tuple[int, int]:
    """
    Deprecated, use getMouseLocationGlobal().
    """
    ...

def getMouseLocationGlobal() -> None | tuple[int, int]:
    """
    Returns current mouse location (x, y).
    """
    ...

def getWindowGeometry(windowID:XPLMWindowID) -> None | tuple[int, int, int, int]:
    """
    Returns window geometry (left, top, right, bottom).
    """
    ...

def getAvionicsGeometry(avionicsID:XPLMAvionicsID) -> tuple[int, int, int, int]:
    """
    Returns geometry (left, top, right, bottom) of popup window
    in X-Plane coordinate system.
    """
    ...

def setWindowGeometry(windowID:XPLMWindowID, left:int, top:int, right:int, bottom:int) -> None:
    """
    Sets window geometry.
    """
    ...

def setAvionicsGeometry(avionicsID:XPLMAvionicsID, left:int, top:int, right:int, bottom:int) -> None:
    """
    Sets window geometry for cockpit device's popup window in X-Plane coordinates.
    """
    ...

def getWindowGeometryOS(windowID:XPLMWindowID) -> None | tuple[int, int, int, int]:
    """
    Returns window geometry for popped-out window (left, top, right, bottom).
    """
    ...

def getAvionicsGeometryOS(avionicsID:XPLMAvionicsID) -> tuple[int, int, int, int]:
    """
    Returns window geometry for popped-out avionics device (left, top, right, bottom).
    """
    ...

def setAvionicsGeometryOS(avionicsID:XPLMAvionicsID, left:int, top:int, right:int, bottom:int) -> None:
    """
    Sets window geometry for popped-out avionics.
    """
    ...

def setWindowGeometryOS(windowID:XPLMWindowID, left:int, top:int, right:int, bottom:int) -> None:
    """
    Sets window geometry for popped-out window.
    """
    ...

def getWindowGeometryVR(windowID:XPLMWindowID) -> None | tuple[int, int]:
    """
    Gets window geometry for VR window (width, height)
    """
    ...

def setWindowGeometryVR(windowID:XPLMWindowID, width:int, height:int) -> None:
    """
    Sets window geometry for VR window.
    """
    ...

def getWindowIsVisible(windowID:XPLMWindowID) -> int:
    """
    Returns 1 if window is visible, 0 otherwise.
    """
    ...

def setWindowIsVisible(windowID:XPLMWindowID, visible:int=1) -> None:
    """
    Sets window visibility. 1 indicates visible, 0 is not-visible.
    """
    ...

def windowIsPoppedOut(windowID:XPLMWindowID) -> int:
    """
    Returns 1 if window is popped-out, 0 otherwise.
    """
    ...

def windowIsInVR(windowID:XPLMWindowID) -> int:
    """
    Returns 1 if window is in VR, 0 otherwise.
    """
    ...

def setWindowGravity(windowID:XPLMWindowID, left:int, top:int, right:int, bottom:int) -> None:
    """
    Set window's gravity (resize window, relative screen size changes.)
    
    values typically range 0.0 - 1.0, and reflect how much the window's
    value will change relative to change in screen size.
    0.0 means zero change relative to the right side (for left/right edges)
        and zero change relative to the bottom (for (for top/bottom edges)
    1.0 means 100% change relative to the right side / bottom
    """
    ...

def setWindowResizingLimits(windowID:XPLMWindowID, minWidth:int=0, minHeight:int=0, maxWidth:int=10000, maxHeight:int=10000) -> None:
    """
    Set maximum and minimum window size.
    """
    ...

def setWindowPositioningMode(windowID:XPLMWindowID, mode:XPLMWindowPositioningMode, index:int=-1) -> None:
    """
    Set window positioning mode:
     * WindowPositionFree
     * WindowCenterOnMonitor
     * WindowFullScreenOnMonitor
     * WindowFullScreenOnAllMonitors
     * WindowPopOut
     * WindowVR
    """
    ...

def setWindowTitle(windowID:XPLMWindowID, title:str) -> None:
    """
    Set window title.
    """
    ...

def getWindowRefCon(windowID:XPLMWindowID) -> Any:
    """
    Return window's reference constant refCon.
    """
    ...

def setWindowRefCon(windowID:XPLMWindowID, refCon:Any) -> None:
    """
    Set window's reference constant refCon.
    """
    ...

def takeKeyboardFocus(windowID:XPLMWindowID) -> None:
    """
    Take keyboard focus. 0 to send focus to X-Plane.
    """
    ...

def hasKeyboardFocus(windowID: XPLMWindowID) -> int:
    """
    Returns 1 if window has current keyboard focus.
    Pass 0 to query if X-Plane has current focus.
    """
    ...

def bringWindowToFront(windowID:XPLMWindowID) -> None:
    """
    Bring window to front (of its window layer).
    """
    ...

def isWindowInFront(windowID:XPLMWindowID) -> int:
    """
    Returns 1 if window is currently in the front of its window layer).
    """
    ...

def registerHotKey(vkey:int, flags:XPLMKeyFlags=NoFlag, description:str='', 
          hotKey:Optional[Callable[[Any], None]]=None, 
          refCon:Any=None) -> XPLMHotKeyID:
    """
    Registers hot key.
    
    Callback is hotKey(refCon), it does not need to return anything.
    Registration returns a hotKeyID, which can be used with unregisterHotKey()
    """
    ...

def unregisterHotKey(hotKeyID:XPLMHotKeyID) -> None:
    """
    Unregisters hot key associated with hotKeyID.
    
    hotKeyID must be registered to this plugin using registerHotKey()
    otherwise unregistration will fail.
    """
    ...

def countHotKeys() -> int:
    """
    Return number of hot keys currently defined in the simulator.
    """
    ...

def getNthHotKey(index:int) -> XPLMHotKeyID:
    """
    Return hotKeyID for (zero-based) Nth hot key defined in sim.
    """
    ...

def getHotKeyInfo(hotKeyID:XPLMHotKeyID) -> None | HotKeyInfo:
    """
    Return object with hot key information.
    
      .description
      .virtualKey
      .flags
      .plugin
    """
    ...

def setHotKeyCombination(hotKeyID:XPLMHotKeyID, vKey:int, flags:XPLMKeyFlags=NoFlag) -> None:
    """
    Update key combination for given hotKeyID to use vKey and flags
    """
    ...

def getDrawCallbackDict() -> dict[int, tuple[str, Callable, int, int, Any]]:
    """
    Returns copy of internal DrawCallbackInfo dictionary for debugging.

    Dictionary maps callback counter (int) to tuple of:
      (module_name, callback, phase, before, refCon)
    """
    ...

def getWindowCallbackDict() -> dict[XPLMWindowID, tuple[Callable, Callable, Callable, Callable, Callable, Callable, Any, str]]:
    """
    Returns copy of internal WindowCallbackInfo dictionary for debugging.

    Dictionary maps windowID to tuple of:
      (draw, click, key, cursor, wheel, rightClick, refCon, module_name)
    """
    ...

def getAvionicsCallbackDict() -> dict[XPLMAvionicsID, tuple]:
    """
    Returns copy of internal AvionicsCallbackInfo dictionary for debugging.
    """
    ...

def getKeySnifferCallbackDict() -> dict[int, tuple]:
    """
    Returns copy of internal KeySnifferCallbackInfo dictionary for debugging.
    """
    ...

def getHotKeyCallbackDict() -> dict[XPLMHotKeyID, tuple]:
    """
    Returns copy of internal HotKeyCallbackInfo dictionary for debugging.
    """
    ...

