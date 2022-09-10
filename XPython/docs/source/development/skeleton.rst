Skeleton
========

This does nothing, except log itself in XPPython3Log.txt -- a minimal "I Am Here".

::

 class PythonInterface:
     def __init__(self):
         self.Name = "Skeleton"
         self.Sig = "skeleton.xppython3"
         self.Desc = "Minimal do-nothing plugin"

     def XPluginStart(self):
         return self.Name, self.Sig, self.Desc

     def XPluginStop(self):
         pass

     def XPluginEnable(self):
         return 1

     def XPluginDisable(self):
         pass

     def XPluginReceiveMessage(self, inFromWho, inMessage, inParam):
         pass

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

#. You **should** have three more methods:  ``XPluginStop()``, ``XPluginDisable()``, ``XPluginReceiveMessage()``

   * These are not require, and their absence is treated with a sane default.
   * If included, the need to have the correct signature with the correct return value.

#. Your ``XPPluginStart`` **must** return three strings, in this order:

   * **Name**: The name should be short, but is used just for display purposes. So whatever you like.
   * **Signature**: The signature *must be unique* across all plugins in the system. Plugins have the
     ability to communicate with other plugins and they use the unique signature to find the other plugin.
     A common technique is to name the plugin relative to a domain name you control. (It does not have
     to be a domain name, you could uses "Fred's First Plugin" as a signature.)
   * **Description**: Like the name, this is really just for descriptive purposes.

   You'll note it our example, we set these values in ``__init__()``, but strictly speaking, that's not required

#. Your ``XPluginEnable`` **should** return ``1``.

   * Returning ``1`` indicates you were successfully enabled. If you don't return ``1``, we'll assume the worse and
     never speak with you again.

... So technically, minimum would be:

::

 class PythonInterface:

     def XPluginStart(self):
         return '', '', ''

     def XPluginEnable(self):
         return 1

That's it!

See :doc:`helloworld` for next example.
