from check_helper import checkBase

from XPLMProcessing import XPLMRegisterFlightLoopCallback
from XPLMDisplay import XPLMRegisterDrawCallback, XPLMUnregisterDrawCallback, xplm_Phase_Window

from XPLMDefs import *
from XPLMGraphics import xplm_Tex_GeneralInterface
from XPLMGraphics import xplmFont_Basic, xplmFont_Proportional
# tested
from XPLMGraphics import XPLMWorldToLocal, XPLMLocalToWorld
from XPLMGraphics import XPLMDrawTranslucentDarkBox
from XPLMGraphics import XPLMDrawString, XPLMDrawNumber, XPLMGetFontDimensions, XPLMMeasureString
from XPLMGraphics import XPLMGenerateTextureNumbers

# to be tested
from XPLMGraphics import XPLMSetGraphicsState

# untested
from XPLMGraphics import XPLMBindTexture2d


class PythonInterface(checkBase):
    def __init__(self):
        checkBase.__init__(self, 'Graphics')
        checkBase.addRef()
        self.testSteps = {}
        self.flightLoopCalled = 0
        self.flightLoopReferenceConstants = {}
        self.drawCallbackFun = self.drawCallback
        self.drawPhase = xplm_Phase_Window
        self.drawBefore = 0
        self.drawRefcon = 'draw'
        return

    def XPluginStart(self):
        self.Sig = "XPython.Graphics"
        self.Name = "{} module test".format(self.Sig)
        self.Desc = "Module testing the {} interface.".format(self.Sig)

        return self.Name, self.Sig, self.Desc

    def XPluginStop(self):
        self.check()
        XPLMUnregisterDrawCallback(self.drawCallbackFun, self.drawPhase, self.drawBefore, self.drawRefcon)
        checkBase.remRef()

    def XPluginEnable(self):
        whichFlightLoop = 0
        self.flightLoopReferenceConstants[whichFlightLoop] = 'main'
        XPLMRegisterFlightLoopCallback(self.flightLoopCallback, -1.0,
                                       self.flightLoopReferenceConstants[whichFlightLoop])
        XPLMRegisterDrawCallback(self.drawCallbackFun, self.drawPhase,
                                 self.drawBefore, self.drawRefcon)
        self.testSteps[whichFlightLoop] = 0

        numbers = XPLMGenerateTextureNumbers(3)
        print('Texture numbers: {}'.format(numbers))

        return 1

    def XPluginDisable(self):
        return

    def XPluginReceiveMessage(self, inFromWho, inMessage, inParam):
        self.xpluginMessageReceived = 1
        return

    def flightLoopCallback(self, elapsedSinceLastCall, elapsedTimeSinceLastFlightLoop, counter, inRefcon):
        x, y, z = XPLMWorldToLocal(35, -119, 1000)
        lat, lng, alt = XPLMLocalToWorld(x, y, z)
        print("Lat: 35 -> {}, Lng: -119 -> {}, Alt: 1000 -> {}".format(lat, lng, alt))
        return 0

    def drawCallback(self, inPhase, inIsBefore, inRefcon):
        XPLMSetGraphicsState(0, 1, 0, 0, 0, 0, 0)
        XPLMDrawTranslucentDarkBox(100, 200, 500, 100)
        width = XPLMMeasureString(xplmFont_Basic, "Hello World", len("Hello World"))
        (w, h, d) = XPLMGetFontDimensions(xplmFont_Basic)
        average = w
        XPLMDrawString([.9, 0, 0], 110, 175, "Hello World: {}, ave: {}".format(width, average), None, xplmFont_Basic)
        XPLMDrawNumber([.9, 5, .9], 110, 150, 3.1467, 5, 3, 1, xplmFont_Basic)  # --> 03.147
        return 1
