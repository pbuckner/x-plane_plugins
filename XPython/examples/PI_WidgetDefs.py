from regression_base import RegressionBase

# definitions
from XPWidgetDefs import xpProperty_Refcon, xpProperty_Dragging, xpProperty_DragXOff, xpProperty_DragYOff
from XPWidgetDefs import xpProperty_Hilited, xpProperty_Object, xpProperty_Clip, xpProperty_Enabled, xpProperty_UserStart
from XPWidgetDefs import xpMode_Direct, xpMode_UpChain, xpMode_Recursive, xpMode_DirectAllCallbacks, xpMode_Once
from XPWidgetDefs import xpMsg_None, xpMsg_Create, xpMsg_Destroy, xpMsg_Paint, xpMsg_Draw, xpMsg_KeyPress
from XPWidgetDefs import xpMsg_KeyTakeFocus, xpMsg_KeyLoseFocus, xpMsg_MouseDown, xpMsg_MouseDrag, xpMsg_MouseUp
from XPWidgetDefs import xpMsg_Reshape, xpMsg_ExposedChanged, xpMsg_AcceptChild, xpMsg_LoseChild, xpMsg_AcceptParent
from XPWidgetDefs import xpMsg_Shown, xpMsg_Hidden, xpMsg_DescriptorChanged, xpMsg_PropertyChanged, xpMsg_MouseWheel
from XPWidgetDefs import xpMsg_CursorAdjust, xpMsg_UserStart
# functions, tested

# functions not tested


class PythonInterface(RegressionBase):
    def __init__(self):
        self.Sig = "xppython3.widgetdefs"
        self.Name = "Regression Test {}".format(self.Sig)
        self.Desc = "Regression test {} example".format(self.Sig)
        self.flightLoopCalled = False

        super(PythonInterface, self).__init__(self.Sig)

    def XPluginStart(self):
        TEST_TO_RUN = 0
        whichFlightLoop = TEST_TO_RUN
        self.startFlightLoop(whichFlightLoop)
        return self.Name, self.Sig, self.Desc

    def XPluginStop(self):
        super(PythonInterface, self).stop()
        self.check()

    def XPluginEnable(self):
        super(PythonInterface, self).enable()
        return 1

    def XPluginDisable(self):
        super(PythonInterface, self).disable()

    def XPluginReceiveMessage(self, inFromWho, inMessage, inParam):
        super(PythonInterface, self).receive(inFromWho, inMessage, inParam)

    def flightLoopCallback(self, elapsedSinceLastCall, elapsedTimeSinceLastFlightLoop, counter, inRefcon):
        try:
            whichFlightLoop = [which for which in self.flightLoopReferenceConstants
                               if self.flightLoopReferenceConstants[which] == inRefcon][0]
        except IndexError:
            self.log("Cannot determine which flight loop for refcon: {}. Exiting flight loop".format(inRefcon))
            return 0

        if not self.flightLoopCalled:
            self.flightLoopCalled = elapsedTimeSinceLastFlightLoop
        elif elapsedTimeSinceLastFlightLoop - self.flightLoopCalled < (5 * self.testSteps[whichFlightLoop]):
            return 1.0

        if whichFlightLoop == 0:
            return self.mainFlightLoop(elapsedSinceLastCall, elapsedTimeSinceLastFlightLoop, counter, inRefcon)
        self.log("Flight Loop #{} unknown".format(whichFlightLoop))
        return 0

    def mainFlightLoop(self, elapsedSinceLastCall, elapsedTimeSinceLastFlightLoop, counter, inRefcon):
        whichFlightLoop = 0
        self.testSteps[whichFlightLoop] += 1
        self.log('[{}] flight loop called {:.2f}, {:.2f}, {}'.format(self.testSteps[whichFlightLoop],
                                                                     elapsedSinceLastCall,
                                                                     elapsedTimeSinceLastFlightLoop, counter))
        codeStep = 0
        if codeStep == self.testSteps[whichFlightLoop]:
            self.log("Step {}".format(codeStep))

        codeStep += 1
        if codeStep == self.testSteps[whichFlightLoop]:
            self.log("Step {}".format(codeStep))
            self.log("Test completed (only definitions)")

        return 1.0

    def drawCallback(self, inPhase, inIsBefore, inRefcon):
        self.drawCallbackCalled = 1
        self.checkVal('drawCallback: Unexpected inPhase', inPhase, self.drawPhase)
        self.checkVal('drawCallback: Unexpected inIsBefore', inIsBefore, self.drawBefore)
        self.checkVal('drawCallback: Unexpected inRefcon', inRefcon, self.drawRefcon)
        return 1
