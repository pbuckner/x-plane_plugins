xp
==

.. py:module:: xp

To use::

  from XPPython3 import xp

To use, add :code:`from XPPython3 import xp` at the top of you file, and all of the SDK API, from all modules
will be available to you.

* **Simplified Mapping**: To further reduce typing, function names and constants have been
  renamed (shortened) in a consistent manner.

* **Optional Arguments**: Some function arguments are optional as documented in this API.
  This further simplifies the interface, for example::

    xp.createFlightLoop(MyCallback)
  
  equates to::

    xp.createFlightLoop(MyCallback, 0, None)
  
* **Keyword Arguments**: In typical python fashion, function arguments can be
  specified using keywords (which makes options arguments even more useful). For example::

    xp.createFlightLoop(callback=MyCallback, refCon=None)

Simplified Mapping
------------------
This module provides simplified mappings to the original C / C++ named functions and constants.
The resulting API is the same -- same parameters, same return values, but
you won't be typing XPLM over-and-over.

Rules:

* Functions are camelCase. Leading XPLM, XP, XPUI are dropped

.. table::

 ================================== =======================
 Laminar SDK                        import xp
 ================================== =======================
 XPLMCamera.XPLMReadCameraPostion() xp.readCameraPosition()
 XPUIGraphics.XPDrawWindow()        xp.drawWindow()
 ================================== =======================

* Constants start with Capital letter. Leading xp\_, xplm\_, etc. are dropped

.. table::

 ======================================== =======================
 Laminar SDK                              import xp
 ======================================== =======================
 XPLMCamera.xplm_ControlCameraForever     xp.ControlCameraForever
 XPLMDisplay.xplm_MouseUp                 xp.MouseUp
 XPStandardWidgets.xpProperty_ButtonState xp.Property_ButtonState
 XPWidgetDefs.xpMsg_Paint                 xp.Msg_Paint
 XPStandardWidgets.xpMsg_TextFieldChanged xp.Msg_TextFieldChanged
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

 from XPPython3 import xp
 x = xp.getDatad(xp.findDataRef('sim/flightmodel/position/local_x'))
 y = xp.getDatad(xp.findDataRef('sim/flightmodel/position/local_y'))
 z = xp.getDatad(xp.findDataRef('sim/flightmodel/position/local_z'))
 pitch, heading, roll = (1, 2, 3)
 position = (x, y, z, pitch, heading, roll)
 xp.instanceSetPosition(self.g_instance, position, [self.g_tire, 0.0])

Because we're not introducing a new API, you can intermix the two calling styles. :code:`xp.py` is located
in :code:`Resources/plugins/XPPython3/` directory, which will be in your python path for install plugins.

Optional Arguments
------------------
Where the XPPython3 documentation specifies default parameters values, you can leave out those parameters
and the function will still work.

For example, :py:func:`registerKeySniffer` normally takes three parameters: the callback function, a *before* indicator,
and a reference constant. The XPPython3 specification says:

 *registerKeySniffer(sniffer, before=0, refCon=None)*

This means if you do not specify the final two parameters, your call will get the indicated defaults::

  xp.registerKeySniffer(MyCallback)

Is identical to::

  xp.registerKeySniffer(MyCallback, 0, None)

Keyword Arguments
-----------------

Keyword arguments free you from a particular order of parameters, and
increases the readiblity of code::

  myRefCon = {'data': 'xxx'}
  xp.registerKeySniffer(MyCallback, refCon=myRefCon)

Is identical to::

  myRefCon = {'data': 'xxx'}
  xp.registerKeySniffer(MyCallback, 0, myRefCon)

This is especially useful for functions such as :py:func:`createWindowEx`, where we provide
a lot of default parameters, so you can create a window with just::

  xp.createWindowEx(draw=MyDraw)
  
instead of::

  xp.createWindowEx(100, 200, 200, 100, 0,
      xp.WindowDecorationRoundRectangle, xp.WindowLayerFloatingWindows,
      MyDraw,
      DummyClickCallback,
      DummyKeyCallback,
      DummyCursorCallback,
      DummyWheelCallback,
      DummyRightClickCallback,
      None)
