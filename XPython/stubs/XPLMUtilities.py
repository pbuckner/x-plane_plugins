from typing import Tuple, Sequence, Callable, Union, Any
from xp_hinting import XPLMCommandRef

Host_Unknown = 0
Host_XPlane = 1
Language_Unknown = 0
Language_English = 1
Language_French = 2
Language_German = 3
Language_Italian = 4
Language_Spanish = 5
Language_Korean = 6
Language_Russian = 7
Language_Greek = 8
Language_Japanese = 9
Language_Chinese = 10
DataFile_Situation = 1
DataFile_ReplayMovie = 2
CommandBegin = 0  # The command is being started.
CommandContinue = 1  # The command is continuing to execute.
CommandEnd = 2  # The command has ended.


def speakString(string: str) -> None:
    return


def getVirtualKeyDescription(vKey: int) -> str:
    return str()


def reloadScenery() -> None:
    return


def getSystemPath() -> str:
    return str()


def getPrefsPath() -> str:
    return str()


def getDirectorySeparator() -> str:
    return str()


def extractFileAndPath(fullPath: str) -> Tuple[str, str]:
    return str(), str()


def getDirectoryContents(dir: str, firstReturn: int = 0,
                         bufSize: int = 2048, maxFiles: int = 100) -> Tuple[int, Sequence[str], int]:
    return int(), list(), int()  # (1=all info returned, list of files, #files in directory)


def getVersions() -> Tuple[int, int, int]:
    return int(), int(), int()  # (outXPlaneVersion, outXPLMVersion, outHostID)


def getLanguage() -> int:
    return int()  # XPLMLanguageCode


def debugString(string: str) -> None:
    return


def setErrorCallback(callback: Callable[[str], None]) -> None:
    return


def findSymbol(string: str) -> int:
    return int()  # pointer to function on success


def loadDataFile(fileType: int, filePath: str) -> int:
    return int()  # 1 on success


def saveDataFile(fileType: int, filePath: str) -> int:
    return int()  # 1 on success


def findCommand(name: str) -> XPLMCommandRef:
    return XPLMCommandRef(0)


def commandBegin(commandRef: XPLMCommandRef) -> None:
    return


def commandEnd(commandRef: XPLMCommandRef) -> None:
    return


def commandOnce(commandRef: XPLMCommandRef) -> None:
    return


def createCommand(name: str, description: Union[None, str] = None) -> XPLMCommandRef:
    return XPLMCommandRef(0)


def registerCommandHandler(commandRef: XPLMCommandRef,
                           callback: Callable[[XPLMCommandRef, int, Any], int],
                           before: int = 1, refCon: Any = None) -> None:
    return


def unregisterCommandHandler(commandRef: XPLMCommandRef,
                             callback: Callable[[XPLMCommandRef, int, Any], int],
                             before: int = 1, refCon: Any = None) -> None:
    return
