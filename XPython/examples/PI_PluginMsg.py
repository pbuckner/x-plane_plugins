from XPLMPlugin import XPLM_MSG_PLANE_CRASHED, XPLM_MSG_PLANE_LOADED, XPLM_MSG_AIRPORT_LOADED, XPLM_MSG_SCENERY_LOADED
from XPLMPlugin import XPLM_MSG_AIRPLANE_COUNT_CHANGED, XPLM_MSG_PLANE_UNLOADED, XPLM_MSG_WILL_WRITE_PREFS
from XPLMPlugin import XPLM_MSG_LIVERY_LOADED, XPLM_MSG_ENTERED_VR, XPLM_MSG_EXITING_VR
from XPLMPlugin import XPLMGetPluginInfo


class PythonInterface(object):
    def __init__(self):
        self.Sig = "xppython.plugin.msgs"
        self.Name = "Regression Test {}".format(self.Sig)
        self.Desc = "Regression test {} example".format(self.Sig)

    def XPluginStart(self):
        return self.Name, self.Sig, self.Desc

    def XPluginStop(self):
        pass

    def XPluginEnable(self):
        return 1

    def XPluginDisable(self):
        pass

    def XPluginReceiveMessage(self, inFromWho, inMessage, inParam):
        decodePluginMessage(inFromWho, inMessage, inParam)


def decodePluginMessage(inFromWho, inMessage, inParam):
    msgs = {
        XPLM_MSG_PLANE_CRASHED: {'name': "PLANE_CRASHED",
                                 'param': lambda x: '<ignored>'},
        XPLM_MSG_PLANE_LOADED: {'name': "PLANE_LOADED",
                                'param': lambda x: 'Plane Index: {}'.format(x)},
        XPLM_MSG_AIRPORT_LOADED: {'name': "AIRPORT_LOADED",
                                  'param': lambda x: '<ignored>'},
        XPLM_MSG_SCENERY_LOADED: {'name': "SCENERY_LOADED",
                                  'param': lambda x: '<ignored>'},
        XPLM_MSG_AIRPLANE_COUNT_CHANGED: {'name': "AIRPLANE_COUNT_CHANGED",
                                          'param': lambda x: '<ignored>'},
        XPLM_MSG_PLANE_UNLOADED: {'name': "PLANE_UNLOADED",
                                  'param': lambda x: 'Plane Index: {}'.format(x)},
        XPLM_MSG_WILL_WRITE_PREFS: {'name': "WILL_WRITE_PREFS",
                                    'param': lambda x: '<ignored>'},
        XPLM_MSG_LIVERY_LOADED: {'name': "LIVERY_LOADED",
                                 'param': lambda x: 'Plane Index: {}'.format(x)},
        XPLM_MSG_ENTERED_VR: {'name': "ENTERED_VR",
                              'param': lambda x: '<ignored>'},
        XPLM_MSG_EXITING_VR: {'name': "EXITING_VR",
                              'param': lambda x: '<ignored>'},
    }
    outName = []
    outFilePath = []
    outSignature = []
    outDescription = []
    XPLMGetPluginInfo(inFromWho, outName, outFilePath, outSignature, outDescription)
    print ("Received message from plugin #{} '{}', {}: ({})".format(inFromWho,
                                                                    outName[0],
                                                                    msgs[inMessage]['name'],
                                                                    msgs[inMessage]['param'](inParam)
    ))
