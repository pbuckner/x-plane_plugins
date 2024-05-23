XPLMDisplay
===========
.. py:module:: XPLMDisplay
.. py:currentmodule:: xp

To use::

  import xp

This module contains a large number of different API. For convenience
we've grouped them on separate pages.
  
This API provides the basic hooks to draw in X-Plane and create user
interface. All X-Plane drawing is done in OpenGL.  The X-Plane plug-in
manager takes care of properly setting up the OpenGL context and matrices.
You do not decide when in your code's  execution to draw; X-Plane tells you
when it is ready to have your plugin draw.

X-Plane's drawing strategy is straightforward: every "frame" the screen is
rendered by drawing the 3-d scene (dome, ground, objects, airplanes, etc.)
and then drawing the cockpit on top of it.  Alpha blending is used to
overlay the cockpit over the world (and the gauges over the panel, etc.).

There are three ways you can draw: directly, directly onto avionics screens,
and in a window.

* **Direct drawing** involves drawing to the screen before or after X-Plane
  finishes a phase of drawing.  When you draw directly, you can specify
  whether x-plane is to complete this phase or not.  This allows you to do
  three things: draw before x-plane does (under it), draw after x-plane does
  (over it), or draw instead of x-plane.

  To draw directly, you register a callback with :py:func:`registerDrawCallback`
  and specify what phase you want
  to intercept.  The plug-in manager will call you over and over to draw that
  phase.

  Direct drawing allows you to override scenery, panels, or anything. Note
  that you cannot assume that you are the only plug-in drawing at this
  phase.

  See :doc:`display_direct`.

* **Window API** provides slightly higher level functionality. With window
  drawing you create a window with :py:func:`createWindowEx`
  that takes up a portion of the screen. Window
  drawing is always two dimensional. Window drawing is front-to-back
  controlled; you can specify that you want your window to be brought on
  top, and other plug-ins may put their window on top of you. Window API
  also allows you to sign up for key presses and receive mouse clicks, and
  control window visibility and position.

  Note: all 2-d (and thus all window drawing) is done in 'cockpit pixels'.
  Even when the OpenGL window contains more than 1024x768 pixels, the cockpit
  drawing is magnified so that only 1024x768 pixels are available.

  See :doc:`display_window`.

* **Avionics API** (X-Plane 12+) allows you to draw directly onto
  (pre-defined) avionics device screens, and programmatically create new Avionics screens.
  In addition to drawing callbacks, you can get notified about mouse and keyboard events.

  For existing devices, you'll register a callback with :py:func:`registerAvionicsCallbacksEx`
  and specify which device to draw on, and whether to draw before or after X-Plane draws.
  Starting X-Plane 12.1.0, you can register for a variety of callbacks (keyboard, mouse clicks, etc.)
  To use these features, you must be using at least XPPython3 v4.4, and X-Plane 12.1.0.

  To create new devices, you'll create the device using :py:func:`createAvionicsEx`, providing
  the necessary callbacks.

  See :doc:`display_avionics`.
  
There are three ways to get keystrokes:

* **Keyboard Focus**: If you create a window, the window can take
  keyboard focus with :py:func:`takeKeyboardFocus`.
  It will then
  receive all keystrokes.  If no window has focus, X-Plane receives
  keystrokes.  Use this to implement typing in dialog boxes, etc.  Only one
  window may have focus at a time; your window will be notified if it loses
  focus. See :doc:`display_window`.

  Similarly, you can query and take keyboard focus for avionics devices. This
  is described in :doc:`display_avionics`.

* **Hot Key**: If you need to associate key strokes with commands/functions in your
  plug-in, register a hot key with :py:func:`registerHotKey`.
  A hot key is a key-specific callback.  Hotkeys are
  sent based on virtual key strokes, so any key may be distinctly mapped with
  any modifiers.  Hot keys  can be remapped by other plug-ins.  As a plug-in,
  you don't have to worry about  what your hot key ends up mapped to; other
  plug-ins may provide a UI for remapping keystrokes.  So hotkeys allow a
  user to resolve conflicts and customize keystrokes.

  See :doc:`display_hotkeys`.

* **Key Sniffer** If you need low level access to the keystroke stream, install a key
  sniffer with :py:func:`registerKeySniffer`.
  Key sniffers can be installed above everything or right in front of the sim.

  See :doc:`display_sniffer`.


.. toctree::
   :hidden:

   /development/modules/display_direct
   /development/modules/display_window
   /development/modules/display_avionics
   /development/modules/display_sniffer
   /development/modules/display_hotkeys
   /development/window_position
