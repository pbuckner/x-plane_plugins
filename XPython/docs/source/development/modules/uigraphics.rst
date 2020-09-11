XPUIGraphics
============
.. py:module:: XPUIGraphics

To use::

  import XPUIGraphics

Functions
---------

.. py:function:: XPDrawWindow(x1:int, y1:int, x2:int, y2:int, style) -> None:

 :param x1:
 :param y1:
 :param x2:
 :param y2: (x1, y1) - lower left corner, (x2, y2) - upper right corner, ints
 :param style: :ref:`XPWindowStyle`            

 This routine draws a window of the given dimensions at the given offset on
 the virtual screen in a given style. The window is automatically scaled as
 appropriate using a bitmap scaling technique (scaling or repeating) as
 appropriate to the style.


.. py:function:: XPGetWindowDefaultDimensions(style) -> width, height:

 :param style: :ref:`XPWindowStyle`            
                 
 This routine returns the default dimensions for a window. Output is either
 a minimum or fixed value depending on whether the window is scalable.


.. py:function:: XPDrawElement(x1, y1, x2, y2, style, lit) -> None:

 :param x1:
 :param y1:
 :param x2:
 :param y2: (x1, y1) - lower left corner, (x2, y2) - upper right corner, ints
 :param style: :ref:`XPElementStyle`
 :param int lit: 1= lit

 XPDrawElement draws a given element at an offset on the virtual screen in
 set dimensions. EVEN if the element is not scalable, it will be scaled if
 the width and height do not match the preferred dimensions; it'll just look
 ugly. Pass lit=1 to see the lit version of the element; if the element
 cannot be lit this is ignored.


.. py:function:: XPGetElementDefaultDimensions(style) -> width, height, canBeLit:

 :param style: :ref:`XPElementStyle`

 This routine returns the recommended or minimum dimensions of a given UI
 element. outCanBeLit tells whether the element has both a lit and unlit
 state. Pass None to not receive any of these parameters.



.. py:function:: XPDrawTrack(x1, y1, x2, y2, min, max, value, style, lit) -> None:

 :param int x1:
 :param int y1:
 :param int x2:
 :param int y2: (x1, y1) - lower left corner, (x2, y2) - upper right corner, ints
 :param int min: min value of track
 :param int max: max value of track
 :param style: :ref:`XPTrackStyle`
 :param int lit: 1= lit
  
    This routine draws a track. You pass in the track dimensions and size; the
    track picks the optimal orientation for these dimensions. Pass in the
    track's minimum current and maximum values; the indicator will be
    positioned appropriately. You can also specify whether the track is lit or
    not.


.. py:function:: XPGetTrackDefaultDimensions(inStyle) -> width, canBeLit:

 :param style: :ref:`XPTrackStyle`

 This routine returns a track's default smaller dimension; all tracks are
 scalable in the larger dimension. It also returns whether a track can be
 lit.

.. py:function:: XPGetTrackMetrics(x1, y1, x2, y2, min, max, value, style) -> trackMetrics

 :param int x1:
 :param int y1:
 :param int x2:
 :param int y2: (x1, y1) - lower left corner, (x2, y2) - upper right corner, ints
 :param int min: min value of track
 :param int max: max value of track
 :param int value: value of track
 :param style: :ref:`XPTrackStyle`
 :return: object which attributes:

   * isVertical: 1= true
   * downBtnSize: int       
   * downPageSize: int       
   * thumbSize: int
   * upPageSize: int
   * upBtnSize: int

 This routine returns the metrics of a track. If you want to write UI code
 to manipulate a track, this routine helps you know where the mouse
 locations are. For most other elements, the rectangle the element is drawn
 in is enough information. However, the scrollbar drawing routine does some
 automatic placement; this routine lets you know where things ended up. You
 pass almost everything you would pass to the draw routine. You get out the
 orientation, and other useful stuff.

 Besides orientation, you get five dimensions for the five parts of a
 scrollbar, which are the down button, down area (area before the thumb),
 the thumb, and the up area and button. For horizontal scrollers, the left
 button decreases; for vertical scrollers, the top button decreases.

Constants
---------

.. _XPWindowStyle:

XPWindowStyle
*************

There are a few built-in window styles in X-Plane that you can use.

Note that X-Plane 6 does not offer real shadow-compositing; you must make
sure to put a window on top of another window of the right style to the
shadows work, etc. This applies to elements with insets and shadows. The
rules are:

Sub windows must go on top of main windows, and screens and list views on
top of subwindows. Only help and main windows can be over the main screen.

With X-Plane 7 any window or element may be placed over any other element.

Some windows are scaled by stretching, some by repeating. The drawing
routines know which scaling method to use. The list view cannot be rescaled
in X-Plane 6 because it has both a repeating pattern and a gradient in one
element. All other elements can be rescaled.


 .. py:data:: xpWindow_Help

  An LCD screen that shows help.

 .. py:data:: xpWindow_MainWindow

  A dialog box window.

 .. py:data:: xpWindow_SubWindow

  A panel or frame within a dialog box window.

 .. py:data:: xpWindow_Screen

  An LCD screen within a panel to hold text displays.

 .. py:data:: xpWindow_ListView

  A list view within a panel for scrolling file names, etc.


.. _XPElementStyle:

XPElementStyle
**************

Elements are individually drawable UI things like push buttons, etc. The
style defines what kind of element you are drawing. Elements can be
stretched in one or two dimensions (depending on the element). Some
elements can be lit.

In X-Plane 6 some elements must be drawn over metal. Some are scalable and
some are not. Any element can be drawn anywhere in X-Plane 7.

 .. table::
    :align: left

    ================================================== ============= ===================    
    Element                                            Scalable Axes Required background
    ================================================== ============= ===================    
    .. py:data:: xpElement_TextField                   x             metal
    .. py:data:: xpElement_CheckBox                    none          metal
    .. py:data:: xpElement_CheckBoxLit                 none          metal
    .. py:data:: xpElement_WindowCloseBox              none          window header
    .. py:data:: xpElement_WindowCloseBoxPressed       none          window header
    .. py:data:: xpElement_PushButton                  x             metal
    .. py:data:: xpElement_PushButtonLit               x             metal
    .. py:data:: xpElement_OilPlatform                 none          any
    .. py:data:: xpElement_OilPlatformSmall            none          any
    .. py:data:: xpElement_Ship                        none          any
    .. py:data:: xpElement_ILSGlideScope               none          any
    .. py:data:: xpElement_MarkerLeft                  none          any
    .. py:data:: xpElement_Airport                     none          any
    .. py:data:: xpElement_Waypoint                    none          any
    .. py:data:: xpElement_NDB                         none          any
    .. py:data:: xpElement_VOR                         none          any
    .. py:data:: xpElement_RadioTower                  none          any
    .. py:data:: xpElement_AircraftCarrier             none          any
    .. py:data:: xpElement_Fire                        none          any
    .. py:data:: xpElement_MarkerRight                 none          any
    .. py:data:: xpElement_CustomObject                none          any
    .. py:data:: xpElement_CoolingTower                none          any
    .. py:data:: xpElement_SmokeStack                  none          any
    .. py:data:: xpElement_Building                    none          any
    .. py:data:: xpElement_PowerLine                   none          any
    .. py:data:: xpElement_CopyButtons                 none          metal
    .. py:data:: xpElement_CopyButtonsWithEditingGrid  none          metal
    .. py:data:: xpElement_EditingGrid                 x, y          metal 
    .. py:data:: xpElement_ScrollBar                                 THIS CAN PROBABLY BE REMOVED
    .. py:data:: xpElement_VORWithCompassRose          none          any
    .. py:data:: xpElement_Zoomer                      none          metal
    .. py:data:: xpElement_TextFieldMiddle             x, y          metal
    .. py:data:: xpElement_LittleDownArrow             none          metal
    .. py:data:: xpElement_LittleUpArrow               none          metal
    .. py:data:: xpElement_WindowDragBar               none          metal
    .. py:data:: xpElement_WindowDragBarSmooth         none          metal
    ================================================== ============= ===================    


.. _XPTrackStyle:

XPTrackStyle
************

A track is a UI element that displays a value vertically or horizontally.
X-Plane has three kinds of tracks: scroll bars, sliders, and progress bars.
Tracks can be displayed either horizontally or vertically; tracks will
choose their own layout based on the larger dimension of their dimensions
(e.g. they know if they are tall or wide). Sliders may be lit or unlit
(showing the user manipulating them).


 .. py:data:: xpTrack_ScrollBar

  ScrollBar - this is a standard scroll bar with arrows and a thumb to drag.

 .. py:data:: xpTrack_Slider

  Slider - this is a simple track with a ball in the middle that can be slid.
  Can be lit, can be rotated.

 .. py:data:: xpTrack_Progress

  Progress - this is a progress indicator showing how a long task is going.
  Cannot be lit, cannot be rotated.


