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
#include <unordered_set>
#include "instance.h"
#include "capsules.h"

static std::unordered_set<void*> instanceSet;


void resetInstances(void) {
  /* Destroy all tracked instances */
  for(auto it = instanceSet.begin(); it != instanceSet.end();) {
    XPLMDestroyInstance(*it);
    it = instanceSet.erase(it);
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

  // Add to instance tracking set
  instanceSet.insert((void*)res);

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
  void *instancePtr = getVoidPtr(instance, "XPLMInstanceRef");
  instanceSet.erase(instancePtr);
  XPLMDestroyInstance_ptr(instancePtr);
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

My_DOCSTR(_instanceSetPositionDouble__doc__, "instanceSetPositionDouble",
          "instance, position, data=None",
          "instance:XPLMInstanceRef, position:XPLMDrawInfo_t | tuple[float, float, float, float, float, float], "
          "data:Optional[tuple[float]]=None",
          "None",
          "Update position (x, y, z, pitch, heading, roll), \n"
          "and all datarefs (<float>, <float>, ...)"
          "\n"
          "This function is identical to instanceSetPosition, as Python treats\n"
          "all floating point as doubles."
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

My_DOCSTR(_createInstanceEx__doc__, "createInstanceEx",
          "objects, dataRefs=None, coordinateSpace=CoordSpace_World, aircraftIndex=0, autoShift=0",
          "objects:Sequence[XPLMObjectRef | tuple[XPLMObjectRef, float, float, float, float, float, float]], "
          "dataRefs:Optional[Sequence[str]]=None, coordinateSpace:XPLMCoordinateSpace_t=CoordSpace_World, "
          "aircraftIndex:int=0, autoShift:int=0",
          "XPLMInstanceRef",
          "Create a multi-object instance (superset of createInstance()).\n"
          "\n"
          "objects is a sequence, each element being either an XPLMObjectRef (placed\n"
          "at the instance origin) or a tuple (obj, x, y, z, pitch, heading, roll)\n"
          "giving a fixed local offset. All objects move rigidly together and share\n"
          "the single dataRefs list. coordinateSpace is one of the CoordSpace_*\n"
          "constants; aircraftIndex (0=user) applies only to the aircraft spaces;\n"
          "autoShift (world space only) is like instanceSetAutoShift().\n"
          "\n"
          "Returns object instance, to be used with instanceSetPosition().");
static PyObject *XPLMCreateInstanceExFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("objects"), CHAR("dataRefs"), CHAR("coordinateSpace"),
                             CHAR("aircraftIndex"), CHAR("autoShift"), nullptr};
  (void) self;
  PyObject *objects, *drefList = Py_None;
  int coordinateSpace = 0 /* xplm_CoordSpace_World */, aircraftIndex = 0, autoShift = 0;
  if(!XPLMCreateInstanceEx_ptr){
    PyErr_SetString(PyExc_RuntimeError , "createInstanceEx is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|Oiii", keywords, &objects, &drefList,
                                  &coordinateSpace, &aircraftIndex, &autoShift)){
    return nullptr;
  }

  if(!PySequence_Check(objects)){
    PyErr_SetString(PyExc_TypeError, "createInstanceEx: objects must be a sequence");
    return nullptr;
  }
  Py_ssize_t objCount = PySequence_Length(objects);
  if(objCount < 1){
    PyErr_SetString(PyExc_ValueError, "createInstanceEx: objects must contain at least one object");
    return nullptr;
  }
  if(objCount > 1000){
    PyErr_SetString(PyExc_ValueError, "createInstanceEx: too many objects (max 1000)");
    return nullptr;
  }

  XPLMInstanceObject_t *objArray = (XPLMInstanceObject_t *)malloc(sizeof(XPLMInstanceObject_t) * objCount);
  if(objArray == nullptr){
    return nullptr;
  }
  for(Py_ssize_t i = 0; i < objCount; ++i){
    PyObject *elem = PySequence_GetItem(objects, i); /* new ref */
    if(!elem){
      free(objArray);
      return nullptr;
    }
    XPLMObjectRef objRef;
    float offsets[6] = {0, 0, 0, 0, 0, 0};   /* x, y, z, pitch, heading, roll */
    if(PySequence_Check(elem)){
      /* (obj, x, y, z, pitch, heading, roll) -- offsets past the object default to 0 */
      Py_ssize_t elen = PySequence_Length(elem);
      if(elen < 1 || elen > 7){
        PyErr_SetString(PyExc_ValueError, "createInstanceEx: each object tuple must be (obj, x, y, z, pitch, heading, roll)");
        Py_DECREF(elem);
        free(objArray);
        return nullptr;
      }
      PyObject *o = PySequence_GetItem(elem, 0); /* new ref */
      objRef = getVoidPtr(o, "XPLMObjectRef");
      Py_XDECREF(o);
      for(Py_ssize_t k = 1; k < elen; ++k){
        PyObject *v = PySequence_GetItem(elem, k); /* new ref */
        offsets[k - 1] = v ? (float)PyFloat_AsDouble(v) : 0.0f;
        Py_XDECREF(v);
      }
      if(PyErr_Occurred()){
        Py_DECREF(elem);
        free(objArray);
        return nullptr;
      }
    } else {
      objRef = getVoidPtr(elem, "XPLMObjectRef");
    }
    Py_DECREF(elem);
    objArray[i].object  = objRef;
    objArray[i].x       = offsets[0];
    objArray[i].y       = offsets[1];
    objArray[i].z       = offsets[2];
    objArray[i].pitch   = offsets[3];
    objArray[i].heading = offsets[4];
    objArray[i].roll    = offsets[5];
  }

  /* Build the shared, NULL-terminated dataref list (same as createInstance). */
  Py_ssize_t len = 0;
  if(PySequence_Check(drefList)){
    len = PySequence_Length(drefList);
    if(len < 0){
      PyErr_SetString(PyExc_TypeError, "dataRefs must be a valid sequence");
      free(objArray);
      return nullptr;
    }
    if(len > 1000){
      PyErr_SetString(PyExc_ValueError, "Too many dataRefs (max 1000)");
      free(objArray);
      return nullptr;
    }
  }
  char **datarefs = (char **)malloc(sizeof(char *) * (len + 1));
  if(datarefs == nullptr){
    free(objArray);
    return nullptr;
  }
  datarefs[len] = nullptr;
  for(Py_ssize_t i = 0; i < len; ++i){
    PyObject *item = PySequence_GetItem(drefList, i); /* new ref */
    if(!item || !PyUnicode_Check(item)){
      PyErr_SetString(PyExc_TypeError, "All dataRefs must be strings");
      Py_XDECREF(item);
      for(Py_ssize_t j = 0; j < i; j++) free(datarefs[j]);
      free(datarefs);
      free(objArray);
      return nullptr;
    }
    PyObject *utf8_obj = PyUnicode_AsUTF8String(item);
    Py_DECREF(item);
    if(!utf8_obj){
      for(Py_ssize_t j = 0; j < i; j++) free(datarefs[j]);
      free(datarefs);
      free(objArray);
      return nullptr;
    }
    datarefs[i] = strdup(PyBytes_AsString(utf8_obj)); /* copy to avoid use-after-free */
    Py_DECREF(utf8_obj);
    if(!datarefs[i]){
      for(Py_ssize_t j = 0; j < i; j++) free(datarefs[j]);
      free(datarefs);
      free(objArray);
      return nullptr;
    }
  }

  XPLMCreateInstance_t params;
  params.structSize = sizeof(params);
  params.objects = objArray;
  params.objectCount = (int)objCount;
  params.datarefs = (const char **)datarefs;
  params.coordinateSpace = (XPLMCoordinateSpace_t)coordinateSpace;
  params.aircraftIndex = aircraftIndex;
  params.autoShift = autoShift;

  XPLMInstanceRef res = XPLMCreateInstanceEx_ptr(&params);

  for(Py_ssize_t i = 0; i < len; i++) free(datarefs[i]);
  free(datarefs);
  free(objArray);

  instanceSet.insert((void*)res);
  return makeCapsule(res, "XPLMInstanceRef");
}

My_DOCSTR(_instanceSetCoordinateSpace__doc__, "instanceSetCoordinateSpace",
          "instance, space, aircraftIndex=0",
          "instance:XPLMInstanceRef, space:XPLMCoordinateSpace_t, aircraftIndex:int=0",
          "None",
          "Change the coordinate space used to interpret positions passed to\n"
          "instanceSetPosition(). space is one of the CoordSpace_* constants;\n"
          "aircraftIndex (0=user) applies only to the aircraft spaces. X-Plane\n"
          "re-expresses the instance's current world location in the new space,\n"
          "so the object does not jump.");
static PyObject *XPLMInstanceSetCoordinateSpaceFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("instance"), CHAR("space"), CHAR("aircraftIndex"), nullptr};
  (void) self;
  PyObject *instance;
  int space, aircraftIndex = 0;
  if(!XPLMInstanceSetCoordinateSpace_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMInstanceSetCoordinateSpace is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Oi|i", keywords, &instance, &space, &aircraftIndex)){
    return nullptr;
  }
  XPLMInstanceSetCoordinateSpace_ptr(getVoidPtr(instance, "XPLMInstanceRef"),
                                     (XPLMCoordinateSpace_t)space, aircraftIndex);
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
  {"instanceSetPositionDouble", (PyCFunction)XPLMInstanceSetPositionFun, METH_VARARGS | METH_KEYWORDS, _instanceSetPositionDouble__doc__},
  {"XPLMInstanceSetPosition", (PyCFunction)XPLMInstanceSetPositionFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"XPLMInstanceSetPositionDouble", (PyCFunction)XPLMInstanceSetPositionFun, METH_VARARGS | METH_KEYWORDS, ""},
  // SDK 440
  {"createInstanceEx", (PyCFunction)XPLMCreateInstanceExFun, METH_VARARGS | METH_KEYWORDS, _createInstanceEx__doc__},
  {"XPLMCreateInstanceEx", (PyCFunction)XPLMCreateInstanceExFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"instanceSetCoordinateSpace", (PyCFunction)XPLMInstanceSetCoordinateSpaceFun, METH_VARARGS | METH_KEYWORDS, _instanceSetCoordinateSpace__doc__},
  {"XPLMInstanceSetCoordinateSpace", (PyCFunction)XPLMInstanceSetCoordinateSpaceFun, METH_VARARGS | METH_KEYWORDS, ""},
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
#if defined(XPLM440)
    PyModule_AddIntConstant(mod, "xplm_CoordSpace_World", xplm_CoordSpace_World); //XPLMCoordinateSpace_t
    PyModule_AddIntConstant(mod, "xplm_CoordSpace_AircraftInterior", xplm_CoordSpace_AircraftInterior); //XPLMCoordinateSpace_t
    PyModule_AddIntConstant(mod, "xplm_CoordSpace_AircraftExterior", xplm_CoordSpace_AircraftExterior); //XPLMCoordinateSpace_t
    PyModule_AddIntConstant(mod, "xplm_CoordSpace_Camera", xplm_CoordSpace_Camera); //XPLMCoordinateSpace_t

    PyModule_AddIntConstant(mod, "CoordSpace_World", xplm_CoordSpace_World); //XPLMCoordinateSpace_t
    PyModule_AddIntConstant(mod, "CoordSpace_AircraftInterior", xplm_CoordSpace_AircraftInterior); //XPLMCoordinateSpace_t
    PyModule_AddIntConstant(mod, "CoordSpace_AircraftExterior", xplm_CoordSpace_AircraftExterior); //XPLMCoordinateSpace_t
    PyModule_AddIntConstant(mod, "CoordSpace_Camera", xplm_CoordSpace_Camera); //XPLMCoordinateSpace_t
#endif
  }
  return mod;
}

