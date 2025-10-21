from dataclasses import dataclass
from typing import Any, Callable, Generic, Optional, Type, TypeVar, NewType, Literal, Sequence
from XPPython3.xp_typing import *    
MouseDown: XPLMMouseStatus
MouseDrag: XPLMMouseStatus
MouseUp: XPLMMouseStatus
WidgetClass_MainWindow: XPWidgetClass
WidgetClass_SubWindow: XPWidgetClass
WidgetClass_Button: XPWidgetClass
PushButton: int
Msg_PushButtonPressed: int
Msg_ButtonStateChanged: int
WidgetClass_TextField: XPWidgetClass
Msg_TextFieldChanged: int
WidgetClass_ScrollBar: XPWidgetClass
Msg_ScrollBarSliderPositionChanged: int
WidgetClass_Caption: XPWidgetClass
WidgetClass_GeneralGraphics: XPWidgetClass
WidgetClass_Progress: XPWidgetClass
Property_UserStart: XPWidgetPropertyID
Mode_Direct: XPDispatchMode
Mode_UpChain: XPDispatchMode
Msg_Create: XPWidgetMessage
Msg_Destroy: XPWidgetMessage
Msg_KeyPress: XPWidgetMessage
Msg_MouseDown: XPWidgetMessage
Msg_MouseDrag: XPWidgetMessage
Msg_MouseUp: XPWidgetMessage
Msg_Reshape: XPWidgetMessage
Msg_AcceptChild: XPWidgetMessage
Msg_LoseChild: XPWidgetMessage
Msg_AcceptParent: XPWidgetMessage
Msg_Shown: XPWidgetMessage
Msg_Hidden: XPWidgetMessage
Msg_PropertyChanged: XPWidgetMessage
Msg_MouseWheel: XPWidgetMessage
Msg_CursorAdjust: XPWidgetMessage
def createWidget(left:int, top:int, right:int, bottom:int, visible:int, descriptor:str, isRoot:int, 
          container:XPWidgetID | Literal[0], widgetClass:XPWidgetClass) -> XPWidgetID:
    """
    Create widget of class at location
    
    isRoot=1 if widget is a root widget, container is None or widgetID of parent widget
    widgetClass is one of predefined classes:
      WidgetClass_MainWindow
      WidgetClass_SubWindow
      WidgetClass_Button
      WidgetClass_TextField
      WidgetClass_ScrollBar
      WidgetClass_Caption
      WidgetClass_GeneralGraphics
      WidgetClass_Progress
    Returns created widgetID
    
    """
    ...

def createCustomWidget(left:int, top:int, right:int, bottom:int, visible:int, descriptor:str, isRoot:int, container:Optional[XPWidgetID], 
          callback:Callable[[XPWidgetMessage, XPWidgetID, int, int], int]) -> XPWidgetID:
    """
    Create widget at location, with custom callback
    
    callback is (message, widget, param1, param2) returning 1 if you've handled
       the message, 0 otherwise.
    Returns created widgetID
    """
    ...

def destroyWidget(widgetID:XPWidgetID, destroyChildren:int=1) -> None:
    """
    Destroys widgetID and (optionally) all children.
    """
    ...

def sendMessageToWidget(widgetID:XPWidgetID, message:XPWidgetMessage | int, dispatchMode:XPDispatchMode=Mode_UpChain, param1:int=0, param2:int=0) -> int:
    """
    dispatchMode default is Mode_UpChain
    """
    ...

def placeWidgetWithin(widgetID:XPWidgetID, container:XPWidgetID | Literal[0] = 0) -> None:
    """
    Change container widget for widgetID to container (widgetID)
    """
    ...

def countChildWidgets(widgetID:XPWidgetID) -> int:
    """
    Return number of child widgets for this widgetID
    """
    ...

def getNthChildWidget(widgetID:XPWidgetID, index:int) -> XPWidgetID:
    """
    Return widgetID of 0-based nth child
    """
    ...

def getParentWidget(widgetID:XPWidgetID) -> XPWidgetID:
    """
    Return widgetID for parent (i.e., container) of this widgetID
    """
    ...

def showWidget(widgetID:XPWidgetID) -> None:
    """
    Make widget visible.
    """
    ...

def hideWidget(widgetID:XPWidgetID) -> None:
    """
    Hide widget
    """
    ...

def isWidgetVisible(widgetID:XPWidgetID) -> int:
    """
    Return 1 if widget is visible
    
    Widget must be itself visible and contained in visible parent.
    Note if widget is outside of parent's geometry it may be clipped
    being reported 'visible' yet still not seen by user.
    """
    ...

def findRootWidget(widgetID:XPWidgetID) -> XPWidgetID:
    """
    Return top-most widget container for given widgetID
    
    If widget is root widget, it will return itself.
    """
    ...

def bringRootWidgetToFront(widgetID:XPWidgetID) -> None:
    """
    Make whole widget hierarchy containing widgetID to the front
    """
    ...

def isWidgetInFront(widgetID:XPWidgetID) -> int:
    """
    Return 1 if widget's hierarchy is front most.
    """
    ...

def getWidgetGeometry(widgetID:XPWidgetID) -> tuple[int, int, int, int]:
    """
    Return bounding box (left, top, right, bottom) of widgetID
    """
    ...

def setWidgetGeometry(widgetID:XPWidgetID, left:int, top:int, right:int, bottom:int) -> None:
    """
    Set bounding box for widgetID
    """
    ...

def getWidgetForLocation(container:XPWidgetID, xOffset:int, yOffset:int, recursive:int=1, visibleOnly:int=1) -> XPWidgetID:
    """
    Return widgetID of the child widget within the container widget at offset
    
    offsets are global coordinates, not relative bounding box of container.
    recursive=1 indicates find 'deepest' child widget
    visibleOnly=1 indicates only visible widgets are considered
    """
    ...

def getWidgetExposedGeometry(widgetID:XPWidgetID) -> tuple[int, int, int, int]:
    """
    Return (left, top, right, bottom) of widget's exposed geometry
    """
    ...

def setWidgetDescriptor(widgetID:XPWidgetID, descriptor:str) -> None:
    """
    Set widget's descriptor string
    """
    ...

def getWidgetDescriptor(widgetID:XPWidgetID) -> str:
    """
    Returns widget's descriptor string
    """
    ...

def getWidgetUnderlyingWindow(widgetID:XPWidgetID) -> XPLMWindowID:
    """
    Return windowID of window underlying widget
    """
    ...

def setWidgetProperty(widgetID:XPWidgetID, propertyID:XPWidgetPropertyID | int, value:Optional[Any]) -> None:
    """
    Set widget property to value
    """
    ...

def getWidgetProperty(widgetID:XPWidgetID, propertyID:XPWidgetPropertyID | int, exists:int=-1) -> Any:
    """
    Returns widget's property value. For XP properties, it is a long, for
    User-defined properties, it can be any python object.
    
    Raises a ValueError if exists=-1 and property does not exist / has not been set
    Set exists=None if you don't care if property exists (value will be 0)
    Set exists to a list object, and we'll set it to [1,] if property exists, 
      [0, ] otherwise
    """
    ...

def setKeyboardFocus(widgetID:XPWidgetID) -> int:
    """
    Set keyboard focus to widgetID
    """
    ...

def loseKeyboardFocus(widgetID:XPWidgetID) -> None:
    """
    Cause widgetID to lose keyboard focus
    """
    ...

def getWidgetWithFocus() -> XPWidgetID:
    """
    Return widgetID with current focus. 0=X-Plane has focus.
    """
    ...

def addWidgetCallback(widgetID:XPWidgetID, callback:Callable[[XPWidgetMessage, XPWidgetID, int, int], int]) -> None:
    """
    Add callback to widgetID
    
    Callback has signature (message, widgetID, param1, param2). See
    createCustomWidget()
    """
    ...

def getWidgetClassFunc(widgetClass:XPWidgetClass) -> int:
    """
    Given widgetClass, return underlying function.

    Not useful with python. Use addWidgetCalback() instead.
    """
    ...

def getWidgetCallbackDict() -> dict:
    """
    Returns internal dictionary of widget callbacks

    Intended for debugging only
    """
    ...

def getWidgetPropertiesDict() -> dict:
    """
    Returns internal dictionary of widget properties

    Intended for debugging only
    """
    ...

