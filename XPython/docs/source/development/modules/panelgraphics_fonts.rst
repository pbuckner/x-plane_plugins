Fonts
=====

.. py:module:: XPLMPanelGraphics
    :no-index:
.. py:currentmodule:: xp

To use::

  import xp

Text rendering for the native panel-graphics API (new with SDK 440). Create a
font handle for a character set, add one or more TrueType faces to it, then
measure and draw text.

.. rst-class:: compact
               
* Draw from a panel-graphics drawing context --- see :doc:`panelgraphics`.

*  *color* arguments are packed values from :func:`makeColor`

* *justification* is one of the :data:`JustLeft` / :data:`JustCenter` /:data:`JustRight` constants.

All of these font functions can be executed at any time with the exception of the font drawing functions: ``fontDrawString*()``.
Ideally, you should load and configure outside of your draw callbacks.

See ``samples/PI_PGFonts.py`` for examples of all of these features.

Functions
---------

.. py:function:: createFont(charset=xp.CharSetUnicode) -> XPLMFontHandle

    :param int charset: One of :data:`CharSetDigits`, :data:`CharSetASCII`, :data:`CharSetUnicode`
    :return: :class:`XPLMFontHandle` font handle

    Create a new font handle for the given character set. Add one or more TrueType
    faces with :func:`fontAddFace` before drawing. Destroy it with
    :func:`destroyFont`.

    The selected ``charset`` is a *hint*, potentially limiting the set of characters baked into the font atlas. For example, :data:`CharSetDigits` will
    add only digits and basic numeric punctuation, representing the smallest set of data to add: This will be the quickest
    and have the least memory impact.
    
    >>> font = xp.createFont(xp.CharSetASCII)
    >>> font
    <capsule object "XPLMFontHandle" at 0x12d9bed0>
    
    .. note::
       By itself, ``font`` won't work, you *must* add at least one face using :func:`fontAddFace` before use.
    
    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMCreateFont>`__ :index:`XPLMCreateFont`

.. py:function:: destroyFont(font) -> None

    :param XPLMFontHandle font: Handle from :func:`createFont`

    Destroy a font handle created with :func:`createFont` and free its resources.

    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMDestroyFont>`__ :index:`XPLMDestroyFont`

.. py:function:: fontAddFace(font, ttf_path) -> int

    :param XPLMFontHandle font: Handle from :func:`createFont`
    :param str ttf_path: Path to a ``.ttf`` / ``.otf`` file
    :return: 1 on success, 0 otherwise                     

    Add a TrueType face to a font handle. Multiple faces may be added to provide
    fallback glyphs, searched in the order added.

    ``ttf_path`` is an absolute path, or path relative to <X-Plane Root>. X-Plane ships with a few fonts under ``Resources/fonts/``.

    >>> font = xp.createFont(xp.CharSetASCII)
    >>> xp.fontAddFace(font, 'Resources/fonts/DejaVuSans.ttf')
    
    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMFontAddFace>`__ :index:`XPLMFontAddFace`

.. py:function:: fontGetMetrics(font, fontSize) -> Tuple[float, float, float]

    :param XPLMFontHandle font: Handle from :func:`createFont`
    :param float fontSize: Point size
    :return: ``(lineHeight, lineAscent, lineDescent)`` in pixels

    Return the line metrics for the font at the given size. Works only for font handles for which
    you have already loaded one or more faces using :func:`fontAddFace`.

    >>> font = xp.createFont(xp.CharSetASCII)
    >>> xp.fontGetMetrics(font, 12)
    (-1.0, -1.0, -1.0)
    >>> xp.fontAddFace(font, 'Resources/fonts/Inconsolata.ttf')
    >>> xp.fontGetMetrics(font, 12)

    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMFontGetMetrics>`__ :index:`XPLMFontGetMetrics`

.. py:function:: fontMeasureString(font, fontSize, string) -> float

    :param XPLMFontHandle font: Handle from :func:`createFont` 
    :param float fontSize: Point size
    :param str string: String to measure
    :return: Width in pixels the string would occupy if drawn

    Return the width in pixels the string would occupy if drawn. The string is not
    drawn (and this function may be called outside of panel graphics context.)

    >>> font = xp.createFont(xp.CharSetASCII)
    >>> xp.fontMeasureString(font, 12, "Hello World")
    0.0
    >>> xp.fontAddFace(font, 'Resources/fonts/DejaVuSans.ttf')
    >>> xp.fontMeasureString(font, 12, "Hello World")
    69.322

    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMFontMeasureString>`__ :index:`XPLMFontMeasureString`

.. py:function:: fontGetLineCount(font, fontSize, string, width) -> int

    :param XPLMFontHandle font: Handle from :func:`createFont`
    :param float fontSize: Point size
    :param str string: String to measure
    :param float width: Wrap width in pixels
    :return: Number of lines the string would occupy if word-wrapped to *width*

    >>> font = xp.createFont(xp.CharSetASCII)
    >>> xp.fontAddFace(font, 'Resources/fonts/DejaVuSans.ttf')
    >>> xp.fontGetLineCount(font, 12, "Hello World", 100)
    1
    >>> xp.fontGetLineCount(font, 12, "Hello World", 50)
    2
    
    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMFontGetLineCount>`__ :index:`XPLMFontGetLineCount`

.. py:function:: fontFitForward(font, fontSize, string, width) -> int

    :param XPLMFontHandle font: Handle from :func:`createFont`
    :param float fontSize: Point size
    :param str string: String to measure
    :param float width: Available width in pixels
    :return: Number of characters from the start of the string that fit within *width*

    Measured left to right.

    >>> font = xp.createFont(xp.CharSetASCII)
    >>> xp.fontAddFace(font, 'Resources/fonts/DejaVuSans.ttf')
    >>> xp.fontFitForward(font, 12, "Hello World", 50)
    7
    >>> xp.fontMeasureString(font, 12, "Hello World"[0:7])
    46.095
    >>> xp.fontMeasureString(font, 12, "Hello World"[0:8])
    53.437
    
    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMFontFitForward>`__ :index:`XPLMFontFitForward`

.. py:function:: fontFitReverse(font, fontSize, string, width) -> int

    :param XPLMFontHandle font: Handle from :func:`createFont`
    :param float fontSize: Point size
    :param str string: String to measure
    :param float width: Available width in pixels
    :return: Number of leading characters to skip so the remaining tail (``string[index:]``) fits within *width*

    Measured right to left: a return of ``0`` means the whole string fits;
    ``len(string)`` means nothing fits.

    >>> font = xp.createFont(xp.CharSetASCII)
    >>> xp.fontAddFace(font, 'Resources/fonts/DejaVuSans.ttf')
    >>> xp.fontFitReverse(font, 12, "Hello World", 50)
    3
    >>> xp.fontMeasureString(font, 12, "Hello World"[3:])
    46.095
    >>> xp.fontMeasureString(font, 12, "Hello World"[2:])
    52.916

    Here, ``3`` means the string "Hello World"[3:], or "lo World" will fit within the given with ``width``.

    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMFontFitReverse>`__ :index:`XPLMFontFitReverse`

.. py:function:: fontDrawString(font, color, fontSize, x, y, string, justification) -> None

    :param XPLMFontHandle font: Handle from :func:`createFont`
    :param int color: Packed color (see :func:`makeColor`)
    :param float fontSize: Point size
    :param float x: X of the anchor, in pixels
    :param float y: Y of the baseline anchor, in pixels
    :param str string: String to draw
    :param int justification: :data:`JustLeft`, :data:`JustCenter`, or :data:`JustRight`

    Draw a string at ``(x, y)`` (the baseline anchor) with the given font, size,
    packed color, and justification.

    The anchor will be the left-most point for :data:`JustLeft`, but the right-most point for :data:`JustRight`.

    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMFontDrawString>`__ :index:`XPLMFontDrawString`

.. py:function:: fontDrawStringFixedSpacing(font, color, fontSize, x, y, string, fixedSpacing, justification) -> None

    :param XPLMFontHandle font: Handle from :func:`createFont`
    :param int color: Packed color
    :param float fontSize: Point size
    :param float x: X of the anchor
    :param float y: Y of the baseline anchor
    :param str string: String to draw
    :param int fixedSpacing: Fixed per-character advance, in pixels
    :param int justification: :data:`JustLeft`, :data:`JustCenter`, or :data:`JustRight`

    Draw a string using fixed per-character spacing (in pixels) instead of the
    font's natural proportional spacing. Useful for numeric readouts.

    Compare natural versus fixed spacing for the string "Illingworth 01234":

    .. image:: /images/panel_fonts_fixed.png
               :width: 400px

    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMFontDrawStringFixedSpacing>`__ :index:`XPLMFontDrawStringFixedSpacing`

.. py:function:: fontDrawStringWordWrapped(font, color, fontSize, x, y, string, wrapWidth, justification) -> None

    :param XPLMFontHandle font: Handle from :func:`createFont`
    :param int color: Packed color
    :param float fontSize: Point size
    :param float x: X of the anchor
    :param float y: Y of the first line's baseline anchor
    :param str string: String to draw
    :param int wrapWidth: Wrap width, in pixels
    :param int justification: :data:`JustLeft`, :data:`JustCenter`, or :data:`JustRight`

    Draw a string with automatic word wrapping at *wrapWidth* pixels. Lines stack
    downward from the initial *y* by the font's line height.

    Remember the X anchor changes based on selected justification.
    

    .. image:: /images/panel_fonts_wrapped.png
               :width: 400px

    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMFontDrawStringWordWrapped>`__ :index:`XPLMFontDrawStringWordWrapped`

.. py:function:: fontDrawStringRotated(font, color, fontSize, x, y, string, angle, justification) -> None

    :param XPLMFontHandle font: Handle from :func:`createFont`
    :param int color: Packed color
    :param float fontSize: Point size
    :param float x: X of the anchor
    :param float y: Y of the baseline anchor
    :param str string: String to draw
    :param float angle: Rotation angle in degrees, **positive clockwise**
    :param int justification: :data:`JustLeft`, :data:`JustCenter`, or :data:`JustRight`

    Draw a string rotated by *angle* degrees (positive clockwise) around the
    ``(x, y)`` anchor point.

    .. image:: /images/panel_fonts_rotation.png
               :width: 400px

    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMFontDrawStringRotated>`__ :index:`XPLMFontDrawStringRotated`

Constants
---------

.. _XPLMCharSet:

XPLMCharSet
***********

The character set of a font, passed to :func:`createFont`. For ``xp.py`` these
are defined without the leading ``xplm_`` e.g., ``xp.CharSetASCII``.

.. py:data:: CharSetDigits
   :value: 0

   Digits and basic numeric punctuation only (smallest).
   `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#xplm_CharSetDigits>`__: :index:`xplm_CharSetDigits`

.. py:data:: CharSetASCII
   :value: 1

   The printable ASCII range.
   `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#xplm_CharSetASCII>`__: :index:`xplm_CharSetASCII`

.. py:data:: CharSetUnicode
   :value: 2

   Full Unicode (largest).
   `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#xplm_CharSetUnicode>`__: :index:`xplm_CharSetUnicode`

.. _XPLMJustification:

XPLMJustification
*****************

Horizontal justification, passed to the ``fontDrawString*`` functions. For
``xp.py`` these are defined without the leading ``xplm_``.

.. image:: /images/panel_fonts_justification.png
    :width: 400px           

.. py:data:: JustLeft
   :value: 0

   Left-justified (the anchor is the left edge).
   `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#xplm_JustLeft>`__: :index:`xplm_JustLeft`

.. py:data:: JustCenter
   :value: 1

   Centered on the anchor.
   `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#xplm_JustCenter>`__: :index:`xplm_JustCenter`

.. py:data:: JustRight
   :value: 2

   Right-justified (the anchor is the right edge).
   `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#xplm_JustRight>`__: :index:`xplm_JustRight`

Types
-----

.. py:class:: XPLMFontHandle

    Opaque capsule representing a font, as returned by :func:`createFont`.
    Release it with :func:`destroyFont`.
