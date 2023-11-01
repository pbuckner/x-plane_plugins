from XPPython3.xp import XPLMWidgetID, XPLMWindowID
from typing import Callable, NewType, Any, Union, Literal, Tuple

def createWidget(left: int, top: int, right: int, bottom: int,
                 visible: int, descriptor: str, isRoot: int, container: Union[Literal[0], XPLMWidgetID],
                 widgetClass: int) -> XPLMWidgetID:
    return XPLMWidgetID(0)


def createCustomWidget(left: int, top: int, right: int, bottom: int,
                       visible: int, descriptor: str, isRoot: int, container: Union[Literal[0], XPLMWidgetID],
                       callback: Callable[[int, XPLMWidgetID, int, int], int]) -> XPLMWidgetID:
    return XPLMWidgetID(0)


def destroyWidget(widgetID: XPLMWidgetID, destroyChildren: int = 1) -> None:
    return


def sendMessageToWidget(widgetID: XPLMWidgetID,
                        message: int,
                        dispatchMode: int = 1,
                        param1: Any = 0,
                        param2: Any = 0) -> int:
    return int()  # 1=message handled; 0=otherwise


def placeWidgetWithin(widgetID: XPLMWidgetID, container: Union[Literal[0], XPLMWidgetID]) -> None:
    return


def countChildWidgets(widgetID: XPLMWidgetID) -> int:
    return int()


def getNthChildWidget(widgetID: XPLMWidgetID, index: int) -> Union[None, XPLMWidgetID]:
    return XPLMWidgetID(0)


def getParentWidget(widgetID: XPLMWidgetID) -> Union[Literal[0], XPLMWidgetID]:
    return XPLMWidgetID(0)


def showWidget(widgetID: XPLMWidgetID) -> None:
    return


def hideWidget(widgetID: XPLMWidgetID) -> None:
    return


def isWidgetVisible(widgetID: XPLMWidgetID) -> int:
    return int()  # 1=visible; 0=otherwise


def findRootWidget(widgetID: XPLMWidgetID) -> Union[None, XPLMWidgetID]:
    return XPLMWidgetID(0)  # XPWidgetID or Null if not in rooted hierarchy


def bringRootWidgetToFront(widgetID: XPLMWidgetID) -> None:
    return


def isWidgetInFront(widgetID: XPLMWidgetID) -> int:
    return int()  # 1=True


def getWidgetGeometry(widgetID: XPLMWidgetID) -> Tuple[int, int, int, int]:
    return int(), int(), int(), int()  # left, top, right, bottom


def setWidgetGeometry(widgetID: XPLMWidgetID,
                      left: int, top: int, right: int, bottom: int) -> None:
    return


def getWidgetForLocation(container: XPLMWidgetID,
                         xOffset: int, yOffset: int,
                         recursive: int = 1,
                         visibleOnly: int = 1) -> Union[Literal[0], XPLMWidgetID]:
    return XPLMWidgetID(0)


def getWidgetExposedGeometry(widgetID: XPLMWidgetID) -> Tuple[int, int, int, int]:
    return int(), int(), int(), int()  # left, top, right, bottom


def setWidgetDescriptor(widgetID: XPLMWidgetID, descriptor: str) -> None:
    return


def getWidgetDescriptor(widgetID: XPLMWidgetID) -> str:
    return str()


def getWidgetUnderlyingWindow(widgetID: XPLMWidgetID) -> Union[None, XPLMWindowID]:
    return XPLMWindowID(0)


def setWidgetProperty(widgetID: XPLMWidgetID, propertyID: int, value: Any = None) -> None:
    return


def getWidgetProperty(widgetID: XPLMWidgetID, propertyID: int, exists: Union[None, Literal[-1], list] = -1) -> Any:
    return Any  # pointer to property

def deleteWidgetProperty(widgetID: XPLMWidgetID, propertyID: int) -> Union[bool, None]:
    return bool()  # delete successful


def setKeyboardFocus(widgetID: XPLMWidgetID) ->  Union[Literal[0], XPLMWidgetID]:
    return XPLMWidgetID(0)  # XPWidgetID of widget with focus or 0 for X-Plane


def loseKeyboardFocus(widgetID: XPLMWidgetID) -> None:
    return


def getWidgetWithFocus() -> Union[Literal[0], XPLMWidgetID]:
    return XPLMWidgetID(0)  # XPWidgetID or 0 if X-Plane has focus


def addWidgetCallback(widgetID: XPLMWidgetID, callback: Callable[[int, XPLMWidgetID, Any, Any], int]) -> None:
    return


def getWidgetClassFunc(inWidgetClass):
    return int  # pointer to function
