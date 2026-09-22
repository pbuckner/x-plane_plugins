# pylint: disable = unused-argument, line-too-long, useless-import-alias, redefined-builtin, redefined-outer-name
# (a stub's parameters are never used -- there is no body; signatures are
#  emitted on one line; 'X as X' is the stub re-export form mypy requires.
#  Some X-Plane parameters are named object/map/type/dir, and drawCalls
#  takes a drawCalls argument -- renaming them would break keyword callers,
#  and a stub has no body for the shadowing to affect.)
# Type stub file generated from XP*.pyi files
# This file contains type hints for all functions, classes, and constants

from typing import Any, Callable, Literal, Optional, Sequence, Union
from XPPython3.xp_typing import (  # noqa: F401 -- re-exports  # pylint: disable=unused-import
    FMOD_CHANNEL as FMOD_CHANNEL,
    FMOD_CHANNELGROUP as FMOD_CHANNELGROUP,
    FMOD_STUDIO_SYSTEM as FMOD_STUDIO_SYSTEM,
    FMSEntryInfo as FMSEntryInfo,
    HotKeyInfo as HotKeyInfo,
    NavAidInfo as NavAidInfo,
    PluginInfo as PluginInfo,
    TrackMetrics as TrackMetrics,
    XPDispatchMode as XPDispatchMode,
    XPElementStyle as XPElementStyle,
    XPLMAudioBus as XPLMAudioBus,
    XPLMAvionicsID as XPLMAvionicsID,
    XPLMBankID as XPLMBankID,
    XPLMCameraControlDuration as XPLMCameraControlDuration,
    XPLMCommandPhase as XPLMCommandPhase,
    XPLMCommandRef as XPLMCommandRef,
    XPLMCoordinateSpace_t as XPLMCoordinateSpace_t,
    XPLMCursorStatus as XPLMCursorStatus,
    XPLMDataFileType as XPLMDataFileType,
    XPLMDataRef as XPLMDataRef,
    XPLMDataRefInfo_t as XPLMDataRefInfo_t,
    XPLMDataTypeID as XPLMDataTypeID,
    XPLMDeviceID as XPLMDeviceID,
    XPLMDrawInfoDouble_t as XPLMDrawInfoDouble_t,
    XPLMDrawInfo_t as XPLMDrawInfo_t,
    XPLMDrawingPhase as XPLMDrawingPhase,
    XPLMEGPWSStyle as XPLMEGPWSStyle,
    XPLMFlightLoopID as XPLMFlightLoopID,
    XPLMFlightLoopPhaseType as XPLMFlightLoopPhaseType,
    XPLMFontHandle as XPLMFontHandle,
    XPLMFontID as XPLMFontID,
    XPLMHostApplicationID as XPLMHostApplicationID,
    XPLMHotKeyID as XPLMHotKeyID,
    XPLMInstanceRef as XPLMInstanceRef,
    XPLMKeyFlags as XPLMKeyFlags,
    XPLMLanguageCode as XPLMLanguageCode,
    XPLMMapDisplayRef as XPLMMapDisplayRef,
    XPLMMapLayerID as XPLMMapLayerID,
    XPLMMapLayerType as XPLMMapLayerType,
    XPLMMapOrientation as XPLMMapOrientation,
    XPLMMapProjectionID as XPLMMapProjectionID,
    XPLMMapStyle as XPLMMapStyle,
    XPLMMenuCheck as XPLMMenuCheck,
    XPLMMenuID as XPLMMenuID,
    XPLMMouseStatus as XPLMMouseStatus,
    XPLMNavFlightPlan as XPLMNavFlightPlan,
    XPLMNavRef as XPLMNavRef,
    XPLMNavType as XPLMNavType,
    XPLMObjectRef as XPLMObjectRef,
    XPLMPluginID as XPLMPluginID,
    XPLMProbeInfo_t as XPLMProbeInfo_t,
    XPLMProbeRef as XPLMProbeRef,
    XPLMProbeResult as XPLMProbeResult,
    XPLMProbeType as XPLMProbeType,
    XPLMRetainedDrawing as XPLMRetainedDrawing,
    XPLMSVTDisplayRef as XPLMSVTDisplayRef,
    XPLMTexture as XPLMTexture,
    XPLMTextureAtlasRef as XPLMTextureAtlasRef,
    XPLMTextureID as XPLMTextureID,
    XPLMWeatherInfoClouds_t as XPLMWeatherInfoClouds_t,
    XPLMWeatherInfoWinds_t as XPLMWeatherInfoWinds_t,
    XPLMWeatherInfo_t as XPLMWeatherInfo_t,
    XPLMWindowContentType as XPLMWindowContentType,
    XPLMWindowDecoration as XPLMWindowDecoration,
    XPLMWindowID as XPLMWindowID,
    XPLMWindowLayer as XPLMWindowLayer,
    XPLMWindowPositioningMode as XPLMWindowPositioningMode,
    XPTrackStyle as XPTrackStyle,
    XPWidgetClass as XPWidgetClass,
    XPWidgetID as XPWidgetID,
    XPWidgetMessage as XPWidgetMessage,
    XPWidgetPropertyID as XPWidgetPropertyID,
    XPWindowStyle as XPWindowStyle
)
from contextlib import AbstractContextManager

# Constants
AircraftCarrier: int
AudioExteriorAircraft: XPLMAudioBus
AudioExteriorEnvironment: XPLMAudioBus
AudioExteriorUnprocessed: XPLMAudioBus
AudioGround: XPLMAudioBus
AudioInterior: XPLMAudioBus
AudioRadioCom1: XPLMAudioBus
AudioRadioCom2: XPLMAudioBus
AudioRadioCopilot: XPLMAudioBus
AudioRadioPilot: XPLMAudioBus
AudioUI: XPLMAudioBus = XPLMAudioBus(8)
Building: int
ButtonBehaviorCheckBox: int
ButtonBehaviorPushButton: int
ButtonBehaviorRadioButton: int
CapsLockFlag: XPLMKeyFlags
CharSetASCII: int
CharSetDigits: int
CharSetUnicode: int
CommandBegin: XPLMCommandPhase
CommandContinue: XPLMCommandPhase
CommandEnd: XPLMCommandPhase
ControlCameraForever: XPLMCameraControlDuration = XPLMCameraControlDuration(2)
ControlCameraUntilViewChanges: XPLMCameraControlDuration = XPLMCameraControlDuration(1)
ControlFlag: XPLMKeyFlags
CoolingTower: int
CoordSpace_AircraftExterior: XPLMCoordinateSpace_t
CoordSpace_AircraftInterior: XPLMCoordinateSpace_t
CoordSpace_Camera: XPLMCoordinateSpace_t
CoordSpace_World: XPLMCoordinateSpace_t = XPLMCoordinateSpace_t(0)
CursorArrow: XPLMCursorStatus
CursorButton: XPLMCursorStatus
CursorCustom: XPLMCursorStatus
CursorDefault: XPLMCursorStatus
CursorDown: XPLMCursorStatus
CursorFourArrows: XPLMCursorStatus
CursorHandle: XPLMCursorStatus
CursorHidden: XPLMCursorStatus
CursorLeft: XPLMCursorStatus
CursorLeftRight: XPLMCursorStatus
CursorRight: XPLMCursorStatus
CursorRotateLarge: XPLMCursorStatus
CursorRotateLargeLeft: XPLMCursorStatus
CursorRotateLargeRight: XPLMCursorStatus
CursorRotateMedium: XPLMCursorStatus
CursorRotateMediumLeft: XPLMCursorStatus
CursorRotateMediumRight: XPLMCursorStatus
CursorRotateSmall: XPLMCursorStatus
CursorRotateSmallLeft: XPLMCursorStatus
CursorRotateSmallRight: XPLMCursorStatus
CursorSplitterH: XPLMCursorStatus
CursorSplitterV: XPLMCursorStatus
CursorText: XPLMCursorStatus
CursorUp: XPLMCursorStatus
CursorUpDown: XPLMCursorStatus
CustomObject: int
DataFile_ReplayMovie: XPLMDataFileType
DataFile_Situation: XPLMDataFileType
DefaultWxrLimitMslFt: int
DefaultWxrRadiusNm: int
Device_CDU739_1: XPLMDeviceID
Device_CDU739_2: XPLMDeviceID
Device_CDU815_1: XPLMDeviceID
Device_CDU815_2: XPLMDeviceID
Device_G1000_MFD: XPLMDeviceID
Device_G1000_PFD_1: XPLMDeviceID
Device_G1000_PFD_2: XPLMDeviceID
Device_GNS430_1: XPLMDeviceID
Device_GNS430_2: XPLMDeviceID
Device_GNS530_1: XPLMDeviceID
Device_GNS530_2: XPLMDeviceID
Device_MCDU_1: XPLMDeviceID
Device_MCDU_2: XPLMDeviceID
Device_MCDU_3: XPLMDeviceID
Device_Primus_MFD_1: XPLMDeviceID
Device_Primus_MFD_2: XPLMDeviceID
Device_Primus_MFD_3: XPLMDeviceID
Device_Primus_PFD_1: XPLMDeviceID
Device_Primus_PFD_2: XPLMDeviceID
Device_Primus_RMU_1: XPLMDeviceID
Device_Primus_RMU_2: XPLMDeviceID
DownFlag: XPLMKeyFlags
EGPWS_Style_Blocky: XPLMEGPWSStyle
EGPWS_Style_Smooth: XPLMEGPWSStyle
Element_AircraftCarrier: XPElementStyle
Element_Airport: XPElementStyle
Element_Building: XPElementStyle
Element_CheckBox: XPElementStyle
Element_CheckBoxLit: XPElementStyle
Element_CoolingTower: XPElementStyle
Element_CopyButtons: XPElementStyle
Element_CopyButtonsWithEditingGrid: XPElementStyle
Element_CustomObject: XPElementStyle
Element_EditingGrid: XPElementStyle
Element_Fire: XPElementStyle
Element_ILSGlideScope: XPElementStyle
Element_LittleDownArrow: XPElementStyle
Element_LittleUpArrow: XPElementStyle
Element_MarkerLeft: XPElementStyle
Element_MarkerRight: XPElementStyle
Element_NDB: XPElementStyle
Element_OilPlatform: XPElementStyle
Element_OilPlatformSmall: XPElementStyle
Element_PowerLine: XPElementStyle
Element_PushButton: XPElementStyle
Element_PushButtonLit: XPElementStyle
Element_RadioTower: XPElementStyle
Element_ScrollBar: XPElementStyle
Element_Ship: XPElementStyle
Element_SmokeStack: XPElementStyle
Element_TextField: XPElementStyle
Element_TextFieldMiddle: XPElementStyle
Element_VOR: XPElementStyle
Element_VORWithCompassRose: XPElementStyle
Element_Waypoint: XPElementStyle
Element_WindowCloseBox: XPElementStyle
Element_WindowCloseBoxPressed: XPElementStyle
Element_WindowDragBar: XPElementStyle
Element_WindowDragBarSmooth: XPElementStyle
Element_Zoomer: XPElementStyle
FMOD_OK: int
FMOD_SOUND_FORMAT_PCM16: int
Fire: int
FlightLoop_Phase_AfterFlightModel: XPLMFlightLoopPhaseType
FlightLoop_Phase_BeforeFlightModel: XPLMFlightLoopPhaseType = XPLMFlightLoopPhaseType(0)
Font_Basic: XPLMFontID
Font_Proportional: XPLMFontID = XPLMFontID(18)
Fpl_CoPilot_Approach: XPLMNavFlightPlan
Fpl_CoPilot_Primary: XPLMNavFlightPlan
Fpl_CoPilot_Temporary: XPLMNavFlightPlan
Fpl_Pilot_Approach: XPLMNavFlightPlan
Fpl_Pilot_Primary: XPLMNavFlightPlan
Fpl_Pilot_Temporary: XPLMNavFlightPlan
Host_Unknown: XPLMHostApplicationID
Host_XPlane: XPLMHostApplicationID
ILSGlideScope: int
INTERNALPLUGINSPATH: str
Init_Invalid: int
Init_MissingAircraft: int
Init_MissingAirport: int
Init_MissingLivery: int
Init_MissingRamp: int
Init_MissingRunway: int
Init_Success: int
JustCenter: int
JustLeft: int
JustRight: int
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
KEY_DELETE: int
KEY_DOWN: int
KEY_ESCAPE: int
KEY_LEFT: int
KEY_RETURN: int
KEY_RIGHT: int
KEY_TAB: int
KEY_UP: int
Language_Chinese: XPLMLanguageCode
Language_English: XPLMLanguageCode
Language_French: XPLMLanguageCode
Language_German: XPLMLanguageCode
Language_Greek: XPLMLanguageCode
Language_Italian: XPLMLanguageCode
Language_Japanese: XPLMLanguageCode
Language_Korean: XPLMLanguageCode
Language_Russian: XPLMLanguageCode
Language_Spanish: XPLMLanguageCode
Language_Ukrainian: XPLMLanguageCode
Language_Unknown: XPLMLanguageCode
LineCapButt: int = 0
LineCapRound: int
LineCapSquare: int
LittleDownArrow: int
LittleUpArrow: int
MAP_IOS: str
MAP_USER_INTERFACE: str = 'XPLM_MAP_USER_INTERFACE'
MSG_AIRPLANE_COUNT_CHANGED: int
MSG_AIRPORT_LOADED: int
MSG_DATAREFS_ADDED: int
MSG_ENTERED_VR: int
MSG_EXITING_VR: int
MSG_FMOD_BANK_LOADED: int
MSG_FMOD_BANK_UNLOADING: int
MSG_LIVERY_LOADED: int
MSG_PLANE_CRASHED: int
MSG_PLANE_LOADED: int
MSG_PLANE_UNLOADED: int
MSG_RELEASE_PLANES: int
MSG_SCENERY_LOADED: int
MSG_WEATHER_DELIVERED: int
MSG_WILL_WRITE_PREFS: int
MainWindowStyle_MainWindow: int
MainWindowStyle_Translucent: int
MapLayer_Fill: XPLMMapLayerType
MapLayer_Markings: XPLMMapLayerType = XPLMMapLayerType(1)
MapOrientation_Map: XPLMMapOrientation
MapOrientation_UI: XPLMMapOrientation
MapStyle_IFR_HighEnroute: XPLMMapStyle
MapStyle_IFR_LowEnroute: XPLMMapStyle
MapStyle_VFR_Sectional: XPLMMapStyle
Map_EGPWS: int
Map_IR: int
Map_Nexrad: int
Map_Terrain: int
Map_Topo: int
Map_Water: int
Map_raw_elev: int
Map_safe_taxi: int
MarkerLeft: int
MarkerRight: int
Master: XPLMAudioBus
MasterBank: XPLMBankID
Menu_Checked: XPLMMenuCheck = XPLMMenuCheck(2)
Menu_NoCheck: XPLMMenuCheck
Menu_Unchecked: XPLMMenuCheck
Message_CloseButtonPushed: int
Mode_Direct: XPDispatchMode
Mode_DirectAllCallbacks: XPDispatchMode
Mode_Once: XPDispatchMode
Mode_Recursive: XPDispatchMode
Mode_UpChain: XPDispatchMode = XPDispatchMode(1)
ModuleMTimes: dict
MouseDown: XPLMMouseStatus
MouseDrag: XPLMMouseStatus
MouseUp: XPLMMouseStatus
MsgAirplaneCountChanged: int
MsgAirportLoaded: int
MsgDatarefsAdded: int
MsgDatarefs_Added: int
MsgEnteredVr: int
MsgExitingVr: int
MsgFmodBankLoaded: int
MsgFmodBankUnloading: int
MsgLiveryLoaded: int
MsgPlaneCrashed: int
MsgPlaneLoaded: int
MsgPlaneUnloaded: int
MsgReleasePlanes: int
MsgSceneryLoaded: int
MsgWeatherDelivered: int
MsgWillWritePrefs: int
Msg_AcceptChild: XPWidgetMessage
Msg_AcceptParent: XPWidgetMessage
Msg_ButtonStateChanged: int
Msg_Create: XPWidgetMessage
Msg_CursorAdjust: XPWidgetMessage
Msg_DescriptorChanged: XPWidgetMessage
Msg_Destroy: XPWidgetMessage
Msg_Draw: XPWidgetMessage
Msg_ExposedChanged: XPWidgetMessage
Msg_Hidden: XPWidgetMessage
Msg_KeyLoseFocus: XPWidgetMessage
Msg_KeyPress: XPWidgetMessage
Msg_KeyTakeFocus: XPWidgetMessage
Msg_LoseChild: XPWidgetMessage
Msg_MouseDown: XPWidgetMessage
Msg_MouseDrag: XPWidgetMessage
Msg_MouseUp: XPWidgetMessage
Msg_MouseWheel: XPWidgetMessage
Msg_None: XPWidgetMessage
Msg_Paint: XPWidgetMessage
Msg_PropertyChanged: XPWidgetMessage
Msg_PushButtonPressed: int
Msg_Reshape: XPWidgetMessage
Msg_ScrollBarSliderPositionChanged: int
Msg_Shown: XPWidgetMessage
Msg_TextFieldChanged: int
Msg_UserStart: XPWidgetMessage
NAV_NOT_FOUND: int
NDB: int
NO_PARENT: int
NO_PLUGIN_ID: int
Nav_Airport: XPLMNavType
Nav_Any: XPLMNavType = XPLMNavType(4294967295)
Nav_DME: XPLMNavType
Nav_Fix: XPLMNavType
Nav_GlideSlope: XPLMNavType
Nav_ILS: XPLMNavType
Nav_InnerMarker: XPLMNavType
Nav_LatLon: XPLMNavType
Nav_Localizer: XPLMNavType
Nav_MiddleMarker: XPLMNavType
Nav_NDB: XPLMNavType
Nav_OuterMarker: XPLMNavType
Nav_TACAN: XPLMNavType
Nav_Unknown: XPLMNavType
Nav_VOR: XPLMNavType
NoFlag: XPLMKeyFlags = XPLMKeyFlags(0)
NumCloudLayers: int
NumTemperatureLayers: int
NumWindLayers: int
OilPlatform: int
OilPlatformSmall: int
OptionAltFlag: XPLMKeyFlags
PARAM_PARENT: int
PLUGINSPATH: str
PLUGIN_XPLANE: int
Phase_FirstCockpit: XPLMDrawingPhase
Phase_Gauges: XPLMDrawingPhase
Phase_LastCockpit: XPLMDrawingPhase
Phase_LocalMap2D: XPLMDrawingPhase
Phase_LocalMap3D: XPLMDrawingPhase
Phase_LocalMapProfile: XPLMDrawingPhase
Phase_Modern3D: XPLMDrawingPhase
Phase_Panel: XPLMDrawingPhase
Phase_Window: XPLMDrawingPhase = XPLMDrawingPhase(50)
PowerLine: int
ProbeError: XPLMProbeResult
ProbeHitTerrain: XPLMProbeResult
ProbeMissed: XPLMProbeResult
ProbeY: XPLMProbeType = XPLMProbeType(0)
Property_ActiveEditSide: int
Property_ButtonBehavior: int
Property_ButtonState: int
Property_ButtonType: int
Property_CaptionLit: int
Property_Clip: XPWidgetPropertyID
Property_DragXOff: XPWidgetPropertyID
Property_DragYOff: XPWidgetPropertyID
Property_Dragging: XPWidgetPropertyID
Property_EditFieldSelDragStart: int
Property_EditFieldSelEnd: int
Property_EditFieldSelStart: int
Property_Enabled: XPWidgetPropertyID
Property_Font: int
Property_GeneralGraphicsType: int
Property_Hilited: XPWidgetPropertyID
Property_MainWindowHasCloseBoxes: int
Property_MainWindowType: int
Property_MaxCharacters: int
Property_Object: XPWidgetPropertyID
Property_PasswordMode: int
Property_ProgressMax: int
Property_ProgressMin: int
Property_ProgressPosition: int
Property_Refcon: XPWidgetPropertyID
Property_ScrollBarMax: int
Property_ScrollBarMin: int
Property_ScrollBarPageAmount: int
Property_ScrollBarSliderPosition: int
Property_ScrollBarSlop: int
Property_ScrollBarType: int
Property_ScrollPosition: int
Property_SubWindowType: int
Property_TextFieldType: int
Property_UserStart: XPWidgetPropertyID
PushButton: int
RadioBank: XPLMBankID
RadioButton: int
RadioTower: int
SVT_AirportSigns: int
SVT_All: int
SVT_FlightPath: int
SVT_HorizonHeading: int
SVT_ILSHoops: int
SVT_Obstacles: int
SVT_Runways: int
SVT_Terrain: int
SVT_Traffic: int
ScrollBarTypeScrollBar: int
ScrollBarTypeSlider: int
ShiftFlag: XPLMKeyFlags
Ship: int
SmokeStack: int
SubWindowStyle_ListView: int
SubWindowStyle_Screen: int
SubWindowStyle_SubWindow: int
TempUndefinedLayer: int
Tex_GeneralInterface: XPLMTextureID
Tex_Radar_Copilot: XPLMTextureID
Tex_Radar_Pilot: XPLMTextureID
TextEntryField: int
TextTranslucent: int
TextTransparent: int
Texture_WeatherRadar1: int
Texture_WeatherRadar2: int
TouchZone_Command: int
TouchZone_Identifier: int
TouchZone_Nothing: int
Track_Progress: XPTrackStyle
Track_ScrollBar: XPTrackStyle
Track_Slider: XPTrackStyle
Type_Data: XPLMDataTypeID
Type_Double: XPLMDataTypeID
Type_Float: XPLMDataTypeID
Type_FloatArray: XPLMDataTypeID
Type_Int: XPLMDataTypeID
Type_IntArray: XPLMDataTypeID
Type_Unknown: XPLMDataTypeID = XPLMDataTypeID(0)
USER_AIRCRAFT: int
UpFlag: XPLMKeyFlags
VERSION: str
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
VK_ADD: int
VK_B: int
VK_BACK: int
VK_BACKQUOTE: int
VK_BACKSLASH: int
VK_C: int
VK_CLEAR: int
VK_COMMA: int
VK_D: int
VK_DECIMAL: int
VK_DELETE: int
VK_DIVIDE: int
VK_DOWN: int
VK_E: int
VK_END: int
VK_ENTER: int
VK_EQUAL: int
VK_ESCAPE: int
VK_EXECUTE: int
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
VK_F1: int
VK_F20: int
VK_F21: int
VK_F22: int
VK_F23: int
VK_F24: int
VK_F2: int
VK_F3: int
VK_F4: int
VK_F5: int
VK_F6: int
VK_F7: int
VK_F8: int
VK_F9: int
VK_F: int
VK_G: int
VK_H: int
VK_HELP: int
VK_HOME: int
VK_I: int
VK_INSERT: int
VK_J: int
VK_K: int
VK_L: int
VK_LBRACE: int
VK_LEFT: int
VK_M: int
VK_MINUS: int
VK_MULTIPLY: int
VK_N: int
VK_NEXT: int
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
VK_NUMPAD_ENT: int
VK_NUMPAD_EQ: int
VK_O: int
VK_P: int
VK_PERIOD: int
VK_PRINT: int
VK_PRIOR: int
VK_Q: int
VK_QUOTE: int
VK_R: int
VK_RBRACE: int
VK_RETURN: int
VK_RIGHT: int
VK_S: int
VK_SELECT: int
VK_SEMICOLON: int
VK_SEPARATOR: int
VK_SLASH: int
VK_SNAPSHOT: int
VK_SPACE: int
VK_SUBTRACT: int
VK_T: int
VK_TAB: int
VK_U: int
VK_UP: int
VK_V: int
VK_W: int
VK_X: int
VK_Y: int
VK_Z: int
VOR: int
VORWithCompassRose: int
WayPoint: int
WidgetClass_Button: XPWidgetClass
WidgetClass_Caption: XPWidgetClass
WidgetClass_GeneralGraphics: XPWidgetClass
WidgetClass_MainWindow: XPWidgetClass
WidgetClass_None: int
WidgetClass_Progress: XPWidgetClass
WidgetClass_ScrollBar: XPWidgetClass
WidgetClass_SubWindow: XPWidgetClass
WidgetClass_TextField: XPWidgetClass
WindUndefinedLayer: int
WindowCenterOnMonitor: XPLMWindowPositioningMode
WindowCloseBox: int
WindowContentTypeBrowser: XPLMWindowContentType
WindowContentTypeOpenGL: XPLMWindowContentType = XPLMWindowContentType(0)
WindowContentTypePanelGraphics: XPLMWindowContentType
WindowDecorationNone: XPLMWindowDecoration
WindowDecorationRoundRectangle: XPLMWindowDecoration = XPLMWindowDecoration(1)
WindowDecorationSelfDecorated: XPLMWindowDecoration
WindowDecorationSelfDecoratedResizable: XPLMWindowDecoration
WindowFullScreenOnAllMonitors: XPLMWindowPositioningMode
WindowFullScreenOnMonitor: XPLMWindowPositioningMode
WindowLayerFlightOverlay: XPLMWindowLayer
WindowLayerFloatingWindows: XPLMWindowLayer = XPLMWindowLayer(1)
WindowLayerGrowlNotifications: XPLMWindowLayer
WindowLayerModal: XPLMWindowLayer
WindowPopOut: XPLMWindowPositioningMode
WindowPositionFree: XPLMWindowPositioningMode
WindowVR: XPLMWindowPositioningMode
Window_Help: XPWindowStyle
Window_ListView: XPWindowStyle
Window_MainWindow: XPWindowStyle = XPWindowStyle(1)
Window_Screen: XPWindowStyle
Window_SubWindow: XPWindowStyle
_Airport: int
kVersion: int
kXPLM_Version: int
pythonDebugLevel: int
pythonExecutable: str

# Classes


class FlightInitError(Exception):
    pass
# Functions


def loadCursor(name: str) -> int:
    """
        Load a cursor from a .png file and return cursor reference.

        Multiple calls with same filename will return same cursor.

        Cursor file must be in RGBA format (with alpha channel).

        Recommended cursor image size is 17x17 or 32x32 pixels to support both
        1x and 2x resolution cursors.
    """
    ...


def setCursor(cursor_id: int) -> None:
    """
        Set the cursor to the given cursor reference.
    """
    ...


def unloadCursor(cursor_id: int) -> None:
    """
        Unload cursor when no longer needed.
    """
    ...


def controlCamera(howLong: XPLMCameraControlDuration = ControlCameraUntilViewChanges, controlFunc: Optional[Callable[[Optional[list[float]], int, Any], int]] = None, refCon: Any = None) -> None:
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


def findDataRef(name: str) -> XPLMDataRef:
    """
        Looks up string name of data ref and returns dataRef code
        to be used with get and set data ref functions,
        or None, if name cannot be found.
    """
    ...


def canWriteDataRef(dataRef: XPLMDataRef) -> bool:
    """
        Returns True if dataRef is writable, False otherwise. Also
        returns False if provided dataRef is None.
    """
    ...


def isDataRefGood(dataRef: XPLMDataRef) -> bool:
    """
        (Deprecated, do not use.)
    """
    ...


def getDataRefTypes(dataRef: XPLMDataRef) -> int:
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


def getDatai(dataRef: XPLMDataRef) -> int:
    """
        Returns integer value for dataRef.
    """
    ...


def setDatai(dataRef: XPLMDataRef, value: int = 0) -> None:
    """
        Sets integer value for dataRef.
    """
    ...


def getDataf(dataRef: XPLMDataRef) -> float:
    """
        Returns float value for dataRef.
    """
    ...


def setDataf(dataRef: XPLMDataRef, value: float = 0.0) -> None:
    """
        Sets float value for dataRef.
    """
    ...


def getDatad(dataRef: XPLMDataRef) -> float:
    """
        Returns double value for dataRef (as a python float)
    """
    ...


def setDatad(dataRef: XPLMDataRef, value: float = 0.0) -> None:
    """
        Sets double value for dataRef.
    """
    ...


def getDatavi(dataRef: XPLMDataRef, values: Optional[list[int]] = None, offset: int = 0, count: int = -1) -> int:
    """
        Get integer array value for dataRef.

        If values is None, return number of elements in the array (only).
        Otherwise, values should be a list into which will be copied elements
        from the dataRef, starting at offset, and continuing for count # of elements.
        If count is negative, or unspecified, all elements (relative offset) are copied.

        Returns the number of elements copied.
    """
    ...


def setDatavi(dataRef: XPLMDataRef, values: list[int], offset: int = 0, count: int = -1) -> None:
    """
        Set integer array value for dataRef.

        values is a list of integers, to be written into dataRef starting
        at offset. Up to count values are written.

        If count is negative (or not provided), all values in the list are copied.
        It is an error for count to be greater than the length of the list.

        No return value.
    """
    ...


def getDatavf(dataRef: XPLMDataRef, values: Optional[list[float]] = None, offset: int = 0, count: int = -1) -> int:
    """
        Get float array value for dataRef.

        If values is None, return number of elements in the array (only).
        Otherwise, values should be a list into which will be copied elements
        from the dataRef, starting at offset, and continuing for count # of elements.
        If count is negative, or unspecified, all elements (relative offset) are copied.

        Returns the number of elements copied.
    """
    ...


def setDatavf(dataRef: XPLMDataRef, values: list[float], offset: int = 0, count: int = -1) -> None:
    """
        Set float array value for dataRef.

        values is a list of floats, to be written into dataRef starting
        at offset. Up to count values are written.

        If count is negative (or not provided), all values in the list are copied.
        It is an error for count to be greater than the length of the list.

        No return value.
    """
    ...


def getDatab(dataRef: XPLMDataRef, values: Optional[list[int]] = None, offset: int = 0, count: int = -1) -> int:
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


def setDatab(dataRef: XPLMDataRef, values: list[int] | bytes, offset: int = 0, count: int = -1) -> None:
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


def getDatas(dataRef: XPLMDataRef, offset: int = 0, count: int = -1) -> str | None:
    """
        Returns string value for dataRef.

        String is the first null-terminated sequence found in the byte-array
        dataRef, starting at offset. If count is given, string returned is
        restricted to count length (not including a null byte).

        Note not all byte-array dataRefs are strings: be sure the requested
        dataRef is storing character information. Otherwise use getDatab().
    """
    ...


def setDatas(dataRef: XPLMDataRef, value: str, offset: int = 0, count: int = -1) -> None:
    """
        Set byte array to string value for dataRef.

        value is a python unicode string (capable of being encoded as 'UTF-8').
        String is written into the dataRef starting at offset. Up to count
        characters are written. If count is more than len(value), the written
        values are padded with zeros ('\x00') up to count.

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


def registerDataAccessor(name: str, dataType: XPLMDataTypeID = Type_Unknown, writable: int = -1, readInt: Optional[Callable[[Any], int]] = None, writeInt: Optional[Callable[[Any, int], None]] = None, readFloat: Optional[Callable[[Any], float]] = None, writeFloat: Optional[Callable[[Any, float], None]] = None, readDouble: Optional[Callable[[Any], float]] = None, writeDouble: Optional[Callable[[Any, float], None]] = None, readIntArray: Optional[Callable[[Any, list[int], int, int], int]] = None, writeIntArray: Optional[Callable[[Any, list[int], int, int], None]] = None, readFloatArray: Optional[Callable[[Any, list[float], int, int], int]] = None, writeFloatArray: Optional[Callable[[Any, list[float], int, int], None]] = None, readData: Optional[Callable[[Any, list[int], int, int], int]] = None, writeData: Optional[Callable[[Any, list[int], int, int], None]] = None, readRefCon: Any = None, writeRefCon: Any = None) -> XPLMDataRef:
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


def unregisterDataAccessor(accessor: XPLMDataRef) -> None:
    """
        Unregisters data accessor.
    """
    ...


def shareData(name: str, dataType: int, dataChanged: Optional[Callable[[Any], None]] = None, refCon: Any = None) -> int:
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


def getDataRefsByIndex(offset: int = 0, count: int = 1) -> None | list[XPLMDataRef]:
    """
        Returns list of dataRefs, each similar to return from xp.findDataRef().
        Use xp.getDataRefInfo() to access information about the dataref.
        As a special case, count=-1 returns all datarefs starting from offset to the end.

        CAUTION: requesting datarefs greater than countDataRefs() returns garbage. If you
        try to use these, you may crash the sim.
    """
    ...


def getDataRefInfo(dataRef: XPLMDataRef) -> XPLMDataRefInfo_t:
    """
        Return DataRefInfo object for provided dataRef.

        XPLMDataRefInfo_t object is .name, .type, .writable, .owner
          recall type is a bitfield, see xp.getDataRefTypes()
    """
    ...


def unshareData(name: str, dataType: int, dataChanged: Optional[Callable[[Any], None]] = None, refCon: Any = None) -> int:
    """
        Unshare data. If dataChanged function was provided with initial shareData()
        the callback will no longer be called on data changes.
        All parameter values must match those provided with shareData()
        in order to be successful.

        Returns 1 on success, 0 otherwise
    """
    ...


def getDataRefCallbackDict() -> dict:
    """
        Returns copy of internal DataRefInfo dictionary.

        Internal debugging aid: contents and structure may change between releases.
    """
    ...


def getSharedDataRefCallbackDict() -> dict:
    """
        Returns copy of internal shared DataRefs dictionary.

        Internal debugging aid: contents and structure may change between releases.
    """
    ...


def registerDrawCallback(draw: Callable[[XPLMDrawingPhase, int, Any], None | int], phase: XPLMDrawingPhase = Phase_Window, after: int = 0, refCon: Any = None) -> int:
    """
        Registers  low-level drawing callback.

        The after parameter indicates you want to be called before (0) or after (1) phase.
        draw() callback function takes three parameters (phase, after, refCon), returning
        0 to suppress further X-Plane drawing in the phase, or 1 to allow X-Plane to finish.
        (Callback's value is ignored if after = 1).

        Registration returns 1 on success, 0 otherwise.
    """
    ...


def registerAvionicsCallbacksEx(deviceId: XPLMDeviceID, before: Optional[Callable[[XPLMDeviceID, int, Any], int]] = None, after: Optional[Callable[[XPLMDeviceID, int, Any], int]] = None, refCon: Any = None, bezelClick: Optional[Callable[[int, int, XPLMMouseStatus, Any], int]] = None, bezelRightClick: Optional[Callable[[int, int, XPLMMouseStatus, Any], int]] = None, bezelScroll: Optional[Callable[[int, int, int, int, Any], int]] = None, bezelCursor: Optional[Callable[[int, int, Any], XPLMCursorStatus]] = None, screenTouch: Optional[Callable[[int, int, XPLMMouseStatus, Any], int]] = None, screenRightTouch: Optional[Callable[[int, int, XPLMMouseStatus, Any], int]] = None, screenScroll: Optional[Callable[[int, int, int, int, Any], int]] = None, screenCursor: Optional[Callable[[int, int, Any], XPLMCursorStatus]] = None, keyboard: Optional[Callable[[int, XPLMKeyFlags, int, Any, int], int]] = None) -> XPLMAvionicsID:
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


def unregisterAvionicsCallbacks(avionicsId: XPLMAvionicsID) -> None:
    """
        Unregisters avionics draw callback(s) associated with given avionicsId.

        Does not return a value.
    """
    ...


def getAvionicsHandle(deviceID: int) -> XPLMAvionicsID:
    """
        Registers no callbacks for a cockpit device, but returns a
        handle which allows you to interact using other Avionics Device
        API. Use this if you do not wish to intercept drawing, clicks, or
        touches but want to interact with its popup programmatically.

        Returns XPLMAvionicsID.
    """
    ...


def isAvionicsBound(avionicsID: XPLMAvionicsID) -> int:
    """
        Return 1 if cockpit device with given ID is used by the current aircraft.
    """
    ...


def isCursorOverAvionics(avionicsID: XPLMAvionicsID) -> tuple[int, int] | None:
    """
        Is the cursor over the device with given avionicsID

        Returns tuple (x, y) with position or None.
    """
    ...


def isAvionicsPopupVisible(avionicsID: XPLMAvionicsID) -> int:
    """
        Is the popup window for the device with given avionicsID visible?
        (It may or may not be popped out into an OS window.)

        Returns 1 if true.
    """
    ...


def isAvionicsPoppedOut(avionicsID: XPLMAvionicsID) -> int:
    """
        Returns 1 (true) if the popup window for the cockpit device is popped out
        into an OS window.
    """
    ...


def hasAvionicsKeyboardFocus(avionicsID: XPLMAvionicsID) -> int:
    """
        Returns 1 (true) if cockpit device has keyboard focus.
    """
    ...


def avionicsNeedsDrawing(avionicsID: XPLMAvionicsID) -> None:
    """
        Tells X-Plane that your device's screens needs to be re-drawn.
        If your device is marked for on-demand drawing, XP will call your screen
        drawing callback before drawing the next simulator frame. If your device
        is already drawn every frame, this has no effect.
    """
    ...


def popOutAvionics(avionicsID: XPLMAvionicsID) -> None:
    """
        Pops out OS window for cockpit device.
    """
    ...


def takeAvionicsKeyboardFocus(avionicsID: XPLMAvionicsID) -> None:
    """
        Sets keyboard focus to the (already) visible popup window of cockpit device.
        Does nothing if device is not visible.
    """
    ...


def destroyAvionics(avionicsID: XPLMAvionicsID) -> None:
    """
        Destroys the cockpit device and deallocates its framebuffer. You should
        only ever call this for devices that you created, not stock X-Plane devices
        you have customized.
    """
    ...


def getAvionicsBusVoltsRatio(avionicsID: XPLMAvionicsID) -> float:
    """
        Return ratio [0.0:1.0] of nominal voltage of electrical bus,
        for given avionics device. Returns -1 if device is not bound
        to the current aircraft.
    """
    ...


def getAvionicsBrightnessRheo(avionicsID: XPLMAvionicsID) -> float:
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


def setAvionicsBrightnessRheo(avionicsID: XPLMAvionicsID, brightness: float = 1.0) -> None:
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


def setAvionicsPopupVisible(avionicsID: XPLMAvionicsID, visible: int = 1) -> None:
    """
        Shows (visible = 1) or Hides popup window for cockpit device.
    """
    ...


def isAvionicsMappedToVR(avionicsID: XPLMAvionicsID) -> int:
    """
        Return 1 if the cockpit device with given ID is mapped into VR.
    """
    ...


def setAvionicsMappedToVR(avionicsID: XPLMAvionicsID, mapped: int = 1) -> None:
    """
        Map (mapped = 1) or unmap the cockpit device with given ID into VR.
    """
    ...


def registerKeySniffer(sniffer: Callable[[int, XPLMKeyFlags, int, Any], int], before: int = 0, refCon: Any = None) -> int:
    """
        Registers a key sniffer callback function.

        sniffer() callback takes four parameters (key, flags, vKey, refCon) and
        should return 0 to consume the key, 1 to pass it to next sniffer or X-Plane.

        before = 1 will intercept keys before windows (i.e., the user may be typing in
        input field), so generally, use before = 0 to sniff keys not already consumed.

        refCon will be passed to your sniffer callback.
    """
    ...


def unregisterDrawCallback(draw: Callable[[XPLMDrawingPhase, int, Any], None], phase: XPLMDrawingPhase = Phase_Window, after: int = 0, refCon: Any = None) -> int:
    """
        Unregisters low-level drawing callback.

        Parameters must match those provided  with registerDrawCallback().
        Returns 1 on success, 0 otherwise.
    """
    ...


def unregisterKeySniffer(sniffer: Callable[[int, XPLMKeyFlags, int, Any], int], before: int = 0, refCon: Any = None) -> int:
    """
        Unregisters key sniffer.

        Parameters must match those provided with registerKeySniffer().
        Returns 1 on success, 0 otherwise.
    """
    ...


def createAvionicsEx(screenWidth: int = 100, screenHeight: int = 200, bezelWidth: int = 140, bezelHeight: int = 250, screenOffsetX: int = 20, screenOffsetY: int = 25, drawOnDemand: int = 0, bezelDraw: Optional[Callable[[float, float, float, Any], None]] = None, screenDraw: Optional[Callable[[Any], None]] = None, bezelClick: Optional[Callable[[int, int, XPLMMouseStatus, Any], int]] = None, bezelRightClick: Optional[Callable[[int, int, XPLMMouseStatus, Any], int]] = None, bezelScroll: Optional[Callable[[int, int, int, int, Any], int]] = None, bezelCursor: Optional[Callable[[int, int, Any], XPLMCursorStatus]] = None, screenTouch: Optional[Callable[[int, int, XPLMMouseStatus, Any], int]] = None, screenRightTouch: Optional[Callable[[int, int, XPLMMouseStatus, Any], int]] = None, screenScroll: Optional[Callable[[int, int, int, int, Any], int]] = None, screenCursor: Optional[Callable[[int, int, Any], XPLMCursorStatus]] = None, keyboard: Optional[Callable[[int, XPLMKeyFlags, int, Any, int], int]] = None, brightness: Optional[Callable[[float, float, float, float], float]] = None, deviceID: str = 'deviceID', deviceName: str = 'deviceName', refCon: Any = None, contentType: XPLMWindowContentType = WindowContentTypeOpenGL, windowWithChrome: int = 0, browserLoadFinished: Optional[Callable[[XPLMAvionicsID, str, Any], None]] = None, browserLoadError: Optional[Callable[[XPLMAvionicsID, str, str, Any], None]] = None) -> XPLMAvionicsID:
    """
        Creates glass cockpit device for 3D cockpit.
        With 12.0 you needed to call this within your XPluginStart callback
        to ensure your texture would be ready. Since 12.1, you may call this
        at anytime and X-Plane will retroactively map your display to it.

        For contentType = WindowContentTypeBrowser (XPLM440), the device screen is a
        CEF web view driven with avionicsSetURL()/avionicsRefresh()/etc.;
        browserLoadFinished(avionicsID, url, refCon) fires when the main frame
        finishes loading (also for rendered HTTP error pages -- treat
        browserLoadError as authoritative and ignore url = 'about: blank'), and
        browserLoadError(avionicsID, url, error, refCon) fires on a network-level
        navigation failure.

        Returns new avionicsID.
    """
    ...


def createWindowEx(left: Union[int, Sequence[Any]] = 100, top: int = 200, right: int = 200, bottom: int = 100, visible: int = 0, draw: Optional[Callable[[XPLMWindowID, Any], None]] = None, click: Optional[Callable[[XPLMWindowID, int, int, XPLMMouseStatus, Any], int]] = None, key: Optional[Callable[[XPLMWindowID, int, int, int, Any, int], None]] = None, cursor: Optional[Callable[[XPLMWindowID, int, int, Any], XPLMCursorStatus]] = None, wheel: Optional[Callable[[XPLMWindowID, int, int, int, int, Any], int]] = None, refCon: Any = None, decoration: XPLMWindowDecoration = WindowDecorationRoundRectangle, layer: XPLMWindowLayer = WindowLayerFloatingWindows, rightClick: Optional[Callable[[XPLMWindowID, int, int, XPLMMouseStatus, Any], int]] = None, contentType: XPLMWindowContentType = WindowContentTypeOpenGL, browserLoadFinished: Optional[Callable[[XPLMWindowID, str, Any], None]] = None, browserLoadError: Optional[Callable[[XPLMWindowID, str, str, Any], None]] = None) -> XPLMWindowID:
    """
        Creates modern window

        Callback functions are optional:
          draw(windowID, refCon) with no return value.
          key(windowID, key, flags, vKey, refCon, losingFocus) with no return value.
          cursor(windowID, x, y, refCon) returns updated CursorStatus
          click(windowID, x, y, mouseStatus, refCon) with 1 = consume click, else 0.
          rightClick(windowID, x, y, mouseStatus, refCon) with 1 = consume click, else 0.
          wheel(windowID, x, y, wheel, clicks, refCon) with 1 = consume click, else 0.
          browserLoadFinished(windowID, url, refCon) with no return value (browser content).
          browserLoadError(windowID, url, error, refCon) with no return value (browser content).

        contentType selects OpenGL, Panel Graphics, or Browser content
        (requires X-Plane with XPLM440 or later for non-OpenGL types).

        Returns new windowID.
    """
    ...


def createWindow(left: int = 100, top: int = 200, right: int = 200, bottom: int = 100, visible: int = 0, draw: Optional[Callable[[XPLMWindowID, Any], None]] = None, key: Optional[Callable[[XPLMWindowID, int, int, int, Any, int], None]] = None, mouse: Optional[Callable[[XPLMWindowID, int, int, XPLMMouseStatus, Any], int]] = None, refCon: Any = None) -> XPLMWindowID:
    """
        (Deprecated do not use)
    """
    ...


def destroyWindow(windowID: XPLMWindowID) -> None:
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


def getAllMonitorBoundsGlobal(bounds: Callable[[int, int, int, int, int, Any], None], refCon: Any) -> None:
    """
        Immediately calls bounds() callback once for each monitor to retrieve
        bounds for each monitor running full-screen simulator.

        Callback function is
          bounds(index, left, top, right, bottom, refCon)
    """
    ...


def getAllMonitorBoundsOS(bounds: Callable[[int, int, int, int, int, Any], None], refCon: Any) -> None:
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


def getWindowGeometry(windowID: XPLMWindowID) -> None | tuple[int, int, int, int]:
    """
        Returns window geometry (left, top, right, bottom).
    """
    ...


def getAvionicsGeometry(avionicsID: XPLMAvionicsID) -> tuple[int, int, int, int]:
    """
        Returns geometry (left, top, right, bottom) of popup window
        in X-Plane coordinate system.
    """
    ...


def setWindowGeometry(windowID: XPLMWindowID, left: int, top: int, right: int, bottom: int) -> None:
    """
        Sets window geometry.
    """
    ...


def setAvionicsGeometry(avionicsID: XPLMAvionicsID, left: int, top: int, right: int, bottom: int) -> None:
    """
        Sets window geometry for cockpit device's popup window in X-Plane coordinates.
    """
    ...


def getWindowGeometryOS(windowID: XPLMWindowID) -> None | tuple[int, int, int, int]:
    """
        Returns window geometry for popped-out window (left, top, right, bottom).
    """
    ...


def getAvionicsGeometryOS(avionicsID: XPLMAvionicsID) -> tuple[int, int, int, int]:
    """
        Returns window geometry for popped-out avionics device (left, top, right, bottom).
    """
    ...


def setAvionicsGeometryOS(avionicsID: XPLMAvionicsID, left: int, top: int, right: int, bottom: int) -> None:
    """
        Sets window geometry for popped-out avionics.
    """
    ...


def setWindowGeometryOS(windowID: XPLMWindowID, left: int, top: int, right: int, bottom: int) -> None:
    """
        Sets window geometry for popped-out window.
    """
    ...


def getWindowGeometryVR(windowID: XPLMWindowID) -> None | tuple[int, int]:
    """
        Gets window geometry for VR window (width, height)
    """
    ...


def setWindowGeometryVR(windowID: XPLMWindowID, width: int, height: int) -> None:
    """
        Sets window geometry for VR window.
    """
    ...


def getWindowIsVisible(windowID: XPLMWindowID) -> int:
    """
        Returns 1 if window is visible, 0 otherwise.
    """
    ...


def setWindowIsVisible(windowID: XPLMWindowID, visible: int = 1) -> None:
    """
        Sets window visibility. 1 indicates visible, 0 is not-visible.
    """
    ...


def windowIsPoppedOut(windowID: XPLMWindowID) -> int:
    """
        Returns 1 if window is popped-out, 0 otherwise.
    """
    ...


def windowIsInVR(windowID: XPLMWindowID) -> int:
    """
        Returns 1 if window is in VR, 0 otherwise.
    """
    ...


def setWindowGravity(windowID: XPLMWindowID, left: int, top: int, right: int, bottom: int) -> None:
    """
        Set window's gravity (resize window, relative screen size changes.)

        values typically range 0.0 - 1.0, and reflect how much the window's
        value will change relative to change in screen size.
        0.0 means zero change relative to the right side (for left/right edges)
            and zero change relative to the bottom (for (for top/bottom edges)
        1.0 means 100% change relative to the right side / bottom
    """
    ...


def setWindowResizingLimits(windowID: XPLMWindowID, minWidth: int = 0, minHeight: int = 0, maxWidth: int = 10000, maxHeight: int = 10000) -> None:
    """
        Set maximum and minimum window size.
    """
    ...


def setWindowPositioningMode(windowID: XPLMWindowID, mode: XPLMWindowPositioningMode, index: int = -1) -> None:
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


def setWindowTitle(windowID: XPLMWindowID, title: str) -> None:
    """
        Set window title.
    """
    ...


def windowSetURL(windowID: XPLMWindowID, url: str) -> None:
    """
        Load a URL into a browser-content-type window.
        Safe to call immediately after createWindowEx; the load is queued
        until the browser is ready. Subsequent calls replace the page.
    """
    ...


def windowRefresh(windowID: XPLMWindowID, ignoreCache: int = 0) -> None:
    """
        Reload the current URL in a browser-content-type window.
        Pass ignoreCache = 1 to bypass the HTTP cache (shift-reload).
    """
    ...


def windowInjectScript(windowID: XPLMWindowID, script: str) -> None:
    """
        Execute a JavaScript snippet in a browser window's main frame.
        The script runs once with access to the same `xplane.*` namespace
        exposed to the page. If injected before the page finishes loading,
        it may run against an empty document.
    """
    ...


def windowAddBrowserFunction(windowID: XPLMWindowID, name: str, function: Callable[[XPLMWindowID, str, Any], Optional[str]], refCon: Any = None) -> None:
    """
        Register a callback the browser page can invoke as `xplane.<name>(arg)`.
        function(windowID, json, refCon) is called with the JSON argument string;
        its return value (a str, parsed as JSON) resolves the page's Promise.
    """
    ...


def avionicsSetURL(avionicsID: XPLMAvionicsID, url: str) -> None:
    """
        Load a URL into a browser-content-type avionics device.
        Safe to call immediately after createAvionicsEx; the load is queued
        until the browser is ready. Subsequent calls replace the page.
    """
    ...


def avionicsRefresh(avionicsID: XPLMAvionicsID, ignoreCache: int = 0) -> None:
    """
        Reload the current URL in a browser-content-type avionics device.
        Pass ignoreCache = 1 to bypass the HTTP cache (shift-reload).
    """
    ...


def avionicsInjectScript(avionicsID: XPLMAvionicsID, script: str) -> None:
    """
        Execute a JavaScript snippet in a browser device's main frame.
        The script runs once with access to the same `xplane.*` namespace
        exposed to the page. If injected before the page finishes loading,
        it may run against an empty document.
    """
    ...


def avionicsAddBrowserFunction(avionicsID: XPLMAvionicsID, name: str, function: Callable[[XPLMAvionicsID, str, Any], Optional[str]], refCon: Any = None) -> None:
    """
        Register a callback the browser page can invoke as `xplane.<name>(arg)`.
        function(avionicsID, json, refCon) is called with the JSON argument string;
        its return value (a str, parsed as JSON) resolves the page's Promise.
    """
    ...


def setObjectAvionics(object: XPLMObjectRef, avionicsID: XPLMAvionicsID) -> int:
    """
        Glue a device you created with createAvionicsEx() onto a loaded 3D object,
        so the device's screen draws on that object. The object must declare an
        ATTR_cockpit_device with the same deviceID passed to createAvionicsEx().

        Returns 1 if the object had a matching device screen and the binding
        succeeded, 0 otherwise.
    """
    ...


def clearObjectAvionics(object: XPLMObjectRef, avionicsID: XPLMAvionicsID) -> None:
    """
        Remove a binding previously made with setObjectAvionics(), restoring the
        object's device screen to black. Bindings are also cleared automatically
        when you destroy the device with destroyAvionics().
    """
    ...


def getWindowRefCon(windowID: XPLMWindowID) -> Any:
    """
        Return window's reference constant refCon.
    """
    ...


def setWindowRefCon(windowID: XPLMWindowID, refCon: Any) -> None:
    """
        Set window's reference constant refCon.
    """
    ...


def takeKeyboardFocus(windowID: XPLMWindowID | int) -> None:
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


def getModifierKeys() -> XPLMKeyFlags:
    """
        Returns the modifier keys currently held down, as an XPLMKeyFlags bitfield.
        Only modifier bits are set (ShiftFlag, OptionAltFlag, ControlFlag, CapsLockFlag);
        the DownFlag/UpFlag key-event bits are never returned.
    """
    ...


def bringWindowToFront(windowID: XPLMWindowID) -> None:
    """
        Bring window to front (of its window layer).
    """
    ...


def isWindowInFront(windowID: XPLMWindowID) -> int:
    """
        Returns 1 if window is currently in the front of its window layer).
    """
    ...


def registerHotKey(vkey: int, flags: XPLMKeyFlags = NoFlag, description: str = '', hotKey: Optional[Callable[[Any], None]] = None, refCon: Any = None) -> XPLMHotKeyID:
    """
        Registers hot key.

        Callback is hotKey(refCon), it does not need to return anything.
        Registration returns a hotKeyID, which can be used with unregisterHotKey()
    """
    ...


def unregisterHotKey(hotKeyID: XPLMHotKeyID) -> None:
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


def getNthHotKey(index: int) -> XPLMHotKeyID:
    """
        Return hotKeyID for (zero-based) Nth hot key defined in sim.
    """
    ...


def getHotKeyInfo(hotKeyID: XPLMHotKeyID) -> None | HotKeyInfo:
    """
        Return object with hot key information.

          .description
          .virtualKey
          .flags
          .plugin
    """
    ...


def setHotKeyCombination(hotKeyID: XPLMHotKeyID, vKey: int, flags: XPLMKeyFlags = NoFlag) -> None:
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


def setGraphicsState(fog: int = 0, numberTexUnits: int = 0, lighting: int = 0, alphaTesting: int = 0, alphaBlending: int = 0, depthTesting: int = 0, depthWriting: int = 0) -> None:
    """
        Change OpenGL's graphics state.

        Use instead of any glEnable / glDisable calls.
    """
    ...


def bindTexture2d(textureID: int, textureUnit: int) -> None:
    """
        Changes currently bound OpenGL texture.

        Use instead of glBindTexture(GL_TEXTURE_2D, ...)
    """
    ...


def generateTextureNumbers(count: int = 1) -> None | list[int]:
    """
        Generate number of textures for a plugin.

        Returns list of numbers.
    """
    ...


def worldToLocal(lat: float, lon: float, alt: float = 0.0) -> tuple[float, float, float]:
    """
        Convert Lat/Lon/Alt to local scene coordinates (x, y, z)

        Latitude and longitude are decimal degrees, altitude is meters MSL.
        Returns (x, y, z) in meters, in local OpenGL coordinates.
    """
    ...


def localToWorld(x: float, y: float, z: float) -> tuple[float, float, float]:
    """
        Convert local scene coordinates (x, y, z) into (lat, lon, alt)

        Latitude and longitude are decimal degrees, altitude is meters MSL.
    """
    ...


def drawTranslucentDarkBox(left: int, top: int, right: int, bottom: int) -> None:
    """
        Draw translucent dark box at location.
    """
    ...


def drawString(rgb: Sequence[float] = (1.0, 1.0, 1.0), x: int = 0, y: int = 0, value: str = '', wordWrapWidth: Optional[int] = None, fontID: XPLMFontID = Font_Proportional) -> None:
    """
        Draw a string at location (x, y)

        Default color is white (1., 1., 1.)
    """
    ...


def drawNumber(rgb: Sequence[float] = (1.0, 1.0, 1.0), x: int = 0, y: int = 0, value: float = 0.0, digits: int = -1, decimals: int = 0, showSign: int = 1, fontID: XPLMFontID = Font_Proportional) -> None:
    """
        Draw a number at location (x, y)

        Default color is white (1., 1., 1.)
    """
    ...


def getTexture(textureID: XPLMTextureID) -> int:
    """
        Return OpenGL texture ID of X-Plane texture
        Returns OpenGL texture ID of X-Plane texture based on
        a generic identifying code.
    """
    ...


def getFontDimensions(fontID: XPLMFontID) -> None | tuple[int, int, int]:
    """
        Get information about font

        Returns (width, height, digitsOnly). Proportional fonts
        return hopefully average width.
    """
    ...


def measureString(fontID: XPLMFontID, string: str) -> float:
    """
        Returns floating point width of string, with indicated font.
    """
    ...


def createInstance(obj: XPLMObjectRef, dataRefs: Optional[Sequence[str]] = None) -> XPLMInstanceRef:
    """
        Create Instance for object retrieved by loadObject() or loadObjectAsync().

        Provide list of string dataRefs to be registered for this object.
        Returns object instance, to be used with instanceSetPosition().
    """
    ...


def destroyInstance(instance: XPLMInstanceRef) -> None:
    """
        Destroys instance created by createInstance().
    """
    ...


def instanceSetAutoShift(instance: XPLMInstanceRef) -> None:
    """
        Tells X-Plane to move instance location if/when Sim's local coordinate
        system changes.
    """
    ...


def instanceSetPosition(instance: XPLMInstanceRef, position: XPLMDrawInfo_t | XPLMDrawInfoDouble_t | Sequence[float], data: Optional[Sequence[float]] = None) -> None:
    """
        Update position (x, y, z, pitch, heading, roll),
        and all datarefs (<float>, <float>, ...)

        You should provide values for all registered dataref, otherwise they'll
        be set to 0.0. This function is identical to instanceSetPositionDouble
        as Python treats all floating point as doubles.
    """
    ...


def instanceSetPositionDouble(instance: XPLMInstanceRef, position: XPLMDrawInfo_t | XPLMDrawInfoDouble_t | Sequence[float], data: Optional[Sequence[float]] = None) -> None:
    """
        Update position (x, y, z, pitch, heading, roll),
        and all datarefs (<float>, <float>, ...)

        This function is identical to instanceSetPosition, as Python treats
        all floating point as doubles.
    """
    ...


def createInstanceEx(objects: Sequence[XPLMObjectRef | tuple[XPLMObjectRef, float, float, float, float, float, float]], dataRefs: Optional[Sequence[str]] = None, coordinateSpace: XPLMCoordinateSpace_t = CoordSpace_World, aircraftIndex: int = 0, autoShift: int = 0) -> XPLMInstanceRef:
    """
        Create a multi-object instance (superset of createInstance()).

        objects is a sequence, each element being either an XPLMObjectRef (placed
        at the instance origin) or a tuple (obj, x, y, z, pitch, heading, roll)
        giving a fixed local offset. All objects move rigidly together and share
        the single dataRefs list. coordinateSpace is one of the CoordSpace_*
        constants; aircraftIndex (0=user) applies only to the aircraft spaces;
        autoShift (world space only) is like instanceSetAutoShift().

        Returns object instance, to be used with instanceSetPosition().
    """
    ...


def instanceSetCoordinateSpace(instance: XPLMInstanceRef, space: XPLMCoordinateSpace_t, aircraftIndex: int = 0) -> None:
    """
        Change the coordinate space used to interpret positions passed to
        instanceSetPosition(). space is one of the CoordSpace_* constants;
        aircraftIndex (0=user) applies only to the aircraft spaces. X-Plane
        re-expresses the instance's current world location in the new space,
        so the object does not jump.
    """
    ...


def createMapLayer(mapType: str = MAP_USER_INTERFACE, layerType: XPLMMapLayerType = MapLayer_Markings, delete: Optional[Callable[[XPLMMapLayerID, Any], None]] = None, prep: Optional[Callable[[XPLMMapLayerID, float, XPLMMapProjectionID, Any], None]] = None, draw: Optional[Callable[[XPLMMapLayerID, float, float, float, XPLMMapStyle, XPLMMapProjectionID, Any], None]] = None, icon: Optional[Callable[[XPLMMapLayerID, float, float, float, XPLMMapStyle, XPLMMapProjectionID, Any], None]] = None, label: Optional[Callable[[XPLMMapLayerID, float, float, float, XPLMMapStyle, XPLMMapProjectionID, Any], None]] = None, showToggle: int = 1, name: Optional[str] = None, refCon: Any = None) -> XPLMMapLayerID:
    """
        Returns layerID of newly created map layer, setting callbacks.

        name defaults to the calling plugin's module name (map.cpp uses
        CurrentPythonModuleName when name is omitted).

        If map does not currently exist, returns 0.
    """
    ...


def destroyMapLayer(layerID: XPLMMapLayerID) -> int:
    """
        Destroys map layer given by layerID.
    """
    ...


def registerMapCreationHook(mapCreated: Callable[[str, Any], None], refCon: Any = None) -> None:
    """
        Registers mapCreated() callback to notify you when a map is created.

        Callback gets two parameters: (mapType, refCon)
    """
    ...


def mapExists(mapType: str) -> int:
    """
        Returns 1 if mapType exists, 0 otherwise.

        mapType is either xp.MAP_USER_INTERFACE or xp.MAP_IOS
    """
    ...


def drawMapIconFromSheet(layerID: XPLMMapLayerID, png: str, s: int, t: int, ds: int, dt: int, x: float, y: float, orientation: XPLMMapOrientation, rotationDegrees: float, mapWidth: float) -> None:
    """
        Draws icon into map layer.

        Only valid within iconLayer() callback.
    """
    ...


def drawMapLabel(layerID: XPLMMapLayerID, text: str, x: float, y: float, orientation: XPLMMapOrientation, rotationDegrees: float) -> None:
    """
        Draws label within map layer.

        Only valid within labelLayer() callback.
    """
    ...


def mapProject(projection: XPLMMapProjectionID, latitude: float, longitude: float) -> None | tuple[float, float]:
    """
        Returns map layer (x, y) for given latitude, longitude.

        Only valid within map layer callbacks.
    """
    ...


def mapUnproject(projection: XPLMMapProjectionID, x: float, y: float) -> None | tuple[float, float]:
    """
        Returns latitude, longitude for given map coordinates.

        Only valid within map layer callbacks.
    """
    ...


def mapScaleMeter(projection: XPLMMapProjectionID, x: float, y: float) -> float:
    """
        Returns number of units for 'one meter' using current projection.

        Only valid within map layer callbacks.
    """
    ...


def mapGetNorthHeading(projection: XPLMMapProjectionID, x: float, y: float) -> float:
    """
        Returns mapping angle for map projection at point.

        Only valid within map layer callbacks.
    """
    ...


def getMapCallbackDict() -> dict:
    """
        Returns copy of internal MapCallbackInfo dictionary.

        Internal debugging aid: contents and structure may change between releases.
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


def createMenu(name: Optional[str] = None, parentMenuID: Optional[XPLMMenuID] = None, parentItem: Optional[int] = 0, handler: Optional[Callable[[Any, Any], None]] = None, refCon: Optional[Any] = None) -> None | XPLMMenuID:
    """
        Creates menu, returning menuID or None on error.

        parentMenuID=None adds menu to PluginsMenu.
    """
    ...


def destroyMenu(menuID: XPLMMenuID) -> None:
    """
        Remove submenu from provided menuID.
    """
    ...


def clearAllMenuItems(menuID: Optional[XPLMMenuID]) -> None:
    """
        Remove menu items from provided menuID, or 'all menus', if menuID is None.
    """
    ...


def appendMenuItem(menuID: Optional[XPLMMenuID] = None, name: str = 'Item', refCon: Any = None) -> int:
    """
        Appends new menu item to end of existing menuID.

        Returns index for created menu item or -1 on error.
    """
    ...


def appendMenuItemWithCommand(menuID: Optional[XPLMMenuID] = None, name: str = 'Command', commandRef: Any = None) -> int:
    """
        Adds menu item to existing menuID, and executes commandRef when selected.

        Returns index for created menu item or -1 on error.
    """
    ...


def appendMenuSeparator(menuID: Optional[XPLMMenuID] = None) -> None | int:
    """
        Adds separator to end of menu.

        Returns index of created item for XP11, None for XP12
    """
    ...


def setMenuItemName(menuID: Optional[XPLMMenuID] = None, index: int = 0, name: str = 'New Name') -> None:
    """
        Change the name of an existing menu item.
    """
    ...


def checkMenuItem(menuID: Optional[XPLMMenuID] = None, index: int = 0, checked: XPLMMenuCheck = Menu_Checked) -> None:
    """
        Set checkmark for given menu item.

          Menu_NoCheck = 0
          Menu_Unchecked = 1
          Menu_Checked = 2
    """
    ...


def checkMenuItemState(menuID: Optional[XPLMMenuID], index: int = 0) -> int:
    """
        Returns menu item checked state.

          Menu_NoCheck = 0
          Menu_Unchecked = 1
          Menu_Checked = 2
    """
    ...


def enableMenuItem(menuID: Optional[XPLMMenuID] = None, index: int = 0, enabled: int = 1) -> None:
    """
        Enables menu item

        Use enabled=0 to disable item, 1 to enable
    """
    ...


def removeMenuItem(menuID: Optional[XPLMMenuID] = None, index: int = 0) -> None:
    """
        Removes one item from menu.

        Note that all menu items below are moved up one index.
    """
    ...


def getMenuCallbackDict() -> dict:
    """
        Returns copy of internal MenuCallbackInfo dictionary.

        Internal debugging aid: contents and structure may change between releases.
    """
    ...


def getFirstNavAid() -> int:
    """
        Returns navRef of first entry in navaid database.
    """
    ...


def getNextNavAid(navRef: XPLMNavRef) -> XPLMNavRef:
    """
        Returns next navRef after the provided value.
    """
    ...


def findFirstNavAidOfType(navType: XPLMNavType) -> XPLMNavRef:
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
          Nav_TACAN       =4096
    """
    ...


def findLastNavAidOfType(navType: XPLMNavType) -> XPLMNavRef:
    """
        Returns navRef of last navAid of given type.
    """
    ...


def findNavAid(name: Optional[str] = None, navAidID: Optional[str] = None, lat: Optional[float] = None, lon: Optional[float] = None, freq: Optional[int] = None, navType: XPLMNavType = Nav_Any) -> XPLMNavRef:
    """
        Returns navRef of last navAid matching information.

        name and navAidID are case-sensitive and will match a fragment of the actual value.
        freq is an integer, 100x the real frequency value (eg. 137.75 -> 13775) except for NDB.
        navType=0xffffffff will match any type value
    """
    ...


def getNavAidInfo(navRef: XPLMNavRef) -> NavAidInfo:
    """
        Returns NavAidInfo object for given navRef

        Attributes are:
         .type        # a NavType
         .latitude
         .longitude
         .height     # in meters
         .frequency  # integer, for NDB, value is exact, otherwise divide by 100.0
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


def setDisplayedFMSEntry(index: int) -> None:
    """
        Sets index number for FMS Entry to be displayed.
    """
    ...


def setDestinationFMSEntry(index: int) -> None:
    """
        Sets index number for FMS Entry to become the current destination.
    """
    ...


def getFMSEntryInfo(index: int) -> FMSEntryInfo:
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


def setFMSEntryInfo(index: int, navRef: XPLMNavRef, altitude: int = 0) -> None:
    """
        Set given FMS Entry to provided navRef and altitude (feet)
    """
    ...


def setFMSEntryLatLon(index: int, lat: float, lon: float, altitude: int = 0) -> None:
    """
        Set given FMS Entry to provided (lat, lon) and altitude(feet).
    """
    ...


def clearFMSEntry(index: int) -> None:
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


def countFMSFlightPlanEntries(flightPlan: XPLMNavFlightPlan) -> int:
    """
        Returns number of FMS Entries in given flightplan:
          Fpl_Pilot_Primary
          Fpl_CoPilot_Primary
          Fpl_Pilot_Approach
          Fpl_CoPilot_Approach
          Fpl_Pilot_Temporary
          Fpl_CoPilot_Temporary
    """
    ...


def getDisplayedFMSFlightPlanEntry(flightPlan: XPLMNavFlightPlan) -> int:
    """
        Returns index number of the entry the pilot is viewing.
    """
    ...


def getDestinationFMSFlightPlanEntry(flightPlan: XPLMNavFlightPlan) -> int:
    """
        Returns the index number of the flight plan destination.
    """
    ...


def setDisplayedFMSFlightPlanEntry(flightPlan: XPLMNavFlightPlan, index: int) -> None:
    """
        Sets index number for FMS Entry to be displayed for this flight plan.
    """
    ...


def setDestinationFMSFlightPlanEntry(flightPlan: XPLMNavFlightPlan, index: int) -> None:
    """
        Sets index number for the FMS Entry to become the destination. The track
        is from the n-1'th point to this point.
    """
    ...


def setDirectToFMSFlightPlanEntry(flightPlan: XPLMNavFlightPlan, index: int) -> None:
    """
        Sets the entry the FMS is flying toward. The track is from the current
        position directly to this point, ignoring the point before it in the flight plan.
    """
    ...


def getFMSFlightPlanEntryInfo(flightPlan: XPLMNavFlightPlan, index: int) -> FMSEntryInfo:
    """
        Returns FMSEntryInfo object for given FMS index for this flightplan.

        Attributes are:
         .type      # a NavType
         .navAidID
         .ref       # navRef (use as input to getNavAidInfo())
         .altitude  # (in feet)
         .lat
         .lon
    """
    ...


def setFMSFlightPlanEntryInfo(flightPlan: XPLMNavFlightPlan, index: int, navRef: XPLMNavRef, altitude: int = 0) -> None:
    """
        Sets given FMS entry to provided navRef and altitude (feet).
    """
    ...


def setFMSFlightPlanEntryLatLon(flightPlan: XPLMNavFlightPlan, index: int, lat: float, lon: float, altitude: int = 0) -> None:
    """
        Set given FMS Entry to provided (lat, lon) and altitude(feet).
    """
    ...


def setFMSFlightPlanEntryLatLonWithId(flightPlan: XPLMNavFlightPlan, index: int, lat: float, lon: float, altitude: int = 0, ID: Optional[str] = None) -> None:
    """
        Sets entry in the FMS to a lat/lon entry, with the given coordinates
        and display ID for the waypoint.
    """
    ...


def clearFMSFlightPlanEntry(flightPlan: XPLMNavFlightPlan, index: int) -> None:
    """
        Clears given FMS Entry.
    """
    ...


def loadFMSFlightPlan(device: int, plan: str) -> None:
    """
        Loads provided flightplan into pilot (device=0) or co-pilot (device=1) unit.
    """
    ...


def saveFMSFlightPlan(device: Optional[int] = 0) -> str:
    """
        Returns X-Plane 11 formatted flightplan from FMS or GPS.
        Use device=0 for pilot, device=1 for co-pilot side unit.
    """
    ...


def makeColor(red: float, green: float, blue: float, alpha: float) -> int:
    """
        Pack four float color components (each 0.0-1.0, clamped) into a single
        packed color value (ABGR) for use with the panel graphics routines.
    """
    ...


def setLineCap(lineCap: int = LineCapButt) -> None:
    """
        Set how subsequent lines are capped at their start and end points.

        lineCap is LineCapButt (straight edge at the endpoint), LineCapRound
        (half circle centered on the endpoint), or LineCapSquare (half square
        centered on the endpoint). Called with no argument, it restores the
        default LineCapButt, which is also in effect at the start of each
        drawing callback.
    """
    ...


def lines(color: int, vertices: Sequence[tuple[float, float]]) -> None:
    """
        Draw disconnected line segments. Each consecutive pair of vertices forms
        one segment.
    """
    ...


def linesWithWidth(color: int, lineWidth: float, vertices: Sequence[tuple[float, float]]) -> None:
    """
        Draw disconnected line segments with the given line width.
    """
    ...


def linesStipple(color: int, vertices: Sequence[tuple[float, float]], dashLength: float, lineWidth: float) -> None:
    """
        Draw disconnected dashed line segments.
    """
    ...


def linesc(vertices: Sequence[tuple[float, float, int]]) -> None:
    """
        Draw disconnected line segments with per-vertex colors.
    """
    ...


def linescWithWidth(lineWidth: float, vertices: Sequence[tuple[float, float, int]]) -> None:
    """
        Draw disconnected line segments with per-vertex colors and line width.
    """
    ...


def lineStrip(color: int, vertices: Sequence[tuple[float, float]]) -> None:
    """
        Draw a connected line strip; the last vertex is not closed back.
    """
    ...


def lineStripWithWidth(color: int, lineWidth: float, vertices: Sequence[tuple[float, float]]) -> None:
    """
        Draw a connected line strip with the given line width.
    """
    ...


def lineStripc(vertices: Sequence[tuple[float, float, int]]) -> None:
    """
        Draw a connected line strip with per-vertex colors.
    """
    ...


def lineStripcWithWidth(lineWidth: float, vertices: Sequence[tuple[float, float, int]]) -> None:
    """
        Draw a connected line strip with per-vertex colors and line width.
    """
    ...


def lineStripStipple(color: int, vertices: Sequence[tuple[float, float]], dashLength: float, lineWidth: float) -> None:
    """
        Draw a connected dashed line strip.
    """
    ...


def lineLoop(color: int, vertices: Sequence[tuple[float, float]]) -> None:
    """
        Draw a closed line loop; the last vertex connects back to the first.
    """
    ...


def lineLoopWithWidth(color: int, lineWidth: float, vertices: Sequence[tuple[float, float]]) -> None:
    """
        Draw a closed line loop with the given line width.
    """
    ...


def lineLoopc(vertices: Sequence[tuple[float, float, int]]) -> None:
    """
        Draw a closed line loop with per-vertex colors.
    """
    ...


def lineLoopcWithWidth(lineWidth: float, vertices: Sequence[tuple[float, float, int]]) -> None:
    """
        Draw a closed line loop with per-vertex colors and line width.
    """
    ...


def lineLoopStipple(color: int, vertices: Sequence[tuple[float, float]], dashLength: float, lineWidth: float) -> None:
    """
        Draw a closed dashed line loop.
    """
    ...


def polygon(color: int, vertices: Sequence[tuple[float, float]]) -> None:
    """
        Draw a filled convex polygon (at least 3 vertices).
    """
    ...


def polygonc(vertices: Sequence[tuple[float, float, int]]) -> None:
    """
        Draw a filled convex polygon with per-vertex colors.
    """
    ...


def quadstrip(color: int, vertices: Sequence[tuple[float, float]]) -> None:
    """
        Draw a series of connected filled quadrilaterals (count even, >= 4).
    """
    ...


def quadstripc(vertices: Sequence[tuple[float, float, int]]) -> None:
    """
        Draw a quad strip with per-vertex colors.
    """
    ...


def transformPush() -> None:
    """
        Save the current transformation matrix onto the transform stack.
    """
    ...


def transformPop() -> None:
    """
        Restore the transformation matrix from the top of the transform stack.
    """
    ...


def transformTranslate(dx: float, dy: float) -> None:
    """
        Offset all subsequent drawing by (dx, dy) pixels.
    """
    ...


def transformRotate(centerX: float, centerY: float, angle: float) -> None:
    """
        Rotate subsequent drawing around (centerX, centerY) by angle degrees.
    """
    ...


def transformScale(scaleX: float, scaleY: float) -> None:
    """
        Scale subsequent drawing by (scaleX, scaleY) relative to the origin.
    """
    ...


def scissorPush() -> None:
    """
        Save the current scissor rectangle onto the scissor stack.
    """
    ...


def scissorPop() -> None:
    """
        Restore the scissor rectangle from the top of the scissor stack.
    """
    ...


def scissorSet(left: int, top: int, right: int, bottom: int) -> None:
    """
        Set an absolute scissor rectangle; only pixels inside are drawn.
    """
    ...


def scissorIntersect(left: int, top: int, right: int, bottom: int) -> None:
    """
        Intersect the current scissor box with the absolute rectangle given by edges (left, top, right, bottom) in panel pixels; the result is their overlap (drawable area only shrinks).
    """
    ...


def beginSetupStencilMask(bits: int, mask: int) -> None:
    """
        Begin stencil mask setup; subsequent drawing writes to the stencil buffer.
    """
    ...


def endSetupStencilMask() -> None:
    """
        End stencil mask setup; subsequent drawing renders to the screen again.
    """
    ...


def useStencilMask(bits: int, mask: int) -> None:
    """
        Activate stencil testing for subsequent drawing.
    """
    ...


def clearStencilMask() -> None:
    """
        Clear the stencil buffer and disable stencil testing.
    """
    ...


def createSVTDisplay(pilotIndex: int = 0, pixelsPerDegree: float = 14.0) -> XPLMSVTDisplayRef:
    """
        Create a Synthetic Vision (SVT) display that renders a 3-D perspective view
        of terrain, runways, and optional overlays into an avionics panel.
        pilotIndex is 0 for pilot-side AHRS, 1 for copilot. pixelsPerDegree is the
        vertical scale of the 3-d view at the center of the display; must be > 0,
        the G1000 PFD uses 14. Which layers are drawn is chosen per draw call.
        Draw it with svtDisplayDrawIn() and free it with destroySVTDisplay().
    """
    ...


def destroySVTDisplay(svt: XPLMSVTDisplayRef) -> None:
    """
        Destroy an SVT display created with createSVTDisplay() and free its resources.
    """
    ...


def svtDisplayDrawIn(svt: XPLMSVTDisplayRef, features: int, left: int, top: int, right: int, bottom: int, dataOverrides: Optional[Sequence[float]] = None) -> None:
    """
        Render the SVT display into the panel within (left, top, right, bottom). Must
        be called from an avionics drawing callback; does nothing until terrain tiles
        finish loading. features is a bitwise OR of SVT_* flags for this draw.
        dataOverrides is None for live sim state, or a sequence of 9 values:
        (pitchDeg, rollDeg, headingMagDeg, magVarDeg, indicatedAltFt, baroSettingInHg,
        hsiSource, hdefDots, vdefDots).
    """
    ...


def createMapDisplay(pilotIndex: int = 0) -> XPLMMapDisplayRef:
    """
        Create a base map display (ND/MFD background) that renders terrain, topography,
        water, weather and taxi layouts. pilotIndex is 0 for pilot-side GPS position,
        1 for copilot. Draw it with mapDisplayDrawIn() and free it with destroyMapDisplay().
    """
    ...


def destroyMapDisplay(map: XPLMMapDisplayRef) -> None:
    """
        Destroy a map display created with createMapDisplay() and free its resources.
    """
    ...


def mapDisplayDrawIn(map: XPLMMapDisplayRef, layers: int, left: int, top: int, right: int, bottom: int, dataOverrides: Optional[Sequence[float]] = None) -> None:
    """
        Render the base map into the panel within (left, top, right, bottom). Must be
        called from an avionics drawing callback; does nothing until terrain tiles
        finish loading. layers is a bitwise OR of Map_* flags (some are mutually
        exclusive -- e.g. Map_Nexrad with Map_EGPWS or Map_IR). dataOverrides is None
        for live sim state, or a sequence of 12 or 15 values: (datLat, datLon, ctrX,
        ctrY, roseRadius, mapRange, orientation, terrainWarn, terrainCaution, acfAlt,
        gearDown, trueRotation[, nearestRwyElev, egpwsBrightness, egpwsStyle]). The last
        three (EGPWS) are XPLM440 additions; omit them (pass 12) to leave them at 0.
    """
    ...


def mapDisplayProject(map: XPLMMapDisplayRef, layers: int, left: int, top: int, right: int, bottom: int, latitude: float, longitude: float, dataOverrides: Optional[Sequence[float]] = None) -> tuple[float, float] | None:
    """
        Convert a latitude/longitude into an (x, y) position in panel coordinates, for
        the map described by these arguments. Inverse of mapDisplayUnproject(). Pass the
        SAME arguments you draw the map with and you get the projection that draw call
        produces. Need not be called from a drawing callback. Returns None if terrain
        tiles have not loaded or the point is not on this map.
    """
    ...


def mapDisplayUnproject(map: XPLMMapDisplayRef, layers: int, left: int, top: int, right: int, bottom: int, x: float, y: float, dataOverrides: Optional[Sequence[float]] = None) -> tuple[float, float] | None:
    """
        Convert an (x, y) position in panel coordinates back into a (latitude, longitude),
        for the map described by these arguments. Inverse of mapDisplayProject(). Need not
        be called from a drawing callback. Returns None if terrain tiles have not loaded or
        the point does not correspond to anywhere on the earth.
    """
    ...


def mapDisplayScaleMeter(map: XPLMMapDisplayRef, layers: int, left: int, top: int, right: int, bottom: int, x: float, y: float, dataOverrides: Optional[Sequence[float]] = None) -> float:
    """
        Return how many pixels correspond to one meter at (x, y) on the map described by
        these arguments. Use it to size symbols and range rings so they stay correct as the
        range changes. Returns 0.0 if terrain tiles have not loaded yet.
    """
    ...


def mapDisplayGetNorthHeading(map: XPLMMapDisplayRef, layers: int, left: int, top: int, right: int, bottom: int, x: float, y: float, dataOverrides: Optional[Sequence[float]] = None) -> float:
    """
        Return the heading, in degrees clockwise from straight up on the display, at which
        true north lies at (x, y). ADD it to a true heading to get the angle to draw that
        heading at. Accounts for the map's own rotation and for projection convergence.
        Returns 0.0 if terrain tiles have not loaded yet.
    """
    ...


def mapDisplayGetTerrainAltitudes(map: XPLMMapDisplayRef) -> tuple[float, float] | None:
    """
        Return (minAltitude, maxAltitude) -- the lowest and highest altitude (feet)
        shown on the map's EGPWS terrain display. Altitudes are only available if the
        map was drawn with the Map_EGPWS layer; if not, returns None. Must be called
        from an avionics drawing callback.
    """
    ...


def createTextureAtlas() -> XPLMTextureAtlasRef:
    """
        Create a new, empty texture atlas. Add images with textureAtlasAddImageFile() /
        textureAtlasAddImage() and their *Set() variants, then call textureAtlasBake()
        before drawing. Destroy with destroyTextureAtlas() when done.
    """
    ...


def destroyTextureAtlas(atlas: XPLMTextureAtlasRef) -> None:
    """
        Destroy a texture atlas created with createTextureAtlas() and free all
        associated GPU and CPU resources.
    """
    ...


def textureAtlasAddImageFile(atlas: XPLMTextureAtlasRef, imageFilePath: str) -> int:
    """
        Load a PNG file and add it to the atlas as a single image. Call before
        textureAtlasBake(). Returns the zero-based image index assigned.
    """
    ...


def textureAtlasAddImageFileSet(atlas: XPLMTextureAtlasRef, imageFilePath: str, cellsX: int, cellsY: int) -> int:
    """
        Load a PNG file and subdivide it into a cellsX x cellsY grid, adding each cell
        as a separate image (useful for sprite sheets). Returns the index of the first
        (top-left) cell; cell (x, y) is index + y * cellsX + x.
    """
    ...


def textureAtlasAddImage(atlas: XPLMTextureAtlasRef, image: bytes, width: int, height: int) -> int:
    """
        Add a single image from raw RGBA pixel data (4 bytes/pixel, rows top to bottom)
        to the atlas. image must hold at least width * height * 4 bytes. Returns the
        zero-based image index assigned.
    """
    ...


def textureAtlasAddImageSet(atlas: XPLMTextureAtlasRef, image: bytes, width: int, height: int, cellsX: int, cellsY: int) -> int:
    """
        Add raw RGBA pixel data (4 bytes/pixel, rows top to bottom) to the atlas,
        subdividing it into a cellsX x cellsY grid; each cell becomes a separate image.
        image must hold at least width * height * 4 bytes. Returns the index of the
        first cell; cell (x, y) is index + y * cellsX + x.
    """
    ...


def textureAtlasBake(atlas: XPLMTextureAtlasRef) -> None:
    """
        Pack all previously added images into a GPU texture. Must be called after
        adding all images and before any draw calls. Once baked, no more images may
        be added.
    """
    ...


def textureAtlasGetImageWidth(atlas: XPLMTextureAtlasRef, imageIndex: int) -> int:
    """
        Return the width in pixels of a single image (or cell) in the atlas.
    """
    ...


def textureAtlasGetImageHeight(atlas: XPLMTextureAtlasRef, imageIndex: int) -> int:
    """
        Return the height in pixels of a single image (or cell) in the atlas.
    """
    ...


def textureAtlasDrawAt(atlas: XPLMTextureAtlasRef, imageIndex: int, tintColor: int, x: float, y: float) -> None:
    """
        Draw an atlas image at its native resolution with its top-left corner at (x, y).
        tintColor is multiplied with the texture; use makeColor(1, 1, 1, 1) for no tint.
    """
    ...


def textureAtlasDrawIn(atlas: XPLMTextureAtlasRef, imageIndex: int, tintColor: int, left: float, top: float, right: float, bottom: float) -> None:
    """
        Draw an atlas image scaled to fill the rectangle (left, top, right, bottom),
        stretching or compressing to match. tintColor is multiplied with the texture.
    """
    ...


def textureAtlasDrawStretched(atlas: XPLMTextureAtlasRef, imageIndex: int, tintColor: int, left: float, top: float, right: float, bottom: float) -> None:
    """
        Draw an atlas image into the rectangle (left, top, right, bottom) using 9-slice
        scaling: the four corners keep their native size, edges stretch along one axis,
        and the center stretches in both. Preserves borders when scaling UI elements.
        tintColor is multiplied with the texture.
    """
    ...


def textureAtlasDrawScaled(atlas: XPLMTextureAtlasRef, imageIndex: int, tintColor: int, xPanel: float, yPanel: float, xAtlas: float, yAtlas: float, xScale: float, yScale: float, rotateCW: float) -> None:
    """
        Draw an atlas image with arbitrary scaling, rotation, and positioning. The
        atlas-space pivot (xAtlas, yAtlas), in pixels from the image's bottom-left, is
        aligned to the panel-space point (xPanel, yPanel), then scaled by (xScale,
        yScale) and rotated rotateCW degrees clockwise about that pivot. tintColor is
        multiplied with the texture.
    """
    ...


def textureAtlasDrawMesh(atlas: XPLMTextureAtlasRef, imageIndex: int, tintColor: int, vertices: Sequence[tuple[float, float, float, float]]) -> None:
    """
        Draw an atlas image onto an arbitrary triangle-strip mesh. Each vertex is an
        (x, y, s, t) tuple: (x, y) panel-space position in pixels, (s, t) normalized
        texture coordinate (0.0-1.0) within the image. At least 3 vertices are required.
        tintColor is multiplied with the texture.
    """
    ...


def textureSourceDrawIn(tex: int, tintColor: int, left: int, top: int, right: int, bottom: int) -> None:
    """
        Draw a stock simulator texture source (e.g. Texture_WeatherRadar1), scaled to
        fill the rectangle (left, top, right, bottom) in panel coordinates. Unlike a
        texture atlas, a texture source is a live texture the simulator renders each
        frame. tintColor is multiplied with the texture; use makeColor(1, 1, 1, 1) for
        no tinting. If the aircraft lacks the requested hardware, the call is skipped.
    """
    ...


def textureSourceDrawMesh(tex: int, tintColor: int, vertices: Sequence[tuple[float, float, float, float]]) -> None:
    """
        Draw a stock simulator texture source onto an arbitrary triangle-strip mesh.
        Each vertex is an (x, y, s, t) tuple: (x, y) panel-space position in pixels,
        (s, t) normalized texture coordinate (0.0-1.0) within the source. At least 3
        vertices are required. tintColor is multiplied with the texture.
    """
    ...


def createTexture(rgba: bytes, width: int, height: int) -> XPLMTexture:
    """
        Upload an RGBA8 image (4 bytes/pixel, rows top to bottom; rgba must hold at
        least width * height * 4 bytes) to the GPU and return an opaque texture handle
        for use as the tex of a draw call passed to drawCalls(). Free it with
        destroyTexture(). The sampler is bilinear, clamp-to-edge, no mipmaps.
    """
    ...


def destroyTexture(tex: XPLMTexture) -> None:
    """
        Free a texture created with createTexture(). The handle must not be used after
        this call. It is safe to create and destroy textures every frame.
    """
    ...


def drawCalls(vertices: Union[bytes, Sequence[tuple[float, ...]]], indices: Union[bytes, Sequence[int]], drawCalls: Sequence, indexSize: int = 2) -> None:
    """
        Render textured indexed-triangle draw calls matching Dear ImGui's ImDrawData
        layout. vertices is a bytes buffer of 20-byte vertices: pos.x, pos.y, uv.x,
        uv.y as float32, then RGBA8 packed as a little-endian uint32. indices is a
        bytes buffer of uint16 indices.

        For convenience, vertices may instead be a sequence of (x, y, u, v, color)
        tuples -- color being a makeColor() value -- and indices a sequence of ints;
        we pack them for you. color may be omitted -- (x, y, u, v) takes opaque white,
        that is, no tint -- but x, y, u and v are always required. Pass bytes when you
        already have them (an ImGui frame, or a mesh you build once and reuse):
        packing here costs a copy per frame.

        indexSize is the width in bytes of each index in the indices BUFFER: 2 (the
        default, what the SDK takes) or 4. Pass 4 when Dear ImGui was built with 32-bit
        ImDrawIdx -- we narrow to uint16 here, which is far cheaper than doing it in
        Python. An index above 65535 raises ValueError; split the mesh across several
        draw calls using vtx_offset. indexSize is ignored when indices is a sequence of
        ints rather than a buffer.

        drawCalls is a sequence of (tex, scissors,
        idx_offset, element_count, vtx_offset): tex is a createTexture() handle or None;
        scissors is (left, top, right, bottom) floats in window-local top-left coords;
        element_count must be a multiple of 3. Call only from a panel-graphics window
        draw callback; the host flips Y for you.
    """
    ...


def accumulateTouchZone(type: int, left: int, top: int, right: int, bottom: int, command: Optional[XPLMCommandRef] = None, identifier: int = 0) -> int:
    """
        Register an interactive touch zone for the current frame. Call every frame from
        your avionics drawing callback for each region. type is one of TouchZone_Nothing,
        TouchZone_Command (fires command on press/release), or TouchZone_Identifier
        (delivers events to the handler set with avionicsSetTouchEventHandler /
        windowSetTouchEventHandler, tagged with identifier). Returns True while the zone
        is being pressed/held. Zones registered later win overlaps.
    """
    ...


def avionicsSetTouchEventHandler(avionic: XPLMAvionicsID, handler: Optional[Callable], refCon: Any = None) -> None:
    """
        Register handler to receive touch events for TouchZone_Identifier zones on the
        given avionics device. handler is called as handler(identifier, status, x, y,
        dx, dy, button, refcon). Pass handler=None to remove the current handler.
    """
    ...


def windowSetTouchEventHandler(window: XPLMWindowID, handler: Optional[Callable], refCon: Any = None) -> None:
    """
        Register handler to receive touch events for TouchZone_Identifier zones on the
        given window. handler is called as handler(identifier, status, x, y, dx, dy,
        button, refcon). Pass handler=None to remove the current handler.
    """
    ...


def createFont(charset: int) -> XPLMFontHandle:
    """
        Create a new font handle for the given character set (CharSetDigits,
        CharSetASCII, CharSetUnicode). Add faces with fontAddFace() before drawing.
    """
    ...


def destroyFont(font: XPLMFontHandle) -> None:
    """
        Destroy a font handle created with createFont() and free its resources.
    """
    ...


def fontAddFace(font: XPLMFontHandle, ttf_path: str) -> int:
    """
        Add a TrueType (.ttf/.otf) face to a font handle. Multiple faces provide
        fallback glyphs, searched in the order added.
    """
    ...


def fontGetMetrics(font: XPLMFontHandle, fontSize: float) -> tuple[float, float, float]:
    """
        Return (lineHeight, lineAscent, lineDescent) in pixels for the font at size.
    """
    ...


def fontMeasureString(font: XPLMFontHandle, fontSize: float, string: str) -> float:
    """
        Return the width in pixels the string would occupy if drawn (not drawn).
    """
    ...


def fontGetLineCount(font: XPLMFontHandle, fontSize: float, string: str, width: float) -> int:
    """
        Return how many lines the string would occupy if word-wrapped to width.
    """
    ...


def fontFitForward(font: XPLMFontHandle, fontSize: float, string: str, width: float) -> int:
    """
        Return the number of characters from the start of the string that fit within
        width, measured left to right.
    """
    ...


def fontFitReverse(font: XPLMFontHandle, fontSize: float, string: str, width: float) -> int:
    """
        Return the number of leading characters to skip so the remaining tail
        (string[index:]) fits within width, measured right to left. 0 = whole string
        fits; len(string) = nothing fits.
    """
    ...


def fontDrawString(font: XPLMFontHandle, color: int, fontSize: float, x: float, y: float, string: str, justification: int) -> None:
    """
        Draw a string at (x, y) baseline anchor with the given font, size, packed
        color, and justification (JustLeft, JustCenter, JustRight).
    """
    ...


def fontDrawStringFixedSpacing(font: XPLMFontHandle, color: int, fontSize: float, x: float, y: float, string: str, fixedSpacing: int, justification: int) -> None:
    """
        Draw a string using fixed per-character spacing (pixels) instead of the font's
        natural proportional spacing.
    """
    ...


def fontDrawStringWordWrapped(font: XPLMFontHandle, color: int, fontSize: float, x: float, y: float, string: str, wrapWidth: int, justification: int) -> None:
    """
        Draw a string with automatic word wrapping at wrapWidth pixels.
    """
    ...


def fontDrawStringRotated(font: XPLMFontHandle, color: int, fontSize: float, x: float, y: float, string: str, angle: float, justification: int) -> None:
    """
        Draw a string rotated by angle degrees (positive clockwise) around the (x, y)
        anchor point.
    """
    ...


def beginRetainedDrawing() -> None:
    """
        Begin recording panel-graphics commands into a retained drawing. All
        panel-graphics calls made until endRetainedDrawing() are captured instead
        of being drawn immediately. Recording sessions must not be nested.
    """
    ...


def endRetainedDrawing() -> XPLMRetainedDrawing:
    """
        End the recording started by beginRetainedDrawing() and return an opaque
        handle to the captured commands. Replay it with drawRetained() and free it
        with destroyRetainedDrawing(). If a font or texture atlas used during
        recording is destroyed, you must destroy the retained drawing as well --
        replaying it afterwards references invalid resources.
    """
    ...


def drawRetained(drawing: XPLMRetainedDrawing) -> None:
    """
        Replay a retained drawing captured with endRetainedDrawing(). May be
        called any number of times per frame and across frames to redraw the same
        content cheaply.
    """
    ...


def destroyRetainedDrawing(drawing: XPLMRetainedDrawing) -> None:
    """
        Destroy a retained drawing captured with endRetainedDrawing() and free its
        resources. The handle must not be used after this call.
    """
    ...


def setUsersAircraft(path: str) -> None:
    """
        Change the user's aircraft and reinitialize.

        path is either relative X-Plane root, or fully qualified,
        including the .acf extension.
    """
    ...


def initFlight(data: Union[str, dict]) -> int:
    """
        Initialize user flight with json data (either string or dict)
    """
    ...


def updateFlight(data: Union[str, dict]) -> int:
    """
        Update user flight with json data (either string or dict).
    """
    ...


def placeUserAtAirport(code: str) -> None:
    """
        Place user at given airport, specified by ICAO code.

        Invalid airport code will crash the sim.
    """
    ...


def placeUserAtLocation(latitude: float, longitude: float, elevation: float, heading: float, speed: float) -> None:
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


def getNthAircraftModel(index: int) -> tuple[str, str]:
    """
        Return (filename, fullPath) of aircraft at index
    """
    ...


def acquirePlanes(aircraft: Optional[Sequence[str]] = None, callback: Optional[Callable[[Any], None]] = None, refCon: Any = None) -> int:
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


def setActiveAircraftCount(count: int) -> None:
    """
        Set number of active planes.

        Only sets up to number of available planes. You must
        first have access -- acquirePlanes().
    """
    ...


def setAircraftModel(index: int, path: str) -> None:
    """
        Load aircraft model into index. (Do not use index=0)

        Path is absolute, or relative to X-Plane root.
    """
    ...


def disableAIForPlane(index: int) -> None:
    """
        Turn off AI for given airplane.

        Plane will continue to draw, but will not move itself.
    """
    ...


def getMyID() -> XPLMPluginID:
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


def getNthPlugin(index: int) -> XPLMPluginID:
    """
        Return the ID of a (non-python) plugin by index.
    """
    ...


def findPluginByPath(path: str) -> XPLMPluginID:
    """
        Return pluginID of (non-python) plugin whose file exists at path.

        Path must be absolute.
    """
    ...


def findPluginBySignature(signature: str) -> XPLMPluginID:
    """
        Return the pluginID of the (non-python) plugin whose signature matches.
    """
    ...


def getPluginInfo(pluginID: XPLMPluginID) -> PluginInfo:
    """
        Return information about plugin.

        Return value is an object with attributes:
          .name
          .filePath
          .signature
          .description
    """
    ...


def isPluginEnabled(pluginID: XPLMPluginID) -> int:
    """
        Return 1 if plugin is enabled, 0 otherwise
    """
    ...


def enablePlugin(pluginID: XPLMPluginID) -> int:
    """
        Enables plugin.
    """
    ...


def disablePlugin(pluginID: XPLMPluginID) -> None:
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


def reloadThisPlugin(forReplacement: int = 0) -> None:
    """
        Reload *this* (the calling) plugin.

        Once you return from the current callback, this plugin receives its
        XPluginDisable / XPluginStop callbacks, is unloaded, then started again as if
        the sim were starting up. If 'forReplacement' is true, a dialog is shown after
        the .xpl is unloaded so you can swap in a newer one manually.

        New in XPLM440.
    """
    ...


def sendMessageToPlugin(pluginID: XPLMPluginID, message: int, param: Optional[Any]) -> None:
    """
        Send message to plugin

        Messages sent to XPPython3 plugin will be forwarded to all
        python plugins.
    """
    ...


def hasFeature(feature: str) -> int:
    """
        Return 1 if X-Plane supports feature.
    """
    ...


def isFeatureEnabled(feature: str) -> int:
    """
        Returns 1 if feature is currently enabled for your plugin.
    """
    ...


def enableFeature(feature: str, enable: int = 1) -> None:
    """
        Enables / disables indicated feature for this plugin.
    """
    ...


def enumerateFeatures(enumerator: Callable[[str, Any], None], refCon: Any = None) -> None:
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


def registerFlightLoopCallback(callback: Callable[[float, float, int, Any], float], interval: float = 0.0, refCon: Any = None) -> None:
    """
        Register flight loop callback.

        interval indicates when you'll be called next:
          0= deactivate
          >0 seconds
          <0 flightLoops
        Callback function gets (lastCall, elapsedTime, counter, refCon)
    """
    ...


def unregisterFlightLoopCallback(callback: Callable[[float, float, int, Any], float], refCon: Any = None) -> None:
    """
        Unregisters flightloop matching callback and refCon.

        Input must match that provided to registerFlightLoopCallback().
    """
    ...


def setFlightLoopCallbackInterval(callback: Callable[[float, float, int, Any], float], interval: float = 0.0, relativeToNow: int = 1, refCon: Any = None) -> None:
    """
        Change interval associated with callback, refCon.

        Must have been previously registered with registerFlightLoopCallback()
    """
    ...


def createFlightLoop(callback: Callable[[float, float, int, Any], float], phase: XPLMFlightLoopPhaseType = FlightLoop_Phase_BeforeFlightModel, refCon: Any = None) -> XPLMFlightLoopID:
    """
        Create flight loop, returning flightLoopID

        Callback take (sinceLast, elapsedTime, counter, refCon)
        returning interval (0=stop, >0 seconds, <0 flightloops)
        phase is 0=before or 1=After flight model integration
    """
    ...


def isFlightLoopValid(flightLoopID: XPLMFlightLoopID) -> bool:
    """
        Return True if flightLoopID exists and is valid: it may or may not be scheduled.
    """
    ...


def destroyFlightLoop(flightLoopID: XPLMFlightLoopID) -> None:
    """
        Destroys flight loop previously created by createFlightLoop()
    """
    ...


def scheduleFlightLoop(flightLoopID: XPLMFlightLoopID, interval: float = 0.0, relativeToNow: int = 1) -> None:
    """
        Change interval associated with flight loop

          0= stop
          >0 seconds
          <0 flightLoops
        If relativeToNow is 1, interval is relative to now, otherwise
        interval is relative to previous callback execution.
    """
    ...


def getFlightLoopCallbackDict() -> dict:
    """
        Returns copy of internal FlightLoopCallbackInfo dictionary.

        Internal debugging aid: contents and structure may change between releases.
    """
    ...


def createProbe(probeType: XPLMProbeType = ProbeY) -> XPLMProbeRef:
    """
        Return a probeRef
    """
    ...


def destroyProbe(probe: XPLMProbeRef) -> None:
    """
        Destroy a probeRef
    """
    ...


def probeTerrainXYZ(probe: XPLMProbeRef, x: float, y: float, z: float) -> XPLMProbeInfo_t:
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
          .velocityZ: Velocity vector (e.g., meter/sec) of movement
          .is_wet:    1=we hit water
    """
    ...


def getMagneticVariation(latitude: float, longitude: float) -> float:
    """
        Magnetic declination at point
    """
    ...


def degTrueToDegMagnetic(degrees: float = 0.0) -> float:
    """
        Convert degrees True to degrees Magnetic, at user's current location
    """
    ...


def degMagneticToDegTrue(degrees: float = 0.0) -> float:
    """
        Convert degrees Magnetic to degrees True, at user's current location
    """
    ...


def loadObject(path: str) -> XPLMObjectRef:
    """
        Load OBJ file from path, returning objectRef

        Path may be absolute, or relative X-Plane Root
    """
    ...


def loadObjectAsync(path: str, loaded: Callable[[XPLMObjectRef, Any], None], refCon: Any = None) -> None:
    """
        Loads OBJ asynchronously, calling callback on completion.

        Callback signature is loaded(objectRef, refCon)
        Object path is absolute or relative X-Plane root.
    """
    ...


def unloadObject(objectRef: XPLMObjectRef) -> None:
    """
        Unloads objectRef
    """
    ...


def lookupObjects(path: str, latitude: float = 0.0, longitude: float = 0.0, enumerator: Optional[Callable[[str, Any], None]] = None, refCon: Any = None) -> int:
    """
        Converts virtual path to file paths, calling enumerator with info

        Path is virtual path, which may have zero or more matching file paths
        in library. Count of results is returned by lookupObjects().
        For each item found, enumerator(path, refCon) is called.
    """
    ...


def getObjects(path: str, latitude: float = 0.0, longitude: float = 0.0) -> tuple[XPLMObjectRef | None]:
    """
        Converts virtual path to tuple of XPLMObjectRef.

        Path is virtual path, which may have zero or more matching file paths
        in library. Each file path is converted to XPLMObjectRef, and resulting
        tuple returned. Note that objects which fail to load will be returned
        as None within the tuple rather than XPLMObjectRef. Remember to unload
        each object when finished to avoid leaks.
    """
    ...


def getFMODStudio() -> FMOD_STUDIO_SYSTEM:
    """
        Get PyCapsule to FMOD_STUDIO_SYSTEM, allowing you to load/process whatever
        else you need. You will need to use python ctypes to access. See
        documentation.
    """
    ...


def getFMODChannelGroup(audioType: XPLMAudioBus) -> FMOD_CHANNELGROUP:
    """
        Returns PyCapsule to the FMOD_CHANNELGROUP with the given index.
        You will need to use python ctypes to access. See documentation.
    """
    ...


def playPCMOnBus(audioBuffer: Any, bufferSize: int, soundFormat: int, freqHz: int, numChannels: int, loop: int = 0, audioType: XPLMAudioBus = AudioUI, callback: Optional[Callable[[Any, int], None]] = None, refCon: Any = None) -> None | FMOD_CHANNEL:
    """
        Play provided data, of length bufferSize on the bus indicated by audioType. On
        completion, or stoppage, invoke (optional) callback with provided refCon.
         * soundFormat is # bytes per frame 1=8bit, 2=16bit, etc.
         * freqHz is sample framerate, e.g., 800, 22000, 44100
         * numChannels is e.g., 1=mono, 2=stereo

        Return audio FMOD_CHANNEL capsule on success.
    """
    ...


def stopAudio(channel: FMOD_CHANNEL) -> int:
    """
        Stop playing an active FMOD channel. If you defined a completion callback,
        this will be called. After this, the FMOD::Channel* will no longer be valid
        and must not be used in any future calls

        Returns FMOD_RESULT, 0= FMOD_OK
    """
    ...


def setAudioPosition(channel: FMOD_CHANNEL, position: Sequence[float], velocity: Optional[Sequence[float]] = None) -> int:
    """
        For audio channel, set position (for panning and attenuation)
        and velocity (for use with doppler).
         * position is OpenGL position (x, y, z) -- list of three float.
         * velocity is (meters/second) change in each (x, y, z) direction. None = (0, 0, 0)
    """
    ...


def setAudioFadeDistance(channel: FMOD_CHANNEL, min_distance: float = 1.0, max_distance: float = 10000.0) -> int:
    """
        Sets minimum and maximum distance for the channel.
        When the listener is in-between the minimum distance and the source, the volume
        will be at it's maximum. As the listener moves from the minimum distance to the
        maximum distance, the sound will attenuate. When outside the maximum distance
        the sound will no longer attenuate.

        Use minimum distance to give the impression that the sound is loud or soft: Small
        quiet objects such as a bumblebee, set minimum to 0.1. This would cause it to
        attenuate quickly and disappear when only a few meters away. A jumbo jet minimum
        might be 100 meters, thereby maintaining maximum volume until 100 meters away, with
        fade out over the next hundred meters.

        Maximum distance is effectively obsolete unless you need the sound to stop fading
        at a certain point. Do not adjust this from the default if you don't need to. Do
        not confuse maximum distance as the point where the sound will fade to zero, this
        is not the case.
    """
    ...


def setAudioVolume(channel: FMOD_CHANNEL, volume: float = 1.0) -> int:
    """
        Set the current volume of an active FMOD channel. This should be used to
        handle changes in the audio source volume, not for fading with distance.
        Values from 0.0 to 1.0 are normal, above 1 can be used to artificially amplify
        a sound.
    """
    ...


def setAudioPitch(channel: FMOD_CHANNEL, pitch: float = 1.0) -> int:
    """
        Change the current pitch of an active FMOD channel.
        This is a multiplier to the original channel value *not* a new frequency,
        so '0.8' lowers the pitch, '1.2' raises the pitch, and '1.0' sets the
        pitch to same as initialized.
    """
    ...


def setAudioCone(channel: FMOD_CHANNEL, inside_angle: float = 360.0, outside_angle: float = 360.0, outside_volume: float = 1.0, orientation: Optional[tuple[float, float, float]] = None) -> int:
    """
        Set a direction code for an active FMOD channel. The orientation vector is in local coordinates.
        This will set the sound to 3D if it is not already.
         * inside_angle: degrees, within this angle, sound is at normal volume.
         * outside_angle: degrees, outside of this angle, sound is at outside volume.
         * outside_volume: sound volume when greater than outside angle, 0 to 1.0 default=1.0
         * Orientation of sound: OpenGL vector (x, y, z). None == (0,0,-1) which is due North
    """
    ...


def speakString(string: str) -> None:
    """
        Display string in translucent overlay and speak string
    """
    ...


def returnString(string: str) -> str:
    """
        Copy string into the host-managed return slot for the current callback.
        This is the only sanctioned way for a `const char *` callback to hand a
        string back to X-Plane.
    """
    ...


def getVirtualKeyDescription(vKey: int) -> str:
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


def extractFileAndPath(fullPath: str) -> tuple[str, str]:
    """
        Given a full path, separate path from file

        Don't use this, use os.path routines instead
    """
    ...


def getDirectoryContents(dir: str, firstReturn: int = 0, bufSize: int = 2048, maxFiles: int = 100) -> tuple[int, list[str], int]:
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


def debugString(string: str) -> None:
    """
        Write string to 'Log.txt' file, with immediate buffer flush

        Use xp.systemLog() instead, to add newline and prefix with your
        plugin's name instead. Use xp.log() to write to XPPython3Log.txt file
    """
    ...


def setErrorCallback(callback: Callable[[str], None]) -> None:
    """
        Install error-reporting callback for your plugin

        Likely not useful for python debugging.
    """
    ...


def findSymbol(symbol: str) -> int:
    """
        Find C-API symbol. See documentation.
    """
    ...


def loadDataFile(fileType: XPLMDataFileType, path: str) -> int:
    """
        Load data file given by path

        fileType is:
          DataFile_Situation   = 1
          DataFile_ReplayMovie = 2
        Path is either absolute or relative X-Plane root.
        Returns 1 on success (file found), 0 otherwise.
    """
    ...


def saveDataFile(fileType: XPLMDataFileType, path: str) -> int:
    """
        Saves data file to disk.

        fileType is:
          DataFile_Situation   = 1
          DataFile_ReplayMovie = 2
        Path is either absolute or relative X-Plane root.
        Returns 1 on success (file found), 0 otherwise.
    """
    ...


def findCommand(name: str) -> XPLMCommandRef:
    """
        Return commandRef for named command or None
    """
    ...


def commandBegin(commandRef: XPLMCommandRef) -> None:
    """
        Start execution of command specified by commandRef
    """
    ...


def commandEnd(commandRef: XPLMCommandRef) -> None:
    """
        Ends execution of command specified by commandRef
    """
    ...


def commandOnce(commandRef: XPLMCommandRef) -> None:
    """
        Executes given commandRef, doing both CommandBegin and CommandEnd
    """
    ...


def createCommand(name: str, description: Optional[str] = None) -> XPLMCommandRef:
    """
        Create a named command: You'll still need to registerCommandHandler()
    """
    ...


def registerCommandHandler(commandRef: XPLMCommandRef, callback: Callable[[XPLMCommandRef, XPLMCommandPhase, Any], int], before: int = 1, refCon: Any = None) -> None:
    """
        Register a callback for given commandRef

        command callback is (commandRef, phase, refCon) and should return 0
           to halt processing, or 1 to let X-Plane continue with other callbacks.
           phase indicates current phase of command execution 0=Begin, 1=Continue, 2=End.
        before indicates you want to be called prior to X-Plane handling the command.
    """
    ...


def unregisterCommandHandler(commandRef: XPLMCommandRef, callback: Callable[[XPLMCommandRef, XPLMCommandPhase, Any], int], before: int = 1, refCon: Any = None) -> None:
    """
        Unregister commandRef. Parameters must match those provided with registerCommandHandler()
    """
    ...


def getCommandCallbackDict() -> dict:
    """
        Returns copy of internal CommandCallbackInfo dictionary.

        Internal debugging aid: contents and structure may change between releases.
    """
    ...


def getErrorCallbackDict() -> dict:
    """
        Returns copy of internal ErrorCallbackInfo dictionary.

        Internal debugging aid: contents and structure may change between releases.
    """
    ...


def getMETARForAirport(airport_id: str) -> str:
    """
        Returns 'last known' METAR report for given airport.

        Note the actual weather may have evolved significantly since
        the last downloaded METAR.
    """
    ...


def getWeatherAtLocation(latitude: float, longitude: float, altitude_m: float) -> None | XPLMWeatherInfo_t:
    """
        Returns current weather conditions at given location.
        Note this appears to work world-wide, though accuracy suffers with distance from current
        position.

        Returns WeatherInfo object on success, None otherwise. This call
        is not intended to be used per-frame.
    """
    ...


def beginWeatherUpdate() -> None:
    """
        Inform the simulator that you are starting a batch update of weather information.
        You must call endWeatherUpdate() prior to exiting your callback in order for
        changes to be recorded.
    """
    ...


def endWeatherUpdate(isIncremental: int = 1, updateImmediately: int = 0) -> None:
    """
        Inform the simulator that you are ending a batch update of weather information.
        Incremental updates add to any previous weather updates you've provided, otherwise
        previous update passed by plugin are ignored. Immediate updates may cause a sudden
        jump in the weather: otherwise weather is transitioned to new data.
    """
    ...


def setWeatherAtLocation(latitude: float, longitude: float, ground_altitude_msl: float, info: XPLMWeatherInfo_t) -> None:
    """
        Set the current weather conditions at given location. See documentation
        for information on use of fields in XPLMWeatherInfo_t.

        Call beginWeatherUpdate() before setting weather, and endWeatherUpdate() after.
    """
    ...


def setWeatherAtAirport(airport_id: str, info: XPLMWeatherInfo_t) -> None:
    """
        Set the current weather conditions at given airport. See documentation
        for information on use of fields in XPLMWeatherInfo_t.

        Call beginWeatherUpdate() before setting weather, and endWeatherUpdate() after.
    """
    ...


def eraseWeatherAtLocation(latitude: float, longitude: float) -> None:
    """
        Erase plugin-provided current weather conditions at given location.

        Call beginWeatherUpdate() before erasing weather, and endWeatherUpdate() after.
    """
    ...


def eraseWeatherAtAirport(airport_id: str) -> None:
    """
        Erase plugin-provided current weather conditions at given airport.

        Call beginWeatherUpdate() before erasing weather, and endWeatherUpdate() after.
    """
    ...


def reloadPlugin(signature: str) -> None:
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


def log(s: Optional[str] = None) -> None:
    """
        Log string to XPPython3log.txt file. Flush buffer, if no string is provided.
    """
    ...


def systemLog(s: Optional[str] = None) -> None:
    """
        Log string to system log file, Log.txt, with newline appended, flushing buffer.
    """
    ...


def getCapsuleDict() -> dict:
    """
        Returns internal dictionary of Capsules

        Intended for debugging only
    """
    ...


def getCapsulePtr(capsule_type: str, capsule: Any) -> int:
    """
        Dereference a capsule to retrieve internal C language pointer

        Intended for debugging only
    """
    ...


def getSelfModuleName() -> str:
    """
        Return module name of currently executing python plugin.
    """
    ...


def getPluginDict() -> dict:
    """
        Returns internal dictionary of loaded plugins

        Intended for debugging only
    """
    ...


def disablePythonPlugin(signature: str) -> None:
    """
        Disables python plugin
    """
    ...


def drawWindow(left: int, bottom: int, right: int, top: int, style: XPWindowStyle = Window_MainWindow) -> None:
    """
        Draw window at location
    """
    ...


def getWindowDefaultDimensions(style: XPWindowStyle = Window_MainWindow) -> tuple[int, int]:
    """
        Default dimension for indicated style

        Returns (width, height)
    """
    ...


def drawElement(left: int, bottom: int, right: int, top: int, style: XPElementStyle, lit: int = 0) -> None:
    """
        Draw element, possibly lit, at location.
    """
    ...


def getElementDefaultDimensions(style: XPElementStyle) -> tuple[int, int, int]:
    """
        Default dimension for indicated element

        Returns (width, height, canBeLit)
    """
    ...


def drawTrack(left: int, bottom: int, right: int, top: int, minValue: int, maxValue: int, value: int, style: XPTrackStyle, lit: int = 0) -> None:
    """
        Draw track at location, with min/max values and current value.

        Track may be 'reversed' if minValue > maxValue.
        Styles are:
          Track_Scrollbar=0
          Track_Slider   =1
          Track_Progress =2
    """
    ...


def getTrackDefaultDimensions(style: XPTrackStyle) -> tuple[int, int]:
    """
        Default dimension for indicated track style

        Returns (width, canBeLit)
    """
    ...


def getTrackMetrics(left: int, bottom: int, right: int, top: int, minValue: int, maxValue: int, value: int, style: XPTrackStyle) -> TrackMetrics:
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


def PI_GetMouseState(param: Any) -> Any:
    """
        Python2 backward compatibility only: returns its argument unchanged.

        Not required for XPPython3 -- widget callbacks already receive mouse state.
    """
    ...


def PI_GetKeyState(param: Any) -> Any:
    """
        Python2 backward compatibility only: returns its argument unchanged.

        Not required for XPPython3 -- widget callbacks already receive key state.
    """
    ...


def createWidgets(widgetDefs: Sequence[Sequence[Any]], parentID: Optional[XPWidgetID]) -> None | list[XPWidgetID]:
    """
        This does not work in X-Plane.
    """
    ...


def moveWidgetBy(widgetID: XPWidgetID, dx: int = 0, dy: int = 0) -> None:
    """
        Move widget by amount. +x = right, +y = up
    """
    ...


def fixedLayout(message: XPWidgetMessage, widgetID: XPWidgetID, param1: int, param2: int) -> int:
    """
        Use this as a widgetCallback to have child widgets maintain relative positions

        Seems to be completely useless with X-Plane 11.55+
    """
    ...


def selectIfNeeded(message: XPWidgetMessage, widgetID: XPWidgetID, param1: int, param2: int, eatClick: int = 1) -> int:
    """
        Call within widget Callback to raise widget, if not already

        Seems completely useless with X-Plane 11.55+
    """
    ...


def defocusKeyboard(message: XPWidgetMessage, widgetID: XPWidgetID, param1: int, param2: int, eatClick: int = 1) -> int:
    """
        Send keyboard focus back to X-Plane

        Seems completely useless in X-Plane 11.55
    """
    ...


def dragWidget(message: XPWidgetMessage, widgetID: XPWidgetID, param1: int, param2: int, left: int, top: int, right: int, bottom: int) -> int:
    """
        Add to your widget callback to support drag areas

        (left, top, right, bottom) define area within your widget where, if drag
        is initiated, this callback will cause the widget to move.
    """
    ...


def createWidget(left: int, top: int, right: int, bottom: int, visible: int, descriptor: str, isRoot: int, container: XPWidgetID | Literal[0], widgetClass: XPWidgetClass) -> XPWidgetID:
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


def createCustomWidget(left: int, top: int, right: int, bottom: int, visible: int, descriptor: str, isRoot: int, container: Optional[XPWidgetID], callback: Callable[[XPWidgetMessage, XPWidgetID, int, int], int]) -> XPWidgetID:
    """
        Create widget at location, with custom callback

        callback is (message, widget, param1, param2) returning 1 if you've handled
           the message, 0 otherwise.
        Returns created widgetID
    """
    ...


def destroyWidget(widgetID: XPWidgetID, destroyChildren: int = 1) -> None:
    """
        Destroys widgetID and (optionally) all children.
    """
    ...


def sendMessageToWidget(widgetID: XPWidgetID, message: XPWidgetMessage | int, dispatchMode: XPDispatchMode = Mode_UpChain, param1: int = 0, param2: int = 0) -> int:
    """
        dispatchMode default is Mode_UpChain
    """
    ...


def placeWidgetWithin(widgetID: XPWidgetID, container: XPWidgetID | Literal[0] = 0) -> None:
    """
        Change container widget for widgetID to container (widgetID)
    """
    ...


def countChildWidgets(widgetID: XPWidgetID) -> int:
    """
        Return number of child widgets for this widgetID
    """
    ...


def getNthChildWidget(widgetID: XPWidgetID, index: int) -> XPWidgetID:
    """
        Return widgetID of 0-based nth child
    """
    ...


def getParentWidget(widgetID: XPWidgetID) -> XPWidgetID:
    """
        Return widgetID for parent (i.e., container) of this widgetID
    """
    ...


def showWidget(widgetID: XPWidgetID) -> None:
    """
        Make widget visible.
    """
    ...


def hideWidget(widgetID: XPWidgetID) -> None:
    """
        Hide widget
    """
    ...


def isWidgetVisible(widgetID: XPWidgetID) -> int:
    """
        Return 1 if widget is visible

        Widget must be itself visible and contained in visible parent.
        Note if widget is outside of parent's geometry it may be clipped
        being reported 'visible' yet still not seen by user.
    """
    ...


def findRootWidget(widgetID: XPWidgetID) -> XPWidgetID:
    """
        Return top-most widget container for given widgetID

        If widget is root widget, it will return itself.
    """
    ...


def bringRootWidgetToFront(widgetID: XPWidgetID) -> None:
    """
        Make whole widget hierarchy containing widgetID to the front
    """
    ...


def isWidgetInFront(widgetID: XPWidgetID) -> int:
    """
        Return 1 if widget's hierarchy is front most.
    """
    ...


def getWidgetGeometry(widgetID: XPWidgetID) -> tuple[int, int, int, int]:
    """
        Return bounding box (left, top, right, bottom) of widgetID
    """
    ...


def setWidgetGeometry(widgetID: XPWidgetID, left: int, top: int, right: int, bottom: int) -> None:
    """
        Set bounding box for widgetID
    """
    ...


def getWidgetForLocation(container: XPWidgetID, xOffset: int, yOffset: int, recursive: int = 1, visibleOnly: int = 1) -> XPWidgetID:
    """
        Return widgetID of the child widget within the container widget at offset

        offsets are global coordinates, not relative bounding box of container.
        recursive=1 indicates find 'deepest' child widget
        visibleOnly=1 indicates only visible widgets are considered
    """
    ...


def getWidgetExposedGeometry(widgetID: XPWidgetID) -> tuple[int, int, int, int]:
    """
        Return (left, top, right, bottom) of widget's exposed geometry
    """
    ...


def setWidgetDescriptor(widgetID: XPWidgetID, descriptor: str) -> None:
    """
        Set widget's descriptor string
    """
    ...


def getWidgetDescriptor(widgetID: XPWidgetID) -> str:
    """
        Returns widget's descriptor string
    """
    ...


def getWidgetUnderlyingWindow(widgetID: XPWidgetID) -> XPLMWindowID:
    """
        Return windowID of window underlying widget
    """
    ...


def setWidgetProperty(widgetID: XPWidgetID, propertyID: XPWidgetPropertyID | int, value: Optional[Any]) -> None:
    """
        Set widget property to value
    """
    ...


def getWidgetProperty(widgetID: XPWidgetID, propertyID: XPWidgetPropertyID | int, exists: int = -1) -> Any:
    """
        Returns widget's property value. For XP properties, it is a long, for
        User-defined properties, it can be any python object.

        Raise ValueError if exists=-1 and property does not exist / has not been set
        Set exists=None if you don't care if property exists (value will be 0)
        Set exists to a list object, and we'll set it to [1,] if property exists,
          [0, ] otherwise
    """
    ...


def setKeyboardFocus(widgetID: XPWidgetID) -> int:
    """
        Set keyboard focus to widgetID
    """
    ...


def loseKeyboardFocus(widgetID: XPWidgetID) -> None:
    """
        Cause widgetID to lose keyboard focus
    """
    ...


def getWidgetWithFocus() -> XPWidgetID:
    """
        Return widgetID with current focus. 0=X-Plane has focus.
    """
    ...


def addWidgetCallback(widgetID: XPWidgetID, callback: Callable[[XPWidgetMessage, XPWidgetID, int, int], int]) -> None:
    """
        Add callback to widgetID

        Callback has signature (message, widgetID, param1, param2). See
        createCustomWidget()
    """
    ...


def getWidgetClassFunc(widgetID: XPWidgetClass) -> int:
    """
        Given widgetClass, return underlying function.

        Not useful with python. Use addWidgetCalback() instead.
    """
    ...


def getWidgetCallbackDict() -> dict[XPWidgetID, tuple[str, list]]:
    """
        Returns dictionary of widget callbacks.

        Keys are XPWidgetID capsules, values are tuples of (module_name, [callbacks])
    """
    ...


def getWidgetPropertiesDict() -> dict[XPWidgetID, dict[int, Any]]:
    """
        Returns dictionary of widget properties.

        Keys are XPWidgetID capsules, values are dicts of {propertyID: value}
    """
    ...


# Hand-maintained stub fragment for names defined in xp.py itself.
#
# generate_xp_pyi.py builds xp.pyi from the XP*.pyi C-module stubs, which know
# nothing about the helpers xp.py adds on top of them.  The generator hoists the
# imports below into xp.pyi's header and appends the rest of this file, so
# anything declared here survives a regeneration.  Add a declaration here
# whenever you add a module-level name to xp.py; the generator warns about any
# xp.py name that neither source covers.


MsgEnteredVR: int
MsgExitingVR: int
Airport: int

pythonLog: Callable[..., None]
sys_log: Callable[..., None]


def playWaveOnBus(wav: Any, loop: int, audioType: XPLMAudioBus,
                  callback: Optional[Callable[[Any, int], None]] = None,
                  refCon: Any = None) -> None | FMOD_CHANNEL:
    """
    Play an open wave file (wave.Wave_read) on the given audio bus.
    """
    ...


class PluginItem:
    """Attributes of one loaded plugin, looked up by instance or by name."""
    instance: Any
    module_name: str
    name: str
    signature: str
    description: str
    disabled: int
    def __init__(self, key: Any) -> None: ...


def getPluginInstance(signature: str = None) -> Any:
    """
    Return the PythonInterface instance for signature, or for the calling
    plugin when signature is None.
    """
    ...


class Retained:
    """Holds a retained-drawing handle; empty until the with-block exits."""
    handle: Optional[XPLMRetainedDrawing]
    def __init__(self) -> None: ...
    def draw(self) -> None: ...
    def destroy(self) -> None: ...


def weatherUpdateContext(isIncremental: int = 1,
                         updateImmediately: int = 0) -> AbstractContextManager[None]:
    """
    Context manager wrapping beginWeatherUpdate() / endWeatherUpdate().
    """
    ...


def transformContext() -> AbstractContextManager[None]:
    """
    Context manager wrapping transformPush() / transformPop().
    """
    ...


def scissorContext() -> AbstractContextManager[None]:
    """
    Context manager wrapping scissorPush() / scissorPop().
    """
    ...


def setupStencilMask(bits: int, mask: int) -> AbstractContextManager[None]:
    """
    Context manager wrapping beginSetupStencilMask() / endSetupStencilMask().
    """
    ...


def retainedDrawing() -> AbstractContextManager[Retained]:
    """
    Context manager wrapping beginRetainedDrawing() / endRetainedDrawing().
    Yields a Retained instance whose handle is set when the block exits.
    """
    ...
