"""
ShareData2.py

by Peter Buckner - 2-Aug-2020

This is an example plugin that demonstrates how to share data, both owned
by a plugin and shared. This plugin should be used WITH PI_SharedData1.py

This plugin will Access (read and write) PI_SharedData1 data -- data "owned"
by the other plugin.

(This plugin does not attempt to access the Shared dataref from PI_SharedData1)
"""

import xp


class PythonInterface:
    def XPluginStart(self):
        self.Name = "SharedData1"
        self.Sig = "sharedData2.demos.xppython3"
        self.Desc = "A plugin that reads a shared data ref."

        return self.Name, self.Sig, self.Desc

    def XPluginStop(self):
        pass

    def XPluginEnable(self):
        # On 'Enable', we located and read the dataref from PI_SharedData1
        dataRef = xp.findDataRef('xppython3/demos/sharedata/number1')
        data = xp.getDataf(dataRef)
        print("PI_SharedData2: Found 'owned' data from ShareadData1: {}".format(data))
        return 1

    def XPluginDisable(self):
        pass

    def XPluginReceiveMessage(self, inFromWho, inMessage, inParam):
        pass
