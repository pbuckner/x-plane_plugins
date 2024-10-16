from XPPython3 import xp
from XPPython3.utils.widgetMsgHelper import WidgetMsgHelper


class PythonInterface:
    def __init__(self):
        self.Name = "HelloWidget1"
        self.Sig = "xppython3.demos.helloWidget1"
        self.Desc = "A test plugin for the Python Interface."
        self.myWidgetWindow = None
        self.widgetMsgCounter = {}  # Count number of messages per type

    def XPluginStart(self):
        return self.Name, self.Sig, self.Desc

    def XPluginEnable(self):
        self.myWidgetWindow = self.createWidgetWindow()
        return 1

    def XPluginDisable(self):
        if self.myWidgetWindow:
            xp.destroyWidget(self.myWidgetWindow['widgetID'], 1)
            self.myWidgetWindow = None

    def createWidgetWindow(self):
        widgetWindow = {'widgetID': None,  # the ID of the main window containing all other widgets
                        'widgets': {}      # dict() of all child widgets we care about
        }
        widgetWindow['widgetID'] = xp.createWidget(100, 200, 600, 50, 1, "Widget Window Test",
                                                   1, 0, xp.WidgetClass_MainWindow)
        xp.addWidgetCallback(widgetWindow['widgetID'], self.widgetCallback)

        # Add five label / editable text fields.
        # We determine placement based on the size of the font.
        # We'll "remember" the text fields so we can interact with them
        fontID = xp.Font_Proportional
        _w, strHeight, _ignore = xp.getFontDimensions(fontID)
        for i in range(5):
            s = f'item {i}'
            strWidth = xp.measureString(fontID, s)
            left = 100 + 10
            top = int(160 - ((strHeight + 4) * i))
            right = int(left + strWidth)
            bottom = int(top - strHeight)
            xp.createWidget(left, top, right, bottom, 1, s, 0,
                            widgetWindow['widgetID'], xp.WidgetClass_Caption)
            widget = xp.createWidget(right + 10, top, right + 100, bottom, 1, f'val {i}', 0,
                                     widgetWindow['widgetID'],
                                     xp.WidgetClass_TextField)
            widgetWindow['widgets'][f'textfield-{i}'] = widget

        # add a button at the bottom
        s = "Hello Widget"
        strWidth = xp.measureString(fontID, s)
        left = 100 + 10 + 20
        top = int(150 - (5 + (strHeight + 4) * 5))
        right = int(left + strWidth + 20)
        bottom = int(top - strHeight)
        widgetWindow['widgets']['button'] = xp.createWidget(left, top, right, bottom,
                                                            1, s, 0, widgetWindow['widgetID'],
                                                            xp.WidgetClass_Button)
        return widgetWindow

    def widgetCallback(self, inMessage, inWidget, inParam1, inParam2):
        self.widgetMsgCounter[inMessage] = 1 + self.widgetMsgCounter.setdefault(inMessage, 0)
        if self.widgetMsgCounter[inMessage] < 10:
            # due to output volume, we'll print only the first 10 instances of each type of message.
            print(f'{inWidget} {WidgetMsgHelper(inMessage, inParam1, inParam2)}')
        if inMessage == xp.Msg_Paint:
            return 0  # so 'draw' is called
        if inMessage == xp.Msg_CursorAdjust:
            inParam2 = xp.CursorDefault
            return 1
        return 0  # forward message to "next"
