Change Log
==========

Known Bugs
----------

 * On startup, log may includes ``Couldn't find the callback list for widget ID <> for message 15``. This appears
   to be harmless.
 
Planned for 3.0.3
-----------------
   * Support for Aircraft plugins: place your plugins under ``Aircraft/.../plugins/PythonPlugins``
     and it will load / unload as the user changes their aircraft. Additional documentation to follow.
   * Support for Scenery plugins: place your plugins under ``Custom Scenery/<..>/plugins/PythonPlugins``
     and it will load on startup. Additional documentation to follow.


3.0.2 (in progress)
-------------------

:New Features:

   * Loading by "packages". Global plugins are now loaded as part of either XPPython3 or PythonPlugins
     package. This allows the use of relative package imports by plugins. See :doc:`development/import`.
   * Improved documentation: added :doc:`changelog`, added values for enumeration / constants, fixed minor bugs
   * Initial support for xpyce: enabled loading of encrypted pyc modules. (Documentation to be updated)

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
