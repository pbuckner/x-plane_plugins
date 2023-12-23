from dataclasses import dataclass
from typing import Any, Callable, Generic, Optional, Type, TypeVar, NewType, Literal, Sequence
from XPPython3.xp_typing import *    
ControlCameraUntilViewChanges: XPLMCameraControlDuration
ControlCameraForever: XPLMCameraControlDuration
Type_Unknown: XPLMDataTypeID
Type_Int: XPLMDataTypeID
Type_Float: XPLMDataTypeID
Type_Double: XPLMDataTypeID
Type_FloatArray: XPLMDataTypeID
Type_IntArray: XPLMDataTypeID
Type_Data: XPLMDataTypeID
kXPLM_Version: int
NO_PLUGIN_ID: int
PLUGIN_XPLANE: int
kVersion: int
NoFlag: XPLMKeyFlags
ShiftFlag: XPLMKeyFlags
OptionAltFlag: XPLMKeyFlags
ControlFlag: XPLMKeyFlags
DownFlag: XPLMKeyFlags
UpFlag: XPLMKeyFlags
KEY_RETURN: int
KEY_ESCAPE: int
KEY_TAB: int
KEY_DELETE: int
KEY_LEFT: int
KEY_RIGHT: int
KEY_UP: int
KEY_DOWN: int
KEY_0: int
KEY_1: int
KEY_2: int
KEY_3: int
KEY_4: int
KEY_5: int
KEY_6: int
KEY_7: int
KEY_8: int
KEY_9: int
KEY_DECIMAL: int
VK_BACK: int
VK_TAB: int
VK_CLEAR: int
VK_RETURN: int
VK_ESCAPE: int
VK_SPACE: int
VK_PRIOR: int
VK_NEXT: int
VK_END: int
VK_HOME: int
VK_LEFT: int
VK_UP: int
VK_RIGHT: int
VK_DOWN: int
VK_SELECT: int
VK_PRINT: int
VK_EXECUTE: int
VK_SNAPSHOT: int
VK_INSERT: int
VK_DELETE: int
VK_HELP: int
VK_0: int
VK_1: int
VK_2: int
VK_3: int
VK_4: int
VK_5: int
VK_6: int
VK_7: int
VK_8: int
VK_9: int
VK_A: int
VK_B: int
VK_C: int
VK_D: int
VK_E: int
VK_F: int
VK_G: int
VK_H: int
VK_I: int
VK_J: int
VK_K: int
VK_L: int
VK_M: int
VK_N: int
VK_O: int
VK_P: int
VK_Q: int
VK_R: int
VK_S: int
VK_T: int
VK_U: int
VK_V: int
VK_W: int
VK_X: int
VK_Y: int
VK_Z: int
VK_NUMPAD0: int
VK_NUMPAD1: int
VK_NUMPAD2: int
VK_NUMPAD3: int
VK_NUMPAD4: int
VK_NUMPAD5: int
VK_NUMPAD6: int
VK_NUMPAD7: int
VK_NUMPAD8: int
VK_NUMPAD9: int
VK_MULTIPLY: int
VK_ADD: int
VK_SEPARATOR: int
VK_SUBTRACT: int
VK_DECIMAL: int
VK_DIVIDE: int
VK_F1: int
VK_F2: int
VK_F3: int
VK_F4: int
VK_F5: int
VK_F6: int
VK_F7: int
VK_F8: int
VK_F9: int
VK_F10: int
VK_F11: int
VK_F12: int
VK_F13: int
VK_F14: int
VK_F15: int
VK_F16: int
VK_F17: int
VK_F18: int
VK_F19: int
VK_F20: int
VK_F21: int
VK_F22: int
VK_F23: int
VK_F24: int
VK_EQUAL: int
VK_MINUS: int
VK_RBRACE: int
VK_LBRACE: int
VK_QUOTE: int
VK_SEMICOLON: int
VK_BACKSLASH: int
VK_COMMA: int
VK_SLASH: int
VK_PERIOD: int
VK_BACKQUOTE: int
VK_ENTER: int
VK_NUMPAD_ENT: int
VK_NUMPAD_EQ: int
Phase_Modern3D: XPLMDrawingPhase
Phase_FirstCockpit: XPLMDrawingPhase
Phase_Panel: XPLMDrawingPhase
Phase_Gauges: XPLMDrawingPhase
Phase_Window: XPLMDrawingPhase
Phase_LastCockpit: XPLMDrawingPhase
Phase_LocalMap3D: XPLMDrawingPhase
Phase_LocalMap2D: XPLMDrawingPhase
Phase_LocalMapProfile: XPLMDrawingPhase
MouseDown: XPLMMouseStatus
MouseDrag: XPLMMouseStatus
MouseUp: XPLMMouseStatus
CursorDefault: XPLMCursorStatus
CursorHidden: XPLMCursorStatus
CursorArrow: XPLMCursorStatus
CursorCustom: XPLMCursorStatus
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
Tex_GeneralInterface: XPLMTextureID
Font_Basic: XPLMFontID
Font_Proportional: XPLMFontID
MapStyle_VFR_Sectional: XPLMMapStyle
MapStyle_IFR_LowEnroute: XPLMMapStyle
MapStyle_IFR_HighEnroute: XPLMMapStyle
MapLayer_Fill: XPLMMapLayerType
MapLayer_Markings: XPLMMapLayerType
MAP_USER_INTERFACE: str
MAP_IOS: str
MapOrientation_Map: XPLMMapOrientation
MapOrientation_UI: XPLMMapOrientation
Menu_NoCheck: XPLMMenuCheck
Menu_Unchecked: XPLMMenuCheck
Menu_Checked: XPLMMenuCheck
Nav_Any: XPLMNavType
Nav_Unknown: XPLMNavType
Nav_Airport: XPLMNavType
Nav_NDB: XPLMNavType
Nav_VOR: XPLMNavType
Nav_ILS: XPLMNavType
Nav_Localizer: XPLMNavType
Nav_GlideSlope: XPLMNavType
Nav_OuterMarker: XPLMNavType
Nav_MiddleMarker: XPLMNavType
Nav_InnerMarker: XPLMNavType
Nav_Fix: XPLMNavType
Nav_DME: XPLMNavType
Nav_LatLon: XPLMNavType
NAV_NOT_FOUND: int
USER_AIRCRAFT: int
MSG_PLANE_CRASHED: int
MSG_PLANE_LOADED: int
MSG_AIRPORT_LOADED: int
MSG_SCENERY_LOADED: int
MSG_AIRPLANE_COUNT_CHANGED: int
MSG_PLANE_UNLOADED: int
MSG_WILL_WRITE_PREFS: int
MSG_LIVERY_LOADED: int
MSG_ENTERED_VR: int
MSG_EXITING_VR: int
MSG_RELEASE_PLANES: int
MsgPlaneCrashed: int
MsgPlaneLoaded: int
MsgAirportLoaded: int
MsgSceneryLoaded: int
MsgAirplaneCountChanged: int
MsgPlaneUnloaded: int
MsgWillWritePrefs: int
MsgLivery_Loaded: int
MsgEnteredVr: int
MsgExitingVr: int
MsgReleasePlanes: int
MSG_FMOD_BANK_LOADED: int
MSG_FMOD_BANK_UNLOADING: int
MSG_DATAREFS_ADDED: int
MsgFmodBankLoaded: int
MsgFmodBankUnloading: int
MsgDatarefs_Added: int
MsgDatarefsAdded: int
FlightLoop_Phase_BeforeFlightModel: XPLMFlightLoopPhaseType
FlightLoop_Phase_AfterFlightModel: XPLMFlightLoopPhaseType
ProbeY: XPLMProbeType
ProbeHitTerrain: XPLMProbeResult
ProbeError: XPLMProbeResult
ProbeMissed: XPLMProbeResult
AudioRadioCom1: XPLMAudioBus
AudioRadioCom2: XPLMAudioBus
AudioRadioPilot: XPLMAudioBus
AudioRadioCopilot: XPLMAudioBus
AudioExteriorAircraft: XPLMAudioBus
AudioExteriorEnvironment: XPLMAudioBus
AudioExteriorUnprocessed: XPLMAudioBus
AudioInterior: XPLMAudioBus
AudioUI: XPLMAudioBus
AudioGround: XPLMAudioBus
Master: XPLMAudioBus
MasterBank: XPLMBankID
RadioBank: XPLMBankID
FMOD_OK: int
FMOD_SOUND_FORMAT_PCM16: int
WidgetClass_MainWindow: XPWidgetClass
MainWindowStyle_MainWindow: int
MainWindowStyle_Translucent: int
Property_MainWindowType: int
Property_MainWindowHasCloseBoxes: int
Message_CloseButtonPushed: int
WidgetClass_SubWindow: XPWidgetClass
SubWindowStyle_SubWindow: int
SubWindowStyle_Screen: int
SubWindowStyle_ListView: int
Property_SubWindowType: int
WidgetClass_Button: XPWidgetClass
PushButton: int
RadioButton: int
WindowCloseBox: int
LittleDownArrow: int
LittleUpArrow: int
ButtonBehaviorPushButton: int
ButtonBehaviorCheckBox: int
ButtonBehaviorRadioButton: int
Property_ButtonType: int
Property_ButtonBehavior: int
Property_ButtonState: int
Msg_PushButtonPressed: int
Msg_ButtonStateChanged: int
WidgetClass_TextField: XPWidgetClass
TextEntryField: int
TextTransparent: int
TextTranslucent: int
Property_EditFieldSelStart: int
Property_EditFieldSelEnd: int
Property_EditFieldSelDragStart: int
Property_TextFieldType: int
Property_PasswordMode: int
Property_MaxCharacters: int
Property_ScrollPosition: int
Property_Font: int
Property_ActiveEditSide: int
Msg_TextFieldChanged: int
WidgetClass_ScrollBar: XPWidgetClass
ScrollBarTypeScrollBar: int
ScrollBarTypeSlider: int
Property_ScrollBarSliderPosition: int
Property_ScrollBarMin: int
Property_ScrollBarMax: int
Property_ScrollBarPageAmount: int
Property_ScrollBarType: int
Property_ScrollBarSlop: int
Msg_ScrollBarSliderPositionChanged: int
WidgetClass_Caption: XPWidgetClass
Property_CaptionLit: int
WidgetClass_GeneralGraphics: XPWidgetClass
Ship: int
ILSGlideScope: int
MarkerLeft: int
_Airport: int
NDB: int
VOR: int
RadioTower: int
AircraftCarrier: int
Fire: int
MarkerRight: int
CustomObject: int
CoolingTower: int
SmokeStack: int
Building: int
PowerLine: int
VORWithCompassRose: int
OilPlatform: int
OilPlatformSmall: int
WayPoint: int
Property_GeneralGraphicsType: int
WidgetClass_Progress: XPWidgetClass
Property_ProgressPosition: int
Property_ProgressMin: int
Property_ProgressMax: int
Window_Help: XPWindowStyle
Window_MainWindow: XPWindowStyle
Window_SubWindow: XPWindowStyle
Window_Screen: XPWindowStyle
Window_ListView: XPWindowStyle
Element_TextField: XPElementStyle
Element_CheckBox: XPElementStyle
Element_CheckBoxLit: XPElementStyle
Element_WindowCloseBox: XPElementStyle
Element_WindowCloseBoxPressed: XPElementStyle
Element_PushButton: XPElementStyle
Element_PushButtonLit: XPElementStyle
Element_OilPlatform: XPElementStyle
Element_OilPlatformSmall: XPElementStyle
Element_Ship: XPElementStyle
Element_ILSGlideScope: XPElementStyle
Element_MarkerLeft: XPElementStyle
Element_Airport: XPElementStyle
Element_Waypoint: XPElementStyle
Element_NDB: XPElementStyle
Element_VOR: XPElementStyle
Element_RadioTower: XPElementStyle
Element_AircraftCarrier: XPElementStyle
Element_Fire: XPElementStyle
Element_MarkerRight: XPElementStyle
Element_CustomObject: XPElementStyle
Element_CoolingTower: XPElementStyle
Element_SmokeStack: XPElementStyle
Element_Building: XPElementStyle
Element_PowerLine: XPElementStyle
Element_CopyButtons: XPElementStyle
Element_CopyButtonsWithEditingGrid: XPElementStyle
Element_EditingGrid: XPElementStyle
Element_ScrollBar: XPElementStyle
Element_VORWithCompassRose: XPElementStyle
Element_Zoomer: XPElementStyle
Element_TextFieldMiddle: XPElementStyle
Element_LittleDownArrow: XPElementStyle
Element_LittleUpArrow: XPElementStyle
Element_WindowDragBar: XPElementStyle
Element_WindowDragBarSmooth: XPElementStyle
Track_ScrollBar: XPTrackStyle
Track_Slider: XPTrackStyle
Track_Progress: XPTrackStyle
Host_Unknown: XPLMHostApplicationID
Host_XPlane: XPLMHostApplicationID
Language_Unknown: XPLMLanguageCode
Language_English: XPLMLanguageCode
Language_French: XPLMLanguageCode
Language_German: XPLMLanguageCode
Language_Italian: XPLMLanguageCode
Language_Spanish: XPLMLanguageCode
Language_Korean: XPLMLanguageCode
Language_Russian: XPLMLanguageCode
Language_Greek: XPLMLanguageCode
Language_Japanese: XPLMLanguageCode
Language_Chinese: XPLMLanguageCode
DataFile_Situation: XPLMDataFileType
DataFile_ReplayMovie: XPLMDataFileType
CommandBegin: XPLMCommandPhase
CommandContinue: XPLMCommandPhase
CommandEnd: XPLMCommandPhase
Property_Refcon: XPWidgetPropertyID
Property_Dragging: XPWidgetPropertyID
Property_DragXOff: XPWidgetPropertyID
Property_DragYOff: XPWidgetPropertyID
Property_Hilited: XPWidgetPropertyID
Property_Object: XPWidgetPropertyID
Property_Clip: XPWidgetPropertyID
Property_Enabled: XPWidgetPropertyID
Property_UserStart: XPWidgetPropertyID
Mode_Direct: XPDispatchMode
Mode_UpChain: XPDispatchMode
Mode_Recursive: XPDispatchMode
Mode_DirectAllCallbacks: XPDispatchMode
Mode_Once: XPDispatchMode
WidgetClass_None: int
Msg_None: XPWidgetMessage
Msg_Create: XPWidgetMessage
Msg_Destroy: XPWidgetMessage
Msg_Paint: XPWidgetMessage
Msg_Draw: XPWidgetMessage
Msg_KeyPress: XPWidgetMessage
Msg_KeyTakeFocus: XPWidgetMessage
Msg_KeyLoseFocus: XPWidgetMessage
Msg_MouseDown: XPWidgetMessage
Msg_MouseDrag: XPWidgetMessage
Msg_MouseUp: XPWidgetMessage
Msg_Reshape: XPWidgetMessage
Msg_ExposedChanged: XPWidgetMessage
Msg_AcceptChild: XPWidgetMessage
Msg_LoseChild: XPWidgetMessage
Msg_AcceptParent: XPWidgetMessage
Msg_Shown: XPWidgetMessage
Msg_Hidden: XPWidgetMessage
Msg_DescriptorChanged: XPWidgetMessage
Msg_PropertyChanged: XPWidgetMessage
Msg_MouseWheel: XPWidgetMessage
Msg_CursorAdjust: XPWidgetMessage
Msg_UserStart: XPWidgetMessage
NO_PARENT: int
PARAM_PARENT: int
VERSION: str
PLUGINSPATH: str
INTERNALPLUGINSPATH: str
pythonDebugLevel: int
pythonExecutable: str
ModuleMTimes: object
def controlCamera(howLong:int=ControlCameraUntilViewChanges,
          controlFunc:Optional[Callable[[list[float], int, Any], int]]=None,
          refCon:Any=None) -> None:
    """
    Reposition camera on next drawing cycle.
      howLong: 1 = until view changes
               2 = forever
      controlFunc(position, isLosingControl, refCon) -> int
          position: list of floats (or None) you'll update with new values
                    [x, y, z, pitch, heading, roll, zoom]
          isLosingControl: 1 -> you are losing control
          refCon: reference constant provided with 'controlCamera' call
          Return 1 to reposition, 0 to surrender control.
      refCon: reference constant passed to call back func().
    """
    ...

def dontControlCamera() -> None:
    """
    Release control of camera.
    """
    ...

def isCameraBeingControlled() -> tuple[int, int]:
    """
    Returns two integer tuple. (isBeingControlled, howLong).
    
    isBeingControlled is 1 when camera is being controlled, 0 otherwise.
    howLong is
      1: Until View Changes
      2: Forever
      value is undefined when isBeingControlled is 0.
    """
    ...

def readCameraPosition() -> tuple[float, float, float, float, float, float, float]:
    """
    Returns tuple of seven floats:
     Index   Value   Meaning
     0,1,2  x,y,z    Camera's position in OpenGL coordinates
     3      pitch    In degrees, 0.0 is flat, positive for nose up.
     4      heading  In degrees, 0.0 is true north.
     5      roll     In degrees, 0.0 is flat, positive for roll right.
     6      zoom     1.0 is normal, 2.0 is 2x zoom (objects appear larger)
    """
    ...

def findDataRef(name:str) -> XPLMDataRef:
    """
    Looks up string name of data ref and returns dataRef code
    to be used with get and set data ref functions,
    or None, if name cannot be found.
    """
    ...

def canWriteDataRef(dataRef:XPLMDataRef) -> bool:
    """
    Returns True if dataRef is writable, False otherwise. Also
    returns False if provided dataRef is None.
    """
    ...

def isDataRefGood(dataRef:XPLMDataRef) -> bool:
    """
    (Deprecated, do not use.)
    """
    ...

def getDataRefTypes(dataRef:XPLMDataRef) -> int:
    """
    Returns or'd values of data type(s) supported by dataRef.
       1 Type_Int
       2 Type_Float
       4 Type_Double
       8 Type_FloatArray
      16 Type_IntArray
      32 Type_Data
    """
    ...

def getDatai(dataRef:XPLMDataRef) -> int:
    """
    Returns integer value for dataRef.
    """
    ...

def setDatai(dataRef:XPLMDataRef, value:int=0) -> None:
    """
    Sets integer value for dataRef.
    """
    ...

def getDataf(dataRef:XPLMDataRef) -> float:
    """
    Returns float value for dataRef.
    """
    ...

def setDataf(dataRef:XPLMDataRef, value:float=0.0) -> None:
    """
    Sets float value for dataRef.
    """
    ...

def getDatad(dataRef:XPLMDataRef) -> float:
    """
    Returns double value for dataRef (as a python float)
    """
    ...

def setDatad(dataRef:XPLMDataRef, value:float=0.0) -> None:
    """
    Sets double value for dataRef.
    """
    ...

def getDatavi(dataRef:XPLMDataRef, values:Optional[list[int]]=None, offset:int=0, count:int=-1) -> int:
    """
    Get integer array value for dataRef.
    
    If values is None, return number of elements in the array (only).
    Otherwise, values should be a list into which will be copied elements
    from the dataRef, starting at offset, and continuing for count # of elements.
    If count is negative, or unspecified, all elements (relative offset) are copied.
    
    Returns the number of elements copied.
    """
    ...

def setDatavi(dataRef:XPLMDataRef, values:list[int], offset:int=0, count:int=-1) -> None:
    """
    Set integer array value for dataRef.
    
    values is a list of integers, to be written into dataRef starting
    at offset. Up to count values are written.
    
    If count is negative (or not provided), all values in the list are copied.
    It is an error for count to be greater than the length of the list.
    
    No return value.
    """
    ...

def getDatavf(dataRef:XPLMDataRef, values:Optional[list[float]]=None, offset:int=0, count:int=-1) -> int:
    """
    Get float array value for dataRef.
    
    If values is None, return number of elements in the array (only).
    Otherwise, values should be a list into which will be copied elements
    from the dataRef, starting at offset, and continuing for count # of elements.
    If count is negative, or unspecified, all elements (relative offset) are copied.
    
    Returns the number of elements copied.
    """
    ...

def setDatavf(dataRef:XPLMDataRef, values:list[float], offset:int=0, count:int=-1) -> None:
    """
    Set float array value for dataRef.
    
    values is a list of floats, to be written into dataRef starting
    at offset. Up to count values are written.
    
    If count is negative (or not provided), all values in the list are copied.
    It is an error for count to be greater than the length of the list.
    
    No return value.
    """
    ...

def getDatab(dataRef:XPLMDataRef, values:Optional[list[int]]=None, offset:int=0, count:int=-1) -> int:
    """
    Get byte array value for dataRef.
    
    If values is None, return number of elements in the array (only).
    Otherwise, values should be a list into which will be copied elements
    from the dataRef, starting at offset, and continuing for count # of elements.
    If count is negative, or unspecified, all elements (relative offset) are copied.
    
    See also getDatas().
    
    Returns the number of elements copied.
    """
    ...

def setDatab(dataRef:XPLMDataRef, values:list[int] | bytes, offset:int=0, count:int=-1) -> None:
    """
    Set byte array value for dataRef.
    
    values is a list of bytes, to be written into dataRef starting
    at offset. Up to count values are written.
    
    If count is negative (or not provided), all values in the list are copied.
    It is an error for count to be greater than the length of the list.
    
    See also setDatas().
    
    No return value.
    """
    ...

def getDatas(dataRef:XPLMDataRef, offset:int=0, count:int=-1) -> str | None:
    """
    Returns string value for dataRef.
    
    String is the first null-terminated sequence found in the byte-array
    dataRef, starting at offset. If count is given, string returned is
    restricted to count length (not including a null byte).
    
    Note not all byte-array dataRefs are strings: be sure the requested
    dataRef is storing character information. Otherwise use getDatab().
    """
    ...

def setDatas(dataRef:XPLMDataRef, value:str, offset:int=0, count:int=-1) -> None:
    """
    Set byte array to string value for dataRef.
    
    value is a python unicode string (capable of being encoded as 'UTF-8').
    String is written into the dataRef starting at offset. Up to count
    characters are written. If count is more than len(value), the written
    values are padded with zeros ('\\x00') up to count.
    
    If count is negative (or not provided), value is copied AND the
    remaining length of the dataRef is zero-filled. Use count to limit
    the amount of padding.
    
    If len(value) is greater than existing dataRef value, and count is not
    specified, the underlying dataRef is NOT extended to accommodate the
    full string. Instead the string is copied upto the end of the existing
    data. To extend the underlying dataRef, provide a larger value for count.
    
    Caution: extend dataRef only if the underlying dataRef is implemented in 
    python. Attempting to extend non-python dataRefs will cause the sim to
    crash.
    
    No return value.
    """
    ...

def registerDataAccessor(name:str, dataType:int=Type_Unknown, writable:int=-1, 
          readInt:Optional[Callable[[Any], int]]=None, writeInt:Optional[Callable[[Any, int], None]]=None, 
          readFloat:Optional[Callable[[Any], float]]=None, writeFloat:Optional[Callable[[Any, float], None]]=None, 
          readDouble:Optional[Callable[[Any], float]]=None, writeDouble:Optional[Callable[[Any, float], None]]=None, 
          readIntArray:Optional[Callable[[Any, list[int], int, int], int]]=None, writeIntArray:Optional[Callable[[Any, list[int], int, int], None]]=None, 
          readFloatArray:Optional[Callable[[Any, list[float], int, int], int]]=None, writeFloatArray:Optional[Callable[[Any, list[float], int, int], None]]=None, 
          readData:Optional[Callable[[Any, list[int], int, int], int]]=None, writeData:Optional[Callable[[Any, list[int], int, int], None]]=None, 
          readRefCon:Any=None, writeRefCon:Any=None) -> XPLMDataRef:
    """
    Register data accessors for provided string name.
    
    Provide one or more read/write callback functions which implement
    get/set access. If dataType is Type_Unknown, or writable is -1, we'll
    calculate their value to match provided callbacks.
    
    Two optional refCon are available, to be passed to your get/set functions.
    
    * Scalar get callback functions take single (refCon) parameter 
      and return the value.
    * Scalar set callback functions take (refCon, value) parameters 
      with no return.
    * Vector gets take (refCon, values, offset, count), return # elements
      copied into values.
    * Vector sets take (refCon, values, offset, count), with no return
    """
    ...

def unregisterDataAccessor(acccessor:XPLMDataRef) -> None:
    """
    Unregisters data accessor.
    """
    ...

def shareData(name:str, dataType:int, 
          dataChanged:Optional[Callable[[Any], None]]=None, refCon:Any=None) -> int:
    """
    Create shared data ref with provided name and dataType.
    
    Optionally provide a callback function which will be called whenever
    this data ref has been changed.
    
    Callback takes single (refCon) parameter
    
    Returns 1 on success 0 otherwise.
    """
    ...

def countDataRefs() -> int:
    """
    Returns the total number of datarefs that have been registered in X-Plane.
    """
    ...

def getDataRefsByIndex(offset:int=0, count:int=1) -> None | list[XPLMDataRef]:
    """
    Returns list of dataRefs, each similar to return from xp.findDataRef().
    Use xp.getDataRefInfo() to access information about the dataref.
    As a special case, count=-1 returns all datarefs starting from offset to the end.
    
    CAUTION: requesting datarefs greater than countDataRefs() returns garbage. If you
    try to use these, you may crash the sim.
    """
    ...

def getDataRefInfo(dataRef:XPLMDataRef) -> XPLMDataRefInfo_t:
    """
    Return DataRefInfo object for provided dataRef.
    
    XPLMDataRefInfo_t object is .name, .type, .writable, .owner
      recall type is a bitfield, see xp.getDataRefTypes()
    """
    ...

def unshareData(name:str, dataType:int, dataChanged:Optional[Callable[[Any], None]]=None, refCon:Any=None) -> int:
    """
    Unshare data. If dataChanged function was provided with initial shareData()
    the callback will no longer be called on data changes.
    All parameter values must match those provided with shareData()
    in order to be successful.
    
    Returns 1 on success, 0 otherwise
    """
    ...

def registerDrawCallback(draw:Callable[[XPLMDrawingPhase, int, Any], None | int], phase:XPLMDrawingPhase=Phase_Window, after:int=0, refCon:Any=None) -> int:
    """
    Registers  low-level drawing callback.
    
    The after parameter indicates you want to be called before (0) or after (1) phase.
    draw() callback function takes three parameters (phase, after, refCon), returning 
    0 to suppress further X-Plane drawing in the phase, or 1 to allow X-Plane to finish
    (Callback's value is ignored if after=1).
    
    Registration returns 1 on success, 0 otherwise.
    """
    ...

def registerAvionicsCallbacksEx(deviceId:XPLMDeviceID, 
          before:Optional[Callable[[XPLMDeviceID, int, Any], int]]=None, 
          after:Optional[Callable[[XPLMDeviceID, int, Any], int]]=None, 
          refCon:Any=None) -> XPLMAvionicsID:
    """
    Registers draw callback for particular device.
    
    Registers drawing callback(s) to enhance or replace X-Plane drawing. For
    'before' callback, return 1 to let X-Plane draw or 0 to suppress X-Plane
    drawing. Return value for 'after' callback is ignored.
    
    Upon entry, OpenGL context will be correctly set in panel coordinates for 2d drawing.
    OpenGL state (texturing, etc.) will be unknwon.
    
    Successful registration returns an AvionicsID.
    
    """
    ...

def unregisterAvionicsCallbacks(avionicsId:XPLMAvionicsID) -> None:
    """
    Unregisters avionics draw callback(s) associated with given avionicsId.
    
    Does not return a value.
    
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

def setWindowGeometry(windowID:XPLMWindowID, left:int, top:int, right:int, bottom:int) -> None:
    """
    Sets window geometry.
    """
    ...

def getWindowGeometryOS(windowID:XPLMWindowID) -> None | tuple[int, int, int, int]:
    """
    Returns window geometry for popped-out window (left, top, right, bottom).
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
    Bring window to front (of it's window layer).
    """
    ...

def isWindowInFront(windowID:XPLMWindowID) -> int:
    """
    Returns 1 if window is currently in the front of it's window layer).
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

def setGraphicsState(fog:int=0, numberTexUnits:int=0, lighting:int=0, alphaTesting:int=0, alphaBlending:int=0, depthTesting:int=0, depthWriting:int=0) -> None:
    """
    Change OpenGL's graphics state.
    
    Use instead of any glEnable / glDisable calls.
    """
    ...

def bindTexture2d(textureID:int, textureUnit:int) -> None:
    """
    Changes currently bound OpenGL texture.
    
    Use instead of glBindTexture(GL_TEXTURE_2D, ...)
    """
    ...

def generateTextureNumbers(count:int=1) -> None | list[int]:
    """
    Generate number of textures for a plugin.
    
    Returns list of numbers.
    """
    ...

def worldToLocal(lat:float, lon:float, alt:float=0.0) -> tuple[float, float, float]:
    """
    Convert Lat/Lon/Alt to local scene coordinates (x, y, z)
    
    Latitude and longitude are decimal degrees, altitude is meters MSL.
    Returns (x, y, z) in meters, in local OpenGL coordinates.
    """
    ...

def localToWorld(x:float, y:float, z:float) -> tuple[float, float, float]:
    """
    Convert local scene coordinates (x, y, z) into (lat, lon, alt)
    
    Latitude and longitude are decimal degrees, altitude is meters MSL.
    """
    ...

def drawTranslucentDarkBox(left:int, top:int, right:int, bottom:int) -> None:
    """
    Draw translucent dark box at location.
    """
    ...

def drawString(rgb:Sequence[float]=(1., 1., 1.), 
          x:int=0, y:int=0, value:str='', 
          wordWrapWidth:Optional[int]=None, fontID:XPLMFontID=Font_Proportional) -> None:
    """
    Draw a string at location (x, y)
    
    Default color is white (1., 1., 1.)
    """
    ...

def drawNumber(rgb:Sequence[float]=(1., 1., 1.), x:int=0, y:int=0, value:float=0.0, digits:int=-1, decimals:int=0, showSign:int=1, fontID:XPLMFontID=Font_Proportional) -> None:
    """
    Draw a number at location (x, y)
    
    Default color is white (1., 1., 1.)
    """
    ...

def getFontDimensions(fontID:XPLMFontID) -> None | tuple[int, int, int]:
    """
    Get information about font
    
    Returns (width, height, digitsOnly). Proportional fonts
    return hopefully average width.
    """
    ...

def measureString(fontID:XPLMFontID, string:str) -> float:
    """
    Returns floating point width of string, with indicated font.
    """
    ...

def createInstance(obj:XPLMObjectRef, dataRefs:Optional[tuple[str]]=None) -> XPLMInstanceRef:
    """
    Create Instance for object retrieved by loadObject() or loadObjectAsync().
    
    Provide list of string dataRefs to be registered for this object.
    Returns object instance, to be used with instanceSetPosition().
    """
    ...

def destroyInstance(instance:XPLMObjectRef) -> None:
    """
    Destroys instance created by createInstance().
    """
    ...

def instanceSetPosition(instance:XPLMObjectRef, position:XPLMDrawInfo_t | tuple[float, float, float, float, float, float], 
          data:Optional[tuple[float]]=None) -> None:
    """
    Update position (x, y, z, pitch, heading, roll), 
    and all datarefs (<float>, <float>, ...)
    
    You should provide values for all registered dataref, otherwise they'll
    be set to 0.0.
    
    """
    ...

def createMapLayer(mapType:str=MAP_USER_INTERFACE, 
          layerType:XPLMMapLayerType=MapLayer_Markings, 
          delete:Optional[Callable[[XPLMMapLayerID, Any], None]]=None, 
          prep:Optional[Callable[[XPLMMapLayerID, float, XPLMMapProjectionID, Any], None]]=None, 
          draw:Optional[Callable[[XPLMMapLayerID, float, float, float, XPLMMapStyle, XPLMMapProjectionID, Any], None]]=None, 
          icon:Optional[Callable[[XPLMMapLayerID, float, float, float, XPLMMapStyle, XPLMMapProjectionID, Any], None]]=None, 
          label:Optional[Callable[[XPLMMapLayerID, float, float, float, XPLMMapStyle, XPLMMapProjectionID, Any], None]]=None, 
          showToggle:int=1, 
          name:str='', 
          refCon:Any=None) -> XPLMMapLayerID:
    """
    Returns layerID of newly created map layer, setting callbacks.
    
    If map does not currently exist, returns 0.
    """
    ...

def destroyMapLayer(layerID:XPLMMapLayerID) -> int:
    """
    Destroys map layer given by layerID.
    """
    ...

def registerMapCreationHook(mapCreated:Callable[[str, Any], None], refCon:Any=None) -> None:
    """
    Registers mapCreated() callback to notify you when a map is created.
    
    Callback gets two parameters: (mapType, refCon)
    """
    ...

def mapExists(mapType:str) -> int:
    """
    Returns 1 if mapType exists, 0 otherwise.
    
    mapType is either xp.MAP_USER_INTERFACE or xp.MAP_IOS
    """
    ...

def drawMapIconFromSheet(layerID:XPLMMapLayerID, png:str, s:int, t:int, ds:int, dt:int, x:float, y:float, orientation:XPLMMapOrientation, rotationDegrees:float, mapWidth:float) -> None:
    """
    Draws icon into map layer.
    
    Only valid within iconLayer() callback.
    """
    ...

def drawMapLabel(layerID:XPLMMapLayerID, text:str, x:float, y:float, orientation:XPLMMapOrientation, rotationDegrees:float) -> None:
    """
    Draws label within map layer.
    
    Only valid within labelLayer() callback.
    """
    ...

def mapProject(projection:XPLMMapProjectionID, latitude:float, longitude:float) -> None | tuple[float, float]:
    """
    Returns map layer (x, y) for given latitude, longitude.
    
    Only valid within map layer callbacks.
    """
    ...

def mapUnproject(projection:XPLMMapProjectionID, x:float, y:float) -> None | tuple[int, int]:
    """
    Returns latitude, longitude for given map coordinates.
    
    Only valid within map layer callbacks.
    """
    ...

def mapScaleMeter(projection:XPLMMapProjectionID, x:float, y:float) -> float:
    """
    Returns number of units for 'one meter' using current projection.
    
    Only valid within map layer callbacks.
    """
    ...

def mapGetNorthHeading(projection:XPLMMapProjectionID, x:float, y:float) -> float:
    """
    Returns mapping angle for map projection at point.
    
    Only valid within map layer callbacks.
    """
    ...

def findPluginsMenu() -> XPLMMenuID:
    """
    Returns menuID of plugins-menu.
    """
    ...

def findAircraftMenu() -> XPLMMenuID:
    """
    Returns menuID of currently loaded aircraft plugins menu.
    
    Note this is always 'None' for XPPython3.
    """
    ...

def createMenu(name:Optional[str]=None, parentMenuID:Optional[XPLMMenuID]=None, parentItem:Optional[int]=0, 
          handler:Optional[Callable[[Any, Any], None]]=None, refCon:Optional[Any]=None) -> None | XPLMMenuID:
    """
    Creates menu, returning menuID or None on error.
    
    parentMenuID=None adds menu to PluginsMenu.
    """
    ...

def destroyMenu(menuID:XPLMMenuID) -> None:
    """
    Remove submenu from provided menuID.
    """
    ...

def clearAllMenuItems(menuID:Optional[XPLMMenuID]) -> None:
    """
    Remove menu items from provided menuID, or 'all menus', if menuID is None.
    """
    ...

def appendMenuItem(menuID:Optional[XPLMMenuID]=None, name:str='Item', refCon:Any=None) -> int:
    """
    Appends new menu item to end of existing menuID.
    
    Returns index for created menu item or -1 on error.
    """
    ...

def appendMenuItemWithCommand(menuID:Optional[XPLMMenuID]=None, name:str='Command', commandRef:Any=None) -> int:
    """
    Adds menu item to existing menuID, and executes commandRef when selected.
    
    Returns index for created menu item or -1 on error.
    """
    ...

def appendMenuSeparator(menuID:XPLMMenuID=None) -> None | int:
    """
    Adds separator to end of menu.
    
    Returns index of created item for XP11, None for XP12
    
    """
    ...

def setMenuItemName(menuID:Optional[XPLMMenuID]=None, index:int=0, name:str='New Name') -> None:
    """
    Change the name of and existing menu item.
    """
    ...

def checkMenuItem(menuID:Optional[XPLMMenuID]=None, index:int=0, checked:XPLMMenuCheck=Menu_Checked) -> None:
    """
    Set checkmark for given menu item.
    
      Menu_NoCheck = 0
      Menu_UnChecked = 1
      Menu_Checked = 2
    
    """
    ...

def checkMenuItemState(menuID:Optional[XPLMMenuID], index:int=0) -> int:
    """
    Returns menu item checked state.
    
      Menu_NoCheck = 0
      Menu_UnChecked = 1
      Menu_Checked = 2
    
    """
    ...

def enableMenuItem(menuID:Optional[XPLMMenuID]=None, index:int=0, enabled:int=1) -> None:
    """
    Enables menu item
    
    Use enabled=0 to disable item, 1 to enable
    """
    ...

def removeMenuItem(menuID:Optional[XPLMMenuID]=None, index:int=0) -> None:
    """
    Removes one item from menu.
    
    Note that all menu items below are moved up one index.
    """
    ...

def getFirstNavAid() -> int:
    """
    Returns navRef of first entry in navaid database.
    """
    ...

def getNextNavAid(navRef:XPLMNavRef) -> int:
    """
    Returns next navRef after the provided value.
    """
    ...

def findFirstNavAidOfType(navType:XPLMNavType) -> int:
    """
    Returns navRef of first navAid of given type.
    
    Types are:
      Nav_Unknown        =0
      Nav_Airport        =1
      Nav_NDB            =2
      Nav_VOR            =4
      Nav_ILS            =8
      Nav_Localizer     =16
      Nav_GlideSlope    =32
      Nav_OuterMarker   =64
      Nav_MiddleMarker =128
      Nav_InnerMarker  =256
      Nav_Fix          =512
      Nav_DME         =1024
      Nav_LatLon      =2048
    
    """
    ...

def findLastNavAidOfType(navType:XPLMNavType) -> int:
    """
    Returns navRef of last navAid of given type.
    """
    ...

def findNavAid(name:Optional[str], navAidID:Optional[str], lat:Optional[float], 
          lon:Optional[float], freq:Optional[int], navType:XPLMNavType=Nav_Any) -> XPLMNavRef:
    """
    Returns navRef of last navAid matching information.
    
    name and navAidID are case-sensitive and will match a fragment of the actual value.
    freq is an integer, 100x the real frequency value (eg. 137.75 -> 13775) except for NDB.
    navType=0xffffffff will match any type value
    """
    ...

def getNavAidInfo(navRef:XPLMNavRef) -> NavAidInfo:
    """
    Returns NavAidInfo object for given navRef
    
    Attibutes are:
     .type        # a NavType
     .latitude
     .longitude
     .height     # in meters
     .frequency  # integer, for NDB, value is exact, otherwise devide by 100.0
     .heading    # See documentation for glideslope headings
     .navAidID
     .reg        # =1 if navaid is within local 'region' of loaded DSFs
    """
    ...

def countFMSEntries() -> int:
    """
    Returns number of FMS Entries
    """
    ...

def getDisplayedFMSEntry() -> int:
    """
    Returns index number of currently displayed FMS entry.
    """
    ...

def getDestinationFMSEntry() -> int:
    """
    Returns index number of destination FMS entry.
    """
    ...

def setDisplayedFMSEntry(index:int) -> None:
    """
    Sets index number for FMS Entry to be displayed.
    """
    ...

def setDestinationFMSEntry(index:int) -> None:
    """
    Sets index number for FMS Entry to become the current destination.
    """
    ...

def getFMSEntryInfo(index:int) -> FMSEntryInfo:
    """
    Return FMSEntryInfo object for given FMS Entry index.
    
    Attributes are:
     .type      # a NavType
     .navAidID
     .ref       # navRef (use as input to getNavAidInfo())
     .altitude  # (in feet)
     .lat
     .lon
    """
    ...

def setFMSEntryInfo(index:int, navRef:XPLMNavRef, altitude:int=0) -> None:
    """
    Set given FMS Entry to provided navRef and altitude (feet)
    """
    ...

def setFMSEntryLatLon(index:int, lat:float, lon:float, altitude:int=0) -> None:
    """
    Set given FMS Entry to provided (lat, lon) and altitude(feet).
    """
    ...

def clearFMSEntry(index:int) -> None:
    """
    Clear given FMS entry.
    """
    ...

def getGPSDestinationType() -> int:
    """
    Return NavType of current GPS Destination.
    """
    ...

def getGPSDestination() -> int:
    """
    Return navRef of current GPS Destination.
    """
    ...

def setUsersAircraft(path:str) -> None:
    """
    Change the user's aircraft and reinitialize.
    
    path is either relative X-Plane root, or fully qualified,
    including the .acf extension.
    """
    ...

def placeUserAtAirport(code:str) -> None:
    """
    Place user at given airport, specified by ICAO code.
    
    Invalid airport code will crash the sim.
    """
    ...

def placeUserAtLocation(latitude:float, longitude:float, elevation:float, heading:float, speed:float) -> None:
    """
    Place user aircraft at location with engines running.
    
    elevation is meters, heading is True, speed is meters per second
    """
    ...

def countAircraft() -> tuple[int, int, XPLMPluginID]:
    """
    Return tuple of (#a/c configured, #a/c active, pluginID of controlling plugin
    """
    ...

def getNthAircraftModel(index:int) -> tuple[str, str]:
    """
    Return (filename, fullPath) of aircraft at index
    """
    ...

def acquirePlanes(aircraft:Optional[Sequence[str]]=None, callback:Optional[Callable[[Any], None]]=None, refCon:Any=None) -> int:
    """
    Get exclusive access to aircraft.
    
    1 on success, 0 otherwise. You callback will be called
    with refCon if acquirePlanes() is not immediately successful.
    """
    ...

def releasePlanes() -> None:
    """
    Release all planes, acquired using acquirePlanes().
    """
    ...

def setActiveAircraftCount(count:int) -> None:
    """
    Set number of active planes.
    
    Only sets up to number of available planes. You must
    first have access -- acquirePlanes().
    """
    ...

def setAircraftModel(index:int, path:str) -> None:
    """
    Load aircraft model into index. (Do not use index=0)
    
    Path is absolute, or relative to X-Plane root.
    """
    ...

def disableAIForPlane(index:int) -> None:
    """
    Turn off AI for given airplane.
    
    Plane will continue to draw, but will not move itself.
    """
    ...

def getMyID() -> int:
    """
    Returns plugin ID of calling plugin.
    
    For XPPython3, this is *always* the ID of the XPPython3 plugin
    not any particular python plugin.
    """
    ...

def countPlugins() -> int:
    """
    Return total number of (non-python) plugins
    """
    ...

def getNthPlugin(index:int) -> XPLMPluginID:
    """
    Return the ID of a (non-python) plugin by index.
    """
    ...

def findPluginByPath(path:str) -> XPLMPluginID:
    """
    Return pluginID of (non-python) plugin whose file exists at path.
    
    Path must be absolute.
    """
    ...

def findPluginBySignature(signature:str) -> XPLMPluginID:
    """
    Return the pluginID of the (non-python) plugin whose signature matches.
    """
    ...

def getPluginInfo(pluginID:XPLMPluginID) -> PluginInfo:
    """
    Return information about plugin.
    
    Return value is an object with attributes:
      .name
      .filePath
      .signature
      .description
    """
    ...

def isPluginEnabled(pluginID:XPLMPluginID) -> int:
    """
    Return 1 if plugin is enabled, 0 otherwise
    """
    ...

def enablePlugin(pluginID:XPLMPluginID) -> int:
    """
    Enables plugin.
    """
    ...

def disablePlugin(pluginID:XPLMPluginID) -> None:
    """
    Disables plugin
    """
    ...

def reloadPlugins() -> None:
    """
    Reload *all* plugins.
    
    Likely crashes the sim. DO NOT USE.
    """
    ...

def sendMessageToPlugin(pluginID:XPLMPluginID, message:int, param:Optional[Any]) -> None:
    """
    Send message to plugin
    
    Messages sent to XPPython3 plugin will be forwarded to all
    python plugins.
    """
    ...

def hasFeature(feature:str) -> int:
    """
    Return 1 if X-Plane supports feature.
    """
    ...

def isFeatureEnabled(feature:str) -> int:
    """
    Returns 1 if feature is currently enabled for your plugin.
    """
    ...

def enableFeature(feature:str, enable:int=1) -> None:
    """
    Enables / disables indicated feature for this plugin.
    """
    ...

def enumerateFeatures(enumerator:Callable[[str, Any], None], refCon:Any=None) -> None:
    """
    Enumerate supported features
    
    You callback takes (name, refCon) as parameters
    """
    ...

def getElapsedTime() -> float:
    """
    Return elapsed time since sim started.
    """
    ...

def getCycleNumber() -> int:
    """
    Get cycle number, increased for each cycle computed by sim.
    """
    ...

def registerFlightLoopCallback(callback:Callable[[float, float, int, Any], float], interval:float=0.0, refCon:Any=None) -> None:
    """
    Register flight loop callback.
    
    interval indicates when you'll be called next:
      0= deactivate 
      >0 seconds 
      <0 flightLoops
    Callback function gets (lastCall, elapsedTime, counter, refCon)
    """
    ...

def unregisterFlightLoopCallback(callback:Callable[[float, float, int, Any], float], refCon:Any=None) -> None:
    """
    Unregisters flightloop matching callback and refCon.
    
    Input must match that provided to registerFlightLoopCallback().
    """
    ...

def setFlightLoopCallbackInterval(callback:Callable[[float, float, int, Any], float], interval:float=0.0, relativeToNow:int=1, refCon:Any=None) -> None:
    """
    Change interval associated with callback, refCon.
    
    Must have been previously registered with registerFlightLoopCallback()
    """
    ...

def createFlightLoop(callback:Callable[[float, float, int, Any], float], 
          phase:XPLMFlightLoopPhaseType=FlightLoop_Phase_BeforeFlightModel, refCon:Any=None) -> XPLMFlightLoopID:
    """
    Create flight loop, returning flightLoopID
    
    Callback take (sinceLast, elapsedTime, counter, refCon)
    returning interval (0=stop, >0 seconds, <0 flightloops)
    phase is 0=before or 1=After flight model integration
    """
    ...

def destroyFlightLoop(flightLoopID:XPLMFlightLoopID) -> None:
    """
    Destroys flight loop previously created by createFlightLoop()
    """
    ...

def scheduleFlightLoop(flightLoopID:XPLMFlightLoopID, interval:float=0.0, relativeToNow:int=1) -> None:
    """
    Change interval associated with flight loop
    
      0= stop
      >0 seconds
      <0 flightLoops
    If relativeToNow is 1, interval is relative to now, duh, otherwise
    interval is relative to previous callback execution.
    """
    ...

def createProbe(probeType:XPLMProbeType=ProbeY) -> XPLMProbeRef:
    """
    Return a probeRef
    """
    ...

def destroyProbe(probe:XPLMProbeType) -> None:
    """
    Destroy a probeRef
    """
    ...

def probeTerrainXYZ(probeRef:XPLMProbeRef, x:float, y:float, z:float) -> XPLMProbeInfo_t:
    """
    Probe terrain using probeRef at (x, y, z) location
    
    Object returned as attributes:
      .result:    0=Hit, 1=Error, 2=Missed
      .locationX,
      .locationY,
      .locationZ: OpenGL point hit by probe
      .normalX,
      .normalY,
      .normalZ:   Normal vector (e.g. slope),
      .velocityX,
      .velocityY,
      .velocityZ: Velocity vector (e.g., meter/sec) of meovement
      .is_set:    1=we hit water
    """
    ...

def getMagneticVariation(latitude:float, longitude:float) -> float:
    """
    Magnetic declination at point
    """
    ...

def degTrueToDegMagnetic(degrees:float=0.0) -> float:
    """
    Convert degrees True to degrees Magnetic, at user's current location
    """
    ...

def degMagneticToDegTrue(degrees:float=0.0) -> float:
    """
    Convert degrees Magnetic to degrees True, at user's current location
    """
    ...

def loadObject(path:str) -> XPLMObjectRef:
    """
    Load OBJ file from path, returning objectRef
    
    Path may be absolute, or relative X-Plane Root
    """
    ...

def loadObjectAsync(path:str, loaded:Callable[[XPLMObjectRef, Any], None], refCon:Any=None) -> None:
    """
    Loads OBJ asynchronously, calling callback on completion.
    
    Callback signature is loaded(objecRef, refCon)
    Object path is absolute or relative X-Plane root.
    """
    ...

def unloadObject(objectRef:XPLMObjectRef) -> None:
    """
    Unloads objectRef
    """
    ...

def lookupObjects(path:str, latitude:float=0.0, longitude:float=0.0, 
          enumerator:Optional[Callable[[str, Any], None]]=None, refCon:Any=None) -> int:
    """
    Converts virtual path to file paths, calling enumerator with info
    
    Path is virual path, which may have zero or more matching file paths
    in library. Count of results is returned by lookupObjects().
    For each item found, enumerator(path, refCon) is called.
    """
    ...

def getFMODStudio() -> FMOD_STUDIO_SYSTEM:
    """
    Get PyCapsule to FMOD_STUDIO_SYSTEM, allowing you to load/process whatever
    else you need. You will need to use python ctypes to access. See
    documentation.
    """
    ...

def getFMODChannelGroup(audioType:XPLMAudioBus) -> FMOD_CHANNELGROUP:
    """
    Returns PyCapsule to the FMOD_CHANNELGROUP with the given index.
    You will need to use python ctypes to access. See documentation.
    """
    ...

def playPCMOnBus(audioBuffer:Any, bufferSize:int, soundFormat:int, freqHz:int, numChannels:int, loop:int=0, 
          audioType:XPLMAudioBus=AudioUI, callback:Optional[Callable[[Any, int], None]]=None, refCon:Any=None) -> None | FMODChannel:
    """
    Play provided data, of length bufferSize on the bus indicatedd by audioType. On
    completion, or stoppage, invoke (optional) callback with provided refCon.
     * soundFormat is # bytes per frame 1=8bit, 2=16bit, etc.
     * freqHz is sample framerate, e.g., 800, 22000, 44100
     * numChannels is e.g., 1=mono, 2=stereo
    
    Return audio FMODChannel on success.
    """
    ...

def stopAudio(channel:FMOD_CHANNEL) -> int:
    """
    Stop playing an active FMOD channel. If you defined a completion callback,
    this will be called. After this, the FMOD::Channel* will no longer be valid
    and must not be used in any future calls
    
    Returns FMOD_RESULT, 0= FMOD_OK
    """
    ...

def setAudioPosition(channel:FMOD_CHANNEL, position:Sequence[float], velocity:Optional[Sequence[float]]=None) -> int:
    """
    For audio channel, set position (for panning and attenuation) 
    and velocity (for use with doppler).
     * position is OpenGL position (x, y, z) -- list of three float.
     * velocity is (meters/second) change in each (x, y, z) direction. None = (0, 0, 0)
    """
    ...

def setAudioFadeDistance(channel:FMOD_CHANNEL, min_distance:float=1.0, max_distance:float=10000.0) -> int:
    """
    Sets minimum and maximum distance for the channel.
    When the listener is in-between the minimum distance and the source, the volume
    will be at it's maximum. As the listener moves from the minimum distance to the
    maximum distance, the sound will attenuate. When outside the maximum distance
    the sound will no longer attenuate.
    
    Use minimum distance to give the impression that the sound is loud or soft: Small
    quiet objects such as a bumblebee, set minimum to 0.1. This would cause it to 
    attenuate quickly and dissapear when only a few meters away. A jumbo jet minimum
    might be 100 meters, thereby maintaining maximum volume until 100 meters away, with
    fade out over the next hundred meters.
    
    Maximum distance is effectively obsolete unless you need the sound to stop fading
    at a certain point. Do not adjust this from the default if you don't need to. Do
    not confuse maximum distance as the point where the sound will fade to zero, this
    is not the case.
    """
    ...

def setAudioVolume(channel:FMOD_CHANNEL, volume:float=1.0) -> int:
    """
    Set the current volume of an active FMOD channel. This should be used to
    handle changes in the audio source volume, not for fading with distance.
    Values from 0.0 to 1.0 are normal, above 1 can be used to artificially amplify
    a sound.
    """
    ...

def setAudioPitch(channel:FMOD_CHANNEL, pitch:float=1.0) -> int:
    """
    Change the current pitch of an active FMOD channel.
    This is a multiplier to the original channel value *not* a new frequency,
    so '0.8' lowers the pitch, '1.2' raises the pitch, and '1.0' sets the
    pitch to same as initialized.
    """
    ...

def setAudioCone(channel:FMOD_CHANNEL, inside_angle:float=360.0, outside_angle:float=360.0, outside_volume:float=1.0, orientation:Optional[tuple[float, float, float]]=None) -> int:
    """
    Set a direction code for an active FMOD channel. The orientation vector is in local coordinates.
    This will set the sound to 3D if it is not already.
     * inside_angle: degrees, within this angle, sound is at normal volume.
     * outside_angle: degrees, outside of this angle, sound is at outside volume.
     * outside_volume: sound volume when greater than outside angle, 0 to 1.0 default=1.0
     * Orientation of sound: OpenGL vector (x, y, z). None == (0,0,-1) which is due North
    """
    ...

def drawWindow(left:int, bottom:int, right:int, top:int, style:XPWindowStyle=Window_MainWindow) -> None:
    """
    Draw window at location
    """
    ...

def getWindowDefaultDimensions(style:XPWindowStyle=Window_MainWindow) -> tuple[int, int]:
    """
    Default dimension for indicated style
    
    Returns (width, height)
    """
    ...

def drawElement(left:int, bottom:int, right:int, top:int, style:XPElementStyle, lit:int=0) -> None:
    """
    Draw element, possibly lit, at location.
    """
    ...

def getElementDefaultDimensions(style:XPElementStyle) -> tuple[int, int, int]:
    """
    Default dimension for indicated element
    
    Returns (width, height, canBeLit)
    """
    ...

def drawTrack(left:int, bottom:int, right:int, top:int, minValue:int, maxValue:int, value:int, style:XPTrackStyle, lit:int=0) -> None:
    """
    Draw track at location, with min/max values and current value.
    
    Track may be 'reversed' if minValue > maxValue.
    Styles are:
      Track_Scrollbar=0
      Track_Slider   =1
      Track_Progress =2
    """
    ...

def getTrackDefaultDimensions(style:XPTrackStyle) -> tuple[int, int]:
    """
    Default dimension for indicated track style
    
    Returns (width, canBeLit)
    """
    ...

def getTrackMetrics(left:int, bottom:int, right:int, top:int, minValue:int, maxValue:int, value:int, style:XPTrackStyle) -> TrackMetrics:
    """
    Return object with metrics about track
    
    Object attributes are:
      .isVertical
      .downBtnSize
      .downPageSize
      .thumbSize
      .upPageSize
      .upBtnSize
    """
    ...

def speakString(string:str) -> None:
    """
    Display string in translucent overlay and speak string
    """
    ...

def getVirtualKeyDescription(vKey:int) -> str:
    """
    Return human-readable string describing virtual key
    """
    ...

def reloadScenery() -> None:
    """
    Reload current set of scenery
    """
    ...

def getSystemPath() -> str:
    """
    Return full page to X-Plane folder, with trailing '/'
    """
    ...

def getPrefsPath() -> str:
    """
    Get path the *file* within X-Plane's preferences directory.
    """
    ...

def getDirectorySeparator() -> str:
    """
    Get string used for directory separator for the current platform.
    
    Don't use this, use python os.path.join() related routines.
    """
    ...

def extractFileAndPath(fullPath:str) -> tuple[str, str]:
    """
    Given a full path, separate path from file
    
    Don't use this, use os.path routines instead
    """
    ...

def getDirectoryContents(dir:str, firstReturn:int=0, bufSize:int=2048, maxFiles:int=100) -> tuple[int, list[str], int]:
    """
    Get contents (files and subdirectories) of directory
    
    Don't use this, use python os.walk() or glob.glob() instead.
    """
    ...

def getVersions() -> tuple[int, int, int]:
    """
    Return tuple with (X-Plane, XPLM SDK, and hostID)
    
    Host ID is either XPlane=1 or Unknown=0
    """
    ...

def getLanguage() -> int:
    """
    Return language code the sim is running in.
    """
    ...

def debugString(string:str) -> None:
    """
    Write string to 'Log.txt' file, with immediate buffer flush
    
    Use xp.systemLog() instead, to add newline and prefix with your
    plugin's name instead. Use xp.log() to write to XPPython3Log.txt file
    """
    ...

def setErrorCallback(callback:Callable[[str], None]) -> None:
    """
    Install error-reporting callback for your plugin
    
    Likely not useful for python debugging.
    """
    ...

def findSymbol(symbol:str) -> int:
    """
    Find C-API symbol. See documentation.
    """
    ...

def loadDataFile(fileType:XPLMDataFileType, path:str) -> int:
    """
    Load data file given by path
    
    fileType is:
      DataFile_Situation   = 1
      DataFile_ReplayMovie = 2
    Path is either absolute or relative X-Plane root.
    Returns 1 on success (file found), 0 otherwise.
    """
    ...

def saveDataFile(fileType:XPLMDataFileType, path:str) -> int:
    """
    Saves data file to disk.
    
    fileType is:
      DataFile_Situation   = 1
      DataFile_ReplayMovie = 2
    Path is either absolute or relative X-Plane root.
    Returns 1 on success (file found), 0 otherwise.
    """
    ...

def findCommand(name:str) -> XPLMCommandRef:
    """
    Return commandRef for named command or None
    """
    ...

def commandBegin(commandRef:XPLMCommandRef) -> None:
    """
    Start execution of command specified by commandRef
    """
    ...

def commandEnd(commandRef:XPLMCommandRef) -> None:
    """
    Ends execution of command specified by commandRef
    """
    ...

def commandOnce(commandRef:XPLMCommandRef) -> None:
    """
    Executes given commandRef, doing both CommandBegin and CommandEnd
    """
    ...

def createCommand(name:str, description:Optional[str]=None) -> XPLMCommandRef:
    """
    Create a named command: You'll still need to registerCommandHandler()
    """
    ...

def registerCommandHandler(commandRef:XPLMCommandRef, callback:Callable[[XPLMCommandRef, XPLMCommandPhase, Any], int], before:int=1, refCon:Any=None) -> None:
    """
    Register a callback for given commandRef
    
    command callback is (commandRef, phase, refCon) and should return 0
       to halt processing, or 1 to let X-Plane continue with other callbacks.
       phase indicates current phase of command execution 0=Begin, 1=Continue, 2=End.
    before indicates you want to be called prior to X-Plane handling the command.
    """
    ...

def unregisterCommandHandler(commandRef:XPLMCommandRef, callback:Callable[[XPLMCommandRef, XPLMCommandPhase, Any], int], before:int=1, refCon:Any=None) -> None:
    """
    Unregister commandRef. Parameters must match those provided with registerCommandHandler()
    """
    ...

def getMETARForAirport(airport_id:str) -> str:
    """
    Returns 'last known' METAR report for given airport.
    
    Note the actual weather may have evolved significantly since
    the last downloaded METAR.
    """
    ...

def getWeatherAtLocation(latitude:float, longitude:float, altitude_m:float) -> None | XPLMWeatherInfo_t:
    """
    Returns current weather conditions at given location.
    Note this appears to work world-wide, though accuracy suffers with distance from current
    position.
    
    Returns WeatherInfo object on success, None otherwise. This call
    is not intended to be used per-frame.
    """
    ...

def createWidget(left:int, top:int, right:int, bottom:int, visible:int, descriptor:str, isRoot:int, 
          container:XPWidgetID | Literal[0], widgetClass:XPWidgetClass) -> XPWidgetID:
    """
    Create widget of class at location
    
    isRoot=1 if widget is a root widget, container is None or widgetID of parent widget
    widgetClass is one of predefined classes:
      WidgetClass_MainWindow
      WidgetClass_SubWindow
      WidgetClass_Button
      WidgetClass_TextField
      WidgetClass_ScrollBar
      WidgetClass_Caption
      WidgetClass_GeneralGraphics
      WidgetClass_Progress
    Returns created widgetID
    
    """
    ...

def createCustomWidget(left:int, top:int, right:int, bottom:int, visible:int, descriptor:str, isRoot:int, container:Optional[XPWidgetID], 
          callback:Callable[[XPWidgetMessage, XPWidgetID, int, int], int]) -> XPWidgetID:
    """
    Create widget at location, with custom callback
    
    callback is (message, widget, param1, param2) returning 1 if you've handled
       the message, 0 otherwise.
    Returns created widgetID
    """
    ...

def destroyWidget(widgetID:XPWidgetID, destroyChildren:int=1) -> None:
    """
    Destroys widgetID and (optionally) all children.
    """
    ...

def sendMessageToWidget(widgetID:XPWidgetID, message:XPWidgetMessage | int, dispatchMode:XPDispatchMode=Mode_UpChain, param1:int=0, param2:int=0) -> int:
    """
    dispatchMode default is Mode_UpChain
    """
    ...

def placeWidgetWithin(widgetID:XPWidgetID, container:XPWidgetID | Literal[0] = 0) -> None:
    """
    Change container widget for widgetID to container (widgetID)
    """
    ...

def countChildWidgets(widgetID:XPWidgetID) -> int:
    """
    Return number of child widgets for this widgetID
    """
    ...

def getNthChildWidget(widgetID:XPWidgetID, index:int) -> XPWidgetID:
    """
    Return widgetID of 0-based nth child
    """
    ...

def getParentWidget(widgetID:XPWidgetID) -> XPWidgetID:
    """
    Return widgetID for parent (i.e., container) of this widgetID
    """
    ...

def showWidget(widgetID:XPWidgetID) -> None:
    """
    Make widget visible.
    """
    ...

def hideWidget(widgetID:XPWidgetID) -> None:
    """
    Hide widget
    """
    ...

def isWidgetVisible(widgetID:XPWidgetID) -> int:
    """
    Return 1 if widget is visible
    
    Widget must be itself visible and contained in visible parent.
    Note if widget is outside of parent's geometry it may be clipped
    being reported 'visible' yet still not seen by user.
    """
    ...

def findRootWidget(widgetID:XPWidgetID) -> XPWidgetID:
    """
    Return top-most widget container for given widgetID
    
    If widget is root widget, it will return itself.
    """
    ...

def bringRootWidgetToFront(widgetID:XPWidgetID) -> None:
    """
    Make whole widget hierarchy containing widgetID to the front
    """
    ...

def isWidgetInFront(widgetID:XPWidgetID) -> int:
    """
    Return 1 if widget's hierarchy is front most.
    """
    ...

def getWidgetGeometry(widgetID:XPWidgetID) -> tuple[int, int, int, int]:
    """
    Return bounding box (left, top, right, bottom) of widgetID
    """
    ...

def setWidgetGeometry(widgetID:XPWidgetID, left:int, top:int, right:int, bottom:int) -> None:
    """
    Set bounding box for widgetID
    """
    ...

def getWidgetForLocation(container:XPWidgetID, xOffset:int, yOffset:int, recursive:int=1, visibleOnly:int=1) -> XPWidgetID:
    """
    Return widgetID of the child widget within the container widget at offset
    
    offsets are global coordinates, not relative bounding box of container.
    recursive=1 indicates find 'deepest' child widget
    visibleOnly=1 indicates only visible widgets are considered
    """
    ...

def getWidgetExposedGeometry(widgetID:XPWidgetID) -> tuple[int, int, int, int]:
    """
    Return (left, top, right, bottom) of widget's exposed geometry
    """
    ...

def setWidgetDescriptor(widgetID:XPWidgetID, descriptor:str) -> None:
    """
    Set widget's descriptor string
    """
    ...

def getWidgetDescriptor(widgetID:XPWidgetID) -> str:
    """
    Returns widget's descriptor string
    """
    ...

def getWidgetUnderlyingWindow(widgetID:XPWidgetID) -> XPLMWindowID:
    """
    Return windowID of window underlying widget
    """
    ...

def setWidgetProperty(widgetID:XPWidgetID, propertyID:XPWidgetPropertyID | int, value:Optional[Any]) -> None:
    """
    Set widget property to value
    """
    ...

def getWidgetProperty(widgetID:XPWidgetID, propertyID:XPWidgetPropertyID | int, exists:int=-1) -> Any:
    """
    Returns widget's property value. For XP properties, it is a long, for
    User-defined properties, it can be any python object.
    
    Raise ValueError if exists=-1 and property does not exist / has not been set
    Set exists=None if you don't care if property exists (value will be 0)
    Set exists to a list object, and we'll set it to [1,] if property exists, 
      [0, ] otherwise
    """
    ...

def setKeyboardFocus(widgetID:XPWidgetID) -> int:
    """
    Set keyboard focus to widgetID
    """
    ...

def loseKeyboardFocus(widgetID:XPWidgetID) -> None:
    """
    Cause widgetID to lose keyboard focus
    """
    ...

def getWidgetWithFocus() -> XPWidgetID:
    """
    Return widgetID with current focus. 0=X-Plane has focus.
    """
    ...

def addWidgetCallback(widgetID:XPWidgetID, callback:Callable[[XPWidgetMessage, XPWidgetID, int, int], int]) -> None:
    """
    Add callback to widgetID
    
    Callback has signature (message, widgetID, param1, param2). See
    createCustomWidget()
    """
    ...

def getWidgetClassFunc(widgetID:XPWidgetID) -> int:
    """
    Given widgetClass, return underlying function.
    
    Not useful with python. Use addWidgetCalback() instead.
    """
    ...

def createWidgets(widgetDefs:Sequence[Sequence[Any]], parentID:Optional[XPWidgetID]) -> None | list[XPWidgetID]:
    """
    This does not work in X-Plane.
    """
    ...

def moveWidgetBy(widgetID:XPWidgetID, dx:int=0, dy:int=0) -> None:
    """
    Move widget by amount. +x = right, +y = up
    """
    ...

def fixedLayout(message:XPWidgetMessage, widgetID:XPWidgetID, param1:int, param2:int) -> int:
    """
    Use this as a widgetCallback to have child widgets maintain relative positions
    
    Seems to be completely useless with X-Plane 11.55+
    """
    ...

def selectIfNeeded(message:XPWidgetMessage, widgetID:XPWidgetID, param1:int, param2:int, eatClick:int=1) -> int:
    """
    Call within widget Callback to raise widget, if not already
    
    Seems completely useless with X-Plane 11.55+
    """
    ...

def defocusKeyboard(message:XPWidgetMessage, widgetID:XPWidgetID, param1:int, param2:int, eatClick:int=1) -> int:
    """
    Send keyboard focus back to X-Plane
    
    Seems completely useless in X-Plane 11.55
    """
    ...

def dragWidget(message:XPWidgetMessage, widgetID:XPWidgetID, param1:int, param2:int, left:int, top:int, right:int, bottom:int) -> int:
    """
    Add to your widget callback to support drag areas
    
    (left, top, right, bottom) define area within your widget where, if drag
    is initiated, this callback will cause the widget to move.
    """
    ...

def reloadPlugin(signature:str) -> None:
    """
    Reload (python) plugin with provided signature
    
    Plugin will be disabled, stopped, reloaded, then
    started and enabled.
    """
    ...

def getPluginStats() -> dict:
    """
    Return dictionary of python plugin performance statistics
    
    Keys are different python plugins, with key=None being
    overall performance of XPPython3 plugin.
    """
    ...

def pythonGetDicts() -> dict:
    """
    Return dictionary of internal xppython3 dictionaries
    
    See documentation, intended for debugging only
    """
    ...

def log(s:Optional[str]=None) -> None:
    """
    Log string to XPPython3log.txt file. Flush buffer, if no string is provided.
    """
    ...

def systemLog(s:Optional[str]=None) -> None:
    """
    Log string to system log file, Log.txt, with newline appended, flushing buffer.
    """
    ...

def pythonGetCapsules() -> dict:
    """
    Returns internal dictionary of Capsules
    
    Intended for debugging only
    """
    ...

def derefCapsule(capsule_type:str, capsule:Any) -> int:
    """
    Dereference a capsule to retrieve internal C language pointer
    
    Intended for debugging only
    """
    ...

