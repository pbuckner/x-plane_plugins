XPWidgetUtils
=============

.. py:module::  XPWidgetUtils
.. py:currentmodule::  xp

To use::

  import xp

The XPWidgetUtils library contains useful functions that make writing and
using widgets less of a pain.

One set of functions are the widget behavior functions. These functions
each add specific useful behaviors to widgets. They can be used in two
manners:

1. You can add a widget behavior function to a widget as a callback proc
   using the :py:func:`addWidgetCallback` function. The widget will gain that behavior.
   Remember that the last function you add has highest priority. You can use
   this to change or augment the behavior of an existing finished widget.

2. You can call a widget function from inside your own widget function.
   This allows you to include useful behaviors in custom-built widgets. A
   number of the standard widgets get their behavior from this library. To do
   this, call the behavior function from your function first. If it returns 1,
   that means it handled the event and you don't need to; simply return 1.


Constants
---------

 .. py:data:: NO_PARENT
  :value: -1
 .. py:data:: PARAM_PARENT
  :value: -2            

Tuples
------

.. _XPCreateWidget_t:

XPCreateWidget_t
****************

Tuple used with :py:func:`createWidgets`. See also :py:func:`createWidget`::

  widgetDef = (left, top, right, bottom,     # widget size (ints)
               visible,                      # int 1= visible
               descriptor,                   # string
               isRoot,                       # int 1= is Root widget
               containerIndex,               # index for parent widget (see description)
               widgetClass                   # XPWidgetClass
              )

All parameters correspond to those of
:py:func:`createWidget` except for the *containerIndex*. If the container
index is equal to the index of a widget in the array, the widget
in the array passed to :py:func:`createWidgets` is used as the parent
of this widget. Note that if you pass an index greater than your
own position in the array, the parent you are requesting will not
exist yet. If the container index is :py:data:`NO_PARENT`, the parent widget
is specified as ``None``. If the container index is :py:data:`PARAM_PARENT`,
the widget passed into :py:func:`createWidgets` as *parentID* is used.

XPWidgetClass is from :py:mod:`XPStandardWidgets` or a new class you created.

For example, the following tuple consists of four widget definitions. The result
will be four widgets, two of which (#0 and #2) will be Main Windows, rooted to X-Plane,
a two caption widets: #1 is a child of Main Window 1, and #3 is a child of Main Window 2::

  widgetDefs = (
                (100, 500, 400, 300,
                 1,
                 "Main Window 1",
                 1,  # isRoot
                 xp.NO_PARENT,
                 xp.WidgetClass_MainWindow),
                (110, 480, 150, 460,
                 1,
                 "Caption in Main 1",
                 0,  # not isRoot
                 0,  # i.e., the parent of this widget is widget #0 from this tuple
                 xp.WidgetClass_Caption),
                (100, 500, 400, 300,
                 1,
                 "Main Window 2",
                 1,  # isRoot
                 xp.NO_PARENT,
                 xp.WidgetClass_MainWindow),
                (110, 480, 150, 460,
                 1,
                 "Caption in Main 2",
                 0,  # not isRoot
                 2,  # i.e., the parent of this widget is widget #2 from this tuple
                 xp.WidgetClass_Caption)
                )

:py:func:`createWidgets` would update the ``result`` parameter to return a list of four widget IDs 

Functions
---------

 
.. py:function:: moveWidgetBy(widgetID, deltaX=0, deltaY=0)

    Simply moves a widget by an amount, +x = right, +y=up, without resizing the
    widget.

    >>> widgetID = xp.createWidget(100, 200, 300, 100, 1, "My Widget", 1, 0, xp.WidgetClass_MainWindow)
    >>> xp.getWidgetGeometry(widgetID)
    (100, 200, 300, 100)
    >>> xp.moveWidgetBy(widgetID, 100, -50)
    >>> xp.getWidgetGeometry(widgetID)
    (200, 150, 400, 50)

    `Official SDK <https://developer.x-plane.com/sdk/XPWidgetUtils/#XPUMoveWidgetBy>`__ :index:`XPUMoveWidgetBy`

.. py:function:: createWidgets(widgetDefs, parentID=None)

 .. warning:: This function does not work. X-Plane 11.55 (at least) does not properly read
   the value in *widgetDefs*. Bug has been filed with Laminar 15-November-2021 and is XPD-11514.


 This function creates a series of widgets, returning a list of created widgetIDs.
 Pass in a list of widget creation tuples (:ref:`XPCreateWidget_t`) as *widgetDefs*.

 Widget parents are specified by index into the created widget table,
 allowing you to create nested widget structures. You can create multiple
 widget trees in one table. Generally you should create widget trees from
 the top down.

 You can also pass in a widget ID that will be used when the widget's parent
 is listed as :py:data:`PARAM_PARENT`; this allows you to embed widgets created with
 XPUCreateWidgets in a widget created previously.

 >>> widgetDefs = (
 ...               (100, 500, 400, 300,
 ...                1,
 ...                "Main Window 1",
 ...                1,
 ...                xp.NO_PARENT,
 ...                xp.WidgetClass_MainWindow),
 ...               (110, 480, 150, 460,
 ...                1,
 ...                "Caption in Main 1",
 ...                0,
 ...                0,  # i.e., the parent of this widget is widget #0 from this tuple
 ...                xp.WidgetClass_Caption),
 ...               (100, 250, 400, 50,
 ...                1,
 ...                "Main Window 2",
 ...                1,
 ...                xp.NO_PARENT,
 ...                xp.WidgetClass_MainWindow),
 ...               (110, 230, 150, 210,
 ...                1,
 ...                "Caption in Main 2",
 ...                0,
 ...                2,  # i.e., the parent of this widget is widget #2 from this tuple
 ...                xp.WidgetClass_Caption)
 ... )
 ...
 >>> widgets = xp.createWidgets(widgetDefs)
 >>> widgets

 `Official SDK <https://developer.x-plane.com/sdk/XPWidgetUtils/#XPUCreateWidgets>`__ :index:`XPUCreateWidgets`

Layout Managers
---------------

The layout managers are widget behavior functions for handling where widgets move. Layout
managers can be called from a widget function or attached to a widget later, using :py:func:`addWidgetCallback`

.. py:function:: fixedLayout(message, widgetID, param1, param2)

    This function causes the widget to maintain its children in fixed position
    relative to itself as it is resized. Use this on the top level 'window'
    widget for your window.

    >>> widgetID1 = xp.createWidget(400, 200, 600, 100, 1, "Widget 1", 1, 0, xp.WidgetClass_MainWindow)
    >>> subwidgetID1 = xp.createWidget(410, 180, 450, 165, 1, "Caption", 0, widgetID1, xp.WidgetClass_Caption)
    >>> xp.addWidgetCallback(widgetID2, xp.fixedLayout)

    .. note:: Honestly, I can't create a useful demonstration of this "Layout Manager" -- perhaps it is
              already encorporated into the existing MainWindow widget, as the positions of child widgets
              change / don't change identically with and without this callback.

    `Official SDK <https://developer.x-plane.com/sdk/XPWidgetUtils/#XPUFixedLayout>`__ :index:`XPUFixedLayout`              

Widget Procedure Behaviors
--------------------------

These widget behaviour functions add other useful behaviors to widgets. These functions cannot
be attached to a widget (e.g., :py:func:`addWidgetCallback`); they must be called from *your widget callback* function.

.. py:function:: selectIfNeeded(message, widgetID, param1, param2, eatClick) -> int:

    This causes the widget to bring its window to the foreground if it is not
    already. ``eatClick`` specifies whether clicks in the background should be
    consumed by bringing the window to the foreground.

    .. note:: This appears to already be implemented with MainWindow widgets: clicking on a window
              which is *not* the frontmost will cause the widget to move to the foreground.

              I've been unable to come up with a useful demonstration of this behavior.
              
    `Official SDK <https://developer.x-plane.com/sdk/XPWidgetUtils/#XPUSelectIfNeeded>`__ :index:`XPUSelectIfNeeded`

.. py:function:: defocusKeyboard(message, widgetID, param1, param2, eatClick=1)

    This causes a click in the widget to send keyboard focus back to X-Plane.
    This stops editing of any text fields, etc.

    .. note:: This appears to already be implemented with MainWindow widgets: clicking away from a TextField
              will cause the keyboard focus to be lost.

              I've been unable to come up with a useful demonstration of this behavior.
              
    `Official SDK <https://developer.x-plane.com/sdk/XPWidgetUtils/#XPUDefocusKeyboard>`__ :index:`XPUDefocusKeyboard`
    


.. py:function:: dragWidget(message, widgetID, param1, param2, left, top, right, bottom)

    :py:func:`dragWidget` drags the widget in response to mouse clicks. Pass in not
    only the event, but the global coordinates of the drag region, which might
    be a sub-region of your widget (for example, a title bar).

    For example, MainWindow title bars are *already* drag regions. To add a drag region
    also at the bottom of the main window, define the geometry for the bottom 20 pixels
    of the widget and call :py:func:`dragWidget` in you callback:
    
    >>> def MyCallback(message, widgetID, param1, param2):
    ...    geom = xp.getWidgetGeometry(widgetID)
    ...    geom[1] = geom[3] + 20
    ...    return xp.dragWidget(message, widgetID, param1, param2, *geom)
    ...
    >>> widgetID1 = xp.createWidget(400, 200, 600, 100, 1, "Widget 1", 1, 0, xp.WidgetClass_MainWindow)
    >>> subwidgetID1 = xp.createWidget(410, 180, 450, 165, 1, "Caption", 0, widgetID1, xp.WidgetClass_Caption)
    >>> xp.addWidgetCallback(widgetID1, MyCallback)

    `Official SDK <https://developer.x-plane.com/sdk/XPWidgetUtils/#XPUDragWidget>`__ :index:`XPUDragWidget`
  
