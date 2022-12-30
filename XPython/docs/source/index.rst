XPPython3
=========

.. Warning:: 

  This documentation is for XPPython3 v3.1.5, which is the **last** version to support X-Plane 11.

  If you are using X-Plane 12, you should install the lastest version of this plugin, available
  `here <https://xppython3.readthedocs.io/en/latest/index.html>`_.
             
XPPython3 [#why]_ is a plugin for Laminar Research `X-Plane <https://www.x-plane.com>`_. It supports writing
of additional plugins using python3. This is an upgrade of Sandy Barbour's excellent
`PythonInterface <http://www.xpluginsdk.org/python_interface.htm>`_ plugin, which supported only python2.

Curious? See `current usage graphs <https://maps.avnwx.com/x-plane/stats>`_.

Key Requirements
----------------

* **X-Plane 11.50** *minimum*, tentative support also for X-Plane 12.
  There is no attempt to make this work on older versions of X-Plane. This plugin
  *cannot* co-exist with the older "PythonInterface" plugin, so you if your favorite plugin still uses Python2, ask the plugin's maintainer
  to port it to Python3. (Python2 is end-of-life.)
* **64-bit only**: X-Plane is now 64-bit only, which means it only works for 64-bit versions of python.
* **X-Plane SDK 303** *minimum*: This is base SDK version for X-Plane 11.50. Vulkan/Metal and OpenGL are supported.
* **Python3 only**: Python3.10 is recommended. This will not work with python2 programs which are not compatible with
  Python3. Note that you will have to use the correct version of this plugin to match the version of python
  installed on your system. We currently support python3.7, python3.8, python3.9, and python3.10. *However* we
  will be dropping support for python versions earlier than python3.9 with the arrival of X-Plane 12.

XPPython3 will also work with X-Plane 12. Please file bug reports if you have issues.

* See :doc:`changelog`.
  

Installation
------------

See :doc:`usage/installation_plugin` to install the XPPython3 plugin. Everyone needs to do this.

Development
-----------

See :doc:`development/index` to use the XPPython3 plugin for developing new python-based XP Plugins. Read this
to see examples and learn how to build your own python plugins.

Building
--------

.. Note::
   You do not need to build XPPython3 to use python plugins, or to develop new python plugins.
   However, you may find it useful if you're trying to track down a crash bug in during your plugin development.

Next
----

Topics

.. toctree::
   :maxdepth: 2

   How to Install <usage/installation_plugin>
   How to Develop <development/index>
   changelog



Indices and tables

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

----

.. [#why]             
    It's called XPPython3:

    1. "xpython" already exists (it's an all python version of python)

    2. "3" to emphasize it is python3, which is not fully backward compatible with python2.

