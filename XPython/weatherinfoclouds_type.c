#define _GNU_SOURCE 1
#include <Python.h>
#include <structmember.h>
#include "utils.h"

/* WeatherInfoClouds Type */
typedef struct {
  PyObject_HEAD
  float cloud_type;
  float coverage;
  float alt_top;
  float alt_base;
} WeatherInfoCloudsObject;

static PyObject *
WeatherInfoClouds_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
  (void) args;
  (void) kwds;
  WeatherInfoCloudsObject *self;
  self = (WeatherInfoCloudsObject *) type->tp_alloc(type, 0);
  return (PyObject *)self;
}

static int
WeatherInfoClouds_traverse(WeatherInfoCloudsObject *self, visitproc visit, void *arg)
{
  (void) visit;
  (void) arg;
  (void) self;
  return 0;
}
static int
WeatherInfoClouds_clear(WeatherInfoCloudsObject *self)
{
  (void) self;
  return 0;
}
    
static void
WeatherInfoClouds_dealloc(WeatherInfoCloudsObject *self)
{
  PyObject_GC_UnTrack(self);
  WeatherInfoClouds_clear(self);
  Py_TYPE(self)->tp_free((PyObject *) self);
}

static int
WeatherInfoClouds_init(WeatherInfoCloudsObject *self, PyObject *args, PyObject *kwds)
{
  static char *kwlist[] = {"cloud_type", "coverage", "alt_top", "alt_base", NULL};
  if (!PyArg_ParseTupleAndKeywords(args, kwds, "ffff", kwlist,
                                   &self->cloud_type, &self->coverage, &self->alt_top, &self->alt_base))
    return -1;
  return 0;
}

static PyMemberDef WeatherInfoClouds_members[] = {
  {"cloud_type", T_FLOAT, offsetof(WeatherInfoCloudsObject, cloud_type), 0, "Cloud type (float enum)"},
  {"coverage", T_FLOAT, offsetof(WeatherInfoCloudsObject, coverage), 0, "Coverage ratio"},
  {"alt_top", T_FLOAT, offsetof(WeatherInfoCloudsObject, alt_top), 0, "Cloud top altitude MSL (meters)"},
  {"alt_base", T_FLOAT, offsetof(WeatherInfoCloudsObject, alt_base), 0, "Cloud base altitude MSL (meters)"},
  {NULL, T_INT, 0, 0, ""} /* Sentinel */
};

static PyObject *WeatherInfoClouds_str(WeatherInfoCloudsObject *obj) {
  char *msg = NULL;
  if (-1 == asprintf(&msg, "<WeatherInfoClouds object> %.1f (%.0f%%) from %.1fm to %.1fm",
                     obj->cloud_type, 100.0 * obj->coverage, obj->alt_base, obj->alt_top)) {
    pythonLog("Failed to allocate asprintf memory for WeatherInfoClouds.\n");
    return (PyObject *)obj;
  }
  PyObject *ret = PyUnicode_FromString(msg);
  Py_INCREF(ret);
  free(msg);
  return ret;
}

PyTypeObject
WeatherInfoCloudsType = {
  PyVarObject_HEAD_INIT(NULL, 0)
  .tp_name = "xppython3.WeatherInfoClouds",
  .tp_doc = "WeatherInfoClouds, class of objects listed in WeatherInfo class, one for each cloud layer",
  .tp_basicsize = sizeof(WeatherInfoCloudsObject),
  .tp_itemsize = 0,
  .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_GC,
  .tp_new = WeatherInfoClouds_new,
  .tp_init = (initproc) WeatherInfoClouds_init,
  .tp_dealloc = (destructor) WeatherInfoClouds_dealloc,
  .tp_traverse = (traverseproc) WeatherInfoClouds_traverse,
  .tp_clear = (inquiry) WeatherInfoClouds_clear,
  .tp_str = (reprfunc) WeatherInfoClouds_str,
  .tp_members = WeatherInfoClouds_members,
};

PyObject *
PyWeatherInfoClouds_New(float cloud_type, float coverage, float alt_top, float alt_base)
{
  PyObject *argsList = Py_BuildValue("ffff", cloud_type, coverage, alt_top, alt_base);
  PyObject *obj = PyObject_CallObject((PyObject *) &WeatherInfoCloudsType, argsList);
  Py_DECREF(argsList);
  return (PyObject*)obj;
}
