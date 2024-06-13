xpgl Shapes
===========

.. py:module:: xpgl
   :noindex:

To use::

  from XPPython3 import xpgl

Every geometric shape is a polygon: a set of vertices which describe a set of connecting, (ideally) non-crossing lines. This polygon
may be *filled* with a solid color. If not filled, it will be an outline, where each line-segment follows the rules
of lines: they have thickness, color, and stipple pattern. Solid objects do not have a separate border.

.. py:function:: drawTriangle(x1, y1, x2, y2, x3, y3, color=Colors['white']) -> None:

  :param float x1: 
  :param float y1: Point 1
  :param float x2:
  :param float y2: Point 2
  :param float x3:
  :param float y3: Point 3
  :param RGBColor color: OpenGL color. Tuple of floats (r, g, b) ranging [0..1]
  :return: None                         

  Draws a *color-filled* triangle with the specified three vertices.::

    xpgl.drawTriangle(0, 0, screen.width / 2, screen.height / 2, screen.width, 0)
    xpgl.drawTriangle(screen.width / 2, screen.height / 2, 400, 400, 50, 400, color=Colors['orange'])
    xpgl.drawTriangle(500, 300, 500, 400, 550, 290, color=Colors['red'])

  .. image:: /images/xpgl_drawTriangle.png
             :width: 50%
                     
.. py:function:: drawRectangle(x, y, width, height, color=Colors['white']) -> None:

  :param float x: 
  :param float y: Origin of rectangle (lower-left corner)
  :param float width: horizontal size of rectangle
  :param float height: vertical size of rectangle
  :param RGBColor color: OpenGL color. Tuple of floats (r, g, b) ranging [0..1]
  :return: None                         

  Draws a *color-filled* rectangle of specified width and height, with origin at (x, y). Negative width
  and height work as expected.::

    xpgl.drawRectangle(1, 1, screen.width / 2, screen.height / 2)
    xpgl.drawRectangle(screen.width / 2, screen.height / 2, -100, -50, color=Colors['orange'])
    xpgl.drawRectangle(500, 300, 20, 20, color=Colors['red'])

  .. image:: /images/xpgl_drawRectangle.png
             :width: 50%

.. py:function:: drawFrame(x, y, width, height, thickness=1., color=Colors['white']) -> None:

  :param float x: 
  :param float y: Origin of rectangle (lower-left corner)
  :param float width: horizontal size of rectangle
  :param float height: vertical size of rectangle
  :param float thickness: width of line
  :param RGBColor color: OpenGL color. Tuple of floats (r, g, b) ranging [0..1]
  :return: None                         

  Draws an *outline* of a rectangle of specified width and height, with origin at (x, y). Negative width
  and height work as expected. ::

    xpgl.drawFrame(1, 1, screen.width / 2, screen.height / 2, thickness=10)
    xpgl.drawFrame(1, 1, screen.width / 2, screen.height / 2, thickness=1, color=Colors['red'] )
    xpgl.setLinePattern(0x5555)
    xpgl.drawFrame(300, 300, 100, 100, thickness=4, color=Colors['cyan'])
    xpgl.setLinePattern()

  Note in the image below how the thickness of the line interacts with the frame's origin. Also,
  line thickness *does not* "round-out" end caps on the lines.

  .. image:: /images/xpgl_drawFrame.png
             :width: 50%

.. py:function:: drawPolygon(points, isFilled=True, thickness=1., color=Colors['white']) -> None:

  :param list points: List or tuple of pairs of floats (x, y) for vertices
  :param bool isFilled: True, if should be a filled-in polyon. Outline otherwise
  :param float thickness: Width of line (ignored if *isFilled*)
  :param RGBColor color: OpenGL color. Tuple of floats (r, g, b) ranging [0..1]
  :return: None                         

  Draws a polygon following specified vertices. If last vertex does not match the first
  vertex, we'll automatically add it, closing the polygon. Polygon may be clockwise or counter-clockwise
  (if you don't know what that means, don't worry about it.) Polygon may be convex or concave.

  Polygons which have crossing lines may yield surprising results, as with the yellow shape in the following
  example::

    shape1 = [(150, 100), (100, 350), (200, 300), (175, 150), (400, 70)]
    xpgl.drawPolygon(shape1)
    xpgl.drawPolygon(shape1, isFilled=False, thickness=3, color=Colors['magenta'])
    
    shape2 = [(300, 300), (300, 450), (450, 450), (250, 350)]
    xpgl.drawPolygon(shape2, color=Colors['yellow'])
    xpgl.drawPolygon(shape2, isFilled=False, thickness=3, color=Colors['red'])

  .. image:: /images/xpgl_drawPolygon.png
             :width: 50%

.. py:function:: drawCircle(x, y, radius, isFilled=False, thickness=1., num_vertices=36, color=Colors['white']) -> None
                 
  :param float x:
  :param float y: (x, y) location of *center* of the circle
  :param bool isFilled: if True, fill the circle with given color. Otherwise draw outline only.
  :param float thickness: Width of line (ignored if *isFilled*)
  :param int num_vertices: Optional parameter, allowing you to change the # of vertices used to draw the circle.
  :param RGBColor color: OpenGL color. Tuple of floats (r, g, b) ranging [0..1]
  :return: None

  Two circles::

    xpgl.setLinePattern(0x5555)
    xpgl.drawCircle(250, 300, 150, thickness=5, color=Colors['orange'])
    xpgl.setLinePattern()
    xpgl.drawCircle(450, 300, 50, thickness=5, num_vertices=3, color=Colors['red'])
    xpgl.drawCircle(200, 200, 50, isFilled=True, color=Colors['green'])

  .. image:: /images/xpgl_drawCircle.png
     :width: 50%
             
  Note, because a circle is actually drawn using many small line segments, the stipple pattern works, but
  is not particularly smooth.
  
.. py:function:: drawArcLine(x, y, radius, start_angle, arc_angle, num_vertices=36, color=Colors['white']) -> None

  :param float x:
  :param float y: (x, y) location of *center* of the arc (center of circle containing arc)
  :param float radius: Radius of circle containing arc.
  :param float start_angle: degrees at which to start drawing angle (0 is 3 o'clock, 90 is 12 o'clock)
  :param float arc_angle: number of degrees to draw the arc (*not* ending angle)
  :param float thickness: Width of line
  :param int num_vertices: Optional parameter, allowing you to change the # of vertices used to draw the circle containing the arc.
  :param RGBColor color: OpenGL color. Tuple of floats (r, g, b) ranging [0..1]
  :return: None

  Draws a line, a portion of a circle outline. Zero degrees for *start_angle* is at 3 o'clock, relative the (x, y) center location.
  The arc is drawn *counter-clockwise* for positive *arc_angle*. Clockwise for negative *arc_angle*.::

    xpgl.setLinePattern(0x5555)
    xpgl.drawArcLine(250, 300, 150, start_angle=90, arc_angle=180, thickness=5, color=Colors['orange'])
    xpgl.setLinePattern()
    xpgl.drawArcLine(450, 300, 50, start_angle=45, arc_angle=-90, thickness=2, color=Colors['red'])
    xpgl.drawArcLine(300, 200, 100, start_angle=270, arc_angle=120, thickness=4, color=Colors['green'])

  .. image:: /images/xpgl_drawArcLine.png
            :width: 50%

.. py:function:: drawArc(x, y, radius_inner, radius_outer, start_angle, arc_angle, num_vertices=36, colorColors['white']) -> None

  :param float x:
  :param float y: (x, y) location of *center* of the arc (center of circle containing arc)
  :param float radius_inner:
  :param float radius_outer: Inner and outer radius of the arc.   
  :param float start_angle: degrees at which to start drawing angle (0 is 3 o'clock, 90 is 12 o'clock)
  :param float arc_angle: number of degrees to draw the arc (*not* ending angle)
  :param int num_vertices: Optional parameter, allowing you to change the # of vertices used to draw the circle containing the arc.
  :param RGBColor color: OpenGL color. Tuple of floats (r, g, b) ranging [0..1]
  :return: None

  Similar to :py:func:`drawArcLine`, this function draws a solid (e.g., filled) arc with a minumum and maximum radius.::
  
    xpgl.drawArc(250, 300, 100, 150, start_angle=90, arc_angle=180, color=Colors['orange'])
    xpgl.drawArc(450, 300, 15, 50, start_angle=45, arc_angle=90, color=Colors['red'])
    xpgl.drawArc(300, 200, 0, 100, start_angle=270, arc_angle=120, color=Colors['green'])

  .. image:: /images/xpgl_drawArc.png
             :width: 50%

