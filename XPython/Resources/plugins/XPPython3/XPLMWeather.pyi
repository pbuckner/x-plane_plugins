from dataclasses import dataclass
from typing import Any, Callable, Generic, Optional, Type, TypeVar, NewType, Literal, Sequence
from XPPython3.xp_typing import *

NumWindLayers: int
NumCloudLayers: int
NumTemperatureLayers: int
WindUndefinedLayer: int
DefaultWxrRadiusNm: int
DefaultWxrLimitMslFt: int

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


def beginWeatherUpdate() -> None:
    """
    Inform the simulator that you are starting a batch update of weather information.
    You must call endWeatherUpdate() prior to exiting your callback in order for
    changes to be recorded.
    """
    ...


def endWeatherUpdate(isIncremental:int=1, updateImmediately:int=0) -> None:
    """
    Inform the simulator that you are ending a batch update of weather information.
    Incremental updates add to any previous weather updates you've provided, otherwise
    previous update passed by plugin are ignored. Immediate updates may cause a sudden
    jump in the weather: otherwise weather is transitioned to new data.
    """
    ...

def setWeatherAtLocation(latitude: float, longitude: float, ground_altitude_msl: float, info: XPLMWeatherInfo_t) -> None:
    """
    Set the current weather conditions at given location. See documentation
    for information on use of fields in XPLMWeatherInfo_t.

    Call beginWeatherUpdate() before setting weather, and endWeatherUpdate() after.
    """
    ...

def setWeatherAtAirport(airport_id:str, info:XPLMWeatherInfo_t) -> None:
    """
    Set the current weather conditions at given airport. See documentation
    for information on use of fields in XPLMWeatherInfo_t.

    Call beginWeatherUpdate() before setting weather, and endWeatherUpdate() after.
    """
    ...

def eraseWeatherAtLocation(latitude:float, longitude:float) -> None:
    """
    Erase plugin-provided current weather conditions at given location.

    Call beginWeatherUpdate() before erasing weather, and endWeatherUpdate() after.
    """
    ...

def eraseWeatherAtAirport(airport_id:str) -> None:
    """
    Erase plugin-provided current weather conditions at given airport.

    Call beginWeatherUpdate() before erasing weather, and endWeatherUpdate() after.
    """
    ...
