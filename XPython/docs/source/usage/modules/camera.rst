XPLMCamera
==========

To use:
::

   import XPLMCamera

.. automodule:: XPLMCamera
   :members: XPLMControlCamera, XPLMDontControlCamera, XPLMIsCameraBeingControlled, XPLMReadCameraPosition

Callback
-----------------------
.. autofunction:: XPLMCameraControl_f
                 
Types
--------------------

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
