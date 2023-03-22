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

  .. Warning::

     While the API will return *"a METAR"* for the airport, it does not necessarily return the *"latest
     METAR"* for the airport, as seen in the X-Plane downloaded METAR file. Bug reported to Laminar 11-March-2023.

 `Official SDK <https://developer.x-plane.com/sdk/XPLMWeather/#XPLMGetMETARForAirport>`__ :index:`XPLMGetMETARForAirport`

.. py:function:: getWeatherAtLocation(latitude, longitude, altitude_m)

  Get the current weather conditions at a given location. Note that this does not work
  world-wide, only within the surrounding region.

  This call is not intended to be used per-frame (use datarefs if at all possible instead).
  
  Returns ``None`` on error or out-of-region.

  On success, returns a WeatherInfo object:

  >>> help(xp.WeatherInfo)
  class WeatherInfo(builtin.object)
  |  ----------------------------------
  |  Data descriptors defined here:
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

  Though not documented, I assume "cloud type" enumeration is the same as the older dataref?:

  | 0 = Clear
  | 1 = High Cirrus
  | 2 = Scattered
  | 3 = Broken
  | 4 = Overcast
  | 5 = Stratus

 `Official SDK <https://developer.x-plane.com/sdk/XPLMWeather/#XPLMGetWeatherAtLocation>`__ :index:`XPLMGetWeatherAtLocation`
  
