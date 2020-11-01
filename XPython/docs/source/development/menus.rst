More about Menus
================

Create or Append? Menu vs. MenuItem?

X-Plane does some manipulation behind the scenes to make menu handling easy,
but as with most magic, you might get confused if you try to figure out what
is actually happening. Add to that the fact that, technically, *all* Python plugins
appear to X-Plane as a single plugin (called XPPython3) additional
manipulation is required in order to present to you, the python plugin developer, a consistent workable interface.

Plugins Menu
------------

.. image:: /images/menu1.png
   :align: right

On start-up the "Plugins" menu contains a single item, "Show Plugin Admin". This is added by Laminar and will display
all loaded plugins. Note that because *all* python plugins are executed by XPPython3, only XPPython3 is listed.
FPS impact for XPPython3 is the sum impact of all python plugins (XPPython3 by itself adds negligable overhead).


Top-Level Menu
--------------

Plugins can append to this "Plugins" Menu, under the "Show Plugin Admin" item.
The order of added items is simply determined by the order of
calls -- by any plugin -- to any of the following::
  
  XPLMCreateMenu(..., parentMenu=None, …)
  XPLMAppendMenuItem(menuID=xp.findPluginsMenu())
  XPLMAppendMenuItemWithCommand(menuID=xp.findPluginsMenu())
  XPLMAppendSeparator(menuID=xp.findPluginsMenu())

We'll call these "top-level" menu items -- items directly placed under
the "Plugins" menu.

If plugins intermix calls, the items will be intermixed: there is
no way to guarantee all items by the same plugin are next
to each other. There is no way to change the order of items.

For this reason, it is recommended that, if order is important,
have a single top-level item for your plugin, and place your
other menu items as sub-items. (That is, as items on a second-level menu.)

.. image:: /images/menu2.png
           :align: right

In the graphic to the right, we cannot tell if:

a. One plugin created 5 items and one separator; or
b. One plugin created Item 1 and item 2 and another plugin created
   Item3, Item 4, the separator and Item 5; or
c. One plugin created Item 1, a second plugin created Item 2,
   the first plugin then created Item 3, a third plugin
   created Item 4, then the first plugin added
   the separator and Item 5.

Do you care? 

.. image:: /images/menu3.png
           :align: right

You might, if you created multiple top-level items, and don't make
it clear to the user. For example, assume you have two plugins and
one creates three items: "About", "Reset", separator, and "Quit"
and the second plugin creates "About" and "Quit". You might end
up with something like the image on the right.

Don't do that.

Menu Item Index
---------------

Internally, as your plugin added items, there is an index associated
with each. The index is relative to the items your plugin
has added not relative to the full set of items on the top-level.

For example, if your plugin created three items and a separator,
you'll know that you have items with
indices [0, 1, 2, 3]. A second plugin creating two items
will have indices [0, 1].

Note that only :py:func:`xp.appendMenuItem` and :py:func:`xp.appendMenuItemWithCommand` return
the newly created index number. Calling :py:func:`xp.createMenu` with ``parentMenu=None``
will add an item, but you'll have to "know" which index it is, as the function returns a
:ref:`XPLMMenuID`, not an index.
Similarly, calling :py:func:`xp.appendSeparator` with ``menuID=None`` will increment the
index. Laminar documentation (and header file) indicates
XPLMAppendSeparator() does not return anything, but actually
it does… and so do we.

You'll need the index value if you want to delete
the item. For example, if the separator is your
plugin's third addition (that is, index #2), then
to remove the separator, call
``XPLMRemoveMenuItem(menuID=findPluginsMenu(), index=2)``.

One more thing about menu item index: When you remove a menu item using :py:func:`xp.removeMenuItem`,
each of the other menu items for the same menu has its menu index updated to remove any numbering gap.
If the menu item you're removing is the last item (numerically), then all the other indices will remain the same. If
you're removing another menu item, the subsequent items will have their index reduced by one. For example, if you were to call
``xp.removeMenuItem(..., index=2)``:

  ===== ========= ===   ===== =========
  Index Name            Index Name
  ===== ========= ===   ===== =========
  [0]   About           [0]   About
  [1]   Config          [1]   Config
  [2]   Update          [2]   \--------
  [3]   \--------       [3]   Quit
  [4]   Quit
  ===== ========= ===   ===== =========

You don't get a notification for this, nor is there any way to query for this information. You just have to "know".
Fortunately, removing a single menu item is an uncommon activity.


.. image:: /images/menu4.png
           :align: right

Menus vs. MenuItems
-------------------
Menus are items which have (or at least, permit) sub-items.
When created (with createMenu()) they will have a
right-arrow ('>') displayed. This will be displayed even if
there are no sub-items. In the graphic to the right, we
know all of the items were added with
XPLMAppendMenuItem (or XPLMAppendMenuItemWithCommand) except `Item 2` which
was added using XPLMCreateMenu.

When you create a Menu (i.e., something with '>'),
you can create it on the top-level menu
by specifying ``None`` for the parentMenu (*not findPluginsMenu()*).
You'll get a :ref:`XPLMMenuID` as a return, and recall that internally
an index is incremented.

.. image:: /images/menu5.png
           :align: right

The benefit of the XPLMMenuID is you'll need it to place menu
items as part of that item's sub-menu.

Add a regular item as a sub-item using
AppendMenuItem (or AppendMenuItemWithCommand) with the parent XPLMMenuID

.. code::

  menuId = xp.createMenu('Item 2', parentMenu=None, 
                          parentItem=0,  # value ignored when parentMenu is None
                          callback=handler, refcon=[]
  xp.appendMenuItem(menuId, "Sub-1", [])
  xp.appendMenuItem(menuId, "Sub-2", [])

Converting MenuItems to Menus
-----------------------------
We know you can add a menu to the bottom of the main Plugins menu by specifying None as the parent::

  xp.createMenu('My Menu', None, 0, self.callback, self.menuRefCon)

Since it's the plugin menu, can you specify :py:func:`xp.findPluginsMenu`, aren't they the same?::

  xp.createMenu('My Menu', xp.findPluginsMenu(), 0, self.callback, self.menuRefCon)

No.

If you specify a parent menu, then `the menu and item must already exist`. So in the former example,
parentMenu is specified as None, so a new item is appended to the Plugins Menu (recall the ``parentItem=0`` is ignored).
In the latter example, the parentMenu is specified as whatever ``xp.findPluginsMenu()`` evaluates to, and the parentItem
`is used` to determine which item to change.

And that's the key: If you already have a menu item specified, you can convert it to a menu by using createMenu with the
correct parent and correct menu item index.

Menus have Callbacks
--------------------

* Menus have callbacks, which you provide when you call :py:func:`xp.createMenu`.
* Menuitems (via appendMenuItem) do not.

  If they're part of a menu, ``appendMenuItem(parentMenu=xxx)``, then the
  parent menu's callback will be executed. If they're simply appended to the main menu (``parentMenu=xp.findPluginsMenu()``)
  they'll not get a callback.
* Menuitems (via appendMenuItemWithCommand) don't have callbacks, but they will immediately execute a pre-defined command, so
  they don't need callbacks.

Since you can append a menu item to the main plugin menu without a callback, why would you? Two reasons:

1. Recall that, given a menu item, you can convert it to a menu::

     idx = xp.appendMenuItem(xp.findPluginsMenu(), 'My Menu', self.itemRefCon)
     self.menuID = xp.createMenu('<ignored>', xp.findPluginsMenu(), idx, self.callback, self.menuRefCon)

2. Alternatively, you many not need / want a callback. You can simple use your menu item as read-only status::

     idx = xp.appendMenuItem(xp.findPluginsMenu(), 'My Item', self.itemRefCon)
     xp.setMenuItemName(xp.findPluginsMenu(), idx, "Status: Phase 1")
     ...
     xp.setMenuItemName(xp.findPluginsMenu(), idx, "Status: Phase 2")


Menu Cleanup
------------
As with most resources, things you create should also be destroyed.

Because plugins can be reloaded in a running system, if you do not clean up you menus on disable / stop, they'll
be re-created on the subsequent start / enable, resulting in two sets of menus. Most likely the initial menus will
no longer work, but they'll confuse the user.

To clean up a menu, all you need to do is call :py:func:`XPLMMenus.XPLMClearAllMenuItems` for each of your menus (each :ref:`XPLMMenuID`, not
each index.) Remember to *also* call using the top-level menuID to remove your item(s) from the top-level plugin menu::

  for menuID in self.menuIDs:
     xp.clearAllMenuItems(menuID)
  xp.clearAllMenuItems(xp.findPluginsMenu())

Just remember:

* Things you create in XPluginStart, destroy in XPluginStop.
* Things you create in XPluginEnable (or after enable), destory in XPluginDisable  

