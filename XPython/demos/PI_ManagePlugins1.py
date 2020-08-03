"""
ManagePlugins.py

Ported to Python by Sandy Barbour - 28/04/2005
Ported to XPPython3 by Peter Buckner - 2-Aug-2020

This example demonstrates how to interact with X-Plane by locating plugins
"""
import xp


class PythonInterface:
    def XPluginStart(self):
        self.Name = "ManagePlugins1"
        self.Sig = "managePlugins.demos.xppython3"
        self.Desc = "A plugin that manages other plugins."

        mySubMenuItem = xp.appendMenuItem(xp.findPluginsMenu(), "Python - Manage Plugins 1", 0)
        self.myMenu = xp.createMenu("Manage Plugins 1", xp.findPluginsMenu(), mySubMenuItem, self.MyMenuHandlerCallback, 0)
        xp.appendMenuItem(self.myMenu, "Disable Others", 0)
        xp.appendMenuItem(self.myMenu, "Enable All", 1)
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
        # This is the menu handler.  We will go through each plugin.
        for n in range(xp.countPlugins() - 1):
            plugin = xp.getNthPlugin(n)
            me = xp.getMyID()

            # Check to see if the plugin is us.  If so, don't
            # disable ourselves!

            print("plugin=%d,me=%d\n" % (plugin, me))
            if plugin != me:
                # Disable based on the item ref for the menu.
                if inItemRef == 0:
                    xp.disablePlugin(plugin)
                else:
                    xp.enablePlugin(plugin)
