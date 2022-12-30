import sys
import urllib
import re
import os
import subprocess
import webbrowser
import XPPython
from XPPython3 import scriptupdate
from XPPython3 import xp
from XPPython3.utils import samples


class Config (scriptupdate.Updater):
    Name = "XPPython3 Updater"
    Sig = f"xppython3.{xp.getVersions()[1]}.{sys.version_info.major}.{sys.version_info.minor}"  #{SDK}
    Desc = "Automatic updater for XPPython3 plugin"
    Version = XPPython.VERSION
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


class PythonInterface(Config):

    def __init__(self):
        self.menu = None
        self.updatePythonCmdRef = None
        self.togglePipCmdRef = None
        self.toggleAboutCmdRef = None
        self.togglePerformanceCmdRef = None
        self.pipWindow = None
        self.aboutWindow = None
        self.performanceWindow = None
        self.status_idx = 0
        self.stats = []
        self.updateMenuIdx = None
        self.frame_rate_period_dref = xp.findDataRef('sim/time/framerate_period')
        super(PythonInterface, self).__init__()

    def XPluginStart(self):
        old_logfile = os.path.join(xp.getSystemPath(), "XPPython3.log")
        if os.path.exists(old_logfile):
            try:
                os.remove(old_logfile)
            except Exception as e:
                xp.log("Old XPPython3 log file, 'XPPython3.log' still exists: you should remove it: {}".format(e))

        self.updatePythonCmdRef = xp.createCommand('xppython3/update', 'Update XPPython3 Plugin')
        xp.registerCommandHandler(self.updatePythonCmdRef, self.updatePython, 1, '')

        self.togglePipCmdRef = xp.createCommand('xppython3/pip', 'Toggle PIP window')
        xp.registerCommandHandler(self.togglePipCmdRef, self.togglePip, 1, '')

        self.toggleAboutCmdRef = xp.createCommand('xppython3/about', 'Toggle XPPython3 about window')
        xp.registerCommandHandler(self.toggleAboutCmdRef, self.toggleAbout, 1, '')

        self.togglePerformanceCmdRef = xp.createCommand('xppython3/performance', 'Toggle XPPython3 performance window')
        xp.registerCommandHandler(self.togglePerformanceCmdRef, self.togglePerformance, 1, '')

        xp.registerFlightLoopCallback(self.performanceFLCallback, 0, None)

        self.menu = xp.createMenu('XPPython3', None, 0, self.menuHandler, 'updatePython')
        xp.appendMenuItem(self.menu, 'About', 'about')
        xp.appendMenuItem(self.menu, 'Performance', 'performance')
        # xp.appendMenuItemWithCommand(self.menu, 'Disable scripts', xp.findCommand('XPPython3/disableScripts'))
        # xp.appendMenuItemWithCommand(self.menu, 'Enable scripts', xp.findCommand('XPPython3/enableScripts'))
        xp.appendMenuSeparator(self.menu)
        xp.appendMenuItem(self.menu, 'Update', 'update')
        xp.appendMenuItem(self.menu, 'Download Samples', 'samples')
        self.updateMenuIdx = 3
        xp.appendMenuItem(self.menu, 'Pip Package Installer', 'pip')
        xp.appendMenuSeparator(self.menu)
        xp.appendMenuItemWithCommand(self.menu, 'Reload scripts', xp.findCommand('XPPython3/reloadScripts'))
        self.setUpdateMenu()

        return self.Name, self.Sig, self.Desc

    def setUpdateMenu(self):
        try_beta = self.config['beta']
        current = self.Version
        beta_version = self.beta_version
        stable_version = self.new_version
        uptodate, version = self.calc_update(try_beta, current, stable_version, beta_version)
        xp.checkMenuItem(xp.findPluginsMenu(), 0, xp.Menu_Checked if not uptodate else xp.Menu_Unchecked)
        xp.setMenuItemName(self.menu, self.updateMenuIdx, self.menu_update_text())

    def menuHandler(self, menuRef, itemRef):
        if itemRef == 'samples':
            samples.download()
        elif itemRef == 'update':
            xp.commandOnce(self.updatePythonCmdRef)
        elif itemRef == 'pip':
            xp.commandOnce(self.togglePipCmdRef)
        elif itemRef == 'about':
            xp.commandOnce(self.toggleAboutCmdRef)
        elif itemRef == 'performance':
            xp.commandOnce(self.togglePerformanceCmdRef)

    def updatePython(self, inCommand, inPhase, inRefcon):
        if inPhase == xp.CommandBegin:
            self.check(forceUpgrade=True)
            xp.checkMenuItem(xp.findPluginsMenu(), 0, xp.Menu_Unchecked)
            try:
                xp.setMenuItemName(self.menu, self.updateMenuIdx, "Will change to new version on restart.")
            except Exception:
                return 0
        return 0

    def togglePerformance(self, inCommand, inPhase, inRefcon):
        if inPhase == xp.CommandBegin:
            if not self.performanceWindow:
                self.performanceWindow = self.createPerformanceWindow()
                xp.setFlightLoopCallbackInterval(self.performanceFLCallback, -1, 1, None)
            else:
                xp.setFlightLoopCallbackInterval(self.performanceFLCallback, 0, 1, None)
                xp.destroyWidget(self.performanceWindow['widgetID'], 1)
                self.performanceWindow = None
        return 0

    def performanceFLCallback(self, *args, **kwargs):
        def sum_merge(a, b):
            res = {}
            for plugin in (set(a) | set(b)):
                fields = set(a.get(plugin, {})) | set(b.get(plugin, {}))
                res[plugin] = {x: (a.get(plugin, {}).get(x, 0) + b.get(plugin, {}).get(x, 0)) for x in fields}
            return res

        maximum = 10
        data = xp.getPluginStats()

        if self.status_idx == 0:
            self.stats = data
        else:
            self.stats = sum_merge(self.stats, data)
        self.status_idx += 1

        if self.performanceWindow:
            if self.status_idx == maximum:
                total = self.stats[None]['fl'] + self.stats[None]['draw'] + self.stats[None]['customw']
                fontID = xp.Font_Proportional
                w = self.performanceWindow['widgets']
                for k, v in self.stats.items():
                    k = str(k) if k is not None else 'All'
                    if k + 'fl' in w:
                        (left, top, right, bottom) = xp.getWidgetGeometry(w[k + 'customw'])
                        value = str(int(v['customw'] / maximum))
                        newleft = int(right - xp.measureString(fontID, value))
                        xp.setWidgetGeometry(w[k + 'customw'], newleft, top, right, bottom)
                        xp.setWidgetDescriptor(w[k + 'customw'], value)

                        (left, top, right, bottom) = xp.getWidgetGeometry(w[k + 'draw'])
                        value = str(int(v['draw'] / maximum))
                        newleft = int(right - xp.measureString(fontID, value))
                        xp.setWidgetGeometry(w[k + 'draw'], newleft, top, right, bottom)
                        xp.setWidgetDescriptor(w[k + 'draw'], value)

                        (left, top, right, bottom) = xp.getWidgetGeometry(w[k + 'fl'])
                        value = str(int(v['fl'] / maximum))
                        newleft = int(right - xp.measureString(fontID, value))
                        xp.setWidgetGeometry(w[k + 'fl'], newleft, top, right, bottom)
                        xp.setWidgetDescriptor(w[k + 'fl'], value)

                        (left, top, right, bottom) = xp.getWidgetGeometry(w[k + 'pct'])
                        value = '{:.1f}%'.format(100.0 * (v['customw'] + v['fl'] + v['draw']) / total)
                        newleft = int(right - xp.measureString(fontID, value))
                        xp.setWidgetGeometry(w[k + 'pct'], newleft, top, right, bottom)
                        xp.setWidgetDescriptor(w[k + 'pct'], value)
                frp = xp.getDataf(self.frame_rate_period_dref)
                xp.setWidgetDescriptor(w['frvalue'],
                                       '{:.0f} / {:4.1f} fps'.format(frp * 1000000, 1.0 / frp))
                if self.stats[None]['fl'] > 0:
                    sq_frp = frp * frp
                    xp.setWidgetDescriptor(w['fl_fps'], 'Cost: {:4.1f} + {:4.1f} + {:4.1f} = {:5.2f} fps'.format(
                        (self.stats[None]['customw'] / (maximum * 1000000.0)) / sq_frp,
                        (self.stats[None]['draw'] / (maximum * 1000000.0)) / sq_frp,
                        (self.stats[None]['fl'] / (maximum * 1000000.0)) / sq_frp,
                        ((self.stats[None]['fl'] + self.stats[None]['draw'] + self.stats[None]['customw']) / (maximum * 1000000.0)) / sq_frp,
                    ))
                self.status_idx = 0
            return -1
        else:
            return 0

    def toggleAbout(self, inCommand, inPhase, inRefcon):
        if inPhase == xp.CommandBegin:
            if not self.aboutWindow:
                self.aboutWindow = self.createAboutWindow()
            else:
                if xp.isWidgetVisible(self.aboutWindow['widgetID']):
                    xp.hideWidget(self.aboutWindow['widgetID'])
                else:
                    xp.showWidget(self.aboutWindow['widgetID'])
        return 0

    def togglePip(self, inCommand, inPhase, inRefcon):
        if inPhase == xp.CommandBegin:
            if not self.pipWindow:
                self.pipWindow = self.createPipWindow()
            else:
                if xp.isWidgetVisible(self.pipWindow['widgetID']):
                    xp.hideWidget(self.pipWindow['widgetID'])
                else:
                    xp.showWidget(self.pipWindow['widgetID'])
        return 0

    def XPluginStop(self):
        if self.menu:
            xp.destroyMenu(self.menu)
        if self.updatePythonCmdRef:
            xp.unregisterCommandHandler(self.updatePythonCmdRef, self.updatePython, 1, '')
        if self.togglePipCmdRef:
            xp.unregisterCommandHandler(self.togglePipCmdRef, self.togglePip, 1, '')
        if self.toggleAboutCmdRef:
            xp.unregisterCommandHandler(self.toggleAboutCmdRef, self.toggleAbout, 1, '')
        if self.togglePerformanceCmdRef:
            xp.unregisterCommandHandler(self.togglePerformanceCmdRef, self.togglePerformance, 1, '')
        xp.unregisterFlightLoopCallback(self.performanceFLCallback, None)
        if self.pipWindow:
            xp.destroyWidget(self.pipWindow['widgetID'], 1)
            self.pipWindow = None
        if self.aboutWindow:
            xp.destroyWidget(self.aboutWindow['widgetID'], 1)
            self.aboutWindow = None
        if self.performanceWindow:
            xp.destroyWidget(self.performanceWindow['widgetID'], 1)
            self.performanceWindow = None
        self.save()
        return

    def XPluginEnable(self):
        return 1

    def XPluginDisable(self):
        return

    def XPluginReceiveMessage(self, inFromWho, inMessage, inParam):
        return

    def createPerformanceWindow(self):
        widgetWindow = {'widgetID': None,
                        'widgets': {}}
        fontID = xp.Font_Proportional
        _w, strHeight, _ignore = xp.getFontDimensions(fontID)
        data = sorted([x[-1] for x in xp.pythonGetDicts()['plugins'].values()])
        data.append('All')

        left = 100
        top = 300
        width = 525
        height = int((3 + len(data)) * (strHeight + 5) + 40)

        widgetWindow['widgetID'] = xp.createWidget(left, top, left + width, top - height, 1, "Python Plugins Performance",
                                                   1, 0, xp.WidgetClass_MainWindow)
        xp.setWidgetProperty(widgetWindow['widgetID'], xp.Property_MainWindowHasCloseBoxes, 1)
        xp.addWidgetCallback(widgetWindow['widgetID'], self.performanceWindowCallback)
        top -= 30
        right = left + width
        bottom = int(top - strHeight)

        # Top line -- header
        colRight = [240, 160, 80, 20]
        label = 'Plugin (times in μsec)'
        widgetWindow['widgets']['title' + 'label'] = xp.createWidget(
            left + 10, top, left + int(xp.measureString(fontID, label)),
            bottom, 1, label, 0, widgetWindow['widgetID'], xp.WidgetClass_Caption)

        for k in (('Custom Widgets', 'customw', colRight[0]),
                  ('Drawing Misc', 'draw', colRight[1]),
                  ('Flight Loop', 'fl', colRight[2]),
                  ('%', 'pct', colRight[3])):
            label, code, col = k
            strWidth = xp.measureString(fontID, label)
            widgetWindow['widgets']['title' + code] = xp.createWidget(
                right - col - int(strWidth), top, right - col, bottom, 1, label, 0, widgetWindow['widgetID'], xp.WidgetClass_Caption)

        # widgets for each plugin line
        top -= int(strHeight * 1.5)
        bottom = int(top - 1.5 * strHeight)

        for k in data:
            if k == data[-1]:
                top -= 5
                bottom -= 5

            # shorten the displayed label, if it's too wide
            k_label = k
            strWidth = int(xp.measureString(fontID, k_label))
            if strWidth > right - 300:
                k_label = k_label.replace('Laminar Research', '.').replace('.plugins.PythonPlugins.', '...')
                strWidth = int(xp.measureString(fontID, k_label))
                if strWidth > right - 300:
                    k_label = '...' + k_label[-35:]
                    strWidth = int(xp.measureString(fontID, k_label))
            widgetWindow['widgets'][k + 'label'] = xp.createWidget(left + 10, top, left + strWidth, bottom,
                                                                   1, k_label, 0, widgetWindow['widgetID'], xp.WidgetClass_Caption)
            widgetWindow['widgets'][k + 'customw'] = xp.createWidget(right - 300, top, right - colRight[0], bottom,
                                                                     1, '', 0, widgetWindow['widgetID'], xp.WidgetClass_Caption)
            widgetWindow['widgets'][k + 'draw'] = xp.createWidget(right - 300, top, right - colRight[1], bottom,
                                                                  1, '', 0, widgetWindow['widgetID'], xp.WidgetClass_Caption)
            widgetWindow['widgets'][k + 'fl'] = xp.createWidget(right - 200, top, right - colRight[2], bottom,
                                                                1, '', 0, widgetWindow['widgetID'], xp.WidgetClass_Caption)
            widgetWindow['widgets'][k + 'pct'] = xp.createWidget(right - 80, top, right - colRight[3], bottom,
                                                                 1, '', 0, widgetWindow['widgetID'], xp.WidgetClass_Caption)
            top -= strHeight + 5
            bottom = top - strHeight

        # skip a line for footer
        top -= int(strHeight) + 5
        bottom = int(top - strHeight)

        # Footer
        label = 'Current frame rate (in μsec):'
        strWidth = xp.measureString(fontID, label)
        widgetWindow['widgets']['frlabel'] = xp.createWidget(left + 10, top, left + int(strWidth), bottom,
                                                             1, label, 0, widgetWindow['widgetID'], xp.WidgetClass_Caption)
        label = "0"
        widgetWindow['widgets']['frvalue'] = xp.createWidget(left + 10 + int(strWidth) + 10, top, right - 300, bottom,
                                                             1, label, 0, widgetWindow['widgetID'], xp.WidgetClass_Caption)
        widgetWindow['widgets']['fl_fps'] = xp.createWidget(right - 225, top, right - 100, bottom,
                                                            1, 'fl_fps', 0, widgetWindow['widgetID'], xp.WidgetClass_Caption)

        return widgetWindow

    def performanceWindowCallback(self, inMessage, inWidget, inParam1, inParam2):
        if inMessage == xp.Message_CloseButtonPushed:
            xp.commandOnce(self.togglePerformanceCmdRef)
            return 1
        return 0

    def createAboutWindow(self):
        widgetWindow = {'widgetID': None,
                        'widgets': {}}
        left = 100
        top = 300
        width = 525
        height = 170

        widgetWindow['widgetID'] = xp.createWidget(left, top, left + width, top - height, 1, "About XPPython3",
                                                   1, 0, xp.WidgetClass_MainWindow)
        xp.setWidgetProperty(widgetWindow['widgetID'], xp.Property_MainWindowHasCloseBoxes, 1)
        fontID = xp.Font_Proportional
        _w, strHeight, _ignore = xp.getFontDimensions(fontID)

        xp.addWidgetCallback(widgetWindow['widgetID'], self.aboutWidgetCallback)
        left += 10
        top -= 30
        bottom = int(top - strHeight)

        s = "X-Plane Plugin to support Python3 plugins"
        strWidth = xp.measureString(fontID, s)
        right = int(left + strWidth)
        xp.createWidget(left, top, right, bottom, 1, s, 0, widgetWindow['widgetID'],
                        xp.WidgetClass_Caption)

        top = bottom - 8
        bottom = int(top - strHeight)
        s = 'This is Version {}'.format(XPPython.VERSION)
        strWidth = xp.measureString(fontID, s)
        right = int(left + strWidth)
        xp.createWidget(left, top, right, bottom, 1, s, 0, widgetWindow['widgetID'],
                        xp.WidgetClass_Caption)

        beta_left = left + width - 150
        right = int(beta_left + 10)
        widgetWindow['widgets']['beta'] = xp.createWidget(beta_left, top, right, bottom,
                                                          1, '', 0, widgetWindow['widgetID'],
                                                          xp.WidgetClass_Button)
        xp.setWidgetProperty(widgetWindow['widgets']['beta'], xp.Property_ButtonType, xp.RadioButton)
        xp.setWidgetProperty(widgetWindow['widgets']['beta'], xp.Property_ButtonBehavior, xp.ButtonBehaviorCheckBox)
        xp.setWidgetProperty(widgetWindow['widgets']['beta'], xp.Property_ButtonState, 1 if self.config['beta'] else 0)

        beta_left = right + 5
        s = "Include Betas "
        strWidth = xp.measureString(fontID, s)
        right = int(beta_left + strWidth)
        xp.createWidget(beta_left, top, right, bottom, 1, s, 0, widgetWindow['widgetID'], xp.WidgetClass_Caption)

        top = bottom - 10
        bottom = int(top - strHeight)
        s = self.get_currency()
        strWidth = xp.measureString(fontID, s)
        right = int(left + 20 + strWidth)
        widgetWindow['widgets']['currency'] = xp.createWidget(left + 20, top, right, bottom, 1, s, 0,
                                                              widgetWindow['widgetID'], xp.WidgetClass_Caption)

        top = bottom - 10
        bottom = int(top - strHeight)
        s = "Based off of Sandy Barbour's Python2 plugin, this plugin is updated for X-Plane 11.50+,"
        strWidth = xp.measureString(fontID, s)
        right = int(left + strWidth)
        xp.createWidget(left, top, right, bottom, 1, s, 0, widgetWindow['widgetID'],
                        xp.WidgetClass_Caption)

        top = bottom - 4
        bottom = int(top - strHeight)
        s = "and python3 language. "
        strWidth = xp.measureString(fontID, s)
        right = int(left + strWidth)
        xp.createWidget(left, top, right, bottom, 1, s, 0, widgetWindow['widgetID'],
                        xp.WidgetClass_Caption)

        top = bottom - 8
        bottom = int(top - strHeight)
        s = "See documentation at xppython3.rtfd.io."
        strWidth = xp.measureString(fontID, s)
        right = int(left + strWidth)
        xp.createWidget(left, top, right, bottom, 1, s, 0, widgetWindow['widgetID'],
                        xp.WidgetClass_Caption)

        top = bottom - 15
        bottom = int(top - strHeight)
        left += 10
        s = " User Documentation "
        strWidth = xp.measureString(fontID, s)
        right = int(left + strWidth)
        widgetWindow['widgets']['documentation'] = xp.createWidget(left, top, right, bottom,
                                                                   1, s, 0, widgetWindow['widgetID'],
                                                                   xp.WidgetClass_Button)

        left = right + 10
        s = " Plugin Development  "
        strWidth = xp.measureString(fontID, s)
        right = int(left + strWidth)
        widgetWindow['widgets']['plugin'] = xp.createWidget(left, top, right, bottom,
                                                            1, s, 0, widgetWindow['widgetID'],
                                                            xp.WidgetClass_Button)
        left = right + 10
        s = " Support "
        strWidth = xp.measureString(fontID, s)
        right = int(left + strWidth)
        widgetWindow['widgets']['support'] = xp.createWidget(left, top, right, bottom,
                                                             1, s, 0, widgetWindow['widgetID'],
                                                             xp.WidgetClass_Button)
        left = right + 10
        s = " Donate "
        strWidth = xp.measureString(fontID, s)
        right = int(left + strWidth)
        widgetWindow['widgets']['donate'] = xp.createWidget(left, top, right, bottom,
                                                            1, s, 0, widgetWindow['widgetID'],
                                                            xp.WidgetClass_Button)
        left = right + 30
        s = " Changelog "
        strWidth = xp.measureString(fontID, s)
        right = int(left + strWidth)
        widgetWindow['widgets']['changelog'] = xp.createWidget(left, top, right, bottom,
                                                               1, s, 0, widgetWindow['widgetID'],
                                                               xp.WidgetClass_Button)
        return widgetWindow

    def menu_update_text(self):
        try_beta = self.config['beta']
        current = self.Version
        beta_version = self.beta_version
        stable_version = self.new_version
        uptodate, version = self.calc_update(try_beta, current, stable_version, beta_version)

        if uptodate:
            return "{} {} is up-to-date".format('Beta' if current == beta_version else 'Stable', current)
        return "Update to {} {}".format('Beta' if version == beta_version else 'Stable', version)

    def get_currency(self):
        try_beta = self.config['beta']
        current = self.Version
        beta_version = self.beta_version
        stable_version = self.new_version

        uptodate, version = self.calc_update(try_beta, current, stable_version, beta_version)
        if uptodate:
            return "{} {} is up-to-date".format('Beta' if current == beta_version else 'Stable', current)
        return "{} {} is available".format('Beta' if version == beta_version else 'Stable', version)

    def aboutWidgetCallback(self, inMessage, inWidget, inParam1, inParam2):
        if inMessage == xp.Message_CloseButtonPushed:
            xp.hideWidget(self.aboutWindow['widgetID'])
            return 1

        if inMessage == xp.Msg_ButtonStateChanged:
            if inParam1 == self.aboutWindow['widgets']['beta']:
                self.config['beta'] = xp.getWidgetProperty(inParam1, xp.Property_ButtonState, None) == 1
                xp.setWidgetDescriptor(self.aboutWindow['widgets']['currency'], self.get_currency())
                self.setUpdateMenu()

        if inMessage == xp.Msg_PushButtonPressed:
            if inParam1 == self.aboutWindow['widgets']['documentation']:
                webbrowser.open('https://xppython3.rtfd.io/en/latest/usage/installation_plugin.html')
                return 1

            if inParam1 == self.aboutWindow['widgets']['support']:
                webbrowser.open('https://forums.x-plane.org/index.php?/forums/topic/225976-xppython3-now-available/')
                return 1

            if inParam1 == self.aboutWindow['widgets']['plugin']:
                webbrowser.open('https://xppython3.rtfd.io/en/latest/development/index.html')
                return 1

            if inParam1 == self.aboutWindow['widgets']['donate']:
                webbrowser.open('https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=TA3EJ9VWFCH3N&source=url')
                return 1

            if inParam1 == self.aboutWindow['widgets']['changelog']:
                webbrowser.open('https://xppython3.rtfd.io/en/latest/changelog.html')
                return 1
        return 0

    def createPipWindow(self):
        widgetWindow = {'widgetID': None,
                        'widgets': {}}
        box_left = 100
        box_right = 500
        top = 300

        widgetWindow['widgetID'] = xp.createWidget(box_left, top, box_right, top - 125, 1, "PIP Package Installer",
                                                   1, 0, xp.WidgetClass_MainWindow)
        xp.setWidgetProperty(widgetWindow['widgetID'], xp.Property_MainWindowHasCloseBoxes, 1)
        fontID = xp.Font_Proportional
        _w, strHeight, _ignore = xp.getFontDimensions(fontID)

        xp.addWidgetCallback(widgetWindow['widgetID'], self.pipWidgetCallback)
        left = box_left + 10
        top -= 30
        bottom = int(top - strHeight)

        # Instructions....
        s = "Enter one or more python packages below, then press 'Install'."
        strWidth = xp.measureString(fontID, s)
        right = int(left + strWidth)
        xp.createWidget(left, top, right, bottom, 1, s, 0, widgetWindow['widgetID'],
                        xp.WidgetClass_Caption)
        top -= strHeight + 4
        bottom = int(top - strHeight)

        # text field
        bottom = int(top - (strHeight + 4))
        right = int(left + strWidth)
        widgetWindow['widgets']['packages'] = xp.createWidget(left, top, right, bottom, 1, '', 0, widgetWindow['widgetID'],
                                                              xp.WidgetClass_TextField)
        top -= strHeight + 8
        bottom = int(top - strHeight)

        # Hints....
        s = "Common packages: requests, pyopengl, cryptography, urllib3, six"
        strWidth = xp.measureString(fontID, s)
        right = int(left + strWidth)
        xp.createWidget(left, top, right, bottom, 1, s, 0, widgetWindow['widgetID'],
                        xp.WidgetClass_Caption)
        top = bottom - 4
        bottom = int(top - strHeight)

        # (add a bit extra vertical buffer)
        top = bottom - 4
        bottom = int(top - strHeight)
        # button
        s = " Install "
        strWidth = xp.measureString(fontID, s)
        right = int(left + strWidth)

        middle = int((box_left + box_right) / 2)
        widgetWindow['widgets']['button'] = xp.createWidget(int(middle - strWidth / 2.0), top,
                                                            int(middle + strWidth / 2.0), bottom,
                                                            1, s, 0, widgetWindow['widgetID'],
                                                            xp.WidgetClass_Button)
        top = bottom - 10
        bottom = int(top - strHeight)
        right = left + 400
        widgetWindow['widgets']['error'] = xp.createWidget(left, top, right, bottom,
                                                           1, '', 0, widgetWindow['widgetID'],
                                                           xp.WidgetClass_Caption)

        return widgetWindow

    def pipWidgetCallback(self, inMessage, inWidget, inParam1, inParam2):
        if inMessage == xp.Message_CloseButtonPushed:
            xp.hideWidget(self.pipWindow['widgetID'])
            return 1

        if inMessage == xp.Msg_PushButtonPressed:
            if inParam1 == self.pipWindow['widgets']['button']:
                s = xp.getWidgetDescriptor(self.pipWindow['widgets']['packages'])
                packages = list(filter(lambda x: x != '', re.split('[, ]+', s)))
                print("Looking to install packages: {}".format(packages))
                if packages:
                    xp.setWidgetDescriptor(self.pipWindow['widgets']['error'], "Looking to install packages: {}".format(' '.join(packages)))
                    cmd = [xp.pythonExecutable, '-m', 'pip', 'install', '--user'] + packages
                    print("Calling pip as: {}".format(' '.join(cmd)))
                    try:
                        xp.setWidgetDescriptor(self.pipWindow['widgets']['error'], "Running pip... please wait.")
                        output = subprocess.check_output(cmd, stderr=subprocess.STDOUT)
                        xp.setWidgetDescriptor(self.pipWindow['widgets']['error'], "Execution complete.")
                        print("From pip:\n{}".format(output.decode('utf-8')))
                        output = output.decode('utf-8').split('\n')
                    except subprocess.CalledProcessError as e:
                        print("Calling pip failed: [{}]: {}".format(e.returncode, e.output.decode('utf-8')))
                        xp.setWidgetDescriptor(self.pipWindow['widgets']['error'], "Failed: Error while executing pip.")
                        output = e.output.decode('utf-8').split('\n')
                    popupWindow('PIP output', output)

                xp.setWidgetDescriptor(self.pipWindow['widgets']['packages'], '')
            return 1

        return 0


def popupCallback(inMessage, inWidget, inParam1, inParam2):
    if inMessage == xp.Message_CloseButtonPushed:
        xp.hideWidget(inWidget)
        return 1
    return 0


def popupWindow(title, lines):
    """
    Simple popup window with title & closebuttons, and multiple lines displayed
    Window is _sized_to_fit_ all lines, without scrolling.
    So do not use this if a line is going to be wider than the screen,
    or if there are so many lines it won't fit vertically. I do not error check.
    """
    fontID = xp.Font_Proportional
    _w, strHeight, _ignore = xp.getFontDimensions(fontID)

    left = 100
    bottom = 400
    top = bottom + 25
    widest = 10
    for line in lines:
        top += strHeight + 4
        widest = max(widest, xp.measureString(fontID, line.strip()))

    main = xp.createWidget(left, top, int(left + widest + 10), bottom,
                           1, title, 1, 0, xp.WidgetClass_MainWindow)
    xp.setWidgetProperty(main, xp.Property_MainWindowHasCloseBoxes, 1)
    xp.addWidgetCallback(main, popupCallback)

    left += 4
    top -= 20
    for line in lines:
        bottom = top - strHeight
        right = int(left + xp.measureString(fontID, line.strip()))
        xp.createWidget(left, top, right, bottom,
                        1, line.strip(), 0, main,
                        xp.WidgetClass_Caption)
        top -= strHeight + 4
