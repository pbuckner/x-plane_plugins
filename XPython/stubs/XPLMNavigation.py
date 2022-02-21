from typing import Union, Literal, Union
from xp_hinting import XPLMNavRef
Nav_Unknown = 0
Nav_Airport = 1
Nav_NDB = 2
Nav_VOR = 4
Nav_ILS = 8
Nav_Localizer = 16
Nav_GlideSlope = 32
Nav_OuterMarker = 64
Nav_MiddleMarker = 128
Nav_InnerMarker = 256
Nav_Fix = 512
Nav_DME = 1024
Nav_LatLon = 2048
NAV_NOT_FOUND = -1


def getFirstNavAid() -> XPLMNavRef:
    return XPLMNavRef(0)


def getNextNavAid(navRef: XPLMNavRef) -> Union[Literal[-1], XPLMNavRef]:
    return XPLMNavRef(0)  # XPLMNavRef


def findFirstNavAidOfType(navType: int) -> Union[Literal[-1], XPLMNavRef]:
    return XPLMNavRef(0)  # XPLMNavRef


def findLastNavAidOfType(navType: int) -> Union[Literal[-1], XPLMNavRef]:
    return XPLMNavRef(0)  # XPLMNavRef


def findNavAid(name: Union[None, str] = None,
               navAidId: Union[None, str] = None,
               lat: Union[None, float] = None,
               lon: Union[None, float] = None,
               freq: Union[None, int] = None,
               navType: int = NAV_NOT_FOUND) -> Union[Literal[-1], XPLMNavRef]:
    return XPLMNavRef(0)  # XPLMNavRef


class NavAidInfo:
    type = 0  # XPLMNavType
    latitude = 0
    longitude = 0
    height = 0
    frequency = 0
    heading = 0
    navAidID = None
    name = None
    reg = 0


def getNavAidInfo(navRef: XPLMNavRef) -> NavAidInfo:
    return NavAidInfo()


def countFMSEntries() -> int:
    return int()  # count


def getDisplayedFMSEntry() -> int:
    return int()  # index


def getDestinationFMSEntry() -> int:
    return int()  # index


def setDisplayedFMSEntry(index: int) -> None:
    return


def setDestinationFMSEntry(index: int) -> None:
    return


class FMSEntryInfo:
    type = 0  # XPLMNavType
    navAidID = None
    ref = 0  # XPLMNavRef
    altitude = 0
    lat = 0
    lon = 0


def getFMSEntryInfo(index: int) -> FMSEntryInfo:
    return FMSEntryInfo()


def setFMSEntryInfo(index: int, navRef: XPLMNavRef, altitude: int = 0) -> None:
    return


def setFMSEntryLatLon(index: int, lat: float, lon: float, altitude: int = 0) -> None:
    return


def clearFMSEntry(index: int) -> None:
    return


def getGPSDestinationType() -> int:
    return int(0)  # XPLMNavType


def getGPSDestination() -> XPLMNavRef:
    return XPLMNavRef(0)
