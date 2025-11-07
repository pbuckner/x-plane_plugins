#ifndef Py_PYTHON_H
#include <Python.h>
#endif
extern PyTypeObject WeatherInfoType, WeatherInfoCloudsType, WeatherInfoWindsType;
extern PyTypeObject DataRefInfoType,  HotKeyInfoType, PluginInfoType, NavAidInfoType, FMSEntryInfoType, TrackMetricsType,
  ProbeInfoType;
extern PyObject *PyHotKeyInfo_New(int virtualKey, int flags, char* description, int plugin);
extern PyObject *PyTrackMetrics_New(int isVertical, int downBtnSize, int downPageSize, int thumbSize, int upPageSize, int upBtnSize);
extern PyObject *PyNavAidInfo_New(int type, float latitude, float longitude, float height, int frequency, float heading, char* navAidID, char *name, int reg);
extern PyObject *PyFMSEntryInfo_New(int type, char *navAidID, int ref, int altitude, float lat, float lon);
extern PyObject *PyPluginInfo_New(char *name, char *filePath, char *signature, char *description);
extern PyObject *PyProbeInfo_New(int result, float locationX, float locationY, float locationZ, float normalX, float normalY, float normalZ, float velocityX, float velocityY, float velocityZ, int is_wet);
extern PyObject *PyDataRefInfo_New(const char *name, int type, int writable, int owner);
extern PyObject *PyWeatherInfo_New(int detail_found, float temperature_alt, float dewpoint_alt, float pressure_alt, float precip_rate_alt,
                                   float wind_dir_alt, float wind_spd_alt, float turbulence_alt,
                                   float wave_height, float wave_length, int wave_dir, float wave_speed,
                                   float visibility, float precip_rate, float thermal_climb, float pressure_sl,
                                   PyObject *wind_layers,PyObject *cloud_layers,
                                   PyObject *temp_layers, PyObject *dewp_layers,
                                   float troposphere_alt, float troposphere_temp, float age, float radius_nm, float max_altitude_msl_ft);
extern PyObject *PyWeatherInfoClouds_New(float cloud_type, float coverage, float alt_top, float alt_base);
extern PyObject *PyWeatherInfoWinds_New(float alt_msl, float speed, float direction, float gust_speed, float shear, float turbulence);
