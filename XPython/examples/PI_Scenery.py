from regression_base import RegressionBase
from XPLMPlanes import XPLMPlaceUserAtAirport
from XPLMGraphics import XPLMWorldToLocal, XPLMLocalToWorld
# definitions
from XPLMScenery import xplm_ProbeY
from XPLMScenery import xplm_ProbeHitTerrain, xplm_ProbeError
from XPLMScenery import xplm_ProbeMissed
# functions, tested
from XPLMScenery import XPLMCreateProbe, XPLMDestroyProbe, XPLMProbeTerrainXYZ
from XPLMScenery import XPLMGetMagneticVariation, XPLMDegTrueToDegMagnetic, XPLMDegMagneticToDegTrue
from XPLMScenery import XPLMLoadObject, XPLMLoadObjectAsync, XPLMUnloadObject
from XPLMScenery import XPLMLookupObjects

# functions not tested

# from XPLMScenery import XPLMDrawObject (DEPRECATED)


class PythonInterface(RegressionBase):
    def __init__(self):
        self.Name = "Regression Test Scenery"
        self.Sig = "xppython3.scenery"
        self.Desc = "Regression test scenery example"

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
            self.log("Step {} Move to SFO".format(codeStep))
            # Probe should be "near" the spot where the user aircraft is, as only nearby
            # information is loaded
            XPLMPlaceUserAtAirport('KSFO')

        codeStep += 1
        if codeStep == self.testSteps[whichFlightLoop]:
            self.log("Step {} Terrain Probe".format(codeStep))
            probe = XPLMCreateProbe(xplm_ProbeY)
            lat, lng = (37.75168, -122.44767)
            (x, y, z) = XPLMWorldToLocal(37.75168, -122.44767, 0)  # Twin Peaks, San Francisco, CA
            info = XPLMProbeTerrainXYZ(probe, x, y, z)
            self.checkVal("XPLMProbleTerrainXYZ", info.result, 0)
            if info.result == xplm_ProbeHitTerrain:
                self.log("Terrain info is [{}] {}".format(info.result, info))
                (lat, lng, alt) = XPLMLocalToWorld(info.locationX, info.locationY, info.locationZ)  # Twin Peaks, San Francisco, CA
                self.log('lat, lng, alt is {} feet'.format((lat, lng, alt * 3.28)))
            elif info.result == xplm_ProbeError:
                self.log("Terrain error")
            elif info.result == xplm_ProbeMissed:
                self.log("Terrain Missed")

            lat, lng = (39.7392, -104.9903)
            (x, y, z) = XPLMWorldToLocal(lat, lng, 0)  # Denver CO
            info = XPLMProbeTerrainXYZ(probe, x, y, z)
            self.checkVal("XPLMProbleTerrainXYZ", info.result, 0)
            #  .. this "succeeds" but returns altitude near zero because we're too far away
            if info.result == xplm_ProbeHitTerrain:
                self.log("Terrain info is [{}] {}".format(info.result, info))
                (lat, lng, alt) = XPLMLocalToWorld(info.locationX, info.locationY, info.locationZ)
                self.log('lat, lng, alt is {} feet'.format((lat, lng, alt * 3.28)))
            elif info.result == xplm_ProbeError:
                self.log("Terrain error")
            elif info.result == xplm_ProbeMissed:
                self.log("Terrain Missed")
            XPLMDestroyProbe(probe)

        codeStep += 1
        if codeStep == self.testSteps[whichFlightLoop]:
            self.log("Step {}, Move user to Denver".format(codeStep))
            XPLMPlaceUserAtAirport('KDEN')

        codeStep += 1
        if codeStep == self.testSteps[whichFlightLoop]:
            self.log("Step {}, but with probe closer to Denver".format(codeStep))
            probe = XPLMCreateProbe(xplm_ProbeY)
            lat, lng = (39.7392, -104.9903)
            (x, y, z) = XPLMWorldToLocal(lat, lng, 0)  # Denver CO .. this return proper altitude
            info = XPLMProbeTerrainXYZ(probe, x, y, z)
            if info.result == xplm_ProbeHitTerrain:
                self.log("Terrain info is [{}] {}".format(info.result, info))
                (lat, lng, alt) = XPLMLocalToWorld(info.locationX, info.locationY, info.locationZ)
                self.log('lat, lng, alt is {} feet'.format((lat, lng, alt * 3.28)))
            elif info.result == xplm_ProbeError:
                self.log("Terrain error")
            elif info.result == xplm_ProbeMissed:
                self.log("Terrain Missed")

            XPLMDestroyProbe(probe)

        codeStep += 1
        if codeStep == self.testSteps[whichFlightLoop]:
            self.log("Step {}, Mag variation, Denver".format(codeStep))
            lat, lng = (39.7392, -104.9903)
            self.log("Magnetic variation, Denver (the city): {}".format(XPLMGetMagneticVariation(lat, lng)))
            # (based on current user postion)
            self.log("True to Magnetic @ KDEN airport: {}".format(XPLMDegTrueToDegMagnetic(0.0)))
            self.log("Magnetic to True @ KDEN airport: {}".format(XPLMDegMagneticToDegTrue(0.0)))

        codeStep += 1
        if codeStep == self.testSteps[whichFlightLoop]:
            self.log("Step {}, Lookup objects availabl at Denver".format(codeStep))
            self.objEnumerator = self.enumeratorCallback
            self.objects = []
            self.objPath = 'lib/airport/landscape/windsock.obj'
            lat, lng = (39.7392, -104.9903)
            XPLMLookupObjects(self.objPath, lat, lng, self.objEnumerator, self.objects)

            for idx, path in enumerate(self.objects):
                self.log('XPLMLookupData returned {}', path)

            ref = XPLMLoadObject(path)
            if not ref:
                self.error("LoadObject({}) failed".format(path))
            else:
                self.log("Object loaded")
                XPLMUnloadObject(ref)

            self.log("Calling Async version")
            self.loadCB = self.loadCallback
            self.loadRef = ['path: {}'.format(path), ]
            XPLMLoadObjectAsync(path, self.loadCB, self.loadRef)
            self.log("Test complete")

            return 0

        return 1.0

    def loadCallback(self, objRef, refCon):
        self.log("AsyncLoaded")
        XPLMUnloadObject(objRef)

    def enumeratorCallback(self, inFilePath, inRef):
        inRef.append(inFilePath)
        self.checkVal('EnumeratorCallback path inconsistent', self.objects[-1], inRef[-1])
