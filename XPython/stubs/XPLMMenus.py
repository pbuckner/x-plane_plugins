from typing import Union, Callable, Any
from XPPython3.xp import XPLMMenuID, XPLMCommandRef

Menu_NoCheck = 0
Menu_Unchecked = 1
Menu_Checked = 2


def findPluginsMenu() -> XPLMMenuID:
    return XPLMMenuID(0)


def findAircraftMenu() -> XPLMMenuID:
    return XPLMMenuID(0)


def createMenu(name: Union[None, str] = None,
                   parentMenuID: Union[None, XPLMMenuID] = None,
                   parentItem: int = 0,
                   handler: Union[None, Callable[[Any, Any], None]] = None,
                   refCon: Union[None, Any] = None) -> XPLMMenuID:
    return XPLMMenuID(0)


def destroyMenu(menuID: XPLMMenuID) -> None:
    return


def clearAllMenuItems(menuID: XPLMMenuID) -> None:
    return


def appendMenuItem(menuID: Union[None, XPLMMenuID] = None, name: str = 'Item', refCon: Union[None, Any] = None) -> int:
    return int()  # menu idx


def appendMenuItemWithCommand(menuID: Union[None, XPLMMenuID] = None,
                                  name: str = "Command",
                                  commandRef: Union[None, XPLMCommandRef] = None) -> int:
    return int()  # menu idx


def appendMenuSeparator(menuID: Union[None, XPLMMenuID] = None) -> None:
    return


def setMenuItemName(menuID: Union[None, XPLMMenuID] = None,
                        index: int = 0, name: str = "New Name") -> None:
    return


def checkMenuItem(menuID: Union[None, XPLMMenuID] = None,
                      index: int = 0, checked: int = 2) -> None:
    return


def checkMenuItemState(menuID: Union[None, XPLMMenuID] = None, index: int = 0) -> int:
    return int()


def enableMenuItem(menuID: Union[None, XPLMMenuID] = None, index: int = 0, enabled: int = 1) -> None:
    return


def removeMenuItem(menuID: Union[None, XPLMMenuID] = None, index: int = 0) -> None:
    return
