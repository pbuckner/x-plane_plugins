XPLMProcessing
==============
.. py:module:: XPLMProcessing
.. py:currentmodule:: xp

To use::

  import xp

This API allows you to get regular callbacks during the flight loop, the
part of X-Plane where the plane's position calculates the physics of
flight, etc. Use these APIs to accomplish periodic tasks like logging data
and performing I/O.

.. warning:: **Do not** use these callbacks to draw! You cannot draw during flight
 loop callbacks. Use the drawing callbacks (see :any:`XPLMDisplay` module for more info)
 for graphics.


Timing Functions
----------------

.. py:function:: getElapsedTime()

    :return float: elapsed time in seconds

    Return the elapsed time since the sim started up, in floating point
    seconds. This is continues to count upward even if the sim is paused.

    Value *does not* reset on new flight, only on simulator re-start.

    .. warning:: :py:func:`getElapsedTime` is not a very good timer! It lacks precision in both its data type and its source.
                 Do not attempt to use it for time critical applications like network multiplayer.

    >>> xp.getElapsedTime()
    196.7588833

    `Official SDK <https://developer.x-plane.com/sdk/XPLMProcessing/#XPLMGetElapsedTime>`__ :index:`XPLMGetElapsedTime`

.. py:function:: getCycleNumber()

    :return int: count

    Return an integer counter starting at zero for each sim cycle computed/video frame rendered. Value *does not* reset on new flight, only
    on simulator re-start.

    >>> xp.getCycleNumber()
    29776

    `Official SDK <https://developer.x-plane.com/sdk/XPLMProcessing/#XPLMGetCycleNumber>`__ :index:`XPLMGetCycleNumber`

FlightLoop Functions
--------------------

There are two sets of flight loop functions. The "new" way is a bit simpler, and allows you to specify phase.

+--------------------------------------+----------------------------------------+----------------------------------------+
| Task                                 |New (flightLoopID)                      |Old (callback + refCon)                 |
|                                      |                                        |                                        |
+======================================+========================================+========================================+
| Create / Register                    |:py:func:`createFlightLoop`             |:py:func:`registerFlightLoopCallback`   |
+--------------------------------------+----------------------------------------+----------------------------------------+
| Reschedule                           |:py:func:`scheduleFlightLoop`           |:py:func:`setFlightLoopCallbackInterval`|
+--------------------------------------+----------------------------------------+----------------------------------------+
| Check Validity                       |:py:func:`isFlightLoopValid`            | not available                          |
+--------------------------------------+----------------------------------------+----------------------------------------+
|Destroy / Unregister                  |:py:func:`destroyFlightLoop`            |:py:func:`unregisterFlightLoopCallback` |
+--------------------------------------+----------------------------------------+----------------------------------------+
 
FlightLoop - New Style
++++++++++++++++++++++

.. py:function:: createFlightLoop(callback, phase=0, refCon=None)
   
  :param function callback: function to be called based on schedule
  :param XPLMFlightLoopPhaseType phase: flag to run before or after X-Plane integrates flight model.
  :param Any refCon: reference constant passed to your callback function
  :return: :data:`XPLMFlightLoopID` capsule                

  Create a flightloop callback and return a :data:`XPLMFlightLoopID` which
  can be used to change or destroy it.

  The flight loop callback is initialized as unscheduled: You'll need
  to call :func:`scheduleFlightLoop`.

  Your *callback* function takes four parameters and must return an interval.
  `Official SDK <https://developer.x-plane.com/sdk/XPLMProcessing/#XPLMFlighLoop_f>`__ :index:`XPLMFlighLoop_f`
    
  .. py:function:: callback(sinceLast, elapsedTime, counter, refCon)
            
        :param float sinceLast: wall time (seconds) since your last callback.
        :param float elapsedTime: wall time (seconds) since start of sim.
        :param int counter: monontonically increasing counter                          
        :param Any refCon: reference constant provided with :py:func:`createFlightLoop`
        :return float: "next" interval to execute.
                                          
        *elapsedTime*  appears to be identical to current value of :py:func:`getElapsedTime`.

        *counter* is bumped once per flight loop dispatched from the sim.
        It appears to be identical to the current cycle (:py:func:`getCycleNumber`). Note that
        "cycle" rate and "flightloop" rate are not the same. Commonly, two cycles are consumed
        between calls to the flight loop.

        Your callback must return a floating point value for the "next" interval (This is identical to value
        in :py:func:`scheduleFlightLoop`):
        
          * 0= stop receving callbacks
          * >0 number of seconds until next callback
          * <0 number of flightloops until next callback
    
        .. note:: Laminar documentation indicates the second parameter to the callback is
                *inElapsedTimeSinceLastFlightLoop*: the wall time since any flight loop was dispatched.
                This is not correct: it is total sim elapsed time
                independent of your callback.
    
  Try to run your flight loop as *infrequently* as is practical, and suspend it (using
  return value 0) when you do not need it; lots of flight loop callbacks that
  do nothing lowers X-Plane's frame rate.
                     
  Your callback will NOT be unregistered if you return 0; it will merely be
  inactive.

  >>> def MyCallback(lastCall, elapsedTime, counter, refCon):
  ...    xp.log(f"{elapsedTime}, {counter}")
  ...    return 1.0
  ...
  >>> myRefCon = {'data': []}
  >>> flightLoopID = xp.createFlightLoop(MyCallback, refCon=myRefCon)
  >>> flightLoopID
  <capsule object "XPLMFlightLoopID" at 0x7fa89eb15720>
  >>> xp.scheduleFlightLoop(flightLoopID, -1)
  >>> xp.destroyFlightLoop(flightLoopID)

  
  To mimic the C-API, you can alternatively call :py:func:`createFlightLoop` with a single, three-element tuple
  matching the :py:func:`createFlightLoop` parameters described above::

    params = (
      phase,
      callback,
      refCon
    )

  >>> myRefCon = {'data': []}
  >>> flightLoopID = xp.createFlightLoop([0, MyCallback, myRefCon])

  `Official SDK <https://developer.x-plane.com/sdk/XPLMProcessing/#XPLMCreateFlightLoop>`__ :index:`XPLMCreateFlightLoop`

.. py:function::  destroyFlightLoop(flightLoopID)

  :param XPLMFlightLoopID flightLoopID: Only call it on flight loops created with :py:func:`createFlightLoop`.

  This routine destroys a flight loop callback specified by *flightLoopID*.

  >>> flightLoopID = xp.createFlightLoop(MyCallback)
  >>> xp.destroyFlightLoop(flightLoopID)

  `Official SDK <https://developer.x-plane.com/sdk/XPLMProcessing/#XPLMDestroyFlightLoop>`__ :index:`XPLMDestroyFlightLoop`   

.. py:function::  scheduleFlightLoop(flightLoopID, interval, relativeToNow=1)

  :param XPLMFlightLoopID flightLoopID: FlightLoop to schedule
  :param float interval: number of seconds, or (if negative) number of frames to wait
  :param int relativeToNow: 1= wait interval relative to now, otherwise wait relative start of sim.                         
  :return: None

  Set the interval associated with your *flightLoopID* received from :py:func:`createFlightLoop`.

  *interval* is set to:
      +----------------+---------------------------------------------+
      |Positive number |indicates seconds from registration time to  |
      |                |the next callback.                           |
      |                |                                             |
      |                |For example pass 10 to be called             |
      |                |(approximately) 10 seconds from now.         |
      +----------------+---------------------------------------------+
      |Negative number |indicates number of FlightLoops to next      |
      |                |callback.                                    |
      |                |                                             |
      |                |For example pass -1 to be called at the next |
      |                |cycle                                        |
      +----------------+---------------------------------------------+
      |Zero            |Deactivate flight loop. Flight loop remains  |
      |                |registered, but is not called.               |
      |                |                                             |
      +----------------+---------------------------------------------+

  If *relativeToNow* is 1, the *interval* value is relative to now, e.g., *interval*\=10.0
  indicates run the callback ten seconds from "now". Otherwise, time (or #flight loops) is
  relative the previous execution of this callback (or when it was created/registered, if not yet ever
  run).

  Note that this schedules the flight loop *once*. Your flight loop callback, itself, returns
  the *next* value for interval. (This next value is always *relativeToNow*, you cannot return
  a value to indicate at some future sim time.)

  >>> flightLoopID = xp.createFlightLoop(MyCallback)
  >>> xp.scheduleFlightLoop(flightLoopID, interval=10)

  .. note:: **THREAD SAFETY**:
     it is legal to call this routine from any thread under the
     following conditions:

     1. The call must be between the beginning of an XPluginEnable and the end of
        an XPluginDisable sequence. (That is, you must not call this routine from
        thread activity when your plugin was supposed to be disabled. Since plugins
        are only enabled while loaded, this also implies you cannot run this
        routine outside an XPluginStart/XPluginStop sequence.)

     2. You may not call this routine re-entrantly for a single flight loop ID.
        (That is, you can't enable from multiple threads at the same time.)

     3. You must call this routine between the time after :py:func:`createFlightLoop`
        returns a value and the time you call :py:func:`destroyFlightLoop`. (That is, you
        must ensure that your threaded activity is within the life of the object.
        The SDK does not check this for you, nor does it synchronize destruction of
        the object.)

     4. The object must be unscheduled if this routine is to be called from a
        thread other than the main thread.

  `Official SDK <https://developer.x-plane.com/sdk/XPLMProcessing/#XPLMScheduleFlightLoop>`__ :index:`XPLMScheduleFlightLoop`
  
.. py:function:: isFlightLoopValid(flightLoopID)

   :param XPLMFlightLoopID flightLoopID: as from :py:func:`createFlightLoop`                
   :return bool: True if valid flightLoopID

   Returns True if flightLoopID is valid and known: it may or may not be scheduled.

   This is an XPPython3-only function.                 

FlightLoop - Old Style
++++++++++++++++++++++

.. py:function::  registerFlightLoopCallback(callback, interval=0.0, refCon=None)

   :param Callable callback: your flight loop callback                  
   :param float interval: number of seconds, or (if negative) number of frames to wait
   :param Any refCon: reference constant passed to callback   
   :return: None                      

   Register, and schedule, your flight loop callback, see :py:func:`createFlightLoop` for information
   about the callback function.

   *interval* is defines when you will be called next:
       * 0= deactivate
       * >0 seconds
       * <0 flightLoops

   >>> def MyCallback(lastCall, elapsedTime, counter, refCon):
   ...    xp.log(f"{elapsedTime}, {counter}")
   ...    return 1.0
   ...
   >>> xp.registerFlightLoopCallback(MyCallback, interval=-1)
    
   `Official SDK <https://developer.x-plane.com/sdk/XPLMProcessing/#XPLMRegisterFlightLoopCallback>`__ :index:`XPLMRegisterFlightLoopCallback`
    
.. py:function:: unregisterFlightLoopCallback(callback, refCon=None)

    :param Callable callback: your flight loop callback                  
    :param Any refCon: reference constant passed to callback   
    :return None:                      

    This routine unregisters your flight loop *callback*. *refCon* must
    match value provided with :py:func:`registerFlightLoopCallback` as we
    use it to find and match the callback.

    You cannot use this function to remove callbacks created with :func:`createFlightLoop`, use :func:`destroyFlightLoop`.

    Do NOT call :py:func:`unregisterFlightLoopCallback` from
    within your flight loop callback. (Set interval to zero instead.)

    Once your callback is unregistered, it will not be called again.

    >>> myRefCon = {'data': []}
    >>> xp.registerFlightLoopCallback(MyCallback, interval=-1, refCon=myRefCon}
    >>> xp.unregisterFlightLoopCallback(MyCallback, myRefCon)

    `Official SDK <https://developer.x-plane.com/sdk/XPLMProcessing/#XPLMUnregisterFlightLoopCallback>`__ :index:`XPLMUnregisterFlightLoopCallback`
    
.. py:function:: setFlightLoopCallbackInterval(callback, interval, relativeToNow=1, refCon=None)

    :param Callable callback: your flight loop callback                  
    :param float interval: number of seconds, or (if negative) number of frames to wait
    :param int relativeToNow: 1= wait interval relative to now, otherwise wait relative start of sim.                         
    :param Any refCon: reference constant passed to callback   
    :return None:                      

    Change the interval asscociated with your *callback*. (Must have already
    been registered with :py:func:`registerFlightLoopCallback`.)

    This is equivalent to :py:func:`scheduleFlightLoop`, but uses *callback* + *refCon*
    to internally locate your callback.
    
    You cannot use this function to schedule callbacks created with :func:`createFlightLoop`, use :func:`scheduleFlightLoop`.

    **Do not** call :py:func:`setFlightLoopCallbackInterval` from your
    callback; use the return value of the callback to change your callback
    interval from inside your callback.

    .. note:: This does not register or change your callback, it merely changes the timing of
              the next call. Your *callback* and *refCon* much match values registered.

    >>> def MyCallback(lastCall, elapsedTime, counter, refCon):
    ...    xp.log(f"{elapsedTime}, {counter}")
    ...    return 1.0
    ...
    >>> myRefCon = {'data': []}
    >>> xp.registerFlightLoopCallback(MyCallback, interval=0, refCon=myRefCon)
    >>> xp.setFlightLoopCallbackInterval(MyCallback, interval=10, refCon=myRefCon)

    `Official SDK <https://developer.x-plane.com/sdk/XPLMProcessing/#XPLMSetFlightLoopCallbackInterval>`__ :index:`XPLMSetFlightLoopCallbackInterval`

Constants
---------

.. py:data:: XPLMFlightLoopID

  Opaque identifier for a flight loop callback. You can use this identifier to easily track and
  remove your callbacks using new flight loop APIs.

  `Official SDK <https://developer.x-plane.com/sdk/XPLMProcessing/#XPLMFlightLoopID>`__ :index:`XPLMFlightLoopID`

.. _XPLMFlightLoopPhaseType:

XPLMFlightLoopPhaseType
+++++++++++++++++++++++

You can register a flight loop callback to run either before or after the
flight model is integrated by X-Plane.

 .. py:data:: FlightLoop_Phase_BeforeFlightModel
   :value: 0

   Your callback runs before X-Plane integrates the flight model.

   `Official SDK <https://developer.x-plane.com/sdk/XPLMProcessing/#xplm_FlightLoop_Phase_BeforeFlightModel>`__ :index:`xplm_FlightLoop_Phase_BeforeFlightModel`
   
 .. py:data:: FlightLoop_Phase_AfterFlightModel
   :value: 1

   Your callback runs after X-Plane integrates the flight model.

   `Official SDK <https://developer.x-plane.com/sdk/XPLMProcessing/#xplm_FlightLoop_Phase_AfterFlightModel>`__ :index:`xplm_FlightLoop_Phase_AfterFlightModel`
