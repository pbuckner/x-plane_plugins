XPLMMenus
=========

.. py:module:: XPLMMenus
.. py:currentmodule:: xp               

To use::

  import xp

Plugins can create menus in the menu bar of X-Plane.  This is done by
creating a menu and then creating items.  Menus are referred to by an
opaque menuID.  Items are referred to by (zero-based) index number.

Menus are "sandboxed" between plugins---no plugin can access the menus of
any other plugin. Furthermore, all menu indices are relative to your
plugin's menus only; if your plugin creates two sub-menus in the Plugins
menu at different times, it doesn't matter how many other plugins also
create sub-menus of Plugins in the intervening time: your sub-menus will be
given menu indices 0 and 1. (The SDK does some work in the back-end to
filter out menus that are irrelevant to your plugin in order to deliver
this consistency for each plugin.)

When you create a menu item, you specify how we should handle clicks on
that menu item. You can either have X-Plane trigger a *handler* callback, or you
can simply have a command be triggered (with no associated call to your
menu handler). The advantage of the latter method is that X-Plane will
display any keyboard shortcuts associated with the command. (In contrast,
there are no keyboard shortcuts associated with menu handler callbacks with
specific parameters.)

For more about menus and menu items see :doc:`/development/menus`.

Functions
---------

.. py:function:: findPluginsMenu()

 This function returns the *menuID* of the plug-ins menu, which is created for you
 at startup.

 `Official SDK <https://developer.x-plane.com/sdk/XPLMMenus/#XPLMFindPluginsMenu>`__ :index:`XPLMFindPluginsMenu`

 >>> menuID = xp.findPluginsMenu()
 >>> menuID
 <capsule object "XPLMMenuIDRef" at 0x7fdd40a74810>

.. py:function::  findAircraftMenu()

 This function returns the *menuID* of the menu for the currently-loaded aircraft,
 used for showing aircraft-specific commands.

 The aircraft menu is created by X-Plane at startup, but it remains hidden
 until it is populated via :py:func:`appendMenuItem` or
 :py:func:`appendMenuItemWithCommand`.

 Only plugins loaded with the user's current aircraft are allowed to access
 the aircraft menu. For all other plugins, this will return None, and any
 attempts to add menu items to it will fail.

 .. note:: This does not do anything with XPPython3. There is no pre-allocated, dedicated
           menu for aircraft python plugins. If you write an aircraft python plugin, it
           will act just like any other plugin with regard to menus.
           
 >>> xp.findAircraftMenu()
 None

 `Official SDK <https://developer.x-plane.com/sdk/XPLMMenus/#XPLMFindAircraftMenu>`__ :index:`XPLMFindAircraftMenu`

.. py:function:: createMenu(name=None, parentMenuID=None, parentItem=0, handler=None, refCon=None)

 Create a new menu and return its menuID. Returns None if the menu cannot be created.
 
 For *parentMenuID*, pass in:

 * An existing parent menuID and an existing *parentItem* item index to create a submenu, or
 * ``None`` to append the new menu in the top-level Plugins menu bar. (*parentItem* will be ignored.)

 The *name* is used only if *parentMenuID* is None, otherwise the provided
 name is ignored, and the item maintains the name set with :py:func:`appendMenuItem`.

 If *parentMenuID* is None and *name* is not provided, we'll default to the current
 plugin's module name (this makes for easier testing).

 >>> menuID = xp.createMenu()
 >>> menuID
 <capsule object "XPLMMenuIDRef" at 0x7fedf68fa030>
 
 .. image:: /images/menu-module.png


 You also pass a *handler* function and a menu reference value. Pass ``None`` for the
 handler if you do not need callbacks from the menu (for example, if it only
 contains submenus).

 *parentItem* is 0-based integer index, ignored if *parentMenuID* is None.

 *handler* callback takes two parameters (menuRefCon, itemRefCon), the reference
 constant provided with :py:func:`createMenu` and the reference constant
 provided with :py:func:`appendMenuItem`. Note these are reference
 constants, **not** menuIDs.

 >>> def MyMenu(menuRefCon, itemRefCon):
 ...    xp.speakString(f"Menu {menuRefCon} selected")
 ...
 >>> menuID = xp.createMenu(handler=MyMenu, refCon="Menu1")
 >>> xp.appendMenuItem(menuID, "Item 1", refCon="Item1")
 0
 
 On startup, each plugin as a (hidden) slot in the X-Plane Plugins menu.
 When you ``createMenu(..., parentMenuID=None, ...)`` you append an item, *and* make that
 item a menu.  Nothing is visible until your first create (or append).

 * Creating a Menu causes an item with a right-arrow ('>') to be displayed,
   allowing you to attach items to *that* menu.

 * A Menu, by itself, *is not selectable*: you'll need to add a menu item to it before
   your menu *handler* will be called. (:py:func:`appendMenuItem`)

 * Creating with parentMenuID=None *always* creates a new menu at the end
   of the X-Plane Plugins menu.

 * Creating with parentMenuID=<otherMenu>, *parentItem* must be set, and must
   exist: You cannot simple set parentItem=1000 in the hope to attach it to the
   end of the parentMenu.

 * Creating with parentMenuID=<otherMenu>, where provided *parentItem* already
   exists **changes**
   the existing item to make it become a (possibly) parent menu. That is,
   it will remove any existing items from slot *parentItem* and change
   that slot by adding a right-arrow ('>'). In a similar fashion, Deleting the
   newly created menuID results in the removal of children and the right-arrow.
   The item will still exist (without the '>') on the parent. You
   would need to call ``removeMenuItem(parentMenuID, <my slot>)`` to remove
   finally remove the (empty) menu.

 * To get rid of **all** your plugin's menus and menuitems, you can call
   :py:func:`clearAllMenuItems` and pass in :py:func:`findPluginsMenu` for
   the menu. (This will clear only your python plugin's menu items -- it will
   not remove all menus of all python plugins.)

 These are equivalent, as both will add "New Menu" to the end of the top-level Plugins Menu::

   menuID = xp.createMenu("New Menu", parentMenuID=None, parentItem=0, handler=callback)

 and::

   itemID = xp.appendMenuItem(xp.findPluginsMenu(), "New Menu", 0)
   menuID = xp.createMenu(parentMenuID=xp.findPluginsMenu(), parentItem=itemID, handler=callback)

 `Official SDK <https://developer.x-plane.com/sdk/XPLMMenus/#XPLMCreateMenu>`__ :index:`XPLMCreateMenu`

.. py:function:: destroyMenu(menuID)

 Destroy a menu.  Use this to remove a
 submenu if necessary.  (Normally this function will not be necessary.)

 The items below this menu are removed, and the provided menuID remains, but
 will no longer have a right-arrow ('>') displayed. It will not be selectable,
 nor will you be able to :py:func:`appendMenuItem` to it.

 `Official SDK <https://developer.x-plane.com/sdk/XPLMMenus/#XPLMDestroyMenu>`__ :index:`XPLMDestroyMenu`

.. py:function:: clearAllMenuItems(menuID=None)

 Remove all menu items from a menu, allowing you to rebuild
 it.  Use this function if you need to change the number of items on a menu.

 If *menuID* is None, (or is the value from :py:func:`findPluginsMenu`), we'll
 remove all menu items added by this plugin.

 `Official SDK <https://developer.x-plane.com/sdk/XPLMMenus/#XPLMClearAllMenuItems>`__ :index:`XPLMClearAllMenuItems`           

 .. note:: Be careful when using this command from within the Python Debugger
           as ``clearAllMenuItems()`` will remove *the Debugger's menu*. Hopefully
           you attached a keyboard shortcut to toggle the display...

           Or, assuming the window is still being displayed, add it back:

           >>> self = getPluginInstance()
           >>> self.menuIdx = xp.appendMenuItemWithCommand(menuID=None,
           ...                                             name='Mini Python',
           ...                                             commandRef=self.toggleCommandRef)
           ...

.. py:function::  appendMenuItem(menuID=None, name="Item", refCon=None)

 Append a new menu item with *name* to the end of existing menu *menuID*.
 Return the new item's index. *refCon* will be passed to menu's ``handler()``
 callback as *itemRefCon*.

 If *menuID* is None, we'll append it to the main plugin menu (identical to
 ``menuID = xp.findPluginsMenu()``).

 Returns a negative index if the append failed (due to an invalid parent
 menu argument).

 Note that all menu indices returned are relative to your plugin's menus
 only; if your plugin creates two sub-menus in the Plugins menu at different
 times, it doesn't matter how many other plugins also create sub-menus of
 Plugins in the intervening time: your sub-menus will be given menu indices
 0 and 1. (The SDK does some work in the back-end to filter out menus that
 are irrelevant to your plugin in order to deliver this consistency for each
 plugin.)

 >>> menuID = xp.createMenu()
 >>> xp.appendMenuItem(menuID, "Item 1")
 0
 >>> xp.appendMenuItem(menuID, "Item 2")
 1

 .. image:: /images/menu-items.png

 `Official SDK <https://developer.x-plane.com/sdk/XPLMMenus/#XPLMAppendMenuItem>`__ :index:`XPLMAppendMenuItem`

 .. Note:: X-Plane's C SDK includes a fourth parameter for this function
    which is required, yet deprecated and ignored. XPPython3 function only
    has three parameters.

.. py:function:: appendMenuItemWithCommand(menuID=None, name="Command", commandRef)

 Like :py:func:`appendMenuItem`, but instead of the new menu item triggering the
 ``handler()`` callback of the containing menu, it will simply execute the
 command you pass in. Using a command for your menu item allows the user to
 bind a keyboard shortcut to the command and see that shortcut represented
 in the menu.

 *commandRef* is reference obtained from either :py:func:`findCommand` or
 :py:func:`createCommand`.

 Returns a negative index if the append failed (due to an invalid parent
 menu argument).

 Like :py:func:`appendMenuItem`, all menu indices are relative to your plugin's
 menus only.

 >>> commandRef = xp.findCommand("sim/operation/show_fps")
 >>> xp.appendMenuItemWithCommand(name="Toggle FPS", commandRef=commandRef)
 2

 `Official SDK <https://developer.x-plane.com/sdk/XPLMMenus/#XPLMAppendMenuItemWithCommand>`__ :index:`XPLMAppendMenuItemWithCommand`

.. py:function:: appendMenuSeparator(menuID=None)

 This routine adds a separator to the end of a menu. SDK303 this returns an integer
 menu index. SDK400 does not return a value (and yet, the separator indeed "consumes"
 a menu index.)

 Append to main Plugins Menu if *menuID* is None.

 >>> xp.appendMenuSeparator()

 `Official SDK <https://developer.x-plane.com/sdk/XPLMMenus/#XPLMAppendMenuSeparator>`__ :index:`XPLMAppendMenuSeparator`

.. py:function:: setMenuItemName(menuID=None, index=0, name="New Name")

 Change the *name* of an existing menu item given by *menuID* and item *index*.

 Sets *index* on main Plugin Menu if *menuID* is None.

 `Official SDK <https://developer.x-plane.com/sdk/XPLMMenus/#XPLMSetMenuItemName>`__ :index:`XPLMSetMenuItemName`
 
.. py:function:: checkMenuItem(menuID=None, index=0, checked=Menu_Checked)

 Set whether a menu item is checked.  Pass in the *menuID* and item *index*,
 and value for *checked*, one of

 .. py:data:: Menu_NoCheck
  :value: 0

  there is no symbol to the left of the menu item.

 .. py:data:: Menu_Unchecked
  :value: 1

  the menu has a mark next to it that is unmarked (not lit).

 .. py:data:: Menu_Checked
  :value: 2

  the menu has a mark next to it that is checked (lit).

 .. note:: In X-Plane 11, "Unchecked" and "NoCheck" are visually identical. The
           state is maintained programmatically, so calls to :py:func:`checkMenuItemState`
           will return the set value, but the user won't see a difference.

 `Official SDK <https://developer.x-plane.com/sdk/XPLMMenus/#XPLMCheckMenuItem>`__ :index:`XPLMCheckMenuItem`

.. py:function:: checkMenuItemState(menuID, index)

 Returns whether a menu item is checked or not. A menu item's
 check mark may be on (``xp.Menu_Checked``) or off (``xp.Menu_Unchecked``),
 or a menu may not have an icon at all (``xp.Menu_NoCheck``).

 >>> xp.checkMenuItem(menuID, index=2, checked=2)
 >>> xp.checkMenuItemState(menuID, index=2)
 2
 >>> xp.checkMenuItem(menuID, index=2, checked=0)
 >>> xp.checkMenuItemState(menuID, index=2)
 0
 
 `Official SDK <https://developer.x-plane.com/sdk/XPLMMenus/#XPLMCheckMenuItemState>`__ :index:`XPLMCheckMenuItemState`

.. py:function:: enableMenuItem(menuID, index, enabled=1)

 Sets whether this menu item is enabled.  Items start out enabled. Set
 *enabled*\=0 to disable.

 >>> xp.enableMenuItem(index=0, enabled=0)  # disables
 >>> xp.enableMenuItem(index=0)  # enables
 
 `Official SDK <https://developer.x-plane.com/sdk/XPLMMenus/#XPLMEnableMenuItem>`__ :index:`XPLMEnableMenuItem`

.. py:function:: removeMenuItem(menuID, index)

 Removes one item with *index* from a menu *menuID*.
 Note that all menu items below are moved up
 one; **your plugin must track the change in index numbers**.

 `Official SDK <https://developer.x-plane.com/sdk/XPLMMenus/#XPLMRemoveMenuItem>`__ :index:`XPLMRemoveMenuItem`

Example
-------

Putting it together, adding "My Menu" to the main plugin menu, and having two
item, "Reset" which calls existing reset command, and "About" which displays an
About Window (code not provided)::

    class PythonInterface:
      def XPluginStart(self):
         # By creating a menu with 'None' parent, it will appear as an on the plugins menu
         # as "My Menu >", but with nothing under the '>'
         self.menuID = xp.createMenu("My Menu", handler=self.callback, refCon='main')
    
         # add my items to my menu.. that is, fill in the '>' of "My Menu"
         xp.appendMenuItemWithCommand(self.menuID, 'Reset', xp.findCommand('sim/reset'))
         xp.appendMenuItem(self.menuID, 'About', 'about')
         return 'PluginName', 'PluginSig', 'PluginDesc'
      
      def XPluginStop(self):
         if self.menuID:
             xp.destroyMenu(self.menuID)
             # Note: this removes the items from this menu, but it will
             # leave the initial named menu "My Menu" without the '>'
             # To completely remove this, you'd need to know which menuitem off of the
             # PluginsMenu is "My Menu" and then essentially do:
             #  xp.removeMenuItem(xp.findPluginsMenu(), <this item>)
             # but, sadly, there's no way to know the menu item index is of your "My Menu"
    
      ...

      def callback(self, menuRefCon, itemRefCon):
         if menuRefCon == 'main' and itemRefCon == 'about':
             displayAboutWindow()
    
For more about menus and menu items see :doc:`/development/menus`.
