from dataclasses import dataclass
from typing import Any, Callable, Generic, Optional, Type, TypeVar, NewType, Literal, Sequence
from XPPython3.xp_typing import *    
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

