ImGui Advanced
--------------

All of the xp / imgui interface is implemented in python code in the provided ``XPPython3/xp_imgui`` module. If
you're curious how this works, check out files under XPPython3.

The module is split into *hosts* --- which decide where the ImGui frame is
displayed --- and *renderers* --- which decide how it is drawn:

 * **xp_imgui/host.py**: ``ImguiHost``, everything that does not depend on where
   the frame appears: the imgui context, style / io / key-map setup, the
   per-frame ``new_frame`` / ``render`` loop, keyboard translation and teardown.
   Both hosts below share it, which is why your draw callback is the same in
   either.
 * **xp_imgui/window.py**: ``Window``, which hosts ImGui in a window created
   with :func:`xp.createWindowEx`, and provides the window manipulation code.
 * **xp_imgui/avionics.py**: ``AvionicsDevice``, which hosts ImGui on the screen
   of a device created with :func:`xp.createAvionicsEx` (see
   :doc:`imgui_avionics`).

The host picks one of two renderers from the window's ``contentType``:

 * **xp_imgui/xprenderer.py**: ``XPRenderer``, the interface between X-Plane's
   OpenGL and the imgui renderer. Used for
   :data:`xp.WindowContentTypeOpenGL`.
 * **xp_imgui/pgrenderer.py**: ``XPPanelGraphicsRenderer``, the XPLM440
   counterpart. Instead of issuing OpenGL calls it hands each ImGui draw list to
   :func:`xp.drawCalls`, which X-Plane renders natively. Used for
   :data:`xp.WindowContentTypePanelGraphics`, and the only renderer available to
   an avionics device.

``XPPanelGraphicsRenderer`` is markedly shorter than ``XPRenderer`` because
:func:`xp.drawCalls` consumes ImGui's own ``ImDrawData`` layout and coordinate
space directly: positions and scissor rectangles are window-local pixels with a
top-left origin, which is exactly ImGui's convention. The
modelview / projection / boxel / native conversion chain the OpenGL path needs is
therefore not used at all. It also touches no OpenGL, so a panel-graphics ImGui
window does not need PyOpenGL.

 * **xp_imgui/profiling.py**: opt-in per-frame timing shared by both renderers,
   off by default. Set ``PROFILE = True`` at the top of either renderer to
   enable it for that path. It measures CPU time spent in the renderer only ---
   it says nothing about GPU cost, so a path that submits quickly may still be
   the slower one on the card.

`You should not have to modify any of these files!` They are provided by XPPython3 and
are automatically updated each time XPPython3 is updated (and your changes will be lost).
If you see the need to make a modification, please contact me (support [at] xppython3.org) so
I can make the improvement available to everyone.
