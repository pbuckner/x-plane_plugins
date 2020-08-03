"""
Navigation.py

Ported to Python by Sandy Barbour - 28/04/2005
Ported to XPPython3 by Peter Buckner - 2-Aug-2020

This example demonstrates how to use the FMC and the navigation databases in
X-Plane.
"""
import xp

nearestAirport = 1
programFMC = 2


class PythonInterface:
    def XPluginStart(self):
        self.Name = "Navigation1"
        self.Sig = "navigation1.demos.xppython3"
        self.Desc = "A plugin that controls the FMC."
        mySubMenuItem = xp.appendMenuItem(xp.findPluginsMenu(), "Python - Navigation 1", 0)
        self.myMenu = xp.createMenu("Navigation1", xp.findPluginsMenu(), mySubMenuItem, self.MyMenuHandlerCallback, 0)
        xp.appendMenuItem(self.myMenu, "Say nearest airport", nearestAirport)
        xp.appendMenuItem(self.myMenu, "Program FMC", programFMC)
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
        if (inItemRef == nearestAirport):
            # First find the plane's position.
            lat = xp.getDataf(xp.findDataRef("sim/flightmodel/position/latitude"))
            lon = xp.getDataf(xp.findDataRef("sim/flightmodel/position/longitude"))
            # Find the nearest airport to us.
            ref = xp.findNavAid(None, None, lat, lon, None, xp.Nav_Airport)
            if (ref != xp.NAV_NOT_FOUND):
                navAidInfo = xp.getNavAidInfo(ref)
                buf = "The nearest airport is %s, %s" % (navAidInfo.navAidID, navAidInfo.name)
                xp.speakString(buf)
                print(buf)
            else:
                xp.speakString("No airports were found!")
                print("No airports were found!")

        if (inItemRef == programFMC):
            # This code programs the flight management computer.  We simply set each entry to a navaid
            # that we find by searching by name or ID.
            xp.setFMSEntryInfo(0, xp.findNavAid(None, "KBOS", None, None, None, xp.Nav_Airport), 3000)
            xp.setFMSEntryInfo(1, xp.findNavAid(None, "LUCOS", None, None, None, xp.Nav_Fix), 20000)
            xp.setFMSEntryInfo(2, xp.findNavAid(None, "SEY", None, None, None, xp.Nav_VOR), 20000)
            xp.setFMSEntryInfo(3, xp.findNavAid(None, "PARCH", None, None, None, xp.Nav_Fix), 20000)
            xp.setFMSEntryInfo(4, xp.findNavAid(None, "CCC", None, None, None, xp.Nav_VOR), 12000)
            xp.setFMSEntryInfo(5, xp.findNavAid(None, "ROBER", None, None, None, xp.Nav_Fix), 9000)
            xp.setFMSEntryInfo(6, xp.findNavAid(None, "KJFK", None, None, None, xp.Nav_Airport), 3000)
            xp.clearFMSEntry(7)
            xp.clearFMSEntry(8)
