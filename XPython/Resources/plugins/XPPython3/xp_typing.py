from dataclasses import dataclass
from typing import NewType
FMODChannel = NewType('FMODChannel', object)
FMOD_CHANNEL = NewType('FMOD_CHANNEL', int)
FMOD_CHANNELGROUP = NewType('FMOD_CHANNELGROUP', object)
FMOD_STUDIO_SYSTEM = NewType('FMOD_STUDIO_SYSTEM', object)
XPDispatchMode = NewType('XPDispatchMode', int)
XPElementStyle = NewType('XPElementStyle', int)
XPLMAudioBus = NewType('XPLMAudioBus', int)
XPLMAvionicsID = NewType('XPLMAvionicsID', int)
XPLMBankID = NewType('XPLMBankID', int)
XPLMCameraControlDuration = NewType('XPLMCameraControlDuration', int)
XPLMCommandPhase = NewType('XPLMCommandPhase', int)
XPLMCommandRef = NewType('XPLMCommandRef', int)
XPLMCursorStatus = NewType('XPLMCursorStatus', int)
XPLMDataFileType = NewType('XPLMDataFileType', int)
XPLMDataRef = NewType('XPLMDataRef', int)
XPLMDataTypeID = NewType('XPLMDataTypeID', int)
XPLMDeviceID = NewType('XPLMDeviceID', int)
XPLMDrawingPhase = NewType('XPLMDrawingPhase', int)
XPLMFlighLoopPhaseType = NewType('XPLMFlighLoopPhaseType', int)
XPLMFlightLoopID = NewType('XPLMFlightLoopID', int)
XPLMFlightLoopPhaseType = NewType('XPLMFlightLoopPhaseType', int)
XPLMFontID = NewType('XPLMFontID', int)
XPLMHostApplicationID = NewType('XPLMHostApplicationID', int)
XPLMHotKeyID = NewType('XPLMHotKeyID', int)
XPLMInstanceRef = NewType('XPLMInstanceRef', int)
XPLMKeyFlags = NewType('XPLMKeyFlags', int)
XPLMLanguageCode = NewType('XPLMLanguageCode', int)
XPLMMapLayerID = NewType('XPLMMapLayerID', int)
XPLMMapLayerType = NewType('XPLMMapLayerType', int)
XPLMMapOrientation = NewType('XPLMMapOrientation', int)
XPLMMapProjectionID = NewType('XPLMMapProjectionID', int)
XPLMMapStyle = NewType('XPLMMapStyle', int)
XPLMMenuCheck = NewType('XPLMMenuCheck', int)
XPLMMenuID = NewType('XPLMMenuID', int)
XPLMMouseStatus = NewType('XPLMMouseStatus', int)
XPLMNavRef = NewType('XPLMNavRef', int)
XPLMNavType = NewType('XPLMNavType', int)
XPLMObjectRef = NewType('XPLMObjectRef', int)
XPLMPluginID = NewType('XPLMPluginID', int)
XPLMProbeRef = NewType('XPLMProbeRef', int)
XPLMProbeResult = NewType('XPLMProbeResult', int)
XPLMProbeType = NewType('XPLMProbeType', int)
XPLMTextureID = NewType('XPLMTextureID', int)
XPLMWindowDecoration = NewType('XPLMWindowDecoration', int)
XPLMWindowID = NewType('XPLMWindowID', int)
XPLMWindowLayer = NewType('XPLMWindowLayer', int)
XPLMWindowPositioningMode = NewType('XPLMWindowPositioningMode', int)
XPTrackStyle = NewType('XPTrackStyle', int)
XPWidgetClass = NewType('XPWidgetClass', int)
XPWidgetID = NewType('XPWidgetID', int)
XPWidgetMessage = NewType('XPWidgetMessage', int)
XPWidgetPropertyID = NewType('XPWidgetPropertyID', int)
XPWindowStyle = NewType('XPWindowStyle', int)

@dataclass
class XPLMDataRefInfo_t:
    name: str
    type: int
    writable: bool
    owner: int
    
@dataclass
class XPLMDrawInfo_t:
    x: float
    y: float
    z: float
    pitch: float
    heading: float
    roll: float
    
@dataclass
class FMSEntryInfo:
    type: int
    navAidID: None | str
    ref: int
    altitude: int
    lat: float
    lon: float

@dataclass
class NavAidInfo:
    type: int
    latitude: float
    longitude: float
    height: float
    frequency: int
    heading: float
    navAidID: str
    name: str
    reg: int
    
    
@dataclass
class PluginInfo:
    name: str
    filePath: str
    signature: str
    description: str
    
@dataclass
class HotKeyInfo:
    virtualKey: int
    flags: int
    description: str
    pluing: int
    
@dataclass
class TrackMetrics:
    isVertical: int
    downBtnSize: int
    downPageSize: int
    thumbSize: int
    upPageSize: int
    upBtnSize: int

@dataclass
class XPLMWeatherInfoClouds_t:
    cloud_type: float
    coverage: float
    alt_type: float
    alt_base: float

@dataclass
class XPLMWeatherInfoWinds_t:
    alt_msl: float
    speed: float
    direction: float
    gust_speed: float
    shear: float
    turbulence: float
    
@dataclass
class XPLMWeatherInfo_t:
    detail_found: int
    temperature_alt: float
    dewpoint_alt: float
    pressure_alt: float
    precip_rate_alt: float
    wind_dir_alt: float
    wind_spd_alt: float
    turbulence_alt: float
    wave_height: float
    wave_length: float
    wave_dir: int
    wave_speed: float
    visibility: float
    precip_rate: float
    thermal_climb: float
    pressure_sl: float
    wind_layers: list[XPLMWeatherInfoWinds_t]
    cloud_layers: list[XPLMWeatherInfoClouds_t]
    
@dataclass
class XPLMProbeInfo_t:
    result:int
    locationX:float
    locationY:float
    locationZ:float
    normalX: float
    normalY: float
    normalZ: float
    velocityX: float
    velocityY: float
    velocityZ: float
    is_wet: int

    
