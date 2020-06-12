from regression_base import RegressionBase

# definitions
from XPStandardWidgets import xpWidgetClass_MainWindow, xpMainWindowStyle_MainWindow, xpMainWindowStyle_Translucent
from XPStandardWidgets import xpProperty_MainWindowType, xpProperty_MainWindowHasCloseBoxes, xpMessage_CloseButtonPushed

from XPStandardWidgets import xpWidgetClass_SubWindow, xpSubWindowStyle_SubWindow, xpSubWindowStyle_Screen, xpSubWindowStyle_ListView
from XPStandardWidgets import xpProperty_SubWindowType

from XPStandardWidgets import xpWidgetClass_Button, xpPushButton, xpRadioButton, xpWindowCloseBox, xpLittleDownArrow, xpLittleUpArrow
from XPStandardWidgets import xpButtonBehaviorPushButton, xpButtonBehaviorCheckBox, xpButtonBehaviorRadioButton, xpProperty_ButtonType
from XPStandardWidgets import xpProperty_ButtonBehavior, xpProperty_ButtonState, xpMsg_PushButtonPressed, xpMsg_ButtonStateChanged

from XPStandardWidgets import xpWidgetClass_TextField, xpTextEntryField, xpTextTransparent, xpTextTranslucent, xpProperty_EditFieldSelStart
from XPStandardWidgets import xpProperty_EditFieldSelEnd, xpProperty_EditFieldSelDragStart, xpProperty_TextFieldType, xpProperty_PasswordMode
from XPStandardWidgets import xpProperty_MaxCharacters, xpProperty_ScrollPosition, xpProperty_Font, xpProperty_ActiveEditSide
from XPStandardWidgets import xpMsg_TextFieldChanged

from XPStandardWidgets import xpWidgetClass_ScrollBar, xpScrollBarTypeScrollBar, xpScrollBarTypeSlider, xpProperty_ScrollBarSliderPosition
from XPStandardWidgets import xpProperty_ScrollBarMin, xpProperty_ScrollBarMax, xpProperty_ScrollBarPageAmount, xpProperty_ScrollBarType
from XPStandardWidgets import xpProperty_ScrollBarSlop, xpMsg_ScrollBarSliderPositionChanged

from XPStandardWidgets import xpWidgetClass_Caption, xpProperty_CaptionLit

from XPStandardWidgets import xpWidgetClass_GeneralGraphics, xpShip, xpILSGlideScope, xpMarkerLeft, xp_Airport, xpNDB, xpVOR, xpRadioTower
from XPStandardWidgets import xpAircraftCarrier, xpFire, xpMarkerRight, xpCustomObject, xpCoolingTower, xpSmokeStack, xpBuilding
from XPStandardWidgets import xpPowerLine, xpVORWithCompassRose, xpOilPlatform, xpOilPlatformSmall, xpWayPoint
from XPStandardWidgets import xpProperty_GeneralGraphicsType

from XPStandardWidgets import xpWidgetClass_Progress, xpProperty_ProgressPosition, xpProperty_ProgressMin, xpProperty_ProgressMax

# functions, tested

# functions not tested


class PythonInterface(RegressionBase):
    def __init__(self):
        self.Sig = "xppython3.standardwidgets"
        self.Name = "Regression Test {}".format(self.Sig)
        self.Desc = "Regression test {} example".format(self.Sig)

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
        self.log("This test doesn't actually do anything, it just makes sure the enumerations are loadable")
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

        return 1.0

    def drawCallback(self, inPhase, inIsBefore, inRefcon):
        self.drawCallbackCalled = 1
        self.checkVal('drawCallback: Unexpected inPhase', inPhase, self.drawPhase)
        self.checkVal('drawCallback: Unexpected inIsBefore', inIsBefore, self.drawBefore)
        self.checkVal('drawCallback: Unexpected inRefcon', inRefcon, self.drawRefcon)
        return 1
