XPLMWeather
===========
.. py:module:: XPLMWeather
.. py:currentmodule:: xp

To use::

  import xp

This API provides access to the X-Plane enhanced weather system and requires at least X-Plane 12.04.
More detailed weather, and the ability to set weather is available with 12.3.0.

All of these functions are relatively expensive, and *should not* be used per-frame.

Theory of Operation
-------------------

* **Get Weather**

  + :func:`getMETARForAirport`: retrieve most recent METAR for given airport. Not necessarily an accurate
    representation of the current *simulated* weather. To get current weather at airport use :func:`getWeatherAtLocation` with
    the proper latitude, longitude.

  + :func:`getWeatherAtLocation`: retrieve weather details at particular latitude, longitude. This
    is current simulated weather. The returned datastructure depends on which version of X-Plane you
    are running, where version 12.3.0 adds more data elements. You can query ``xp.XPLMWeatherInfo_t`` to
    get a full description.
  

* **Set Weather**

  Setting weather will *not change* the UI "Weather Settings", weather set via plugin "adds" to the set of information
  used to calculate simulated weather. One way to visualize many of these changes is to enable to X-Plane Weather Map (Developer menu).
  Then select "plugin METARS" Layer. You'll see weather that has been set by (all) plugins.

  For example, in the following image, you can see weather set specifically at Cambridge airport (labeled EGSC) using :func:`setWeatherAtAirport`, and
  weather set to nearby locations (labeled _PLG) using :func:`setWeatherAtLocation`.
  Latitude, longitude ar displayed in white along the edges of the image. Note the "Age" of the report at the bottom of each.
  
  .. image:: /images/weather_map_plugin.png
  
  
  + :func:`setWeatherAtLocation`: sets (or perhaps "injects" is better...) weather at a particular latitude, longitude.
    This data is further combined with otherwise existing data to result in new weather. Your weather update
    merely *influences* the current weather simulation: the more data you inject (say over a particular region) the
    greater the effect.

  + :func:`setWeatherAtAirport`: sets (or perhaps "injects" is better...) weather at a particular airport.
    This can be interpreted as replacing a previously downloaded METAR.
    This data is further combined with otherwise existing data to result in new weather. Your weather update
    merely *influences* the current weather simulation.

  + :func:`eraseWeatherAtLocation`: removes all weather effects your plugin has injected for the given location.
    If other plugins have changed the weather, those plugin effects are still valid.
    
  + :func:`eraseWeatherAtAirport`: removes all weather effects your plugin has injected for the given airport, similar
    to :func:`eraseWeatherAtLocation`.

  + :func:`beginWeatherUpdate`, :func:`endWeatherUpdate`: To facilitate making multiple changes to the weather, you
    should bracket your changes between "begin" and "end". This function pair also allows you to indicate your
    changes are incremental, and if your changes should take effect "immediately" (as might be useful on startup) or
    transitioned over the next few minutes. For programming convenience, we've included a context manager :func:`weatherUpdateContext`
    to avoid mis-handling an "end".
    

Get Weather
-----------

.. py:function:: getMETARForAirport(airport_id)

  Get the last known METAR report for an airport by ICAO code. Note that the
  actual weather at that airport may have evolved significantly since the last
  downloaded METAR. This call is not intended to be used per-frame.

  >>> xp.getMETARForAirport('KJFK')
  'KJFK 111451Z 01012KT 7SM -RA OVC011 03/02 A2974 RMK AO2 SNB1356E02 SLP070 P0005 60015 T00280017 51024'
  >>> xp.getMETARForAirport('JFK')
  ''
  >>> xp.getMETARForAirport('KNYC')
  ''

  Note that valid METAR codes which *are not airports* such as Central Park NYC (KNYC) do not
  return values.

  Note also that "Download Real Weather" must be enabled in order for X-Plane to have
  downloaded *any* METARs. If real weather was not enabled, the query will return an empty
  string with no indication that real weather is disabled.:

  >>> xp.getMETARForAirport('KJFK')
  ''

 `Official SDK <https://developer.x-plane.com/sdk/XPLMWeather/#XPLMGetMETARForAirport>`__ :index:`XPLMGetMETARForAirport`

.. py:function:: getWeatherAtLocation(latitude, longitude, altitude_m)

  Get the current weather conditions at a given location. Note that this does not work
  world-wide, only within the surrounding region.

  This call is not intended to be used per-frame (use datarefs if at all possible instead).
  
  Returns ``None`` on error.

  .. note::

     The above information is from Laminar's documentation, but it needs to be *clarified*. (`XPD-14674 <https://developer.x-plane.com/x-plane-bug-database/?issue=XPD-14674>`_)

     * It appears to *execute* world-wide, with latitudes +/- 90 degrees and longitudes +/- 180 degrees.
       **However**, weather outside the current region will be less accurate, due to math errors
       and METAR information. (There is no way of knowing the extent of the *current region*, so
       the understanding should be "nearby weather is more accurate that far-away weather". Generally,
       within 50nm of current location has good accuracy.)
       Using values with illegal latitude/longitude range will result in a ``None`` return value.

     * The ``XPLMWeatherInfo_t.detail_found`` attribute, on a successful return, is the *actual*
       return value from X-Plane's XPLMGetWeatherAtLocation() call. This, I'm told,
       indicates "if detail weather is found", where *detail weather* refers to the
       existence of a METAR within about 50 miles of the queried location. It is **not**
       a success/failure return value of the actual XPLMWeatherInfo_t object. (Nor is it an
       indication of "weather in region" vs. "weather out of region".)

       What to do with this ``detail_found`` value? Beats me. It won't tell you which METAR
       is nearby, or which of multiple nearby METARs are used perhaps to alter the world-wide
       GRIB weather data. Bug Report has been filed requesting clarification.

     Because ``detail_found`` does not indicate success or failure, XPPython3 uses
     a flag to attempt to detect if data is valid. (We set an internal data structure
     temperature to a bogus number and if post-query it is still bad, we assume
     data was not successfully obtained, and return ``None``.)

     Please, if any of this makes sense to you let me know!

  On success, returns a XPLMWeatherInfo_t object:

  >>> help(xp.XPLMWeatherInfo_t)
  class XPLMWeatherInfo_t(builtin.object)
  |  ----------------------------------
  |  Data descriptors defined here:
  |  detail_found:     actual return value from X-Plane XPLMGetWeatherAtLocation()
  |  temperature_alt:  temperature at altitude (Celsius)
  |  dewpoint_alt:     dewpoint at altitude (Celsius)
  |  pressure_alt:     pressure at altitude (Pascals)
  |  precip_rate_alt:  precipitation ratio at altitude
  |  wind_dir_alt:     wind direction at altitude (True, presumably)
  |  wind_spd_alt:     wind speed at altitude (meters/second)
  |  turbulence_alt:   turbulence ratio at altitude (units?)
  |  wave_dir:         wave direction (waves moving from...)
  |  wave_length:      wave length (meters)
  |  wave_speed:       wave speed (meters/second)
  |  visibility:       base visibility at 0 altitude (meters)
  |  precip_rate:      base precipitation ratio at 0 altitude
  |  thermal_climb:    climb rate due to thermals (meters/second)
  |  pressure_sl:      pressure at 0 altitude (Pascals)
  |  wind_layers:      List of XPLMWeatherInfoWinds_t objects
  |  cloud_layers:     List of XPLMWeatherInfoClouds_t objects

  Which include a list of objects for `wind_layer` and `cloud_layers`:

  >>> help(xp.XPLMWeatherInfoWinds_t)
  class XPLMWeatherInfoWinds_t(builtin.object)
  |  ------------------------------------
  |  Data descriptors defined here:
  |  alt_msl:        Altitude MSL (meters)
  |  speed:          Speed (meters/second)
  |  direction:      Direction (degrees true)
  |  gust_speed:     Gust speed (meters/second)
  |  shear:          Shear arc i.e., 50% of this arc in either direction from base (degrees)
  |  turbulence:     Turbulence ratio

  >>> help(xp.XPLMWeatherInfoClouds_t)
  class XPLMWeatherInfoClouds_t(builtin.object)
  |  ------------------------------------
  |  Data descriptors defined here:
  |  cloud_type:     Cloud type (float enum)
  |  coverage:       Coverage ratio
  |  alt_top:        Cloud top altitude MSL (meters)
  |  alt_base:       Cloud base altitude MSL (meters)

  For example:

  >>> info = xp.getWeatherAtLocation(34, -117, 2000)
  >>> print(info)
  <XPLMWeatherInfo_ object> 7.9/-10.1 252@3 QNH1009
  >>> info.temperature_alt
  7.93484
  >>> info.dewpoint_alt
  -10.11784
  >>> info.wind_dir_alt
  252.4056
  >>> info.wind_speed_alt
  1.4252
  >>> len(info.wind_layers)
  13
  >>> [x.alt_msl for x in info.wind_layers]
  [0.0, 540.1, 988.5, 1948.3, 3010.8, 4206.5, 5572.0, 7182.3, 9160.1,
   10362.9, 11887.2, 13594.1, 16179.4]
  >>> print(info.wind_layers[0])
  <XPLMWeatherInfoWinds_t object> 175@9 at 0.0m MSL
  >>> print(info.wind_layers[1])
  <XPLMWeatherInfoWinds_t object> 176@10 at 540.1m MSL
  >>> print(info.cloud_layers[1])
  <XPLMWeatherInfoClouds_t object> 2.0 (50%) from 4442.0m to 5759.5m

  If ``coverage`` is zero, there are no clouds at that layer regardless of the
  other parameters. Just skip that layer and look at the next.

  Though not documented, I assume "cloud type" enumeration is the same as the ``sim/weather/aircraft/cloud_type``
  and ``sim/weather/region/cloud_type`` datarefs:

  | 0 = Cirrus
  | 1 = Stratus
  | 2 = Cumulus
  | 3 = Cumulo-nimbus

  `Official SDK <https://developer.x-plane.com/sdk/XPLMWeather/#XPLMGetWeatherAtLocation>`__ :index:`XPLMGetWeatherAtLocation`
  
Set Weather
-----------

.. note:: "Setting Weather" is available only with X-Plane 12.3.0 and above, which is currently in beta. There
          remain some bugs in the X-Plane implementation, so the following API and/or explanations may
          change.

   
The following functions are *only* available with X-Plane 12.3.0+. Calling them on earlier
versions of X-Plane will log an error.

.. py:function:: setWeatherAtLocation(latitude, longitude, altitude_m, info)
                 
  :param float latitude:
  :param float longitude: floating point latitude and longitude
  :param float altitude_m: altitude in meters, where the weather is to be changed
  :param XPLMWeatherInfo_t info: :data:`XPLMWeatherInfo_t` data structure containing requested change. Note not all element members are applicable for "set".
                                  
  `Official SDK <https://developer.x-plane.com/sdk/XPLMWeather/#XPLMSetWeatherAtLocation>`__ :index:`XPLMSetWeatherAtLocation`

.. py:function:: setWeatherAtAirport(airport_id, info)
                 
  :param str airport_id: airport Id
  :param XPLMWeatherInfo_t info: :data:`XPLMWeatherInfo_t` data structure contining requested change. Note not all element members are applicable for "set".

  `Official SDK <https://developer.x-plane.com/sdk/XPLMWeather/#XPLMSetWeatherAtAirport>`__ :index:`XPLMSetWeatherAtAirport`

.. py:function:: eraseWeatherAtLocation(latitude, longitude)

  :param float latitude:
  :param float longitude: floating point latitude and longitude

  Erase *all* weather information provided by this plugin a the given latitude, longitude. Note
  if other plugins have also provide weather at this location, those changes will still be valid.
  It is not harmful to call :func:`eraseWeatherAtLocation` where you have not yet provided weather.
   
  `Official SDK <https://developer.x-plane.com/sdk/XPLMWeather/#XPLMEraseWeatherAtLocation>`__ :index:`XPLMEraseWeatherAtLocation`

.. py:function:: eraseWeatherAtAirport(airport)

  :param str airport_id: airport Id

  Erase *all* weather information provided by this plugin a the given airport. Note
  if other plugins have also provide weather at this airport, those changes will still be valid.
  It is not harmful to call :func:`eraseWeatherAtAirport` where you have not yet provided weather.
   
  `Official SDK <https://developer.x-plane.com/sdk/XPLMWeather/#XPLMEraseWeatherAtAirport>`__ :index:`XPLMEraseWeatherAtAirport`

.. py:function:: beginWeatherUpdate()

  Inform the simulator that you are starting a batch update of weather information.
  **You must call** :func:`endWeatherUpdate` **prior to exiting your callback**. Failure to
  do so will result in error and performance issues. For this reason, we suggest you use :func:`weatherUpdateContext` instead.

  Because this function must be used at the same time as :func:`endWeatherUpdate`, you cannot type this function directly
  into the python debugger (though you can, if you place it within a function definition.) Use :func:`weatherUpdateContext`
  if you want to experiment within the debugger.

  This call is not intended to be used per-frame.

  ::

     >>> def changeWeather():
     ...    xp.beginWeatherUpdate()
     ...    info = xp.getWeatherAtLocation(35, -172, 100)
     ...    info.thermal_climb += .5
     ...    xp.setWeatherAtLocation(35, -172, 100, info)
     ...    xp.endWeatherUpdate()
     ...
     >>> changeWeather()

  `Official SDK <https://developer.x-plane.com/sdk/XPLMWeather/#XPLMBeginWeatherUpdate>`__ :index:`XPLMBeginWeatherUpdate`

.. py:function:: endWeatherUpdate(isIncremental=1, updateImmediately=0)

  :param int isIncremental: Make incremental update or over-write plugin weather
  :param int updateImmediately: Make changes immediately, or transition to the requested change over the next few minutes.

  Inform the simulator that you are ending a batch update of weather information. Incremental updates
  *add* to any previous weather update you've provided. Otherwise, you will *replace* all
  of your plugin weather associated with this location. This makes is possible to only
  update a fraction of your weather data at any one time.

  The normal mode of operation is that you are setting the weather "in the near future". Currently
  this is somewhere between one and two minutes (though this may change). Setting future weather
  ensures that there is no sudden jump in weather conditions. In some situations, notably
  for an initial setup, you may want to ensure that the weather is changed instantly. To do
  this, set ``updateImmediately`` to one.

  This call is not intended to be used per-frame.
   
  `Official SDK <https://developer.x-plane.com/sdk/XPLMWeather/#XPLMEndWeatherUpdate>`__ :index:`XPLMEndWeatherUpdate`

.. py:function:: weatherUpdateContext(isIncremental=1, updateImmediately=0)

  :param int isIncremental: Make incremental update or over-write plugin weather
  :param int updateImmediately: Make changes immediately, or transition to the requested change over the next few minutes.

  This is a context manager which can be used to enforce calling :func:`beginWeatherUpdate` and :func:`endWeatherUpdate`.
  Additionally, if your code has a bug in it, we'll catch it and apply :func:`endWeatherUpdate` to avoid catastrophe.::

     >>> with xp.weatherUpdateContext(updateImmediately=1):
     ...     xp.eraseWeatherAtLocation(35, -177)
     ...     xp.setWeatherAtLocation(35, -177, 100, info)
     ...     xp.setWeatherAtLocation(35, -177.5, 100, info)
     ...     xp.setWeatherAtLocation(35, -178.0, 100, info)
     ...
     >>>


Types
-----

.. data:: XPLMWeatherInfo_t

Weather information is stored in the XPLMWeatherInfo_t type. It is represented in XPPython3 as a class.
As with most python, you can get more information about it using ``help(xp.XPLMWeatherInfo_t)``.

This type was introduced in X-Plane 12.0, and then expanded with more fields in X-Plane 12.3.0. You
can use with any X-Plane 12.0+, though some of the fields will not have meaning unless used with 12.3.0+

+----------------------------------+------+------------------------------------------------+------------+-------------------+
| Field                            | Type | Description                                    |  Get/Set   |  X_Plane_Version  |
+==================================+======+================================================+============+===================+
|           detail_found           | int  |Return value from                               |Get         |12.0-Read          |
|                                  |      |xp.getWeatherAtLocation().                      |            |                   |
|                                  |      |1='detailed                                     |            |                   |
|                                  |      |weather found'. Not meaningful                  |            |                   |
|                                  |      |otherwise.                                      |            |                   |
+----------------------------------+------+------------------------------------------------+------------+-------------------+
|         temperature_alt          |float |Temperature at altitude (Celsius).              |Get/Set     | | 12.0-Read       |
|                                  |      |                                                |            | | 12.3-R/W        |
|                                  |      |To **set** temperature, either use              |            |                   |
|                                  |      |'temp_layers', *or* set ``temp_layers[0] =      |            |                   |
|                                  |      |-100`` and set this attribute for ground level: |            |                   |
|                                  |      |existing weather data will be used for          |            |                   |
|                                  |      |temperatures at altitude.                       |            |                   |
|                                  |      |                                                |            |                   |
|                                  |      |                                                |            |                   |
+----------------------------------+------+------------------------------------------------+------------+-------------------+
|dewpoint_alt                      |float |Dewpoint at altitude (Celsius).                 |Get/Set     | | 12.0-Read       |
|                                  |      |                                                |            | | 12.3-R/W        |
|                                  |      |Similar to ``temperature_alt``, to **set**, use |            |                   |
|                                  |      |'dewpoint_layers' or set initial to -100 and use|            |                   |
|                                  |      |this for ground level dewpoint                  |            |                   |
+----------------------------------+------+------------------------------------------------+------------+-------------------+
|pressure_alt                      |float |Pressure at altitude (Pascals).                 |Get/Set     | | 12.0-Read       |
|                                  |      |                                                |            | | 12.3-R/W        |
|                                  |      |On **set**, should be QNH as reported by station|            |                   |
|                                  |      |at the ground altitude given, *or* set this to 0|            |                   |
|                                  |      |and set sealevel pressure in 'pressure_sl'      |            |                   |
|                                  |      |instead.                                        |            |                   |
|                                  |      |                                                |            |                   |
+----------------------------------+------+------------------------------------------------+------------+-------------------+
|precip_rate_alt                   |float |Precipitation rate at altitude (0.0 - 1.0)      |Get         |12.0-Read          |
|                                  |      |                                                |            |                   |
|                                  |      |Ignored on **set**.                             |            |                   |
+----------------------------------+------+------------------------------------------------+------------+-------------------+
|wind_dir_alt                      |float |Wind direction at altitude. (0.0 - 360.0)       |Get         |12.0-Read          |
|                                  |      |                                                |            |                   |
|                                  |      |Ignored on **set**. (Use wind_layers.)          |            |                   |
|                                  |      |                                                |            |                   |
+----------------------------------+------+------------------------------------------------+------------+-------------------+
|wind_spd_alt                      |float |Wind speed at altitude (meters/second).         |Get         |12.0-Read          |
|                                  |      |                                                |            |                   |
|                                  |      |Ignored on **set**. (Use wind_layers.)          |            |                   |
|                                  |      |                                                |            |                   |
+----------------------------------+------+------------------------------------------------+------------+-------------------+
|turbulence_alt                    |float |Clear-air turbulence ratio at altitude. (Values |Get         |12.0-Read          |
|                                  |      |to be determined)                               |            |                   |
|                                  |      |                                                |            |                   |
|                                  |      |Ignored on **set**. (Use wind_layers)           |            |                   |
+----------------------------------+------+------------------------------------------------+------------+-------------------+
|wave_height                       |float |Wave height (meters)                            |Get/Set     | | 12.0-Read       |
|                                  |      |                                                |            | | 12.3-R/W        |
|                                  |      |                                                |            |                   |
+----------------------------------+------+------------------------------------------------+------------+-------------------+
|wave_length                       |float |Wave length (meters).                           |Get         |12.0-Read          |
|                                  |      |                                                |            |                   |
|                                  |      |Ignored on **set**. (Derived from other data)   |            |                   |
+----------------------------------+------+------------------------------------------------+------------+-------------------+
|wave_dir                          |int   |Wave direction (waves coming from...)  degrees. |Get/Set     | | 12.0-Read       |
|                                  |      |                                                |            | | 12.3-R/W        |
|                                  |      |                                                |            |                   |
|                                  |      |                                                |            |                   |
+----------------------------------+------+------------------------------------------------+------------+-------------------+
|wave_speed                        |float |Wave speed (meters/second).                     |Get         | | 12.0-Read       |
|                                  |      |                                                |            | | 12.3-R/W        |
|                                  |      |Ignored on **set**. (Derived from other data.)  |            |                   |
+----------------------------------+------+------------------------------------------------+------------+-------------------+
|visibility                        |float |Base visibility at *0 altitude* (distance in    |Get/Set     | | 12.0-Read       |
|                                  |      |meters)                                         |            | | 12.3-R/W        |
|                                  |      |                                                |            |                   |
+----------------------------------+------+------------------------------------------------+------------+-------------------+
|precip_rate                       |float |Base precipitation ratio at *0 altitude*. (0.0 -|Get/Set     | | 12.0-Read       |
|                                  |      |1.0)                                            |            | | 12.3-R/W        |
|                                  |      |                                                |            |                   |
+----------------------------------+------+------------------------------------------------+------------+-------------------+
|thermal_climb                     |float |Climb rate due to thermals (meters/second)      |Get/Set     | | 12.0-Read       |
|                                  |      |                                                |            | | 12.3-R/W        |
|                                  |      |                                                |            |                   |
+----------------------------------+------+------------------------------------------------+------------+-------------------+
|pressure_sl                       |float |Pressure at sealevel (Pascals).                 |Get/Set     | | 12.0-Read       |
|                                  |      |                                                |            | | 12.3-R/W        |
|                                  |      |On **set**, this is ignored if 'pressure_alt' is|            |                   |
|                                  |      |non-zero.                                       |            |                   |
+----------------------------------+------+------------------------------------------------+------------+-------------------+
|wind_layers                       |list  |List of up to :data:`NumWindLayers`             |Get/Set     | | 12.0-Read       |
|                                  |      |:data:`XPLMWeatherInfoWinds_t` objects.         |            | | 12.3-R/W        |
|                                  |      |                                                |            |                   |
|                                  |      |Not all wind layers are always defined.         |            |                   |
|                                  |      |                                                |            |                   |
+----------------------------------+------+------------------------------------------------+------------+-------------------+
|cloud_layers                      |list  |List of up to :data:`NumCloudLayers`            |Get/Set     | | 12.0-Read       |
|                                  |      |:data:`XPLMWeatherInfoClouds_t` objects.        |            | | 12.3-R/W        |
|                                  |      |                                                |            |                   |
|                                  |      |Not all cloud layers are always defined.        |            |                   |
+----------------------------------+------+------------------------------------------------+------------+-------------------+
|temp_layers                       |list  |List of floats for temperature in Celsius at    |Get/Set     |12.3               |
|                                  |      |predefined atmosphere levels.                   |            |                   |
|                                  |      |                                                |            |                   |
|                                  |      |For layer altitudes, see dataref                |            |                   |
|                                  |      |'sim/weather/region/atmosphere_alt_levels_m'.   |            |                   |
|                                  |      |                                                |            |                   |
|                                  |      |On **set**, if temp_layers[0] <= -100.,         |            |                   |
|                                  |      |'temperature_alt' is used for ground temperature|            |                   |
|                                  |      |and existing altitude temps are used.           |            |                   |
+----------------------------------+------+------------------------------------------------+------------+-------------------+
|dewp_layers                       |list  |List of floats for dewpoints in Celsius at      |Get/Set     |12.3               |
|                                  |      |predefined atmosphere levels.                   |            |                   |
|                                  |      |                                                |            |                   |
|                                  |      |For layer altitudes, see dataref                |            |                   |
|                                  |      |'sim/weather/region/atmosphere_alt_levels_m'.   |            |                   |
|                                  |      |                                                |            |                   |
|                                  |      |On **set**, if dewp_layers[0] <= -100.,         |            |                   |
|                                  |      |'dewpoint_alt' is used for ground dewpoint      |            |                   |
|                                  |      |temperature and existing altitude dewpoints     |            |                   |
|                                  |      |temps are used.                                 |            |                   |
|                                  |      |                                                |            |                   |
|                                  |      |                                                |            |                   |
+----------------------------------+------+------------------------------------------------+------------+-------------------+
|troposphere_alt                   |float |Altitude of troposphere in meters.              |Get/Set     |12.3               |
|                                  |      |                                                |            |                   |
|                                  |      |On **set**, if this is 0 or lower,              |            |                   |
|                                  |      |'troposphere_alt' and 'troposphere_temp' will be|            |                   |
|                                  |      |derived from existing data.                     |            |                   |
|                                  |      |                                                |            |                   |
+----------------------------------+------+------------------------------------------------+------------+-------------------+
|troposphere_temp                  |float |Temperature in Celsius of troposphere.          |Get/Set     |12.3               |
|                                  |      |                                                |            |                   |
|                                  |      |On **set** this is ignored when                 |            |                   |
|                                  |      |``troposphere_alt`` is 0 or less.               |            |                   |
|                                  |      |                                                |            |                   |
+----------------------------------+------+------------------------------------------------+------------+-------------------+
|age                               |float |Age in seconds of this weather report. Age      |Set         |12.3               |
|                                  |      |affects how strongly the report affects the     |            |                   |
|                                  |      |weather. Commonly, you will set this to zero so |            |                   |
|                                  |      |you change will be effective.                   |            |                   |
|                                  |      |                                                |            |                   |
|                                  |      |This is meaningless on read and *should* be     |            |                   |
|                                  |      |zero.                                           |            |                   |
+----------------------------------+------+------------------------------------------------+------------+-------------------+
|radius_nm                         |float |Horizontal radius of effect of this weather     |Set         |12.3               |
|                                  |      |report, nautical miles. You need to set this to |            |                   |
|                                  |      |something.                                      |            |                   |
|                                  |      |                                                |            |                   |
|                                  |      |This is meaningless on read.                    |            |                   |
+----------------------------------+------+------------------------------------------------+------------+-------------------+
|max_altitude_msl_ft               |float |Vertical radius of effect of this weather       |Set         |12.3               |
|                                  |      |report, feet MSL. You need to set this to       |            |                   |
|                                  |      |something.                                      |            |                   |
|                                  |      |                                                |            |                   |
|                                  |      |This meaningless on read.                       |            |                   |
+----------------------------------+------+------------------------------------------------+------------+-------------------+

.. data:: XPLMWeatherInfoClouds_t

+----------------------------------+------+------------------------------------------------+------------+-------------------+
| Field                            | Type | Description                                    |  Get/Set   |  X_Plane_Version  |
+==================================+======+================================================+============+===================+
|            cloud_type            |float |Cloud Type, effectively an enumeration:         |Get/Set     | | 12.0-Read       |
|                                  |      |                                                |            | | 12.3-R/W        |
|                                  |      |  | 0 = Cirrus                                  |            |                   |
|                                  |      |  | 1 = Stratus                                 |            |                   |
|                                  |      |  | 2 = Cumulus                                 |            |                   |
|                                  |      |  | 3 = Cumulo-nimbus                           |            |                   |
+----------------------------------+------+------------------------------------------------+------------+-------------------+
|             coverage             |float |Coverage radio (0.0 - 1.0)                      |Get/Set     | | 12.0-Read       |
|                                  |      |                                                |            | | 12.3-R/W        |
|                                  |      |                                                |            |                   |
+----------------------------------+------+------------------------------------------------+------------+-------------------+
|alt_top                           |float |Altitude of cloud top. (MSL in meters)          |Get/Set     | | 12.0-Read       |
|                                  |      |                                                |            | | 12.3-R/W        |
|                                  |      |When alt_top == alt_base, the cloud layer is    |            |                   |
|                                  |      |undefined.                                      |            |                   |
+----------------------------------+------+------------------------------------------------+------------+-------------------+
|alt_base                          |float |Altitude of cloud base. (MSL in meters)         |Get/Set     | | 12.0-Read       |
|                                  |      |                                                |            | | 12.3 R/W        |
|                                  |      |                                                |            |                   |
+----------------------------------+------+------------------------------------------------+------------+-------------------+

.. data:: XPLMWeatherInfoWinds_t

+----------------------------------+------+------------------------------------------------+------------+-------------------+
| Field                            | Type | Description                                    |  Get/Set   |  X_Plane_Version  |
+==================================+======+================================================+============+===================+
|             alt_msl              |float |Altitude of wind layer. (MSL in meters)         |Get/Set     | | 12.0-Read       |
|                                  |      |                                                |            | | 12.3-R/W        |
|                                  |      |(Depth of wind layer is not specified.)         |            |                   |
+----------------------------------+------+------------------------------------------------+------------+-------------------+
|              speed               |float |Wind speed (meters/second)                      |Get/Set     | | 12.0-Read       |
|                                  |      |                                                |            | | 12.3-R/W        |
|                                  |      |On **set** if this is                           |            |                   |
|                                  |      |:data:`WindUndefinedLayer`, this layer is       |            |                   |
|                                  |      |undefined and (effectively) skipped.            |            |                   |
|                                  |      |                                                |            |                   |
|                                  |      |You can have defined layers above undefined     |            |                   |
|                                  |      |layers.                                         |            |                   |
+----------------------------------+------+------------------------------------------------+------------+-------------------+
|direction                         |float |Wind direction (degrees True) (0.0-360.0)       |Get/Set     | | 12.0-Read       |
|                                  |      |                                                |            | | 12.3-R/W        |
+----------------------------------+------+------------------------------------------------+------------+-------------------+
|gust_speed                        |float |Gust speed, (meters/second). This is the total  |Get/Set     | | 12.0-Read       |
|                                  |      |speed, not the amount of increase over the wind |            | | 12.3-R/W        |
|                                  |      |speed.                                          |            |                   |
+----------------------------------+------+------------------------------------------------+------------+-------------------+
|shear                             |float |Searh arc, degrees. The wind will shear 50% of  |Get/Set     | | 12.0-Read       |
|                                  |      |this arc in either direction from base          |            | | 12.3-R/W        |
|                                  |      |direction.                                      |            |                   |
+----------------------------------+------+------------------------------------------------+------------+-------------------+
|turbulence                        |float |Clear Air Turbulence ratio                      |Get/Set     | | 12.0-Read       |
|                                  |      |                                                |            | | 12.3-R/W        |
+----------------------------------+------+------------------------------------------------+------------+-------------------+

Constants
=========

.. py:data:: NumWindLayers
   :value: 13

   `Official SDK <https://developer.x-plane.com/sdk/XPLMWeather/#XPLM_NUM_WIND_LAYERS>`__ :index:`XPLM_NUM_WIND_LAYERS`

.. py:data:: NumCloudLayers
   :value: 3

   `Official SDK <https://developer.x-plane.com/sdk/XPLMWeather/#XPLM_NUM_CLOUD_LAYERS>`__ :index:`XPLM_NUM_CLOUD_LAYERS`

.. py:data:: NumTemperatureLayers
   :value: 13

   `Official SDK <https://developer.x-plane.com/sdk/XPLMWeather/#XPLM_NUM_TEMPERATURE_LAYERS>`__ :index:`XPLM_NUM_TEMPERATURE_LAYERS`

.. py:data:: WindUndefinedLayer
   :value: -1

   `Official SDK <https://developer.x-plane.com/sdk/XPLMWeather/#XPLM_NUM_UNDEFINED_LAYER>`__ :index:`XPLM_NUM_UNDEFINED_LAYER`

.. py:data:: DefaultWxrRadiusNm
   :value: 30

   Default horizonal radius of weather data points set using XPLMSetWeatherAtLocation and XPLMSetWeatherAtAirport.
   Note *you still need to specify this (or another) value* on set.

   `Official SDK <https://developer.x-plane.com/sdk/XPLMWeather/#XPLM_DEFAULT_WXR_RADIUS_NM>`__ :index:`XPLM_DEFAULT_WXR_RADIUS_NM`

.. py:data:: DefaultWxrRadiusMslFt
   :value: 10000

   Default vertical radius of weather data points set using XPLMSetWeatherAtLocation and XPLMSetWeatherAtAirport.
   Note *you still need to specify this (or another) value* on set.

   `Official SDK <https://developer.x-plane.com/sdk/XPLMWeather/#XPLM_DEFAULT_WXR_RADIUS_MSL_FT>`__ :index:`XPLM_DEFAULT_WXR_RADIUS_MSL_FT`

..
  Weather notes:

  XPLMGetWeatherAtLocation always returns a single point sample, so 'age' and 'radius' are irrelevant
  They _should_ be cleared on get -- buy that may not be implemented yet.
  'age' is in seconds, and is used when the system reads the modified weather points, to decide
  how relevant that reading is.
  Sets() work during manual and real weather, but it does "age", so if you don't update it, it becomes
  less-and-less relevant. The fade-out is something like 3 or 4 hours
  Set() has radius_nm and max_altitude_msl_ft -- is it a sphere or cylinder? How does max_altitude_msl_ft
  interact with Set()'s altitude_m? ... I'm guessing 'max_altitude_msl_ft' is used only on SetAirport, but
  ignored on SetLocation -- which uses 'altitude_msl' parameter instead.

  Weather aging is based on sim time, not clock time (run sim at double speed, and transition occurs twice
  as fast.)

  wind_speed set to -1 ("undefined") means no data for this layer (i.e., skip) This is different from 0 wind at layer.

  I believe(?) you can pass a version1 XPLMWeatherInfo_t struct, in which case the default radius and radious msl_feet
  are set to XPLM_DEFAULT_WXR_RADIUS_NM and XPLM_DEFAULT_WXR_RADIUS_MSL_FT.
  
  When 'set' is done withing begin/end

  WeatherInfo vs XPLMWeatherInfo_t ??
