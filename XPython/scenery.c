#define _GNU_SOURCE 1
#include <Python.h>
#include <sys/time.h>
#include <stdio.h>
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMScenery.h>
#include "utils.h"
#include "plugin_dl.h"
#include "xppythontypes.h"

static PyObject *loaderDict; /* {
                                   <loaderCntr1>: ([0]path, [1]callback, [2]refcon, [3]module),
                                } */
#define LOADER_PATH 0
#define LOADER_CALLBACK 1
#define LOADER_REFCON 2
#define LOADER_MODULE_NAME 3
static intptr_t loaderCntr;
static void genericObjectLoaded(XPLMObjectRef inObject, void *inRefcon);


static const char probeName[] = "XPLMProbeRef";

My_DOCSTR(_createProbe__doc__, "createProbe", "probeType=0",
          "Return a probeRef");
static PyObject *XPLMCreateProbeFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"probeType", NULL};
  (void) self;
  int inProbeType = 0;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|i", keywords, &inProbeType)){
    return NULL;
  }
  return getPtrRefOneshot(XPLMCreateProbe(inProbeType), probeName);
}

My_DOCSTR(_destroyProbe__doc__, "destroyProbe", "probe",
          "Destroy a probeRef");
static PyObject *XPLMDestroyProbeFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"probe", NULL};
  (void) self;
  PyObject *inProbe = NULL;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &inProbe)){
    return NULL;
  }
  XPLMDestroyProbe(refToPtr(inProbe, probeName));
  Py_RETURN_NONE;
}

My_DOCSTR(_probeTerrainXYZ__doc__, "probeTerrainXYZ", "probeRef, x, y, z",
          "Probe terrain using probeRef at (x, y, z) location\n"
          "\n"
          "Object returned as attributes:\n"
          "  .result:    0=Hit, 1=Error, 2=Missed\n"
          "  .locationX,\n"
          "  .locationY,\n"
          "  .locationZ: OpenGL point hit by probe\n"
          "  .normalX,\n"
          "  .normalY,\n"
          "  .normalZ:   Normal vector (e.g. slope),\n"
          "  .velocityX,\n"
          "  .velocityY,\n"
          "  .velocityZ: Velocity vector (e.g., meter/sec) of meovement\n"
          "  .is_set:    1=we hit water");
static PyObject *XPLMProbeTerrainXYZFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"probeRef", "x", "y", "z", NULL};
  (void) self;
  PyObject *probe;
  float inX, inY, inZ;

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Offf", keywords, &probe, &inX, &inY, &inZ)){
    return NULL;
  }

  XPLMProbeRef inProbe = refToPtr(probe, probeName);
  XPLMProbeInfo_t info;
  info.structSize = sizeof(info);
  XPLMProbeResult res = XPLMProbeTerrainXYZ(inProbe, inX, inY, inZ, &info);

  PyObject *ret;
  ret = PyProbeInfo_New(res, info.locationX, info.locationY, info.locationZ,
                           info.normalX, info.normalY, info.normalZ,
                           info.velocityX, info.velocityY, info.velocityZ, info.is_wet);
  return ret;
}

My_DOCSTR(_getMagneticVariation__doc__, "getMagneticVariation", "latitude, longitude",
          "Magnetic declination at point");
static PyObject *XPLMGetMagneticVariationFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"latitude", "longitude", NULL};
  (void)self;
  if(!XPLMGetMagneticVariation_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMGetMagneticVariation is available only in XPLM300 and up.");
    return NULL;
  }
  double latitude, longitude;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "dd", keywords, &latitude, &longitude)){
    return NULL;
  }
  return PyFloat_FromDouble(XPLMGetMagneticVariation_ptr(latitude, longitude));
}

My_DOCSTR(_degTrueToDegMagnetic__doc__, "degTrueToDegMagnetic", "degrees=0.0",
          "Convert degrees True to degrees Magnetic, at user's current location");
static PyObject *XPLMDegTrueToDegMagneticFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"degrees", NULL};
  (void)self;
  if(!XPLMDegTrueToDegMagnetic_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMDegTrueToDegMagnetic is available only in XPLM300 and up.");
    return NULL;
  }
  float headingDegreesTrue = 0.0;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|f", keywords, &headingDegreesTrue)){
    return NULL;
  }
  return PyFloat_FromDouble(XPLMDegTrueToDegMagnetic_ptr(headingDegreesTrue));
}

My_DOCSTR(_degMagneticToDegTrue__doc__, "degMagneticToDegTrue", "degrees=0.0",
          "Convert degrees Magnetic to degrees True, at user's current location");
static PyObject *XPLMDegMagneticToDegTrueFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"degrees", NULL};
  (void)self;
  if(!XPLMDegMagneticToDegTrue_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMDegMagneticToDegTrue is available only in XPLM300 and up.");
    return NULL;
  }
  float headingDegreesMagnetic=0.0;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|f", keywords, &headingDegreesMagnetic)){
    return NULL;
  }
  return PyFloat_FromDouble(XPLMDegMagneticToDegTrue_ptr(headingDegreesMagnetic));
}


My_DOCSTR(_loadObject__doc__, "loadObject", "path",
          "Load OBJ file from path, returning objectRef\n"
          "\n"
          "Path may be absolute, or relative X-Plane Root");
static PyObject *XPLMLoadObjectFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"path", NULL};
  (void)self;
  const char *inPath;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "s", keywords, &inPath)){
    return NULL;
  }
  XPLMObjectRef res = XPLMLoadObject(inPath);
  return getPtrRefOneshot(res, objRefName);
}



static void genericObjectLoaded(XPLMObjectRef inObject, void *inRefcon)
{
  PyObject *object = getPtrRefOneshot(inObject, objRefName);
  PyObject *loaderDictKey = PyLong_FromVoidPtr(inRefcon);
  PyObject *loaderCallbackInfo = PyDict_GetItem(loaderDict, loaderDictKey);
  if(loaderCallbackInfo == NULL){
    printf("Unknown callback requested in objectLoaded(%p).\n", inRefcon);
    return;
  }
  set_moduleName(PyTuple_GetItem(loaderCallbackInfo, LOADER_MODULE_NAME));
  PyObject *res = PyObject_CallFunctionObjArgs(PyTuple_GetItem(loaderCallbackInfo, LOADER_CALLBACK),
                                           object, PyTuple_GetItem(loaderCallbackInfo, LOADER_REFCON), NULL);
  PyObject *err = PyErr_Occurred();
  if(err){
    printf("Error occured during the objectLoaded callback(inRefcon = %p):\n", inRefcon);
    pythonLogException();
  }else{
    Py_DECREF(res);
  }
  PyDict_DelItem(loaderDict, loaderDictKey);
  Py_DECREF(loaderDictKey);
  Py_DECREF(object);
}


My_DOCSTR(_loadObjectAsync__doc__, "loadObjectAsync", "path, loaded, refCon=None",
          "Loads OBJ asynchronously, calling callback on completion.\n"
          "\n"
          "Callback signature is loaded(objecRef, refCon)\n"
          "Object path is absolute or relative X-Plane root.");
static PyObject *XPLMLoadObjectAsyncFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"path", "loaded", "refCon", NULL};
  (void)self;
  if(!XPLMLoadObjectAsync_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMLoadObjectAsync is available only in XPLM210 and up.");
    return NULL;
  }
  const char *inPath;
  PyObject *callback, *inRefcon=Py_None;

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "sO|O", keywords, &inPath, &callback, &inRefcon)) {
    return NULL;
  }

  PyObject *argsObj = Py_BuildValue("(sOOs)", inPath, callback, inRefcon, CurrentPythonModuleName);

  void *refcon = (void *)++loaderCntr;
  PyObject *key = PyLong_FromVoidPtr(refcon);
  PyDict_SetItem(loaderDict, key, argsObj);
  Py_DECREF(key);
  XPLMLoadObjectAsync_ptr(inPath, genericObjectLoaded, refcon); /* path, callback, refcon */
  Py_DECREF(argsObj);
  Py_RETURN_NONE;
}


My_DOCSTR(_unloadObject__doc__, "unloadObject", "objectRef",
          "Unloads objectRef");
static PyObject *XPLMUnloadObjectFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"objectRef", NULL};
  (void)self;

  PyObject *objectRef;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &objectRef)) {
    return NULL;
  }

  XPLMObjectRef inObject = refToPtr(objectRef, objRefName);
  XPLMUnloadObject(inObject);
  Py_RETURN_NONE;
}

static PyObject *libEnumDict;
#define LIBRARY_PLUGIN 0
#define LIBRARY_PATH 1
#define LIBRARY_LATITUDE 2
#define LIBRARY_LONGITUDE 3
#define LIBRARY_CALLBACK 4
#define LIBRARY_REFCON 5
#define LIBRARY_MODULE_NAME 6
static intptr_t libEnumCntr;

static void libraryEnumerator(const char *inFilePath, void *inRef)
{
  PyObject *pID = PyLong_FromVoidPtr(inRef);
  PyObject *libEnumCallbackInfo = PyDict_GetItem(libEnumDict, pID);
  Py_XDECREF(pID);
  if(libEnumCallbackInfo == NULL){
    printf("Unknown callback requested from libraryEnumerator(%p).\n", inRef);
    return;
  }
  set_moduleName(PyTuple_GetItem(libEnumCallbackInfo, LIBRARY_MODULE_NAME));
  PyObject *res = PyObject_CallFunction(PyTuple_GetItem(libEnumCallbackInfo, LIBRARY_CALLBACK), "(sO)",
                                        inFilePath, PyTuple_GetItem(libEnumCallbackInfo, LIBRARY_REFCON));
  Py_XDECREF(res);
}

My_DOCSTR(_lookupObjects__doc__, "lookupObjects", "path, latitude=0.0, longitude=0.0, enumerator=None, refCon=None",
          "Converts virtual path to file paths, calling enumerator with info\n"
          "\n"
          "Path is virual path, which may have zero or more matching file paths\n"
          "in library. Count of results is returned by lookupObjects().\n"
          "For each item found, enumerator(path, refCon) is called.");
static PyObject *XPLMLookupObjectsFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"path", "latitude", "longitude", "enumerator", "refCon", NULL};
  (void)self;
  const char *inPath;
  float inLatitude=0.0, inLongitude=0.0;
  PyObject *enumerator=Py_None;
  PyObject *ref=Py_None;
  PyObject *pluginSelf;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "s|ffOO", keywords, &inPath, &inLatitude, &inLongitude, &enumerator, &ref)) {
    return NULL;
  }
  if (!PyCallable_Check(enumerator)) {
    PyErr_SetString(PyExc_RuntimeError , "lookupObject() enumerator callback not callable function.");
    return NULL;
  }

  pluginSelf = get_moduleName_p();
  void *myRef = (void *)++libEnumCntr;
  PyObject *refObj = PyLong_FromVoidPtr(myRef);
  
  PyObject *argsObj = Py_BuildValue("(OsffOOs)", pluginSelf, inPath, inLatitude, inLongitude, enumerator, ref, CurrentPythonModuleName);
  PyDict_SetItem(libEnumDict, refObj, argsObj);
  Py_DECREF(argsObj);
  Py_XDECREF(refObj);
  int res = XPLMLookupObjects(inPath, inLatitude, inLongitude, libraryEnumerator, myRef);
  return PyLong_FromLong(res);
}

static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  PyDict_Clear(loaderDict);
  Py_DECREF(loaderDict);
  PyDict_Clear(libEnumDict);
  Py_DECREF(libEnumDict);
  Py_RETURN_NONE;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
static PyMethodDef XPLMSceneryMethods[] = {
  {"createProbe", (PyCFunction)XPLMCreateProbeFun, METH_VARARGS | METH_KEYWORDS, _createProbe__doc__},
  {"XPLMCreateProbe", (PyCFunction)XPLMCreateProbeFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"destroyProbe", (PyCFunction)XPLMDestroyProbeFun, METH_VARARGS | METH_KEYWORDS, _destroyProbe__doc__},
  {"XPLMDestroyProbe", (PyCFunction)XPLMDestroyProbeFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"probeTerrainXYZ", (PyCFunction)XPLMProbeTerrainXYZFun, METH_VARARGS | METH_KEYWORDS, _probeTerrainXYZ__doc__},
  {"XPLMProbeTerrainXYZ", (PyCFunction)XPLMProbeTerrainXYZFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"degMagneticToDegTrue", (PyCFunction)XPLMDegMagneticToDegTrueFun, METH_VARARGS | METH_KEYWORDS, _degMagneticToDegTrue__doc__},
  {"XPLMDegMagneticToDegTrue", (PyCFunction)XPLMDegMagneticToDegTrueFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"getMagneticVariation", (PyCFunction)XPLMGetMagneticVariationFun, METH_VARARGS | METH_KEYWORDS, _getMagneticVariation__doc__},
  {"XPLMGetMagneticVariation", (PyCFunction)XPLMGetMagneticVariationFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"degTrueToDegMagnetic", (PyCFunction)XPLMDegTrueToDegMagneticFun, METH_VARARGS | METH_KEYWORDS, _degTrueToDegMagnetic__doc__},
  {"XPLMDegTrueToDegMagnetic", (PyCFunction)XPLMDegTrueToDegMagneticFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"loadObject", (PyCFunction)XPLMLoadObjectFun, METH_VARARGS | METH_KEYWORDS, _loadObject__doc__},
  {"XPLMLoadObject", (PyCFunction)XPLMLoadObjectFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"loadObjectAsync", (PyCFunction)XPLMLoadObjectAsyncFun, METH_VARARGS | METH_KEYWORDS, _loadObjectAsync__doc__},
  {"XPLMLoadObjectAsync", (PyCFunction)XPLMLoadObjectAsyncFun, METH_VARARGS | METH_KEYWORDS, ""},
#if defined(XPLM_DEPRECATED)
  {"drawObjects", (PyCFunction)XPLMDrawObjectsFun, METH_VARARGS | METH_KEYWORDS, _drawObjects__doc__},
  {"XPLMDrawObjects", (PyCFunction)XPLMDrawObjectsFun, METH_VARARGS | METH_KEYWORDS, ""},
#endif
  {"unloadObject", (PyCFunction)XPLMUnloadObjectFun, METH_VARARGS | METH_KEYWORDS, _unloadObject__doc__},
  {"XPLMUnloadObject", (PyCFunction)XPLMUnloadObjectFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"lookupObjects", (PyCFunction)XPLMLookupObjectsFun, METH_VARARGS | METH_KEYWORDS, _lookupObjects__doc__},
  {"XPLMLookupObjects", (PyCFunction)XPLMLookupObjectsFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"_cleanup", cleanup, METH_VARARGS, ""},
  {NULL, NULL, 0, NULL}
};
#pragma GCC diagnostic pop


static struct PyModuleDef XPLMSceneryModule = {
  PyModuleDef_HEAD_INIT,
  "XPLMScenery",
  "Laminar documentation: \n"
  "   https://developer.x-plane.com/sdk/XPLMScenery/\n"
  "XPPython3 documentation: \n"
  "   https://xppython3.rtfd.io/en/stable/development/modules/scenery.html",
  -1,
  XPLMSceneryMethods,
  NULL,
  NULL,
  NULL,
  NULL
};

PyMODINIT_FUNC
PyInit_XPLMScenery(void)
{
  if(!(loaderDict = PyDict_New())){
    return NULL;
  }
  if(!(libEnumDict = PyDict_New())){
    return NULL;
  }

  PyObject *mod = PyModule_Create(&XPLMSceneryModule);
  if(mod){
    PyModule_AddStringConstant(mod, "__author__", "Peter Buckner (pbuck@avnwx.com)");
    PyModule_AddIntConstant(mod, "xplm_ProbeY", xplm_ProbeY);
    PyModule_AddIntConstant(mod, "xplm_ProbeHitTerrain", xplm_ProbeHitTerrain);
    PyModule_AddIntConstant(mod, "xplm_ProbeError", xplm_ProbeError);
    PyModule_AddIntConstant(mod, "xplm_ProbeMissed", xplm_ProbeMissed );

    PyModule_AddIntConstant(mod, "ProbeY", xplm_ProbeY);
    PyModule_AddIntConstant(mod, "ProbeHitTerrain", xplm_ProbeHitTerrain);
    PyModule_AddIntConstant(mod, "ProbeError", xplm_ProbeError);
    PyModule_AddIntConstant(mod, "ProbeMissed", xplm_ProbeMissed );
  }

  return mod;
}




