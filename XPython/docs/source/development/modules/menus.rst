XPLMMenus
=========
.. py:module:: XPLMMenus

To use::

  import XPLMMenus

Plug-ins can create menus in the menu bar of X-Plane.  This is done by
creating a menu and then creating items.  Menus are referred to by an
opaque ID.  Items are referred to by (zero-based) index number.

Menus are "sandboxed" between plugins---no plugin can access the menus of
any other plugin. Furthermore, all menu indices are relative to your
plugin's menus only; if your plugin creates two sub-menus in the Plugins
menu at different times, it doesn't matter how many other plugins also
create sub-menus of Plugins in the intervening time: your sub-menus will be
given menu indices 0 and 1. (The SDK does some work in the back-end to
filter out menus that are irrelevant to your plugin in order to deliver
this consistency for each plugin.)

When you create a menu item, you specify how we should handle clicks on
that menu item. You can either have the XPLM trigger a callback (the
:py:func:`XPLMMenuHandler_f` associated with the menu that contains the item), or you
can simply have a command be triggered (with no associated call to your
menu handler). The advantage of the latter method is that X-Plane will
display any keyboard shortcuts associated with the command. (In contrast,
there are no keyboard shortcuts associated with menu handler callbacks with
specific parameters.)


Functions
---------

.. py:function:: XPLMMenuHandler_f(menuRef, itemRef) -> None:
                 
 :param object menuRef: Reference constant for menu, set with :py:func:`XPLMCreateMenu`
 :param object itemRef: Reference constant for item, set with :py:func:`XPLMAppendMenuItem`

 You provide a menu handler callback function which takes two reference pointers,
 one for the menu (specified when the menu was created) and one for the item (specified when
 the item was created).



.. py:function:: XPLMFindPluginsMenu(None) -> menuID:

 This function returns the ID of the plug-ins menu, which is created for you
 at startup.

 :return int menuID: :ref:`XPLMMenuID`


.. py:function::  XPLMFindAircraftMenu(None) -> menuID:

 This function returns the ID of the menu for the currently-loaded aircraft,
 used for showing aircraft-specific commands.

 :return int menuID: :ref:`XPLMMenuID`

 The aircraft menu is created by X-Plane at startup, but it remains hidden
 until it is populated via :py:func:`XPLMAppendMenuItem` or
 :py:func:`XPLMAppendMenuItemWithCommand`.

 Only plugins loaded with the user's current aircraft are allowed to access
 the aircraft menu. For all other plugins, this will return None, and any
 attempts to add menu items to it will fail.


.. py:function:: XPLMCreateMenu(name, parentMenu, parentItem, callback, menuRefCon) -> menuID

 :param str name: Menu's name, only used if the menu is in the menubar.                 
 :param int parentMenu: :ref:`XPLMMenuID` Parent's menu ID or None
 :param int parentItem: integer index of parent menu.
 :param callback: :py:func:`XPLMMenuHandler_f` callback you provide to handle interaction. May be None if you do not need callbacks.
 :param object menuRefCon: Reference constant to be returned to you inside your callback.                 

 This function creates a new menu and returns its ID.  It returns None if
 the menu cannot be created.  Pass in a parent menu ID and an item index to
 create a submenu, or None for the parent menu to put the menu in the menu
 bar.  The menu's name is only used if the menu is in the menubar.  You also
 pass a handler function and a menu reference value. Pass None for the
 handler if you do not need callbacks from the menu (for example, if it only
 contains submenus).

 .. note:: You must pass a valid, non-empty menu title even if the menu is
   a submenu where the title is not visible.


.. py:function:: XPLMDestroyMenu(menuID) -> None:

 :param menuID: :ref:`XPLMMenuID`  

 This function destroys a menu that you have created.  Use this to remove a
 submenu if necessary.  (Normally this function will not be necessary.)


.. py:function:: XPLMClearAllMenuItems(menuID):

 :param menuID: :ref:`XPLMMenuID`

 This function removes all menu items from a menu, allowing you to rebuild
 it.  Use this function if you need to change the number of items on a menu.


.. py:function::  XPLMAppendMenuItem(menuID, itemName, itemRefCon) -> int: 

 :param menuID: :ref:`XPLMMenuID` of menu on which to append the new item
 :param str itemName: Name displayed in the menu
 :param object itemRefCon: Reference constant to be returned in your callback
 :return: int index of of added item or negative number of append failed.

 This routine appends a new menu item to the bottom of a menu and returns
 its index. Pass in the menu to add the item to, the items name, and a
 itemRefCon for this item.

 Returns a negative index if the append failed (due to an invalid parent
 menu argument).

 Note that all menu indices returned are relative to your plugin's menus
 only; if your plugin creates two sub-menus in the Plugins menu at different
 times, it doesn't matter how many other plugins also create sub-menus of
 Plugins in the intervening time: your sub-menus will be given menu indices
 0 and 1. (The SDK does some work in the back-end to filter out menus that
 are irrelevant to your plugin in order to deliver this consistency for each
 plugin.)


.. py:function:: XPLMAppendMenuItemWithCommand(menuID, itemName, commandRef):

 :param int menuID: :ref:`XPLMMenuID`
 :param str itemName: Name to be displayed in the menu                    
 :param int commandRef: :ref:`XPLMCommandRef` such as returned by :py:func:`XPLMFindCommand` or :py:func:`XPLMCreateCommand`
 :return: int index of of added item or negative number of append failed.

 Like :py:func:`XPLMAppendMenuItem`, but instead of the new menu item triggering the
 :py:func:`XPLMMenuHandler_f` of the containing menu, it will simply execute the
 command you pass in. Using a command for your menu item allows the user to
 bind a keyboard shortcut to the command and see that shortcut represented
 in the menu.

 Returns a negative index if the append failed (due to an invalid parent
 menu argument).

 Like :py:func:`XPLMAppendMenuItem`, all menu indices are relative to your plugin's
 menus only.


.. py:function:: XPLMAppendMenuSeparator(menuID) -> int:

 This routine adds a separator to the end of a menu.

 :param int menuID: :ref:`XPLMMenuId`

 Returns a negative index if the append failed (due to an invalid parent
 menu argument).


.. py:function:: XPLMSetMenuItemName(menuID, index, itemName) -> None:

 :param int menuID: :ref:`XPLMMenuId`
 :param int index: index of menu item to be changed
 :param str itemName: New menu item name

 This routine changes the name of an existing menu item.  Pass in the menu
 ID and the index of the menu item.


.. py:function:: XPLMCheckMenuItem(menuID, index, check):

 :param int menuID: :ref:`XPLMMenuId`
 :param int index: index of menu item to be changed
 :param int check: 1= set checkmark

 Set whether a menu item is checked.  Pass in the menu ID and item index.


.. py:function:: XPLMCheckMenuItemState(menuID, index) -> menuCheck:

 :param int menuID: :ref:`XPLMMenuId`
 :param int index: index of menu item to be changed
 :return int menuCheck: one of :ref:`XPLMMenuCheck`

 This routine returns whether a menu item is checked or not. A menu item's
 check mark may be on or off, or a menu may not have an icon at all.


.. py:function:: XPLMEnableMenuItem(menuID, index, enable) -> None:

 :param int menuID: :ref:`XPLMMenuId`
 :param int index: index of menu item to be changed
 :param int enabel: 1= enable this item

 Sets whether this menu item is enabled.  Items start out enabled.


.. py:function:: XPLMRemoveMenuItem(menuID, index):

 :param int menuID: :ref:`XPLMMenuId`
 :param int index: index of menu item to be removed

 Removes one item from a menu.  Note that all menu items below are moved up
 one; your plugin must track the change in index numbers.

  
Constants
---------

.. _XPLMMenuID:

XPLMMenuID
**********

Unique ID for each menu you create.

.. _XPLMMenuCheck:

XPLMMenuCheck
*************

These enumerations define the various 'check' states for an X-Plane menu.
'checking' in X-Plane actually appears as a light which may or may not be
lit.  So there are three possible states.


 .. py:data:: xplm_Menu_NoCheck

  there is no symbol to the left of the menu item.

 .. py:data:: xplm_Menu_Unchecked

  the menu has a mark next to it that is unmarked (not lit).

 .. py:data:: xplm_Menu_Checked

  the menu has a mark next to it that is checked (lit).
              
Example
-------

Putting it together, adding "My Menu" to the main plugin menu, and having two
item, "Reset" which calls existing reset command, and "About" which displays an
About Window (code not provided)::

    class PythonInterface:
      def XPluginStart(self):
         self.menuID = XPLMCreateMenu("My Menu", None, self.callback, 'main')
         XPLMAppendMenuItem(XPLMFindPluginsMenu(), 'My Menu', 'main')  # attach to plugin menu
    
         # add my items to my menu
         XPLMAppendMenuItemWithCommand(self.menuID, 'Reset', XPLMFindCommand('sim/reset'))
         XPLMAppendMenuItem(self.menuID, 'About', 'about')
         return 'PluginName', 'PluginSig', 'PluginDesc'
    
      ...

      def callback(self, menuRefCon, itemRefCon):
         if menuRefCon == 'main' and itemRefCon == 'about':
             displayAboutWindow()
    
