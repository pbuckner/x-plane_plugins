"""
SimData1.py

Ported to Python by Sandy Barbour - 28/04/2005
Ported to XPPython3 by Peter Buckner - 2-Aug-2020

This example demonstrates how to interact with X-Plane by reading and writing
data.  This example creates menus items that change the nav-1 radio frequency.
"""

import xp


class PythonInterface:
    def XPluginStart(self):
        self.Name = "SimData1"
        self.Sig = "simData1.demos.xppython3"
        self.Desc = "A plugin that changes sim data."

        mySubMenuItem = xp.appendMenuItem(xp.findPluginsMenu(), "Python - Sim Data 1", 0)
        self.myMenu = xp.createMenu("Sim Data", xp.findPluginsMenu(), mySubMenuItem, self.MyMenuHandlerCallback, 0)
        xp.appendMenuItem(self.myMenu, "Decrement Nav1", -1000)
        xp.appendMenuItem(self.myMenu, "Increment Nav1", +1000)
        self.DataRef = xp.findDataRef("sim/cockpit/radios/nav1_freq_hz")
        return self.Name, self.Sig, self.Desc

    def XPluginStop(self):
        xp.destroyMenu(self.myMenu)

    def XPluginEnable(self):
        return 1

    def XPluginDisable(self):
        pass

    def XPluginReceiveMessage(self, inFromWho, inMessage, inParam):
        pass

    def MyMenuHandlerCallback(self, inMenuRef, inItemRef):
        """
        This is our handler for the menu item.  Our inItemRef is the refcon
        we registered in our XPLMAppendMenuItem calls.  It is either +1000 or
        -1000 depending on which menu item is picked.
        """
        if (self.DataRef != 0):
            # We read the data ref, add the increment and set it again.
            # This changes the nav frequency.
            xp.setDatai(self.DataRef, xp.getDatai(self.DataRef) + inItemRef)
