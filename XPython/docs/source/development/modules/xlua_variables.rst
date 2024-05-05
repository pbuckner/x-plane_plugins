Xlua Variables
==============

.. warning:: This is proposed documentation, for review only.
             
..
  py:module:: xlua

To use:
::

   from XPPython3.utils import xlua

For convenience, Xlua pre-defines some variables. They're actually just
pre-mapped datarefs. 

  >>> xlua.IN_REPLAY
  False
  >>> xlua.PAUSED
  False
  >>> xlua.SIM_PERIOD
  0.221887
  >>> xlua.RUNNING_TIME
  123.333
  >>> xlua.FLIGHT_TIME
  35.1233

+-----------------+------------------------------------------------------------------+
|IN_REPLAY        |Boolean for ``sim/time/is_in_replay`` dataref. True if in replay. |
+-----------------+------------------------------------------------------------------+
|PAUSED           |Boolean for ``sim/time/paused``. True if sim is paused.           |
+-----------------+------------------------------------------------------------------+
|SIM_PERIOD       |Value of ``sim/operation/misc/frraem_rate_period``, which is the  |
|                 |inverse of FPS.                                                   |
+-----------------+------------------------------------------------------------------+
|RUNNING_TIME     |Number of seconds the sim has been running: does not increment    |
|                 |when sim is paused. (Use this for animation)                      |
|                 |``sim/time/total_running_time_sec``.                              |
+-----------------+------------------------------------------------------------------+
|FLIGHT_TIME      |Number of seconds of this flight. Changing user aircraft or       |
|                 |changing the airport will reset flight_time to                    |
|                 |zero. ``sim/time/total_flight_time_sec``.                         |
+-----------------+------------------------------------------------------------------+

