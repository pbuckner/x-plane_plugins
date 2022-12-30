Using OpenGL
============

OpenGL is a graphics library supported on Mac, Windows and Linux. Though X-Plane has moved to use
Vulkan / Metal as its base graphics library, OpenGL is still support for use by plugins.

To use OpenGL with python, you use the ``OpenGL`` python module, which provides an interface between python
and the system's OpenGL library. Because this module is not automatically
installed by python, plugin developers should include a check with a helpful error message, perhaps
pointing to this page to help users. (Suggestion see :ref:`DeveloperHints` below.)

Installing OpenGL
-----------------

OpenGL is already on your computer, what you really want to do is install **pyopengl** python package. ("OpenGL" is the python
module included with the "pyopengl" package.)

Installing pyopengl package
---------------------------

You'll need to install ``pyopengl`` into the version of python being used by XPPython3. The easiest way to do this is use
the "Pip Package Installer" included with XPPython3. Select that menu item while running X-Plane.
See :doc:`/usage/pip`.

You will need to install the ``pyopengl`` package.

Verifying Installation
----------------------

.. image:: /images/opengl.gif
   :align: right        

Assuming pyopengl is installed correctly, you can quickly verify success by running the simple plugin ``PI_OpenGL.py`` which
is found under ``PythonPlugins/samples/`` directory. Copy the file up to ``PythonPlugins/`` and restart X-Plane. On success,
you'll see an animated window:

If you don't see the window immediately, check the logs.

.. code-block:: none

  <X-Plane>/
  └─── Resources/
       └─── plugins/
            ├─── XPPython3/
            └─── PythonPlugins/
                 ├─── PI_OpenGL.py  ❮══════╗
                 └─── samples/             ║
                      └─── PI_OpenGL.py ❯══╝

.. Note::

  Older versions of python on Macs had a bug which failed to locate the OpenGL libraries. The best solution is to upgrade python
  to at least version 3.11.0.

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

.. _DeveloperHints:
  
Developer Hints
---------------

Because it is very likely users of your plugin have not installed the pyopengl module, it's a
good idea to verify your import, doing something like::

  try:
      import OpenGL
  except ModuleNotFoundError as e:
      xp.log(f"Fail: Cannot import OpenGL: {e}. "
      xp.log("Install 'pyopengl' package")
      xp.log("See https://xppython3.readthedocs.io/en/latest/development/opengl.html")
      raise

This way, when *your* plugin fails, you can guide the user to the correct next step.      

