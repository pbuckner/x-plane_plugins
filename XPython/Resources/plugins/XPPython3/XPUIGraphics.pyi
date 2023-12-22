from dataclasses import dataclass
from typing import Any, Callable, Generic, Optional, Type, TypeVar, NewType, Literal, Sequence
from XPPython3.xp_typing import *    
PushButton: int
WindowCloseBox: int
LittleDownArrow: int
LittleUpArrow: int
Ship: int
ILSGlideScope: int
MarkerLeft: int
_Airport: int
NDB: int
VOR: int
RadioTower: int
AircraftCarrier: int
Fire: int
MarkerRight: int
CustomObject: int
CoolingTower: int
SmokeStack: int
Building: int
PowerLine: int
VORWithCompassRose: int
OilPlatform: int
OilPlatformSmall: int
Window_Help: XPWindowStyle
Window_MainWindow: XPWindowStyle
Window_SubWindow: XPWindowStyle
Window_Screen: XPWindowStyle
Window_ListView: XPWindowStyle
Element_TextField: XPElementStyle
Element_CheckBox: XPElementStyle
Element_CheckBoxLit: XPElementStyle
Element_WindowCloseBox: XPElementStyle
Element_WindowCloseBoxPressed: XPElementStyle
Element_PushButton: XPElementStyle
Element_PushButtonLit: XPElementStyle
Element_OilPlatform: XPElementStyle
Element_OilPlatformSmall: XPElementStyle
Element_Ship: XPElementStyle
Element_ILSGlideScope: XPElementStyle
Element_MarkerLeft: XPElementStyle
Element_Airport: XPElementStyle
Element_Waypoint: XPElementStyle
Element_NDB: XPElementStyle
Element_VOR: XPElementStyle
Element_RadioTower: XPElementStyle
Element_AircraftCarrier: XPElementStyle
Element_Fire: XPElementStyle
Element_MarkerRight: XPElementStyle
Element_CustomObject: XPElementStyle
Element_CoolingTower: XPElementStyle
Element_SmokeStack: XPElementStyle
Element_Building: XPElementStyle
Element_PowerLine: XPElementStyle
Element_CopyButtons: XPElementStyle
Element_CopyButtonsWithEditingGrid: XPElementStyle
Element_EditingGrid: XPElementStyle
Element_ScrollBar: XPElementStyle
Element_VORWithCompassRose: XPElementStyle
Element_Zoomer: XPElementStyle
Element_TextFieldMiddle: XPElementStyle
Element_LittleDownArrow: XPElementStyle
Element_LittleUpArrow: XPElementStyle
Element_WindowDragBar: XPElementStyle
Element_WindowDragBarSmooth: XPElementStyle
Track_ScrollBar: XPTrackStyle
Track_Slider: XPTrackStyle
Track_Progress: XPTrackStyle
def drawWindow(left:int, bottom:int, right:int, top:int, style:XPWindowStyle=Window_MainWindow) -> None:
    """
    Draw window at location
    """
    ...

def getWindowDefaultDimensions(style:XPWindowStyle=Window_MainWindow) -> tuple[int, int]:
    """
    Default dimension for indicated style
    
    Returns (width, height)
    """
    ...

def drawElement(left:int, bottom:int, right:int, top:int, style:XPElementStyle, lit:int=0) -> None:
    """
    Draw element, possibly lit, at location.
    """
    ...

def getElementDefaultDimensions(style:XPElementStyle) -> tuple[int, int, int]:
    """
    Default dimension for indicated element
    
    Returns (width, height, canBeLit)
    """
    ...

def drawTrack(left:int, bottom:int, right:int, top:int, minValue:int, maxValue:int, value:int, style:XPTrackStyle, lit:int=0) -> None:
    """
    Draw track at location, with min/max values and current value.
    
    Track may be 'reversed' if minValue > maxValue.
    Styles are:
      Track_Scrollbar=0
      Track_Slider   =1
      Track_Progress =2
    """
    ...

def getTrackDefaultDimensions(style:XPTrackStyle) -> tuple[int, int]:
    """
    Default dimension for indicated track style
    
    Returns (width, canBeLit)
    """
    ...

def getTrackMetrics(left:int, bottom:int, right:int, top:int, minValue:int, maxValue:int, value:int, style:XPTrackStyle) -> TrackMetrics:
    """
    Return object with metrics about track
    
    Object attributes are:
      .isVertical
      .downBtnSize
      .downPageSize
      .thumbSize
      .upPageSize
      .upBtnSize
    """
    ...

