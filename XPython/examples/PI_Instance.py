from regression_base import RegressionBase
from XPLMDataAccess import XPLMGetDatad, XPLMFindDataRef
from XPLMScenery import XPLMLoadObject, XPLMLookupObjects

# definitions

# functions, tested
from XPLMInstance import XPLMCreateInstance, XPLMDestroyInstance
from XPLMInstance import XPLMInstanceSetPosition

# functions not tested


class PythonInterface(RegressionBase):
    def __init__(self):
        self.Name = "Regression Test Instance"
        self.Sig = "Instance.XPython"
        self.Desc = "Regression test Instance example"
        self.g_object = None
        self.g_objPath = 'lib/airport/vehicles/pushback/tug.obj'
        # Resources/default scenery/sim objects/library.txt exports the tug.obj, from
        # Resources/default scenery/sim objects/apt_vehicles/pushback/Tug_GT110.obj
        self.g_instance = None
        self.g_tire = 0.0

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
        return 1

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

        if whichFlightLoop == 0:
            return self.mainFlightLoop(elapsedSinceLastCall, elapsedTimeSinceLastFlightLoop, counter, inRefcon)
        self.log("Flight Loop #{} unknown".format(whichFlightLoop))
        return 0

    def load_cb(self, real_path, ref):
        self.log('callback')
        if ref is None:
            self.g_object = XPLMLoadObject(real_path)
            self.log("loaded g_object: {}".format(self.g_object))

    def mainFlightLoop(self, elapsedSinceLastCall, elapsedTimeSinceLastFlightLoop, counter, inRefcon):
        whichFlightLoop = 0
        self.testSteps[whichFlightLoop] += 1
        self.log('[{}] flight loop called {:.2f}, {:.2f}, {}'.format(self.testSteps[whichFlightLoop],
                                                                     elapsedSinceLastCall,
                                                                     elapsedTimeSinceLastFlightLoop, counter))
        codeStep = 0
        if not self.g_object:
            XPLMLookupObjects(self.g_objPath, 0, 0, self.load_cb, self.g_object)

        if codeStep == self.testSteps[whichFlightLoop]:
            self.log("Step {}".format(codeStep))

            if self.g_object:
                # The object we selected (above) has a few already defined datarefs,
                # We list the ones we want to pass in (i.e., we want to control)
                # len(drefs) in XPLMCreateInstance == len(data) in XPLMInstanceSetPosition
                # We don't need to create callback _in_this_case_ it's a library
                # object and the sim/graphics/animation/ground_traffic/tire_steer_deg dataref already
                # exists.
                # (we could check that, to be sure)
                #
                # The result is we'll load the tag and then move the tires back and forth
                if XPLMFindDataRef('sim/graphics/animation/ground_traffic/tire_steer_deg'):
                    self.log('tire_steer already exists')
                drefs = ['sim/graphics/animation/ground_traffic/tire_steer_deg', 'foo/bar/ground']
                if not self.g_instance:
                    self.g_instance = XPLMCreateInstance(self.g_object, drefs)
            else:
                self.log("no g_object yet... repeating step")
                self.testSteps[whichFlightLoop] -= 1
                return 1.0

        codeStep += 1
        if self.testSteps[whichFlightLoop] < 40:
            self.log("Setting instance")
            x = XPLMGetDatad(XPLMFindDataRef('sim/flightmodel/position/local_x'))
            y = XPLMGetDatad(XPLMFindDataRef('sim/flightmodel/position/local_y'))
            z = XPLMGetDatad(XPLMFindDataRef('sim/flightmodel/position/local_z'))
            pitch, heading, roll = (1, 2, 3)
            position = (x, y, z, pitch, heading, roll)
            self.g_tire += 10.0
            if self.g_tire > 45.0:
                self.g_tire -= 90.0

            XPLMInstanceSetPosition(self.g_instance,
                                    position,
                                    [self.g_tire, 0.0])

        codeStep = 41
        if codeStep == self.testSteps[whichFlightLoop]:
            self.log("Step {}".format(codeStep))
            XPLMDestroyInstance(self.g_instance)

        return .1

    def drawCallback(self, inPhase, inIsBefore, inRefcon):
        self.drawCallbackCalled = 1
        self.checkVal('drawCallback: Unexpected inPhase', inPhase, self.drawPhase)
        self.checkVal('drawCallback: Unexpected inIsBefore', inIsBefore, self.drawBefore)
        self.checkVal('drawCallback: Unexpected inRefcon', inRefcon, self.drawRefcon)
        return 1
