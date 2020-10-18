Change Log
==========

Known Bugs
----------

 None. Tell me otherwise!

In progress
-----------
   * Support for Scenery plugins: place your plugins under ``Custom Scenery/<..>/plugins/PythonPlugins``
     and it will load on startup. Additional documentation to follow.
 
3.0.5 (17-10-2020)
------------------
:New Features:
   
   * Detects the existence of python2 PythonInterface plugin and immediately exits XPPython3 with
     a Fatal Error written to Log.txt: These two plugins cannot run simultaneously: they almost can, but
     will fail in miserably difficult-to-debug ways. So we don't even try.
   * Initial documentaion for :doc:`development/xpyce`, which enables loading of encrypted pyc modules.

:Fixes:

  * Fixed problems with :py:func:`XPLMDataAccess.XPLMRegisterDataAccessor`. When accessing a data item which
    had not yet been defined, sim would crash. Incorrect logic caused accessing data arrays
    to return incorrect values. Now tested with `DataRefEditor plugin <http://www.xsquawkbox.net/xpsdk/mediawiki/DataRefEditor>`_.
    Documentation has been updated to better describe use of accessors.
    

3.0.4 (15-10-2020)
------------------
:Fixes:

  * Script updater had incorrect logic

3.0.3 (15-10-2020)
------------------
:New Features:
   
   * Support for Python 3.9
   * Support for Aircraft plugins: place your plugins under ``Aircraft/.../plugins/PythonPlugins``
     and it will load / unload as the user changes their aircraft. Additional documentation to follow.
   * Convenience functions:

     * :py:func:`xp.log` writes to XPPython3.log, prepends your string with name of calling module.
     * :py:func:`xp.sys_log` writes to Log.txt log, prepends your string with name of calling module.

:Improvements:

   * XPListBox (in demos) improved to automatically wrap long lines: this is used in PI_MiniPython plugin
   * Better error messages when trying to initialize xpyce -- we'll tell you if you're missing ``cryptography``
     package and you'll be instructed to use Pip Package Installer.

:Fixes:
   
 * On startup, log may include ``Couldn't find the callback list for widget ID <> for message 15``. This appears
   to be harmless. This was due to newly created CustomWidgets not passing the initial "Accept_Parent" message
   correctly.
 * :py:data:`XPWidgetDefs.xpMsg_MouseWheel` message incorrectly processed
   during :py:func:`XPWidgetUtils.XPUSelectIfNeeded`, which would result in an
   error message being sent to XPPython3.log. This has been corrected.

3.0.2 (29-9-2020)
-----------------

:New Features:

   * Loading by "packages". Global plugins are now loaded as part of either XPPython3 or PythonPlugins
     package. This allows the use of relative package imports by plugins. See :doc:`development/import`.
   * Improved documentation: added :doc:`changelog`, added values for enumeration / constants, fixed minor bugs
   * Initial support for xpyce: enabled loading of encrypted pyc modules. (Documentation :doc:`development/xpyce`)

:Fixes:

   * Fix for menu items: Each python plugin gets an independent set of menu items, mimicking
     what X-Plane does. This means one python plugin cannot remove another python plugin's menu
     items, and additions / deletions by one plugin do not change the number of items as seen
     by another python plugin.


3.0.1 (19-9-2020)
-----------------

:New Features:

   * PI_MiniPython plugin: allows interactive python within a running X-Plane process. See :doc:`development/debugger`
   * Built-in PIP Package Installer accessible from XPPython3 plugin menu.
   * Additional API support, mimicking Sandy's PythonInterface (python2) SDK. Function
     parameters which are no longer required as part of XPPython3 are now, "accepted" on the interface and
     ignored, rather than causing an exception: this should simplify porting of older python2 plugins.


3.0.0
-----
:New Features:

   * Support for Python 3.6, 3.7, and 3.8 on Mac, Windows 10 and Linux
   * Support for X-Plane SDK303.
   * Documentation online at https://xppython3.rtfd.io
