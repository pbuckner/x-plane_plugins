XPLMInstance
============
.. py:module:: XPLMInstance
               
To use::

  import XPLMInstance

This API provides instanced drawing of X-Plane objects (.obj files). In
contrast to old drawing APIs, which required you to draw your own objects
per-frame, the instancing API allows you to simply register an OBJ for
drawing, then move or manipulate it later (as needed).

This provides one tremendous benefit: it keeps all dataref operations for
your object in one place. Because datarefs are main thread only, allowing
dataref access anywhere is a serious performance bottleneck for the
simulator---the whole simulator has to pause and wait for each dataref
access. This performance penalty will only grow worse as X-Plane moves
toward an ever more heavily multithreaded engine.

The instancing API allows X-Plane to isolate all dataref manipulations for
all plugin object drawing to one place, potentially providing huge
performance gains.

Here's how it works:

When an instance is created, it provides a list of all datarefs you want to
manipulate in for the OBJ in the future. This list of datarefs replaces the
ad-hoc collections of dataref objects previously used by art assets. Then,
per-frame, you can manipulate the instance by passing in a "block" of
packed floats representing the current values of the datarefs for your
instance. (Note that the ordering of this set of packed floats must exactly
match the ordering of the datarefs when you created your instance.)

Functions
---------

.. py:function:: XPLMCreateInstance(obj, datarefs) -> XPLMInstance:

    Registers an instance of an X-Plane object.

    :param int obj: Handle returned by :py:func:`XPLMLoadObject` or :py:func:`XPLMLoadObjectAsync`)
    :param datarefs: dataref names
    :type datarefs: list of strings                     
    :return: :ref:`XPLMInstanceRef`

    The following example loads an XP standard object using :py:func:`XPLMLookupObjects`, and creates
    an instance of it, with two datarefs::

      XPLMLookupObjects('lib/airport/vehicles/pushback/tug.obj', 0, 0, self.load_cb, self.g_object)
      drefs = ['sim/graphics/animation/ground_traffic/tire_steer_deg', 'foo/bar/ground']

      self.g_instance = XPLMCreateInstance(self.g_object, drefs)

.. py:function:: XPLMDestroyInstance(instance) -> None:

    Unregisters an instance.

    :param int instance: :ref:`XPLMInstanceRef` Handle returned from :py:func:`XPLMCreateInstance`


.. py:function:: XPLMInstanceSetPosition(instance, new_position, data) -> None:

    Updates both the position of the instance and all datarefs you registered
    for it.

    :param int instance: :ref:`XPLMInstanceRef` Handle returned from :py:func:`XPLMCreateInstance`
    :param new_position: list of six floats (x, y, z, pitch, heading, roll)
    :param data: list of floats (values of datarefs). Same length and order as number of datarefs provided with XPLMCreateInstance.

    The following example builds on the example in :py:func:`XPLMCreateInstance`, and sets the position
    of the instance, and sets values for each of the (two) datarefs. Note you have to always provide
    all values. You'll likely call this in your flight loop callback (*not* a draw callback)::

            # get or create values for position:
            x = XPLMGetDatad(XPLMFindDataRef('sim/flightmodel/position/local_x'))
            y = XPLMGetDatad(XPLMFindDataRef('sim/flightmodel/position/local_y'))
            z = XPLMGetDatad(XPLMFindDataRef('sim/flightmodel/position/local_z'))
            pitch, heading, roll = (0, 0, 0)
            position = (x, y, z, pitch, heading, roll)
    
            self.g_tire += 10.0  # rotate tire 10 degrees each time called.
            if self.g_tire > 45.0:
                self.g_tire -= 90.0

            # The first dataref (see XPLMCreateInstance example) "tire_steer_deg" positions the tires.
            # Passing new values for it will rotate the tires for the demo.
            XPLMInstanceSetPosition(self.g_instance, position, [self.g_tire, 0.0])

Types
-----

.. _XPLMInstanceRef:

XPLMInstanceRef
***************

   Opaque handle to an instance.          

