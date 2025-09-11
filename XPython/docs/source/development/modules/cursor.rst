XPCursor
========

To use:
::

   import xp

.. py:module:: XPCursor
.. py:currentmodule:: xp


Platform-independent way to use custom cursors. These routines are not
part of X-Plane SDK.

Cursor files
------------

A custom cursor consists of two files: A PNG file to be used for Mac and Linux, and
a Windows CUR (Cursor) file. Usually, you'd create the 32x32 pixel PNG file and then
use an online-converter to create the corresponding CUR file.

Both files should be located in the same directory, where they will be found
by :py:func:`loadCursor`.

XPPython3 includes some pre-defined cursors which are loaded at startup: you do
not need to separated load these as they are immediately available for use with
:py:func:`setCursor`.

Normally, there is no need to call :py:func:`unloadCursor`.

Usage
-----

You should load any custom cursors prior to use using :py:func:`loadCursor`. Repeated calls will result
in additional cursor definitions, in which case you might want to remove previous definitions
using :py:func:`unloadCursor`.

Custom cursors can be set (:py:func:`setCursor`)  in only a few very-specific callbacks:

* Within a Window :py:func:`xp.cursor` callback.

* Within Avionics :py:func:`xp.screenCursor` or :py:func:`xp.bezelCursor` callback.

* When processing a Widget message :data:`xp.Msg_CursorAdjust`.

Functions
---------

.. py:function:: loadCursor(name: str) -> int

    Loads a custom cursor from a file using the provided name and
    a platform-specific filename extension. The file to be loaded
    will be `<name>.png` for Linux and Mac; `<name>.cur` for Windows.
    Name must include relative path from the X-Plane root.

    Returns a integer ``cursor_id``, to be used with :py:func:`setCursor`
    and :py:func:`unloadCursor`. Throws an exception if file cannot be read,
    or has incorrect format.::

      >>> cursorID = xp.loadCursor('Resources/plugins/XPPython3/cursors/Arrow_Up_Black')
      >>> cursorID
      18

    .. note:: In the above example, the "Arrow_Up_Black" cursor is already loaded with cursor_id=1.
        I use it as an example because I *know* the files exist on your installation.
        You don't need to (i.e., you should not) load any of the pre-defined cursors listed below
        as that is just a waste of space.

.. py:function:: unloadCursor(cursor_id: int) -> None

    Unloads image data related to the given ``cursor_id``.::

      >>> xp.unloadCursor(18)

.. py:function:: setCursor(cursor_id: int) -> None

    Temporarily replaces system cursor with the given custom cursor. Throws an
    exception if ``cursor_id`` is not a known cursor.

    **For X-Plane windows** (i.e., created with :py:func:`xp.createWindowEx`), you should
    define a `cursor` callback, and within that callback, call :py:func:`setCursor`
    and have your callback return :data:`xp.CursorCustom`. If you return anything
    else, your cursor will not be used::

      >>> def myCursorCallback(windowID, x, y, refCon):
      ...     xp.setCursor(11)
      ...     return xp.CursorCustom
      ...
      >>> xp.createWindowEx(visible=1, cursor=myCursorCallback)

    .. image:: /images/window_pointer_cursor.png
               :width: 130px

    **For X-Plane Avionics**, you can define separate callbacks for :py:func:`xp.bezelCursor` and
    :py:func:`xp.screenCursor`. In this example, we create a simple avionics, with a green lower-half::

      >>> from XPPython3 import xpgl
      >>> def screenCB(refCon):
      ...     xpgl.drawRectangle(0, 0, 100, 100, color=(0, 1, 0))
      ...
      >>> def bezelCB(r, g, b, refCon):
      ...     xpgl.drawRectangle(0, 0, 140, 250, color=(0, 0, 0))
      ...
      >>> def cursorCB(x, y, refCon):
      ...     if x < 100 and y < 100:  # only change cursor within the green portion of screen
      ...         xp.setCursor(11)
      ...         return xp.CursorCustom
      ...     return xp.CursorDefault
      ...
      >>> avionicsID = xp.createAvionicsEx(screenCursor=cursorCB, screenDraw=screenCB, bezelDraw=bezelCB)
      >>> xp.setAvionicsPopupVisible(avionicsID)

    .. image:: /images/avionics_cursor.png
               :width: 140px

    For built-in avionics, you'll use :py:func:`xp.registerAvionicsCallbacksEx` to provide
    the screen and/or bezel cursor callbacks.

    **For Widgets**, you'll install a widget callback routine using :py:func:`xp.addWidgetCallback` (you'll
    probably already have a callback routine if you're doing much of anything with widgets.) That callback
    will receive a series of messages. When it receives the :data:`xp.Msg_CursorAdjust`, you should check
    to see if the mouse is over the widget you're interested in, or at the correct X, Y location. Then
    call :py:func:`setCursor`, update param2 to indicate :data:`xp.CursorCustom`, and return 1 to indicate
    you've consumed the event.::

      >>> widgetID = xp.createWidget(100, 200, 300, 100, 1, "My Widget", 1, 0, xp.WidgetClass_MainWindow)
      >>> subwidgetID = xp.createWidget(110, 190, 130, 170, 1, "WIDE", 0, widgetID, xp.WidgetClass_Caption)
      >>> def myCallback(message, widgetID, param1, param2):
      ...     if message == xp.Msg_CursorAdjust:
      ...         xp.setCursor(10)
      ...         param2[0] = xp.CursorCustom
      ...         return 1
      ...     return 0
      ...
      >>> xp.addWidgetCallback(subwidgetID, myCallback)

    *Why is it* ``param2[0]`` *and not* ``param2`` *like everywhere else?*

      Because we need to effectively
      pass by reference instead of pass by value for this parameter. This is a C vs. Python thing which
      we generally don't need to worry about. But for ``MSG_CursorAdjust``, we need a mutable ``param2``. The best
      way to achieve that is using a single-element list: ``param2`` stays unchanged, but its first element
      may be changed. Otherwise, your callback could change ``param2`` to anything, but the calling program
      wouldn't ever see the update!)

Preloaded Custom Cursors
------------------------

XPPython3 pre-loads a number of basic cursors with hard-coded ``cursor_id``. You can use these with :py:func:`setCursor`. Please
do not unload them as that would make them unavailable to other python plugins. These constants
*should not* be used as a :ref:`CursorStatus <XPLMCursorStatus>` return. Cursor Status should be set
to :data:`xp.CursorCustom`.

.. table::
   :align: left

   +-----------+---------------------------+--------------------------------------------------+
   | Cursor ID | Name                      | Image                                            |
   +===========+===========================+==================================================+
   | 0         | Arrow_Down_Black          | .. image:: /images/Arrow_Down_Black.png          |
   +-----------+---------------------------+--------------------------------------------------+
   | 1         | Arrow_Up_Black            | .. image:: /images/Arrow_Up_Black.png            |
   +-----------+---------------------------+--------------------------------------------------+
   | 2         | Arrow_Down_White          | .. image:: /images/Arrow_Down_White.png          |
   +-----------+---------------------------+--------------------------------------------------+
   | 3         | Arrow_Up_White            | .. image:: /images/Arrow_Up_White.png            |
   +-----------+---------------------------+--------------------------------------------------+
   | 4         | Arrow_Left_White          | .. image:: /images/Arrow_Left_White.png          |
   +-----------+---------------------------+--------------------------------------------------+
   | 5         | Arrow_Right_White         | .. image:: /images/Arrow_Right_White.png         |
   +-----------+---------------------------+--------------------------------------------------+
   | 6         | ColumnResize_Black        | .. image:: /images/ColumnResize_Black.png        |
   +-----------+---------------------------+--------------------------------------------------+
   | 7         | RowResize_Black           | .. image:: /images/RowResize_Black.png           |
   +-----------+---------------------------+--------------------------------------------------+
   | 8         | FourWay_White             | .. image:: /images/FourWay_White.png             |
   +-----------+---------------------------+--------------------------------------------------+
   | 9         | HandGrab_Up_White         | .. image:: /images/HandGrab_Up_White.png         |
   +-----------+---------------------------+--------------------------------------------------+
   |10         | HandGrabbing_Up_White     | .. image:: /images/HandGrabbing_Up_White.png     |
   +-----------+---------------------------+--------------------------------------------------+
   |11         | HandPointer_Up_White      | .. image:: /images/HandPointer_Up_White.png      |
   +-----------+---------------------------+--------------------------------------------------+
   |12         | Rotate_Large_Minus_White  | .. image:: /images/Rotate_Large_Minus_White.png  |
   +-----------+---------------------------+--------------------------------------------------+
   |13         | Rotate_Large_Plus_White   | .. image:: /images/Rotate_Large_Plus_White.png   |
   +-----------+---------------------------+--------------------------------------------------+
   |14         | Rotate_Medium_Minus_White | .. image:: /images/Rotate_Medium_Minus_White.png |
   +-----------+---------------------------+--------------------------------------------------+
   |15         | Rotate_Medium_Plus_White  | .. image:: /images/Rotate_Medium_Plus_White.png  |
   +-----------+---------------------------+--------------------------------------------------+
   |16         | Rotate_Small_Minus_White  | .. image:: /images/Rotate_Small_Minus_White.png  |
   +-----------+---------------------------+--------------------------------------------------+
   |17         | Rotate_Small_Plus_White   | .. image:: /images/Rotate_Small_Plus_White.png   |
   +-----------+---------------------------+--------------------------------------------------+

Files for these cursors can be found under ``XPPython3/cursors``. For your
convenience GIMP (\*.xcf)
files are included. If you want to change a cursor, please make a copy and place the
resulting PNG file somewhere else. These
images were extracted from X-Plane 12's ``Resources/bitmaps/interface.png``.
