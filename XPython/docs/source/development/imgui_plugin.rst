ImGui in Windows
================

Similar to using X-Plane Widgets, you'll make ImGui calls within an X-Plane window:

.. rst-class:: compact
   
* Create an ``xp_imgui.Window()``
* Add ImGui code to the window's draw callback
   
ImGui works within a window. To create an imgui-capable window, we provide::

  instance = xp_imgui.Window(...)

xp_imgui.Window() takes parameters similar to :py:func:`xp.createWindowEx`, but returns an instance rather
than a simple windowID. You can create a window in response to a command or menu selection in
your plugin code (just as you might create a non-ImGui window). (See class description :py:class:`xp_imgui.Window`.)

The biggest difference is that the ``draw()``
callback you provide with window creation will contain imgui calls. 

XPPython3 will set up the context prior to calling your draw callback, so you can be focussed
on just your implementation-specific code, which could be as simple as:

  >>> def drawWindow(windowID, refCon):
  ...     imgui.button("Click me")
  ...

This is because xp_imgui interface code handles imgui-specific setup and rendering for you, so you
don't have to::

  imgui.set_current_context()
  imgui.set_new_frame()
  imgui.begin()
  # -----
  #  your drawWindow() code is executed here
  # -----
  imgui.end()
  imgui.render()
  render.imgui.get_draw_data()

The underlying windowID can be found as an attribute of the returned instance. This allows you
to do things to the window such as :py:func:`xp.setWindowIsVisible`, :py:func:`xp.setWindowPositioningMode`
and other operations as described in :doc:`modules/display`.

>>> xp.setWindowPositioningMode(instance.windowID, 4)


OpenGL / Panel Graphics
-----------------------

Previously we executed ImGui within an OpenGL window (:data:`xp.WindowContentTypeOpenGL`).
With X-Plane 12.4.4 you can set ``contentType =`` :data:`xp.WindowContentTypePanelGraphics`. (That is, with 12.4.4, you can use either WindowContentType.)
With the former **OpenGL** setup, you're able to augment your ImGui drawing with graphics using OpenGL (:doc:`/development/modules/xpgl`).
With the new **Panel Graphics** option, you're able to augment your ImGui drawing with all the
functionality in :doc:`/development/modules/panelgraphics`. Panel Graphics
is also much faster as it bypasses X-Plane's OpenGL bridge layer and directly works with the GPU.

.. note:: By default, we use OpenGL to maintain backward compatibility, but **you should transition to PanelGraphics**: it
    is much, much faster at rendering even the basic ImGui calls. So use it even if you're not looking to do any
    custom graphics (OpenGL or PanelGraphics) in your display.

See ``samples/PI_ImguiBoth.py`` for a working example: it opens the same ImGui
drawing twice side by side, once with each ``contentType``, so the two render
paths can be compared directly. Note that translucent colors are *expected* to
differ between them --- panel graphics blends in linear space --- so a
low-alpha fill reads brighter on the right-hand window.

Imports
.......
To use imgui, you'll need to add two imports::

   import imgui
   from XPPython3 import xp_imgui

The first import provides ImGui drawing routines.

The second import provides an X-Plane compatible interface by creating a window into which you will be
able to draw.

Simple Example
--------------

You can copy & paste this directly into the :doc:`/development/debugger`:

>>> from XPPython3 import xp_imgui
>>> import imgui
>>> def drawWindow(windowID, refCon):
...     imgui.button(refCon)
...
>>> window = xp_imgui.Window(draw=drawWindow, refCon="Click Me!", contentType=xp.WindowContentTypePanelGraphics)
>>> xp.setWindowIsVisible(window.windowID)

.. image:: /images/imgui_click.png
            

Longer Example (PI_imgui.py)
----------------------------

This example is available under :doc:`samples`, and listed here simply for reference.

.. include:: PI_imgui.py
    :code:

The above is pretty basic: a menu item is created to call a command. Each time the command
is called, we'll create a new window and give that window the title "PI_imgui Window #<n>".

Each window's reference constant is a dictionary, which contains values for its widgets. In real
life, this might point to datarefs to set or other internal data.

Note that :code:`drawWindow` does the imgui work and everything else is nearly identical to
a non-imgui example. Also check out the ``PI_imguiBarometer.py`` example under :doc:`samples`.

Next, :doc:`imgui_avionics`
