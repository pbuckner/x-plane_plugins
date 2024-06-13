Change Log
==========

.. Note::

   With X-Plane 12, the new SDK is likely to be updated a few more times. This
   will require updates to XPPython3 which I hope to keep to a minimum.


Known Bugs
----------

  None (prove me wrong!)
  
4.4.0 (BETA)
------------
:New Features:
   * **SDK 410 Support**. With X-Plane 12.1 new SDK features were added to support
     more flexible Avionics Display (:doc:`/development/modules/display`) and
     FMS flight plan manipulation (:doc:`/development/modules/navigation`).
     XPPython v4.4 can be used with X-Plane 12.0 or greater, though SDK410 functions
     are available only while using 12.1 or greater.

   * **OpenGL Utilities**. The :py:mod:`xpgl` module has been added to XPPython3.
     This module wraps OpenGL routines, allowing you to easily
     draw :doc:`Lines </development/modules/xpgl_lines>`,
     :doc:`Shapes </development/modules/xpgl_shapes>`,
     :doc:`Images </development/modules/xpgl_images>`,
     :doc:`Text </development/modules/xpgl_text>`,
     :doc:`Transformations </development/modules/xpgl_transformations>`,
     :doc:`Masks </development/modules/xpgl_mask>`,
     without needing to understand OpenGL. Also included
     is a :doc:`pygame Template </development/modules/xpgl_debugging>` which
     allows you to experiment with XPPython XPGL drawing without needing
     to fire up X-Plane.
     
   * **EasyPython**. To help developers who want to transition from using xlua
     for aircraft plugins, and to support a simpler way to write python plugins,
     some helper classes have been introduced. These simplify:

     * :doc:`/development/modules/datarefs`
     * :doc:`/development/modules/commands`
     * :doc:`/development/modules/timers`

     These mimic the interface provided by xlua, and are fully usable in *any* XPPython3 plugin.
     Additionally, a simpler "main loop" is introduced as :doc:`/development/modules/easy_python`.
     These enhancement should be considered *BETA* at present, and improved documentation with
     examples will be included in the future.

   * **xp_pip module**. This module allows plugin developers to automatically install
     python modules into the correct location withing XPPython3. See :doc:`/development/modules/xp_pip`.

   |

:Improvements:
   * **Aircraft Plugin handling**. Previously, we loaded python aircraft plugins *after*
     sending the ``XPLM_MSG_PLANE_LOADED`` message to all (other) python plugins, which
     results in the aircraft plugin *itself* not receive this message. Now we load
     the aircraft python plugin(s), and then send the ``PLANE_LOADED`` message to *all*
     python plugins.

     
4.3.0 (5-Apr-2024)
------------------
:New Features:
   * **Python Included**. For all architectures, we now include a version of Python 3.12. *You no
     longer need to install your own python, or alter your OS-provided version.*

   * **OpenGL, IMGUI, and requests modules Included**. By including our own copy of Python, we can also provide
     standard copies of OpenGL, IMGUI, and requests modules. For users, this means you
     no longer have to install these yourself. For developers, this means *all your users
     will have this installed already!* 

   |
   
:Improvements:
   * **IMGUI / Wayland** interaction fixed. Some Linux distros (Ubuntu 22.04, for example) use
     Wayland graphics interface which (currently) fails with IMGUI. Attempts to use IMGUI within
     X-Plane result in blank windows. I've embedded a fix which instructs linux to use X.org rather
     than Wayland interface and this appears to work. If your UI doesn't use Wayland, you shouldn't
     see anything.
     
   |

:Fixes:
   * **XPLMCreateWindow_Ex** was missing a sentinel on keyword argument list causing calls to sometimes
     fail.
     
4.2.1 (29-Dec-2023)
-------------------
:Fixes:
   * **Removed extraneous newlines**. Python ``logging`` module, and stdout ``print()`` resulted
     in extra newlines in the log file. Bug introduced in v4.2, now corrected.
   

4.2 (24-Dec-2023)
-------------------
:New Features:
   * **Type Hinting and \*.pyi files**. Doc strings and type hinting for all SDK functions are generated
     from code. We've removed the "stub" files which were previously shipped and have now included this
     information in (standard) ``*.pyi`` files, included with the standard XPPython3 distribution. See
     updated use in :doc:`/development/stubs`.
     
     |

:Fixes:
   * **Corrected** :py:func:`xp.getWeatherAtLocation` **error detection**. See linked documentation for details.
     Previously XPPython3 interpreted a zero return from XPLMGetWeatherAtLocation() as an error
     indicator. Apparently this is not the case, so we now detect error returns differently. We still
     return ``WeatherInfo`` object on success and ``None`` on failure: the good news is times when
     we *thought* there were failures we'll now indicate as success. Your code or usage shouldn't change.
     (`XPD-14674 <https://developer.x-plane.com/x-plane-bug-database/?issue=XPD-14674>`_)
     
   * **createMapLayer alternate API fixed**. This function can take either a list of parameters, or a
     single parameter, itself being a tuple of values. The former API could fail to create a map layer
     under particular circumstances. This has been fixed. (:py:func:`xp.createMapLayer`)

     |

:Improvements:
   * **FirstTime loader** is now called on first installation of XPPython3 *and when you upgrade* XPPython3.
     This allows us to clean out files from XPPython3 folder on upgrade.

   * **Improved Pip Package Installer**. When using the built-it Pip Package Installer, the output from Pip is
     now displayed in real-time in a scrolling window. Contents of the window (i.e., pip output) are also
     automatically added to the python log. (Also, pressing the Enter key on the pip package installer window
     will start the installation -- no need to click the "Install" button.)
     
   * **Cleanup up python code** under XPPython3 folder. Mostly to better support type-hinting.

   * **Add "reset" code for reload**. XPPython3 maintains a lot of internal dictionaries to handle callbacks
     and other functionality. If you forget to destroy windows or widgets, or unregister callbacks *and* you
     attempt to reload your python plugin (perhaps you're debugging your plugin and it failed to properly
     execute), then X-Plane may execute your callbacks more than once: once for the initial registration &
     again with your reloaded registration. We now reset all menus, callbacks, windows, hotkeys, keysniffers and other
     resources so your reloaded plugin can execute without left-over callbacks. (This is a non-issue if
     you don't *reload* your python plugins.)
     
   * **Enable XPLMSetErrorCallback** for beta releases or if you've enabled ``debug``
     in :doc:`/development/xppython3.ini`. This performs additional run-time error checking, with output
     sent to python log.

   * **Improved conversion of widget message parameters**. Some *custom* widget messages were not being
     converted correctly between C and Python implementations.

4.1.1 (12-Oct-2023)
-------------------

:New Features:
   * **Python 3.12 Support**. XPPython3 now supports python versions 3.10, 3.11, and 3.12. You may
     use any version. Python version 3.12 is internally faster than earlier versions of python but
     I have no tests to confirm any advantage within the context of X-Plane. For new installations, use
     3.12. There is no need to upgrade your current installation.

   * **Python IMGUI Update**. Current versions of ImGui (version 2.0+) now correctly handle
     contexts in a way compatible with X-Plane. Therefore, XPPython3 *no longer includes* a copy
     of the python imgui module.

     .. note::
        If you are *upgrading* from a previous version of XPPython3, you *should* delete the XPPython3/imgui
        directory, as it is no longer required. However, you (and users of your plugin) *will* need to install
        the standard python :code:`imgui` module::

          $ rm -rf XPPython3/imgui
          $ pip3 install imgui
        
        Caution: You *do* still need the :code:`xp_imgui` module found under XPPython3. This is required to integrate
        python imgui and X-Plane!

   * **Logging Flexibility**. You can now specify python logging information in the 
     :doc:`/development/xppython3.ini` :code:`xppython3.ini`. This allows you to (among other things) redirect XPPython3 output to
     X-Plane's Log.txt, which will result in the information also being visible in the X-Plane Dev
     Console. See :doc:`/development/xppython3.ini` for more information.

    |

:Improvements:
   * **Mini Python** debugger window now auto-scrolls on input (in case you'd been viewing history)
     and "remembers" and reloads into history previous commands. This allows you easily repeat
     commands from one session to the next as the debugger's history recall spans sessions.

   * **Plugin Reload** will now skip checking for updated versions. It will check on X-Plane startup only.
     This is primarily to aid python developers, allowing them to quickly reload plugins (saving a few seconds
     each time.)

   * **Tentative Support for X-Plane 11**: This is not fully tested, but changes have been made to the
     loading code which *should* permit this and future versions of XPPython3 to run on X-Plane 11. Though
     few, if any, new features are available to XP11 users, this would mean Python version 3.12+ would
     be usable for X-Plane 11.
     
    |
     
:Fixes:
   * Incorrectly converted AcceptParent widget message when using :py:func:`xp.fixedLayout` (this is rare).
     Updated documentation for this function as well.

4.1.0 (2-Apr-2023)
------------------

:New Features:
   * **Weather Module** SDK400 introduced XPLMWeather which provides :py:func:`xp.getMETARForAirport`
     and :py:func:`xp.getWeatherAtLocation`.

   * **Sound Module** SDK400 introduced XPLMSound which provides the ability to easily
     play 3d sound using :py:func:`xp.playPCMOnBus` and related functions.

   * **DataRef Query** functions added by SDK400. :py:func:`xp.countDataRefs`, :py:func:`xp.getDataRefsByIndex`,
     and :py:func:`xp.getDataRefInfo`.

   * **Avionics Draw** times now contribute XPPython3 performance calculation. Time
     spent within avionics draw callbacks is added to the "Drawing Misc." time. See
     :doc:`/usage/performance`.

   * **SDK401** supported. X-Plane SDK400 had a c-compiler compatibility issue which has been
     fixed in SDK401 -- there was no additional functionality in this SDK version.
     
     |
        
:Improvements:
   * **Authorize XPL** script changed slightly to update the XPL file found in same
     folder hierarchy as the script. Previously, it relied on the location of the
     script with the same script id. Unfortunately, if you had multiple copies of
     the script on your computer, click on *one* of the copies might actually
     execute in the folder of a *different* copy.

   * **Disabled means disabled**. If you disable your python plugin (perhaps by
     returning zero in response the the XPluginEnable request.) We'll no longer
     forward messages to you, or attempt to disable your plugin when reloading or
     shutting down. However, if you reload all plugins, we'll attempt to re-enable
     your plugin.
     
   * **CommandCallback error processing** improved: if you write a CommandCallback which
     fails to return required 0 or 1, we'll report the error more clearly.

   * **Internal Python Dicts** changed. ``modules`` dict now uses module name as the key
     instead of plugin info tuple. This allows us the change plugin information dynamically.
     PyCapsule names have been changed to match their original C datatype.
     
     |

:Fixes:
   * Changed python2 compatibility check. Formerly, if we discovered PythonInterface (python2)
     plugin running, we would block loading XPPython3 plugin, to avoid compatibility issues.
     It appears PythonInterface and XPPython3 can run together under Windows. On other platforms
     it appears to fail. Rather than stopping XPPython3, we now just issue a warning (and you're on
     your own!)
     
4.0.0 (2-Jan-2023)
------------------
:Note:
   * Supports Python 3.10 and 3.11 **only**. Please upgrade your python
     installation. We'll likely add 3.12 once it's fully released (scheduled for late 2023)

   * XPPython3 v3.1.5 is the *final* release for X-Plane 11.

     |

:New Features:
   * **SDK400**: This is the first version of the updated SDK for X-Plane 12
     (and is why this will not work with older versions of X-Plane.)
     
   * **Python 3.11** support required some internal changes. You should not
     need to change any of your plugins.

   * **ImGui 1.8.2** widget library is now supported (and included). See :doc:`/development/imgui`.
     This is an upgrade from previous v1.4.6.

     |
   
:Improvements:
   * **Reorganized documentation**. This on-line documentation has been reorganized
     with new sections added to (I hope) make writing plugins easier. Also Table
     of Contents section (left side of every page) now shows your progress through
     all the information.

3.1.5 (9-Sep-2022)
-------------------
:Note:
   * This release *does not support* python 3.6 which has reached end-of-life.
     Use XPPython3 v3.1.4 if that is required.

   * This release will be the *last* to support python 3.7, 3.8, 3.9. Most users have
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
     X-Plane 12 is more sensitive about access from non-main threads. The download routines (used
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
     reinstall by downloading the latest version. See :doc:`/usage/older_python`
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

  * Fixed problems with :py:func:`xp.registerDataAccessor`. When accessing a data item which
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
 * :py:data:`xp.Msg_MouseWheel` message incorrectly processed
   during :py:func:`xp.selectIfNeeded`, which would result in an
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
