import os
from XPPython3 import xp
from XPPython3 import samples

class PythonInterface:
    def __init__(self):
        # If this file exists, we'll do nothing.
        self.touch_file = os.path.normpath(os.path.join(xp.getSystemPath(), xp.INTERNALPLUGINSPATH, '.firstTimeComplete'))

    def XPluginStart(self):
        return 'FirstTime', 'xppython3.firstTime', 'Performs tasks which should be run "first time" for XPPython3 plugin'

    def XPluginEnable(self):
        # Register flight loop, but set to zero to disable (this way, we know it exists and
        # can unconditionally unregister it in XPluginDisable). "Enable" the flight loop, only if touchfile does not exist.
        xp.registerFlightLoopCallback(self.flightLoopCallback, 0, self.touch_file)
        if os.path.exists(self.touch_file):
            return 0
        xp.setFlightLoopCallbackInterval(self.flightLoopCallback, -1, 1, self.touch_file)
        return 1

    def flightLoopCallback(self, sinceLastCall, sinceLastFlightLoop, counter, refcon):
        # we do work within a flight loop so we can display progress / results
        # in a window. The flight loop callback is not enabled & nothing is done if the touch_file exists.
        self.firstTime(forceCopySamples=True)
        with open(self.touch_file, 'w') as fp:
            fp.write(' ')
        # execute only once
        return 0

    def XPluginReceiveMessage(self, inFromWho, inMessage, inParam):
        return

    def XPluginDisable(self):
        xp.unregisterFlightLoopCallback(self.flightLoopCallback, self.touch_file)

    def XPluginStop(self):
        pass

    def firstTime(self, forceCopySamples=False):
        # 1) Create PythonPlugins if not already exists
        pluginsPath = os.path.normpath(os.path.join(xp.getSystemPath(), xp.PLUGINSPATH))
        if not os.path.exists(pluginsPath):
            xp.log("Making directory for 3rd party python plugins: {}".format(pluginsPath))
            os.makedirs(pluginsPath)

        samples.download()
        return
