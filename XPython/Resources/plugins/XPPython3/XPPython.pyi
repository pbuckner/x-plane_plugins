from dataclasses import dataclass
from typing import Any, Callable, Generic, Optional, Type, TypeVar, NewType, Literal, Sequence
from XPPython3.xp_typing import *    
VERSION: str
PLUGINSPATH: str
INTERNALPLUGINSPATH: str
pythonDebugLevel: int
pythonExecutable: str
ModuleMTimes: object
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

def getCapsuleDict() -> dict:
    """
    Returns internal dictionary of Capsules

    Intended for debugging only
    """
    ...

def getCapsulePtr(capsule_type:str, capsule:Any) -> int:
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

