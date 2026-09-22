.. index:: Object Instances, Tasks; Object Instances

XPLMInstance
============
.. py:module:: XPLMInstance
.. py:currentmodule:: xp
   
To use::

  import xp

This API provides instanced drawing of X-Plane objects (.obj files). In
contrast to old drawing APIs, (:doc:`graphics`) which required you to draw your own objects
per-frame, the instancing API allows you to simply register an OBJ for
drawing, then move or manipulate it later (as needed).

This provides one tremendous benefit: it keeps all dataRef operations for
your object in one place. Because datarefs are main thread only, allowing
dataRef access anywhere is a serious performance bottleneck for the
simulator---the whole simulator has to pause and wait for each dataRef
access. This performance penalty will only grow worse as X-Plane moves
toward an ever more heavily multi-threaded engine.

The instancing API allows X-Plane to isolate all dataRef manipulations for
all plugin object drawing to one place, potentially providing huge
performance gains.

Here's how it works:

When an instance is created using :py:func:`createInstance`, it provides a list of all datarefs you want to
manipulate in for the OBJ in the future. This list of datarefs replaces the
ad-hoc collections of dataRef objects previously used by art assets. Then,
per-frame, you can manipulate the instance by passing in a "block" of
packed floats representing the current values of the datarefs for your
instance using :py:func:`instanceSetPosition`. (Note that the ordering of this set of packed floats must exactly
match the ordering of the datarefs when you created your instance.)

Functions
---------

.. py:function:: createInstance(obj, dataRefs=None) -> XPLMInstanceRef

    :param XPLMObjectRef obj: object reference, a description of instance to be created
    :param List[str] dataRefs: List of dataRef strings to be passed to the instances (or None)
    :return: :class:`XPLMInstanceRef`

    Registers an instance of an X-Plane object, with an optional list of *dataRefs*.

    *obj* is an object handle, as returned by :py:func:`loadObject` or :py:func:`loadObjectAsync`.
    Note that this object must be fully loaded before you can attempt to create an instance of that object: you
    cannot pass in a Null object reference or change the reference later.

    Also, if you use an custom datarefs in your object, they must be registered before the object is loaded. This
    is true even if their data will be provided via the instance dataref list.
    
    Returns an object instance, which you'll pass to :py:func:`instanceSetPosition` and :py:func:`destroyInstance`.

    The following example loads an XP standard object using :py:func:`lookupObjects`, and creates
    an instance of it, with two datarefs:

    >>> paths = []
    >>> xp.lookupObjects('lib/airport/vehicles/pushback/tug.obj', 0, 0, lambda path, refCon: paths.append(path), None)
    1
    >>> paths
    ['Resources/default scenery/sim objects/apt_vehicles/pushback/Tug_GT110.obj']
    >>> obj = xp.loadObject(paths[0])
    >>> drefs = ['sim/graphics/animation/ground_traffic/tire_steer_deg', 'foo/bar/ground']
    >>> instance = xp.createInstance(obj, drefs)
    >>> instance
    <capsule object "XPLMInstanceRef" at 0x7f8946a4a9f0>

    (With the above code, you've found, loaded and created the Instance, but you still need to :py:func:`instanceSetPosition`
    in order to actually see it.)

    Note you can also release the loaded object (:py:func:`unloadObject`) immediately
    after successful ``createInstance()`` (if you don't need to create another
    instance of that object.) The created ``XPLMInstance`` will maintain its own reference to the object and the object will
    be deallocated when the instance is destroyed.
    
    `Official SDK <https://developer.x-plane.com/sdk/XPLMInstance/#XPLMCreateInstance>`__ :index:`XPLMCreateInstance`

.. py:function:: destroyInstance(instance) -> None

    :param XPLMInstanceRef instance: Instance to be destroyed.

    Unregisters an instance (as returned from :py:func:`createInstance`.) You are still responsible for
    eventually releasing the *Object* using :py:func:`unloadObject`.

    >>> xp.destroyInstance(instance)

    `Official SDK <https://developer.x-plane.com/sdk/XPLMInstance/#XPLMDestroyInstance>`__ :index:`XPLMDestroyInstance`

.. py:function:: instanceSetPosition(instance, position, data=None) -> None

    :param XPLMInstanceRef instance: Instance to be positioned
    :param Tuple position: Six-float tuple (x, y, z, pitch, heading, roll)                                 
    :param List[float] data: List of floats, matching count and order the dataRefs provided on create
                             
    Updates both the position of the instance and all datarefs you registered
    for it. Call this from a flight loop callback or UI callback;

    **Do Not** call this from a drawing callback; the whole point of instancing is that you do
    not need any drawing callbacks. Setting instance data from drawing callback may have undefined
    consequences, and the drawing callback hurts FPS unnecessarily.

    *instance* is as returned by :py:func:`createInstance`.
    You must always provide a six-float tuple for *position*: (x, y, z, pitch, heading, roll),
    and *data* may be None.
    Otherwise *data* should be a list of floats in the same order as
    the dataRefs provided during instance creation. (If you don't provide *data*, it effectively
    sets each registered dataRef to 0.0.)

    *Floats*, even if your dataref takes integers. If you dataref is a vector, you need to explicitly
    list the elements in :func:`createInstance` (e.g., ``dataRefs=["mydataref[0]", "mydataref[1]", "mydataref[2]"]``)
    and pass each value as a float (e.g., ``(0.0, 1.0, 1.0)``)

    The following example builds on the example in :py:func:`createInstance`, and sets the position
    of the instance, and sets values for each of the (two) datarefs.
    You'll likely call this in your flight loop callback (*not* a draw callback):

    >>> # get current aircraft position
    >>> x = xp.getDatad(xp.findDataRef('sim/flightmodel/position/local_x'))
    >>> y = xp.getDatad(xp.findDataRef('sim/flightmodel/position/local_y'))
    >>> z = xp.getDatad(xp.findDataRef('sim/flightmodel/position/local_z'))
    >>> pitch, heading, roll = (0, 0, 0)
    >>> # Place the tug a bit away from the aircraft (just so you can see it: you may need to pan)
    >>> position = (x, y-1, z+10, pitch, heading+90, roll)
    >>> # Set wheel steer degrees to 0, then 20, 40, pausing in between.
    >>> xp.instanceSetPosition(instance, position, [0, 0.0])
    >>> xp.instanceSetPosition(instance, position, [20, 0.0])
    >>> xp.instanceSetPosition(instance, position, [40, 0.0])

    By changing the passed-in dataRef values, we can drive animation of the tug.

    .. image:: /images/animated_tug.gif
               
    `Official SDK <https://developer.x-plane.com/sdk/XPLMInstance/#XPLMInstanceSetPosition>`__ :index:`XPLMInstanceSetPosition`

.. py:function:: instanceSetPositionDouble(instance, position, data=None) -> None

    :param XPLMInstanceRef instance: Instance to be positioned
    :param Tuple position: Six-float tuple (x, y, z, pitch, heading, roll)                                 
    :param List[float] data: List of floats, matching count and order the dataRefs provided on create
                             
    Laminar provides two C functions, one taking floats (``XPLMInstanceSetPosition``)
    and the other taking doubles (``XPLMInstanceSetPositionDouble``). Because
    Python floating point numbers are always double, both of this interfaces are the same. For
    convenience, we provide this "double" api interface, though it is identical to :func:`instanceSetPosition`.

    Technically, the XPPython3 code will pass doubles to X-Plane, if the double interface is available,
    otherwise it will pass floats. Both functions use the same XPPython3 code.
    
    `Official SDK <https://developer.x-plane.com/sdk/XPLMInstance/#XPLMInstanceSetPositionDouble>`__ :index:`XPLMInstanceSetPositionDouble`

.. py:function:: instanceSetAutoShift(instance) -> None

    :param XPLMInstanceRef instance: Instance to be positioned

    Tell X-Plane to move the (local) location of this instance every time the sim's
    local coordinate system changes, so that a static instance does move globally.
    Without this, a plugin is responsible for updating an instance's local position
    using :func:`instanceSetPosition` when the coordinate system shifts. Use
    this for static instances that you would not otherwise have to move. (Always
    call :func:`instanceSetPosition`, once, first.)

    Autoshift remains active until the next :func:`instanceSetPosition` is called for this
    instance. If you call :func:`instanceSetPosition` you will have to repeat the call
    to :func:`instanceSetAutoShift` to re-enable this feature.

    If your instance requires passing datarefs (which is done using :func:`instanceSetPosition`)
    this function isn't helpful as you will still have to re-calculate position information on each
    call.

    `Official SDK <https://developer.x-plane.com/sdk/XPLMInstance/#XPLMInstanceSetPositionAutoShift>`__ :index:`XPLMInstanceSetPositionAutoShift`

.. py:function:: createInstanceEx(objects, dataRefs=None, coordinateSpace=CoordSpace_World, aircraftIndex=0, autoShift=0) -> XPLMInstanceRef

    :param objects: Sequence of objects making up the instance (see below)
    :param List[str] dataRefs: List of dataRef strings shared by every object (or None)
    :param XPLMCoordinateSpace_t coordinateSpace: One of the :data:`CoordSpace_World`, :data:`CoordSpace_AircraftInterior`, :data:`CoordSpace_AircraftExterior`, :data:`CoordSpace_Camera` constants
    :param int aircraftIndex: Aircraft index (0 = user aircraft), used only for the two aircraft coordinate spaces
    :param int autoShift: If non-zero, enable auto-shift (world space only); see :func:`instanceSetAutoShift`
    :return: :class:`XPLMInstanceRef` representing the *set* of objects

    A superset of :func:`createInstance` (new with X-Plane 12.4.4 / SDK 440). It
    builds a single instance out of **one or more** objects that draw and move
    together as one rigid group, lets you choose the *coordinate space* in which
    positions are interpreted, and lets you enable auto-shift --- all in one call.

    *objects* is a sequence where each element is either:

    * an :class:`XPLMObjectRef` capsule (as returned by :func:`loadObject` or :func:`getObjects`), placed at the
      instance origin with no offset; or
    * a tuple ``(obj, x, y, z, pitch, heading, roll)`` giving a fixed local offset
      for that object relative to the shared instance origin. Trailing offset
      values default to ``0``. That is ``[obj, (obj, 5)])`` is equivalent to ``[obj, (obj, 5, 0, 0, 0, 0, 0)]``.

    All objects share the single *dataRefs* list (exactly as in
    :func:`createInstance`); the data you later pass to :func:`instanceSetPosition`
    fills one shared block used by every object. The set of objects is fixed at
    creation --- you cannot add or remove objects later. Destroy the instance with
    :func:`destroyInstance`.

    The *coordinateSpace* selects how the positions you pass to
    :func:`instanceSetPosition` are interpreted: world space (the classic
    behavior), relative to an aircraft's CG and body axes (interior or exterior),
    or relative to the camera. You can also change it later with
    :func:`instanceSetCoordinateSpace`.

    In the following example, we load a single pole object, then combine three
    copies into a single *instance* which we'll then place at the current aircraft position.
    Because ``coordinateSpace`` is World, the instances are stationary.
    
    >>> objs = xp.getObjects('lib/airport/Common_Elements/Markers/Poles/Thin_Red_White.obj')
    >>> obj = objs[0]
    >>> instance = xp.createInstanceEx([obj, (obj, 5.0, 0, 0, 0, 0, 0), (obj, -5.0, 0, 0, 0, 0, 0)],
    ...                                coordinateSpace=xp.CoordSpace_World)
    ...
    >>> x = xp.getDatad(xp.findDataRef('sim/flightmodel/position/local_x'))
    >>> y = xp.getDatad(xp.findDataRef('sim/flightmodel/position/local_y'))
    >>> z = xp.getDatad(xp.findDataRef('sim/flightmodel/position/local_z'))
    >>> position = (x, y, z, 0, 0, 0)
    >>> xp.instanceSetPosition(instance, position)

    `Official SDK <https://developer.x-plane.com/sdk/XPLMInstance/#XPLMCreateInstanceEx>`__ :index:`XPLMCreateInstanceEx`

.. py:function:: instanceSetCoordinateSpace(instance, space, aircraftIndex=0) -> None

    :param XPLMInstanceRef instance: Instance to modify
    :param XPLMCoordinateSpace_t space: One of the :data:`CoordSpace_World`, :data:`CoordSpace_AircraftInterior`, :data:`CoordSpace_AircraftExterior`, :data:`CoordSpace_Camera` constants
    :param int aircraftIndex: Aircraft index (0 = user aircraft), used only for the two aircraft coordinate spaces

    Change the coordinate space used to interpret the positions you pass to
    :func:`instanceSetPosition`. You can set this once up front with
    :func:`createInstanceEx`, or change it on the fly here (new with SDK 440).

    Changing the space does **not** make the instance jump: X-Plane re-expresses
    the instance's current world location in the new space, so the object stays
    exactly where it is and then begins tracking the new parent. After the change
    it is up to you to feed positions that are correct for the new space.

    Auto-shift (:func:`instanceSetAutoShift`) is independent of the coordinate
    space, but only has an effect while the instance is in world space.

    Assume the example provided with :func:`createInstanceEx` above, with three objects
    set in World coordinates. Now, change the coordinate space to AircraftExterior and
    set position (x, y, z) to (0, 0, 0) and the set of objects will be tied to the
    user aircraft's center of gravity, moving as the aircraft moves.

    >>> xp.instanceSetCoordinateSpace(instance, xp.CoordSpace_AircraftExterior)
    >>> xp.instanceSetPosition(instance, (0, 0, 0, 0, 0, 0))

    Finally, change the coordinate space to Camera, using the same position,
    and the set of objects will hover in front of your view... no matter where you look!

    >>> xp.instanceSetCoordinateSpace(instance, xp.CoordSpace_Camera)


    `Official SDK <https://developer.x-plane.com/sdk/XPLMInstance/#XPLMInstanceSetCoordinateSpace>`__ :index:`XPLMInstanceSetCoordinateSpace`

Constants
---------

.. _XPLMCoordinateSpace_t:

XPLMCoordinateSpace_t
*********************

.. py:type:: XPLMCoordinateSpace_t

The coordinate space in which an instance's positions are interpreted, passed to
:func:`createInstanceEx` and :func:`instanceSetCoordinateSpace` (new with SDK 440).
Interior and exterior aircraft spaces use the same transform today; the
distinction lets X-Plane light the objects correctly in a future release.

For ``xp.py``, these are defined without the leading ``xplm_`` e.g., ``xp.CoordSpace_World``.

.. py:data:: CoordSpace_World
   :value: 0

   Position is in global OpenGL / tangent-plane ("local") coordinates (the default). |BR|
   `Official SDK <https://developer.x-plane.com/sdk/XPLMInstance/#xplm_CoordSpace_World>`__: :index:`xplm_CoordSpace_World`

.. py:data:: CoordSpace_AircraftInterior
   :value: 1

   Position is relative to an aircraft's CG and body axes (+X right wing, +Y up,
   +Z tail), for objects inside the cockpit/cabin. |BR|
   `Official SDK <https://developer.x-plane.com/sdk/XPLMInstance/#xplm_CoordSpace_AircraftInterior>`__: :index:`xplm_CoordSpace_AircraftInterior`

.. py:data:: CoordSpace_AircraftExterior
   :value: 2

   Position is relative to an aircraft's CG and body axes (+X right wing, +Y up,
   +Z tail), for objects mounted on the exterior. |BR|
   `Official SDK <https://developer.x-plane.com/sdk/XPLMInstance/#xplm_CoordSpace_AircraftExterior>`__: :index:`xplm_CoordSpace_AircraftExterior`

.. py:data:: CoordSpace_Camera
   :value: 3

   Position is relative to the camera / view position and orientation. |BR|
   `Official SDK <https://developer.x-plane.com/sdk/XPLMInstance/#xplm_CoordSpace_Camera>`__: :index:`xplm_CoordSpace_Camera`

.. note:: The difference between :data:`CoordSpace_AircraftExterior` and :data:`CoordSpace_AircraftInterior`
  seems to how the object reacts to ambient light, using exterior or interior lights. Objects will be placed in the same
  location regardless of which value is used.

Types
-----

.. py:class:: XPLMInstanceRef

    Opaque capsule representing a drawable instance of an object, as returned by
    :func:`createInstance` and :func:`createInstanceEx`. Release it with
    :func:`destroyInstance`.
