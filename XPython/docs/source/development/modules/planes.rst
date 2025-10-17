XPLMPlanes
==========
.. py:module:: XPLMPlanes
.. py:currentmodule:: xp

To use::

  import xp

The XPLMPlanes APIs allow you to control the various aircraft in X-Plane,
both the user's and the sim's.


Functions
---------

.. py:function:: setUsersAircraft(path)

 :param str path: Full or relative path to aircraft's ".acf" file.

 This routine changes the user's aircraft.  Note that this will reinitialize
 the user to be on the nearest airport's first runway **with its engines running**.

 *path* is either relative to X-Plane root, or fully qualified path, including the .acf
 extension.

 If the acf file cannot be found the user will be notified and their aircraft
 will be re-initialized.

 >>> xp.setUsersAircraft("Aircraft/Laminar Research/Cessna 172 SP/Cessna_172SP_G1000.acf")
 >>> xp.setUsersAircraft("/Volumes/SSD1/X-Plane/Aircraft/Laminar Research/Cessna 172 SP/Cessna_172SP_G1000.acf")

 `Official SDK <https://developer.x-plane.com/sdk/XPLMPlanes/#XPLMSetUsersAircraft>`__ :index:`XPLMSetUsersAircraft`

.. py:function::  placeUserAtAirport(code)

 :param str code: Airport ICAO code

 This routine places the user at a given airport.  Specify the airport by
 its ICAO code (e.g. 'KBOS').

 .. warning:: Using an invalid airport code will **crash the sim**, this includes using something other than ICAO. For example, 'JFK' does not work.

 .. note:: You cannot call ``placeUserAtAirport()`` in your Start or Enable callback as the (initial) user aircraft has not yet
           been placed (you will crash the sim). You may
           create a flight loop callback and then execute ``placeUserAtAirport()`` within the callback, or in response to a menu or command.

 >>> xp.placeUserAtAirport('KBOS')

 `Official SDK <https://developer.x-plane.com/sdk/XPLMPlanes/#XPLMPlaceUserAtAirport>`__ :index:`XPLMPlaceUserAtAirport`
 

.. py:function:: placeUserAtLocation(latitude, longitude, elevation, heading, speed)

 :param float latitude:
 :param float longitude: location in decimal degrees
 :param float elevation: meters MSL
 :param float heading: degrees True.
 :param float speed: meters per second
                     
 Places the user at a specific location after performing any necessary
 scenery loads.

 As with in-air starts initiated from the X-Plane user interface, the
 aircraft will always start with **its engines running**, regardless of the
 user's preferences (i.e., regardless of what the dataref
 ``sim/operation/prefs/startup_running`` says).

 | *elevation* is meters MSL (1 meter = 3.28084 feet),
 | *heading* is degrees True, (use :py:func:`getMagneticVariation` to convert.)
 | *speed* is meters per second (1 meter per second = 1.94384 knots).

 >>> xp.placeUserAtLocation(35, -122.5, 2000 / 3.28084, 90, 110 / 1.94384)

 `Official SDK <https://developer.x-plane.com/sdk/XPLMPlanes/#XPLMPlaceUserAtLocation>`__ :index:`XPLMPlaceUserAtLocation`
 
.. py:function::  countAircraft()

 :return: Tuple of three integers (max aircraft, current aircraft, controlling plugin)

 Return three integers representing:

 * the number of aircraft X-Plane is capable of having,

 * the number of aircraft that are currently active.

 * The pluginID of the plugin currently controlling the aircraft. (-1 for none)
 
 These numbers count the user's aircraft plus the number of AI Aircraft (to increase the max
 you would need to manually add more AI Aircraft).

 >>> xp.countAircraft()
 (4, 4, -1)

 `Official SDK <https://developer.x-plane.com/sdk/XPLMPlanes/#XPLMCountAircraft>`__ :index:`XPLMCountAircraft`
 
.. py:function:: getNthAircraftModel(int: index) -> (model, path):

 :param int index: 0-based index of aircraft information to get                
 :return: Tuple with two elements: (filename, full path)

 Return two strings based on the aircraft *index*. User's aircraft is always 0.

    * filename of aircraft
    * path to the model filename

 >>> xp.getNthAircraftModel(0)
 ('Cessna_172SP.acf', '/Volumes/SSD1/X-Plane/Aircraft/Laminar Research/Cessna 172SP/Cessna_172SP.acf')

 `Official SDK <https://developer.x-plane.com/sdk/XPLMPlanes/#XPLMGetNthAircraftModel>`__ :index:`XPLMGetNthAircraftModel`

.. py:function::  acquirePlanes(aircraft=None, callable=None, refCon=none)

 :param None aircraft: this parameter is deprecated
 :param Callable callable: Callback to notify you *when* you're able to acquire planes
 :param Any refCon: Reference Constant passed to your callback
 :return: 1 on successful acquisition.

 Grants your plugin exclusive access to the aircraft.  It
 returns 1 if you gain access, 0 if you do not. If you received 0 and you provided a callback,
 you'll get notification when acquired.

 In the simplest form, attempt to acquire all the aircraft:

 >>> xp.acquirePlanes()
 1

 If you provide a *callback*, **and do not immediately get access**, :py:func:`acquirePlanes` will
 return 0, and call you callback when able. Your ``callback()`` is passed the *refCon*.
 If you are able to acquire immediately, you callback will not be called.

 >>> def MyCallback(refCon):
 ...    xp.log("Acquired airplanes")
 ...
 >>> xp.acquirePlanes(callback=MyCallback)
 1

 *aircraft* is supposed to be a list of strings, specifying the planes you
 want to load, this does not appear to do anything in X-Plane 11, so keep it set
 to None. (Laminar has confirmed this does nothing in 11.5, and they suggest the parameter
 is deprecated and will not be fixed.)

 `Official SDK <https://developer.x-plane.com/sdk/XPLMPlanes/#XPLMAcquirePlanes>`__ :index:`XPLMAcquirePlanes`

.. py:function::  releasePlanes()

 Call this function to release access to the planes (:py:func:`acquirePlanes`).  Note that if your plugin is
 disabled, access to planes is released for you and you must reacquire it.

 `Official SDK <https://developer.x-plane.com/sdk/XPLMPlanes/#XPLMReleasePlanes>`__ :index:`XPLMReleasePlanes`

 
.. py:function:: setActiveAircraftCount(count)

 :param int count: Sets number of active planes (effectively reducing # of AI aircraft)

 This routine sets the number of active planes.  If you pass in a number
 higher than the total number of planes available, only the total number of
 planes available is actually used.

 You must have exclusive access to planes first (:py:func:`acquirePlanes`)

 >>> xp.countAircraft()
 (4, 4, -1)
 >>> xp.acquirePlanes()
 1
 >>> xp.setActiveAircraftCount(6)
 >>> xp.countAircraft()
 (4, 4, 3)  # '3' being my plugin ID
 >>> xp.setActiveAircraftCount(2)
 >>> xp.countAircraft()
 (4, 2, 3)  # '3' being my plugin ID
 >>> xp.releasePlanes()
 
 `Official SDK <https://developer.x-plane.com/sdk/XPLMPlanes/#XPLMSetActiveAircraftCount>`__ :index:`XPLMSetActiveAircraftCount`

.. py:function:: setAircraftModel(index, path)

 :param int index: aircraft index (i.e., which AI aircraft...)
 :param str path: Aircraft *.acf file

 This routine loads an aircraft model.  It may only be called if you  have
 exclusive access to the airplane APIs (:py:func:`acquirePlanes`).
 Pass in the *path* of the  model with
 the .acf extension. Path may be relative X-Plane Root.
 The *index* is zero based, but you  may not pass in 0
 (use :py:func:`setUsersAircraft` to load the user's aircraft).

 This *does not* add a new AI aircraft, if you use an index greater
 than current aircraft count. (It will do nothing.)

 If the aircraft path is invalid, the user will be notified.

 >>> xp.acquirePlanes()
 1
 >>> xp.setAircraftModel(2, 'Aircraft/Laminar Research/Boeing B737-800/b738.acf')
 >>> xp.releasePlanes()

 `Official SDK <https://developer.x-plane.com/sdk/XPLMPlanes/#XPLMSetAircraftModel>`__ :index:`XPLMSetAircraftModel`

.. py:function:: disableAIForPlane(index)

 :param int index: aircraft index                 

 This routine turns off X-Plane's AI for a given plane.  The plane will
 continue to be drawn and be a "real" plane in X-Plane, but will not  move itself.

 .. note:: There is no ``enableAIForPlane()`` function: you cannot
    simple re-enable AI. However, if you acquire all planes,
    set the active count to 1 (User aircraft only) and then reset the count to
    something larger than 1,
    all of the added aircraft will have their AI re-enabled once you call :py:func:`xp.releasePlanes`

    It seems this is likely an unintended side-effect, but works for XP11 and XP12.
    

 `Official SDK <https://developer.x-plane.com/sdk/XPLMPlanes/#XPLMDisableAIForPlane>`__ :index:`XPLMDisableAIForPlane`

Constants
---------

.. py:data:: USER_AIRCRAFT
 :value: 0

 User's Aircraft             

            
