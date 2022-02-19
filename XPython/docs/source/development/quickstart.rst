Quick Start
===========

So you want to create a plugin?

First, make sure you can actually run an XPPython3 plugin -- if X-Plane, Python, and XPPython3 aren't set up
correctly, you'll get no where.

* Your version of X-Plane is *at least* 11.50 (beta versions are okay)

* You've installed XPPython3 as described in :doc:`/usage/installation_plugin`.

* You've successfully executed an XPPython3 plugin within X-Plane. If you need an example, download
  :download:`PI_HelloWorld1.py <https://github.com/pbuckner/x-plane_plugins/raw/master/XPython/demos/PI_HelloWorld1.py>`
  and place it in your Resources/plugins/PythonPlugins folder.

Once you have a running system, go through these basic examples:


 #. :doc:`skeleton`: the required minimum code to do absolutely nothing.

 #. :doc:`helloworld`: a simple plugin with a popup window.
  
 #. :doc:`hellowidget`: a simple plugin with a popup window using widgets.
  
Couple of quick points common to all XPPython3 plugins:

* ``print()`` sends information to the XPPython3Log.txt. This is great for quick debugging. Remove these
  prior to deploying your plugin.
* ``import logging``: Using the standard python ``logging`` module sends information to stderr. Like ``stdout``, ``stderr`` is
  also redirected to XPPython3Log.txt.
* :py:func:`xp.log` sends to the XPPython3Log.txt, and automatically adds ``[<your plugin name>]`` to the
  text. This is better for "real" messages you want to keep in the python log that the end user might need to
  see to help you debug in the field.
* :py:func:`xp.sys_log` sends to the main Log.txt, and automatically adds your plugin name. Use this
  sparingly (there's enough stuff in Log.txt already) but this is most useful for initial
  configuration problems, as the user is most likely to provide you with a Log.txt file
  and say, "it doesn't work."
* The python interface supports positional and keyword parameters. For many functions,
  some parameters are optional. See documentation for details. (Keyword parameters are
  *only* for the ``xp`` varient, e.g., ``xp.destroyWindow``. ``XPLM`` varients, e.g., ``XPLMDisplay.XPLMDestroyWindow``,
  support only positional parameters, and all parameters must be specified.::

     xp.destroyWindow(myWindowID)           # Works! Use positional parameters
     xp.destroyWindow(windowID=myWindowID)  # Also Works!

* In our examples, we freely mix traditional API with shortened *xp* API::

     # These two calls are equivalent:
     # a) Using the simplified xp interface
     import xp
     xp.destroyWindow(myWindowID)

     # b) Using the traditional interface
     import XPLMDisplay
     XPLMDisplay.XPLMDestroyWindow(myWindowID)

  The benefit of the *xp* interface is you don't have to remember which module
  the function is defined in & if it's XP... or XPLM... or XPUI... See :doc:`xp module documentation </development/modules/xp>`.
  Plus, you can use optional and keyword parameters.

.. toctree::
   :caption: Code Examples:

   skeleton
   helloworld
   hellowidget
   samples

