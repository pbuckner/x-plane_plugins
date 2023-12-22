from dataclasses import dataclass
from typing import Any, Callable, Generic, Optional, Type, TypeVar, NewType, Literal, Sequence
from XPPython3.xp_typing import *    
def getMETARForAirport(airport_id:str) -> str:
    """
    Returns 'last known' METAR report for given airport.
    
    Note the actual weather may have evolved significantly since
    the last downloaded METAR.
    """
    ...

def getWeatherAtLocation(latitude:float, longitude:float, altitude_m:float) -> None | XPLMWeatherInfo_t:
    """
    Returns current weather conditions at given location.
    Note this appears to work world-wide, though accuracy suffers with distance from current
    position.
    
    Returns WeatherInfo object on success, None otherwise. This call
    is not intended to be used per-frame.
    """
    ...

