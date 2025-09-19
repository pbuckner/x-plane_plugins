#define _GNU_SOURCE 1
#include <Python.h>
#include "utils.h"

int allErrorsEncountered = 0;

static PyObject *logWriterWrite(PyObject *self, PyObject *args)
{
  (void) self;
  char *msg;
  if(!PyArg_ParseTuple(args, "s", &msg)){
    return nullptr;
  }
  pythonLogRaw(msg);
  pythonLogFlush();
  Py_RETURN_NONE;
}

static PyObject *logWriterFlush(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  pythonLogFlush();
  Py_RETURN_NONE;
}

static PyObject *logWriterAddAllErrors(PyObject *self, PyObject *args)
{
  (void) self;
  int errs;
  if(!PyArg_ParseTuple(args, "i", &errs)){
    return nullptr;
  }
  printf("Adding %d errors...\n", errs);
  allErrorsEncountered += errs;
  Py_RETURN_NONE;
}


static PyMethodDef logWriterMethods[] = {
  {"write", logWriterWrite, METH_VARARGS, ""},
  {"flush", logWriterFlush, METH_VARARGS, ""},
  {"addAllErrors", logWriterAddAllErrors, METH_VARARGS, ""},
  {nullptr, nullptr, 0, nullptr}
};

static struct PyModuleDef XPythonLogWriterModule = {
  PyModuleDef_HEAD_INIT,
  "XPythonLogWriter",
  nullptr,
  -1,
  logWriterMethods,
  nullptr,
  nullptr,
  nullptr,
  nullptr
};

PyMODINIT_FUNC
PyInit_XPythonLogWriter(void)
{
  PyObject *mod = PyModule_Create(&XPythonLogWriterModule);
  if(mod){
    PyModule_AddStringConstant(mod, "__author__", "Peter Buckner (pbuck@xppython3.org)");
    PySys_SetObject("stdout", mod);
    PySys_SetObject("stderr", mod);
  }

  return mod;
};
