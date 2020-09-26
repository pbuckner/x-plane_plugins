XPLMDisplay
===========
.. py:module:: XPLMDisplay


To use::

  import XPLMDisplay

This API provides the basic hooks to draw in X-Plane and create user
interface. All X-Plane drawing is done in OpenGL.  The X-Plane plug-in
manager takes care of properly setting up the OpenGL context and matrices.
You do not decide when in your code's  execution to draw; X-Plane tells you
when it is ready to have your plugin draw.

X-Plane's drawing strategy is straightforward: every "frame" the screen is
rendered by drawing the 3-d scene (dome, ground, objects, airplanes, etc.)
and then drawing the cockpit on top of it.  Alpha blending is used to
overlay the cockpit over the world (and the gauges over the panel, etc.).

There are two ways you can draw: directly and in a window.

* **Direct drawing** involves drawing to the screen before or after X-Plane
  finishes a phase of drawing.  When you draw directly, you can specify
  whether x-plane is to complete this phase or not.  This allows you to do
  three things: draw before x-plane does (under it), draw after x-plane does
  (over it), or draw instead of x-plane.

  To draw directly, you register a callback and specify what phase you want
  to intercept.  The plug-in manager will call you over and over to draw that
  phase.

  Direct drawing allows you to override scenery, panels, or anything. Note
  that you cannot assume that you are the only plug-in drawing at this
  phase.

  See :ref:`Direct Drawing`.

* **Window drawing** provides slightly higher level functionality. With window
  drawing you create a window that takes up a portion of the screen. Window
  drawing is always two dimensional. Window drawing is front-to-back
  controlled; you can specify that you want your window to be brought on
  top, and other plug-ins may put their window on top of you. Window drawing
  also allows you to sign up for key presses and receive mouse clicks.

  Note: all 2-d (and thus all window drawing) is done in 'cockpit pixels'.
  Even when the OpenGL window contains more than 1024x768 pixels, the cockpit
  drawing is magnified so that only 1024x768 pixels are available.

  See :ref:`Window Drawing`.

There are three ways to get keystrokes:

* **Keyboard Focus**: If you create a window, the window can take keyboard focus.  It will then
  receive all keystrokes.  If no window has focus, X-Plane receives
  keystrokes.  Use this to implement typing in dialog boxes, etc.  Only one
  window may have focus at a time; your window will be notified if it loses
  focus.

* **Hot Key**: If you need to associate key strokes with commands/functions in your
  plug-in, use a hot key.  A hoy is a key-specific callback.  Hotkeys are
  sent based on virtual key strokes, so any key may be distinctly mapped with
  any modifiers.  Hot keys  can be remapped by other plug-ins.  As a plug-in,
  you don't have to worry about  what your hot key ends up mapped to; other
  plug-ins may provide a UI for remapping keystrokes.  So hotkeys allow a
  user to resolve conflicts and customize keystrokes.

  See :ref:`Hot Keys`

* **Key Sniffer** If you need low level access to the keystroke stream, install a key
  sniffer.  Key sniffers can be installed above everything or right in front
  of the sim.

  See :ref:`Key Sniffing`

.. _Direct Drawing:

Direct Drawing
--------------

Basic drawing callbacks are for low level intercepting of render loop. The
purpose of drawing callbacks is to provide targeted additions or
replacements to x-plane's graphics environment (for example, to add extra
custom objects, or replace drawing of the AI aircraft).  Do not assume that
the drawing callbacks will be called in the order implied by the
enumerations. Also do not assume that each drawing phase ends before
another begins; they may be nested.

.. note:: Laminar says, "Note that all APIs in this second are depreacated, and
          will likely be removed during the X-Plane 11 run as part of the
          transition to Vulkan/Metal/etc. See :mod:`XPLMInstance` API for
          future-proof drawing of 3-D objects.

.. _XPLMDrawingPhase:

XPLMDrawingPhase
****************

This constant indicates which part of drawing we are in.  Drawing is done
from the back to the front.  We get a callback before or after each item.
Metaphases provide access to the beginning and end of the 3d (scene) and 2d
(cockpit) drawing in a manner that is independent of new phases added via
x-plane implementation.

 .. warning:: As X-Plane's scenery evolves, some drawing phases may cease to
   exist and new ones may be invented. If you need a particularly specific
   use of these codes, consult Austin and/or be prepared to revise your code
   as X-Plane evolves.

 .. data:: xplm_Phase_Modern3D
    :value: 31

    A chance to do modern 3D drawing. This is supported under OpenGL and Vulkan. It **is not supported under Metal**.
    It comes with potentially a substantial performance overhead. Please **do not** opt into this
    phase if you don't do any actual drawing that request the depth buffer in some way!
 
 .. data:: xplm_Phase_FirstCockpit
    :value: 35
 
    This is the first phase where you can draw in 2-d.
 
 .. data::  xplm_Phase_Panel
    :value: 40

    The non-moving parts of the aircraft panel.
 
 .. data:: xplm_Phase_Gauges
    :value: 45
 
    The moving parts of the aircraft panel.
 
 .. data:: xplm_Phase_Window
    :value: 50
 
    Floating windows from plugins.
 
 .. data::  xplm_Phase_LastCockpit
    :value: 55

    The last chance to draw in 2d.
 
Draw Callbacks
**************

Register and Unregister your drawing callback(s). You may register a callback multiple times for
the same or different phases as long as the reference constant is unique for each registration.

 .. py:function:: XPLMDrawCallback_f(inPhase: int, inIsBefore: int, inRefcon: object) -> int:
 
  Prototype for a low level drawing callback.
 
  :param inPhase: Current drawing phase
  :type inPhase: int (xplm_Phase_*)                
  :param inIsBefore: Are we before (=0) or after (=1) current phase.
  :type inIsBefore: int (0/1)                    
  :param inRefcon: Reference constant you specified when registering the callback
  :type inRefcon: object                  
  :return: 0= Suppress x-plane drawing; 1=let x-plane draw. Only used if ``inIsBefore == 0``
  :rtype: int
 
  You are passed in the current drawing phase and whether it is before or after. If you are
  before the phase, return 1 to let x-plane draw or 0 to suppress x-plane
  drawing. If you are after the phase the return value is ignored.
 
  Refcon is a unique value that you specify when registering the callback.
 
  Upon entry the OpenGL context will be correctly set up for you and OpenGL
  will be in 'local' coordinates for 3d drawing and panel coordinates for 2d
  drawing.  The OpenGL state (texturing, etc.) will be unknown.
 
 .. py:function:: XPLMRegisterDrawCallback(inCallback: callable, inPhase: int, inWantsBefore: int, inRefcon: object) -> int:
 
  Register a low level drawing callback.
 
  :param inCallback: Your callback function
  :type inCallback: callable :py:func:`XPLMDrawCallback_f`
  :param inPhase: Phase you want to be called for 
  :type inPhase: int (:ref:`XPLMDrawingPhase`)
  :param inWantsBefore: whether you want to be called before or after phase
  :type inWantsBefore: int (0= before, 1= after)
  :param inRefcon: Reference constant to be passed back to you within the callback                      
  :type inRefcon: object
  :return: 1= success
  :rtype: int
 
  Pass in the phase you want to be called for and whether you want to be
  called before or after. This routine returns 1 if the registration was
  successful, or 0 if the phase does not exist in this version of x-plane.
  You may register a callback multiple times for the same or different
  phases as long as the refcon is unique for each time.
 
 .. py:function:: XPLMUnregisterDrawCallback(inCallback: callable, inPhase: int, inWantsBefore: int, inRefcon: object) -> int:
 
  Unregister a low level drawing callback.
 
  :param inCallback: Your callback function
  :type inCallback: callable :py:func:`XPLMDrawCallback_f`
  :param inPhase: Phase you registered to be called for 
  :type inPhase: int (:ref:`XPLMDrawingPhase`)
  :param inWantsBefore: whether you registered to be called before or after phase
  :type inWantsBefore: int (0= before, 1= after)
  :param inRefcon: Reference constant to be passed back to you within the callback                      
  :type inRefcon: object
  :return: 1= success
  :rtype: int          
 
  You must unregister a callback for each time you register a callback if
  you have registered it multiple times with different refcons.
 

.. _Window Drawing:

Window Drawing
--------------

The window API provides a high-level abstraction for drawing with UI interaction.

Windows are created via :func:`XPLMCreateWindowEx`. and have access to new X-Plane 11 windowing
features, like support for new positioning modes (including being "popped out" into their own first-class
window in the operating system). The can also optionally be decorated in the style of X-Plane 11 windows
(like the map). (Pre X-Plane 11.50 function XPLMCreateWindow is deprecated.

Windows operate in "boxel" units. A boxel ("box of pixels") is a unit of virtual pixels which,
depending on X-Plane's scaling, may ocrrespond to an orbitrary NxN "box" of real pixels on screen.
Because X-Plane handles this scaling automatically, you can effectively treat the units as though you
where simply drawing in pixels, and know that when X-Plane is running with 150% or 200% scaling, your
draing will be automatically scaled (and likewise all mouse coordinates, screen bounds, etc. will also be auto-scaled).

Modern windows are not constrained to the main window, they have their origin in the lower left of the entire
global desktop space, and the lower left of the main X-Plane window is not guaranteed to
be (0, 0). X increases as you move left, and Y increases as you move up.

.. _XPLMWindowID:

XPLMWindowID
************

WindowID is an opaque indentifier for a window. You use it to control your window. When you
create a window (:func:`XPLMCreateWindowEx`) you will specify callbacks to handle drawing,
mouse interaction, etc. XPLMCreateWindowEx requires you to provide five callback, with an optional
sixth callback:

* :func:`XPLMDrawWindow_f`
* :func:`XPLMHandleMouseClick_f` for left-clicks within your window
* :func:`XPLMHandleKey_f`
* :func:`XPLMHandleCursor_f`: to indicate how to display the cursor while over your window
* :func:`XPLMHandleMouseWheel_f`
* XPLMHandleRightClick_f: (Same as :func:`XPLMHandleMouseClick_f`) for right-clicks within your window

Window Drawing Callbacks
************************

These are the callbacks you'll provide and pass into :func:`XPLMCreateWindowEx` when creating
the window, using the :ref:`XPLMCreateWindow_t` structure.

 .. py:function:: XPLMDrawWindow_f(inWindowID: int, inRefcon: object) -> None:
 
  Window drawing callback prototype.
 
  :param inWindowID: WindowID of window to be drawn
  :type inWindowID: int (:ref:`XPLMWindowID`)                    
  :param inRefCon: reference constant you provided on registration
  :type inRefCon: object
  :return: None
 
  This function handles drawing. You are passed in your window and its
  refcon. Draw the window. You can use XPLM functions to find the current
  dimensions of your window, etc.  When this callback is called, the OpenGL
  context will be set properly for cockpit drawing. NOTE: Because you are
  drawing your window over a background, you can make a translucent window
  easily by simply not filling in your entire window's bounds.
 
 
 .. py:function::  XPLMHandleMouseClick_f(inWindowID: int, x: int, y: int, inMouse: int, inRefcon: object) -> int:
 
  Mouse handling (except for mouse wheels) callback prototype. Same signature
  for Left-clicks and Right-clicks. (Note if you do use the same callback for both
  right and left clicks, you cannot determine from the parameters if you are
  being called due to a right or left click. For this reason, you might want to
  use two different functions.)
 
  :param inWindowID: WindowID of window receiving the mouse click
  :type inWindowID: int (:ref:`XPLMWindowID`)
  :param x: horizontal position of mouse
  :type x: int
  :param y: vertical position of mouse
  :type y: int           
  :param inMouse: flag, one of :ref:`XPLMMouseStatus`
  :type inMouse: int
  :param inRefcon: reference constant you provided with window registration
  :type inRefcon: object
  :return: 1= consume the click, or 0= to pass it through.
  :rtype: int
 
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
 
 
 .. py:function:: XPLMHandleKey_f(inWindowID: int, inKey: int, inFlags: int, inVirtualKey: int, inRefcon: object, losingFocus: int) -> None:
 
  Window keyboard input handling callback prototype.
 
  :param inWindowID: WindowID of window receiving the key press or focus
  :type inWindowID: int (:ref:`XPLMWindowID`)
  :param inKey: Key pressed
  :type inKey: int               
  :param inFlags: Or'd values for Shift / Ctrl, etc.
  :type inFlags: int (:ref:`XPLMKeyFlags`)
  :param inVirtualKey: Virtual key code
  :type inVirtualKey: int (:ref:`Virtual Key Codes`)
  :param inRefcon: reference constant you provided on registration
  :type inRefcon: object
  :param losingFocus: 1= your window is losing keyboard focus (and inKey should be ignored)
  :type losingFocus: int
 
  This function is called when a key is pressed or keyboard focus is taken
  away from your window.  If losingFocus is 1, you are losing the keyboard
  focus, otherwise a key was pressed and inKey contains its character.  You
  are also passed your window and a refcon.
  
 
 .. py:function:: XPLMHandleCursor_f(inWindowID: int, x: int, y: int, inRefcon: object) -> int:
 
  Mouse cursor handling callback prototype.
 
  :param inWindowID: WindowID of window receiving the key press or focus
  :type inWindowID: int (:ref:`XPLMWindowID`)
  :param x: horizontal position of mouse
  :type x: int
  :param y: vertical position of mouse
  :type y: int           
  :param inRefcon: reference constant you provided on registration
  :type inRefcon: object
  :return: Cursor status
  :rtype: int (one of :ref:`XPLMCursorStatus`)
 
  The SDK calls your cursor status callback when the mouse is over your
  plugin window.  Return a cursor status code to indicate how you would like
  X-Plane to manage the cursor.  If you return :data:`xplm_CursorDefault`, the SDK
  will try lower-Z-order plugin windows, then let the sim manage the cursor.
  
  .. note:: you should never show or hide the cursor yourself - these APIs are
   typically reference-counted and thus  cannot safely and predictably be used
   by the SDK.  Instead return one of :data:`xplm_CursorHidden` to hide the cursor or
   :data:`xplm_CursorArrow`/:data:`xplm_CursorCustom` to show the cursor.
  
  If you want to implement a custom cursor by drawing a cursor in OpenGL, use
  :data:`xplm_CursorHidden` to hide the OS cursor and draw the cursor using a 2-d
  drawing callback (after :data:`xplm_Phase_Window` is probably a good choice).  If
  you want to use a custom OS-based cursor, use :data:`xplm_CursorCustom` to ask
  X-Plane to show the cursor but not affect its image.  You can then use an
  OS specific call like SetThemeCursor (Mac) or SetCursor/LoadCursor
  (Windows).
  
 
 .. py:function:: XPLMHandleMouseWheel_f(inWindowID: int, x: int, y: int, wheel: int, clicks: int, inRefcon: object) -> int:
 
  Mouse wheel handling callback prototype.
 
  :param inWindowID: WindowID of window receiving the key press or focus
  :type inWindowID: int (:ref:`XPLMWindowID`)
  :param x: horizontal position of mouse
  :type x: int
  :param y: vertical position of mouse
  :type y: int           
  :param wheel: 0= vertical axis, 1= horizonal axis
  :type wheel: int
  :param clicks: number of "clicks" indicating how far the wheel has turned since previous callback
  :type clicks: int
  :param inRefcon: reference constant you provided on registration
  :type inRefcon: object
  :return: 1= consume the mouse wheel click, 0= pass to lower window                
  :rtype: int
 
  The SDK calls your mouse wheel callback when one of the mouse wheels is
  turned within your window.  Return 1 to consume the  mouse wheel clicks or
  0 to pass them on to a lower window.  (You should consume mouse wheel
  clicks even if they do nothing if your window appears opaque to the user.)
  The number of clicks indicates how far the wheel was turned since the last
  callback. The wheel is 0 for the vertical axis or 1 for the horizontal axis
  (for OS/mouse combinations that support this).
 
  The units for x and y values matches the units used in your window (i.e., boxels),
  with origin in lower left of global desktop space.
 
Window Drawing Enums
********************

.. _XPLMMouseStatus:

XPLMMouseStatus
+++++++++++++++

When the mouse is clicked, your mouse click routine is called repeatedly.
It is first called with the mouse down message.  It is then called zero or
more times with the mouse-drag message, and finally it is called once with
the mouse up message.  All of these messages will be directed to the same
window.

 .. data:: xplm_MouseDown
  :value: 1
 .. data:: xplm_MouseDrag
  :value: 2
 .. data:: xplm_MouseUp
  :value: 3


.. _XPLMCursorStatus:

XPLMCursorStatus
++++++++++++++++

Describes how you would like X-Plane to manage the cursor.
See :func:`XPLMHandleCursor_f` for more info.

 .. data:: xplm_CursorDefault
   :value: 0
     
   X-Plane manages the cursor normally, plugin does not affect the cursor.
     
 .. data:: xplm_CursorHidden
   :value: 1
  
   X-Plane hides the cursor.          
     
 .. data:: xplm_CursorArrow
   :value: 2
     
   X-Plane shows the cursor as the default arrow.
     
 .. data:: xplm_CursorCustom
   :value: 3
     
   X-Plane shows the cursor but lets you select an OS cursor.
 
.. _XPLMWindowLayer:

XPLMWindowLayer
+++++++++++++++

Describes where in the ordering of windows X-Plane should place
a particular window. Windows in higher layers cover windows in lower layers.
So, a given window might be at the top of its particular layer, but it might
still be obscured by a window in a higher layer. (This happens frequently when
floating windows, like X-Plane’s map, are covered by a modal alert.)

Your window’s layer can only be specified when you create the window (in the
:ref:`XPLMCreateWindow_t` you pass to :func:`XPLMCreateWindowEx`). For this reason, layering
only applies to windows created with new X-Plane 11 GUI features.

 .. data::  xplm_WindowLayerFlightOverlay
   :value: 0
 
   The lowest layer, used for HUD-like displays while flying.
 
 .. data:: xplm_WindowLayerFloatingWindows
   :value: 1
 
    Windows that "float" over the sim, like the X-Plane
    11 map does. If you are not sure which layer to
    create your window in, choose floating
 
 .. data:: xplm_WindowLayerModel
    :value: 2

     An interruptive modal that covers the sim with a
     transparent black overaly to draw the user's focus to the alert.
 
 .. data::  xplm_WindowLayerGrowlNotifications
    :value: 3

     "Growl"-style notifications that are visible in a corner of the screen, even over modals.
 
.. _XPLMWindowDecoration:

XPLMWindowDecoration
++++++++++++++++++++

Describes how “modern” windows will be displayed. This
impacts both how X-Plane draws your window as well as certain mouse handlers.

Your window’s decoration can only be specified when you create the window
(in the :ref:`XPLMCreateWindow_t` you pass to :func:`XPLMCreateWindowEx`).

 .. data:: xplm_WindowDecorationNone
  :value: 0
 
  X-Plane will draw no decoration for
  your window, and apply no automatic
  click handlers. The window will not
  stop click from passing through its
  bounds. This is suitable for "windows"
  which request, say, the full screen
  bounds, then only draw in a small
  portion of the available area.
 
 .. data:: xplm_WindowDecorationRoundRectangle
    :value: 1
 
    The default decoration for
    "native" windows, like the map.
    Provides a solid background, as
    well as click handlers for resizing
    and dragging the window.
 
 .. data::   xplm_WindowDecorationSelfDecorated
    :value: 2
 
     X-Plane will draw no decoration
     for your window, nor will it
     provide resize handlers for your
     window edges, but it will stop
     clicks from passing through your
     windows bounds.
 
 .. data::   xplm_WindowDecorationSelfDecoratedResizable
     :value: 3

     Like self-decorated, but with
     resizing; X-Plane will draw no
     decoration for your window, but
     it will stop clicks from passing
     through your windows bounds, and
     provide automatic mouse handlers
     for resizing.
 
.. _XPLMWindowPositioningMode:

XPLMWindowPositioningMode
+++++++++++++++++++++++++

XPLMWindowPositionMode describes how X-Plane will position your window on the user’s screen. X-Plane will
maintain this positioning mode even as the user resizes their window or adds/removes full-screen monitors.

Positioning mode can only be set for “modern” windows (that is, windows created using :func:`XPLMCreateWindowEx`
and compiled against the XPLM300 SDK). Windows created using the deprecated XPLMCreateWindow(), or windows
compiled against a pre-XPLM300 version of the SDK will simply get the “free” positioning mode.

 .. data:: xplm_WindowPositionFree
  :value: 0
 
  The default positioning mode. Set the window geometry and its
  future position will be determined by its window gravity, resizing
  limits, and user interactions.
 
 .. data:: xplm_WindowCenterOnMonitor
  :value: 1
 
  Keep the window centered on the monitor you specify
 
 .. data:: xplm_WindowFullScreenOnMonitor
  :value: 2

  Keep the window full screen on the monitor you specify
 
 .. data:: xplm_WindowFullScreenOnAllMonitors
  :value: 3
 
  Like gui_window_full_screen_on_monitor, but stretches
  over *all* monitors and popout windows.
  This is an obscure one... unless you have a very good
  reason to need it, you probably don't!
 
 .. data:: xplm_WindowPopOut
  :value: 4

  A first-class window in the operating system, completely
  separate from the X-Plane window(s)
 
 .. data:: xplm_WindowVR
  :value: 5
 
  A floating window visible on the VR headset
 

.. _XPLMCreateWindow_t:

XPLMCreateWindow_t
******************

The XPLMCreateWindow_t structure defines all of the parameters used to create a modern
window using :func:`XPLMCreateWindowEx`. The structure will be expanded in future SDK APIs
to include more features.

All windows created by this function in the XPLM300 version of the API are created
with the new X-Plane 11 GUI features. This means your plugin will get to “know” about
the existence of X-Plane windows other than the main window. All drawing and mouse
callbacks for your window will occur in “boxels,” giving your windows automatic
support for high-DPI scaling in X-Plane. In addition, your windows can opt-in to
decoration with the X-Plane 11 window styling, and you can use the
:func:`XPLMSetWindowPositioningMode` API to make your window “popped out” into a First-class
operating system window.

Note that this requires dealing with your window’s bounds in “global desktop” positioning
units, rather than the traditional panel coordinate system. In global desktop coordinates,
the main X-Plane window may not have its origin at coordinate (0, 0), and your own window
may have negative coordinates. Assuming you don’t implicitly assume (0, 0) as your origin,
the only API change you should need is to start using :func:`XPLMGetMouseLocationGlobal` rather
than (deprecated) XPLMGetMouseLocation(), and :func:`XPLMGetScreenBoundsGlobal` instead
of (deprecated) XPLMGetScreenSize().

If you ask to be decorated as a floating window, you’ll get the blue window control bar
and blue backing that you see in X-Plane 11’s normal “floating” windows (like the map).

The structure is tuple:

| ``(``
|   ``int left, int top, int right, int bottom,``
|   ``int visible,``
|   :func:`XPLMDrawWindow_f` ``drawWindowFunc,``
|   :func:`XPLMHandleMouseClick_f` ``handleMouseClickFunc,  # May be None``
|   :func:`XPLMHandleKey_f` ``handleKeyFunc,``
|   :func:`XPLMHandleCursor_f` ``handleCursorFunc,``
|   :func:`XPLMHandleMouseWheel_f` ``handleMouseWheelFunc,``
|   ``object refcon,``
|   :ref:`XPLMWindowDecoration` ``decorateAsFloatingWindow,``
|   :ref:`XPLMWindowLayer` ``layer,``
|   :func:`XPLMHandleMouseClick_f` ``handleRightClickFun  # May be None``
| ``)``
  


Window Drawing Functions
************************

.. py:function:: XPLMCreateWindowEx(createWindowTuple) -> int:

 This routine creates a new “modern” window. You pass in an :ref:`XPLMCreateWindow_t` tuple
 with all of the fields set in. Also, you must provide functions for every
 callback—you may not leave them null! (If you do not support the cursor or mouse wheel,
 use functions that return the default values.)

 :param createWindowTuple: :ref:`XPLMCreateWindow_t`
 :return: Created :ref:`XPLMWindowID`
 :rtype: int


.. py:function:: XPLMDestroyWindow(inWindowID: int) -> None:

 Destroys a window based on the handle passed in.

 The callbacks are not called after this call. Keyboard focus is removed
 from the window before destroying it.


.. py:function:: XPLMGetScreenSize() -> (int, int):

 Query X-Plane screen size.
 This routine returns the size of the size of the X-Plane OpenGL window in
 pixels.  Please note that this is not the size of the screen when  doing
 2-d drawing (the 2-d screen is currently always 1024x768, and  graphics are
 scaled up by OpenGL when doing 2-d drawing for higher-res monitors).  This
 number can be used to get a rough idea of the amount of detail the user
 will be able to see when drawing in 3-d.

 :return: (width, height)


.. py:function::  XPLMGetScreenBoundsGlobal() -> (int, int, int, int):

 This routine returns the bounds of the “global” X-Plane desktop, in boxels.
 Unlike the non-global version :func:`XPLMGetScreenSize`, this is multi-monitor
 aware. There are three primary consequences of multimonitor awareness.

 First, if the user is running X-Plane in full-screen on two or more monitors
 (typically configured using one full-screen window per monitor), the global
 desktop will be sized to include all X-Plane windows.

 Second, the origin of the screen coordinates is not guaranteed to be (0, 0).
 Suppose the user has two displays side- by-side, both running at 1080p.
 Suppose further that they’ve configured their OS to make the left display
 their “primary” monitor, and that X-Plane is running in full-screen on their
 right monitor only. In this case, the global desktop bounds would be the
 rectangle from (1920, 0) to (3840, 1080). If the user later asked X-Plane to
 draw on their primary monitor as well, the bounds would change to (0, 0) to
 (3840, 1080).

 Finally, if the usable area of the virtual desktop is not a perfect rectangle
 (for instance, because the monitors have different resolutions or because one
 monitor is configured in the operating system to be above and to the right of
 the other), the global desktop will include any wasted space. Thus, if you have
 two 1080p monitors, and monitor 2 is configured to have its bottom left touch
 monitor 1’s upper right, your global desktop area would be the rectangle from
 (0, 0) to (3840, 2160).

 Note that popped-out windows (windows drawn in their own operating system
 windows, rather than “floating” within X-Plane) are not included in these bounds.

 :return: (left, top, right, bottom)


.. py:function:: XPLMGetAllMonitorBoundsGlobal(inMonitorBoundsCallback: callable, inRefcon: object) -> None:

 This routine immediately calls you back with the bounds (in boxels) of each
 full-screen X-Plane window within the X- Plane global desktop space. Note that
 if a monitor is not covered by an X-Plane window, you cannot get its bounds this
 way. Likewise, monitors with only an X-Plane window (not in full-screen mode)
 will not be included.

 If X-Plane is running in full-screen and your monitors are of the same size and
 conUgured contiguously in the OS, then the combined global bounds of all full-screen
 monitors will match the total global desktop bounds, as returned by
 :func:`XPLMGetScreenBoundsGlobal`. (Of course, if X-Plane is running in windowed mode,
 this will not be the case. Likewise, if you have differently sized monitors, the
 global desktop space will include wasted space.)
 
 Note that this function’s monitor indices match those provided by
 :func:`XPLMGetAllMonitorBoundsOS`, but the coordinates are different (since the X-Plane
 global desktop may not match the operating system’s global desktop, and one X-Plane
 boxel may be larger than one pixel due to 150% or 200% scaling).
 
 Callback is::

       def inMonitorBoundsCallback(inMonitorIndex,
                                   inLeftBx, inTopBx, inRightBx, inBottomBx,
                                   inRefcon):
            pass

 This function is informed of the global bounds (in boxels) of a particular monitor
 within the X-Plane global desktop space. Note that X-Plane must be running in full
 screen on a monitor in order for that monitor to be passed to you in this callback.

.. py:function:: XPLMGetAllMonitorBoundsOS(inMonitorBoundCallback: callable, inRefcon: object) -> None:

 This routine immediately calls you back with the bounds (in pixels) of each monitor
 within the operating system’s global desktop space. Note that unlike
 :func:`XPLMGetAllMonitorBoundsGlobal`, this may include monitors that have no X-Plane window
 on them.

 Note that this function’s monitor indices match those provided by
 :func:`XPLMGetAllMonitorBoundsGlobal`, but the coordinates are different (since the X-Plane
 global desktop may not match the operating system’s global desktop, and one X-Plane
 boxel may be larger than one pixel).

 Callback is::

       def inMonitorBoundsCallback(inMonitorIndex,
                                   inLeftPx, inTopPx, inRightPx, inBottomPx,
                                   inRefcon):
            pass

 This function is informed of the global bounds (in pixels) of a particular monitor
 within the operating system’s global desktop space. Note that a monitor index being
 passed to you here does not indicate that X-Plane is running in full screen on this
 monitor, or even that any X-Plane windows exist on this monitor.



.. py:function:: XPLMGetMouseLocationGlobal() -> (int, int):

 Returns the current mouse location in global desktop boxels. Unlike
 :func:`XPLMGetMouseLocation`, the bottom left of the main X-Plane window is not guaranteed
 to be (0, 0)—instead, the origin is the lower left of the entire global desktop space.
 In addition, this routine gives the real mouse location when the mouse goes to X-Plane
 windows other than the primary display. Thus, it can be used with both pop-out windows
 and secondary monitors.
 
 This is the mouse location function to use with modern windows (i.e., those created by
 :func:`XPLMCreateWindowEx`).

 :return: (x, y)


.. py:function:: XPLMGetWindowGeometry(inWindowID: int) -> (int, int, int, int):

 This routine returns the position and size of a window. The units and coordinate
 system vary depending on the type of window you have.

 If this is a legacy window (one compiled against a pre-XPLM300 version of the SDK,
 or an XPLM300 window that was not created using :func:`XPLMCreateWindowEx`), the units
 are pixels relative to the main X-Plane display.

 If, on the other hand, this is a new X-Plane 11-style window (compiled against the
 XPLM300 SDK and created using :func:`XPLMCreateWindowEx`), the units are global desktop boxels.

 :return: (left, top, right, bottom)


.. py:function:: XPLMSetWindowGeometry(inWindowID: int, inLeft: int, inTop: int, inRight:int, inBottom: int):

 Set window position and size.

 This routine allows you to set the position and size of a window.

 The units and coordinate system match those of :func:`XPLMGetWindowGeometry`. That is,
 modern windows use global desktop boxel coordinates, while legacy windows use
 pixels relative to the main X-Plane display.

 Note that this only applies to “floating” windows (that is, windows that are drawn
 within the X-Plane simulation windows, rather than being “popped out” into their
 own first-class operating system windows). To set the position of windows whose
 positioning mode is :data:`xplm_WindowPopOut`, you’ll need to instead use :func:`XPLMSetWindowGeometryOS`.


.. py:function:: XPLMGetWindowGeometryOS(inWindowID: int) -> (int, int, int, int):

 This routine returns the position and size of a “popped out” window (i.e., a window
 whose positioning mode is xplm_WindowPopOut), in operating system pixels.

 :return: (left, top, right, bottom)


.. py:function:: XPLMSetWindowGeometryOS(inWindowID: int, inLeft: int, inTop: int, inRight: int, inBottom: int) -> None:

 This routine allows you to set the position and size, in operating system pixel
 coordinates, of a popped out window (that is, a window whose positioning mode
 is :data:`xplm_WindowPopOut`, which exists outside the X-Plane simulation window, in its
 own first-class operating system window).

 Note that you are responsible for ensuring both that your window is popped out
 (using :func:`XPLMWindowIsPoppedOut`) and that a monitor really exists at the OS coordinates
 you provide (using :func:`XPLMGetAllMonitorBoundsOS`).


.. py:function:: XPLMGetWindowGeometryVR(inWindowID: int) -> (int, int):

 Returns the width and height, in boxels, of a window in VR. Note that you are responsible
 for ensuring your window is in VR (using :func:`XPLMWindowIsInVR`).

 :return: (widthBoxels, heightBoxels)


.. py:function:: XPLMSetWindowGeometryVR(inWindowID: int, widthBoxels: int, heightBoxels: int) -> None:

 This routine allows you to set the size, in boxels, of a window in VR (that is, a
 window whose positioning mode is :data:`xplm_WindowVR`).

 Note that you are responsible for ensuring your window is in VR (using :func:`XPLMWindowIsInVR`).


.. py:function:: XPLMGetWindowIsVisible(inWindowID: int) -> int:

 Get window's isVisible attribute value.

 :return: 1= visible


.. py:function::  XPLMSetWindowIsVisible(inWindowID: int, inIsVisible: int) -> None:

 Set window's isVisible attribute value.

 :param inIsVisible: 1=visible


.. py:function:: XPLMWindowIsPoppedOut(inWindowID: int) -> int:

 True if this window has been popped out (making it a first-class window in the
 operating system), which in turn is true if and only if you have set the
 window’s positioning mode to :data:`xplm_WindowPopOut`.
 
 Only applies to modern windows. (Windows created using the deprecated
 XPLMCreateWindow(), or windows compiled against a pre-XPLM300 version of the
 SDK cannot be popped out.)

 :return: 1= True



.. py:function:: XPLMWindowIsInVR(inWindowID: int) -> int:

 True if this window has been moved to the virtual reality (VR) headset, which
 in turn is true if and only if you have set the window’s positioning mode to :data:`xplm_WindowVR`.

 Only applies to modern windows. (Windows created using the deprecated XPLMCreateWindow(), or windows
 compiled against a pre-XPLM301 version of the SDK cannot be moved to VR.)

 :return: 1= True


.. py:function::  XPLMSetWindowGravity(inWindowID: int, inLeftGravity: float, inTopGravity: float, inRightGravity: float, inBottomGravity: float) -> None:

 A window’s “gravity” controls how the window shifts as the whole X-Plane window
 resizes. A gravity of 1 means the window maintains its positioning relative to the right or top
 edges, 0 the left/bottom, and 0.5 keeps it centered.
 
 Default gravity is (0.0, 1.0, 0.0, 1.0), meaning your window will maintain its position relative
 to the top left and will not change size as its containing window grows.
 
 If you wanted, say, a window that sticks to the top of the screen (with a constant height), but
 which grows to take the full width of the window, you would pass (0.0, 1.0, 1.0, 1.0). Because
 your left and right edges would maintain their positioning relative to their respective edges
 of the screen, the whole width of your window would change with the X-Plane window.
 
 Only applies to modern windows. (Windows created using the deprecated XPLMCreateWindow(), or
 windows compiled against a pre-XPLM300 version of the SDK will simply get the default gravity.)



.. py:function:: XPLMSetWindowResizingLimits(inWindowID: int, inMinWidthBoxels: int, inMinHeightBoxels: int, inMaxWidthBoxels: int, inMaxHightBoxels: int) -> None:

 Sets the minimum and maximum size of the client rectangle of the given window. (That is,
 it does not include any window styling that you might have asked X-Plane to apply on your
 behalf.) All resizing operations are constrained to these sizes.
 
 Only applies to modern windows. (Windows created using the deprecated XPLMCreateWindow(),
 or windows compiled against a pre-XPLM300 version of the SDK will have no minimum or maximum size.)


.. py:function:: XPLMSetWindowPositioningMode(inWindowID: int, inPositioningMode: int, inMonitorIndex: int) -> None:

 Sets the policy for how X-Plane will position your window.

 Some positioning modes apply to a particular monitor. For those modes, you can pass a negative
 monitor index to position the window on the main X-Plane monitor (the screen with the X-Plane
 menu bar at the top). Or, if you have a speciUc monitor you want to position your window on,
 you can pass a real monitor index as received from, e.g., :func:`XPLMGetAllMonitorBoundsOS`.

 Only applies to modern windows. (Windows created using the deprecated XPLMCreateWindow(),
 or windows compiled against a pre-XPLM300 version of the SDK will always use xplm_WindowPositionFree.)


.. py:function:: XPLMSetWindowTitle(inWindowID: int, inWindowTitle: str) -> None:

 Sets the name for a window. This only applies to windows that opted-in to styling as an X-Plane
 11 floating window (i.e., with styling mode :data:`xplm_WindowDecorationRoundRectangle`) when they
 were created using :func:`XPLMCreateWindowEx`.


.. py:function:: XPLMGetWindowRefCon(inWindowID: int) -> object:

 Return window's refCon attribute value (which you provided on window creation.)


.. py:function:: XPLMSetWindowRefCon(inWindowID: int, inRefcon: object) -> None:

 Set window's refcon attribute value.
 Use this to pass data to yourself in the callbacks.


.. py:function:: XPLMTakeKeyboardFocus(inWindowID: int) -> None:

 Give a specific window keyboard focus.

 This routine gives a speciUc window keyboard focus. Keystrokes will be sent to that window.
 Pass a window ID of 0 to remove keyboard focus from any plugin-created windows and instead
 pass keyboard strokes directly to X-Plane.


.. py:function:: XPLMHasKeyboardFocus(inWindowID: int) -> int:

 Returns 1 if the indicated window has keyboard focus. Pass a window ID of 0 to see
 if no plugin window has focus, and all keystrokes will go directly to X-Plane.


.. py:function:: XPLMBringWindowToFront(inWindowID: int) -> None:

 Bring window to the front of the Z-order.

 This routine brings the window to the front of the Z-order for its layer. Windows are brought
 to the front automatically when they are created. Beyond that, you should make sure you are
 front before handling mouse clicks.

 Note that this only brings your window to the front of its layer (:ref:`XPLMWindowLayer`). Thus, if
 you have a window in the floating window layer (:data:`xplm_WindowLayerFloatingWindows`), but there
 is a modal window (in layer :data:`xplm_WindowLayerModal`) above you, you would still not be the true
 frontmost window after calling this. (After all, the window layers are strictly ordered, and
 no window in a lower layer can ever be above any window in a higher one.)   Windows are brought
 to the front when they are created. Beyond that you should make sure you are front before handling
 mouse clicks.


.. py:function:: XPLMIsWindowInFront(inWindowID: int) -> int:

 This routine returns 1 if the window you passed in is the frontmost visible window in
 its layer (:ref:`XPLMWindowLayer`).

 Thus, if you have a window at the front of the floating window layer
 (:data:`xplm_WindowLayerFloatingWindows`), this will return true even if there is a modal window (in
 layer :data:`xplm_WindowLayerModal`) above you. (Not to worry, though: in such a case, X-Plane will not
 pass clicks or keyboard input down to your layer until the window above stops “eating” the input.)

 Note that legacy windows are always placed in layer :data:`xplm_WindowLayerFlightOverlay`, while
 modern-style windows default to :data:`xplm_WindowLayerFloatingWindows`. This means it’s perfectly consistent
 to have two different plugin-created windows (one legacy, one modern) both be in the front
 (of their different layers!) at the same time.

.. _Key Sniffing:

Key Sniffing
------------

.. py:function:: XPLMKeySniffer_f(inChar: int, inFlags: int, inVirtualKey: int, inRefcon: object) -> int:

 Prototype for a low level key-sniffing callback.

 :param inChar: the character pressed
 :type inChar: int
 :param inFlags: Or'd values for Shift / Ctrl, etc.
 :type inFlags: int (:ref:`XPLMKeyFlags`)
 :param inVirtualKey: Virtual key code
 :type inVirtualKey: int (:ref:`Virtual Key Codes`)
 :param inRefCon: Reference constant you specified when registering the callback
 :type inRefcon: object
 :return: 0= consume the key, 1= pass the key on to the next sniffer
          window manager, x-plane, or whomever is down stream.

 Window-based UI should not use this! The windowing system provides
 high-level mediated keyboard access. By comparison, the key sniffer
 provides low level keyboard access.

 Key sniffers are provided to allow libraries to provide non-windowed user
 interaction. For example, the MUI library uses a key sniffer to do pop-up
 text entry.

.. py:function:: XPLMRegisterKeySniffer(inCallback: callable, inBeforeWindows: int, inRefcon: object) -> int:

 This routine registers a key sniffing callback. You specify whether you want to sniff before
 the window system, or only sniff keys the window system does not consume. You should ALMOST
 ALWAYS sniff non-control keys after the window system. When the window system consumes a key, it
 is because the user has “focused” a window. Consuming the key or taking action based on the key
 will produce very weird results.

 Returns 1 if successful.

 Your callback::

    def snifferCallback(inChar, inFlags, inVirtualKey, inRefcon):
        return int  # 1=pass the key to next sniffer, 0=consume the key

 This is the prototype for a low level key-sniffing function. Window-based UI should not use this!
 The windowing system provides high-level mediated keyboard access, via the callbacks you attach
 to your XPLMCreateWindow_t. By comparison, the key sniffer provides low level keyboard access.

 Key sniffers are provided to allow libraries to provide non-windowed user interaction. For example,
 the MUI library uses a key sniffer to do pop-up text entry. Return 1 to pass the key on to the next sniffer,
 the window manager, X-Plane, or whomever is down stream. Return 0 to consume the key.


.. py:function:: XPLMUnregisterKeySniffer(inCallback: callable, inBeforeWindows: int, inRefcon: object) -> int:

 This routine unregisters a key sniffer. You must unregister a key sniffer for every time you register
 one with the exact same signature. Returns 1 if successful.


.. _Hot Keys:

Hot Keys
--------

Keystrokes that can be managed by others. These are lower-level than window keyboard handlers (i.e., callbacks you attach to your
:ref:`XPLMCreateWindow_t`, but higher leven than sniffers.

.. py:function:: XPLMHotKey_f(inRefcon: object) -> None:
  
 Hotkey callback. ``inRefcon`` is the object you provided on registration.


.. py:function:: XPLMRegisterHotKey(inVirtualKey: int, inFlags: int, inDescription: str, inCallback: callable, inRefcon: object) -> int:

 Register a hotkey

 :param inVirtualKey: Hot key to be pressed to activate
 :type inVirtualKey: int (:ref:`Virtual Key Codes`)
 :param inFlags: Or'd values for Shift / Ctrl, to be pressed with Hot Key
 :type inFlags: int (:ref:`XPLMKeyFlags`)
 :param inDescription: description of what the hot key does
 :type inDescription: str
 :param inCallback: Your callback function, called when hot key is invoked
 :type inCallback: callable
 :param inRefcon: reference constant provided to you callback function
 :type inRefcon: object
 :return: HotKeyID
 :rtype: int (HotKeyID)

 Specify your preferred key stroke virtual key/flag combination,
 a description of what your callback does (so the other plug-ins can
 describe the plug-in to the user for remapping) and a callback function
 and opaque pointer to pass in).  A new hot key ID is returned.
 During execution, the actual key associated with your hot key
 may change, but you are insulated from this.


.. py:function:: XPLMUnregisterHotKey(inHotKey: int) -> None:

    Unregister a hotkey.

    Only your own hotkeys can be unregistered!

.. py:function:: XPLMCountHotKeys() -> int:

    Return number of hotkeys defined.

.. py:function:: XPLMGetNthHotKey(inIndex: int) -> int:

    Returns HotKeyID of Nth hotkey.


.. py:function:: XPLMGetHotKeyInfo(inHotKey: int) -> object

 Return information about the hotkey as an object with attributes:

   | virtualKey:  int (:ref:`Virtual Key Codes`)
   | flags: int (:ref:`XPLMKeyFlags`)
   | description: str
   | plugin: int (:ref:`XPLMPluginID`)

.. py:function:: XPLMSetHotKeyCombination(inHotKey: int, inVirtualKey: int, inFlags: int):

   Remap a hot key's keystroke.

   :param inHotKey: ID of hot key to be changed
   :type inHotKey: int (HotKeyID)
   :param inVirtualKey: (new) Key to be used for the hot key
   :type inVirtualKey: int (XPLM_VK_*)
   :param inFlags: Shift / Ctrl keys to be pressed with hot key
   :type inFlags: int, Or'd values of xplm_*Flags

   You may remap another plugin's keystrokes.
