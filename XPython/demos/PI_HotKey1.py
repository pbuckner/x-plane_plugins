"""
HotKey.py

Ported to Python by Sandy Barbour - 28/04/2005
Ported to Python by Peter Buckner - 2-Aug-2020

This code shows how to implement a trivial hot key.  A hot key is a mappable command
key the user can press; in this case, this plugin maps F1 being pressed down to getting
the sim to say stuff.
"""

import xp


class PythonInterface:
    def XPluginStart(self):
        self.Name = "HotKey1"
        self.Sig = "hotKey1.demos.xppython3"
        self.Desc = "An example using a hotkey."

        # Setting up a hot key is quite easy; we simply register a callback.
        # We also provide a text description so that the plugin manager can
        # list the hot key in the hot key mapping dialog box.
        self.HotKey = xp.registerHotKey(xp.VK_F1, xp.DownFlag, "Says 'Hello World 1'", self.MyHotKeyCallback, 0)
        return self.Name, self.Sig, self.Desc

    def XPluginStop(self):
        xp.unregisterHotKey(self.HotKey)

    def XPluginEnable(self):
        return 1

    def XPluginDisable(self):
        pass

    def XPluginReceiveMessage(self, inFromWho, inMessage, inParam):
        pass

    def MyHotKeyCallback(self, inRefcon):
        # This is our hot key handler.  Note that we don't know what key stroke
        # was pressed!  We can identify our hot key by the 'refcon' value though.
        # This is because our hot key could have been remapped by the user and we
        # wouldn't know it.
        xp.speakString("Hello World 1!")
