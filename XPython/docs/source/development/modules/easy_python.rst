EasyPython
==========

.. warning:: This is proposed documentation for review only.
             
..
  py:module:: easy_python

To use::

  from XPPython3.utils.easy_python import EasyPython

Have your PythonInterface class inherit from the `EasyPython` class
and you'll gain some ease-of-use without losing the full power
of python and the full X-Plane SDK.

Your python plugin needs to inherit from ``EasyPython`` class::

  Basic XPPython3:                                    |  With EasyPython:
                                                      |
  from XPPython3 import xp                            |  from XPPython3 import xp
                                                      |  from XPPython3.utils.easy_python import EasyPython
                                                      |
  class PythonInteface:                               |  class PythonInterface(EasyPython):
      def XPluginStart(self):                         |      def onStart(self):
          return name, signature, description         |          ...
                                                      |      
      def XPluginEnable(self):                        |      def aircraft_load(self):
          return 1                                    |          ...
                                                      | 
      def XPluginReceiveMessage(self, inFromWho,      |      def before_physics(self):
                                inMessage, inParam):  |          ...
          ...                                         |

There still needs to be a class named ``PythonInterface`` in your ``PI_*.py`` file,
as that's what we're looking for. However, by
inheriting from the ``EasyPython`` class, you'll gain the following callbacks:

.. table::
   :align: left

   +-------------------+-------------------------------------------------+
   |aircraft_load()    |Run once when your aircraft is loaded. This is   |
   |                   |run after the aircraft is initialized enough to  |
   |                   |set overrides.                                   |
   +-------------------+-------------------------------------------------+
   |aircraft_unload()  |Run once when your aircraft is unloaded.         |
   |                   |                                                 |
   |                   |                                                 |
   +-------------------+-------------------------------------------------+
   |flight_start()     |Run once each time a flight is started. The      |
   |                   |aircraft is already initialized and can thus be  |
   |                   |customized. This is always called after          |
   |                   |``aircraft_load()`` has been run at least once.  |
   +-------------------+-------------------------------------------------+
   |flight_crash()     |Called if X­Plane detects that the user has      |
   |                   |crashed the airplane.                            |
   |                   |                                                 |
   +-------------------+-------------------------------------------------+
   |before_physics()   |Called every frame that the sim is not paused and|
   |                   |not in replay, before physics are calculated.    |
   |                   |                                                 |
   +-------------------+-------------------------------------------------+
   |after_physics()    |Called every frame that the sim is not paused and|
   |                   |not in replay, after physics are calculated.     |
   |                   |                                                 |
   +-------------------+-------------------------------------------------+
   |after_replay()     |Called every frame that the sim is in replay     |
   |                   |mode, regardless of pause status.                |
   |                   |                                                 |
   +-------------------+-------------------------------------------------+

Each callback, being a method of the ``PythonInterface`` class takes a single parameter ``self``
which points to the PythonInterface instance. None of these callback return a value.

Additionally, simplified functionality is available with respect to :doc:`datarefs <datarefs>`, :doc:`commands <commands>`, and
:doc:`timers <timers>`.

The methods are identical to those defined by xlua, making this functionality easy for
people transitioning from that xlua. However, this functionality works with Aircraft and non-Aircraft
plugins withon compromise.

If your code does not need one of these callback, `do not provide` one
in your code.
The default implementation of these callbacks is lightweight, and
automatically disable themselves if not used.

