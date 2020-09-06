XPLMPlanes
==========
.. module:: XPLMPlanes

To use::

  import XPLMPlanes  

The XPLMPlanes APIs allow you to control the various aircraft in X-Plane,
both the user's and the sim's.


Functions
---------

.. py:function:: XPLMSetUsersAircraft(path: str) -> None:

 This routine changes the user's aircraft.  Note that this will reinitialize
 the user to be on the nearest airport's first runway.  Pass in a full path
 (hard drive and everything including the .acf extension) to the .acf file.


.. py:function::  XPLMPlaceUserAtAirport(code: str) -> None:

 This routine places the user at a given airport.  Specify the airport by
 its ICAO code (e.g. 'KBOS').


.. py:function:: XPLMPlaceUserAtLocation(latitude, longitude, elevation, heading, speed) -> None:

 :param float latitude:
 :param float longitude: degrees
 :param float elevation: meters MSL                         
 :param float heading: degrees True
 :param float speed: meters per second

 Places the user at a specific location after performing any necessary
 scenery loads.

 As with in-air starts initiated from the X-Plane user interface, the
 aircraft will always start with its engines running, regardless of the
 user's preferences (i.e., regardless of what the dataref
 ``sim/operation/prefs/startup_running`` says).


.. py:function::  XPLMCountAircraft(None) -> (total, active, controller):

 :return: list of three ints,

          * number of planes X-Plane can have
          * number of active planes                
          * plugin ID of plugin controlling aircraft                    

 This function returns the number of aircraft X-Plane is capable of having,
 as well as the number of aircraft that are currently active.  These numbers
 count the user's aircraft.  It can also return the plugin that is currently
 controlling aircraft.


.. py:function:: XPLMGetNthAircraftModel(int: index) -> (model, path):

 :param int index: zero-based index of aircraft: User's aircraft is always 0.
 :return: list of two strs:

    * filename of aircraft
    * path to the model filename

 This function returns the aircraft model for the Nth aircraft.  Indices are
 zero based, with zero being the user's aircraft.


.. py:function:: XPLMPlanesAvailable_f(object: inRefcon) -> None:

 Callback you provide, to be called when another plugin gives up
 access to the multiplayer planes.  Use this to wait for access to
 multiplayer.



.. py:function::  XPLMAcquirePlanes(list: aircraft, callable: callback, object: refCon) -> int:

 :param aircraft: list of strings (or None). If provide, load these airfact
 :param callback: :py:func:`XPLMPlanesAvailable_f` callback, called if planes are not immediately available.
 :param refCon: reference constant to be passed to your callback
 :return: 1= you gained access (will not call callback), 0= you did not: your callback will be called if/when aircraft are released
 :rtype: int

 XPLMAcquirePlanes grants your plugin exclusive access to the aircraft.  It
 returns 1 if you gain access, 0 if you do not. inAircraft - pass in a list
 of strings specifying the planes you want loaded.  For any plane index you
 do not want loaded, pass an empty string.  Other strings should be full
 paths with the .acf extension.  Pass None if there are no planes you want
 loaded. If you pass in a callback and do not receive access to the planes
 your callback will be called when the airplanes are available. If you do
 receive airplane access, your callback will not be called.


.. py:function::  XPLMReleasePlanes(None) -> None:

 Call this function to release access to the planes.  Note that if your plugin is
 disabled, access to planes is released for you and you must reacquire it.


.. py:function:: XPLMSetActiveAircraftCount(count) -> None:

 This routine sets the number of active planes.  If you pass in a number
 higher than the total number of planes availables, only the total number of
 planes available is actually used.


.. py:function:: XPLMSetAircraftModel(index, path) -> None:

 :param int index: load model (specified by path) into this slot.
 :param str path: path of aircraft model with .acf extension.

 This routine loads an aircraft model.  It may only be called if you  have
 exclusive access to the airplane APIs.  Pass in the path of the  model with
 the .acf extension.  The index is zero based, but you  may not pass in 0
 (use :py:func:`XPLMSetUsersAircraft` to load the user's aircracft).


.. py:function:: XPLMDisableAIForPlane(index) -> None:

 This routine turns off X-Plane's AI for a given plane.  The plane will
 continue to draw and be a real plane in X-Plane, but will not  move itself.

Constants
---------

.. py:data:: XPLM_USER_AIRCRAFT

 User's Aircraft             

            
