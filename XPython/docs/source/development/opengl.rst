Using OpenGL
============

OpenGL is a graphics library supported on Mac, Windows and Linux. Though X-Plane has moved to use
Vulkan / Metal as its base graphics library, OpenGL is still support for use by plugins.

To use OpenGL with python, you use the ``pyopengl`` module, which provides an interface between python
and the system's OpenGL library. Because this module is not automatically
installed by python, plugin developers should include a check with a helpful error message, perhaps
pointing to this page to help users.

Installing OpenGL
-----------------

OpenGL is already on your computer, what you really want to do is install **pyopengl** python module.

Installing pyopengl
-------------------

You'll need to install pyopengl into the version of python being used by XPPython3. The easiest way to do this is use
the "Pip Package Installer" included with XPPython3. Select that menu item while running X-Plane.
See `Installation - Pip Package Installer  <../usage/installation_plugin.html#running-the-first-time>`_

You will need to install the ``pyopengl`` package.

Verifying Installation
----------------------

.. image:: /images/opengl.gif
   :align: right        

Assuming pyopengl is installed correctly, you can quickly verify success by running the simple plugin ``PI_OpenGL.py`` which
is found under ``PythonPlugins/samples/`` directory. Copy the file up to ``PythonPlugins/`` and restart X-Plane. On success,
you'll see an animated window:

If you don't see the window immediately, check the logs.

OpenGL on Macs
++++++++++++++

While OpenGL installation is easy on most platforms, there is a bug in OpenGL on recent Macintosh macOS. This
is described in detail in the PI_OpenGL.py plugin, if you're interested.

Running the PI_OpenGL.py plugin *attempts* to fix this problem, but if that fails, it will complain in the
logs, and you (the user of the plugin, not the developer of the plugin) will have to intervene.

See comments in that plugin for details. This only needs to be done once.

Programming OpenGL
------------------

I'm not an OpenGL expert, so you're on your own. The key is you will draw within a window's DrawCallback (the "window"
may be transparent and does not require decorations). Then, each call you'll::

  import OpenGL.GL as GL

  def drawWindowCallback(...):
      xp.setGraphicsState(...)  # So it's in a known state
      GL.glBegin(...)
      do_stuff()
      GL.glEnd()

Developer Hints
---------------

Because it is very likely users of your plugin have not installed the pyopengl module, it's a
good idea to verify you import, doing something like::

  try:
      import OpenGL
  except ModuleNotFoundError as e:
      xp.log("Fail: Cannot import OpenGL: {}. ".format(e))
      xp.log("See https://xppython3.readthedocs.io/en/latest/development/opengl.html")
      return 0

This way, when *your* plugin fails, you can guide the user to the correct next step.      

