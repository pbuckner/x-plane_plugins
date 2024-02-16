Plugin Installation
-------------------

Requirements
============

* **X-Plane 12.00+**. Linux, Mac (Intel and M-series processors) or Windows.
  This plugin is built with SDK 400 and is NOT backward compatible to X-Plane 11 or earlier.

.. note:: This version of XPPython3 requires X-Plane 12. If you are using X-Plane 11.52, you
   must install XPPython3 v3.1.5.
   See `Installation for X-Plane 11 <https://xppython3.rtfd.io/en/3.1.5/usage/installation_plugin.html>`_.

If you're **upgrading** from XPPython v3.1.5, see :doc:`upgrading`.

Installation
============

Three steps:

1. **Download XPPython3 zipfile**:

  Select zipfile:

  +----------------+--------------------------------------------------------------------------------------------------------------------+
  | For Windows    |`xp3-win32.zip <https://github.com/pbuckner/x-plane_plugins/raw/master/XPython/Resources/plugins/xp3-win32.zip>`_   |
  +----------------+--------------------------------------------------------------------------------------------------------------------+
  | For Linux      |`xp3-linux.zip <https://github.com/pbuckner/x-plane_plugins/raw/master/XPython/Resources/plugins/xp3-linux.zip>`_   |
  +----------------+--------------------------------------------------------------------------------------------------------------------+
  | For Mac        |`xp3-darwin.zip <https://github.com/pbuckner/x-plane_plugins/raw/master/XPython/Resources/plugins/xp3-darwin.zip>`_ |
  +----------------+--------------------------------------------------------------------------------------------------------------------+


2. **Extract the xp3xxx.zip** into your :code:`X-Plane/Resources/plugins` folder, such that you have folder there called :code:`XPPython3`.

  This plugin XPPython3 folder must be placed in :code:`<XP>/Resources/plugins`.
  On first execution of X-Plane, XPPython3 **will create** the :code:`Resources/plugins/PythonPlugins` folder.

  You should place any third-party python plugins in that PythonPlugins folder. (On first run of
  the plugin, XPPython3 will automatically download a set of sample plugins into
  your ``Resources/plugins/PythonPlugins/samples``
  folder. To execute a sample, move a plugin up into the `PythonPlugins` folder.)

  You file system layout should match (with only one of mac_x64 or lin_x64 or win_x64::

    <X-Plane>/
    └─── Resources/
         └─── plugins/
              └─── XPPython3/
                   ├─── mac_x64/
                   |    ├─── XPPython3.xpl
                   |    └─── python3.12/
                   ├─── lin_x64/
                   |    ├─── XPPython3.xpl
                   |    └─── python3.12/
                   └─── win_x64/
                        ├─── XPPython3.xpl
                        ├─── python.exe
                        ├─── ...
                        └─── Lib/


.. _for-mac:

  **Extra For Mac**
     
     STOP! Before continuing, you need to remove the quarantine. Follow instructions in :doc:`mac_quarantine`.
     
     If you forget to do this, *XPPython3 will not load* and you'll get error popup
     indicating XPPython3.xpl cannot be opened. See :doc:`common_errors`.

3. **Start X-Plane**.

  On installation success, we'll popup a window:

  .. image:: /images/python_samples_popup.png

.. Note::
   If you have problems with installation

   a) *Actually read this page*.
   b) Check :doc:`common_errors`.
   c) If you still have problems with installation provide information described in :doc:`getting_support`.

Errors are logged, see :doc:`logfiles`.

If it's running, get familiar with :doc:`runtime_menus`.

.. toctree::
   :titlesonly:
   :hidden:

   common_errors
   getting_support
   
