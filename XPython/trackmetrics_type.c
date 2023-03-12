#define _GNU_SOURCE 1
#include <Python.h>
#include <structmember.h>
#include "trackMetrics.h"

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

static Py_ssize_t
TrackMetrics_len(TrackMetricsObject *self) {
  (void)self;
  return 6;
}

static PyObject *
TrackMetrics_getitem(TrackMetricsObject *self, Py_ssize_t i) {
  switch(i) {
  case 0:
    return PyLong_FromLong(self->isVertical);
  case 1:
    return PyLong_FromLong(self->downBtnSize);
  case 2:
    return PyLong_FromLong(self->downPageSize);
  case 3:
    return PyLong_FromLong(self->thumbSize);
  case 4:
    return PyLong_FromLong(self->upPageSize);
  case 5:
    return PyLong_FromLong(self->upBtnSize);
  default:
    ;
  }
  PyErr_SetString(PyExc_IndexError, "list index out of range");
  return NULL;
}

                 

static PyMemberDef TrackMetrics_members[] = {
    {"isVertical", T_INT, offsetof(TrackMetricsObject, isVertical), 0, "1= vertically aligned, horizontal otherwise"},
    {"downBtnSize", T_INT, offsetof(TrackMetricsObject, downBtnSize), 0, "Down (or left) button size (pixels)"},
    {"downPageSize", T_INT, offsetof(TrackMetricsObject, downPageSize), 0, "Down page size, between button and thumb (pixels)"},
    {"thumbSize", T_INT, offsetof(TrackMetricsObject, thumbSize), 0, "Size of thumb (pixels)"},
    {"upPageSize", T_INT, offsetof(TrackMetricsObject, upPageSize), 0, "Up page size, thumb and button (pixels)"},
    {"upBtnSize", T_INT, offsetof(TrackMetricsObject, upBtnSize), 0, "Up (or right) button size (pixels)"},
    {NULL, T_INT, 0, 0, ""}  /* Sentinel */
};

static PyObject* TrackMetrics_str(TrackMetricsObject *self) {
  char s[1024];
  sprintf(s, "<TrackMetrics {isVertical: %d, downBtnSize: %d, downPageSize: %d, thumbSize: %d, upPageSize: %d, upBtnSize: %d}>",
          self->isVertical, self->downBtnSize, self->downPageSize, self->thumbSize, self->upPageSize, self->upBtnSize);
  return PyUnicode_FromString(s);
}

static PySequenceMethods TrackMetrics_SequenceMethods = {
                                                         .sq_length = (lenfunc) TrackMetrics_len,
                                                         .sq_item = (ssizeargfunc) TrackMetrics_getitem,
};

PyTypeObject
TrackMetricsType = {
  PyVarObject_HEAD_INIT(NULL, 0)
  .tp_name = "XPPython.TrackMetrics",
  .tp_doc = "TrackMetrics, return from xp.getTrackMetrics",
  .tp_basicsize = sizeof(TrackMetricsObject),
  .tp_itemsize = 0,
  .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_GC,
  .tp_new = TrackMetrics_new,
  .tp_init = (initproc) TrackMetrics_init,
  .tp_dealloc = (destructor) TrackMetrics_dealloc,
  .tp_as_sequence = &TrackMetrics_SequenceMethods,
  .tp_str = (reprfunc) TrackMetrics_str,
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
