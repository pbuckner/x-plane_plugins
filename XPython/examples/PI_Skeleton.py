class PythonInterface:
    def __init__(self):
        self.Name = "Skeleton"
        self.Sig = "skeleton.xppython3"
        self.Desc = "Minimal do-nothing plugin"

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
