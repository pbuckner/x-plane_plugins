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

.. py:function:: createInstance(obj, dataRefs=None)

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

.. py:function:: destroyInstance(instance)

    Unregisters an instance (as returned from :py:func:`createInstance`.) You are still responsible for
    eventually releasing the *Object* using :py:func:`unloadObject`.

    >>> xp.destroyInstance(instance)

    `Official SDK <https://developer.x-plane.com/sdk/XPLMInstance/#XPLMDestroyInstance>`__ :index:`XPLMDestroyInstance`

.. py:function:: instanceSetPosition(instance, position, data=None)

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

    The following example builds on the example in :py:func:`createInstance`, and sets the position
    of the instance, and sets values for each of the (two) datarefs.
    You'll likely call this in your flight loop callback (*not* a draw callback):

    >>> # get current aircraft position
    >>> x = xp.getDatad(xp.findDataRef('sim/flightmodel/position/local_x'))
    >>> y = xp.getDatad(xp.findDataRef('sim/flightmodel/position/local_y'))
    >>> z = xp.getDatad(xp.findDataRef('sim/flightmodel/position/local_z'))
    >>> pitch, heading, roll = (0, 0, 0)
    >>> # Place the tug in front of the aircraft (just so you can see it)
    >>> position = (x, y-1, z+10, pitch, heading+90, roll)
    >>> # Set wheel steer degrees to 0, then 20, 40, pausing in between.
    >>> xp.instanceSetPosition(instance, position, [0, 0.0])
    >>> xp.instanceSetPosition(instance, position, [20, 0.0])
    >>> xp.instanceSetPosition(instance, position, [40, 0.0])


    `Official SDK <https://developer.x-plane.com/sdk/XPLMInstance/#XPLMInstanceSetPosition>`__ :index:`XPLMInstanceSetPosition`
