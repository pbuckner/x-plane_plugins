from regression_base import RegressionBase
# definitions
from XPLMNavigation import xplm_Nav_Unknown, xplm_Nav_Airport, xplm_Nav_NDB, xplm_Nav_VOR, xplm_Nav_ILS, xplm_Nav_Localizer
from XPLMNavigation import xplm_Nav_GlideSlope, xplm_Nav_OuterMarker, xplm_Nav_MiddleMarker, xplm_Nav_InnerMarker
from XPLMNavigation import xplm_Nav_Fix, xplm_Nav_DME, xplm_Nav_LatLon, XPLM_NAV_NOT_FOUND

# functions, tested
from XPLMNavigation import XPLMGetFirstNavAid, XPLMGetNextNavAid
from XPLMNavigation import XPLMGetNavAidInfo
from XPLMNavigation import XPLMFindNavAid, XPLMFindFirstNavAidOfType, XPLMFindLastNavAidOfType
from XPLMNavigation import XPLMCountFMSEntries, XPLMSetFMSEntryInfo, XPLMGetFMSEntryInfo

from XPLMNavigation import XPLMGetDisplayedFMSEntry, XPLMGetDestinationFMSEntry
from XPLMNavigation import XPLMSetDisplayedFMSEntry, XPLMSetDestinationFMSEntry
from XPLMNavigation import XPLMClearFMSEntry, XPLMSetFMSEntryLatLon
from XPLMNavigation import XPLMGetGPSDestinationType, XPLMGetGPSDestination

# functions not tested

# It should be noted that while "FindFirstNavAidOfType()" works fine, there is no "GetNext..OfType()". Instead
# GetNextNavAid() merely returns the next one in the database, regarless of type.
# A common pattern, therefore, it so iterate (and perhaps cache) the whole nav databases -- interation
# is pretty quick.


class PythonInterface(RegressionBase):
    def __init__(self):
        self.Sig = "xppython3.navigation"
        self.Name = "Regression Test Nave"
        self.Desc = "Regression test Nave example"

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
            num = 10
            navaid = XPLMGetFirstNavAid()
            while num:
                if navaid == XPLM_NAV_NOT_FOUND:
                    self.log("No more navaids")
                    break
                self.describeNavaid(navaid)
                navaid = XPLMGetNextNavAid(navaid)
                num -= 1
            self.log("First airport->last airport")
            self.describeNavaid(XPLMFindFirstNavAidOfType(xplm_Nav_Airport))
            self.describeNavaid(XPLMFindLastNavAidOfType(xplm_Nav_Airport))

        codeStep += 1
        if codeStep == self.testSteps[whichFlightLoop]:
            self.log("Step {} Find fragments".format(codeStep))
            self.log("San airports")
            navaid = XPLMFindNavAid("San Francisco", None, None, None, None, xplm_Nav_Airport)
            self.describeNavaid(navaid)

            self.log("KX codes")
            navaid = XPLMFindNavAid(None, "KX", None, None, None, xplm_Nav_Airport)
            self.describeNavaid(navaid)

            self.log("(42, -122) location")
            navaid = XPLMFindNavAid(None, None, 42, -122, None, xplm_Nav_Airport)
            self.describeNavaid(navaid)

            freq = 11790
            self.log("{} freq".format(freq / 100.0))
            navaid = XPLMFindNavAid(None, None, 39.8, -104, freq, xplm_Nav_VOR + xplm_Nav_DME)
            self.describeNavaid(navaid)

        codeStep += 1
        if codeStep == self.testSteps[whichFlightLoop]:
            self.log("Step {} FMS Currently".format(codeStep))
            self.log("Currently {} entries".format(XPLMCountFMSEntries()))
            self.listFMSEntries()

        codeStep += 1
        if codeStep == self.testSteps[whichFlightLoop]:
            self.log("Step {} FMS clear and reset".format(codeStep))
            self.clearFMS()
            XPLMSetFMSEntryInfo(0, XPLMFindNavAid(None, "KSFO", None, None, None, xplm_Nav_Airport), 6000)
            XPLMSetFMSEntryInfo(1, XPLMFindNavAid(None, "KSJC", None, None, None, xplm_Nav_Airport), 7000)
            XPLMSetFMSEntryInfo(2, XPLMFindNavAid(None, "KOAK", None, None, None, xplm_Nav_Airport), 7000)
            XPLMSetFMSEntryInfo(3, XPLMFindNavAid(None, "KHAF", None, None, None, xplm_Nav_Airport), 2000)
            self.listFMSEntries()
            self.log("After adding, now {} entries".format(XPLMCountFMSEntries()))

        codeStep += 1
        if codeStep == self.testSteps[whichFlightLoop]:
            self.log("Step {} Set Displayed to entry 1, Destination to 2".format(codeStep))
            # ... I'm not sure on the 530, what "displayed" refers to & why setting it to 1, here, fails
            XPLMSetDisplayedFMSEntry(1)
            self.checkVal('XPLMGetDisplayedFMSEntry', XPLMGetDisplayedFMSEntry(), 1)
            XPLMSetDestinationFMSEntry(2)
            self.checkVal('XPLMGetDestinationFMSEntry', XPLMGetDestinationFMSEntry(), 2)
            self.listFMSEntries()

        codeStep += 1
        if codeStep == self.testSteps[whichFlightLoop]:
            self.log("Step {} Clear first entry".format(codeStep))
            XPLMClearFMSEntry(0)  # this shifts down all the entries by 1
            self.log("After clearing {} entries".format(XPLMCountFMSEntries()))
            self.listFMSEntries()

        codeStep += 1
        if codeStep == self.testSteps[whichFlightLoop]:
            self.log("Step {} Add Lat Lon entry".format(codeStep))
            XPLMSetFMSEntryLatLon(XPLMCountFMSEntries(), 42, -132, 3000)
            self.describeFMSEntry(XPLMCountFMSEntries() - 1)
            self.log("After setting Lat Lon {} entries".format(XPLMCountFMSEntries()))
            self.listFMSEntries()

        if codeStep == self.testSteps[whichFlightLoop]:
            self.log("Step {} GPS".format(codeStep))
            self.log("Type of destination is {}".format(XPLMGetGPSDestinationType()))
            self.describeNavaid(XPLMGetGPSDestination())

        codeStep += 1
        if codeStep == self.testSteps[whichFlightLoop]:
            self.log("Step {} Done".format(codeStep))
            return 0

        return 1.0

    def clearFMS(self):
        self.log("Clearing")
        self.log("Therea re {} entries".format(XPLMCountFMSEntries()))
        while XPLMCountFMSEntries() > 1:
            XPLMClearFMSEntry(XPLMCountFMSEntries() - 1)
            self.log("There are now {} entries".format(XPLMCountFMSEntries()))
        XPLMClearFMSEntry(0)

    def listFMSEntries(self):
        self.log("FMS is ----")
        for idx in range(XPLMCountFMSEntries()):
            self.describeFMSEntry(idx)

    def describeFMSEntry(self, idx):
        entryInfo = XPLMGetFMSEntryInfo(idx)
        self.log("[{}] '{}' {}: @{} ({}, {})".format(idx, entryInfo.navAidID, entryInfo.type, entryInfo.altitude, entryInfo.lat, entryInfo.lon))

    def describeNavaid(self, navaid):
        navTypes = {xplm_Nav_Unknown: 'Unknown',
                    xplm_Nav_Airport: 'Airport',
                    xplm_Nav_NDB: 'NDB',
                    xplm_Nav_VOR: 'VOR',
                    xplm_Nav_ILS: 'ILS',
                    xplm_Nav_Localizer: 'Localizer',
                    xplm_Nav_GlideSlope: 'GlideSlope',
                    xplm_Nav_OuterMarker: 'OuterMarker',
                    xplm_Nav_MiddleMarker: 'MiddleMarker',
                    xplm_Nav_InnerMarker: 'InnerMarker',
                    xplm_Nav_Fix: 'Fix',
                    xplm_Nav_DME: 'DME',
                    xplm_Nav_LatLon: 'LatLon',
        }
        info = XPLMGetNavAidInfo(navaid)
        self.log("{} '{}': {} ({}, {})".format(info.navAidID, info.name, navTypes[info.type], info.latitude, info.longitude))
