XPLMNavigation
==============
.. py:module:: XPLMNavigation
.. py:currentmodule:: xp

To use::

  import xp

The XPLMNavigation APIs give you some access to the navigation databases
inside X-Plane.  X-Plane stores all navigation information in RAM, so by
using these APIs you can gain access to most information without having to
go to disk or parse the files yourself. See :ref:`navaid_functions`.

You can also use this API to program the FMS.  You must use the navigation
APIs to find the navaids you want to program into the FMS, since the FMS
is powered internally by X-Plane's navigation database. See :ref:`fms_flightplan`.


.. _navaid_functions:

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

 :return: :ref:`XPLMNavRef` or :py:data:`NAV_NOT_FOUND`

 This returns integer index (XPLMNavRef) of the very first navaid in the database.
 Use this to traverse the entire database. Returns :py:data:`NAV_NOT_FOUND`
 if the navaid database is empty.

 >>> navRef = xp.getFirstNavAid()
 >>> navRef
 0
 
 `Official SDK <https://developer.x-plane.com/sdk/XPLMNavigation/#XPLMGetFirstNavAid>`__ :index:`XPLMGetFirstNavAid`


.. py:function:: getNextNavAid(navRef)

 :param: :ref:`XPLMNavRef` navRef
 :return: Next :ref:`XPLMNavRef` navRef or :py:data:`NAV_NOT_FOUND`

 Given a navRef, this routine returns the next navRef.  It returns
 :py:data:`NAV_NOT_FOUND` if the navRef passed in was invalid or if
 was the last one in the database.  Use this routine to iterate
 across all like-typed navaids or the entire database. (Start with :py:func:`getFirstNavAid`.)

 .. note:: This merely returns the *next* navaid in the database. This does not return
  the next of the same type, or same query (see :py:func:`findFirstNavAidOfType`, or
  :py:func:`findNavAid`). It is very
  fast, so one strategy is to enumerate through the full navaid database if you're trying
  to do anything complicated (e.g., find all navaids with the same frequency). If you're
  looking for all navaids of the same type, you can iterate between ``getFirstNavAidOfType()``
  and ``getLastNavAidOfType()``: navaids of the same type are guaranteed to be grouped
  together, though they are not guaranteed to be sequentially continuous.

 >>> navRef = xp.getFirstNavAid()
 >>> navRef = xp.getNextNavAid(navRef)
 >>> navRef
 1

 `Official SDK <https://developer.x-plane.com/sdk/XPLMNavigation/#XPLMGetNextNavAid>`__ :index:`XPLMGetNextNavAid`

.. py:function:: findFirstNavAidOfType(navType)

 :param: :ref:`XPLMNavType` navType for search
 :return: First :ref:`XPLMNavRef` navRef or :py:data:`NAV_NOT_FOUND`

 Given a *navType* (See :ref:`XPLMNavType` below),
 return the navRef of the first navaid of the given
 type in the
 database or :py:data:`NAV_NOT_FOUND` if there are no navaids of that type in the
 database.  *You must pass exactly one navaid type to this routine.*

 >>> xp.findFirstNavAidOfType(navType=xp.Nav_DME)
 18826
 >>> xp.findFirstNavAidOfType(navType=xp.Nav_DME | xp.Nav_VOR)
 -1
 
 `Official SDK <https://developer.x-plane.com/sdk/XPLMNavigation/#XPLMFindFirstNavAidOfType>`__ :index:`XPLMFindFirstNavAidOfType`

.. py:function:: findLastNavAidOfType(navType)

 :param: :ref:`XPLMNavType` navType for search
 :return: Last :ref:`XPLMNavRef` navRef or :py:data:`NAV_NOT_FOUND`

 Given a *navType* (See :ref:`XPLMNavType` below),
 return the navRef of the last navaid of the given type
 database or :py:data:`NAV_NOT_FOUND` if there are no navaids of that type in the
 database.  *You must pass exactly one navaid type to this routine.*

 You'll note there is no ``findNextNavAidOfType()`` function. Because all navaids of the same type
 are grouped together, you can iterate between ``findFirstNavAidOfType()`` and ``findLastNavAidOfType()``.
 A common work around is to load all navaid information into your plugin and then search within that
 data structure.

 >>> xp.findLastNavAidOfType(navType=xp.Nav_DME)
 26189

 `Official SDK <https://developer.x-plane.com/sdk/XPLMNavigation/#XPLMFindLastNavAidOfType>`__ :index:`XPLMFindLastNavAidOfType`

.. py:function:: findNavAid(name=None, navAidID=None, lat=None, lon=None, freq=None, navType=-1)

 :param str name: case-sensitive fragment to search
 :param str navAidID: case-sensitive fragment to search
 :param float lat: latitude near navaid
 :param float lon: longitude near navaid
 :param int freq: integer representation of frequency (see below)
 :param XPLMNavType navType: OR'd together set of :ref:`XPLMNavType`
 :return: :ref:`XPLMNavRef` of *first* match
  
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
   **Note this is an integer**, frequency input is real frequency times 100 to create a integer (e.g., specify
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

 .. warning:: This function may lead you astray. You might think search
              for ``navAidID="EDDF", navType=xp.Nav_Airport``
              might retrieve Frankfurt Airport (EDDF). But instead it will retrieve the *first match*
              which is the airport Karlstadt Saupurzel (XEDDF). Check your results!

 `Official SDK <https://developer.x-plane.com/sdk/XPLMNavigation/#XPLMFindNavAid>`__ :index:`XPLMFindNavAid`
 
.. py:function:: getNavAidInfo(navRef)

   :param: :ref:`XPLMNavRef` integer retrieved from e.g., :py:func:`findNavAid`             
   :return: :ref:`NavAidInfo` or None

 See information about returned :ref:`NavAidInfo` data structure below.
 
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
 or 0 for false, not a string.)::

    >>> navRef = xp.findNavAid(name="OAK", lat=35, lon=-122, navType=xp.Nav_Airport)
    >>> navRef
    16793550
    >>> navInfo = xp.getNavAidInfo(navRef)
    >>> navInfo
    <xppython3.NavAidInfo object at 0x7f84a16a3220>
    >>> navInfo.name
    'LIVE OAK CO'
    
.. _fms_flightplan:

Flight Management Flight Plans
------------------------------

X-Plane 12.1 allows you to specific which flight plan you want to interact with. These routines replace similar
routines from previous versions of X-Plane which provide access only to the main "pilot" flight plan. In general,
you can replace calls using the older interface with calls to 12.1 interface by including the parameter specifying
the ``Fpl_Pilot_Primary`` flight plan. Full set of flight plan types are listed :ref:`XPLMNavFlightPlan`.

Note: the FMS works based on an array of entries.  Indices into the array
are zero-based.  Each entry is a nav-aid plus an altitude.  The FMS tracks
the currently displayed entry and the entry that it is flying to ("destination entry").

The FMS must be programmed with contiguous entries, so clearing an entry at
the end shortens the effective flight plan.  There is a max of 100
waypoints in the flight plan.

* Manipulate entry status in FMS

  * :py:func:`countFMSFlightPlanEntries`

  * :py:func:`getDisplayedFMSFlightPlanEntry`, :py:func:`setDisplayedFMSFlightPlanEntry`

  * :py:func:`getDestinationFMSFlightPlanEntry`, :py:func:`setDestinationFMSFlightPlanEntry`

  * :py:func:`setDirectToFMSFlightPlanEntry`

* Manipulate a particular FMS entry

  * :py:func:`getFMSFlightPlanEntryInfo`, :py:func:`setFMSFlightPlanEntryInfo`, :py:func:`setFMSFlightPlanEntryLatLon`, :py:func:`setFMSFlightPlanEntryLatLonWithId`

  * :py:func:`clearFMSFlightPlanEntry`

* Load and Save a flight plan :py:func:`loadFMSFlightPlan`, :py:func:`saveFMSFlightPlan`

For most of our examples below, we'll be using the flight plan described in X-Plane's `Airbus MCDU Manual <https://www.x-plane.com/manuals/Airbus_MCDU_Manual.pdf>`__. In there, they describe inputting a flight plan for the route:

   ``EDDS25 ETAS4B T163 SPESA SPES3B ILS25L``

We'll input that same route programmatically.   

Functions
*********

.. py:function:: loadFMSFlightPlan(device, plan)

  Loads specially formatted string as the flight plan for the selected device

  :param int device: 0= pilot side, 1= co-pilot side
  :param str plan: X-Plane 11+ formatted flight plan                   
  :return: Return

  The format of the flight plan is X-Plane specific, and is described in `Flightplan files - v11 .fms file format <https://developer.x-plane.com/article/flightplan-files-v11-fms-file-format/>`__.

  .. _edds_eddf.fms:

  Assume the file ``edds-eddf.fms`` with contents:

  .. code:: none
    
      I
      1100 Version
      CYCLE 2112 
      ADEP EDDS 
      DEPRWY RW25 
      SID ETAS4B 
      ADES EDDF 
      DESRWY RW25L 
      STAR SPES3B 
      APP I25L 
      APPTRANS CHA 
      NUMENR 6 
      1 EDDS ADEP 1272.000000 48.689877 9.221964 
      11 XINLA T163 0.000000 49.283646 9.141608 
      11 SUKON T163 0.000000 49.659721 9.195556 
      11 SUPIX T163 0.000000 49.727779 9.305278 
      11 SPESA T163 0.000000 49.862240 9.348325 
      1 EDDF ADES 354.000000 50.033306 8.570456  
            
  You can load that file directly into the pilot's primary FMS using::
  
    >>> with open("edds-eddf.fms", "r") as fp:
    ...    plan = fp.read()
    >>> xp.loadFMSFlightPlan(0, plan)
    >>> xp.countFMSFlightPlanEntries(xp.Fpl_Pilot_Primary)
    22
    
  (Your results may differ, as this is dependent on the current X-Plane navigation database).
  
  `Official SDK <https://developer.x-plane.com/sdk/XPLMNavigation/#XPLMLoadFMSFlightPlan>`__ :index:`XPLMLoadFMSFlightPlan`

.. py:function:: saveFMSFlightPlan(device)

  Returns a string representing flight plan for selected device
  
  :param int device: 0= pilot side, 1= co-pilot side
  :return str: X-Plane 11+ formatted flight plan                   

  The format of the flight plan is X-Plane specific, and is described in `Flightplan files - v11 .fms file format <https://developer.x-plane.com/article/flightplan-files-v11-fms-file-format/>`__.
  
  >>> print(xp.saveFMSFlightPlan(0))
  I
  1100 Version
  CYCLE 2112 
  ADEP EDDS 
  DEPRWY RW25 
  SID ETAS4B 
  ADES EDDF 
  DESRWY RW25L 
  STAR SPES3B 
  APP I25L 
  APPTRANS CHA 
  NUMENR 6 
  1 EDDS ADEP 1272.000000 48.689877 9.221964 
  11 XINLA T163 0.000000 49.283646 9.141608 
  11 SUKON T163 0.000000 49.659721 9.195556 
  11 SUPIX T163 0.000000 49.727779 9.305278 
  11 SPESA T163 0.000000 49.862240 9.348325 
  1 EDDF ADES 354.000000 50.033306 8.570456  
  
  You are responsible for saving it to a file, if that's desired.
  
  `Official SDK <https://developer.x-plane.com/sdk/XPLMNavigation/#XPLMSaveFMSFlightPlan>`__ :index:`XPLMSaveFMSFlightPlan`

.. py:function:: countFMSFlightPlanEntries(flightPlan)

  :param XPLMNavFlightPlan flightPlan: selected flight plan
  :return int: number of entries in flight plan.

  Also returns 0 if given flightPlan is not supported. (There is no way to determine if a
  particular flight plan is supported by the aircraft except, I suppose, by setting a value
  and attempting to read it back.)
  
  Note that load/save use a simple
  integer for Pilot/Copilot flight plan. Most of the remain API use :ref:`XPLMNavFlightPlan` which
  can manipulate primary, approach, and temporary flight plans.::

    >>> xp.countFMSFlightPlanEntries(xp.Fpl_Pilot_Primary)
    22

  `Official SDK <https://developer.x-plane.com/sdk/XPLMNavigation/#XPLMCountFMSFlightPlanEntries>`__ :index:`XPLMCountFMSFlightPlanEntries`

.. py:function:: getFMSFlightPlanEntryInfo(flightPlan, index)

  Returns information about a single entry in the given flight plan.

  :param XPLMNavFlightPlan flightPlan: selected flight plan
  :param int index: existing 0-based index to retrieve
  :return FMSEntryInfo: Instance of :ref:`FMSEntryInfo`

  >>> for i in range(xp.countFMSFlightPlanEntries(xp.Fpl_Pilot_Primary)):
  ...    print(str(xp.getFMSFlightPlanEntryInfo(xp.Fpl_Pilot_Primary, i)))
  ...
  Airport:   [16803968]   EDDS, (48.690, 9.222) @1272'
  LatLon:                 RW25, (48.694, 9.244) @1180'
  Unknown:              (1700), (48.690, 9.220) @1699'
  Fix:       [33622837]  DS050, (48.671, 9.122) @0'
  Fix:       [33622827]  DS040, (48.690, 9.067) @0'
  Fix:       [33622830]  DS043, (48.801, 9.010) @0'
  Fix:       [33613119]  KOVAN, (48.882, 9.084) @0'
  Fix:       [33611008]  ETASA, (49.191, 9.128) @0'
  Fix:       [33620051]  XINLA, (49.284, 9.142) @0'
  Fix:       [33617115]  SUKON, (49.660, 9.196) @0'
  Fix:       [33617153]  SUPIX, (49.728, 9.305) @0'
  Fix:       [33617059]  SPESA, (49.862, 9.348) @0'
  VOR:       [    5353]    CHA, (49.921, 9.040) @0'
  Fix:       [33621703]  D338K, (50.095, 8.942) @0'
  Fix:       [33621910]  LEDKI, (50.104, 8.856) @0'
  Fix:       [33621902]  FF25L, (50.081, 8.759) @0'
  LatLon:                RW25L, (50.040, 8.587) @411'
  Fix:       [33621697]  D247E, (50.021, 8.505) @0'
  Fix:       [33621696]  D241H, (49.993, 8.453) @0'
  Unknown:              (5000), (49.925, 8.232) @4999'
  VOR:       [    5353]    CHA, (49.921, 9.040) @0'
  Airport:   [16787675]   EDDF, (50.033, 8.570) @354'
 
  Note that loading the same flight plan into a different aircraft may
  results in a different set of entries. The above entries were for the Airbus,
  the following entries are from the Cessan 172 G1000

  >>> for i in range(xp.countFMSFlightPlanEntries(xp.Fpl_Pilot_Primary)):
  ...    print(str(xp.getFMSFlightPlanEntryInfo(xp.Fpl_Pilot_Primary, i)))
  ...
  Airport:   [16803968]   EDDS, (48.690, 9.222) @1272'
  LatLon:                 RW25, (48.694, 9.244) @1180'
  Unknown:              (1700), (48.690, 9.220) @1699'
  Fix:       [33622837]  DS050, (48.671, 9.122) @0'
  Fix:       [33622827]  DS040, (48.690, 9.067) @0'
  Fix:       [33622830]  DS043, (48.801, 9.010) @0'
  Fix:       [33613119]  KOVAN, (48.882, 9.084) @0'
  Fix:       [33611008]  ETASA, (49.191, 9.128) @0'
  Fix:       [33620051]  XINLA, (49.284, 9.142) @0'
  Fix:       [33617115]  SUKON, (49.660, 9.196) @0'
  Fix:       [33617153]  SUPIX, (49.728, 9.305) @0'
  Fix:       [33617059]  SPESA, (49.862, 9.348) @0'
  VOR:       [    5353]    CHA, (49.921, 9.040) @0'
  Fix:       [33621832]  DF606, (49.945, 8.912) @0'
  Fix:       [33621834]  DF610, (50.029, 8.917) @0'
  Fix:       [33621835]  DF611, (50.043, 8.973) @0'
  Fix:       [33621836]  DF612, (50.066, 9.071) @0'
  Fix:       [33621837]  DF613, (50.088, 9.168) @0'
  Fix:       [33621838]  DF614, (50.111, 9.266) @0'
  Fix:       [33621839]  DF615, (50.134, 9.363) @0'
  Fix:       [33621840]  DF616, (50.156, 9.461) @0'
  Unknown:              VECTOR, (52.515, 26.154) @0'
  Airport:   [16787675]   EDDF, (50.033, 8.570) @354'

  Note the use of VECTOR here, which is a lat/lon a few hundred miles away
  *in the direction* one is suppose to fly. Not something to actually
  over-fly.

  And... because G1000 has a separate Approach plan, don't forget:

  >>> for i in range(xp.countFMSFlightPlanEntries(xp.Fpl_Pilot_Approach)):
  ...    print(str(xp.getFMSFlightPlanEntryInfo(xp.Fpl_Pilot_Approach, i)))
  ...
  VOR:       [    5353]    CHA, (49.921, 9.040) @0'
  Fix:       [33621703]  D338K, (50.095, 8.942) @0'
  Fix:       [33621910]  LEDKI, (50.104, 8.856) @0'
  Fix:       [33621902]  FF25L, (50.081, 8.759) @0'
  LatLon:                RW25L, (50.040, 8.587) @411'
  Fix:       [33621697]  D247E, (50.021, 8.505) @0'
  Fix:       [33621696]  D241H, (49.993, 8.453) @0'
  Unknown:              (5000), (49.925, 8.232) @4999'
  VOR:       [    5353]    CHA, (49.921, 9.040) @0'

  `Official SDK <https://developer.x-plane.com/sdk/XPLMNavigation/#XPLMGetFMSFlightPlanEntryInfo>`__ :index:`XPLMGetFMSFlightPlanEntryInfo`

.. py:function:: setFMSFlightPlanEntryInfo(flightPlan, index, navRef, altitude=0)
                 setFMSFlightPlanEntryLatLon(flightPlan, index, lat, lon, altitude=0)
                 setFMSFlightPlanEntryLatLonWithId(flightPlan, index, lat, lon, altitude=0, ID=None)

  Sets entry in FMS at the given index.

  :param XPLMNavFlightPlan flightPlan: selected flight plan
  :param int index: 0-based index to set
  :param XPLMNavRef navRef: :ref:`XPLMNavRef` integer from :py:func:`findNavAid`
  :param float lat:
  :param float lon: Latitude / Longitude. To be used when navRef is not appropriate   
  :param int altitude: Altitude of fix in feet.                 
  :param str ID: Optional string used (with LatLon) to be displayed with entry
  :return: None

  These three functions all do the same thing, but using slightly different data as input. If a *navRef* is
  provided, its latitude, longitude and display name will used. If no navRef is provided, latitude and longitude
  must be provided.

  The *index* you provide must be for either an existing entry, or one more than the last entry, otherwise
  it is ignored. That is, if you only have three entries, setting entry index #10 will do nothing.

  To match our example EDDS-EDDF flight plan, one could use the SDK to create it as::

  >>> xp.setFMSFlightPlanEntryInfo(0, 0, 16803968, 1271)
  >>> xp.setFMSFlightPlanEntryLatLonWithId(0, 1, 48.694, 9.244, 1180, "RW25")
  >>> xp.setFMSFlightPlanEntryLatLonWithId(0, 2, 48.690, 9.220, 1699, "(1700)")
  >>> xp.setFMSFlightPlanEntryInfo(0, 3, 33622837, 0)
  >>> xp.setFMSFlightPlanEntryInfo(0, 4, 33622827, 0)
  >>> xp.setFMSFlightPlanEntryInfo(0, 5, 33622830, 0)
  >>> xp.setFMSFlightPlanEntryInfo(0, 6, 33613119, 19999)
  >>> xp.setFMSFlightPlanEntryInfo(0, 7, 33611008, 19035)
  >>> xp.setFMSFlightPlanEntryInfo(0, 8, 33620051, 0)
  >>> xp.setFMSFlightPlanEntryInfo(0, 9, 33617115, 0)
  >>> xp.setFMSFlightPlanEntryInfo(0, 10, 33617153, 0)
  >>> xp.setFMSFlightPlanEntryInfo(0, 11, 33617059, 0)
  >>> xp.setFMSFlightPlanEntryInfo(0, 12, 5353, 0)
  >>> xp.setFMSFlightPlanEntryLatLonWithId(0, 13, 50.095, 8.942, 0, "D338K")
  >>> xp.setFMSFlightPlanEntryInfo(0, 14, 33621910, 0)
  >>> xp.setFMSFlightPlanEntryLatLonWithId(0, 15, 50.081, 8.759, 0, "FF25L")
  >>> xp.setFMSFlightPlanEntryLatLonWithId(0, 16, 50.040, 8.587, 411, "RW25L")
  >>> xp.setFMSFlightPlanEntryLatLonWithId(0, 17, 50.021, 8.505, 0, "D247E")
  >>> xp.setFMSFlightPlanEntryLatLonWithId(0, 18, 49.993, 8.453, 0, "D241H")
  >>> xp.setFMSFlightPlanEntryLatLonWithId(0, 19, 49.925, 8.232, 4999, "5000")
  >>> xp.setFMSFlightPlanEntryInfo(0, 20, 5353, 0)
  >>> xp.setFMSFlightPlanEntryInfo(0, 21, 16787675, 353)

  The resulting formatted flight plan looks like:

  .. code:: none

   I
   1100 Version
   CYCLE 2112
   DEP EDDS
   DES EDDF
   NUMENR 22
   1 EDDS DRCT 1272.000000 48.689877 9.221964
   28 RW25 DRCT 0.000000 48.694000 9.244000
   28 (1700 DRCT 0.000000 48.689999 9.220000
   11 DS050 DRCT 0.000000 48.670815 9.122375
   11 DS040 DRCT 0.000000 48.689812 9.067369
   11 DS043 DRCT 0.000000 48.800713 9.009992
   11 KOVAN DRCT 0.000000 48.882385 9.084291
   11 ETASA DRCT 0.000000 49.190796 9.128403
   11 XINLA DRCT 0.000000 49.283646 9.141608
   11 SUKON DRCT 0.000000 49.659721 9.195556
   11 SUPIX DRCT 0.000000 49.727779 9.305278
   11 SPESA DRCT 0.000000 49.862240 9.348325
   3 CHA DRCT 0.000000 49.921104 9.039817
   28 D338K DRCT 0.000000 50.095001 8.942000
   11 LEDKI DRCT 0.000000 50.104099 8.855891
   28 FF25L DRCT 0.000000 50.081001 8.759000
   28 RW25L DRCT 0.000000 50.040001 8.587000
   28 D247E DRCT 0.000000 50.021000 8.505000
   28 D241H DRCT 0.000000 49.993000 8.453000
   28 5000 DRCT 0.000000 49.924999 8.232000
   3 CHA DRCT 0.000000 49.921104 9.039817
   1 EDDF DRCT 354.000000 50.033306 8.570456
  
  You'll note this formatted flight plan is different from the formatted flight plan created
  by *manually* entering the data to the MCDU (See edds_eddf.fms_ above). You'll lose explicit
  departure and destination block information with named SID and STAR. At present (XP 12.1) there
  is no way to provide such information through the SDK. Nor can you indicate that your
  route should use a particular airway, as you can by using MCDU.
  Fortunately waypoints and altitudes are all the same.
  
  .. note:: Regarding altitude: Due to internal conversion from integer feet to floating point to meters
            and back again, the altitude you *set* may be slightly different from the altitude you *observe*
            on the FMS device and/or the altitude you *get* programmatically. The different is likely to be
            only a foot or so. 
     
  `Official SDK <https://developer.x-plane.com/sdk/XPLMNavigation/#XPLMSetFMSFlightPlanEntryInfo>`__ :index:`XPLMSetFMSFlightPlanEntryInfo`

  `Official SDK <https://developer.x-plane.com/sdk/XPLMNavigation/#XPLMSetFMSFlightPlanEntryLatLon>`__ :index:`XPLMSetFMSFlightPlanEntryLatLon`

  `Official SDK <https://developer.x-plane.com/sdk/XPLMNavigation/#XPLMSetFMSFlightPlanEntryLatLonWithId>`__ :index:`XPLMSetFMSFlightPlanEntryLatLonWithId`

.. py:function:: clearFMSFlightPlanEntry(flightPlan, index)

  Removes indicated flight plan entry

  :param XPLMNavFlightPlan flightPlan: selected flight plan
  :param int index: 0-based index entry to be removed
  :return: None

  The entry will be removed and the result flight plan with either be shortened,
  or a discontinuity may result. Attempting to clear an index which does not exist is ignored.

  Continuing with our flight plan example, if we delete the single entry in position 15:

  >>> xp.clearFMSFlightPlanEntry(0, 15)

  We'll still have 22 entries, but the entry #15 will now be a discontinuity.

  .. code-block:: none

    ┏━━━━━┳━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┳━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
    ┃Entry┃       Before delete #15 "FF25L"                  ┃     After delete note DISCON at #15               ┃
    ┠━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┃
    ┃ [0] ┃Airport: [16803968]  EDDS, (48.690, 9.222) @1271' ┃  Airport: [16803968]  EDDS, (48.690, 9.222) @1271'┃
    ┃ [1] ┃LatLon:              RW25, (48.694, 9.244) @1180' ┃  LatLon:              RW25, (48.694, 9.244) @1180'┃
    ┃ [2] ┃Unknown:           (1700), (48.690, 9.220) @1699' ┃  Unknown:           (1700), (0.000, 0.000) @0'    ┃
    ┃ [3] ┃Fix:     [33622837] DS050, (48.671, 9.122) @0'    ┃  Fix:     [33622837] DS050, (48.671, 9.122) @0'   ┃
    ┃ [4] ┃Fix:     [33622827] DS040, (48.690, 9.067) @0'    ┃  Fix:     [33622827] DS040, (48.690, 9.067) @0'   ┃
    ┃ [5] ┃Fix:     [33622830] DS043, (48.801, 9.010) @0'    ┃  Fix:     [33622830] DS043, (48.801, 9.010) @0'   ┃
    ┃ [6] ┃Fix:     [33613119] KOVAN, (48.882, 9.084) @19999'┃  Fix:     [33613119] KOVAN, (48.882, 9.084) @0'   ┃
    ┃ [7] ┃Fix:     [33611008] ETASA, (49.191, 9.128) @19035'┃  Fix:     [33611008] ETASA, (49.191, 9.128) @0'   ┃
    ┃ [8] ┃Fix:     [33620051] XINLA, (49.284, 9.142) @0'    ┃  Fix:     [33620051] XINLA, (49.284, 9.142) @0'   ┃
    ┃ [9] ┃Fix:     [33617115] SUKON, (49.660, 9.196) @0'    ┃  Fix:     [33617115] SUKON, (49.660, 9.196) @0'   ┃
    ┃[10] ┃Fix:     [33617153] SUPIX, (49.728, 9.305) @0'    ┃  Fix:     [33617153] SUPIX, (49.728, 9.305) @0'   ┃
    ┃[11] ┃Fix:     [33617059] SPESA, (49.862, 9.348) @0'    ┃  Fix:     [33617059] SPESA, (49.862, 9.348) @0'   ┃
    ┃[12] ┃VOR:     [    5353]   CHA, (49.921, 9.040) @0'    ┃  VOR:     [    5353]   CHA, (49.921, 9.040) @0'   ┃
    ┃[13] ┃Fix:     [33621703] D338K, (50.095, 8.942) @0'    ┃  Fix:     [33621703] D338K, (50.095, 8.942) @0'   ┃
    ┃[14] ┃Fix:     [33621910] LEDKI, (50.104, 8.856) @0'    ┃  Fix:     [33621910] LEDKI, (50.104, 8.856) @0'   ┃
    ┃[15] ┃Fix:     [33621902] FF25L, (50.081, 8.759) @0'    ┃  Unknown:           DISCON, (50.072, 8.721) @0'   ┃
    ┃[16] ┃LatLon:             RW25L, (50.040, 8.587) @411'  ┃  LatLon:             RW25L, (50.040, 8.587) @411' ┃
    ┃[17] ┃Fix:     [33621697] D247E, (50.021, 8.505) @0'    ┃  Fix:     [33621697] D247E, (50.021, 8.505) @0'   ┃
    ┃[18] ┃Fix:     [33621696] D241H, (49.993, 8.453) @0'    ┃  Fix:     [33621696] D241H, (49.993, 8.453) @0'   ┃
    ┃[19] ┃Unknown:           (5000), (49.925, 8.232) @4999' ┃  Unknown:           (5000), (49.925, 8.232) @4999'┃
    ┃[20] ┃VOR:     [    5353]   CHA, (49.921, 9.040) @0'    ┃  VOR:     [    5353]   CHA, (49.921, 9.040) @0'   ┃
    ┃[21] ┃Airport: [16787675]  EDDF, (50.033, 8.570) @353'  ┃  Airport: [16787675]  EDDF, (50.033, 8.570) @353' ┃
    ┗━━━━━┻━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┻━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛


  `Official SDK <https://developer.x-plane.com/sdk/XPLMNavigation/#XPLMClearFMSFlightPlanEntry>`__ :index:`XPLMClearFMSFlightPlanEntry`


.. py:function:: getDestinationFMSFlightPlanEntry(flightPlan)

  :param XPLMNavFlightPlan flightPlan: selected flight plan
  :return int: Return 0-based index number of the entry the FMS is flying to.

  If the flightPlan is not valid for the aircraft, 0 is returned. There is no way to distinguish
  between a valid and invalid response.

  >>> xp.getDestinationFMSFlightPlanEntry(xp.Fpl_Pilot_Primary)
  0

  `Official SDK <https://developer.x-plane.com/sdk/XPLMNavigation/#XPLMGetDestinationFMSFlightPlanEntry>`__ :index:`XPLMGetDestinationFMSFlightPlanEntry`

.. py:function:: setDestinationFMSFlightPlanEntry(flightPlan, index)

  :param XPLMNavFlightPlan flightPlan: selected flight plan
  :param int index: existing 0-based index to become new destination
  :return: None

  The flight track is from the location immediately before the index to the index entry.
  (Compare with :py:func:`setDirectToFMSFlightPlanEntry`.) If the index is not valid, the command is ignored.

  Changing destination from KDEN to WITNE::

    >>> xp.setDestinationFMSFlightPlanEntry(xp.Nav_Pilot_Primary, 1)

  .. image:: /images/g530destination.png

  `Official SDK <https://developer.x-plane.com/sdk/XPLMNavigation/#XPLMSetDestinationFMSFlightPlanEntry>`__ :index:`XPLMSetDestinationFMSFlightPlanEntry`

.. py:function:: setDirectToFMSFlightPlanEntry(flightPlan, index)

  :param XPLMNavFlightPlan flightPlan: selected flight plan
  :param int index: existing 0-based index to become new direct-to destination
  :return: Return

  The flight track is changed to be the *current aircraft position* to the indicated destination, ignoring
  flight plan entries before the index value.
  
  Compare :py:func:`setDestinationFMSFlightPlanEntry` (left) versus :py:func:`setDirectToFMSFlightPlanEntry` (right)
  for a plan where the aircraft is currently at KDEN and we set the entry to index #3 GLD:

  .. image:: /images/directToG1000.png

  `Official SDK <https://developer.x-plane.com/sdk/XPLMNavigation/#XPLMSetDirectToFMSFlightPlanEntry>`__ :index:`XPLMSetDirectToFMSFlightPlanEntry`

.. py:function:: getDisplayedFMSFlightPlanEntry(flightPlan)

  :param XPLMNavFlightPlan flightPlan: selected flight plan
  :return int: Return 0-based index number of the currently displayed flight plan entry

  Some aircraft are able to step through the flight plan. For example, the Airbus can be set to "PLAN" and
  each leg displayed for review.

  This function retrieves the currently displayed entry index.
  
  `Official SDK <https://developer.x-plane.com/sdk/XPLMNavigation/#XPLMGetDisplayedFMSFlightPlanEntry>`__ :index:`XPLMGetDisplayedFMSFlightPlanEntry`

.. py:function:: setDisplayedFMSFlightPlanEntry(flightPlan, index)

  :param XPLMNavFlightPlan flightPlan: selected flight plan
  :param int index: existing 0-based index to be displayed                                      
  :return: None

  Change the entry being displayed on ND (Navigation Display), where applicable.

  For example, on the Airbus:

  .. image:: /images/airbusPlan.jpeg

  To cycle through each leg::

    >>> xp.setDisplayedFMSFlightPlanEntry(0, 1)             
    >>> xp.setDisplayedFMSFlightPlanEntry(0, 2)             
    >>> xp.setDisplayedFMSFlightPlanEntry(0, 3)             
       
  Note that, for the Airbus, if the MCDU is set on "F-PLN" the display will not update. Select any
  other MCDU button and the views will update.

  Note also that G1000 MFD can be made to view each leg of the flight plan by manually moving
  the on-screen cursor, but does not respond to these Get/SetDisplay functions.

  
  `Official SDK <https://developer.x-plane.com/sdk/XPLMNavigation/#XPLMSetDisplayedFMSFlightPlanEntry>`__ :index:`XPLMSetDisplayedFMSFlightPlanEntry`






Flight Management Computer: Pre-XP12.1 Style
--------------------------------------------
X-Plane 12.1 introduced a new way of interacting with Flight Management computers, allowing
you to select *which* flight plan you want to work with. See :ref:`fms_flightplan`. You should
be using *that* method for new development.


Some aircraft have a Flight Management System which responds to these
commands. Some do not. *sigh*. The Laminar G530 and G1000 work.

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

Functions
*********

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

 
.. py:function::  setFMSEntryInfo(index, navRef, altitude=0)

 This routine changes an entry at *index* in the FMS to have the
 destination navaid specified by *navRef* (as returned by :py:func:`findNavAid`)
 at *altitude* (in feet) specified.
 Use this only for airports, fixes,
 and radio-beacon navaids.  Currently for radio beacons, the FMS can only
 support VORs, NDBs, and TACANS.

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

GPS Receiver
------------

.. py:function::  getGPSDestinationType()

 Return the :ref:`XPLMNavType` of the current GPS destination.

 :return: XPLMNavType

 >>> xp.getGPSDestinationType()
 512
 
 `Official SDK <https://developer.x-plane.com/sdk/XPLMNavigation/#XPLMGetGPSDestinationType>`__ :index:`XPLMGetGPSDestinationType`


.. py:function:: getGPSDestination()

 Return the :ref:`XPLMNavRef` of current GPS destination.

 : return: XPLMNavRef

 >>> xp.getGPSDestination()
 33706498
 >>> print(xp.getNavAidInfo(xp.getGPSDestination()))
 LUCOS (LUCOS) Fix (41.638, -70.768) ---

 `Official SDK <https://developer.x-plane.com/sdk/XPLMNavigation/#XPLMGetGPSDestination>`__ :index:`XPLMGetGPSDestination` 

Constants
---------

.. _XPLMNavFlightPlan:

:index:`XPLMNavFlightPlan`
**************************
These enumerations define the type of flight plan used with
:ref:`fms_flightplan` routines. Not all are available on all
aircraft. For convenience, I've included at least one aircraft
which has the named flight plan. This is not a complete list of
aircraft.

 .. table::
   :align: left
     
   ================================== === =====================================
   .. py:data:: Fpl_Pilot_Primary     =0  Cessna 172 G1000, GNS530, Airbus MCDU
   .. py:data:: Fpl_CoPilot_Primary   =1  Cessna 172 GNS430, Airbus MCDU
   .. py:data:: Fpl_Pilot_Approach    =2  GNS530, G1000
   .. py:data:: Fpl_CoPilot_Approach  =3  GNS430
   .. py:data:: Fpl_Pilot_Temporary   =4  Airbus MCDU
   .. py:data:: Fpl_CoPilot_Temporary =5  Airbus MCDU
   ================================== === =====================================


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
   .. py:data:: Nav_TACAN        =4096
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

 .. py:data:: NAV_NOT_FOUND
  :value: -1

 NAV_NOT_FOUND is returned by functions that return an :ref:`XPLMNavRef` when
 the iterator must be invalid.

`Official SDK <https://developer.x-plane.com/sdk/XPLMNavigation/#XPLMNavRef>`__ :index:`XPLMNavRef`

.. _FMSEntryInfo:

:index:`FMSEntryInfo`
*********************       

Data structure returned by :py:func:`getFMSFlightPlanEntryInfo` and py:func:`getFMSEntryInfo`.

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

.. _NavAidInfo:

:index:`NavAidInfo`
*******************

Data structure returned by :py:func:`getNavAidInfo`.

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


