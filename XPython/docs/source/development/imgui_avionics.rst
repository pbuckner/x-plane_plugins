ImGui in Avionics
=================

ImGui is not limited to floating windows: you can draw it onto the screen of a
cockpit device created with :py:func:`xp.createAvionicsEx`. There are two ways to
do it, and they solve different problems:

.. rst-class:: compact

* :ref:`imgui-avionics-device` --- a full ImGui context on the device screen,
  with mouse, scroll and keyboard wired up. Interactive.
* :ref:`imgui-avionics-drawcalls` --- hand the panel-graphics API a mesh you built
  yourself. Not interactive, but no ImGui context is involved at all.

.. note:: Both require ``contentType =`` :data:`xp.WindowContentTypePanelGraphics`
          (X-Plane 12.4.4, SDK440). A device has no window, so the OpenGL
          renderer described in :doc:`imgui_plugin` cannot be used here.

.. _imgui-avionics-device:

Using xp_imgui.AvionicsDevice
-----------------------------

:py:class:`xp_imgui.AvionicsDevice` is the device counterpart of
:py:class:`xp_imgui.Window`. Both share the same ImGui context handling, frame
loop and keyboard translation, so your draw callback looks the same as it does
for a window --- only the creation call and the callback's first parameter
differ::

   from XPPython3 import xp_imgui
   import imgui

   def drawScreen(avionicsID, refCon):
       imgui.text("Hello from a device screen")
       if imgui.button("Press Me"):
           refCon['presses'] += 1

   device = xp_imgui.AvionicsDevice(screenWidth=400, screenHeight=300,
                                    draw=drawScreen, refCon={'presses': 0},
                                    deviceID='myplugin.pfd')
   device.popup(True)        # show the 2-D pop-up

When you're finished, ``device.delete()`` tears down the ImGui context, the
renderer and the device itself --- use it *instead of*
:py:func:`xp.destroyAvionics`.

Where your ImGui appears
........................

On the device **screen**, which means everywhere the device is drawn: in the 2-D
pop-up, and on any 3-D object bound to the device with
:py:func:`xp.setObjectAvionics`.

The **bezel** is not ImGui. It is drawn by your own ``bezelDraw`` callback using
plain :doc:`modules/panelgraphics` calls, and only while the pop-up is visible.

Differences from a window
.........................

The drawing half is identical; the surrounding behavior is not:

.. rst-class:: compact

* Your draw callback receives ``(avionicsID, refCon)`` rather than
  ``(windowID, refCon)``.
* Screen coordinates are device-local, so no window-geometry or boxel/native
  transformation is involved.
* Keyboard focus applies only to the **pop-up**: typing is not routed to a
  device drawn on a 3-D object or in the panel. ``requestInputFocus(True)``
  calls :py:func:`xp.takeAvionicsKeyboardFocus`; there is no avionics
  equivalent of releasing focus, so ``requestInputFocus(False)`` does nothing
  and X-Plane drops focus on its own.
* ``popup()`` / ``isPopupVisible()`` replace window visibility and positioning.

.. warning:: Dragging behaves differently from a window. On avionics devices
             ``MouseUp`` is reported at (0, 0) and ``MouseDrag`` is not
             delivered at all, so a drag cannot be tracked the way it can in a
             window. Reported to Laminar as
             `XPD-18371 <https://developer.x-plane.com/x-plane-bug-database/?issue=XPD-18371>`_.

See ``samples/PI_ImguiAvionics.py`` for a working example, and
``samples/PI_ImguiBoth.py`` for the same ImGui drawing in two windows, one per
``contentType``.

.. _imgui-avionics-drawcalls:

Using drawCalls directly
------------------------

If you do not need interaction, you can skip the ImGui context entirely and
hand :py:func:`xp.drawCalls` a mesh of your own. The API is shaped to match
ImGui's ``ImDrawData`` --- vertices, indices and a list of draw calls, each able
to rebind a texture and a scissor rectangle --- but nothing requires the data to
have come from ImGui. Building the texture with PIL and the geometry yourself is
often simpler for static artwork.

This is described in full, with examples, under
:ref:`PIL-style draw calls <pg-pil-draw-calls>` in
:doc:`modules/panelgraphics_textures`. See also ``samples/PI_PGDrawCalls.py``.

Choosing between them
---------------------

Use :py:class:`xp_imgui.AvionicsDevice` when you want buttons, sliders, text
entry --- anything the user interacts with. Use :py:func:`xp.drawCalls` when you
are drawing a fixed image and would rather not carry an ImGui context, or when
you want to retain the result with :doc:`modules/panelgraphics_retained`.

Next, :doc:`imgui_coding`
