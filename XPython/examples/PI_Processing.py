from check_helper import checkBase

from XPLMProcessing import xplm_FlightLoop_Phase_AfterFlightModel
from XPLMProcessing import XPLMRegisterFlightLoopCallback, XPLMUnregisterFlightLoopCallback, XPLMSetFlightLoopCallbackInterval
from XPLMProcessing import XPLMGetElapsedTime, XPLMGetCycleNumber
from XPLMProcessing import XPLMCreateFlightLoop, XPLMScheduleFlightLoop, XPLMDestroyFlightLoop


class PythonInterface(checkBase):
    """
    We create two flight callbacks, one using Register/Unregister, the other
    using Create/Schedule/Destroy.
    Within the callback, we call GetElapsedTime and GetCycleNumber
    """
    def __init__(self):
        checkBase.__init__(self, 'Processing')
        checkBase.addRef()
        self.results = {'flightLoopCalled': False,
                        'XPLMUnregisterFlightLoopCallback': False,
                        'XPLMSetFlightLoopCallbackInterval': False,
                        'XPLMDestroyFlightLoop': False,
                        'flightLoopReferenceConstant': False,
                        'XPLMCreateFlightLoop': False,
        }
        self.flightLoopReferenceConstant = 57

    def XPluginStart(self):
        self.Sig = "xppython3.processing"
        self.Name = "{} regression test".format(self.Sig)
        self.Desc = "Regression test for {} module".format(self.Sig)

        # register the a flight loop, and collect results in the flight loop callback.
        XPLMRegisterFlightLoopCallback(self.flightLoopCallbackImmediate, 0.0, self.flightLoopReferenceConstant)
        XPLMSetFlightLoopCallbackInterval(self.flightLoopCallbackImmediate, 1.0, 0, self.flightLoopReferenceConstant)

        return self.Name, self.Sig, self.Desc

    def XPluginStop(self):
        XPLMUnregisterFlightLoopCallback(self.flightLoopCallbackImmediate, self.flightLoopReferenceConstant)
        self.results['XPLMUnregisterFlightLoopCallback'] = True

        XPLMDestroyFlightLoop(self.flightLoopDelayedID)
        self.results['XPLMDestroyFlightLoop'] = True
        for k, v in self.results.items():
            self.checkVal(k, v, True)
        self.check()
        checkBase.remRef()

    def XPluginEnable(self):
        phase = xplm_FlightLoop_Phase_AfterFlightModel
        params = [phase, self.flightLoopCallbackDelayed, self.flightLoopReferenceConstant]
        self.flightLoopDelayedID = XPLMCreateFlightLoop(params)
        self.log("flightloopDelayed ID is {}".format(self.flightLoopDelayedID))
        XPLMScheduleFlightLoop(self.flightLoopDelayedID, 60.0, 1)
        self.log("For testing, you should see \"Immediate callback\", followed in 60 seconds with a \"Delayed callback\"")
        return 1

    def XPluginDisable(self):
        return

    def flightLoopCallbackDelayed(self, elapsedSinceLastCall, elapsedTimeSinceLastFlightLoop, counter, inRefcon):
        self.results['XPLMCreateFlightLoop'] = True
        self.results['XPLMScheduleFlightLoop'] = True
        self.log("Delayed elasped {}, {}, [{}]".format(elapsedSinceLastCall, elapsedTimeSinceLastFlightLoop, counter))
        secondsPassed = XPLMGetElapsedTime()
        self.log("Delayed callback: {} seconds have passed".format(secondsPassed))
        currentCycleNumber = XPLMGetCycleNumber()
        self.log("Delayed callback: {} cycles have passed".format(currentCycleNumber))
        return 0

    def flightLoopCallbackImmediate(self, elapsedSinceLastCall, elapsedTimeSinceLastFlightLoop, counter, inRefcon):
        self.log("Immediate elasped {}, {}, [{}]".format(elapsedSinceLastCall, elapsedTimeSinceLastFlightLoop, counter))
        secondsPassed = XPLMGetElapsedTime()
        self.log("Immediate callback: {} seconds have passed".format(secondsPassed))
        currentCycleNumber = XPLMGetCycleNumber()
        self.log("Immediate callback: {} cycles have passed".format(currentCycleNumber))
        self.results['XPMLGetElapsedTime'] = secondsPassed > 0.0 and secondsPassed < 100.0
        self.results['XPLMGetCycleNumber'] = currentCycleNumber == 1

        self.results['flightLoopCalled'] = True
        self.results['XPLMSetFlightLoopCallbackInterval'] = True
        self.results['flightLoopReferenceConstant'] = inRefcon == self.flightLoopReferenceConstant

        return 0

    def XPluginReceiveMessage(self, inFromWho, inMessage, inParam):
        pass
