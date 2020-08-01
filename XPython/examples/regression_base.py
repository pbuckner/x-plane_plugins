from check_helper import checkBase
from XPLMDataAccess import XPLMFindDataRef, XPLMGetDatai
from XPLMDisplay import XPLMRegisterDrawCallback, XPLMUnregisterDrawCallback
from XPLMDisplay import XPLMDestroyWindow, xplm_Phase_Window
from XPLMProcessing import XPLMRegisterFlightLoopCallback, XPLMUnregisterFlightLoopCallback


class RegressionBase(checkBase):

    def __init__(self, *args):
        super(RegressionBase, self).__init__(*args)
        self.xpluginStopCalled = 0
        self.xpluginEnableCalled = 0
        self.xpluginDisableCalled = 0
        self.xpluginMessageReceived = 0
        self.flightLoopCalled = 0
        self.drawCallbackCalled = 0

        self.flightLoopReferenceConstants = {}  # indexed by whichFlightLoop
        self.testSteps = {}  # indexed by whichFlightLoop

        self.winRefcon = {}  # indexed by winID
        self.winID = {}  # indexed by "which"

        self.drawCallbackFun = self.drawCallback
        self.drawPhase = xplm_Phase_Window
        self.drawBefore = 0
        self.drawRefcon = ['draw callback']

        self.is_modern = XPLMGetDatai(XPLMFindDataRef('sim/graphics/view/using_modern_driver'))
        self.log("Running {}".format('Vulkan/Metal' if self.is_modern else 'OpenGL'))
        self.addRef()

    def startFlightLoop(self, whichFlightLoop):
        self.flightLoopReferenceConstants[whichFlightLoop] = '{}:FlightLoop'.format(whichFlightLoop)
        XPLMRegisterFlightLoopCallback(self.flightLoopCallback, -1.0, self.flightLoopReferenceConstants[whichFlightLoop])
        self.testSteps[whichFlightLoop] = -1

    def enable(self):
        self.xpluginEnableCalled = 1
        self.drawCallbackFun = self.drawCallback
        self.checkVal("Draw callback registration",
                      XPLMRegisterDrawCallback(self.drawCallbackFun, self.drawPhase,
                                               self.drawBefore, self.drawRefcon),
                      1)
        return 1

    def receive(self, inFromWho, inMessage, inParams):
        self.xpluginMessageReceived = 1

    def disable(self):
        self.xpluginDisableCalled = 1
        return 1

    def stop(self):
        self.xpluginStopCalled = 1

        self.checkVal("Draw callback called", self.drawCallbackCalled, 1)
        if self.drawCallbackCalled:
            self.checkVal('UnregisterDrawCallback',
                          XPLMUnregisterDrawCallback(self.drawCallbackFun, self.drawPhase,
                                                     self.drawBefore, self.drawRefcon),
                          1)

        for which in self.winID:
            self.log("Looking to destroy window [{}] with id: {}".format(which, self.winID[which]))
            XPLMDestroyWindow(self.winID[which])

        for flightLoop in self.flightLoopReferenceConstants:
            self.log("Looking to unregister flight loop [{}] {}".format(flightLoop,
                                                                        self.flightLoopReferenceConstants[flightLoop]))
            XPLMUnregisterFlightLoopCallback(self.flightLoopCallback,
                                             self.flightLoopReferenceConstants[flightLoop])
        self.log("Stopped")

    def flightLoopCallback(self, elapsedSinceLastCall, elapsedTimeSinceLastFlightLoop, counter, inRefcon):
        """
        We return 0 to stop the callback -- You should provide your own flightLoopCallback
        """
        return 0

    def drawCallback(self, inPhase, inIsBefore, inRefcon):
        """
        We return 0 to stop the callback -- You should provide your own drawCallback
        """
        self.drawCallbackCalled = 1
        self.checkVal('drawCallback: Unexpected inPhase', inPhase, self.drawPhase)
        self.checkVal('drawCallback: Unexpected inIsBefore', inIsBefore, self.drawBefore)
        self.checkVal('drawCallback: Unexpected inRefcon', inRefcon, self.drawRefcon)
        return 0

    def checkRightWindow(self, prompt, which, inWindowID, inRefcon):
        if (inWindowID == self.winID[which]) and (inRefcon == self.winRefcon[self.winID[which]]):
            return True
        self.error((' ** Error ** {0} !!!! unexpected windowID and refcon combination: ' +
                    'got (0x{1:x}, {2}), expected (0x{3:x}, {4})').format(
                        prompt, inWindowID, inRefcon, self.winID[which], self.winRefcon[self.winID[which]]))
        return

    def check(self):
        if not self.xpluginStopCalled:
            self.error('xpluginReceiveMessage was not called!')
        if not self.xpluginEnableCalled:
            self.error('pluginEnable was not called!')
        if not self.xpluginDisableCalled:
            self.error('pluginDisable was not called!')
        if not self.xpluginMessageReceived:
            self.error('xpluginReceiveMessage was not called!')
        checkBase.check(self)
        checkBase.remRef()
