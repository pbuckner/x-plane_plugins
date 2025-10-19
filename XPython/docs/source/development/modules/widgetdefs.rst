XPWidgetDefs
============

.. py:module:: XPWidgetDefs
.. py:currentmodule:: xp

To use::

  import xp


A widget is a call-back driven screen entity like a push-button, window,
text entry field, etc.

Use the widget API to create widgets of various classes. You can nest them
into trees of widgets to create complex user interfaces.


Constants
---------

.. _XPWidgetPropertyID:

XPWidgetPropertyID
******************

Properties are values attached to instances of your widgets. A property is
identified by a 32-bit ID, and may be accessed using :py:func:`setWidgetProperty` and :py:func:`getWidgetProperty`.

Each widget instance may have a property or not have it. When you set a
property on a widget for the first time, the property is added to the
widget; it then stays there for the life of the widget.

Some property IDs are predefined by the widget package; you can make up
your own property IDs as well.

+----------------------------------------+------------------------------------------------------------------------------------------+
| .. py:data:: Property_Refcon           |A window's refcon is an opaque value used by client code to find other data based on it.  |
|  :value: 0                             |                                                                                          |
|                                        |`Official SDK <https://developer.x-plane.com/sdk/XPWidgetDefs/#xpProperty_Refcon>`__      |
|                                        |:index:`xpProperty_Refcon`                                                                |
+----------------------------------------+------------------------------------------------------------------------------------------+
| .. py:data:: Property_Dragging         |These properties are used by the utilities to implement dragging.                         |
|  :value: 1                             |                                                                                          |
| .. py:data:: Property_DragXOff         |`Official SDK <https://developer.x-plane.com/sdk/XPWidgetDefs/#xpProperty_Dragging>`__    |
|  :value: 2                             |:index:`xpProperty_Dragging`                                                              |
| .. py:data:: Property_DragYOff         |                                                                                          |
|  :value: 3                             |`Official SDK <https://developer.x-plane.com/sdk/XPWidgetDefs/#xpProperty_DragXOff>`__    |
|                                        |:index:`xpProperty_DragXOff`                                                              |
|                                        |                                                                                          |
|                                        |`Official SDK <https://developer.x-plane.com/sdk/XPWidgetDefs/#xpProperty_DragYOff>`__    |
|                                        |:index:`xpProperty_DragYOff`                                                              |
|                                        |                                                                                          |
+----------------------------------------+------------------------------------------------------------------------------------------+
| .. py:data:: Property_Hilited          |Is the widget hilited?  (For widgets that support this kind of thing.)                    |
|  :value: 4                             |                                                                                          |
|                                        |`Official SDK <https://developer.x-plane.com/sdk/XPWidgetDefs/#xpProperty_Hilited>`__     |
|                                        |:index:`xpProperty_Hilited`                                                               |
|                                        |                                                                                          |
+----------------------------------------+------------------------------------------------------------------------------------------+
| .. py:data:: Property_Object           |Is there a C++ object attached to this widget?                                            |
|  :value: 5                             |                                                                                          |
|                                        |`Official SDK <https://developer.x-plane.com/sdk/XPWidgetDefs/#xpProperty_Object>`__      |
|                                        |:index:`xpProperty_Object`                                                                |
+----------------------------------------+------------------------------------------------------------------------------------------+
| .. py:data:: Property_Clip             |If this property is 1, the widget package will use OpenGL to restrict drawing to the      |
|  :value: 6                             |Widget's exposed rectangle.                                                               |
|                                        |                                                                                          |
|                                        |`Official SDK <https://developer.x-plane.com/sdk/XPWidgetDefs/#xpProperty_Clip>`__        |
|                                        |:index:`xpProperty_Clip`                                                                  |
+----------------------------------------+------------------------------------------------------------------------------------------+
|.. py:data:: Property_Enabled           |Is this widget enabled (for those that have a disabled state too)?                        |
|  :value: 7                             |                                                                                          |
|                                        |`Official SDK <https://developer.x-plane.com/sdk/XPWidgetDefs/#xpProperty_Enabled>`__     |
|                                        |:index:`xpProperty_Enabled`                                                               |
+----------------------------------------+------------------------------------------------------------------------------------------+
|                                        |                                                                                          |
+----------------------------------------+------------------------------------------------------------------------------------------+
|.. py:data:: Property_UserStart         |Minimum value for a user-defined property                                                 |
|  :value: 10000                         |                                                                                          |
|                                        |`Official SDK <https://developer.x-plane.com/sdk/XPWidgetDefs/#xpProperty_UserStart>`__   |
|                                        |:index:`xpProperty_UserStart`                                                             |
+----------------------------------------+------------------------------------------------------------------------------------------+

.. note:: Property IDs 1 - 999 are reserved for the widget's library.
.. note:: Property IDs 1000 - 9999 are allocated to the standard widget classes
  provided with the library Properties 1000 - 1099 are for widget class 0,
  1100 - 1199 for widget class 1, etc.

  If you create your own property, make sure it's value is greater than :py:data:`Property_UserStart`.
  Such properties will use Python object for storage rather than a simple int or float.



.. _XPDispatchMode:

XPDispatchMode
**************

The dispatching modes describe how the widgets library sends out messages.

 +----------------------------------------------------------+------------------------------------------------------------------------------+
 |.. py:data:: Mode_Direct                                  |The message will only be sent to the target widget.                           |
 |  :value: 0                                               |                                                                              |
 |                                                          |`Official SDK                                                                 |
 |                                                          |<https://developer.x-plane.com/sdk/XPWidgetDefs/#xpMode_Direct>`__            |
 |                                                          |:index:`xpMode_Direct`                                                        |
 |                                                          |                                                                              |
 +----------------------------------------------------------+------------------------------------------------------------------------------+
 |.. py:data:: Mode_UpChain                                 |The message is sent to the target widget, then up the chain of                |
 |  :value: 1                                               |parents until the message is handled or a parentless widget is                |
 |                                                          |reached.                                                                      |
 |                                                          |                                                                              |
 |                                                          |`Official SDK                                                                 |
 |                                                          |<https://developer.x-plane.com/sdk/XPWidgetDefs/#xpMode_UpChain>`__           |
 |                                                          |:index:`xpMode_UpChain`                                                       |
 |                                                          |                                                                              |
 +----------------------------------------------------------+------------------------------------------------------------------------------+
 |.. py:data:: Mode_Recursive                               |The message is sent to the target widget and then all of its children         |
 |  :value: 2                                               |recursively depth-first.                                                      |
 |                                                          |                                                                              |
 |                                                          |`Official SDK                                                                 |
 |                                                          |<https://developer.x-plane.com/sdk/XPWidgetDefs/#xpMode_Recursive>`__         |
 |                                                          |:index:`xpMode_Recursive`                                                     |
 +----------------------------------------------------------+------------------------------------------------------------------------------+
 |.. py:data:: Mode_DirectAllCallbacks                      |The message is sent just to the target, but goes to every callback, even if it|
 |  :value: 3                                               |is handled.                                                                   |
 |                                                          |                                                                              |
 |                                                          |`Official SDK                                                                 |
 |                                                          |<https://developer.x-plane.com/sdk/XPWidgetDefs/#xpMode_DirectAllCallbacks>`__|
 |                                                          |:index:`xpMode_DirectAllCallbacks`                                            |
 |                                                          |                                                                              |
 +----------------------------------------------------------+------------------------------------------------------------------------------+
 |.. py:data:: Mode_Once                                    |The message is only sent to the very first handler even if it is not          |
 |  :value: 4                                               |accepted. (This is really only useful for some internal Widget Lib functions. |
 |                                                          |                                                                              |
 |                                                          |`Official SDK <https://developer.x-plane.com/sdk/XPWidgetDefs/#xpMode_Once>`__|
 |                                                          |:index:`xpMode_Once`                                                          |
 |                                                          |                                                                              |
 +----------------------------------------------------------+------------------------------------------------------------------------------+
 

.. _XPWidgetClass:

XPWidgetClass
*************

Widget classes define predefined widget types. A widget class basically
specifies from a library the widget function to be used for the widget.
Most widgets can be made right from classes.

 .. py:data:: WidgetClass_None
  :value: 0

 `Official SDK <https://developer.x-plane.com/sdk/XPWidgetDefs/#xpWidgetClass_None>`__ :index:`xpWidgetClass_None`
 
.. note:: Additional widget classes are defined in :py:mod:`XPStandardWidgets`.

.. _XPWidgetMessage:

XPWidgetMessage
***************

Widgets receive 32-bit messages indicating what action is to be taken or
notifications of events. The list of messages may be expanded.

You can intercept widget messages by adding a callback (:py:func:`addWidgetCallback`)
to the widget, or one of its parents.

 .. note:: Additional widget messages are defined in :py:mod:`XPStandardWidgets`.

 .. py:data:: Msg_None
  :value: 0

  No message, should not be sent.

  `Official SDK <https://developer.x-plane.com/sdk/XPWidgetDefs/#xpMsg_None>`__ :index:`xpMsg_None`

 .. py:data:: Msg_Create
  :value: 1

  The create message is sent once per widget that is created with your widget
  function and once for any widget that has your widget function attached.   

  .. table::
     :align: left

     =========== =========================== ===============
     Dispatching Param1                      Param2
     =========== =========================== ===============
     Direct      1= you are being added as a N/A
                 subclass, 0 if the widget
                 is first being created.
     =========== =========================== ===============

  `Official SDK <https://developer.x-plane.com/sdk/XPWidgetDefs/#xpMsg_Create>`__ :index:`xpMsg_Create`

 .. py:data:: Msg_Destroy
  :value: 2

  The destroy message is sent once for each message that is destroyed that   
  has your widget function.                                                  

  .. table::
     :align: left

     =========== =========================== ===============
     Dispatching Param1                      Param2
     =========== =========================== ===============
     Direct for  1= you are being deleted by N/A
     all         a recursive delete to the
                 parent, 0 for explicit
                 deletion.
     =========== =========================== ===============

  `Official SDK <https://developer.x-plane.com/sdk/XPWidgetDefs/#xpMsg_Destroy>`__ :index:`xpMsg_Destroy`

 .. py:data:: Msg_Paint
  :value: 3

  The paint message is sent to your widget to draw itself. The paint message 
  is the bare-bones message; in response you must draw yourself, draw your   
  children, set up clipping and culling, check for visibility, etc. If you   
  don't want to do all of this, ignore the paint message and a :py:data:`Msg_Paint`
  (see below) will be sent to you.                                           
                                                                              
  .. table::
     :align: left

     =========== =========================== ===============
     Dispatching Param1                      Param2
     =========== =========================== ===============
     Direct      N/A                         N/A
     =========== =========================== ===============

  `Official SDK <https://developer.x-plane.com/sdk/XPWidgetDefs/#xpMsg_Paint>`__ :index:`xpMsg_Paint`

 .. py:data:: Msg_Draw
  :value: 4

  The draw message is sent to your widget when it is time to draw yourself.  
  OpenGL will be set up to draw in 2-d global screen coordinates, but you    
  should use the XPLM to set up OpenGL state.                                
                                                                              
  .. table::
     :align: left

     =========== =========================== ===============
     Dispatching Param1                      Param2
     =========== =========================== ===============
     Direct      N/A                         N/A
     =========== =========================== ===============

  `Official SDK <https://developer.x-plane.com/sdk/XPWidgetDefs/#xpMsg_Draw>`__ :index:`xpMsg_Draw`

 .. py:data:: Msg_KeyPress
  :value: 5

  The key press message is sent once per key that is pressed. The first      
  parameter is the type of key code (integer or char) and the second is the  
  code itself.
                                                                             
  .. note:: Handling this message 'consumes' the keystroke; not handling it passes it  
      to your parent widget.                                                     
                                                                              
  .. table::
     :align: left

     =========== =========================== ===============
     Dispatching Param1                      Param2
     =========== =========================== ===============
     Up Chain    :ref:`XPKeyState_t` tuple   key code
     =========== =========================== ===============

  `Official SDK <https://developer.x-plane.com/sdk/XPWidgetDefs/#xpMsg_KeyPress>`__ :index:`xpMsg_KeyPress`

 .. py:data:: Msg_KeyTakeFocus
  :value: 6

  Keyboard focus is being given to you.
                                                                              
  .. note:: Handling this message accepts focus; not handling refuses focus.         
                                                                              
  .. table::
     :align: left

     =========== =========================== ===============
     Dispatching Param1                      Param2
     =========== =========================== ===============
     Direct      1= a child of yours gave up N/A
                 focus to you. 0= someone
                 set focus to you
                 explicitly.
     =========== =========================== ===============

  `Official SDK <https://developer.x-plane.com/sdk/XPWidgetDefs/#xpMsg_KeyTakeFocus>`__ :index:`xpMsg_KeyTakeFocus`

 .. py:data:: Msg_KeyLoseFocus
  :value: 7

  Keyboard focus is being taken away from you. The first parameter will be   
  one if you are losing focus because another widget is taking it, or 0 if   
  someone called the API to make you lose focus explicitly.                  
                                                                             
  .. table::
     :align: left

     =========== =========================== ===============
     Dispatching Param1                      Param2
     =========== =========================== ===============
     Direct      1= if focus is being taken  N/A
                 by another widget.
                 0= code requested to remove
                 focus.
     =========== =========================== ===============

  `Official SDK <https://developer.x-plane.com/sdk/XPWidgetDefs/#xpMsg_KeyLoseFocus>`__ :index:`xpMsg_KeyLoseFocus`

 .. py:data:: Msg_MouseDown
  :value: 8

  You receive one mousedown event per click with a mouse-state structure     
  pointed to by parameter 1, by accepting this you eat the click, otherwise  
  your parent gets it. You will not receive drag and mouse up messages if you
  do not accept the down message.                                            
                                                                              
  .. note::
    Handling this message consumes the mouse click, not handling it passes it  
    to the next widget. You can act 'transparent' as a window by never handling
    mouse clicks to certain areas.                                             
                                                                              
  Dispatching: Up chain BUT: Technically this is direct dispatched, but the 
  widgets library will shop it to each widget until one consumes the click,  
  making it effectively "up chain".                                          
                                                                              
  .. table::
     :align: left

     =========== =========================== ===============
     Dispatching Param1                      Param2
     =========== =========================== ===============
     Up chain    :ref:`XPMouseState_t`       N/A
                 tuple.
     =========== =========================== ===============

  `Official SDK <https://developer.x-plane.com/sdk/XPWidgetDefs/#xpMsg_MouseDown>`__ :index:`xpMsg_MouseDown`

 .. py:data:: Msg_MouseDrag
  :value: 9

  You receive a series of mouse drag messages (typically one per frame in the
  sim) as the mouse is moved once you have accepted a mouse down message.    
  Parameter one points to a mouse-state structure describing the mouse       
  location. You will continue to receive these until the mouse button is     
  released. You may receive multiple mouse state messages with the same mouse
  position. You will receive mouse drag events even if the mouse is dragged  
  out of your current or original bounds at the time of the mouse down.      
                                                                              
  .. table::
     :align: left

     =========== =========================== ===============
     Dispatching Param1                      Param2
     =========== =========================== ===============
     Direct      :ref:`XPMouseState_t`       N/A
                 tuple.
     =========== =========================== ===============

  `Official SDK <https://developer.x-plane.com/sdk/XPWidgetDefs/#xpMsg_MouseDrag>`__ :index:`xpMsg_MouseDrag`

 .. py:data:: Msg_MouseUp
  :value: 10

  The mouseup event is sent once when the mouse button is released after a   
  drag or click. You only receive this message if you accept the MouseDown   
  message. Parameter one points to a mouse state structure.                  
                                                                              
  .. table::
     :align: left

     =========== =========================== ===============
     Dispatching Param1                      Param2
     =========== =========================== ===============
     Direct      :ref:`XPMouseState_t`       N/A
                 tuple.
     =========== =========================== ===============

  `Official SDK <https://developer.x-plane.com/sdk/XPWidgetDefs/#xpMsg_MouseUp>`__ :index:`xpMsg_MouseUp`

 .. py:data:: Msg_Reshape
  :value: 11

  Your geometry or a child's geometry is being changed.                      

  .. table::
     :align: left

     =========== =========================== ===============================
     Dispatching Param1                      Param2
     =========== =========================== ===============================
     Up-chain    WidgetID of original        :ref:`XPwidgetGeometryChange_t`
                 reshaped target.            tuple.
     =========== =========================== ===============================

  `Official SDK <https://developer.x-plane.com/sdk/XPWidgetDefs/#xpMsg_Reshape>`__ :index:`xpMsg_Reshape`

 .. py:data:: Msg_ExposedChanged
  :value: 12

  Your exposed area has changed.                                             
                                                                              
  .. table::
     :align: left

     =========== =========================== ===============
     Dispatching Param1                      Param2
     =========== =========================== ===============
     Direct      N/A                         N/A
     =========== =========================== ===============

  `Official SDK <https://developer.x-plane.com/sdk/XPWidgetDefs/#xpMsg_ExposedChanged>`__ :index:`xpMsg_ExposedChanged`

 .. py:data:: Msg_AcceptChild
  :value: 13

  A child has been added to you. The child's ID is passed in parameter one.  

  .. table::
     :align: left

     =========== =========================== ===============
     Dispatching Param1                      Param2
     =========== =========================== ===============
     Direct      Widget ID of child being    N/A
                 added.
     =========== =========================== ===============

  `Official SDK <https://developer.x-plane.com/sdk/XPWidgetDefs/#xpMsg_AcceptChild>`__ :index:`xpMsg_AcceptChild`

 .. py:data:: Msg_LoseChild
  :value: 14

  A child has been removed from to you. The child's ID is passed in parameter
  one.                                                                       

  .. table::
     :align: left

     =========== =========================== ===============
     Dispatching Param1                      Param2
     =========== =========================== ===============
     Direct      Widget ID of child being    N/A
                 removed.
     =========== =========================== ===============

  `Official SDK <https://developer.x-plane.com/sdk/XPWidgetDefs/#xpMsg_LoseChild>`__ :index:`xpMsg_LoseChild`

 .. py:data:: Msg_AcceptParent
  :value: 15

  You now have a new parent, or have no parent. The parent's ID is passed in,
  or 0 for no parent.                                                        
                                                                              
  .. table::
     :align: left

     =========== =========================== ===============
     Dispatching Param1                      Param2
     =========== =========================== ===============
     Direct      Widget ID of your parent,   N/A
                 0 if no parent.
     =========== =========================== ===============

  `Official SDK <https://developer.x-plane.com/sdk/XPWidgetDefs/#xpMsg_AcceptParent>`__ :index:`xpMsg_AcceptParent`

 .. py:data:: Msg_Shown
  :value: 16

  You or a child has been shown. Note that this does not include you being   
  shown because your parent was shown, you were put in a new parent, your    
  root was shown, etc.                                                       
                                                                              
  Dispatching: Up chain                                                      
                                                                              
  Param 1: The widget ID of the shown widget.                                

  .. table::
     :align: left

     =========== =========================== ===============
     Dispatching Param1                      Param2
     =========== =========================== ===============
     Up-chain    WidgetID of shown widget.   N/A
     =========== =========================== ===============

  `Official SDK <https://developer.x-plane.com/sdk/XPWidgetDefs/#xpMsg_Shown>`__ :index:`xpMsg_Shown`

 .. py:data:: Msg_Hidden
  :value: 17

  You have been hidden. See limitations as with :py:data:`Msg_Shown` above.                               

  .. table::
     :align: left

     =========== =========================== ===============
     Dispatching Param1                      Param2
     =========== =========================== ===============
     Up-chain    WidgetID of hidden widget.  N/A
     =========== =========================== ===============

  `Official SDK <https://developer.x-plane.com/sdk/XPWidgetDefs/#xpMsg_Hidden>`__ :index:`xpMsg_Hidden`

 .. py:data:: Msg_DescriptorChanged
  :value: 18

  Your descriptor has changed.                                               

  .. table::
     :align: left

     =========== =========================== ===============
     Dispatching Param1                      Param2
     =========== =========================== ===============
     Direct      N/A                         N/A
     =========== =========================== ===============

  `Official SDK <https://developer.x-plane.com/sdk/XPWidgetDefs/#xpMsg_DescriptorChanged>`__ :index:`xpMsg_DescriptorChanged`

 .. py:data:: Msg_PropertyChanged
  :value: 19

  A property has changed. Param 1 contains the property ID.                  

  .. table::
     :align: left

     =========== =========================== ===============
     Dispatching Param1                      Param2
     =========== =========================== ===============
     Direct      :ref:`XPWidgetPropertyID`   New property
                 being changed.              value.
     =========== =========================== ===============

  `Official SDK <https://developer.x-plane.com/sdk/XPWidgetDefs/#xpMsg_PropertyChanged>`__ :index:`xpMsg_PropertyChanged`

 .. py:data:: Msg_MouseWheel
  :value: 20

  The mouse wheel has moved.                                                 
                                                                              
  .. note:: Handling this message 'consumes' the wheel event; not handling it passes it  
      to your parent widget.                                                     

  .. table::
     :align: left

     =========== =========================== ===============
     Dispatching Param1                      Param2
     =========== =========================== ===============
     Up-chain    :ref:`XPMouseState_t`       N/A
                 tuple.
     =========== =========================== ===============

  `Official SDK <https://developer.x-plane.com/sdk/XPWidgetDefs/#xpMsg_MouseWheel>`__ :index:`xpMsg_MouseWheel`

 .. py:data:: Msg_CursorAdjust
  :value: 21

  The cursor is over your widget. If you consume this message, change the    
  second parameter of this message to the updated :ref:`CursorStatus <XPLMCursorStatus>`, and return
  1 to consume the event.
                                                                              
  .. note:: Handling this message 'consumes' the message.

  .. table::
     :align: left

     ============== =========================== ======================================
     Dispatching    Param1                      Param2
     ============== =========================== ======================================
     Up-chain       :ref:`XPMouseState_t`       Param2 is a single-element list.
                    tuple                       You callback should set ``param2[0]``
                                                to updated
                                                :ref:`CursorStatus <XPLMCursorStatus>`
                                                result you desire, and return 1 to
                                                consume the event.
                                                (See also :py:func:`xp.setCursor`)
     ============== =========================== ======================================

  `Official SDK <https://developer.x-plane.com/sdk/XPWidgetDefs/#xpMsg_CursorAdjust>`__ :index:`xpMsg_CursorAdjust`

 .. py:data:: Msg_UserStart
  :value: 1000

  .. table::
     :align: left

     ============= =========================== ===============
     Dispatching   Param1                      Param2
     ============= =========================== ===============
     User Provided User Provided               User Provided
     ============= =========================== ===============

  `Official SDK <https://developer.x-plane.com/sdk/XPWidgetDefs/#xpMsg_UserStart>`__ :index:`xpMsg_UserStart`

Tuples
------

The C-API packed some returns in a structure, requiring you
to manipulate it to extract out the values. XPPython3 extracts the
values for you, and returns a tuple instead for these items:

.. _XPMouseState_t:
 
XPMouseState_t
**************
 
XPMouseState_t is a four integer tuple which contains information about the
current state of the mouse. It is provided with messages when the mouse is
is clicked or dragged::

  mouseState = (x,       # (x, y) position of the mouse
                y,
                button,  # mouse button, left = 0 (right button not supported yet)
                delta    # scroll wheel delta (button, in this case would be the wheel axis number)
               )

.. _XPKeyState_t:
 
XPKeyState_t
************

XPKeyState_t is a three integer tuple which contains information about
the key. It is provided with messages when a key is pressed::
 
  keyState = (key,    # The ASCII key pressed. WARNING: this may be 0 for some non-ASCII
              flags,  # XPLMKeyFlags int
              vkey    # Virtual key code 
             )

* ``flags`` is a bitfield or'd values :ref:`XPLMKeyFlags` to indicate state of Shift, Ctrl, Alt and whether the key is up or down.
* ``vkey`` is the virtual key, one of :ref:`Virtual Key Codes`

.. _XPWidgetGeometryChange_t:
 
XPWidgetGeometryChange_t
************************
 
XPWidgetGeometryChange_t is a four integer tuple which contains deltas for
your widget's geometry when it changes::

  geometryChange = (dx,     # (dx, dy) change in position +Y = the widget moved up.    
                    dy,
                    dwidth, # (dwidth, dheight) change in size
                    dheight
                   )
