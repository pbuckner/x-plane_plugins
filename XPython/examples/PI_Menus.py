from regression_base import RegressionBase
from XPLMUtilities import XPLMFindCommand
# definitions
from XPLMMenus import xplm_Menu_NoCheck, xplm_Menu_Unchecked, xplm_Menu_Checked
# functions, tested
from XPLMMenus import XPLMFindPluginsMenu, XPLMFindAircraftMenu, XPLMCreateMenu
from XPLMMenus import XPLMAppendMenuItem, XPLMAppendMenuItemWithCommand
from XPLMMenus import XPLMSetMenuItemName
from XPLMMenus import XPLMAppendMenuSeparator, XPLMCheckMenuItem, XPLMCheckMenuItemState
from XPLMMenus import XPLMEnableMenuItem, XPLMRemoveMenuItem
from XPLMMenus import XPLMClearAllMenuItems
from XPLMMenus import XPLMDestroyMenu

# functions not tested


class PythonInterface(RegressionBase):
    def __init__(self):
        self.Sig = "xppython3.menus"
        self.Name = "Regression Test {}".format(self.Sig)
        self.Desc = "Regression test {}".format(self.Sig)
        self.menus = {}

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
            pluginsMenuID = XPLMFindPluginsMenu()
            self.log('plugins Menu ID: {}'.format(pluginsMenuID))
            aircraftMenuID = XPLMFindAircraftMenu()
            self.checkVal('XPLMFindAircraftMenu should be None', aircraftMenuID, None)

        codeStep += 1
        if codeStep == self.testSteps[whichFlightLoop]:
            # Notes on menus
            # *) You cannot place a menu / menu item on the top menubar -- your starting point
            #    is the Plugin Menu drop down
            # *) You *can* AppendMenuItem to plugin menu, but if you merely append, you
            #    cannot attach a menu handler to it. You can:
            #    + create an additional menu (with its handler)
            #    + set that new menu's parent as the initial appended menu
            #    + and/or, use that menu item for status -- user can't select it, but you
            #      can change the text, update check mark, etc.
            # *) You *can* AppendMenuItemWith Command to plugin Menu
            # Careful about confusing menuID (an XPLMMenuID) with menuItemID (an integer index)
            # (one returned by CreateMenu, the other by AppendMenuItem)

            #####
            # First, two different ways to add menu + submenu, the second method re-uses data
            self.log("Step {} Create top menu".format(codeStep))
            self.menus['top'] = {'name': 'top', 'title': "Menu1 w/sub", 'refCon': ['top', ], 'parent': None}
            self.menus['top']['menuID'] = XPLMCreateMenu(self.menus['top']['title'], self.menus['top']['parent'], 0,
                                                         self.menuHandler, self.menus['top']['refCon'])
            self.menus['top']['menuIDX'] = 0

        codeStep += 1
        if codeStep == self.testSteps[whichFlightLoop]:
            self.log("Step {} appending top menu".format(codeStep))
            self.menus['top-sub'] = {'name': 'top-sub',
                                     'title': "Appended to top bar",
                                     'refCon': ['top-sub', ],
                                     'parent': self.menus['top']['menuID']}

            self.menus['top-sub']['menuIDX'] = XPLMAppendMenuItem(self.menus['top-sub']['parent'],
                                                                  self.menus['top-sub']['title'],
                                                                  self.menus['top-sub']['refCon'])
            self.log("Appended")

            m = {'name': 'top2', 'title': "Menu2 w/sub", 'refCon': ['top2', ], 'parent': None}
            m['menuID'] = XPLMCreateMenu(m['title'], m['parent'], 1, self.menuHandler, m['refCon'])
            self.menus[m['name']] = m

            # (Python note: this assigment results in a "new" m... id(m) is different from id(<previous>m)
            # and this keeps the refCon, for example, different
            # If instead we'd simply did m.update({'refCon': 'new refCon'})
            # id(m) doesn't change. So, both menus would get 'new refCon', since both menus point
            # to the same spot in memory.... Don't do that!
            m = {'name': 'top2-sub',
                 'title': "Appended to top2 bar",
                 'refCon': ['top-sub2', ],
                 'parent': self.menus['top2']['menuID']}
            m['menuIDX'] = XPLMAppendMenuItem(m['parent'], m['title'], m['refCon'])
            self.menus[m['name']] = m
            XPLMAppendMenuSeparator(m['parent'])
            m = {'name': 'top2-sub2',
                 'title': "Also Appended to top2 bar",
                 'refCon': ['top-sub2-second', ],
                 'parent': self.menus['top2']['menuID']}
            m['menuIDX'] = XPLMAppendMenuItem(m['parent'], m['title'], m['refCon'])
            self.menus[m['name']] = m

            XPLMAppendMenuSeparator(XPLMFindPluginsMenu())
            #####
            # Simply append menu item to bottom of Plugins menu -- no handler or command
            m = {'name': 'time', 'title': 'ElapsedTime', 'refCon': ['time', ], 'parent': XPLMFindPluginsMenu()}
            m['menuIDX'] = XPLMAppendMenuItem(m['parent'], m['title'], m['refCon'])
            self.menus[m['name']] = m

            #####
            # Simply append menu item to bottom of Plugins menu -- no handler or command
            m = {'name': 'pause', 'title': 'Pause', 'refCon': ['time', ], 'parent': XPLMFindPluginsMenu()}
            cmdRef = XPLMFindCommand('sim/operation/pause_toggle')
            m['menuIDX'] = XPLMAppendMenuItemWithCommand(m['parent'], m['title'], cmdRef)
            self.menus[m['name']] = m

            # if not firstMenuID:
            #     self.error("firstMenuID should not be null: {}".format(firstMenuID))
            # subMenuID = XPLMAppendMenuItem(XPLMFindPluginsMenu(), 'Appneded to plugins', ['appended', ])
            # secondMenuID = XPLMCreateMenu('PluginBar', XPLMFindPluginsMenu(), 1, self.menuHandler, ['pluginsbar', ])
            # if not secondMenuID:
            #     self.error("secondMenuID should not be null: {}".format(secondMenuID))

            self.log("Menus are: {}".format(self.menus))

        if 'time' in self.menus:
            XPLMSetMenuItemName(self.menus['time']['parent'], self.menus['time']['menuIDX'], '{}'.format(counter))
            # depending on the counter, check/uncheck the menu item, and enable/disable the 'Pause' menuitem
            if (counter % 10) < 5:
                XPLMCheckMenuItem(self.menus['time']['parent'], self.menus['time']['menuIDX'], xplm_Menu_Unchecked)
                self.checkVal('XPLMCheckMenuItemState',
                              XPLMCheckMenuItemState(self.menus['time']['parent'], self.menus['time']['menuIDX']),
                              xplm_Menu_Unchecked)
                XPLMEnableMenuItem(self.menus['pause']['parent'], self.menus['pause']['menuIDX'], 1)
            else:
                XPLMCheckMenuItem(self.menus['time']['parent'], self.menus['time']['menuIDX'], xplm_Menu_Checked)
                self.checkVal('XPLMCheckMenuItemState',
                              XPLMCheckMenuItemState(self.menus['time']['parent'], self.menus['time']['menuIDX']),
                              xplm_Menu_Checked)
                XPLMEnableMenuItem(self.menus['pause']['parent'], self.menus['pause']['menuIDX'], 0)

        codeStep += 1
        if self.testSteps[whichFlightLoop] == 10:
            self.log('Step {} Removing one menu item'.format(10))
            XPLMRemoveMenuItem(self.menus['top2-sub']['parent'], self.menus['top2-sub']['menuIDX'])
            # (technically, this then changes the IDX of all menu items 'below' this item -- I'm ignorning that here)

        if self.testSteps[whichFlightLoop] == 12:
            self.log('Step {} Clearning menu items'.format(12))
            XPLMClearAllMenuItems(self.menus['top2']['menuID'])

        if self.testSteps[whichFlightLoop] == 14:
            self.log('Step {} Destroying menu'.format(14))
            XPLMDestroyMenu(self.menus['top2']['menuID'])
            ###
            # Destroy works, but, if you destroy a menu, WHILE ITS Submenu is displayed
            #  it will crash the plugin / application. If you're merely viewing the menu
            #  (but not its submenu) then it will correctly disappear).
            # Ideally, I should recreate this simple test in C++ and send to Laminar
            # Note, XP documentation says DestroyMenu isn't normally necessary, so best
            # advice is to just not use it!
            self.log('destroyed')

        codeStep += 1
        if codeStep == self.testSteps[whichFlightLoop]:
            self.log("Step {} Test complete".format(codeStep))
            return 0

        return 1.0

    def menuHandler(self, inMenuRef, inItemRef):
        self.log('menuRef: {}, itemRef: {}'.format(inMenuRef, inItemRef))
