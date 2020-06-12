class PythonInterface(object):
    def __init__(self):
        self.Sig = "XPython.Simple"
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
        pass
