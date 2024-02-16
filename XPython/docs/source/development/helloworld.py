from XPPython3 import xp

class PythonInterface:
    def __init__(self):
        self.name = "HelloWorld1"
        self.sig = "helloWorld1.demos.xppython3"
        self.desc = "A test plugin for the Python Interface."
        self.windowId = None

    def XPluginStart(self):
        windowInfo = (50, 600, 300, 400, 1,
                      self.drawWindowCallback,
                      self.mouseClickCallback,
                      self.keyCallback,
                      self.cursorCallback,
                      self.mouseWheelCallback,
                      0,
                      xp.WindowDecorationRoundRectangle,
                      xp.WindowLayerFloatingWindows,
                      None)
        self.windowId = xp.createWindowEx(windowInfo)
        return self.name, self.sig, self.desc

    def XPluginStop(self):
        xp.destroyWindow(self.windowId)

    def XPluginEnable(self):
        return 1

    def XPluginDisable(self):
        pass

    def XPluginReceiveMessage(self, inFromWho, inMessage, inParam):
        pass

    def drawWindowCallback(self, inWindowID, inRefcon):
        (left, top, right, bottom) = xp.getWindowGeometry(inWindowID)
        xp.drawTranslucentDarkBox(left, top, right, bottom)
        color = 1.0, 1.0, 1.0
        xp.drawString(color, left + 5, top - 20, "Hello World", 0, xp.Font_Basic)

    def keyCallback(self, inWindowID, inKey, inFlags, inVirtualKey, inRefcon, losingFocus):
        pass

    def mouseClickCallback(self, inWindowID, x, y, inMouse, inRefcon):
        return 1

    def cursorCallback(self, inWindowID, x, y, inRefcon):
        return xp.CursorDefault

    def mouseWheelCallback(self, inWindowID, x, y, wheel, clicks, inRefcon):
        return 1
