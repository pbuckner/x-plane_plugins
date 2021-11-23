XPLMNavigation
==============
.. py:module:: XPLMNavigation
.. py:currentmodule:: xp

To use::

  import xp

The XPLMNavigation APIs give you some access to the navigation databases
inside X-Plane.  X-Plane stores all navigation information in RAM, so by
using these APIs you can gain access to most information without having to
go to disk or parse the files yourself.

You can also use this API to program the FMS.  You must use the navigation
APIs to find the navaids you want to program into the FMS, since the FMS
is powered internally by X-Plane's navigation database.


Navaid Functions
----------------

* Iterate through the navaid database:

  * :py:func:`getFirstNavAid`, :py:func:`getNextNavAid`

* Search database based on location, type, partials names:

  * :py:func:`findFirstNavAidOfType`, :py:func:`findLastNavAidOfType`

  * :py:func:`findNavAid`

* Get information about the navaid:

  * :py:func:`getNavAidInfo`

.. warning:: The values retrieved using these functions generally 
    match the format described in the Navigation Database document
    (e.g., `XP-NAV1150-Spec.pdf <http://developer.x-plane.com/wp-content/uploads/2020/03/XP-NAV1150-Spec.pdf>`_)
    For example, the heading value for Glideslopes is defined as:

      Associated localizer bearing in **true** degrees prefixed by glideslope angle times
      100,000. E.g., Glideslope of 3.00 degrees on heading of 122.53125 becomes 300122.53125.
              
    To add to the confusion, whereas the document indicate heights are in feet, it
    appears the values returned via the API are in meters.


  
.. py:function:: getFirstNavAid()

 This returns integer index (navRef) of the very first navaid in the database.
 Use this to traverse the entire database. Returns :py:data:`NAV_NOT_FOUND`
 if the navaid database is empty.

 >>> navRef = xp.getFirstNavAid()
 >>> navRef
 0
 
 `Official SDK <https://developer.x-plane.com/sdk/XPLMNavigation/#XPLMGetFirstNavAid>`__ :index:`XPLMGetFirstNavAid`


.. py:function:: getNextNavAid(navRef)

 Given a navRef, this routine returns the next (navRef).  It returns
 :py:data:`NAV_NOT_FOUND` if the navRef passed in was invalid or if
 was the last one in the database.  Use this routine to iterate
 across all like-typed navaids or the entire database. (Start with :py:func:`getFirstNavAid`.)

 .. note:: This merely returns the *next* navaid in the database. This does not return
  the next of the same type, or same query (see :py:func:`findFirstNavAidOfType`, or
  :py:func:`findNavAid`). It is very
  fast, so one strategy is to enumerate through the full navaid database if you're trying
  to do anything complicated (e.g., find all navaids with the same frequency).

 .. warning:: due to a bug in the SDK, when fix loading is disabled in the
  rendering settings screen, calling this routine with the last airport
  returns a bogus navaid.  Using this navaid can crash X-Plane.

 >>> navRef = xp.getFirstNavAid()
 >>> navRef = xp.getNextNavAid(navRef)
 >>> navRef
 1

 `Official SDK <https://developer.x-plane.com/sdk/XPLMNavigation/#XPLMGetNextNavAid>`__ :index:`XPLMGetNextNavAid`

.. py:function:: findFirstNavAidOfType(navType)

 Given a *navType* (See :ref:`XPLMNavType` below),
 return the navRef of the first navaid of the given
 type in the
 database or :py:data:`NAV_NOT_FOUND` if there are no navaids of that type in the
 database.  *You must pass exactly one navaid type to this routine.*

 .. warning:: Due to a bug in the SDK, when fix loading is disabled in the
  rendering settings screen, calling this routine with fixes returns a bogus
  navaid.  Using this navaid can crash X-Plane.

 >>> xp.findFirstNavAidOfType(navType=xp.Nav_DME)
 18826
 >>> xp.findFirstNavAidOfType(navType=xp.Nav_DME | xp.Nav_VOR)
 -1
 
 `Official SDK <https://developer.x-plane.com/sdk/XPLMNavigation/#XPLMFindFirstNavAidOfType>`__ :index:`XPLMFindFirstNavAidOfType`

.. py:function:: findLastNavAidOfType(navType)

 Give a *navType* (See :ref:`XPLMNavType` below),
 return the navRef of the last navaid of the given type
 database or :py:data:`NAV_NOT_FOUND` if there are no navaids of that type in the
 database.  *You must pass exactly one navaid type to this routine.*

 You'll note there is no ``findNextNavAidOfType()`` function. A common work around
 is to load all navaid information into your plugin and then search within that
 data structure.

 .. warning:: Due to a bug in the SDK, when fix loading is disabled in the
  rendering settings screen, calling this routine with fixes returns a bogus
  navaid.  Using this navaid can crash X-Plane.

 >>> xp.findLastNavAidOfType(navType=xp.Nav_DME)
 26189

 `Official SDK <https://developer.x-plane.com/sdk/XPLMNavigation/#XPLMFindLastNavAidOfType>`__ :index:`XPLMFindLastNavAidOfType`

.. py:function:: findNavAid(name=None, navAidID=None, lat=None, lon=None, freq=None, navType=-1)

 This routine provides a number of searching capabilities for the navaid
 database. :py:func:`findNavAid` will search through every navaid whose type is
 within *navType* (See :ref:`XPLMNavType`.)
 Multiple types may be OR'd together, with the default being "any" (matches imply
 at least one of the OR'd navTypes match *not* all).
 :py:func:`findNavAid` returns **one** navRef based on the following rules:

 * If *lat* and *lon* are specified, the navaid nearest to that lat/lon will be
   returned, otherwise the *last* navaid found will be returned.

 * If *freq* is provided , then any navaids considered must match this
   frequency.  Note that this will screen out radio beacons that do not have
   frequency data published (like inner markers) but not fixes and airports.
   **Note this is an integer**, frequency input is real frequency time 100 to create a integer (e.g., specify
   13775 to search for 137.75).

 * If *name* is provided, only navaids that contain the fragment in
   their name will be returned. (Search is case-sensitive, so "Oakland" will find KOAK, but "OAKLAND" will not.

 * If *navAidID* is provided, only navaids that contain the fragment in their IDs will be returned.

 This routine provides a simple way to do a number of useful searches:

 Find the nearest navaid on this frequency. Find the nearest airport. Find
 the VOR whose ID is "KBOS". Find the nearest airport whose name contains
 "Chicago".

 >>> xp.findNavAid(name="Chicago", navType=xp.Nav_Airport | xp.Nav_DME)
 16813190
 >>> xp.findNavAid(name="OAK")
 16800138
 >>> xp.findNavAid(name="OAK", lat=35, lon=-122)
 33705283

 `Official SDK <https://developer.x-plane.com/sdk/XPLMNavigation/#XPLMFindNavAid>`__ :index:`XPLMFindNavAid`
 
.. py:function:: getNavAidInfo(navRef)
 
 See warning about Navaid values above.

 This routine returns information about a navaid indicated by its *navRef*.  Fields are
 filled out with information if it is available. For example, Airports have neither
 frequency nor heading, so they will always be zero. Fixes (:data:`Nav_Fix`) do not
 have height, frequency or heading. There is no way to distinguish between true values
 of zero and missing values.

 Frequencies are in the nav.dat convention as described in the X-Plane nav
 database FAQ: NDB frequencies are exact, all others are multiplied by 100.

 The ``reg`` field tells if the navaid is within the local "region" of
 loaded DSFs.  (This information may not be particularly useful to plugins.)
 (Unlike C API, for python, this parameter is a single byte value 1 for true
 or 0 for false, not a string.)

 Returned value is a NavAidInfo object the attributes

 ============ ===========================================================
 type         :ref:`XPLMNavType`
 latitude     float
 longitude    float
 height       float (in meters)
 frequency    int. For NDB, frequency is exact. Otherwise,
              divide by 100 to get actual.
 heading      float
 name         str
 navAidID     str
 reg          int 1= navaid is within the local "region" of loaded DSFs.
 ============ ===========================================================

 Conveniently, it also has a ``str()`` representation

 >>> navRef = xp.findNavAid(name="OAK", lat=35, lon=-122, navType=xp.Nav_Airport)
 >>> navRef
 16793550
 >>> navInfo = xp.getNavAidInfo(navRef)
 >>> navInfo
 <xppython3.NavAidInfo object at 0x7f84a16a3220>
 >>> navInfo.name
 'LIVE OAK CO'
 >>> navInfo.navAidID
 '8T6'
 >>> navInfo.latitude
 28.36280
 >>> print(navInfo)
 LIVE OAK CO (8T6) (28.363, -98.116) ---

 `Official SDK <https://developer.x-plane.com/sdk/XPLMNavigation/#XPLMGetNavAidInfo>`__ :index:`XPLMGetNavAidInfo`

Flight Management Computer
--------------------------
Some aircraft have a Flight Management System which responds to these
commands. Some do not. *sigh* The Laminar G530 mostly works.

Note: the FMS works based on an array of entries.  Indices into the array
are zero-based.  Each entry is a nav-aid plus an altitude.  The FMS tracks
the currently displayed entry and the entry that it is flying to.

The FMS must be programmed with contiguous entries, so clearing an entry at
the end shortens the effective flight plan.  There is a max of 100
waypoints in the flight plan.

* Manipulate entry status in FMS

  * :py:func:`countFMSEntries`

  * :py:func:`getDisplayedFMSEntry`, :py:func:`setDisplayedFMSEntry`

  * :py:func:`getDestinationFMSEntry`, :py:func:`setDestinationFMSEntry`

  * :py:func:`getGPSDestinationType`, :py:func:`getGPSDestination`

* Manipulate a particular FMS entry

  * :py:func:`getFMSEntryInfo`, :py:func:`setFMSEntryInfo`, :py:func:`setFMSEntryLatLon`

  * :py:func:`clearFMSEntry`

.. py:function:: countFMSEntries()

 Returns the number of entries in the FMS.

 >>> xp.countFMSEntries()
 6

 `Official SDK <https://developer.x-plane.com/sdk/XPLMNavigation/#XPLMCountFMSEntries>`__ :index:`XPLMCountFMSEntries`

.. py:function:: getDisplayedFMSEntry()

 Return the index of the entry the pilot is viewing. (For XP 11.55, this appears to always return 0.
 This has been acknowledged by Laminar Research as bug XPD-11386. The X-Plane 10 737 works correctly,
 but the 737 which comes with X-Plane 11 does not.

 Practically speaking, assume this does not work.

 >>> xp.getDisplayedFMSEntry()
 0

 `Official SDK <https://developer.x-plane.com/sdk/XPLMNavigation/#XPLMGetDisplayedFMSEntry>`__ :index:`XPLMGetDisplayedFMSEntry`

.. py:function:: getDestinationFMSEntry()

 Return the index of the entry the FMS is flying to. (The "destination" refers to the active leg.)
 This is an index into the FMS, not into the navaid database.

 >>> xp.getDestinationFMSEntry()
 0

 `Official SDK <https://developer.x-plane.com/sdk/XPLMNavigation/#XPLMGetDestinationFMSEntry>`__ :index:`XPLMGetDestinationFMSEntry`

.. py:function:: setDisplayedFMSEntry(index)

 Change which entry the FMS is showing to the integer index specified.
 (For X-Plane 11.55, this does not appear to do anything.  This has been acknowledged by Laminar Research as bug XPD-11386. Like :py:func:`getDisplayedFMSEntry`, this appears to work in X-Plane 11 only for older X-Plane 10 version
 of the 737 aircraft.)

 >>> xp.setDestinationFMSEntry(3)

 `Official SDK <https://developer.x-plane.com/sdk/XPLMNavigation/#XPLMSetDisplayedFMSEntry>`__ :index:`XPLMSetDisplayedFMSEntry`

.. py:function:: setDestinationFMSEntry(index)

 Change which entry the FMS is flying the aircraft toward.

 >>> xp.getDestinationFMSEntry()
 0
 >>> xp.setDestinationFMSEntry(3)
 >>> xp.getDestinationFMSEntry()
 3

 `Official SDK <https://developer.x-plane.com/sdk/XPLMNavigation/#XPLMSetDestinationFMSEntry>`__ :index:`XPLMSetDestinationFMSEntry`

.. py:function:: getFMSEntryInfo(index)

 Returns information about the zero-based *index* entry in the FMS.
 Value returned is an object (see below).

 A reference to a
 navaid can be returned allowing you to find additional information (such as
 a frequency, ILS heading, name, etc.).  Some information is available
 immediately.  For a lat/lon entry, the lat/lon is returned by this routine
 but the navaid cannot be looked up (and the reference will be
 :py:data:`XPLM_NAV_NOT_FOUND`.

 .. Note::
    X-Plane C SDK function takes many parameters, where the data is returned.
    The XPPython3 function takes a single parameter (Index) and returns all
    the values in an FMSEntryInfo object.

 Object returned by :py:func:`getFMSEntryInfo` containing
 information about an entry. It has the following attributes:

 ============ ===================================================
 type         :ref:`XPLMNavType` or :py:data:`XPLM_NAV_NOT_FOUND`
              if this is a lat/lon entry
 navAidID     str or None if this is a lat/lon entry
 ref          :ref:`XPLMNavRef` or None
 altitude     int (in feet)
 lat          float latitude
 lon          float longitude
 ============ ===================================================


 Conveniently, it also has a ``str()`` representation
 
 >>> info = xp.getFMSEntryInfo(0)
 >>> info
 <xppython3.FMSEntryInfo object at 0x7ff38c0a5040>
 >>> info.navAidID
 'LUCOS'
 >>> info.ref
 337064980
 >>> print(info)
 Fix: [337064980] LUCOS, (41.638, -70.768) @0'
 >>> print(xp.getNavAidInfo(info.ref))
 LUCOS (LUCOS) Fix (41.638, -70.768) ---

 `Official SDK <https://developer.x-plane.com/sdk/XPLMNavigation/#XPLMGetFMSEntryInfo>`__ :index:`XPLMGetFMSEntryInfo`
 
.. py:function::  setFMSEntryInfo(index, navRef, altitude=0)

 This routine changes an entry at *index* in the FMS to have the
 destination navaid specified by *navRef* (as returned by :py:func:`findNavAid`)
 at *altitude* (in feet) specified.
 Use this only for airports, fixes,
 and radio-beacon navaids.  Currently for radio beacons, the FMS can only
 support VORs and NDBs.

 Use the :py:func:`setFMSEntryLatLon` fly to a lat/lon.

 >>> navRef = xp.findNavAid(navType=xp.Nav_VOR)
 >>> xp.setFMSEntryInfo(3, navRef, 1000)
 >>> print(xp.getFMSEntryInfo(3))
 VOR: [9469] ZDA, (44.095, 15.364) @1000'

 `Official SDK <https://developer.x-plane.com/sdk/XPLMNavigation/#XPLMSetFMSEntryInfo>`__ :index:`XPLMSetFMSEntryInfo`

.. py:function:: setFMSEntryLatLon(index, lat, lon, altitude=0)

 This routine changes an entry at *index* in the FMS to have the
 destination specified by *lat*, *lon* at *altitude* (in feet) specified.

 >>> xp.setFMSEntryLatLon(3, 34, -122.5, 1000)
 >>> print(xp.getFMSEntryInfo(3))
 LatLon: (34.000, -122.5) @1000'

 `Official SDK <https://developer.x-plane.com/sdk/XPLMNavigation/#XPLMSetFMSEntryLatLon>`__ :index:`XPLMSetFMSEntryLatLon`
 
.. py:function::  clearFMSEntry(index)

 Clears the given entry, potentially shortening the flight plan.

 `Official SDK <https://developer.x-plane.com/sdk/XPLMNavigation/#clearFMSEntry>`__ :index:`clearFMSEntry`

.. py:function::  getGPSDestinationType()

 Return the :ref:`XPLMNavType` of the current GPS destination.

 >>> xp.getGPSDestinationType()
 512
 
 `Official SDK <https://developer.x-plane.com/sdk/XPLMNavigation/#XPLMGetGPSDestinationType>`__ :index:`XPLMGetGPSDestinationType`


.. py:function:: getGPSDestination()

 Return the :ref:`XPLMNavRef` of current GPS destination.

 >>> xp.getGPSDestination()
 33706498
 >>> print(xp.getNavAidInfo(xp.getGPSDestination()))
 LUCOS (LUCOS) Fix (41.638, -70.768) ---

 `Official SDK <https://developer.x-plane.com/sdk/XPLMNavigation/#XPLMGetGPSDestination>`__ :index:`XPLMGetGPSDestination` 

Constants
---------

.. _XPLMNavType:

:index:`XPLMNavType`
********************
These enumerations define the different types of navaids.  They are each
defined with a separate bit so that they may be bit-wise added together to
form sets of nav-aid types.

 .. table::
   :align: left

   ============================= =====
   .. py:data:: Nav_Unknown      =0
   .. py:data:: Nav_Airport      =1
   .. py:data:: Nav_NDB          =2
   .. py:data:: Nav_VOR          =4
   .. py:data:: Nav_ILS          =8
   .. py:data:: Nav_Localizer    =16
   .. py:data:: Nav_GlideSlope   =32
   .. py:data:: Nav_OuterMarker  =64
   .. py:data:: Nav_MiddleMarker =128
   .. py:data:: Nav_InnerMarker  =256
   .. py:data:: Nav_Fix          =512
   .. py:data:: Nav_DME          =1024
   .. py:data:: Nav_LatLon       =2048
   ============================= =====

.. note:: ``Nav_LatLon`` is a specific lat-lon coordinate entered into the
 FMS. It will not exist in the database, and cannot be programmed into the
 FMS. Querying the FMS for navaids will return it.  Use
 :py:func:`setFMSEntryLatLon` to set a lat/lon waypoint.

`Official SDK <https://developer.x-plane.com/sdk/XPLMNavigation/#XPLMNavType>`__ :index:`XPLMNavType`     

.. _XPLMNavRef:

:index:`XPLMNavRef`
*******************

XPLMNavRef is an iterator into the navigation database.  The navigation
database is essentially an array, but it is not necessarily densely
populated. The only assumption you can safely make is that like-typed
nav-aids are  grouped together.

Use XPLMNavRef to refer to a nav-aid.

 .. py:data:: XPLM_NAV_NOT_FOUND
  :value: -1

 XPLM_NAV_NOT_FOUND is returned by functions that return an :ref:`XPLMNavRef` when
 the iterator must be invalid.

`Official SDK <https://developer.x-plane.com/sdk/XPLMNavigation/#XPLMNavRef>`__ :index:`XPLMNavRef`

