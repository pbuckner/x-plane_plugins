#define _GNU_SOURCE 1
#include <Python.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdbool.h>
#include <structmember.h>
#include "xppythontypes.h"
#include "utils.h"

PyObject *xppythonDicts = NULL, *xppythonCapsules = NULL;
extern const char *pythonPluginVersion, *pythonPluginsPath, *pythonInternalPluginsPath;

/* HotKeyInfo Type */
typedef struct {
  PyObject_HEAD
  int virtualKey;
  int flags;
  PyObject *description;
  int plugin;
} HotKeyInfoObject;

static PyObject *
HotKeyInfo_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
  (void) args;
  (void) kwds;
  HotKeyInfoObject *self;
  self = (HotKeyInfoObject *) type->tp_alloc(type, 0);
  if (self != NULL) {
    self->virtualKey = 0;
    self->flags = 0;
    self->description = PyUnicode_FromString("");
    if (self->description == NULL) {
      Py_DECREF(self);
      return NULL;
    }
    self->plugin = 0;
  }
  return (PyObject *) self;
}

static int
HotKeyInfo_traverse(HotKeyInfoObject *self, visitproc visit, void *arg)
{
  Py_VISIT(self->description);
  return 0;
}

static int
HotKeyInfo_clear(HotKeyInfoObject *self)
{
  Py_CLEAR(self->description);
  return 0;
}
    
static void
HotKeyInfo_dealloc(HotKeyInfoObject *self)
{
  PyObject_GC_UnTrack(self);
  HotKeyInfo_clear(self);
  Py_TYPE(self)->tp_free((PyObject *) self);
}

static int
HotKeyInfo_init(HotKeyInfoObject *self, PyObject *args, PyObject *kwds)
{
  static char *kwlist[] = {"virtualKey", "flags", "description", "plugin", NULL};
  PyObject *description = NULL, *tmp;
  if (!PyArg_ParseTupleAndKeywords(args, kwds, "|iiUi", kwlist,
                                   &self->virtualKey, &self->flags, &description, &self->plugin))
    return -1;
  if (description) {
    tmp = self->description;
    Py_INCREF(description);
    self->description = description;
    Py_XDECREF(tmp);
  }
  return 0;
}

static PyMemberDef HotKeyInfo_members[] = {
    {"virtualKey", T_INT, offsetof(HotKeyInfoObject, virtualKey), 0, "virtual key code"},
    {"flags", T_INT, offsetof(HotKeyInfoObject, flags), 0, "XPLMKeyFlags"},
    {"description", T_OBJECT_EX, offsetof(HotKeyInfoObject, description), 0, "Description"},
    {"plugin", T_INT, offsetof(HotKeyInfoObject, plugin), 0, "XPLMPluginID"},
    {NULL}  /* Sentinel */
};

static PyTypeObject HotKeyInfoType = {
                                      PyVarObject_HEAD_INIT(NULL, 0)
                                      .tp_name = "xppython3.HotKeyInfo",
                                      .tp_doc = "HotKeyInfo",
                                      .tp_basicsize = sizeof(HotKeyInfoObject),
                                      .tp_itemsize = 0,
                                      .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_GC,
                                      .tp_new = HotKeyInfo_new,
                                      .tp_init = (initproc) HotKeyInfo_init,
                                      .tp_dealloc = (destructor) HotKeyInfo_dealloc,
                                      .tp_traverse = (traverseproc) HotKeyInfo_traverse,
                                      .tp_clear = (inquiry) HotKeyInfo_clear,
                                      .tp_members = HotKeyInfo_members,

};


PyObject *
PyHotKeyInfo_New(int virtualKey, int flags, char *description, int plugin)
{
  PyObject *argsList = Py_BuildValue("iisi", virtualKey, flags, description, plugin);
  PyObject *obj = PyObject_CallObject((PyObject *) &HotKeyInfoType, argsList);
  Py_DECREF(argsList);
  return (PyObject*)obj;
}

/* ProbeInfo Type */
typedef struct {
  PyObject_HEAD
  int result;
  float locationX;
  float locationY;
  float locationZ;
  float normalX;
  float normalY;
  float normalZ;
  float velocityX;
  float velocityY;
  float velocityZ;
  int is_wet;
} ProbeInfoObject;

static PyObject *
ProbeInfo_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
  (void) args;
  (void) kwds;
  ProbeInfoObject *self;
  self = (ProbeInfoObject *) type->tp_alloc(type, 0);
  return (PyObject *) self;
}

static int
ProbeInfo_traverse(ProbeInfoObject *self, visitproc visit, void *arg)
{
  (void) self;
  (void) visit;
  (void) arg;
  return 0;
}

static int
ProbeInfo_clear(ProbeInfoObject *self)
{
  (void) self;
  return 0;
}
    
static void
ProbeInfo_dealloc(ProbeInfoObject *self)
{
  PyObject_GC_UnTrack(self);
  ProbeInfo_clear(self);
  Py_TYPE(self)->tp_free((PyObject *) self);
}

static int
ProbeInfo_init(ProbeInfoObject *self, PyObject *args, PyObject *kwds)
{
  static char *kwlist[] = {"result",
                           "locationX", "locationY", "locationZ",
                           "normalX", "normalY", "normalZ",
                           "velocityX", "velocityY", "velocityZ",
                           "is_wet", NULL};
  if (!PyArg_ParseTupleAndKeywords(args, kwds, "|ifffffffffi", kwlist,
                                   &self->result,
                                   &self->locationX, &self->locationY, &self->locationZ,
                                   &self->normalX, &self->normalY, &self->normalZ,
                                   &self->velocityX, &self->velocityY, &self->velocityZ,
                                   &self->is_wet))
    return -1;
  return 0;
}

static PyMemberDef ProbeInfo_members[] = {
    {"result", T_INT, offsetof(ProbeInfoObject, result), 0, "XPLMProbResult, result of query"},
    {"locationX", T_FLOAT, offsetof(ProbeInfoObject, locationX), 0, "locationX"},
    {"locationY", T_FLOAT, offsetof(ProbeInfoObject, locationY), 0, "locationY"},
    {"locationZ", T_FLOAT, offsetof(ProbeInfoObject, locationZ), 0, "locationZ"},
    {"normalX", T_FLOAT, offsetof(ProbeInfoObject, normalX), 0, "normalX"},
    {"normalY", T_FLOAT, offsetof(ProbeInfoObject, normalY), 0, "normalY"},
    {"normalZ", T_FLOAT, offsetof(ProbeInfoObject, normalZ), 0, "normalZ"},
    {"velocityX", T_FLOAT, offsetof(ProbeInfoObject, velocityX), 0, "velocityX"},
    {"velocityY", T_FLOAT, offsetof(ProbeInfoObject, velocityY), 0, "velocityY"},
    {"velocityZ", T_FLOAT, offsetof(ProbeInfoObject, velocityZ), 0, "velocityZ"},
    {"is_wet", T_INT, offsetof(ProbeInfoObject, is_wet), 0, "is_wet"},
    {NULL}  /* Sentinel */
};

static PyTypeObject ProbeInfoType = {
                                      PyVarObject_HEAD_INIT(NULL, 0)
                                      .tp_name = "xppython3.ProbeInfo",
                                      .tp_doc = "ProbeInfo",
                                      .tp_basicsize = sizeof(ProbeInfoObject),
                                      .tp_itemsize = 0,
                                      .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_GC,
                                      .tp_new = ProbeInfo_new,
                                      .tp_init = (initproc) ProbeInfo_init,
                                      .tp_dealloc = (destructor) ProbeInfo_dealloc,
                                      .tp_traverse = (traverseproc) ProbeInfo_traverse,
                                      .tp_clear = (inquiry) ProbeInfo_clear,
                                      .tp_members = ProbeInfo_members,

};


PyObject *
PyProbeInfo_New(int result, float locationX, float locationY, float locationZ, float normalX, float normalY, float normalZ, float velocityX, float velocityY, float velocityZ, int is_wet)
{
  PyObject *argsList = Py_BuildValue("ifffffffffi", result, locationX, locationY, locationZ, normalX, normalY, normalZ,
                                     velocityX, velocityY, velocityZ, is_wet);
  PyObject *obj = PyObject_CallObject((PyObject *) &ProbeInfoType, argsList);
  Py_DECREF(argsList);
  return (PyObject*)obj;
}

/* Plugininfo Type */
typedef struct {
  PyObject_HEAD
  PyObject *name;
  PyObject *filePath;
  PyObject *signature;
  PyObject *description;
} PluginInfoObject;

static PyObject *
PluginInfo_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
  (void) args;
  (void) kwds;
  PluginInfoObject *self;
  self = (PluginInfoObject *) type->tp_alloc(type, 0);
  if (self != NULL) {
    self->filePath = PyUnicode_FromString("");
    if (self->filePath == NULL) {
      Py_DECREF(self);
      return NULL;
    }
    self->signature = PyUnicode_FromString("");
    if (self->signature == NULL) {
      Py_DECREF(self);
      return NULL;
    }
    self->name = PyUnicode_FromString("");
    if (self->name == NULL) {
      Py_DECREF(self);
      return NULL;
    }
    self->description = PyUnicode_FromString("");
    if (self->description == NULL) {
      Py_DECREF(self);
      return NULL;
    }
  }
  return (PyObject *) self;
}

static int
PluginInfo_traverse(PluginInfoObject *self, visitproc visit, void *arg)
{
  Py_VISIT(self->description);
  Py_VISIT(self->name);
  Py_VISIT(self->filePath);
  Py_VISIT(self->signature);
  return 0;
}

static int
PluginInfo_clear(PluginInfoObject *self)
{
  Py_CLEAR(self->description);
  Py_CLEAR(self->name);
  Py_CLEAR(self->filePath);
  Py_CLEAR(self->signature);
  return 0;
}
    
static void
PluginInfo_dealloc(PluginInfoObject *self)
{
  PyObject_GC_UnTrack(self);
  PluginInfo_clear(self);
  Py_TYPE(self)->tp_free((PyObject *) self);
}

static int
PluginInfo_init(PluginInfoObject *self, PyObject *args, PyObject *kwds)
{
  static char *kwlist[] = {"name", "filePath", "signature", "description", NULL};
  PyObject *description = NULL, *name = NULL, *filePath = NULL, *signature = NULL, *tmp;
  if (!PyArg_ParseTupleAndKeywords(args, kwds, "|UUUU", kwlist,
                                   &name, &filePath, &signature, &description))
    return -1;
  if (description) {
    tmp = self->description;
    Py_INCREF(description);
    self->description = description;
    Py_XDECREF(tmp);
  }
  if (name) {
    tmp = self->name;
    Py_INCREF(name);
    self->name = name;
    Py_XDECREF(tmp);
  }
  if (signature) {
    tmp = self->signature;
    Py_INCREF(signature);
    self->signature = signature;
    Py_XDECREF(tmp);
  }
  if (filePath) {
    tmp = self->filePath;
    Py_INCREF(filePath);
    self->filePath = filePath;
    Py_XDECREF(tmp);
  }
  return 0;
}

static PyMemberDef PluginInfo_members[] = {
    {"name", T_OBJECT_EX, offsetof(PluginInfoObject, name), 0, "name"},
    {"filePath", T_OBJECT_EX, offsetof(PluginInfoObject, filePath), 0, "filePath"},
    {"signature", T_OBJECT_EX, offsetof(PluginInfoObject, signature), 0, "signature"},
    {"description", T_OBJECT_EX, offsetof(PluginInfoObject, description), 0, "Description"},
    {NULL}  /* Sentinel */
};

static PyTypeObject PluginInfoType = {
                                      PyVarObject_HEAD_INIT(NULL, 0)
                                      .tp_name = "xppython3.PluginInfo",
                                      .tp_doc = "PluginInfo",
                                      .tp_basicsize = sizeof(PluginInfoObject),
                                      .tp_itemsize = 0,
                                      .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_GC,
                                      .tp_new = PluginInfo_new,
                                      .tp_init = (initproc) PluginInfo_init,
                                      .tp_dealloc = (destructor) PluginInfo_dealloc,
                                      .tp_traverse = (traverseproc) PluginInfo_traverse,
                                      .tp_clear = (inquiry) PluginInfo_clear,
                                      .tp_members = PluginInfo_members,

};


PyObject *
PyPluginInfo_New(char *name, char *filePath, char *signature, char *description)
{
  PyObject *argsList = Py_BuildValue("ssss", name, filePath, signature, description);
  PyObject *obj = PyObject_CallObject((PyObject *) &PluginInfoType, argsList);
  Py_DECREF(argsList);
  return (PyObject*)obj;
}

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
  static char *kwlist[] = {"type", "latitude", "longitude", "height", "frequency", "heading", "navaAidID", "name", "reg", NULL};
  PyObject *navAidID = NULL, *name, *tmp;
  if (!PyArg_ParseTupleAndKeywords(args, kwds, "|ifffifUUi", kwlist,
                                   &self->type, &self->latitude, &self->longitude, &self->height, &self->frequency, &self->heading,
                                   &navAidID, &name, &self->reg))
    return -1;
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
    {"type", T_INT, offsetof(NavAidInfoObject, type), 0, "XPLMNavType"},
    {"latitude", T_FLOAT, offsetof(NavAidInfoObject, latitude), 0, "latitude"},
    {"longitude", T_FLOAT, offsetof(NavAidInfoObject, longitude), 0, "longitude"},
    {"height", T_FLOAT, offsetof(NavAidInfoObject, height), 0, "height"},
    {"frequency", T_INT, offsetof(NavAidInfoObject, frequency), 0, "frequency"},
    {"heading", T_FLOAT, offsetof(NavAidInfoObject, heading), 0, "heading"},
    {"navAidID", T_OBJECT_EX, offsetof(NavAidInfoObject, navAidID), 0, "nav aid ID"},
    {"name", T_OBJECT, offsetof(NavAidInfoObject, name), 0, "nav aid name"},
    {"reg", T_INT, offsetof(NavAidInfoObject, reg), 0, "navaid is within local 'region'"},
    {NULL}  /* Sentinel */
};

static PyTypeObject NavAidInfoType = {
                                      PyVarObject_HEAD_INIT(NULL, 0)
                                      .tp_name = "xppython3.NavAidInfo",
                                      .tp_doc = "NavAidInfo",
                                      .tp_basicsize = sizeof(NavAidInfoObject),
                                      .tp_itemsize = 0,
                                      .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_GC,
                                      .tp_new = NavAidInfo_new,
                                      .tp_init = (initproc) NavAidInfo_init,
                                      .tp_dealloc = (destructor) NavAidInfo_dealloc,
                                      .tp_traverse = (traverseproc) NavAidInfo_traverse,
                                      .tp_clear = (inquiry) NavAidInfo_clear,
                                      .tp_members = NavAidInfo_members,

};


PyObject *
PyNavAidInfo_New(int type, float latitude, float longitude, float height, int frequency, float heading, char* navAidID, char *name, int reg)
{
  PyObject *argsList = Py_BuildValue("ifffifUUi", type, latitude, longitude, height, frequency, heading, navAidID, name, reg);
  PyObject *obj = PyObject_CallObject((PyObject *) &NavAidInfoType, argsList);
  Py_DECREF(argsList);
  return (PyObject*)obj;
}

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
    {"type", T_INT, offsetof(FMSEntryInfoObject, type), 0, "XPLMNavType"},
    {"navAidID", T_OBJECT_EX, offsetof(FMSEntryInfoObject, navAidID), 0, "nav aid ID"},
    {"ref", T_INT, offsetof(FMSEntryInfoObject, ref), 0, "XPLMNavref"},
    {"altitude", T_INT, offsetof(FMSEntryInfoObject, altitude), 0, "altitude"},
    {"lat", T_FLOAT, offsetof(FMSEntryInfoObject, lat), 0, "latitude"},
    {"lon", T_FLOAT, offsetof(FMSEntryInfoObject, lon), 0, "longitude"},
    {NULL}  /* Sentinel */
};

static PyTypeObject FMSEntryInfoType = {
                                      PyVarObject_HEAD_INIT(NULL, 0)
                                      .tp_name = "xppython3.FMSEntryInfo",
                                      .tp_doc = "FMSEntryInfo",
                                      .tp_basicsize = sizeof(FMSEntryInfoObject),
                                      .tp_itemsize = 0,
                                      .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_GC,
                                      .tp_new = FMSEntryInfo_new,
                                      .tp_init = (initproc) FMSEntryInfo_init,
                                      .tp_dealloc = (destructor) FMSEntryInfo_dealloc,
                                      .tp_traverse = (traverseproc) FMSEntryInfo_traverse,
                                      .tp_clear = (inquiry) FMSEntryInfo_clear,
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

/* TrackMetrics TYPE */
typedef struct {
  PyObject_HEAD
  int isVertical;
  int downBtnSize;
  int downPageSize;
  int thumbSize;
  int upPageSize;
  int upBtnSize;
} TrackMetricsObject;

static PyObject *
TrackMetrics_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
  (void) args;
  (void) kwds;
  TrackMetricsObject *self;
  self = (TrackMetricsObject *) type->tp_alloc(type, 0);
  if (self != NULL) {
    self->isVertical = 0;
    self->downBtnSize = 0;
    self->downPageSize = 0;
    self->thumbSize = 0;
    self->upPageSize = 0;
    self->upBtnSize = 0;
  }
  return (PyObject *) self;
}

static int
TrackMetrics_traverse(TrackMetricsObject *self, visitproc visit, void *arg)
{
  (void) self;
  (void) visit;
  (void) arg;
  return 0;
}

static int
TrackMetrics_clear(TrackMetricsObject *self)
{
  (void)self;
  return 0;
}

static void
TrackMetrics_dealloc(TrackMetricsObject *self)
{
  Py_TYPE(self)->tp_free((PyObject *) self);
}

static int
TrackMetrics_init(TrackMetricsObject *self, PyObject *args, PyObject *kwds)
{
  static char *kwlist[] = {"isVertical", "downBtnSize", "downPageSize", "thumbSize", "upPageSize", "upBtnSize", NULL};
  if (!PyArg_ParseTupleAndKeywords(args, kwds, "|iiiiii", kwlist,
                                   &self->isVertical, &self->downBtnSize, &self->downPageSize, &self->thumbSize,
                                   &self->upPageSize, &self->upBtnSize))
    return -1;
  return 0;
}

static PyMemberDef TrackMetrics_members[] = {
    {"isVertical", T_INT, offsetof(TrackMetricsObject, isVertical), 0, "isVertical"},
    {"downBtnSize", T_INT, offsetof(TrackMetricsObject, downBtnSize), 0, "downButtonSize"},
    {"downPageSize", T_INT, offsetof(TrackMetricsObject, downPageSize), 0, "downPageSize"},
    {"thumbSize", T_INT, offsetof(TrackMetricsObject, thumbSize), 0, "thumbSize"},
    {"upPageSize", T_INT, offsetof(TrackMetricsObject, upPageSize), 0, "upPageSize"},
    {"upBtnSize", T_INT, offsetof(TrackMetricsObject, upBtnSize), 0, "upButtonSize"},
    {NULL}  /* Sentinel */
};

static PyTypeObject TrackMetricsType = {
                                      PyVarObject_HEAD_INIT(NULL, 0)
                                      .tp_name = "xppython3.TrackMetrics",
                                      .tp_doc = "TrackMetrics",
                                      .tp_basicsize = sizeof(TrackMetricsObject),
                                      .tp_itemsize = 0,
                                      .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_GC,
                                      .tp_new = TrackMetrics_new,
                                      .tp_init = (initproc) TrackMetrics_init,
                                      .tp_dealloc = (destructor) TrackMetrics_dealloc,
                                      .tp_traverse = (traverseproc) TrackMetrics_traverse,
                                      .tp_clear = (inquiry) TrackMetrics_clear,
                                      .tp_members = TrackMetrics_members,
};


PyObject *
PyTrackMetrics_New(int isVertical, int downBtnSize, int downPageSize, int thumbSize, int upPageSize, int upBtnSize)
{
  PyObject *argsList = Py_BuildValue("iiiiii", isVertical, downBtnSize, downPageSize, thumbSize, upPageSize, upBtnSize);
  PyObject *obj = PyObject_CallObject((PyObject *) &TrackMetricsType, argsList);
  Py_DECREF(argsList);
  return (PyObject*)obj;
}



static PyObject *XPPythonGetDictsFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  Py_INCREF(xppythonDicts);
  return xppythonDicts;
}
static PyObject *XPPythonGetCapsulesFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  Py_INCREF(xppythonCapsules);
  return xppythonCapsules;
}
/*      "cam":            one for each camera controller */
/*        key:  idx, */
/*        val: tuple(<module_filename>,       "PI_Display.py" */
/*                   howLong, */
/*                   controlFunc, */
/*                   refcon */
/*                   ) */
/*      "module":         one for each loaded Python Plugin module */
/*        key: tuple(<name>, <sig>, <description>, <module_name>)  ("Display regression test", "XPPython3.Display", "This is ...", "PI_Display") */
/*        val: <PythonInterface object at 0x...> */
  
/*      "fl":   one for each registered flight loop */
/*        key: idx, */
/*        val: tuple(<module_filename>,       "PI_Display.py" */
/*                   <bound method <> of <PythonInterface object at 0x...>>, */
/*                   <interval>,              -1.0 */
/*                   <referenceConstant>      "my flight loop" */
/*                  ) */
/*      "flRev": */
/*        key: tuple(<module_filename>,       "PI_Display.py" */
/*                   <bound method <> of <PythonInterface object at 0x...>>, */
/*                   <slot>                   24704555319,  (the reference constant address) */
/*        val: idx into callbackInfo          1 */
  
/*      "keySniffCallback": one for each registered key sniffer */
/*        key: idx, */
/*        value: tuple(<module_filename>,     "PI_Display.py" */
/*                     <bound method <> of PythonInterface object at 0x...>>, */
/*                     <beforeWindows>        1, */
/*                     <referenceConstant>    ["booya, ] */
  
/*       "window": one for each registered window */
/*         key: windowID,                      14055678383, */
/*         val: tuple(<bound method <drawWindowFunc>       of PythonInterface object at 0x...>>, */
/*                    <bound method <handleMouseClickFunc> of PythonInterface object at 0x...>>, */
/*                    <bound method <handleKeyFunc>        of PythonInterface object at 0x...>>, */
/*                    <bound method <handleCursorFunc>     of PythonInterface object at 0x...>>, */
/*                    <bound method <handleMouseWheelFunc> of PythonInterface object at 0x...>>, */
/*                    <bound method <handleRightClickFunc> of PythonInterface object at 0x...>>, */
/*   *\/ */

/*   PyObject *ret = PyDict_New(); */
/*   if(PyDict_Check(widgetCallbackDict)) { */
/*     PyDict_SetItemString(ret, "widgetCallback", PyDict_Copy(widgetCallbackDict)); //widgets */
/*   } else { */
/*     PyDict_SetItemString(ret, "widgetCallback", PyDict_New()); */
/*   } */
  
/*   if(PyDict_Check(moduleDict)) { */
/*     PyDict_SetItemString(ret, "module", PyDict_Copy(moduleDict)); //plugins */
/*   } else { */
/*     PyDict_SetItemString(ret, "module", PyDict_New()); */
/*   } */
/*   if(feDict && PyDict_Check(feDict)){ */
/*     PyDict_SetItemString(ret, "fe", PyDict_Copy(feDict)); // plugins */
/*   } else { */
/*     PyDict_SetItemString(ret, "fe", PyDict_New()); */
/*   } */
/*   if(windowDict && PyDict_Check(windowDict)){ */
/*     PyDict_SetItemString(ret, "window", PyDict_Copy(windowDict)); // display */
/*   } else { */
/*     PyDict_SetItemString(ret, "window", PyDict_New()); */
/*   } */
/*   if(hotkeyDict && PyDict_Check(hotkeyDict)){ */
/*     PyDict_SetItemString(ret, "hotkey", PyDict_Copy(hotkeyDict)); // display */
/*   } else { */
/*     PyDict_SetItemString(ret, "hotkey", PyDict_New()); */
/*   } */
/*   if(hotkeyIDDict && PyDict_Check(hotkeyIDDict)){ */
/*     PyDict_SetItemString(ret, "hotkeyID", PyDict_Copy(hotkeyIDDict)); // display */
/*   } else { */
/*     PyDict_SetItemString(ret, "hotkeyID", PyDict_New()); */
/*   } */
/*   if(drawCallbackDict && PyDict_Check(drawCallbackDict)){ */
/*     PyDict_SetItemString(ret, "drawCallback", PyDict_Copy(drawCallbackDict)); // display */
/*   } else { */
/*     PyDict_SetItemString(ret, "drawCallback", PyDict_New()); */
/*   } */
/*   if(drawCallbackIDDict && PyDict_Check(drawCallbackIDDict)){ */
/*     PyDict_SetItemString(ret, "drawCallbackID", PyDict_Copy(drawCallbackIDDict)); // display */
/*   } else { */
/*     PyDict_SetItemString(ret, "drawCallbackID", PyDict_New()); */
/*   } */
/*   if(keySniffCallbackDict && PyDict_Check(keySniffCallbackDict)){ */
/*     PyDict_SetItemString(ret, "keySniffCallback", PyDict_Copy(keySniffCallbackDict)); // display */
/*   } else { */
/*     PyDict_SetItemString(ret, "keySniffCallback", PyDict_New()); */
/*   } */
/*   if(menuDict && PyDict_Check(menuDict)){ */
/*     PyDict_SetItemString(ret, "menuDict", PyDict_Copy(menuDict));  // menut */
/*   } else { */
/*     PyDict_SetItemString(ret, "menu", PyDict_New()); */
/*   } */
/*   if(menuRefDict && PyDict_Check(menuRefDict)){ */
/*     PyDict_SetItemString(ret, "menuRefDict", PyDict_Copy(menuRefDict)); //menu */
/*   } else { */
/*     PyDict_SetItemString(ret, "menuRef", PyDict_New()); */
/*   } */
/*   if(camDict && PyDict_Check(camDict)){ */
/*     PyDict_SetItemString(ret, "cam", PyDict_Copy(camDict)); //camera */
/*   } else { */
/*     PyDict_SetItemString(ret, "cam", PyDict_New()); */
/*   } */
/*   if(accessorDict && PyDict_Check(accessorDict)){ */
/*     PyDict_SetItemString(ret, "accessor", PyDict_Copy(accessorDict));  //data_access */
/*   } else { */
/*     PyDict_SetItemString(ret, "accessor", PyDict_New()); */
/*   } */
/*   if(drefDict && PyDict_Check(drefDict)){ */
/*     PyDict_SetItemString(ret, "dref", PyDict_Copy(drefDict)); // data_access */
/*   } else { */
/*     PyDict_SetItemString(ret, "dref", PyDict_New()); */
/*   } */
/*   if(sharedDict && PyDict_Check(sharedDict)){ */
/*     PyDict_SetItemString(ret, "shared", PyDict_Copy(sharedDict)); //data_access */
/*   } else { */
/*     PyDict_SetItemString(ret, "shared", PyDict_New()); */
/*   } */
/*   if(availableDict && PyDict_Check(availableDict)){ */
/*     PyDict_SetItemString(ret, "available", PyDict_Copy(availableDict)); //planes */
/*   } else { */
/*     PyDict_SetItemString(ret, "available", PyDict_New()); */
/*   } */
/*   if(flDict && PyDict_Check(flDict)){ */
/*     PyDict_SetItemString(ret, "fl", PyDict_Copy(flDict));  //processing */
/*   } else { */
/*     PyDict_SetItemString(ret, "fl", PyDict_New()); */
/*   } */
/*   if(flRevDict && PyDict_Check(flRevDict)){ */
/*     PyDict_SetItemString(ret, "flRev", PyDict_Copy(flRevDict));//processing */
/*   } else { */
/*     PyDict_SetItemString(ret, "flRev", PyDict_New()); */
/*   } */
/*   if(flIDDict && PyDict_Check(flIDDict)){ */
/*     PyDict_SetItemString(ret, "flID", PyDict_Copy(flIDDict));//procesing */
/*   } else { */
/*     PyDict_SetItemString(ret, "flID", PyDict_New()); */
/*   } */
/*   if(loaderDict && PyDict_Check(loaderDict)){ */
/*     PyDict_SetItemString(ret, "loader", PyDict_Copy(loaderDict)); // scenery */
/*   } else { */
/*     PyDict_SetItemString(ret, "loader", PyDict_New()); */
/*   } */
/*   if(libEnumDict && PyDict_Check(libEnumDict)){ */
/*     PyDict_SetItemString(ret, "libEnum", PyDict_Copy(libEnumDict)); // scenery */
/*   } else { */
/*     PyDict_SetItemString(ret, "libEnum", PyDict_New()); */
/*   } */
/*   return ret; */
/* } */


static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  PyDict_Clear(xppythonDicts);
  Py_DECREF(xppythonDicts);
  PyDict_Clear(xppythonCapsules);
  Py_DECREF(xppythonCapsules);
  Py_RETURN_NONE;
}

static PyMethodDef XPPythonMethods[] = {
  {"XPPythonGetDicts", XPPythonGetDictsFun, METH_VARARGS, ""},
  {"XPPythonGetCapsules", XPPythonGetCapsulesFun, METH_VARARGS, ""},
  {"cleanup", cleanup, METH_VARARGS, ""},
  {NULL, NULL, 0, NULL}
};

static struct PyModuleDef XPPythonModule = {
  PyModuleDef_HEAD_INIT,
  "XPPython",
  NULL,
  -1,
  XPPythonMethods,
  NULL,
  NULL,
  NULL,
  NULL
};

PyMODINIT_FUNC
PyInit_XPPython(void)
{
  if (PyType_Ready(&HotKeyInfoType) < 0)
    return NULL;
  if (PyType_Ready(&ProbeInfoType) < 0)
    return NULL;
  if (PyType_Ready(&PluginInfoType) < 0)
    return NULL;
  if (PyType_Ready(&TrackMetricsType) < 0)
    return NULL;
  if (PyType_Ready(&NavAidInfoType) < 0)
    return NULL;
  if (PyType_Ready(&FMSEntryInfoType) < 0)
    return NULL;

  xppythonDicts = PyDict_New();
  Py_INCREF(xppythonDicts);
  xppythonCapsules = PyDict_New();
  Py_INCREF(xppythonCapsules);
  PyObject *mod = PyModule_Create(&XPPythonModule);
  if (mod != NULL) {
    PyModule_AddStringConstant(mod, "VERSION", pythonPluginVersion);
    PyModule_AddStringConstant(mod, "PLUGINSPATH", pythonPluginsPath);
    PyModule_AddStringConstant(mod, "INTERNALPLUGINSPATH", pythonInternalPluginsPath);
    PyModule_AddObject(mod, "HotKeyInfo", (PyObject *) &HotKeyInfoType);
    PyModule_AddObject(mod, "ProbeInfo", (PyObject *) &ProbeInfoType);
    PyModule_AddObject(mod, "PluginInfo", (PyObject *) &PluginInfoType);
    PyModule_AddObject(mod, "NavAidInfo", (PyObject *) &NavAidInfoType);
    PyModule_AddObject(mod, "FMSEntryInfo", (PyObject *) &FMSEntryInfoType);
    PyModule_AddObject(mod, "TrackMetrics", (PyObject *) &TrackMetricsType);
  }
  Py_INCREF(&HotKeyInfoType);
  Py_INCREF(&ProbeInfoType);
  Py_INCREF(&PluginInfoType);
  Py_INCREF(&NavAidInfoType);
  Py_INCREF(&FMSEntryInfoType);
  Py_INCREF(&TrackMetricsType);

  return mod;
}
