Typical Aircraft xlua Plugin
============================

Most Aircraft Xlua scripts look similar, so taking that as an example, here's
a annotated version for python.

Assume you have an xlua script in ``Aircraft/MyAircraft/plugins/xlua/scripts/MyInit/MyInit.xlua``.
You might create the python replacement as: ``Aircraft/MyAircraft/plugins/PythonPlugins/PI_Init.py``.
Your plugin file will (likely) have the following sections, which we will cover in detail:

* Imports

* Global variables

* Datarefs

* Commands

* Miscellaneous functions

* Runtime code


Imports
-------
This is standard python. A convenient block might want to copy & paste could be::

  from XPPython3.utils.easy_python import EasyPython
  from XPPython3 import xp
  from XPPython3.utils.datarefs import find_dataref, create_dataref
  from XPPython3.utils.commands import find_command, create_command, replace_command, wrap_command, filter_command
  from XPPython3.utils.timers import run_timer, run_after_time, run_at_interval, stop_timer, is_timer_running
  from XPPython3.utils import xlua

You `must` include the ``easy_python`` module as this is used to provide the interface
between X-Plane, XPPython3's PythonInterface, and your xlua-like code (See `Runtime code`, below.)

Importing from ``datarefs``, ``commands``, and ``timers`` will give you xlua-like commands
which have equivalent functionality.

Importing ``xlua`` will give you access to read-only variables provided by xlua.

There is no performance penalty by importing "too much", so copy & paste is safe.

Global Variables
----------------
Variable scoping differs in python and xlua, so the easiest way to use variables across
multiple functions in the module is to simply create them as globals. They can be `read`
anywhere within the module, but to writer to them, you will need to include the ``global``
keyword: this is standard python::

  ############################################
  # GLOBAL Variables
  VERSION = 'MyPlugin v.01'
  BatteryPREV = 0   # "saved" state of battery status
  Battery_amps_c172NEW = 0
  Fuel_tank_selector_c172 = 0

For your sanity, consider using ALL_CAPS for "constants" and Title Case for
global variables.

Datarefs
--------
Datarefs can be defined in global scope, making them available anywhere
in the module. Because the result of ``find_dataref()`` and ``create_dataref``
is an object instance, you'll refer to their value using the ``.value``
attribute, and don't need to declare them as ``global``. If you name them
``dr_..`` or similar, you'll remember they're datarefs instead of regular
variables.

Datarefs you create `may` have a callback function, called when you (or
anyone) sets the dataref's value. So, your `Datarefs` section may contain two
parts: your callbacks and your set of datarefs::

  ############################################
  #   DATAREFS
  #
  # -------------
  # Functions for writable dataref callbacks.
  # -------------

  def myDataRefCallback():
      if dr_my_data_ref.value < 2:
          cmd_do_some_command.once()

  # -------------
  # Find / Create datarefs
  # -------------

  dr_paused = find_dataref("sim/time/paused")
  dr_my_data_ref = create_dataref('test/my_dataref', callback=myDataRefCallback)

You can refer to :doc:`modules/datarefs` for details these commands.

Commands
--------
If you need to create or access commands, you can define them in a way
similar to datarefs.
You'll create callback functions for each command you create.::

  ############################################
  #  COMMANDS
  #
  # -------------
  # Function callbacks for my commands
  # -------------

  def cmd_my_command_callback(phase, duration):
      if phase == 0 and not dr.paused.value:
          do_something()

  def cmd_version_callback(phase, duration):
      if phase == 0:
          print(VERSION)

  # -------------
  # Find / Create commands
  # -------------
  cmd_my_command = create_command("test/my_command", "Execute my command", cmd_my_command_callback)
  cmd_version_command = replace_command("sim/version/sim_build_string", cmd_version_callback)

See :doc:`modules/commands` for these commands.

Miscellaneous functions
-----------------------
Nothing special here, but you may want to create any number of other module-level functions.
These can either be for callback (such as timer callbacks), or convenience functions. Putting
such function in a section away from datarefs and commands is simply for maintenance purposes: python
doesn't care.::

  ############################################
  # MISC. FUNCTIONS

  def func_animate_slowly(reference_value, animated_VALUE, anim_speed):
      return animated_VALUE + ((reference_value - animated_VALUE) * (anim_speed * xlua.SIM_PERIOD))

  def deferred_flight_start():
      print('Deferred flight start')
      dr_interior_lites_0.value = 0.4
      dr_interior_lites_1.value = 1


Runtime code
------------
This part is crucial. All the previous code is executed `once`, when the python module is loaded.
To have X-Plane execute our plugin's functionality at the proper time, you'll need to provide
the appropriate callback `as methods of the` ``PythonInterface`` `class`::

  ############################################
  # RUNTIME CODE

  class PythonInterface(EasyPython):

      def flight_start(self):
          do_something_on_each_flight_start()

      def after_physics(self):
          do_something_on_each_frame()

      ...

Being a python class, you may certainly add other methods to ``PythonInterface``. Its
parent class, ``EasyPython`` already defines the full set of xlua-like callbacks available.

Providing an implementation for any of the seven pre-defined callbacks will allow X-Plane
(and XPPython3) to invoke your code at the appropriate time.

See :doc:`modules/easy_python` for these commands.

Skeleton
--------
Download a simple python skeleton you can use: `PI_xlua_skeleton.py <https://github.com/pbuckner/xppython3-demos/main/PI_xlua_skeleton.py>`_.
