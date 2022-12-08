Plugin Installation
-------------------

Requirements
============
* **Python3** not Python2, 64-bit.
* **X-Plane 12.00+**. Linux, Mac (Intel and M-series processors) or Windows.
  This plugin is built with SDK 400 and is NOT backward compatible to X-Plane 11 or earlier.

Installation
============

Four steps:

1. **Download and install Python 3.11.x**

  + https://www.python.org/downloads/

  **For Windows**
    select "Add Python x.x to PATH" (*PLEASE*. This is the single most common error!)

    .. image:: /images/windows_python_environment.jpg

    Choose either "Install Now" or "Customize Installation", either will work as long as python is
    added to your path!

  **For Linux**
    use your package manager (deb, apt, etc.) install as root, and make sure you also have pip and libpython.
    (e.g., ``apt-get install python3-pip``)
    Check which version of python3 is actually installed, because you'll need to get *that* version of XPPython3 plugin: ``python3 --version``.

    We search for the matching library along the usual library search path. If you're using a virtual environment or
    install a personal copy of python, just make sure it's visible on the search path.

  **For Mac**
    just install directly from python.org.

  Questions, see [#F1]_. Want to use a different version of python, see `Other Versions of Python <older_python.html>`_.

2. **Download XPPython3 zipfile**:

  + `xppython311.zip <https://github.com/pbuckner/x-plane_plugins/raw/master/XPython/Resources/plugins/xppython311.zip>`_.

    (Use the above zip file only if you have Python 3.11.x installed. Otherwise, use `Other Versions of Python <older_python.html>`_.

3. **Extract the xppython*.zip** into your :code:`X-Plane/Resources/plugins` folder, such that you have folder there called :code:`XPPython3`.

  This plugin XPPython3 folder must be placed in :code:`<XP>/Resources/plugins`.
  On first execution, XPPython3 **will create** the :code:`Resources/plugins/PythonPlugins` folder.

  You should place any third-party python plugins in that PythonPlugins folder. (On first run of
  the plugin, XPPython3 will automatically download a set of sample plugins into
  your ``Resources/plugins/PythonPlugins/samples``
  folder. To execute a sample, move a plugin up into the `PythonPlugins` folder.)

  You file system layout should match::

    <X-Plane>/
    └─── Resources/
         └─── plugins/
              └─── XPPython3/
                   ├─── mac_x64/
                   |    └─── XPPython3.xpl
                   ├─── lin_x64/
                   |    └─── XPPython3.xpl
                   └─── win_x64/
                        └─── XPPython3.xpl


.. _for-mac:

  **Extra For Mac**
     
     STOP! Before continuing, you need to remove the quarantine. Follow instructions in :doc:`mac_quarantine`.
     
     If you forget to do this, *XPPython3 will not load* and you'll get error popup
     indicating XPPython3.xpl cannot be opened. See :doc:`common_errors`.

4. **Start X-Plane**.

  On installation success, we'll popup a window:

  .. image:: /images/python_samples_popup.png

If you have problems with installation
======================================

a) *Actually read this page*.
b) Check :doc:`common_errors`.
c) If you still have problems with installation provide information described in :doc:`getting_support`.

Errors are logged, see :doc:`logfiles`.

|
|

Next, How to use XPPython3:

.. toctree::
   :maxdepth: 2

   runtime_menus
   pip
   logfiles
   add_plugin

----

.. [#F1] "*But I already have a copy of python on my computer*". Yes, you probably do. Is it the right version,
         includes the necessary packages and scripts, and located in the correct location? Maybe, but **no one** 
         wants to try and debug that, so *please*: you have the disk space. Download a brand new copy of python,
         place it in the recommended location and think of it as being used exclusively for X-Plane.
