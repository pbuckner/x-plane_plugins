Touch Zones
===========

.. py:module:: XPLMPanelGraphics
    :no-index:
.. py:currentmodule:: xp

To use::

  import xp

Touch zones make regions of an avionics device or window interactive (new with X-Plane 12.4.4,
SDK 440). Register the zones you want *every frame* from your drawing callback
with :func:`accumulateTouchZone`; a zone can do nothing, fire a command, or
deliver touch events to a handler you register with
:func:`avionicsSetTouchEventHandler` / :func:`windowSetTouchEventHandler`. Use
these from a panel-graphics drawing context --- see :doc:`panelgraphics`.

See ``samples/PI_PGTouch.py`` for a working example: it registers an
Identifier zone over a drawn button each frame, colors the button by the
returned held-state, and displays the fields of each touch event as it
arrives.

Functions
---------

.. py:function:: accumulateTouchZone(type, left, top, right, bottom, command=None, identifier=0) -> bool

    :param int type: One of :data:`TouchZone_Nothing`, :data:`TouchZone_Command`, :data:`TouchZone_Identifier`
    :param int left: Left edge, in the coordinates you are drawing in
    :param int top: Top edge
    :param int right: Right edge
    :param int bottom: Bottom edge
    :param command: For ``TouchZone_Command``, the command to fire on press/release (default: None)
    :type command: Optional[XPLMCommandRef]
    :param int identifier: For ``TouchZone_Identifier``, the id delivered to the touch handler (default: 0)
    :return: True while the zone is being pressed/held (use it for pressed-state feedback)

    Register an interactive touch zone for the current frame. Call this every frame
    from your drawing callback for each region. Zones registered later win
    overlaps. (That is, overlapping zones will *not* result in multiple callbacks.)

    A touch zone rides the transform stack, exactly like the drawing it sits on
    top of. Declare the zone in the same coordinates you drew in and X-Plane
    applies the transform in force for you --- **do not** offset or scale the
    rectangle yourself, or the transform is applied twice. Draw a button and put
    a zone on it using the same numbers, under any combination of translations
    and scales, and the two stay together. This holds for windows and avionics
    devices alike. See the example at the bottom of this page.

    Two limits follow from a zone being an axis-aligned rectangle:

    .. rst-class:: compact

    * Do not declare a zone while a rotation is in effect. An axis-aligned
      rectangle cannot describe a rotated graphic, so this is an error --- and
      it is decided by whether you called :func:`transformRotate` at all, not by
      the angle, so a rotation of zero degrees still counts.
    * A zone cannot be declared inside a :func:`beginRetainedDrawing` recording.
      A zone is per-frame state rather than drawing, and a retained drawing holds
      drawing only. Accumulate your zones outside the recording, once per frame;
      they are cheap to re-declare and are meant to be re-declared.

    .. rst-class:: compact
                   
    * :data:`TouchZone_Command` fires *command* on press and release.
    * :data:`TouchZone_Identifier` delivers events to the handler set with
      :func:`avionicsSetTouchEventHandler` / :func:`windowSetTouchEventHandler`,
      tagged with *identifier*.

    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMAccumulateTouchZone>`__ :index:`XPLMAccumulateTouchZone`

.. py:function:: avionicsSetTouchEventHandler(avionic, handler, refCon=None) -> None

    :param XPLMAvionicsID avionic: An avionics device (see :func:`createAvionicsEx`)
    :param handler: Callback ``f(identifier, status, x, y, dx, dy, button, refCon)``, or None to remove
    :type handler: Optional[Callable]
    :param Any refCon: Reference constant passed to *handler* (default: None)

    Register *handler* to receive touch events for :data:`TouchZone_Identifier`
    zones on the given avionics device. Pass ``handler=None`` to remove the current
    handler. See :func:`touchHandler` for the callback signature.

    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMAvionicsSetTouchEventHandler>`__ :index:`XPLMAvionicsSetTouchEventHandler`

.. py:function:: windowSetTouchEventHandler(window, handler, refCon=None) -> None

    :param XPLMWindowID window: A window (see :func:`createWindowEx`)
    :param handler: Callback ``f(identifier, status, x, y, dx, dy, button, refCon)``, or None to remove
    :type handler: Optional[Callable]
    :param Any refCon: Reference constant passed to *handler* (default: None)

    Register *handler* to receive touch events for :data:`TouchZone_Identifier`
    zones on the given window. Pass ``handler=None`` to remove the current handler.

    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMWindowSetTouchEventHandler>`__ :index:`XPLMWindowSetTouchEventHandler`

Callback
--------

.. py:function:: touchHandler(identifier, status, x, y, dx, dy, button, refCon) -> None

    :param int identifier: The *identifier* of the touched zone (from :func:`accumulateTouchZone`)
    :param int status: Touch phase (press / move / release)
    :param int x: Touch X, in the coordinates the zone was declared in
    :param int y: Touch Y, in the same coordinates
    :param int dx: X delta from the initial click point
    :param int dy: Y delta from the initial click point
    :param int button: 0 for left, 1 for right
    :param refCon: The *refCon* you registered with SetTouchEvent, *not with window or device* refCon

    The handler you pass to :func:`avionicsSetTouchEventHandler` /
    :func:`windowSetTouchEventHandler`. (This is the function signature -- you don't have to call is ``touchHandler``.)

Constants
---------

.. _XPLMTouchZone:

XPLMTouchZone
*************

The kind of a touch zone, passed as *type* to :func:`accumulateTouchZone`. 

.. py:data:: TouchZone_Nothing
   :value: 0

   A passive zone --- consumes the touch but takes no action (e.g. to block
   click-through). |BR|
   `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#xplm_TouchZone_Nothing>`__: :index:`xplm_TouchZone_Nothing`

.. py:data:: TouchZone_Command
   :value: 1

   Fires the given command on press and release.  |BR|
   `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#xplm_TouchZone_Command>`__: :index:`xplm_TouchZone_Command`

.. py:data:: TouchZone_Identifier
   :value: 2

   Delivers touch events (tagged with *identifier*) to your registered touch
   handler. |BR|
   `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#xplm_TouchZone_Identifier>`__: :index:`xplm_TouchZone_Identifier`

Example
-------

Putting together all the ideas, the following code creates an avionics device and draws
a blue button within it. While mouse is press, the button changes to magenta (even as the
mouse moves off of the button).

    >>> def myScreen(refCon):
    ...     ret = xp.accumulateTouchZone(xp.TouchZone_Identifier, left=10, top=30, right=50, bottom=10, command=None, identifier=42)
    ...     rect = [(10, 10), (50, 10), (50, 30), (10, 30)]
    ...     if ret:
    ...         xp.polygon(xp.makeColor(1, 0, 1, 1), rect)
    ...     else:
    ...         xp.polygon(xp.makeColor(0, 0, 1, 1), rect)
    ...
    >>> def myBezel(r, g, b, refCon):
    ...     xp.polygon(xp.makeColor(0, 0, 0, 1), [(0, 0), (0, 200), (300, 200), (300, 0)])
    ...
    >>> avionicsID = xp.createAvionicsEx(300, 200, 300, 200, 0, 0,
    ...                 screenDraw=myScreen, bezelDraw=myBezel,
    ...                 contentType=xp.WindowContentTypePanelGraphics)
    ...
    >>> def touchHandler(identifier, status, x, y, dx, dy, button, refCon):
    ...     xp.log(f"Touch region #{identifier}, {status=} at ({x}, {y}) with button #{button}")
    ...
    >>> xp.avionicsSetTouchEventHandler(avionicsID, touchHandler)
    >>> xp.setAvionicsPopupVisible(avionicsID)

Putting together all the ideas, the following code creates a window and draws a blue button within it.
Click on the box, and you'll get log a message.

Logs:

    .. code-block:: none

       Touch region #42, status=1 at (24, 13) with button #0
       Touch region #42, status=3 at (24, 13) with button #0

Showing initial :data:`MouseDown`, followed with :data:`MouseUp`,
using button #0 (Left). Button #1 is Right; center and wheel deliver no event
at all.

The same button in a window, drawn inside a transform so it scales with the
window. Note that the zone is declared with the *same* numbers the button is
drawn with --- X-Plane runs the transform stack over the rectangle for you, and
hands the results back to your handler in that same space, so there is no
geometry arithmetic to do on either side:

.. code-block:: python
                
    >>> def myDraw(windowID, refCon):
    ...     left, top, right, bottom = xp.getWindowGeometry(windowID)
    ...     rect = [(10, 10), (50, 10), (50, 30), (10, 30)]
    ...     with xp.transformContext():
    ...         xp.transformTranslate(left, bottom)
    ...         xp.transformScale((right - left) / 200., (top - bottom) / 100.)
    ...         ret = xp.accumulateTouchZone(xp.TouchZone_Identifier,
    ...                   left=10, top=30, right=50, bottom=10,
    ...                   command=None, identifier=42)
    ...         if ret:
    ...             xp.polygon(xp.makeColor(1, 0, 1, 1), rect)
    ...         else:
    ...             xp.polygon(xp.makeColor(0, 0, 1, 1), rect)
    ...
    >>> winID = xp.createWindowEx(draw=myDraw, visible=1,
    ...                           left=100, top=400, right=300, bottom=300,
    ...                           contentType=xp.WindowContentTypePanelGraphics)
    ...
    >>> def touchHandler(identifier, status, x, y, dx, dy, button, refCon):
    ...     xp.log(f"Touch region #{identifier}, {status=} at ({x}, {y}) with button #{button}")
    ...
    >>> xp.windowSetTouchEventHandler(winID, touchHandler, winID)

Logs:

    .. code-block:: none

       Touch region #42, status=1 at (24, 13) with button #0
       Touch region #42, status=2 at (24, 13) with button #0
       Touch region #42, status=2 at (24, 13) with button #0
       Touch region #42, status=2 at (24, 13) with button #0
       Touch region #42, status=2 at (24, 13) with button #0
       Touch region #42, status=2 at (24, 13) with button #0
       Touch region #42, status=3 at (24, 13) with button #0

Showing initial :data:`MouseDown`, a few (non-moving) :data:`MouseDrag` while the mouse was pressed, ending with :data:`MouseUp`,
using button #0 (Left). Button #1 is Right; center and wheel deliver no event at all.

The logged coordinates are in the 200x100 space the button was drawn and the
zone declared in, whatever size the user has dragged the window to. Resize the
window and the numbers do not change.

.. note:: On an avionics device, ``MouseUp`` is reported at (0, 0) and
          ``MouseDrag`` is not delivered, so a drag cannot be tracked there the
          way it can in a window. Reported to Laminar as
          `XPD-18371 <https://developer.x-plane.com/x-plane-bug-database/?issue=XPD-18371>`_.
