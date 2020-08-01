.. automodule:: xp
   :members:             

xp
============

This module provides simplified mappings to the original C / C++ named functions and constants.
The resulting API is the same -- same parameters, same return values, but
you won't be typing XPLM over-and-over.

To use, :code:`import xp` at the top of you file, and all of the SDK API, from all modules
will be available to you. To further reduce typing, function names and constants have been
renamed (shortened) in a consistent manner:


Rules:

* Functions are camelCase. Leading XPLM, XP, XPUI are dropped

.. table::

 ================================== =======================
 Laminar SDK                        import xp
 ================================== =======================
 XPLMCamera.XPLMReadCameraPostion() readCameraPosition()
 XPUIGraphics.XPDrawWindow()        drawWindow()
 ================================== =======================

* Constants start with Capital letter. Leading xp\_, xplm\_, etc. are dropped

.. table::

 ======================================== =======================
 Laminar SDK                              import xp
 ======================================== =======================
 XPLMCamera.xplm_ControlCameraForever     ControlCameraForever
 XPLMDisplay.xplm_MouseUp                 MouseUp
 XPStandardWidgets.xpProperty_ButtonState Property_ButtonState
 XPWidgetDefs.xpMsg_Paint                 Msg_Paint
 XPStandardWidgets.xpMsg_TextFieldChanged Msg_TextFieldChanged
 ======================================== =======================

Compare:

::

 import XPLMDataAccess
 import XPLMInstance

 x = XPLMDataAccess.XPLMGetDatad(XPLMDataAccess.XPLMFindDataRef('sim/flightmodel/position/local_x'))
 y = XPLMDataAccess.XPLMGetDatad(XPLMDataAccess.XPLMFindDataRef('sim/flightmodel/position/local_y'))
 z = XPLMDataAccess.XPLMGetDatad(XPLMDataAccess.XPLMFindDataRef('sim/flightmodel/position/local_z'))
 pitch, heading, roll = (1, 2, 3)
 position = (x, y, z, pitch, heading, roll)
 XPLMInstance.XPLMInstanceSetPosition(self.g_instance,
                                     position,
                                     [self.g_tire, 0.0])

with:

::

 import xp
 x = xp.getDatad(xp.findDataRef('sim/flightmodel/position/local_x'))
 y = xp.getDatad(xp.findDataRef('sim/flightmodel/position/local_y'))
 z = xp.getDatad(xp.findDataRef('sim/flightmodel/position/local_z'))
 pitch, heading, roll = (1, 2, 3)
 position = (x, y, z, pitch, heading, roll)
 xp.instanceSetPosition(self.g_instance, position, [self.g_tire, 0.0])

Because we're not introducing a new API, you can intermix the two calling styles. :code:`xp.py` is located
in :code:`Resources/plugins/XPPython3/` directory, which will be in your python path for install plugins.
