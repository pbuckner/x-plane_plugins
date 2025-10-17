#define _GNU_SOURCE 1
#include <Python.h>
#include <sys/time.h>
#include <stdio.h>
#include "plugin_dl.h"
#include "utils.h"
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMScenery.h>
#include <XPLM/XPLMInstance.h>
#include <vector>
#include <string>
#include <unordered_map>
#include "instance.h"
#include "capsules.h"


void resetInstances(void) {
  /* loaded instances need to be destroyed */
  for(auto it = CapsuleDict.begin(); it != CapsuleDict.end();) {
    PyObject *capsule = it->second;
    if (! strcmp(PyCapsule_GetName(capsule), "XPLMInstanceRef")) {
      char *moduleName = (char *)PyCapsule_GetContext(capsule);
      char *s1 = objToStr(capsule);
      pythonDebug("     Reset --      %s  %s", moduleName, s1);
      free(s1);
      free(moduleName);
      XPLMDestroyInstance(getVoidPtr(capsule, "XPLMInstanceRef"));
      Py_DECREF(it->second);
      it = CapsuleDict.erase(it);
    } else {
      it++;
    }
  }
}

My_DOCSTR(_createInstance__doc__, "createInstance",
          "obj, dataRefs=None",
          "obj:XPLMObjectRef, dataRefs:Optional[tuple[str]]=None",
          "XPLMInstanceRef",
          "Create Instance for object retrieved by loadObject() or loadObjectAsync().\n"
          "\n"
          "Provide list of string dataRefs to be registered for this object.\n"
          "Returns object instance, to be used with instanceSetPosition().");
static PyObject *XPLMCreateInstanceFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("obj"), CHAR("dataRefs"), nullptr};
  (void) self;
  PyObject *obj, *drefList=Py_None;
  if(!XPLMCreateInstance_ptr){
    PyErr_SetString(PyExc_RuntimeError , "createInstance is available only in XPLM300 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|O", keywords, &obj, &drefList)){
    return nullptr;
  }
  Py_ssize_t len = 0;
  /* PyObject *drefListTuple = Py_None; */
  if (PySequence_Check(drefList)) {
    len = PySequence_Length(drefList);
    if (len < 0) {
      PyErr_SetString(PyExc_TypeError, "dataRefs must be a valid sequence");
      return nullptr;
    }
    if (len > 1000) { // reasonable limit for datarefs
      PyErr_SetString(PyExc_ValueError, "Too many dataRefs (max 1000)");
      return nullptr;
    }
    /* drefListTuple = PySequence_Tuple(drefList); */
  }
  char **datarefs = (char **)malloc(sizeof(char *) * (len + 1));
  if(datarefs == nullptr){
    return nullptr;
  }
  // Malloced len + 1, so we're not overflowing!
  datarefs[len] = nullptr;

  Py_ssize_t i;
  for(i = 0; i < len; ++i){
    PyObject *item = PySequence_GetItem(drefList, i); /* GetItem borrows */
    if (!item) {
      // Clean up previously allocated strings
      for (Py_ssize_t j = 0; j < i; j++) {
        free(datarefs[j]);
      }
      free(datarefs);
      return nullptr;
    }

    // Require string types only
    if (!PyUnicode_Check(item)) {
      PyErr_SetString(PyExc_TypeError, "All dataRefs must be strings");
      // Clean up previously allocated strings
      for (Py_ssize_t j = 0; j < i; j++) {
        free(datarefs[j]);
      }
      free(datarefs);
      return nullptr;
    }

    PyObject *utf8_obj = PyUnicode_AsUTF8String(item);
    if (!utf8_obj || PyErr_Occurred()) {
      // Clean up previously allocated strings
      for (Py_ssize_t j = 0; j < i; j++) {
        free(datarefs[j]);
      }
      free(datarefs);
      Py_XDECREF(utf8_obj);
      return nullptr;
    }

    char *tmp = PyBytes_AsString(utf8_obj); /* borrowed */
    datarefs[i] = strdup(tmp); /* make a copy to avoid use-after-free */
    Py_DECREF(utf8_obj);
    if (!datarefs[i]) {
      // Clean up previously allocated strings
      for (Py_ssize_t j = 0; j < i; j++) {
        free(datarefs[j]);
      }
      free(datarefs);
      return nullptr;
    }
  }
  XPLMObjectRef inObj = getVoidPtr(obj, "XPLMObjectRef");

  XPLMInstanceRef res = XPLMCreateInstance_ptr(inObj, (const char**)datarefs);

  // Clean up allocated strings
  for (i = 0; i < len; i++) {
    free(datarefs[i]);
  }
  free(datarefs);
  return makeCapsule(res, "XPLMInstanceRef");
}

My_DOCSTR(_destroyInstance__doc__, "destroyInstance",
          "instance",
          "instance:XPLMInstanceRef",
          "None",
          "Destroys instance created by createInstance().");
static PyObject *XPLMDestroyInstanceFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("instance"), nullptr};
  (void) self;
  (void) args;
  PyObject *instance;
  if(!XPLMDestroyInstance_ptr){
    PyErr_SetString(PyExc_RuntimeError , "destroyInstance is available only in XPLM300 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &instance)){
    return nullptr;
  }
  XPLMDestroyInstance_ptr(getVoidPtr(instance, "XPLMInstanceRef"));
  deleteCapsule(instance);
  Py_RETURN_NONE;
}

My_DOCSTR(_instanceSetAutoShift__doc__, "instanceSetAutoShift",
          "instance",
          "instance:XPLMInstanceRef",
          "None",
          "Tells X-Plane to move instance location if/when Sim's local coordinate\n"
          "system changes.");
static PyObject *XPLMInstanceSetAutoShiftFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("instance"), nullptr};
  (void) self;
  (void) args;
  PyObject *instance;
  if(!XPLMInstanceSetAutoShift_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMInstanceSetAutoShift is available only in XPLM420 and up, and requires at least X-Plane 12.3.0");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &instance)){
    return nullptr;
  }
  XPLMInstanceSetAutoShift_ptr(getVoidPtr(instance, "XPLMInstanceRef"));
  Py_RETURN_NONE;
}

My_DOCSTR(_instanceSetPosition__doc__, "instanceSetPosition",
          "instance, position, data=None",
          "instance:XPLMInstanceRef, position:XPLMDrawInfo_t | tuple[float, float, float, float, float, float], "
          "data:Optional[tuple[float]]=None",
          "None",
          "Update position (x, y, z, pitch, heading, roll), \n"
          "and all datarefs (<float>, <float>, ...)"
          "\n"
          "You should provide values for all registered dataref, otherwise they'll\n"
          "be set to 0.0. This function is identical to instanceSetPositionDouble\n"
          "as Python treats all floating point as doubles."
          );
static PyObject *XPLMInstanceSetPositionFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("instance"), CHAR("position"), CHAR("data"), nullptr};
  (void) self;
  PyObject *instance, *newPositionSeq, *data = Py_None;
  if(!XPLMInstanceSetPosition_ptr){
    PyErr_SetString(PyExc_RuntimeError, "XPLMInstanceSetPosition is available only in XPLM300 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "OO|O", keywords, &instance, &newPositionSeq, &data)){
    return nullptr;
  }
  XPLMDrawInfo_t inNewPosition;
  XPLMDrawInfoDouble_t inNewPositionDouble;
  if (PySequence_Check(newPositionSeq) && PySequence_Length(newPositionSeq) == 6) {
    if (XPLMInstanceSetPositionDouble_ptr) {
      inNewPositionDouble.structSize = sizeof(XPLMDrawInfoDouble_t);
      inNewPositionDouble.x = PyFloat_AsDouble(PySequence_GetItem(newPositionSeq, 0));
      inNewPositionDouble.y = PyFloat_AsDouble(PySequence_GetItem(newPositionSeq, 1));
      inNewPositionDouble.z = PyFloat_AsDouble(PySequence_GetItem(newPositionSeq, 2));
      inNewPositionDouble.pitch = PyFloat_AsDouble(PySequence_GetItem(newPositionSeq, 3));
      inNewPositionDouble.heading = PyFloat_AsDouble(PySequence_GetItem(newPositionSeq, 4));
      inNewPositionDouble.roll = PyFloat_AsDouble(PySequence_GetItem(newPositionSeq, 5));
    } else {
      inNewPosition.structSize = sizeof(XPLMDrawInfo_t);
      inNewPosition.x = PyFloat_AsDouble(PySequence_GetItem(newPositionSeq, 0));
      inNewPosition.y = PyFloat_AsDouble(PySequence_GetItem(newPositionSeq, 1));
      inNewPosition.z = PyFloat_AsDouble(PySequence_GetItem(newPositionSeq, 2));
      inNewPosition.pitch = PyFloat_AsDouble(PySequence_GetItem(newPositionSeq, 3));
      inNewPosition.heading = PyFloat_AsDouble(PySequence_GetItem(newPositionSeq, 4));
      inNewPosition.roll = PyFloat_AsDouble(PySequence_GetItem(newPositionSeq, 5));
    }
  } else if (PyObject_HasAttrString(newPositionSeq, "heading")) {
    PyObject *value;
    value = PyObject_GetAttrString(newPositionSeq, "x");
    if (XPLMInstanceSetPositionDouble_ptr) {
      inNewPositionDouble.x = PyFloat_AsDouble(value);
    } else {
      inNewPosition.x = PyFloat_AsDouble(value);
    }
    Py_DECREF(value);

    value = PyObject_GetAttrString(newPositionSeq, "y");
    if (XPLMInstanceSetPositionDouble_ptr) {
      inNewPositionDouble.y = PyFloat_AsDouble(value);
    } else {
      inNewPosition.y = PyFloat_AsDouble(value);
    }
    Py_DECREF(value);

    value = PyObject_GetAttrString(newPositionSeq, "z");
    if (XPLMInstanceSetPositionDouble_ptr) {
      inNewPositionDouble.z = PyFloat_AsDouble(value);
    } else {
      inNewPosition.z = PyFloat_AsDouble(value);
    }
    Py_DECREF(value);

    value = PyObject_GetAttrString(newPositionSeq, "pitch");
    if (XPLMInstanceSetPositionDouble_ptr) {
      inNewPositionDouble.pitch = PyFloat_AsDouble(value);
    } else {
      inNewPosition.pitch = PyFloat_AsDouble(value);
    }
    Py_DECREF(value);

    value = PyObject_GetAttrString(newPositionSeq, "heading");
    if (XPLMInstanceSetPositionDouble_ptr) {
      inNewPositionDouble.heading = PyFloat_AsDouble(value);
    } else {
      inNewPosition.heading = PyFloat_AsDouble(value);
    }
    Py_DECREF(value);

    value = PyObject_GetAttrString(newPositionSeq, "roll");
    if (XPLMInstanceSetPositionDouble_ptr) {
      inNewPositionDouble.roll = PyFloat_AsDouble(value);
    } else {
      inNewPosition.roll = PyFloat_AsDouble(value);
    }
    Py_DECREF(value);

  } else if (PyMapping_Check(newPositionSeq)) {
    if (XPLMInstanceSetPositionDouble_ptr) {
      inNewPositionDouble.x = PyFloat_AsDouble(PyMapping_GetItemString(newPositionSeq, "x"));
      inNewPositionDouble.y = PyFloat_AsDouble(PyMapping_GetItemString(newPositionSeq, "y"));
      inNewPositionDouble.z = PyFloat_AsDouble(PyMapping_GetItemString(newPositionSeq, "z"));
      inNewPositionDouble.pitch = PyFloat_AsDouble(PyMapping_GetItemString(newPositionSeq, "pitch"));
      inNewPositionDouble.heading = PyFloat_AsDouble(PyMapping_GetItemString(newPositionSeq, "heading"));
      inNewPositionDouble.roll = PyFloat_AsDouble(PyMapping_GetItemString(newPositionSeq, "roll"));
    } else {
      inNewPosition.x = PyFloat_AsDouble(PyMapping_GetItemString(newPositionSeq, "x"));
      inNewPosition.y = PyFloat_AsDouble(PyMapping_GetItemString(newPositionSeq, "y"));
      inNewPosition.z = PyFloat_AsDouble(PyMapping_GetItemString(newPositionSeq, "z"));
      inNewPosition.pitch = PyFloat_AsDouble(PyMapping_GetItemString(newPositionSeq, "pitch"));
      inNewPosition.heading = PyFloat_AsDouble(PyMapping_GetItemString(newPositionSeq, "heading"));
      inNewPosition.roll = PyFloat_AsDouble(PyMapping_GetItemString(newPositionSeq, "roll"));
    }
  } else {
    PyErr_SetString(PyExc_AttributeError, "instanceSetPosition: unknown data type for position");
    return nullptr;
  }
    
  Py_ssize_t len = 0;
  if (data != Py_None) {
    len = PySequence_Length(data);
    if (len < 0) {
      PyErr_SetString(PyExc_TypeError, "data must be a valid sequence");
      return nullptr;
    }
    if (len > 10000) { // reasonable limit for data values
      PyErr_SetString(PyExc_ValueError, "Too many data values (max 10000)");
      return nullptr;
    }
  }
  float *inData = (float*)malloc(sizeof(float) * len);
  if(len && inData == nullptr){
    return nullptr;
  }
  if (len) {
    Py_ssize_t i;
    for(i = 0; i < len; ++i){
      PyObject *item = PySequence_GetItem(data, i);
      if (!item) {
        free(inData);
        return nullptr;
      }
      inData[i] = PyFloat_AsDouble(item);
      if (PyErr_Occurred()) {
        free(inData);
        return nullptr;
      }
    }
  }
  void *p = getVoidPtr(instance, "XPLMInstanceRef");
  if (p != nullptr) {
    if (XPLMInstanceSetPositionDouble_ptr) {
      XPLMInstanceSetPositionDouble_ptr(p, &inNewPositionDouble, inData);
    } else {
      XPLMInstanceSetPosition_ptr(p, &inNewPosition, inData);
    }
  }
  free(inData);
  Py_RETURN_NONE;
}

static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  Py_RETURN_NONE;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
static PyMethodDef XPLMInstanceMethods[] = {
  {"createInstance", (PyCFunction)XPLMCreateInstanceFun, METH_VARARGS | METH_KEYWORDS, _createInstance__doc__},
  {"XPLMCreateInstance", (PyCFunction)XPLMCreateInstanceFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"destroyInstance", (PyCFunction)XPLMDestroyInstanceFun, METH_VARARGS | METH_KEYWORDS, _destroyInstance__doc__},
  {"XPLMDestroyInstance", (PyCFunction)XPLMDestroyInstanceFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"instanceSetAutoShift", (PyCFunction)XPLMInstanceSetAutoShiftFun, METH_VARARGS | METH_KEYWORDS, _instanceSetAutoShift__doc__},
  {"XPLMInstanceSetAutoShift", (PyCFunction)XPLMInstanceSetAutoShiftFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"instanceSetPosition", (PyCFunction)XPLMInstanceSetPositionFun, METH_VARARGS | METH_KEYWORDS, _instanceSetPosition__doc__},
  {"instanceSetPositionDouble", (PyCFunction)XPLMInstanceSetPositionFun, METH_VARARGS | METH_KEYWORDS, _instanceSetPosition__doc__},
  {"XPLMInstanceSetPosition", (PyCFunction)XPLMInstanceSetPositionFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"XPLMInstanceSetPositionDouble", (PyCFunction)XPLMInstanceSetPositionFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"_cleanup", cleanup, METH_VARARGS, ""},
  {nullptr, nullptr, 0, nullptr}
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
  nullptr,
  nullptr,
  nullptr,
  nullptr
};

PyMODINIT_FUNC
PyInit_XPLMInstance(void)
{
  PyObject *mod = PyModule_Create(&XPLMInstanceModule);
  if(mod) {
    PyModule_AddStringConstant(mod, "__author__", "Peter Buckner (pbuck@xppython3.org)");
  }
  return mod;
}

