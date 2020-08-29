XPPython3 Plugin Installation
-----------------------------

1. Install 64-bit Python3 (version 3.6 or greater) from https://python.org

2. Download ONE zipfile:

  + For Python3.6: `xppython36.zip <https://github.com/pbuckner/x-plane_plugins/raw/master/XPython/Resources/plugins/xppython36.zip>`_.
  + For Python3.7: `xppython37.zip <https://github.com/pbuckner/x-plane_plugins/raw/master/XPython/Resources/plugins/xppython37.zip>`_.
  + For Python3.8: `xppython38.zip <https://github.com/pbuckner/x-plane_plugins/raw/master/XPython/Resources/plugins/xppython38.zip>`_.

3. Extract it into your :code:`X-Plane/Resources/plugins` folder, such that you have folder there called :code:`XPPython3`.

Requirements
============
* requires 64-bit operating system
* requires python3, tested with

  + python37 and python38 on windows,
  + python37 on Mac,
  + ubuntu 18 (python36, python37, python38), ubuntu 20 (python38)

* XP 11.50+. Plugin is built with SDK 303 and is NOT backward compatible to X-Plane 11.41 or earlier.

The plugin version **must match** the version of python (3.6, 3.7, 3.8) you computer is
running: the plugin will not load if python is not correctly installed, or if the
version does not match. If you change python versions on you computer, you must change plugin version
to match. Any micro-release may be used for a particular major.minor release: For example, python 3.7.0 and 3.7.3 are both "3.7"

.. Note::
   All operating systems will allow you to have multiple versions of python installed (a bold statement, I know).
   The hard part becomes setting `which` python is used when you merely invoke :code:`python`. XPPython3 does not
   care which is the `default` version of python on you system: It looks for, and loads the first version of
   python3 it can find in the standard locations, so if you've installed it from https://python.org it will be loaded.

Installation
============

This plugin XPPython3 folder should be placed in :code:`<XP>/Resources/plugins`.
On startup, this plugin will create the PythonPlugins folder, if you don't have
one.

::

  <X-Plane>/
  └─── Resources/
       └─── plugins/
            ├─── XPPython3/
            │    ├─── mac_x64/
            │    |    └─── XPPython3.xpl
            │    ├─── lin_x64/
            │    |    └─── XPPython3.xpl
            │    └─── win_x64/
            │         └─── XPPython3.xpl
            └─── PythonPlugins/
                 │
                 ├─── PI_<plugin1>.py
                 ├─── PI_<plugin2>.py
                 └─── ....

    
Third-party Python plugins themselves go to:

* **Resources/plugins/PythonPlugins/** folder

  + Single file named :code:`PI_<anything>.py` for each separate python plugin. This is the starting point for each python plugin
    (For Python2, we used "PythonScripts" folder -- same idea, but we need a different folder.)
  + Plugins are loaded in order as returned by the OS: that is, do not assume alphabetically!
  + Python files can then import other python files.

Internally, we also use:

* **Resources/plugins/XPPython3/** folder

  + "internal" plugins. This is intended for internal use, and are additional python plugins loaded
    prior to the user directory "PythonPlugins". Note this is (usually) the same folder as holding
    the binary :code:`*.xpl` plugin files. To be loaded on startup, files need to be named :code:`I_PI<anything>.py`.
  + python files in this directory will be in you PYTHONPATH and therefore accessible to your
    scripts.


Plugin Menus
============

XPPython3 installs a single menu item, "XPPython3", on the plugin menu. From there, you can:

* **Disable** - disables all scripts.
* **Enable** - only useful if you've disabled the scripts.
* **Reload** - Stops and restarts python, reloading all scripts. While this works, note that some
  plugins do not clean up after themselves, so loading all python plugins may result in duplications.

There is also an **About** menu item which further describes this plugin, including links to documentation
and current version number.

The XPPython3 menu includes two final items:

* **Version** -- with an indication if you're using the most current version. If you're not using the
  most current version, selecting this item will attempt to update your XPPython3 version to the latest.
  (It will not change Python versions, just XPPython3 plugin versions.) If successful, the new version
  is downloaded, but you'll need to restart X-Plane to have the new plugin take effect.
* **Pip Package Installer** -- *pip* is a python-based installer that is common used to install
  python packages (i.e., modules or script libraries). While Python comes with lots of built-in libraries,
  you may need to install some 3rd party libraries. Using this installer will make sure the
  libraries are installed into the proper version of python on your computer. This is *not* to be used
  to install X-Plane plugins. If your python plugin requires additional Python packages, your plugin should
  tell you which packages you'll need to install.

Logging
=======

There are two main log files. (Any particular plugin may also create their own log file.)

* **Log.txt**: The standard X-Plane Log file
* **XPPython3.log**: Standard output for python plugins

`Log.txt`
*********

* Some messages go to Log.txt. Specifically, when python plugin itself is loaded:

  :code:`Loaded: <XP>/Resources/plugins/XPPython3/mac_x64/xppython3.xpl (XPPython3.0.0).`
   
  If XPPython3 cannot load, you'll see an error in this log file.

* Common error on windows:

  :code:`<XP>/Resources/plugins/XPPython3/win_x64/XPPython3.xpl: Error Code = 126 : The specified module could not be found.`
     
  **Cause**: X-Plane cannot load all DLLs required by plugin. In this case, the python plugin is looking for python itself.
  Usually, python is installed in `C:\\Program Files\\Python3X folder`, where you'll find a file `python3.dll`.
   
  **Solution**:

  1. Python needs to be installed "for all users" -- that places the folder under \Program Files, if not for all
     users, it's stored somewhere else & X-Plane may not be able to find it. Or,
  2. Python needs to be installed with "Set Environment Path" (**need correct wording**)
     This helps X-Plane find it -- perhaps, it could be stored "for single user" but PATH needs to set?

`XPPython3.log`
***************

Python messages go to :code:`<XP>/XPPython3.log` (for python2 it was a couple files in the
:code:`<XP>/Resources/plugins/PythonScripts` folder.) You can change location of this logfile
by setting environment variable :code:`XPPYTHON3_LOG`. Log is re-written each time (Python2,
we appended to the file rather than clearing it out.) If you want to preserve
the contents of the logfile, set environment variable :code:`XPPYTHON3_PRESERVE`.

Log always contains:

.. parsed-literal::

   XPPython3 Version *<x.x.x>* Started.

Then the script folder(s) are scanned. If the folder cannot be found (not an error really, but just to
let you know):

.. parsed-literal::

   Can\'t open *<folder>* to scan for plugins.

On *each* python plugin startup, we print:

.. parsed-literal::

   PI\_\ *<plugin>* initialized.
        Name: *<plugin name>*
        Sig:  *<plugin signature>*
        Desc: *<plugin description>*

Successful shutdown will included:

::

  XPPython Stopped.

Errors
======

If you have errors running python plugins,

1. Check ``Log.txt``. Make sure python and the python plugin are installed correctly. If not,
   there will be a message in Log.txt, and XPPython3.txt will not be created. Verify it's Python3, not Python2
   getting loaded.

2. Check ``XPPython3.log``. Most python coding errors / exceptions will be reported in this
   log.

You should provide both Log.txt and XPPython3.log when looking for assistance with a python plugin.

.. Warning:: It is **very** unlikely that a python2 plugin will work without modification with python3. The
             python language itself changed between versions. Additionally, python2 plugins used older
             X-Plane APIs, some of which have been retired with X-Plane 11.50+.

             If you have questions about a particular python plugin, please contact the plugin's creator!
             
Getting Support
===============

Include:

#. Name of the plugin you're trying to use
#. Copy of ``Log.txt`` (which tells us most everything about your X-Plane installation)
#. Copy of ``XPPython3.log`` (which tells us most everything about your Python installation)

Provide that information to x-plane.org/forums or email support: Information is available from the
*About* XPPython3 menu.
