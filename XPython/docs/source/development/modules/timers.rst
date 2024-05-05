Easy Timers
===========

.. warning:: This is proposed documentation, for review only.

..
  py:module:: timers

To use:
::

   from XPPython3.utils import timers

X-Plane SDK uses flight loop callbacks to schedule events in the future. Xlua
provides an simple wrapper which defines some easy-to-use timers. XPPython3
also supports this interface.

Timers are keyed by their callback funtion: any particular function can
have only one timer active at a time, though that timer may repeat, and you
are permitted an unlimited number of timers.

The callback function, executed on timer expiration requires no parameters and
should not return any value. As it is executed within a flightloop, it should be
quick so as to not delay the sim::

  >>> def timer_callback():
  ...    print(f"Current flight time is {xlua.FLIGHT_TIME}")
  ...

To start a timer, use:

* :py:func:`run_after_time`: execute callback once after a delay

* :py:func:`run_at_interval`: execute callback repeatedly, at given interval

* :py:func:`run_timer`: execute callback after a delay, and repeatedly at interval.

To query if a timer is active use:

* :py:func:`is_timer_scheduled`

Repeating timers, and timers on a delay can be stopped using:

* :py:func:`stop_timer`

Note that timers are `paused` while the simulator is paused, and they will run faster if
the sim flight model is executing faster (e.g., command ``sim/operation/flightmodel_speed_change``
or dataref ``sim/time/sim_speed``). Change in ground-speed multiplier does `not` change timer
execution.


Functions
---------

.. py:function:: run_after_time(func, delay)

  Run function once after `delay` seconds. This does not return any value.
  You can stop this timer prior to execution if required by passing the same
  callback ``func`` to :py:func:`stop_timer`::

    >>> def deferred_flight_start():
    ...   print('Deferred flight start')
    ...   interior_lites_0.value = .4
    ...   interior_lites_1.value = 1
    ...   interior_lites_2.value = 1
    ...   interior_lites_3.value = 0.2
    ...   panel_glareshield_brightness.value = 0.3
    ...
    >>> run_after_time(deferred_flight_start, .1)
  
  This is implemented as ``run_timer(func, delay, 0)``.

.. py:function:: run_at_interval(func, interval)

  Run function until stopped, after each ``interval`` seconds. The
  `first` execution of the call back is `after` the initial interval.
  To have the first execution different from the interval, use :py:func:`run_timer`.
  This does not return any value. You can stop this timer by passing
  the same callback ``func`` to :py:func:`stop_timer`::
                 
    >>> def func():
    ...    print("Hello World")
    ...
    >>> run_at_interval(func, 5)
    Hello World
    Hello World
    >>> is_timer_scheduled(func)
    True
    >>> stop_timer(func)

  This is implemented as ``run_timer(func, interval, interval)``.

.. py:function:: run_timer(func, delay, interval)

  Run function until stopped. First execution is after ``delay`` seconds
  (which may be 0), and each subsequent execution is after ``interval``
  seconds. Tis does not return any value. You can stop this timer by passing
  the same callback ``func`` to :py:func:`stop_timer`::
   
    >>> run_timer(func, 0, 5)
    Hello World
    Hello World
    >>> is_timer_scheduled(func)
    True
    >>> stop_timer(func)
  
.. py:function:: is_timer_scheduled(func)

  Returns True if given callback function is scheduled as a one-time or
  repeating timer.
  If timer has already executed (and is not repeating),
  stopped, or never scheduled, returns False.

.. py:function:: stop_timer(func)  
 
  Stops timer with given callback. No error fs timer is not found.
