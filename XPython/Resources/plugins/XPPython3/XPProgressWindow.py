import xp


class XPProgressWindow:
    """
    Simple Progress window, with single progress bar going 0.0 -> 1.0
    and a single optional caption line immediately under the progress bar.
    (if you provide a caption with a new line ('\n') in it, we'll split to display
    the value on two lines.)
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

    def __init__(self, title, num_captions=2):
        windowTop = 500
        left = 100
        # make sure the box is wide enough for the title
        width = max(300, 20 + int(xp.measureString(xp.Font_Proportional, title)))
        right = left + width
        height = self.calcWindowHeight(num_captions)
        self.captionWidgets = []
        self.maxValue = 1000
        self.progressWindow = xp.createWidget(left, windowTop, right, windowTop - height, 0, title, 1, 0, xp.WidgetClass_MainWindow)
        self.progressWidget = xp.createWidget(left + 10, windowTop - 25, right - 10, windowTop - 45, 1, title, 0,
                                              self.progressWindow, xp.WidgetClass_Progress)
        xp.setWidgetProperty(self.progressWidget, xp.Property_ProgressPosition, 0)
        xp.setWidgetProperty(self.progressWidget, xp.Property_ProgressMin, 0)
        xp.setWidgetProperty(self.progressWidget, xp.Property_ProgressMax, self.maxValue)

        for i in range(num_captions):
            top, bottom = self.calcCaptionOffset(i)
            self.captionWidgets.append(xp.createWidget(left + 10, windowTop - top, right - 10, windowTop - bottom, 1, '', 0,
                                                       self.progressWindow, xp.WidgetClass_Caption))

        middle = int((right + left) / 2)
        top, bottom = self.calcButtonOffset(num_captions)

        self.button = xp.createWidget(middle - 25, windowTop - top, middle + 25, windowTop - bottom, 1, 'Close', 0,
                                      self.progressWindow, xp.WidgetClass_Button)
        xp.setWidgetProperty(self.button, xp.Property_ButtonType, xp.PushButton)
        xp.setWidgetProperty(self.button, xp.Property_ButtonBehavior, xp.ButtonBehaviorPushButton)

        winID = xp.getWidgetUnderlyingWindow(self.progressWindow)
        xp.setWindowPositioningMode(winID, xp.WindowCenterOnMonitor, -1)
        xp.addWidgetCallback(self.progressWindow, self.progressWindowCallback)

    def progressWindowCallback(self, inMessage, inWidget, inParam1, inParam2):
        if inMessage == xp.Msg_PushButtonPressed and inParam1 == self.button:
            self.hide()
            return 1
        return 0

    def calcWindowHeight(self, num_captions):
        return self.calcButtonOffset(num_captions)[1] + 8

    def calcButtonOffset(self, num_captions):
        top, bottom = self.calcCaptionOffset(num_captions - 1)
        # bottom is now bottom of last widget
        return bottom + 15, bottom + 40

    def calcCaptionOffset(self, captionNumber):
        """
        captionNumber goes 0..n
        returns (top, bottom), positive offsets from the window's top
        """
        _w, strHeight, _ignore = xp.getFontDimensions(xp.Font_Proportional)
        strHeight = int(strHeight)
        top = 55 + ((7 + strHeight) * captionNumber)
        return top, top + strHeight

    def destroy(self):
        xp.destroyWidget(self.progressWindow, 1)

    def hide(self):
        xp.hideWidget(self.progressWindow)

    def show(self):
        xp.showWidget(self.progressWindow)

    def setProgress(self, value=0):
        xp.setWidgetProperty(self.progressWidget, xp.Property_ProgressPosition, int(self.maxValue * (value if value < 1 else 1)))

    def setCaption(self, caption=''):
        caption_text = caption.split('\n')
        for i in range(len(self.captionWidgets)):
            if i < len(caption_text):
                xp.setWidgetDescriptor(self.captionWidgets[i], caption_text[i])
            else:
                xp.setWidgetDescriptor(self.captionWidgets[i], '')
