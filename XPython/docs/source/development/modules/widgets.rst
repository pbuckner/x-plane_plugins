XPWidgets
=========
.. py:module:: XPWidgets
.. py:currentmodule:: xp

To use::

  import xp

Widgets are persistent view 'objects' for X-Plane. A widget is an object
referenced by its opaque handle (:ref:`XPWidgetID`).. You
cannot access the widget's guts directly. Every Widget has the following
intrinsic data:

- A bounding box defined in global screen coordinates with 0,0 in the
  bottom left and +y = up, +x = right.

  * :py:func:`getWidgetGeometry`, :py:func:`setWidgetGeometry`

- A visible box, which is the intersection of the bounding box with the
  widget's parents visible box.

- Zero or one parent widgets. (Always zero if the widget is a root widget.)

  * :py:func:`getParentWidget`

- Zero or more child widgets.

  * :py:func:`countChildWidgets`, :py:func:`getNthChildWidget`

- Whether the widget is a root. Root widgets are the top level plugin
  windows.

  * :py:func:`findRootWidget`, :py:func:`bringRootWidgetToFront`

- Whether the widget is visible.

  * :py:func:`isWidgetVisible`, :py:func:`showWidget`, :py:func:`hideWidget`

- A text string descriptor, whose meaning varies from widget to widget.

  * :py:func:`setWidgetDescriptor`, :py:func:`getWidgetDescriptor`

- An arbitrary set of properties defined by keys.
  This is how specific widgets store specific data.

  * :py:func:`setWidgetProperty`, :py:func:`getWidgetProperty`

- A list of widget callback procedures that implements the widgets behaviors.

  * :py:func:`addWidgetCallback`

The Widgets library sends messages to widgets to request specific behaviors
or notify the widget of things. You can send messages to widgets using :py:func:`sendMessageToWidget`.

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

- All coordinates are in global screen coordinates. Coordinates are **not
  relative to an enclosing widget**, nor are they relative to a display window.

- Widget messages are always dispatched synchronously, and there is no
  concept of scheduling an update or a dirty region. Messages originate from
  X-Plane as the sim cycle goes by. Since X-Plane is constantly redrawing, so
  are widgets; there is no need to mark a part of a widget as 'needing
  redrawing' because redrawing happens frequently whether the widget needs it
  or not.

- Any widget may be a 'root' widget, causing it to be drawn; there is no
  relationship between widget class and "rootness". Root widgets are implemented
  as :py:mod:`XPLMDisplay` windows.


.. py:function:: createWidget(left, top, right, bottom, visible, descriptor, isRoot, container, class)

    This function creates a new widget and returns the new widget's ``widgetID`` to you.
    If the widget creation fails for some reason, it returns None. Widget
    creation will fail either if you pass a bad *class* ID or if there is not
    adequate memory.

    *left*, *top*, *right*, *bottom* are global screen coordinates (**not relative to container widget**).
    (See :py:func:`getScreenBoundsGlobal`)

    *visible* = 1 indicates widget should be drawn, otherwise widget is hidden. Change it later
    using :py:func:`showWidget`.
    
    *descriptor* is the widget's initial descriptor, changeable by :py:func:`setWidgetDescriptor`. Note
    that "descriptor" meaning/usage depends on the widget class.

    *isRoot* = 1, if this widget is to be a root widget, 0 otherwise. When creating root widgets, *container* is ignored.
    If not a root widget, *container* should point to the containing widgetID. (You can have
    an non-root widget without specifying a *container*. In this case, the created widget, and it's children,
    will not be displayed until placed within a root widget.
    
    *class* is one of the standard pre-defined widget classes, see :py:mod:`XPStandardWidgets`.

    A note on widget embedding: a widget is only called (and will be drawn,
    etc.) if it is placed within a widget that will be called. Root widgets are
    always called. So it is possible to have whole chains of widgets that are
    simply not called. You can pre-construct widget trees and then place them
    into root widgets later to activate them if you wish.

    >>> xp.createWidget(100, 200, 300, 100, 1, "My Widget", 1, 0, xp.WidgetClass_MainWindow)
    <capsule object "XPLMWidgetID" at 0x7f98abdd6803>

    .. image:: /images/widget_mainwindow.png

    `Official SDK <https://developer.x-plane.com/sdk/XPWidgets/#XPCreateWidget>`__ :index:`XPCreateWidget`


.. py:function:: createCustomWidget(left, top, right, bottom, visible, descriptor, isRoot, container, callback)

    This function is the same as :py:func:`createWidget` except that instead of passing
    a predefined widget *class*, you pass your widget *callback* function pointer defining the
    widget. Use this function to define a custom widget. All other parameters are the
    same as :py:func:`createWidget`, except that the widget *class* has been replaced with
    the widget *callback*:

    .. py:function:: widgetCallback(message, widget, param1, param2)
    
      :param message: :ref:`XPWidgetMessage` (may be custom)
      :param widgetID: Your WidgetID
      :param param1:
      :param param2: param1 and param2 are dependent on the particular message sent   
      :return: 1= you have handled the message, 0 otherwise.
    
      This function defines your custom widget's behavior. It will be called by
      the widgets library to send messages to your widget. The *message* and *widget*
      ID are passed in, as well as two pointer-width signed parameters whose meaning
      varies with the message.
    
      Return 1 to indicate that you have processed the
      message, 0 to indicate that you have not. For any message that is not
      understood, return 0.
    
    `Official SDK <https://developer.x-plane.com/sdk/XPWidgets/#XPCreateCustomWidget>`__ :index:`XPCreateCustomWidget`

.. py:function:: destroyWidget(widgetID, destroyChildren=1)

    This class destroys a widget. Pass in the :ref:`XPWidgetID` of the widget to kill. If you
    pass 1 for *destroyChildren*, the widget's children will be destroyed first,
    then this widget will be destroyed. (Furthermore, the widget's children
    will be destroyed with the flag set to 1, so the
    destruction will recurse down the widget tree.) If you pass 0 for this
    flag, the child widgets will simply end up with their parent set to 0.

    >>> xp.destroyWidget(widgetID)

    `Official SDK <https://developer.x-plane.com/sdk/XPWidgets/#XPDestroyWidget>`__ :index:`XPDestroyWidget`

.. py:function:: sendMessageToWidget(widgetID, message, dispatchMode=xp.Mode_UpChain, param1=0, param2=0)

    This sends any *message* to a widget given by *widgetID*. You should probably not go around
    simulating the predefined messages that the widgets library defines for
    you. You may however define custom messages for your widgets and send them
    with this method.

    For each widget that receives the message (see the dispatching modes), each
    widget function from the most recently installed to the oldest one receives
    the message in order until it is handled.

    Dispatching modes are described in :ref:`XPDispatchMode`. By default, the message goes
    to the widget, and is passed up the chain of parents until it has been handled.

    Params are message dependent, (see :ref:`XPWidgetMessage` for standard messages) and
    default to 0.

    >>> def MyCallback(message, widgetID, param1, param2):
    ...     if message == xp.Message_CloseButtonPushed:
    ...         xp.destroyWidget(widgetID)
    ...         return 1
    ...     return 0
    ...
    >>> widgetID = xp.createWidget(100, 200, 300, 100, 1, "My Widget", 1, 0, xp.WidgetClass_MainWindow)
    >>> xp.addWidgetCallback(widgetID, MyCallback)
    >>> xp.setWidgetProperty(widgetID, xp.Property_MainWindowHasCloseBoxes, 1)
    >>> # Now, send message "CloseButtonPushed" message to widget
    >>> xp.sendMessageToWidget(widgetID, xp.Message_CloseButtonPushed)
    1

    :py:func:`sendMessageToWidget` returns 1 if the message was handled, 0 otherwise.

    `Official SDK <https://developer.x-plane.com/sdk/XPWidgets/#XPSendMessageToWidget>`__ :index:`XPSendMessageToWidget`

.. py:function:: placeWidgetWithin(widgetID, container=0)

    Change the *container* widget of the provided *widgetID*. *widgetID* **must not**
    be a root widget. *container*\=0 removes the widget from it's containing parent (but does
    **not** make it a root widget.)

    The moved widget will become the last/closest widget in the container.
    Any call to :py:func:`placeWidgetWithin` with *container* not the same
    as the widget's existing containing widget will cause
    the widget to be removed from its old containing parent. Placing a widget within its
    own parent simply makes it the last widget.

    .. note:: This routine does not reposition the widget in global
      coordinates. If the container has layout management code, it will
      reposition the widget for you, otherwise you must do it with
      :py:func:`setWidgetGeometry`.

      This means when you place a widget within it's container, the widget may or
      may not be visible, due to clipping: the widget's geometry, may be "outside" of
      the containing widget's geometry.

    >>> widgetID = xp.createWidget(100, 200, 300, 100, 1, "My Widget", 1, 0, xp.WidgetClass_MainWindow)
    >>> subwidgetID = xp.createWidget(110, 190, 290, 110, 1, "Caption", 0, widgetID, xp.WidgetClass_Caption)
    >>> # Remove subwidget from parent (which will cause it to no longer be displayed)
    >>> xp.placeWidgetWithin(subwidgetID, 0)
    >>> # and... replace it, making it visible again
    >>> xp.placeWidgetWithin(subwidgetID, widgetID)
    
    `Official SDK <https://developer.x-plane.com/sdk/XPWidgets/#XPPlaceWidgetWithin>`__ :index:`XPPlaceWidgetWithin`

.. py:function:: countChildWidgets(widgetID)

    Return the number of widgets another widget contains.

    >>> widgetID = xp.createWidget(100, 200, 300, 100, 1, "My Widget", 1, 0, xp.WidgetClass_MainWindow)
    >>> xp.countChildWidgets(widgetID)
    0
    >>> subwidgetID = xp.createWidget(110, 190, 290, 110, 1, "Caption", 0, widgetID, xp.WidgetClass_Caption)
    >>> xp.countChildWidgets(widgetID)
    1

    `Official SDK <https://developer.x-plane.com/sdk/XPWidgets/#XPCountChildWidgets>`__ :index:`XPCountChildWidgets`

.. py:function:: getNthChildWidget(widgetID, index)

    Return the ``widgetID`` of a child widget by *index*. Indexes are
    0 based. If the index is invalid, 0 is returned.

    >>> widgetID = xp.createWidget(100, 200, 300, 100, 1, "My Widget", 1, 0, xp.WidgetClass_MainWindow)
    >>> subwidgetID = xp.createWidget(110, 190, 290, 110, 1, "Caption", 0, widgetID, xp.WidgetClass_Caption)
    >>> xp.countChildWidgets(widgetID)
    1
    >>> subwidgetID == xp.getNthChildWidget(widgetID, 0)
    True

    `Official SDK <https://developer.x-plane.com/sdk/XPWidgets/#XPGetNthChildWidget>`__ :index:`XPGetNthChildWidget`

.. py:function:: getParentWidget(widgetID)

    Return the ``widgetID`` of the parent (containing) widget of this *widgetID*, or None if the widget has no
    parent. Root widgets never have parents and therefore always return None.

    >>> widgetID = xp.createWidget(100, 200, 300, 100, 1, "My Widget", 1, 0, xp.WidgetClass_MainWindow)
    >>> subwidgetID = xp.createWidget(110, 190, 290, 110, 1, "Caption", 0, widgetID, xp.WidgetClass_Caption)
    >>> widgetID == xp.getParentWidget(subwidgetID)
    True

    `Official SDK <https://developer.x-plane.com/sdk/XPWidgets/#XPGetParentWidget>`__ :index:`XPGetParentWidget`

.. py:function:: showWidget(widgetID)

    Makes a widget visible if it is not already. Note that if a
    widget is not in a rooted widget hierarchy or one of its parents is not
    visible, it will still not be visible to the user. Also, if the child widget is
    outside of the parent widget's geometry, it may be clipped and not visible.

    `Official SDK <https://developer.x-plane.com/sdk/XPWidgets/#XPShowWidget>`__ :index:`XPShowWidget`

.. py:function:: hideWidget(widgetID)

    Makes a widget invisible. See :py:func:`showWidget` for considerations of when a
    widget might not be visible despite its own visibility state.

    >>> widgetID = xp.createWidget(100, 200, 300, 100, 1, "My Widget", 1, 0, xp.WidgetClass_MainWindow)
    >>> subwidgetID = xp.createWidget(110, 190, 290, 110, 1, "Caption", 0, widgetID, xp.WidgetClass_Caption)
    >>> xp.hideWidget(subwidgetID)
    >>> xp.showWidget(subwidgetID)

    `Official SDK <https://developer.x-plane.com/sdk/XPWidgets/#XPHideWidget>`__ :index:`XPHideWidget`

.. py:function:: isWidgetVisible(widgetID)

    This returns 1 if a widget is visible, 0 if it is not. Note that this
    routine takes into consideration whether a parent is invisible. Use this
    routine to tell if the user can see the widget.

    Except, if the widget is outside of it's parent's geometry, it may be subject to clipping.
    :py:func:`isWidgetVisible` will indicate the widget is visible, but the user won't see it.

    >>> widgetID = xp.createWidget(100, 200, 300, 100, 1, "My Widget", 1, 0, xp.WidgetClass_MainWindow)
    >>> subwidgetID = xp.createWidget(110, 190, 290, 110, 1, "Caption", 0, widgetID, xp.WidgetClass_Caption)
    >>> xp.isWidgetVisible(subwidgetID)
    1
    >>> xp.placeWidgetWithin(subwidgetID, 0)
    >>> xp.isWidgetVisible(subwidgetID)
    0
    

    `Official SDK <https://developer.x-plane.com/sdk/XPWidgets/#XPIsWidgetVisible>`__ :index:`XPIsWidgetVisible`

.. py:function:: findRootWidget(widgetID) -> widgetID:

    Return the :ref:`XPWidgetID` of the root widget that contains the
    passed in *widgetID* or None if the passed in widget is not in a rooted
    hierarchy.

    >>> widgetID = xp.createWidget(100, 200, 300, 100, 1, "My Widget", 1, 0, xp.WidgetClass_MainWindow)
    >>> widgetID == xp.findRootWidget(widgetID)
    True
    >>> subwidgetID = xp.createWidget(110, 190, 290, 110, 1, "Caption", 0, widgetID, xp.WidgetClass_Caption)
    >>> widgetID == xp.findRootWidget(subwidgetID)
    True
    

    `Official SDK <https://developer.x-plane.com/sdk/XPWidgets/#XPFindRootWidget>`__ :index:`XPFindRootWidget`

.. py:function:: bringRootWidgetToFront(widgetID)

    Make the specified widget be in the front most widget
    hierarchy. If this widget is a root widget, its widget hierarchy comes to
    front, otherwise the *widget's root* is brought to the front. If this widget
    is not in an active widget hierarchy (e.g. there is no root widget at the
    top of the tree), this routine does nothing.


    `Official SDK <https://developer.x-plane.com/sdk/XPWidgets/#XPBringRootWidgetToFront>`__ :index:`XPBringRootWidgetToFront`

.. py:function:: isWidgetInFront(widgetID)

    Return 1 if this widget's hierarchy is the front most
    hierarchy. It returns 0 if the widget's hierarchy is not in front, or
    if the widget is not in a rooted hierarchy.

    Widgets which belong to the same root will have the same return value.

    `Official SDK <https://developer.x-plane.com/sdk/XPWidgets/#XPIsWidgetInFront>`__ :index:`XPIsWidgetInFront`

.. py:function:: getWidgetGeometry(widgetID)

    Returns the bounding box of a widget in global coordinates (left, top, right, bottom).

    >>> widgetID = xp.createWidget(100, 200, 300, 100, 1, "My Widget", 1, 0, xp.WidgetClass_MainWindow)
    >>> xp.getWidgetGeometry(widgetID)
    (100, 200, 300, 100)
    >>> # if you were to move the widget on-screen...
    >>> xp.getWidgetGeometry(widgetID)
    (347, 440, 527, 360)
    
    `Official SDK <https://developer.x-plane.com/sdk/XPWidgets/#XPGetWidgetGeometry>`__ :index:`XPGetWidgetGeometry`

    
.. py:function:: setWidgetGeometry(widgetID, left, top, right, bottom)

    This function changes the bounding box of a widget, in global coordinates.

    >>> widgetID = xp.createWidget(100, 200, 300, 100, 1, "My Widget", 1, 0, xp.WidgetClass_MainWindow)
    >>> position = list(xp.getWidgetGeometry(widgetID))
    >>> # shift window to the right...
    >>> position[0] += 10
    >>> position[2] += 10
    >>> xp.setWidgetGeometry(widgetID, *pos)

    `Official SDK <https://developer.x-plane.com/sdk/XPWidgets/#XPSetWidgetGeometry>`__ :index:`XPSetWidgetGeometry`

.. py:function::  getWidgetForLocation(container, xOffset, yOffset, recursive=1, visibleOnly=1)

    Given a *container* :ref:`XPWidgetID` and a location,
    this routine returns the :ref:`XPWidgetID` of the
    child of that container widget that owns that location. If *recursive* is 1 then
    this will return a child of a child of a widget as it tries to find the
    deepest widget at that location. If *visibleOnly* is 1, then only
    visible widgets are considered, otherwise all widgets are considered. The
    widget ID passed for *container* will be returned if the location is in
    that widget but not in a child widget. 0 is returned if the location is not
    in the container.

    *xOffset* and *yOffset* are global coordinates, **not relative the container widget**.

    >>> widgetID = xp.createWidget(100, 200, 300, 100, 1, "My Widget", 1, 0, xp.WidgetClass_MainWindow)
    >>> subwidgetID = xp.createWidget(110, 190, 290, 110, 1, "Caption", 0, widgetID, xp.WidgetClass_Caption)
    >>> subwidgetID == xp.getWidgetForLocation(widgetID, 115, 185)
    True
  
    .. note:: If a widget's geometry extends outside its parents geometry, it will
      **not** be returned by this call for mouse locations outside the parent
      geometry. The parent geometry limits the child's eligibility for mouse
      location.


    `Official SDK <https://developer.x-plane.com/sdk/XPWidgets/#XPGetWidgetForLocation>`__ :index:`XPGetWidgetForLocation`

.. py:function:: getWidgetExposedGeometry(widgetID)

    Returns the bounds of the area of a widget that is completely
    within its parent widgets. Since a widget's bounding box can be outside its
    parent, part of its area will not be eligible for mouse clicks and should
    not draw. Use :py:func:`getWidgetGeometry` to find out what area defines your
    widget's shape, but use this routine to find out what area to actually draw
    into. Note that the widget library does not use OpenGL clipping to keep
    frame rates up, although you could use it internally.

    >>> widgetID = xp.createWidget(100, 200, 300, 100, 1, "My Widget", 1, 0, xp.WidgetClass_MainWindow)
    >>> subwidgetID = xp.createWidget(110, 190, 350, 110, 1, "WIDE Caption", 0, widgetID, xp.WidgetClass_Caption)
    >>> xp.getWidgetGeometry(subwidgetID)
    (110, 190, 350, 110)
    >>> xp.getWidgetExposedGeometry(subwidgetID)
    (110, 190, 300, 110)

    `Official SDK <https://developer.x-plane.com/sdk/XPWidgets/#XPGetWidgetExposedGeometry>`__ :index:`XPGetWidgetExposedGeometry`

.. py:function:: setWidgetDescriptor(widgetID, descriptor)

    Every widget has a text string descriptor. What the text string
    is used for varies from widget to widget; for example, a push button's text
    is its descriptor, a caption shows its descriptor, and a text field's
    descriptor is the text being edited. In other words, the usage for the text
    varies from widget to widget, but this API provides a universal and
    convenient way to get at it. While not all UI widgets need their
    descriptor, many do. See :py:mod:`XPStandardWidgets`.

    >>> widgetID = xp.createWidget(100, 200, 300, 100, 1, "My Widget", 1, 0, xp.WidgetClass_MainWindow)
    >>> xp.setWidgetDescriptor(widgetID, "Updated Title")

    `Official SDK <https://developer.x-plane.com/sdk/XPWidgets/#XPSetWidgetDescriptor>`__ :index:`XPSetWidgetDescriptor`

.. py:function:: getWidgetDescriptor(widgetID)

    Return the widget's descriptor. If the length of the
    descriptor exceeds the internal buffer an error will be printed to
    Log.txt.

    .. Note:: The X-Plane C SDK function has three parameters and returns
              the length of the descriptor. This is overkill for python.
              Instead, provided with only the *widgetID*, we'll return the
              string descriptor.

    >>> widgetID = xp.createWidget(100, 200, 300, 100, 1, "My Widget", 1, 0, xp.WidgetClass_MainWindow)
    >>> xp.getWidgetDescriptor(widgetID)
    'My Widget'
              
    `Official SDK <https://developer.x-plane.com/sdk/XPWidgets/#XPGetWidgetDescriptor>`__ :index:`XPGetWidgetDescriptor`

.. py:function:: getWidgetUnderlyingWindow(widgetID)

    Returns the ``windowID`` (from the :py:mod:`XPLMDisplay` API) that backs your widget
    window. You can use the
    returned window ID for display APIs like :py:func:`setWindowPositioningMode`,
    allowing you to pop the widget window out into a real OS window, or move it
    into VR.

    >>> windowID = xp.getWidgetUnderlyingWindow(widgetID)
    >>> xp.setWindowPositioningMode(windowID, xp.WindowPositionPopOut, -1)

    `Official SDK <https://developer.x-plane.com/sdk/XPWidgets/#XPGetWidgetUnderlyingWindow>`__ :index:`XPGetWidgetUnderlyingWindow`
    
.. py:function:: setWidgetProperty(widgetID, propertyID, value=0)

    This function sets a widget's property. Properties are arbitrary values
    associated by a widget by ID. See :py:mod:`XPStandardWidgets`.

    Note that if the *propertyID* is less than :py:data:Property_UserStart, we
    assume the provide *value* is an integer, and call the X-Plane API
    to update the widget with new property value.

    Otherwise, we'll *implement* the widget property on behalf of the widget, storing
    *value* as a python object.

    >>> widgetID = xp.createWidget(100, 200, 300, 100, 1, "My Widget", 1, 0, xp.WidgetClass_MainWindow)
    >>> xp.getWidgetProperty(widgetID, xp.Property_MainWindowHasCloseBoxes, exists=None)
    0
    >>> xp.setWidgetProperty(widgetID, xp.Property_MainWindowHasCloseBoxes, 1)
    >>> xp.getWidgetProperty(widgetID, xp.Property_MainWindowHasCloseBoxes)
    1
    >>> xp.setWidgetProperty(widgetID, 999999, {'data': {'value': {'name': 'John']}})
    >>> xp.getWidgetProperty(widgetID, 999999)
    {'data': {'value': {'name': 'John']}}

    `Official SDK <https://developer.x-plane.com/sdk/XPWidgets/#XPSetWidgetProperty>`__ :index:`XPSetWidgetProperty`


.. py:function:: getWidgetProperty(widgetID, propertyID, exists=-1)

    Return the value of widget's property, or 0 (or ValueError) if the property is not defined (See *exists*, below).

    For *exists*:
    
    * Pass python list object: if property exists on widget, ``exists = [1,]`` otherwise ``exists = [0,]``
    * Pass ``None`` to indicate you don't care if property exists.
    * Do not provide parameter and we'll raise ``ValueError`` exception if property does not exist.

    If you need to know whether the property is defined, pass a
    list for *exists*; the existence of that property will be
    returned in the list. Pass None for *exists* if you do not need this
    information, and don't want to raise an exception.
    (This allows you to distinguish between ``value = 0`` property does not exist
    and ``value = 0`` property exists.)

    If the final parameter is not passed and the property does not exist, a ``ValueError`` exception
    is raised.

    Note that widgets may "understand" a property, but not have the property defined yet. For example,
    ``MainWindow`` widgets use the ``Property_MainWindowHasCloseBoxes`` to cause the window to
    display close boxes. However, if you have not set this property, *the property is undefined* rather than
    set to 0:

    >>> widgetID = xp.createWidget(100, 200, 300, 100, 1, "My Widget", 1, 0, xp.WidgetClass_MainWindow)
    >>> xp.getWidgetProperty(widgetID, xp.Property_MainWindowHasCloseBoxes)
    ValueError: Widget does not have this value
    >>> xp.getWidgetProperty(widgetID, xp.Property_MainWindowHasCloseBoxes)
    0
    >>> exists = []
    >>> xp.getWidgetProperty(widgetID, xp.Property_MainWindowHasCloseBoxes, exists)
    0
    >>> exists
    [0]

    `Official SDK <https://developer.x-plane.com/sdk/XPWidgets/#XPGetWidgetProperty>`__ :index:`XPGetWidgetProperty`

.. py:function:: setKeyboardFocus(widgetID)

    Controls which widget will receive keystrokes. Pass the
    Widget ID of the widget to get the keys. Note that if the widget does not
    care about keystrokes, they will go to the parent widget, and if no widget
    cares about them, they go to X-Plane.

    If you set the keyboard focus to widget ``0``, X-Plane gets keyboard focus.

    This routine returns the widget ID that ended up with keyboard focus, or 0
    for X-Plane.

    Keyboard focus is not changed if the new widget will not accept it. For
    setting to X-Plane, keyboard focus is always accepted.

    >>> widgetID = xp.createWidget(100, 200, 300, 100, 1, "My Widget", 1, 0, xp.WidgetClass_MainWindow)
    >>> xp.setKeyboardFocus(widgetID)  # MainWindow widget cannot accept focus
    0
    >>> subwidgetID = xp.createWidget(110, 180, 290, 165, 1, "Edit me", 0, widgetID, xp.WidgetClass_TextField)
    >>> xp.setKeyboardFocus(subwidgetID)  # Text Edit widget can accept focus
    <capsule object "XPLMWidgetID" at 0x7fdc99871276>

    `Official SDK <https://developer.x-plane.com/sdk/XPWidgets/#XPSetKeyboardFocus>`__ :index:`XPSetKeyboardFocus`

.. py:function:: loseKeyboardFocus(widgetID)

    This causes the specified widget to lose focus; focus is passed to its
    parent, or the next parent that will accept it. This routine does nothing
    if this widget does not have focus.

    `Official SDK <https://developer.x-plane.com/sdk/XPWidgets/#XPLoseKeyboardFocus>`__ :index:`XPLoseKeyboardFocus`

.. py:function:: getWidgetWithFocus()

    This routine returns the widget that has keyboard focus, or 0 if X-Plane
    has keyboard focus or some other plugin window that does not have widgets
    has focus.

    >>> xp.getWidgetWithFocus()
    <capsule object "XPLMWidgetID" at <0x7fdd765d34cd>

    (Note that if you type this into the Debugger, it will *always* report a debugger
    widget because, well, you're typing into the widget which currently has focus!)

    `Official SDK <https://developer.x-plane.com/sdk/XPWidgets/#XPGetWidgetWithFocus>`__ :index:`XPGetWidgetWithFocus`

.. py:function::  addWidgetCallback(widgetID, callback)

    This function adds a new widget callback
    to a widget. This widget callback
    supersedes any existing ones and will receive messages first; if it does
    not handle messages they will go on to be handled by pre-existing widgets.

    *callback* function is identical to that described in :py:func:`createCustomWidget`,
    with signature ``callback(message, widgetID, param1, param2)``.

    The callback will remain on the widget for the life of the widget.
    The creation message will be sent to the new callback immediately with the
    widget ID, and the destruction message will be sent before the other widget
    function receives a destruction message.

    This provides a way to 'subclass' an existing widget. By providing a second
    hook that only handles certain widget messages, you can customize or extend
    widget behavior.

    `Official SDK <https://developer.x-plane.com/sdk/XPWidgets/#XPAddWidgetCallback>`__ :index:`XPAddWidgetCallback`

.. py:function::  getWidgetClassFunc(widgetClass)

    Given a *widgetClass* (:ref:`XPWidgetClass`), this function returns the callback that power that
    widget class.

    >>> widgetID = xp.createWidget(100, 200, 300, 100, 1, "My Widget", 1, 0, xp.WidgetClass_MainWindow)
    >>> text_func = xp.getWidgetClassFunc(xp.WidgetClass_TextField)
    >>> textwidgetID = xp.createCustomWidget(110, 180, 290, 165, 1, "Edit me", 0, widgetID, text_func)

    .. note:: This is problematic in python because the returned callback is a C function. There's
              not much you can do with it to really "subclass" the functionality. And, if you use
              it directly it works, but would seem to add zero benefit over just using :py:func:`createWidget`
              with given ``WidgetClass``.

              Short answer: don't use :py:func:getWidgetClassFunc`.
              Instead, create the (non-custom) widget and use :py:func:`addWidgetCallback` to add a python
              callback, and return 0 for some messages, to allow the original callback to handle
              instead.

    Not sure what the value is of this in the python implementation. If someone has a good reason to use it / need it, let me
    know.

    `Official SDK <https://developer.x-plane.com/sdk/XPWidgets/#XPGetWidgetClassFunc>`__ :index:`XPGetWidgetClassFunc`
    
Constants
---------

.. _XPWidgetID:

XPWidgetID
**********

Unique handle to a widget


