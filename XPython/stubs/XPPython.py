from typing import Dict, Union

VERSION = 'x.x.x'
PLUGINSPATH = 'Resources/plugins/PythonPlugins'
INTERNALPLUGINSPATH = 'Resources/plugins/XPPython3'
pythonExecutable = '/path/to/python'
pythonDebugLevel = 0


def pythonGetDicts() -> Dict:
    return dict()


def pythonGetCapsules() -> Dict:
    return dict()


def getPluginStats() -> Dict:
    return dict()


def systemLog(s: str) -> None:
    return


def log(s: Union[None, str] = None) -> None:
    return


def derefCapsule(capsule_type: str, capsule: object) -> int:
    return int()


sys_log = systemLog
pythonLog = log

class HotKeyInfo: pass
class ProbeInfo: pass
class DataRefInfo: pass
class WeatherInfo: pass
class WeatherInfoClouds: pass
class WeatherInfoWinds: pass
class PluginInfo: pass
class NavAidInfo: pass
class FMSEntryInfo: pass
class TrackMetrics: pass
class ModuleMTimes: pass
