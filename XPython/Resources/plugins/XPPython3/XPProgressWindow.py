import xp

class XPProgressWindow:
    """
    Simple Progress window, with single progress bar going 0.0 -> 1.0
    and a single optional caption line immediately under the progress bar.
    Finally, a centered "Close" button.

    To use:
       self.myprogress = XPProgressWindow('Title')
       self.show()
       self.setCaption("Downloading")
       self.setProgress(0)
       self.setProgress(.15)
       ...
       self.setCaption("Complete")

       # either self.hide(), or let the user click "Close"

       self.destroy()
    """

    def __init__(self, title):
        top = 500
        left = 100
        right = left + 300
        height = 125
        self.maxValue = 1000
        self.progressWindow = xp.createWidget(left, top, right, top - height, 0, title, 1, 0, xp.WidgetClass_MainWindow)

        top -= 25
        bottom = top - 20
        self.progressWidget = xp.createWidget(left + 10, top, right - 10, bottom, 1, title, 0, self.progressWindow, xp.WidgetClass_Progress)
        xp.setWidgetProperty(self.progressWidget, xp.Property_ProgressPosition, 0)
        xp.setWidgetProperty(self.progressWidget, xp.Property_ProgressMin, 0)
        xp.setWidgetProperty(self.progressWidget, xp.Property_ProgressMax, self.maxValue)

        _w, strHeight, _ignore = xp.getFontDimensions(xp.Font_Proportional)
        top = bottom - 10
        bottom = top - int(strHeight)
        self.captionWidget = xp.createWidget(left + 10, top, right - 10, bottom, 1, '', 0, self.progressWindow, xp.WidgetClass_Caption)

        top = bottom - 25
        bottom = top - 25
        middle = int((right + left) / 2)
        self.button = xp.createWidget(middle - 25, top, middle + 25, bottom, 1, 'Close', 0, self.progressWindow, xp.WidgetClass_Button)
        xp.setWidgetProperty(self.button, xp.Property_ButtonType, xp.PushButton)
        xp.setWidgetProperty(self.button, xp.Property_ButtonBehavior, xp.ButtonBehaviorPushButton)

        xp.addWidgetCallback(self.progressWindow, self.progressWindowCallback)

    def progressWindowCallback(self, inMessage, inWidget, inParam1, inParam2):
        if inMessage == xp.Msg_PushButtonPressed and inParam1 == self.button:
            self.hide()
            return 1
        return 0

    def destroy(self):
        xp.destroyWidget(self.progressWindow, 1)

    def hide(self):
        xp.hideWidget(self.progressWindow)

    def show(self):
        xp.showWidget(self.progressWindow)

    def setProgress(self, value=0):
        xp.setWidgetProperty(self.progressWidget, xp.Property_ProgressPosition, self.maxValue * value if value < 1 else 1)

    def setCaption(self, caption=''):
        xp.setWidgetDescriptor(self.captionWidget, caption)
