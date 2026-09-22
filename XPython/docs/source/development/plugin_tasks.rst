Basic Tasks
===========

Previously we've covered basic start/stop of a plugin, but what kinds of things can a plugin *do*, especially as there are only five
methods in the :doc:`PythonInterface Skeleton <skeleton>`?

Within your Enable or Start methods, or perhaps in receipt of a particular message in ReceiveMessage, you'll create:

* :ref:`Timers <task-timers>` to fire at some time in the future.

* :ref:`Commands <task-commands>` to fire in response to user action (menu, keystroke).

* :ref:`Flight Loops <task-flightloops>` the mechanism used to implement timers.

* :doc:`Key Sniffers <modules/display_sniffer>` and :doc:`Hot Keys <modules/display_hotkeys>` to listen for user key strokes.

* :doc:`Menus <modules/menus>` to execute commands in response to user interactions.

For each of the above, you might provide a python callback function to be executed when activated.

Your callback might:

* Control a :doc:`Camera <modules/camera>`, providing the user with a different viewpoint.

* Create, Get or Set :doc:`DataRefs <modules/dataaccess>`, which may change underlying X-Plane functionality

* Create a :doc:`Window <modules/display_window>` to inform the user, or gather input.

* Display a :doc:`Web Page <modules/display_window_browser>`.

* Create an :doc:`Avionics Device <modules/display_avionics>` to mimic aircraft hardware.

* Draw an :doc:`Object Instance <modules/instance>` somewhere.
  
* Modify the :doc:`Map <modules/map>` by adding icons or labels.
  
* Play a :doc:`Sound <modules/sound>`.

* Query the X-Plane :doc:`Navaid Database <modules/navigation>`, :ref:`Probe Elevation <terrain-testing>`, or :ref:`Get Magnetic Variation <magnetic-variation>`.
  
* :doc:`Initialize <modules/planes>` a new flight, or acquire AI aircraft.

* :ref:`Message other plugins <Inter-plugin Messaging>`.

* Get or Set :doc:`Weather <modules/weather>`.

* Call other python code, perhaps accessing the internet, spawning :doc:`subprocesses <multiprocessing>`, solving world hunger.
  
* And/or create new timers, commands, flight loops, menus, etc.  

When it comes to drawing something, you have lots of options and these are outlined in the next section, :doc:`graphics`.
