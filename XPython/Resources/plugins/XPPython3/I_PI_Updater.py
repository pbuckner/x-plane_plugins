from typing import List, Any
import sys
import urllib
import os
from XPPython3 import scriptupdate
from XPPython3 import xp
from XPPython3.utils import samples
from XPPython3.ui import popups
from XPPython3.updater import Pip, About, Performance, Preferences, Usage
from XPPython3.updater.version import calc_update
from XPPython3.xp_typing import PythonInterfaceType


class MyConfig(scriptupdate.Updater):
    Name = "XPPython3 Updater"
    Sig = f"xppython3.{xp.getVersions()[1]}.{sys.version_info.major}.{sys.version_info.minor}"
    Desc = "Automatic updater for XPPython3 plugin"
    Version = xp.VERSION
    VersionCheckData = {'product': urllib.parse.quote_plus(Sig),
                        'current': urllib.parse.quote_plus(Version),
                        'platform': sys.platform,
                        'xp': xp.getVersions()[0],
                        'sdk': xp.getVersions()[1]}
    VersionCheckURL = 'https://maps.avnwx.com/x-plane/versions.json'
    ConfigFilename = 'updater.pkl'
    defaults = {
        'autoUpgrade': False,
        'beta': False,
    }
    internal = True


class PythonInterface(MyConfig):

    def __init__(self: PythonInterfaceType):
        self.menu = None
        self.about = About(self)
        self.performance = Performance(self)
        self.preferences = Preferences(self)
        self.pip = Pip(self)
        self.status_idx = 0
        self.stats: dict = {}
        self.updateMenuIdx = None
        self.frame_rate_period_dref = xp.findDataRef('sim/time/framerate_period')
        self.need_to_keep_a_handle_to_this: Any = None  # need to hold onto window handle (see below)
        self.cmds: List = [
            {'command': 'xppython3/update',
             'description': 'Update XPPython3 Plugin',
             'callback': self.updatePython,
             'commandRef': None},
            {'command': 'xppython3/pip',
             'description': 'Toggle PIP window',
             'callback': self.pip.toggleCommand,
             'commandRef': None},
            {'command': 'xppython3/about',
             'description': 'Toggle XPPython3 About window',
             'callback': self.about.toggleCommand,
             'commandRef': None},
            {'command': 'xppython3/performance',
             'description': 'Toggle XPPython3 Performance window',
             'callback': self.performance.toggleCommand,
             'commandRef': None},
            {'command': 'xppython3/preferences',
             'description': 'Toggle XPPython3 Preferences window',
             'callback': self.preferences.toggleCommand,
             'commandRef': None},
        ]

        self.do_not_check_for_updates = False
        if self.preferences.preferences['debug']:
            xp.log("User has selected 'debug' mode, which blocks update checking")
            self.do_not_check_for_updates = True
        elif self.preferences.popup_preferences_dialog:
            xp.log("Preferences not set: defaulting to not check for updates and will popup preferences dialog.")
            self.do_not_check_for_updates = True
        elif not self.preferences.preferences['check_for_update']:
            xp.log("User preference is set to not check for updates")
            self.do_not_check_for_updates = True

        self.uuid = self.preferences.preferences['uuid'] if self.preferences.preferences['collect_xppython3_stats'] else ''
        super(PythonInterface, self).__init__()

    def XPluginStart(self):
        for cmd in self.cmds:
            cmd['commandRef'] = xp.createCommand(cmd['command'], cmd['description'])
            xp.registerCommandHandler(cmd['commandRef'], cmd['callback'], 1, '')

        xp.registerFlightLoopCallback(self.performance.fLCallback, 0, None)

        self.menu = xp.createMenu('XPPython3', handler=self.menuHandler)
        xp.appendMenuItemWithCommand(self.menu, 'About', self.cmds[2]['commandRef'])
        xp.appendMenuItemWithCommand(self.menu, 'Preferences', self.cmds[4]['commandRef'])
        # xp.appendMenuItemWithCommand(self.menu, 'Disable scripts', xp.findCommand('XPPython3/disableScripts'))
        # xp.appendMenuItemWithCommand(self.menu, 'Enable scripts', xp.findCommand('XPPython3/enableScripts'))
        xp.appendMenuSeparator(self.menu)
        xp.appendMenuItemWithCommand(self.menu, 'Performance', self.cmds[3]['commandRef'])
        xp.appendMenuItemWithCommand(self.menu, 'Update', self.cmds[0]['commandRef'])
        xp.appendMenuItem(self.menu, 'Download Samples', 'samples')
        self.updateMenuIdx = 4
        xp.appendMenuItemWithCommand(self.menu, 'Pip Package Installer', self.cmds[1]['commandRef'])
        xp.appendMenuSeparator(self.menu)
        xp.appendMenuItemWithCommand(self.menu, 'Reload scripts', xp.findCommand('XPPython3/reloadScripts'))
        self.setUpdateMenu()

        return self.Name, self.Sig, self.Desc

    def setUpdateMenu(self):
        try_beta = self.config['beta']
        current = self.Version
        beta_version = self.beta_version
        stable_version = self.new_version
        if not self.do_not_check_for_updates:
            uptodate, _version = calc_update(try_beta, current, stable_version, beta_version)
            xp.checkMenuItem(xp.findPluginsMenu(), 0, xp.Menu_Checked if not uptodate else xp.Menu_Unchecked)

        xp.setMenuItemName(self.menu, self.updateMenuIdx, self.menu_update_text())

    def menuHandler(self, _menuRef, itemRef):
        if itemRef == 'samples':
            samples.download()

    def updatePython(self, _inCommand, inPhase, _inRefcon):
        if inPhase == xp.CommandBegin:
            if self.do_not_check_for_updates:
                # (user has selected the menu item, but 'do_not_check' is set...)
                lines = [" ",
                         "Current XPPython3 preferences are set to NOT check for updates.",
                         " ",
                         "To update, you must enable checking:",
                         " 1) Select XPPython3-> Preferences menu",
                         ' 2) Enable the "Check for updates to XPPython3" option',
                         ' 3) Select "Save" in the preferences popup',
                         " 4) Reload or Restart X-Plane",
                         " ",
                         "XPPython3 will then automatically determine if it is up-to-date, ",
                         "and will enable you to update it, if you desire."
                         ]

                self.need_to_keep_a_handle_to_this = popups.Popup("Updates Disabled", lines)
                return 0

            for variant in ('mac_x64', 'lin_x64', 'win_x64'):
                path = os.path.join(xp.getSystemPath(), 'Resources', 'plugins', 'XPPython3', variant, 'XPPython3.xpl')
                if os.path.exists(path) and os.path.islink(path):
                    xp.log("Development platform with symbolic link to XPPython3. Not updating.")
                    return 0
            self.check(forceUpgrade=True)
            xp.checkMenuItem(xp.findPluginsMenu(), 0, xp.Menu_Unchecked)
            xp.setMenuItemName(self.menu, self.updateMenuIdx, "Will change to new version on restart.")
        return 0

    def XPluginStop(self):
        if self.menu:
            xp.destroyMenu(self.menu)
        for cmd in self.cmds:
            if cmd['commandRef']:
                xp.unregisterCommandHandler(cmd['commandRef'], cmd['callback'], 1, '')

        xp.unregisterFlightLoopCallback(self.performance.fLCallback, None)

        for x in (self.pip, self.about, self.performance, self.preferences):
            window = getattr(x, 'window')
            if window:
                widgetID = getattr(window, 'widgetID')
                if widgetID:
                    xp.destroyWidget(widgetID, 1)
                    widgetID = None

        self.save()
        Usage(self)
        return

    def XPluginEnable(self):
        if self.preferences.popup_preferences_dialog:
            xp.commandOnce(self.cmds[4]['commandRef'])
        return 1

    def menu_update_text(self):
        try_beta = self.config['beta']
        current = self.Version
        beta_version = self.beta_version
        stable_version = self.new_version
        uptodate, version = calc_update(try_beta, current, stable_version, beta_version)

        if self.do_not_check_for_updates or version == "Unknown":
            return f"{current} - Currency status not verified"
        if uptodate:
            return (f"{'Stable' if current == stable_version else 'Beta' if current == beta_version else ''}"
                    f" {current} is up-to-date")
        return f"Update to {'Beta' if version == beta_version else 'Stable'} {version}"
