Synthetic Vision (SVT)
======================

.. py:module:: XPLMPanelGraphics
    :no-index:
.. py:currentmodule:: xp

To use::

  import xp

A Synthetic Vision (SVT) display renders a 3-D perspective view of terrain,
runways, and optional overlays into an avionics panel (new with SDK 440). Create
one, then draw it each frame from a panel-graphics drawing context. See
:doc:`panelgraphics`. You can create and destroy outside of draw callbacks, you must
be within a draw callback to execute :func:`svtDisplayDrawIn`.

Which visual layers are drawn is a property of the *draw call*, not of the display:
pass a bitwise OR of the ``SVT_*`` constants as the *features* argument of
:func:`svtDisplayDrawIn` each time you draw. There is no create-time default.

Each display manages its own terrain tile loading and GPU state, so you may create
multiple independent SVT views -- for example pilot and copilot PFDs at different
scales. SVT rendering works on any aircraft, whether or not the stock cockpit has
G1000 or other SVT-capable avionics installed.

See ``samples/PI_PGSVTFeatures.py`` for a working example: it draws one SVT view
in a pop-out avionics device, with SPACE cycling the feature flags and TAB
stepping through the ``dataOverrides`` fields.

In its simplest form, you can create SVT with a few lines of code:

>>> def myScreen(refCon):
...     xp.svtDisplayDrawIn(refCon, xp.SVT_All, 0, 200, 300, 0)
...
>>> def myBezel(r, g, b, refCon):
...     xp.polygon(xp.makeColor(0, 0, 0, 1), [(0, 0), (0, 200), (300, 200), (300, 0)])
...
>>> svtRef = xp.createSVTDisplay()
>>> avionicsID = xp.createAvionicsEx(300, 200, 300, 200, 0, 0,
...                 screenDraw=myScreen, bezelDraw=myBezel,
...                 contentType=xp.WindowContentTypePanelGraphics,
...                 refCon=svtRef)
...
>>> xp.setAvionicsPopupVisible(avionicsID)

.. image:: /images/panel_svt_basic.png
           :width: 300px
           :align: center        

Functions
---------

.. py:function:: createSVTDisplay(pilotIndex=0, pixelsPerDegree=14.0) -> XPLMSVTDisplayRef

    :param int pilotIndex: 0 for pilot-side AHRS, 1 for copilot-side AHRS
    :param float pixelsPerDegree: Vertical scale of the 3-d view, in pixels per degree
                                  at the center of the display. Must be greater than
                                  zero; the G1000 PFD uses 14.
    :return: An SVT display handle

    Create a Synthetic Vision display. Draw it with :func:`svtDisplayDrawIn` and
    free it with :func:`destroySVTDisplay`. The display begins loading terrain tiles
    for the current aircraft position immediately; until tiles are available, drawing
    it is a no-op.

    The ``pixelsPerDegree`` scale and the rectangle you pass to :func:`svtDisplayDrawIn`
    together determine the field of view: the rectangle is simply the scale applied to the
    view's angular extent. So drawing into a bigger rectangle at the same scale shows
    *more* of the world at the same magnification rather than zooming in. To zoom, you
    change the scale, not the rectangle. Pick the same scale your pitch ladder uses and the
    3-d horizon will line up with your artificial horizon.

    .. image:: /images/panel_svt_pixels.png
               :align: center
               :width: 400px

    Which visual layers are rendered is a property of the draw call -- see
    :func:`svtDisplayDrawIn`.

    >>> displayRef = xp.createSVTDisplay()
    
    When drawn, it might look like the following image:
    
    .. image:: /images/panel_svt_wide.png
          :width: 100%
               
    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMCreateSVTDisplay>`__ :index:`XPLMCreateSVTDisplay`

.. py:function:: destroySVTDisplay(svt) -> None

    :param XPLMSVTDisplayRef svt: Handle from :func:`createSVTDisplay`

    Destroy an SVT display and free its resources. The handle must not be used
    after this call. Handles are destroyed automatically when your plugin is
    unloaded.

    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMDestroySVTDisplay>`__ :index:`XPLMDestroySVTDisplay`

.. py:function:: svtDisplayDrawIn(svt, features, left, top, right, bottom, dataOverrides=None) -> None

    :param XPLMSVTDisplayRef svt: Handle from :func:`createSVTDisplay`
    :param int features: Bitwise OR of ``SVT_*`` flags to enable for this draw
    :param int left: Left edge in panel coordinates
    :param int top: Top edge
    :param int right: Right edge
    :param int bottom: Bottom edge
    :param dataOverrides: None for live sim state, or a sequence of 9 values: 8 floats and one int (see below)
    :type dataOverrides: Optional[Sequence[float]]

    Render the SVT display into the panel within ``(left, top, right, bottom)``.
    SVT sets up its own 3-d perspective projection to fit the rectangle, so you do
    not need to manipulate the transform stack. Must be called from an avionics
    drawing callback; does nothing until terrain tiles finish loading.

    ``features`` applies to *this draw call only*, so the same display can be drawn
    with different layers in different places or on different frames.

    *dataOverrides*, if given, is a sequence of exactly 9 values in order. If provided,
    you **must** provide values for all overrides:

    +-----------------+-------+-----------------------------------------------------+
    | item            | type  | dataref equivalent (?)                              |
    +=================+=======+=====================================================+
    | pitchDeg        | float | sim/flightmodel/position/theta                      |
    +-----------------+-------+-----------------------------------------------------+
    | rollDeg         | float | sim/flightmodel/position/phi                        |
    +-----------------+-------+-----------------------------------------------------+
    | headingMagDeg   | float | sim/flightmodel/position/psi                        |
    +-----------------+-------+-----------------------------------------------------+
    | magVarDeg       | float |                                                     |
    +-----------------+-------+-----------------------------------------------------+
    | indicatedAltFt  | float | sim/flightmodel/msc/h_ind                           |
    +-----------------+-------+-----------------------------------------------------+
    | baroSettingInHg | float | sim/weather/barometer_sealevel_inhg                 |
    +-----------------+-------+-----------------------------------------------------+
    | hsiSource       | int   |                                                     |
    +-----------------+-------+-----------------------------------------------------+
    | hdefDots        | float | sim/cockpit2/radios/indicators/nav1_hdef_dots_pilot |
    +-----------------+-------+-----------------------------------------------------+
    | vdefDots        | float | sim/cockpit2/radios/indicators/nav1_vdef_dots_pilot |
    +-----------------+-------+-----------------------------------------------------+
     
    These overrides allow you to change the view / angle of SVT directly, scanning up/down using ``pitchDeg`` or rolling using ``rollDeg``
    for example. The flight path and ILS guidance "hoops" can be changed using ``hdefDots`` and ``vdefDots`` to change the deflection.
    
    Pass None for ``dataOverrides`` to use live simulator state.

    .. warning:: You cannot use :func:`svtDisplayDrawIn` in a regular Window, it may only
                 be used for Avionics Device windows *with* :data:`WindowContentTypePanelGraphics`.
                 
    .. warning:: You cannot use :func:`svtDisplayDrawIn` in an active rotation transformation (:func:`transformRotate`),
                 even if rotation is 0. You may use scale or translation transformations.

    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMSVTDisplayDrawIn>`__ :index:`XPLMSVTDisplayDrawIn`

Constants
---------

.. _XPLMSVTFeatures:

XPLMSVTFeatures
***************

Layers enabled on an SVT display, combined with bitwise OR. For ``xp.py`` these
are defined without the leading ``xplm_`` e.g., ``xp.SVT_Terrain``.

.. py:data:: SVT_Terrain
   :value: 1

   .. image:: /images/panel_svt_terrain.png
              :width: 500px

   3-D terrain mesh with elevation coloring. `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#xplm_SVT_Terrain>`__: :index:`xplm_SVT_Terrain`

.. py:data:: SVT_Runways
   :value: 2

   .. image:: /images/panel_svt_runways.png
              :width: 500px

   Runway outlines, centerline stripes, and numbers. `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#xplm_SVT_Runways>`__: :index:`xplm_SVT_Runways`

.. py:data:: SVT_Obstacles
   :value: 4

   .. image:: /images/panel_svt_obstacles.png
              :width: 500px

   Obstacle markers (towers, masts, etc.). `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#xplm_SVT_Obstacles>`__: :index:`xplm_SVT_Obstacles`

.. py:data:: SVT_FlightPath
   :value: 8

   .. image:: /images/panel_svt_flightpath.png
              :width: 500px

   Flight path guidance hoops along the active route. `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#xplm_SVT_FlightPath>`__: :index:`xplm_SVT_FlightPath`

.. py:data:: SVT_Traffic
   :value: 16

   TCAS traffic symbols (symbols vary in size, shape, and color depending on risk). `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#xplm_SVT_Traffic>`__: :index:`xplm_SVT_Traffic`

   .. image:: /images/panel_svt_traffic.png
              :width: 500px
              
.. py:data:: SVT_AirportSigns
   :value: 32

   .. image:: /images/panel_svt_signs.png
              :width: 500px

   Airport identification signs near airports. `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#xplm_SVT_AirportSigns>`__: :index:`xplm_SVT_AirportSigns`

.. py:data:: SVT_ILSHoops
   :value: 64

   ILS approach guidance hoops. `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#xplm_SVT_ILSHoops>`__: :index:`xplm_SVT_ILSHoops`

.. py:data:: SVT_HorizonHeading
   :value: 128

   .. image:: /images/panel_svt_heading.png
              :width: 500px
              
   Horizon line and heading reference. `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#xplm_SVT_HorizonHeading>`__: :index:`xplm_SVT_HorizonHeading`

.. py:data:: SVT_All
   :value: 255

   .. image:: /images/panel_svt_all.png
              :width: 500px
              
   All SVT features (all bits set).
   `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#xplm_SVT_All>`__: :index:`xplm_SVT_All`

Types
-----

.. py:class:: XPLMSVTDisplayRef

    Opaque capsule representing an SVT display, as returned by :func:`createSVTDisplay`.
    Release it with :func:`destroySVTDisplay`.
