from dataclasses import dataclass
from typing import Any, Callable, Generic, Optional, Type, TypeVar, NewType, Literal, Sequence
from XPPython3.xp_typing import *    
MapStyle_VFR_Sectional: XPLMMapStyle
MapStyle_IFR_LowEnroute: XPLMMapStyle
MapStyle_IFR_HighEnroute: XPLMMapStyle
MapLayer_Fill: XPLMMapLayerType
MapLayer_Markings: XPLMMapLayerType
MAP_USER_INTERFACE: str
MAP_IOS: str
MapOrientation_Map: XPLMMapOrientation
MapOrientation_UI: XPLMMapOrientation
def createMapLayer(mapType:str=MAP_USER_INTERFACE, 
          layerType:XPLMMapLayerType=MapLayer_Markings, 
          delete:Optional[Callable[[XPLMMapLayerID, Any], None]]=None, 
          prep:Optional[Callable[[XPLMMapLayerID, float, XPLMMapProjectionID, Any], None]]=None, 
          draw:Optional[Callable[[XPLMMapLayerID, float, float, float, XPLMMapStyle, XPLMMapProjectionID, Any], None]]=None, 
          icon:Optional[Callable[[XPLMMapLayerID, float, float, float, XPLMMapStyle, XPLMMapProjectionID, Any], None]]=None, 
          label:Optional[Callable[[XPLMMapLayerID, float, float, float, XPLMMapStyle, XPLMMapProjectionID, Any], None]]=None, 
          showToggle:int=1, 
          name:str='', 
          refCon:Any=None) -> XPLMMapLayerID:
    """
    Returns layerID of newly created map layer, setting callbacks.
    
    If map does not currently exist, returns 0.
    """
    ...

def destroyMapLayer(layerID:XPLMMapLayerID) -> int:
    """
    Destroys map layer given by layerID.
    """
    ...

def registerMapCreationHook(mapCreated:Callable[[str, Any], None], refCon:Any=None) -> None:
    """
    Registers mapCreated() callback to notify you when a map is created.
    
    Callback gets two parameters: (mapType, refCon)
    """
    ...

def mapExists(mapType:str) -> int:
    """
    Returns 1 if mapType exists, 0 otherwise.
    
    mapType is either xp.MAP_USER_INTERFACE or xp.MAP_IOS
    """
    ...

def drawMapIconFromSheet(layerID:XPLMMapLayerID, png:str, s:int, t:int, ds:int, dt:int, x:float, y:float, orientation:XPLMMapOrientation, rotationDegrees:float, mapWidth:float) -> None:
    """
    Draws icon into map layer.
    
    Only valid within iconLayer() callback.
    """
    ...

def drawMapLabel(layerID:XPLMMapLayerID, text:str, x:float, y:float, orientation:XPLMMapOrientation, rotationDegrees:float) -> None:
    """
    Draws label within map layer.
    
    Only valid within labelLayer() callback.
    """
    ...

def mapProject(projection:XPLMMapProjectionID, latitude:float, longitude:float) -> None | tuple[float, float]:
    """
    Returns map layer (x, y) for given latitude, longitude.
    
    Only valid within map layer callbacks.
    """
    ...

def mapUnproject(projection:XPLMMapProjectionID, x:float, y:float) -> None | tuple[float, float]:
    """
    Returns latitude, longitude for given map coordinates.

    Only valid within map layer callbacks.
    """
    ...

def mapScaleMeter(projection:XPLMMapProjectionID, x:float, y:float) -> float:
    """
    Returns number of units for 'one meter' using current projection.
    
    Only valid within map layer callbacks.
    """
    ...

def mapGetNorthHeading(projection:XPLMMapProjectionID, x:float, y:float) -> float:
    """
    Returns mapping angle for map projection at point.

    Only valid within map layer callbacks.
    """
    ...

def getMapCallbackDict() -> dict:
    """
    Returns internal dictionary of map callbacks

    Intended for debugging only
    """
    ...

