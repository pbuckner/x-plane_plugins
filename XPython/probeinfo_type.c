#define _GNU_SOURCE 1
#include <Python.h>
#include <structmember.h>
#include "utils.h"

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
    {"result", T_INT, offsetof(ProbeInfoObject, result), 0, "XPLMProbeResult, 0=valid, 1=Error in call, 2=No terrain found"},
    {"locationX", T_FLOAT, offsetof(ProbeInfoObject, locationX), 0, "OpenGL point 'x'"},
    {"locationY", T_FLOAT, offsetof(ProbeInfoObject, locationY), 0, "OpenGL point 'y'"},
    {"locationZ", T_FLOAT, offsetof(ProbeInfoObject, locationZ), 0, "OpenGL point 'z'"},
    {"normalX", T_FLOAT, offsetof(ProbeInfoObject, normalX), 0, "Normal vector (slope) 'x' of terrain found"},
    {"normalY", T_FLOAT, offsetof(ProbeInfoObject, normalY), 0, "Normal vector (slope) 'y' of terrain found"},
    {"normalZ", T_FLOAT, offsetof(ProbeInfoObject, normalZ), 0, "Normal vector (slope) 'z' of terrain found"},
    {"velocityX", T_FLOAT, offsetof(ProbeInfoObject, velocityX), 0, "Velocity vector 'x' (meters/second) of movement of terrain"},
    {"velocityY", T_FLOAT, offsetof(ProbeInfoObject, velocityY), 0, "Velocity vector 'y' (meters/second) of movement of terrain"},
    {"velocityZ", T_FLOAT, offsetof(ProbeInfoObject, velocityZ), 0, "Velocity vector 'z' (meters/second) of movement of terrain"},
    {"is_wet", T_INT, offsetof(ProbeInfoObject, is_wet), 0, "1=Location at surface is water"},
    {NULL, T_INT, 0, 0, ""}  /* Sentinel */
};

PyTypeObject
ProbeInfoType = {
  PyVarObject_HEAD_INIT(NULL, 0)
  .tp_name = "XPPython.XPLMProbeInfo_t",
  .tp_doc = PyDoc_STR("XPLMProbeInfo_t, return from xp.probeTerrainXYZ()"),
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
  Py_XDECREF(argsList);
  return (PyObject*)obj;
}
