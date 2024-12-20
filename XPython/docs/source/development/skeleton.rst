.. py:module:: PythonInterface

Skeleton
========

Plugins (C and Python) work by having a standard calling interface. For C, it's 5 functions, for Python it's one class ``PythonInterface``
with five methods:

+--+----------------------------------------------------------------------------+-----------------------------------------------------------------+
|1.|.. py:function:: XPluginStart(self)                                         | Called by X-Plane at startup, this is called for every plugin   |
|  |                                                                            | and each plugin returns its signature.                          |
|  |   Returns three strings                                                    |                                                                 |
+--+----------------------------------------------------------------------------+-----------------------------------------------------------------+
|2.|.. py:function:: XPluginEnable(self)                                        |Once all plugins are started, X-Plane calls each plugin to       |
|  |                                                                            |register callbacks and other resources.  Each plugin returns 1 or|
|  |   Returns int 1 or 0                                                       |0 indicating it enabled properly.                                |
+--+----------------------------------------------------------------------------+-----------------------------------------------------------------+
|3.|.. py:function:: XPluginReceiveMessage(self, inFromWhom, inMessage, inParam)|While running, X-Plane may send messages to all plugins such as  |
|  |                                                                            |"PLANE_LOADED" or "ENTERED_VR". Plugins use this call to handle  |
|  |   No return value                                                          |(or ignore) the message.                                         |
|  |                                                                            |                                                                 |
+--+----------------------------------------------------------------------------+-----------------------------------------------------------------+
|4.|.. py:function:: XPluginDisable(self)                                       |Called at X-Plane termination so each plugin can unregister any  |
|  |                                                                            |callbacks and stop "doing work".                                 |
|  |   No return value                                                          |                                                                 |
+--+----------------------------------------------------------------------------+-----------------------------------------------------------------+
|5.|.. py:function:: XPluginStop(self)                                          |Called after all plugins are disabled so each plugin is able to  |
|  |                                                                            |save its state, close files, deallocate resources.               |
|  |   No return value                                                          |                                                                 |
+--+----------------------------------------------------------------------------+-----------------------------------------------------------------+



In Python, here's how it looks.
The following code does nothing, except log itself in XPPython3Log.txt -- a minimal "I Am Here". (Seriously, don't bother
coding this example -- just use it as a reference.)

.. include:: skeleton.py
  :code:

Place it into the PythonPlugins folder, restart X-Plane and nothing will happen, except XPPython3Log.txt will
include:

::

   PI_Skeleton initialized.
      Name: Skeleton
      Sig:  skeleton.xppython3
      Desc: Minimal do-nothing plugin

This is all required:

#. The file **must** be named starting ``PI_`` and ending ``.py``.

   * The filename doesn't have to match the name of the plugin, we don't care. (Rename ``PI_HelloWorld.py`` to ``PI_avDD3.py`` and
     it works precisely the same.)
   * The filename **must** start with ``PI_``. That's how we know it contains a plugin to be loaded into X-Plane:
     otherwise it's just a python file.
   * The filename (as with all python) becomes the module name. That's different (can be different...) from the Plugin Name (the
     string you return via ``XPluginStart()``.

#. You **must** have a ``class PythonInterface``.

   * You can have other classes also.
   * Your ``PythonInterface`` can inherit from other classes.
   * BUT, you must have exactly one ``class PythonInterface``. That's how we get the plugin's entry point.

#. You **must** have two named methods: ``XPluginStart()`` and ``XPluginEnable()``

   * You can have other methods also.
   * The required methods could be imported from another class.
   * Order of methods are not important.
   * BUT, they all must exist. X-Plane will invoke your methods directly, calling your ``PI_avDD3.PythonInterface().XPluginStart()`` when
     it is ready to start your plugin, so that had better exist.

#. Your ``XPPluginStart`` **must** return three strings, in this order:

   * **Name**: The name should be short, but is used just for display purposes. So whatever you like.
   * **Signature**: The signature *must be unique* across all plugins in the system. Plugins have the
     ability to communicate with other plugins and they use the unique signature to find the other plugin.
     A common technique is to name the plugin relative to a domain name you control. (It does not have
     to be a domain name, you could uses "Fred's First Plugin" as a signature.)
   * **Description**: Like the name, this is really just for descriptive purposes.

   You'll note it our example, we set these values in ``__init__()``, but strictly speaking, that's not required

#. You **should** have three more methods:  ``XPluginStop()``, ``XPluginDisable()``, ``XPluginReceiveMessage()``

   * These are not required, and their absence is handled with a sane default.
   * If included, the need to have the correct signature with the correct return value.
   * See :ref:`Inter-plugin Messaging` for more information about ``XPluginReceiveMessage()``.

#. Your ``XPluginEnable`` **should** return ``1``.

   * Returning ``1`` indicates you were successfully enabled. If you don't return ``1``, we'll assume the worse and
     never speak with you again.

... So technically, the absolute minimum would be:

::

 class PythonInterface:

     def XPluginStart(self):
         return '', '', ''

     def XPluginEnable(self):
         return 1

That's it!

See :doc:`helloworld` for next example.
