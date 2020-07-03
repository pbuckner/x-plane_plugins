import sys
import XPPython
import scriptupdate
from XPLMMenus import XPLMCreateMenu, XPLMFindPluginsMenu, XPLMDestroyMenu, XPLMAppendMenuItem, XPLMCheckMenuItem, xplm_Menu_Checked, xplm_Menu_Unchecked, XPLMSetMenuItemName
from XPLMUtilities import XPLMRegisterCommandHandler, XPLMCreateCommand, xplm_CommandBegin, XPLMUnregisterCommandHandler, XPLMCommandOnce


class Config (scriptupdate.Updater):
    Name = "XPPython3 Updater"
    Sig = "com.avnwx.xppython3.updater.{}.{}".format(sys.version_info.major, sys.version_info.minor)
    Desc = "Automatic updater for XPPython3 plugin"
    Version = XPPython.VERSION
    VersionCheckURL = 'https://maps.avnwx.com/data/x-plane/versions.json'
    ConfigFilename = 'updater.pkl'
    defaults = {
        'autoUpgrade': False,
    }
    internal = True


class PythonInterface(Config):

    def __init__(self):
        self.menu = None
        self.updatePythonCmdRef = None
        super(PythonInterface, self).__init__()

    def XPluginStart(self):
        self.updatePythonCmdRef = XPLMCreateCommand('xppython3/update', 'Update XPPython3 Plugin')
        XPLMRegisterCommandHandler(self.updatePythonCmdRef, self.updatePython, 1, '')
        self.menu = XPLMCreateMenu('XPPython3 Updater', None, 0, self.menuHandler, 'updatePython')
        XPLMAppendMenuItem(self.menu, 'Update', None)

        XPLMCheckMenuItem(XPLMFindPluginsMenu(), 1, xplm_Menu_Checked if self.new_version else xplm_Menu_Unchecked)  # '1' because the main XPPython Menu is first...?
        XPLMSetMenuItemName(self.menu, 0, "Update to {}".format(self.new_version) if self.new_version else "{} is up-to-date".format(self.Version))

        return self.Name, self.Sig, self.Desc

    def menuHandler(self, menuRef, itemRef):
        XPLMCommandOnce(self.updatePythonCmdRef)

    def updatePython(self, inCommand, inPhase, inRefcon):
        if inPhase == xplm_CommandBegin:
            self.check(forceUpgrade=True)
            XPLMCheckMenuItem(XPLMFindPluginsMenu(), 1, xplm_Menu_Unchecked)  # '1' because the main XPPython Menu is first...?
            XPLMSetMenuItemName(self.menu, 0, "Will change to {} on restart.".format(self.new_version))
        return 0

    def XPluginStop(self):
        if self.menu:
            XPLMDestroyMenu(self.menu)
        if self.updatePythonCmdRef:
            XPLMUnregisterCommandHandler(self.updatePythonCmdRef, self.updatePython, 1, '')
        return

    def XPluginEnable(self):
        return 1

    def XPluginDisable(self):
        return

    def XPluginReceiveMessage(self, inFromWho, inMessage, inParam):
        return
