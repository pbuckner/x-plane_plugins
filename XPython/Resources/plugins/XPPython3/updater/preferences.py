from typing import Self, Any
import uuid
import webbrowser
import os
import configparser
from XPPython3 import xp
from XPPython3.xp_typing import XPLMCommandRef, XPLMCommandPhase, XPWidgetID, XPWidgetMessage
from .my_widget_window import MyWidgetWindow


class Preferences:
    def __init__(self: Self) -> None:
        self.preferences: dict = {}
        self.popup_preferences_dialog = False
        self.temp_preferences: dict = {}
        self.window = MyWidgetWindow()
        self.config_ini = self.load_config()

    def save_config(self: Self, preferences: dict) -> None:
        ini_file = os.path.join(xp.getSystemPath(), 'Output', 'preferences', 'xppython3.ini')
        parser = configparser.ConfigParser()
        parser.read(ini_file)
        if not parser.has_section('Main'):
            parser.add_section('Main')
        parser['Main'].update(preferences)
        try:
            with open(ini_file, 'w', encoding='UTF-8') as configfile:
                parser.write(configfile)
        except PermissionError:
            xp.log(f"Failed to write preferences to {ini_file}. Permission error")

    def load_config(self: Self) -> configparser.ConfigParser:
        # returns configuration as read from .ini file...
        # Which_may_be empty (will not return any error notification)
        ini_file = os.path.join(xp.getSystemPath(), 'Output', 'preferences', 'xppython3.ini')
        parser = configparser.ConfigParser()
        parser.read(ini_file)
        if parser.get(section='Main', option='uuid', fallback='') == '':
            self.popup_preferences_dialog = True
            if not parser.has_section('Main'):
                parser.add_section('Main')
            parser['Main'].update({'uuid': str(uuid.uuid4())})
            self.save_config(dict(parser['Main']))
            parser.read(ini_file)

        self.preferences = {'check_for_update': True,
                            'collect_xppython3_stats': True,
                            'collect_python_plugin_stats': True,
                            'debug': False,
                            'flush_log': False,
                            'log_file_preserve': False,
                            'log_file_name': 'XPPython3Log.txt',
                            'py_verbose': False,
                            'uuid': '',
                            }
        for pref, value in list(self.preferences.items()):
            if isinstance(value, bool):
                self.preferences[pref] = parser['Main'].getboolean(pref, fallback=value)
            elif isinstance(value, float):
                self.preferences[pref] = parser['Main'].getfloat(pref, fallback=value)
            elif isinstance(value, int):
                self.preferences[pref] = parser['Main'].getint(pref, fallback=value)
            else:
                self.preferences[pref] = parser['Main'].get(pref, fallback=value)

        return parser

    def toggleCommand(self: Self, _inCommand: XPLMCommandRef, inPhase: XPLMCommandPhase, _refCon: Any) -> int:
        if inPhase == xp.CommandBegin:
            if not (self.window and self.window.widgetID):
                self.createWindow()
            else:
                if not xp.isWidgetVisible(self.window.widgetID):
                    xp.destroyWidget(self.window.widgetID)
                    self.window.widgetID = None
                    self.createWindow()
                else:
                    xp.showWidget(self.window.widgetID)
                    xp.bringRootWidgetToFront(self.window.widgetID)
        return 0

    def createWindow(self: Self) -> None:
        self.load_config()
        self.temp_preferences = self.preferences.copy()
        fontID = xp.Font_Proportional
        _w, strHeight, _ignore = xp.getFontDimensions(fontID)

        lines = ["Enable/Disable common preferences and click 'Save' when complete.",
                 " ",
                 "We'd like your permission to do the following, anonymously:",
                 " ",
                 "          Check for updates to XPPython3",
                 "          Collect XPPython3 usage statistics",
                 "          Collect Python Plugins usage statistics",
                 " ",
                 "XPPython3 is free and does not require Internet access to use.",
                 "For details on what we may collect, why, and what your options are, see:",
                 " ",
                 " ",
                 "You can update by selecting the XPPython3 -> Preferences menu.",
                 " ",
                 "------",
                 " ",
                 "If you are debugging python plugins, you might find these options useful",
                 "(there is a slight performance impact):",
                 " ",
                 "          Enable debug mode (verbose python logging & disable usage collection)",
                 "          Enable log flush on write",
                 " ",
                 " ",
                 ]
        left = 100
        top = 500

        width = -1
        for line in lines:
            width = int(max(width, xp.measureString(fontID, line)))
        width = width + 20
        height = int(len(lines) * (strHeight + 5) + 40 + 40)

        self.window.widgetID = xp.createWidget(left, top, left + width, top - height, 1, "XPPython3 Preferences",
                                               1, 0, xp.WidgetClass_MainWindow)
        winID = xp.getWidgetUnderlyingWindow(self.window.widgetID)
        xp.setWindowPositioningMode(winID, xp.WindowCenterOnMonitor, -1)
        # No close button in window header... use "Save", "Cancel" buttons
        xp.addWidgetCallback(self.window.widgetID, self.windowCallback)
        top -= 30
        right = left + width
        bottom = int(top - strHeight)

        checkboxes = {4: 'check_for_update',
                      5: 'collect_xppython3_stats',
                      6: 'collect_python_plugin_stats',
                      19: 'debug',
                      20: 'flush_log'}

        for idx, line in enumerate(lines):
            strWidth = xp.measureString(fontID, line)
            xp.createWidget(left + 10, top, int(left + strWidth), bottom,
                            1, line, 0,
                            self.window.widgetID, xp.WidgetClass_Caption)
            if idx in checkboxes:
                self.window.widgets[checkboxes[idx]] = xp.createWidget(left + 25, top, left + 35, bottom,
                                                                       1, '', 0, self.window.widgetID,
                                                                       xp.WidgetClass_Button)
                xp.setWidgetProperty(self.window.widgets[checkboxes[idx]],
                                     xp.Property_ButtonType, xp.RadioButton)
                xp.setWidgetProperty(self.window.widgets[checkboxes[idx]],
                                     xp.Property_ButtonBehavior, xp.ButtonBehaviorCheckBox)
                xp.setWidgetProperty(self.window.widgets[checkboxes[idx]],
                                     xp.Property_ButtonState, 1 if self.preferences[checkboxes[idx]] else 0)

            top -= strHeight + 5
            bottom = top - strHeight

        s = " Documentation "
        strWidth = xp.measureString(fontID, s)
        right = int(left + strWidth)
        v_offset = 13 * (strHeight + 5) - 3
        self.window.widgets['documentation'] = xp.createWidget(left + 135, top + v_offset, right + 135, bottom + v_offset,
                                                               1, s, 0, self.window.widgetID,
                                                               xp.WidgetClass_Button)

        s = " Save "
        strWidth = xp.measureString(fontID, s)
        right = int(left + strWidth)
        self.window.widgets['save_button'] = xp.createWidget(left + 100, top, right + 100, bottom,
                                                             1, s, 0, self.window.widgetID,
                                                             xp.WidgetClass_Button)

        s = " Cancel "
        strWidth = xp.measureString(fontID, s)
        right = int(left + strWidth)
        self.window.widgets['cancel_button'] = xp.createWidget(left + 200, top, right + 200, bottom,
                                                               1, s, 0, self.window.widgetID,
                                                               xp.WidgetClass_Button)

    def windowCallback(self: Self, inMessage: XPWidgetMessage, _inWidget: XPWidgetID, inParam1: Any, _inParam2: Any) -> int:
        if inMessage == xp.Msg_ButtonStateChanged:
            for x in self.preferences:
                try:
                    if inParam1 == self.window.widgets[x]:
                        self.temp_preferences[x] = xp.getWidgetProperty(inParam1, xp.Property_ButtonState) == 1
                except KeyError:
                    pass
            return 1
        if inMessage == xp.Msg_PushButtonPressed:
            if inParam1 == self.window.widgets['save_button']:
                self.preferences = {key: str(value) for key, value in self.temp_preferences.items()}
                self.save_config(self.preferences)
                if self.window.widgetID:
                    xp.hideWidget(self.window.widgetID)

            if inParam1 == self.window.widgets['cancel_button']:
                if self.window.widgetID:
                    xp.hideWidget(self.window.widgetID)

            if inParam1 == self.window.widgets['documentation']:
                webbrowser.open('https://xppython3.rtfd.io/en/latest/usage/preferences.html')
                return 1
        return 0
