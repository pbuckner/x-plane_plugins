from dataclasses import dataclass
from typing import Any, Callable, Generic, Optional, Type, TypeVar, NewType, Literal, Sequence
from XPPython3.xp_typing import *    
Menu_NoCheck: XPLMMenuCheck
Menu_Unchecked: XPLMMenuCheck
Menu_Checked: XPLMMenuCheck
def findPluginsMenu() -> XPLMMenuID:
    """
    Returns menuID of plugins-menu.
    """
    ...

def findAircraftMenu() -> XPLMMenuID:
    """
    Returns menuID of currently loaded aircraft plugins menu.
    
    Note this is always 'None' for XPPython3.
    """
    ...

def createMenu(name:Optional[str]=None, parentMenuID:Optional[XPLMMenuID]=None, parentItem:Optional[int]=0, 
          handler:Optional[Callable[[Any, Any], None]]=None, refCon:Optional[Any]=None) -> None | XPLMMenuID:
    """
    Creates menu, returning menuID or None on error.
    
    parentMenuID=None adds menu to PluginsMenu.
    """
    ...

def destroyMenu(menuID:XPLMMenuID) -> None:
    """
    Remove submenu from provided menuID.
    """
    ...

def clearAllMenuItems(menuID:Optional[XPLMMenuID]) -> None:
    """
    Remove menu items from provided menuID, or 'all menus', if menuID is None.
    """
    ...

def appendMenuItem(menuID:Optional[XPLMMenuID]=None, name:str='Item', refCon:Any=None) -> int:
    """
    Appends new menu item to end of existing menuID.
    
    Returns index for created menu item or -1 on error.
    """
    ...

def appendMenuItemWithCommand(menuID:Optional[XPLMMenuID]=None, name:str='Command', commandRef:Any=None) -> int:
    """
    Adds menu item to existing menuID, and executes commandRef when selected.
    
    Returns index for created menu item or -1 on error.
    """
    ...

def appendMenuSeparator(menuID:XPLMMenuID=None) -> None | int:
    """
    Adds separator to end of menu.
    
    Returns index of created item for XP11, None for XP12
    
    """
    ...

def setMenuItemName(menuID:Optional[XPLMMenuID]=None, index:int=0, name:str='New Name') -> None:
    """
    Change the name of and existing menu item.
    """
    ...

def checkMenuItem(menuID:Optional[XPLMMenuID]=None, index:int=0, checked:XPLMMenuCheck=Menu_Checked) -> None:
    """
    Set checkmark for given menu item.
    
      Menu_NoCheck = 0
      Menu_UnChecked = 1
      Menu_Checked = 2
    
    """
    ...

def checkMenuItemState(menuID:Optional[XPLMMenuID], index:int=0) -> int:
    """
    Returns menu item checked state.
    
      Menu_NoCheck = 0
      Menu_UnChecked = 1
      Menu_Checked = 2
    
    """
    ...

def enableMenuItem(menuID:Optional[XPLMMenuID]=None, index:int=0, enabled:int=1) -> None:
    """
    Enables menu item
    
    Use enabled=0 to disable item, 1 to enable
    """
    ...

def removeMenuItem(menuID:Optional[XPLMMenuID]=None, index:int=0) -> None:
    """
    Removes one item from menu.

    Note that all menu items below are moved up one index.
    """
    ...

def getMenuCallbackDict() -> dict:
    """
    Returns internal dictionary of menu callbacks

    Intended for debugging only
    """
    ...

