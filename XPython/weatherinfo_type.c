#define _GNU_SOURCE 1
#include <Python.h>
#include <structmember.h>
#include "utils.h"

/* WeatherInfo Type */
typedef struct {
  PyObject_HEAD
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
  return 0;
}
static int
WeatherInfo_clear(WeatherInfoObject *self)
{
  Py_CLEAR(self->wind_layers);
  Py_CLEAR(self->cloud_layers);
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
  static char *kwlist[] = {"temperature_alt", "dewpoint_alt", "pressure_alt", "precip_rate_alt",
                           "wind_dir_alt", "wind_spd_alt", "turbulence_alt", "wave_height",
                           "wave_length", "wave_dir", "wave_speed", "visibility",
                           "precip_rate", "thermal_climb", "pressure_sl", "wind_layers", "cloud_layers", NULL};
  if (!PyArg_ParseTupleAndKeywords(args, kwds, "fffffffffifffffOO", kwlist,
                                   &self->temperature_alt, &self->dewpoint_alt, &self->pressure_alt, &self->precip_rate_alt,
                                   &self->wind_dir_alt, &self->wind_spd_alt, &self->turbulence_alt, &self->wave_height,
                                   &self->wave_length, &self->wave_dir, &self->wave_speed, &self->visibility,
                                   &self->precip_rate, &self->thermal_climb, &self->pressure_sl, &self->wind_layers,
                                   &self->cloud_layers))
    return -1;
  return 0;
}

static PyMemberDef WeatherInfo_members[] = {
  {"temperature_alt", T_FLOAT, offsetof(WeatherInfoObject, temperature_alt), 0, "Temperature at altitude (Celsius)"},
  {"dewpoint_alt", T_FLOAT, offsetof(WeatherInfoObject, dewpoint_alt), 0, "Dewpoint at altitude (Celsius)"},
  {"pressure_alt", T_FLOAT, offsetof(WeatherInfoObject, pressure_alt), 0, "Pressure at altitude (Pascals)"},
  {"precip_rate_alt", T_FLOAT, offsetof(WeatherInfoObject, precip_rate_alt), 0, "Precipitation ratio at altitude"},
  {"wind_dir_alt", T_FLOAT, offsetof(WeatherInfoObject, wind_dir_alt), 0, "Wind direction at altitude"},
  {"wind_spd_alt", T_FLOAT, offsetof(WeatherInfoObject, wind_spd_alt), 0, "Wind speed at altitude (meters/second)"},
  {"turbulence_alt", T_FLOAT, offsetof(WeatherInfoObject, turbulence_alt), 0, "Turbulence ratio at altitude"},
  {"wave_height", T_FLOAT, offsetof(WeatherInfoObject, wave_height), 0, "Wave height (meters)"},
  {"wave_length", T_FLOAT, offsetof(WeatherInfoObject, wave_length), 0, "Wave length (meters)"},
  {"wave_dir", T_INT, offsetof(WeatherInfoObject, wave_dir), 0, "Wave direction (waves moving from...)"},
  {"wave_speed", T_FLOAT, offsetof(WeatherInfoObject, wave_speed), 0, "Wave speed (meters/second)"},
  {"visibility", T_FLOAT, offsetof(WeatherInfoObject, visibility), 0, "Base visibility at 0 altitude (meters)"},
  {"precip_rate", T_FLOAT, offsetof(WeatherInfoObject, precip_rate), 0, "Base precipitation ratio at 0 altitude"},
  {"thermal_climb", T_FLOAT, offsetof(WeatherInfoObject, thermal_climb), 0, "Climb rate due to thermals (meters/second)"},
  {"pressure_sl", T_FLOAT, offsetof(WeatherInfoObject, pressure_sl), 0, "Pressure at 0 altitude (Pascals)"},
  {"wind_layers", T_OBJECT_EX, offsetof(WeatherInfoObject, wind_layers), 0, "List of WeatherInfoWinds objects"},
  {"cloud_layers", T_OBJECT_EX, offsetof(WeatherInfoObject, cloud_layers), 0, "List of WeatherInfoClouds objects"},
  {NULL, T_INT, 0, 0, ""} /* Sentinel */
};

static PyObject *WeatherInfo_str(WeatherInfoObject *obj) {
  char *msg = NULL;
  if (-1 == asprintf(&msg, "<WeatherInfo object> %.1f/%.1f %03.0f@%.0f QNH%.0f",
                     obj->temperature_alt, obj->dewpoint_alt,
                     obj->wind_dir_alt, obj->wind_spd_alt * 1.94384,
                     obj->pressure_sl / 100.0)) {
    pythonLog("Failed to allocate asprintf memory for WeatherInfo.\n");
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
  .tp_name = "XPPython.WeatherInfo",
  .tp_doc = "WeatherInfo class, return value of xp.getWeatherAtLocation().",
  .tp_basicsize = sizeof(WeatherInfoObject),
  .tp_itemsize = 0,
  .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_GC,
  .tp_new = WeatherInfo_new,
  .tp_init = (initproc) WeatherInfo_init,
  .tp_dealloc = (destructor) WeatherInfo_dealloc,
  .tp_traverse = (traverseproc) WeatherInfo_traverse,
  .tp_clear = (inquiry) WeatherInfo_clear,
  .tp_str = (reprfunc) WeatherInfo_str,
  .tp_members = WeatherInfo_members,
};

PyObject *
PyWeatherInfo_New(float temperature_alt, float dewpoint_alt, float pressure_alt, float precip_rate_alt,
                  float wind_dir_alt, float wind_spd_alt, float turbulence_alt, float wave_height,
                  float wave_length, int wave_dir, float wave_speed, float visibility, float precip_rate,
                  float thermal_climb, float pressure_sl, PyObject *wind_layers, PyObject *cloud_layers)
{
  PyObject *argsList = Py_BuildValue("fffffffffifffffOO", temperature_alt, dewpoint_alt, pressure_alt, precip_rate_alt,
                                     wind_dir_alt, wind_spd_alt, turbulence_alt, wave_height, wave_length,
                                     wave_dir, wave_speed, visibility, precip_rate, thermal_climb, pressure_sl, wind_layers, cloud_layers);
  PyObject *obj = PyObject_CallObject((PyObject *) &WeatherInfoType, argsList);
  Py_DECREF(argsList);
  return (PyObject*)obj;
}
