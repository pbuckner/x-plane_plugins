try:
    from XPListBox import XPCreateListBox
except ImportError:
    print("XPListBox is a custom python file provided with XPPython3, and required by this example you could copy it into PythonPlugins folder")
    raise
import xp


class PythonInterface:
    def __init__(self):
        self.Name = "ListBox"
        self.Sig = "listbox.demos.xppython3"
        self.Desc = "Demonstrate ListBox"
        self.windowWidgetID = None
        self.listboxWidget = None
        self.listboxData = None

    def clearDisplay(self):
        self.listboxWidget.clear()

    def display(self, s):
        self.listboxWidget.add(s)

    def XPluginStart(self):
        return self.Name, self.Sig, self.Desc

    def XPluginStop(self):
        pass

    def XPluginEnable(self):
        self.createLoggingWindow()
        self.display("First Message!")
        for x in range(10):
            self.display("... and {}".format(x))
        return 1

    def XPluginDisable(self):
        if self.windowWidgetID:
            self.clearDisplay()
            xp.destroyWidget(self.windowWidgetID, 1)

    def XPluginReceiveMessage(self, inFromWho, inMessage, inParam):
        self.display("In message: {}".format(inMessage))

    def createLoggingWindow(self, num_rows=16, num_characters=75):
        # Rather than hard-code a fixed size of scrolling area of list box,
        # we'll set size of box based on input parameters
        FontWidth, FontHeight, _other = xp.getFontDimensions(xp.Font_Basic)
        listbox_item_height = int(FontHeight * 1.2)
        left = 100
        bottom = 50

        top = bottom + (listbox_item_height) * num_rows
        right = left + int(num_characters * FontWidth)

        self.windowWidgetID = xp.createWidget(left - 5, top + 20, right + 5, bottom - 5, 1, "XPPython3", 1, 0, xp.WidgetClass_MainWindow)
        self.listboxWidget = XPCreateListBox(left, top, right, bottom, 1, self.windowWidgetID)
