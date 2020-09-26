XPLMScenery
===========

.. py:module:: XPLMScenery

To use::
  
  import XPLMScenery

Terrain Y-Testing
-----------------

The Y-testing API allows you to locate the physical scenery mesh. This
would be used to place dynamic graphics on top of the ground in a plausible
way or do physics interactions.

The Y-test API works via probe objects, which are allocated by your plugin
and used to query terrain. Probe objects exist both to capture which
algorithm you have requested (see probe types) and also to cache query
information.

Performance guidelines: It is generally faster to use the same probe for
nearby points and different probes for different points. Try not to
allocate more than "hundreds" of probes at most. Share probes if you need
more. Generally, probing operations are expensive, and should be avoided
via caching when possible.

Y testing returns a location on the terrain, a normal vectory, and a
velocity vector. The normal vector tells you the slope of the terrain at
that point. The velocity vector tells you if that terrain is moving (and is
in meters/second). For example, if your Y test hits the aircraft carrier
deck, this tells you the velocity of that point on the deck.

Note: the Y-testing API is limited to probing the loaded scenery area,
which is approximately 300x300 km in X-Plane 9. Probes outside this area
will return the height of a 0 MSL sphere.

.. _XPLMProbeType:

XPLMProbeType
*************

XPLMProbeType defines the type of terrain probe - each probe has a
different algorithm. (Only one type of probe is provided right now, but
future APIs will expose more flexible or poewrful or useful probes.

 .. py:data:: xplm_ProbeY
   :value: 0

   The Y probe gives you the location of the tallest physical scenery along
   the Y axis going through the queried point.


.. _XPLMProbeResult:

XPLMProbeResult
***************

Probe results - possible results from a probe query.


 .. py:data:: xplm_ProbeHitTerrain
  :value: 0

  The probe hit terrain and returned valid values.

 .. py:data:: xplm_ProbeError
  :value: 1

  An error in the API call.  Either the probe struct size is bad, or the
  probe is invalid or the type is mismatched for the specific query call.

 .. py:data:: xplm_ProbeMissed
  :value: 2

  The probe call succeeded but there is no terrain under this point (perhaps
  it is off the side of the planet?)


.. py:function:: XPLMCreateProbe(probeType) -> probe:

  :param probeType: :py:data:`XPLMProbeType`                  
  :return: probe (int)

.. py:function:: XPLMDestroyProbe(inProbe: int) -> None:

    Deallocates an existing probe object created by :py:func:`XPLMCreateProbe`.


.. py:function:: XPLMProbeTerrainXYZ(probe: int, x: float, y: float, z: float) -> probeInfo 

    Probes the terrain. Pass in the XYZ coordinate of the probe point
    and a probe object. The return is a single
    object with attributes described below.

    :return: probeInfo object with attributes:
             
       * result: :ref:`XPLMProbeResult`
       * locationX, locationY, locationZ: point hit by the probe
         in local OpenGL coordinates (floats)
       * normalX, normalY, normalZ: normal vector to the terrain found (floats)
       * velocityX, velocityY, velocityZ: velocity vector of the terrain found (floats)
       * is_wet: tells if the surface we hit is water (1= water)


Magnetic Variation
------------------

Use the magnetic variation (more properly, the "magnetic declination") API
to find the offset of magnetic north from true north at a given latitude
and longitude within the simulator.

In the real world, the Earth's magnetic field is irregular, such that true
north (the direction along a meridian toward the north pole) does not
necessarily match what a magnetic compass shows as north.

Using this API ensures that you present the same offsets to users as
X-Plane's built-in instruments.

.. py:function:: XPLMGetMagneticVariation(latitude: float, longitude: float) ->variation:

 Returns X-Plane's simulated magnetic variation (declination) at the
 indication latitude and longitude (float).


.. py:function::  XPLMDegTrueToDegMagnetic(degreesTrue: float) -> degreesMagnetic

 Converts a heading in degrees relative to true north into a value relative
 to magnetic north at the user's current location. (float)


.. py:function::  XPLMDegMagneticToDegTrue(degreesMagnetic: float) -> degreesTrue:

 Converts a heading in degrees relative to magnetic north at the user's
 current location into a value relative to true north. (float)

Object Drawing
--------------

The object drawing routines let you load and draw X-Plane OBJ files.
Objects are loaded by file path and managed via an opaque handle. X-Plane
naturally reference counts objects, so it is important that you balance
every successful call to XPLMLoadObject with a call to XPLMUnloadObject!

.. py:function:: XPLMObjectLoaded_f(objectRef, refCon) -> None:

  :param int objectRef: reference to loaded object
  :param object refCon: Reference Constant provled with :py:func:`XPLMLoadObjectAsync`               

  You provide this callback when loading an object asynchronously (i.e.,
  via :py:func:`XPLMLoadObjectAsync`; it will be
  called once the object is loaded. Your refCon is passed back. The object
  ref passed in is the newly loaded object (ready for use) or None if an
  error occured.

  If your plugin is disabled, this callback will be delivered as soon as the
  plugin is re-enabled. If your plugin is unloaded before this callback is
  ever called, the SDK will release the object handle for you.


.. py:function:: XPLMLoadObject(path: str) -> objectRef:

    This routine loads an OBJ file and returns a handle to it. If X-Plane has
    already loaded the object, the handle to the existing object is returned.
    Do not assume you will get the same handle back twice, but do make sure to
    call unload once for every load to avoid "leaking" objects. The object will
    be purged from memory when no plugins and no scenery are using it.

    The path for the object must be relative to the X-System base folder. If
    the path is in the root of the X-System folder you may need to prepend ./
    to it; loading objects in the root of the X-System folder is STRONGLY
    discouraged - your plugin should not dump art resources in the root folder!

    :py:func:`XPLMLoadObject` will return None if the object cannot be loaded (either
    because it is not found or the file is misformatted). This routine will
    load any object that can be used in the X-Plane scenery system.

    It is important that the datarefs an object uses for animation already be
    loaded before you load the object. For this reason it may be necessary to
    defer object loading until the sim has fully started.


.. py:function:: XPLMLoadObjectAsync(path: str, callback: callable, refcon: object) -> None:

    :param str path: path to the object to be loaded (string)
    :param callback: :py:func:`XPLMObjectLoaded_f`
    :param object refCon: Reference constant to be passed to your callback

    This routine loads an object asynchronously; control is returned to you
    immediately while X-Plane loads the object. The sim will not stop flying
    while the object loads. For large objects, it may be several seconds before
    the load finishes.

    You provide a callback function that is called once the load has completed.
    Note that if the object cannot be loaded, you will not find out until the
    callback function is called with a None object handle.

    There is no way to cancel an asynchronous object load; you must wait for
    the load to complete and then release the object if it is no longer
    desired.



.. py:function::  XPLMUnloadObject(objectRef: int) -> None:

    This routine marks an object as no longer being used by your plugin.
    Objects are reference counted: once no plugins are using an object, it is
    purged from memory. Make sure to call :py:func:`XPLMUnloadObject`
    once for each
    successful call to :py:func:`XPLMLoadObject` or :py:func:`XPLMLoadObjectAsync`.



Library Access
--------------

The library access routines allow you to locate scenery objects via the
X-Plane library system. Right now library access is only provided for
objects, allowing plugin-drawn objects to be extended using the library
system.


.. py:function:: XPLMLibraryEnumerator_f(path: str, refCon: object) -> None:

    You provide this callback which is called once
    for each library element that is located. The returned paths will be
    relative to the X-System folder.


.. py:function:: XPLMLookupObjects(path: str, latitude: float, longitude: float, enumerator: callable, refCon: object) -> int:

    :param str path: Virtual path to look up in the system library
    :param float latitude:
    :param float longitude: location where the object will be used.
    :param enumerators: :py:func:`XPLMLibraryEnumerator_f` function
    :param object refCon: Reference constant provided to callback.
    :return: Number of items found.

    This routine looks up a virtual path in the library system and returns all
    matching elements. You provide a callback - one virtual path may match many
    objects in the library. XPLMLookupObjects returns the number of objects
    found.

    The latitude and longitude parameters specify the location the object will
    be used. The library system allows for scenery packages to only provide
    objects to certain local locations. Only objects that are allowed at the
    latitude/longitude you provide will be returned.

    ::

       def enumerator(path, refCon):
           refCon.append(path)

       objects = []
       XPLMLookupObjects('lib/airport/landscape/windsock.obj', 39.74, -104.99,
                         enumerator, objects)

       for path in objects:
          print("Object path: {}".format(path)

    You can also do this simply with a lambda function::

      objects = []
      XPLMLookupObjects('lib/airport/landscape/windsock.obj', 39.74, -104.99,
                        lambda path, _: objects.append(path), None)

      for path in objects:
          print("Object path: {}".format(path)
