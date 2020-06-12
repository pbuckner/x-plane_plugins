from regression_base import RegressionBase
from XPLMUtilities import XPLMGetSystemPath
# definitions

# functions, tested
from XPLMPlanes import XPLMCountAircraft, XPLMGetNthAircraftModel
from XPLMPlanes import XPLMSetUsersAircraft, XPLMPlaceUserAtAirport
from XPLMPlanes import XPLMPlaceUserAtLocation
from XPLMPlanes import XPLMAcquirePlanes, XPLMReleasePlanes, XPLMSetAircraftModel
from XPLMPlanes import XPLMSetActiveAircraftCount, XPLMDisableAIForPlane

# functions not tested
# from XPLMPlanes import XPLMReinitUsersPlane DEPRECATED


class PythonInterface(RegressionBase):
    def __init__(self):
        self.Name = "Regression Test Planes"
        self.Sig = "Planes.XPython"
        self.Desc = "Regression test planes example"

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
            self.log("Step {} Count Planes".format(codeStep))
            total, active, controllerID = XPLMCountAircraft()
            self.log("There are {} total aircraft, {} are active, Plugin controlling is #{}".format(total, active, controllerID))
            for i in range(total):
                filename, path = XPLMGetNthAircraftModel(i)
                self.log('[{}] {}, {}'.format(i, filename, path))

        codeStep += 1
        if codeStep == self.testSteps[whichFlightLoop]:
            filename = XPLMGetSystemPath() + 'Aircraft/Laminar Research/Boeing B737-800/b738.acf'
            self.log("Step {} Set aircraft {}".format(codeStep, filename))
            XPLMSetUsersAircraft(filename)

        codeStep += 1
        if codeStep == self.testSteps[whichFlightLoop]:
            self.log("Step {} Move to Portland".format(codeStep))
            XPLMPlaceUserAtAirport('KPDX')

        codeStep += 1
        self.acquireCB = self.acquireCallback
        self.acquireRefCon = []
        if codeStep == self.testSteps[whichFlightLoop]:
            self.log("Step {} Acquire Planes".format(codeStep))

            if XPLMAcquirePlanes(None, self.acquireCB, self.acquireRefCon):
                # XPLMAcquirePlanes second parameter seems to be fully ignored.
                # To load/change an aircraft model, use XPLMSetAircraftModel() post acquire.
                self.acquireCB(self.acquireRefCon)

        codeStep += 1
        if codeStep == self.testSteps[whichFlightLoop]:
            self.log("Step {} DisableAI".format(codeStep))
            if XPLMAcquirePlanes(None, self.acquireCB, self.acquireRefCon):
                self.log("Number planes was: {}, setting one to inactive".format(XPLMCountAircraft()))
                XPLMDisableAIForPlane(1)
                self.log("Number planes now: {}".format(XPLMCountAircraft()))

        codeStep += 5
        if codeStep == self.testSteps[whichFlightLoop]:
            self.log("Step {} Set Active".format(codeStep))
            if XPLMAcquirePlanes(None, self.acquireCB, self.acquireRefCon):
                self.log("Number planes was: {}, setting to 2".format(XPLMCountAircraft()))
                XPLMSetActiveAircraftCount(2)
                self.log("Number planes now: {}".format(XPLMCountAircraft()))

        codeStep += 10
        if codeStep == self.testSteps[whichFlightLoop]:
            self.log("Step {} Move to KSFO Approach".format(codeStep))
            XPLMPlaceUserAtLocation(37.6213, -122.3790, 1000, 10, 90)

        codeStep += 5
        if codeStep == self.testSteps[whichFlightLoop]:
            self.log("Test Complete")
            return 0

        return 1.0

    def acquireCallback(self, refCon):
        self.log("Planes acquired, requesting: {}".format(refCon))
        total, active, controllerID = XPLMCountAircraft()
        self.log("There are {} total aircraft, {} are active, Plugin controlling is #{}".format(total, active, controllerID))
        for i in range(total):
            filename, path = XPLMGetNthAircraftModel(i)
            self.log('[{}] {}'.format(i, filename))

        barron = XPLMGetSystemPath() + 'Aircraft/Laminar Research/Baron B58/Baron_58.acf'
        for i in range(1, total):
            if i == 0:
                continue  # use XPLMSetUsersAircraft() instead
            self.log('setting #{}'.format(i))
            XPLMSetAircraftModel(i, barron)

        for i in range(total):
            filename, path = XPLMGetNthAircraftModel(i)
            self.log('NOW: [{}] {}'.format(i, filename))

        XPLMReleasePlanes()

    def drawCallback(self, inPhase, inIsBefore, inRefcon):
        self.drawCallbackCalled = 1
        self.checkVal('drawCallback: Unexpected inPhase', inPhase, self.drawPhase)
        self.checkVal('drawCallback: Unexpected inIsBefore', inIsBefore, self.drawBefore)
        self.checkVal('drawCallback: Unexpected inRefcon', inRefcon, self.drawRefcon)
        return 1
