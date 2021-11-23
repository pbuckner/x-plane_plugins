Plugin Installation
-------------------

Requirements
============
* 64-bit operating system: linux, mac, or windows
* Python3, 64-bit version, not python2 (aka "python")
* X-Plane 11.50+. Plugin is built with SDK 303 and is NOT backward compatible to X-Plane 11.41 or earlier.

Installation
============

1. Install 64-bit Python 3.9

  (*Python 3.10* is supported, but not yet recommended: it works, but as it was just released, some
  third-party modules are not available.)

  + https://www.python.org/downloads/

  **For Windows**
    install it "For All Users".  (*PLEASE*. This is the single most common error!)

  **For Linux**
    use your package manager (deb, apt, etc.) install as root, and make sure you also have pip and libpython.
    (e.g., ``apt-get install python3-pip``)
    Check which version of python3 is actually installed, because you'll need to get *that* version of XPPython3 plugin: ``python3 --version``.

  **For Mac**
    just install directly from python.org.

  Questions, see [#F1]_. Want to use a different version of python, see `Other Versions of Python <older_python.html>`_.

2. Download XPPython zipfile:

  + `xppython39.zip <https://github.com/pbuckner/x-plane_plugins/raw/master/XPython/Resources/plugins/xppython39.zip>`_.

3. Extract the xppython*.zip into your :code:`X-Plane/Resources/plugins` folder, such that you have folder there called :code:`XPPython3`.

  This plugin XPPython3 folder must be placed in :code:`<XP>/Resources/plugins`.
  On first execution, XPPython3 **will create** the :code:`Resources/plugins/PythonPlugins` folder.

  You should place any third-party python plugins in that PythonPlugins folder. (On first run of
  the plugin, XPPython3 will automatically download a set of sample plugins into
  your ``Resources/plugins/PythonPlugins/samples``
  folder. To execute a sample, move a plugin up into the `PythonPlugins` folder.)

  **For Mac**
     Your newly downloaded plugin is subject to Apple's quarantine on recent versions of macOS. You will **not** be
     able to execute the plugin until you first remove the quarantine::

       $ cd /users/pbuck/X-Plane/Resources/plugins/XPPython3/mac_x64
       $ xattr -d com.apple.quarantine XPPython3.xpl 

     (Or whereever you installed X-Plane: the top folder name will differ.)
     
     If you forget to do this, *XPPython3 will not load* and you'll get error popup indicating XPPython3.xpl cannot be opened. See
     :doc:`common_errors`.
  
4. Start X-Plane.

  On installation success, we'll popup a window:

  .. image:: /images/python_samples_popup.png

If you have problems with installation, *actually read this page*. Check :doc:`common_errors`.
If you still have problems with installation provide
information described in :ref:`GettingSupport`.

Errors are logged, see :doc:`logfiles`.

.. _pip-package-installer:

Running the First Time - Pip Package Installer
==============================================

First time running X-Plane, go to the XPPython3 Menu, select "Pip Package Installer" and install two
useful packages: ``pyopengl, cryptography``. Type in the package names and press Install. It will take a few seconds (depends on
your Internet speed.) Though these packages are not required by XPPython3, they are commonly used by plugins, so you might
as well install them now.

If Pip fails because it cannot find pip (most commonly on linux) make sure you've manually added the python3-pip package.

.. image:: /images/pip_installer.png

File System Layout
==================

XPPython3 is an X-Plane plugin, so it needs to be in the ``Resources/plugins`` folder.
In turn, XPPython3 loads PI\_\*.py plugin files, so *they* need to be in ``Resources/plugins/PythonPlugins`` folder.
Python plugins in the ``samples`` folder won't get loaded on X-Plane startup.

The structure is:

.. code-block:: none

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
                 ├─── PI_<plugin1>.py
                 ├─── PI_<plugin2>.py
                 ├─── ....
                 └─── samples/
                      ├─── PI_<sample1>.py
                      ├─── PI_<sample2>.py
                      └─── ....

    
Third-party Python plugins themselves go to:

* **Resources/plugins/PythonPlugins/** folder

  + Single file named :code:`PI_<anything>.py` for each separate python plugin. This is the starting point for each python plugin [#F2]_ 
  + Plugins are loaded in order as returned by the OS: that is, do not assume alphabetically!
  + Python files can then import other python files.

Internally, we also use:

* **Resources/plugins/XPPython3/** folder

  + *Do Not* place your python files in this directory. They may be deleted on reload of the XPPython3 plugin.
  + These are "internal" plugins. This is intended for internal use, and are additional python plugins loaded
    prior to the user directory "PythonPlugins". Note this is (usually) the same folder as holding
    the binary :code:`*.xpl` plugin files. To be loaded on startup, files need to be named :code:`I_PI<anything>.py`.
  + Python files in this directory will also be in your PYTHONPATH and therefore accessible to your
    scripts. (Feel free to look at these source files for examples / inspiration).


Errors
======

If you have errors running python plugins,

1. Check ``Log.txt``. Make sure python and the python plugin are installed correctly. If not,
   there will be a message in Log.txt, and XPPython3.txt will not be created. Verify it's Python3, not Python2
   getting loaded.

2. Check ``XPPython3Log.txt``. Most python coding errors / exceptions will be reported in this
   log.

See also :doc:`common_errors`.

You should provide both **Log.txt** and **XPPython3Log.txt** when looking for assistance with a python plugin.

.. Warning:: It is **very** unlikely that a python2 plugin will work without modification with python3. The
             python language itself changed between versions. Additionally, python2 plugins used older
             X-Plane APIs, some of which have been retired with X-Plane 11.50+.

             If you have questions about a particular python plugin, please contact the plugin's creator!
             
.. _GettingSupport:

Getting Support
===============

Include:

#. Name of the plugin you're trying to use, (e.g., PI_foobar.py)
#. Copy of ``Log.txt`` (which tells us most everything about your X-Plane installation)
#. Copy of ``XPPython3Log.txt`` (which tells us most everything about your Python installation). This log is found
   in the same folder as Log.txt.

Provide that information to x-plane.org/forums or email support: Information is available from the
*About* XPPython3 menu.


More Installation Topics
========================

.. toctree::
  :maxdepth: 2

  common_errors
  runtime_menus
  logfiles
  older_python

----

.. [#F1] "*But I already have a copy of python on my computer*". Yes, you probably do. Is it the right version,
         includes the necessary packages and scripts, and located in the correct location? Maybe, but **no one** 
         wants to try and debug that, so *please*: you have the disk space. Download a brand new copy of python,
         place it in the recommended location and think of it as being used exclusively for X-Plane.

.. [#F2] For Python2, we used "PythonScripts" folder -- same idea, but we need a different folder.
