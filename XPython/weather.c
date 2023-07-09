#define _GNU_SOURCE 1
#include <Python.h>
#include <XPLM/XPLMWeather.h>
#include <XPLM/XPLMDefs.h>
#include "xppythontypes.h"
#include "plugin_dl.h"
#include "utils.h"

static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  Py_RETURN_NONE;
}

My_DOCSTR(_getMETARForAirport__doc__, "getMETARForAirport", "airport_id",
          "Returns 'last known' METAR report for given airport.\n"
          "\n"
          "Note the actual weather may have evolved significantly since\n"
          "the last downloaded METAR.");

static PyObject *XPLMGetMETARForAirportFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"airport_id", NULL};
  (void) self;
  if(!XPLMGetMETARForAirport_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMGetMETARForAirport is available only in XPLM400 and up, and requires at least X-Plane v12.04.");
    return NULL;
  }
  char *airport_id = NULL;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "s", keywords, &airport_id)){
    return NULL;
  }
  XPLMFixedString150_t outMetar;
  XPLMGetMETARForAirport_ptr(airport_id, &outMetar);
  return PyUnicode_DecodeUTF8((char *)&outMetar, strlen((char*)&outMetar), NULL);
}

My_DOCSTR(_getWeatherAtLocation__doc__, "getWeatherAtLocation", "latitude, longitude, altitude_m",
          "Returns current weather conditions at given location.\n"
          "Note that this does not work world-wide, only within the surrounding region.\n"
          "\n"
          "Returns WeatherInfo object if detailed weather is found, None otherwise. This call\n"
          "is not intended to be used per-frame.");
static PyObject *XPLMGetWeatherAtLocationFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"latitude", "longitude", "altitude_m", NULL};
  (void)self;
  if(!XPLMGetWeatherAtLocation_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMGetWeatherAtLocation is available only in XPLM400 and up, and requires at least X-Plane v12.04.");
    return NULL;
  }
  
  double latitude, longitude, altitude_m;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "ddd", keywords, &latitude, &longitude, &altitude_m)){
    return NULL;
  }
  XPLMWeatherInfo_t out_info;
  out_info.structSize = sizeof(XPLMWeatherInfo_t);
  int ret = XPLMGetWeatherAtLocation_ptr(latitude, longitude, altitude_m, &out_info);
  if (ret == 0) {
    Py_RETURN_NONE;
  }
  PyObject *cloud_layers = PyList_New(0);
  for (int i=0; i<3; i++) {
    PyObject *layer = PyWeatherInfoClouds_New(out_info.cloud_layers[i].cloud_type,
                                              out_info.cloud_layers[i].coverage,
                                              out_info.cloud_layers[i].alt_top,
                                              out_info.cloud_layers[i].alt_base);
    PyList_Append(cloud_layers, layer);
  }
  PyObject *wind_layers = PyList_New(0);
  for (int i=0; i<13; i++) {
    PyObject *layer = PyWeatherInfoWinds_New(out_info.wind_layers[i].alt_msl,
                                             out_info.wind_layers[i].speed,
                                             out_info.wind_layers[i].direction,
                                             out_info.wind_layers[i].gust_speed,
                                             out_info.wind_layers[i].shear,
                                             out_info.wind_layers[i].turbulence);
                                              
    PyList_Append(wind_layers, layer);
  }
  return PyWeatherInfo_New(out_info.temperature_alt, out_info.dewpoint_alt, out_info.pressure_alt, out_info.precip_rate_alt,
                           out_info.wind_dir_alt, out_info.wind_spd_alt, out_info.turbulence_alt, out_info.wave_height,
                           out_info.wave_length, out_info.wave_dir, out_info.wave_speed, out_info.visibility, out_info.precip_rate,
                           out_info.thermal_climb, out_info.pressure_sl, wind_layers, cloud_layers);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
static PyMethodDef XPLMWeatherMethods[] = {
  {"getMETARForAirport", (PyCFunction)XPLMGetMETARForAirportFun, METH_VARARGS | METH_KEYWORDS, _getMETARForAirport__doc__},
  {"XPLMGetMETARForAirport", (PyCFunction)XPLMGetMETARForAirportFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"getWeatherAtLocation", (PyCFunction)XPLMGetWeatherAtLocationFun, METH_VARARGS | METH_KEYWORDS, _getWeatherAtLocation__doc__},
  {"XPLMGetWeatherAtLocation", (PyCFunction)XPLMGetWeatherAtLocationFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"_cleanup", cleanup, METH_VARARGS, ""},
  {NULL, NULL, 0, NULL}
};
#pragma GCC diagnostic pop


static struct PyModuleDef XPLMWeatherModule = {
  PyModuleDef_HEAD_INIT,
  "XPLMWeather",
  "Laminar documentation: \n"
  "   https://developer.x-plane.com/sdk/XPLMWeather/\n"
  "XPPython3 documentation: \n"
  "   https://xppython3.rtfd.io/en/stable/development/modules/weather.html",
  -1,
  XPLMWeatherMethods,
  NULL,
  NULL,
  NULL,
  NULL
};

PyMODINIT_FUNC
PyInit_XPLMWeather(void)
{
  PyObject *mod = PyModule_Create(&XPLMWeatherModule);
  if(mod){
    PyModule_AddStringConstant(mod, "__author__", "Peter Buckner (pbuck@avnwx.com)");
  }
  return mod;
}
