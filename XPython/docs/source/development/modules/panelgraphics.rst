XPLMPanelGraphics
=================

.. py:module:: XPLMPanelGraphics
.. py:currentmodule:: xp

To use::

  import xp

This is X-Plane's **native** 2-D panel-graphics primitive drawing API.
It supports lines, polygons, text, textures, and higher-level avionics displays
(synthetic vision and moving-map), drawn directly by the simulator rather than
through OpenGL. Because X-Plane renders these itself, the calls work correctly on
all backends (OpenGL, Vulkan, Metal) --- unlike the older OpenGL drawing.

.. caution:: These APIs are available only when running under X-Plane 12.4.4+.
             XPPython3 loads them dynamically; calling them on an older sim
             raises a ``RuntimeError``.

Where you can draw
------------------

Panel-graphics calls must be made from within a **panel-graphics drawing
context**, otherwise X-Plane raises a *"Panel graphics violation"* error. A
panel-graphics context is provided by setting window content type to ``WindowContentTypePanelGraphics``:

* **window** created with :func:`createWindowEx` --- draw from its
  :func:`draw` callback; or
* **custom avionics** device created with :func:`createAvionicsEx`  --- draw from its
  :func:`screenDraw` / :func:`bezelDraw` callback.
* **built-in avionics** device registered with :func:`registerAvionicsCallbacksEx`  --- draw from its
  :func:`screenDraw` / :func:`bezelDraw` callback.

Relationship to other drawing modules
--------------------------------------

* :doc:`xpgl` is a *pure-Python* convenience layer over **OpenGL**. It predates
  this module and is unrelated to it --- it draws with legacy OpenGL, not the
  native panel-graphics calls documented here. Can be used only with ``OpenGL`` WindowContentType.
* :doc:`graphics` (``XPLMGraphics``) provides the old, limited ``drawString`` /
  ``measureString`` and coordinate conversion. Can be used only with ``OpenGL`` WindowContentType.
* :doc:`display_window` and :doc:`display_avionics` create the windows / devices
  whose callbacks give you a panel-graphics context. These PanelGraphics primitives are used
  to draw *within* windows and device panels.

Drawing state
-------------

Panel graphics functions modify a shared drawing state that includes a transformation matrix, a scissor (clip) rectangle, and
a stencil mask. Each of these has a push/pop stack so you can save and restore state around localized drawing operations.
The line cap (:func:`setLineCap`) is also part of this shared state, but has no stack: set it, and it stays
set for all subsequent line drawing until you change it or the callback ends.

Primitives are drawn in the order you submit them. Later drawing calls paint over earlier ones. Use the retained-drawing API to record
a sequence of draw calls once and replay it efficiently on subsequent frames. Commands are buffered and
rendered by X-Plane at the end of your callback; you do not manage OpenGL state directly.

Drawing coordinate system has X increasing to the right and Y increasing upwards. *However*, for Windows, (0,0) vertex
is lower left of the global coordinate system *not* lower left of the window. For Avionics, (0, 0) vertex is the
lower left of the device window.

.. note:: Though you can use panel graphics in both Windows and Avionics devices, the change
          of origin (0, 0) between the two coordinate systems may cause confusion. Translation
          and scaling of your avionics device is automatically handled by X-Plane. You would
          need to provide your own translation and scaling for Windows, especially if you want
          to use :func:`drawRetained`: See that function for an example transformation for Window drawing.

**Do not** intermix raw OpenGL (or :doc:`xpgl`) with these Panel Graphics calls.

Colors
------

Most drawing routines take a single packed integer *color*, built with
:func:`makeColor`. Per-vertex-color variants (the ``c`` suffix, e.g.
:func:`linesc`) take that packed color as the third element of each vertex tuple.

.. py:function:: makeColor(red=1, green=1, blue=1, alpha=1) -> int

    :param float red: Red component, [0.0..1.0]
    :param float green: Green component, [0.0..1.0]
    :param float blue: Blue component, [0.0..1.0]
    :param float alpha: Alpha component, [0.0..1.0]
    :return: Packed color value for use with the panel-graphics routines

    Pack four float color components into a single packed color value (ABGR) for
    use with the panel-graphics routines. Without parameters, ``makeColor()`` returns opaque white.

    >>> red = xp.makeColor(1.0, 0.0, 0.0, 1.0)
    >>> red
    4278190335
    >>> xp.makeColor(1000.0, -55.0, 0.0, 1.0)
    4278190335
    
    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMMakeColor>`__ :index:`XPLMMakeColor`

Function groups
---------------

.. toctree::
   :maxdepth: 2

   /development/modules/panelgraphics_primitives
   /development/modules/panelgraphics_state
   /development/modules/panelgraphics_retained
   /development/modules/panelgraphics_fonts
   /development/modules/panelgraphics_textures
   /development/modules/panelgraphics_svt
   /development/modules/panelgraphics_map
   /development/modules/panelgraphics_touch
