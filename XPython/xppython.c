#define _GNU_SOURCE 1
#include <Python.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdbool.h>
#include <structmember.h>
#include "xppythontypes.h"
#include <XPLM/XPLMDataAccess.h>
#include <XPLM/XPLMUtilities.h>
#include <XPLM/XPLMNavigation.h>
#include "utils.h"
#include "trackMetrics.h"
#include "xppython.h"
#include "manage_instance.h"

PyObject *xppythonDicts = NULL, *xppythonCapsules = NULL;
PyObject *PythonModuleMTimes = NULL;
extern const char *pythonPluginVersion, *pythonPluginsPath, *pythonInternalPluginsPath;
int pythonFlushLog = 0;
static PyObject *getExecutable(void);

PluginStats pluginStats[512];
static int numPlugins = 0;


int getPluginIndex(PyObject *pluginInstance)
{
  /* add check for 'max plugins', so we don't die on multiple reloads with lots of python plugins? */
  if (numPlugins == 0) {
    pluginStats[numPlugins].pluginInstance = Py_None; /* slot zero is for 'all' */
    pluginStats[numPlugins].fl_time = pluginStats[numPlugins].customw_time = pluginStats[numPlugins].draw_time = 0;
    numPlugins++;
  }
  for (int i = 0; i < numPlugins; i++) {
    if(pluginStats[i].pluginInstance == pluginInstance) {
      return i;
    }
  }
  pluginStats[numPlugins].pluginInstance = pluginInstance;
  pluginStats[numPlugins].fl_time = pluginStats[numPlugins].customw_time = pluginStats[numPlugins].draw_time = 0;
  return numPlugins++;
}

PyObject *getExecutable()
/* get value for executable to be 'python' rather than 'X-Plane'.
   This will enable subprocess and multiprocessing to work as
   anticipated
*/
{
  PyObject *exec_prefix = PySys_GetObject("exec_prefix");
#if LIN || APL
  PyObject *executable = PyUnicode_FromFormat("%S/bin/python" PYTHONVERSION, exec_prefix);
#endif
#if IBM
  PyObject *executable = PyUnicode_FromFormat("%S\\pythonw.exe", exec_prefix);
#endif
  return executable;
}


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
  {"virtualKey", T_INT, offsetof(HotKeyInfoObject, virtualKey), 0, "Virtual Key"},
  {"flags", T_INT, offsetof(HotKeyInfoObject, flags), 0, "XPLMKeyFlags"},
  {"description", T_OBJECT_EX, offsetof(HotKeyInfoObject, description), 0, "Description"},
  {"plugin", T_INT, offsetof(HotKeyInfoObject, plugin), 0, "XPLMPluginID"},
  {NULL, T_INT, 0, 0, ""}  /* Sentinel */
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

/* DataRefInfo Type */
typedef struct {
  PyObject_HEAD
  PyObject *name;
  XPLMDataTypeID type;
  PyObject *writable;
  XPLMPluginID owner;
} DataRefInfoObject;

static PyObject *
DataRefInfo_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
  (void) args;
  (void) kwds;
  DataRefInfoObject *self;
  self = (DataRefInfoObject *) type->tp_alloc(type, 0);
  if (self != NULL) {
    self->name = PyUnicode_FromString("");
    if (self->name == NULL) {
      Py_DECREF(self);
      return NULL;
    }
  }
  return (PyObject *) self;
}

static int
DataRefInfo_traverse(DataRefInfoObject *self, visitproc visit, void *arg)
{
  (void) visit;
  (void) arg;
  Py_VISIT(self->name);  // visit python objects
  Py_VISIT(self->writable);  // visit python objects
  return 0;
}
static int
DataRefInfo_clear(DataRefInfoObject *self)
{
  (void) self;
  Py_CLEAR(self->name); // clear python objects
  Py_CLEAR(self->writable);
  return 0;
}
    
static void
DataRefInfo_dealloc(DataRefInfoObject *self)
{
  PyObject_GC_UnTrack(self);
  DataRefInfo_clear(self);
  Py_TYPE(self)->tp_free((PyObject *) self);
}

static int
DataRefInfo_init(DataRefInfoObject *self, PyObject *args, PyObject *kwds)
{
  static char *kwlist[] = {"name", "type", "writable", "owner", NULL};
  PyObject *name = NULL, *writable = NULL;
  if (!PyArg_ParseTupleAndKeywords(args, kwds, "|UiOi", kwlist,
                                   &name, &self->type, &writable, &self->owner))
    return -1;
  PyObject *tmp;
  if (name) {
    tmp = self->name;
    Py_INCREF(name);
    self->name = name;
    Py_XDECREF(tmp);
  }
  if (writable) {
    tmp = self->writable;
    self->writable = writable ? Py_True : Py_False;
    Py_INCREF(self->writable);
    Py_XDECREF(tmp);
  }
  return 0;
}

static PyMemberDef DataRefInfo_members[] = {
  {"name", T_OBJECT_EX, offsetof(DataRefInfoObject, name), 0, "name"},
  {"type", T_INT, offsetof(DataRefInfoObject, type), 0, "type"},
  {"writable", T_OBJECT_EX, offsetof(DataRefInfoObject, writable), 0, "writable"},
  {"owner", T_INT, offsetof(DataRefInfoObject, owner), 0, "owner"},
  {NULL, T_INT, 0, 0, ""}  /* Sentinel */
};

static PyObject *DataRefInfo_str(DataRefInfoObject *obj) {
  return PyUnicode_FromFormat("%S: 0x%x %s [%i]",
                              obj->name,
                              obj->type, 
                              obj->writable == Py_True ? "writable" : "read-only",
                              obj->owner);
}

static PyTypeObject DataRefInfoType = {
  PyVarObject_HEAD_INIT(NULL, 0)
  .tp_name = "xppython3.DataRefInfo",
  .tp_doc = "DataRefInfo",
  .tp_basicsize = sizeof(DataRefInfoObject),
  .tp_itemsize = 0,
  .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_GC,
  .tp_new = DataRefInfo_new,
  .tp_init = (initproc) DataRefInfo_init,
  .tp_dealloc = (destructor) DataRefInfo_dealloc,
  .tp_traverse = (traverseproc) DataRefInfo_traverse,
  .tp_clear = (inquiry) DataRefInfo_clear,
  .tp_str = (reprfunc) DataRefInfo_str,
  .tp_members = DataRefInfo_members,
};

PyObject *
PyDataRefInfo_New(const char *name, int type, int writable, int owner)
{
  PyObject *argsList = Py_BuildValue("siii", name, type, writable, owner);
  PyObject *obj = PyObject_CallObject((PyObject *) &DataRefInfoType, argsList);
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
    {"result", T_INT, offsetof(ProbeInfoObject, result), 0, "XPLMProbeResult, result of query"},
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
    {NULL, T_INT, 0, 0, ""}  /* Sentinel */
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
    {NULL, T_INT, 0, 0, ""}  /* Sentinel */
};

static PyObject *PluginInfo_str(PluginInfoObject *obj) {
  return PyUnicode_FromFormat("%S: '%S'\n  %S\n  ---\n  %S",
                              obj->name,
                              obj->signature,
                              obj->filePath,
                              obj->description);
}

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
                                      .tp_str = (reprfunc) PluginInfo_str,
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
    {"height", T_FLOAT, offsetof(NavAidInfoObject, height), 0, "height (meters)"},
    {"frequency", T_INT, offsetof(NavAidInfoObject, frequency), 0, "frequency (x100, expect for NDB)"},
    {"heading", T_FLOAT, offsetof(NavAidInfoObject, heading), 0, "heading, for GlideSlope value is localizer bearing (true) + GS angle x 100,000\n"
     "e.g., GS of 3.00 degrees on heading 122.53125 becomes 300122.53125"},
    {"navAidID", T_OBJECT_EX, offsetof(NavAidInfoObject, navAidID), 0, "navaid ID"},
    {"name", T_OBJECT, offsetof(NavAidInfoObject, name), 0, "navaid name"},
    {"reg", T_INT, offsetof(NavAidInfoObject, reg), 0, "1 if navaid is within local 'region', 0 otherwise"},
    {NULL, T_INT, 0, 0, ""}  /* Sentinel */
};

static PyObject *NavAidInfo_str(NavAidInfoObject *obj) {
  /* Name (<ID>), type, (lat, lon), frequncy*/
  char *navAidType;
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
  case xplm_Nav_Unknown:
    navAidType = "Unknown"; break;
  default:
    navAidType = "Other Unknown";
  }
  
  if (-1 == asprintf_ret) {
    fprintf(pythonLogFile, "Failed to allocate asprintf memory. Failing navaid info.\n");
  }

  PyObject *ret = PyUnicode_FromFormat("%S (%S) %s %s",
                                       obj->name,
                                       obj->navAidID,
                                       navAidType,
                                       floats);
  free(floats);
  return ret;
                              
}

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
                                      .tp_str = (reprfunc) NavAidInfo_str,
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
    {"type", T_INT, offsetof(FMSEntryInfoObject, type), 0, "XPLMNavType or NAV_NOT_FOUND if LatLon"},
    {"navAidID", T_OBJECT_EX, offsetof(FMSEntryInfoObject, navAidID), 0, "navaid ID or None, if LatLon"},
    {"ref", T_INT, offsetof(FMSEntryInfoObject, ref), 0, "XPLMNavref or None"},
    {"altitude", T_INT, offsetof(FMSEntryInfoObject, altitude), 0, "altitude (in feet)"},
    {"lat", T_FLOAT, offsetof(FMSEntryInfoObject, lat), 0, "latitude"},
    {"lon", T_FLOAT, offsetof(FMSEntryInfoObject, lon), 0, "longitude"},
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

My_DOCSTR(_reloadPlugin__doc__, "reloadPlugin", "signature",
          "Reload (python) plugin with provided signature\n"
          "\n"
          "Plugin will be disabled, stopped, reloaded, then\n"
          "started and enabled.");
static PyObject *XPReloadPlugin(PyObject *self, PyObject *args, PyObject *kwargs)
{
  (void) self;
  static char *kwlist[] = {"signature", NULL};
  PyObject *signature;
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O", kwlist, &signature)) {
    return NULL;
  }
  xpy_reloadInstance(signature);
  Py_RETURN_NONE;
}


My_DOCSTR(_getPluginStats__doc__, "getPluginStats", "",
          "Return dictionary of python plugin performance statistics\n"
          "\n"
          "Keys are different python plugins, with key=None being\n"
          "overall performance of XPPython3 plugin.");
static PyObject *XPGetPluginStats(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  PyObject *info = PyDict_New();
  
  PyObject *fl_string = PyUnicode_FromString("fl");
  PyObject *customw_string = PyUnicode_FromString("customw");
  PyObject *draw_string = PyUnicode_FromString("draw");
  PyObject *py_fltime, *py_drawtime, *py_customwtime;

  for(int i=0; i< numPlugins; i++) {
    PyObject *plugin_info = PyDict_New();

    py_fltime = PyLong_FromLong(pluginStats[i].fl_time);
    py_drawtime = PyLong_FromLong(pluginStats[i].draw_time);
    py_customwtime = PyLong_FromLong(pluginStats[i].customw_time);

    PyDict_SetItem(plugin_info, fl_string, py_fltime);
    PyDict_SetItem(plugin_info, draw_string, py_drawtime);
    PyDict_SetItem(plugin_info, customw_string, py_customwtime);
    Py_DECREF(py_fltime);
    Py_DECREF(py_drawtime);
    Py_DECREF(py_customwtime);

    PyDict_SetItem(info, pluginStats[i].pluginInstance, plugin_info);
    
    Py_DECREF(plugin_info);
    pluginStats[i].fl_time = pluginStats[i].customw_time = pluginStats[i].draw_time = 0;
  }

  Py_DECREF(fl_string);
  Py_DECREF(draw_string);
  Py_DECREF(customw_string);

  Py_INCREF(info);
  return info;
}

My_DOCSTR(_pythonGetDicts__doc__, "pythonGetDicts", "",
          "Return dictionary of internal xppython3 dictionaries\n"
          "\n"
          "See documentation, intended for debugging only");
static PyObject *XPPythonGetDictsFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  Py_INCREF(xppythonDicts);
  return xppythonDicts;
}

My_DOCSTR(_pythonLog__doc__, "log", "",
          "Log string to XPPython3log.txt file. Flush buffer, if no string is provided.");
static PyObject *XPPythonLogFun(PyObject *self, PyObject *args)
{
  (void) self;
  const char *inString;
  int flush = 0;
  if(!PyArg_ParseTuple(args, "s", &inString)) {
    /* don't bother sending error */
    PyErr_Clear();
    flush = 1;
  } else {
    if (strlen(inString)) {
      char *moduleName = get_moduleName();
      fprintf(pythonLogFile, "[%s] %s\n", moduleName, inString);
      free(moduleName);
    } else {
      flush = 1;
    }
  }
  if (flush || pythonFlushLog) {
      fflush(pythonLogFile);
  }
  Py_RETURN_NONE;
}

My_DOCSTR(_pythonSystemLog__doc__, "systemLog", "",
          "Log string to system log file, Log.txt, with newline appended, flushing buffer.");
static PyObject *XPSystemLogFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  const char *inString;
  if(!PyArg_ParseTuple(args, "s", &inString)) {
    /* don't bother sending error */
    PyErr_Clear();
  } else {
    if (strlen(inString)) {
      char *moduleName = get_moduleName();
      char *msg;
      if (-1 == asprintf(&msg, "[XP3: %s] %s\n", moduleName, inString)) {
        fprintf(pythonLogFile, "Failed to allocate memory for asprintf syslog.\n");
      }
      free(moduleName);
      XPLMDebugString(msg);
      free(msg);
    } else {
      /* DebugString already, always flushes, so ignore empty prints */
    }
  }
  Py_RETURN_NONE;
}

My_DOCSTR(_pythonGetCapsules__doc__, "pythonGetCapsules", "",
          "Returns internal dictionary of Capsules\n"
          "\n"
          "Intended for debugging only");
static PyObject *XPPythonGetCapsulesFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  Py_INCREF(xppythonCapsules);
  return xppythonCapsules;
}

My_DOCSTR(_derefCapsule__doc__, "derefCapsule", "capsule_type, capsule",
          "Dereference a capsule to retrieve internal C language pointer\n"
          "\n"
          "Intended for debugging only");

static PyObject *XPPythonDerefCapsuleFun(PyObject *self, PyObject *args)
{
  (void) self;
  const char *capsule_type;
  PyObject *capsule;
  if(!PyArg_ParseTuple(args, "sO", &capsule_type, &capsule)) {
    return NULL;
  }
  /* fprintf(pythonLogFile, "Capsule Name: %s\n", PyCapsule_GetName(capsule)); */
  /* fflush(pythonLogFile); */
  /* fprintf(pythonLogFile, "Capsule Context: %p\n", PyCapsule_GetContext(capsule)); */
  /* fflush(pythonLogFile); */
  /* fprintf(pythonLogFile, "Capsule Pointer: %p\n", PyCapsule_GetPointer(capsule, capsule_type)); */
  /* fflush(pythonLogFile); */

  return PyLong_FromVoidPtr(PyCapsule_GetPointer(capsule, capsule_type));
}

static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  PyDict_Clear(xppythonCapsules);
  Py_DECREF(xppythonCapsules);
  Py_RETURN_NONE;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
static PyMethodDef XPPythonMethods[] = {
  {"pythonGetDicts", (PyCFunction)XPPythonGetDictsFun, METH_VARARGS, _pythonGetDicts__doc__},
  {"XPPythonGetDicts", (PyCFunction)XPPythonGetDictsFun, METH_VARARGS, ""},
  {"pythonGetCapsules", (PyCFunction)XPPythonGetCapsulesFun, METH_VARARGS, _pythonGetCapsules__doc__},
  {"XPPythonGetCapsules", (PyCFunction)XPPythonGetCapsulesFun, METH_VARARGS, ""},
  {"derefCapsule", (PyCFunction)XPPythonDerefCapsuleFun, METH_VARARGS, _derefCapsule__doc__},
  {"XPPythonDerefCapsule", (PyCFunction)XPPythonDerefCapsuleFun, METH_VARARGS, ""},
  {"log", (PyCFunction)XPPythonLogFun, METH_VARARGS, _pythonLog__doc__},
  {"XPPythonLog", (PyCFunction)XPPythonLogFun, METH_VARARGS, ""},
  {"XPSystemLog", XPSystemLogFun, METH_VARARGS, ""},
  {"systemLog", XPSystemLogFun, METH_VARARGS, _pythonSystemLog__doc__},
  {"sys_log", XPSystemLogFun, METH_VARARGS, _pythonSystemLog__doc__},
  {"getPluginStats", (PyCFunction)XPGetPluginStats, METH_VARARGS | METH_KEYWORDS, _getPluginStats__doc__},
  {"XPGetPluginStats", (PyCFunction)XPGetPluginStats, METH_VARARGS | METH_KEYWORDS, ""},
  {"reloadPlugin", (PyCFunction)XPReloadPlugin, METH_VARARGS | METH_KEYWORDS, _reloadPlugin__doc__},
  {"XPReloadPlugin", (PyCFunction)XPReloadPlugin, METH_VARARGS | METH_KEYWORDS, ""},
  {"_cleanup", cleanup, METH_VARARGS, ""},
  {NULL, NULL, 0, NULL}
};
#pragma GCC diagnostic pop


static struct PyModuleDef XPPythonModule = {
  PyModuleDef_HEAD_INIT,
  "XPPython",
  "XPPython3 documentation: \n"
  "   https://xppython3.rtfd.io/en/stable/development/modules/kpython.html",
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
  if (PyType_Ready(&DataRefInfoType) < 0)
    return NULL;
  if (PyType_Ready(&PluginInfoType) < 0)
    return NULL;
  if (PyType_Ready(&TrackMetricsType) < 0)
    return NULL;
  if (PyType_Ready(&NavAidInfoType) < 0)
    return NULL;
  if (PyType_Ready(&FMSEntryInfoType) < 0)
    return NULL;

  PyObject *mod = PyModule_Create(&XPPythonModule);
  PythonModuleMTimes = PyDict_New();

  if (mod != NULL) {
    PyModule_AddStringConstant(mod, "__author__", "Peter Buckner (xppython3@avnwx.com)");
    PyModule_AddStringConstant(mod, "VERSION", pythonPluginVersion);
    PyModule_AddStringConstant(mod, "PLUGINSPATH", pythonPluginsPath);
    PyModule_AddStringConstant(mod, "INTERNALPLUGINSPATH", pythonInternalPluginsPath);
    PyModule_AddIntConstant(mod, "pythonDebugLevel", pythonDebugs);
    PyModule_AddObject(mod, "pythonExecutable", getExecutable());
    PyModule_AddObject(mod, "HotKeyInfo", (PyObject *) &HotKeyInfoType);
    PyModule_AddObject(mod, "ProbeInfo", (PyObject *) &ProbeInfoType);
    PyModule_AddObject(mod, "DataRefInfo", (PyObject *) &DataRefInfoType);
    PyModule_AddObject(mod, "PluginInfo", (PyObject *) &PluginInfoType);
    PyModule_AddObject(mod, "NavAidInfo", (PyObject *) &NavAidInfoType);
    PyModule_AddObject(mod, "FMSEntryInfo", (PyObject *) &FMSEntryInfoType);
    PyModule_AddObject(mod, "TrackMetrics", (PyObject *) &TrackMetricsType);
    PyModule_AddObject(mod, "ModuleMTimes", PythonModuleMTimes);
  }
  Py_INCREF(&HotKeyInfoType);
  Py_INCREF(&ProbeInfoType);
  Py_INCREF(&DataRefInfoType);
  Py_INCREF(&PluginInfoType);
  Py_INCREF(&NavAidInfoType);
  Py_INCREF(&FMSEntryInfoType);
  Py_INCREF(&TrackMetricsType);

  return mod;
}
