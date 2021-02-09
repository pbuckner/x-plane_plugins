from typing import Optional
VERSION = 'x.x.x'
PLUGINSPATH = 'Resources/plugins/PythonPlugins'
INTERNALPLUGINSPATH = 'Resources/plugins/XPPython3'
pythonExecutable = '/path/to/python'


def XPPythonGetDicts():
    return {}


def XPPythonGetCapsules():
    return {}


def XPSystemLog(s: str) -> None:
    """
    Print string to X-Plane Log.txt (automatically append a single \n)
    Will fflush() after each write.
    """


def XPPythonLog(s: Optional[str] = None) -> None:
    """
    Print string to X-Plane XPPython3.log (automatically append a single \n)
    Will _not_ fflush after each write. To fflush(), simply call with zero-length
    string: XPPythonLog('')
    """
