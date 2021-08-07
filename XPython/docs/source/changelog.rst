Change Log
==========

Known Bugs
----------

 None. Tell me otherwise!

3.0.9 (7-Aug-2021)
------------------
:New Features:
   * **imgui**. Seriously. We now support a custom version of the pyimgui python module (included with installation)
     which permits plugin developers to use imgui to build user interfaces. Much simpler to use than
     Laminar widgets. See :doc:`development/imgui`. A demo plugin is included.

   * **"First Time" startup script**. On a successful (initial) installation, we:

       * Create the :code:`Resources/plugins/PythonPlugins` folder, if needed.

       * Download a set of sample plugins into :code:`PythonPlugins/samples` so
         you can see examples. (We've also included a new menu item **Download Samples**
         which allows you to re-download the latest set of samples.)

       * Pop-up an "Installation Successful" message

   * **OpenGL test plugin** :code:`Py_OpenGL.py` included (in samples).
     OpenGL installation, especially on a Mac, is problematic. Move :code:`PI_OpenGL.py`
     up one level from :code:`PythonPlugins/samples` to :code:`PythonPlugins` and restart X-Plane and we'll attempt to
     automatically load OpenGL, patch Big Sur (Macs only) and provide verbose error log to help diagnose failures.
     On success, you'll see a mesmerizing graphic.

     Note, this is to use OpenGL with python plugins (with or without imgui module). You
     can still run X-Plane using Vulkan / Metal.

:Improvements:

   * **Renamed log file** from XPPython3.log to XPPython3Log.txt. This will make it easier to upload the file
     to x-plane.org which prefers \*.txt files.

   * **Improved installation process and documentation**. Though there are versions of XPPython3 plugin for
     different minor versions of Python. The documentation is streamlined for the current 3.9 version of
     python. Also, see "First Time" startup under new features above.

   * Added **more detailed documentation** on use of window positioning code, most applicable to installations
     which include multiple-monitors.  See :doc:`development/window_position`.

:Fixes:

   * Changed built-in `sys.path` to initialize with absolute rather than relative paths. This
     allows us to import shared object libraries. (Required to get imgui to work.)

   * Fixed internal manipulation of reference constants associared with registering command handlers. We
     need a stable key so we can "find" the correct handler. Previous method of calculating the key was
     not stable for some reference constants.

3.0.8 (27-Dec-2020)
-------------------
:Fixes:

   * Fixed problem with :py:data:`XPStandardWidgets.xpMsg_ScrollBarSliderPositionChanged`. The incoming
     message should set param1 to the widget, instead it was setting param1 incorrectly.

3.0.7 (05-Nov-2020)
-------------------
:New Features:

   * Support for python ``multiprocessing`` module through the use of :py:data:`xp.pythonExecutable`.
     Documentation has been update to describe usage, with an example. See :doc:`development/multiprocessing`.

:Improvements:

   * Upgrade system now includes progress bar: when you update XPPython3 (after updating to 3.0.7) upgrades
     will display a popup window with download & verification status. You can use this this ProgressWindow
     in your own code by importing ``XPPython3.XPProgressWindow``. (Documentation to be provided.).
 
3.0.6 (24-Oct-2020)
-------------------
:New Features:

   * Support for Scenery plugins: place your plugins under ``Custom Scenery/<..>/plugins/PythonPlugins``
     and it will load on startup. See :doc:`development/plugins`. Scenery plugins are loaded
     based on order of X-Plane's ``scenery_packs.ini`` file: if the scenery isn't loaded, the scenery's plugin
     is not loaded.
 
3.0.5 (17-Oct-2020)
-------------------
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
    

3.0.4 (15-Oct-2020)
-------------------
:Fixes:

  * Script updater had incorrect logic

3.0.3 (15-Oct-2020)
-------------------
:New Features:
   
   * Support for Python 3.9
   * Support for Aircraft plugins: place your plugins under ``Aircraft/.../plugins/PythonPlugins``
     and it will load / unload as the user changes their aircraft. Additional documentation to follow.
   * Convenience functions:

     * :py:func:`xp.log` writes to XPPython3Log.txt, prepends your string with name of calling module.
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
   error message being sent to XPPython3Log.txt. This has been corrected.

3.0.2 (29-Sep-2020)
-------------------

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


3.0.1 (19-Sep-2020)
-------------------

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
