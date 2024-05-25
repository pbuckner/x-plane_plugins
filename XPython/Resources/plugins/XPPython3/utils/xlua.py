from . import datarefs


class xlua:
    def __init__(self):
        self.is_in_replay = datarefs.find_dataref('sim/time/is_in_replay')
        self.frame_rate_period = datarefs.find_dataref('sim/operation/misc/frame_rate_period')
        self.paused = datarefs.find_dataref('sim/time/paused')
        self.running_time = datarefs.find_dataref('sim/time/total_running_time_sec')
        self.flight_time = datarefs.find_dataref('sim/time/total_flight_time_sec')

    @property
    def IN_REPLAY(self):
        return self.is_in_replay.value

    @property
    def SIM_PERIOD(self):
        return self.frame_rate_period.value

    @property
    def PAUSED(self):
        return self.paused.value

    @property
    def RUNNING_TIME(self):
        return self.running_time.value

    @property
    def FLIGHT_TIME(self):
        return self.flight_time.value


_x = xlua()


def __getattr__(name):
    if name in ('IN_REPLAY', 'SIM_PERIOD', 'PAUSED', 'RUNNING_TIME', 'FLIGHT_TIME'):
        return _x.__getattribute__(name)
    raise AttributeError(f"module '{__name__}' has no attribute '{name}'")
