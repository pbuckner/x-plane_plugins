#define _GNU_SOURCE 1
#include <Python.h>
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMDataAccess.h>
#include <structmember.h>
#include <vector>
#include <string>
#include "utils.h"
#include "cpp_utilities.hpp"
#include "xppythontypes.h"

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
  std::vector<std::string> params = {"name", "type", "writable", "owner"};
  char **keywords = stringVectorToCharArray(params);
  PyObject *name = NULL, *writable = NULL;
  if (!PyArg_ParseTupleAndKeywords(args, kwds, "|UiOi", keywords,
                                   &name, &self->type, &writable, &self->owner)) {
    freeCharArray(keywords, params.size());
    return -1;
  }
  freeCharArray(keywords, params.size());
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
  {"name", T_OBJECT_EX, offsetof(DataRefInfoObject, name), 0, "Name of dataRef"},
  {"type", T_INT, offsetof(DataRefInfoObject, type), 0, "DataRef type (bitwise 1=Int, 2=Float, 4=Double, 8=FloatArray, 16=IntArray, 32=Data"},
  {"writable", T_OBJECT_EX, offsetof(DataRefInfoObject, writable), 0, "DataRef is Writable"},
  {"owner", T_INT, offsetof(DataRefInfoObject, owner), 0, "PluginID of owner: 0 is X-Plane built-in"},
  {NULL, T_INT, 0, 0, ""}  /* Sentinel */
};

static PyObject *DataRefInfo_str(DataRefInfoObject *obj) {
  return PyUnicode_FromFormat("%S: 0x%x %s [%i]",
                              obj->name,
                              obj->type, 
                              obj->writable == Py_True ? "writable" : "read-only",
                              obj->owner);
}

PyTypeObject
DataRefInfoType = {
  PyVarObject_HEAD_INIT(NULL, 0)
  .tp_name = "XPPython.XPLMDataRefInfo_t",
  .tp_doc = PyDoc_STR("XPLMDataRefInfo_t, return from xp.getDataRefInfo()"),
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
  if (PyErr_Occurred() || ! argsList) {
    pythonLog("Failed to create DataRefInfo args list");
    Py_RETURN_NONE;
  }
  PyObject *obj = PyObject_CallObject((PyObject *) &DataRefInfoType, argsList);
  Py_XDECREF(argsList);
  return (PyObject*)obj;
}
