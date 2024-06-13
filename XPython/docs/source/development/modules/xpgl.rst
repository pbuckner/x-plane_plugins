xpgl
====

.. py:module:: xpgl
               
X-Plane permits the use of OpenGL for drawing two dimensional objects. This can include drawing directly on the screen
(:py:func:`xp.registerDrawCallback`), within windows (:py:func:`xp.draw`), or within avionics windows (:py:func:`xp.screenDraw`).
X-Plane sets up the proper OpenGL context and you pass in a callback function to do the actually drawing.

However, OpenGL is a complicated API which is powerful, yet does not do simple things simply.

This module attempts to reduce the pain of OpenGL.

.. caution:: The module is a work-in-progress. While I do not expect the
             interface to change much, if you *require* stability, you should test and ship with your own
             copy of this module. Better yet, if you see an issue, let me know so we can all
             benefit!

To use::

  from XPPython3 import xpgl

The functions below are grouped:

* :doc:`xpgl_lines`:  single lines, sets of connected lines, different thicknesses and with stipple patterns.

* :doc:`xpgl_shapes`: lines configured as circles, rectangles and other polygons, filled or outlined only.

* :doc:`xpgl_images`: Textures, read from image files

* :doc:`xpgl_text`: Loading fonts and displaying text with different styles, sizes, and colors

* :doc:`xpgl_transformations`: Transform part of a drawing using rotation, translation, and scaling  

* :doc:`xpgl_mask`: Alter the drawing by showing or hiding part of it using a mask

Constants
---------

.. py:data:: Colors
             
   Colors is large dictionary of pre-defined colors, you may find these convenient to use, but there is nothing
   special about them.

   To use::

     >>> from XPPython3.xpgl import Colors
     >>> Colors['orange']
     (1, 0.5, 0.0)

.. py:class:: RGBColor
              
  Each color is three-element tuple with floating point values in range [0..1].
  
Additional Functions
--------------------

In additon to the line, shape, image, etc., drawing function grouped above, this module includes:

.. py:function:: clear() -> None

  Clears the screen                 

.. py:function:: report() -> None

  Print to stdout the current state of OpenGL
             
.. toctree::
   :hidden:

   /development/modules/xpgl_lines
   /development/modules/xpgl_shapes
   /development/modules/xpgl_images
   /development/modules/xpgl_text
   /development/modules/xpgl_transformations
   /development/modules/xpgl_mask
   /development/modules/xpgl_debugging
   
