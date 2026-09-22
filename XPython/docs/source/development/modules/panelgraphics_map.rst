Map Display
===========

.. py:module:: XPLMPanelGraphics
    :no-index:
.. py:currentmodule:: xp

To use::

  import xp

A base map display renders an navigation display (ND) or multi-function display (MFD) style background,
(e.g., terrain, topography, water, NEXRAD weather, EGPWS ground proximity, and taxi layouts) with a
projection centered near the map's datum.
Create one, then draw it each frame
from a panel-graphics drawing context --- see :doc:`panelgraphics`. The *layers*
argument is a bitwise OR of the ``Map_*`` constants.

.. note:: Available only with SDK440, X-Plane 12.4.4+
          
See ``samples/PI_PGMapLayers.py`` for a working example: it draws one map view
in a pop-out avionics device, with SPACE cycling the layers and TAB stepping
through the ``dataOverrides`` fields.

In its simplest form, you can create a map with a few lines of code:

>>> def myScreen(refCon):
...     xp.mapDisplayDrawIn(refCon, xp.Map_Topo, 0, 200, 300, 0)
...
>>> def myBezel(r, g, b, refCon):
...     xp.polygon(xp.makeColor(0, 0, 0, 1), [(0, 0), (0, 200), (300, 200), (300, 0)])
...
>>> mapRef = xp.createMapDisplay()
>>> avionicsID = xp.createAvionicsEx(300, 200, 300, 200, 0, 0,
...                 screenDraw=myScreen, bezelDraw=myBezel,
...                 contentType=xp.WindowContentTypePanelGraphics,
...                 refCon=mapRef)
...
>>> xp.setAvionicsPopupVisible(avionicsID)

.. image:: /images/panel_map_basic.png
           :width: 300px
           :align: center

Functions
---------

.. py:function:: createMapDisplay(pilotIndex=0) -> XPLMMapDisplayRef

    :param int pilotIndex: 0 for pilot-side GPS position, 1 for copilot
    :return: A :class:`XPLMMapDisplayRef` display handle

    Create a base map display. Draw it with :func:`mapDisplayDrawIn` and free it
    with :func:`destroyMapDisplay`. You'll select size, features, and any overrides
    using :func:`mapDisplayDrawIn`.

    Setting ``pilotIndex`` allows for displaying (potentially) different flight
    paths (e.g. active vs alternate), for aircraft configured with multiple displays.

    >>> mapRef = xp.createMapDisplay()

    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMCreateMapDisplay>`__ :index:`XPLMCreateMapDisplay`

.. py:function:: destroyMapDisplay(map) -> None

    :param XPLMMapDisplayRef map: Handle from :func:`createMapDisplay`

    Destroy a map display and free its resources. The handle must not be used
    after this call.

    >>> xp.destroyMapDisplay(mapRef)

    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMDestroyMapDisplay>`__ :index:`XPLMDestroyMapDisplay`

.. py:function:: mapDisplayDrawIn(map, layers, left, top, right, bottom, dataOverrides=None) -> None

    :param XPLMMapDisplayRef map: Handle from :func:`createMapDisplay`
    :param int layers: Bitwise OR of ``Map_*`` flags to enable for this draw
    :param int left: Left edge in panel coordinates
    :param int top: Top edge
    :param int right: Right edge
    :param int bottom: Bottom edge
    :param dataOverrides: None for live sim state, or a sequence of 12 or 15 values (see below)
    :type dataOverrides: Optional[Sequence[float]]

    Render the base map into the panel within ``(left, top, right, bottom)``. Must
    be called from an avionics drawing callback; does nothing until terrain tiles
    finish loading. Some *layers* are mutually exclusive --- e.g. :data:`Map_Nexrad`
    with :data:`Map_EGPWS` or :data:`Map_IR`.

    Initially (that is, without overrides), map is drawn with aircraft in the center, track up (not North up).

    *dataOverrides*, if given, is a sequence of **12 or 15** values in order:

    +-----------------+----------------+-------------------------------------------------------------------+
    | dataLat         | float          | Degrees latitude, longitude                                       |
    | dataLon         |                |                                                                   |
    +-----------------+----------------+-------------------------------------------------------------------+
    | centerX,        | int            | map center (x, y) coordinate (pixels). Uses same panel coordinates|
    | centerY         |                | as the rectangle in MapDrawInfo. **Not** relative to that         |
    |                 |                | rectangle. This is the point the maps is centered on and the point|
    |                 |                | it rotates about. For a map centered in its own rectangle it is   |
    |                 |                | ((left+right)/2, (bottom+top)/2). It is also the same space       |
    |                 |                | :func:`mapdisplayProject` reports positions in, so you can put a  |
    |                 |                | symbol on the map without offsetting anything yourself.           |
    |                 |                |                                                                   |
    |                 |                | The center need not be the rectangle's midpoint, and may sit on or|
    |                 |                | outside its edge: pusing it down towards the bottom edge puts more|
    |                 |                | of the map ahead of theaircraft, which is how an EFIS arc mode is |
    |                 |                | laid out.                                                         |
    +-----------------+----------------+-------------------------------------------------------------------+
    | roseRadius      | int            | pixels from maps center to compass rose                           |
    +-----------------+----------------+-------------------------------------------------------------------+
    | mapRange        | float          | nautical miles, distance from map center to compass rose          |
    +-----------------+----------------+-------------------------------------------------------------------+
    | orientation     | int            | 0=North Up, 1=Track Up, 2 = Hdg up, 3=Custom                      |                   
    |                 |                | (use ``trueRotation``)                                            |                       
    +-----------------+----------------+-------------------------------------------------------------------+
    | terrainWarn     | float          | red altitude height in feet                                       |
    +-----------------+----------------+-------------------------------------------------------------------+
    | terrainCaution  | float          | yellow altitude height in feet                                    |
    +-----------------+----------------+-------------------------------------------------------------------+
    | acfAlt          | float          | aircraft altitude (ft)                                            |
    +-----------------+----------------+-------------------------------------------------------------------+
    | gearDown        | int            | 1=gear down                                                       |
    +-----------------+----------------+-------------------------------------------------------------------+
    | trueRotation    | float          | If map orientation is custom rotate map degrees                   |
    |                 |                | counterclockwise: The true heading that points **up**, so 90 puts |
    |                 |                | east at the top and true north to the left.                       |
    +-----------------+----------------+-------------------------------------------------------------------+
    | nearestRwyElev  | float          | altitude in feet of nearest runway, used for EGPWS terrain display|
    +-----------------+----------------+-------------------------------------------------------------------+
    | egpwsBrightness | float          | brightness of EGPWS overlay [0.0 .. 1.0]                          |
    +-----------------+----------------+-------------------------------------------------------------------+
    | egpwsStyle      | XPLMEGPWSStyle | 0=EGPWS_Style_Blocky, 1=EGPWS_Style_Smooth                        |
    +-----------------+----------------+-------------------------------------------------------------------+

    The last three (EGPWS) are optional; pass 12
    values to leave the optional ones set to 0. Pass None instead of the tuple to use live simulator state.

    *roseRadius* and *mapRange* are a matching pair: *roseRadius* is the distance
    from the center of the map out to the compass rose in **pixels**, and *mapRange*
    is that same distance in **nautical miles** --- so ``mapRange=40`` puts the rose
    edge 40 nm from the aircraft, like the range knob on a real EFIS, and a centered
    rose spans 80 nm across. (To be clear, an actual compass rose is not drawn by
    X-Plane.)

    The map uses a transverse Mercator projection centered near the map's datum.

    .. warning:: You cannot use :func:`mapDisplayDrawIn` in a regular Window, it may only
                 be used for Avionics Device windows *with* :data:`WindowContentTypePanelGraphics`.
                 
    .. warning:: You cannot use :func:`mapDisplayDrawIn` in an active rotation transformation (:func:`transformRotate`),
                 even if rotation is 0. You may use scale or translation transformations.

    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMMapDisplayDrawIn>`__ :index:`XPLMMapDisplayDrawIn`

.. py:function:: mapDisplayGetTerrainAltitudes(map) -> Tuple[float, float]

    :param XPLMMapDisplayRef map: Handle from :func:`createMapDisplay`
    :return: ``(minAltitude, maxAltitude)`` in feet, or None

    Return the lowest and highest altitude ``(min, max)`` shown on the map's EGPWS terrain
    display. Altitudes are only available if the map was drawn with the
    :data:`Map_EGPWS` layer; otherwise returns None. Must be called from an
    avionics drawing callback.

    >>> def myScreen(refCon):
    ...     xp.mapDisplayDrawIn(refCon, xp.Map_EGPWS, 0, 200, 300, 0)
    ...     terrainAlts = xp.mapDisplayGetTerrainAltitudes(refCon)
    ...     if terrainAlts is not None:
    ...         xp.log(f"min: {terrainAlts[0]}, max: {terrainAlts[1]}")
    ...


    .. image:: /images/panel_map_basic_egpws.png
               :width: 300px

    ::

       min: 400.0, max: 6961.832
       min: 400.0, max: 6961.832
       min: 400.0, max: 6961.832

    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMMapDisplayGetTerrainAltitudes>`__ :index:`XPLMMapDisplayGetTerrainAltitudes`

Projection
----------

The projection routines take **the same** *map*, *layers*, rectangle and
*dataOverrides* you draw with, so the projection you query is provably the
projection you drew --- your symbology cannot end up a frame or a zoom step out
of step with the terrain under it. Unlike :func:`mapDisplayDrawIn`, none of
these has to be called from a drawing callback; a click handler or flight loop
is equally valid.

The usual pattern is: project your symbols, draw the map, then draw the symbols
on top. Nothing needs to be cached between frames.

.. py:function:: mapDisplayProject(map, layers, left, top, right, bottom, latitude, longitude, dataOverrides=None) -> Tuple[float, float]

    :param XPLMMapDisplayRef map: Handle from :func:`createMapDisplay`
    :param int layers: Bitwise OR of ``Map_*`` flags
    :param int left: Left edge in panel coordinates
    :param int top: Top edge
    :param int right: Right edge
    :param int bottom: Bottom edge
    :param float latitude: Latitude in degrees
    :param float longitude: Longitude in degrees
    :param dataOverrides: None for live sim state, or a sequence of 12 or 15 values (see :func:`mapDisplayDrawIn`)
    :type dataOverrides: Optional[Sequence[float]]
    :return: ``(x, y)`` in panel coordinates, or None

    Convert a latitude/longitude into a position in panel coordinates. This is the
    inverse of :func:`mapDisplayUnproject`.

    Returns None if the map's terrain tiles have not loaded yet, or if the point has
    no position on this map.

    With the following example, we place a magenta square at the aircraft's current position by redefining
    ``myScreen`` draw callback. Get current position based on datarefs, project, then draw based on
    resulting ``(x, y)`` point.
    
    >>> lat_df = xp.findDataRef('sim/flightmodel/position/latitude')
    >>> lon_df = xp.findDataRef('sim/flightmodel/position/longitude')
    >>> def myScreen(refCon):
    ...     lat = xp.getDataf(lat_df)
    ...     lon = xp.getDataf(lon_df)
    ...     location = xp.mapDisplayProject(refCon, xp.Map_Topo, 0, 200, 300, 0, lat, lon)
    ...     xp.mapDisplayDrawIn(refCon, xp.Map_Topo, 0, 200, 300, 0)
    ...     if location:
    ...        x, y = location
    ...        xp.polygon(xp.makeColor(1, 0, 1, 1), [(x-5, y-5), (x-5, y+5), (x+5, y+5), (x+5, y-5)])
    ...

    .. image:: /images/panel_map_project.png
               :width: 300px

    .. note::
       The returned coordinates are in the same space as the rectangle you passed,
       and like that rectangle they do **not** account for the panel graphics
       transform stack.

    This does not have to be called within a draw callback, but the map (XPLMMapDisplayRef) must
    already be created.

    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMMapDisplayProject>`__ :index:`XPLMMapDisplayProject`

.. py:function:: mapDisplayUnproject(map, layers, left, top, right, bottom, x, y, dataOverrides=None) -> Tuple[float, float]

    :param XPLMMapDisplayRef map: Handle from :func:`createMapDisplay`
    :param int layers: Bitwise OR of ``Map_*`` flags
    :param int left: Left edge in panel coordinates
    :param int top: Top edge
    :param int right: Right edge
    :param int bottom: Bottom edge
    :param float x: X in panel coordinates
    :param float y: Y in panel coordinates
    :param dataOverrides: None for live sim state, or a sequence of 12 or 15 values
    :type dataOverrides: Optional[Sequence[float]]
    :return: ``(latitude, longitude)`` in degrees, or None

    Convert a position in panel coordinates back into a latitude/longitude. This is
    the inverse of :func:`mapDisplayProject`. Use it to turn a touch or click on your
    map into a place in the world --- picking a waypoint, or reading out the position
    under the cursor.

    >>> lat = xp.getDataf(lat_df)
    >>> lon = xp.getDataf(lon_df)
    >>> lat, lon
    (39.89558792114258, -104.69609832763672)
    >>> location = xp.mapDisplayProject(mapRef, xp.Map_Topo, 0, 200, 300, 0, lat, lon)
    >>> location
    (150.0, 100.0)
    >>> xp.mapDisplayUnproject(mapRef, xp.Map_Topo, 0, 200, 300, 0, location[0], location[1])
    (39.89558792114258, -104.69609832763672)

    Returns None if the map's terrain tiles have not loaded yet, or if the point does
    not correspond to anywhere on the earth.

    This does not have to be called within a draw callback, but the map (XPLMMapDisplayRef) must
    already be created.

    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMMapDisplayUnproject>`__ :index:`XPLMMapDisplayUnproject`

.. py:function:: mapDisplayScaleMeter(map, layers, left, top, right, bottom, x, y, dataOverrides=None) -> float

    :param XPLMMapDisplayRef map: Handle from :func:`createMapDisplay`
    :param int layers: Bitwise OR of ``Map_*`` flags
    :param int left: Left edge in panel coordinates
    :param int top: Top edge
    :param int right: Right edge
    :param int bottom: Bottom edge
    :param float x: X in panel coordinates
    :param float y: Y in panel coordinates
    :param dataOverrides: None for live sim state, or a sequence of 12 or 15 values
    :type dataOverrides: Optional[Sequence[float]]
    :return: Pixels per meter at that point

    Return how many pixels correspond to one meter at the given point. Use it to size
    symbols and range rings so they stay correct as the range changes.

    Returns 0.0 if the map's terrain tiles have not loaded yet.

    This does not have to be called within a draw callback, but the map (XPLMMapDisplayRef) must
    already be created.

    >>> xp.mapDisplayScaleMeter(mapRef, xp.Map_Topo, 0, 200, 300, 0, x, y)
    0.0027

    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMMapDisplayScaleMeter>`__ :index:`XPLMMapDisplayScaleMeter`

.. py:function:: mapDisplayGetNorthHeading(map, layers, left, top, right, bottom, x, y, dataOverrides=None) -> float

    :param XPLMMapDisplayRef map: Handle from :func:`createMapDisplay`
    :param int layers: Bitwise OR of ``Map_*`` flags
    :param int left: Left edge in panel coordinates
    :param int top: Top edge
    :param int right: Right edge
    :param int bottom: Bottom edge
    :param float x: X in panel coordinates
    :param float y: Y in panel coordinates
    :param dataOverrides: None for live sim state, or a sequence of 12 or 15 values
    :type dataOverrides: Optional[Sequence[float]]
    :return: Degrees clockwise from straight up on the display

    Return the heading at which true north lies at the given point. **Add** it to a
    true heading to get the angle to draw that heading at.

    This accounts both for the map's own rotation --- a heading-up map is turned to
    put the aircraft's nose at the top --- and for the projection's convergence, which
    tilts north away from vertical as you move away from the map's center. (That is "vertical" from the center
    of the map is usually slightly different from *vertical* at the left and right edges of the map. See
    related issue with the built-in map's :func:`mapGetNorthHeading`.

    Returns 0.0 if the map's terrain tiles have not loaded yet.

    This does not have to be called within a draw callback, but the map (XPLMMapDisplayRef) must
    already be created.

    >>> xp.mapDisplayGetNorthHeading(mapRef, xp.Map_Topo, 0, 200, 300, 0, x, y)
    179.4768

    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMMapDisplayGetNorthHeading>`__ :index:`XPLMMapDisplayGetNorthHeading`

Constants
---------

.. _XPLMMapLayers:

XPLMMapLayers
*************

Layers enabled on a map display, combined with bitwise OR.

.. py:data:: Map_Nexrad
   :value: 1

   .. image:: /images/panel_map_nexrad.png
              :width: 400px

   NEXRAD weather. `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#xplm_Map_Nexrad>`__: :index:`xplm_Map_Nexrad`

.. py:data:: Map_IR
   :value: 2

   .. image:: /images/panel_map_ir.png
              :width: 400px

   Infrared satellite. `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#xplm_Map_IR>`__: :index:`xplm_Map_IR`

.. py:data:: Map_Topo
   :value: 4

   .. image:: /images/panel_map_topo.png
              :width: 400px

   Topography. `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#xplm_Map_Topo>`__: :index:`xplm_Map_Topo`

.. py:data:: Map_Terrain
   :value: 8

   .. image:: /images/panel_map_terrain.png
              :width: 400px

   Terrain Warning. `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#xplm_Map_Terrain>`__: :index:`xplm_Map_Terrain`

.. py:data:: Map_Water
   :value: 16

   .. image:: /images/panel_map_water.png
              :width: 400px

   Water. `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#xplm_Map_Water>`__: :index:`xplm_Map_Water`

.. py:data:: Map_EGPWS
   :value: 32

   EGPWS terrain-warning overlay. Required for :func:`mapDisplayGetTerrainAltitudes`.

   .. image:: /images/panel_map_egpws.png
              :width: 400px

   `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#xplm_Map_EGPWS>`__: :index:`xplm_Map_EGPWS`

.. py:data:: Map_raw_elev
   :value: 64

   .. image:: /images/panel_map_elev.png
              :width: 400px

   Raw elevation. `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#xplm_Map_raw_elev>`__: :index:`xplm_Map_raw_elev`

.. py:data:: Map_safe_taxi
   :value: 128

   .. image:: /images/panel_map_taxi.png
              :width: 400px

   Airport runway and taxiway layouts.
   `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#xplm_Map_safe_taxi>`__: :index:`xplm_Map_safe_taxi`

.. _XPLMEGPWSStyle:

XPLMEGPWSStyle
**************

.. py:type:: XPLMEGPWSStyle

How the EGPWS terrain layer is rendered, used in the *egpwsStyle* field of
:func:`mapDisplayDrawIn`'s *dataOverrides*.

.. py:data:: EGPWS_Style_Blocky
   :value: 0

   Terrain drawn as small dithered blocks (common in most airliner avionics).
   `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#xplm_EGPWS_Style_Blocky>`__: :index:`xplm_EGPWS_Style_Blocky`

.. py:data:: EGPWS_Style_Smooth
   :value: 1

   Terrain contours drawn smooth and curved (common in modern avionics).
   `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#xplm_EGPWS_Style_Smooth>`__: :index:`xplm_EGPWS_Style_Smooth`

Types
-----

.. py:class:: XPLMMapDisplayRef

    Opaque capsule representing a map display, as returned by :func:`createMapDisplay`.
    Release it with :func:`destroyMapDisplay`.
