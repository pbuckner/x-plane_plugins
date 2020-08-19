XPLMCamera
==========

To use:
::

   import XPLMCamera

Theory Of Operation
-------------------

The XPLMCamera APIs allow plug-ins to
control the camera angle in X-Plane. This has a number of applications,
including but not limited to:

 - Creating new views (including dynamic/user-controllable views) for the user.

 - Creating applications that use X-Plane as a renderer of scenery, aircrafts, or both.

The camera is controlled via six parameters: a location in OpenGL
coordinates and pitch, roll and yaw, similar to an airplane's position.
OpenGL coordinate info is described in detail in the :py:mod:`XPLMGraphics`
documentation; generally you should use the XPLMGraphics routines to
convert from world to local coordinates. The camera's orientation starts
facing level with the ground directly up the negative-Z axis (approximately
north) with the horizon horizontal. It is then rotated clockwise for yaw,
pitched up for positive pitch, and rolled clockwise around the vector it is
looking along for roll.

You control the camera either either until the user selects a new view or
permanently (the later being similar to how UDP camera control works). You
control the camera by registering a callback per frame from which you
calculate the new camera positions. This guarantees smooth camera motion.

Use the :py:mod:`XPLMDataAccess` APIs to get information like the position of the
aircraft, etc. for complex camera positioning.

.. Note:: if your goal is to move the virtual pilot in the cockpit, this API is
          not needed; simply update the datarefs for the pilot's head position.

For custom exterior cameras, set the camera's mode to an external view first
to get correct sound and 2-d panel bahavior.

.. module:: XPLMCamera

Functions
---------
.. index::
   pair: controlCamera; XPLMControlCamera
.. autofunction:: XPLMControlCamera
.. autofunction:: XPLMDontControlCamera
.. index::
   pair: isCameraBeingControlled; XPLMIsCameraBeingControlled
.. autofunction:: XPLMIsCameraBeingControlled
.. index::
   pair: readCameraPosition; XPLMReadCameraPosition
.. autofunction:: XPLMReadCameraPosition

Callback
-----------------------
.. index:: cameraControl_f
.. autofunction:: XPLMCameraControl_f
                 
Types
--------------------

.. index:: cameraPosition_t
.. autodata:: XPLMCameraPosition_t
              :annotation: list of floats [x, y, z, pitch, heading, roll, zoom]

Constants
------------------------

.. py:data:: XPLMCameraControlDuration

  Enumeration states how long you want to retain control of the camera.
  You can retain it indefinitely or until the user selects a new view.

  .. autodata:: xplm_ControlCameraUntilViewChanges
  .. autodata:: xplm_ControlCameraForever

Example
-------

::

    def myControlFunc(position, isLosingControl, refcon):
        if isLosingControl:
            xp.dontControlCamera()
        else:
            currentPostion = xp.readCameraPosition()
            position.clear()  # (it's an undefined list on entry)
            position.extend(currentPosition)
            position[4] = currentPosition[4] + 0.5  # heading.. spin camera clockwise .5 degrees each frame
            
            if position[heading] > 270.0:
                xp.dontControlCamera()  # and stop spin once we're facing West
        return 1
    
    xp.controlCamera(xp.ControlCameraUntilViewChanges, myControlFunc, 0)

See also `PI_Camera1.py <https://github.com/pbuckner/x-plane_plugins/raw/master/XPython/demos/PI_Camera1.py>`_ demo.
