Quick Start
===========

So you want to create a plugin?

First, make sure you can actually run an XPPython3 plugin -- if X-Plane, Python, and XPPython3 aren't set up
correctly, you'll get nowhere.

* Your version of X-Plane is *at least* 12.0 (beta versions are okay)

* You've installed XPPython3 as described in :doc:`/usage/installation_plugin`.

* You've successfully executed an XPPython3 plugin within X-Plane. If you need an example, download
  :download:`PI_HelloWorld1.py <https://github.com/pbuckner/xppython3-demos/raw/main/PI_HelloWorld1.py>`
  and place it in your Resources/plugins/PythonPlugins folder.

Once you have a running system, go through these basic examples:

 #. :doc:`skeleton`: the required minimum code to do absolutely nothing.

 #. :doc:`helloworld`: a simple plugin with a popup window.
  
 #. :doc:`hellowidget`: a simple plugin with a popup window using widgets.
  
Couple of quick points common to all XPPython3 plugins:

* Python ``print()`` sends information to the XPPython3Log.txt. This is great for quick debugging. Remove these
  prior to deploying your plugin. You can also use standard python logging (``import logging``) which
  sends information to stderr. Both ``stdout`` and ``stderr`` are redirected to XPPython3Log.txt.
* :py:func:`xp.log` sends to the XPPython3Log.txt, and automatically adds ``[<your plugin name>]`` to the
  text. This is better for "real" messages you want to keep in the python log that the end user might need to
  see to help you debug in the field. (Because "your" python plugins will be writing to the same log as other
  python plugins.)
* :py:func:`xp.sys_log` sends to the main Log.txt, and automatically adds your plugin name. Use this
  sparingly (there's enough stuff in Log.txt already) but this is most useful for initial
  configuration problems, as the user is most likely to provide you with a Log.txt file
  and say, "it doesn't work."
* The XPPython3 interface is *nearly identical* to the C API. Being python, we've improved things
  by adding keywords and making some parameters optional. If you know the X-Plane C API, you'll find
  the python API easy.
  See `Simplified Python Interface in module xp <modules/xp.html>`_ for details. (Keyword parameters are
  *only* for the ``xp`` varient, e.g., ``xp.destroyWindow()``. The ``XPLM`` varients such as ``XPLMDisplay.XPLMDestroyWindow()``,
  support only positional parameters, and all parameters must be specified.
  ::

     xp.destroyWindow(myWindowID)              # Works! Use positional parameters
     xp.destroyWindow(windowID=myWindowID)     # Also Works!
     XPLMDisplay.XPLMDestroyWindow(myWindowID) # Works as well.

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

  You may intermix the two interfaces without problems (other than confusion on your part)::

    import xp
    import XPLMDisplay

    myWindowID = xp.createWindow()
    XPLMDisplay.XPLMDestroyWindow(myWindowID)

* Finally, if you write bad python code, or use Python2 rather than Python3 syntax, you'll see the exception in
  XPPython3Log.txt -- usually, but see warning.

.. warning:: Errors in some calls to the X-Plane interface don't cause exceptions. Instead
             they'll just terminate the plugin's method (e.g., terminate the flightloop or draw callback)
             with no further message. These are hard to debug, but adding print statements to your plugin
             to verify statement execution will help

             Other errors may cause X-Plane to simply terminate.

Code Examples
-------------

* :doc:`skeleton`.  Bare-bones skeleton.

* :doc:`helloworld`. Create and display a message in a window.

* :doc:`hellowidget`. Create and display widgets in a window.

* :doc:`samples`.   Ported versions of C++ and Python2 demos. Many of
  these are copied to your ``PythonPlugins/samples/`` folder on installation, but there may be more, and more recent demos
  on github. You can download the latest set, refreshing the ``samples`` folder, by selecting **Download Samples** from the
  XPPython3 menu.

.. toctree::
   :hidden:

   skeleton
   helloworld
   hellowidget
   samples

