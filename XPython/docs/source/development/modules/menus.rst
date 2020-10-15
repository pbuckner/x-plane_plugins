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

.. py:function:: XPLMMenuHandler_f(menuRefCon, itemRefCon) -> None:
                 
 :param object menuRefCon: Reference constant for menu, set with :py:func:`XPLMCreateMenu`
 :param object itemRefCon: Reference constant for item, set with :py:func:`XPLMAppendMenuItem`

 You provide a menu handler callback function which takes two reference pointers,
 one for the menu (specified when the menu was created) and one for the item (specified when
 the item was created). (Note these are reference constants, **not** MenuID or itemID.)


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

 :param str name: Menu's name, only used if parentMenu is None.
 :param int parentMenu: :ref:`XPLMMenuID` Parent's menu ID or None (to append to top-level Plugins Menu)
 :param int parentItem: integer index of existing parent menu item (ignored if parentMenu is None)
 :param callback: :py:func:`XPLMMenuHandler_f` callback you provide to handle interaction. May be None if you do not need callbacks.
 :param object menuRefCon: Reference constant to be returned to you inside your callback.                 

 This function creates a new menu and returns its ID.  It returns None if
 the menu cannot be created.

 For parentMenu, pass in:

 * An existing parent menu ID and an existing item index to create a submenu, or
 * ``None`` for the parent menu ID to append the menu in the top-level Plugins menu bar.

 The menu's name is only used if parentMenu is None, otherwise the provided
 name is ignored, and the item maintains the name set with :py:func:`XPLMAppendMenuItem`.

 You also pass a handler function and a menu reference value. Pass None for the
 handler if you do not need callbacks from the menu (for example, if it only
 contains submenus).

 On startup, each plugin as a (hidden) slot in the X-Plane Plugins menu.
 When you ``XPLMCreateMenu(..., parentMenu=None, ...)`` you append an item, *and* make that
 item a menu.  Nothing is visible until your first create (or append).

 * Creating a Menu causes an item with a right-arrow ('>') to be displayed,
   allowing you to attach items to *that* menu.

 * Creating with parentMenu=None *always* creates a new menu at the end
   of the X-Plane Plugins menu.

 * Creating with parentMenu=<otherMenu>, parentItem must be set, and must
   exist: You cannot simple set parentItem=1000 in the hope to attach it to the
   of the the parentMenu.

 * Creating with parentMenu=<otherMenu>, parentItem exists **changes**
   the existing item to make it become a (possible) parent menu. That is,
   it will remove any existing items from slot *parentItem* and change
   that slot by adding a right-arrow ('>'). In a similar fashion, Deleting the
   newly created menuID results in the removal of children and the right-arrow.
   The item will still exist (without the '>') on the parent. You
   would need to call XPLMRemoveMenuItem(parentMenu, <my slot>) to remove
   finally remove the (empty) menu.

 * To get rid of **all** your plugin's menus and menuitems, you can call
   :py:func:`XPLMClearAllMenuItems` and pass in :py:func:`XPLMFindPluginsMenu` for
   the menu.

 These are equivalent, as both will add "New Menu" to the end of the top-level Plugins Menu::

   menuID = xp.createMenu("New Menu", None, 0, callback, [])

 and::

   itemID = xp.appendMenuItem(xp.findPluginsMenu(), "New Menu", 0)
   menuID = xp.createMenu("<ignored>", xp.findPluginsMenu(), itemID, callback, [])

 .. note:: You must pass a valid, non-empty menu title even if the menu is
   a submenu where the title is not visible (any string will do, as it is ignored).


.. py:function:: XPLMDestroyMenu(menuID) -> None:

 :param menuID: :ref:`XPLMMenuID`  

 This function destroys a menu that you have created.  Use this to remove a
 submenu if necessary.  (Normally this function will not be necessary.)


.. py:function:: XPLMClearAllMenuItems(menuID) -> None:

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


.. py:function:: XPLMAppendMenuItemWithCommand(menuID, itemName, commandRef) -> int:

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

 This routine adds a separator to the end of a menu. (Note Laminar documentation, and
 Laminar provided header file indicates this returns void, but actually, it returns
 the integer index.)

 :param int menuID: :ref:`XPLMMenuId`
 :return: int index of of added item or negative number of append failed.


.. py:function:: XPLMSetMenuItemName(menuID, index, itemName) -> None:

 :param int menuID: :ref:`XPLMMenuId`
 :param int index: index of menu item to be changed
 :param str itemName: New menu item name

 This routine changes the name of an existing menu item.  Pass in the menu
 ID and the index of the menu item.


.. py:function:: XPLMCheckMenuItem(menuID, index, check) -> None:

 :param int menuID: :ref:`XPLMMenuId`
 :param int index: index of menu item to be changed
 :param int check: one of :ref:`XPLMMenuCheck`

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
 :param int enable: 1= enable this item

 Sets whether this menu item is enabled.  Items start out enabled.


.. py:function:: XPLMRemoveMenuItem(menuID, index) -> None:

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
  :value: 0

  there is no symbol to the left of the menu item.

 .. py:data:: xplm_Menu_Unchecked
  :value: 1

  the menu has a mark next to it that is unmarked (not lit).

 .. py:data:: xplm_Menu_Checked
  :value: 2

  the menu has a mark next to it that is checked (lit).
              
Example
-------

Putting it together, adding "My Menu" to the main plugin menu, and having two
item, "Reset" which calls existing reset command, and "About" which displays an
About Window (code not provided)::

    class PythonInterface:
      def XPluginStart(self):
         # By creating a menu with 'None' parent, it will appear as an on the plugins menu
         # as "My Menu >", but with nothing under the '>'
         self.menuID = XPLMCreateMenu("My Menu", None, 0, self.callback, 'main')
    
         # add my items to my menu.. that is, fill in the '>' of "My Menu"
         XPLMAppendMenuItemWithCommand(self.menuID, 'Reset', XPLMFindCommand('sim/reset'))
         XPLMAppendMenuItem(self.menuID, 'About', 'about')
         return 'PluginName', 'PluginSig', 'PluginDesc'
      
      def XPluginStop(self):
         if self.menuID:
             XPLMDestroyMenu(self.menuID)
             # Note: this removes the items from this menu, but it will
             # leave the initial named menu "My Menu" without the '>'
             # To completely remove this, you'd need to know which menuitem off of the
             # PluginsMenu is "My Menu" and then essentially do:
             #  XPLMRemoveMenuItem(XPLMFindPluginsMenu(), <this item>)
             # but, sadly, there's no way to know the menu item index is of your "My Menu"
    
      ...

      def callback(self, menuRefCon, itemRefCon):
         if menuRefCon == 'main' and itemRefCon == 'about':
             displayAboutWindow()
    
