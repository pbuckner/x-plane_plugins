from typing import Self, Optional, Any
from XPPython3 import xp
from XPPython3.XPListBox import XPListBox
from XPPython3.xp_typing import XPWidgetID, XPWidgetMessage, XPLMFontID


class ScrollingPopup(XPListBox):
    """
    Popup window with title and close buttons,
    Window includes single, writable scrolling widget, fitting the full height.
    (Only vertically scrolling.)

    Window is shown immediately and deleted on close.
    """
    def __init__(self: Self, title: str, left: int = 100, top: int = 400, right: int = 400, bottom: int = 100, _fontID: Optional[int] = None) -> None:
        self.mainWindowID = xp.createWidget(left, top, right, bottom, 1, title,
                                            1, 0, xp.WidgetClass_MainWindow)

        xp.setWidgetProperty(self.mainWindowID, xp.Property_MainWindowHasCloseBoxes, 1)
        xp.addWidgetCallback(self.mainWindowID, self.popupCallback)
        super().__init__(left, top - 20, right + 2, bottom, 1, '', self.mainWindowID)

    def popupCallback(self: Self, inMessage: XPWidgetMessage, inWidget: XPWidgetID, _inParam1: Any, _inParam2: Any) -> int:
        if inMessage == xp.Message_CloseButtonPushed:
            xp.hideWidget(inWidget)  # i.e., self.mainWindowID
            self.destroy()  # destroys the scrolling bits from XPListBox
            xp.destroyWidget(inWidget)
            return 1
        return 0


class Popup:
    """
    Simple popup window with title & closebuttons, and multiple lines displayed
    Window is _sized_to_fit_ all lines, without scrolling.
    Positions window using left, bottom corner,

    Do not use this if a line is going to be wider than the screen,
    or if there are so many lines it won't fit vertically. I do not error check.

    Window is shown immediately and hidden on close.
    If/when the Popup Instance goes out of scope, it wil be deleted (so
    you will need to 'hold' a reference to it, to keep it displayed!)
    """
    def __init__(self: Self, title: str, lines: list[str], left: int = 100, bottom: int = 400, fontID: Optional[XPLMFontID] = None):
        fontID = fontID or xp.Font_Proportional
        _w, strHeight, _ignore = xp.getFontDimensions(fontID)

        top = bottom + 25
        widest = 10
        for line in lines:
            top += strHeight + 4
            widest = int(max(widest, xp.measureString(fontID, line.strip())))

        self.main: Optional[XPWidgetID] = xp.createWidget(left, top, int(left + widest + 10), bottom,
                                                          1, title, 1, 0, xp.WidgetClass_MainWindow)
        xp.setWidgetProperty(self.main, xp.Property_MainWindowHasCloseBoxes, 1)
        xp.addWidgetCallback(self.main, self.popupCallback)

        left += 4
        top -= 20
        for line in lines:
            bottom = top - strHeight
            right = int(left + xp.measureString(fontID, line.strip()))
            xp.createWidget(left, top, right, bottom,
                            1, line.strip(), 0, self.main,
                            xp.WidgetClass_Caption)
            top -= strHeight + 4

    def popupCallback(self: Self, inMessage: XPWidgetMessage, inWidget: XPWidgetID, _inParam1: Any, _inParam2: Any) -> int:
        if inMessage == xp.Message_CloseButtonPushed:
            xp.hideWidget(inWidget)
            # xp.destroyWidget(inWidget)
            return 1
        return 0

    def __del__(self: Self) -> None:
        if self.main is not None:
            xp.destroyWidget(self.main)
            self.main = None
