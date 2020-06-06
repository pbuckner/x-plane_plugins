#define _GNU_SOURCE 1
#include <Python.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdbool.h>

extern const char *pythonPluginVersion;
/* static PyObject *XPPythonVersionFun(PyObject *self, PyObject *args) */
/* { */
/*   (void) self; */
/*   (void) args; */
/*   PyObject *version = PyUnicode_FromString(pythonPluginVersion); */
/*   return version; */
/* } */


static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  Py_RETURN_NONE;
}

static PyMethodDef XPPythonMethods[] = {
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
  PyObject *mod = PyModule_Create(&XPPythonModule);
  if (mod != NULL) {
    PyModule_AddStringConstant(mod, "VERSION", pythonPluginVersion);
  }

  return mod;
}




