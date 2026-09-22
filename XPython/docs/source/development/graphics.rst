Graphics
========

X-Plane includes lots of ways to draw text and images on the screen. Which technique you should
use depends on what you're trying to display and how your drawing should appear within the 3D
simulator environment.

Terms
-----

So we're clear on what is being covered, let's nail down some terminology:

* **Display Window**: This is a permanent or popup window, unattached to aircraft or scenery. Most
  display windows can, with a line or two of code, be made to "pop-out" into a regular OS window
  external to the main X-Plane window, allowing you to reposition them, possibly to a separate
  monitor. We say these are positioned in 2D space, as they float on the surface of your monitor, in
  front of aircraft and scenery.

* **Avionics Device Window**: This is a drawable screen, usually added to the cockpit: it is
  positioned in 3D space. You *may* be able to pop-up the screen by clicking on it (as with the
  G1000), making the 3D screen a floating 2D window. You may also be able to "pop-out" the window
  into an external OS window. Strictly speaking it doesn't have to be an "avionics device" -- it
  could be a web page. Avionics Devices are commonly associated with an electric bus so their
  display may be "on" or "off". Similarly, they can respond to changes in ambient lighting, perhaps
  darkening at night: this does not happen with Display Windows. Some avionics devices don't need a
  drawn screen, and consist merely of a set of objects (e.g., most steam gauges)

* **Widget**: These are user-interactive display elements: check-boxes, buttons, sliders, etc. which
  you might want to use: Widgets (generally) are pre-built standardized code. It's software: you
  *could* build your own user-interactive widget set, but these are already provided for
  you. Widgets are placed within windows.

  .. rst-class:: compact

  * X-Plane SDK includes a simple old-style set of widgets (:doc:`modules/widgets`, and :doc:`modules/standardwidgets`).

  * Dear ImGui is another, more flexible set of widgets (:doc:`imgui`).

* **Object**: These are 3D "things" usually stored in (X-Plane-specific) ``*.obj`` files, which describe a
  three-dimensional object wrapped with a texture. You'll create instances of these objects and then
  place them into 3D space as scenery or (parts of) aircraft. Object instances can be easily moved,
  rotated, resized, can have lights and emitters (e.g., sparks, smoke). Draw a tree? Make an object
  & place it in 3D space. Draw a rotating knob? Make an object & attach it to the aircraft.

Most aircraft are made up hundreds or thousands of objects created "off-line" using Blender or other
3D drawing tool. Similarly, scenery packages include objects and overlays with rules regarding
when and how to display them. Both aircraft modeling and scenery are outside the scope of the
SDK, and developers should consult Laminar `Plane Maker <https://developer.x-plane.com/docs/aircraft/>`_
and `WorldEditor (WED) <https://developer.x-plane.com/tools/worldeditor/>`_ documentation.


Drawing Technologies
--------------------

There are many drawing technologies available within X-Plane, with differing complexity and restrictions:

* **Basic SDK**: Using a :py:func:`xp.registerDrawCallback`, draw strings directly on the
  display in 2D space. *Very limited*.  See :py:func:`xp.drawString`.

* **Instance Drawing**: As mentioned above, place pre-defined objects at a location in 3D space and
  possibly manipulate these objects through datarefs. See :doc:`/development/modules/instance`
  with the use of :py:func:`xp.createInstance`.

* **OpenGL** `(opengl.org) <https://opengl.org>`_ An ancient (1992) drawing technology useful for
  drawing text, 2D and 3D objects.  It includes primitives for drawing lines & polygons, and supports
  wrapping meshes with textures. While it has been succeeded by Vulkan `(vulkan.org)
  <https://www.vulkan.org>`_, OpenGL can be used by X-Plane plugins via an elaborate bridge
  layer (`Zink <https://docs/mesa3d.org/drivers/zink.html>`_).  OpenGL is difficult to learn and not
  recommended any more. For convenience, XPPython3 provides a higher-level drawing library called
  :doc:`modules/xpgl` which make OpenGL easier, but the drawing still goes through the bridge.

  OpenGL drawing within X-Plane is restricted for use within either a Window or Avionics Device,
  or using :func:`xp.registerDrawCallback` ("Direct Drawing") and drawing within a limited set of drawing phase layers.

  For convenience, XPPython3 provides the :doc:`modules/xpgl` module which makes OpenGL a bit easier
  to use. See also :doc:`opengl`.

* **PanelGraphics**: (:doc:`XPLMPanelGraphics module <modules/panelgraphics>`) An X-Plane (12.4.4+)
  set of primitives written to provide much of the functionality of OpenGL, but without the need to
  go through the bridge layer. Think of it as the same features, much faster. Plus it provides
  higher-level (i.e., easier-to-use) interfaces negating the need for :doc:`modules/xpgl`.
  In general, if you were
  thinking about OpenGL, you should now try Panel Graphics. Only available X-Plane 12.4.4+.

  Not restricted for use only in *avionics panels*, you can also use Panel Graphics to draw either
  Avionics Devices or Display Windows. It cannot be used at the same time as OpenGL, nor can
  it be called from :func:`xp.registerDrawCallback`.
  
* **Browser**: X-Plane includes `CEF <https://chromiumembedded.github.io/cef>`_ embedded web browser. This
  allows you to set URL to local (i.e., ``file:///``) locations or anywhere on the web. Inject scripts into
  the running browser, and define javascript functions allowing callback from JS into X-Plane. See
  :doc:`modules/display_window_browser`.

Python also provides:

* **Python PIL** (`PIL module <https://pillow.readthedocs.io/en/stable/>`_) A common python module
  useful for drawing and manipulating 2D images: This makes drawing *textures* easy: you still need
  to use Panel Graphics or OpenGL, or reference the texture in an Object in order for the texture to
  be displayed. That is, PIL won't draw to the screen, but can create buffers which can be used
  as textures.

As an over-simplification:

* If you just want to display a string, use **Basic SDK**.

* If you want to display an object in 3D space, use **Instance Drawing**.

* If you want do draw images in 2D space (circles, load PNG files, better text handling)
  use **PanelGraphics** unless you need to support pre-12.4.4, in
  which case you have to go with **OpenGL**. Draw this within a Display Window.

  If it's an avionics panel and would benefit from interaction with electrical bus power
  and ambient lighting, draw with an Avionics Device Window.

* If you have lots of pages to display or need to reference the internet use **Browser**.

* If you need widgets to interact with the user:

  * Use Dear ImGui with **PanelGraphics** within a Display Window unless you need to support
    pre-12.4.4, in which case use Dear ImGui with **OpenGL**.
    Alternatively you could use **Browser** with HTML/JavaScript widgets (12.4.4.+).

  * Use **X-Plane SDK Widgets** for simpler interactions and support since X-Plane 11.

For details on non-SDK drawing, see:

.. toctree::
   :maxdepth: 1

   opengl
   imgui
