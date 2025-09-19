#define _GNU_SOURCE 1
#include <Python.h>
#include <structmember.h>
#include <vector>
#include <string>
#include "utils.h"
#include "xppythontypes.h"
#include "cpp_utilities.hpp"

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
  if (self != nullptr) {
    self->virtualKey = 0;
    self->flags = 0;
    self->description = PyUnicode_FromString("");
    if (self->description == nullptr) {
      Py_DECREF(self);
      return nullptr;
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
  std::vector<std::string> params = {"virtualKey", "flags", "description", "plugin"};
  char **kwlist = stringVectorToCharArray(params);
  PyObject *description = nullptr, *tmp;
  if (!PyArg_ParseTupleAndKeywords(args, kwds, "|iiUi", kwlist,
                                   &self->virtualKey, &self->flags, &description, &self->plugin)) {
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
  return 0;
}

static PyMemberDef HotKeyInfo_members[] = {
  {"virtualKey", T_INT, offsetof(HotKeyInfoObject, virtualKey), 0, "Virtual Key"},
  {"flags", T_INT, offsetof(HotKeyInfoObject, flags), 0, "XPLMKeyFlags"},
  {"description", T_OBJECT_EX, offsetof(HotKeyInfoObject, description), 0, "Description"},
  {"plugin", T_INT, offsetof(HotKeyInfoObject, plugin), 0, "XPLMPluginID"},
  {nullptr, T_INT, 0, 0, ""}  /* Sentinel */
};

PyTypeObject
HotKeyInfoType = {
  PyVarObject_HEAD_INIT(nullptr, 0)
  .tp_name = "XPPython.HotKeyInfo",
  .tp_basicsize = sizeof(HotKeyInfoObject),
  .tp_itemsize = 0,
  .tp_dealloc = (destructor) HotKeyInfo_dealloc,
  .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_GC,
  .tp_doc = PyDoc_STR("HotKeyInfo, return from xp.getHotKeyInfo()"),
  .tp_traverse = (traverseproc) HotKeyInfo_traverse,
  .tp_clear = (inquiry) HotKeyInfo_clear,
  .tp_members = HotKeyInfo_members,
  .tp_init = (initproc) HotKeyInfo_init,
  .tp_new = HotKeyInfo_new,
};


PyObject *
PyHotKeyInfo_New(int virtualKey, int flags, char *description, int plugin)
{
  PyObject *argsList = Py_BuildValue("iisi", virtualKey, flags, description, plugin);
  PyObject *obj = PyObject_CallObject((PyObject *) &HotKeyInfoType, argsList);
  Py_XDECREF(argsList);
  return (PyObject*)obj;
}
