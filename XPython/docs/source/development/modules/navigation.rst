XPLMNavigation
==============
.. py:module:: XPLMNavigation

To use::

  import XPLMNavigation

The XPLM Navigation APIs give you some access to the navigation databases
inside X-Plane.  X-Plane stores all navigation information in RAM, so by
using these APIs you can gain access to most information without having to
go to disk or parse the files yourself.

You can also use this API to program the FMS.  You must use the navigation
APIs to find the nav-aids you want to program into the FMS, since the FMS
is powered internally by X-Plane's navigation database.


Nav Aid Functions
-----------------

.. warning:: The values retrieved using these functions generally 
    match the format described in the Navigation Database document
    (e.g., `XP-NAV1150-Spec.pdf <http://developer.x-plane.com/wp-content/uploads/2020/03/XP-NAV1150-Spec.pdf>`_)
    For example, the frequency value for Glideslopes is defined as:

      Associated localizer bearing in **true** degrees prefixed by glideslope angle times
      100,000. E.g., Glideslope of 3.25 degrees on heading of 123.456 becomes 325123.456.
              
    To add to the confusion, whereas the document indicate heights are in feet, it
    appears the values returned via the API are in meters.

.. py:data:: NavAidInfo

 Object returned by :py:func:`XPLMGetNavAidInfo` containing
 information about a nav aid. It has the following attributes:

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
 reg          int 1= nav aid is within the local "region" of loaded DSFs.
 ============ ===========================================================

  
.. py:function:: XPLMGetFirstNavAid(None) -> navRef:

 :return:  int navRef :ref:`XPLMNavRef`                 

 This returns integer index of the very first navaid in the database.
 Use this to traverse the entire database. Returns :py:data:`XPLM_NAV_NOT_FOUND`
 if the nav database is empty.


.. py:function:: XPLMGetNextNavAid(navRef) -> navRef:

 :param int navRef: :ref:`XPLMNavRef`                 
 :return:  int navRef :ref:`XPLMNavRef`                 

 Given a navRef, this routine returns the next navaid (navRef).  It returns
 :py:data:`XPLM_NAV_NOT_FOUND` if the nav aid passed in was invalid or if the navaid
 passed in was the last one in the database.  Use this routine to iterate
 across all like-typed navaids or the entire database.

 .. note:: This merely returns the *next* nav aid in the database. This does not return
  the next of the same type, or same query (see :py:func:`XPLMFindFirstNavAidOfType`, or
  :py:func:`XPLMFindNavAid`). It is very
  fast, so one strategy is to enumerate through the full nav aid database if you're trying
  to do anything complicated (e.g., find all nav aids with the same frequency).

 .. warning:: due to a bug in the SDK, when fix loading is disabled in the
  rendering settings screen, calling this routine with the last airport
  returns a bogus nav aid.  Using this nav aid can crash X-Plane.


.. py:function:: XPLMFindFirstNavAidOfType(navType) -> navRef:

 :param int navType: :ref:`XPLMNavType`                 
 :return:  int navRef :ref:`XPLMNavRef`                 

 This routine returns the ref of the first navaid of the given type in the
 database or :py:data:`XPLM_NAV_NOT_FOUND` if there are no navaids of that type in the
 database.  You must pass exactly one nav aid type to this routine.

 .. warning:: Due to a bug in the SDK, when fix loading is disabled in the
  rendering settings screen, calling this routine with fixes returns a bogus
  nav aid.  Using this nav aid can crash X-Plane.


.. py:function:: XPLMFindLastNavAidOfType(navType) -> navRef:

 :param int navType: :ref:`XPLMNavType`                 
 :return:  int navRef :ref:`XPLMNavRef`                 

 This routine returns the ref of the last navaid of the given type in the
 database or :py:data:`XPLM_NAV_NOT_FOUND` if there are no navaids of that type in the
 database.  You must pass exactly one nav aid type to this routine.

 .. warning:: Due to a bug in the SDK, when fix loading is disabled in the
  rendering settings screen, calling this routine with fixes returns a bogus
  nav aid.  Using this nav aid can crash X-Plane.


.. py:function:: XPLMFindNavAid(name, ID, lat, lon, freq, navType) ->

 :param str name: Name fragment to be used while searching (or None)
 :param str ID: ID fragment to be used while searching (or None)
 :param float lat:
 :param float lon: latitude and longitude to be used while searching (or both None)   
 :param int freq: Frequency to be used while searching. **Note this is an integer**. To search
   for ``114.70``, use ``11470``, for ``109.00`` use ``10900``. 
 :param int navType: :ref:`XPLMNavType`                 
 :return:  int navRef :ref:`XPLMNavRef`                 

 This routine provides a number of searching capabilities for the nav
 database. :py:func:`XPLMFindNavAid` will search through every nav aid whose type is
 within inType (multiple types may be added together) and return any
 nav-aids found based  on the following rules:

 * If ``lat`` and ``lon`` are not None, the navaid nearest to that lat/lon will be
   returned, otherwise the last navaid found will be returned.

 * If ``freq`` is not None, then any navaids considered must match this
   frequency.  Note that this will screen out radio beacons that do not have
   frequency data published (like inner markers) but not fixes and airports. (Note frequency
   input is real frequency x100 to create a integer).

 * If ``name`` is not None, only navaids that contain the fragment in
   their name will be returned.

 * If ``ID`` is not None, only navaids that contain the fragment in their IDs will be returned.

 This routine provides a simple way to do a number of useful searches:

 Find the nearest navaid on this frequency. Find the nearest airport. Find
 the VOR whose ID is "KBOS". Find the nearest airport whose name contains
 "Chicago".


.. py:function:: XPLMGetNavAidInfo(ref) -> navAidInfo:
 
 :param ref: :ref:`XPLMNavRef`
 :return: navAidInfo :py:data:`NavAidInfo`

 See warning about Nav Aid values above.

 This routine returns information about a navaid.  Fields are
 filled out with information if it is available. For example, Airports have neither
 frequency nor heading, so they will alway be zero. Fixes (:data:`xplm_Nav_Fix`) do not
 have height, frequency or heading. There is no way to distinguish between true values
 of zero and missing values.

 Frequencies are in the nav.dat convention as described in the X-Plane nav
 database FAQ: NDB frequencies are exact, all others are multiplied by 100.

 The ``reg`` field tells if the navaid is within the local "region" of
 loaded DSFs.  (This information may not be particularly useful to plugins.)
 (Unlike C API, for python, this parameter is a single byte value 1 for true
 or 0 for false, not a string.)


Flight Management Computer
--------------------------
Note: the FMS works based on an array of entries.  Indices into the array
are zero-based.  Each entry is a nav-aid plus an altitude.  The FMS tracks
the currently displayed entry and the entry that it is flying to.

The FMS must be programmed with contiguous entries, so clearing an entry at
the end shortens the effective flight plan.  There is a max of 100
waypoints in the flight plan.


.. py:function:: XPLMCountFMSEntries(None) -> int:

 This routine returns the number of entries in the FMS.


.. py:function:: XPLMGetDisplayedFMSEntry(None) -> int:

 This routine returns the index of the entry the pilot is viewing.


.. py:function:: XPLMGetDestinationFMSEntry(None) -> int:

 This routine returns the index of the entry the FMS is flying to.


.. py:function:: XPLMSetDisplayedFMSEntry(index):

 This routine changes which entry the FMS is showing to the integer index specified.

.. py:function:: XPLMSetDestinationFMSEntry(index):

 This routine changes which entry the FMS is flying the aircraft toward.

.. py:data:: FMSEntryInfo

 Object returned by :py:func:`XPLMGetFMSEntryInfo` containing
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


.. py:function:: XPLMGetFMSEntryInfo(inIndex) -> fmsEntryInfo:

 :param int index: zero-based index of FMS entries
 :return: fmsEntryInfo :data:`FMSEntryInfo` object                  

 This routine returns information about a given FMS entry.  A reference to a
 navaid can be returned allowing you to find additional information (such as
 a frequency, ILS heading, name, etc.).  Some information is available
 immediately.  For a lat/lon entry, the lat/lon is returned by this routine
 but the navaid cannot be looked up (and the reference will be
 :py:data:`XPLM_NAV_NOT_FOUND`.


.. py:function::  XPLMSetFMSEntryInfo(index, navRef, altitude) -> None:

 :param int index: zero-based index of FMS entries
 :param int navRef: :ref:`XPLMNavRef`
 :param int altitude: altitude (in feet)

 This routine changes an entry in the FMS to have the destination navaid
 passed in and the altitude specified.  Use this only for airports, fixes,
 and radio-beacon navaids.  Currently of radio beacons, the FMS can only
 support VORs and NDBs. Use the routines below to clear or fly to a lat/lon.


.. py:function:: XPLMSetFMSEntryLatLon(index, lat, lon, altitude) -> None:

 :param int index: zero-based index of FMS entries
 :param float lat: latitude (degrees)
 :param float lon: longitude (degrees)
 :param int altitude: altitude (in feet)

 This routine changes the entry in the FMS to a lat/lon entry with the given
 coordinates.

.. py:function::  XPLMClearFMSEntry(index) -> None:

 This routine clears the given entry, potentially shortening the flight plan.

.. py:function::  XPLMGetGPSDestinationType(None) -> navType:

 :return:  int navType :ref:`XPLMNavType` of current GPS destination, one of fix, airport, VOR or NDB.


.. py:function:: XPLMGetGPSDestination(None) -> navRef:

 :return:  int navRef :ref:`XPLMNavRef` of current GPS destination                 
  
Constants
---------

.. _XPLMNavType:

XPLMNavType
***********
These enumerations define the different types of navaids.  They are each
defined with a separate bit so that they may be bit-wise added together to
form sets of nav-aid types.

 .. py:data::
    xplm_Nav_Unknown
    xplm_Nav_Airport
    xplm_Nav_NDB
    xplm_Nav_VOR
    xplm_Nav_ILS
    xplm_Nav_Localizer
    xplm_Nav_GlideSlope
    xplm_Nav_OuterMarker
    xplm_Nav_MiddleMarker
    xplm_Nav_InnerMarker
    xplm_Nav_Fix
    xplm_Nav_DME
    xplm_Nav_LatLon

.. note: xplm_Nav_LatLon is a specific lat-lon coordinate entered into the
 FMS. It will not exist in the database, and cannot be programmed into the
 FMS. Querying the FMS for navaids will return it.  Use
 :py:func:`XPLMSetFMSEntryLatLon` to set a lat/lon waypoint.

.. _XPLMNavRef:

XPLMNavRef
**********

XPLMNavRef is an iterator into the navigation database.  The navigation
database is essentially an array, but it is not necessarily densely
populated. The only assumption you can safely make is that like-typed
nav-aids are  grouped together.

Use XPLMNavRef to refer to a nav-aid.

 .. py:data:: XPLM_NAV_NOT_FOUND

 XPLM_NAV_NOT_FOUND is returned by functions that return an :ref:`XPLMNavRef` when
 the iterator must be invalid.

