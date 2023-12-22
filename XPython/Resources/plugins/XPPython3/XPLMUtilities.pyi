from dataclasses import dataclass
from typing import Any, Callable, Generic, Optional, Type, TypeVar, NewType, Literal, Sequence
from XPPython3.xp_typing import *    
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

