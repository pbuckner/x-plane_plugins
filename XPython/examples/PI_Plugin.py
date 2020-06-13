import os
from check_helper import checkBase

from XPLMPlugin import XPLM_MSG_PLANE_CRASHED, XPLM_MSG_PLANE_LOADED, XPLM_MSG_AIRPORT_LOADED, XPLM_MSG_SCENERY_LOADED, XPLM_MSG_AIRPLANE_COUNT_CHANGED, XPLM_MSG_PLANE_UNLOADED, XPLM_MSG_WILL_WRITE_PREFS, XPLM_MSG_LIVERY_LOADED, XPLM_MSG_ENTERED_VR, XPLM_MSG_EXITING_VR, XPLM_MSG_RELEASE_PLANES
from XPLMPlugin import XPLMGetMyID, XPLMIsPluginEnabled, XPLMSendMessageToPlugin, XPLMCountPlugins
from XPLMPlugin import XPLMGetNthPlugin, XPLMGetPluginInfo, XPLMEnablePlugin, XPLMDisablePlugin
from XPLMPlugin import XPLMFindPluginByPath, XPLMFindPluginBySignature
from XPLMPlugin import XPLMEnumerateFeatures, XPLMHasFeature, XPLMIsFeatureEnabled, XPLMEnableFeature
from XPLMPlugin import XPLMReloadPlugins


class PythonInterface(checkBase):
    def __init__(self):
        checkBase.__init__(self, 'Plugin')
        checkBase.addRef()

    def XPluginStart(self):
        self.Sig = "xppython3.Plugin"
        self.Name = "{} regression test".format(self.Sig)
        self.Desc = "Regression test for {} module".format(self.Sig)

        self.plugin_id = XPLMGetMyID()

        self.msgCntr = 43
        self.checkVal('XPLMIsPluginEnabled returned wrong value', XPLMIsPluginEnabled(self.plugin_id), 0)  # ... not enabled yet!
        return self.Name, self.Sig, self.Desc

    def XPluginStop(self):
        self.check()
        checkBase.remRef()

    def XPluginEnable(self):
        current = self.msgCntr
        XPLMSendMessageToPlugin(XPLMGetMyID(), 1004, current)
        return 1

    def XPluginDisable(self):
        return

    def XPluginReceiveMessage(self, inFromWho, inMessage, inParam):
        if inMessage != 1004:
            return

        # I'm expecting to receive a 1004 message from myself (sent in PluginEnable)
        self.checkVal('XPlugin Receive message passed incorrect inParam via SendMessageToPlugin', inParam, self.msgCntr)
        self.checkVal('XPlugin Receive message passed incorrect inFromWho via SendMessageToPlugin', inFromWho, self.plugin_id)

        # because we're NOT testing with other plugins, "my" id should one less than total number of plugins
        # (this is not guaranteed by the system, but is how the implmentation appears to work: enabling / disabling
        # plugins will cause the index to change)
        self.checkVal('XPLMCountPlugins returned wrong value', XPLMCountPlugins(), self.plugin_id + 1)

        for i in range(XPLMCountPlugins()):
            info = XPLMGetPluginInfo(i)
            print("Plugin [{}] ID: {} - {}, {}".format(i, XPLMGetNthPlugin(i), info.signature, info.description))

        tmp = 1023
        # doesn't seem to be an error to ask for plugin which aren't there
        self.checkVal('XPLMGetNthPlugin returned wrong value', XPLMGetNthPlugin(tmp), tmp)

        path = '/path/to/plugin'
        signature = '+++'
        self.checkVal('XPLMFindPluginByPath returned wrong value', XPLMFindPluginByPath(path), -1)  # that is, XPLM_NO_PLUGIN_ID
        self.checkVal('XPLMFindPluginBySignature returned wrong value', XPLMFindPluginBySignature(signature), -1)

        # Check "My" plugin... which is the Python3 plugin (not this python script!!!!)
        info = XPLMGetPluginInfo(self.plugin_id)
        self.checkVal('XPLMGetPluginInfo didn\'t pass the inPlugin correctly', XPLMGetMyID(), self.plugin_id)
        self.checkVal('XPLMGetPluginInfo didn\'t get the outName correctly', info.name, "XPPython3")
        if not info.filePath.endswith(".xpl"):
            self.checkVal('XPLMGetPluginInfo didn\'t get the outFilePath correctly', info.filePath, 'ends with :mac.xpl')
        self.checkVal('XPLMGetPluginInfo didn\'t get the outSignature correctly', info.signature, "avnwx.xppython3")
        self.checkVal('XPLMGetPluginInfo didn\'t get the outDescription correctly', info.description,
                      "X-Plane interface for Python 3")

        self.checkVal('XPLMIsPluginEnabled returned wrong value', XPLMIsPluginEnabled(self.plugin_id), 1)

        # enabling a non-existent plug returns 0 false
        self.checkVal('XPLMEnablePlugin returned wrong value', XPLMEnablePlugin(8192), 0)
        XPLMDisablePlugin(tmp)

        # XPLMReloadPlugins() -- this will generate on-screen prompt "Place plugins in directory and I'll reload"

        # "Features" are SDK feature, not things you can define on your own (so it appears)
        # So "HasFeature" indicates if the SDK supports it
        #    "IsFeatureEnabled" indicates if currently enabled
        #    "EableFeature" sets / unsets that feature
        self.fEnum = self.featureEnumerator
        self.keys = []
        XPLMEnumerateFeatures(self.fEnum, self.keys)
        self.checkVal('XPLMEnumerateFeatures length does not match expected', len(self.keys), 3)

        # You cannot define a new feature
        feature = 'bogus feature'
        self.checkVal('XPLMHasFeature returned wrong value for {}'.format(feature), XPLMHasFeature(feature), 0)
        self.checkVal('XPLMIsFeatureEnabled returned wrong value for {}'.format(feature), XPLMIsFeatureEnabled(feature), 0)
        XPLMEnableFeature(feature, 1)
        self.checkVal('XPLMIsFeatureEnabled returned wrong value for {}'.format(feature), XPLMIsFeatureEnabled(feature), 0)

        # An example feature
        feature = 'XPLM_WANTS_REFLECTIONS'
        self.checkVal('XPLMHasFeature returned wrong value for {}'.format(feature), XPLMHasFeature(feature), 1)
        reflection_set = XPLMIsFeatureEnabled(feature)
        # toggle the value and read it back
        XPLMEnableFeature(feature, 0 if reflection_set else 1)
        self.checkVal('XPLMIsFeatureEnabled returned wrong value for {}'.format(feature), XPLMIsFeatureEnabled(feature),
                      0 if reflection_set else 1)

        self.log("Test Complete")
        return

    def featureEnumerator(self, name, ref):
        ref.append(name)
