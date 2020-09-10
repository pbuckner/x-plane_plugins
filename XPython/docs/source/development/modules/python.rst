XPPython
============
.. py:module:: XPPython

To use::

   import XPPython

This module provides some XPPython3 specific constants and functions.

Functions
---------

.. py:function:: XPPythonGetDicts(None) -> internal_dict:

 Returns a dictionary of internal plugin dictionaries.

 Internally, the plugin maintains a number of dictionaries which
 allow us to map the Python API to the X-Plane C API. For example,
 we have a an internal ``widgetPropertyDict`` which allows us
 to store and update custom properties associated with your widgets.
 (Your properties are stored as Python objects which the C-API does
 not understand.) The ``widgetPropertyDict``, therefore is a dictionary
 whose key is the widget + propertyID, and the value is a python object.

 This function returns a dictionary of all dictionaries, the key
 is the dictionary name (e.g., ``widgetProperites``), and the value
 is the particular dictionary.

 The dictionaries are intended to be read-only and for debugging purposes
 only.
 

.. py:function:: XPPythonGetCapsules(None) -> capsules_dict:

 Returns an internal dictionary of Capsules.

 A Capsule is a way
 to implement opaque pointers to match the C API. For example,
 rather than simply returning a Python integer::

   >>> print(XPGetWidgetWithFocus())
   0x78662234

 as a widget ID, we
 declare a capsule type called "XPLMWidgetID" and return a wrapped
 integer which will look like::

   >>> print(XPGetWidgetWithFocus())
   <capsule object "XPLMWidgetID" at 0x78662234>

 The benefit is we can do some error checking (essentially type-checking).
 Similarly, you can check a value to make sure it's the expected "type".

 This function returns a dictionary keyed by capsule types (strings), with
 values a list of known instances. Note these instances will cover all
 python plugins, not just your own. (There is currently no way to distiguish
 the owning plugin for a particular capsule.)
 
Constants
---------

.. py:data:: VERSION

  Version number of XPPython3 plugin, e.g., '3.0.0'             

.. py:data:: PLUGINSPATH

  Relative path the where plugins are located, e.g., 'Resources/plugins/PythonPlugins'

.. py:data:: INTERNALPLUGINSPATH

  Relative path to where internal plugins are located, e.g., 'Resources/plugsin/XPPython3'
  
Note that both PLUGINSPATH and INTERNALPLUGINSPATH will be in your plugin's
python path.

    
