from XPPython3 import xp
from typing import Any
from XPPython3.utils import xlua
from XPPython3 import internals
import inspect
import os

VERBOSE = False


class EasyPython:
    # Callbacks
    #   SASL:
    #   update(): called each flight loop -- do update logic here. Do Not Draw.
    #   onFirstFlight(): called before first update. equivalent to onModuleInit()
    #   onDisable(): equivalent to onModuleShutDown()
    #   onStop(): equivalent to onModuleDown()
    #   onAirportLoaded(flightNumber): called when plane is positioned at airport. flightNumber is count of flights started.
    #   onSceneryLoaded()
    #   onPlaneLoaded()
    #   onPlaneUnloaded()
    #   onAirplaneCountChanged()
    #   onPlaneCrash()
    #   not implemented:
    #     draw()
    #     draw3D()
    #     drawObjects()
    #
    #  XLUA:
    # from xlua, note the _key_ is the function (rather than an actual timer object):
    #   run_at_interval(func, interval) [once]
    #   run_after_time(func, delay)     [once]
    #   run_time(func, delay, interval) [run after delay, then every interval
    #   stop_timer(func)
    #   is_timer_scheduled(func)        [true if function is schedule to run anytime in the future. false if no longer/never scheduled
    #
    #  xlua Callbacks (every module can define these):
    #     aircraft_load() - run once when your aircraft is loaded. This is run after the aircraft is initialized enough to set overrides.
    #     aircraft_unload() - run once when your aircraft is unloaded.
    #     flight_start() - run once each time a flight is started. The aircraft is already initialized and can thus be customized.
    #                      This is always called after aircraft_load has been run at least once.
    #     flight_crash() - called if X­Plane detects that the user has crashed the airplane.
    #     before_physics() - called every frame that the sim is not paused and not in replay, before physics are calculated
    #     after_physics() - called every frame that the sim is not paused and not in replay, after physics are calculated
    #     after_replay() - called every frame that the sim is in replay mode, regardless of pause status.
    #

    def __init__(self):
        self.name = os.path.basename(inspect.getfile(self.__class__))
        self.signature = f"easypython.{self.name}"
        self.description = self.name
        self.prev = None
        self.flightNumber = 0
        # flags which allow us to (possibly) stop executing flight loops
        self.no_update = False
        self.no_after_replay = False
        self.no_after_physics = False
        self.no_before_physics = False
        self._startflightloop_before = None
        self._startflightloop_after = None

    def XPluginReceiveMessage(self, _inFromWho, inMessage, _inParam):
        # xp.log(f"{xlua.RUNNING_TIME=} {xlua.FLIGHT_TIME=}")
        if inMessage == xp.MSG_PLANE_LOADED:
            self.onPlaneLoaded()
            self.aircraft_load()
            self.no_after_replay = False
            self.no_after_physics = False
            self.no_before_physics = False
        elif inMessage == xp.MSG_PLANE_UNLOADED:
            self.onPlaneUnloaded()
            self.aircraft_unload()
        elif inMessage == xp.MSG_LIVERY_LOADED:
            self.onLiveryLoaded()
        elif inMessage == xp.MSG_AIRPLANE_COUNT_CHANGED:
            self.onAirplaneCountChanged()
        elif inMessage == xp.MSG_AIRPORT_LOADED:
            # aircraft is always loaded prior to airport: Airport
            # is always loaded whenever aircraft is changed or Airport is changed,
            # so this is an indicate that a new flight has started.
            self.flight_start()
            self.onFirstFlight()

            self.flightNumber += 1
            self.onAirportLoaded(self.flightNumber)
        elif inMessage == xp.MSG_PLANE_CRASHED:
            self.onPlaneCrash()
            self.flight_crash()

    def XPluginStart(self):
        self._startflightloop_before = None
        self._startflightloop_after = None
        self.onStart()
        return self.name, self.signature, self.description

    def XPluginStop(self):
        self.onStop()

    def XPluginDisable(self):
        if self._startflightloop_before:
            xp.destroyFlightLoop(self._startflightloop_before)
            self._startflightloop_before = None
        if self._startflightloop_after:
            xp.destroyFlightLoop(self._startflightloop_after)
            self._startflightloop_after = None
        self.onDisable()
        for accessor in internals.getAccessors(self):
            xp.unregisterDataAccessor(accessor)
        for command in internals.getCommands(self):
            xp.unregisterCommandHandler(*command)
        for flightLoop in internals.getFlightLoops(self):
            print(f"Destroying flightLoop: {flightLoop}")
            xp.destroyFlightLoop(flightLoop)

    def XPluginEnable(self):
        self._startflightloop_before = xp.createFlightLoop(self._do_flightloop_before, xp.FlightLoop_Phase_BeforeFlightModel)

        xp.scheduleFlightLoop(self._startflightloop_before, -1)
        self._startflightloop_after = xp.createFlightLoop(self._do_flightloop_after, xp.FlightLoop_Phase_AfterFlightModel)
        xp.scheduleFlightLoop(self._startflightloop_after, -1)
        return self.onEnable()

    def _do_flightloop_before(self, _lastCall: float, _elapsedTime: float, counter: int, _refCon: Any) -> int:
        self.update()
        if not xlua.IN_REPLAY and not xlua.PAUSED:
            self.before_physics()
        if self.no_before_physics and self.no_update:
            return 0
        return -1

    def _do_flightloop_after(self, _lastCall: float, _elapsedTime: float, _counter: int, _refCon: Any) -> int:
        if xlua.IN_REPLAY:
            self.after_replay()
            if self.no_after_replay:
                return 0
        elif not xlua.PAUSED:
            self.after_physics()
            if self.no_after_physics:
                return 0
        return -1

    ###############################################
    #  XLUA - type callbacks
    def aircraft_load(self) -> None:
        """ XLUA
        Called once, when user aircraft is loaded. The aircraft is initialized enough
        to set overrides.
        """
        if VERBOSE: xp.log('aircraft_load')

    def aircraft_unload(self) -> None:
        """ XLUA
        Called once, when user aircraft is unloaded.
        """
        if VERBOSE: xp.log('aircraft_unload')

    def flight_start(self) -> None:
        """ XLUA
        Called once, each time flight is started. The aircraft is already initialized
        and can now be customized. This is always called after aircraft_load().
        """
        if VERBOSE: xp.log('flight_start')

    def flight_crash(self) -> None:
        """ XLUA
        Called if X-Plane detects that the user aircraft has crashed.
        and not in replay.
        """
        if VERBOSE: xp.log('flight_crash')

    def before_physics(self) -> None:
        """ XLUA
        Called every frame, before physics has been calculated, while the sim is not paused
        and not in replay.
        """
        if VERBOSE: xp.log('before_physics')
        self.no_before_physics = True

    def after_physics(self) -> None:
        """ XLUA
        Called every frame, after physics has been calculated, while the sim is not paused
        and not in replay.
        """
        if VERBOSE: xp.log('after_physics')
        self.no_after_physics = True

    def after_replay(self) -> None:
        """ XLUA called every frame that the sim is in reply mode, regardless of pause status"""
        if VERBOSE: xp.log('after_replay')
        self.no_after_replay = True

    #######################################
    # SASL - type callbacks
    def update(self) -> None:
        """SASL update()"""
        if VERBOSE: xp.log('update')
        self.no_update = True

    def onAirportLoaded(self, flightNumber: int) -> None:
        if VERBOSE: xp.log(f'airport loaded {flightNumber=}')

    def onAirplaneCountChanged(self) -> None:
        if VERBOSE: xp.log('plane count changed')

    def onDisable(self) -> None:
        if VERBOSE: xp.log('disable')

    def onStop(self) -> None:
        if VERBOSE: xp.log('stop')

    def onEnable(self) -> int:
        if VERBOSE: xp.log('enable')
        return 1

    def onStart(self) -> None:
        if VERBOSE: xp.log('start')

    def onLiveryLoaded(self) -> None:
        if VERBOSE: xp.log('liveryLoaded')

    def onPlaneLoaded(self) -> None:
        if VERBOSE: xp.log('planeLoaded')

    def onPlaneUnloaded(self) -> None:
        if VERBOSE: xp.log('planeUnLoaded')

    def onSceneryLoaded(self) -> None:
        if VERBOSE: xp.log('sceneryLoaded')

    def onFirstFlight(self) -> None:
        if VERBOSE: xp.log('onFirstFlight')

    def onPlaneCrash(self) -> None:
        if VERBOSE: xp.log('onPlaneCrash')

    onModuleInit = onFirstFlight
    onModuleShutdown = onDisable
    onModuleDown = onStop
