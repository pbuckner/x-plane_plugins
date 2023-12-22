from dataclasses import dataclass
from typing import Any, Callable, Generic, Optional, Type, TypeVar, NewType, Literal, Sequence
from XPPython3.xp_typing import *    
Tex_GeneralInterface: XPLMTextureID
Font_Basic: XPLMFontID
Font_Proportional: XPLMFontID
def setGraphicsState(fog:int=0, numberTexUnits:int=0, lighting:int=0, alphaTesting:int=0, alphaBlending:int=0, depthTesting:int=0, depthWriting:int=0) -> None:
    """
    Change OpenGL's graphics state.
    
    Use instead of any glEnable / glDisable calls.
    """
    ...

def bindTexture2d(textureID:int, textureUnit:int) -> None:
    """
    Changes currently bound OpenGL texture.
    
    Use instead of glBindTexture(GL_TEXTURE_2D, ...)
    """
    ...

def generateTextureNumbers(count:int=1) -> None | list[int]:
    """
    Generate number of textures for a plugin.
    
    Returns list of numbers.
    """
    ...

def worldToLocal(lat:float, lon:float, alt:float=0.0) -> tuple[float, float, float]:
    """
    Convert Lat/Lon/Alt to local scene coordinates (x, y, z)
    
    Latitude and longitude are decimal degrees, altitude is meters MSL.
    Returns (x, y, z) in meters, in local OpenGL coordinates.
    """
    ...

def localToWorld(x:float, y:float, z:float) -> tuple[float, float, float]:
    """
    Convert local scene coordinates (x, y, z) into (lat, lon, alt)
    
    Latitude and longitude are decimal degrees, altitude is meters MSL.
    """
    ...

def drawTranslucentDarkBox(left:int, top:int, right:int, bottom:int) -> None:
    """
    Draw translucent dark box at location.
    """
    ...

def drawString(rgb:Sequence[float]=(1., 1., 1.), 
          x:int=0, y:int=0, value:str='', 
          wordWrapWidth:Optional[int]=None, fontID:XPLMFontID=Font_Proportional) -> None:
    """
    Draw a string at location (x, y)
    
    Default color is white (1., 1., 1.)
    """
    ...

def drawNumber(rgb:Sequence[float]=(1., 1., 1.), x:int=0, y:int=0, value:float=0.0, digits:int=-1, decimals:int=0, showSign:int=1, fontID:XPLMFontID=Font_Proportional) -> None:
    """
    Draw a number at location (x, y)
    
    Default color is white (1., 1., 1.)
    """
    ...

def getFontDimensions(fontID:XPLMFontID) -> None | tuple[int, int, int]:
    """
    Get information about font
    
    Returns (width, height, digitsOnly). Proportional fonts
    return hopefully average width.
    """
    ...

def measureString(fontID:XPLMFontID, string:str) -> float:
    """
    Returns floating point width of string, with indicated font.
    """
    ...

