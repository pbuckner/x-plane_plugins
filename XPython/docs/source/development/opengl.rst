Using OpenGL
============

OpenGL is a graphics library supported on Mac, Windows and Linux. Though X-Plane has moved to use
Vulkan / Metal as its base graphics library, OpenGL is still supported for use by plugins.

To use OpenGL with python, you use the ``OpenGL`` python module, which provides an interface between python
and the system's OpenGL library. This module is automatically installed with XPPython3 v4.3+.


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

  Older versions of python on Macs had a bug which failed to locate the OpenGL libraries. Because
  XPPython3 now includes its own version of Python and OpenGL, this should always work.

Programming OpenGL
------------------

If you're an OpenGL expert, have at it. The key is you will draw within a window's DrawCallback (the "window"
may be transparent and does not require decorations). Then, each call you'll::

  import OpenGL.GL as GL

  def drawWindowCallback(...):
      xp.setGraphicsState(...)  # So it's in a known state
      GL.glBegin(...)
      do_stuff()
      GL.glEnd()

For the rest of us, you can use :py:mod:`xpgl` routines to do most anything you want. This module us pure-python
so you can see what OpenGL calls are actually being made if you're curious.
