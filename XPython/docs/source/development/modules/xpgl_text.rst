xpgl Text
=========

.. py:module:: xpgl
  :noindex:

To use::

  from XPPython3 import xpgl

We directly support three types of fonts and provide the same interface for all.

* **X-Plane bitmap fonts**: These are "built-in" fonts and each are provided in only one size.

  * :py:data:`xp.Font_Proportional`: 10pt
  * :py:data:`xp.Font_Basic`: 10pt 

* **OpenGL GLUT bitmap fonts**: These are "built-in" OpenGL fonts with a few more options.

  * "Helvetica": 10pt, 12pt, 18pt
  * "Times_Roman": 10pt, 24pt

* **TrueType scalable fonts**: These are loaded from font files and scaled on request. X-Planes
  includes a number of font files, you can also provide additional fonts with your plugin.
  For example, check out the fonts under ``<X-Plane>/Resources/fonts``.

.. py:function:: loadFont(fontName, size=0) -> Font:

  :param fontName: Name of font (string or int, see below)
  :param int size: point size of font (there are restrictions)
  :return: Font instance

  You must load a font before using it. For scalable fonts, there is
  some preprocessing performed. For bitmap fonts, this merely sets
  up some data-structures.

  *fontName* is:

  * File path to a scalable font. Paths may be absolute, or relative to <X-Plane> root

  * Literal string "Helvetica" or "Times_Roman", indicating the preference to use a GLUT
    bitmap font

  * Integer enumeration :py:data:`Font_Proportional` or :py:data:`Font_Basic`, indicating
    the preference to use a built-in X-Plane bitmap font.

  *size* is an integer:

  * Any positive value for scalable fonts.
  * Only 10, 12, or 18 for "Helvetica" GLUT font; only 10 or 24 for "Times_Roman" GLUT font
  * Ignored for X-Plane bitmap fonts

  Bitmap fonts *do not scale* as the window changes size or shape. Scalable fonts will
  change size (and aspect ratio) as the underinglying window changes size / shape.

  You should load all your desired fonts early in your plugin and keep a refernce to
  them handle for your draw routines::

    f1 = xpgl.loadFont(xp.Font_Proportional)
    f2 = xpgl.loadFont("Helvetica", 12)
    f3 = xpgl.loadFont("Resources/fonts/Roboto-Bold.ttf", 26)

  If you want multiple sizes for the same typeface, make separate calls to :py:func:`loadFont`.
    
.. py:function:: measureText(fontID, text) -> float

  :param Font fontID: fontID *must* be something returned by :py:func:`loadFont`
  :param str text: Text string to be measured (horizontally)
  :return: Width of string using the given font

  This can be called outside of a draw routine, if useful::

    >>> f2 = xpgl.loadFont("Helvetica", 12)
    >>> xpgl.measureText(f2, "Hello World")
    65
    
.. py:function:: drawText(fontID, x, y, text, alignment='L', color=Colors['white']) -> float

  :param Font fontID: fontID *must* be something returned by :py:func:`loadFont`
  :param int x:
  :param int y: (x, y) location of text to be drawn (depends on *alignment*)
  :param str text: Text string to be drawn                
  :param str alignment: Either 'L', 'C', or 'R' for left-, center- or right-aligned text
  :param RGBColor color: Color of the text to be drawn. Background of text is always transparent.
  :return: Width of string being drawn (same as :py:func:`measureText`)                         

  Draw the string using font and color, at the (x, y) position.

  .. image:: /images/xpgl_drawTextFont.png
     :width: 50%

  Note that as the window changes shape, scaled fonts also change their aspect ratio. Bitmap fonts
  (e.g., GLUT_Helvetica in this example) do not.

  .. image:: /images/xpgl_drawTextFontsStretched.png
     :width: 90%        

  Alignment value is case-insensitive::

    xpgl.drawLine(320, 0, 320, 480)
    xpgl.drawText(Fonts['Inconsolata.ttf'], 320, 260, "Left-Aligned")
    xpgl.drawText(Fonts['Inconsolata.ttf'], 320, 240, "Center-Aligned", alignment="C")
    xpgl.drawText(Fonts['Inconsolata.ttf'], 320, 220, "Right-Aligned", alignment='right')
  
  .. image:: /images/xpgl_drawTextFontAligned.png
        :width: 50%

  Non-printable characters, such as newlines (``\n``) are ignored.

.. Caution:: GLUT is *not* fully installed on Windows and *will not work* for your
             windows-based users without considerable effort on their part. **I do not recommend
             using GLUT bitmap fonts.**
