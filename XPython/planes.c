#define _GNU_SOURCE 1
#include <Python.h>
#include <sys/time.h>
#include <stdio.h>
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMPlanes.h>
#include "utils.h"
#include "plugin_dl.h"

static intptr_t availableCntr;
static PyObject *availableDict;
#define AVAILABLE_PLUGIN 0
#define AVAILABLE_AIRCRAFT 1
#define AVAILABLE_CALLBACK 2
#define AVAILABLE_REFCON 3
#define AVAILABLE_MODULE_NAME 4

My_DOCSTR(_setUsersAircraft__doc__, "setUsersAircraft",
          "path",
          "path:str",
          "None",
          "Change the user's aircraft and reinitialize.\n"
          "\n"
          "path is either relative X-Plane root, or fully qualified,\n"
          "including the .acf extension.");
static PyObject *XPLMSetUsersAircraftFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"path", NULL};
  (void)self;
  const char *inAircraftPath;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "s", keywords, &inAircraftPath)){
    return NULL;
  }
  XPLMSetUsersAircraft(inAircraftPath);
  Py_RETURN_NONE;
}

My_DOCSTR(_placeUserAtAirport__doc__, "placeUserAtAirport",
          "code",
          "code:str",
          "None",
          "Place user at given airport, specified by ICAO code.\n"
          "\n"
          "Invalid airport code will crash the sim.");
static PyObject *XPLMPlaceUserAtAirportFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"code", NULL};
  (void)self;
  const char *inAirportCode;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "s", keywords, &inAirportCode)){
    return NULL;
  }
  XPLMPlaceUserAtAirport(inAirportCode);
  Py_RETURN_NONE;
}

My_DOCSTR(_placeUserAtLocation__doc__, "placeUserAtLocation",
          "latitude, longitude, elevation, heading, speed",
          "latitude:float, longitude:float, elevation:float, heading:float, speed:float",
          "None",
          "Place user aircraft at location with engines running.\n"
          "\n"
          "elevation is meters, heading is True, speed is meters per second");
static PyObject *XPLMPlaceUserAtLocationFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"latitude", "longitude", "elevation", "heading", "speed", NULL};
  (void)self;
  double latitudeDegrees, longitudeDegrees;
  float elevationMetersMSL, headingDegreesTrue, speedMetersPerSecond;
  if(!XPLMPlaceUserAtLocation_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMPlaceUserAtLocation is available only in XPLM300 and up.");
    return NULL;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "ddfff", keywords, &latitudeDegrees, &longitudeDegrees,
                       &elevationMetersMSL, &headingDegreesTrue, &speedMetersPerSecond)){
    return NULL;
  }
  XPLMPlaceUserAtLocation_ptr(latitudeDegrees, longitudeDegrees,
                              elevationMetersMSL, headingDegreesTrue, speedMetersPerSecond);
  Py_RETURN_NONE;
}

My_DOCSTR(_countAircraft__doc__, "countAircraft",
          "",
          "",
          "tuple[int, int, XPLMPluginID]",
          "Return tuple of (#a/c configured, #a/c active, pluginID of controlling plugin");
static PyObject *XPLMCountAircraftFun(PyObject *self, PyObject *args)
{
  (void)self;
  (void)args;
  int outTotalAircraft, outActiveAircraft;
  XPLMPluginID outController;

  XPLMCountAircraft(&outTotalAircraft, &outActiveAircraft, &outController);
  return Py_BuildValue("(iii)", outTotalAircraft, outActiveAircraft, outController);
}

My_DOCSTR(_getNthAircraftModel__doc__, "getNthAircraftModel",
          "index",
          "index:int",
          "tuple[str, str]",
          "Return (filename, fullPath) of aircraft at index");
static PyObject *XPLMGetNthAircraftModelFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"index", NULL};
  (void)self;
  int inIndex;
  char outFileName[512];
  char outPath[512];
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "i", keywords, &inIndex)){
    return NULL;
  }

  XPLMGetNthAircraftModel(inIndex, outFileName, outPath);
  return Py_BuildValue("(ss)", outFileName, outPath);
}

void planesAvailable(void *inRefcon)
{
  PyObject *pID = PyLong_FromVoidPtr(inRefcon);
  PyObject *callback = PyDict_GetItem(availableDict, pID);
  Py_XDECREF(pID);
  if(callback == NULL){
    printf("Unknown callback (%p) requested in planesAvailable.", inRefcon);
    return;
  }
  PyObject *func = PyTuple_GetItem(callback, AVAILABLE_CALLBACK);
  if (func == Py_None) return;
  set_moduleName(PyTuple_GetItem(callback, AVAILABLE_MODULE_NAME));
  PyObject *res = PyObject_CallFunctionObjArgs(func, PyTuple_GetItem(callback, AVAILABLE_REFCON), NULL);
  PyObject *err = PyErr_Occurred();
  if(err){
    printf("Error occured during the planesAvailable callback(inRefcon = %p):\n", inRefcon);
    pythonLogException();
  }
  Py_XDECREF(res);
}

My_DOCSTR(_acquirePlanes__doc__, "acquirePlanes",
          "aircraft=None, callback=None, refCon=None",
          "aircraft:Optional[Sequence[str]]=None, callback:Optional[Callable[[Any], None]]=None, refCon:Any=None",
          "int",
          "Get exclusive access to aircraft.\n"
          "\n"
          "1 on success, 0 otherwise. You callback will be called\n"
          "with refCon if acquirePlanes() is not immediately successful.");
static PyObject *XPLMAcquirePlanesFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"aircraft", "callback", "refCon", NULL};
  (void)self;
  PyObject *pluginSelf, *aircraft=Py_None, *inCallback=Py_None, *inRefcon=Py_None;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|OOO", keywords, &aircraft, &inCallback, &inRefcon)){
    return NULL;
  }
  pluginSelf = get_moduleName_p();
  int res;
  void *refcon = (void*)++availableCntr;
  PyObject *refObj = PyLong_FromVoidPtr(refcon);
  PyObject *argsObj = Py_BuildValue( "(OOOOs)", pluginSelf, aircraft, inCallback, inRefcon, CurrentPythonModuleName);
  PyDict_SetItem(availableDict, refObj, argsObj);
  Py_DECREF(argsObj);
  Py_DECREF(refObj);
  if(aircraft == Py_None){
    res = XPLMAcquirePlanes(NULL, planesAvailable, refcon);
  }else{
    Py_ssize_t len = PySequence_Length(aircraft);
    char **inAircraft = (char **)malloc((len + 1) * sizeof(char *));
    Py_ssize_t i;
    for(i = 0; i < len; ++i){
      PyObject *tmpItem = PySequence_GetItem(aircraft, i);
      PyObject *tmpStr = PyObject_Str(tmpItem);
      PyObject *tmpObj = PyUnicode_AsUTF8String(tmpStr);
      char *tmp = PyBytes_AsString(tmpObj);

      Py_DECREF(tmpItem);
      Py_DECREF(tmpStr);

      if (PyErr_Occurred()) return NULL;
      if(tmp[0] == '\0'){
        Py_DECREF(tmpObj);
        break;
      }else{
        inAircraft[i] = strdup(tmp);
      }
      Py_DECREF(tmpObj);
    }
    inAircraft[i] = NULL;
    res = XPLMAcquirePlanes(inAircraft, planesAvailable, refcon);
    i = 0;
    while(inAircraft[i]){
      free(inAircraft[i]);
      ++i;
    }
    free(inAircraft);
  }
  return PyLong_FromLong(res);
}





My_DOCSTR(_releasePlanes__doc__, "releasePlanes",
          "",
          "",
          "None",
          "Release all planes, acquired using acquirePlanes().");
static PyObject *XPLMReleasePlanesFun(PyObject *self, PyObject *args)
{
  (void)self;
  (void)args;
  XPLMReleasePlanes();
  Py_RETURN_NONE;
}

My_DOCSTR(_setActiveAircraftCount__doc__, "setActiveAircraftCount",
          "count",
          "count:int",
          "None",
          "Set number of active planes.\n"
          "\n"
          "Only sets up to number of available planes. You must\n"
          "first have access -- acquirePlanes().");
static PyObject *XPLMSetActiveAircraftCountFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"count", NULL};
  (void)self;
  int inCount;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "i", keywords, &inCount)){
    return NULL;
  }

  XPLMSetActiveAircraftCount(inCount);
  Py_RETURN_NONE;
}

My_DOCSTR(_setAircraftModel__doc__, "setAircraftModel",
          "index, path",
          "index:int, path:str",
          "None",
          "Load aircraft model into index. (Do not use index=0)\n"
          "\n"
          "Path is absolute, or relative to X-Plane root.");
static PyObject *XPLMSetAircraftModelFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"index", "path", NULL};
  (void)self;
  int inIndex;
  const char *inAircraftPath;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "is", keywords, &inIndex, &inAircraftPath)){
    return NULL;
  }
  XPLMSetAircraftModel(inIndex, inAircraftPath);
  Py_RETURN_NONE;
}

My_DOCSTR(_disableAIForPlane__doc__, "disableAIForPlane",
          "index",
          "index:int",
          "None",
          "Turn off AI for given airplane.\n"
          "\n"
          "Plane will continue to draw, but will not move itself.");
static PyObject *XPLMDisableAIForPlaneFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"index", NULL};
  (void)self;
  int inPlaneIndex;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "i", keywords, &inPlaneIndex)){
    return NULL;
  }

  XPLMDisableAIForPlane(inPlaneIndex);
  Py_RETURN_NONE;
}

static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  PyDict_Clear(availableDict);
  Py_DECREF(availableDict);
  Py_RETURN_NONE;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
static PyMethodDef XPLMPlanesMethods[] = {
  {"setUsersAircraft", (PyCFunction)XPLMSetUsersAircraftFun, METH_VARARGS | METH_KEYWORDS, _setUsersAircraft__doc__},
  {"XPLMSetUsersAircraft", (PyCFunction)XPLMSetUsersAircraftFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"placeUserAtAirport", (PyCFunction)XPLMPlaceUserAtAirportFun, METH_VARARGS | METH_KEYWORDS, _placeUserAtAirport__doc__},
  {"XPLMPlaceUserAtAirport", (PyCFunction)XPLMPlaceUserAtAirportFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"placeUserAtLocation", (PyCFunction)XPLMPlaceUserAtLocationFun, METH_VARARGS | METH_KEYWORDS, _placeUserAtLocation__doc__},
  {"XPLMPlaceUserAtLocation", (PyCFunction)XPLMPlaceUserAtLocationFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"countAircraft", (PyCFunction)XPLMCountAircraftFun, METH_VARARGS, _countAircraft__doc__},
  {"XPLMCountAircraft", (PyCFunction)XPLMCountAircraftFun, METH_VARARGS, ""},
  {"getNthAircraftModel", (PyCFunction)XPLMGetNthAircraftModelFun, METH_VARARGS | METH_KEYWORDS, _getNthAircraftModel__doc__},
  {"XPLMGetNthAircraftModel", (PyCFunction)XPLMGetNthAircraftModelFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"releasePlanes", (PyCFunction)XPLMReleasePlanesFun, METH_VARARGS, _releasePlanes__doc__},
  {"XPLMReleasePlanes", (PyCFunction)XPLMReleasePlanesFun, METH_VARARGS, ""},
  {"setActiveAircraftCount", (PyCFunction)XPLMSetActiveAircraftCountFun, METH_VARARGS | METH_KEYWORDS, _setActiveAircraftCount__doc__},
  {"XPLMSetActiveAircraftCount", (PyCFunction)XPLMSetActiveAircraftCountFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"acquirePlanes", (PyCFunction)XPLMAcquirePlanesFun, METH_VARARGS | METH_KEYWORDS, _acquirePlanes__doc__},
  {"XPLMAcquirePlanes", (PyCFunction)XPLMAcquirePlanesFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"setAircraftModel", (PyCFunction)XPLMSetAircraftModelFun, METH_VARARGS | METH_KEYWORDS, _setAircraftModel__doc__},
  {"XPLMSetAircraftModel", (PyCFunction)XPLMSetAircraftModelFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"disableAIForPlane", (PyCFunction)XPLMDisableAIForPlaneFun, METH_VARARGS | METH_KEYWORDS, _disableAIForPlane__doc__},
  {"XPLMDisableAIForPlane", (PyCFunction)XPLMDisableAIForPlaneFun, METH_VARARGS | METH_KEYWORDS, ""},
#if defined(XPLM_DEPRECATED)
  {"drawAircraft", (PyCFunction)XPLMDrawAircraftFun, METH_VARARGS | METH_KEYWORDS, _drawAircraft__doc__},
  {"XPLMDrawAircraft", (PyCFunction)XPLMDrawAircraftFun, METH_VARARGS | METH_KEYWORDS, ""},
#endif
#if defined(XPLM_DEPRECATED)
  {"reinitUsersPlane", (PyCFunction)XPLMReinitUsersPlaneFun, METH_VARARGS | METH_KEYWORDS, _reinitUsersPlane__doc__},
  {"XPLMReinitUsersPlane", (PyCFunction)XPLMReinitUsersPlaneFun, METH_VARARGS | METH_KEYWORDS, ""},
#endif
  {"_cleanup", cleanup, METH_VARARGS, ""},
  {NULL, NULL, 0, NULL}
};
#pragma GCC diagnostic pop


static struct PyModuleDef XPLMPlanesModule = {
  PyModuleDef_HEAD_INIT,
  "XPLMPlanes",
  "Laminar documentation: \n"
  "   https://developer.x-plane.com/sdk/XPLMPlanes/\n"
  "XPPython3 documentation: \n"
  "   https://xppython3.rtfd.io/en/stable/development/modules/planes.html",
  -1,
  XPLMPlanesMethods,
  NULL,
  NULL,
  NULL,
  NULL
};

PyMODINIT_FUNC
PyInit_XPLMPlanes(void)
{
  if(!(availableDict = PyDict_New())){
    return NULL;
  }
  PyObject *mod = PyModule_Create(&XPLMPlanesModule);
  if(mod){
    PyModule_AddStringConstant(mod, "__author__", "Peter Buckner (pbuck@xppython3.org)");
    PyModule_AddIntConstant(mod, "XPLM_USER_AIRCRAFT", XPLM_USER_AIRCRAFT);
    PyModule_AddIntConstant(mod, "USER_AIRCRAFT", XPLM_USER_AIRCRAFT);
  }

  return mod;
}



