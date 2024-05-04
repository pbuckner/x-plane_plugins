XLua Variables
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

IN_REPLAY is boolean for 'sim/time/is_in_replay' dataref; PAUSED is 'sim/time/paused'; and SIM_PERIOD
is 'sim/operation/misc/frame_rate_period', which is the inverse of FPS.
