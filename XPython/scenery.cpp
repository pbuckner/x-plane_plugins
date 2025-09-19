#define _GNU_SOURCE 1
#include <Python.h>
#include <sys/time.h>
#include <stdio.h>
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMScenery.h>
#include <vector>
#include <string>
#include <unordered_map>
#include "utils.h"
#include "plugin_dl.h"
#include "xppythontypes.h"
#include "cpp_utilities.hpp"
#include "capsules.h"

struct LoaderInfo {
  std::string module_name;
  PyObject *refCon;
  PyObject *callback;
  std::string path;
};
static std::unordered_map<intptr_t, LoaderInfo> objectLoadCallbacks;
static intptr_t loaderCntr;
static void genericObjectLoaded(XPLMObjectRef inObject, void *inRefcon);

My_DOCSTR(_createProbe__doc__, "createProbe",
          "probeType=0",
          "probeType:XPLMProbeType=ProbeY",
          "XPLMProbeRef",
          "Return a probeRef");
static PyObject *XPLMCreateProbeFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  std::vector<std::string> params = {"probeType"};
  char **keywords = stringVectorToCharArray(params);
  (void) self;
  int inProbeType = xplm_ProbeY;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|i", keywords, &inProbeType)){
    freeCharArray(keywords, params.size());
    return NULL;
  }
  freeCharArray(keywords, params.size());
  return makeCapsule(XPLMCreateProbe(inProbeType), "XPLMProbeRef");
}

My_DOCSTR(_destroyProbe__doc__, "destroyProbe",
          "probe",
          "probe:XPLMProbeRef",
          "None",
          "Destroy a probeRef");
static PyObject *XPLMDestroyProbeFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  std::vector<std::string> params = {"probe"};
  char **keywords = stringVectorToCharArray(params);
  (void) self;
  PyObject *inProbe = NULL;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &inProbe)){
    freeCharArray(keywords, params.size());
    return NULL;
  }
  freeCharArray(keywords, params.size());
  XPLMDestroyProbe(getVoidPtr(inProbe, "XPLMProbeRef"));
  Py_RETURN_NONE;
}

My_DOCSTR(_probeTerrainXYZ__doc__, "probeTerrainXYZ",
          "probeRef, x, y, z",
          "probeRef:XPLMProbeRef, x:float, y:float, z:float",
          "XPLMProbeInfo_t",
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
  std::vector<std::string> params = {"probeRef", "x", "y", "z"};
  char **keywords = stringVectorToCharArray(params);
  (void) self;
  PyObject *probe;
  float inX, inY, inZ;

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Offf", keywords, &probe, &inX, &inY, &inZ)){
    freeCharArray(keywords, params.size());
    return NULL;
  }
  freeCharArray(keywords, params.size());

  XPLMProbeRef inProbe = getVoidPtr(probe, "XPLMProbeRef");
  XPLMProbeInfo_t info;
  info.structSize = sizeof(info);
  XPLMProbeResult res = XPLMProbeTerrainXYZ(inProbe, inX, inY, inZ, &info);

  PyObject *ret;
  ret = PyProbeInfo_New(res, info.locationX, info.locationY, info.locationZ,
                           info.normalX, info.normalY, info.normalZ,
                           info.velocityX, info.velocityY, info.velocityZ, info.is_wet);
  return ret;
}

My_DOCSTR(_getMagneticVariation__doc__, "getMagneticVariation",
          "latitude, longitude",
          "latitude:float, longitude:float",
          "float",
          "Magnetic declination at point");
static PyObject *XPLMGetMagneticVariationFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  std::vector<std::string> params = {"latitude", "longitude"};
  char **keywords = stringVectorToCharArray(params);
  (void)self;
  if(!XPLMGetMagneticVariation_ptr){
    freeCharArray(keywords, params.size());
    PyErr_SetString(PyExc_RuntimeError , "XPLMGetMagneticVariation is available only in XPLM300 and up.");
    return NULL;
  }
  double latitude, longitude;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "dd", keywords, &latitude, &longitude)){
    freeCharArray(keywords, params.size());
    return NULL;
  }
  freeCharArray(keywords, params.size());
  return PyFloat_FromDouble(XPLMGetMagneticVariation_ptr(latitude, longitude));
}

My_DOCSTR(_degTrueToDegMagnetic__doc__, "degTrueToDegMagnetic",
          "degrees=0.0",
          "degrees:float=0.0",
          "float",
          "Convert degrees True to degrees Magnetic, at user's current location");
static PyObject *XPLMDegTrueToDegMagneticFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  std::vector<std::string> params = {"degrees"};
  char **keywords = stringVectorToCharArray(params);
  (void)self;
  if(!XPLMDegTrueToDegMagnetic_ptr){
    freeCharArray(keywords, params.size());
    PyErr_SetString(PyExc_RuntimeError , "XPLMDegTrueToDegMagnetic is available only in XPLM300 and up.");
    return NULL;
  }
  float headingDegreesTrue = 0.0;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|f", keywords, &headingDegreesTrue)){
    freeCharArray(keywords, params.size());
    return NULL;
  }
  freeCharArray(keywords, params.size());
  return PyFloat_FromDouble(XPLMDegTrueToDegMagnetic_ptr(headingDegreesTrue));
}

My_DOCSTR(_degMagneticToDegTrue__doc__, "degMagneticToDegTrue",
          "degrees=0.0",
          "degrees:float=0.0",
          "float",
          "Convert degrees Magnetic to degrees True, at user's current location");
static PyObject *XPLMDegMagneticToDegTrueFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  std::vector<std::string> params = {"degrees"};
  char **keywords = stringVectorToCharArray(params);
  (void)self;
  if(!XPLMDegMagneticToDegTrue_ptr){
    freeCharArray(keywords, params.size());
    PyErr_SetString(PyExc_RuntimeError , "XPLMDegMagneticToDegTrue is available only in XPLM300 and up.");
    return NULL;
  }
  float headingDegreesMagnetic=0.0;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|f", keywords, &headingDegreesMagnetic)){
    freeCharArray(keywords, params.size());
    return NULL;
  }
  freeCharArray(keywords, params.size());
  return PyFloat_FromDouble(XPLMDegMagneticToDegTrue_ptr(headingDegreesMagnetic));
}


My_DOCSTR(_loadObject__doc__, "loadObject",
          "path",
          "path:str",
          "XPLMObjectRef",
          "Load OBJ file from path, returning objectRef\n"
          "\n"
          "Path may be absolute, or relative X-Plane Root");
static PyObject *XPLMLoadObjectFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  std::vector<std::string> params = {"path"};
  char **keywords = stringVectorToCharArray(params);
  (void)self;
  const char *inPath;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "s", keywords, &inPath)){
    freeCharArray(keywords, params.size());
    return NULL;
  }
  freeCharArray(keywords, params.size());
  XPLMObjectRef res = XPLMLoadObject(inPath);
  return makeCapsule(res, "XPLMObjectRef");
}



static void genericObjectLoaded(XPLMObjectRef inObject, void *inRefcon)
{
  intptr_t refcon_id = (intptr_t)inRefcon;
  auto it = objectLoadCallbacks.find(refcon_id);
  if (it == objectLoadCallbacks.end()) {
    printf("Unknown callback requested in objectLoaded(%p).\n", inRefcon);
    return;
  }

  LoaderInfo& info = it->second;
  PyObject *module_name_obj = PyUnicode_FromString(info.module_name.c_str());
  set_moduleName(module_name_obj);
  Py_DECREF(module_name_obj);

  PyObject *object = makeCapsule(inObject, "XPLMObjectRef");
  PyObject *res = PyObject_CallFunctionObjArgs(info.callback, object, info.refCon, NULL);
  PyObject *err = PyErr_Occurred();
  if(err){
    printf("Error occured during the objectLoaded callback(inRefcon = %p):\n", inRefcon);
    pythonLogException();
  }else{
    Py_DECREF(res);
  }

  // Cleanup
  Py_DECREF(info.callback);
  Py_DECREF(info.refCon);
  objectLoadCallbacks.erase(it);
  Py_DECREF(object);
}


My_DOCSTR(_loadObjectAsync__doc__, "loadObjectAsync",
          "path, loaded, refCon=None",
          "path:str, loaded:Callable[[XPLMObjectRef, Any], None], refCon:Any=None",
          "None",
          "Loads OBJ asynchronously, calling callback on completion.\n"
          "\n"
          "Callback signature is loaded(objecRef, refCon)\n"
          "Object path is absolute or relative X-Plane root.");
static PyObject *XPLMLoadObjectAsyncFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  std::vector<std::string> params = {"path", "loaded", "refCon"};
  char **keywords = stringVectorToCharArray(params);
  (void)self;
  if(!XPLMLoadObjectAsync_ptr){
    freeCharArray(keywords, params.size());
    PyErr_SetString(PyExc_RuntimeError , "XPLMLoadObjectAsync is available only in XPLM210 and up.");
    return NULL;
  }
  const char *inPath;
  PyObject *callback, *inRefcon=Py_None;

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "sO|O", keywords, &inPath, &callback, &inRefcon)) {
    freeCharArray(keywords, params.size());
    return NULL;
  }
  freeCharArray(keywords, params.size());

  intptr_t refcon = ++loaderCntr;
  objectLoadCallbacks[refcon] = {
    .module_name = std::string(CurrentPythonModuleName),
    .refCon = inRefcon,
    .callback = callback,
    .path = std::string(inPath)
  };
  Py_INCREF(callback);
  Py_INCREF(inRefcon);
  XPLMLoadObjectAsync_ptr(inPath, genericObjectLoaded, (void*)refcon); /* path, callback, refcon */
  Py_RETURN_NONE;
}


My_DOCSTR(_unloadObject__doc__, "unloadObject",
          "objectRef",
          "objectRef:XPLMObjectRef",
          "None",
          "Unloads objectRef");
static PyObject *XPLMUnloadObjectFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  std::vector<std::string> params = {"objectRef"};
  char **keywords = stringVectorToCharArray(params);
  (void)self;

  PyObject *objectRef;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &objectRef)) {
    freeCharArray(keywords, params.size());
    return NULL;
  }
  freeCharArray(keywords, params.size());

  XPLMObjectRef inObject = getVoidPtr(objectRef, "XPLMObjectRef");
  XPLMUnloadObject(inObject);
  deleteCapsule(objectRef);
  
  Py_RETURN_NONE;
}

struct LibraryInfo {
  PyObject *plugin;
  std::string path;
  float latitude;
  float longitude;
  PyObject *callback;
  PyObject *refCon;
  std::string module_name;
};

static std::unordered_map<intptr_t, LibraryInfo> libraryEnumCallbacks;
static intptr_t libEnumCntr;

static void libraryEnumerator(const char *inFilePath, void *inRef)
{
  intptr_t refcon_id = (intptr_t)inRef;
  auto it = libraryEnumCallbacks.find(refcon_id);
  if (it == libraryEnumCallbacks.end()) {
    printf("Unknown callback requested from libraryEnumerator(%p).\n", inRef);
    return;
  }

  LibraryInfo& info = it->second;
  PyObject *module_name_obj = PyUnicode_FromString(info.module_name.c_str());
  set_moduleName(module_name_obj);
  Py_DECREF(module_name_obj);

  PyObject *res = PyObject_CallFunction(info.callback, "(sO)", inFilePath, info.refCon);
  Py_XDECREF(res);
}

My_DOCSTR(_lookupObjects__doc__, "lookupObjects",
          "path, latitude=0.0, longitude=0.0, enumerator=None, refCon=None",
          "path:str, latitude:float=0.0, longitude:float=0.0, "
          "enumerator:Optional[Callable[[str, Any], None]]=None, refCon:Any=None",
          "int",
          "Converts virtual path to file paths, calling enumerator with info\n"
          "\n"
          "Path is virual path, which may have zero or more matching file paths\n"
          "in library. Count of results is returned by lookupObjects().\n"
          "For each item found, enumerator(path, refCon) is called.");
static PyObject *XPLMLookupObjectsFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  std::vector<std::string> params = {"path", "latitude", "longitude", "enumerator", "refCon"};
  char **keywords = stringVectorToCharArray(params);
  (void)self;
  const char *inPath;
  float inLatitude=0.0, inLongitude=0.0;
  PyObject *enumerator=Py_None;
  PyObject *ref=Py_None;
  PyObject *pluginSelf;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "s|ffOO", keywords, &inPath, &inLatitude, &inLongitude, &enumerator, &ref)) {
    freeCharArray(keywords, params.size());
    return NULL;
  }
  freeCharArray(keywords, params.size());
  if (!PyCallable_Check(enumerator)) {
    PyErr_SetString(PyExc_RuntimeError , "lookupObject() enumerator callback not callable function.");
    return NULL;
  }

  pluginSelf = get_moduleName_p();
  intptr_t myRef = ++libEnumCntr;
  libraryEnumCallbacks[myRef] = {
    .plugin = pluginSelf,
    .path = std::string(inPath),
    .latitude = inLatitude,
    .longitude = inLongitude,
    .callback = enumerator,
    .refCon = ref,
    .module_name = std::string(CurrentPythonModuleName)
  };
  Py_INCREF(pluginSelf);
  Py_INCREF(enumerator);
  Py_INCREF(ref);
  int res = XPLMLookupObjects(inPath, inLatitude, inLongitude, libraryEnumerator, (void*)myRef);
  return PyLong_FromLong(res);
}

static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  for (auto& pair : objectLoadCallbacks) {
    Py_DECREF(pair.second.callback);
    Py_DECREF(pair.second.refCon);
  }
  objectLoadCallbacks.clear();
  for (auto& pair : libraryEnumCallbacks) {
    Py_DECREF(pair.second.plugin);
    Py_DECREF(pair.second.callback);
    Py_DECREF(pair.second.refCon);
  }
  libraryEnumCallbacks.clear();
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
  PyObject *mod = PyModule_Create(&XPLMSceneryModule);
  if(mod){
    PyModule_AddStringConstant(mod, "__author__", "Peter Buckner (pbuck@xppython3.org)");
    PyModule_AddIntConstant(mod, "xplm_ProbeY", xplm_ProbeY);  // XPLMProbeType
    PyModule_AddIntConstant(mod, "xplm_ProbeHitTerrain", xplm_ProbeHitTerrain);  // XPLMProbeResult
    PyModule_AddIntConstant(mod, "xplm_ProbeError", xplm_ProbeError);  // XPLMProbeResult
    PyModule_AddIntConstant(mod, "xplm_ProbeMissed", xplm_ProbeMissed );  // XPLMProbeResult

    PyModule_AddIntConstant(mod, "ProbeY", xplm_ProbeY);  // XPLMProbeType
    PyModule_AddIntConstant(mod, "ProbeHitTerrain", xplm_ProbeHitTerrain);  // XPLMProbeResult
    PyModule_AddIntConstant(mod, "ProbeError", xplm_ProbeError);  // XPLMProbeResult
    PyModule_AddIntConstant(mod, "ProbeMissed", xplm_ProbeMissed );  // XPLMProbeResult
  }

  return mod;
}




