from dataclasses import dataclass
from typing import Any, Callable, Generic, Optional, Type, TypeVar, NewType, Literal, Sequence
from XPPython3.xp_typing import *    
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

