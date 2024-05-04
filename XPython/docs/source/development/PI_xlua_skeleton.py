from XPPython3.utils.easy_python import EasyPython
from XPPython3 import xp
from XPPython3.utils.datarefs import find_dataref, create_dataref
from XPPython3.utils.commands import find_command, create_command, replace_command, wrap_command, filter_command
from XPPython3.utils.timers import run_timer, run_after_time, run_at_interval, stop_timer, is_timer_running
from XPPython3.utils import xlua

############################################
# GLOBAL Variables
# These are accessible, and potentially changeable, by this module only
# To update their value _within_a_function_, you'll need to use 'global' command.
# If you name them with leading Capital letter, you'll rememeber they're global.
# Perhaps using ALL CAPS for "constants".

# STARTING_VALUE = 2
# BatteryPREV = 0   # "saved" state of battery status
# Battery_amps_c172NEW = 0
# Fuel_tank_selector_c172 = 0




############################################
#   DATAREFS
#
# -------------
# Functions for writable dataref callbacks.
# -------------
# Unlike xlua, there is no need to create 'empty' functions for callbacks where you don't
# need them. Functions take no parameters, and return no value.

# def sync_values():
#    dr_my_battery_on.value = dr_laminar_battery_on.value
#    if my_battery_on.value == 1:
#        cmd_turn_on_lights.once()
#




# -------------
# Find / Create datarefs
# -------------
# Just like xlua, find_dataref(), and create_datareaf()
# You'll get/set values using their '.value' attribute.
# If you name them "dr_..." you'll remember they're datarefs,
# instead of "regular" variables.

# dr_laminar_battery_on = find_dataref('sim/cockpit2/electrical/battery_on[0]')
# dr_my_battery_on = create_dataref('my/cockpit/battery_on', 'number')




############################################
#  COMMANDS
#
# -------------
# Function callbacks for my commands
# -------------
# Just like xlua, they take two parameters (phase, duration) and
# return no value

# def cmd_turn_on_lights_cb(phase, duration):
#    if phase == 0:
#        df_lights.value = [1, 1, 1, 1, 1, 1]




# -------------
# Find / Create commands
# -------------
# Just like xlua: find_command(), create_command(), replace_command(), filter_command()  and wrap_command()

# cmd_thermo_units_toggle = find_command("sim/instruments/thermo_units_toggle")  # -- toggle OAT from F and C
# cmd_fuel_sel_both = find_command("sim/fuel/fuel_selector_all")
# cmd_turn_on_lights = create_command("my/turn_on_lights", "Set all lights to '1'.", cmd_cmd_turn_on_lights_cb)
# cmd_fuelshutoff = replace_command("sim/starters/shut_down", cmd_fuel_cutoff)



############################################
# MISC. FUNCTIONS
# whatever functions you like, to make the rest of your
# code easier to maintain

# def deferred_flight_start():
#     print('Deferred flight start')
#     dr_interior_lites_0.value = 0.4
#     dr_interior_lites_1.value = 1
#
# def func_animate_slowly(reference_value, animated_VALUE, anim_speed):
#     # Note, 'animated_VALUE' is local to this function -- no need
#     # to make it global... This is just simple python
#     # We do use a built-in read-only variable xlua.SIM_PERIOD
#     animated_VALUE = animated_VALUE + ((reference_value - animated_VALUE) * (anim_speed * xlua.SIM_PERIOD))
#     return animated_VALUE




############################################
# RUNTIME CODE
# This set up the plugin and allows X-Plane to call the
# event callbacks.
# You MUST call it PythonInterface, and it should inherit from (at least)
# EasyPython. This enables the hookup between X-Plane and your run-time callbacks.
# !! Comment out the methods you don't need: this will save processing time!!

class PythonInteface(EasyPython):

    def airacraft_load(self):
        # once, when aircraft is loaded
        pass

    def aircraft_unload(self):
        # once, when aircraft is unloaded
        pass
        
    def flight_start(self):
        # once, each time a flight is started. Always _after_ aircraft_load()
        pass

    def flight_crash(self):
        # ... when you crash
        pass

    # def before_physics(self):
    #     # Called _every_frame_, when not paused and not in replay. Before physics
    #     # is calculated.
    #     pass

    # def after_physics(self):
    #     # Called _every_frame_, when not paused and not in replay. After physics
    #     # is calculated.
    #     pass

    # def after_replay(self):
    #     # Called _every_frame_, when in replay, regardless of pause status.
    #     pass
