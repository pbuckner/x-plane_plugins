from typing import Self, Any
import re
from XPPython3 import xp
from XPPython3.xp_typing import XPLMCommandRef, XPLMCommandPhase, XPWidgetMessage, XPWidgetID
from XPPython3.utils import xp_pip
from .my_widget_window import MyWidgetWindow


class Pip:
    def __init__(self: Self) -> None:
        self.window = MyWidgetWindow()

    def toggleCommand(self: Self, _inCommand: XPLMCommandRef, inPhase: XPLMCommandPhase, _refCon: Any) -> int:
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
        box_left = 100
        box_right = 500
        top = 300

        self.window.widgetID = xp.createWidget(box_left, top, box_right, top - 125, 1, "PIP Package Installer",
                                               1, 0, xp.WidgetClass_MainWindow)
        xp.setWidgetProperty(self.window.widgetID, xp.Property_MainWindowHasCloseBoxes, 1)
        fontID = xp.Font_Proportional
        _w, strHeight, _ignore = xp.getFontDimensions(fontID)

        xp.addWidgetCallback(self.window.widgetID, self.pipWidgetCallback)
        left = box_left + 10
        top -= 30
        bottom = int(top - strHeight)

        # Instructions....
        s = "Enter one or more python packages below, then press 'Install'."
        strWidth = xp.measureString(fontID, s)
        right = int(left + strWidth)
        xp.createWidget(left, top, right, bottom, 1, s, 0, self.window.widgetID,
                        xp.WidgetClass_Caption)
        top -= strHeight + 4
        bottom = int(top - strHeight)

        # text field
        bottom = int(top - (strHeight + 4))
        right = int(left + strWidth)
        self.window.widgets['packages'] = xp.createWidget(left, top, right, bottom, 1, '', 0, self.window.widgetID,
                                                          xp.WidgetClass_TextField)
        top -= strHeight + 8
        bottom = int(top - strHeight)

        # Hints....
        s = "Common packages: requests, pyopengl, cryptography, urllib3, six"
        strWidth = xp.measureString(fontID, s)
        right = int(left + strWidth)
        xp.createWidget(left, top, right, bottom, 1, s, 0, self.window.widgetID,
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
        self.window.widgets['button'] = xp.createWidget(int(middle - strWidth / 2.0), top,
                                                        int(middle + strWidth / 2.0), bottom,
                                                        1, s, 0, self.window.widgetID,
                                                        xp.WidgetClass_Button)
        top = bottom - 10
        bottom = int(top - strHeight)
        right = left + 400
        self.window.widgets['error'] = xp.createWidget(left, top, right, bottom,
                                                       1, '', 0, self.window.widgetID,
                                                       xp.WidgetClass_Caption)

    def pipWidgetCallback(self:Self, inMessage: XPWidgetMessage, _inWidget: XPWidgetID, inParam1: Any, _inParam2: Any) -> int:
        if inMessage == xp.Message_CloseButtonPushed:
            if self.window.widgetID is not None:
                xp.hideWidget(self.window.widgetID)
            return 1

        if any([inMessage == xp.Msg_KeyPress and inParam1[2] == xp.VK_RETURN and inParam1[1] & xp.DownFlag,
                inMessage == xp.Msg_PushButtonPressed and inParam1 == self.window.widgets['button']]):
            s = xp.getWidgetDescriptor(self.window.widgets['packages'])
            packages = list(filter(lambda x: x != '', re.split('[, ]+', s)))
            xp.log(f"Looking to install packages: {packages}")
            if packages:
                xp.setWidgetDescriptor(self.window.widgets['error'],
                                       f"Looking to install packages: {' '.join(packages)}")
                xp_pip.load_packages(packages, end_message="This information has been added to XPPython3 log file.")
            xp.setWidgetDescriptor(self.window.widgets['packages'], '')
            return 1

        return 0
