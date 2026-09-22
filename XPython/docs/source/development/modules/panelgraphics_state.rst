Transform, Scissor, Stencil
===========================

.. py:module:: XPLMPanelGraphics
    :no-index:
.. py:currentmodule:: xp

To use::

  import xp

Graphics-state operations for the native panel-graphics API (new with SDK 440):

.. rst-class:: compact

* :ref:`pg-transform`
* :ref:`pg-transformations`
* :ref:`pg-scissor`
* :ref:`pg-stencil`

These can only be used in Windows or Avionics Devices where are created to
support :data:`WindowContentTypePanelGraphics`. (See :doc:`panelgraphics`)

.. _pg-transform:

Transform stack
---------------

Transforms compose; push the current matrix, apply translate / rotate / scale,
draw, then pop to restore.

.. py:function:: transformContext() -> None
.. py:function:: transformPush() -> None
.. py:function:: transformPop() -> None

    :func:`transformTranslate`, :func:`transformRotate` and :func:`transformScale`
    **must** be called inside a :func:`transformPush` / :func:`transformPop` pair
    --- calling one outside a pair is an error. X-Plane does not push a transform
    scope around your drawing callback, so a transform with no enclosing push has
    no defined end.

    Using the :func:`transformContext` context manager is therefore preferred over
    calling :func:`transformPush` and :func:`transformPop` yourself: it cannot
    leave the pair unbalanced, even if your drawing code raises.

    Transformations can be nested.
    
    .. note::

        Because Window coordinate system is relative the user's global window, it is common
        to provide a *Translation* transformation to "move" the (0, 0) origin to the lower left
        of the window. Avionics Device coordinate system already (internally) translates the origin.

    The following three examples will draw lines using vertices relative the window's lower
    left corner, moving the lines as the window is moved.

    First, using :func:`transformContext`

    >>> def lines(windowID, _refCon):
    ...     left, _top, _right, bottom = xp.getWindowGeometry(windowID)
    ...     with xp.transformContext():
    ...         xp.transformTranslate(left, bottom)
    ...         xp.lines(xp.makeColor(1, 0, 1, 1), [(10, 10), (90, 10), (10, 20), (90, 20)])
    ...

    Second, using :func:`transformPush` and :func:`transformPop` instead of the recommended context manager:
       
    >>> def lines(windowID, _refCon):
    ...     left, _top, _right, bottom = xp.getWindowGeometry(windowID)
    ...     xp.transformPush()
    ...     xp.transformTranslate(left, bottom)
    ...     xp.lines(xp.makeColor(1, 0, 1, 1), [(10, 10), (90, 10), (10, 20), (90, 20)])
    ...     xp.transformPop()
    ...

    Third, not using a transform, but directly updating the vertices coordinates:
    
    >>> def lines(windowID, _refCon):
    ...     left, _top, _right, bottom = xp.getWindowGeometry(windowID)
    ...     xp.lines(xp.makeColor(1, 0, 1, 1), [(left + 10, bottom +10),
    ...                                         (left + 90, bottom + 10),
    ...                                         (left + 10, bottom + 20),
    ...                                         (left + 90, bottom + 20)])
    ...

    Define ``lines()`` any way you like and then create a window using it
    as the :func:`draw` function.
    
    >>> winID = xp.createWindowEx(draw=lines, visible=1,
    ...                           left=100, top=400, right=300, bottom=300,
    ...                           contentType=xp.WindowContentTypePanelGraphics)
    ...

    .. image:: /images/panel_transform_lines.png
       :width: 200        


    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMTransformPush>`__ :index:`XPLMTransformPush`

    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMTransformPop>`__ :index:`XPLMTransformPop`

.. _pg-transformations:

Transformations
---------------

There are three basic transformations:

.. rst-class:: compact
               
* Translation: move the origin point of the drawing somewhere else

* Rotation: rotate the drawing around a (specified) point

* Scale: enlarge or shrink the drawing, relative the origin  

.. py:function:: transformTranslate(dx, dy) -> None

    :param float dx: Horizontal offset in pixels
    :param float dy: Vertical offset in pixels

    Offset all subsequent drawing by ``(dx, dy)`` pixels.

    Must be called inside a :func:`transformPush` / :func:`transformPop` pair.

    See example at the top of this page.

    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMTransformTranslate>`__ :index:`XPLMTransformTranslate`

.. py:function:: transformRotate(centerX=0, centerY=0, angle=0) -> None

    :param float centerX: X of rotation center, in pixels
    :param float centerY: Y of rotation center, in pixels
    :param float angle: Rotation angle in degrees (counter-clockwise)

    Rotate subsequent drawing around ``(centerX, centerY)`` counter-clockwise by
    *angle* degrees.

    Must be called inside a :func:`transformPush` / :func:`transformPop` pair.
    
    >>> def rotateLines(windowID, _refCon):
    ...     left, top, right, bottom = xp.getWindowGeometry(windowID)
    ...     centerx = (left + right) / 2.
    ...     centery = (top + bottom) / 2.
    ...     with xp.transformContext():
    ...         xp.transformTranslate(centerx, centery)
    ...         xp.transformRotate(angle=xp.getCycleNumber() % 360)
    ...         xp.lines(xp.makeColor(1, 0, 1, 1), [(-50, 0), (50, 0), (0, 10), (00, -10)])
    ...
    >>> winID = xp.createWindowEx(draw=rotateLines, visible=1,
    ...                           left=100, top=400, right=300, bottom=300,
    ...                           contentType=xp.WindowContentTypePanelGraphics)
    ...

    This example first moves the origin (0, 0) to the center of the window, then sets up a rotation. The lines
    are drawn "knowing" that (0, 0) is the center. Because :func:`transformRotate` defaults to (0, 0) as the rotation
    point, the drawing rotates in the obvious way.

    Note that if we'd specify the rotation *after* drawing the lines, the line would not rotate, even though
    they're within the transformation context: order is important!
    
    .. image:: /images/transform_rotate.gif
       :width: 200px        

    .. note:: Not all Panel Graphics drawing routines can be executed within a rotation transformation -- even
              a 0 degree rotation. These are not allowed:

              .. rst-class:: compact
                             
              * :func:`scissorSet`, :func:`scissorIntersect`

              * :func:`accumulateTouchZone`

              * :func:`drawCalls`

              * :func:`svtDisplayDrawIn`, :func:`mapDisplayDrawIn`
                
    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMTransformRotate>`__ :index:`XPLMTransformRotate`

.. py:function:: transformScale(scaleX, scaleY) -> None

    :param float scaleX: Horizontal scale factor
    :param float scaleY: Vertical scale factor

    Scale subsequent drawing by ``(scaleX, scaleY)`` relative to the origin.

    Must be called inside a :func:`transformPush` / :func:`transformPop` pair.

    **Neither factor may be zero**: a zero scale collapses the coordinate system
    onto a line, so a position expressed in it can no longer be recovered.
    Negative factors are fine, and mirror your drawing --- ``transformScale(-1, 1)``
    flips it horizontally about the current origin.

    >>> def scalePoly(windowID, _refCon):
    ...     left, top, right, bottom = xp.getWindowGeometry(windowID)
    ...     centerx = (left + right) / 2.
    ...     centery = (top + bottom) / 2.
    ...     with xp.transformContext():
    ...         xp.transformTranslate(centerx, centery)
    ...         xp.transformScale((right-left) / 200., (top-bottom) / 100.)
    ...         xp.polygon(xp.makeColor(1, 0, 1, 1), [(-20, -20), (-20, 20), (20, 20), (20, -20)])
    ...
    >>> winID = xp.createWindowEx(draw=scalePoly, visible=1,
    ...                           left=100, top=400, right=300, bottom=300,
    ...                           contentType=xp.WindowContentTypePanelGraphics)
    ...

    .. image:: /images/transform_scale1.gif
       :width: 400px
    
    Without the call to :func:`transformScale` the square would remain the same size, regardless of window size: the
    :func:`transformTranslate` would still keep the square centered within the window.
    
    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMTransformScale>`__ :index:`XPLMTransformScale`

.. _pg-scissor:

Scissor clipping
----------------

.. py:function:: scissorContext() -> None
.. py:function:: scissorPush() -> None
.. py:function:: scissorPop() -> None

    Save the current scissor rectangle onto the scissor stack. Use the context manager :func:`scissorContext` instead
    of separate calls to :func:`scissorPush` and :func:`scissorPop`.

    Scissors mask rectangular portions of the drawing space, blocking drawing within the rectangle. They
    are similar to and faster than Stencil Masks, but are limited to rectangular shapes.

    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMScissorPush>`__ :index:`XPLMScissorPush`

    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMScissorPop>`__ :index:`XPLMScissorPop`

    >>> def scissorDraw(windowID, refCon):
    ...     left, _top, _right, bottom = xp.getWindowGeometry(windowID)
    ...     fullWindow = [(0, 0), (0, 100), (200, 100), (200, 0)]
    ...     with xp.transformContext():
    ...         xp.transformTranslate(left, bottom)
    ...         xp.polygon(xp.makeColor(1, 0, 0, 1), fullWindow)
    ...         with xp.scissorContext():
    ...             xp.scissorSet(20, 80, 180, 20)
    ...             xp.polygon(xp.makeColor(0, 1, 0, 1), fullWindow)
    ...
    >>> winID = xp.createWindowEx(draw=scissorDraw, visible=1,
    ...                           left=100, top=400, right=300, bottom=300,
    ...                           contentType=xp.WindowContentTypePanelGraphics)
    ...

    .. image:: /images/panel_scissor.png
        :width: 200px       

    See ``samples/PI_PGScissor.py`` for this example as a runnable plugin.

.. py:function:: scissorSet(left, top, right, bottom) -> None

    :param int left: Left edge in panel pixels
    :param int top: Top edge in panel pixels
    :param int right: Right edge in panel pixels
    :param int bottom: Bottom edge in panel pixels

    Set an absolute scissor rectangle; only pixels inside it are drawn. If a scissor has already been defined, it
    will be replaced with this new rectangle.

    .. note:: You cannot use :func:`scissorSet` within an active rotation transformation.
              
    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMScissorSet>`__ :index:`XPLMScissorSet`

.. py:function:: scissorIntersect(left, top, right, bottom) -> None

    :param int left: Left edge in panel pixels
    :param int top: Top edge in panel pixels
    :param int right: Right edge in panel pixels
    :param int bottom: Bottom edge in panel pixels

    Intersect the current scissor box with the absolute rectangle given by edges
    ``(left, top, right, bottom)`` in panel pixels; the resulting scissor is their
    intersection, so the clip area only ever shrinks. You must have an "active" scissor defined with :func:`scissorSet`
    to use this function.

    .. note:: You cannot use :func:`scissorIntersect` within an active rotation transformation.
              
    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMScissorIntersect>`__ :index:`XPLMScissorIntersect`

.. _pg-stencil:

Stencil mask
------------

Define a mask shape using :func:`setupStencilMask`, then activate it using  :func:`useStencilMask` and draw normally
to restrict output to (or away from) the masked region. Turn off stencil testing with ``useStencilMask(0, 0)``.
Mask can be reset using :func:`clearStencilMask` (but only after you've turned off testing).

There are two common uses cases for this:

* **Mask**: Draw an object, but leave a "hole" in it. For example, composing a donut
  by drawing a large circle, but masking "out" a circlur interior section. (Think, "masking tape"
  used to exclude getting paint in unwanted locations.)

* **Stencil** Make visible only a portion of an object. For example, drawing a whole image, but
  only having part of it visible. (Think, "paper stencil" used with spray paint to form
  letters or shapes.)

In either case, you'll first draw a "shape" into your stencil-mask which, itself, *will not be displayed*.
This shape will form the mask or stencil. Then, you'll *activate* the stencil mask and draw your visuals.
These portions of these visuals will be made visible depending on the previously defined mask or stencil.

X-Plane initializes stencil state before calling you and restores state at the end of your drawing callback.

.. image:: /images/panel_stencil.png
    :width: 300px

Above, two overlapping squares and a star are recorded into three separate stencil bits. The
red, green and yellow areas are actually each a *full-window* drawn polygon, confined to its region purely by
the stencil test, and the star is knocked out of all of them. See :doc:`stencilmask` for how
this is done.

Similar to transformations  you'll bracket all this drawing with use
a context manager :py:func:`setupStencilMask`, or the pair :py:func:`beginSetupStencilMask` and :py:func:`endSetupStencilMask`.

Create StencilMask
++++++++++++++++++

.. py:function:: setupStencilMask(bits, mask) -> None

    :param int bits: Stencil value written to the bit positions selected by ``mask`` (only 8-bits significant)
    :param int mask: Bit mask selecting which stencil bits are written (only 8-bits significant)

    Context manager supporting :func:`beginSetupStencilMask` and :func:`endSetupStencilMask`.

    Panel Graphics drawing routines *within this context* write to the stencil buffer
    instead of the screen. Color and opacity of this drawing *does not
    matter*, only the shape of the drawn object is used.

    Each pixel covered by the drawn shape has ``bits`` written into it, but only in the bit
    positions selected by ``mask``. Bits outside of ``mask`` are left unchanged, which is what
    allows several shapes to share one stencil buffer without interfering.

    .. warning:: **Only** Lines, Stipples, Polygons, and Quadstrip panel graphics primitives can be used to define
      the stencil. Panel graphics Fonts will cause the sim to crash. Not providing any valid geometry will
      also crash the sim. You *can* draw fonts under :func:`useStencilMask`: they just can't be used to define
      the stencil itself.

    See also :doc:`stencilmask` for more information about how to interpret parameters ``bits`` and ``mask``.

.. py:function:: beginSetupStencilMask(bits, mask) -> None
.. py:function:: endSetupStencilMask() -> None

    Same as context manager :func:`setupStencilMask`
    
    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMBeginSetupStencilMask>`__ :index:`XPLMBeginSetupStencilMask`

    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMEndSetupStencilMask>`__ :index:`XPLMEndSetupStencilMask`

Use StencilMask
+++++++++++++++

.. py:function:: useStencilMask(bits=0, mask=0) -> None

    :param int bits: Stencil value to test against (only 8-bits significant)
    :param int mask: Bit mask selecting which stencil bits are tested (only 8-bits significant)

    Activate stencil testing for subsequent drawing. A pixel is drawn only where the stencil
    buffer matches, in the bit positions selected by ``mask``::

      (stencil & mask) == (bits & mask)

    That is, if ``mask=0x05``, only BIT0 and BIT2 of the stencil are tested, and those bits
    *must* match the corresponding bits of ``bits``. Every other bit position is ignored.

    Note that ``bits`` is a value to *match*, not a shape identifier: ``useStencilMask(0x03,
    0x03)`` draws where BIT0 and BIT1 are *both* set -- the intersection of two shapes -- while
    ``useStencilMask(0x01, 0x03)`` draws where BIT0 is set and BIT1 is clear. 

    You can change which part of the stencil to use through successive calls to :func:`useStencilMask`.
    You can turn off stencil testing
    by calling ``useStencilMask(0, 0)`` because "everything passes" the test. (You *must* turn off stencil testing
    prior to calling :func:`clearStencilMask` but, in general, you never need to call :func:`clearStencilMask`.)

    See also :doc:`stencilmask`.

    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMUseStencilMask>`__ :index:`XPLMUseStencilMask`

Clear Stencil Mask
++++++++++++++++++

.. py:function:: clearStencilMask() -> None

    Erase the entire stencil buffer, discarding all masks.

    It does not change whether stencil testing is on -- use ``useStencilMask(0, 0)`` for that.

    This is all-or-nothing: it zeroes every bit of the whole buffer. To clear selected bits over
    only *part* of the buffer, write zeros instead -- :func:`setupStencilMask` with ``bits=0``
    and a ``mask`` identifying the bit(s) to reset.

    .. note:: You *must* disable stencil mask by calling ``useStencilMask(0, 0)`` *before* calling
              ``clearStencilMask()``. It is an error otherwise.

    This is rarely needed: it may be useful if you need to clear and re-record the whole stencil mask.

    The stencil buffer does not persists between frames, so call you do not need to call
    :func:`clearStencilMask` at the start (or end) of your drawing routine.

    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMClearStencilMask>`__ :index:`XPLMClearStencilMask`

.. toctree::
   :hidden:

   /development/modules/stencilmask
