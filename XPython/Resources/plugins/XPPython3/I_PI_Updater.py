import sys
import re
import os
import subprocess
import webbrowser
import sysconfig
import XPPython
from XPPython3 import scriptupdate
from XPPython3 import xp


class Config (scriptupdate.Updater):
    Name = "XPPython3 Updater"
    Sig = "com.avnwx.xppython3.updater.{}.{}".format(sys.version_info.major, sys.version_info.minor)
    Desc = "Automatic updater for XPPython3 plugin"
    Version = XPPython.VERSION
    VersionCheckURL = 'https://maps.avnwx.com/data/x-plane/versions.json'
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
        self.pipWindow = None
        self.aboutWindow = None
        self.updateMenuIdx = None
        super(PythonInterface, self).__init__()

    def XPluginStart(self):
        self.updatePythonCmdRef = xp.createCommand('xppython3/update', 'Update XPPython3 Plugin')
        xp.registerCommandHandler(self.updatePythonCmdRef, self.updatePython, 1, '')

        self.togglePipCmdRef = xp.createCommand('xppython3/pip', 'Toggle PIP window')
        xp.registerCommandHandler(self.togglePipCmdRef, self.togglePip, 1, '')

        self.toggleAboutCmdRef = xp.createCommand('xppython3/about', 'Toggle XPPython3 about window')
        xp.registerCommandHandler(self.toggleAboutCmdRef, self.toggleAbout, 1, '')

        self.menu = xp.createMenu('XPPython3', None, 0, self.menuHandler, 'updatePython')
        xp.appendMenuItem(self.menu, 'About', 'about')
        xp.appendMenuSeparator(self.menu)
        xp.appendMenuItemWithCommand(self.menu, 'Disable scripts', xp.findCommand('XPPython3/disableScripts'))
        xp.appendMenuItemWithCommand(self.menu, 'Enable scripts', xp.findCommand('XPPython3/enableScripts'))
        xp.appendMenuItemWithCommand(self.menu, 'Reload scripts', xp.findCommand('XPPython3/reloadScripts'))
        xp.appendMenuSeparator(self.menu)
        xp.appendMenuItem(self.menu, 'Update', 'update')
        self.updateMenuIdx = 6
        xp.appendMenuItem(self.menu, 'Pip Package Installer', 'pip')
        self.setUpdateMenu()

        return self.Name, self.Sig, self.Desc

    def setUpdateMenu(self):
        checkMark = (self.config['beta'] and self.beta_version != self.Version) or (not self.config['beta'] and self.new_version != self.Version)
        xp.checkMenuItem(xp.findPluginsMenu(), 0, xp.Menu_Checked if checkMark else xp.Menu_Unchecked)
        xp.setMenuItemName(self.menu, self.updateMenuIdx, self.menu_update_text())

    def menuHandler(self, menuRef, itemRef):
        if itemRef == 'update':
            xp.commandOnce(self.updatePythonCmdRef)
        if itemRef == 'pip':
            xp.commandOnce(self.togglePipCmdRef)
        if itemRef == 'about':
            xp.commandOnce(self.toggleAboutCmdRef)

    def updatePython(self, inCommand, inPhase, inRefcon):
        if inPhase == xp.CommandBegin:
            self.check(forceUpgrade=True)
            xp.checkMenuItem(xp.findPluginsMenu(), 0, xp.Menu_Unchecked)  # '1' because the main XPPython Menu is first...?
            xp.setMenuItemName(self.menu, self.updateMenuIdx,
                               "Will change to {} on restart.".format(
                                   self.beta_version if self.config['beta'] else self.new_version))
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
        if self.pipWindow:
            xp.destroyWidget(self.pipWindow['widgetID'], 1)
            self.pipWindow = None
        if self.aboutWindow:
            xp.destroyWidget(self.aboutWindow['widgetID'], 1)
            self.aboutWindow = None
        self.save()
        return

    def XPluginEnable(self):
        return 1

    def XPluginDisable(self):
        return

    def XPluginReceiveMessage(self, inFromWho, inMessage, inParam):
        return

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
        return widgetWindow

    def menu_update_text(self):
        if not self.config['beta']:
            if self.Version == self.new_version:
                return "{} is up-to-date".format(self.Version)
            return "Update to {}".format(self.new_version)
        if self.Version == self.beta_version:
            return "Beta {} is up-to-date".format(self.beta_version)
        return "Update to Beta {}".format(self.beta_version)

    def get_currency(self):
        if not self.config['beta']:
            if self.Version == self.new_version:
                return "You have the current version."
            return "Version {} is available".format(self.new_version)
        if self.Version == self.beta_version:
            return "You have the current beta version."
        return "Beta version {} is available".format(self.beta_version)

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
        s = "Enter one or more python packages below, then press 'Install'"
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
                    found = False
                    for i in ('pip', 'pip3', 'pip.exe', 'pip3.exe'):
                        pip = os.path.join(sysconfig.get_paths()['scripts'], i)
                        if os.path.isfile(pip) and os.access(pip, os.X_OK):
                            found = True
                            cmd = [pip, 'install', '--user'] + packages
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
                        if found:
                            popupWindow('PIP output', output)
                            break

                    if not found:
                        print("Could not find pip")
                        xp.setWidgetDescriptor(self.pipWindow['widgets']['error'], "Failed: Could not find pip.")

                xp.setWidgetDescriptor(self.pipWindow['widgets']['packages'], '')
            return 1

        if inMessage == xp.Msg_CursorAdjust:
            inParam2 = xp.CursorDefault
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
