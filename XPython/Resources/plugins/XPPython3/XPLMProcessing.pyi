from dataclasses import dataclass
from typing import Any, Callable, Generic, Optional, Type, TypeVar, NewType, Literal, Sequence
from XPPython3.xp_typing import *    
FlightLoop_Phase_BeforeFlightModel: XPLMFlightLoopPhaseType
FlightLoop_Phase_AfterFlightModel: XPLMFlightLoopPhaseType
def getElapsedTime() -> float:
    """
    Return elapsed time since sim started.
    """
    ...

def getCycleNumber() -> int:
    """
    Get cycle number, increased for each cycle computed by sim.
    """
    ...

def registerFlightLoopCallback(callback:Callable[[float, float, int, Any], float], interval:float=0.0, refCon:Any=None) -> None:
    """
    Register flight loop callback.
    
    interval indicates when you'll be called next:
      0= deactivate 
      >0 seconds 
      <0 flightLoops
    Callback function gets (lastCall, elapsedTime, counter, refCon)
    """
    ...

def unregisterFlightLoopCallback(callback:Callable[[float, float, int, Any], float], refCon:Any=None) -> None:
    """
    Unregisters flightloop matching callback and refCon.
    
    Input must match that provided to registerFlightLoopCallback().
    """
    ...

def setFlightLoopCallbackInterval(callback:Callable[[float, float, int, Any], float], interval:float=0.0, relativeToNow:int=1, refCon:Any=None) -> None:
    """
    Change interval associated with callback, refCon.
    
    Must have been previously registered with registerFlightLoopCallback()
    """
    ...

def createFlightLoop(callback:Callable[[float, float, int, Any], float], 
          phase:XPLMFlightLoopPhaseType=FlightLoop_Phase_BeforeFlightModel, refCon:Any=None) -> XPLMFlightLoopID:
    """
    Create flight loop, returning flightLoopID
    
    Callback take (sinceLast, elapsedTime, counter, refCon)
    returning interval (0=stop, >0 seconds, <0 flightloops)
    phase is 0=before or 1=After flight model integration
    """
    ...

def destroyFlightLoop(flightLoopID:XPLMFlightLoopID) -> None:
    """
    Destroys flight loop previously created by createFlightLoop()
    """
    ...

def scheduleFlightLoop(flightLoopID:XPLMFlightLoopID, interval:float=0.0, relativeToNow:int=1) -> None:
    """
    Change interval associated with flight loop
    
      0= stop
      >0 seconds
      <0 flightLoops
    If relativeToNow is 1, interval is relative to now, duh, otherwise
    interval is relative to previous callback execution.
    """
    ...

