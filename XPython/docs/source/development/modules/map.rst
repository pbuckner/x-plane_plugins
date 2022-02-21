XPLMMap
=======
.. py:module:: XPLMMap
.. py:currentmodule:: xp

To use::

  import xp

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

The XPLMMap API reflects both aspects of this draw layering: you can
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

.. note:: This API doesn't create new map windows, it creates *layers*, which can be
     shown/hidden on the existing map window.

API consists of:

* Layer creation and destruction:

  * :py:func:`createMapLayer`, :py:func:`destroyMapLayer`

  * :py:func:`registerMapCreationHook`, :py:func:`mapExists`

* Drawing routines available *from within map drawing callback*:

  * :py:func:`drawMapIconFromSheet`, :py:func:`drawMapLabel`

* Projection and scaling utilities so your drawing gets displayed properly:

  * :py:func:`mapProject`, :py:func:`mapUnproject`

  * :py:func:`mapScaleMeter`

  * :py:func:`mapGetNorthHeading`

Map Layer Creation and Destruction
----------------------------------

Layers are created for a particular
instance of the X-Plane map. For instance, if you want your layer to appear
in both the normal map interface and the Instructor Operator Station (IOS),
you would need two separate calls to :py:func:`createMapLayer`, with two
different values for *map*.

Your layer's lifetime will be determined by the lifetime of the map it is
created in. If the map is destroyed (on the X-Plane side), your layer will
be too, and you'll receive a callback to your ``deleteLayer()``.

You can only create a layer *after* the map has been created (by X-Plane). You
can check to see if it exists (:py:func:`mapExists`) and register to be notified
when it is created (:py:func:`registerMapCreationHook`).

.. py:function:: createMapLayer(mapType=xp.MAP_USER_INTERFACE, layerType=xp.MapLayer_Markings, delete=None, prep=None, draw=None, icon=None, label=None, showToggle=1, name="", refCon=None)

 Create a new map layer, setting callback functions.

 *mapType* indicates which map type this layer applies to. Either ``xp.MAP_USER_INTERFACE``
 for the X-Plane Map Window or ``xp.MAP_IOS`` for the Instructor Operator Station.

 *layerType* is either ``xp.MapLayer_Fill``, which will draw "fill" graphics, like weather patterns, terrain, etc, or
 ``xp.MapLayer_Markings``, which provides marking for map features like NAVAIDs, airports, etc. Markings are always drawn
 over Fill layers.

 The layer's *name* will be displayed to the user within the Map Window, and if *showToggle* is 1, the user will have the
 option to show/hide the layer.

 >>> layerID = xp.createMapLayer(name="My New Layer")
 >>> layerID
 <capsule object "LayerIDRef" at 0x7fecc31d67b0>

 .. image:: /images/map_layer.png

 The remaining items are callback functions, each of which will be passed the *refCon* reference constant.
 The callback functions do not return a value.

 Parameters passed to most callback functions include:

 * *layerID*: the layer you created via :py:func:`createMapLayer`

 * *bounds*: list of four floats (left, top, right, bottom) representing map bounds              

 * *zoom*: ratio of zoom (1.0 = 100%)

 * *mapUnits*: Map Units per User Interface Unit (See :py:func:`mapScaleMeter`)

 * *mapStyle*: One of:
    .. data:: MapStyle_VFR_Sectional
      :value: 0        
    .. data:: MapStyle_IFR_LowEnroute
      :value: 1        
    .. data:: MapStyle_IFR_HighEnroute
      :value: 2        

 * *projection*: opaque handle for a map projection: Pass it to the projection APIs to translate between map coordinates and latitude/longitude

 * *refCon*: reference constant provided with :py:func:`createMapLayer`

 **Three Drawing callbacks:**
 
 .. py:function:: drawLayer(layerID, bounds, zoom, mapUnits, mapStyle, projection, refCon)
        
     ``drawLayer()`` layer is lowest. You can perform arbitrary OpenGL drawing from
     this callback, with one
     exception: changes to the Z-buffer are not permitted, and will result in
     map drawing errors.

     All drawing done from within the ``drawLayer()`` callback appears beneath all built-in
     X-Plane icons and labels, but above the built-in "fill" layers (layers
     providing major details, like terrain and water). Note, however, that the
     relative ordering between the drawing callbacks of different plugins is not
     guaranteed.

     >>> from OpenGL import GL
     >>> def drawLayer(layerID, bounds, zoom, mapUnits, mapStyle, projection, refCon):
     ...    (left, top, right, bottom) = bounds
     ...    xp.setGraphicsState(0, 0, 0, 0, 1, 1, 0)
     ...    GL.glColor3f(0, 1, 0)  # Green
     ...    GL.glLineWidth(10)
     ...    GL.glBegin(GL.GL_LINES)
     ...    GL.glVertex2f(left, top)
     ...    GL.glVertex2f(right, bottom)
     ...    GL.glLineWidth(1)
     ...    GL.glEnd()
     ...
     >>> layerID = xp.createMapLayer(name="Green Diagonal", draw=drawLayer)

     .. image:: /images/map_diagonal.png
     
 .. py:function:: iconLayer(layerID, bounds, zoom, mapUnits, mapStyle, projection, refCon)

     The ``iconLayer()`` callback enables plugin-created map layers to
     draw icons using X-Plane's built-in icon drawing functionality. You can
     request an arbitrary number of PNG icons to be drawn via
     :py:func:`drawMapIconFromSheet` from within this callback, but you may not
     perform any OpenGL drawing here.

     Icons enqueued by this function will appear above all OpenGL drawing
     (performed by your optional ``drawLayer()``), and above all
     built-in X-Plane map icons of the same layer type ("fill" or "markings," as
     determined by the *layerType* provided with :py:func:`createMapLayer`). Note,
     however, that the relative ordering between the drawing callbacks of
     different plugins is not guaranteed.

     >>> from OpenGL import GL
     >>> SAMPLE_IMG = "Resources/bitmaps/interface11/star.png"
     >>> def iconLayer(layerID, bounds, zoom, mapUnits, mapStyle, projection, refCon):
     ...    (left, top, right, bottom) = bounds
     ...    width = xp.mapScaleMeter(projection, (left + right) / 2, (top + bottom) / 2) * 1609.34 * 10
     ...    xp.drawMapIconFromSheet(layerID, SAMPLE_IMG, 0, 0, 1, 1, (left + right) / 2, (top + bottom) / 2, xp.MapOrientation_Map, 0, width)
     ...
     >>> layerID = xp.createMapLayer(name="Black Star", icon=iconLayer)

     .. image:: /images/map_star.png
                
 .. py:function:: labelLayer(layerID, bounds, zoom, mapUnits, mapStyle, projection, refCon)

     This is the label drawing callback that enables plugin-created map layers
     to draw text labels using X-Plane's built-in labeling functionality. You
     can request an arbitrary number of text labels to be drawn via
     :py:func:`drawMapLabel` from within this callback, but you may not perform any
     OpenGL drawing here.
    
     Labels enqueued by this function will appear above all OpenGL drawing
     (performed by your optional :py:func:`drawLayer`), and above all
     built-in map icons and labels of the same layer type ("fill" or "markings,"
     as determined by the *layerType* in :py:func:`createMapLayer`). Note,
     however, that the relative ordering between the drawing callbacks of
     different plugins is not guaranteed.
    
     >>> from OpenGL import GL
     >>> def labelLayer(layerID, bounds, zoom, mapUnits, mapStyle, projection, refCon):
     ...    (left, top, right, bottom) = bounds
     ...    xp.drawMapLabel(layerID, "Map Middle", (left + right) / 2, (top + bottom) / 2 , xp.MapOrientation_Map, 45)
     ...
     >>> layerID = xp.createMapLayer(name="Label Example", label=labelLayer)

     .. image:: /images/map_label.png

 **One callback whenever the maps bounds changes:**
    
 .. py:function:: prepLayer(layerID, bounds, projection, refCon)

     A callback used to allow you to cache whatever information your layer needs
     to draw in the current map area. (Because the draw callbacks will be called every frame!)

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
     
     >>> def prepLayer(layerID, bounds, projection, refCon):
     ...    xp.log(f"Bounds changed to: {bounds}")
     ...
     >>> layerID = xp.createMapLayer(name="Prep Example", prep=prepLayer)

     (Significantly change your aircraft location and your map bounds will
     change, causing the log message to be printed.)

 **One callback just before map layer is deleted:**

 .. py:function:: deleteLayer(layerID, refCon)

     Called just before your map layer gets deleted. Because SDK-created map
     layers have the same lifetime as the X-Plane map that contains them, if the
     map gets unloaded from memory, your layer will too.
     
     >>> def deleteLayer(layerID, refCon):
     ...    xp.log("Layer being deleted")
     ...
     >>> layerID = xp.createMapLayer(name="Delete Example", delete=deleteLayer)

     This does not trigger when your map *layer* is disabled by the user, nor
     when the map window itself is closed. For X-Plane 11.50+ it appears to
     occur only when the sim is exited.
     

 On success, a :py:func:`createMapLayer` returns a layerID. Most common failure occurs when the map you specified in *map* does not exist
 (e.g., :py:func:`mapExists` returns 0). You can use 
 :py:func:`registerMapCreationHook` to get a notification each time a new map is
 opened in X-Plane, at which time you can create layers in it.

 For legacy purposes, you may pass a 10-element tuple *instead of* individually
 specifying the parameters.

 The tuple is:

 ::

    (
      mapType,
      layerType,
      delete, prep, draw, icon, label,
      showToggle,
      name,
      refCon
    )
    
 Note the order is very important!
 
 You pass in a tuple with **all of the fields** set in.

 >>> t = (xp.MAP_USER_INTERFACE,
 ...      xp.MapLayer_Markings,
 ...      None, None, draw, icon, label,
 ...      1,
 ...      "My New Map",
 ...      None)
 ...
 >>> layerID = xp.createMapLayer(t)

 `Official SDK <https://developer.x-plane.com/sdk/XPLMMap/#XPLMCreateMapLayer>`__ :index:`XPLMCreateMapLayer`


.. py:function:: destroyMapLayer(layerID)

 Destroys a map layer you created (calling your
 ``deleteLayer()`` callback if applicable). Returns 1 on success.

 >>> layerID = xp.createMapLayer()
 >>> xp.destroyMapLayer(layerID)
 1

 `Official SDK <https://developer.x-plane.com/sdk/XPLMMap/#XPLMDestroyMapLayer>`__ :index:`XPLMDestroyMapLayer`
  

.. py:function:: registerMapCreationHook(mapCreated, refCon)

 Registers your ``mapCreated()`` callback to receive a
 notification each time a new map is constructed in X-Plane. This callback is
 the best time to add your custom
 map layer using :py:func:`createMapLayer`.

 .. py:function:: mapCreated(mapType, refCon)
    
    *mapType* is the type of map being created, either ``xp.MAP_USER_INTERFACE`` or ``xp.MAP_IOS``.
    *refCon* is what you passed to :py:func:`registerMapCreationHook`

    No OpenGL drawing is permitted within this callback.

 `Official SDK <https://developer.x-plane.com/sdk/XPLMMap/#XPLMRegisterMapCreationHook>`__ :index:`XPLMRegisterMapCreationHook`
 
 .. note:: You will not be notified about any maps that already exist --- you
    can use :py:func:`mapExists` to check for maps that were created previously.

 >>> def mapCreated(mapType, refCon):
 ...    if mapType == xp.MAP_USER_INTERFACE:
 ...        do_my_create_layer()
 ...
 >>> if not xp.mapExists(xp.MAP_USER_INTERFACE):
 ...    # map does not yet exist, so register a creation callback.
 ...    xp.registerMapCreationHook(mapCreated)
 ... else:
 ...    # map already exists, so immediately create the layer
 ...    do_my_create_layer()

.. py:function:: mapExists(mapType)

 Returns 1 if the map with the specified identifier already exists in
 X-Plane. In that case, you can safely call :py:func:`createMapLayer` specifying
 that your layer should be added to that map.

 Note that "exists" is different from "displayed". The map may have been displayed
 and then hidden, yet it will still exist. You may create layers while the map window is
 hidden.

 *mapType* is one of ``xp.MAP_USER_INTERFACE``
 for the X-Plane Map Window or ``xp.MAP_IOS`` for the Instructor Operator Station.

 >>> xp.mapExists(xp.MAP_IOS)
 0
 >>> xp.mapExists(xp.MAP_USER_INTERFACE)
 1

 `Official SDK <https://developer.x-plane.com/sdk/XPLMMap/#XPLMMapExists>`__ :index:`XPLMMapExists`

Map Drawing
-----------

These APIs are only valid from within a map drawing callback (one of
``drawLayer()``, ``labelLayer()`` or ``iconLayer()``.)

Your drawing
callbacks are registered when you create a new map layer as part of your
:py:func:`createMapLayer`. The functions here hook into X-Plane's built-in map
drawing functionality for icons and labels, so that you get a consistent
style with the rest of the X-Plane map.

Note that the X-Plane 11 map introduces a strict ordering: layers of type
:data:`MapLayer_Fill` get drawn beneath all :data:`MapLayer_Markings` layers.
Likewise, all OpenGL drawing (performed in your layer's ``drawLayer()`` callback)
will appear beneath any icons and labels you draw.

.. py:function:: drawMapIconFromSheet(layerID, png, s, t, ds, dt, x, y, orientation, rotationDegrees, mapWidth)

 Enables plugin-created map layers to draw PNG icons using X-Plane's
 built-in icon drawing functionality. Only valid from within an
 ``iconLayer()`` callback (but you can request an arbitrary number of icons
 to be drawn from within your callback).
 
 *layerID* is the value returned from your :py:func:`createMapLayer`, *png* is the string
 path to a PNG file, relative X-Plane root (e.g., "Resources/plugins/PythonPlugins/foo.png").

 *s*, *t*, *ds*, *dt* are icon coordinates within the png file (see below).

 *x*, *y* are projected latitude and longitude coordinates, where you want to draw the icon. (See :py:func:`mapProject`).

 *orientation* is the map's current orientation, either ``xp.MapOrientation_Map`` (0 degrees rotation matches map's north),
 or ``xp.MapOrientation_UI`` (0 degrees rotation is "up" relative to the user interface). *rotationDegrees* indicated degrees
 of clockwise rotation.

 *mapWidth* is the width of the icon in **map units** (See :py:func:`mapScaleMeter`)

 X-Plane will automatically manage the memory for your PNG texture so that it
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
 
 This function is only valid from within an ``iconLayer()`` (but
 you can request an arbitrary number of icons to be drawn from within your
 callback).

 See example in :py:func:`createMapLayer` ``iconLayer()`` callback above.

 `Official SDK <https://developer.x-plane.com/sdk/XPLMMap/#XPLMDrawMapIconFromSheet>`__ :index:`XPLMDrawMapIconFromSheet`

.. py:function:: drawMapLabel(layerID, label, x, y, orientation, rotate)

 Enables plugin-created map layers to draw text labels using X-Plane's
 built-in labeling functionality. Only valid from within a ``labelLayer()`` callback
 (but you can request an arbitrary number of text labels to be drawn from within your callback).

 *layerID* is the value returned from your :py:func:`createMapLayer`, *label* is the string
 to be displayed.

  *x*, *y* are projected latitude and longitude coordinates, where you want to draw the icon. (See :py:func:`mapProject`).
  The string will be centered on that point.

 *orientation* is the map's current orientation, either ``xp.MapOrientation_Map`` (0 degrees rotation matches map's north),
 or ``xp.MapOrientation_UI`` (0 degrees rotation is "up" relative to the user interface). *rotate* indicated degrees
 of clockwise rotation.

 See example in :py:func:`createMapLayer` ``labelLayer()`` callback above.

 `Official SDK <https://developer.x-plane.com/sdk/XPLMMap/#XPLMDrawMapLabel>`__ :index:`XPLMDrawMapLabel`

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


.. py:function:: mapProject(projection, latitude, longitude)

 Projects a *latitude*, *longitude* (in degrees) into map coordinates (x, y). This is the inverse of
 :py:func:`mapUnproject`.

 Only valid from within a map layer callback (one of
 :py:func:`prepLayer`, :py:func:`drawLayer`,
 :py:func:`iconLayer`, or :py:func:`labelLayer`.)

 The returned (x, y) tuple can be used to draw on the map
 (e.g, with :py:func:`drawMapIconFromSheet`, :py:func:`drawMapLabel`, or OpenGL).   

 *projection* is opaque handle to the current projection, which you'll receive as
 an input to you callbacks.

 `Official SDK <https://developer.x-plane.com/sdk/XPLMMap/#XPLMMapProject>`__ :index:`XPLMMapProject`

.. py:function:: mapUnproject(projection, x, y)

 Transforms map coordinates back into a latitude and longitude. This is the
 inverse of :py:func:`mapProject`.

 Only valid from within a map layer callback (one of
 :py:func:`prepLayer`, :py:func:`drawLayer`,
 :py:func:`iconLayer`, or :py:func:`labelLayer`.)

 *projection* is opaque handle to the current projection, which you'll receive as
 an input to you callbacks.

 Returns a pair of floats (latitude, longitude)

 `Official SDK <https://developer.x-plane.com/sdk/XPLMMap/#XPLMMapUnproject>`__ :index:`XPLMMapUnproject`

 .. note:: While :py:func:`mapProject` and :py:func:`mapUnproject` are inverse functions,
           they are only valid over the bounds of the current map. That is, the map coordinates
           (0, 0) will map to many different (latitude, longitude) depending on
           where the map is currently loaded. This is the reason for :py:func:`prepLayer`,
           which will be called whenever the map's domain has changed. See example in
           :doc:`PI_Map.py<../samples>` demo.

.. py:function:: mapScaleMeter(projection, x, y)

 Returns the number of map units that correspond to a distance of **one meter**
 at a given set of map coordinates. (Because of the projection, "one meter"
 at the center of the projected map may not be the same number of map units
 as at the edges.)

 Only valid from within a map layer callback (one of
 :py:func:`prepLayer`, :py:func:`drawLayer`,
 :py:func:`iconLayer`, or :py:func:`labelLayer`.)

 *projection* is opaque handle to the current projection, which you'll receive as
 an input to you callbacks.

 Number of map units allows you to scale map icons. For example, the width of a icon (:py:func:`iconLayer`)
 is in map units. If you want the icon to be "10 miles" wide on the map, calculate the units, then convert
 meters to miles and multiply for 10 miles:

 >>> width = xp.mapScaleMeter(projection, x, y) * 1609.34 * 10

 `Official SDK <https://developer.x-plane.com/sdk/XPLMMap/#XPLMMapScaleMeter>`__ :index:`XPLMMapScaleMeter`

.. py:function:: mapGetNorthHeading(projection, x, y)

 Returns the heading (in degrees clockwise from "up") that corresponds to
 north at a given point on the map. In other words, if your runway has a
 true heading of 360, you would use "north" as the Cartesian angle at which
 to draw the runway on the map. (You would add the result of
 :py:func:`mapGetNorthHeading` to your true heading to get the map angle.)
 
 This is necessary because X-Plane's map can be rotated to match your
 aircraft's orientation; north is not always "up."

 Only valid from within a map layer callback (one of
 :py:func:`prepLayer`, :py:func:`drawLayer`,
 :py:func:`iconLayer`, or :py:func:`labelLayer`.)

 *projection* is opaque handle to the current projection, which you'll receive as
 an input to you callbacks.

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

 `Official SDK <https://developer.x-plane.com/sdk/XPLMMap/#XPLMMapGetNorthHeading>`__ :index:`XPLMMapGetNorthHeading`             

Example
-------

See :doc:`PI_Map.py<../samples>` demo for detailed example. To run, you'll need to:

#. Copy the necessary image file ``map-sample-image.png`` from
   ``PythonPlugins/samples`` to your ``Resources/plugins`` directory, and
#. Install OpenGL: You can do this within X-Plane by using the XPPython3 Pip Installer menu item and install the
   package ``pyopengl``.
