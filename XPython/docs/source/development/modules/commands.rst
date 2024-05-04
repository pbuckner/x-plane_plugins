Easy Commands
=============

.. warning:: This is proposed documentation, for review only.

..
   py:module:: commands
  
To use::

  from XPPython3.utils import commands

.. py:function:: find_command(name: str)

.. py:function:: create_command(name: str, description: str, function: Callable[[int, float], None])   

.. py:function:: replace_command(name: str, function: Callable[[int, float], None])

.. py:function:: wrap_command(name: str, before: Callable[[int, float], None], after: Callable[[int, float], None])

.. py:function:: filter_command(name: str, filter: Callable[[], int])

  >>> pause = find_command('sim/operation/pause_toggle')
  >>> pause.once()  # sim is paused
  >>> pause.once()  # sim is not paused

And, recreating from ``c172_fuel_selector.lua``, first define the callback function,
in this case using other datarefs, and then define the command::

  >>> def cmd_fuel_selector_up(phase, duration):
  ...    if phase == 0:
  ...        if fuel_tank_selector_c172.value == 1:
  ...            fuel_tank_selector_c172.value = 4
  ...        elif fuel_tank_selector_c172.value == 4:
  ...            fuel_tank_selector_c172.value = 3
  ...        elif fuel_tank_selector_c172.value == 3:
  ...            fuel_tank_selector_c172.value = 3
  ...        else:
  ...            fuel_tank_selector.value = 4
  ...    if fuel_cutoff_selector.value == 0:
  ...        fuel_tank_selector.value = fuel_tank_selector_c172.value
  ...
  >>> cmdcustomfuelup = create_command("laminar/c172/fuel_selector_up",
  ...                                  "Move the fuel selector up one",
  ...                                  cmd_fuel_selector_up)
  ...

Example::

 # cmd = find_command("")
 # cmd.once() execute command once
 # cmd.start() starts command
 # cmd.end() ends command
 #
 # cmd = create_command(name, description, function)
 #    function(phase:int{0,1,2}, duration:float)
 #
 # cmd = replace_command(name, function)
 # cmd = wrap_command(name, before_function, after_function)
 # cmd = filter_command(name, bool_function)
 #     bool_function() returns True if command is to execute, false to block
 #     ... note, if filter_command returns false for python command, we'll
 #     fabricate an end(). If filter_command returns false for laminar command
 #     we simply block that command continuing. may or may not be able to send and "end"
 
