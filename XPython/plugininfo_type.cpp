#define _GNU_SOURCE 1
#include <Python.h>
#include <XPLM/XPLMDefs.h>
#include <structmember.h>
#include <vector>
#include <string>
#include "utils.h"
#include "xppythontypes.h"
#include "cpp_utilities.hpp"

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
  std::vector<std::string> params = {"name", "filePath", "signature", "description"};
  char **kwlist = stringVectorToCharArray(params);
  PyObject *description = NULL, *name = NULL, *filePath = NULL, *signature = NULL, *tmp;
  if (!PyArg_ParseTupleAndKeywords(args, kwds, "|UUUU", kwlist,
                                   &name, &filePath, &signature, &description)) {
    freeCharArray(kwlist, params.size());
    return -1;
  }
  freeCharArray(kwlist, params.size());
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
    {"name", T_OBJECT_EX, offsetof(PluginInfoObject, name), 0, "Plugin name"},
    {"filePath", T_OBJECT_EX, offsetof(PluginInfoObject, filePath), 0, "Full path to the plugin's .xpl file"},
    {"signature", T_OBJECT_EX, offsetof(PluginInfoObject, signature), 0, "Plugin signature"},
    {"description", T_OBJECT_EX, offsetof(PluginInfoObject, description), 0, "Description of the plugin"},
    {NULL, T_INT, 0, 0, ""}  /* Sentinel */
};

static PyObject *PluginInfo_str(PluginInfoObject *obj) {
  return PyUnicode_FromFormat("%S: '%S'\n  %S\n  ---\n  %S",
                              obj->name,
                              obj->signature,
                              obj->filePath,
                              obj->description);
}

PyTypeObject
PluginInfoType = {
  PyVarObject_HEAD_INIT(NULL, 0)
  .tp_name = "XPPython.PluginInfo",
  .tp_doc = PyDoc_STR("PluginInfo, return from xp.getPluginInfo()"),
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
  Py_XDECREF(argsList);
  return (PyObject*)obj;
}
