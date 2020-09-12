XPWidgetUtils
=============

.. py:module::  XPWidgetUtils

To use::

  import XPWidgetUtils

The XPWidgetUtils library contains useful functions that make writing and
using widgets less of a pain.

One set of functions are the widget behavior functions. These functions
each add specific useful behaviors to widgets. They can be used in two
manners:

1. You can add a widget behavior function to a widget as a callback proc
   using the :py:func:`XPWidgets.XPAddWidgetCallback` function. The widget will gain that behavior.
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
              PARAM_PARENT

Tuples
------

.. _XPCreateWidget_t:

XPCreateWidget_t
****************

Tuple used with :py:func:`XPUCreateWidgets`. See also :py:func:`XPWidgets.XPCreateWidget`::

  widgetDef = (left, top, right, bottom,     # widget size (ints)
               visible,                      # int 1= visible
               descriptor,                   # string
               isRoot,                       # int 1= is Root widget
               containerIndex,               # index for parent widget (see description)
               widgetClass                   # XPWidgetClass
              )

All parameters correspond to those of
:py:func:`XPWidgets.XPCreateWidget` except for the containerIndex. If the container
index is equal to the index of a widget in the array, the widget
in the array passed to :py:func:`XPUCreateWidgets` is used as the parent
of this widget. Note that if you pass an index greater than your
own position in the array, the parent you are requesting will not
exist yet. If the container index is :py:data:`NO_PARENT`, the parent widget
is specified as None. If the container index is :py:data:`PARAM_PARENT`,
the widget passed into XPUCreateWidgets as ``parentID`` is used.

XPWidgetClass is from :py:mod:`XPStandardWidgets` or a new class you created.

For example, the following tuple consists of four widget definitions. The result
will be four widgets, two of which (#0 and #2) will be Main Windows, rooted to X-Plane,
a two caption widets: #1 is a child of Main Window 1, and #3 is a child of Main Window 2::

  widgetDefs = (
                (100, 500, 400, 300,
                 1,
                 "Main Window 1",
                 1,
                 NO_PARENT,
                 xpWidgetClass_MainWindow),
                (110, 480, 150, 460,
                 1,
                 "Caption in Main 1"
                 0,  # i.e., the parent of this widget is widget #0 from this tuple
                 xpWidgetClass_Caption)
                (100, 500, 400, 300,
                 1,
                 "Main Window 2",
                 1,
                 NO_PARENT,
                 xpWidgetClass_MainWindow),
                (110, 480, 150, 460,
                 1,
                 "Caption in Main 2"
                 2,  # i.e., the parent of this widget is widget #2 from this tuple
                 xpWidgetClass_Caption)
                )

XPUCreateWidgets would update the ``result`` parameter to return a list of four widget IDs 

Functions
---------

.. py:function:: XPUCreateWidgets(widgetDefs, count, parentID, result) -> None

 :param widgetDefs: list of :ref:`XPCreateWidget_t` tuples
 :param count: int ignored (formerly, the # of widgets to be defined
 :param parentID: :ref:`XPWidgetID` of parent widget
 :result: list, which will contain list of created :ref:`XPWidgetID`\s                 

 This function creates a series of widgets.
 Pass in a list of widget creation tuples (:ref:`XPCreateWidget_t`) and
 an empty `result` list that will receive each widgetID.

 Widget parents are specified by index into the created widget table,
 allowing you to create nested widget structures. You can create multiple
 widget trees in one table. Generally you should create widget trees from
 the top down.

 You can also pass in a widget ID that will be used when the widget's parent
 is listed as :py:data:`PARAM_PARENT`; this allows you to embed widgets created with
 XPUCreateWidgets in a widget created previously.


.. py:function:: XPUMoveWidgetBy(widgetID, deltaX: int, deltaY:int) -> None:

    Simply moves a widget by an amount, +x = right, +y=up, without resizing the
    widget.



Layout Managers
---------------

The layout managers are widget behavior functions for handling where widgets move. Layout
managers can be called from a widget function or attached to a widget later.

.. py:function:: XPUFixedLayout(message, widgetID, param1, param2) -> int:

    This function causes the widget to maintain its children in fixed position
    relative to itself as it is resized. Use this on the top level 'window'
    widget for your window.


Widget Procedure Behaviors
--------------------------

These widget behaviour functions add other useful behaviors to widgets. These functions cannot
be attached to a widget; they must be called from your widget function.

.. py:function:: XPUSelectIfNeeded(message, widgetID, param1, param2, eatClick) -> int:

    This causes the widget to bring its window to the foreground if it is not
    already. ``eatClick`` specifies whether clicks in the background should be
    consumed by bringing the window to the foreground.


.. py:function:: XPUDefocusKeyboard(message, widgetID, param1, param2, eatClick) -> int:

    This causes a click in the widget to send keyboard focus back to X-Plane.
    This stops editing of any text fields, etc.


.. py:function:: XPUDragWidget(message, widgetID, param1, param2, left, top, right, bottom) -> int

    XPUDragWidget drags the widget in response to mouse clicks. Pass in not
    only the event, but the global coordinates of the drag region, which might
    be a sub-region of your widget (for example, a title bar).

  
