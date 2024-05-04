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
does also::

  >>> def func():
  ...    print("Hello World")
  ...
  >>> run_at_interval(func, 5)
  Hello World
  Hello World
  >>> is_timer_scheduled(func)
  True
  >>> stop_timer(func)

Recreating a simple timer in ``c172_init.lua``. First define
the function to be called and then create the timer::

  >>> def deferred_flight_start():
  ...   print('Deferred flight start')
  ...   interior_lites_0.value = .4
  ...   interior_lites_1.value = 1
  ...   interior_lites_2.value = 1
  ...   interior_lites_3.value = 0.2
  ...   panel_glareshield_brightness.value = 0.3
  ...
  >>> run_after_time(deferred_flight_start, .1)


::
   
  run_at_interval(func, interval) -> run_timer(func, interval, interval)
  run_after_time(func, delay)   -> run_timer(func, delay, 0)
  run_timer(func, delay, interval)
     (after delay run & repeat every interval after that)
  stop_timer(func)
  is_timer_scheduled(func)
 
  Timers are _paused_ while Sim is paused & run faster if sim flight model is executing faster
  (say via 'sim/operation/flightmodel_speed_change' command, or dataref 'sim/time/sim_speed')
  (x ground-speed speed up does _not_ change timer speed.)
 
