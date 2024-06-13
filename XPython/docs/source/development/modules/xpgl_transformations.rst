xpgl Transformations
====================

.. py:module:: xpgl
  :noindex:
     
To use::

  from XPPython3 import xpgl

You can transform any drawn object/text/image (or set of objects) using:

* :py:func:`setRotateTransform`: Rotate around a specific point.

* :py:func:`setTranslateTransform`: Move horizontally and/or vertically to new position.

* :py:func:`setScaleTransform`: Enlarge or shrink, relative a specific point.

These transformation need to be surrounded by their own *graphics context* so as to not alter
all subsequent drawing.

Within your draw callback, you'll *bracket* the set of objects you want modfied with calls to
*save* and *restore* the graphics context:

.. py:function:: graphicsContext() -> None
.. py:function:: saveGraphicsContext() -> None
.. py:function:: restoreGraphicsContext() -> None

  :param: None
  :return: None
  
  Call :py:func:`saveGraphicsContext` immediately before you want to *transform* a
  drawn object, and :py:func:`restoreGraphicsContext` after you've completed *drawing*. 

  The sequence is:

  .. code:: none

    saveGraphicsContext()
    <rotate>, <translate> and / or <scale>
    <draw>
    restoreGraphicsContext()
    
  -or-:

  .. code:: none

    with graphicsContext():
        <rotate>, <translate> and / or <scale>
        <draw>

  You *must* pair each :py:func:`saveGraphicsContext` with :py:func:`restoreGraphicsContext` and
  they may be nested.

  For example::
  
    xpgl.saveGraphicsContext()
    xpgl.setRotateTransform(-xp.getCycleNumber() % 360, 320, 380)
    xpgl.drawWideLine(320, 380, 320, 430, thickness=7, color=Colors['orange'])
    xpgl.restoreGraphicsContext()

    xpgl.saveGraphicsContext()
    xpgl.setRotateTransform(xp.getCycleNumber() % 360, 320, 240)
    xpgl.drawWideLine(320, 100, 320, 380, thickness=7, color=Colors['red'])

    xpgl.saveGraphicsContext()
    xpgl.setRotateTransform(-xp.getCycleNumber() * 4 % 360, 320, 400)
    xpgl.drawWideLine(320, 380, 320, 420, thickness=3, color=Colors['white'])
    xpgl.restoreGraphicsContext()
    xpgl.restoreGraphicsContext()

    xpgl.saveGraphicsContext()
    xpgl.setRotateTransform(-xp.getCycleNumber() % 360, 320, 75)
    xpgl.drawWideLine(320, 50, 320, 100, thickness=7, color=Colors['green'])
    xpgl.restoreGraphicsContext()
    
  .. image:: /images/xpgl_graphicsRotate.gif
             :width: 50%

  Note that for each drawn line, we have a separate graphics context, *and*
  for the white line at the end of the red line, we've nested an additional
  rotation.
  
  A more pythonic approach is to use a context manager which handles
  the save/restore automatically, and improves code clarity by adding indentation blocks.
  
  The same code using the context manager :py:func:`graphicsContext`::

    with xpgl.graphicsContext():
        xpgl.setRotateTransform(-xp.getCycleNumber() % 360, 320, 380)
        xpgl.drawWideLine(320, 380, 320, 430, thickness=7, color=Colors['orange'])

    with xpgl.graphicsContext():
        xpgl.setRotateTransform(xp.getCycleNumber() % 360, 320, 240)
        xpgl.drawWideLine(320, 100, 320, 380, thickness=7, color=Colors['red'])

        with xpgl.graphicsContext():
            xpgl.setRotateTransform(-xp.getCycleNumber() * 4 % 360, 320, 400)
            xpgl.drawWideLine(320, 380, 320, 420, thickness=3, color=Colors['white'])

    with xpgl.graphicsContext():
        xpgl.setRotateTransform(-xp.getCycleNumber() % 360, 320, 75)
        xpgl.drawWideLine(320, 50, 320, 100, thickness=7, color=Colors['green'])

.. py:function:: setRotateTransform(angle, x=0, y=0) -> None

  :param float angle: Angle of rotation, in degrees, counter-clockwise.
  :param float x:                      
  :param float y: Origin, around which the rotation is performed.
  :return: None

  If the origin is not specific, the rotation will be performed around (0,0), the
  lower left corner. More commonly, you'll want to perform rotation around the center
  or one end of the *object* being displayed. In this case, you'd calculate center
  of rotation and specify it in the function call::

    def drawLine(color):
        xpgl.drawWideLine(208, 156, 432, 324, thickness=7, color=color)

    degrees = xp.getCycleNumber() * 4 % 360

    with xpgl.graphicsContext():
        xpgl.setRotateTransform(degrees, 320, 240)
        drawLine(Colors['red'])

    with xpgl.graphicsContext():
        xpgl.setRotateTransform(degrees, 0, 0)
        drawLine(Colors['white'])

    with xpgl.graphicsContext():
        xpgl.setRotateTransform(degrees, 432, 324)
        drawLine(Colors['orange'])

  For example, here the *same line* is draw three times. As a red line, it rotates
  around its center (320, 240). As a white line, it rotates around the screen
  origin (0, 0). As an orange line, it rotates around one of the line's end points (432, 324).

  .. image:: /images/xpgl_rotateOrigin.gif
        :width: 50%
           
  In the image displayed with the previous command, :py:func:`graphicsContext` above, note how the small
  white line is simultaneously subjected to two rotations due to nesting. The first
  rotation causes it to rotate with the red line, relative the mid-point on the screen.
  The second rotation causes it to rotate around its own center point. When objects
  are subjected to multiple transformations, the order in which they are performed matters.


.. py:function:: setTranslateTransform(dx=0, dy=0) -> None

  :param float dx:                 
  :param float dy: move to the right (+dx) and up (+dy)

  Translation moves an object (or set of objects) along one or both axis::

    xpgl.drawWideLine(320, 0, 320, 480, thickness=7, color=Colors['red'])

    span = 200
    offset = xp.getCycleNumber() % span
    if offset >= span / 2:
        offset = span - offset
    offset = offset - span / 4

    with xpgl.graphicsContext():
        xpgl.setTranslateTransform(offset, 0)
        xpgl.drawTriangle(320, 370, 335, 390, 305, 390)
        xpgl.drawTriangle(320, 110, 335, 90, 305, 90)
        xpgl.drawLine(320, 100, 320, 380)

  .. image:: /images/xpgl_translate.gif
      :width: 50%       

  In most cases, you could simply change the (x,y) coordinates for each object instead
  of using a translation transform, but as your drawings become more complicated, using
  translations can simplify things.

  Alternatively, you could design your objects to use (0, 0) as their center point, and
  then always use a translation to "move" the object into the correct position on the screen.


.. py:function:: translateContext(dx=0, dy=0)
                 
  :param float dx:                 
  :param float dy: move to the right (+dx) and up (+dy)

  Sometimes you'll want to perform a translation, do some work and then translate "back". For this you can
  use :py:func:`translateContext` with a ``with`` statement::

    with xpgl.translateContext(10, 10):
        xpgl.drawLine(...)

  This is identical to::

    xpgl.setTranslateTransform(10, 10)
    xpgl.drawLine(...)
    xpgl.setTranslateTransform(-10, -10)
    

.. py:function:: setScaleTransform(dx=1, dy=1, x=0, y=0) -> None

  :param float dx:                 
  :param float dy: Amount to scale in x and y directions (multiplier)                 
  :param float x:                 
  :param float y: Location of origin point for scaling

  Scaling refers to mulitiplying the size of the drawing in one or both
  directions. By default (dx, dy) == (1, 1) results in no change to scale.

  The origin point (x, y) sets the point from which the drawing is scaled.
  If the point is in the center of a object, the object will appear to grow
  (or shrink) relative its center, for x::

    span = 20
    offset = (xp.getCycleNumber() / 4) % span
    if offset >= span / 2:
        offset = span - offset

    with xpgl.graphicsContext():
        xpgl.setScaleTransform(offset, offset, 320, 240)
        xpgl.drawCircle(320, 240, 10, thickness=5, color=Colors['orange'])

  .. image:: /images/xpgl_scaleCenter.gif
             :width: 50%
    
  By default it is the screen origin (0,0) which is unlikey
  what you want::

    span = 20
    offset = (xp.getCycleNumber() / 4) % span
    if offset >= span / 2:
        offset = span - offset
    offset = offset / 5

    with xpgl.graphicsContext():
        xpgl.setScaleTransform(offset, offset)
        xpgl.drawCircle(320, 240, 10, thickness=5, color=Colors['orange'])

  .. image:: /images/xpgl_scaleOrigin.gif
             :width: 50%
