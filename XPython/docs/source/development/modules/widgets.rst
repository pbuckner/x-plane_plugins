XPWidgets
=========
.. py:module:: XPWidgets

To use::

  import XPWidgets

Widgets are persistent view 'objects' for X-Plane. A widget is an object
referenced by its opaque handle (:ref:`XPWidgetID`) and the APIs in this file. You
cannot access the widget's guts directly. Every Widget has the following
intrinsic data:

- A bounding box defined in global screen coordinates with 0,0 in the
  bottom left and +y = up, +x = right.

- A visible box, which is the intersection of the bounding box with the
  widget's parents visible box.

- Zero or one parent widgets. (Always zero if the widget is a root widget.)

- Zero or more child widgets.

- Whether the widget is a root. Root widgets are the top level plugin
  windows.

- Whether the widget is visible.

- A text string descriptor, whose meaning varies from widget to widget.

- An arbitrary set of properties defined by keys.
  This is how specific widgets store specific data.

- A list of widget callback procedures that implements the widgets behaviors.

The Widgets library sends messages to widgets to request specific behaviors
or notify the widget of things.

Widgets may have more than one callback function, in which case messages
are sent to the most recently added callback function until the message is
handled. Messages may also be sent to parents or children; see the
:ref:`XPDispatchMode` for the different widget message dispatching
functions. By adding a callback function to a window you can 'subclass' its
behavior.

A set of standard widgets are provided (See :py:mod:`XPStandardWidgets`) that serve common UI purposes. You
can also customize or implement entirely custom widgets.

Widgets are different than other view hierarchies (most notably Win32,
which they bear a striking resemblance to) in the following ways:

- Not all behavior can be patched. State that is managed by the XPWidgets
  DLL and not by individual widgets cannot be customized.

- All coordinates are in global screen coordinates. Coordinates are not
  relative to an enclosing widget, nor are they relative to a display window.


- Widget messages are always dispatched synchronously, and there is no
  concept of scheduling an update or a dirty region. Messages originate from
  X-Plane as the sim cycle goes by. Since X-Plane is constantly redrawing, so
  are widgets; there is no need to mark a part of a widget as 'needing
  redrawing' because redrawing happens frequently whether the widget needs it
  or not.

- Any widget may be a 'root' widget, causing it to be drawn; there is no
  relationship between widget class and rootness. Root widgets are implemented
  as :py:mod:`XPLMDisplay` windows.


.. py:function:: XPCreateWidget(left, top, right, bottom, visible, descriptor, isRoot, container, class) -> widgetID

    This function creates a new widget and returns the new widget's ID to you.
    If the widget creation fails for some reason, it returns None. Widget
    creation will fail either if you pass a bad class ID or if there is not
    adequate memory.

    :param int left:
    :param int top:
    :param int right:
    :param int bottom: position in global screen coordindates defining the widget's location on the screen
    :param int visible: 1= widget should be drawn; 0 to start as hidden      
    :param str descriptor: Widget's descriptor
    :param int isRoot: 1= this is going to be a root widget; 0 otherwise.                            
    :param container: :ref:`XPWidgetID` of non-root widget, or 0 for a root widget. If this widget is not going to start inside another
      widget, pass 0; this new widget will then just be floating off in space
      (and will not be drawn until it is placed in a widget.
    :param class: One of standard pre-defined widget classes, see :py:mod:`XPStandardWidgets`.
    :return: :ref:`XPWidgetID`

    A note on widget embedding: a widget is only called (and will be drawn,
    etc.) if it is placed within a widget that will be called. Root widgets are
    always called. So it is possible to have whole chains of widgets that are
    simply not called. You can preconstruct widget trees and then place them
    into root widgets later to activate them if you wish.


.. py:function:: XPCreateCustomWidget(left, top, right, bottom, visible, descriptor, isRoot, container, callback) -> widgetID

    This function is the same as :py:func:`XPCreateWidget` except that instead of passing
    a class ID, you pass your widget callback function pointer defining the
    widget. Use this function to define a custom widget. All parameters are the
    same as XPCreateWidget, except that the widget class has been replaced with
    the widget function (See :py:func:`XPWidgetDefs.XPWidgetFunc_t`).


.. py:function:: XPDestroyWidget(widgetID, destroyChildren: int) -> None:

    This class destroys a widget. Pass in the :ref:`XPWidgetID` of the widget to kill. If you
    pass 1 for ``destroyChildren``, the widget's children will be destroyed first,
    then this widget will be destroyed. (Furthermore, the widget's children
    will be destroyed with the flag set to 1, so the
    destruction will recurse down the widget tree.) If you pass 0 for this
    flag, the child widgets will simply end up with their parent set to 0.


.. py:function:: XPSendMessageToWidget(widgetID, message, dispatchMode, param1, parm2) -> handled

    :param widgetID: :ref:`XPWidgetID`
    :param int message: custom message or one from :py:mod:`XPStandardWidgets`
    :param dispatchMode: :ref:`XPDispatchMode`
    :param param1: message dependent
    :param param2: message dependent
    :return: int, 1= message was handled, 0 otherwise

    This sends any message to a widget. You should probably not go around
    simulating the predefined messages that the widgets library defines for
    you. You may however define custom messages for your widgets and send them
    with this method.

    For each widget that receives the message (see the dispatching modes), each
    widget function from the most recently installed to the oldest one receives
    the message in order until it is handled.


.. py:function:: XPPlaceWidgetWithin(subWidgetID, container) -> None:

    :param subWidgetID: :ref:`XPWidgetID` of widget to be moved.
    :param container: :ref:`XPWidgetID` of new parent, or 0 to remove from current parent

    This function changes which container a widget resides in. You may NOT use
    this function on a root widget!
    The moved widget will become the last/closest widget in the container.
    Any call to this other than
    passing the widget ID of the old parent of the affected widget will cause
    the widget to be removed from its old parent. Placing a widget within its
    own parent simply makes it the last widget.

    .. note:: This routine does not reposition the sub widget in global
      coordinates. If the container has layout management code, it will
      reposition the subwidget for you, otherwise you must do it with
      :py:func:`XPSetWidgetGeometry`.


.. py:function:: XPCountChildWidgets(widgetID) -> count:

    This routine returns the number of widgets another widget contains.


.. py:function:: XPGetNthChildWidget(widgetID, index) -> widgetID:

    This routine returns the widget ID of a child widget by index. Indexes are
    0 based. If the index is invalid, 0 is returned.


.. py:function:: XPGetParentWidget(widgetID) -> widgetID:

    This routine returns the parent of a widget, or None if the widget has no
    parent. Root widgets never have parents and therefore always return None.


.. py:function:: XPShowWidget(widgetID) -> None:

    This routine makes a widget visible if it is not already. Note that if a
    widget is not in a rooted widget hierarchy or one of its parents is not
    visible, it will still not be visible to the user.

.. py:function:: XPHideWidget(widgetID) -> None:

    Makes a widget invisible. See :py:func:`XPShowWidget` for considerations of when a
    widget might not be visible despite its own visibility state.


.. py:function:: XPIsWidgetVisible(widgetID) -> isVisible:

    This returns 1 if a widget is visible, 0 if it is not. Note that this
    routine takes into consideration whether a parent is invisible. Use this
    routine to tell if the user can see the widget.


.. py:function:: XPFindRootWidget(widgetID) -> widgetID:

    XPFindRootWidget returns the :ref:`XPWidgetID` of the root widget that contains the
    passed in widget or None if the passed in widget is not in a rooted
    hierarchy.


.. py:function:: XPBringRootWidgetToFront(widgetID) -> None:

    This routine makes the specified widget be in the front most widget
    hierarchy. If this widget is a root widget, its widget hierarchy comes to
    front, otherwise the widget's root is brought to the front. If this widget
    is not in an active widget hiearchy (e.g. there is no root widget at the
    top of the tree), this routine does nothing.


.. py:function:: XPIsWidgetInFront(widgetID) -> isFront:

    This routine returns 1 if this widget's hierarchy is the front most
    hierarchy. It returns 0 if the widget's hierarchy is not in front, or
    if the widget is not in a rooted hierarchy.


.. py:function:: XPGetWidgetGeometry(widgetID) -> (left, top, right, bottom):

    This routine returns the bounding box of a widget in global coordinates (list of ints).

.. py:function:: XPSetWidgetGeometry(widgetID, left: int, top: int, right: int, bottom: int) -> None:

    This function changes the bounding box of a widget.


.. py:function::  XPGetWidgetForLocation(container, xOffset, yOffset, recursive, visibleOnly) -> widgetID

    Given a container :ref:`XPWidgetID` and a location, this routine returns the :ref:`XPWidgetID` of the
    child of that container widget that owns that location. If ``recursive`` is true then
    this will return a child of a child of a widget as it tries to find the
    deepest widget at that location. If ``visibleOnly`` is true, then only
    visible widgets are considered, otherwise all widgets are considered. The
    widget ID passed for ``container`` will be returned if the location is in
    that widget but not in a child widget. 0 is returned if the location is not
    in the container.

    .. note:: If a widget's geometry extends outside its parents geometry, it will
      **not** be returned by this call for mouse locations outside the parent
      geometry. The parent geometry limits the child's eligibility for mouse
      location.


.. py:function:: XPGetWidgetExposedGeometry(widgetID) -> (left, top, right, bottom):

    This routine returns the bounds of the area of a widget that is completely
    within its parent widgets. Since a widget's bounding box can be outside its
    parent, part of its area will not be elligible for mouse clicks and should
    not draw. Use :py:func:`XPGetWidgetGeometry` to find out what area defines your
    widget's shape, but use this routine to find out what area to actually draw
    into. Note that the widget library does not use OpenGL clipping to keep
    frame rates up, although you could use it internally.


.. py:function:: XPSetWidgetDescriptor(widgetID, descriptor:str) -> None:

    Every widget has a descriptor, which is a text string. What the text string
    is used for varies from widget to widget; for example, a push button's text
    is its descriptor, a caption shows its descriptor, and a text field's
    descriptor is the text being edited. In other words, the usage for the text
    varies from widget to widget, but this API provides a universal and
    convenient way to get at it. While not all UI widgets need their
    descriptor, many do.


.. py:function:: XPGetWidgetDescriptor(widgetID) -> str:

    This routine returns the widget's descriptor. If the length of the
    descriptor exceeds the interal buffer an error will be printed to
    Log.txt


.. py:function:: XPGetWidgetUnderlyingWindow(widgetID) -> windowID:

    Returns the :ref:`XPLMWindowID` (from the :py:mod:`XPLMDisplay` API) that backs your widget
    window. You can use the
    returned window ID for display APIs like :py:func:`XPLMDisplay.XPLMSetWindowPositioningMode`,
    allowing you to pop the widget window out into a real OS window, or move it
    into VR.


.. py:function:: XPSetWidgetProperty(widgetID, propertyID, value: object) -> None:

    This function sets a widget's property. Properties are arbitrary values
    associated by a widget by ID.


.. py:function:: XPGetWidgetProperty(widgetID, propertyID, exists) -> value:

    This routine returns the value of a widget's property, or 0 if the property
    is not defined. If you need to know whether the property is defined, pass a
    list for inExists; the existence of that property will be
    returned in the list. Pass None for inExists if you do not need this
    information. (This allows you to distinguish between ``value = 0`` property does not exist
    and ``value = 0`` property exists.)


.. py:function:: XPSetKeyboardFocus(widgetID) -> widgetID:

    XPSetKeyboardFocus controls which widget will receive keystrokes. Pass the
    Widget ID of the widget to get the keys. Note that if the widget does not
    care about keystrokes, they will go to the parent widget, and if no widget
    cares about them, they go to X-Plane.

    If you set the keyboard focus to Widget ID 0, X-Plane gets keyboard focus.

    This routine returns the widget ID that ended up with keyboard focus, or 0
    for X-Plane.

    Keyboard focus is not changed if the new widget will not accept it. For
    setting to X-Plane, keyboard focus is always accepted.


.. py:function:: XPLoseKeyboardFocus(widgetID) -> None:

    This causes the specified widget to lose focus; focus is passed to its
    parent, or the next parent that will accept it. This routine does nothing
    if this widget does not have focus.


.. py:function:: XPGetWidgetWithFocus() -> widgetID:

    This routine returns the widget that has keyboard focus, or 0 if X-Plane
    has keyboard focus or some other plugin window that does not have widgets
    has focus.


.. py:function::  XPAddWidgetCallback(widgetID, callback):

    This function adds a new widget callback (see :py:func:`XPWidgetDefs.XPWidgetFunc_t`)
    to a widget. This widget callback
    supercedes any existing ones and will receive messages first; if it does
    not handle messages they will go on to be handled by pre-existing widgets.

    The widget function will remain on the widget for the life of the widget.
    The creation message will be sent to the new callback immediately with the
    widget ID, and the destruction message will be sent before the other widget
    function receives a destruction message.

    This provides a way to 'subclass' an existing widget. By providing a second
    hook that only handles certain widget messages, you can customize or extend
    widget behavior.



.. py:function::  XPGetWidgetClassFunc(inWidgetClass) -> function:

    Given a widget class, this function returns the callbacks that power that
    widget class.


Constants
---------

.. _XPWidgetID:

XPWidgetID
**********

Unique handle to a widget


