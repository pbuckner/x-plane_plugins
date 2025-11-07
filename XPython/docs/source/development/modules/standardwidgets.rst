XPStandardWidgets
=================
.. py:module:: XPStandardWidgets
.. py:currentmodule:: xp               

To use::

   import xp

The standard widgets are widgets built into the widgets library. While you
can gain access to the widget function that drives them, you generally use
them by calling :py:func:`createWidget` and then listening for special messages,
etc.

The standard widgets often send mesages to themselves when the user
performs an event; these messages are sent up the widget hierarchy until
they are handled. So you can add a widget proc directly to a push button
(for example) to intercept the message when it is clicked, or you can put
one widget proc on a window for all of the push buttons in the window. Most
of these messages contain the original widget ID as a parameter so you can
know which widget is messaging no matter who it is sent to.

 * :py:data:`WidgetClass_MainWindow`
 * :py:data:`WidgetClass_SubWindow`
 * :py:data:`WidgetClass_Button`
 * :py:data:`WidgetClass_TextField`
 * :py:data:`WidgetClass_ScrollBar`
 * :py:data:`WidgetClass_Caption`
 * :py:data:`WidgetClass_GeneralGraphics`
 * :py:data:`WidgetClass_Progress`

Widgets have properties which can be accessed using :py:func:`getWidgetProperty` and
:py:func:`setWidgetProperty`. Common properties (applicable to all most widgets are
listed :ref:`XPWidgetPropertyID`

Widget *also* receiving messages common to all widgets. These are listed :ref:`XPWidgetMessage`.    

----

Main Window
-----------
The main window widget class provides a "window" as the user knows it.
These windows are dragable and can be selected. Use them to create floating
windows and non-modal dialogs.

Class
*****

  .. py:data:: WidgetClass_MainWindow
    :value: 1

Properties
**********

 .. py:data:: Property_MainWindowType
  :value: 1100

  This property specifies the type of window. Set to one of the main window
  styles:

  .. table::
     :align: left

     ============================================ === ================================================
     .. py:data:: MainWindowStyle_MainWindow       =0 The standard main window; pin stripes on XP7+

     .. py:data:: MainWindowStyle_Translucent      =1 A translucent dark gray window, like the one ATC
                                                      messages appear in.
     ============================================ === ================================================
    
 .. py:data:: Property_MainWindowHasCloseBoxes
  :value: 1200

  This property specifies whether the main window has close boxes in its
  corners.

Messages
********
 .. py:data:: Message_CloseButtonPushed
  :value: 1200

  This message is sent when the close buttons are pressed for your window.

  .. table::
     :align: left

     ============================= ==============================
     Param 1                       Param 2
     ============================= ==============================
     N/A                           N/A
     ============================= ==============================

----

Sub Window
----------

X-Plane dialogs are divided into separate areas; the sub window widgets
allow you to make these areas. Create one main window and place several
subwindows inside it. Then place your controls inside the subwindows.

Class
*****

 .. py:data:: WidgetClass_SubWindow
    :value: 2


Properties
**********

 .. py:data:: Property_SubWindowType
  :value: 1200

  This property specifies the type of window. Set to one of the subwindow
  types:

  .. table::
     :align: left

     ===================================== === ========================================================
     .. py:data:: SubWindowStyle_SubWindow  =0 A panel that sits inside a main window.
     .. py:data:: SubWindowStyle_Screen     =2 A screen that sits inside a panel for showing
                                               text information.
     .. py:data:: SubWindowStyle_ListView   =3 A list view for scrolling lists.
     ===================================== === ========================================================

----

Button
------

The button class provides a number of different button styles and
behaviors, including push buttons, radio buttons, check boxes, etc. The
button label appears on or next to the button depending on the button's
appearance, or type.

The button's behavior is a separate property that dictates who it hilights
and what kinds of messages it sends. Since behavior and type are different,
you can do strange things like make check boxes that act as push buttons or
push buttons with radio button behavior.

Class
*****

 .. py:data:: WidgetClass_Button
    :value: 3


Properties
**********

 .. py:data:: Property_ButtonType
  :value: 1300

  This property sets the visual type of button. Use one of the button types:

  .. table::
     :align: left

     ============================ === ========================================================
     .. py:data:: PushButton       =0 This is a standard push button, like an 'OK' or 'Cancel'
                                      button in a dialog box.
     .. py:data:: RadioButton      =1 A check box or radio button. Use this and 
                                      the button behaviors below to get the
                                      desired behavior. 
     .. py:data:: WindowCloseBox   =3 A window close box.
     .. py:data:: LittleDownArrow  =5 A small down arrow.
     .. py:data:: LittleUpArrow    =6 A small up arrow.
     ============================ === ========================================================

 .. py:data:: Property_ButtonBehavior
  :value: 1301

  This property sets the button's behavior. Use one of the button behaviors
  below.

  .. table::
     :align: left
             
     ====================================== === =================================================
     .. py:data:: ButtonBehaviorPushButton   =0 Standard push button behavior. The button
                                                hilites while the mouse is clicked over
                                                it and unhilites when the mouse is moved
                                                outside of it or released. If the mouse
                                                is released over the button, the
                                                Msg_PushButtonPressed message is sent.
     .. py:data:: ButtonBehaviorCheckBox     =1 Check box behavior. The button
                                                immediately toggles its value when the
                                                mouse is clicked and sends out a
                                                Msg_ButtonStateChanged message.
     .. py:data:: ButtonBehaviorRadioButton  =2 Radio button behavior. The button
                                                immediately sets its state to one and
                                                sends out a Msg_ButtonStateChanged
                                                message if it was not already set to
                                                one. You must turn off other radio
                                                buttons in a group in your code.
     ====================================== === =================================================
  
 .. py:data:: Property_ButtonState
  :value: 1302

  This property tells whether a check box or radio button is "checked" or
  not. Not used for push buttons.

Messages
********

 .. py:data:: Msg_PushButtonPressed
  :value: 1300

  This message is sent when the user completes a click and release in a
  button with push button behavior. Parameter one of the message is the
  widget ID of the button. This message is dispatched up the widget
  hierarchy.

  .. table::
     :align: left

     ============================= ==============================
     Param 1                       Param 2
     ============================= ==============================
     WidgetID of the button        N/A
     ============================= ==============================

 .. py:data:: Msg_ButtonStateChanged
  :value: 1301

  This message is sent when a button is clicked that has radio button or
  check box behavior and its value changes. (Note that if the value changes
  by setting a property you do not receive this message!) Parameter one is
  the widget ID of the button, parameter 2 is the new state value, either
  zero or one. This message is dispatched up the widget hierarchy.

  .. table::
     :align: left

     ============================= ==============================
     Param 1                       Param 2
     ============================= ==============================
     WidgetID of the button        New state value (0 or 1)
     ============================= ==============================
   
----

Text Field
----------

The text field widget provides an editable text field including mouse
selection and keyboard navigation. The contents of the text field are its
descriptor. (The descriptor changes as the user types.)

The text field can have a number of types, that effect the visual layout of
the text field. The text field sends messages to itself so you may control
its behavior.

If you need to filter keystrokes, add a new handler and intercept the key
press message. Since key presses are passed by pointer, you can modify the
keystroke and pass it through to the text field widget.

Class
*****

 .. py:data:: WidgetClass_TextField
    :value: 4


Properties
**********

 .. py:data:: Property_EditFieldSelStart
  :value: 1400

  This is the character position the selection starts at, zero based. If it
  is the same as the end insertion point, the insertion point is not a
  selection.

 .. py:data:: Property_EditFieldSelEnd
  :value: 1401

  This is the character position of the end of the selection.

 .. py:data:: Property_EditFieldSelDragStart
  :value: 1402

  This is the character position a drag was started at if the user is
  dragging to select text, or -1 if a drag is not in progress.

 .. py:data:: Property_TextFieldType
  :value: 1403

  This is the type of text field to display:

  .. table::
     :align: left

     ============================ === ===============================================
     .. py:data:: TextEntryField   =0 A field for text entry.
     .. py:data:: TextTransparent  =3 A transparent text field. The user can type and
                                      the text is drawn, but no
                                      background is drawn. You can draw your own
                                      background by adding a widget handler and
                                      prehandling the draw message.
     .. py:data:: TextTranslucent  =4 A translucent edit field, dark gray.
     ============================ === ===============================================
   
 .. py:data:: Property_PasswordMode
  :value: 1404

  Set this property to 1 to password protect the field. Characters will be
  drawn as \*s even though the descriptor will contain plain-text.

 .. py:data:: Property_MaxCharacters
  :value: 1405

  The max number of characters you can enter, if limited. Zero means
  unlimited.

 .. py:data:: Property_ScrollPosition
  :value: 1406

  The first visible character on the left. This effectively scrolls the text
  field.

 .. py:data:: Property_Font
  :value: 1407

  The font to draw the field's text with. (An XPLMFontID.)

 .. py:data:: Property_ActiveEditSide
  :value: 1408

  This is the active side of the insert selection. (Internal)

Messages
********

 .. py:data:: Msg_TextFieldChanged
  :value: 1400

  The text field sends this message to itself when its text changes. It sends
  the message up the call chain; param1 is the text field's widget ID.

  .. table::
     :align: left

     ============================= ==============================
     Param 1                       Param 2
     ============================= ==============================
     WidgetID of the text field    N/A
     ============================= ==============================

----

Scroll Bar
----------

A standard scroll bar or slider control. The scroll bar has a minimum,
maximum and current value that is updated when the user drags it. The
scroll bar sends continuous messages as it is dragged.

Class
*****

 .. py:data:: WidgetClass_ScrollBar
    :value: 5

Properties
**********

 .. py:data:: Property_ScrollBarSliderPosition
  :value: 1500

  The current position of the thumb (in between the min and max, inclusive)

 .. py:data:: Property_ScrollBarMin
  :value: 1501

  The value the scroll bar has when the thumb is in the lowest position.

 .. py:data:: Property_ScrollBarMax
  :value: 1502

  The value the scroll bar has when the thumb is in the highest position.

 .. py:data:: Property_ScrollBarPageAmount
  :value: 1503

  How many units to move the scroll bar when clicking next to the thumb. The
  scroll bar always moves one unit when the arrows are clicked.

 .. py:data:: Property_ScrollBarType
  :value: 1504

  The type of scrollbar:

  .. table::
     :align: left

     ===================================== === =========================================================
     .. py:data:: ScrollBarTypeScrollBar    =0 A standard X-Plane scroll bar (with arrows on the ends).
     .. py:data:: ScrollBarTypeSlider       =1 A slider, no arrows.
     ===================================== === =========================================================
   
 .. py:data:: Property_ScrollBarSlop
  :value: 1505

  Used internally.


Messages
********

 .. py:data:: Msg_ScrollBarSliderPositionChanged
  :value: 1500

  The Scroll Bar sends this message when the slider position changes. It
  sends the message up the call chain; param1 is the Scroll Bar widget ID.

  .. table::
     :align: left
 
     ============================= ==============================
     Param 1                       Param 2
     ============================= ==============================
     WidgetID of the scroll bar    N/A
     ============================= ==============================

----

Caption
-------

A caption is a simple widget that shows its descriptor as a string, useful
for labeling parts of a window. It always shows its descriptor as its
string and is otherwise transparent.

Class
*****

 .. py:data:: WidgetClass_Caption
    :value: 6

Properties
**********

 .. py:data:: Property_CaptionLit
  :value: 1600

  This property specifies whether the caption is lit; use lit captions
  against screens.

----

General Graphics
----------------
The general graphics widget can show one of many icons available from
X-Plane.


Class
*****

 .. py:data:: WidgetClass_GeneralGraphics
    :value: 7

Properties
**********

 .. py:data:: Property_GeneralGraphicsType

  This property controls the type of icon that is drawn:

  .. table::
    :align: left

    =============================== ===
    .. py:data:: Ship                =4
    .. py:data:: ILSGlideScope       =5
    .. py:data:: MarkerLeft          =6
    .. py:data:: _Airport            =7
    .. py:data:: NDB                 =8
    .. py:data:: VOR                 =9
    .. py:data:: RadioTower         =10
    .. py:data:: AircraftCarrier    =11
    .. py:data:: Fire               =12
    .. py:data:: MarkerRight        =13
    .. py:data:: CustomObject       =14
    .. py:data:: CoolingTower       =15
    .. py:data:: SmokeStack         =16
    .. py:data:: Building           =17
    .. py:data:: PowerLine          =18 
    .. py:data:: VORWithCompassRose =19
    .. py:data:: OilPlatform        =21
    .. py:data:: OilPlatformSmall   =22
    .. py:data:: WayPoint           =23
    =============================== ===
   
----

Progress Indicator
------------------
This widget implements a progress indicator as seen when X-Plane starts up.


Class
*****

 .. py:data:: WidgetClass_Progress
    :value: 8

Properties
**********

 .. py:data:: Property_ProgressPosition
  :value: 1800

  This is the current value of the progress indicator.

 .. py:data:: Property_ProgressMin
  :value: 1801

  This is the minimum value, equivalent to 0% filled.

 .. py:data:: Property_ProgressMax
  :value: 1802
               
  This is the maximum value, equivalent to 100% filled.
