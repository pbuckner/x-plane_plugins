Window API
==========

.. py:module:: XPLMDisplay
    :no-index:
.. py:currentmodule:: xp       

To use::

  import xp

The window API provides a high-level abstraction for drawing with UI interaction.

Windows are created with :py:func:`createWindowEx`, and have access to X-Plane 11 windowing
features, like support for new positioning modes (:py:func:`setWindowPositioningMode`)
including being "popped out" into their own first-class
window in the operating system. They can also optionally be decorated in the style of X-Plane 11 windows
(like the map). (The pre X-Plane 11.50 function XPLMCreateWindow is deprecated.)

Windows operate in "boxel" units. A boxel ("box of pixels") is a unit of virtual pixels which,
depending on X-Plane's scaling, may correspond to an arbitrary NxN "box" of real pixels on screen.
Because X-Plane handles this scaling automatically, you can effectively treat the units as though you
where simply drawing in pixels, and know that when X-Plane is running with 150% or 200% scaling, your
drawing will be automatically scaled (and likewise all mouse coordinates, screen bounds, etc. will also be auto-scaled).

Modern windows are not constrained to the main window, they have their origin in the lower left of the entire
global desktop space, and the lower left of the main X-Plane window is not guaranteed to
be (0, 0). X increases as you move right, and Y increases as you move up.

Note that this requires dealing with your window’s bounds in “global desktop” positioning
units, rather than the traditional panel coordinate system. In global desktop coordinates,
the main X-Plane window may not have its origin at coordinate (0, 0), and your own window
may have negative coordinates. Assuming you don’t implicitly assume (0, 0) as your origin,
the only API change you should need is to start using :py:func:`getMouseLocationGlobal` rather
than (deprecated) XPLMGetMouseLocation(), and :py:func:`getScreenBoundsGlobal` instead
of (deprecated) XPLMGetScreenSize().

For more information and examples on window positioning, and determining monitor bounds, see :doc:`/development/window_position`.

Note: There is no API or callback for window close: When the user closes your window, you'll simply
not get any more draw callbacks.

In addition to the basic functions :py:func:`createWindowEx` and :py:func:`destroyWindow`, functions include:

* Obtain and manipulate window geometry:

  * :py:func:`getWindowGeometry`, :py:func:`setWindowGeometry`
  * :py:func:`getWindowGeometryOS`, :py:func:`setWindowGeometryOS`
  * :py:func:`getWindowGeometryVR`, :py:func:`setWindowGeometryVR`
  * :py:func:`windowIsPoppedOut`
  * :py:func:`windowIsInVR`

* Change window visibility and keyboard focus:

  * :py:func:`getWindowIsVisible`, :py:func:`setWindowIsVisible`
  * :py:func:`takeKeyboardFocus`, :py:func:`hasKeyboardFocus`
  * :py:func:`bringWindowToFront`, :py:func:`isWindowInFront`

* Change window resize and positioning:

  * :py:func:`setWindowGravity`
  * :py:func:`setWindowResizingLimits`
  * :py:func:`setWindowPositioningMode`

* Change window attributes:

  * :py:func:`setWindowTitle`
  * :py:func:`getWindowRefCon`, :py:func:`setWindowRefCon`

Window Drawing Functions
************************

.. py:function:: createWindowEx(left=100, top=200, right=200, bottom=100, visible=0, draw=None, click=None, key=None, cursor=None, wheel=None, refCon=None, decoration=WindowDecorationRoundRectangle, layer=WindowLayerFloatingWindows, rightClick=None)

 Create a new "modern" window.

 :param left: Left edge of window in boxels (default: 100)
 :type left: int
 :param top: Top edge of window in boxels (default: 200)
 :type top: int
 :param right: Right edge of window in boxels (default: 200)
 :type right: int
 :param bottom: Bottom edge of window in boxels (default: 100)
 :type bottom: int
 :param visible: 1 for visible, 0 for hidden (default: 0)
 :type visible: int
 :param draw: Draw callback function (default: None)
 :type draw: Optional[Callable[[:data:`XPLMWindowID`, Any], None]]
 :param click: Left-click callback function (default: None)
 :type click: Optional[Callable[[:data:`XPLMWindowID`, int, int, int, Any], int]]
 :param key: Keyboard callback function (default: None)
 :type key: Optional[Callable[[:data:`XPLMWindowID`, int, int, int, Any, int], None]]
 :param cursor: Cursor callback function (default: None)
 :type cursor: Optional[Callable[[:data:`XPLMWindowID`, int, int, Any], int]]
 :param wheel: Mouse wheel callback function (default: None)
 :type wheel: Optional[Callable[[:data:`XPLMWindowID`, int, int, int, int, Any], int]]
 :param refCon: Reference constant passed to callbacks (default: None)
 :type refCon: Any
 :param decoration: Window decoration style (default: WindowDecorationRoundRectangle)
 :type decoration: int
 :param layer: Window layer (default: WindowLayerFloatingWindows)
 :type layer: int
 :param rightClick: Right-click callback function (default: None)
 :type rightClick: Optional[Callable[[:data:`XPLMWindowID`, int, int, int, Any], int]]
 :return: WindowID for the created window
 :rtype: :data:`XPLMWindowID`

 Window style is indicated by *decoration*, and can only be specified at creation time. By default, window is created
 as ``WindowDecorationRoundRectangle`` and looks like:

  .. image:: /images/small_round_rect_window.png
             
 *decoration* must be one of:

 +--------------------------------------------------+-----------------------------------------------------------------------------------------+
 | Decoration Value                                 | Meaning                                                                                 |
 +==================================================+=========================================================================================+
 | .. data:: WindowDecorationNone                   | X-Plane will draw no decoration for your window, and apply no automatic                 |
 |    :value: 0                                     | click handlers. The window will not stop click from passing through its                 |
 |                                                  | bounds. This is suitable for "windows" which request, say, the full screen              |
 |                                                  | bounds, then only draw in a small portion of the available area.                        |
 |                                                  |                                                                                         |
 |                                                  | `Official SDK                                                                           |
 |                                                  | <https://developer.x-plane.com/sdk/XPLMDisplay/#xplm_WindowDecorationNone>`__           |
 |                                                  | :index:`xplm_WindowDecorationNone`                                                      |  
 +--------------------------------------------------+-----------------------------------------------------------------------------------------+
 | .. data:: WindowDecorationRoundRectangle         | The default decoration for "native" windows, like the map.                              |
 |    :value: 1                                     | Provides a solid background, as well as click handlers for resizing                     |
 |                                                  | and dragging the window.                                                                |
 |                                                  |                                                                                         |
 |                                                  | `Official SDK                                                                           |
 |                                                  | <https://developer.x-plane.com/sdk/XPLMDisplay/#xplm_WindowDecorationRoundRectangle>`__ |
 |                                                  | :index:`xplm_WindowDecorationRoundRectangle`                                            |
 +--------------------------------------------------+-----------------------------------------------------------------------------------------+  
 | .. data:: WindowDecorationSelfDecorated          | X-Plane will draw no decoration for your window, nor will it provide resize             |
 |    :value: 2                                     | handlers for your window edges, but it will stop clicks from passing through            |
 |                                                  | your windows bounds.                                                                    |
 |                                                  |                                                                                         |
 |                                                  | `Official SDK                                                                           |
 |                                                  | <https://developer.x-plane.com/sdk/XPLMDisplay/#xplm_WindowDecorationSelfDecorated>`__  |
 |                                                  | :index:`xplm_WindowDecorationSelfDecorated`                                             |
 +--------------------------------------------------+-----------------------------------------------------------------------------------------+
 | .. data:: WindowDecorationSelfDecoratedResizable | Like self-decorated, but with resizing; X-Plane will draw no decoration for             |  
 |    :value: 3                                     | your window, but it will stop clicks from passing through your windows bounds,          | 
 |                                                  | and provide automatic mouse handlers for resizing.                                      | 
 |                                                  |                                                                                         |
 |                                                  | `Official SDK                                                                           |
 |                                                  | <https://developer.x-plane.com/sdk/XPLMDisplay/#xplm_WindowDecorationNone>`__           |
 |                                                  | :index:`xplm_WindowDecorationNone`                                                      |
 +--------------------------------------------------+-----------------------------------------------------------------------------------------+
 
 *layer* describes where in the ordering of windows X-Plane should place this window.
 Windows in higher layers cover windows in lower layers. So, a given window might be at the top of its particular layer, but it might
 still be obscured by a window in a higher layer. (This happens frequently when
 floating windows, like X-Plane’s map, are covered by a modal alert.) Layer is specified only at
 creation time, and is one of:

 .. _window-layer:

 :index:`XPLMWindowLayer`
        
 +---------------------------------------------+----------------------------------------------------------------------------------------+
 | Layer Value                                 | Meaning                                                                                |
 +=============================================+========================================================================================+
 | .. py:data:: WindowLayerFlightOverlay       | The lowest layer, used for HUD-like displays while flying.                             |
 |   :value: 0                                 |                                                                                        |
 |                                             | `Official SDK                                                                          |
 |                                             | <https://developer.x-plane.com/sdk/XPLMDisplay/#xplm_WindowLayerFlightOverlay>`__      |
 |                                             | :index:`xplm_WindowLayerFlightOverlay`                                                 |
 +---------------------------------------------+----------------------------------------------------------------------------------------+
 | .. py:data:: WindowLayerFloatingWindows     | Windows that "float" over the sim, like the X-Plane 11 map does. If you are not sure   |
 |   :value: 1                                 | which layer to create your window in, choose floating                                  |
 |                                             |                                                                                        |
 |                                             | `Official SDK                                                                          |
 |                                             | <https://developer.x-plane.com/sdk/XPLMDisplay/#xplm_WindowLayerFloatingWindows>`__    |
 |                                             | :index:`xplm_WindowLayerFloatingWindows`                                               |
 +---------------------------------------------+----------------------------------------------------------------------------------------+
 | .. py:data:: WindowLayerModal               | An interruptive modal that covers the sim with a                                       |    
 |   :value: 2                                 | transparent black overlay to draw the user's focus to the                              |
 |                                             | alert.                                                                                 |
 |                                             |                                                                                        |
 |                                             | `Official SDK                                                                          |
 |                                             | <https://developer.x-plane.com/sdk/XPLMDisplay/#xplm_WindowLayerModal>`__              |
 |                                             | :index:`xplm_WindowLayerModal`                                                         |
 |                                             |                                                                                        |
 +---------------------------------------------+----------------------------------------------------------------------------------------+
 | .. py:data:: WindowLayerGrowlNotifications  | "Growl"-style notifications that are visible in a corder of the screen, even over      |
 |    :value: 3                                | modals.                                                                                |
 |                                             |                                                                                        |
 |                                             | `Official SDK                                                                          |
 |                                             | <https://developer.x-plane.com/sdk/XPLMDisplay/#xplm_WindowLayerGrowlNotifications>`__ |
 |                                             | :index:`xplm_WindowLayerGrowlNotifications`                                            |
 +---------------------------------------------+----------------------------------------------------------------------------------------+

 >>> windowID = xp.createWindowEx(visible=1)

 .. image:: /images/small_round_rect_window.png

 There are six possible callback functions to be provided.

 .. table::
    :align: left

    +--------------------------------------------------------+--------------------+
    | Callback function signature                            | Returns            |
    +========================================================+====================+
    |draw(windowID, refCon)                                  | No return          |
    +--------------------------------------------------------+--------------------+
    |key(windowID, key, flags, vKey, refCon, losingFocus)    | No return          |
    +--------------------------------------------------------+--------------------+
    |cursor(windowID, x, y, refCon)                          |cursorStatus        |
    +--------------------------------------------------------+--------------------+
    |click(windowID, x, y, mouseStatus, refCon)              | 1 =Consume click   |
    |                                                        +--------------------+
    |                                                        | 0 =Pass it through |
    +--------------------------------------------------------+--------------------+
    |wheel(windowID, x, y, wheel, clicks, refCon)            | 1 =Consume click   |
    |                                                        +--------------------+
    |                                                        | 0 =Pass it through |
    +--------------------------------------------------------+--------------------+
    |rightClick(windowID, x, y, mouseStatus, refCon)         | 1 =Consume click   |
    |                                                        +--------------------+
    |                                                        | 0 =Pass it through |
    +--------------------------------------------------------+--------------------+
     

 
 For legacy purposes, you may pass a 14-element tuple *instead of* individually specifying
 the parameters.

 The tuple is:
 
 ::
    
     (
       left, top, right, bottom,
       visible,
       draw,
       click,
       key,
       cursor,
       wheel,
       refCon,
       decoration,
       layer,
       rightClick
     )
    
 Note the order is very important!
 
 You pass in a tuple with all of the fields set in.

 >>> t = (100, 200, 200, 100, 1,
 ...      None, None, None, None, None,
 ...      None,
 ...      xp.WindowDecorationRoundRectangle, xp.WindowLayerFloatingWindows,
 ...      None)
 ...
 >>> windowID = xp.createWindowEx(t)

 .. image:: /images/small_round_rect_window.png

 `Official SDK <https://developer.x-plane.com/sdk/XPLMDisplay/#XPLMCreateWindowEx>`__ :index:`XPLMCreateWindowEx`

.. py:function:: destroyWindow(windowID)

 Destroys a window based on the handle passed in.

 :param windowID: Window to be destroyed
 :type windowID: :data:`XPLMWindowID`
 :return: None

 The callbacks are not called after this call. Keyboard focus is removed
 from the window before destroying it.

 `Official SDK <https://developer.x-plane.com/sdk/XPLMDisplay/#XPLMDestroyWindow>`__ :index:`XPLMDestroyWindow`


Window Drawing Callbacks
************************

These are the callbacks you'll provide and pass into :py:func:`createWindowEx` when creating
the window, or using the tuple.

.. py:function:: draw(windowID, refCon)
 
  Window drawing callback prototype.
 
  :param windowID: window to be drawn
  :param refCon: refCon you provided on creation
  :return: No return value
 
  This function handles drawing. You are passed in your windowID and its
  refCon. Draw the window. You can use :py:func:`getWindowGeometry`
  to find its current dimensions.  When this callback is called, the OpenGL
  context will be set properly for cockpit drawing. NOTE: Because you are
  drawing your window over a background, you can make a transparent window
  easily by simply not filling in your entire window's bounds.              
 
  >>> def MyDraw(windowID, refCon):
  ...     (left, top, right, bottom) = xp.getWindowGeometry(windowID)
  ...     xp.drawString([1, 0, 0], left + 10, top - 10, refCon, None, xp.Font_Basic)
  ...
  >>> phrase = "Hello"
  >>> windowID = xp.createWindowEx(visible=1, draw=MyDraw, refCon=phrase)
  >>>
  >>> xp.destroyWindow(windowID)
  
.. py:function::  click(windowID, x, y, mouseStatus, refCon)
                  rightClick(windowID, x, y, mouseStatus, refCon)
 
  Mouse handling callback prototype. Same signature
  for Left-clicks and Right-clicks. (Note if you do use the same callback for both
  right and left clicks, you cannot determine from the parameters if you are
  being called due to a right or left click. For this reason, you might want to
  use two different functions.)
 
  :param windowID: window receiving the mouse click
  :param x: horizontal position of mouse
  :param y: vertical position of mouse
  :param mouseStatus: flag, see table below.
  :param refCon: refCon you provided on creation
  :return: 1= consume the click, or 0= to pass it through.
 
  You receive this call when the mouse button is pressed down or released.
  Between then these two calls is a drag.  You receive the x and y of the
  click, your window,  and a refcon.  Return 1 to consume the click, or 0 to
  pass it through.
 
  .. warning:: passing clicks through windows (as of this writing) causes mouse
     tracking problems in X-Plane; do not use this feature!
 
  When the mouse is clicked, your mouse click routine is called repeatedly.
  It is first called with the mouse down message.  It is then called zero or
  more times with the mouse-drag message, and finally it is called once with
  the mouse up message.  All of these messages will be directed to the same
  window.
  
  >>> def MyDraw(windowID, refCon):
  ...     if refCon:
  ...        (left, top, right, bottom) = xp.getWindowGeometry(windowID)
  ...        xp.drawString([1, 0, 0], left + 10, top - 10, refCon, None, xp.Font_Basic)
  ...
  >>> def MyClick(windowID, x, y, mouseStatus, refCon):
  ...     status = "Down" if mouseStatus == xp.MouseDown else "Up" if mouseStatus == xp.MouseUp else "Drag"
  ...     xp.setWindowRefCon(windowID, f"({x}, {y}): {status}")
  ...     return 1  # Remember to return a value!!
  ...
  >>> windowID = xp.createWindowEx(visible=1, click=MyClick, draw=MyDraw)
  >>>
  >>> xp.destroyWindow(windowID)
  
  .. table::
     :align: left

     +---------------------+-----------------------+
     | Mouse Status Value  | SDK Value             |
     +=====================+=======================+
     |.. data:: MouseDown  |:index:`xplm_MouseDown`|
     | :no-index:          |                       |
     | :value: 1           |                       |
     +---------------------+-----------------------+
     |.. data:: MouseDrag  |:index:`xplm_MouseDrag`|
     | :no-index:          |                       |
     | :value: 2           |                       |
     +---------------------+-----------------------+
     |.. data:: MouseUp    |:index:`xplm_MouseUp`  |
     | :no-index:          |                       |
     | :value: 3           |                       |
     +---------------------+-----------------------+
        
  .. note:: It appears only *MouseDown* is ever sent: You will never receive *MouseDrag* or *MouseUp* events.
            Also, be aware that edges of popups are reserved for X-Plane window manipulation (i.e., resize and
            move) and *you will not receive events* near the edges. This includes what might be considered the
            "title bar", the 25 pixels or so at the top of the popup window.
              
.. py:function:: key(windowID, key, flags, vKey, refCon, losingFocus)

  Window keyboard input handling callback prototype.
 
  :param windowID: window receiving the key press or focus
  :param key: Key pressed
  :param flags: OR'd values for Shift / Ctrl, etc. See table below
  :param vKey: Virtual key code (:ref:`Virtual Key Codes`)
  :param refCon: refCon you provided on creation
  :param losingFocus: 1= your window is losing keyboard focus (and key should be ignored)
  :return: No return value
 
  This function is called when a key is pressed or keyboard focus is taken
  away from your window.  If losingFocus is 1, you are losing the keyboard
  focus, otherwise a key was pressed and *key* contains its character.
  
  >>> def MyDraw(windowID, refCon):
  ...     if refCon:
  ...        (left, top, right, bottom) = xp.getWindowGeometry(windowID)
  ...        xp.drawString([1, 0, 0], left + 10, top - 10, refCon, None, xp.Font_Basic)
  ...
  >>> def MyKey(windowID, key, flags, vKey, refCon, losingFocus):
  ...     if losingFocus:
  ...         status = "Lost Focus"
  ...     elif flags & xp.DownFlag:
  ...         status = ['Key Down', ]
  ...         if flags & xp.ShiftFlag:
  ...            status.append("Shift")
  ...         if flags & xp.OptionAltFlag:
  ...            status.append("Option")
  ...         if flags & xp.ControlFlag:
  ...            status.append("Control")
  ...         status.append(xp.getVirtualKeyDescription(vKey))
  ...         status = ' '.join(status)
  ...     elif flags & xp.UpFlag:
  ...         status = "Key Up"
  ...     else:
  ...         return 1  # status unchanged
  ...     xp.setWindowRefCon(windowID, status)
  ...     return 1  # Remember to return a value!!
  ...
  >>> windowID = xp.createWindowEx(visible=1, key=MyKey, draw=MyDraw)
  >>> xp.takeKeyboardFocus(windowID)  # (because the debugger has focus right now)
  >>>
  >>> xp.destroyWindow(windowID)
  
  .. table::
     :align: left

     +-----------------------+---------------------------+
     | Key Flags Value       | SDK Value                 |
     +=======================+===========================+
     |:index:`ShiftFlag`     |:index:`xplm_ShiftFlag`    |
     +-----------------------+---------------------------+
     |:index:`OptionAltFlag` |:index:`xplm_OptionAltFlag`|
     +-----------------------+---------------------------+
     |:index:`ControlFlag`   |:index:`xplm_ControlFlag`  |
     +-----------------------+---------------------------+
     |:index:`DownFlag`      |:index:`xplm_DownFlag`     |
     +-----------------------+---------------------------+
     |:index:`UpFlag`        |:index:`xplm_UpFlag`       |
     +-----------------------+---------------------------+

  .. warning:: X-Plane sends the wrong *windowID* when *losingFocus* is set. We're supposed to get
               the windowID of the losing window, instead we get the windowID of the window
               gaining focus. The problem is, we cannot determine which window is actually losing
               focus & therefore cannot forward this "losing" message to the correct window's
               ``key()`` callback function. Bug filed with Laminar 22-October-2021. As this will
               require a re-work of the X-Plane API to actually fix it, there is no time line on this.

               As a result, *you will never receive* a callback with ``losingFocus=1``.
 
.. py:function:: cursor(windowID, x, y, refCon)
 
  Mouse cursor handling callback prototype.
 
  :param windowID: window receiving the notice
  :param x: horizontal position of mouse
  :param y: vertical position of mouse
  :param refCon: refCon you provided on creation
  :return: Cursor status (see below)
 
  The SDK calls your cursor status callback when the mouse is over your
  plugin window.  Return a cursor status code to indicate how you would like
  X-Plane to manage the cursor.  If you return :data:`CursorDefault`, the SDK
  will try lower-Z-order plugin windows, then let the sim manage the cursor.
  
  .. note:: you should never show or hide the cursor yourself using
            non-X-Plane routines as these APIs are
            typically reference-counted and thus cannot safely and predictably be used
            by the SDK.  Instead return one of :data:`CursorHidden` to hide the cursor or
            :data:`CursorArrow`/:data:`CursorCustom` to show the cursor.
  
  >>> def MyCursor(windowID, x, y, refCon):
  ...     (left, top, right, bottom) = xp.getWindowGeometry(windowID)
  ...     # Arrow, if on the left half of window, Hide if on the right
  ...     if x > left and x < (right + left) / 2:
  ...          return xp.CursorArrow
  ...     else:
  ...          return xp.CursorHidden
  ...
  >>> windowID = xp.createWindowEx(visible=1, cursor=MyCursor)
  >>>
  >>> xp.destroyWindow(windowID)

  If you want to implement a custom cursor by drawing a cursor in OpenGL, use
  :data:`CursorHidden` to hide the OS cursor and draw the cursor using a 2-d
  drawing callback (after :data:`Phase_Window` is probably a good choice).

  If you want to use a custom OS-based cursor, return :data:`CursorCustom` to ask
  X-Plane to show the cursor but not affect its image.  You can then use
  :py:func:`xp.setCursor` to display a custom cursor you've loaded. (See
  :py:func:`xp.setCursor` for detailed example.)


  .. _XPLMCursorStatus:

  :index:`XPLMCursorStatus`
  =========================       

  .. table::
     :align: left

     +-------------------------------------+---------------------------------------------------------------------------------+
     | Cursor Status Value                 | SDK Value                                                                       |
     +=====================================+=================================================================================+
     | .. data:: CursorDefault             | X-Plane manages the cursor normally,                                            |
     |  :value: 0                          | plugins does not affect the cursor.                                             |
     |                                     |                                                                                 |
     |                                     | `Official SDK                                                                   |
     |                                     | <https://developer.x-plane.com/sdk/XPLMDisplay/#xplm_CursorDefault>`__          |
     |                                     | :index:`xplm_CursorDefault`                                                     |
     +-------------------------------------+---------------------------------------------------------------------------------+
     | .. data:: CursorHidden              | X-Plane hides the cursor.                                                       |
     |  :value: 1                          |                                                                                 |
     |                                     | `Official SDK                                                                   |
     |                                     | <https://developer.x-plane.com/sdk/XPLMDisplay/#xplm_CursorHidden>`__           |
     |                                     | :index:`xplm_CursorHidden`                                                      |
     +-------------------------------------+---------------------------------------------------------------------------------+
     | .. data:: CursorArrow               | X-Plane shows the cursor as the                                                 |
     |  :value: 2                          | default arrow.                                                                  |
     |                                     |                                                                                 |
     |                                     | `Official SDK                                                                   |
     |                                     | <https://developer.x-plane.com/sdk/XPLMDisplay/#xplm_CursorArrow>`__            |
     |                                     | :index:`xplm_CursorArrow`                                                       |
     +-------------------------------------+---------------------------------------------------------------------------------+
     | .. data:: CursorCustom              | X-Plane shows the cursor but lets                                               |
     |  :value: 3                          | you select an OS cursor. See :doc:`cursor`.                                     |
     |                                     |                                                                                 |
     |                                     | `Official SDK                                                                   |
     |                                     | <https://developer.x-plane.com/sdk/XPLMDisplay/#xplm_CursorCustom>`__           |
     |                                     | :index:`xplm_CursorCustom`                                                      |
     +-------------------------------------+---------------------------------------------------------------------------------+
     | .. data:: CursorRotateSmall         | X-Plane shows the small left-right rotation cursor.                             |
     |  :value: 4                          |                                                                                 |
     |                                     | `Official SDK                                                                   |
     |                                     | <https://developer.x-plane.com/sdk/XPLMDisplay/#xplm_CursorRotateSmall>`__      |
     |                                     | :index:`xplm_CursorRotateSmall`                                                 |
     |                                     |                                                                                 |
     +-------------------------------------+---------------------------------------------------------------------------------+
     | .. data:: CursorRotateSmallLeft     | X-Plane shows the small left rotation cursor.                                   |
     |  :value: 5                          |                                                                                 |
     |                                     | `Official SDK                                                                   |
     |                                     | <https://developer.x-plane.com/sdk/XPLMDisplay/#xplm_CursorRotateSmallLeft>`__  |
     |                                     | :index:`xplm_CursorRotateSmallLeft`                                             |
     |                                     |                                                                                 |
     |                                     |                                                                                 |
     +-------------------------------------+---------------------------------------------------------------------------------+
     | .. data:: CursorRotateSmallRight    | X-Plane shows the small right rotation cursor.                                  |
     |  :value: 6                          |                                                                                 |
     |                                     | `Official SDK                                                                   |
     |                                     | <https://developer.x-plane.com/sdk/XPLMDisplay/#xplm_CursorRotateSmallRight>`__ |
     |                                     | :index:`xplm_CursorRotateSmallRight`                                            |
     |                                     |                                                                                 |
     +-------------------------------------+---------------------------------------------------------------------------------+
     | .. data:: CursorRotateMedium        | X-Plane shows the medium left-right rotation cursor.                            |
     |  :value: 7                          |                                                                                 |
     |                                     | `Official SDK                                                                   |
     |                                     | <https://developer.x-plane.com/sdk/XPLMDisplay/#xplm_CursorRotateMedium>`__     |
     |                                     | :index:`xplm_CursorRotateMedium`                                                |
     |                                     |                                                                                 |
     |                                     |                                                                                 |
     +-------------------------------------+---------------------------------------------------------------------------------+
     | .. data:: CursorRotateMediumLeft    | X-Plane shows the medium left rotation cursor.                                  |
     |  :value: 8                          |                                                                                 |
     |                                     | `Official SDK                                                                   |
     |                                     | <https://developer.x-plane.com/sdk/XPLMDisplay/#xplm_CursorRotateMediumLeft>`__ |
     |                                     | :index:`xplm_CursorRotateMediumLeft`                                            |
     |                                     |                                                                                 |
     |                                     |                                                                                 |
     +-------------------------------------+---------------------------------------------------------------------------------+
     | .. data:: CursorRotateMediumRight   | X-Plane shows the medium right rotation cursor.                                 |
     |  :value: 9                          |                                                                                 |
     |                                     | `Official SDK                                                                   |
     |                                     | <https://developer.x-plane.com/sdk/XPLMDisplay/#xplm_CursorRotateMediumRight>`__|
     |                                     | :index:`xplm_CursorRotateMediumRight`                                           |
     |                                     |                                                                                 |
     |                                     |                                                                                 |
     +-------------------------------------+---------------------------------------------------------------------------------+
     | .. data:: CursorRotateLarge         | X-Plane shows the large left-right rotation cursor.                             |
     |  :value: 10                         |                                                                                 |
     |                                     | `Official SDK                                                                   |
     |                                     | <https://developer.x-plane.com/sdk/XPLMDisplay/#xplm_CursorRotateLarge>`__      |
     |                                     | :index:`xplm_CursorRotateLarge`                                                 |
     |                                     |                                                                                 |
     |                                     |                                                                                 |
     +-------------------------------------+---------------------------------------------------------------------------------+
     | .. data:: CursorRotateLargeLeft     | X-Plane shows the large left rotation cursor.                                   |
     |  :value: 11                         |                                                                                 |
     |                                     | `Official SDK                                                                   |
     |                                     | <https://developer.x-plane.com/sdk/XPLMDisplay/#xplm_CursorRotateLargeLeft>`__  |
     |                                     | :index:`xplm_CursorRotateLargeLeft`                                             |
     |                                     |                                                                                 |
     +-------------------------------------+---------------------------------------------------------------------------------+
     | .. data:: CursorRotateLargeRight    | X-Plane shows the large right rotation cursor.                                  |
     |  :value: 12                         |                                                                                 |
     |                                     | `Official SDK                                                                   |
     |                                     | <https://developer.x-plane.com/sdk/XPLMDisplay/#xplm_CursorRotateLargeRight>`__ |
     |                                     | :index:`xplm_CursorRotateLargeRight`                                            |
     |                                     |                                                                                 |
     +-------------------------------------+---------------------------------------------------------------------------------+
     | .. data:: CursorUpDown              | X-Plane shows the up-down arrow cursor.                                         |
     |  :value: 13                         |                                                                                 |
     |                                     | `Official SDK                                                                   |
     |                                     | <https://developer.x-plane.com/sdk/XPLMDisplay/#xplm_CursorUpDown>`__           |
     |                                     | :index:`xplm_CursorUpDown`                                                      |
     |                                     |                                                                                 |
     +-------------------------------------+---------------------------------------------------------------------------------+
     | .. data:: CursorDown                | X-Plane shows the down arrow cursor.                                            |
     |  :value: 14                         |                                                                                 |
     |                                     | `Official SDK                                                                   |
     |                                     | <https://developer.x-plane.com/sdk/XPLMDisplay/#xplm_CursorDown>`__             |
     |                                     | :index:`xplm_CursorDown`                                                        |
     |                                     |                                                                                 |
     +-------------------------------------+---------------------------------------------------------------------------------+
     | .. data:: CursorUp                  | X-Plane shows the up arrow cursor.                                              |
     |  :value: 15                         |                                                                                 |
     |                                     | `Official SDK                                                                   |
     |                                     | <https://developer.x-plane.com/sdk/XPLMDisplay/#xplm_CursorUp>`__               |
     |                                     | :index:`xplm_CursorUp`                                                          |
     |                                     |                                                                                 |
     +-------------------------------------+---------------------------------------------------------------------------------+
     | .. data:: CursorLeftRight           | X-Plane shows the left-right arrow cursor.                                      |
     |  :value: 16                         |                                                                                 |
     |                                     | `Official SDK                                                                   |
     |                                     | <https://developer.x-plane.com/sdk/XPLMDisplay/#xplm_CursorLeftRight>`__        |
     |                                     | :index:`xplm_CursorLeftRight`                                                   |
     |                                     |                                                                                 |
     +-------------------------------------+---------------------------------------------------------------------------------+
     | .. data:: CursorLeft                | X-Plane shows the left arrow cursor.                                            |
     |  :value: 17                         |                                                                                 |
     |                                     | `Official SDK                                                                   |
     |                                     | <https://developer.x-plane.com/sdk/XPLMDisplay/#xplm_CursorLeft>`__             |
     |                                     | :index:`xplm_CursorLeft`                                                        |
     |                                     |                                                                                 |
     +-------------------------------------+---------------------------------------------------------------------------------+
     | .. data:: CursorRight               | X-Plane shows the right arrow cursor.                                           |
     |  :value: 18                         |                                                                                 |
     |                                     | `Official SDK                                                                   |
     |                                     | <https://developer.x-plane.com/sdk/XPLMDisplay/#xplm_CursorRight>`__            |
     |                                     | :index:`xplm_CursorRight`                                                       |
     |                                     |                                                                                 |
     +-------------------------------------+---------------------------------------------------------------------------------+
     | .. data:: CursorButton              | X-Plane shows the button-pushing cursor.                                        |
     |  :value: 19                         |                                                                                 |
     |                                     | `Official SDK                                                                   |
     |                                     | <https://developer.x-plane.com/sdk/XPLMDisplay/#xplm_CursorButton>`__           |
     |                                     | :index:`xplm_CursorButton`                                                      |
     |                                     |                                                                                 |
     +-------------------------------------+---------------------------------------------------------------------------------+
     | .. data:: CursorHandle              | X-Plane shows the handle-grabbing cursor.                                       |
     |  :value: 20                         |                                                                                 |
     |                                     | `Official SDK                                                                   |
     |                                     | <https://developer.x-plane.com/sdk/XPLMDisplay/#xplm_CursorHandle>`__           |
     |                                     | :index:`xplm_CursorHandle`                                                      |
     |                                     |                                                                                 |
     +-------------------------------------+---------------------------------------------------------------------------------+
     | .. data:: CursorFourArrows          | X-Plane shows the four-arrows cursor.                                           |
     |  :value: 21                         |                                                                                 |
     |                                     | `Official SDK                                                                   |
     |                                     | <https://developer.x-plane.com/sdk/XPLMDisplay/#xplm_CursorFourArrows>`__       |
     |                                     | :index:`xplm_CursorFourArrows`                                                  |
     |                                     |                                                                                 |
     +-------------------------------------+---------------------------------------------------------------------------------+
     | .. data:: CursorSplitterH           | X-Plane shows the cursor to drag a horizontal splitter bar.                     |
     |  :value: 22                         |                                                                                 |
     |                                     | `Official SDK                                                                   |
     |                                     | <https://developer.x-plane.com/sdk/XPLMDisplay/#xplm_CursorSplitterH>`__        |
     |                                     | :index:`xplm_CursorSplitterH`                                                   |
     |                                     |                                                                                 |
     +-------------------------------------+---------------------------------------------------------------------------------+
     | .. data:: CursorSplitterV           | X-Plane shows the cursor to drag a vertical splitter bar.                       |
     |  :value: 23                         |                                                                                 |
     |                                     | `Official SDK                                                                   |
     |                                     | <https://developer.x-plane.com/sdk/XPLMDisplay/#xplm_CursorSplitterV>`__        |
     |                                     | :index:`xplm_CursorSplitterV`                                                   |
     |                                     |                                                                                 |
     +-------------------------------------+---------------------------------------------------------------------------------+
     | .. data:: CursorText                | X-Plane shows the I-Beam cursor for text editing.                               |
     |  :value: 24                         |                                                                                 |
     |                                     | `Official SDK                                                                   |
     |                                     | <https://developer.x-plane.com/sdk/XPLMDisplay/#xplm_CursorText>`__             |
     |                                     | :index:`xplm_CursorText`                                                        |
     |                                     |                                                                                 |
     +-------------------------------------+---------------------------------------------------------------------------------+

.. py:function:: wheel(windowID, x, y, wheel, clicks, refCon)
 
  Mouse wheel handling callback prototype.
 
  :param windowID: window receiving the mouse event
  :param x: horizontal position of mouse
  :param y: vertical position of mouse
  :param wheel: 0= vertical axis, 1= horizontal axis
  :param clicks: number of "clicks" indicating how far the wheel has turned since previous callback
  :param refCon: refCon you provided on creation
  :return: 1= consume the mouse wheel click, 0= pass to lower window                
 
  The SDK calls your mouse wheel callback when one of the mouse wheels is
  turned within your window.  Return 1 to consume the  mouse wheel clicks or
  0 to pass them on to a lower window.  (You should consume mouse wheel
  clicks even if they do nothing, if your window appears opaque to the user.)
  The number of clicks indicates how far the wheel was turned since the last
  callback. The wheel is 0 for the vertical axis or 1 for the horizontal axis
  (for OS/mouse combinations that support this).
 
  The units for x and y values matches the units used in your window (i.e., boxels),
  with origin in lower left of global desktop space.


Screen and Monitor Functions
****************************
See detailed explanation of screens vs. monitors and positioning windows
within bounds in :doc:`/development/window_position`.


.. py:function:: getScreenSize()

 Query X-Plane screen size.

 :return: Tuple of (width, height) in pixels
 :rtype: tuple[int, int]

 This routine returns the size of the size of the X-Plane OpenGL window in
 pixels.  Please note that this is not the size of the screen when doing
 2-d drawing (the 2-d screen is currently always 1024x768, and  graphics are
 scaled up by OpenGL when doing 2-d drawing for higher-res monitors).  This
 number can be used to get a rough idea of the amount of detail the user
 will be able to see when drawing in 3-d.

 >>> xp.getScreenSize()
 (1280, 1024)

 `Official SDK <https://developer.x-plane.com/sdk/XPLMDisplay/#XPLMGetScreenSize>`__ :index:`XPLMGetScreenSize`

.. py:function::  getScreenBoundsGlobal()

 This routine returns the bounds of the "global" X-Plane desktop, in boxels.

 :return: Tuple of (left, top, right, bottom) in boxels
 :rtype: tuple[int, int, int, int]

 Unlike the non-global version :func:`getScreenSize`, this is multi-monitor
 aware. There are three primary consequences of multimonitor awareness:

 * First, if the user is running X-Plane in full-screen on two or more monitors
   (typically configured using one full-screen window per monitor), the global
   desktop will be sized to include all X-Plane windows.

 * Second, the origin of the screen coordinates is not guaranteed to be (0, 0).
   Suppose the user has two displays side- by-side, both running at 1080p.
   Suppose further that they’ve configured their OS to make the left display
   their “primary” monitor, and that X-Plane is running in full-screen on their
   right monitor only. In this case, the global desktop bounds would be the
   rectangle from (1920, 0) to (3840, 1080). If the user later asked X-Plane to
   draw on their primary monitor as well, the bounds would change to (0, 0) to
   (3840, 1080).

 * Finally, if the usable area of the virtual desktop is not a perfect rectangle
   (for instance, because the monitors have different resolutions or because one
   monitor is configured in the operating system to be above and to the right of
   the other), the global desktop will include any wasted space. Thus, if you have
   two 1080p monitors, and monitor 2 is configured to have its bottom left touch
   monitor 1’s upper right, your global desktop area would be the rectangle from
   (0, 0) to (3840, 2160).

 If the X-Plane is running in a window (i.e., not full screen), the values
 returned are the size of the within in boxels (:func:`getScreenSize` returns pixels)
 with the (0, 0) origin in the bottom left corner.
 
 Note that popped-out windows (windows drawn in their own operating system
 windows, rather than “floating” within X-Plane) are not included in these bounds.

 Return (left, top, right, bottom)

 >>> xp.getScreenBoundsGlobal()
 (-1280, 1440, 2560, 0)

 `Official SDK <https://developer.x-plane.com/sdk/XPLMDisplay/#XPLMGetScreenBoundsGlobal>`__ :index:`XPLMGetScreenBoundsGlobal`

.. py:function:: getAllMonitorBoundsGlobal(bounds, refCon)

 This routine immediately calls your *bounds()* function the bounds (in boxels) of each
 full-screen X-Plane window within the X- Plane global desktop space.

 :param bounds: Callback function to receive monitor bounds
 :type bounds: Callable[[int, int, int, int, int, Any], None]
 :param refCon: Reference constant passed to callback
 :type refCon: Any
 :return: None

 Note that if a monitor is not covered by an X-Plane window, you cannot get its bounds this
 way. Likewise, monitors with only an X-Plane window (not in full-screen mode)
 will not be included.

 If X-Plane is running in full-screen and your monitors are of the same size and
 configured contiguously in the OS, then the combined global bounds of all full-screen
 monitors will match the total global desktop bounds, as returned by
 :func:`getScreenBoundsGlobal`. (Of course, if X-Plane is running in windowed mode,
 this will not be the case. Likewise, if you have differently sized monitors, the
 global desktop space will include wasted space.)
 
 Note that this function’s monitor indices match those provided by
 :func:`getAllMonitorBoundsOS`, but the coordinates are different (since the X-Plane
 global desktop may not match the operating system’s global desktop, and one X-Plane
 boxel may be larger than one pixel due to 150% or 200% scaling).
 
 >>> def MyBounds(index, left, top, right, bottom, refCon):
 ...     refCon[index] = (left, top, right, bottom)
 ...
 >>> data = {}
 >>> xp.getAllMonitorBoundsGlobal(MyBounds, data)
 >>> data
 {0: (0, 1440, 2560, 0), 2: (-1280, 1368, 0 344)}

 (In the above example, I have two of my three monitors running fullscreen.)

 This function is informed of the global bounds (in boxels) of a particular monitor
 within the X-Plane global desktop space. **Note** that X-Plane must be running in full
 screen on a monitor in order for that monitor to be passed to you in this callback. If
 it is not running full screen, the function will set data to ``{}``.

.. py:function:: getAllMonitorBoundsOS(bounds, refCon)

 This routine immediately calls your *bounds()* function with the bounds (in pixels) of each monitor
 within the operating system's global desktop space.

 :param bounds: Callback function to receive monitor bounds
 :type bounds: Callable[[int, int, int, int, int, Any], None]
 :param refCon: Reference constant passed to callback
 :type refCon: Any
 :return: None

 Note that unlike :func:`getAllMonitorBoundsGlobal`, this may include monitors that have no X-Plane window
 on them.

 Note that this function’s monitor indices match those provided by
 :func:`getAllMonitorBoundsGlobal`, but the coordinates are different (since the X-Plane
 global desktop may not match the operating system’s global desktop, and one X-Plane
 boxel may be larger than one pixel).

 >>> def MyBoundsOS(index, left, top, right, bottom, refCon):
 ...     refCon[index] = (left, top, right, bottom)
 ...
 >>> data = {}
 >>> xp.getAllMonitorBoundsOS(MyBoundsOS, data)
 >>> data
 {0: (0, 1440, 2560, 0), 1: (2560, 1840, 3760, -80), 2: (-1280, 1368, 0, 344)}

 This function is informed of the global bounds (in pixels) of a particular monitor
 within the operating system’s global desktop space. Note that a monitor index being
 passed to you here does not indicate that X-Plane is running in full screen on this
 monitor, or even that any X-Plane windows exist on this monitor. (So this will work
 regardless of whether X-Plane is running full-screen or windowed.

 `Official SDK <https://developer.x-plane.com/sdk/XPLMDisplay/#XPLMGetAllMonitorBoundsOS>`__ :index:`XPLMGetAllMonitorBoundsOS`

.. py:function:: getMouseLocationGlobal()

 Returns the current mouse location in global desktop boxels.

 :return: Tuple of (x, y) in boxels
 :rtype: tuple[int, int]

 Unlike older :func:`getMouseLocation`, the bottom left of the main X-Plane window is not guaranteed
 to be (0, 0). Instead, the origin is the lower left of the entire global desktop space.
 In addition, this routine gives the real mouse location when the mouse goes to X-Plane
 windows other than the primary display. Thus, it can be used with both pop-out windows
 and secondary monitors.

 For windowed (not full-screen) simulator, (0,0) is lower left corner of the X-Plane window,
 with the mouse position going negative when moved left or below the window.

 This is the mouse location function to use with modern windows (i.e., those created by
 :func:`createWindowEx`).

 Returns (x, y)

 >>> xp.getMouseLocationGlobal()
 (3025, 204)
 
 `Official SDK <https://developer.x-plane.com/sdk/XPLMDisplay/#XPLMGetMouseLocationGlobal>`__ :index:`XPLMGetMouseLocationGlobal`


Window Functions
****************

.. py:function:: getWindowGeometry(windowID)

 This routine returns the position and size of a window.

 :param windowID: Window to query
 :type windowID: :data:`XPLMWindowID`
 :return: Tuple of (left, top, right, bottom)
 :rtype: tuple[int, int, int, int]

 The units and coordinate system vary depending on the type of window you have.

 If this is a legacy window (one compiled against a pre-XPLM300 version of the SDK,
 or an XPLM300 window that was not created using :func:`createWindowEx`), the units
 are pixels relative to the main X-Plane display.

 For X-Plane 11 and 12 windows (compiled against the
 XPLM300 SDK and created using :func:`createWindowEx`), the units are global desktop boxels.

 >>> windowID = xp.createWindowEx()
 >>> xp.getWindowGeometry(windowID)
 (100, 200, 200, 100)

 Note that a window has geometry *even when not visible* & :func:`createWindowEx` creates hidden
 windows by default. Make it visible using ``xp.setWindowIsVisible(windowID)``.
 
 Also supports older calling style where you pass in lists as parameters, the results
 are copied rather than returned. (Don't use this -- it's here really just
 to help those used to the way C-language SDK worked.)

 >>> left = []; right = []; top = []; bottom = [];
 >>> xp.getWindowGeometry(windowID, left, top, right, bottom)
 >>> left[0]
 100
 >>> top[0]
 200
 
 `Official SDK <https://developer.x-plane.com/sdk/XPLMDisplay/#XPLMGetWindowGeometry>`__ :index:`XPLMGetWindowGeometry`


.. py:function:: setWindowGeometry(windowID, left, top, right, bottom)

 Set window position and size.

 :param windowID: Window to modify
 :type windowID: :data:`XPLMWindowID`
 :param left: Left edge coordinate
 :type left: int
 :param top: Top edge coordinate
 :type top: int
 :param right: Right edge coordinate
 :type right: int
 :param bottom: Bottom edge coordinate
 :type bottom: int
 :return: None

 This routine allows you to set the position and size of a window.

 The units and coordinate system match those of :func:`getWindowGeometry`. That is,
 modern windows use global desktop boxel coordinates, while legacy windows use
 pixels relative to the main X-Plane display.

 Note that this only applies to “floating” windows (that is, windows that are drawn
 within the X-Plane simulation windows, rather than being “popped out” into their
 own first-class operating system windows). To set the position of windows whose
 positioning mode is :data:`WindowPopOut`, you’ll need to instead use :func:`setWindowGeometryOS`.

 >>> windowID = xp.createWindowEx()
 >>> xp.getWindowGeometry(windowID)
 (100, 200, 200, 100)
 >>> xp.setWindowGeometry(windowID, 200, 300, 400, 250)
 >>> xp.getWindowGeometry(windowID)
 (200, 300, 400, 250)

 `Official SDK <https://developer.x-plane.com/sdk/XPLMDisplay/#XPLMSetWindowGeometry>`__ :index:`XPLMSetWindowGeometry`

.. py:function:: getWindowGeometryOS(windowID)

 This routine returns the position and size of a "popped out" window (i.e., a window
 whose positioning mode is WindowPopOut), in operating system pixels.

 :param windowID: Window to query (must be popped out)
 :type windowID: :data:`XPLMWindowID`
 :return: Tuple of (left, top, right, bottom) in OS pixels
 :rtype: tuple[int, int, int, int]

 If the window is not popped out, do not use.

 >>> windowID = xp.createWindowEx()
 >>> xp.setWindowPositioningMode(windowID, xp.WindowPopOut, -1)
 >>> xp.getWindowGeometryOS(windowID)
 (90 610, 210, 490)
          
 Also supports alternate calling style where you can pass lists as parameters (see example
 with :py:func:`getWindowGeometry`.
 
 `Official SDK <https://developer.x-plane.com/sdk/XPLMDisplay/#XPLMGetWindowGeometryOS>`__ :index:`XPLMGetWindowGeometryOS`

.. py:function:: setWindowGeometryOS(windowID, left, top, right, bottom)

 This routine allows you to set the position and size, in operating system pixel
 coordinates, of a popped out window.

 :param windowID: Window to modify (must be popped out)
 :type windowID: :data:`XPLMWindowID`
 :param left: Left edge in OS pixels
 :type left: int
 :param top: Top edge in OS pixels
 :type top: int
 :param right: Right edge in OS pixels
 :type right: int
 :param bottom: Bottom edge in OS pixels
 :type bottom: int
 :return: None

 The window must have positioning mode :data:`WindowPopOut`, which exists outside the X-Plane simulation window, in its
 own first-class operating system window.

 Note that you are responsible for ensuring both that your window is popped out
 (using :func:`windowIsPoppedOut`) and that a monitor really exists at the OS coordinates
 you provide (using :func:`getAllMonitorBoundsOS`).

 `Official SDK <https://developer.x-plane.com/sdk/XPLMDisplay/#XPLMSetWindowGeometryOS>`__ :index:`XPLMSetWindowGeometryOS`

.. py:function:: getWindowGeometryVR(windowID)

 Returns the width and height, in boxels, of a window in VR.

 :param windowID: Window to query (must be in VR)
 :type windowID: :data:`XPLMWindowID`
 :return: Tuple of (widthBoxels, heightBoxels)
 :rtype: tuple[int, int]

 Note that you are responsible for ensuring your window is in VR (using :func:`windowIsInVR`).

 >>> windowID = xp.createWindowEx()
 >>> if xp.windowIsInVR(windowID):
 ...     xp.getWindowGeometryVR(windowID)
 ...
 (200, 100)

 `Official SDK <https://developer.x-plane.com/sdk/XPLMDisplay/#XPLMGetWindowGeometryVR>`__ :index:`XPLMGetWindowGeometryVR`

.. py:function:: setWindowGeometryVR(windowID, width, height)

 This routine allows you to set the size, in boxels, of a window in VR.

 :param windowID: Window to modify (must be in VR)
 :type windowID: :data:`XPLMWindowID`
 :param width: Width in boxels
 :type width: int
 :param height: Height in boxels
 :type height: int
 :return: None

 The window must have positioning mode :data:`WindowVR`.

 Note that you are responsible for ensuring your window is in VR (using :func:`windowIsInVR`).

 `Official SDK <https://developer.x-plane.com/sdk/XPLMDisplay/#XPLMSetWindowGeometryVR>`__ :index:`XPLMSetWindowGeometryVR`

.. py:function:: getWindowIsVisible(windowID)

 Get window's isVisible attribute value.

 :param windowID: Window to query
 :type windowID: :data:`XPLMWindowID`
 :return: 1 if visible, 0 otherwise
 :rtype: int

 >>> windowID = xp.createWindowEx()
 >>> xp.getWindowIsVisible(windowID)
 0
 
 `Official SDK <https://developer.x-plane.com/sdk/XPLMDisplay/#XPLMGetWindowIsVisible>`__ :index:`XPLMGetWindowIsVisible`


.. py:function::  setWindowIsVisible(windowID, visible=1)

 Set window's visible attribute value.

 :param windowID: Window to modify
 :type windowID: :data:`XPLMWindowID`
 :param visible: 1 for visible, 0 for hidden (default: 1)
 :type visible: int
 :return: None

 `Official SDK <https://developer.x-plane.com/sdk/XPLMDisplay/#XPLMSetWindowIsVisible>`__ :index:`XPLMSetWindowIsVisible`

.. py:function:: windowIsPoppedOut(windowID)

 True if this window has been popped out (making it a first-class window in the
 operating system).

 :param windowID: Window to query
 :type windowID: :data:`XPLMWindowID`
 :return: 1 if window is popped out, 0 otherwise
 :rtype: int

 This is true if and only if you have set the window's positioning mode to :data:`WindowPopOut`.

 Only applies to modern windows. (Windows created using the deprecated
 XPLMCreateWindow(), or windows compiled against a pre-XPLM300 version of the
 SDK cannot be popped out.)

 `Official SDK <https://developer.x-plane.com/sdk/XPLMDisplay/#XPLMWindowIsPoppedOut>`__ :index:`XPLMWindowIsPoppedOut`
 
.. py:function:: windowIsInVR(windowID)

 True if this window has been moved to the virtual reality (VR) headset.

 :param windowID: Window to query
 :type windowID: :data:`XPLMWindowID`
 :return: 1 if window is in VR, 0 otherwise
 :rtype: int

 This is true if and only if you have set the window's positioning mode to :data:`WindowVR`.

 Only applies to modern windows. (Windows created using the deprecated XPLMCreateWindow(), or windows
 compiled against a pre-XPLM301 version of the SDK cannot be moved to VR.)

 `Official SDK <https://developer.x-plane.com/sdk/XPLMDisplay/#XPLMWindowIsInVR>`__ :index:`XPLMWindowIsInVR`

.. py:function::  setWindowGravity(windowID, left, top, right, bottom)

 A window's "gravity" controls how the window shifts as the whole X-Plane window resizes.

 :param windowID: Window to modify
 :type windowID: :data:`XPLMWindowID`
 :param left: Left edge gravity (0.0 to 1.0)
 :type left: float
 :param top: Top edge gravity (0.0 to 1.0)
 :type top: float
 :param right: Right edge gravity (0.0 to 1.0)
 :type right: float
 :param bottom: Bottom edge gravity (0.0 to 1.0)
 :type bottom: float
 :return: None

 A gravity of 1 means the window maintains its positioning relative to the right or top
 edges, 0 the left/bottom, and 0.5 keeps it centered.
 
 Default gravity is (0.0, 1.0, 0.0, 1.0), meaning your window will maintain its position relative
 to the top left and will not change size as its containing window grows. (That is, the right and
 bottom edges of your window will *also* maintain relative positions to top left.)

 ``(0.0, 1.0, 0.0, 1.0)`` can be interpreted as:

 +-------+-----------------------------+---------------------------------------+
 | value | Refers to your...           | Resulting in...                       |
 +=======+=============================+=======================================+
 | 0.0   | left edge of your window    | "0.0" means it will maintain position |
 |       |                             | relative to left of screen            |
 +-------+-----------------------------+---------------------------------------+
 | 1.0   | top edge of your window     | "1.0" means it will maintain position |
 |       |                             | relative to top of screen             |
 +-------+-----------------------------+---------------------------------------+
 | 0.0   | right edge of your window,  | "0.0" means it will maintain position |
 |       |                             | relative to left of screen            |
 +-------+-----------------------------+---------------------------------------+
 | 1.0   | bottom edge of your window  | "1.0" means it will maintain position |
 |       |                             | relative to top of screen             |
 +-------+-----------------------------+---------------------------------------+

 Therefore your window will keep its size, and the upper left of your window will stay in the same location (i.e., same
 number of pixels from down from the top and over from the left of the screen.

 ``(0.0, 1.0, 1.0, 1.0)`` can be interpreted as:

 +-------+-----------------------------+---------------------------------------+
 | value | Refers to your...           | Resulting in...                       |
 +=======+=============================+=======================================+
 | 0.0   | left edge of your window    | "0.0" means it will maintain position |
 |       |                             | relative to left of screen            |
 +-------+-----------------------------+---------------------------------------+
 | 1.0   | top edge of your window     | "1.0" means it will maintain position |
 |       |                             | relative to top of screen             |
 +-------+-----------------------------+---------------------------------------+
 | *1.0* | right edge of your window,  | **"1.0"** means it will maintain      |
 |       |                             | position 100% relative to **right**   |
 +-------+-----------------------------+---------------------------------------+
 | 1.0   | bottom edge of your window  | "1.0" means it will maintain position |
 |       |                             | relative to top of screen             |
 +-------+-----------------------------+---------------------------------------+

 As before, this will keep upper left edges where they are, bottom remains same distance from top (so the window
 remains same height). However, right edge of your window will track the right side of your screen:
 if your screen increases 100, your window's right edge expands 100%, or 100 pixels.

 ``(0.0, 1.0, 0.5, 1.0)`` can be interpreted as:

 +-------+-----------------------------+---------------------------------------+
 | value | Refers to your...           | Resulting in...                       |
 +=======+=============================+=======================================+
 | 0.0   | left edge of your window    | "0.0" means it will maintain position |
 |       |                             | relative to left of screen            |
 +-------+-----------------------------+---------------------------------------+
 | 1.0   | top edge of your window     | "1.0" means it will maintain position |
 |       |                             | relative to top of screen             |
 +-------+-----------------------------+---------------------------------------+
 | *0.5* | right edge of your window,  | **"0.5"** means it will maintain      |
 |       |                             | position 50% relative to **right**    |
 +-------+-----------------------------+---------------------------------------+
 | 1.0   | bottom edge of your window  | "1.0" means it will maintain position |
 |       |                             | relative to top of screen             |
 +-------+-----------------------------+---------------------------------------+

 Same as previous but right edge expands 50% of the change in screen width: If your screen
 increases 100, your window's right edge expands 50)
 
 Only applies to modern windows. (Windows created using the deprecated older XPLMCreateWindow(), or
 windows compiled against a pre-XPLM300 version of the SDK will simply get the default gravity.)

 `Official SDK <https://developer.x-plane.com/sdk/XPLMDisplay/#XPLMSetWindowGravity>`__ :index:`XPLMSetWindowGravity`
 
.. py:function:: setWindowResizingLimits(windowID, minWidth=0, minHeight=0, maxWidth=10000, maxHeight=10000)

 Sets the minimum and maximum size of the client rectangle of the given window.

 :param windowID: Window to modify
 :type windowID: :data:`XPLMWindowID`
 :param minWidth: Minimum width in boxels (default: 0)
 :type minWidth: int
 :param minHeight: Minimum height in boxels (default: 0)
 :type minHeight: int
 :param maxWidth: Maximum width in boxels (default: 10000)
 :type maxWidth: int
 :param maxHeight: Maximum height in boxels (default: 10000)
 :type maxHeight: int
 :return: None

 The client rectangle does not include any window styling that you might have asked X-Plane to apply on your
 behalf. All resizing operations are constrained to these sizes. (Except see Note below.)
 
 Only applies to modern windows. (Windows created using the deprecated XPLMCreateWindow(),
 or windows compiled against a pre-XPLM300 version of the SDK will have no minimum or maximum size.)

 This is especially useful if you've set window gravity (:py:func:`setWindowGravity` such that the
 height or width of the window changes as the screen changes.

 .. note:: :py:func:`setWindowGravity` *may* cause the window to expand exceeding *maxWidth*, *maxHeight* values for
           :py:func:`setWindowResizingLimits`, any future changes to that window's geometry (either
           via manual dragging or :py:func:`setWindowGeometry`) will cause the window to snap
           to its declared sizing limit, but until the window is resized, it will remain too large.
           Bug filed with Laminar 23-October-2021 as `XPD-11455 <https://developer.x-plane.com/x-plane-bug-database/?issue=XPD-11455>`_.
           Minimum sizes are correctly constrained.

 `Official SDK <https://developer.x-plane.com/sdk/XPLMDisplay/#XPLMSetWindowResizingLimits>`__ :index:`XPLMSetWindowResizingLimits`

 >>> windowID = xp.createWindowEx(visible=1, left=100, right=200)
 >>> xp.setWindowResizingLimits(windowID, minWidth=100)

.. py:function:: setWindowPositioningMode(windowID, mode, index=-1)

 Sets the policy for how X-Plane will position your window.

 :param windowID: Window to modify
 :type windowID: :data:`XPLMWindowID`
 :param mode: Positioning mode constant
 :type mode: int
 :param index: Monitor index, or -1 for main monitor (default: -1)
 :type index: int
 :return: None

 Some positioning modes apply to a particular monitor. For those modes, you can pass a negative
 monitor index to position the window on the main X-Plane monitor (the screen with the X-Plane
 menu bar at the top). Or, if you have a specific monitor you want to position your window on,
 you can pass a real monitor index as received from, e.g., :func:`getAllMonitorBoundsOS`.

 Only applies to modern windows. (Windows created using the deprecated XPLMCreateWindow(),
 or windows compiled against a pre-XPLM300 version of the SDK will always use xplm_WindowPositionFree.)

 The mode describes how X-Plane will position your window on the user’s screen. X-Plane will
 maintain this positioning mode even as the user resizes their window or adds/removes full-screen monitors.

 `Official SDK <https://developer.x-plane.com/sdk/XPLMDisplay/#XPLMSetWindowPositioningMode>`__ :index:`XPLMSetWindowPositioningMode`

 >>> windowID = xp.createWindowEx(visible=1)
 >>> xp.setWindowPositioningMode(windowID, xp.WindowPopOut)
 
 
 :index:`XPLMWindowPositioningMode`

 .. table::
    :align: left
 
    +---------------------------------------------+---------------------------------------------------------------------------------------------+
    | Positioning Mode Value                      | Meaning                                                                                     |
    +=============================================+=============================================================================================+
    | .. data:: WindowPositionFree                | The default positioning mode. Set the window geometry and its                               |
    |    :value: 0                                | future position will be determined by its window gravity, resizing                          |
    |                                             | limits, and user interactions.                                                              |
    |                                             |                                                                                             |
    |                                             | `Official SDK                                                                               |
    |                                             | <https://developer.x-plane.com/sdk/XPLMDisplay/#xplm_WindowPositionFree>`__                 |
    |                                             | :index:`xplm_WindowPositionFree`                                                            |
    +---------------------------------------------+---------------------------------------------------------------------------------------------+
    | .. data:: WindowCenterOnMonior              | Keep the window centered on the monitor you specify                                         |
    |    :value: 1                                |                                                                                             |
    |                                             | `Official SDK                                                                               |
    |                                             | <https://developer.x-plane.com/sdk/XPLMDisplay/#xplm_WindowCenterOnMonitor>`__              |
    |                                             | :index:`xplm_WindowCenterOnMonitor`                                                         |
    +---------------------------------------------+---------------------------------------------------------------------------------------------+
    | .. data:: WindowFullScreenOnMonitor         | Keep the window full screen on the monitor you specify                                      |
    |    :value: 2                                |                                                                                             |
    |                                             | `Official SDK                                                                               |
    |                                             | <https://developer.x-plane.com/sdk/XPLMDisplay/#xplm_WindowFullScreenOnMonitor>`__          |
    |                                             | :index:`xplm_WindowFullScreenOnMonitor`                                                     |
    +---------------------------------------------+---------------------------------------------------------------------------------------------+
    | .. data:: WindowFullScreenOnAllMonitors     | Like gui_window_full_screen_on_monitor, but stretches                                       |
    |    :value: 3                                | over *all* monitors and popout windows.                                                     |
    |                                             | This is an obscure one... unless you have a very good                                       |
    |                                             | reason to need it, you probably don't!                                                      |
    |                                             |                                                                                             |
    |                                             | `Official SDK                                                                               |
    |                                             | <https://developer.x-plane.com/sdk/XPLMDisplay/#xplm_WindowFullScreenOnAllMonitors>`__      |
    |                                             | :index:`xplm_WindowFullScreenOnAllMonitors`                                                 |
    |                                             |                                                                                             |
    +---------------------------------------------+---------------------------------------------------------------------------------------------+
    | .. data:: WindowPopOut                      | A first-class window in the operating system, completely                                    |
    |    :value: 4                                | separate from the X-Plane window(s)                                                         |
    |                                             |                                                                                             |
    |                                             | `Official SDK                                                                               |
    |                                             | <https://developer.x-plane.com/sdk/XPLMDisplay/#xplm_WindowPopOut>`__                       |
    |                                             | :index:`xplm_WindowPopOut`                                                                  |
    +---------------------------------------------+---------------------------------------------------------------------------------------------+
    | .. data:: WindowVR                          | A floating window visible on the VR headset                                                 |
    |    :value: 5                                |                                                                                             |
    |                                             | `Official SDK <https://developer.x-plane.com/sdk/XPLMDisplay/#xplm_WindowVR>`__             |
    |                                             | :index:`xplm_WindowVR`                                                                      |
    +---------------------------------------------+---------------------------------------------------------------------------------------------+

.. py:function:: setWindowTitle(windowID, title)

 Sets the name for a window.

 :param windowID: Window to modify
 :type windowID: :data:`XPLMWindowID`
 :param title: New window title
 :type title: str
 :return: None

 This only applies to windows that opted-in to styling as an X-Plane
 11 floating window (i.e., with styling mode :data:`xplm_WindowDecorationRoundRectangle`) when they
 were created using :func:`XPLMCreateWindowEx`.

 >>> windowID = xp.createWindowEx(visible=1)
 >>> xp.setWindowTitle(windowID, "New Title")
 
 `Official SDK <https://developer.x-plane.com/sdk/XPLMDisplay/#XPLMSetWindowTitle>`__: :index:`XPLMSetWindowTitle`

.. py:function:: getWindowRefCon(windowID)

 Return window's refCon attribute value (which you provided on window creation.)

 :param windowID: Window to query
 :type windowID: :data:`XPLMWindowID`
 :return: Reference constant value
 :rtype: Any

 >>> windowID = xp.createWindowEx(visible=1)
 >>> xp.getWindowRefCon(windowID)
 None

 `Official SDK <https://developer.x-plane.com/sdk/XPLMDisplay/#XPLMGetWindowRefCon>`__: :index:`XPLMGetWindowRefCon`

.. py:function:: setWindowRefCon(windowID, refCon)

 Set window's refcon attribute value.

 :param windowID: Window to modify
 :type windowID: :data:`XPLMWindowID`
 :param refCon: Reference constant value to store
 :type refCon: Any
 :return: None

 Use this to pass data to yourself in the callbacks.

 >>> windowID = xp.createWindowEx(visible=1)
 >>> xp.getWindowRefCon(windowID)
 None
 >>> xp.setWindowRefCon(windowID, {"data": "value"})
 >>> xp.getWindowRefCon(windowID, {"data": "value"})
 {"data": "value"}

 `Official SDK <https://developer.x-plane.com/sdk/XPLMDisplay/#XPLMSetWindowRefCon>`__: :index:`XPLMSetWindowRefCon`


.. py:function:: takeKeyboardFocus(windowID)

 Give a specific window keyboard focus.

 :param windowID: Window to receive focus, or 0 to give focus to X-Plane
 :type windowID: :data:`XPLMWindowID`
 :return: None

 This routine gives a specific window keyboard focus. Keystrokes will be sent to that window.
 Pass a window ID of 0 to remove keyboard focus from any plugin-created windows and instead
 pass keyboard strokes directly to X-Plane.

 >>> windowID = xp.createWindowEx(visible=1)
 >>> xp.hasKeyboardFocus(windowID)
 0
 >>> xp.takeKeyboardFocus(windowID) ; xp.hasKeyboardFocus(windowID)
 1

 (Because the debugger will have keyboard focus, for obvious reasons, execute
 *take* and *has* on the same line to see the above results.)

 `Official SDK <https://developer.x-plane.com/sdk/XPLMDisplay/#XPLMTakeKeyboardFocus>`__: :index:`XPLMTakeKeyboardFocus`

.. py:function:: hasKeyboardFocus(windowID)

 Returns 1 if the indicated window has keyboard focus.

 :param windowID: Window to query, or 0 for X-Plane focus
 :type windowID: :data:`XPLMWindowID`
 :return: 1 if window has focus, 0 otherwise
 :rtype: int

 Pass a window ID of 0 to see if no plugin window has focus, and all keystrokes will go directly to X-Plane.

 `Official SDK <https://developer.x-plane.com/sdk/XPLMDisplay/#XPLMHasKeyboardFocus>`__: :index:`XPLMHasKeyboardFocus`

.. py:function:: bringWindowToFront(windowID)

 Bring window to the front of the Z-order.

 :param windowID: Window to bring to front
 :type windowID: :data:`XPLMWindowID`
 :return: None

 This routine brings the window to the front of the Z-order for its layer. Windows are brought
 to the front automatically when they are created. Beyond that, you should make sure you are
 front before handling mouse clicks.

 Note that this only brings your window to the front of its layer XPLMWindowLayer. Thus, if
 you have a window in the floating window layer (:data:`WindowLayerFloatingWindows`), but there
 is a modal window (in layer :data:`WindowLayerModal`) above you, you would still not be the true
 frontmost window after calling this. (After all, the window layers are strictly ordered, and
 no window in a lower layer can ever be above any window in a higher one.)   Windows are brought
 to the front when they are created. Beyond that you should make sure you are front before handling
 mouse clicks.

 `Official SDK <https://developer.x-plane.com/sdk/XPLMDisplay/#XPLMBringWindowToFront>`__: :index:`XPLMBringWindowToFront`

.. py:function:: isWindowInFront(windowID)

 This routine returns 1 if the window you passed in is the frontmost visible window in
 its layer.

 :param windowID: Window to query
 :type windowID: :data:`XPLMWindowID`
 :return: 1 if window is in front of its layer, 0 otherwise
 :rtype: int

 See :ref:`Window layer<window-layer>` for more information.

 Thus, if you have a window at the front of the floating window layer
 (:data:`WindowLayerFloatingWindows`), this will return true even if there is a modal window (in
 layer :data:`WindowLayerModal`) above you. (Not to worry, though: in such a case, X-Plane will not
 pass clicks or keyboard input down to your layer until the window above stops “eating” the input.)

 Note that legacy windows are always placed in layer :data:`WindowLayerFlightOverlay`, while
 modern-style windows default to :data:`WindowLayerFloatingWindows`. This means it’s perfectly consistent
 to have two different plugin-created windows (one legacy, one modern) both be in the front
 (of their different layers!) at the same time.

 `Official SDK <https://developer.x-plane.com/sdk/XPLMDisplay/#XPLMIsWindowInFront>`__: :index:`XPLMIsWindowInFront`

Capsules
--------

.. py:data:: XPLMWindowID

    Capsule for XPLMWindow         
