XPLMScenery
===========

.. py:module:: XPLMScenery
.. py:currentmodule:: xp               

To use::
  
  import xp

Scenery-related APIs include:

* Terrain testing: the ability to understand height, slope and relative movement of a point
  of terrain:

  * :py:func:`probeTerrainXYZ`, with :py:func:`createProbe`, :py:func:`destroyProbe`.

* Understanding magnetic variation (declination) at a particular point:

  * :py:func:`getMagneticVariation`

  * :py:func:`degTrueToDegMagnetic` and :py:func:`degMagneticToDegTrue`
    
* Finding and loading scenery library objects:

  * :py:func:`lookupObjects`
    
  * :py:func:`loadObject`, :py:func:`loadObjectAsync`

  * :py:func:`unloadObject`

    
    
Terrain Y-Testing
-----------------

The Y-testing API allows you to locate the physical scenery mesh. This
would be used to place dynamic graphics on top of the ground in a plausible
way or do physics interactions.

The Y-test API works via probeRef objects, which are allocated by your plugin
and used to query terrain. ProbeRef objects exist both to capture which
algorithm you have requested (see probe types) and also to cache query
information.

Performance guidelines: It is generally faster to use the same probeRef for
nearby points and different probeRefs for different points. Try not to
allocate more than "hundreds" of probeRefs at most. Share probeRefs if you need
more. Generally, probing operations are expensive, and should be avoided
via caching when possible.

Y testing returns a location on the terrain, a normal vector, and a
velocity vector. The normal vector tells you the slope of the terrain at
that point. The velocity vector tells you if that terrain is moving (and is
in meters/second). For example, if your Y test hits the aircraft carrier
deck, this tells you the velocity of that point on the deck.

Note: the Y-testing API is limited to probing the loaded scenery area,
which is approximately 300x300 km in X-Plane 9. ProbeRefs outside this area
will return the height of a 0 MSL sphere.

.. py:function:: createProbe(probeType=0)

  Returns a probeRef, which is to be  used with :py:func:`probeTerrain`.

  The *probeType* defines the type of probe, of which there is only one currently:

  .. py:data:: ProbeY
      :value: 0

      The Y probe gives you the location of the tallest physical scenery along
      the Y axis going through the queried point.

  >>> probe = xp.createProbe()
  >>> probe
  <capsule object "XPLMProbeRef" at 0x7f9c37464f90>
           
  `Official SDK <https://developer.x-plane.com/sdk/XPLMScenery/#XPLMCreateProbe>`__ :index:`XPLMCreateProbe`

.. py:function:: destroyProbe(probeRef)

  Deallocates an existing *probeRef* created by :py:func:`createProbe`.

  >>> probe = xp.createProbe()
  >>> xp.destroyProbe(probe)

  `Official SDK <https://developer.x-plane.com/sdk/XPLMScenery/#XPLMDestroyProbe>`__ :index:`XPLMDestroyProbe`

.. py:function:: probeTerrainXYZ(probeRef, x, y, z)

  Probes the terrain. Pass in *probeRef*, and the *x*, *y*, *z* coordinates of the point.
  (You can obtain (x, y, z) information using, for example, :py:func:`worldToLocal` or
  datarefs ``sim/flightmodel/position/local_[xyz]``.) 

  The return value is an object with attributes:

   | .result: integer probe result value (See table below)
   | .locationX,
   | .locationY,
   | .locationZ: OpenGL point hit by the probe.
   | .normalX,
   | .normalY,
   | .normalZ: normal vector (e.g., the slope) of the terrain found
   | .velocityX,
   | .velocityY,
   | .velocityZ: velocity vector (e.g., meter/s) of movement of the terrain found
   | .is_wet: tells if the surface we hit is water (1= water)

  For example, to determine actual height above terrain, get current aircraft
  (x, y, z) position, call :py:func:`probeTerrainXYZ` for that position. The returned
  value will tell you the (x, y, z) of the highest point of terrain there. Then
  subtract the two "y" values to determine meters above ground & convert to feet:

  >>> x = xp.getDatad(xp.findDataRef('sim/flightmodel/position/local_x'))
  >>> y = xp.getDatad(xp.findDataRef('sim/flightmodel/position/local_y'))
  >>> z = xp.getDatad(xp.findDataRef('sim/flightmodel/position/local_z'))
  >>> probeRef = xp.createProbe()
  >>> info = xp.probeTerrainXYZ(probeRef, x, y, z)
  >>> info
  <xppython3.ProbeInfo object at 0x7f9c34564220>
  >>> print(f"AGL is {(y - info.locationY) * 3.28084}")
  AGL is 1004.0637
  
  +------------------------------------+------------------------------------+
  |Probe Result Value                  |Meaning                             |
  +====================================+====================================+
  | .. py:data:: ProbeHitTerrain       |The probe hit terrain and returned  |
  |  :value: 0                         |valid values.                       |
  +------------------------------------+------------------------------------+
  |.. py:data:: ProbeError             |An error in the API call.  Either   |
  |  :value: 1                         |the probe struct size is bad, or the|
  |                                    |probe is invalid or the type is     |
  |                                    |mismatched for the specific query   |
  |                                    |call.                               |
  +------------------------------------+------------------------------------+
  | .. py:data:: ProbeMissed           |The probe call succeeded but there  |
  |   :value: 2                        |is no terrain under this point      |
  |                                    |(perhaps it is off the side of the  |
  |                                    |planet?)                            |
  +------------------------------------+------------------------------------+

  `Official SDK <https://developer.x-plane.com/sdk/XPLMScenery/#XPLMProbeTerrainXYZ>`__ :index:`XPLMProbeTerrainXYZ`

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

.. py:function:: getMagneticVariation(latitude, longitude)

 Returns X-Plane's simulated magnetic variation (declination) at the
 indicated latitude and longitude.

 >>> xp.getMagneticVariation(37.62, -122.38)
 -13.2694

 `Official SDK <https://developer.x-plane.com/sdk/XPLMScenery/#XPLMGetMagneticVariation>`__ :index:`XPLMGetMagneticVariation`

.. py:function::  degTrueToDegMagnetic(degreesTrue=0.0)

 Converts a heading in degrees relative to true north into a value relative
 to magnetic north *at the user's current location*.

 >>> xp.degTrueToDegMagnetic()
 344.657
 >>> xp.degTrueToDegMagnetic(-90)
 254.657

 `Official SDK <https://developer.x-plane.com/sdk/XPLMScenery/#XPLMDegTrueToDegMagnetic>`__ :index:`XPLMDegTrueToDegMagnetic`

.. py:function::  degMagneticToDegTrue(degreesMagnetic=0.0)

 Converts a heading in degrees relative to magnetic north *at the user's
 current location* into a value relative to true north.

 >>> xp.degMagneticToDegTrue()
 15.3428

 `Official SDK <https://developer.x-plane.com/sdk/XPLMScenery/#XPLMDegMagneticToDegTrue>`__ :index:`XPLMDegMagneticToDegTrue`

Library Access
--------------

The library access routines allow you to locate scenery objects via the
X-Plane library system. Right now library access is only provided for
objects, allowing plugin-drawn objects to be extended using the library
system.


.. py:function:: lookupObjects(path, latitude, longitude, enumerator, refCon)

    This routine looks up a virtual *path* in the library system and returns a
    count of the number of matching elements found.
    You provide an *enumerator* callback to get loadable paths. (One virtual path may
    match many objects in the library). Your enumerator function will be called
    immediately and takes two parameters, one is the found path and the other is the *refCon*
    provided with the :py:func:`lookupObjects` call.

    >>> def MyEnumerator(path, refCon):
    ...   refCon.append(path)
    ...
    >>> objects = []
    >>> xp.lookupObjects('lib/airport/landscape/windsock.obj', enumerator=MyEnumerator, refCon=objects)
    1
    >>> objects
    ['Resources/default scenery/sim objects/landscape/windsock.obj']

    You can also do this simply with a lambda function:

    >>> objects = []
    >>> xp.lookupObjects('lib/ships/SailBoat.obj', enumerator=lambda path, _: objects.append(path))
    1
    >>> objects
    ['Resources/default scenery/sim objects/dynamic/SailBoat.obj']
    

    The *latitude* and *longitude* parameters specify the location the object will
    be used. The library system allows for scenery packages to only provide
    objects to certain local locations. Only objects that are allowed at the
    latitude/longitude you provide will be returned. (Not specifying latitude / longitude results
    in using location (0, 0).)

    For example, look some autogen resources differ when searching within Europe:

    >>> path = "lib/g10/autogen/urban_low_broken_0.ags"
    >>> xp.lookupObjects(path, latitude=35, longitude=-122, enumerator=lambda path, _: print(path))
    Resources/default scenery/1000 autogen/US/suburban/SubResSW32m.ags
    1
    >>> xp.lookupObjects(path, latitude=50, longitude=8.7, enumerator=lambda path, _: print(path))
    Resources/default scenery/1000 autogen/EU/sub_Resid02.ags
    1
    
    `Official SDK <https://developer.x-plane.com/sdk/XPLMScenery/#XPLMLookupObjects>`__ :index:`XPLMLookupObjects`

Object Drawing
--------------

The object drawing routines let you load and draw X-Plane OBJ files.
Objects are loaded by file path and managed via an opaque handle. X-Plane
naturally reference counts objects, so it is important that you balance
every successful call to :py:func:`loadObject` with a call to :py:func:`unloadObject`!

.. py:function:: loadObject(path)

    This routine loads an OBJ file and returns an ``objectRef`` handle to it. If X-Plane has
    already loaded the object, the handle to the existing object is returned.
    Do not assume you will get the same handle back twice, but do make sure to
    call unload once for every load to avoid "leaking" objects. The object will
    be purged from memory when no plugins and no scenery are using it.

    The path for the object must be relative to the X-System base folder. If
    the path is in the root of the X-System folder you may need to prepend ./
    to it; loading objects in the root of the X-System folder is STRONGLY
    discouraged - your plugin should not dump art resources in the root folder!

    :py:func:`loadObject` will return None if the object cannot be loaded (either
    because it is not found or the file is misformatted). This routine will
    load any object that can be used in the X-Plane scenery system.

    It is important that the datarefs an object uses for animation already be
    loaded before you load the object. For this reason it may be necessary to
    defer object loading until the sim has fully started.

    >>> objRef = xp.loadObject('Resources/default scenery/sim objects/dynamic/SailBoat.obj')
    >>> objRef
    <capsule object "XPLMObjectRef" at 0x7fe1d8353090>

    `Official SDK <https://developer.x-plane.com/sdk/XPLMScenery/#XPLMLoadObject>`__ :index:`XPLMLoadObject`


.. py:function:: loadObjectAsync(path, loaded, refCon)

    This routine loads an object asynchronously; control is returned to you
    immediately while X-Plane loads the object. The sim will not stop flying
    while the object loads. For large objects, it may be several seconds before
    the load finishes.

    You provide an *loaded* callback function that is called once the load has completed.
    Note that if the object cannot be loaded, you will not find out until the
    callback function is called with a None object handle.

    The callback function takes two parameters, the ``objectRef`` of the object
    loaded, and your ``refCon``. For example:

    >>> def MyLoaded(objectRef, refCon):
    ...    xp.log(f"Object {objectRef} has been loaded")
    ...
    >>> xp.loadObjectAsync('Resources/default scenery/sim objects/dynamic/SailBoat.obj')
    >>>

    If your plugin is disabled, this callback will be delivered as soon as the
    plugin is re-enabled. If your plugin is unloaded before this callback is
    ever called, the SDK will release the object handle for you.

    There is no way to cancel an asynchronous object load; you must wait for
    the load to complete and then release the object if it is no longer
    desired.

    `Official SDK <https://developer.x-plane.com/sdk/XPLMScenery/#XPLMLoadObjectAsync>`__ :index:`XPLMLoadObjectAsync`

.. py:function::  unloadObject(objectRef)

    This routine marks an *objectRef* as no longer being used by your plugin.
    Objects are reference counted: once no plugins are using an object, it is
    purged from memory. Make sure to call :py:func:`unloadObject`
    once for each
    successful call to :py:func:`loadObject` or :py:func:`loadObjectAsync`.

    `Official SDK <https://developer.x-plane.com/sdk/XPLMScenery/#XPLMUnloadObject>`__ :index:`XPLMUnloadObject`

.. note:: To *draw* a loaded object, use the :py:mod:`XPLMInstance` API, for example

    >>> # lookup object, getting the objectRef
    >>> objects = []
    >>> xp.lookupObjects('lib/ships/SailBoat.obj', enumerator=lambda path, _: objects.append(path))
    >>> objRef = xp.loadObject(objects[0])
    >>> # Create Instance, using this objectRef
    >>> instance = xp.createInstance(objRef)
    >>> # Get location of user aircraft
    >>> x = xp.getDatad(xp.findDataRef('sim/flightmodel/position/local_x'))
    >>> y = xp.getDatad(xp.findDataRef('sim/flightmodel/position/local_y'))
    >>> z = xp.getDatad(xp.findDataRef('sim/flightmodel/position/local_z'))
    >>> pitch, heading, roll = (0, 0, 0)
    >>> position = x, y, z + 10, pitch, heading, roll
    >>> xp.instanceSetPosition(instance, position)


