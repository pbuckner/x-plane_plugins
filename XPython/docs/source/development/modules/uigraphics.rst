XPUIGraphics
============
.. py:module:: XPUIGraphics
.. py:currentmodule:: xp               

To use::

  import xp

Functions
---------

.. py:function:: drawWindow(left, bottom, right, top, style=1)

 :param int left:
 :param int bottom:
 :param int right:
 :param int top: dimensions of window
 :param int style: :ref:`XPWindowStyle`

 Draw a window of the given dimensions at the given offset on
 the virtual screen in a given style. The window is automatically scaled as
 appropriate using a bitmap scaling technique (scaling or repeating) as
 appropriate to the style. Window styles are :ref:`XPWindowStyle`.

 Unlike drawing widgets, :py:func:`drawWindow` needs to be done within a draw callback,
 so that it gets drawn *every frame*.

 There is no "handle" to this window: you stop drawing it and it goes away.

 .. note:: Laminar documentation lists parameters as (x1, y1, x2, y2, style).
           For consistency with widget and window routines, I'm using
           (left, bottom, right, top). Yes, other routines use a different ordering,
           but at least we'll remain consistent on nomenclature.)

 >>> def MyDraw(phase, before, refCon):
 ...    xp.drawWindow(100, 100, 300, 200, style=refCon)
 ...    return 1
 ...    
 >>> xp.registerDrawCallback(MyDraw, refCon=1)

 .. image:: /images/drawwindow-1.png
 
 `Official SDK <https://developer.x-plane.com/sdk/XPUIGraphics/#XPDrawWindow>`__ :index:`XPDrawWindow`
 
.. py:function:: getWindowDefaultDimensions(style)

 :param int style: :ref:`XPWindowStyle`
 :return: Tuple (width: int, height: int)

 This routine returns the default dimensions (width, height) for a window, given its *style* (:ref:`XPWindowStyle`).
 Output is either
 a minimum or fixed value depending on whether the window is scalable.

 >>> xp.getWindowDefaultDimensions(xp.Window_MainWindow)
 (100, 100)

 [Since you'll specify the window size via :func:`drawWindow` and all of them are scalable, this function
 seems extraneous.]

 `Official SDK <https://developer.x-plane.com/sdk/XPUIGraphics/#XPGetWindowDefaultDimensions>`__ :index:`XPGetWindowDefaultDimensions`
 
.. py:function:: drawElement(left, bottom, right, top, style, lit=0)

 :param int left:
 :param int bottom:
 :param int right:
 :param int top: dimensions of element
 :param int style: :ref:`XPElementStyle`
 :param int lit: 1= draw "lit" version of element.

 Draws a given element at an offset on the virtual screen in
 set dimensions. EVEN if the element is not scalable, it will be scaled if
 the width and height do not match the preferred dimensions; it'll just look
 ugly. Pass lit=1 to see the lit version of the element; if the element
 cannot be lit this is ignored.

 *style* is one of :ref:`XPElementStyle`.

 .. warning:: Not all element styles are drawn correctly. Bug has been filed
    with Laminar 17-November-2021.

 >>> def MyDraw(phase, before, refCon):
 ...    (width, height, canBeLit) = xp.getElementDefaultDimensions(style=refCon['element'])
 ...    xp.drawElement(10, 10, 10+width, 10+height, style=refCon['element'])
 ...    if canBeLit:
 ...       xp.drawElement(10 + width + 20, 10, 10 + width + width + 20, 10+height, style=refCon['element'], lit=1)
 ...    return 1
 ...    
 >>> refCon = {'element': xp.Element_Airport}
 >>> xp.registerDrawCallback(MyDraw, refCon=refCon)
 >>> refCon['element'] = xp.Element_VORWithCompassRose
 >>> xp.unregisterDrawCallback(MyDraw, refCon=refCon)

 .. image:: /images/drawelement-29.png

 `Official SDK <https://developer.x-plane.com/sdk/XPUIGraphics/#XPDrawElement>`__ :index:`XPDrawElement`
 
.. py:function:: getElementDefaultDimensions(style)

 :param XPElementStyle style: :ref:`XPElementStyle`
 :return: Tuple[width: int, height: int, canBeLit: int]                             

 Returns the recommended or minimum dimensions of a given UI
 element style :ref:`XPElementStyle` as a tuple (width, height, canBeLit).

 canBeLit tells whether the element has both a lit and unlit
 state.

 >>> xp.getElementDefaultDimensions(xp.Element_Airport)
 (15, 15, 0)
 
 `Official SDK <https://developer.x-plane.com/sdk/XPUIGraphics/#XPGetElementDefaultDimensions>`__ :index:`XPGetElementDefaultDimensions`

.. py:function:: drawTrack(left, bottom, right, top, minValue, maxValue, value, style, lit=0) -> None:

 :param int left:
 :param int bottom:
 :param int right:
 :param int top: dimensions of track element
 :param int minValue: 
 :param int maxValue: range of the track
 :param int value: placement of thumb
 :param style: :ref:`XPTrackStyle`
 :param int lit: 1= hightlight track
  
 This routine draws a track. You pass in the track dimensions, range of possible values,
 initial *value*, *style* (:ref:`XPTrackStyle`) and if it should be *lit*.
 The orientation (horizontal vs. vertical) will be determined based on dimensions.

 >>> def MyDraw(phase, before, refCon):
 ...     xp.drawTrack(10, 100, 30, 300, refCon['minValue'], refCon['maxValue'], 25, style=refCon['style'])
 ...     xp.drawTrack(60, 100, 80, 300, refCon['minValue'], refCon['maxValue'], 25, style=refCon['style'], lit=1)
 ...     xp.drawTrack(10, 70, 210, 90,  refCon['minValue'], refCon['maxValue'], 25, style=refCon['style'])
 ...     xp.drawTrack(10, 40, 210, 60,  refCon['minValue'], refCon['maxValue'], 25, style=refCon['style'], lit=1)
 ...     return 1
 ...
 >>> refCon = {'minValue': 0, 'maxValue': 100, 'style': xp.Track_ScrollBar}
 >>> xp.registerDrawCallback(MyDraw, refCon=refCon)

 .. image:: /images/track_scrollbar.png
 
 .. note:: "Drawing" does not manage clicks, so you'll still have to do that by responding
    to either ``MouseDown`` window events, or ``Msg_MouseDown`` widget events (if this track
    is placed within a widget.) See example code in ``XPListBox.py``, available
    under ``Resources/plugins/XPPython3``.

 `Official SDK <https://developer.x-plane.com/sdk/XPUIGraphics/#XPDrawTrack>`__ :index:`XPDrawTrack`

.. py:function:: getTrackDefaultDimensions(style)

 :param style: :ref:`XPTrackStyle`
 :return: Tuple[width: int, canBeLit: int]

 This routine returns a track's *default smaller dimension* as a tuple ``(width, canBeLit)``.

 All tracks are scalable in the larger dimension. It also returns whether a track can be
 lit.

 >>> xp.getTrackDefaultDimensions(xp.Track_ScrollBar)
 (11, 1)
 
 `Official SDK <https://developer.x-plane.com/sdk/XPUIGraphics/#XPGetTrackDefaultDimensions>`__ :index:`XPGetTrackDefaultDimensions`

.. py:function:: getTrackMetrics(left, bottom, right, top, minValue, maxValue, value, style)

 :param int left:
 :param int bottom:
 :param int right:
 :param int top: dimensions of track element
 :param int minValue: 
 :param int maxValue: range of the track
 :param int value: placement of thumb
 :param style: :ref:`XPTrackStyle`
 :return: TrackMetrics instance              

 Returns a structure with metrics for the given track.
 If you want to write UI code
 to manipulate a track, this routine helps you know where the mouse
 locations are. For most other elements, the rectangle the element is drawn
 in is enough information. However, the track drawing routine does some
 automatic placement; this routine lets you know where things ended up. You
 pass almost everything you would pass to the draw routine. You get out the
 orientation, and other useful stuff as an object with the following attributes:

   | .isVertical: 1= true
   | .downBtnSize: int       
   | .downPageSize: int       
   | .thumbSize: int
   | .upPageSize: int
   | .upBtnSize: int

 Besides orientation, you get five dimensions for the five parts of a
 scrollbar, which are the down button, down area (area before the thumb),
 the thumb, and the up area and button. For horizontal scrollers, the left
 button decreases; for vertical scrollers, the top button decreases.

 (Tracks don't have to be drawn to retrieve this information.)
 
 >>> info = xp.getTrackMetrics(10, 100, 30, 300, 0, 100, 25, xp.Track_ScrollBar)
 >>> info.isVertical
 1
 >>> print(info)
 <TrackMetrics {isVertical: 1, downBtnSize: 14, downPageSize: 39, thumbSize: 16, upPageSize:118, upBtnSize: 13}>

 Currently (XP12) widget button sizes don't change based on size of track or contents. This
 means the down button, up button and thumb always remain the same. The down page and up page sizes reflect the "space" between
 the up/down button and the thumb. Summing the five metrics will always equal the longer dimension of the track.
 
 `Official SDK <https://developer.x-plane.com/sdk/XPUIGraphics/#XPGetTrackMetrics>`__ :index:`XPGetTrackMetrics` 

Constants
---------

.. _XPWindowStyle:

XPWindowStyle
*************

There are a few built-in window styles in X-Plane that you can use.

.. table::
   :align: left

   +--------------------------------+------------------------------------+
   |WindowStyle                     |Example                             |
   +================================+====================================+
   | .. py:data:: Window_Help       |.. image:: /images/drawwindow-0.png |
   |  :value: 0                     |                                    |
   |                                |                                    |
   | An LCD screen that shows help. |                                    |
   +--------------------------------+------------------------------------+
   | .. py:data:: Window_MainWindow |.. image:: /images/drawwindow-1.png |
   |  :value: 1                     |                                    |
   |                                |                                    |
   | A dialog box window.           |                                    |
   +--------------------------------+------------------------------------+
   | .. py:data:: Window_SubWindow  |.. image:: /images/drawwindow-2.png |
   |  :value: 2                     |                                    |
   |                                |                                    |
   | A panel or frame within a      |                                    |
   | dialog box window.             |                                    |
   +--------------------------------+------------------------------------+
   | .. py:data:: Window_Screen     |.. image:: /images/drawwindow-4.png |
   |  :value: 4                     |                                    |
   |                                |                                    |
   | An LCD screen within a panel to|                                    |
   | hold text displays.            |                                    |
   |                                |                                    |
   |                                |                                    |
   +--------------------------------+------------------------------------+
   | .. py:data:: Window_ListView   |.. image:: /images/drawwindow-5.png |
   |  :value: 5                     |                                    |
   |                                |                                    |
   | A list view within a panel for |                                    |
   | scrolling file names, etc.     |                                    |
   +--------------------------------+------------------------------------+
   

.. _XPElementStyle:

XPElementStyle
**************

Elements are individually drawable UI things like push buttons, etc. The
style defines what kind of element you are drawing. Elements can be
stretched in one or two dimensions (depending on the element). Some
elements can be lit.

 .. table::
    :align: left

    ================================================ ==== ============= ===================    
    Element                                               Scalable Axes Required background
    ================================================ ==== ============= ===================    
    .. py:data:: Element_TextField                   =6   x             metal
    .. py:data:: Element_CheckBox                    =9   none          metal
    .. py:data:: Element_CheckBoxLit                 =10  none          metal
    .. py:data:: Element_WindowCloseBox              =14  none          window header
    .. py:data:: Element_WindowCloseBoxPressed       =15  none          window header
    .. py:data:: Element_PushButton                  =16  x             metal
    .. py:data:: Element_PushButtonLit               =17  x             metal
    .. py:data:: Element_OilPlatform                 =24  none          any
    .. py:data:: Element_OilPlatformSmall            =25  none          any
    .. py:data:: Element_Ship                        =26  none          any
    .. py:data:: Element_ILSGlideScope               =27  none          any
    .. py:data:: Element_MarkerLeft                  =28  none          any
    .. py:data:: Element_Airport                     =29  none          any
    .. py:data:: Element_Waypoint                    =30  none          any
    .. py:data:: Element_NDB                         =31  none          any
    .. py:data:: Element_VOR                         =32  none          any
    .. py:data:: Element_RadioTower                  =33  none          any
    .. py:data:: Element_AircraftCarrier             =34  none          any
    .. py:data:: Element_Fire                        =35  none          any
    .. py:data:: Element_MarkerRight                 =36  none          any
    .. py:data:: Element_CustomObject                =37  none          any
    .. py:data:: Element_CoolingTower                =38  none          any
    .. py:data:: Element_SmokeStack                  =39  none          any
    .. py:data:: Element_Building                    =40  none          any
    .. py:data:: Element_PowerLine                   =41  none          any
    .. py:data:: Element_CopyButtons                 =45  none          metal
    .. py:data:: Element_CopyButtonsWithEditingGrid  =46  none          metal
    .. py:data:: Element_EditingGrid                 =47  x, y          metal 
    .. py:data:: Element_ScrollBar                   =48                THIS CAN PROBABLY BE REMOVED
    .. py:data:: Element_VORWithCompassRose          =49  none          any
    .. py:data:: Element_Zoomer                      =51  none          metal
    .. py:data:: Element_TextFieldMiddle             =52  x, y          metal
    .. py:data:: Element_LittleDownArrow             =53  none          metal
    .. py:data:: Element_LittleUpArrow               =54  none          metal
    .. py:data:: Element_WindowDragBar               =61  none          metal
    .. py:data:: Element_WindowDragBarSmooth         =62  none          metal
    ================================================ ==== ============= ===================    

 .. warning::  Not all elements draw correctly with XP 11.55+. Bug has been filed with Laminar
    17-November-2021.




.. _XPTrackStyle:

XPTrackStyle
************

A track is a UI element that displays a value vertically or horizontally.
X-Plane has three kinds of tracks: scroll bars, sliders, and progress bars.
Tracks can be displayed either horizontally or vertically; tracks will
choose their own layout based on the larger dimension of their dimensions
(e.g. they know if they are tall or wide). Sliders may be lit or unlit
(showing the user manipulating them).

+----------------------------------------------------------------+-----------------------------------------------+-----------------------------------------------+
|TrackStyle                                                      |Example unlit /lit,                            |Flipped,                                       |
|                                                                |                                               |                                               |
|                                                                |Vertical / Horizontal                          |minValue > maxValue                            |
+================================================================+===============================================+===============================================+
|.. py:data:: Track_ScrollBar                                    |.. image:: /images/track_scrollbar.png         |.. image:: /images/track_scrollbar_flipped.png |
|  :value: 0                                                     |                                               |                                               |
|                                                                |                                               |                                               |
|ScrollBar - this is a standard scroll bar with arrows and a     |                                               |                                               |
|thumb to drag.                                                  |                                               |                                               |
|                                                                |                                               |                                               |                 
|The longer dimension will scale, but the shorter dimension will |                                               |                                               |
|always display the same (e.g., you can make the bar longer, but |                                               |                                               |
|you cannot make it fatter.)                                     |                                               |                                               |
|                                                                |                                               |                                               |
|                                                                |                                               |                                               |
|                                                                |                                               |                                               |
+----------------------------------------------------------------+-----------------------------------------------+-----------------------------------------------+
|.. py:data:: Track_Slider                                       |.. image:: /images/track_slider.png            |.. image:: /images/track_slider_flipped.png    |
|  :value: 1                                                     |                                               |                                               |
|                                                                |                                               |                                               |
|Slider - this is a simple track with a ball in the middle that  |                                               |                                               |
|can be slid.                                                    |                                               |                                               |
|                                                                |                                               |                                               |
+----------------------------------------------------------------+-----------------------------------------------+-----------------------------------------------+
|.. py:data:: Track_Progress                                     |.. image:: /images/track_progress.png          |.. image:: /images/track_progress_flipped.png  |
|  :value: 2                                                     |                                               |                                               |
|                                                                |                                               |                                               |
|Progress - this is a progress indicator showing how a long task |                                               |                                               |
|is going.                                                       |                                               |                                               |
|                                                                |                                               |                                               |
+----------------------------------------------------------------+-----------------------------------------------+-----------------------------------------------+
 

 



