from typing import Optional
VERSION = 'x.x.x'
PLUGINSPATH = 'Resources/plugins/PythonPlugins'
INTERNALPLUGINSPATH = 'Resources/plugins/XPPython3'
pythonExecutable = '/path/to/python'


def XPPythonGetDicts():
    return {}


def XPPythonGetCapsules():
    return {}


def XPGetPluginStats() -> {}:
    """
    Returns dict of plugins. For each, a dict of performance statistics:
     customw: microseconds spent in custom widget execution
     draw:    microseconds spent in drawing callbacks
     fl:      microseconds spent in fligh loop callbacks
    Key "None" returns total value spent by XPPython3 plugin in these area.

    NOTE: Side effect of call this routine, we'll also zero-out stat values.
    Recommended usage is to call this routine in your flightloop and divide
    retrieved values by how often your flight loop is a called (that is, if
    you call XPGetPluginStats() once every 10 seconds, divide value by 10
    to get microseconds per second.)
      
    {None: {'customw': int,
            'draw': int,
            'fl': int},
     'plugina': {'customw': int,
                 'draw': int,
                 'fl': int},
     ... 
    }
    """
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

