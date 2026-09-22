Retained Drawing
================


.. py:module:: XPLMPanelGraphics
    :no-index:
.. py:currentmodule:: xp

To use::

  import xp

Record a batch of panel-graphics commands once, then replay it cheaply every
frame. Useful for complex static artwork.

.. warning:: You *must* be within a PanelGraphics context in order to define or execute
             a retained drawing.

             Once you've created the retained drawing, you can store
             it, to replay over-and-over, on any window or device (with PanelGraphics context).
             You don't have to re-create the retained set on each draw.

Some rules:

* Retained Drawing *must* be created and used in :data:`WindowContentTypePanelGraphics` context -- either windows or avionics devices.

* Retained Drawing contains only Panel Graphics:
  
  .. rst-class:: compact
                 
  * :doc:`panelgraphics_primitives` (lines, polygons, quadstrips),

  * :doc:`panelgraphics_state` transformations (stencils, scissors, translation, rotation, scale) With the caveat
    that *if* the retained drawing contains a stencil, it (the retained drawing) *cannot* be used within another
    stencil: any active stencil would need to be disabled first (``useStencilMask(0, 0)``)

  * :doc:`panelgraphics_fonts` may be used, but any created Font Handles (:func:`createFont`) must still exist.

  * :doc:`panelgraphics_textures` may be used, but the Texture Atlas (:func:`createTextureAtlas`) must still exist.

  * :doc:`panelgraphics_svt` and :doc:`panelgraphics_map` cannot be used within the retained drawing

* Retained Drawing *cannot* contain another Retained Drawing.    

We describe two approaches to defining and using Retailed Drawing. First, the more pythonic-approach
using a Context Manager. Second, the C-language equivalent which maps more closely to the internal
X-Plane implementation. Both do *exactly* the same thing.

Retained Drawing Context
------------------------

By using a context manager :func:`retainedDrawing`, you'll never forget to end the context and information
will be properly cleaned up in case of exceptions.

Generally, you add your drawing list within the context, saving the result into :class:`Retained` instance:

>>> with xp.retainedDrawing() as myDraw:
...    draw1
...    draw2
...    draw3
...

Execute the list by calling ``draw()`` on the instance

>>> myDraw.draw()

.. py:function:: retainedDrawing() -> Retained

    :return: instance of Retained

    Context manager to surround a set of static drawing calls.

    Record panel-graphics commands into a retained drawing. All
    panel-graphics calls made within the context are captured instead of
    being drawn immediately. Context *must not be nested*.

    Once completed, the context manager returns an instance of :class:`Retained`, which can be used to actually draw graphics.

    This is equivalent to combining :func:`beginRetainedDrawing` and :func:`endRetainedDrawing`.

    .. py:class:: Retained

        Return value from :func:`retainedDrawing` context manager, this holds information
        about the set of drawing commands.

        .. py:attribute:: handle
            :type: XPLMRetainedDrawing

            Contains set of drawing instructions.

        .. py:method:: draw()
                       
            Executes the set of drawing commands stored in the :class:`Retained` instance. Internally
            it simply calls ``drawRetained(MyRetained.handle)``.

        .. py:method:: destroy()

            Removes drawing information stored in :attr:`handle`. Internally this is identical to
            ``destroyRetainedDrawing(MyRetained.handle)``

For example, we'll globally store our draw list into ``MyRetained``, so the creation occurs only
once. It needs to be created within a window or avionics device draw callback with 
:data:`WindowContentTypePanelGraphics`.

First, we create the avionics device version:

>>> MyRetained = None
>>> def myScreenDraw(refCon):
...     global MyRetained
...     rect = [(0, 0), (50, 0), (50, 30), (0, 30)]
...     if MyRetained is None:
...         with xp.retainedDrawing() as MyRetained:
...             xp.polygon(xp.makeColor(), rect)
...             xp.lineLoop(xp.makeColor(0, 0, 0, 1), rect)
...     with xp.transformContext():
...         xp.transformTranslate(100, 50)
...         xp.polygon(xp.makeColor(1, 0, 1, 1), rect)
...         with xp.transformContext():
...             xp.transformRotate(0, 0, 10)
...             MyRetained.draw()
...             xp.transformRotate(0, 0, 20)
...             MyRetained.draw()
...     
>>> def myBezelDraw(r, g, b, refCon):
...     xp.polygon(xp.makeColor(0, 0, 0, 1), [(0, 0), (200, 0), (200, 100), (0, 100)])
...
>>> avionicsID = xp.createAvionicsEx(screenDraw=myScreenDraw, bezelDraw=myBezelDraw,
...                                  screenWidth=200, screenHeight=100, bezelWidth=200, bezelHeight=100,
...                                  screenOffsetX=0, screenOffsetY=0,
...                                  contentType=xp.WindowContentTypePanelGraphics, windowWithChrome=1)
...
>>> xp.setAvionicsPopupVisible(avionicsID)

.. image:: /images/panel_retained_avionics.png
           :width: 300px

Then, *using the same RetainedDrawing* we'll create a display window version. It's
a bit more complicated because we want to handle window scaling and translation.

>>> def myDraw(windowID, refCon):
...     if MyRetained is None: return
...     l, t, r, b = xp.getWindowGeometry(windowID)
...     scaleX = ((r-l) / 200.)
...     scaleY = ((t-b) / 100.)
...     rect = [(0, 0), (50, 0), (50, 30), (0, 30)]
...     with xp.transformContext():
...         xp.transformTranslate((l+r)/2, (t+b)/2)
...         xp.transformScale(scaleX, scaleY)
...         xp.polygon(xp.makeColor(1, 0, 1, 1), rect)
...         with xp.transformContext():
...            xp.transformRotate(0, 0, 10)
...            MyRetained.draw()
...            xp.transformRotate(0, 0, 20)
...            MyRetained.draw()
...
>>> winID = xp.createWindowEx(draw=myDraw, visible=1,
...                           left=100, top=400, right=300, bottom=300,
...                           contentType=xp.WindowContentTypePanelGraphics)
...
    
.. image:: /images/panel_retained_window.png
           :width: 300px

(You may notice the avionics version is less sharp than the windows version. This is due to the way avionics device windows are displayed and
scaled and has nothing to do with retained drawing.)

Classic Retained Functions
--------------------------

These functions more closely match Laminar's C-API. Use them if you like, or use the context manager above instead.

See ``samples/PI_PGRetained.py`` for a working example using these functions:
it records one motif, replays it across several tiles under different
transforms, and draws the same motif in immediate mode as a reference to
compare against.

.. py:function:: beginRetainedDrawing() -> None
    
    Begin recording panel-graphics commands into a retained drawing. All
    panel-graphics calls made until :func:`endRetainedDrawing` are captured
    instead of being drawn immediately. Recording sessions must not be nested.
    
    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMBeginRetainedDrawing>`__ :index:`XPLMBeginRetainedDrawing`
    
.. py:function:: endRetainedDrawing() -> XPLMRetainedDrawing
    
    :return: :class:`XPLMRetainedDrawing` capsule
    
    End the recording started by :func:`beginRetainedDrawing` and return an opaque
    handle to the captured commands. Replay it with :func:`drawRetained` and free
    it with :func:`destroyRetainedDrawing`.
    
    .. caution:: If a font or texture atlas used while recording is later
       destroyed, you must destroy the retained drawing as well --- replaying it
       against a freed resource is undefined.
    
    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMEndRetainedDrawing>`__ :index:`XPLMEndRetainedDrawing`
    
.. py:function:: drawRetained(drawing) -> None

    :param XPLMRetainedDrawing drawing: Handle from :func:`endRetainedDrawing`

    Replay a retained drawing. May be called any number of times per frame and
    across frames to redraw the same commands.

    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMDrawRetained>`__ :index:`XPLMDrawRetained`

.. py:function:: destroyRetainedDrawing(drawing) -> None

    :param XPLMRetainedDrawing drawing: Handle from :func:`endRetainedDrawing`

    Destroy a retained drawing and free its resources. The handle must not be used
    after this call.

    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMDestroyRetainedDrawing>`__ :index:`XPLMDestroyRetainedDrawing`

Types
-----

.. py:class:: XPLMRetainedDrawing

    Opaque capsule representing a recorded set of panel-graphics commands, as
    returned by :func:`endRetainedDrawing`. Release it with
    :func:`destroyRetainedDrawing`.

    Using the :func:`retainedDrawing` context manager instead, the capsule is
    stored as :attr:`Retained.handle`.
