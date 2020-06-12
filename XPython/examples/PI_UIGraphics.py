from regression_base import RegressionBase

# definitions
from XPUIGraphics import xpWindow_Help, xpWindow_MainWindow, xpWindow_SubWindow, xpWindow_Screen, xpWindow_ListView

from XPUIGraphics import xpElement_TextField, xpElement_CheckBox, xpElement_CheckBoxLit, xpElement_WindowCloseBox
from XPUIGraphics import xpElement_WindowCloseBoxPressed, xpElement_PushButton, xpElement_PushButtonLit
from XPUIGraphics import xpElement_OilPlatform, xpElement_OilPlatformSmall, xpElement_Ship, xpElement_ILSGlideScope
from XPUIGraphics import xpElement_MarkerLeft, xpElement_Airport, xpElement_Waypoint, xpElement_NDB, xpElement_VOR
from XPUIGraphics import xpElement_RadioTower, xpElement_AircraftCarrier, xpElement_Fire, xpElement_MarkerRight
from XPUIGraphics import xpElement_CustomObject, xpElement_CoolingTower, xpElement_SmokeStack, xpElement_Building
from XPUIGraphics import xpElement_PowerLine, xpElement_CopyButtons, xpElement_CopyButtonsWithEditingGrid
from XPUIGraphics import xpElement_EditingGrid, xpElement_ScrollBar, xpElement_VORWithCompassRose, xpElement_Zoomer
from XPUIGraphics import xpElement_TextFieldMiddle, xpElement_LittleDownArrow, xpElement_LittleUpArrow
from XPUIGraphics import xpElement_WindowDragBar, xpElement_WindowDragBarSmooth

from XPUIGraphics import xpTrack_ScrollBar, xpTrack_Slider, xpTrack_Progress

# functions, tested
from XPUIGraphics import XPDrawWindow, XPGetWindowDefaultDimensions
from XPUIGraphics import XPDrawElement, XPGetElementDefaultDimensions
from XPUIGraphics import XPDrawTrack, XPGetTrackDefaultDimensions, XPGetTrackMetrics

# functions not tested


class PythonInterface(RegressionBase):
    def __init__(self):
        self.Sig = "xppython3.uigraphics"
        self.Name = "Regression Test {}".format(self.Sig)
        self.Desc = "Regression test {} example".format(self.Sig)

        self.styles = (xpElement_TextField, xpElement_TextField, xpElement_CheckBox, xpElement_CheckBoxLit,
                       xpElement_WindowCloseBox, xpElement_WindowCloseBoxPressed, xpElement_PushButton,
                       xpElement_PushButtonLit, xpElement_OilPlatform, xpElement_OilPlatformSmall, xpElement_Ship,
                       xpElement_ILSGlideScope, xpElement_MarkerLeft, xpElement_Airport, xpElement_Waypoint,
                       xpElement_NDB, xpElement_VOR, xpElement_RadioTower, xpElement_AircraftCarrier, xpElement_Fire,
                       xpElement_MarkerRight, xpElement_CustomObject, xpElement_CoolingTower, xpElement_SmokeStack,
                       xpElement_Building, xpElement_PowerLine, xpElement_CopyButtons,
                       xpElement_CopyButtonsWithEditingGrid, xpElement_EditingGrid, xpElement_ScrollBar,
                       xpElement_VORWithCompassRose, xpElement_Zoomer, xpElement_TextFieldMiddle,
                       xpElement_LittleDownArrow, xpElement_LittleUpArrow, xpElement_WindowDragBar,
                       xpElement_WindowDragBarSmooth)

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
            self.log("Step {} Draw Window".format(codeStep))
            x = []
            y = []
            for style in (xpWindow_Help, xpWindow_MainWindow, xpWindow_SubWindow, xpWindow_Screen, xpWindow_ListView):
                XPGetWindowDefaultDimensions(style, x, y)
                self.log("style: {}, {} x {}".format(style, x[0], y[0]))

        codeStep += 1
        if codeStep == self.testSteps[whichFlightLoop]:
            self.log("Step {}".format(codeStep))
            self.log("Draw callback is called and, for the next few minutes, will update a window in the lower left display")
        return 1.0

    def drawCallback(self, inPhase, inIsBefore, inRefcon):
        self.drawCallbackCalled = 1
        basex = 100
        basey = 100
        if self.testSteps[0] < 5:
            XPDrawWindow(basex, basey, 600, 400, xpWindow_Help)
        elif self.testSteps[0] < 10:
            XPDrawWindow(basex, basey, 600, 400, xpWindow_MainWindow)
        elif self.testSteps[0] < 15:
            XPDrawWindow(basex, basey, 600, 400, xpWindow_SubWindow)
        elif self.testSteps[0] < 20:
            XPDrawWindow(basex, basey, 600, 400, xpWindow_Screen)
        elif self.testSteps[0] < 25:
            XPDrawWindow(basex, basey, 600, 400, xpWindow_ListView)

        offsetx = 10
        offsety = 10
        style = self.styles[self.testSteps[0] % len(self.styles)]
        w = []
        h = []
        canBeLit = []
        XPGetElementDefaultDimensions(style, w, h, canBeLit)
        XPDrawElement(basex + offsetx, basey + offsety, basex + offsetx + w[0], basey + offsety + h[0], style,
                      canBeLit[0])

        offsetx += 20
        style = xpTrack_ScrollBar
        XPGetTrackDefaultDimensions(style, w, canBeLit)
        XPDrawTrack(basex + offsetx, basey + offsety,
                    basex + offsetx + w[0], basey + offsety + 100,
                    0, 100, self.testSteps[0],
                    style, canBeLit[0])
        isVertical = []
        downBtnSize = []
        downPageSize = []
        thumbSize = []
        upPageSize = []
        upBtnSize = []
        XPGetTrackMetrics(basex + offsetx, basey + offsety,
                          basex + offsetx + w[0], basey + offsety + 100,
                          0, 100, self.testSteps[0], style,
                          isVertical, downBtnSize, downPageSize, thumbSize, upPageSize, upBtnSize)
        self.log('Track is vertical: {}'.format(isVertical[0]))

        self.checkVal('drawCallback: Unexpected inPhase', inPhase, self.drawPhase)
        self.checkVal('drawCallback: Unexpected inIsBefore', inIsBefore, self.drawBefore)
        self.checkVal('drawCallback: Unexpected inRefcon', inRefcon, self.drawRefcon)
        return 1
