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

Once you have a running system, we'll cover some basic code:

* **Skeleton**: the required minimum code to do absolutely nothing

* **Hello World**: a simple plugin with a popup window.
  
* **Hello Widget**: a simple plugin with a popup window using widgets.
  
Couple of quick points:

1. ``print()`` sends information to the XPPython3.log. This is great for quick debugging. Remove these
   prior to deploying your plugin.
2. :py:func:`xp.log` sends to the XPPython3.log, and automatically adds ``[<your plugin name>]`` to the
   text. This is better for "real" messages you want to keep in the python log that the end user might need to
   see to help you debug in the field.
3. :py:func:`xp.sys_log` sends to the main Log.txt, and automatically adds your plugin name. Use this
   sparingly (there's enough stuff in Log.txt already) but this is most useful for initial
   configuration problems, as the user is most likely to provide you with a Log.txt file
   and say, "it doesn't work."
4. While we provide a python interface, you have to call all functions with all the positional parameters::

     xp.destroyWindow(myWindowID)           # Works! Use positional parameters
     xp.destroyWindow(windowId=myWindowID)  # Fails! Keyword parameters not supported


.. toctree::
   :caption: Code Examples

   skeleton
   helloworld
   hellowidget

