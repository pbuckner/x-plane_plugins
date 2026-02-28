from dataclasses import dataclass
from typing import Any, Callable, Generic, Optional, Type, TypeVar, NewType, Literal, Sequence
from XPPython3.xp_typing import *    
USER_AIRCRAFT: int
Init_Success: int
Init_Invalid: int
Init_MissingAircraft: int
Init_MissingLivery: int
Init_MissingAirport: int
Init_MissingRamp: int
Init_MissingRunway: int
def initFlight(data: str) -> int:
    """
    Initialize new flight
    ...

def updateFlight(data: str) -> int:
    """
    Updates current flight
    ...

def setUsersAircraft(path:str) -> None:
    """
    Change the user's aircraft and reinitialize.
    
    path is either relative X-Plane root, or fully qualified,
    including the .acf extension.
    """
    ...

def placeUserAtAirport(code:str) -> None:
    """
    Place user at given airport, specified by ICAO code.
    
    Invalid airport code will crash the sim.
    """
    ...

def placeUserAtLocation(latitude:float, longitude:float, elevation:float, heading:float, speed:float) -> None:
    """
    Place user aircraft at location with engines running.
    
    elevation is meters, heading is True, speed is meters per second
    """
    ...

def countAircraft() -> tuple[int, int, XPLMPluginID]:
    """
    Return tuple of (#a/c configured, #a/c active, pluginID of controlling plugin
    """
    ...

def getNthAircraftModel(index:int) -> tuple[str, str]:
    """
    Return (filename, fullPath) of aircraft at index
    """
    ...

def acquirePlanes(aircraft:Optional[Sequence[str]]=None, callback:Optional[Callable[[Any], None]]=None, refCon:Any=None) -> int:
    """
    Get exclusive access to aircraft.
    
    1 on success, 0 otherwise. You callback will be called
    with refCon if acquirePlanes() is not immediately successful.
    """
    ...

def releasePlanes() -> None:
    """
    Release all planes, acquired using acquirePlanes().
    """
    ...

def setActiveAircraftCount(count:int) -> None:
    """
    Set number of active planes.
    
    Only sets up to number of available planes. You must
    first have access -- acquirePlanes().
    """
    ...

def setAircraftModel(index:int, path:str) -> None:
    """
    Load aircraft model into index. (Do not use index=0)
    
    Path is absolute, or relative to X-Plane root.
    """
    ...

def disableAIForPlane(index:int) -> None:
    """
    Turn off AI for given airplane.
    
    Plane will continue to draw, but will not move itself.
    """
    ...

