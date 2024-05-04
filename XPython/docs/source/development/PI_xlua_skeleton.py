# You'll need to include some modules, here we've listed
# the most common ones. Using explicit imports, commands like 'find_dataref()'
# work just like their xlua versions.
from XPPython3 import xp
from XPPython3.utils.easy_python import EasyPython
from XPPython3.utils.datarefs import find_dataref, create_dataref
from XPPython3.utils.commands import find_command, create_command, replace_command
from XPPython3.utils.timers import run_after_time
from XPPython3.utils import xlua

############################################
# GLOBAL Variables
# These are accessible, and potentially changeable, by this module only
# To update their value _within_a_function_, you'll need to use 'global' command.
# If you name them with leading Capital letter, you'll rememeber they're global.
# Perhaps using ALL CAPS for "constants".

STARTING_VALUE = 2
BatteryPREV = 0   # "saved" state of battery status
Battery_amps_c172NEW = 0
Fuel_tank_selector_c172 = 0

############################################
#   DATAREFS
#
# -------------
# Functions for writable dataref callbacks.
# -------------
# Unlike xlua, there is no need to create 'empty' functions for callbacks where you don't
# need them. Functions take no parameters, and return no value.


def knobOATfunc():
    # toggle OAT from F and C when in temp mode
    if dr_knob_OAT.value < 2:
        cmd_thermo_units_toggle.once()


# -------------
# Find / Create datarefs
# -------------
# Just like xlua, find_dataref(), and create_datareaf()
# You'll get/set values using their '.value' attribute.
# If you name them "dr_..." you'll remember they're datarefs,
# instead of "regular" variables.

dr_batteryEMERG = find_dataref("sim/cockpit2/electrical/battery_on[1]")
dr_battery_amps = find_dataref("sim/cockpit2/electrical/battery_amps[0]")
dr_battery_on = find_dataref("sim/cockpit2/electrical/battery_on[0]")
dr_crossTIE = find_dataref("sim/cockpit2/electrical/cross_tie")
dr_cross_tie = find_dataref("sim/cockpit2/electrical/cross_tie")
dr_fuel_pump_on = find_dataref("sim/cockpit2/engine/actuators/fuel_pump_on[0]")
dr_fuel_tank_selector = find_dataref("sim/cockpit2/fuel/fuel_tank_selector")  # (0=none,1=left,2=center,3=right,4=all)
dr_generator_on = find_dataref("sim/cockpit2/electrical/generator_on[0]")
dr_interior_lites_0 = find_dataref("sim/cockpit2/switches/instrument_brightness_ratio[0]")
dr_interior_lites_1 = find_dataref("sim/cockpit2/switches/instrument_brightness_ratio[1]")
dr_main_bus_volts = find_dataref("sim/cockpit2/electrical/bus_volts[0]")
dr_max_bat_volt_standard = find_dataref("sim/aircraft/limits/max_bat_volt_standard")  # -- the nominal battery volts from acf planemaker
dr_num_batteries = find_dataref("sim/aircraft/electrical/num_batteries")  # we use this to know whether we are on the G1000 or analogue cessna
dr_startup_running = find_dataref("sim/operation/prefs/startup_running")  # start as cold and dark or not

dr_fuel_cutoff_selector = create_dataref("laminar/c172/fuel/fuel_cutoff_selector")  # (0=none,1=fuel cutoff)
dr_fuel_tank_selector_c172_handle = create_dataref("laminar/c172/fuel/fuel_tank_selector")  # -- (1=left,2=all,3=right)
dr_battery_amps_c172 = create_dataref("laminar/c172/electrical/battery_amps")  # -- the instrument needle
dr_knob_OAT = create_dataref("laminar/c172/knob_OAT", "number", knobOATfunc)  # the OAT/VOLTS toggle button of the clock: 0 is F / 1 is C / 2 is volts
dr_radio_vol_pilot = create_dataref("laminar/C172/sound/radio_volume_pilot", "array[10]")


############################################
#  COMMANDS
#
# -------------
# Function callbacks for my commands
# -------------
# Just like xlua, they take two parameters (phase, duration) and
# return no value

def cmd_fuel_selector_up(phase, duration):
    # Note: because we _change_ Fuel_tank_selector_c172, we need to declare it global
    global Fuel_tank_selector_c172
    if phase == 0:
        if Fuel_tank_selector_c172 == 1:
            Fuel_tank_selector_c172 = 4
        elif Fuel_tank_selector_c172 == 4:
            Fuel_tank_selector_c172 = 3
        elif Fuel_tank_selector_c172 == 3:
            Fuel_tank_selector_c172 = 3
        else:
            Fuel_tank_selector_c172 = 4
        if dr_fuel_cutoff_selector.value == 0:
            dr_fuel_tank_selector.value = Fuel_tank_selector_c172


def cmd_fuel_cutoff(phase, duration):
    # Note: because we're _NOT_ changing Fuel_tank_selector_c172, we don't need
    # to declare it global
    if phase == 0:
        if dr_fuel_cutoff_selector.value == 0:
            dr_fuel_cutoff_selector.value = 1
            dr_fuel_tank_selector.value = 0
        else:
            dr_fuel_cutoff_selector.value = 0
            dr_fuel_tank_selector.value = Fuel_tank_selector_c172


# -------------
# Find / Create commands
# -------------
# Just like xlua: find_command(), create_command(), replace_command() and wrap_command()

cmd_thermo_units_toggle = find_command("sim/instruments/thermo_units_toggle")  # -- toggle OAT from F and C
cmd_fuel_sel_both = find_command("sim/fuel/fuel_selector_all")
cmd_customfuelup = create_command("laminar/c172/fuel_selector_up", "Move the fuel selector up one", cmd_fuel_selector_up)
cmd_fuelshutoff = replace_command("sim/starters/shut_down", cmd_fuel_cutoff)


############################################
# MISC. FUNCTIONS
# whatever functions you like, to make the rest of your
# code easier to maintain

def deferred_flight_start():
    print('Deferred flight start')
    dr_interior_lites_0.value = 0.4
    dr_interior_lites_1.value = 1


def func_animate_slowly(reference_value, animated_VALUE, anim_speed):
    # Note, 'animated_VALUE' is local to this function -- no need
    # to make it global... This is just simple python
    # We do use a built-in read-only variable xlua.SIM_PERIOD
    animated_VALUE = animated_VALUE + ((reference_value - animated_VALUE) * (anim_speed * xlua.SIM_PERIOD))
    return animated_VALUE


def update_amps_needles():
    dr_battery_amps_c172.value += ((Battery_amps_c172NEW - dr_battery_amps_c172.value) * (10 * xlua.SIM_PERIOD))


############################################
# RUNTIME CODE
# This set up the plugin and allows X-Plane to call the
# event callbacks.
# You MUST call it PythonInterface, and it should inherit from (at least)
# EasyPython. This enables the hookup between X-Plane and your run-time callbacks.

class PythonInteface(EasyPython):

    def flight_start(self):
        run_after_time(deferred_flight_start, 0.1)

        # -- IF NUMBERS OF BATTERIES IS 1 MEAN WE ARE ON THE ANALOG CESSNA (SINCE THE G1000 CESSNA HAS TWO)
        if dr_startup_running.value:  # -------- IF START WITH ENGINE RUNNING
            if dr_num_batteries.value == 1:  # -- IN THE ANALOG CESSNA:
                dr_crossTIE.value = 1
                dr_fuel_pump_on.value = 0
            elif dr_num_batteries.value == 2:  # -- IN THE G1000 CESSNA:
                dr_crossTIE.value = 0
                dr_batteryEMERG.value = 1
                dr_fuel_pump_on.value = 0
        else:  # -------------------------------- IF START COLD AND DARK:
            dr_crossTIE.value = 0
            dr_batteryEMERG.value = 0
            dr_fuel_pump_on.value = 0

    def after_physics(self):
        global Fuel_tank_selector_c172, Battery_amps_c172NEW, BatteryPREV

        # -- KEEP UPDATED THE FUEL SELECTOR VALUE
        if dr_fuel_tank_selector.value not in (Fuel_tank_selector_c172, 0):
            Fuel_tank_selector_c172 = dr_fuel_tank_selector.value
        # -- KEEP UPDATED THE FUEL HANDLE
        if dr_fuel_tank_selector_c172_handle.value != Fuel_tank_selector_c172:
            new_handle_position = 2 if Fuel_tank_selector_c172 == 4 else Fuel_tank_selector_c172
            dr_fuel_tank_selector_c172_handle.value = func_animate_slowly(new_handle_position, dr_fuel_tank_selector_c172_handle.value, 15)

        # UPDATE AMPS NEEDLE
        Battery_amps_c172NEW = dr_battery_amps.value
        update_amps_needles()  # this is globally defined in this module... works perfectly

        # CROSS-TIE RELAY ON THE G1000 CESSNA
        if dr_num_batteries.value == 2:
            dr_cross_tie.value = 1 if (dr_main_bus_volts.value > dr_max_bat_volt_standard.value) else 0

        # ALT/BAT MASTER SWITCHES LOGIC
        if BatteryPREV != dr_battery_on.value:
            if dr_battery_on.value == 0:
                dr_generator_on.value = 0

        BatteryPREV = dr_battery_on.value
