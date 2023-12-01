#define _GNU_SOURCE 1
#include <Python.h>
#include <sys/time.h>
#include <stdio.h>
#include "plugin_dl.h"
#include "utils.h"
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMScenery.h>
#include <XPLM/XPLMInstance.h>
#include "instance.h"


static const char instanceRefName[] = "XPLMInstanceRef";
static PyObject *instanceRefCapsules;

void resetInstances(void) {
  /* loaded instances need to be destroyed */
  PyObject *key, *value;
  Py_ssize_t pos = 0;
  while(PyDict_Next(instanceRefCapsules, &pos, &key, &value)) {
    char *s = objToStr(value);
    pythonLog("[XPPython3] Reset --     %s\n", s);
    free(s);
    XPLMDestroyInstance(PyLong_AsVoidPtr(key));
  }
  PyDict_Clear(instanceRefCapsules);
}

My_DOCSTR(_createInstance__doc__, "createInstance", "obj, dataRefs=None",
          "Create Instance for object retrieved by loadObject() or loadObjectAsync().\n"
          "\n"
          "Provide list of string dataRefs to be registered for this object.\n"
          "Returns object instance, to be used with instanceSetPosition().");
static PyObject *XPLMCreateInstanceFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"obj", "dataRefs", NULL};
  (void) self;
  PyObject *obj, *drefList=Py_None;
  if(!XPLMCreateInstance_ptr){
    PyErr_SetString(PyExc_RuntimeError , "createInstance is available only in XPLM300 and up.");
    return NULL;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|O", keywords, &obj, &drefList)){
    return NULL;
  }
  Py_ssize_t len = 0;
  /* PyObject *drefListTuple = Py_None; */
  if (drefList != Py_None) {
    len = PySequence_Length(drefList);
    /* drefListTuple = PySequence_Tuple(drefList); */
  }
  const char **datarefs = malloc(sizeof(char *) * (len + 1));
  if(datarefs == NULL){
    return NULL;
  }
  // Malloced len + 1, so we're not overflowing!
  datarefs[len] = NULL;

  Py_ssize_t i;
  char *tmp;
  PyObject *tmpObj;
  for(i = 0; i < len; ++i){
    PyObject *s = PyObject_Str(PySequence_GetItem(drefList, i)); /* GetItem borrows, Str is NEW */
    tmpObj = PyUnicode_AsUTF8String(s); /* new object */
    tmp = PyBytes_AsString(tmpObj); /* borrowed */
    Py_DECREF(s);
    if (PyErr_Occurred()) {
      Py_XDECREF(tmpObj);
      return NULL;
    }
    datarefs[i] = tmp;
    Py_DECREF(tmpObj);
  }
  /* if (len) { */
  /*   Py_DECREF(drefListTuple); */
  /* } */
  XPLMObjectRef inObj = refToPtr(obj, objRefName);

  XPLMInstanceRef res = XPLMCreateInstance_ptr(inObj, datarefs);
  free(datarefs);
  return getPtrRef(res, instanceRefCapsules, instanceRefName);
}

My_DOCSTR(_destroyInstance__doc__, "destroyInstance", "instance",
          "Destroys instance created by createInstance().");
static PyObject *XPLMDestroyInstanceFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"instance", NULL};
  (void) self;
  (void) args;
  PyObject *instance;
  if(!XPLMDestroyInstance_ptr){
    PyErr_SetString(PyExc_RuntimeError , "destroyInstance is available only in XPLM300 and up.");
    return NULL;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &instance)){
    return NULL;
  }
  XPLMDestroyInstance_ptr(refToPtr(instance, instanceRefName));
  removePtrRef(refToPtr(instance, instanceRefName), instanceRefCapsules);
  Py_RETURN_NONE;
}

My_DOCSTR(_instanceSetPosition__doc__, "instanceSetPosition", "instance, position, data=None",
          "Update position (x, y, z, pitch, heading, roll), \n"
          "and all datarefs (<float>, <float>, ...)"
          "\n"
          "You should provide values for all registered dataref, otherwise they'll\n"
          "be set to 0.0."
          );
static PyObject *XPLMInstanceSetPositionFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"instance", "position", "data", NULL};
  (void) self;
  PyObject *instance, *newPositionSeq, *data = Py_None;
  if(!XPLMInstanceSetPosition_ptr){
    PyErr_SetString(PyExc_RuntimeError , "instanceSetPosition is available only in XPLM300 and up.");
    return NULL;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "OO|O", keywords, &instance, &newPositionSeq, &data)){
    return NULL;
  }
  XPLMDrawInfo_t inNewPosition;
  inNewPosition.structSize = sizeof(XPLMDrawInfo_t);
  inNewPosition.x = PyFloat_AsDouble(PySequence_GetItem(newPositionSeq, 0));
  inNewPosition.y = PyFloat_AsDouble(PySequence_GetItem(newPositionSeq, 1));
  inNewPosition.z = PyFloat_AsDouble(PySequence_GetItem(newPositionSeq, 2));
  inNewPosition.pitch = PyFloat_AsDouble(PySequence_GetItem(newPositionSeq, 3));
  inNewPosition.heading = PyFloat_AsDouble(PySequence_GetItem(newPositionSeq, 4));
  inNewPosition.roll = PyFloat_AsDouble(PySequence_GetItem(newPositionSeq, 5));
  
  Py_ssize_t len = 0;
  if (data != Py_None) {
    len = PySequence_Length(data);
  }
  float *inData = malloc(sizeof(float) * len);
  if(len && inData == NULL){
    return NULL;
  }
  if (len) {
    Py_ssize_t i;
    for(i = 0; i < len; ++i){
      inData[i] = PyFloat_AsDouble(PySequence_GetItem(data, i));
    }
  }
  XPLMInstanceSetPosition_ptr(refToPtr(instance, instanceRefName), &inNewPosition, inData);
  free(inData);
  Py_RETURN_NONE;
}

static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  PyDict_Clear(instanceRefCapsules);
  Py_DECREF(instanceRefCapsules);
  Py_RETURN_NONE;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
static PyMethodDef XPLMInstanceMethods[] = {
  {"createInstance", (PyCFunction)XPLMCreateInstanceFun, METH_VARARGS | METH_KEYWORDS, _createInstance__doc__},
  {"XPLMCreateInstance", (PyCFunction)XPLMCreateInstanceFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"destroyInstance", (PyCFunction)XPLMDestroyInstanceFun, METH_VARARGS | METH_KEYWORDS, _destroyInstance__doc__},
  {"XPLMDestroyInstance", (PyCFunction)XPLMDestroyInstanceFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"instanceSetPosition", (PyCFunction)XPLMInstanceSetPositionFun, METH_VARARGS | METH_KEYWORDS, _instanceSetPosition__doc__},
  {"XPLMInstanceSetPosition", (PyCFunction)XPLMInstanceSetPositionFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"_cleanup", cleanup, METH_VARARGS, ""},
  {NULL, NULL, 0, NULL}
};
#pragma GCC diagnostic pop


static struct PyModuleDef XPLMInstanceModule = {
  PyModuleDef_HEAD_INIT,
  "XPLMInstance",
  "Laminar documentation: \n"
  "   https://developer.x-plane.com/sdk/XPLMInstance/\n"
  "XPPython3 documentation: \n"
  "   https://xppython3.rtfd.io/en/stable/development/modules/instance.html",
  -1,
  XPLMInstanceMethods,
  NULL,
  NULL,
  NULL,
  NULL
};

PyMODINIT_FUNC
PyInit_XPLMInstance(void)
{
  PyObject *mod = PyModule_Create(&XPLMInstanceModule);
  if(mod) {
    PyModule_AddStringConstant(mod, "__author__", "Peter Buckner (pbuck@avnwx.com)");
  }
  if(!(instanceRefCapsules = PyDict_New())){return NULL;}
  PyDict_SetItemString(XPY3pythonCapsules, instanceRefName, instanceRefCapsules);
  return mod;
}

