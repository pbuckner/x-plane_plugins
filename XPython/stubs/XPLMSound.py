from XPPython3.xp import XPLMAudioBus, FMOD_STUDIO_SYSTEM, FMOD_CHANNELGROUP
from typing import Union, Callable, Any, Tuple

AudioRadioCom1 = 0
AudioRadioCom2 = 1
AudioRadioPilot = 2
AudioRadioCopilot = 3
AudioExteriorAircraft = 4
AudioExteriorEnvironment = 5
AudioExteriorUnprocessed = 6
AudioInterior = 7
AudioUI = 8
AudioGround = 9
Master = 10
MasterBank = 0
RadioBank = 1
FMOD_OK = 0
FMOD_SOUND_FORMAT_PCM16 = 2

def playPCMOnBus(audioBuffer: XPLMAudioBus,
                 bufferSize: int,
                 soundFormat: int,
                 freqHz: int,
                 numChannels: int,
                 loop: int,
                 audioType: int,
                 callback: Union[None, Callable[[Any, int], None]] = None,
                 refCon: Any = None) -> int:
    return int()
    
def stopAudio(channel: int) -> int:
    return int()

def setAudioPosition(channel: int,
                     position: Tuple[int, int, int],
                     velocity: Union[Tuple[float, float, float], None] = None) -> int:
    return int()

def setAudioFadeDistance(channel: int,
                         min_distance: float = 1.0,
                         max_distance: float = 10000.0) -> int:
    return int()

def setAudioVolume(channel: int,
                   volume: float = 1.0) -> int:
    return int()

def setAudioPitch(channel: int,
                  pitch: float = 1.0) -> int:
    return int()

def setAudioCone(channel: int,
                 inside_angle: float = 360.0,
                 outside_angle: float = 360.0,
                 outside_volume: float = 1.0,
                 orientation: Union[Tuple[int, int, int], None] = None) -> int:
    return int()

def getFMODStudio() -> FMOD_STUDIO_SYSTEM:
    return FMOD_STUDIO_SYSTEM(object)

def getFMODChannelGroup(audioType: int) -> FMOD_CHANNELGROUP:
    return FMOD_CHANNELGROUP(object)
