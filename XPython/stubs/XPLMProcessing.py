from typing import Callable, Any
from xp_hinting import XPLMFlightLoopID
FlightLoop_Phase_BeforeFlightModel = 0
FlightLoop_Phase_AfterFlightModel = 1


def getElapsedTime() -> float:
    return float()  # seconds


def getCycleNumber() -> int:
    return int()  # cycle number


def registerFlightLoopCallback(callback: Callable[[float, float, int, Any], float],
                               interval: float = 0.0, refCon: Any = None) -> None:
    return


def unregisterFlightLoopCallback(callback: Callable[[float, float, int, Any], float], refCon: Any) -> None:
    return


def setFlightLoopCallbackInterval(callback: Callable[[float, float, int, Any], float],
                                  interval: float = 0.0, relativeToNow: int = 1, refCon: Any = None):
    return


def createFlightLoop(callback: Callable[[float, float, int, Any], float],
                     phase: int = FlightLoop_Phase_BeforeFlightModel, refCon: Any = None) -> XPLMFlightLoopID:
    return XPLMFlightLoopID(0)


def destroyFlightLoop(flightLoopID: XPLMFlightLoopID) -> None:
    return


def scheduleFlightLoop(flightLoopID: XPLMFlightLoopID, interval: float, relativeToNow: int = 1) -> None:
    return
