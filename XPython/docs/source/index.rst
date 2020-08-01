
XPPython3
=========

.. Note:: **Status**

   * The plugin appears to be complete and working
   * The documentation, especially in the stub files, is complete and accurate.

   Current effort is porting documentation to this web format, adding demos

   While I invite curious plugin developers to experiment with this plugin, please realize
   there is a non-zero chance we'll still see a minor API change.

   Please contact me directly with questions ``pbuck [at] avwnx.com``.
             
XPPython3 is a plugin for Laminar Research `X-Plane <https://www.x-plane.com>`_. It supports writing
of additional plugins using python3. This is an upgrade of Sandy Barbour's excellent
`PythonInterface <http://www.xpluginsdk.org/python_interface.htm>`_ plugin, which supported only python2.

It's called XPPython3:

1. "xpython" already exists (it's an all python version of python)

2. "3" to emphasize it is python3, which is not fully backward compatible with python2.

Key features
------------

* **X-Plane 11.50** *minimum* There is no attempt to make this work on older versions of X-Plane. This plugin
  _cannot_ co-exist with the older plugin, so you if your favorite plugin still uses Python2, ask the maintainer
  to port it to Python2. (Python2 is end-of-life.)
* **64-bit only**: X-Plane is now 64-bit only.
* **X-Plane SDK 303** *minimum*: This is base SDK version for X-Plane 11.50. Vulkan/Metal and OpenGL are supported.
* **Python3 only**: This will not work with python2 programs which are not compatible with Python3. Note that you will have
  to use the correct version of this plugin to match the version of python installed on your system. We currently support
  python3.6, python3.7 and python3.8

Installing the Plugin
---------------------

To install the XPPython3 plugin (not develop new python plugsin) go to :doc:`usage/installation_plugin`.

Using the SDK
-------------

To use the XPPython3 plugin for developing new python-based XP Plugins, go to :doc:`development/index`.

Building XPPython3
------------------

.. Note::
   You do not need to build XPPython3 to use python plugins, or to develop new python plugins.
   However, you may find it useful if you're trying to track down a crash bug in during your plugin development.

Build instructions are provided in :doc:`building/index`.   

.. toctree::
   :maxdepth: 1
   :caption: Contents:

   usage/installation_plugin
   development/index
   building/index
   development/modules/index



Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
