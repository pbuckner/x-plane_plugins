XPLMCamera
==========

To use:
::

   import xp

.. py:module:: XPLMCamera
.. py:currentmodule:: xp

Theory Of Operation
-------------------

The XPLMCamera APIs allow plug-ins to
control the camera angle in X-Plane. This has a number of applications,
including but not limited to:

 - Creating new views (including dynamic/user-controllable views) for the user.

 - Creating applications that use X-Plane as a renderer of scenery, aircrafts, or both.

The camera is controlled via six parameters: a location in OpenGL
coordinates (i.e., x, y and z) pitch, roll and yaw, similar to an airplane's position.
OpenGL coordinate info is described in detail in the :py:mod:`XPLMGraphics`
documentation; generally you should use the XPLMGraphics routines to
convert from world to local coordinates. The camera's orientation starts
facing level with the ground directly up the negative-Z axis (approximately
north) with the horizon horizontal. It is then rotated clockwise for yaw,
pitched up for positive pitch, and rolled clockwise around the vector it is
looking along for roll.

You control the camera either either until the user selects a new view or
permanently (the later being similar to how :doc:`UDP camera control<../udp/vehx>` works). You
control the camera by registering a callback per frame from which you
calculate the new camera positions. This guarantees smooth camera motion.

Use the :py:mod:`XPLMDataAccess` APIs to get information like the position of the
aircraft, etc. for complex camera positioning.

.. Note:: if your goal is to move the virtual pilot in the cockpit, this API is
          not needed; simply update the datarefs for the pilot's head position.

For custom exterior cameras, set the camera's mode to an external view first
to get correct sound and 2-d panel behavior.


Functions
---------

.. py:function:: controlCamera(howLong=ControlCameraUntilViewChanges, controlFunc=None, refCon=None)

    :param int howLong: :data:`ControlCameraUntilViewChanges` or :data:`ControlCameraForever`
    :param Callable controlFunc: your callback called during draw cycle (See below).
    :param Any refCon: Reference constant passed to your callback
    :return: None

    Repositions the camera on the next drawing cycle.

    *howLong* is either ``ControlCameraUntilViewChanges`` which controls until the view is set by some other means, or
    ``ControlCameraForever`` which controls until the plugin exits, releases control (:py:func:`dontControlCamera`)
    or control is forcibly taken by another plugin.

    *controlFunc* is your callback, called during draw cycle. It will be called with three parameters:
      * **position**: list of seven floats
          - x, y, z (OpenGL position)
          - pitch
          - heading
          - roll
          - zoom

        Update one or more values in the list to change position, and return 1. Zero return indicates
        you do not want to change position, *and you relinquish control*.

        .. note:: Laminar says *position* list values are undefined on input, so you should call :py:func:`readCameraPosition`
                  and update the position accordingly. (In practice, it appears *position* is initialized
                  with the correct information, but this `is not guaranteed`.)

                  *Do not* create a **new** *position* list, you need to update the container provided, so the
                  calling function has the same pointer to the data structure:

                  ::

                     # THIS WILL FAIL -- the value for 'position' is changed
                     def myControlFunc(position, isLosingControl, refCon):
                         position = xp.readCameraPosition()
                         position[4] = position[4] + 0.5
                         ...

                     # THIS WORKS -- the value is unchanged, the contents are changed
                     def myControlFunc(position, isLosingControl, refCon):
                         position.clear()
                         position.extend(xp.readCameraPosition())
                         position[4] = position[4] + 0.5
                         ...

      * **isLosingControl**:
          - 1 if you are losing control, 0 otherwise
      * **refCon**:
          - reference constant you provided with call to `controlCamera()`

    The final *refCon* is a reference constant passed to your *controlFunc()*. :py:func:`controlCamera` requires only the *controlFunc* parameter.
    ::

       >>> def myControlFunc(position, isLosingControl, refCon):
       ...     if isLosingControl:
       ...         xp.dontControlCamera()
       ...         return 0
       ...     currentPosition = xp.readCameraPosition()
       ...     position.clear()  # (it's an undefined list on entry)
       ...     position.extend(currentPosition)
       ...     position[4] = currentPosition[4] + 0.5  # i.e., 'heading'.. spin camera .5 degrees @ frame
       ...
       ...     if position[4] > 270.0:
       ...         xp.dontControlCamera()  # and stop spin and release camera once we're facing West
       ...         return 0
       ...     return 1
       ...
       >>> xp.controlCamera(controlFunc=myControlFunc)

    `Official SDK <https://developer.x-plane.com/sdk/XPLMCamera/#XPLMControlCamera>`__: :index:`XPLMControlCamera`

.. py:function:: dontControlCamera()

    :return: None

    Releases control of camera. (See :py:func:`controlCamera`). You should not use this routine unless
    you have possession of the camera. (See :py:func:`isCameraBeingControlled`).

    (If you call this, X-Plane *will not* call your camera control function with `isLosingControl=1`: it will
    never call your control function again.)

    `Official SDK <https://developer.x-plane.com/sdk/XPLMCamera/#XPLMDontControlCamera>`__: :index:`XPLMDontControlCamera`

.. py:function:: isCameraBeingControlled()
                 
    :return: Tuple [isBeingControlled: int, howLong: int]

    Returns a two element tuple (*isBeingControlled*, *howLong*)
    
    *isBeingControlled* is 1 if the camera is being controlled (by anyone), 0 otherwise.
    If first element is non-zero, second element matches *howLong* value provided to :py:func:`controlCamera`.

    ::

       >>> xp.controlCamera(howLong=xp.ControlCameraForever, controlFunc=myControlFunc)
       >>> xp.isCameraBeingControlled()
       (1, 2)
       >>> xp.controlCamera(howLong=xp.ControlCameraUntilViewChanges, controlFunc=myControlFunc)
       >>> xp.isCameraBeingControlled()
       (1, 1)
       >>> xp.dontControlCamera()
       >>> xp.isCameraBeingControlled()
       (0, 32644)

    In the above ``controlCamera`` examples, both will cause the camera to spin 270 degrees and then stop. The difference is
    with the second example, you can stop the spin early by switching to a different view (e.g., Shift-8 View-External-Chase).
    The first example will not switch to a different view until the callback as relinquished control (at 270 degrees).
    
    `Official SDK <https://developer.x-plane.com/sdk/XPLMCamera/#XPLMIsCameraBeingControlled>`__: :index:`XPLMIsCameraBeingControlled`

.. py:function:: readCameraPosition()
                 
   :return: Tuple of seven floats

   Returns current camera position, a tuple of seven floats:

    ===== ========= ===========================================================
    Index Value     Meaning
    ===== ========= ===========================================================
    0,1,2 *x, y, z* camera's position in OpenGL coordinates
    3     *pitch*   In degrees, 0.0 is flat, positive for nose up.
    4     *heading* In degrees, 0.0 is true north
    5     *roll*    In degrees, 0.0 is flat, positive for roll right
    6     *zoom*    1.0 is normal, 2.0 is 2x zoom (objects appear larger), etc.
    ===== ========= ===========================================================

   You do not need to control the camera in order to read its position::

     >>> xp.readCameraPosition()
     (-22567.63552, 94.40988, 4305.8530, 2.407667, 180.40255, -0.86660, 1.0)

   
   `Official SDK <https://developer.x-plane.com/sdk/XPLMCamera/#XPLMReadCameraPosition>`__: :index:`XPLMReadCameraPosition`
    

Constants
---------

Control Camera Duration
.......................

Enumeration states how long you want to retain control of the camera.
You can retain it indefinitely or until the user selects a new view.
Used by :py:func:`controlCamera` and :py:func:`isCameraBeingControlled`.
You can relinquish control by calling :py:func:`dontControlCamera` or returning
zero from you :py:func:`controlCamera` callback function.
    

    .. py:data:: ControlCameraUntilViewChanges
        :value: 1
    
        Camera is controlled until the view is changes (e.g., user requests "External View" from X-Plane menu.)

        `Official SDK <https://developer.x-plane.com/sdk/XPLMCamera/#xplm_ControlCameraUntilViewChanges>`__: :index:`xplm_ControlCameraUntilViewChanges`
    
    .. py:data:: ControlCameraForever
        :value: 2
    
        Camera is controlled forever.

        `Official SDK <https://developer.x-plane.com/sdk/XPLMCamera/#xplm_ControlCameraForever>`__: :index:`xplm_ControlCameraForever`
    
Example
-------
See also :doc:`PI_Camera1.py<../samples>` demo.
