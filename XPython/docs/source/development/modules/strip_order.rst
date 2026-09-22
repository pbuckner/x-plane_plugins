Triangle Strip Ordering
=======================

Most people are comfortable with describing shapes dot-to-dot, drawing lines
following an outline until a the shape is formed:

.. image:: /images/strip_rectangle.png
           :width: 100px

For performance reasons, some drawing routines use a triangle-strip ordering.

The idea is that one creates triangle-after-triangle to fill the space:

.. image:: /images/strip_triangle_pair.png
           :width: 200px

You'll still specify four vertices, but each triangle (after the first) reuses
one new vertex plus the previous two vertices. So the order becomes::

  [ (lower-left), (upper-left), (lower-right), (upper-right) ]


This can repeat, possibly curving up and down, tapering, etc.:

.. image:: /images/strip_long.png
           :width: 300px
  
For more complex shapes, you may just break it into multiple calls. For example,
a five pointed star can be thought of as an inner pentagon, surrounded by five
triangles, each representing an arm.

The triangles are simple with just three
vertices each; the pentagon needs some extra thought. Start at a point, go
to it's adjacent clockwise point, the it's (the first point's) adjacent counter-clockwise
point. Continue clockwise, followed by counter-clockwise bouncing back and forth
until fully drawn.:

.. image:: /images/strip_pentagon.png
           :width: 200px

Visualize the pentagon's triangles
