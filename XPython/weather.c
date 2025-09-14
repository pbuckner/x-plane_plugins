#define _GNU_SOURCE 1
#include <Python.h>
#include <math.h>
#include <stddef.h>
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMUtilities.h>
#include <XPLM/XPLMWeather.h>
#include "plugin_dl.h"
#include "utils.h"
#include "xppythontypes.h"

static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  Py_RETURN_NONE;
}

static int extractFloatAttr(PyObject *obj, char* attr_name, float *target) {
  PyObject *attr;
  if ((attr = PyObject_GetAttrString(obj, attr_name))) {
    *target = PyFloat_AsDouble(attr);
    Py_DECREF(attr);
    if (!PyErr_Occurred()) return 1;
  }
  return 0;
}

static int extractIntAttr(PyObject *obj, char* attr_name, int *target) {
  PyObject *attr;
  if ((attr = PyObject_GetAttrString(obj, attr_name))) {
    *target = PyLong_AsLong(attr);
    Py_DECREF(attr);
    if (!PyErr_Occurred()) return 1;
  }
  return 0;
}

static int extractWindLayersFromPyList(PyObject *windLayersList, XPLMWeatherInfoWinds_t *wind_layers)
{
  if (!windLayersList) {
    PyErr_SetString(PyExc_AttributeError, "wind_layers list is NULL");
    return 0;
  }
  if (!PyList_Check(windLayersList)) {
    PyErr_SetString(PyExc_TypeError, "wind_layers must be a list");
    return 0;
  }

  Py_ssize_t windLayersSize = PyList_Size(windLayersList);
  if (windLayersSize < 0) {
    return 0;
  }
  if (windLayersSize > XPLM_NUM_WIND_LAYERS) {
    windLayersSize = XPLM_NUM_WIND_LAYERS;
  }

  for (Py_ssize_t i = 0; i < windLayersSize; i++) {
    PyObject *windLayer = PyList_GetItem(windLayersList, i);
    if (!windLayer) return 0;
    if (!extractFloatAttr(windLayer, "alt_msl", &wind_layers[i].alt_msl)) return 0;
    if (!extractFloatAttr(windLayer, "speed", &wind_layers[i].speed)) return 0;
    if (!extractFloatAttr(windLayer, "direction", &wind_layers[i].direction)) return 0;
    if (!extractFloatAttr(windLayer, "gust_speed", &wind_layers[i].gust_speed)) return 0;
    if (!extractFloatAttr(windLayer, "shear", &wind_layers[i].shear)) return 0;
    if (!extractFloatAttr(windLayer, "turbulence", &wind_layers[i].turbulence)) return 0;
  }

  // Fill remaining layers with default/empty values if needed
  for (Py_ssize_t i = windLayersSize; i < XPLM_NUM_WIND_LAYERS; i++) {
    memset(&wind_layers[i], 0, sizeof(XPLMWeatherInfoWinds_t));
  }
  
  return 1;
}

static int extractCloudLayersFromPyList(PyObject *cloudLayersList, XPLMWeatherInfoClouds_t *cloud_layers)
{
  if (!cloudLayersList) {
    PyErr_SetString(PyExc_AttributeError, "cloud_layers list is NULL");
    return 0;
  }
  if (!PyList_Check(cloudLayersList)) {
    PyErr_SetString(PyExc_TypeError, "cloud_layers must be a list");
    return 0;
  }

  Py_ssize_t cloudLayersSize = PyList_Size(cloudLayersList);
  if (cloudLayersSize < 0) return 0;
  if (cloudLayersSize > XPLM_NUM_CLOUD_LAYERS) {
    cloudLayersSize = XPLM_NUM_CLOUD_LAYERS;
  }

  for (Py_ssize_t i = 0; i < cloudLayersSize; i++) {
    PyObject *cloudLayer = PyList_GetItem(cloudLayersList, i);
    if (!cloudLayer) return 0;
    if (!extractFloatAttr(cloudLayer, "cloud_type", &cloud_layers[i].cloud_type)) return 0;
    if (!extractFloatAttr(cloudLayer, "coverage", &cloud_layers[i].coverage)) return 0;
    if (!extractFloatAttr(cloudLayer, "alt_top", &cloud_layers[i].alt_top)) return 0;
    if (!extractFloatAttr(cloudLayer, "alt_base", &cloud_layers[i].alt_base)) return 0;
  }

  // Fill remaining layers with default/empty values if needed
  for (Py_ssize_t i = cloudLayersSize; i < XPLM_NUM_CLOUD_LAYERS; i++) {
    memset(&cloud_layers[i], 0, sizeof(XPLMWeatherInfoClouds_t));
  }
  return 1;
}

static int extractDewpLayersFromPyList(PyObject *dewpLayersList, float *dewp_layers)
{
  if (!dewpLayersList) {
    PyErr_SetString(PyExc_AttributeError, "dewp_layers list is NULL");
    return 0;
  }
  if (!PyList_Check(dewpLayersList)) {
    PyErr_SetString(PyExc_TypeError, "dewp_layers must be a list");
    return 0;
  }

  Py_ssize_t dewpLayersSize = PyList_Size(dewpLayersList);
  if (dewpLayersSize < 0) {
    return 0;
  }
  if (dewpLayersSize > XPLM_NUM_TEMPERATURE_LAYERS) {
    dewpLayersSize = XPLM_NUM_TEMPERATURE_LAYERS;
  }

  for (Py_ssize_t i = 0; i < dewpLayersSize; i++) {
    PyObject *dewpValue = PyList_GetItem(dewpLayersList, i);
    if (!dewpValue) {
      return 0;
    }
    
    dewp_layers[i] = (dewpValue == Py_None ? 0.0 : PyFloat_AsDouble(dewpValue));
    if (PyErr_Occurred()) {
      return 0;
    }
  }

  // Fill remaining layers with default/empty values if needed
  for (Py_ssize_t i = dewpLayersSize; i < XPLM_NUM_TEMPERATURE_LAYERS; i++) {
    dewp_layers[i] = 0.0f;
  }
  return 1;
}

static int extractTempLayersFromPyList(PyObject *tempLayersList, float *temp_layers)
{
  if (!tempLayersList) {
    PyErr_SetString(PyExc_AttributeError, "temp_layers list is NULL");
    return 0;
  }
  if (!PyList_Check(tempLayersList)) {
    PyErr_SetString(PyExc_TypeError, "temp_layers must be a list");
    return 0;
  }

  Py_ssize_t tempLayersSize = PyList_Size(tempLayersList);
  if (tempLayersSize < 0) {
    return 0;
  }
  if (tempLayersSize > XPLM_NUM_TEMPERATURE_LAYERS) {
    tempLayersSize = XPLM_NUM_TEMPERATURE_LAYERS;
  }

  for (Py_ssize_t i = 0; i < tempLayersSize; i++) {
    PyObject *tempValue = PyList_GetItem(tempLayersList, i);
    if (!tempValue) {
      return 0;
    }
    
    temp_layers[i] = (tempValue == Py_None ? 0.0 : PyFloat_AsDouble(tempValue));
    if (PyErr_Occurred()) {
      return 0;
    }
  }

  // Fill remaining layers with default/empty values if needed
  for (Py_ssize_t i = tempLayersSize; i < XPLM_NUM_TEMPERATURE_LAYERS; i++) {
    temp_layers[i] = 0.0f;
  }
  return 1;
}

static int setWeather(PyObject *infoObj, XPLMWeatherInfo_t *infop) {
  /* we KNOW this is SDK420 (at least)  */
  infop->structSize = sizeof(XPLMWeatherInfo_t);

  if (!extractFloatAttr(infoObj, "temperature_alt", &infop->temperature_alt)) return 0;
  if (!extractFloatAttr(infoObj, "dewpoint_alt", &infop->dewpoint_alt)) return 0;
  if (!extractFloatAttr(infoObj, "pressure_alt", &infop->pressure_alt)) return 0;
  
  //infop->precip_rate_alt: Not used for Set
  //infop->wind_dir_alt: Not used for Set
  //infop->wind_spd_alt: Not used for Set
  //infop->turbulence_alt: Not used for Set
  
  if (!extractFloatAttr(infoObj, "wave_height", &infop->wave_height)) return 0;
  
  //infop->wave_length: Not used for Set
  
  if (!extractIntAttr(infoObj, "wave_dir", &infop->wave_dir)) return 0;
  
  //infop->wave_speed: Not used for Set 
  
  if (!extractFloatAttr(infoObj, "visibility", &infop->visibility)) return 0;
  if (!extractFloatAttr(infoObj, "precip_rate", &infop->precip_rate)) return 0;
  if (!extractFloatAttr(infoObj, "thermal_climb", &infop->thermal_climb)) return 0;
  if (!extractFloatAttr(infoObj, "pressure_sl", &infop->pressure_sl)) return 0;

  // Extract wind_layers from Python list
  PyObject *windLayersList = PyObject_GetAttrString(infoObj, "wind_layers");
  if (!windLayersList) {
    PyErr_SetString(PyExc_AttributeError, "infoObj missing wind_layers attribute");
    return 0;
  }
  if (!extractWindLayersFromPyList(windLayersList, infop->wind_layers)) {
    Py_DECREF(windLayersList);
    return 0;
  }
  Py_DECREF(windLayersList);

  // Extract cloud_layers from Python list
  PyObject *cloudLayersList = PyObject_GetAttrString(infoObj, "cloud_layers");
  if (!cloudLayersList) {
    PyErr_SetString(PyExc_AttributeError, "infoObj missing cloud_layers attribute");
    return 0;
  }
  if (!extractCloudLayersFromPyList(cloudLayersList, infop->cloud_layers)) {
    Py_DECREF(cloudLayersList);
    return 0;
  }
  Py_DECREF(cloudLayersList);

  // Extract temp_layers from Python list
  PyObject *tempLayersList = PyObject_GetAttrString(infoObj, "temp_layers");
  if (!tempLayersList) {
    PyErr_SetString(PyExc_AttributeError, "infoObj missing temp_layers attribute");
    return 0;
  }
  if (!extractTempLayersFromPyList(tempLayersList, infop->temp_layers)) {
    Py_DECREF(tempLayersList);
    return 0;
  }
  Py_DECREF(tempLayersList);

  // Extract dewp_layers from Python list
  PyObject *dewpLayersList = PyObject_GetAttrString(infoObj, "dewp_layers");
  if (!dewpLayersList) {
    PyErr_SetString(PyExc_AttributeError, "infoObj missing dewp_layers attribute");
    return 0;
  }
  if(!extractDewpLayersFromPyList(dewpLayersList, infop->dewp_layers)) {
    Py_DECREF(dewpLayersList);
    return 0;
  }
  Py_DECREF(dewpLayersList);
  
  if (!extractFloatAttr(infoObj, "troposphere_alt", &infop->troposphere_alt)) return 0;
  if (!extractFloatAttr(infoObj, "troposphere_temp", &infop->troposphere_temp)) return 0;
  if (!extractFloatAttr(infoObj, "age", &infop->age)) return 0;
  if (!extractFloatAttr(infoObj, "radius_nm", &infop->radius_nm)) return 0;
  if (!extractFloatAttr(infoObj, "max_altitude_msl_ft", &infop->max_altitude_msl_ft)) return 0;

  return 1;
}

My_DOCSTR(_getMETARForAirport__doc__, "getMETARForAirport",
          "airport_id",
          "airport_id:str",
          "str",
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

My_DOCSTR(_getWeatherAtLocation__doc__, "getWeatherAtLocation",
          "latitude, longitude, altitude_m",
          "latitude:float, longitude:float, altitude_m:float",
          "None | XPLMWeatherInfo_t",
          "Returns current weather conditions at given location.\n"
          "Note this appears to work world-wide, though accuracy suffers with distance from current\n"
          "position.\n"
          "\n"
          "Returns WeatherInfo object on success, None otherwise. This call\n"
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
  int xp, xplm;
  XPLMHostApplicationID hostID;
  XPLMGetVersions(&xp, &xplm, &hostID);
  out_info.structSize = sizeof(XPLMWeatherInfo_t);
  if (xplm < 420) {
    out_info.structSize = offsetof(XPLMWeatherInfo_t, temp_layers);
  }

  out_info.temperature_alt = NAN;  /* set a bad value & use that to check success/failure of return */
  int ret = XPLMGetWeatherAtLocation_ptr(latitude, longitude, altitude_m, &out_info);
  if (isnan(out_info.temperature_alt)) {
    Py_RETURN_NONE;
  }

  PyObject *cloud_layers = PyList_New(XPLM_NUM_CLOUD_LAYERS);
  for (int i=0; i<XPLM_NUM_CLOUD_LAYERS; i++) {
    PyObject *layer = PyWeatherInfoClouds_New(out_info.cloud_layers[i].cloud_type,
                                              out_info.cloud_layers[i].coverage,
                                              out_info.cloud_layers[i].alt_top,
                                              out_info.cloud_layers[i].alt_base);
    PyList_SetItem(cloud_layers, i, layer);
  }
  PyObject *wind_layers = PyList_New(XPLM_NUM_WIND_LAYERS);
  for (int i=0; i<XPLM_NUM_WIND_LAYERS; i++) {
    PyObject *layer = PyWeatherInfoWinds_New(out_info.wind_layers[i].alt_msl,
                                             out_info.wind_layers[i].speed,
                                             out_info.wind_layers[i].direction,
                                             out_info.wind_layers[i].gust_speed,
                                             out_info.wind_layers[i].shear,
                                             out_info.wind_layers[i].turbulence);
                                              
    PyList_SetItem(wind_layers, i, layer);
  }
  
  PyObject *temp_layers = PyList_New(XPLM_NUM_TEMPERATURE_LAYERS);
  for (int i=0; i<XPLM_NUM_TEMPERATURE_LAYERS; i++) {
    PyObject *layer = PyFloat_FromDouble(out_info.temp_layers[i]);
    PyList_SetItem(temp_layers, i, layer);
  }
  
  PyObject *dewp_layers = PyList_New(XPLM_NUM_TEMPERATURE_LAYERS);
  for (int i=0; i<XPLM_NUM_TEMPERATURE_LAYERS; i++) {
    PyObject *layer = PyFloat_FromDouble(out_info.dewp_layers[i]);
    PyList_SetItem(dewp_layers, i, layer);
  }

  return PyWeatherInfo_New(ret, out_info.temperature_alt, out_info.dewpoint_alt, out_info.pressure_alt, out_info.precip_rate_alt,
                           out_info.wind_dir_alt, out_info.wind_spd_alt, out_info.turbulence_alt, out_info.wave_height,
                           out_info.wave_length, out_info.wave_dir, out_info.wave_speed, out_info.visibility, out_info.precip_rate,
                           out_info.thermal_climb, out_info.pressure_sl, wind_layers, cloud_layers,
                           temp_layers, dewp_layers, out_info.troposphere_alt, out_info.troposphere_temp,
                           out_info.age, out_info.radius_nm, out_info.max_altitude_msl_ft);
}

My_DOCSTR(_beginWeatherUpdate__doc__, "beginWeatherUpdate",
          "",
          "",
          "None",
          "Inform the simulator that you are starting a batch update of weather information.\n"
          "You must call endWeatherUpdate() prior to exiting your callback in order for\n"
          "changes to be recorded.");
static PyObject *XPLMBeginWeatherUpdateFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  (void) self;
  (void) args;
  (void) kwargs;
  if(!XPLMBeginWeatherUpdate_ptr) {
    PyErr_SetString(PyExc_RuntimeError , "XPLMBeginWeatherUpdate is available only in XPLM420 and up.");
    return NULL;
  }

  XPLMBeginWeatherUpdate_ptr();

  Py_RETURN_NONE;
}
  
My_DOCSTR(_endWeatherUpdate__doc__, "endWeatherUpdate",
          "isIncremental=1, updateImmediately=0",
          "isIncremental:int, updateImmediately:int",
          "None",
          "Inform the simulator that you are ending a batch update of weather information.\n"
          "Incremental updates add to any previous weather updates you've provided, otherwise\n"
          "previous update passed by plugin are ignored. Immediate updates may cause a sudden\n"
          "jump in the weather: otherwise weather is transitioned to new data.");
static PyObject *XPLMEndWeatherUpdateFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  (void) self;
  if(!XPLMEndWeatherUpdate_ptr) {
    PyErr_SetString(PyExc_RuntimeError , "XPLMEndWeatherUpdate is available only in XPLM420 and up.");
    return NULL;
  }
  int immediately = 0, incremental = 1;
  
  static char *keywords[] = {"isIncremental", "updateImmediately", NULL};
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|ii", keywords, &incremental, &immediately)){
    return NULL;
  }    
  
  XPLMEndWeatherUpdate_ptr(incremental, immediately);
  Py_RETURN_NONE;
}

My_DOCSTR(_eraseWeatherAtLocation__doc__, "eraseWeatherAtLocation",
          "latitude, longitude",
          "latitude:float, longitude:float",
          "None",
          "Erase the plugin-provided weather conditions at given location.");
static PyObject *XPLMEraseWeatherAtLocationFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  (void) self;
  static char *keywords[] = {"latitude", "longitude", NULL};
  if(!XPLMEraseWeatherAtLocation_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMEraseWeatherAtLocation is available only in XPLM420 and up.");
    return NULL;
  }

  double latitude, longitude;

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "dd", keywords, &latitude, &longitude)) {
    return NULL;
  }

  XPLMEraseWeatherAtLocation_ptr(latitude, longitude);

  Py_RETURN_NONE;
}

My_DOCSTR(_setWeatherAtLocation__doc__, "setWeatherAtLocation",
          "latitude, longitude, altitude_m, info",
          "latitude:float, longitude:float, altitude_m:float, info:XPLMWeatherInfo_t",
          "None",
          "Set the current weather conditions at given location. See documentation\n"
          "for information on use of fields in XPLMWeatherInfo_t.");

static PyObject *XPLMSetWeatherAtLocationFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  (void) self;
  static char *keywords[] = {"latitude", "longitude", "altitude_m", "info", NULL};
  if(!XPLMSetWeatherAtLocation_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMSetWeatherAtLocation is available only in XPLM420 and up.");
    return NULL;
  }

  double latitude, longitude, altitude_m;
  PyObject *infoObj;
    
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "dddO", keywords, &latitude, &longitude, &altitude_m, &infoObj)) {
    return NULL;
  }

  XPLMWeatherInfo_t info;
  if(!setWeather(infoObj, &info)) return NULL;
  XPLMSetWeatherAtLocation_ptr(latitude, longitude, altitude_m, &info);
  Py_RETURN_NONE;
}


My_DOCSTR(_setWeatherAtAirport__doc__, "setWeatherAtAirport",
          "airport_id, info",
          "airport_id:str, info:XPLMWeatherInfo_t",
          "None",
          "Set the current weather conditions at given airport. See documentation\n"
          "for information on use of fields in XPLMWeatherInfo_t.");

static PyObject *XPLMSetWeatherAtAirportFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  (void) self;
  static char *keywords[] = {"airport_id", "info", NULL};
  if(!XPLMSetWeatherAtAirport_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMSetWeatherAtAirport is available only in XPLM420 and up.");
    return NULL;
  }
  char *airport_id = NULL;
  PyObject *infoObj;
    
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "sO", keywords, &airport_id, &infoObj)) {
    return NULL;
  }

  XPLMWeatherInfo_t info;
  if(!setWeather(infoObj, &info)) return NULL;
  XPLMSetWeatherAtAirport_ptr(airport_id, &info);
  Py_RETURN_NONE;
}
  
My_DOCSTR(_eraseWeatherAtAirport__doc__, "eraseWeatherAtAirport",
          "airport_id",
          "airport_id:str",
          "None",
          "Erase plugin-provided current weather conditions at given airport.");

static PyObject *XPLMEraseWeatherAtAirportFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  (void) self;
  static char *keywords[] = {"airport_id", NULL};
  if(!XPLMEraseWeatherAtAirport_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMEraseWeatherAtAirport is available only in XPLM420 and up.");
    return NULL;
  }

  char *airport_id = NULL;
    
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "s", keywords, &airport_id)) {
    return NULL;
  }

  XPLMEraseWeatherAtAirport_ptr(airport_id);

  Py_RETURN_NONE;
}
  


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
static PyMethodDef XPLMWeatherMethods[] = {
  {"getMETARForAirport", (PyCFunction)XPLMGetMETARForAirportFun, METH_VARARGS | METH_KEYWORDS, _getMETARForAirport__doc__},
  {"XPLMGetMETARForAirport", (PyCFunction)XPLMGetMETARForAirportFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"getWeatherAtLocation", (PyCFunction)XPLMGetWeatherAtLocationFun, METH_VARARGS | METH_KEYWORDS, _getWeatherAtLocation__doc__},
  {"XPLMGetWeatherAtLocation", (PyCFunction)XPLMGetWeatherAtLocationFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"beginWeatherUpdate",  (PyCFunction)XPLMBeginWeatherUpdateFun, METH_VARARGS | METH_KEYWORDS, _beginWeatherUpdate__doc__},
  {"XPLMBeginWeatherUpdate",  (PyCFunction)XPLMBeginWeatherUpdateFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"endWeatherUpdate",  (PyCFunction)XPLMEndWeatherUpdateFun, METH_VARARGS | METH_KEYWORDS, _endWeatherUpdate__doc__},
  {"XPLMEndWeatherUpdate",  (PyCFunction)XPLMEndWeatherUpdateFun, METH_VARARGS | METH_KEYWORDS, ""},

  {"XPLMSetWeatherAtLocation",  (PyCFunction)XPLMSetWeatherAtLocationFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"setWeatherAtLocation",  (PyCFunction)XPLMSetWeatherAtLocationFun, METH_VARARGS | METH_KEYWORDS, _setWeatherAtLocation__doc__},
  {"XPLMEraseWeatherAtLocation",  (PyCFunction)XPLMEraseWeatherAtLocationFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"eraseWeatherAtLocation",  (PyCFunction)XPLMEraseWeatherAtLocationFun, METH_VARARGS | METH_KEYWORDS, _eraseWeatherAtLocation__doc__},

  {"XPLMSetWeatherAtAirport",  (PyCFunction)XPLMSetWeatherAtAirportFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"setWeatherAtAirport",  (PyCFunction)XPLMSetWeatherAtAirportFun, METH_VARARGS | METH_KEYWORDS, _setWeatherAtAirport__doc__},
  {"XPLMEraseWeatherAtAirport",  (PyCFunction)XPLMEraseWeatherAtAirportFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"eraseWeatherAtAirport",  (PyCFunction)XPLMEraseWeatherAtAirportFun, METH_VARARGS | METH_KEYWORDS, _eraseWeatherAtAirport__doc__},

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
    PyModule_AddStringConstant(mod, "__author__", "Peter Buckner (pbuck@xppython3.org)");
    PyModule_AddIntConstant(mod, "NumWindLayers", XPLM_NUM_WIND_LAYERS);
    PyModule_AddIntConstant(mod, "NumCloudLayers", XPLM_NUM_CLOUD_LAYERS);
    PyModule_AddIntConstant(mod, "NumTemperatureLayers", XPLM_NUM_TEMPERATURE_LAYERS);
    PyModule_AddIntConstant(mod, "WindUndefinedLayer", XPLM_WIND_UNDEFINED_LAYER);
    PyModule_AddIntConstant(mod, "DefaultWxrRadiusNm", XPLM_DEFAULT_WXR_RADIUS_NM);
    PyModule_AddIntConstant(mod, "DefaultWxrRadiusMslFt", XPLM_DEFAULT_WXR_RADIUS_MSL_FT);
  }
  return mod;
}
