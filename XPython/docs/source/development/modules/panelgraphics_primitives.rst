Primitives
==========

.. py:module:: XPLMPanelGraphics
    :no-index:
.. py:currentmodule:: xp

To use::

  import xp

Geometry primitives for the native panel-graphics API (new with SDK 440). These
can only be used in Windows or Avionics Devices which are created to support
:data:`WindowContentTypePanelGraphics`. (See :doc:`panelgraphics`)

Vertices are ``(x, y)`` tuples in panel pixels. The *color* argument is a packed
integer from :func:`makeColor`. The per-vertex-color variants (``c`` suffix) take
no *color* argument; instead each vertex is a ``(x, y, color)`` tuple.

Vertices may be wound clockwise or counter-clockwise: there is no culling of faces.

"Primitives" here refer to basic drawing routines:

.. rst-class:: compact
               
* :ref:`pg-line-caps`
* :ref:`pg-lines`
* :ref:`pg-filled-shapes`
* :ref:`pg-stipples`
* :ref:`pg-vertex-color`  

More complex drawing routines are on subsequent pages.

See ``samples/PI_PGPrimitives.py`` for a working example: it draws a labelled
grid with one cell per primitive, so you can compare them side by side.

For examples on this page, we'll create a window whose :func:`draw` callback calls
whatever function is defined by it's ``refCon``.

Example Drawing Window, window is set to 200x100:
  
  >>> refCon = {'draw_f': lambda :None}  # initially the draw function does nothing
  >>> def myDraw(windowID, refCon):
  ...     left, top, right, bottom = xp.getWindowGeometry(windowID)
  ...     with xp.transformContext():
  ...         xp.transformTranslate(left, bottom)
  ...         xp.transformScale((right-left) / 200., (top-bottom) / 100.)
  ...         refCon['draw_f']()
  ...
  >>> winID = xp.createWindowEx(draw=myDraw, visible=1,
  ...                           left=100, top=400, right=300, bottom=300,
  ...                           contentType=xp.WindowContentTypePanelGraphics, refCon=refCon)
  ...


Example Drawing Avionics, window is set to 200x100, we include a black bezel background::

  >>> refCon = {'draw_f': lambda :None}  # initially the draw function does nothing
  >>> def myScreenDraw(refCon):
  ...     refCon['draw_f']()
  ...
  >>> def myBezelDraw(r, g, b, refCon):
  ...     xp.polygon(xp.makeColor(0, 0, 0, 1), [(0, 0), (200, 0), (200, 100), (0, 100)])
  ...
  >>> avionicsID = xp.createAvionicsEx(screenDraw=myScreenDraw, bezelDraw=myBezelDraw,
  ...                                  screenWidth=200, screenHeight=100, bezelWidth=200, bezelHeight=100,
  ...                                  screenOffsetX=0, screenOffsetY=0,
  ...                                  contentType=xp.WindowContentTypePanelGraphics, refCon=refCon)
  ...
  >>> xp.setAvionicsGeometry(avionicsID, left=400, top=400, right=600, bottom=300)
  >>> xp.setAvionicsPopupVisible(avionicsID)

.. _pg-line-caps:

Line caps
---------

Line caps control how the two free *ends* of a line are drawn. The cap is part of
the shared drawing state: it applies to every line-drawing call that follows it
--- :func:`lines`, :func:`lineStrip` and their ``WithWidth``,
``Stipple`` and per-vertex-color (``c``) variants --- until you change it again.
At the start of each drawing callback the cap is :data:`LineCapButt`.

:data:`LineCapRound` and :data:`LineCapSquare` extend the line *past* its
endpoint by half the line width; :data:`LineCapButt` stops flush at the endpoint.
There is no way to read the current setting back.

Intermediate vertices in a line strip, and all vertices in a line loop are "joined" using
a rounded cap: there is no way to change this. For stipples, *each* line segment is drawn
with the currently active line cap.

.. image:: /images/lineCap.png
           :width: 300px
           :align: center
                   
.. py:function:: setLineCap(lineCap=LineCapButt) -> None

    :param int lineCap: One of :data:`LineCapButt`, :data:`LineCapRound`, :data:`LineCapSquare`

    Set how subsequent lines are capped at their start and end points. Called
    without a parameter, ``setLineCap()`` restores the default
    :data:`LineCapButt`.

    >>> def myCaps():
    ...     xp.setLineCap(xp.LineCapRound)
    ...     xp.lineStripWithWidth(xp.makeColor(0, 1, 1, 1), 12, [(20, 10), (100, 60), (180, 10)])
    ...     xp.setLineCap()   # restore the default
    ...     xp.lineStripWithWidth(xp.makeColor(1, 1, 1, 1), 12, [(20, 50), (100, 100), (180, 50)])
    ...
    >>> refCon['draw_f'] = myCaps

    .. image:: /images/setLineCap.png
          :width: 250px
          :align: center        
                  
    .. note:: For the stipple variants the cap is applied to *each individual
              dash*, so *dashLength* is the length of a dash *before* capping. A
              non-butt cap therefore makes every dash longer by half the line
              width at each end, and shrinks the gaps by the same amount.

              The variants which take no *lineWidth* (:func:`lines`,
              :func:`lineStrip`, :func:`lineLoop`) draw at X-Plane's default
              line width, which is narrow --- roughly two pixels. The cap
              extension is half of that, so the cap is barely perceptible.
              Use the ``WithWidth`` variants to see the effect.

    .. warning:: As of X-Plane 12.4.4, the **first** endpoint of a *stipple* run
                 is always drawn with a round cap, whatever the setting has been
                 set to. The last endpoint and the interior dashes are capped
                 correctly. This has been reported to Laminar. `XPD-18441 <https://developer.x-plane.com/x-plane-bug-database/?issue=XPD-18441>`_.

    See ``samples/PI_PGLineCaps.py`` for a working example: it draws one cap
    style per cell with endpoint ticks at the exact coordinates passed in, so
    the cap extension is measurable rather than merely visible.

    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMSetLineCap>`__ :index:`XPLMSetLineCap`

.. py:data:: LineCapButt
   :value: 0

   Lines are capped by straight edges at the start and end point: the line stops
   flush at the endpoint, adding nothing to its length. This is the default, and
   is restored at the start of every drawing callback.
   `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#xplm_LineCapButt>`__: :index:`xplm_LineCapButt`

.. py:data:: LineCapRound
   :value: 1

   Lines are capped by half circles centered on the start and end points,
   extending half the line width past each.
   `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#xplm_LineCapRound>`__: :index:`xplm_LineCapRound`

.. py:data:: LineCapSquare
   :value: 2

   Lines are capped by half squares centered on the start and end points,
   extending half the line width past each.
   `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#xplm_LineCapSquare>`__: :index:`xplm_LineCapSquare`

.. _pg-lines:

Lines
-----

.. py:function:: lines(color, vertices) -> None

    :param int color: Packed color (see :func:`makeColor`)
    :param vertices: Sequence of ``(x, y)`` tuples

    Draw disconnected line segments. Each consecutive *pair* of vertices forms one
    segment.

    >>> def myLines():
    ...     xp.lines(xp.makeColor(1, 0, 1, 1), [(10, 10), (90, 10), (10, 20), (90, 20)])
    ...
    >>> refCon['draw_f'] = myLines

    .. image:: /images/panel_lines.png
       :width: 200px
               
    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMLines>`__ :index:`XPLMLines`

.. py:function:: linesWithWidth(color, lineWidth, vertices) -> None

    :param int color: Packed color
    :param float lineWidth: Line width in pixels
    :param vertices: Sequence of ``(x, y)`` tuples

    Draw disconnected line segments with the given line width.

    >>> def myLinesW():
    ...     xp.linesWithWidth(xp.makeColor(1, 0, 1, 1), 10, [(10, 10), (190, 90), (10, 20), (90, 20)])
    ...
    >>> refCon['draw_f'] = myLinesW

    .. image:: /images/panel_lines_width.png
       :width: 200px
               
    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMLinesWithWidth>`__ :index:`XPLMLinesWithWidth`

.. py:function:: lineStrip(color, vertices) -> None

    :param int color: Packed color
    :param vertices: Sequence of ``(x, y)`` tuples

    Draw a connected line strip; the last vertex is not closed back to the first.

    >>> def myLineStrip():
    ...     xp.lineStrip(xp.makeColor(1, 0, 1, 1), [(10, 10), (190, 90), (10, 20), (90, 20)])
    ...
    >>> refCon['draw_f'] = myLineStrip

    .. image:: /images/panel_linestrip.png
       :width: 200px
               
    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMLineStrip>`__ :index:`XPLMLineStrip`

.. py:function:: lineStripWithWidth(color, lineWidth, vertices) -> None

    :param int color: Packed color
    :param float lineWidth: Line width in pixels
    :param vertices: Sequence of ``(x, y)`` tuples

    Draw a connected line strip with the given line width.

    >>> def myLineStripW():
    ...     xp.lineStripWithWidth(xp.makeColor(1, 0, 1, 1), 10, [(10, 10), (190, 90), (10, 90), (190, 10)])
    ...
    >>> refCon['draw_f'] = myLineStripW

    .. image:: /images/panel_linestrip_width.png
       :width: 200px
               
    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMLineStripWithWidth>`__ :index:`XPLMLineStripWithWidth`

.. py:function:: lineLoop(color, vertices) -> None

    :param int color: Packed color
    :param vertices: Sequence of ``(x, y)`` tuples

    Draw a closed line loop; the last vertex connects back to the first.

    >>> def myLineLoop():
    ...     xp.lineLoop(xp.makeColor(1, 0, 1, 1), [(10, 10), (190, 90), (10, 90), (190, 10)])
    ...
    >>> refCon['draw_f'] = myLineLoop

    .. image:: /images/panel_lineloop.png
       :width: 200px
               
    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMLineLoop>`__ :index:`XPLMLineLoop`

.. py:function:: lineLoopWithWidth(color, lineWidth, vertices) -> None

    :param int color: Packed color
    :param float lineWidth: Line width in pixels
    :param vertices: Sequence of ``(x, y)`` tuples

    Draw a closed line loop with the given line width.

    >>> def myLineLoopW():
    ...     xp.lineLoopWithWidth(xp.makeColor(1, 0, 1, 1), 10, [(10, 10), (190, 90), (10, 90), (190, 10)])
    ...
    >>> refCon['draw_f'] = myLineLoopW

    .. image:: /images/panel_lineloop_width.png
       :width: 200px
               
    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMLineLoopWithWidth>`__ :index:`XPLMLineLoopWithWidth`

.. _pg-filled-shapes:

Filled shapes
-------------

.. py:function:: polygon(color, vertices) -> None

    :param int color: Packed color
    :param vertices: Sequence of ``(x, y)`` tuples (at least 3)

    Draw a filled convex polygon.

    .. note:: *Convex* polygons only. This means you must not have an interior angle greater than 180°. A five-pointed star, for
         example will fail. To draw non-convex polygons, you need to decompose it into a triangle fan.

    >>> def myPolygon():
    ...     xp.polygon(xp.makeColor(1, 0, 1, 1), [(10, 10), (100, 90), (190, 10)])
    ...
    >>> refCon['draw_f'] = myPolygon

    .. image:: /images/panel_polygon.png
       :width: 200px
               
    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMPolygon>`__ :index:`XPLMPolygon`

.. py:function:: quadstrip(color, vertices) -> None

    :param int color: Packed color
    :param vertices: Sequence of ``(x, y)`` tuples (even count, at least 4)

    Draw a series of connected filled quadrilaterals. Quadstrip require
    a particular ordering -- not simple [lower-left, lower-right, upper-right, upper-left] as you
    might use defining a polygon. Instead use (lower-left, upper-left, *lower-right*, upper-right) for
    the first four vertices::

       V4 +--------------------+ V3
          |                    |
       V1 +--------------------+ V2
   
    That is, [V1, V2, V4, V3]. After that, you can continue to add two more vertices, which will define
    the next quad using the more recent two vertices and the new new vertices::
      
        V5 +----------------------+ V6
          /                     /
       V4 +--------------------+ V3
          |                    |
       V1 +--------------------+ V2
                           
    The second quad is [V4, V3, V5, V6]

    >>> def myQuadstrip():
    ...     xp.quadstrip(xp.makeColor(0, 1, 0, 1), [(10, 10), (50, 10), (10, 70), (50, 70), (20, 90), (60, 90)])
    ...
    >>> refCon['draw_f'] = myQuadstrip

    .. image:: /images/panel_quadstrip.png
       :width: 200px
               
    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMQuadstrip>`__ :index:`XPLMQuadstrip`

.. _pg-stipples:

Line stipples
-------------

.. py:function:: linesStipple(color, vertices, dashLength, lineWidth) -> None

    :param int color: Packed color
    :param vertices: Sequence of ``(x, y)`` tuples
    :param float dashLength: Length of each dash, in pixels
    :param float lineWidth: Line width in pixels

    Draw disconnected dashed line segments.

    >>> def myLinesStipple():
    ...     xp.linesStipple(xp.makeColor(0, 1, 0, 1), [(10, 10), (90, 10), (110, 10), (190, 40)], 1, 1)
    ...     xp.linesStipple(xp.makeColor(1, 1, 0, 1), [(10, 30), (90, 30), (110, 30), (190, 60)], 10, 2)
    ...     xp.linesStipple(xp.makeColor(0, 1, 1, 1), [(10, 50), (90, 50), (110, 50), (190, 80)], 2, 10)
    ...
    >>> refCon['draw_f'] = myLinesStipple

    .. image:: /images/panel_linesStipple.png
       :width: 200px
               
    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMLinesStipple>`__ :index:`XPLMLinesStipple`

.. py:function:: lineStripStipple(color, vertices, dashLength, lineWidth) -> None

    :param int color: Packed color
    :param vertices: Sequence of ``(x, y)`` tuples
    :param float dashLength: Length of each dash, in pixels
    :param float lineWidth: Line width in pixels

    Draw a connected dashed line strip.

    >>> def myLineStripStipple():
    ...     xp.lineStripStipple(xp.makeColor(0, 1, 0, 1), [(10, 10), (90, 10), (110, 10), (190, 40)], 1, 1)
    ...     xp.lineStripStipple(xp.makeColor(1, 1, 0, 1), [(10, 30), (90, 30), (110, 30), (190, 60)], 10, 2)
    ...     xp.lineStripStipple(xp.makeColor(0, 1, 1, 1), [(10, 50), (90, 50), (110, 50), (190, 80)], 2, 10)
    ...
    >>> refCon['draw_f'] = myLineStripStipple

    .. image:: /images/panel_lineStripStipple.png
       :width: 200px

    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMLineStripStipple>`__ :index:`XPLMLineStripStipple`

.. py:function:: lineLoopStipple(color, vertices, dashLength, lineWidth) -> None

    :param int color: Packed color
    :param vertices: Sequence of ``(x, y)`` tuples
    :param float dashLength: Length of each dash, in pixels
    :param float lineWidth: Line width in pixels

    Draw a closed dashed line loop.

    >>> def myLineLoopStipple():
    ...     xp.lineLoopStipple(xp.makeColor(0, 1, 0, 1), [(10, 10), (90, 10), (110, 10), (190, 40)], 1, 1)
    ...     xp.lineLoopStipple(xp.makeColor(1, 1, 0, 1), [(10, 30), (90, 30), (110, 30), (190, 60)], 10, 2)
    ...     xp.lineLoopStipple(xp.makeColor(0, 1, 1, 1), [(10, 50), (90, 50), (110, 50), (190, 80)], 2, 10)
    ...
    >>> refCon['draw_f'] = myLineLoopStipple

    .. image:: /images/panel_lineLoopStipple.png
       :width: 200px

    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMLineLoopStipple>`__ :index:`XPLMLineLoopStipple`

.. _pg-vertex-color:

Per-vertex color
----------------

These take no *color* argument; each vertex is a ``(x, y, color)`` tuple, where
*color* is a packed value from :func:`makeColor`.

.. py:function:: linesc(vertices) -> None

    :param vertices: Sequence of ``(x, y, color)`` tuples

    Draw disconnected line segments with per-vertex colors.

    >>> def myLinesc():
    ...     xp.linesc([(10, 10, xp.makeColor(1, 1, 0, 1)),
    ...                (90, 10, xp.makeColor(1, 0, 0, 1)),
    ...                (110, 10, xp.makeColor(0, 1, 1, 1)),
    ...                (190, 40, xp.makeColor(1, 1, 1, 1))])
    ...
    >>> refCon['draw_f'] = myLinesc

    .. image:: /images/panel_linesc.png
       :width: 200px

    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMLinesc>`__ :index:`XPLMLinesc`

.. py:function:: linescWithWidth(lineWidth, vertices) -> None

    :param float lineWidth: Line width in pixels
    :param vertices: Sequence of ``(x, y, color)`` tuples

    Draw disconnected line segments with per-vertex colors and line width.

    >>> def myLinescW():
    ...     xp.linescWithWidth(10, [(10, 10, xp.makeColor(1, 1, 0, 1)),
    ...                             (90, 10, xp.makeColor(1, 0, 0, 1)),
    ...                             (110, 10, xp.makeColor(0, 1, 1, 1)),
    ...                             (190, 40, xp.makeColor(1, 1, 1, 1))])
    ...
    >>> refCon['draw_f'] = myLinescW

    .. image:: /images/panel_linesc_width.png
       :width: 200px

    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMLinescWithWidth>`__ :index:`XPLMLinescWithWidth`

.. py:function:: lineStripc(vertices) -> None

    :param vertices: Sequence of ``(x, y, color)`` tuples

    Draw a connected line strip with per-vertex colors.

    >>> def myLineStripc():
    ...     xp.lineStripc([(10, 10, xp.makeColor(1, 1, 0, 1)),
    ...                    (90, 10, xp.makeColor(1, 0, 0, 1)),
    ...                    (110, 10, xp.makeColor(0, 1, 1, 1)),
    ...                    (190, 40, xp.makeColor(1, 1, 1, 1))])
    ...
    >>> refCon['draw_f'] = myLineStripc

    .. image:: /images/panel_lineStripc.png
       :width: 200px

    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMLineStripc>`__ :index:`XPLMLineStripc`

.. py:function:: lineStripcWithWidth(lineWidth, vertices) -> None

    :param float lineWidth: Line width in pixels
    :param vertices: Sequence of ``(x, y, color)`` tuples

    Draw a connected line strip with per-vertex colors and line width.

    >>> def myLineStripcW():
    ...     xp.lineStripcWithWidth(10, [(10, 10, xp.makeColor(1, 1, 0, 1)),
    ...                                 (90, 10, xp.makeColor(1, 0, 0, 1)),
    ...                                 (110, 10, xp.makeColor(0, 1, 1, 1)),
    ...                                 (190, 40, xp.makeColor(1, 1, 1, 1))])
    ...
    >>> refCon['draw_f'] = myLineStripcW

    .. image:: /images/panel_lineStripc_width.png
       :width: 200px

    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMLineStripcWithWidth>`__ :index:`XPLMLineStripcWithWidth`

.. py:function:: lineLoopc(vertices) -> None

    :param vertices: Sequence of ``(x, y, color)`` tuples

    Draw a closed line loop with per-vertex colors.

    >>> def myLineLoopc():
    ...     xp.lineLoopc([(10, 10, xp.makeColor(1, 1, 0, 1)),
    ...                   (90, 10, xp.makeColor(1, 0, 0, 1)),
    ...                   (110, 10, xp.makeColor(0, 1, 1, 1)),
    ...                   (190, 40, xp.makeColor(1, 1, 1, 1))])
    ...
    >>> refCon['draw_f'] = myLineLoopc

    .. image:: /images/panel_lineLoopc.png
       :width: 200px

    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMLineLoopc>`__ :index:`XPLMLineLoopc`

.. py:function:: lineLoopcWithWidth(lineWidth, vertices) -> None

    :param float lineWidth: Line width in pixels
    :param vertices: Sequence of ``(x, y, color)`` tuples

    Draw a closed line loop with per-vertex colors and line width.

    >>> def myLineLoopcW():
    ...     xp.lineLoopcWithWidth(10, [(10, 10, xp.makeColor(1, 1, 0, 1)),
    ...                                (90, 10, xp.makeColor(1, 0, 0, 1)),
    ...                                (110, 10, xp.makeColor(0, 1, 1, 1)),
    ...                                (190, 40, xp.makeColor(1, 1, 1, 1))])
    ...
    >>> refCon['draw_f'] = myLineLoopcW

    .. image:: /images/panel_lineLoopc_width.png
       :width: 200px

    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMLineLoopcWithWidth>`__ :index:`XPLMLineLoopcWithWidth`

.. py:function:: polygonc(vertices) -> None

    :param vertices: Sequence of ``(x, y, color)`` tuples

    Draw a filled convex polygon with per-vertex colors.

    >>> def myPolygonc():
    ...     xp.polygonc([(10, 10, xp.makeColor(1, 1, 0, 1)),
    ...                  (100, 90, xp.makeColor(1, 0, 0, 1)),
    ...                  (190, 10, xp.makeColor(0, 1, 1, 1))])
    ...
    >>> refCon['draw_f'] = myPolygonc

    .. image:: /images/panel_polygonc.png
       :width: 200px
               
    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMPolygonc>`__ :index:`XPLMPolygonc`

.. py:function:: quadstripc(vertices) -> None

    :param vertices: Sequence of ``(x, y, color)`` tuples

    Draw a quad strip with per-vertex colors.

    >>> def myQuadstripc():
    ...     xp.quadstripc([(10, 10, xp.makeColor(1, 1, 0, 1)),
    ...                    (50, 10, xp.makeColor(1, 0, 0, 1)),
    ...                    (10, 70, xp.makeColor(0, 1, 0, 1)),
    ...                    (50, 70, xp.makeColor(1, 1, 1, 1)),
    ...                    (20, 90, xp.makeColor(0, 1, 1, 1)),
    ...                    (60, 90, xp.makeColor(0, 0, 0, 1))])
    ...
    >>> refCon['draw_f'] = myQuadstripc

    .. image:: /images/panel_quadstripc.png
       :width: 200px
               
    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMQuadstripc>`__ :index:`XPLMQuadstripc`
