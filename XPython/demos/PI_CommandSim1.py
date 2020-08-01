"""
CommandSim.py

Ported to Python by Sandy Barbour - 28/04/2005
Ported to XPPython3 by Peter Buckner - 26-Jul-2020

This function demonstrates how to send commands to the sim.  Commands allow you to simulate
any keystroke or joystick button press or release.
"""

import xp


class PythonInterface:
    def XPluginStart(self):
        self.Name = "CommandSim1"
        self.Sig = "commandSim1.demos.xppython3"
        self.Desc = "An example of sending commands."

        mySubMenuItem = xp.appendMenuItem(xp.findPluginsMenu(), "Python - Command Sim 1", 0)
        self.myMenu = xp.createMenu("Command Sim 1", xp.findPluginsMenu(), mySubMenuItem, self.MyMenuHandlerCallback, 0)

        """
        For each command, we set the menu item to execute the command directly.
        (XPLMCommandKeyStroke() is deprecated, so we cannot use the Python2 method.
        """
        xp.appendMenuItemWithCommand(self.myMenu, "Pause", xp.findCommand('sim/operation/pause_toggle'))
        xp.appendMenuItemWithCommand(self.myMenu, "Reverse Thrust", xp.findCommand('sim/engines/thrust_reverse_toggle'))
        xp.appendMenuItemWithCommand(self.myMenu, "Jettison", xp.findCommand('sim/flight_controls/jettison_payload'))
        xp.appendMenuItemWithCommand(self.myMenu, "Brakes (Regular)", xp.findCommand('sim/flight_controls/brakes_toggle_regular'))
        xp.appendMenuItemWithCommand(self.myMenu, "Brakes (Full)", xp.findCommand('sim/flight_controls/brakes_toggle_max'))
        xp.appendMenuItemWithCommand(self.myMenu, "Landing Gear", xp.findCommand('sim/flight_controls/landing_gear_toggle'))
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
        Python2:
        This is the menu callback.  We simply turn the item ref back
        into a command ID and tell the sim to do it.

        Python3:
        No need for callback to do anything.
        """
        # xp.commandKeyStroke(inItemRef)
        pass
