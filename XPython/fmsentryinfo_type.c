#define _GNU_SOURCE 1
#include <Python.h>
#include <XPLM/XPLMNavigation.h>
#include <structmember.h>
#include "utils.h"

/* FMSEntryInfo TYPE */
typedef struct {
  PyObject_HEAD
  int type;
  PyObject *navAidID;
  int ref;
  int altitude;
  float lat;
  float lon;
} FMSEntryInfoObject;

static PyObject *
FMSEntryInfo_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
  (void) args;
  (void) kwds;
  FMSEntryInfoObject *self;
  self = (FMSEntryInfoObject *) type->tp_alloc(type, 0);
  if (self != NULL) {
    self->type = 0;
    self->lat = 0;
    self->lon = 0;
    self->altitude = 0;
    self->ref = 0;
    self->navAidID = PyUnicode_FromString("");
    if (self->navAidID == NULL) {
      Py_DECREF(self);
      return NULL;
    }
  }
  return (PyObject *) self;
}

static int
FMSEntryInfo_traverse(FMSEntryInfoObject *self, visitproc visit, void *arg)
{
  Py_VISIT(self->navAidID);
  return 0;
}

static int
FMSEntryInfo_clear(FMSEntryInfoObject *self)
{
  Py_CLEAR(self->navAidID);
  return 0;
}
    
static void
FMSEntryInfo_dealloc(FMSEntryInfoObject *self)
{
  PyObject_GC_UnTrack(self);
  FMSEntryInfo_clear(self);
  Py_TYPE(self)->tp_free((PyObject *) self);
}

static int
FMSEntryInfo_init(FMSEntryInfoObject *self, PyObject *args, PyObject *kwds)
{
  static char *kwlist[] = {"type", "navAidID", "ref", "altitude", "lat", "lon", NULL};
  PyObject *navAidID = NULL,  *tmp;
  if (!PyArg_ParseTupleAndKeywords(args, kwds, "|iUiiff", kwlist,
                                   &self->type, &navAidID, &self->ref, &self->altitude, &self->lat, &self->lon))
    return -1;
  if (navAidID) {
    tmp = self->navAidID;
    Py_INCREF(navAidID);
    self->navAidID = navAidID;
    Py_XDECREF(tmp);
  }
  return 0;
}

static PyMemberDef FMSEntryInfo_members[] = {
    {"type", T_INT, offsetof(FMSEntryInfoObject, type), 0, "XPLMNavType or NAV_NOT_FOUND if LatLon"},
    {"navAidID", T_OBJECT_EX, offsetof(FMSEntryInfoObject, navAidID), 0, "Navaid ID or None, if LatLon"},
    {"ref", T_INT, offsetof(FMSEntryInfoObject, ref), 0, "XPLMNavref or None"},
    {"altitude", T_INT, offsetof(FMSEntryInfoObject, altitude), 0, "Altitude (in feet)"},
    {"lat", T_FLOAT, offsetof(FMSEntryInfoObject, lat), 0, "Latitude"},
    {"lon", T_FLOAT, offsetof(FMSEntryInfoObject, lon), 0, "Longitude"},
    {NULL, T_INT, 0, 0, ""}  /* Sentinel */
};

static PyObject *FMSEntryInfo_str(FMSEntryInfoObject *obj) {
  char *navAidType;
  char *floats;
  switch(obj->type) {
  case xplm_Nav_Airport:
    navAidType = "Airport"; break;
  case xplm_Nav_NDB:
    navAidType = "NDB"; break;
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
  case xplm_Nav_Unknown:
    navAidType = "Unknown"; break;
  default:
    navAidType = "Other Unknown";
  }
  PyObject *ret;
  if (-1==asprintf(&floats, "(%.3f, %.3f) @%d'", obj->lat, obj->lon, obj->altitude)) {
    fprintf(pythonLogFile, "Failed to allocate memory for asprintf, FMSEntryInfo\n");
  }
  if (obj->type == xplm_Nav_LatLon) {
    ret = PyUnicode_FromFormat("%s: %s", navAidType, floats);
  } else {
    ret = PyUnicode_FromFormat("%s: [%d] %S, %s", navAidType, obj->ref, obj->navAidID, floats);
  }
  free(floats);
  return ret;
}


PyTypeObject
FMSEntryInfoType = {
  PyVarObject_HEAD_INIT(NULL, 0)
  .tp_name = "XPPython.FMSEntryInfo",
  .tp_doc = "FMSEntryInfo, return from xp.getFMSEntryInfo()",
  .tp_basicsize = sizeof(FMSEntryInfoObject),
  .tp_itemsize = 0,
  .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_GC,
  .tp_new = FMSEntryInfo_new,
  .tp_init = (initproc) FMSEntryInfo_init,
  .tp_dealloc = (destructor) FMSEntryInfo_dealloc,
  .tp_traverse = (traverseproc) FMSEntryInfo_traverse,
  .tp_clear = (inquiry) FMSEntryInfo_clear,
  .tp_str = (reprfunc) FMSEntryInfo_str,
  .tp_members = FMSEntryInfo_members,
};


PyObject *
PyFMSEntryInfo_New(int type, char *navAidID, int ref, int altitude, float lat, float lon)
{
  PyObject *argsList = Py_BuildValue("iUiiff", type, navAidID, ref, altitude, lat, lon);
  PyObject *obj = PyObject_CallObject((PyObject *) &FMSEntryInfoType, argsList);
  Py_DECREF(argsList);
  return (PyObject*)obj;
}
