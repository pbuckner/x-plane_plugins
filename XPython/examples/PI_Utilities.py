import ctypes
from regression_base import RegressionBase

from XPLMDefs import XPLM_VK_Q, XPLM_VK_NUMPAD2
from XPLMPlugin import XPLMEnumerateFeatures, XPLMIsFeatureEnabled
# definitions
from XPLMUtilities import xplm_DataFile_Situation, xplm_DataFile_ReplayMovie
from XPLMUtilities import xplm_CommandBegin, xplm_CommandContinue, xplm_CommandEnd

# functions, tested
from XPLMUtilities import XPLMGetSystemPath, XPLMGetPrefsPath, XPLMGetDirectorySeparator, XPLMExtractFileAndPath
from XPLMUtilities import XPLMGetDirectoryContents
from XPLMUtilities import XPLMLoadDataFile, XPLMSaveDataFile
from XPLMUtilities import XPLMGetVersions
from XPLMUtilities import XPLMGetLanguage, XPLMFindSymbol
from XPLMUtilities import XPLMSetErrorCallback, XPLMDebugString, XPLMSpeakString
from XPLMUtilities import XPLMGetVirtualKeyDescription
from XPLMUtilities import XPLMFindCommand, XPLMCommandBegin, XPLMCommandEnd, XPLMCommandOnce
from XPLMUtilities import XPLMCreateCommand, XPLMRegisterCommandHandler, XPLMUnregisterCommandHandler
from XPLMUtilities import XPLMReloadScenery

# functions not tested


class PythonInterface(RegressionBase):
    def __init__(self):
        self.Sig = "xppython3.utilities"
        self.Name = "Regression Test {}".format(self.Sig)
        self.Desc = "Regression {} example".format(self.Sig)

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
        XPLMDebugString("Print Debug Executed\n")

        return 1

    def XPluginDisable(self):
        XPLMUnregisterCommandHandler(self.cmdID, self.cmdCB, 0, self.cmdRefcon)
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
            # NOTE: the XPython plugin forces Native Paths on. We should also block
            # disabling it (not done yet).
            def featureEnumerator(feature, ref):
                ref.append(feature)

            self.fEnum = featureEnumerator
            self.features = []

            XPLMEnumerateFeatures(self.fEnum, self.features)
            self.log("Features are {}".format(self.features))
            self.log('{} is {}enabled'.format('XPLM_USE_NATIVE_PATHS',
                                              '' if XPLMIsFeatureEnabled('XPLM_USE_NATIVE_PATHS') else 'not '))

            self.log("Step {}".format(codeStep))

        codeStep += 1
        if codeStep == self.testSteps[whichFlightLoop]:
            XPLMSpeakString("This is flight loop {}. Just starting up.".format(whichFlightLoop))
            self.log("System path is {}".format(XPLMGetSystemPath()))
            self.log("Prefs path is {}".format(XPLMGetPrefsPath()))
            prefs = XPLMGetPrefsPath()
            self.log("Prefs file is {0}, directory is {1}".format(*XPLMExtractFileAndPath(prefs)))
            self.checkVal('XPLMGetDirectorySeparator', XPLMGetDirectorySeparator(), '/')
            self.log("Step {}".format(codeStep))

        codeStep += 1
        if codeStep == self.testSteps[whichFlightLoop]:
            time_up_cmd = XPLMFindCommand('sim/operation/time_up_lots')
            time_dn_cmd = XPLMFindCommand('sim/operation/time_down_lots')
            XPLMCommandOnce(time_up_cmd)
            XPLMCommandOnce(time_up_cmd)

        codeStep += 1
        if codeStep == self.testSteps[whichFlightLoop]:
            time_up_cmd = XPLMFindCommand('sim/operation/time_up_lots')
            time_dn_cmd = XPLMFindCommand('sim/operation/time_down_lots')
            XPLMCommandOnce(time_dn_cmd)
            XPLMCommandOnce(time_dn_cmd)

        codeStep += 1
        if codeStep == self.testSteps[whichFlightLoop]:
            time_up_cmd = XPLMFindCommand('sim/operation/time_up_lots')
            time_dn_cmd = XPLMFindCommand('sim/operation/time_down_lots')
            XPLMCommandBegin(time_dn_cmd)
            XPLMCommandOnce(time_dn_cmd)

        codeStep += 1
        if codeStep == self.testSteps[whichFlightLoop]:
            time_up_cmd = XPLMFindCommand('sim/operation/time_up_lots')
            time_dn_cmd = XPLMFindCommand('sim/operation/time_down_lots')
            XPLMCommandEnd(time_dn_cmd)

        codeStep += 1
        if codeStep == self.testSteps[whichFlightLoop]:
            var = XPLMFindSymbol('XPLMGetVersions')
            self.log("Symbol for GetVersions is {}".format(var))
            XPLMGetVersionsPrototype = ctypes.CFUNCTYPE(ctypes.POINTER(ctypes.c_int),
                                                        ctypes.POINTER(ctypes.c_int),
                                                        ctypes.POINTER(ctypes.c_int))
            XPLMGetVersionsTemp = XPLMGetVersionsPrototype(var)
            xp_version = ctypes.c_int()
            version = ctypes.c_int()
            hostID = ctypes.c_int()
            XPLMGetVersionsTemp(ctypes.byref(xp_version), ctypes.byref(version), ctypes.byref(hostID))
            self.log("XPlaneVersion: {}, version: {}, hostID: {}".format(xp_version, version, hostID))

            self.errCB = self.errorCallback
            XPLMSetErrorCallback(self.errCB)
            # Trigger error callback...
            XPLMLoadDataFile(42, '///')

        codeStep += 1
        if codeStep == self.testSteps[whichFlightLoop]:
            self.cmdID = XPLMCreateCommand('test/regression', 'Regression Test Command')
            self.cmdCB = self.commandCallback
            self.cmdRefcon = ['command', ]
            XPLMRegisterCommandHandler(self.cmdID, self.cmdCB, 0, self.cmdRefcon)

        codeStep += 1
        if codeStep == self.testSteps[whichFlightLoop]:
            XPLMCommandOnce(XPLMFindCommand('test/regression'))
            start = 0
            max_rows = 100
            buffersize = 1024
            path = XPLMGetSystemPath() + "Output"
            # implements a 'paging' type of interface. There is no indication if returned value is file or directory.
            (retCode, files, total) = XPLMGetDirectoryContents(path, start, buffersize, max_rows)
            self.log('gdc returned:\n  retCode: {},\n  files: {},\n  total_files: {}'.format(retCode, files, total))

            # check the returned filenames
            for i, x in enumerate(files):
                self.log('[{}] {}'.format(i, x))

            self.log('return from load data file: {}'.format(
                XPLMLoadDataFile(xplm_DataFile_ReplayMovie, "Output/replays/Cessna Skyhawk Replay.rep")))

            self.log('return from save data file: {}'.format(
                XPLMSaveDataFile(xplm_DataFile_Situation, "Output/situations/Cessna Skyhawk Replay.sit")))

        codeStep += 1
        if codeStep == self.testSteps[whichFlightLoop]:
            replay_off = XPLMFindCommand('sim/replay/replay_off')
            XPLMCommandOnce(replay_off)

            xp_version, version, hostID = XPLMGetVersions()
            self.log("XPlaneVersion: {}, version: {}, hostID: {}".format(xp_version, version, hostID))
            self.log("Language code: {}".format(XPLMGetLanguage()))

        codeStep += 1
        if codeStep == self.testSteps[whichFlightLoop]:
            XPLMReloadScenery()

        codeStep += 1
        if codeStep == self.testSteps[whichFlightLoop]:
            self.log('XPLM_VK_Q is {}'.format(XPLMGetVirtualKeyDescription(XPLM_VK_Q)))
            self.log('XPLM_VK_NUMPAD2 is {}'.format(XPLMGetVirtualKeyDescription(XPLM_VK_NUMPAD2)))
            self.log("Finshed flightloop: {}".format(whichFlightLoop))
            self.log("Test complete")
            return 0

        return 1.0

    def errorCallback(self, c):
        print('********ERROR CALLBACK: {}'.format(c))

    def drawCallback(self, inPhase, inIsBefore, inRefcon):
        self.drawCallbackCalled = 1
        self.checkVal('drawCallback: Unexpected inPhase', inPhase, self.drawPhase)
        self.checkVal('drawCallback: Unexpected inIsBefore', inIsBefore, self.drawBefore)
        self.checkVal('drawCallback: Unexpected inRefcon', inRefcon, self.drawRefcon)
        return 1

    def commandCallback(self, cmd, phase, refcon):
        self.log("Command executed")
        if phase not in (xplm_CommandBegin, xplm_CommandContinue, xplm_CommandEnd):
            self.error("Expected phase to be Begin, Continue or End")
        self.checkVal('XPLMCreateCommand - cmd', cmd, self.cmdID)
        self.checkVal('XPLMCreateCommand - cmd', refcon, self.cmdRefcon)
        return 1
