from typing import Any, Union, Callable, Tuple
from xp_hinting import XPLMMapLayerID, XPLMMapProjectionID

MapStyle_VFR_Sectional = 0
MapStyle_IFR_LowEnroute = 1
MapStyle_IFR_HighEnroute = 2
MapLayer_Fill = 0
MapLayer_Markings = 1
MAP_USER_INTERFACE = "XPLM_MAP_USER_INTERFACE"
MAP_IOS = "XPLM_MAP_IOS"
MapOrientation_Map = 0
MapOrientation_UI = 1


def createMapLayer(mapType: str = MAP_USER_INTERFACE,
                   layerType: int = MapLayer_Markings,
                   delete: Union[None, Callable[[XPLMMapLayerID, Any], None]] = None,
                   prep: Union[None, Callable[[XPLMMapLayerID, Tuple[float, float, float, float],
                                               XPLMMapProjectionID, Any], Any]] = None,
                   draw: Union[None, Callable[[XPLMMapLayerID, Tuple[float, float, float, float],
                                               float, float, int, XPLMMapProjectionID, Any], None]] = None,
                   icon: Union[None, Callable[[XPLMMapLayerID, Tuple[float, float, float, float],
                                               float, float, int, XPLMMapProjectionID, Any], None]] = None,
                   label: Union[None, Callable[[XPLMMapLayerID, Tuple[float, float, float, float],
                                                float, float, int, XPLMMapProjectionID, Any], None]] = None,
                   showToggle: int = 1,
                   name: str = "",
                   refCon: Any = None) -> Union[None, XPLMMapLayerID]:
    return XPLMMapLayerID(0)


def destroyMapLayer(layerID: XPLMMapLayerID) -> int:
    return int()  # 1 if a deletion occurs


def registerMapCreationHook(mapCreated: Callable[[str, Any], None], refCon: Any = None) -> None:
    return


def mapExists(mapType: str) -> int:
    return int()  # 1 if map already exists


def drawMapIconFromSheet(layerID: XPLMMapLayerID,
                         png: str,
                         s: int, t: int, ds: int, dt: int,
                         x: float, y: float,
                         orientation: int,
                         rotationDegrees: float,
                         mapWidth: float) -> None:
    return


def drawMapLabel(layerID: XPLMMapLayerID,
                 label: str,
                 x: float, y: float,
                 orientation: int,
                 rotationDegrees: float) -> None:
    return


def mapProject(projection: XPLMMapProjectionID, latitude: float, longitude: float) -> Tuple[float, float]:
    return float(), float()  # x, y


def mapUnproject(projection: XPLMMapProjectionID, x: float, y: float) -> Tuple[float, float]:
    return float(), float()  # latitude, longitude


def mapScaleMeter(projection: XPLMMapProjectionID, x: float, y:float) -> float:
    return float()  # number of map units corresponding to a distance of one meter


def mapGetNorthHeading(projection: XPLMMapProjectionID, x: float, y: float) -> float:
    return float()  # degrees clockwise from "up"
