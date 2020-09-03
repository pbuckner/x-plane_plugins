XPLMMap
=======
.. py:module:: XPLMMap

To use::

  import XPLMMap

This API allows you to create new layers within X-Plane maps. Your layers
can draw arbitrary OpenGL, but they conveniently also have access to
X-Plane's built-in icon and label drawing functions.

As of X-Plane 11, map drawing happens in three stages:

1. backgrounds and "fill,"
2. icons, and
3. labels.

Thus, all background drawing gets layered beneath all icons, which likewise
get layered beneath all labels. Within each stage, the map obeys a
consistent layer ordering, such that "fill" layers (layers that cover a
large amount of map area, like the terrain and clouds) appear beneath
"markings" layers (like airport icons). This ensures that layers with fine
details don't get obscured by layers with larger details.

The XPLM map API reflects both aspects of this draw layering: you can
register a layer as providing either markings or fill, and X-Plane will
draw your fill layers beneath your markings layers (regardless of
registration order). Likewise, you are guaranteed that your layer's icons
(added from within an icon callback) will go above your layer's OpenGL
drawing, and your labels will go above your icons.

The XPLM guarantees that all plugin-created fill layers go on top of all
native X-Plane fill layers, and all plugin-created markings layers go on
top of all X-Plane markings layers (with the exception of the aircraft
icons). It also guarantees that the draw order of your own plugin's layers
will be consistent. But, for layers created by different plugins, the only
guarantee is that we will draw all of one plugin's layers of each type
(fill, then markings), then all of the others'; we don't guarantee which
plugin's fill and markings layers go on top of the other's.

As of X-Plane 11, maps use true cartographic projections for their drawing,
and different maps may use different projections. For that reason, all
drawing calls include an opaque handle for the projection you should use to
do the drawing. Any time you would draw at a particular latitude/longitude,
you'll need to ask the projection to translate that position into "map
coordinates." (Note that the projection is guaranteed not to change between
calls to your prepare-cache hook, so if you cache your map coordinates
ahead of time, there's no need to re-project them when you actually draw.)

In addition to mapping normal latitude/longitude locations into map
coordinates, the projection APIs also let you know the current heading for
north. (Since X-Plane 11 maps can rotate to match the heading of the user's
aircraft, it's not safe to assume that north is at zero degrees rotation.)

Callbacks
---------
When you create a new map layer (using :py:func:`XPLMCreateMapLayer`), you can provide
any or all of these callbacks. They allow you to insert your own OpenGL
drawing, text labels, and icons into the X-Plane map at the appropriate
places, allowing your layer to behave as similarly to X-Plane's built-in
layers as possible.

There are three "drawing" callbacks:

* :py:func:`XPLMMapDrawingCallback_f` which draws the map layer: you can use OpenGL here.
* :py:func:`XPLMMapIconDrawingCallback_f` which draws icons "above" the map layer.
* :py:func:`XPLMMapLabelDrawingCallback_f` which draws labels.


There are two "layer management" callbacks
that your map layer can receive
(if you provide the callback in your :ref:`XPLMCreateMapLayer_t`). They allow you
to manage the lifecycle of your layer, as well as cache any
computationally-intensive preparation you might need for drawing.

* :py:func:`XPLMMapPrepareCacheCallback_f` which is called when the map's bounds change.
* :py:func:`XPLMMapWillBeDeletedCallback_f` which is called just before the map gets deleted.

.. py:function:: XPLMMapDrawingCallback_f(layer, bounds, zoom, mapUnits, mapStyle, projection, refCon) -> None:

 :param layer: :ref:`XPLMMapLayerID` you created via :py:func:`XPLMCreateMapLayer`
 :param bounds: list of four floats (left, top, right, bottom) representing map bounds              
 :param float zoom: ratio of zoom (1.0 = 100%)
 :param float mapUnits: Map Units per User Interface Unit
 :param mapStyle: :ref:`XPLMMapStyle`
 :param projection: :ref:`XPLMMapProjectionID`
 :param object refCon: reference constant

 This is the OpenGL map drawing callback for plugin-created map layers. You
 can perform arbitrary OpenGL drawing from this callback, with one
 exception: changes to the Z-buffer are not permitted, and will result in
 map drawing errors.

 All drawing done from within this callback appears beneath all built-in
 X-Plane icons and labels, but above the built-in "fill" layers (layers
 providing major details, like terrain and water). Note, however, that the
 relative ordering between the drawing callbacks of different plugins is not
 guaranteed.



.. py:function:: XPLMMapIconDrawingCallback_f(layer, bounds, zoom, mapUnits, mapStyle, projection, refCon) -> None:

 :param layer: :ref:`XPLMMapLayerID` you created via :py:func:`XPLMCreateMapLayer`
 :param bounds: list of four floats (left, top, right, bottom) representing map bounds              
 :param float zoom: ratio of zoom (1.0 = 100%)
 :param float mapUnits: Map Units per User Interface Unit
 :param mapStyle: :ref:`XPLMMapStyle`
 :param projection: :ref:`XPLMMapProjectionID`
 :param object refCon: reference constant

 This is the icon drawing callback that enables plugin-created map layers to
 draw icons using X-Plane's built-in icon drawing functionality. You can
 request an arbitrary number of PNG icons to be drawn via
 :py:func:`XPLMDrawMapIconFromSheet` from within this callback, but you may not
 perform any OpenGL drawing here.

 Icons enqueued by this function will appear above all OpenGL drawing
 (performed by your optional :py:func:`XPLMMapDrawingCallback_f`), and above all
 built-in X-Plane map icons of the same layer type ("fill" or "markings," as
 determined by the :ref:`XPLMMapLayerType` in your :ref:`XPLMCreateMapLayer_t`). Note,
 however, that the relative ordering between the drawing callbacks of
 different plugins is not guaranteed.


.. py:function:: XPLMMapLabelDrawingCallback_f(layer, bounds, zoom, mapUnits, mapStyle, projection, refCon) -> None:

 :param layer: :ref:`XPLMMapLayerID` you created via :py:func:`XPLMCreateMapLayer`
 :param bounds: list of four floats (left, top, right, bottom) representing map bounds              
 :param float zoom: ratio of zoom (1.0 = 100%)
 :param float mapUnits: Map Units per User Interface Unit
 :param mapStyle: :ref:`XPLMMapStyle`
 :param projection: :ref:`XPLMMapProjectionID`
 :param object refCon: reference constant

 This is the label drawing callback that enables plugin-created map layers
 to draw text labels using X-Plane's built-in labeling functionality. You
 can request an arbitrary number of text labels to be drawn via
 :py:func:`XPLMDrawMapLabel` from within this callback, but you may not perform any
 OpenGL drawing here.

 Labels enqueued by this function will appear above all OpenGL drawing
 (performed by your optional :py:func:`XPLMMapDrawingCallback_f`), and above all
 built-in map icons and labels of the same layer type ("fill" or "markings,"
 as determined by the :ref:`XPLMMapLayerType` in your :ref:`XPLMCreateMapLayer_t`). Note,
 however, that the relative ordering between the drawing callbacks of
 different plugins is not guaranteed.


.. py:function:: XPLMMapPrepareCacheCallback_f(layer, bounds, projection, refCon) -> None:

 :param layer: :ref:`XPLMMapLayerID` you created via :py:func:`XPLMCreateMapLayer`
 :param bounds: list of four floats (left, top, right, bottom) representing map bounds              
 :param projection: :ref:`XPLMMapProjectionID`
 :param object refCon: reference constant

 A callback used to allow you to cache whatever information your layer needs
 to draw in the current map area.

 This is called each time the map's total bounds change. This is typically
 triggered by new DSFs being loaded, such that X-Plane discards old,
 now-distant DSFs and pulls in new ones. At that point, the available bounds
 of the map also change to match the new DSF area.

 By caching just the information you need to draw in this area, your future
 draw calls can be made faster, since you'll be able to simply "splat" your
 precomputed information each frame.

 We guarantee that the map projection will not change between successive
 prepare cache calls, nor will any draw call give you bounds outside these
 total map bounds. So, if you cache the projected map coordinates of all the
 items you might want to draw in the total map area, you can be guaranteed
 that no draw call will be asked to do any new work.


.. py:function:: XPLMMapWillBeDeletedCallback_f(layer, refCon) -> None:

 :param layer: :ref:`XPLMMapLayerID` you created via :py:func:`XPLMCreateMapLayer`
 :param object refCon: reference constant

 Called just before your map layer gets deleted. Because SDK-created map
 layers have the same lifetime as the X-Plane map that contains them, if the
 map gets unloaded from memory, your layer will too.



Map Layer Creation and Destruction
----------------------------------

Enables the creation of new map layers. Layers are created for a particular
instance of the X-Plane map. For instance, if you want your layer to appear
in both the normal map interface and the Instructor Operator Station (IOS),
you would need two separate calls to :py:func:`XPLMCreateMapLayer`, with two
different values for your :ref:`XPLMCreateMapLayer_t` ``layer_name``. (That's what
the documentation says -- but is that really two different ``layerName`` or two different ``mapToCreateLayerIn``?)

Your layer's lifetime will be determined by the lifetime of the map it is
created in. If the map is destroyed (on the X-Plane side), your layer will
be too, and you'll receive a callback to your :py:func:`XPLMMapWillBeDeletedCallback_f`.

.. py:function:: XPLMCreateMapLayer(params) -> XPLMMapLayerID:

 This routine creates a new map layer. You pass in an :ref:`XPLMCreateMapLayer_t`
 list with all of the fields set in.

 :param params: :ref:`XPLMCreateMapLayer_t` tuple
 :return: :ref:`XPLMMapLayerID` on success, None on failure

 Returns None if the layer creation failed. This happens most frequently
 because the map you specified in your ``mapToCreateLayerIn``
 field doesn't exist (that is, if
 :py:func:`XPLMMapExists` returns 0 for the specified map). You can use
 :py:func:`XPLMRegisterMapCreationHook` to get a notification each time a new map is
 opened in X-Plane, at which time you can create layers in it.

 .. _XPLMCreateMapLayer_t:

XPLMCreateMapLayer_t
********************

 Ten-item tuple (or list) used as input to :py:func:`XPLMCreateMapLayer`

 = ====================== ==============================
 #  Tuple field            Value
 = ====================== ==============================
 0 mapToCreateLayerIn     :ref:`XPLM Map Layer` string
 1 layerType              :ref:`XPLMMapLayerType`
 2 willBeDeletedCallback  :py:func:`XPLMMapWillBeDeletedCallback_f`
 3 prepCacheCallback      :py:func:`XPLMMapPrepareCacheCallback_f`
 4 drawCallback           :py:func:`XPLMMapDrawingCallback_f`
 5 iconCallback           :py:func:`XPLMMapIconDrawingCallback_f`
 6 labelCallback          :py:func:`XPLMMapLabelDrawingCallback_f`
 7 showUiToggle           int 1= show toggle button for this layer
 8 layerName              str to display in UI
 9 refCon                 any object
 = ====================== ==============================


.. py:function:: XPLMDestroyMapLayer(inLayer) -> int:

 Destroys a map layer you created (calling your
 :py:func:`XPLMMapWillBeDeletedCallback_f` if applicable).
 
 :param layer: :ref:`XPLMMapLayerID`
 :return: int 1= successfully deleted               

.. py:function::  XPLMMapCreatedCallback_f(mapIdentifier, refcon) -> None:

 A callback you provide, to notify your plugin that a new map has been created in
 X-Plane. This is the best time to add a custom map layer using
 :py:func:`XPLMCreateMapLayer`. Register the callback using :py:func:`XPLMRegisterMapCreationHook`.

 No OpenGL drawing is permitted within this callback.

 :param str mapIdentifier: :ref:`XPLM Map Layer` string
 :param object refcon: Reference constant you provided during :py:func:`XPLMRegisterMapCreationHook`.


.. py:function:: XPLMRegisterMapCreationHook(callback, refcon) -> None:

 Registers your :py:func:`XPLMMapCreatedCallback_f` callback to receive a
 notification each time a new map is constructed in X-Plane. This callback is
 the best time to add your custom
 map layer using :py:func:`XPLMCreateMapLayer`.

 :param callback: :py:func:`XPLMMapCreatedCallback_f` function you provide.
 :param object refcon: reference constant that will be provided to your callback.

 .. note:: You will not be notified about any maps that already exist --- you
    can use :py:func:`XPLMMapExists` to check for maps that were created previously.

.. py:function:: XPLMMapExists(mapIdentifier) -> int:

 :param str mapIdentifier: :ref:`XPLM Map Layer` string
 :return int: 1= map already exists

 Returns 1 if the map with the specified identifier already exists in
 X-Plane. In that case, you can safely call :py:func:`XPLMCreateMapLayer` specifying
 that your layer should be added to that map.


Map Drawing
-----------

These APIs are only valid from within a map drawing callback (one of
:py:func:`XPLMMapIconDrawingCallback_f` or :py:func:`XPLMMapLabelDrawingCallback_f`).
Your drawing
callbacks are registered when you create a new map layer as part of your
:ref:`XPLMCreateMapLayer_t`. The functions here hook into X-Plane's built-in map
drawing functionality for icons and labels, so that you get a consistent
style with the rest of the X-Plane map.

Note that the X-Plane 11 map introduces a strict ordering: layers of type
:data:`xplm_MapLayer_Fill` get drawn beneath all :data:`xplm_MapLayer_Markings` layers.
Likewise, all OpenGL drawing (performed in your layer's
:py:func:`XPLMMapDrawingCallback_f`) will appear beneath any icons and labels you
draw.

.. py:function:: XPLMDrawMapIconFromSheet(layer, png, s, t, ds, dt, x, y, orientation, rotate, mapWidth) -> None:

 Enables plugin-created map layers to draw PNG icons using X-Plane's
 built-in icon drawing functionality. Only valid from within an
 XPLMIconDrawingCallback_t (but you can request an arbitrary number of icons
 to be drawn from within your callback).
 
 :param layer: :ref:`XPLMMapLayerID`
 :param str png: path of the png file, relative X-Plane, e.g., "Resources/plugins/PythonPlugins/foo.png"
 :param int s:
 :param int t:
 :param int ds:
 :param int dt: s, t, ds, dt are icon coordinates within the png file (see below)
 :param float x:
 :param float y: (x, y) coordinates to draw the icon (projected Lat Lng, see :py:func:`XPLMMapProject`).
 :param int orientation: :ref:`XPLMMapOrientation`
 :param float rotate: clockwise rotation in degrees
 :param float mapWidth: width of the icon, in map units (See :py:func:`XPLMMapScaleMeter`)

 X-Plane will automatically manage the memory for your texture so that it
 only has to be loaded from disk once as long as you continue drawing it
 per-frame. (When you stop drawing it, the memory may purged in a "garbage
 collection" pass, require a load from disk in the future.)
 
 Instead of having X-Plane draw a full PNG, this method allows you to use UV
 coordinates to request a portion of the image to be drawn. This allows you
 to use a single texture load (of an icon sheet, for example) to draw many
 icons. Doing so is much more efficient than drawing a dozen different small
 PNGs.
 
 The UV coordinates used here treat the texture you load as being comprised
 of a number of identically sized "cells." You specify the width and height of the full image
 in cells (ds and dt, respectively), as well as the coordinates within the
 cell grid for the sub-image you'd like to draw.
 
 Note that you can use different ds and dt values in subsequent calls with
 the same texture sheet. This enables you to use icons of different sizes in
 the same sheet if you arrange them properly in the PNG.

 .. image:: /images/icon-map.png
 
 For example, in the image above, sub-image *E* can be identified by describing
 the full image as 3 cells x 3 cells (i.e, ds, dt = (3, 3)), and the sub-image *E*
 is in cell (2, 0).
 
 This function is only valid from within an :py:func:`XPLMIconDrawingCallback_f` (but
 you can request an arbitrary number of icons to be drawn from within your
 callback).



.. py:function:: XPLMDrawMapLabel(layer, label, x, y, orientation, rotate) -> None:

 Enables plugin-created map layers to draw text labels using X-Plane's
 built-in labeling functionality. Only valid from within an
 :py:func:`XPLMMapLabelDrawingCallback_f` (but you can request an arbitrary number of
 text labels to be drawn from within your callback).

 :param layer: :ref:`XPLMMapLayerID`
 :param str label: label to be drawn
 :param float x:
 :param float y: (x, y) coordinates to draw the icon (projected Lat Lng, see :py:func:`XPLMMapProject`).
 :param int orientation: :ref:`XPLMMapOrientation`
 :param float rotate: clockwise rotation in degrees

Map Projections
---------------

As of X-Plane 11, the map draws using true cartographic projections, and
different maps may use different projections. Thus, to draw at a particular
latitude and longitude, you must first transform your real-world
coordinates into map coordinates.

The map projection is also responsible for giving you the current scale of
the map. That is, the projection can tell you how many map units correspond
to 1 meter at a given point.

Finally, the map projection can give you the current rotation of the map.
Since X-Plane 11 maps can rotate to match the heading of the aircraft, the
map's rotation can potentially change every frame.


.. py:function:: XPLMMapProject(projection, latitude, longitude) -> (x, y)

 Projects a latitude/longitude into map coordinates. This is the inverse of
 :py:func:`XPLMMapUnproject`.

 Only valid from within a map layer callback (one of
 :py:func:`XPLMMapPrepareCacheCallback_f`, py:func:`XPLMMapDrawingCallback_f`,
 :py:func:`XPLMMapIconDrawingCallback_f`, or py:func:`XPLMMapLabelDrawingCallback_f`.)

 :param projection: :ref:`XPLMMapProjectionID` (You'll get this in the map layer callback)
 :param float latitude:
 :param float longitude: Latitude, longitude to be projected
 :return: (x, y)
 :rtype: (float, float)         

.. py:function:: XPLMMapUnproject(projection, x, y) -> (latitude, longitude)

 Transforms map coordinates back into a latitude and longitude. This is the
 inverse of :py:func:`XPLMMapProject`.

 Only valid from within a map layer callback (one of
 :py:func:`XPLMMapPrepareCacheCallback_f`, :py:func:`XPLMMapDrawingCallback_f`,
 :py:func:`XPLMMapIconDrawingCallback_f`, or :py:func:`XPLMMapLabelDrawingCallback_f`.)

 :param projection: :ref:`XPLMMapProjectionID` (You'll get this in the map layer callback)
 :param float x:
 :param float y: (x, y) to be unprojected
 :return: (latitude, longitude
 :rtype: (float, float)         


.. py:function:: XPLMMapScaleMeter(projection, x, y) -> mapUnits:

 Returns the number of map units that correspond to a distance of one meter
 at a given set of map coordinates.

 Only valid from within a map layer callback (one of
 :py:func:`XPLMMapPrepareCacheCallback_f`, :py:func:`XPLMMapDrawingCallback_f`,
 :py:func:`XPLMMapIconDrawingCallback_f`, or :py:func:`XPLMMapLabelDrawingCallback_f`.)

 :param projection: :ref:`XPLMMapProjectionID` (You'll get this in the map layer callback)
 :param float x:
 :param float y: (x, y) location on map for the calculation
 :return float mapUnits: Number of map units representing one meter of distance.


.. py:function:: XPLMMapGetNorthHeading(projection, x, y) -> heading:

 Returns the heading (in degrees clockwise from "up") that corresponds to
 north at a given point on the map. In other words, if your runway has a
 true heading of 360, you would use "north" as the Cartesian angle at which
 to draw the runway on the map. (You would add the result of
 XPLMMapGetNorthHeading() to your true heading to get the map angle.)
 
 This is necessary becuase X-Plane's map can be rotated to match your
 aircraft's orientation; north is not always "up."

 Only valid from within a map layer callback (one of
 XPLMMapPrepareCacheCallback_f, XPLMMapDrawingCallback_f,
 XPLMMapIconDrawingCallback_f, or XPLMMapLabelDrawingCallback_f.)

 :param projection: :ref:`XPLMMapProjectionID` (You'll get this in the map layer callback)
 :param float x:
 :param float y: (x, y) location on map for the calculation
 :return: Heading, degrees clockwise from "up"
 :rtype: float

 .. warning::

  The above reflects current documentation which is inaccurate.
  Documentation bug filed with Laminar 5-May-2020.

  The interface is correct, but the result is unrelated to the orientation of
  the user aircraft. Instead, this returns the "mapping angle" which is the angle
  measured clockwise from the tangent to the projection of the meridian to the
  northing coordinate line (grid north). This has typical values (for LR VFR
  sectional map projection) on the order of 0.0002 or less. Essentially it says,
  for the given map, and a given point on that map: where is true north vis-a-vis
  "up" in the projection. For Northern Hemisphere meridians curve ever-so-slightly
  inward from bottom-to-top using the Laminar map projection.

  .. image:: /images/XPLMMapGetNorthHeading.jpg

Constants
---------

.. _XPLMMapLayerID:

XPLMMapLayerID
**************

XPLMMapLayerID is an opaque handle for a plugin-created map layer.
Pass it to the map drawing APIs from an appropriate callback
to draw in the layer you created.

.. _XPLMMapProjectionID:

XPLMMapProjectionID
*******************

XPLMMapProjectionID is an opaque handle for a map projection.
Pass it to the projection APIs to translate between map coordinates
and latitude/longitudes.

.. _XPLMMapStyle:

XPLMMapStyle
************

Indicates the visual style being drawn by the map. In X-Plane, the user can
choose between a number of map types, and different map types may have use
a different visual representation for the same elements (for instance, the
visual style of the terrain layer changes drastically between the VFR and
IFR layers), or certain layers may be disabled entirely in some map types
(e.g., localizers are only visible in the IFR low-enroute style).

 .. data:: xplm_MapStyle_VFR_Sectional
 .. data:: xplm_MapStyle_IFR_LowEnroute
 .. data:: xplm_MapStyle_IFR_HighEnroute

.. _XPLMMapLayerType:

XPLMMapLayerType
****************

Indicates the type of map layer you are creating. Fill layers will always
be drawn beneath markings layers.

 .. data:: xplm_MapLayer_Fill

    A layer that draws "fill" graphics, like weather patterns, terrain, etc.
    Fill layers frequently cover a large portion of the visible map area.

 .. data:: xplm_MapLayer_Markings

    A layer that provides markings for particular map features, like NAVAIDs,
    airports, etc. Even dense markings layers cover a small portion of the
    total map area.

.. _XPLMMapOrientation:

XPLMMapOrientation
******************

Indicates whether a map element should be match its rotation to the map
itself, or to the user interface. For instance, the map itself may be
rotated such that "up" matches the user's aircraft, but you may want to
draw a text label such that it is always rotated zero degrees relative to
the user's perspective. In that case, you would have it draw with UI
orientation.

 .. data:: xplm_MapOrientation_Map

   Orient such that a 0 degree rotation matches the map's north

 .. data:: xplm_MapOrientation_UI

   Orient such that a 0 degree rotation is "up" relative to the user interface

.. _XPLM Map Layer:

XPLM Map Layer
**************

Globally unique identifiers, used as the
mapToCreateLayerIn parameter in :ref:`XPLMCreateMapLayer_t`

 .. data:: XPLM_MAP_USER_INTERFACE

  X-Plane's Map window

 .. data:: XPLM_MAP_IOS

  X-Plane's Instructor Operator Station


Example
-------

See `XPython/demos/PI_Map.py <https://github.com/pbuckner/x-plane_plugins/raw/master/XPython/demos/PI_Map.py>`_
demo for detailed example. To run, you'll need to:

#. Copy the necessary image file ``map-sample-image.png`` from
   `XPython/demos/ <https://github.com/pbuckner/x-plane_plugins/raw/master/XPython/demos/>`_ to your ``Resources/plugins``
   directory, and
#. Install OpenGL: You can do this within X-Plane by using the XPPython3 Pip Installer menu item and install the
   package ``pyopengl``.


