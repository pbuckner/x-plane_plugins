Graphics
========

The X-Plane SDK provides a few ways of drawing:

* **Instance Drawing**: Place objects at a location and manipulate these objects through datarefs. See :doc:`/development/modules/instance`
  with the use of :py:func:`xp.createInstance`.

* **Direct Drawing (SDK only)**: Using a :py:func:`xp.registerDrawCallback` draw strings on the display. See :py:func:`xp.drawString`.

* **Direct Drawing (OpenGL)**: Using a :py:func:`xp.registerDrawCallback` draw whatever you like using OpenGL. X-Plane manipulates
  the graphics context so you can use OpenGL within the Vulkan/Metal environment supported by X-Plane. See :doc:`opengl` for examples.

* **Windows with Widgets**: Create a widget window and attach user-interaction widgets to it. See :doc:`/development/modules/widgets` and
  :doc:`/development/modules/standardwidgets`.
  
Additionally, XPPython3 has wrapped the `Dear IMGUI <https://github.com/ocornut/imgui>`_ library and OpenGL to provide:

* **Windows with ImGui Widgets**:
  Create a window and attach `Dear IMGUI <https://github.com/ocornut/imgui>`_ widgets to it. See :doc:`imgui`.
    
* **XPGL Graphics**:
  Draw shapes, fonts, images with a pure-python graphics drawing package. See :doc:`/development/modules/xpgl`.

For details on non-SDK drawing, see:

.. toctree::
   :maxdepth: 1

   opengl
   imgui
   /development/modules/xpgl
