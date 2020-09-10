XPLMProcessing
==============
.. py:module:: XPLMProcessing

To use::

  import XPLMProcessing

This API allows you to get regular callbacks during the flight loop, the
part of X-Plane where the plane's position calculates the physics of
flight, etc. Use these APIs to accomplish periodic tasks like logging data
and performing I/O.

.. warning:: Do NOT use these callbacks to draw! You cannot draw during flight
 loop callbacks. Use the drawing callbacks (see :any:`XPLMDisplay` module for more info)
 for graphics.


Functions
---------

.. py:function:: XPLMFlightLoop_f(sinceLastCall, sinceLastFLightLoope, counter, refCon) -> interval:

    :param float sinceLastCall: Wall time (seconds) since your last callback
    :param float sinceLastFlightLoop: Wall time (seconds) since *any* flight loop was dispatched
    :param int counter: monotonically increasing counter, bumped once per flight loop dispatched from the sim.                                   
    :param object refCon: reference constant provided when callback was registered.
    :return:

      * 0= stop receving callbacks
      * >0 number of seconds until next callback
      * <0 number of flightloops until next callback

    :rtype: float

    This is your flight loop callback. Each time the flight loop is iterated
    through, you receive this call at the end. You receive a time since you
    were last called and a time since the last loop, as well as a loop counter.
    The 'phase' parameter is deprecated and should be ignored.

    Your return value controls when you will next be called. Return 0 to stop
    receiving callbacks. Pass a positive number to specify how many seconds
    until the next callback. (You will be called at or after this time, not
    before.) Pass a negative number to specify how many loops must go by until
    you are called. For example, -1.0 means call me the very next loop. Try to
    run your flight loop as infrequently as is practical, and suspend it (using
    return value 0) when you do not need it; lots of flight loop callbacks that
    do nothing lowers X-Plane's frame rate.

    Your callback will NOT be unregistered if you return 0; it will merely be
    inactive.

    The reference constant you passed to your loop is passed back to you.


.. py:function:: XPLMGetElapsedTime(None) -> seconds:

    This routine returns the elapsed time since the sim started up in floating point
    seconds. This is continues to count upward even if the sim is paused.

    .. warning:: XPLMGetElapsedTime is not a very good timer! It lacks precisiion in both its data type and its source.
                 Do not attempt to use it for time critical applications like network multiplayer.


.. py:function:: XPLMGetCycleNumber(None) -> counter:

    This routine returns an integer counter starting at zero for each sim cycle
    computed/video frame rendered.


.. py:function::  XPLMRegisterFlightLoopCallback(callback: callable, interval: float, refCon: object) -> None:

    :param callback: Your :py:func:`XPLMFlightLoop_f` callback function
    :param float interval:

       * 0= deactivate
       * >0 seconds
       * <0 flightLoops
    :param object refCon: Reference Constanct which will be passed to your callback.

    This routine registers your :py:func:`XPLMFlightLoop_f` flight loop callback.
    ``interval`` defines when you will be called. Pass in a positive number to specify seconds
    from registration time to the next callback. Pass in a negative number to indicate when you will
    be called (e.g. pass -1 to be called at the next cylcle). Pass 0 to not be
    called; your callback will be inactive.


.. py:function:: XPLMUnregisterFlightLoopCallback(callback: callable, refCon: object) -> None:

    :param callback: Your :py:func:`XPLMFlightLoop_f` callback function
    :param object refCon: Reference Constanct which was passed to your callback.

    This routine unregisters your flight loop callback. Do NOT call it from
    your flight loop callback. Once your flight loop callback is unregistered,
    it will not be called again.

    The input parameters must match the parameters provided with :py:func:`XPLMRegisterFlightLoopCallback`.


.. py:function:: XPLMSetFlightLoopCallbackInterval(callback: callable, interval: float, relativeToNow: int, refCon: object) -> None:

    :param callback: Your :py:func:`XPLMFlightLoop_f` callback function
    :param float interval:

       * 0= deactivate
       * >0 seconds
       * <0 flightLoops
    :param int relativeToNow: 1= ``interval`` is relative to now, otherwise relative to last time called or time registered

    This routine sets when a callback will be called. Do NOT call it from your
    callback; use the return value of the callback to change your callback
    interval from inside your callback.

    ``interval`` is formatted the same way as in :py:func:`XPLMRegisterFlightLoopCallback`;
    positive for seconds, negative for cycles, and 0 for deactivating the
    callback. If ``relativeToNow`` is 1, times are from the time of this call;
    otherwise they are from the time the callback was last called (or the time
    it was registered if it has never been called.

    .. note:: This does not register or change your callback, it merely changes the timing of
              the next call. Your callback and refCon much match values registered.


.. py:function:: XPLMCreateFlightLoop(params: list) -> flightLoopID:
   
  :param params: list

     * phase: :ref:`XPLMFlightLoopPhaseType` indicates phase when to invoke the callback
     * callback: :py:func:`XPLMFlighLoop_f` callback function
     * refcon: object Reference Constant

  :return: :py:data:`XPLMFlightLoopID`

  This routine creates a flight loop callback and returns its ID. The flight
  loop callback is created using the input param struct, and is inited to be
  unscheduled.


.. py:function::  XPLMDestroyFlightLoop(flightLoopID: int) -> None:

    This routine destroys a flight loop callback by :py:data:`XPLMFLightLoopID`.
    Only call it on flight loops created with :py:func:`XPLMCreateFlighLoop`.


.. py:function::  XPLMScheduleFlightLoop(flightLoopID: int, interval: float, relativeToNow: int) -> None:

    This routine schedules a flight loop callback for future execution. It
    uses a :py:data:`XPLMFlightLoopID` as obtained from :py:func:`XPLMCreateFlightLoop`.
    If interval is negative, it is run in a certain number of frames based on
    the absolute value of the input. If the interval is positive, it is a
    duration in seconds.

    If relativeToNow is 1, times are interpretted relative to the time this
    routine is called; otherwise they are relative to the last call time or the
    time the flight loop was registered (if never called).

    .. note:: **THREAD SAFETY**:
     it is legal to call this routine from any thread under the
     following conditions:

     1. The call must be between the beginning of an XPLMEnable and the end of
        an XPLMDisable sequence. (That is, you must not call this routine from
        thread activity when your plugin was supposed to be disabled. Since plugins
        are only enabled while loaded, this also implies you cannot run this
        routine outside an XPLMStart/XPLMStop sequence.)

     2. You may not call this routine re-entrantly for a single flight loop ID.
        (That is, you can't enable from multiple threads at the same time.)

     3. You must call this routine between the time after XPLMCreateFlightLoop
        returns a value and the time you call XPLMDestroyFlightLoop. (That is, you
        must ensure that your threaded activity is within the life of the object.
        The SDK does not check this for you, nor does it synchronize destruction of
        the object.)

     4. The object must be unscheduled if this routine is to be called from a
        thread other than the main thread.

Constants
---------

.. py:data:: XPLMFlightLoopID

  Opaque identifier for a flight loop callback. You can use this identifier to easily track and
  remove your callbacks or to use with new flight loop APIs.

.. _XPLMFlightLoopPhaseType:

XPLMFlightLoopPhaseType
***********************

You can register a flight loop callback to run either before or after the
flight model is integrated by X-Plane.

 .. py:data:: xplm_FlightLoop_Phase_BeforeFlightModel

   Your callback runs before X-Plane integrates the flight model.

 .. py:data:: xplm_FlightLoop_Phase_AfterFlightModel

   Your callback runs after X-Plane integrates the flight model.


        
