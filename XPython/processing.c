#define _GNU_SOURCE 1
#include <Python.h>
#include <sys/time.h>
#include <stdio.h>
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMProcessing.h>
#include "plugin_dl.h"
#include "utils.h"
#include "xppython.h"
#include "processing.h"

static intptr_t flCntr;
/* the 'key' to flDict, we use as the refCon for genericFlightLoopCallback function.
   When genericFlightLoopCallback function is _called_ by X-Plane, we:
   1) take provided refCon
   2) lookup info in flDict to get "real" callback & refcon
*/
   
static PyObject *flDict;  /* {
                               flCntr1: ([0]callback, [1]refCon, [2]moduleName, [3]type),
                               flCntr2: ([0]callback, [1]refCon, [2]moduleName, [3]type),
                             } */
#define FLIGHTLOOP_CALLBACK 0
#define FLIGHTLOOP_REFCON 1
#define FLIGHTLOOP_MODULE_NAME 2
#define FLIGHTLOOP_TYPE 3  /*  0 == "old style -- Register/Unregister/SetFlightLoopCallbackInterval" 
                               1 == "new style -- Create/Schedule/Destroy;*/

static PyObject *flRevDict; /*{
                               ([0]callback, [1]refCon, [2]moduleName): flCntr1 (key into flDict),  # type == 0
                               <Capsule XPLMFlightLoopID> x:            flCntr2 (key into flDict),  # type == 1
                              }*/
#define FLIGHTLOOP_REV_CALLBACK 0
#define FLIGHTLOOP_REV_REFCON 1
#define FLIGHTLOOP_REV_MODULE_NAME 2

static const char flIDRef[] = "XPLMFlightLoopID";
static float genericFlightLoopCallback(float inElapsedSinceLastCall, float inElapsedTimeSinceLastFlightLoop, 
                                       int counter, void * inRefcon);

void resetFlightLoops()
{
  /* flDict & flRevDict:
   * unregisters all flight loops by going through flDict
   * Then, at the end, clears both dicts
   */
  PyObject *keys = PyDict_Keys(flRevDict);  /* new */
  PyObject *iterator = PyObject_GetIter(keys); /* new */
  PyObject *flRevDictKey;
  while((flRevDictKey = PyIter_Next(iterator))) {  /* new */
    PyObject *flDictKey = PyDict_GetItem(flRevDict, flRevDictKey); /* key into flDict */ /* borrowed */
    PyObject *flInfo = PyDict_GetItem(flDict, flDictKey); /* borrowed */

    char *s = objToStr(PyTuple_GetItem(flInfo, FLIGHTLOOP_MODULE_NAME)); /* borrowed */
    pythonLog("[XPPython3] Reload --     (%s)\n", s);
    free(s);
    PyObject *flightloop_type = PyTuple_GetItem(flInfo, FLIGHTLOOP_TYPE);  /* borrowed */
    int fl_type = PyLong_AsLong(flightloop_type);

    if(fl_type == 0) {
      XPLMUnregisterFlightLoopCallback(genericFlightLoopCallback, PyLong_AsVoidPtr(flDictKey));
    } else {
      XPLMDestroyFlightLoop(refToPtr(flRevDictKey, "XPLMFlightLoopID"));
    }
    Py_DECREF(flRevDictKey);
  }
  Py_DECREF(keys);
  Py_DECREF(iterator);
  PyDict_Clear(flDict);
  PyDict_Clear(flRevDict);
}

static float genericFlightLoopCallback(float inElapsedSinceLastCall, float inElapsedTimeSinceLastFlightLoop, 
                                int counter, void * inRefcon)
{
  errCheck("prior flightLoop");
  struct timespec all_stop, all_start;
  clock_gettime(CLOCK_MONOTONIC, &all_start);

  PyObject *flDictKey = PyLong_FromVoidPtr(inRefcon);
  PyObject *flInfo = PyDict_GetItem(flDict, flDictKey);
  Py_XDECREF(flDictKey);
  if(flInfo == NULL){
    pythonDebug("Unknown flightLoop callback requested! (inRefcon = %p). Disabling it.\n", inRefcon);
    return 0.0;
  }
  errCheck("post callbackInfo");
  PyObject *inElapsedSinceLastCallObj = PyFloat_FromDouble(inElapsedSinceLastCall);
  PyObject *inElapsedTimeSinceLastFlightLoopObj = PyFloat_FromDouble(inElapsedTimeSinceLastFlightLoop);
  PyObject *counterObj = PyLong_FromLong(counter);

  /* vvvvvvvvvvvvvvvvv */
  struct timespec stop, start;
  clock_gettime(CLOCK_MONOTONIC, &start);
  errCheck("before set_moduleName");

  set_moduleName(PyTuple_GetItem(flInfo, FLIGHTLOOP_MODULE_NAME));

  errCheck("after set_moduleName");
  PyObject *res = PyObject_CallFunctionObjArgs(PyTuple_GetItem(flInfo, FLIGHTLOOP_CALLBACK), inElapsedSinceLastCallObj,
                                               inElapsedTimeSinceLastFlightLoopObj, counterObj,
                                               PyTuple_GetItem(flInfo, FLIGHTLOOP_REFCON), NULL);
  clock_gettime(CLOCK_MONOTONIC, &stop);
  pluginStats[getPluginIndex(PyTuple_GetItem(flInfo, FLIGHTLOOP_MODULE_NAME))].fl_time += (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_nsec - start.tv_nsec) / 1000;
  /* ^^^^^^^^^^^^^^^^^ */
  
  float tmp;
  PyObject *err = PyErr_Occurred();
  Py_DECREF(inElapsedSinceLastCallObj);
  Py_DECREF(inElapsedTimeSinceLastFlightLoopObj);
  Py_DECREF(counterObj);
  if(err){
    pythonLogException();
    char *s = objToStr(PyTuple_GetItem(flInfo, FLIGHTLOOP_CALLBACK));
    pythonLog("[%s]: %s Error occured during the flightLoop callback (inRefcon = %p), disabling:\n",
              CurrentPythonModuleName, s, inRefcon);
    free(s);
    errCheck("opps, trying to log error failed");
    tmp = 0.0f;
  } else if (PyFloat_Check(res)) {
    tmp = PyFloat_AsDouble(res);
  } else if (PyLong_Check(res)) {
    tmp = PyLong_AsDouble(res);
  } else {
    char *s = objToStr(PyTuple_GetItem(flInfo, FLIGHTLOOP_CALLBACK));
    char *s2 = objToStr(res); 
    pythonLog("[%s]: %s Error occured during the flightLoop callback (inRefcon = %p), disabling: Bad return value '%s'\n",
              CurrentPythonModuleName, s, inRefcon, s2);
    free(s);
    free(s2);
    errCheck("opps, trying to log 'else' error failed");
    tmp = 0.0f;
  }
  Py_XDECREF(res);

  errCheck("end flight loop");
  clock_gettime(CLOCK_MONOTONIC, &all_stop);
  pluginStats[0].fl_time += (all_stop.tv_sec - all_start.tv_sec) * 1000000 + (all_stop.tv_nsec - all_start.tv_nsec) / 1000;
  errCheck("after second stats");
  return tmp;
}

/* static float flightLoopStats(float inElapsedSinceLastCall, float inElapsedTimeSinceLastFlightLoop, */
/*                              int counter, void *inRefcon) */
/* { */
/*   (void) inElapsedSinceLastCall; */
/*   (void) inElapsedTimeSinceLastFlightLoop; */
/*   (void) counter; */
/*   (void) inRefcon; */
/*   struct foobar { */
/*     char *name; */
/*     long time; */
/*   }; */
/*   struct foobar results[512]; */
/*   int maxY = 0; */
/*   for (int y = 0; y < flCntr; y++) { */
/*     results[y].name = NULL; */
/*     results[y].time = 0; */
/*   } */
/*   for (long x = 1; x <= (long) flCntr; x++) { */
/*     int y = 0; */
/*     for (; y < maxY; y++) { */
/*       /\* pythonLog("   results[%d]: %s, flInfo[%ld]: %s -- ", *\/ */
/*       /\*         y, *\/ */
/*       /\*         results[y].name, *\/ */
/*       /\*         x, *\/ */
/*       /\*         flInfo[x].name); *\/ */
/*       if (0 == strcmp(results[y].name, flInfo[x].name)) { */
/*         results[y].time += flInfo[x].time; */
/*         break; */
/*       } */
/*     } */
/*     if (y == maxY) { */
/*       results[maxY].name = flInfo[x].name; */
/*       results[maxY].time = flInfo[x].time; */
/*       maxY++; */
/*     } */
/*     /\* pythonLog("  @ %s: %ld usec \n", *\/ */
/*     /\*         flInfo[x].name, flInfo[x].time); *\/ */
/*     flInfo[x].time = 0; */
/*   } */
  
/*   pythonLog("-\n   all: %ld usec\n", flInfo[0].time); */
/*   flInfo[0].time = 0; */
/*   for (int y = 0; y < maxY; y++) { */
/*     if (results[y].time) { */
/*       pythonLog("   %s: %ld usec \n", */
/*               results[y].name, */
/*               results[y].time */
/*               ); */
/*     } */
/*   } */
/*   return -1.0; */
/* } */

My_DOCSTR(_getElapsedTime__doc__, "getElapsedTime", "",
          "Return elapsed time since sim started.");
static PyObject *XPLMGetElapsedTimeFun(PyObject *self, PyObject *args)
{
  (void)self;
  (void)args;
  float res = XPLMGetElapsedTime();
  return PyFloat_FromDouble(res);
}

My_DOCSTR(_getCycleNumber__doc__, "getCycleNumber", "",
          "Get cycle number, increased for each cycle computed by sim.");
static PyObject *XPLMGetCycleNumberFun(PyObject *self, PyObject *args)
{
  (void)self;
  (void)args;
  int res = XPLMGetCycleNumber();
  return PyLong_FromLong(res);
}



My_DOCSTR(_registerFlightLoopCallback__doc__, "registerFlightLoopCallback", "callback, interval=0.0, refCon=None",
          "Register flight look callback.\n"
          "\n"
          "interval indicates when you'll be called next:\n"
          "  0= deactivate \n"
          "  >0 seconds \n"
          "  <0 flightLoops\n"
          "Callback function gets (lastCall, elapsedTime, counter, refCon)");
static PyObject *XPLMRegisterFlightLoopCallbackFun(PyObject* self, PyObject *args, PyObject *kwargs)
{
  /* We're provided a python callback and (possibly) a refCon. We "know" the python module
     We'll register genericFlightLoopCallback, but instead of provided refCon, we pass
     in unique integer value which is the flDict key.

     Because we'll need to unregister it, we need to store the provided information
     into flRevDict-- the key to that dict as (callback, refCon, moduleName) with
     the value being the flDict key.. the unique integer created from flCntr.
   */

  static char *keywords[] = {"callback", "interval", "refCon", NULL};
  (void)self;
  PyObject *callback=Py_None, *refCon=Py_None;
  float interval=0.0;

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|fO", keywords, &callback, &interval, &refCon)){
    return NULL;
  }

  void *uniqueInt = (void *)++flCntr;
  PyObject *flDictKey = PyLong_FromVoidPtr(uniqueInt);

  PyObject *flInfo = Py_BuildValue("(OOsi)", callback, refCon, CurrentPythonModuleName, 0);
  PyDict_SetItem(flDict, flDictKey, flInfo);
  Py_DECREF(flInfo);

  PyObject *flRevDictKey = Py_BuildValue("(OOs)", callback, refCon, CurrentPythonModuleName);
  PyDict_SetItem(flRevDict, flRevDictKey, flDictKey);

  Py_DECREF(flDictKey);
  Py_DECREF(flRevDictKey);
  XPLMRegisterFlightLoopCallback(genericFlightLoopCallback, interval, uniqueInt);
  Py_RETURN_NONE;
}

My_DOCSTR(_unregisterFlightLoopCallback__doc__, "unregisterFlightLoopCallback", "callback, refCon=None",
          "Unregisters flightloop matching callback and refCon.\n"
          "\n"
          "Input must match that provided to registerFlightLoopCallback().");
static PyObject *XPLMUnregisterFlightLoopCallbackFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  /* We get python callback and refcon and we "know" python module
     1) Lookup tuple(callback, refcon, module) in flRevDict to get key into flDict
     2) Lookup info in flDict
     3) We unregister genericFlightLoopCallback, with refCon (which is unique flDict key)
  */
  static char *keywords[] = {"callback", "refCon", NULL};
  (void)self;
  PyObject *callback, *refcon=Py_None;
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|O", keywords, &callback, &refcon)) {
    return NULL;
  }
  PyObject *flRevDictKey = Py_BuildValue("(OOs)", callback, refcon, CurrentPythonModuleName);
  PyObject *flDictKey = PyDict_GetItem(flRevDict, flRevDictKey);
  if(flDictKey == NULL){
    Py_DECREF(flRevDictKey);
    char *s2 = objToStr(callback);
    char *s3 = objToStr(refcon);
    pythonLog("[%s] Couldn't find the id of the requested callback for %s with refCon %s.\n", CurrentPythonModuleName, s2, s3);
    free(s2);
    free(s3);
    Py_RETURN_NONE;
  }
  PyDict_DelItem(flRevDict, flRevDictKey);
  XPLMUnregisterFlightLoopCallback(genericFlightLoopCallback, PyLong_AsVoidPtr(flDictKey));
  PyDict_DelItem(flDict, flDictKey);
  Py_DECREF(flRevDictKey);
  Py_RETURN_NONE;
}

My_DOCSTR(_setFlightLoopCallbackInterval__doc__, "setFlightLoopCallbackInterval", "callback, interval=0.0, relativeToNow=1, refCon=None",
          "Change interval associated with callback, refCon.\n"
          "\n"
          "Must have been previously registered with registerFlightLoopCallback()");
static PyObject *XPLMSetFlightLoopCallbackIntervalFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"callback", "interval", "relativeToNow", "refCon", NULL};
  (void)self;
  PyObject *callback, *refcon=Py_None;
  float inInterval=0.0;
  int inRelativeToNow=1;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|fiO", keywords, &callback, &inInterval, &inRelativeToNow, &refcon)) {
    return NULL;
  }
  PyObject *flRevDictKey = Py_BuildValue("(OOs)", callback, refcon, CurrentPythonModuleName);
  PyObject *flDictKey = PyDict_GetItem(flRevDict, flRevDictKey);
  Py_DECREF(flRevDictKey);
  if(flDictKey == NULL){
    printf("Couldn't find the id of the requested callback.\n");
    return NULL;
  }
  XPLMSetFlightLoopCallbackInterval(genericFlightLoopCallback, inInterval, inRelativeToNow, PyLong_AsVoidPtr(flDictKey));
  Py_DECREF(flDictKey);
  Py_RETURN_NONE;
}

My_DOCSTR(_createFlightLoop__doc__, "createFlightLoop", "callback, phase=0, refCon=None",
          "Create flight loop, returning flightLoopID\n"
          "\n"
          "Callback take (sinceLast, elapsedTime, counter, refCon)\n"
          "returning interval (0=stop, >0 seconds, <0 flightloops)\n"
          "phase is 0=before or 1=After flight model integration");
static PyObject *XPLMCreateFlightLoopFun(PyObject* self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"callback", "phase", "refCon", NULL};
  (void)self;
  PyObject *firstObj=Py_None;
  PyObject *refCon=Py_None;
  PyObject *callback=Py_None;
  int phase=0;

  if(!XPLMCreateFlightLoop_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMCreateFlightLoop is available only in XPLM210 and up.");
    return NULL;
  }

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|iO", keywords, &firstObj, &phase, &refCon)) {
    return NULL;
  }

  XPLMCreateFlightLoop_t fl;
  fl.structSize = sizeof(fl);

  if (PySequence_Check(firstObj)) {
    fl.phase = PyLong_AsLong(PySequence_GetItem(firstObj, 0));
    callback = PySequence_GetItem(firstObj, 1);
    refCon = PySequence_GetItem(firstObj, 2);
  } else {
    fl.phase = phase;
    callback = firstObj;
    /* refCon set with parseTuple */
  }

  fl.callbackFunc = genericFlightLoopCallback;
  void *flDictKey = (void *)++flCntr;
  PyObject *flDictKeyObj = PyLong_FromVoidPtr(flDictKey);
  fl.refcon = flDictKey;
  
  XPLMFlightLoopID flightLoopID = XPLMCreateFlightLoop_ptr(&fl);

  PyObject *argObj = Py_BuildValue("(OOsi)", callback, refCon, CurrentPythonModuleName, 1);
  PyDict_SetItem(flDict, flDictKeyObj, argObj);
  Py_DECREF(argObj);

  //we need to uniquely identify the id of the callback based on the caller and inRefcon
  PyObject *flightLoopIDObj = getPtrRefOneshot(flightLoopID, flIDRef);
  PyDict_SetItem(flRevDict, flightLoopIDObj, flDictKeyObj);
  Py_XDECREF(flDictKeyObj);
  return flightLoopIDObj;
}

My_DOCSTR(_destroyFlightLoop__doc__, "destroyFlightLoop", "flightLoopID",
          "Destroys flight loop previously created by createFlightLoop()");
static PyObject *XPLMDestroyFlightLoopFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"flightLoopID", NULL};
  (void)self;
  PyObject *revId;
  if(!XPLMDestroyFlightLoop_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMDestroyFlightLoop is available only in XPLM210 and up.");
    return NULL;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &revId)){
    return NULL;
  }
  PyObject *id = PyDict_GetItem(flRevDict, revId);
  if(id == NULL){
    printf("Couldn't find the id of the requested flight loop.\n");
    return NULL;
  }
  PyDict_DelItem(flRevDict, revId);
  PyDict_DelItem(flDict, id);
  XPLMDestroyFlightLoop_ptr(refToPtr(revId, flIDRef));
  Py_RETURN_NONE;
}

My_DOCSTR(_scheduleFlightLoop__doc__, "scheduleFlightLoop", "flightLoopID, interval=0.0, relativeToNow=1",
          "Change interval associated with flight loop\n"
          "\n"
          "  0= stop\n"
          "  >0 seconds\n"
          "  <0 flightLoops\n"
          "If relativeToNow is 1, interval is relative to now, duh, otherwise\n"
          "interval is relative to previous callback execution.");
static PyObject *XPLMScheduleFlightLoopFun(PyObject *self, PyObject*args, PyObject *kwargs)
{
  static char *keywords[] = {"flightLoopID", "interval", "relativeToNow", NULL};
  (void)self;
  PyObject *flightLoopID;
  float inInterval=0.0;
  int inRelativeToNow=1;
  if(!XPLMScheduleFlightLoop_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMScheduleFlightLoop is available only in XPLM210 and up.");
    return NULL;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|fi", keywords, &flightLoopID, &inInterval, &inRelativeToNow)){
    return NULL;
  }
  XPLMFlightLoopID inFlightLoopID = refToPtr(flightLoopID, flIDRef);
  XPLMScheduleFlightLoop_ptr(inFlightLoopID, inInterval, inRelativeToNow);
  Py_RETURN_NONE;
}

static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  PyDict_Clear(flDict);
  Py_DECREF(flDict);
  PyDict_Clear(flRevDict);
  Py_DECREF(flRevDict);
  Py_RETURN_NONE;
}


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
static PyMethodDef XPLMProcessingMethods[] = {
  {"getElapsedTime", (PyCFunction)XPLMGetElapsedTimeFun, METH_VARARGS, _getElapsedTime__doc__},
  {"XPLMGetElapsedTime", (PyCFunction)XPLMGetElapsedTimeFun, METH_VARARGS, ""},
  {"getCycleNumber", (PyCFunction)XPLMGetCycleNumberFun, METH_VARARGS, _getCycleNumber__doc__},
  {"XPLMGetCycleNumber", (PyCFunction)XPLMGetCycleNumberFun, METH_VARARGS, ""},
  {"registerFlightLoopCallback", (PyCFunction)XPLMRegisterFlightLoopCallbackFun, METH_VARARGS | METH_KEYWORDS, _registerFlightLoopCallback__doc__},
  {"XPLMRegisterFlightLoopCallback", (PyCFunction)XPLMRegisterFlightLoopCallbackFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"unregisterFlightLoopCallback", (PyCFunction)XPLMUnregisterFlightLoopCallbackFun, METH_VARARGS | METH_KEYWORDS, _unregisterFlightLoopCallback__doc__},
  {"XPLMUnregisterFlightLoopCallback", (PyCFunction)XPLMUnregisterFlightLoopCallbackFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"setFlightLoopCallbackInterval", (PyCFunction)XPLMSetFlightLoopCallbackIntervalFun, METH_VARARGS | METH_KEYWORDS, _setFlightLoopCallbackInterval__doc__},
  {"XPLMSetFlightLoopCallbackInterval", (PyCFunction)XPLMSetFlightLoopCallbackIntervalFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"createFlightLoop", (PyCFunction)XPLMCreateFlightLoopFun, METH_VARARGS | METH_KEYWORDS, _createFlightLoop__doc__},
  {"XPLMCreateFlightLoop", (PyCFunction)XPLMCreateFlightLoopFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"destroyFlightLoop", (PyCFunction)XPLMDestroyFlightLoopFun, METH_VARARGS | METH_KEYWORDS, _destroyFlightLoop__doc__},
  {"XPLMDestroyFlightLoop", (PyCFunction)XPLMDestroyFlightLoopFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"scheduleFlightLoop", (PyCFunction)XPLMScheduleFlightLoopFun, METH_VARARGS | METH_KEYWORDS, _scheduleFlightLoop__doc__},
  {"XPLMScheduleFlightLoop", (PyCFunction)XPLMScheduleFlightLoopFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"_cleanup", cleanup, METH_VARARGS, ""},
  {NULL, NULL, 0, NULL}
};
#pragma GCC diagnostic pop


static struct PyModuleDef XPLMProcessingModule = {
  PyModuleDef_HEAD_INIT,
  "XPLMProcessing",
  "Laminar documentation: \n"
  "   https://developer.x-plane.com/sdk/XPLMProcessing/\n"
  "XPPython3 documentation: \n"
  "   https://xppython3.rtfd.io/en/stable/development/modules/processing.html",
  -1,
  XPLMProcessingMethods,
  NULL,
  NULL,
  NULL,
  NULL
};

PyMODINIT_FUNC
PyInit_XPLMProcessing(void)
{
  if(!(flDict = PyDict_New())){
    return NULL;
  }
  if(!(flRevDict = PyDict_New())){
    return NULL;
  }
  PyObject *mod = PyModule_Create(&XPLMProcessingModule);
  if(mod){
    PyModule_AddStringConstant(mod, "__author__", "Peter Buckner (pbuck@avnwx.com)");
    PyModule_AddIntConstant(mod, "xplm_FlightLoop_Phase_BeforeFlightModel", xplm_FlightLoop_Phase_BeforeFlightModel);
    PyModule_AddIntConstant(mod, "xplm_FlightLoop_Phase_AfterFlightModel", xplm_FlightLoop_Phase_AfterFlightModel);

    PyModule_AddIntConstant(mod, "FlightLoop_Phase_BeforeFlightModel", xplm_FlightLoop_Phase_BeforeFlightModel);
    PyModule_AddIntConstant(mod, "FlightLoop_Phase_AfterFlightModel", xplm_FlightLoop_Phase_AfterFlightModel);
  }

  /* XPLMRegisterFlightLoopCallback(flightLoopStats, -1, NULL); */

  return mod;
}



