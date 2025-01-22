from typing import Self, Any, Optional, Callable
import webbrowser
from XPPython3 import xp
from XPPython3.xp_typing import XPLMCommandRef, XPLMCommandPhase, XPWidgetMessage, XPWidgetID
from .my_widget_window import MyWidgetWindow
from .currency import Currency


class About:
    def __init__(self: Self) -> None:
        self.window = MyWidgetWindow()
        self.config: dict = {}
        self.new_version = '0.0'
        self.beta_version = '0.0'
        self.setUpdateMenu: Optional[Callable] = None
        self.Version = '0.0'

    def toggleCommand(self, _inCommand: XPLMCommandRef, inPhase: XPLMCommandPhase, _refCon: Any) -> int:
        if inPhase == xp.CommandBegin:
            if not (self.window and self.window.widgetID):
                self.createWindow()
            else:
                if xp.isWidgetVisible(self.window.widgetID) and xp.isWidgetInFront(self.window.widgetID):
                    xp.hideWidget(self.window.widgetID)
                else:
                    xp.showWidget(self.window.widgetID)
                    xp.bringRootWidgetToFront(self.window.widgetID)
        return 0

    def createWindow(self: Self) -> None:
        left = 100
        top = 300
        width = 525
        height = 170

        self.window.widgetID = xp.createWidget(left, top, left + width, top - height, 1, "About XPPython3",
                                               1, 0, xp.WidgetClass_MainWindow)
        xp.setWidgetProperty(self.window.widgetID, xp.Property_MainWindowHasCloseBoxes, 1)
        fontID = xp.Font_Proportional
        _w, strHeight, _ignore = xp.getFontDimensions(fontID)

        xp.addWidgetCallback(self.window.widgetID, self.windowCallback)
        left += 10
        top -= 30
        bottom = int(top - strHeight)

        s = "X-Plane Plugin to support Python3 plugins"
        strWidth = xp.measureString(fontID, s)
        right = int(left + strWidth)
        xp.createWidget(left, top, right, bottom, 1, s, 0, self.window.widgetID,
                        xp.WidgetClass_Caption)

        top = bottom - 8
        bottom = int(top - strHeight)
        s = f'This is Version {xp.VERSION}'
        strWidth = xp.measureString(fontID, s)
        right = int(left + strWidth)
        xp.createWidget(left, top, right, bottom, 1, s, 0, self.window.widgetID,
                        xp.WidgetClass_Caption)

        beta_left = left + width - 150
        right = int(beta_left + 10)
        self.window.widgets['beta'] = xp.createWidget(beta_left, top, right, bottom,
                                                      1, '', 0, self.window.widgetID,
                                                      xp.WidgetClass_Button)
        xp.setWidgetProperty(self.window.widgets['beta'], xp.Property_ButtonType, xp.RadioButton)
        xp.setWidgetProperty(self.window.widgets['beta'], xp.Property_ButtonBehavior, xp.ButtonBehaviorCheckBox)
        xp.setWidgetProperty(self.window.widgets['beta'],
                             xp.Property_ButtonState, 1 if self.config.get('beta', 0) else 0)

        beta_left = right + 5
        s = "Include Betas "
        strWidth = xp.measureString(fontID, s)
        right = int(beta_left + strWidth)
        xp.createWidget(beta_left, top, right, bottom, 1, s, 0, self.window.widgetID, xp.WidgetClass_Caption)

        top = bottom - 10
        bottom = int(top - strHeight)
        s = Currency(self.Version, self.beta_version,
                     self.new_version).get_currency(self.config.get('beta', False))
        strWidth = xp.measureString(fontID, s)
        right = int(left + 20 + strWidth)
        self.window.widgets['currency'] = xp.createWidget(left + 20, top, right, bottom, 1, s, 0,
                                                          self.window.widgetID, xp.WidgetClass_Caption)

        top = bottom - 10
        bottom = int(top - strHeight)
        s = "Based off of Sandy Barbour's Python2 plugin, this plugin is updated for X-Plane 11.50+,"
        strWidth = xp.measureString(fontID, s)
        right = int(left + strWidth)
        xp.createWidget(left, top, right, bottom, 1, s, 0, self.window.widgetID,
                        xp.WidgetClass_Caption)

        top = bottom - 4
        bottom = int(top - strHeight)
        s = "and python3 language. "
        strWidth = xp.measureString(fontID, s)
        right = int(left + strWidth)
        xp.createWidget(left, top, right, bottom, 1, s, 0, self.window.widgetID,
                        xp.WidgetClass_Caption)

        top = bottom - 8
        bottom = int(top - strHeight)
        s = "See documentation at xppython3.rtfd.io."
        strWidth = xp.measureString(fontID, s)
        right = int(left + strWidth)
        xp.createWidget(left, top, right, bottom, 1, s, 0, self.window.widgetID,
                        xp.WidgetClass_Caption)

        top = bottom - 15
        bottom = int(top - strHeight)
        left += 10
        s = " User Documentation "
        strWidth = xp.measureString(fontID, s)
        right = int(left + strWidth)
        self.window.widgets['documentation'] = xp.createWidget(left, top, right, bottom,
                                                               1, s, 0, self.window.widgetID,
                                                               xp.WidgetClass_Button)

        left = right + 10
        s = " Plugin Development  "
        strWidth = xp.measureString(fontID, s)
        right = int(left + strWidth)
        self.window.widgets['plugin'] = xp.createWidget(left, top, right, bottom,
                                                        1, s, 0, self.window.widgetID,
                                                        xp.WidgetClass_Button)
        left = right + 10
        s = " Support "
        strWidth = xp.measureString(fontID, s)
        right = int(left + strWidth)
        self.window.widgets['support'] = xp.createWidget(left, top, right, bottom,
                                                         1, s, 0, self.window.widgetID,
                                                         xp.WidgetClass_Button)
        left = right + 10
        s = " Donate "
        strWidth = xp.measureString(fontID, s)
        right = int(left + strWidth)
        self.window.widgets['donate'] = xp.createWidget(left, top, right, bottom,
                                                        1, s, 0, self.window.widgetID,
                                                        xp.WidgetClass_Button)
        left = right + 30
        s = " Changelog "
        strWidth = xp.measureString(fontID, s)
        right = int(left + strWidth)
        self.window.widgets['changelog'] = xp.createWidget(left, top, right, bottom,
                                                           1, s, 0, self.window.widgetID,
                                                           xp.WidgetClass_Button)

    def windowCallback(self: Self, inMessage: XPWidgetMessage, _inWidget: XPWidgetID, inParam1: Any, _inParam2: Any) -> int:
        if inMessage == xp.Message_CloseButtonPushed:
            if self.window.widgetID is not None:
                xp.hideWidget(self.window.widgetID)
            return 1

        if inMessage == xp.Msg_ButtonStateChanged:
            if inParam1 == self.window.widgets['beta']:
                self.config['beta'] = xp.getWidgetProperty(inParam1, xp.Property_ButtonState, None) == 1
                xp.setWidgetDescriptor(self.window.widgets['currency'],
                                       Currency(self.Version, self.beta_version,
                                                self.new_version).get_currency(self.config.get('beta', False)))
                if self.setUpdateMenu is not None:
                    self.setUpdateMenu()

        if inMessage == xp.Msg_PushButtonPressed:
            if inParam1 == self.window.widgets['documentation']:
                webbrowser.open('https://xppython3.rtfd.io/en/latest/usage/installation_plugin.html')
                return 1

            if inParam1 == self.window.widgets['support']:
                webbrowser.open('https://forums.x-plane.org/index.php?/forums/topic/225976-xppython3-now-available/')
                return 1

            if inParam1 == self.window.widgets['plugin']:
                webbrowser.open('https://xppython3.rtfd.io/en/latest/development/index.html')
                return 1

            if inParam1 == self.window.widgets['donate']:
                webbrowser.open('https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=TA3EJ9VWFCH3N&source=url')
                return 1

            if inParam1 == self.window.widgets['changelog']:
                webbrowser.open('https://xppython3.rtfd.io/en/latest/changelog.html')
                return 1
        return 0
