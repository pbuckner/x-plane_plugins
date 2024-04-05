XPPython3 Version 4
===================

XPPython3 [#why]_ is a plugin for Laminar Research `X-Plane <https://www.x-plane.com>`_. It supports writing
of additional plugins using python3. This is a replacement for Sandy Barbour's excellent
`PythonInterface <http://www.xpluginsdk.org/python_interface.htm>`_ plugin, which supported only python2.

Curious? See `current usage graphs <https://maps.avnwx.com/x-plane/stats>`_.

This is XPPython3 version 4 and includes both the X-Plane plugin as well as a private
version of Python3. Unlike previous versions of XPPython3, you no longer need to install
your own copy of Python.

* **X-Plane 12**

* **Python3** (included)
  
.. note::

   For **X-Plane 11** or other versions of python, you should use an earlier
   version of this plugin: `XPPython3 Version 3.1.5 <https://xppython3.rtfd.io/en/3.1.5/>`_,
   you *may* try using this **Version 4.x** with X-Plane 11: it appears to work with the older
   version of X-Plane, but this is not fully tested.

See :doc:`usage/installation_plugin` to install the plugin.

See :doc:`development/index` to write your own python plugins.

:doc:`changelog` lists current and previous versions.
  



|
|
|

Next:

.. toctree::
   :hidden:
   :titlesonly:
      
   usage/installation_plugin
   usage/runtime_menus
   usage/pip
   usage/logfiles
   usage/add_plugin
   development/index
   development/udp/index


..   
   How to Install <usage/installation_plugin>
   How to Develop <development/index>



----

.. [#why]             
    It's called XPPython3:

    1. "xpython" already exists (it's an all python version of python)

    2. "3" to emphasize it is python3, which is not fully backward compatible with python2.

