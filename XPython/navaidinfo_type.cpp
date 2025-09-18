#define _GNU_SOURCE 1
#include <Python.h>
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMNavigation.h>
#include <structmember.h>
#include <string>
#include <vector>
#include "utils.h"
#include "xppythontypes.h"
#include "cpp_utilities.hpp"

/* NavAidInfo TYPE */
typedef struct {
  PyObject_HEAD
  int type;
  float latitude;
  float longitude;
  float height;
  int frequency;
  float heading;
  PyObject *navAidID;
  PyObject *name;
  int reg;
} NavAidInfoObject;

static PyObject *
NavAidInfo_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
  (void) args;
  (void) kwds;
  NavAidInfoObject *self;
  self = (NavAidInfoObject *) type->tp_alloc(type, 0);
  if (self != NULL) {
    self->type = 0;
    self->latitude = 0;
    self->longitude = 0;
    self->height = 0;
    self->frequency = 0;
    self->heading = 0;
    self->reg = 0;
    self->navAidID = PyUnicode_FromString("");
    if (self->navAidID == NULL) {
      Py_DECREF(self);
      return NULL;
    }
    self->name = PyUnicode_FromString("");
    if (self->name == NULL) {
      Py_DECREF(self);
      return NULL;
    }
  }
  return (PyObject *) self;
}

static int
NavAidInfo_traverse(NavAidInfoObject *self, visitproc visit, void *arg)
{
  Py_VISIT(self->navAidID);
  Py_VISIT(self->name);
  return 0;
}

static int
NavAidInfo_clear(NavAidInfoObject *self)
{
  Py_CLEAR(self->navAidID);
  Py_CLEAR(self->name);
  return 0;
}
    
static void
NavAidInfo_dealloc(NavAidInfoObject *self)
{
  PyObject_GC_UnTrack(self);
  NavAidInfo_clear(self);
  Py_TYPE(self)->tp_free((PyObject *) self);
}

static int
NavAidInfo_init(NavAidInfoObject *self, PyObject *args, PyObject *kwds)
{
  std::vector<std::string> params = {"type", "latitude", "longitude", "height", "frequency", "heading", "navaAidID", "name", "reg"};
  char **kwlist = stringVectorToCharArray(params);
  PyObject *navAidID = NULL, *name, *tmp;
  if (!PyArg_ParseTupleAndKeywords(args, kwds, "|ifffifUUi", kwlist,
                                   &self->type, &self->latitude, &self->longitude, &self->height, &self->frequency, &self->heading,
                                   &navAidID, &name, &self->reg)) {
    freeCharArray(kwlist, params.size());
    return -1;
  }
  freeCharArray(kwlist, params.size());
  if (navAidID) {
    tmp = self->navAidID;
    Py_INCREF(navAidID);
    self->navAidID = navAidID;
    Py_XDECREF(tmp);
  }
  if (name) {
    tmp = self->name;
    Py_INCREF(name);
    self->name = name;
    Py_XDECREF(tmp);
  }
  return 0;
}

static PyMemberDef NavAidInfo_members[] = {
  {"type", T_INT, offsetof(NavAidInfoObject, type), 0, "XPLMNavType, (airport, ndb, dem, etc.) bitfield OR'd together"},
    {"latitude", T_FLOAT, offsetof(NavAidInfoObject, latitude), 0, "Latitude"},
    {"longitude", T_FLOAT, offsetof(NavAidInfoObject, longitude), 0, "Longitude"},
    {"height", T_FLOAT, offsetof(NavAidInfoObject, height), 0, "Height (meters)"},
    {"frequency", T_INT, offsetof(NavAidInfoObject, frequency), 0, "Frequency (x100, expect for NDB)"},
    {"heading", T_FLOAT, offsetof(NavAidInfoObject, heading), 0, "Heading, for GlideSlope value is localizer bearing (true) + GS angle x 100,000\n"
     "e.g., GS of 3.00 degrees on heading 122.53125 becomes 300122.53125"},
    {"navAidID", T_OBJECT_EX, offsetof(NavAidInfoObject, navAidID), 0, "Navaid ID"},
    {"name", T_OBJECT, offsetof(NavAidInfoObject, name), 0, "Navaid name"},
    {"reg", T_INT, offsetof(NavAidInfoObject, reg), 0, "1 if navaid is within local 'region', 0 otherwise"},
    {NULL, T_INT, 0, 0, ""}  /* Sentinel */
};

static PyObject *NavAidInfo_str(NavAidInfoObject *obj) {
  /* Name (<ID>), type, (lat, lon), frequncy*/
  std::string navAidType;
  char *floats;
  int asprintf_ret;
  if (obj->frequency == 0.0) {
    asprintf_ret = asprintf(&floats, "(%.3f, %.3f) ---", obj->latitude, obj->longitude);
  } else {
    asprintf_ret = asprintf(&floats, "(%.3f, %.3f) %.2f", obj->latitude, obj->longitude, obj->frequency/100.0);
  }

  switch(obj->type) {
  case xplm_Nav_Airport:
    navAidType = "Airport"; break;
  case xplm_Nav_NDB:
    navAidType = "NDB";
    free(floats);
    asprintf_ret = asprintf(&floats, "(%.3f, %.3f) %d", obj->latitude, obj->longitude, obj->frequency);
    break;
  case xplm_Nav_VOR:
    navAidType = "VOR"; break;
  case xplm_Nav_ILS:
    navAidType = "ILS"; break;
  case xplm_Nav_Localizer:
    navAidType = "Localizer"; break;
  case xplm_Nav_GlideSlope:
    navAidType = "GlideSlope"; break;
  case xplm_Nav_OuterMarker:
    navAidType = "OuterMarker"; break;
  case xplm_Nav_MiddleMarker:
    navAidType = "MiddleMarker"; break;
  case xplm_Nav_InnerMarker:
    navAidType = "InnerMarker"; break;
  case xplm_Nav_Fix:
    navAidType = "Fix"; break;
  case xplm_Nav_DME:
    navAidType = "DME"; break;
  case xplm_Nav_LatLon:
    navAidType = "LatLon"; break;
  case xplm_Nav_TACAN:
    navAidType = "TACAN"; break;
  case xplm_Nav_Unknown:
    navAidType = "Unknown"; break;
  default:
    navAidType = "Other Unknown";
  }
  
  if (-1 == asprintf_ret) {
    pythonLog("Failed to allocate asprintf memory. Failing navaid info.");
  }

  PyObject *ret = PyUnicode_FromFormat("%S (%S) %s %s",
                                       obj->name,
                                       obj->navAidID,
                                       navAidType.c_str(),
                                       floats);
  free(floats);
  return ret;
                              
}

PyTypeObject
NavAidInfoType = {
  PyVarObject_HEAD_INIT(NULL, 0)
  .tp_name = "XPPython.NavAidInfo",
  .tp_basicsize = sizeof(NavAidInfoObject),
  .tp_itemsize = 0,
  .tp_dealloc = (destructor) NavAidInfo_dealloc,
  .tp_str = (reprfunc) NavAidInfo_str,
  .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_GC,
  .tp_doc = PyDoc_STR("NavAidInfo, return from xp.getNavAidInfo()"),
  .tp_traverse = (traverseproc) NavAidInfo_traverse,
  .tp_clear = (inquiry) NavAidInfo_clear,
  .tp_members = NavAidInfo_members,
  .tp_init = (initproc) NavAidInfo_init,
  .tp_new = NavAidInfo_new,
};


PyObject *
PyNavAidInfo_New(int type, float latitude, float longitude, float height, int frequency, float heading, char* navAidID, char *name, int reg)
{
  PyObject *argsList = Py_BuildValue("ifffifUUi", type, latitude, longitude, height, frequency, heading, navAidID, name, reg);
  PyObject *obj = PyObject_CallObject((PyObject *) &NavAidInfoType, argsList);
  Py_XDECREF(argsList);
  return (PyObject*)obj;
}

