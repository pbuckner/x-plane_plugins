from typing import Any, Callable
MSG_PLANE_CRASHED = 101
MSG_PLANE_LOADED = 102
MSG_AIRPORT_LOADED = 103
MSG_SCENERY_LOADED = 104
MSG_AIRPLANE_COUNT_CHANGED = 105
MSG_PLANE_UNLOADED = 106
MSG_WILL_WRITE_PREFS = 107
MSG_LIVERY_LOADED = 108
MSG_ENTERED_VR = 109
MSG_EXITING_VR = 110
MSG_RELEASE_PLANES = 111
MSG_FMOD_BANK_LOADED = 112
MSG_FMOD_BANK_UNLOADING = 113
MSG_DATAREFS_ADDED = 114


def getMyID() -> int:
    return int()  # XPLMPluginID


def countPlugins() -> int:
    return int()  # count


def getNthPlugin(index: int) -> int:
    return int()  # XPLMPluginID


def findPluginByPath(path: str) -> int:
    return int()  # XPLMPluginID


def findPluginBySignature(inSignature: str) -> int:
    return int()  # XPLMPluginID


class PluginInfo(object):
    name = None
    filePath = None
    signature = None
    description = None


def getPluginInfo(pluginID: int) -> PluginInfo:
    return PluginInfo()


def isPluginEnabled(pluginID: int) -> int:
    return int()  # 1=is enabled


def enablePlugin(pluginID: int) -> int:
    return int()  # 1=successfully enabled; 0=otherwise


def disablePlugin(pluginID: int) -> None:
    return


def reloadPlugins() -> None:
    return


def sendMessageToPlugin(pluginID: int, message: int, param: Any = None) -> None:
    return


def hasFeature(feature: str) -> int:
    return int()  # 1=feature is supported


def isFeatureEnabled(feature: str) -> int:
    return int()  # 1=feature is enabled


def enableFeature(feature: str, enable: int = 1) -> None:
    return


def XPLMFeatureEnumerator_f(name: str, refCon: Any) -> None:
    return


def enumerateFeatures(enumerator: Callable[[str, Any], None], refCon: Any = None) -> None:
    return
