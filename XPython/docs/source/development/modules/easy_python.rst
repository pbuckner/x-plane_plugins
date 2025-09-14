EasyPython
==========

.. py:module:: utils.easy_python

To use::

  from XPPython3.utils.easy_python import EasyPython

EasyPython Class
----------------

.. py:class:: EasyPython

Have your PythonInterface class inherit from the `EasyPython` class
and you'll gain some ease-of-use without losing the full power
of python and the full X-Plane SDK. (This module can be used with or without other python-xlua
inspired modules).

Your python plugin needs to inherit from ``EasyPython`` class::

  Basic XPPython3:                                    |  With EasyPython:
                                                      |
  from XPPython3 import xp                            |  from XPPython3 import xp
                                                      |  from XPPython3.utils.easy_python import EasyPython
                                                      |
  class PythonInterface:                              |  class PythonInterface(EasyPython):
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
inheriting from the ``EasyPython`` class, you'll gain the following callback interface:

.. py:function:: EasyPython.aircraft_load(self)

   Run one when your aircraft (user aircraft) is loaded. This is
   run after the aircraft is initialized enough to set overrides. (Note that the aircraft
   isn't placed at the airport yet, so don't query it's location: wait until flight_start()
   if you need that.)

.. py:function:: EasyPython.aircraft_unload(self)

   Run once, when your aircraft is unloaded.                 

.. py:function:: EasyPython.flight_start(self)

   Run once each time a flight is started. The      
   aircraft is already initialized and can thus be  
   customized. This is always called after          
   ``aircraft_load()`` has been run at least once.  
                 
.. py:function:: EasyPython.flight_crash(self)

   Called if X­Plane detects that the user has      
   crashed the airplane.                            
                 
.. py:function:: EasyPython.before_physics(self)

   Called every frame that the sim is not paused and
   not in replay, before physics are calculated.    

.. py:function:: EasyPython.after_physics(self)

   Called every frame that the sim is not paused and
   not in replay, after physics are calculated.     

.. py:function:: EasyPython.after_replay(self)

   Called every frame that the sim is in replay     
   mode, regardless of pause status.                
                                                 

Each callback, being a method of the ``PythonInterface`` class takes a single parameter ``self``
which points to the PythonInterface instance. None of these callback return a value.

Additionally, simplified functionality is available with respect to :doc:`datarefs <datarefs>`, :doc:`commands <commands>`, and
:doc:`timers <timers>`.

The methods are identical to those defined by xlua, making this functionality easy for
people transitioning from that xlua. However, this functionality works with Aircraft and non-Aircraft
plugins without compromise.

If your code does not need one of these callback, `do not provide` one
in your code.
The default implementation of these callbacks is lightweight, and
automatically disable themselves if not used.

Overriding Methods
------------------

Never use EasyPython by itself: XPPython3 looks for an instance of PythonInterface in order
to execute. That being said, if you do inherit from EasyPython, you must be careful
about overriding methods used by EasyPython. For example, do not define your own ``XPluginReceiveMessage``
function `unless you call` ``super()``::

  class PythonInterface(EasyPython):

      def onStart(self):
         ...

      def XPluginReceiveMessage(self, inFromWho, inMessage, inParam):
          # call parent.. to allow EasyPython to do what it needs to do
          super().XPluginReceiveMessage(self, inFromWho, inMessage, inParam)
          if inMessage == 43:
             do_my_stuff()

If you forget to call ``super()``, all EasyPython callbacks which rely on receiving messages
will fail.

Similarly, EasyPython automatically provides something for your plugin's name, id and description, but you
can override that by setting values within your ``__init__()`` `after` calling super::

  class PythonInterface(EasyPython):

      def __init__(self):
         super().__init__()
         self.name = "My Plugin Name"
         self.description = "My Plugin which does awesome stuff"
         self.id = "xppython3.test.special"

      ...
