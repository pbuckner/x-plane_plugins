XPWidgetDefs
============
.. py:module:: XPWidgetDefs

To use::

  import XPWidgetDefs


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
identified by a 32-bit ID.

Each widget instance may have a property or not have it. When you set a
property on a widget for the first time, the property is added to the
widget; it then stays there for the life of the widget.

Some property IDs are predefined by the widget package; you can make up
your own property IDs as well.

 .. py:data:: xpProperty_Refcon

  A window's refcon is an opaque value used by client code to find other data
  based on it.

 .. py:data:: xpProperty_Dragging
    xpProperty_DragXOff
    xpProperty_DragYOff

   These properties are used by the utlities to implement dragging.

 .. py:data:: xpProperty_Hilited

  Is the widget hilited?  (For widgets that support this kind of thing.)

 .. py:data:: xpProperty_Object

   Is there a C++ object attached to this widget?

 .. py:data:: xpProperty_Clip

  If this property is 1, the widget package will use OpenGL to restrict
  drawing to the Wiget's exposed rectangle.

 .. py:data:: xpProperty_Enabled

  Is this widget enabled (for those that have a disabled state too)?

 .. py:data:: xpProperty_UserStart

  Minimum value for a user-defined property

.. note:: Property IDs 1 - 999 are reserved for the widget's library.
.. note:: Property IDs 1000 - 9999 are allocated to the standard widget classes
  provided with the library Properties 1000 - 1099 are for widget class 0,
  1100 - 1199 for widget class 1, etc.

  If you create your own property, make sure it's value is greater than :py:data:`xpProperty_UserStart`.
  Such properties will use Python object for storage rather than a simple int or float.



.. _XPDispatchMode:

XPDispatchMode
**************

The dispatching modes describe how the widgets library sends out messages.

 .. py:data:: xpMode_Direct

  The message will only be sent to the target widget.

 .. py:data:: xpMode_UpChain

  The message is sent to the target widget, then up the chain of parents
  until the message is handled or a parentless widget is reached.

 .. py:data:: xpMode_Recursive

  The message is sent to the target widget and then all of its children
  recursively depth-first.

 .. py:data:: xpMode_DirectAllCallbacks

   The message is sent just to the target, but goes to every callback, even if
   it is handled.

 .. py:data:: xpMode_Once

  The message is only sent to the very first handler even if it is not
  accepted. (This is really only useful for some internal Widget Lib
  functions.


.. _XPWidgetClass:

XPWidgetClass
*************

Widget classes define predefined widget types. A widget class basically
specifies from a library the widget function to be used for the widget.
Most widgets can be made right from classes.

 .. py:data:: xpWidgetClass_None

.. note:: Additional widget classes are defined in :py:mod:`XPStandardWidgets`.

.. _XPWidgetMessage:

XPWidgetMessage
***************

Widgets receive 32-bit messages indicating what action is to be taken or
notifications of events. The list of messages may be expanded.

 .. note:: Additional widget messages are defined in :py:mod:`XPStandardWidgets`.

 .. py:data:: xpMsg_None

  No message, should not be sent.

 .. py:data:: xpMsg_Create

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

 .. py:data:: xpMsg_Destroy

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

 .. py:data:: xpMsg_Paint

  The paint message is sent to your widget to draw itself. The paint message 
  is the bare-bones message; in response you must draw yourself, draw your   
  children, set up clipping and culling, check for visibility, etc. If you   
  don't want to do all of this, ignore the paint message and a :py:data:`xpMsg_Paint`
  (see below) will be sent to you.                                           
                                                                              
  .. table::
     :align: left

     =========== =========================== ===============
     Dispatching Param1                      Param2
     =========== =========================== ===============
     Direct      N/A                         N/A
     =========== =========================== ===============

 .. py:data:: xpMsg_Draw

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

 .. py:data:: xpMsg_KeyPress

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

 .. py:data:: xpMsg_KeyTakeFocus

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

 .. py:data:: xpMsg_KeyLoseFocus

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

 .. py:data:: xpMsg_MouseDown

  You receive one mousedown event per click with a mouse-state structure     
  pointed to by parameter 1, by accepting this you eat the click, otherwise  
  your parent gets it. You will not receive drag and mouse up messages if you
  do not accept the down message.                                            
                                                                              
  .. note::
    Handling this message consumes the mouse click, not handling it passes it  
    to the next widget. You can act 'transparent' as a window by never handling
    moues clicks to certain areas.                                             
                                                                              
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

 .. py:data:: xpMsg_MouseDrag

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

 .. py:data:: xpMsg_MouseUp

  The mouseup event is sent once when the mouse button is released after a   
  drag or click. You only receive this message if you accept the mouseDown   
  message. Parameter one points to a mouse state structure.                  
                                                                              
  .. table::
     :align: left

     =========== =========================== ===============
     Dispatching Param1                      Param2
     =========== =========================== ===============
     Direct      :ref:`XPMouseState_t`       N/A
                 tuple.
     =========== =========================== ===============

 .. py:data:: xpMsg_Reshape

  Your geometry or a child's geometry is being changed.                      

  .. table::
     :align: left

     =========== =========================== ===============================
     Dispatching Param1                      Param2
     =========== =========================== ===============================
     Up-chain    WidgetID of original        :ref:`XPwidgetGeometryChange_t`
                 reshaped target.            tuple.
     =========== =========================== ===============================

 .. py:data:: xpMsg_ExposedChanged

  Your exposed area has changed.                                             
                                                                              
  .. table::
     :align: left

     =========== =========================== ===============
     Dispatching Param1                      Param2
     =========== =========================== ===============
     Direct      N/A                         N/A
     =========== =========================== ===============

 .. py:data:: xpMsg_AcceptChild

  A child has been added to you. The child's ID is passed in parameter one.  

  .. table::
     :align: left

     =========== =========================== ===============
     Dispatching Param1                      Param2
     =========== =========================== ===============
     Direct      Widget ID of child being    N/A
                 added.
     =========== =========================== ===============

 .. py:data:: xpMsg_LoseChild

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

 .. py:data:: xpMsg_AcceptParent

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

 .. py:data:: xpMsg_Shown

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

 .. py:data:: xpMsg_Hidden

  You have been hidden. See limitations as with :py:data:`xpMsg_Shown` above.                               

  .. table::
     :align: left

     =========== =========================== ===============
     Dispatching Param1                      Param2
     =========== =========================== ===============
     Up-chain    WidgetID of hidden widget.  N/A
     =========== =========================== ===============

 .. py:data:: xpMsg_DescriptorChanged

  Your descriptor has changed.                                               

  .. table::
     :align: left

     =========== =========================== ===============
     Dispatching Param1                      Param2
     =========== =========================== ===============
     Direct      N/A                         N/A
     =========== =========================== ===============

 .. py:data:: xpMsg_PropertyChanged

  A property has changed. Param 1 contains the property ID.                  

  .. table::
     :align: left

     =========== =========================== ===============
     Dispatching Param1                      Param2
     =========== =========================== ===============
     Direct      :ref:`XPWidgetPropertyID`   New property
                 being changed.              value.
     =========== =========================== ===============

 .. py:data:: xpMsg_MouseWheel

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

 .. py:data:: xpMsg_CursorAdjust

  The cursor is over your widget. If you consume this message, change the    
  XPLMCursorStatus value to indicate the desired result, with the same rules 
  as in with :py:func:`XPLMDisplay.XPLMHandleCursor_f` callback used with windows.
                                                                              
  .. note:: Handling this message 'consumes' the message.

  .. table::
     :align: left

     =========== =========================== =======================
     Dispatching Param1                      Param2
     =========== =========================== =======================
     Up-chain    :ref:`XPMouseState_t`       :ref:`XPLMCursorStatus`
                 tuple.                      Set this to cursor   
                                             result you desire.
     =========== =========================== =======================

 .. py:data:: xpMsg_UserStart

  .. table::
     :align: left

     ============= =========================== ===============
     Dispatching   Param1                      Param2
     ============= =========================== ===============
     User Provided User Provided               User Provided
     ============= =========================== ===============

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

Functions
---------

.. py:function:: XPWidgetFunc_t(inMessage, inWidget, inParam1, inParam2) -> int:

  :param inMessage: :ref:`XPWidgetMessage` (may be custom)
  :param inWidget: Your WidgetID
  :param inParam1:
  :param inParam2: param1 and param2 are dependent on the particular message sent   
  :return: 1= you have handled the message, 0 otherwise.

  This function defines your custom widget's behavior. It will be called by
  the widgets library to send messages to your widget. The message and widget
  ID are passed in, as well as two ptr-width signed parameters whose meaning
  varies with the message. Return 1 to indicate that you have processed the
  message, 0 to indicate that you have not. For any message that is not
  understood, return 0.
