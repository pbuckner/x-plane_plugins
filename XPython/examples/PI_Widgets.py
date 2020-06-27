from regression_base import RegressionBase

from XPWidgetDefs import xpMode_UpChain, xpMsg_MouseDown
from XPStandardWidgets import xpWidgetClass_MainWindow, xpWidgetClass_Caption, xpWidgetClass_TextField
from XPStandardWidgets import xpProperty_CaptionLit, xpMsg_TextFieldChanged
from XPLMGraphics import xplmFont_Proportional, XPLMGetFontDimensions, XPLMMeasureString
# definitions

# functions, tested
from XPWidgets import XPCreateWidget, XPDestroyWidget, XPSetWidgetDescriptor, XPGetWidgetDescriptor
from XPWidgets import XPShowWidget, XPHideWidget, XPIsWidgetVisible, XPFindRootWidget
from XPWidgets import XPCountChildWidgets, XPGetNthChildWidget, XPGetParentWidget
from XPWidgets import XPBringRootWidgetToFront, XPIsWidgetInFront
from XPWidgets import XPGetWidgetGeometry, XPSetWidgetGeometry
from XPWidgets import XPSetKeyboardFocus, XPLoseKeyboardFocus, XPGetWidgetWithFocus
from XPWidgets import XPSetWidgetProperty, XPGetWidgetProperty
from XPWidgets import XPAddWidgetCallback, XPSendMessageToWidget
from XPWidgets import XPPlaceWidgetWithin
from XPWidgets import XPGetWidgetUnderlyingWindow
from XPWidgets import XPGetWidgetExposedGeometry, XPGetWidgetForLocation
from XPWidgets import XPGetWidgetClassFunc
from XPWidgets import XPCreateCustomWidget

# functions not tested


class PythonInterface(RegressionBase):
    def __init__(self):
        self.Sig = "xppython2.widgets"
        self.Name = "Regression Test {}".format(self.Sig)
        self.Desc = "Regression test {} example".format(self.Sig)

        self.aWidget = None
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
            self.log("Step {} Create one".format(codeStep))
            self.aWidget = XPCreateWidget(100, 200, 600, 50, 1, "Descriptor", 1, 0, xpWidgetClass_MainWindow)
            self.log("Class function is {}".format(XPGetWidgetClassFunc(xpWidgetClass_MainWindow)))

        codeStep += 1
        if codeStep == self.testSteps[whichFlightLoop]:
            self.log("Step {} Change descriptor".format(codeStep))
            out = XPGetWidgetDescriptor(self.aWidget)
            self.checkVal("GetWidgetDescriptor", out, 'Descriptor')

        codeStep += 1
        if codeStep == self.testSteps[whichFlightLoop]:
            XPSetWidgetDescriptor(self.aWidget, "New Descriptor")
            out = XPGetWidgetDescriptor(self.aWidget)
            self.checkVal("GetWidgetDescriptor", out, 'New Descriptor')

        codeStep += 1
        if codeStep == self.testSteps[whichFlightLoop]:
            self.log("Step {} Hide".format(codeStep))
            self.checkVal("IsWidgetVisible should be 1", XPIsWidgetVisible(self.aWidget), 1)
            XPHideWidget(self.aWidget)
            self.checkVal("IsWidgetVisible should be 0", XPIsWidgetVisible(self.aWidget), 0)

        codeStep += 1
        if codeStep == self.testSteps[whichFlightLoop]:
            self.log("Step {} Show".format(codeStep))
            self.checkVal("IsWidgetVisible should be 0", XPIsWidgetVisible(self.aWidget), 0)
            XPShowWidget(self.aWidget)
            self.checkVal("IsWidgetVisible should be 1", XPIsWidgetVisible(self.aWidget), 1)

        codeStep += 1
        if codeStep == self.testSteps[whichFlightLoop]:
            self.log("Step {} GetRoot".format(codeStep))
            self.checkVal("FindRootWidget", XPFindRootWidget(self.aWidget), self.aWidget)

        codeStep += 1
        if codeStep == self.testSteps[whichFlightLoop]:
            self.log("Step {} Add children".format(codeStep))
            fontID = xplmFont_Proportional
            _w, strHeight, _ignore = XPLMGetFontDimensions(fontID)

            for i in range(5):
                s = 'item {}'.format(i)
                strWidth = XPLMMeasureString(fontID, s)
                left = 100 + 10
                top = int(150 - ((strHeight + 4) * i))
                right = int(left + strWidth)
                bottom = int(top - strHeight)
                XPCreateWidget(left, top, right, bottom, 1, s, 0, self.aWidget, xpWidgetClass_Caption)
                widget = XPCreateWidget(right + 10, top, right + 30, bottom, 1, "...", 0,
                                        self.aWidget,
                                        xpWidgetClass_TextField)
            self.checkVal('CountChildren', XPCountChildWidgets(self.aWidget), 10)
            self.checkVal('GetParentWidget', XPGetParentWidget(widget), self.aWidget)
            self.checkVal('GetNthChildWidget', XPGetNthChildWidget(self.aWidget, 9), widget)

        codeStep += 1
        if codeStep == self.testSteps[whichFlightLoop]:
            self.log("Step {} Set Callback".format(codeStep))
            text2 = XPGetNthChildWidget(self.aWidget, 3)
            self.wCB = self.widgetCallback
            self.log("wCB: {} [{}], widgetCallback: {} [{}]".format(self.wCB, id(self.wCB),
                                                                    self.widgetCallback, id(self.widgetCallback)))
            XPAddWidgetCallback(text2, self.widgetCallback)
            self.log("Callback added")

        if codeStep == self.testSteps[whichFlightLoop]:
            self.log("Step {} Send Message widget".format(codeStep))
            # it's a field, the only message it knows:
            text2 = XPGetNthChildWidget(self.aWidget, 3)
            x = 0
            XPSendMessageToWidget(text2, xpMsg_TextFieldChanged, xpMode_UpChain, text2, x)
            self.log("Message sent")

        codeStep += 1
        if codeStep == self.testSteps[whichFlightLoop]:
            self.log("Step {} Another root widget".format(codeStep))

            # self.bWidget = XPCreateWidget(120, 180, 620, 80, 1, "Top Widget", 1, 0, xpWidgetClass_MainWindow)
            self.bWidget = XPCreateCustomWidget(120, 180, 620, 80, 1, "Top Widget", 1, 0,
                                                XPGetWidgetClassFunc(xpWidgetClass_MainWindow))
            self.log("bwidget is {}".format(self.bWidget))
            self.checkVal("In Front should be true", XPIsWidgetInFront(self.bWidget), 1)
            self.checkVal("In Front should be false", XPIsWidgetInFront(self.aWidget), 0)

        codeStep += 1
        if codeStep == self.testSteps[whichFlightLoop]:
            self.log("Step {} Switch front widget".format(codeStep))
            XPBringRootWidgetToFront(self.aWidget)
            self.checkVal("In Front should be true", XPIsWidgetInFront(self.aWidget), 1)

        codeStep += 1
        if codeStep == self.testSteps[whichFlightLoop]:
            self.log("Step {} Move widget".format(codeStep))
            l, t, r, b = XPGetWidgetGeometry(self.bWidget)
            left = l + 20
            top = t + 50
            right = r + 10
            bottom = b - 5
            XPSetWidgetGeometry(self.bWidget, left, top, right, bottom)
            l, t, r, b = XPGetWidgetExposedGeometry(self.bWidget)
            self.checkVal('Exposed left', l, left)
            self.checkVal('Exposed top', t, top)
            self.checkVal('Exposed right', r, right)
            self.checkVal('Exposed bottom', b, bottom)
            self.log("Child id: {}".format(XPGetWidgetForLocation(self.aWidget, 100, 200, 1, 1)))

        codeStep += 1
        if codeStep == self.testSteps[whichFlightLoop]:
            self.log("Step {} Focus widget".format(codeStep))
            text1 = XPGetNthChildWidget(self.aWidget, 1)
            self.checkVal("XP should have focus", XPGetWidgetWithFocus(), None)
            XPSetKeyboardFocus(text1)
            self.checkVal("text1 should have focus", XPGetWidgetWithFocus(), text1)

        codeStep += 1
        if codeStep == self.testSteps[whichFlightLoop]:
            self.log("Step {} Change focus widget".format(codeStep))
            text2 = XPGetNthChildWidget(self.aWidget, 3)
            text1 = XPGetNthChildWidget(self.aWidget, 1)
            self.checkVal("text1 should have focus", XPGetWidgetWithFocus(), text1)
            XPSetKeyboardFocus(text2)
            self.checkVal("text2 should have focus", XPGetWidgetWithFocus(), text2)

        codeStep += 1
        if codeStep == self.testSteps[whichFlightLoop]:
            self.log("Step {} Lose focus widget".format(codeStep))
            text2 = XPGetNthChildWidget(self.aWidget, 3)
            self.checkVal("text2 should have focus", XPGetWidgetWithFocus(), text2)
            XPLoseKeyboardFocus(text2)
            self.checkVal("XP should have focus", XPGetWidgetWithFocus(), None)

        codeStep += 1
        if codeStep == self.testSteps[whichFlightLoop]:
            self.log("Step {} Property widget".format(codeStep))
            field2 = XPGetNthChildWidget(self.aWidget, 2)
            exists = []
            self.checkVal("1 Widget Lit Property should be 0", XPGetWidgetProperty(field2, xpProperty_CaptionLit, exists), 0)
            # until it's set, the property doesn't exist!
            self.checkVal("2 Widget Lit Property should not Exist yet", exists[0], 0)
            XPSetWidgetProperty(field2, xpProperty_CaptionLit, 1)
            exists = []
            self.checkVal("3 Widget Lit Property should be 1", XPGetWidgetProperty(field2, xpProperty_CaptionLit, exists), 1)
            self.checkVal("4 Widget Lit Property Exists", exists[0], 1)

        codeStep += 1
        if codeStep == self.testSteps[whichFlightLoop]:
            self.log("Step {} Property widget".format(codeStep))
            field2 = XPGetNthChildWidget(self.aWidget, 2)
            exists = []
            self.checkVal("5 Widget Lit Property should be 1", XPGetWidgetProperty(field2, xpProperty_CaptionLit, exists), 1)
            self.checkVal("6 Widget Lit Property Exists", exists[0], 1)
            XPSetWidgetProperty(field2, xpProperty_CaptionLit, 0)
            exists = []
            self.checkVal("7 Widget Lit Property should be 0", XPGetWidgetProperty(field2, xpProperty_CaptionLit, exists), 0)
            self.checkVal("8 Widget Lit Property Exists", exists[0], 1)

        codeStep += 1
        if codeStep == self.testSteps[whichFlightLoop]:
            self.log("Step {} Move subwidget".format(codeStep))
            field2 = XPGetNthChildWidget(self.aWidget, 2)
            XPPlaceWidgetWithin(field2, self.bWidget)
            self.checkVal('PlaceWidgetWithin', XPGetParentWidget(field2), self.bWidget)
            self.log("underlying window is {} / {}".format(XPGetWidgetUnderlyingWindow(field2), self.bWidget))

        codeStep += 2
        if codeStep == self.testSteps[whichFlightLoop]:
            self.log("Step {} Destroy".format(codeStep))
            XPDestroyWidget(self.aWidget, 1)
            XPDestroyWidget(self.bWidget, 1)

        codeStep += 1
        if codeStep == self.testSteps[whichFlightLoop]:
            self.log("Step {}".format(codeStep))
            self.log("Test complete")
            return 0

        return 1.0

    def widgetCallback(self, message, widget, param1, param2):
        if message == xpMsg_TextFieldChanged:
            self.log("Received TextFieldChanged!")
            self.log("nth widget is {}, param1 is {}".format(XPGetNthChildWidget(self.aWidget, 3), param1))
            self.checkVal("text field change on wrong widget", XPGetNthChildWidget(self.aWidget, 3), param1)
            return 1
        if message == xpMsg_MouseDown:
            self.log("Mouse down!!!")
            self.log("param1 is {}".format(param1))
            return 1

        return 0

    def drawCallback(self, inPhase, inIsBefore, inRefcon):
        self.drawCallbackCalled = 1
        self.checkVal('drawCallback: Unexpected inPhase', inPhase, self.drawPhase)
        self.checkVal('drawCallback: Unexpected inIsBefore', inIsBefore, self.drawBefore)
        self.checkVal('drawCallback: Unexpected inRefcon', inRefcon, self.drawRefcon)
        return 1
