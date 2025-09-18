#define _GNU_SOURCE 1
#include <Python.h>
#include <structmember.h>
#include <vector>
#include <string>
#include "XPLMWeather.h"
#include "xppythontypes.h"
#include "utils.h"
#include "cpp_utilities.hpp"

/* WeatherInfo Type */
typedef struct {
  PyObject_HEAD
  int detail_found;
  float temperature_alt;
  float dewpoint_alt;
  float pressure_alt;
  float precip_rate_alt;
  float wind_dir_alt;
  float wind_spd_alt;
  float turbulence_alt;
  float wave_height;
  float wave_length;
  int wave_dir;
  float wave_speed;
  float visibility;
  float precip_rate;
  float thermal_climb;
  float pressure_sl;
  PyObject *wind_layers;
  PyObject *cloud_layers;
  PyObject *temp_layers;
  PyObject *dewp_layers;
  float troposphere_alt;
  float troposphere_temp;
  float age;
  float radius_nm;
  float max_altitude_msl_ft;
} WeatherInfoObject;

static PyObject *
WeatherInfo_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
  (void) args;
  (void) kwds;
  WeatherInfoObject *self;
  self = (WeatherInfoObject *) type->tp_alloc(type, 0);
  return (PyObject *)self;
}

static int
WeatherInfo_traverse(WeatherInfoObject *self, visitproc visit, void *arg)
{
  (void) visit;
  (void) arg;
  Py_VISIT(self->wind_layers);
  Py_VISIT(self->cloud_layers);
  Py_VISIT(self->temp_layers);
  Py_VISIT(self->dewp_layers);
  return 0;
}
static int
WeatherInfo_clear(WeatherInfoObject *self)
{
  Py_CLEAR(self->wind_layers);
  Py_CLEAR(self->cloud_layers);
  Py_CLEAR(self->temp_layers);
  Py_CLEAR(self->dewp_layers);
  return 0;
}
    
static void
WeatherInfo_dealloc(WeatherInfoObject *self)
{
  PyObject_GC_UnTrack(self);
  WeatherInfo_clear(self);
  Py_TYPE(self)->tp_free((PyObject *) self);
}

static int
WeatherInfo_init(WeatherInfoObject *self, PyObject *args, PyObject *kwds)
{
  std::vector<std::string> params = {"detail_found",
                           "temperature_alt", "dewpoint_alt", "pressure_alt", "precip_rate_alt",
                           "wind_dir_alt", "wind_spd_alt", "turbulence_alt", "wave_height",
                           "wave_length", "wave_dir", "wave_speed", "visibility",
                           "precip_rate", "thermal_climb", "pressure_sl", "wind_layers", "cloud_layers",
                           "temp_layers", "dewp_layers", "troposphere_alt", "troposphere_temp",
                           "age", "radius_nm", "max_altitude_msl_ft"};
  char **kwlist = stringVectorToCharArray(params);

  self->detail_found = -1;
  self->radius_nm = XPLM_DEFAULT_WXR_RADIUS_NM;
  self->max_altitude_msl_ft = XPLM_DEFAULT_WXR_RADIUS_MSL_FT;
  int result = PyArg_ParseTupleAndKeywords(args, kwds, "|ifffffffffifffffOOOOfffff", kwlist,
                                   &self->detail_found,
                                   &self->temperature_alt, &self->dewpoint_alt, &self->pressure_alt, &self->precip_rate_alt,
                                   &self->wind_dir_alt, &self->wind_spd_alt, &self->turbulence_alt, &self->wave_height,
                                   &self->wave_length, &self->wave_dir, &self->wave_speed, &self->visibility,
                                   &self->precip_rate, &self->thermal_climb, &self->pressure_sl, &self->wind_layers,
                                   &self->cloud_layers,
                                   &self->temp_layers, &self->dewp_layers, &self->troposphere_alt, &self->troposphere_temp,
                                   &self->age, &self->radius_nm, &self->max_altitude_msl_ft);
  freeCharArray(kwlist, params.size());
  if (!result)
    return -1;
  if (self->temp_layers == 0) {
    self->temp_layers = PyList_New(XPLM_NUM_TEMPERATURE_LAYERS);
    for(int i=0; i< XPLM_NUM_TEMPERATURE_LAYERS; i++ ){
      PyList_SET_ITEM(self->temp_layers, i, Py_None);
      Py_INCREF(Py_None);
    }
    Py_INCREF(self->temp_layers);
  }
  if (self->dewp_layers == 0) {
    self->dewp_layers = PyList_New(XPLM_NUM_TEMPERATURE_LAYERS);
    for(int i=0; i< XPLM_NUM_TEMPERATURE_LAYERS; i++ ){
      PyList_SET_ITEM(self->dewp_layers, i, Py_None);
      Py_INCREF(Py_None);
    }
    Py_INCREF(self->dewp_layers);
  }
  if (self->cloud_layers == 0) {
    self->cloud_layers = PyList_New(XPLM_NUM_CLOUD_LAYERS);
    for(int i=0; i< XPLM_NUM_CLOUD_LAYERS; i++ ){
      PyObject *obj = PyWeatherInfoClouds_New(0, 0, 0, 0);
      Py_INCREF(obj);
      PyList_SET_ITEM(self->cloud_layers, i, obj);
    }
    Py_INCREF(self->cloud_layers);
  }
  if(self->wind_layers == 0) {
    self->wind_layers = PyList_New(XPLM_NUM_WIND_LAYERS);
    for(int i=0; i< XPLM_NUM_WIND_LAYERS; i++) {
      PyObject *obj = PyWeatherInfoWinds_New(0, -1, 0, 0, 0, 0);
      Py_INCREF(obj);
      PyList_SET_ITEM(self->wind_layers, i, obj);
    }
    Py_INCREF(self->wind_layers);
  }
  return 0;
}

static PyMemberDef WeatherInfo_members[] = {
  {"detail_found", T_INT, offsetof(WeatherInfoObject, detail_found), 0,
   "Return value from XPLMGetWeatherAtLocation(), 1='detailed weather found'"},
  {"temperature_alt", T_FLOAT, offsetof(WeatherInfoObject, temperature_alt), 0,
   "Temperature at altitude (Celsius).\n"
   "To set temperature, either use 'temp_layers', or set temp_layer[0] = -100\n"
   "and set this for ground level temperature: existing weather data will be\n"
   "used for temperatures at altitude."},
  {"dewpoint_alt", T_FLOAT, offsetof(WeatherInfoObject, dewpoint_alt), 0,
   "Dewpoint at altitude (Celsius). On set, similiar to 'temperature_alt'\n"
   "and 'temp_layers'."},
  {"pressure_alt", T_FLOAT, offsetof(WeatherInfoObject, pressure_alt), 0,
   "Pressure at altitude (Pascals). On set, should be QNH as reported\n"
   "by station at the ground altitude given, or 0 if you\n"
   "are passing sealevel pressure in 'pressure_sl' instead."},
  {"precip_rate_alt", T_FLOAT, offsetof(WeatherInfoObject, precip_rate_alt), 0,
   "Precipitation ratio at altitude (0.0 - 1.0)"},
  {"wind_dir_alt", T_FLOAT, offsetof(WeatherInfoObject, wind_dir_alt), 0,
   "Wind direction at altitude. Ignored on set (derived from other data)."},
  {"wind_spd_alt", T_FLOAT, offsetof(WeatherInfoObject, wind_spd_alt), 0,
   "Wind speed at altitude (meters/second). Ignored on set (derived from other data)."},
  {"turbulence_alt", T_FLOAT, offsetof(WeatherInfoObject, turbulence_alt), 0,
   "Turbulence ratio at altitude. Ignored on set (derived from other data)."},
  {"wave_height", T_FLOAT, offsetof(WeatherInfoObject, wave_height), 0,
   "Wave height (meters)"},
  {"wave_length", T_FLOAT, offsetof(WeatherInfoObject, wave_length), 0,
   "Wave length (meters). Ignored on set (Derived from other data)."},
  {"wave_dir", T_INT, offsetof(WeatherInfoObject, wave_dir), 0,
   "Wave direction (waves moving from...) True degrees."},
  {"wave_speed", T_FLOAT, offsetof(WeatherInfoObject, wave_speed), 0,
   "Wave speed (meters/second). Ignored on set (derived from other data)."},
  {"visibility", T_FLOAT, offsetof(WeatherInfoObject, visibility), 0,
   "Base visibility at 0 altitude (distance in meters)"},
  {"precip_rate", T_FLOAT, offsetof(WeatherInfoObject, precip_rate), 0,
   "Base precipitation ratio at 0 altitude (0.0 - 1.0)"},
  {"thermal_climb", T_FLOAT, offsetof(WeatherInfoObject, thermal_climb), 0,
   "Climb rate due to thermals (meters/second)"},
  {"pressure_sl", T_FLOAT, offsetof(WeatherInfoObject, pressure_sl), 0,
   "Pressure at 0 altitude (Pascals). On set, this is ignored if\n"
   "'pressure_alt' is non-zero."},
  {"wind_layers", T_OBJECT_EX, offsetof(WeatherInfoObject, wind_layers), 0,
   "List of XPLMWeatherInfoWinds_t objects"},
  {"cloud_layers", T_OBJECT_EX, offsetof(WeatherInfoObject, cloud_layers), 0,
   "List of XPLMWeatherInfoClouds_t objects"},
  {"temp_layers", T_OBJECT_EX, offsetof(WeatherInfoObject, temp_layers), 0,
   "List of floats. See 'sim/weather/region/atmosphere_alt_levels_m'. \n"
   "On set, if temp_layers[0] <= -100., 'temperature_alt' is used for\n"
   "ground temperature and existing altitude temps are used."},
  {"dewp_layers", T_OBJECT_EX, offsetof(WeatherInfoObject, dewp_layers), 0,
   "List of floats. See 'sim/weather/region/atmosphere_alt_levels_m'. \n"
   "On set, similar to 'temp_layers' and 'temperature_alt'."},
  {"troposphere_alt", T_FLOAT, offsetof(WeatherInfoObject, troposphere_alt), 0,
   "Altitude of troposphere in meters. On set, if 0 or lower, \n"
   "'troposphere_alt' and 'troposhere_temp' will be derived from\n"
   "existing data."},
  {"troposphere_temp", T_FLOAT, offsetof(WeatherInfoObject, troposphere_temp), 0,
   "Temperature in degrees C of troposphere. Except see 'troposphere_alt'."},
  {"age", T_FLOAT, offsetof(WeatherInfoObject, age), 0,
   "Age in seconds of this weather report. Age affects how strongly the\n"
   "report affects the weather. Not meaningful on get."},
  {"radius_nm", T_FLOAT, offsetof(WeatherInfoObject, radius_nm), 0,
   "Horizontal radius of effect of this weather report, nautical miles"},
  {"max_altitude_msl_ft", T_FLOAT, offsetof(WeatherInfoObject, max_altitude_msl_ft), 0,
   "Vertical radius of effect of this weather report, feet MSL."},
  {NULL, T_INT, 0, 0, ""} /* Sentinel */
};

static PyObject *WeatherInfo_str(WeatherInfoObject *obj) {
  char *msg = NULL;
  if (-1 == asprintf(&msg, "<XPLMWeatherInfo_t object> %.1f/%.1f %03.0f@%.0f QNH%.0f",
                     obj->temperature_alt, obj->dewpoint_alt,
                     obj->wind_dir_alt, obj->wind_spd_alt * 1.94384,
                     obj->pressure_sl / 100.0)) {
    pythonLog("Failed to allocate asprintf memory for WeatherInfo.");
    return (PyObject *)obj;
  }
  PyObject *ret = PyUnicode_FromString(msg);
  Py_INCREF(ret);
  free(msg);
  return ret;
}

PyTypeObject
WeatherInfoType = {
  PyVarObject_HEAD_INIT(NULL, 0)
  .tp_name = "XPPython.XPLMWeatherInfo_t",
  .tp_basicsize = sizeof(WeatherInfoObject),
  .tp_itemsize = 0,
  .tp_dealloc = (destructor) WeatherInfo_dealloc,
  .tp_str = (reprfunc) WeatherInfo_str,
  .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_GC,
  .tp_doc = "XPLMWeatherInfo_t class, return value of xp.getWeatherAtLocation(), input to setWeather*()",
  .tp_traverse = (traverseproc) WeatherInfo_traverse,
  .tp_clear = (inquiry) WeatherInfo_clear,
  .tp_members = WeatherInfo_members,
  .tp_init = (initproc) WeatherInfo_init,
  .tp_new = WeatherInfo_new,
};

PyObject *
PyWeatherInfo_New(int detail_found, float temperature_alt, float dewpoint_alt, float pressure_alt, float precip_rate_alt,
                  float wind_dir_alt, float wind_spd_alt, float turbulence_alt, float wave_height,
                  float wave_length, int wave_dir, float wave_speed, float visibility, float precip_rate,
                  float thermal_climb, float pressure_sl, PyObject *wind_layers, PyObject *cloud_layers,
                  PyObject *temp_layers, PyObject *dewp_layers,
                  float troposphere_alt, float troposphere_temp, float age, float radius_nm, float max_altitude_msl_ft)
{
  PyObject *argsList = Py_BuildValue("ifffffffffifffffOOOOfffff", detail_found, temperature_alt, dewpoint_alt, pressure_alt, precip_rate_alt,
                                     wind_dir_alt, wind_spd_alt, turbulence_alt, wave_height, wave_length,
                                     wave_dir, wave_speed, visibility, precip_rate, thermal_climb, pressure_sl, wind_layers, cloud_layers,
                                     temp_layers, dewp_layers, troposphere_alt, troposphere_temp, age, radius_nm, max_altitude_msl_ft);
  PyObject *obj = PyObject_CallObject((PyObject *) &WeatherInfoType, argsList);
  Py_DECREF(argsList);
  return (PyObject*)obj;
}
