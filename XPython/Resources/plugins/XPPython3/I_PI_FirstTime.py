import shutil
import os
from XPPython3 import xp
from XPPython3.utils import samples, xp_pip
from XPPython3.scriptupdate import Version, VersionUnknownException

PLUGIN_DISABLED = 5


class PythonInterface:
    def __init__(self):
        # touch_file stores most recently _executed_ version, which may differ from newly installed version
        self.touch_file = os.path.normpath(os.path.join(xp.getSystemPath(), xp.INTERNALPLUGINSPATH, '.firstTimeComplete'))
        self.version_file = os.path.normpath(os.path.join(xp.getSystemPath(), xp.INTERNALPLUGINSPATH, 'version.txt'))
        self.flID = None
        self.current_version = None

    def XPluginStart(self):
        return 'FirstTime', 'xppython3.firstTime', 'Performs tasks which should be run "first time" for XPPython3 plugin'

    def XPluginEnable(self):
        self.current_version = Version(xp.VERSION)
        try:
            touch_version = get_version(self.touch_file)
        except VersionUnknownException:
            touch_version = Version('0.0')

        # Check (and update if necessary) required modules
        requirements = ['numpy>=1.26.4', 'freetype-py>=2.4.0', 'pillow>=10.3.0', 'charset-normalizer>=3.4.1']
        xp_pip.load_requirements(requirements, force=False)

        if self.current_version > touch_version:
            self.flID = xp.createFlightLoop(self.flightLoopCallback)
            xp.log(f"Current is {self.current_version} and touchfile is {touch_version}")
            xp.scheduleFlightLoop(self.flID, -1)
            return 1

        return 0

    def flightLoopCallback(self, _sinceLastCall, _sinceLastFlightLoop, _counter, _refcon):
        # we do work within a flight loop so we can display progress / results
        # in a window. The flight loop callback is not enabled & nothing is done if the touch_file exists.

        self.firstTime()
        with open(self.touch_file, 'w', encoding="UTF-8") as fp:
            fp.write(f'{self.current_version}\n')
        # execute only once
        self.disablePlugin()
        return 0

    def XPluginDisable(self):
        xp.destroyFlightLoop(self.flID)

    def firstTime(self):
        # 1) Create PythonPlugins if not already exists
        pluginsPath = os.path.normpath(os.path.join(xp.getSystemPath(), xp.PLUGINSPATH))
        if not os.path.exists(pluginsPath):
            xp.log(f"Making directory for 3rd party python plugins: {pluginsPath}")
            os.makedirs(pluginsPath)

        # 2) Download current set of samples
        samples.download()

        # 3) Delete local XPPython3/imgui directory, if exists (was used in older releases)
        imgui_dir = os.path.normpath(os.path.join(xp.getSystemPath(), xp.INTERNALPLUGINSPATH, "imgui"))
        if os.path.exists(imgui_dir):
            xp.log("Found, and removing old XPPython3/imgui directory")
            shutil.rmtree(imgui_dir)

        # 4) add other modules
        requirements = []
        xp_pip.load_requirements(requirements)

        return

    def disablePlugin(self):
        # Mark this python plugin disabled:
        # This will cause it to not be included in the XPPython3 performance window.
        # and this plugin will not be reloaded & will not receive XP messages
        #
        # Normally, you'd disable a plugin by returning '0' in XPluginEnable.
        # Here, we disable the plugin _after_ it's done it's stuff. "Disabled"
        # is kind of a misnomer therefore. By disabling it "late", we stop
        # it from receiving futher messages and prevent it from appearing in
        # the 'performance' popup window.
        xp.pythonGetDicts()['plugins'][self][PLUGIN_DISABLED] = True


def get_version(filename) -> Version:
    """
        return Version instance from contents of file (or Version(0.0))
    """
    try:
        with open(filename, "r", encoding="UTF-8") as fp:
            content = fp.read().replace('XPPython3', '')
            return Version(content)
    except (FileNotFoundError, PermissionError):
        return Version(None)
