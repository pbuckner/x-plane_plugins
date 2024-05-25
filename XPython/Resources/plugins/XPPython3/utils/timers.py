from XPPython3 import xp
from . import xlua

# run_at_interval(func, interval) -> run_timer(func, interval, interval)
# run_after_time(func, delay)   -> run_timer(func, delay, 0)
# run_timer(func, delay, interval)
#    (after delay run & repeat every interval after that)
# stop_timer(func)
# is_timer_scheduled(func)

# Timers are _paused_ while Sim is paused & run faster if sim flight model is executing faster
# (say via 'sim/operation/flightmodel_speed_change' command, or dataref 'sim/time/sim_speed')
# (x ground-speed speed up does _not_ change timer speed.)


class Timer:
    def __init__(self, func, delay, interval):
        self.func = func
        self.interval = interval
        self.delay = delay
        try:
            self.next_fire = RunningTime.value + delay
        except AttributeError:
            self.next_fire = delay


RunningTime = None


class Timers:
    _Timers = []

    def genericTimerCallback(self, _sinceLast, _elapsedTime, _counter, _refCon):
        if self._Timers:
            now = xlua.RUNNING_TIME
            for t in self._Timers:
                if t.next_fire <= now:
                    t.func()
                    t.next_fire = now + t.interval if t.interval > 0 else -1
            self._Timers = [x for x in self._Timers if x.interval > 0]
        return -1

    def __init__(self):
        self._timerFlightLoop = None

    def get(self, func):
        try:
            return [x for x in self._Timers if x.func == func][0]
        except IndexError:
            return None

    def is_timer_scheduled(self, func):
        return bool(self.get(func))

    def stop_timer(self, func):
        self._Timers = [x for x in self._Timers if x.func != func]

    def run_timer(self, func, delay, interval):
        if self._timerFlightLoop is None or not xp.isFlightLoopValid(self._timerFlightLoop):
            self._Timers = []  # delete any existing dead timers (will happen if all plugins are reloaded)
            self._timerFlightLoop = xp.createFlightLoop(self.genericTimerCallback, phase=xp.FlightLoop_Phase_BeforeFlightModel)
            xp.scheduleFlightLoop(self._timerFlightLoop, -1)
        self._Timers.append(Timer(func, delay, interval))


_t = Timers()


def is_timer_scheduled(func):
    xp.log(f"timers {id(_t)=}, {len(_t._Timers)}")
    _t.is_timer_scheduled(func)


def stop_timer(func):
    _t.stop_timer(func)


def run_after_time(func, seconds):
    _t.run_timer(func, seconds, 0)


def run_at_interval(func, interval):
    xp.log(f"timers {id(_t)=}, {len(_t._Timers)}")
    _t.run_timer(func, interval, interval)


def run_timer(func, delay, interval):
    _t.stop_timer(func)
    _t.run_timer(func, delay, interval)
