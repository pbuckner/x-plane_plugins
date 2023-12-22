from dataclasses import dataclass
from typing import Any, Callable, Generic, Optional, Type, TypeVar, NewType, Literal, Sequence
from XPPython3.xp_typing import *    
Nav_Any: XPLMNavType
Nav_Unknown: XPLMNavType
Nav_Airport: XPLMNavType
Nav_NDB: XPLMNavType
Nav_VOR: XPLMNavType
Nav_ILS: XPLMNavType
Nav_Localizer: XPLMNavType
Nav_GlideSlope: XPLMNavType
Nav_OuterMarker: XPLMNavType
Nav_MiddleMarker: XPLMNavType
Nav_InnerMarker: XPLMNavType
Nav_Fix: XPLMNavType
Nav_DME: XPLMNavType
Nav_LatLon: XPLMNavType
NAV_NOT_FOUND: int
_Airport: int
NDB: int
VOR: int
def getFirstNavAid() -> int:
    """
    Returns navRef of first entry in navaid database.
    """
    ...

def getNextNavAid(navRef:XPLMNavRef) -> int:
    """
    Returns next navRef after the provided value.
    """
    ...

def findFirstNavAidOfType(navType:XPLMNavType) -> int:
    """
    Returns navRef of first navAid of given type.
    
    Types are:
      Nav_Unknown        =0
      Nav_Airport        =1
      Nav_NDB            =2
      Nav_VOR            =4
      Nav_ILS            =8
      Nav_Localizer     =16
      Nav_GlideSlope    =32
      Nav_OuterMarker   =64
      Nav_MiddleMarker =128
      Nav_InnerMarker  =256
      Nav_Fix          =512
      Nav_DME         =1024
      Nav_LatLon      =2048
    
    """
    ...

def findLastNavAidOfType(navType:XPLMNavType) -> int:
    """
    Returns navRef of last navAid of given type.
    """
    ...

def findNavAid(name:Optional[str], navAidID:Optional[str], lat:Optional[float], 
          lon:Optional[float], freq:Optional[int], navType:XPLMNavType=Nav_Any) -> XPLMNavRef:
    """
    Returns navRef of last navAid matching information.
    
    name and navAidID are case-sensitive and will match a fragment of the actual value.
    freq is an integer, 100x the real frequency value (eg. 137.75 -> 13775) except for NDB.
    navType=0xffffffff will match any type value
    """
    ...

def getNavAidInfo(navRef:XPLMNavRef) -> NavAidInfo:
    """
    Returns NavAidInfo object for given navRef
    
    Attibutes are:
     .type        # a NavType
     .latitude
     .longitude
     .height     # in meters
     .frequency  # integer, for NDB, value is exact, otherwise devide by 100.0
     .heading    # See documentation for glideslope headings
     .navAidID
     .reg        # =1 if navaid is within local 'region' of loaded DSFs
    """
    ...

def countFMSEntries() -> int:
    """
    Returns number of FMS Entries
    """
    ...

def getDisplayedFMSEntry() -> int:
    """
    Returns index number of currently displayed FMS entry.
    """
    ...

def getDestinationFMSEntry() -> int:
    """
    Returns index number of destination FMS entry.
    """
    ...

def setDisplayedFMSEntry(index:int) -> None:
    """
    Sets index number for FMS Entry to be displayed.
    """
    ...

def setDestinationFMSEntry(index:int) -> None:
    """
    Sets index number for FMS Entry to become the current destination.
    """
    ...

def getFMSEntryInfo(index:int) -> FMSEntryInfo:
    """
    Return FMSEntryInfo object for given FMS Entry index.
    
    Attributes are:
     .type      # a NavType
     .navAidID
     .ref       # navRef (use as input to getNavAidInfo())
     .altitude  # (in feet)
     .lat
     .lon
    """
    ...

def setFMSEntryInfo(index:int, navRef:XPLMNavRef, altitude:int=0) -> None:
    """
    Set given FMS Entry to provided navRef and altitude (feet)
    """
    ...

def setFMSEntryLatLon(index:int, lat:float, lon:float, altitude:int=0) -> None:
    """
    Set given FMS Entry to provided (lat, lon) and altitude(feet).
    """
    ...

def clearFMSEntry(index:int) -> None:
    """
    Clear given FMS entry.
    """
    ...

def getGPSDestinationType() -> int:
    """
    Return NavType of current GPS Destination.
    """
    ...

def getGPSDestination() -> int:
    """
    Return navRef of current GPS Destination.
    """
    ...

