:orphan:

Change Log
==========

.. Note::

   We will be dropping support for Python 3.6, 3.7, and 3.8 once X-Plane 12 is released.
   Most users are already using Python 3.10. If you're not, you might consider upgrading.


Known Bugs
----------

 None (prove me wrong!)

3.1.5 (9-Sep-2022)
-------------------
:Note:
   * This release *does not support* python 3.6 which has reached end-of-life.
     Use XPPython3 v3.1.4 if that is required.

   * This release will be the *last* to support python 3.7 and 3.8. Most users have
     moved to 3.10 & I suggest you do as well.

   * This release is *compatible* with X-Plane 12. It does not support any X-Plane 12 specific
     features. (E.g., SDK 400+). As X-Plane 12 beta releases evolve we may see issues, please
     let me know.

     | 
     
:New Features:
   * **Universal Binaries for Mac** Python 3.10 and Python 3.9 versions of XPPython3 support
     x86 (Intel) and newer ARM (M1 Silicon) architectures using the same binary file.
     Other versions of python support only the older Intel processor.

     | 

:Fixes:
   * **Removed xpyce** cryptography feature. While the encrypting python works great, it is
     way to easy to obtain decrypted byte-code in a running process and decompile it back to
     reasonable source code. I've removed this feature as it might give a false sense of security.
     If encrypted python is important to you, use Cython and generate per-platform binary files.

   * **Fixed download bug** in ``zip_download.py``. This was not an issue with X-Plane 11, but
     X-Plane 12 is more senstive about access from non-main threads. The download routines (used
     to update XPPython3) used a child thread to write to the popup dialog box to indicated
     download status. This worked with X-Plane 11, but failed with X-Plane 12. An alternative
     implementation is provided which works for both.
     
     | 
     
:Improvements:
   * **Reload Plugins**, now even better for development. We record timestamps *of all python modules* on
     loading. When *Reload* is called, we first disable and stop all python plugins, do some internal
     cleanup of data-structures, then reload all python plugins, *and all changed python modules*. Finally,
     all python plugins are re-Enabled, and re-Started. Reloaded modules are enumerated in the log file.

   * **Updated Stub Files** with full type-hinting are available as a separate download. Their use
     is documented in :doc:`/development/stubs`.

   * **XPPython3 Preferences file** documented, useful for debugging. See :doc:`/development/xppython3.ini`.


3.1.4 (19-Feb-2022)
-------------------
:Fixes:
   * **print() regression**: Fixes problem created in v3.1.3 where python stdout and stderr where not
     redirected to XPPython3Log.txt. For 3.1.3, python stdout and stderr went to X-Plane stdout and stderr,
     which (for most people) is really inconvenient. Now ``print()`` output is sent to XPPython3Log.txt file,
     just as it used to prior to v3.1.3.  (Also fixes the use of standard python ``logging`` module, which normally
     writes to stderr: it will *also* now correctly write to XPPython3Log.txt.)

3.1.3 (2-Feb-2022)
------------------
:New Features:
   * **Reload Plugins** support. Added back the "Reload Scripts" which reloads updated python code and restarts
     all plugins.

   * **xppython3.ini** config file. Mostly to enable detailed plugin debugging, an initialization file
     allows for setting some run-time parameters for the main XPPython3 plugin. File is *optional*, and
     located in ``Output/preferences``.
     
:Improvements:
   * **Error reporting** now occurs primarily into the XPPython3Log.txt file. Previously, some python coding
     errors resulted in messages to stderr.
     
   * **Improved performance reporting** for python plugins: we're able to track and display a greater percentage
     of time spent by each python plugin, resulting in more accurate reporting.
     
:Fixes:
   * :py:func:`xp.setGraphicsState` keyword parameter name has been corrected. Was ``numberTextUnits``, now correctly
     spelled ``numberTexUnits``. If you use just positional parameters, you'll not see the error.

   * :py:func:`xp.createWidget` keyword parameter name has been corrected. Was ``class``, now ``widgetClass``. (*class* is
     a reserved word.)

   * Linking code change allows XPPython3 to run on older MacOS (minimum required: Mojave - 10.14). Mojave support
     is deprecated and is enabled now merely to aid transition. It will likely not be support with X-Plane 12.
     
3.1.2 (7-Dec-2021)
------------------
:Fixes:
   * Python updater script sometimes failed to successful download updated software.
     If your software is "stuck" and not updating, *delete* XPPython3 folder, and
     reinstall by downloading the lastest version. See :doc:`/usage/older_python`
     and get the version which matches your version of Python.

3.1.1 (7-Dec-2021)
------------------
:Fixes:
   * createWindowEx() ordering of keyword parameters was incorrect.

3.1.0 (4-Dec-2021)
------------------
:New Features:
   * **Support for Python 3.10**. Because my life just isn't complicated enough.

   * Added script to simplify Mac Installation (removing quarantine flag). See
     :doc:`/usage/mac_quarantine`.
     
   * Made interface more "pythonic":

     * Added keywords for all parameters (e.g., ``xp.getWindowGeometry(windowID=xxx)``)

     * Made many parameters optional (e.g., ``xp.createWindowEx(visible=1, draw=MyDraw)``)

     * Documented with doc strings, so when used with **Mini Python**:

       >>> help(xp.windowIsPoppedOut)
       windowIsPoppedOut(windowID)
           Returns 1 if window is popped-out, 0 otherwise

     Older interface still works, but will never support keywords or optional parameters:

       >>> import XPLMDisplay
       >>> windowID = XPLMDisiplay.XPLMCreateWindowEx(100, 200, 200, 100, 1,
       ...    None, None, None, None, None,
       ...    XPLMDisplay.xplm_WindowDecorationRoundRectangle,
       ...    XPLMDisplay.xplm_WindowLayerFloatingWindows, None))

     vs:

       >>> import xp
       >>> windowID = xp.createWindowEx(1, 200, 200, 100)

     | 


:Improvements:
   * Upgraded **Mini Python** plugin which supports resizable debugging window and *full language scripting*, see
     :doc:`/development/debugger`. (You'll need to re-download Samples and copy updated `PI_MiniPython.py` if
     you want to use it.)     

   * Hundreds of documentation improvements, providing *executable examples* of most SDK functions, and documenting
     the dozen or so interfaces where X-Plane does not actually implement the Laminar SDK documentation correctly.
     

3.0.11 (1-Oct-2021)
-------------------
:New Features:

   * **Per-plugin performance window**. Amount of time spent, per-plugin, within flight loop or drawing callbacks is recorded
     and displayed. Now you can see *which* plugin is slowing your system down, or tune your own plugin to run faster. See :doc:`usage/performance`.


     | 
:Improvements:
   * **Delete former XPPython3.log** on startup, if it exists -- this to avoid confusion as to which file is the log file.
     XPPython3 now logs to XPPython3Log**.txt**, as this allows the file to be seen properly as a text file.

   * **Improved error handling**: If your python code misbehaves, we're better at catching the error,
     printing out where the problem is, and avoiding crashing the whole sim.

   * Methods XPluginReceiveMessage(), XPluginDisable(), and XPluginStop() **are now optional** in
     implemented plugins. Previously, these methods were required, even if all they did
     was ``pass``. It's always good practice to have these methods defined, but
     no harm if they're missing.

   * **Platform-sensitive updater** now loads only the XPPython3 version (mac, windows, linux) you need rather
     than downloading all versions resulting in a much faster upgrade.


     | 
:Fixes:

   * Changed order of python sys.path updates: We now INSERT (rather than APPEND) X-Plane related paths. The result
     is you'll have Airplane and Scenery plugin paths first, followed by PythonPlugins directory, followed by XPPython3
     directory, followed by the original python path. (No known error related to this, but it seems 'proper'
     to set path like this.

3.0.10 (9-Aug-2021)
-------------------
:Fixes:

   * Race condition caused XPPython3 and X-Plane to crash immediately on startup. Only occurred on some
     configurations (Known to occur on some Linux and Window 11).


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


     | 
:Improvements:

   * **Renamed log file** from XPPython3.log to XPPython3Log.txt. This will make it easier to upload the file
     to x-plane.org which prefers \*.txt files.

   * **Improved installation process and documentation**. Though there are versions of XPPython3 plugin for
     different minor versions of Python. The documentation is streamlined for the current 3.9 version of
     python. Also, see "First Time" startup under new features above.

   * Added **more detailed documentation** on use of window positioning code, most applicable to installations
     which include multiple-monitors.  See :doc:`development/window_position`.


     | 
:Fixes:

   * Changed built-in `sys.path` to initialize with absolute rather than relative paths. This
     allows us to import shared object libraries. (Required to get imgui to work.)

   * Fixed internal manipulation of reference constants associated with registering command handlers. We
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


     | 
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
   * Initial documentation for xpyce, which enables loading of encrypted pyc modules. [This feature has
     been removed since 3.1.5.]

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


   | 
:Improvements:

   * XPListBox (in demos) improved to automatically wrap long lines: this is used in PI_MiniPython plugin
   * Better error messages when trying to initialize xpyce -- we'll tell you if you're missing ``cryptography``
     package and you'll be instructed to use Pip Package Installer.


   | 
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
   * Initial support for xpyce: enabled loading of encrypted pyc modules. (Documentation xpyce [This feature has been removed
     since 3.1.5])


   | 
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
