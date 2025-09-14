#define _GNU_SOURCE 1
#include <Python.h>
#include <structmember.h>
#include "XPLMWeather.h"
#include "utils.h"

/* WeatherInfoWinds Type */
typedef struct {
  PyObject_HEAD
  float alt_msl;
  float speed;
  float direction;
  float gust_speed;
  float shear;
  float turbulence;
} WeatherInfoWindsObject;

static PyObject *
WeatherInfoWinds_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
  (void) args;
  (void) kwds;
  WeatherInfoWindsObject *self;
  self = (WeatherInfoWindsObject *) type->tp_alloc(type, 0);
  return (PyObject *)self;
}

static int
WeatherInfoWinds_traverse(WeatherInfoWindsObject *self, visitproc visit, void *arg)
{
  (void) visit;
  (void) arg;
  (void) self;
  return 0;
}
static int
WeatherInfoWinds_clear(WeatherInfoWindsObject *self)
{
  (void) self;
  return 0;
}
    
static void
WeatherInfoWinds_dealloc(WeatherInfoWindsObject *self)
{
  PyObject_GC_UnTrack(self);
  WeatherInfoWinds_clear(self);
  Py_TYPE(self)->tp_free((PyObject *) self);
}

static int
WeatherInfoWinds_init(WeatherInfoWindsObject *self, PyObject *args, PyObject *kwds)
{
  static char *kwlist[] = {"alt_msl", "speed", "direction", "gust_speed", "shear", "turbulence", NULL};
  self->speed = XPLM_WIND_UNDEFINED_LAYER;
  if (!PyArg_ParseTupleAndKeywords(args, kwds, "|ffffff", kwlist,
                                   &self->alt_msl, &self->speed, &self->direction, &self->gust_speed,
                                   &self->shear, &self->turbulence))
    return -1;
  return 0;
}

static PyMemberDef WeatherInfoWinds_members[] = {
  {"alt_msl", T_FLOAT, offsetof(WeatherInfoWindsObject, alt_msl), 0, "Altitude MSL (meters). (Depth of layer is not specified.)"},
  {"speed", T_FLOAT, offsetof(WeatherInfoWindsObject, speed), 0, "Speed (meters/second). On set, use negative value to signify 'undefined' for the layer."},
  {"direction", T_FLOAT, offsetof(WeatherInfoWindsObject, direction), 0, "Direction (degrees true)"},
  {"gust_speed", T_FLOAT, offsetof(WeatherInfoWindsObject, gust_speed), 0, "Gust speed (meters/second). Total speed, not incremental over wind speed."},
  {"shear", T_FLOAT, offsetof(WeatherInfoWindsObject, shear), 0, "Shear arc i.e., 50% of this arc in either direction from base direction (degrees)"},
  {"turbulence", T_FLOAT, offsetof(WeatherInfoWindsObject, turbulence), 0, "Turbulence ratio"},
  {NULL, T_INT, 0, 0, ""} /* Sentinel */
};

static PyObject *WeatherInfoWinds_str(WeatherInfoWindsObject *obj) {
  char *msg = NULL;
  if (obj->speed < 0) {
    msg = "<XPLMWeatherInfoWinds_t object> layer undefined";
  } else {
    if (-1 == asprintf(&msg, "<XPLMWeatherInfoWinds_t object> %03.0f@%.0f at %.1fm MSL",
                       obj->direction, obj->speed * 1.94384, obj->alt_msl)) {
      pythonLog("Failed to allocate asprintf memory for WeatherInfoWinds.");
      return (PyObject *)obj;
    }
  }
  PyObject *ret = PyUnicode_FromString(msg);
  Py_INCREF(ret);
  free(msg);
  return ret;
}

PyTypeObject
WeatherInfoWindsType = {
  PyVarObject_HEAD_INIT(NULL, 0)
  .tp_name = "XPPython.XPLMWeatherInfoWinds_t",
  .tp_doc = "XPLMWeatherInfoWinds_t, class of objects listed in XPLMWeatherInfo_t class, one for each wind layer",
  .tp_basicsize = sizeof(WeatherInfoWindsObject),
  .tp_itemsize = 0,
  .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_GC,
  .tp_new = WeatherInfoWinds_new,
  .tp_init = (initproc) WeatherInfoWinds_init,
  .tp_dealloc = (destructor) WeatherInfoWinds_dealloc,
  .tp_traverse = (traverseproc) WeatherInfoWinds_traverse,
  .tp_clear = (inquiry) WeatherInfoWinds_clear,
  .tp_str = (reprfunc) WeatherInfoWinds_str,
  .tp_members = WeatherInfoWinds_members,
};

PyObject *
PyWeatherInfoWinds_New(float alt_msl, float speed, float direction, float gust_speed, float shear, float turbulence)
{
  PyObject *argsList = Py_BuildValue("ffffff", alt_msl, speed, direction, gust_speed, shear, turbulence);
  PyObject *obj = PyObject_CallObject((PyObject *) &WeatherInfoWindsType, argsList);
  Py_DECREF(argsList);
  return (PyObject*)obj;
}
