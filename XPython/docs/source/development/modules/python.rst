XPPython
============
.. py:module:: XPPython
.. py:currentmodule:: xp
                      
To use::

   import xp

This module provides some XPPython3 specific constants and functions.

Functions
---------

.. py:function:: pythonGetDicts()

 Returns a dictionary of internal plugin dictionaries.

 Internally, the plugin maintains a number of dictionaries which
 allow us to map the Python API to the X-Plane C API. For example,
 we have a an internal ``widgetPropertyDict`` which allows us
 to store and update custom properties associated with your widgets.
 (Your properties are stored as Python objects which the C-API does
 not understand.) The ``widgetPropertyDict``, therefore is a dictionary
 whose key is the widget + propertyID, and the value is a python object.

 This function returns a dictionary of all dictionaries, the key
 is the dictionary name (e.g., ``widgetProperties``), and the value
 is the particular dictionary.

 The dictionaries are intended to be read-only and for debugging purposes
 only. They are documented in :doc:`/development/xppythondicts`.

.. py:function:: log(s)

 Print string into XPPython3Log.txt file, appending a newline.
 Normally, the log is *not* flushed after each write. To force
 a flush, call with no parameters (e.g., ``xp.log()``)

 Your str is prefixed with ``[<module>]`` to help user understand
 the message context.

 >>> xp.log(f"This is version {xp.VERSION}")
 [PythonPlugins.PI_MiniPython] This is version 3.0.12a1 - for Python 3.9

.. py:function:: systemLog(s)
.. py:function:: sys_log(s)

 Print string into X-Plane System log file, ``Log.txt``, appending a newline.
 The log *is* flushed after each write. (``sys_log`` is an alias of ``systemLog``).

 Your str is prefixed with ``[XP3: <module>]`` to help user understand the message
 context. Please, write to System Log only when you need to alert the user
 that something is mis-configured or failing. Otherwise, write to XPPython3Log.txt using
 :py:func:`log`.

 >>> xp.systemLog(f"This is version {xp.VERSION}")
 [XP3: PythonPlugins.PI_MiniPython] This is version 3.0.12a1 - for Python 3.9

.. py:function:: pythonGetCapsules()

 Returns an internal dictionary of Capsules.

 A Capsule is a way
 to implement opaque pointers to match the C API. For example,
 rather than simply returning a Python integer::

   >>> print(xp.getWidgetWithFocus())
   0x78662234

 as a widget ID, we
 declare a capsule type called "XPLMWidgetID" and return a wrapped
 integer which will look like::

   >>> print(xp.getWidgetWithFocus())
   <capsule object "XPLMWidgetID" at 0x78662588>

 The benefit is we can do some error checking (essentially type-checking).
 Similarly, you can check a value to make sure it's the expected "type".

 This function returns a dictionary keyed by capsule types (strings), with
 values a list of known instances. Note these instances will cover all
 python plugins, not just your own. (There is currently no way to distiguish
 the owning plugin for a particular capsule.)

 The original value (id, or C-pointer) for the capsule'd object will be
 the key to the capsule in the returned dictionary. For example, X-Plane will
 see the command reference id ``2305`` when XPPython3 works with :py:func:`commandRef`
 capsule at ``0x7fdea8b9a3c0``. The capsule is a Python object. Similarly,
 X-Plane WidgetID 140594295845456 is represented by the Python object at ``0x7fdea95825d0``.
 (``140594295845456`` is hex ``0x7fdea90fb250``, so you can tell CommandRefs are
 probably integer indices, Widget IDs are probably C-Pointers.)

 Within python, you should be using capsules. However it may be useful to
 convert from python capsules back to original value in order to understand internal X-Plane
 error messages, which would not report the capsule value.::

    {'XPLMCommandRef': {2305: <capsule object "XPLMCommandRef" at 0x7fdea8b9a3c0>,
                        2306: <capsule object "XPLMCommandRef" at 0x7fdea9582600>,
                        2101: <capsule object "XPLMCommandRef" at 0x7fdea9b59360>,
                        620: <capsule object "XPLMCommandRef" at 0x7fdea9b591e0>,
                        618: <capsule object "XPLMCommandRef" at 0x7fdea9b591b0>,
                        384: <capsule object "XPLMCommandRef" at 0x7fdea9b594b0>,
                        385: <capsule object "XPLMCommandRef" at 0x7fdea9b594e0>,
                        386: <capsule object "XPLMCommandRef" at 0x7fdea9b59510>},
     'XPLMWindowIDRef': {},
     'XPLMHotkeyIDRef': {},
     'LayerIdRef': {},
     'XPLMMenuIDRef': {140594492097600: <capsule object "XPLMMenuIDRef" at 0x7fdea95824b0>,
                       140594288617360: <capsule object "XPLMMenuIDRef" at 0x7fdea9582690>,
                       140594303861088: <capsule object "XPLMMenuIDRef" at 0x7fdea9b59540>},
     'XPLMWidgetID': {140594295845456: <capsule object "XPLMWidgetID" at 0x7fdea95825d0>,
                      140594295807728: <capsule object "XPLMWidgetID" at 0x7fdea95825a0>,
                      140594295846192: <capsule object "XPLMWidgetID" at 0x7fdea9582930>,
                      140594295173136: <capsule object "XPLMWidgetID" at 0x7fdea9582810>,
                      140594295515808: <capsule object "XPLMWidgetID" at 0x7fdea95d87b0>}
    }
 
.. py:function:: getPluginStats

  Return dictionary of plugin statistics. Currently includes per-plugin performance data, keyed
  by the plugin module name. Key value ``None`` is the overall XPPython3 performance information.

  .. note:: values are in microseconds and *reset to zero* each time this function is called.

  >>> xp.getPluginStats()
  {None:   {'fl'; 0, 'draw': 123, 'customw': 99},
   'PythonPlugins.PI_MiniPython': {'fl'; 0, 'draw': 0, 'customw': 33499},
  }

  Values are in micro-seconds, keys are

  * **fl**: Time spent within flight loop callback(s).

  * **draw**: Time spent within draw callback(s).

  * **customw**: Time spent within custom widgets. This includes handling mouse and keyboard events, but is
    primarily the time taken to draw the custom widgets. Non-custom widgets (i.e., standard Laminar widgets)
    are not counted (SDK does not provide access to this timing information.)

Constants
---------

.. py:data:: pythonExecutable

   Full path to the python executable, for example, ``/usr/bin/python3.8`` or ``C:\Program Files\Python39\pythonw.exe``.
   The primary use for this is with ``subprocess`` or ``multiprocessing`` python modules when you want
   to spawn another python process.

   >>> xp.pythonExecutable
   '/Library/Frameworks/Python.framework/Version/3.9/bin/python3.9'

.. py:data:: VERSION

  Version number of XPPython3 plugin, e.g., '3.0.0'

  >>> xp.VERSION
  '3.0.12a1 - for Python 3.9'

.. py:data:: PLUGINSPATH
  :value: "Resources/plugins/PythonPlugins"

  Relative path the where "regular" plugins are located. Note that scenery and aircraft plugins are stored
  with their scenery / aircraft files.

.. py:data:: INTERNALPLUGINSPATH
  :value: "Resources/plugins/XPPython3"

  Relative path to where internal plugins are located.
  
Note that both PLUGINSPATH and INTERNALPLUGINSPATH will be in your plugin's
python path.

    
