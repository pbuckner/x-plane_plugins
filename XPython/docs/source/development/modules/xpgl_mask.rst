xpgl Mask and Stencil
=====================

.. py:module:: xpgl
  :noindex:
     
To use::

  from XPPython3 import xpgl

These routines allow you to display only a part of what you draw. There are two
common uses cases for this:

* **Mask**: Draw an object, but leave a "hole" in it. For example, composing a donut
  by drawing a large circle, but masking "out" a circlur interior section. (Think, "masking tape"
  used to exclude getting paint in unwanted locations.)

* **Stencil** Make visible only a portion of an object. For example, drawing a whole image, but
  only having part of it visible. (Think, "paper stencil" used with spray paint to form
  letters or shapes.)

In either case, you'll first draw a "shape" which *will not be displayed*. This shape will
form the mask or stencil. Then, you'll draw your visuals. These portions of these
will be made visible depending on the previously defined mask or stencil.

Similar to transformations (:doc:`xpgl_transformations`) you'll bracket
all this drawing with :py:func:`drawMaskStart` and :py:func:`drawMaskEnd` or use
a context manager :py:func:`maskContext`.

.. py:function:: maskContext() -> None
.. py:function:: drawMaskStart() -> None                 
.. py:function:: drawMaskEnd() -> None                 

  :return: None
  
.. py:function:: drawUnderMask(stencil=False) -> None

  :param bool stencil: by default (stencil=False), EXCLUDE image under mask shape, otherwise make visible ONLY under stencil shape
  :return: None

  Call :py:func:`drawMaskStart` immediately before you want to *define the mask or stencil*, and :py:func:`drawMaskEnd` after you've
  completed *drawing* the stencil/mask and the underlying image. Alternatively, using :py:func:`maskContext` context manager.

  You'll be drawing two shapes (two sets of shapes). The first set of shapes define the stencil or mask, that is, the
  *shape* of the final part to be made visible or excluded. The second set of shapes define the underlying image itself which
  will be visible/invisible based on mask/stencil.

  These two sections are delineated by the call to :py:func:`drawUnderMask`.
  Depending on the value of that function's *stencil* parameter, the first shape(s) and the second shape(s) are combined.

  The sequence is:

  .. code:: none

    drawMaskStart()
    <draw stencil/mask>
    drawUnderMask()
    <draw images>
    drawMaskEnd()
    
  -or-:

  .. code:: none

    with maskContext():
        <draw stencil/mask>
        drawUnderMask()
        <draw images>

  You *must* pair each :py:func:`drawMaskStart` with :py:func:`drawMaskEnd` and
  they should not be nested.

  For example, we'll draw a large green triangle, a large orange circle and a
  small white circle. Without a mask this is shown in the first panel on the left, below.::
  
    xpgl.drawTriangle(320, 440, 500, 40, 140, 40, color=Colors['green'])
    xpgl.drawCircle(320, 240, 200, isFilled=True, color=Colors['orange'])
    xpgl.drawCircle(320, 240, 50, isFilled=True)

  Setting the triangle
  as *stencil*, by calling :py:func:`drawUnderMask` with stencil=True, the triangle (regardless
  of its color) now specifies the portion of the orange circle to expose (middle panel).
  Finally, set *stencil* to False and the triangle mask now describes the part of the
  orange circle to *not* draw.
  
  .. image:: /images/xpgl_drawMask.png

  Note the smaller white circle is always draw as it is unaffected by the mask.::

    with xpgl.maskContext():
        xpgl.drawTriangle(320, 440, 500, 40, 140, 40, color=Colors['green'])
        xpgl.drawUnderMask()  # (set to stencil=True for 2nd panel)
        xpgl.drawCircle(320, 240, 200, isFilled=True, color=Colors['orange'])
    xpgl.drawCircle(320, 240, 50, isFilled=True)
