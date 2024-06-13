xpgl Lines
==========

.. py:module:: xpgl
  :noindex:

To use::

  from XPPython3 import xpgl

A line is defined with a start and end point. Polylines are simple a series of connected lines.
Each line has:

* **Color**: a 3-float tuple representing RGB values. You can use :py:data:`Colors` for some predefined colors.
  If a color is not specified, White is used.

* **Thickness**: a single float representing the (approxiamate) width of the line in texuals. If thickness is not provided,
  the line will be one textual wide.

* **Pattern**: a line may be solid, dotted, or a sequence of dot and dashes. Once a pattern is set, it will be
  use by *all* lines: it must be reset to solid if desired.
  
*Color* and *Thickness* are settable within each line function call, and do not "carry-over" into subsequent calls.
*Pattern* is only settable using a separate function, and remains set until changed.

.. py:function:: drawLine(x1, y1, x2, y2, thickness=1, color=Colors['white']) -> None

  :param float x1: 
  :param float y1: Starting (x1, y1) endpoint
  :param float x2:
  :param float y2: Ending (x2, y2) endpoint
  :param float thickness: Line width
  :param RGBColor color: OpenGL color. Tuple of floats (r, g, b) ranging [0..1]
  :return: None                         
 
  Without specifying thickness, this draws a single line of width "1" in given color::

    xpgl.drawLine(0, 0, 640, 480, Colors['red'])
    xpgl.drawLine(320, 240, 640, 0)

  .. image:: /images/xpgl_drawLine.png
    :width: 50%

  Specifying thickness uses and underlying OpenGL capability::
  
    offset = 0
    for i in range(1, 41, 4):
        offset += (2 * i) + 10
        xpgl.drawLine(-50 + offset, 10 , 600 + offset, 400, thickness=i, color=Colors['red'])

  .. image:: /images/xpgl_drawWideLine.png
             :width: 50%

.. py:function:: drawPolyLine(points, color=Colors['white']) -> None:
                   
  :param list points: List of vertices (x, y) for the line segments
  :param float thickness: Line width
  :param RGBColor color: OpenGL color. Tuple of floats (r, g, b) ranging [0..1]
  :return: None                         
                      
  Draws a set of line segments from vertex to vertex. Resulting shape does not
  close unless last vertex is the same as the first. Drawing without specifying thickness::
  
    xpgl.drawPolyLine([(200, 200), (300, 200), (500, 310), (500, 300), (250, 300)])
    xpgl.drawPolyLine([(100, 100), (300, 100), (200, 350), (550, 375), (100, 100)], color=Colors['orange'])

  .. image:: /images/xpgl_drawPolyLine.png
             :width: 50%

  Drawing with thickness::
    
    xpgl.drawWidePolyLine([(200, 200), (300, 200), (500, 310), (500, 300), (250, 300)], thickness=3)
    xpgl.drawWidePolyLine([(100, 100), (300, 100), (200, 350), (550, 375), (100, 100)], thickness=10, color=Colors['orange'])

  .. image:: /images/xpgl_drawWidePolyLine.png
             :width: 50%
                     

.. py:function:: setLinePattern(pattern: int = 0xffff) -> None

  Sets (or removes) stipple pattern associated with all lines. `0xffff` disables the
  pattern, resulting in solid lines.

  Stipple pattern is a 16-bit value. Each ``1`` bit represents "on", ``0`` bit represents "off".
  Patterns are evaluated low-bit to high-bit (that is right-to-left), and repeat using the 16-bit pattern.

  For example:

    +-----------+------------------------+------------------------------------+
    | Pattern   | Value in binary        |Result                              |
    +===========+========================+====================================+
    | ``0xffff``| ``1111-1111-1111-1111``|"all on", resulting in solid line   |
    +-----------+------------------------+------------------------------------+
    | ``0x5555``| ``0101-0101-0101-0101``|"on-off-on-off...", resulting in a  |
    |           |                        |dot pattern, where the dot length is|
    |           |                        |the same as the blank space between |
    |           |                        |to dots.                            |
    |           |                        |                                    |
    +-----------+------------------------+------------------------------------+
    |``0x087f`` | ``0000-1000-0111-1111``|"7 on, four off, 1 on, four off",   |
    |           |                        |yields a dash-dot pattern.          |
    +-----------+------------------------+------------------------------------+
  
  A single pattern is repeated for the length of the line or poly line. If you need
  a different pattern (different color or different thickness) you'll need to make
  a separate function call.::
  
    x1 = 50
    x2 = 550
    y1 = 100
    for pattern in (0xffff, 0x5555, 0x3333, 0x1111, 0x3131, 0x5151, 0x18ff, 0x2727, 0x023f, 0x087f, 0x3f3f, 0x7f7f,  ):
        y2 = y1 = y1 + 15
        xpgl.setLinePattern(pattern)
        xpgl.drawWideLine(x1, y1, x2, y2, thickness=10, color=Colors['orange'])
    xpgl.setLinePattern()

  .. image:: /images/xpgl_setLinePattern.png
             :width: 50%

  Because (typically) your set of drawing functions are called repeatedly, *and* because a stipple pattern
  remains active until reset, you *should* disable the pattern when you are finished. Otherwise, the pattern
  remains active and on the next call to your function, your lines will *continue* to use the previous stipple
  pattern.
  
