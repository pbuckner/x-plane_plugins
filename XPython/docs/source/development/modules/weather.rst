XPLMWeather
===========
.. py:module:: XPLMWeather
.. py:currentmodule:: xp

To use::

  import xp

This API provides access to the X-Plane enhanced weather system and requires at least X-Plane 12.04.

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

     * The ``WeatherInfo.detail_found`` attribute, on a successful return, is the *actual*
       return value from X-Plane's XPLMGetWeatherAtLocation() call. This, I'm told,
       indicates "if detail weather is found", where *detail weather* refers to the
       existence of a METAR within about 50 miles of the queried location. It is **not**
       a success/failure return value of the actual WeatherInfo object. (Nor is it an
       indication of "weather in region" vs. "weather out of region".)

       What to do with this ``detail_found`` value? Beats me. It won't tell you which METAR
       is nearby, or which of multiple nearby METARs are used perhaps to alter the world-wide
       GRIB weather data. Bug Report has been filed requesting clarification.

     Because ``detail_found`` does not indicate success or failure, XPPython3 uses
     a flag to attempt to detect if data is valid. (We set an internal data structure
     temperature to a bogus number and if post-query it is still bad, we assume
     data was not successfully obtained, and return ``None``.)

     Please, if any of this makes sense to you let me know!

  On success, returns a WeatherInfo object:

  >>> help(xp.WeatherInfo)
  class WeatherInfo(builtin.object)
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
  |  wind_layers:      List of WeatherInfoWinds objects
  |  cloud_layers:     List of WeatherInfoClouds objects

  Which include a list of objects for `wind_layer` and `cloud_layers`:

  >>> help(xp.WeatherInfoWinds)
  class WeatherInfoWinds(builtin.object)
  |  ------------------------------------
  |  Data descriptors defined here:
  |  alt_msl:        Altitude MSL (meters)
  |  speed:          Speed (meters/second)
  |  direction:      Direction (degrees true)
  |  gust_speed:     Gust speed (meters/second)
  |  shear:          Shear arc i.e., 50% of this arc in either direction from base (degrees)
  |  turbulence:     Turbulence ratio

  >>> help(xp.WeatherInfoClouds)
  class WeatherInfoClouds(builtin.object)
  |  ------------------------------------
  |  Data descriptors defined here:
  |  cloud_type:     Cloud type (float enum)
  |  coverage:       Coverage ratio
  |  alt_top:        Cloud top altitude MSL (meters)
  |  alt_base:       Cloud base altitude MSL (meters)

  For example:

  >>> info = xp.getWeatherAtLocation(34, -117, 2000)
  >>> print(info)
  <WeatherInfo object> 7.9/-10.1 252@3 QNH1009
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
  <WeatherInfoWinds object> 175@9 at 0.0m MSL
  >>> print(info.wind_layers[1])
  <WeatherInfoWinds object> 176@10 at 540.1m MSL
  >>> print(info.cloud_layers[1])
  <WeatherInfoClouds object> 2.0 (50%) from 4442.0m to 5759.5m

  If ``coverage`` is zero, there are no clouds at that layer regardless of the
  other parameters. Just skip that layer and look at the next.

  Though not documented, I assume "cloud type" enumeration is the same as the ``sim/weather/aircraft/cloud_type``
  and ``sim/weather/region/cloud_type`` datarefs:

  | 0 = Cirrus
  | 1 = Stratus
  | 2 = Cumulus
  | 3 = Cumulo-nimbus

 `Official SDK <https://developer.x-plane.com/sdk/XPLMWeather/#XPLMGetWeatherAtLocation>`__ :index:`XPLMGetWeatherAtLocation`
  
