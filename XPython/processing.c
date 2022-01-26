#define _GNU_SOURCE 1
#include <Python.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdbool.h>
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMProcessing.h>
#include "plugin_dl.h"
#include "utils.h"
#include "xppython.h"

static intptr_t flCntr;
static PyObject *flDict;  /* {flCntr: ([0]pluginSelf, [1]callback, [2]refCon)} */
static PyObject *flRevDict; /*{(pluginSelf, callback, refConAddr): flCntr }*/

static const char flIDRef[] = "FlightLoopIDRef";
static float flightLoopCallback(float inElapsedSinceLastCall, float inElapsedTimeSinceLastFlightLoop, 
                                int counter, void * inRefcon)
{
  struct timespec all_stop, all_start;
  clock_gettime(CLOCK_MONOTONIC, &all_start);

  PyObject *key = PyLong_FromVoidPtr(inRefcon);
  PyObject *callbackInfo = PyDict_GetItem(flDict, key);
  Py_XDECREF(key);
  if(callbackInfo == NULL){
    printf("Unknown flightLoop callback requested! (inRefcon = %p)\n", inRefcon);
    return 0.0;
  }
  PyObject *inElapsedSinceLastCallObj = PyFloat_FromDouble(inElapsedSinceLastCall);
  PyObject *inElapsedTimeSinceLastFlightLoopObj = PyFloat_FromDouble(inElapsedTimeSinceLastFlightLoop);
  PyObject *counterObj = PyLong_FromLong(counter);

  /* vvvvvvvvvvvvvvvvv */
  struct timespec stop, start;
  clock_gettime(CLOCK_MONOTONIC, &start);
  PyObject *res = PyObject_CallFunctionObjArgs(PyTuple_GetItem(callbackInfo, 1), inElapsedSinceLastCallObj,
                                               inElapsedTimeSinceLastFlightLoopObj, counterObj,
                                               PyTuple_GetItem(callbackInfo, 2), NULL);
  clock_gettime(CLOCK_MONOTONIC, &stop);
  pluginStats[getPluginIndex(PyTuple_GetItem(callbackInfo, 0))].fl_time += (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_nsec - start.tv_nsec) / 1000;
  /* ^^^^^^^^^^^^^^^^^ */
  
  float tmp;
  PyObject *err = PyErr_Occurred();
  Py_DECREF(inElapsedSinceLastCallObj);
  Py_DECREF(inElapsedTimeSinceLastFlightLoopObj);
  Py_DECREF(counterObj);
  if(err){
    fprintf(pythonLogFile, "[%s]: %s Error occured during the flightLoop callback (inRefcon = %p):\n",
            objToStr(PyTuple_GetItem(callbackInfo, 0)),
            objToStr(PyTuple_GetItem(callbackInfo, 1)),
            inRefcon);
    pythonLogException();
    tmp = -1.0f;
  }else{
    tmp = PyFloat_AsDouble(res);
  }
  Py_XDECREF(res);

  clock_gettime(CLOCK_MONOTONIC, &all_stop);
  pluginStats[0].fl_time += (all_stop.tv_sec - all_start.tv_sec) * 1000000 + (all_stop.tv_nsec - all_start.tv_nsec) / 1000;

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
/*       /\* fprintf(pythonLogFile, "   results[%d]: %s, flInfo[%ld]: %s -- ", *\/ */
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
/*     /\* fprintf(pythonLogFile, "  @ %s: %ld usec \n", *\/ */
/*     /\*         flInfo[x].name, flInfo[x].time); *\/ */
/*     flInfo[x].time = 0; */
/*   } */
  
/*   fprintf(pythonLogFile, "-\n   all: %ld usec\n", flInfo[0].time); */
/*   flInfo[0].time = 0; */
/*   for (int y = 0; y < maxY; y++) { */
/*     if (results[y].time) { */
/*       fprintf(pythonLogFile, "   %s: %ld usec \n", */
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
          "  <0 flightLoopes\n"
          "Callback function gets (lastCall, elapsedTime, counter, refCon)");
static PyObject *XPLMRegisterFlightLoopCallbackFun(PyObject* self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"callback", "interval", "refCon", NULL};
  (void)self;
  PyObject *callback=Py_None, *refCon=Py_None;
  float interval=0.0;

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|fO", keywords, &callback, &interval, &refCon)){
    return NULL;
  }

  PyObject *pluginSelf = get_pluginSelf();

  void *flDictKey = (void *)++flCntr;
  PyObject *flDictKeyObj = PyLong_FromVoidPtr(flDictKey);

  PyObject *argObj = Py_BuildValue("(OOO)", pluginSelf, callback, refCon);
  PyDict_SetItem(flDict, flDictKeyObj, argObj);
  Py_DECREF(argObj);

  PyObject *refConAddr = PyLong_FromVoidPtr(refCon);
  PyObject *reverseId = Py_BuildValue("(OOO)", pluginSelf, callback, refConAddr);
  PyDict_SetItem(flRevDict, reverseId, flDictKeyObj);

  Py_DECREF(flDictKeyObj);
  Py_DECREF(refConAddr);
  Py_DECREF(reverseId);
  XPLMRegisterFlightLoopCallback(flightLoopCallback, interval, flDictKey);
  Py_RETURN_NONE;
}

My_DOCSTR(_unregisterFlightLoopCallback__doc__, "unregisterFlightLoopCallback", "callback, refCon=None",
          "Unregisters flightloop matching callback and refCon.\n"
          "\n"
          "Input must match that provided to registerFlightLoopCallback().");
static PyObject *XPLMUnregisterFlightLoopCallbackFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"callback", "refCon", NULL};
  (void)self;
  PyObject *pluginSelf, *callback, *refcon=Py_None;
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|O", keywords, &callback, &refcon)) {
    return NULL;
  }
  pluginSelf = get_pluginSelf();
  PyObject *refconAddr = PyLong_FromVoidPtr(refcon);
  PyObject *revId = Py_BuildValue("(OOO)", pluginSelf, callback, refconAddr);
  PyObject *id = PyDict_GetItem(flRevDict, revId);
  if(id == NULL){
    Py_DECREF(revId);
    Py_DECREF(refconAddr);
    Py_DECREF(pluginSelf);
    fprintf(pythonLogFile, "[%s] Couldn't find the id of the requested callback for %s with refCon %s.\n",
            objToStr(pluginSelf), objToStr(callback), objToStr(refcon));
    Py_RETURN_NONE;
  }
  PyDict_DelItem(flRevDict, revId);
  XPLMUnregisterFlightLoopCallback(flightLoopCallback, PyLong_AsVoidPtr(id));
  PyDict_DelItem(flDict, id);
  Py_DECREF(revId);
  Py_DECREF(refconAddr);
  Py_DECREF(pluginSelf);
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
  PyObject *pluginSelf, *callback, *refcon=Py_None;
  float inInterval=0.0;
  int inRelativeToNow=1;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|fiO", keywords, &callback, &inInterval, &inRelativeToNow, &refcon)) {
    return NULL;
  }
  pluginSelf = get_pluginSelf();
  PyObject *refconAddr = PyLong_FromVoidPtr(refcon);
  PyObject *revId = Py_BuildValue("(OOO)", pluginSelf, callback, refconAddr);
  PyObject *id = PyDict_GetItem(flRevDict, revId);
  Py_DECREF(pluginSelf);
  Py_DECREF(revId);
  Py_DECREF(refconAddr);
  if(id == NULL){
    printf("Couldn't find the id of the requested callback.\n");
    return NULL;
  }
  XPLMSetFlightLoopCallbackInterval(flightLoopCallback, inInterval, inRelativeToNow, PyLong_AsVoidPtr(id));
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

  PyObject *pluginSelf = get_pluginSelf();
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

  fl.callbackFunc = flightLoopCallback;
  void *flDictKey = (void *)++flCntr;
  PyObject *flDictKeyObj = PyLong_FromVoidPtr(flDictKey);
  fl.refcon = flDictKey;
  
  XPLMFlightLoopID flightLoopID = XPLMCreateFlightLoop_ptr(&fl);

  PyObject *argObj = Py_BuildValue("(OOO)", pluginSelf, callback, refCon);
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
    PyModule_AddStringConstant(mod, "__author__", "Peter Buckner (xppython3@avnwx.com)");
    PyModule_AddIntConstant(mod, "xplm_FlightLoop_Phase_BeforeFlightModel", xplm_FlightLoop_Phase_BeforeFlightModel);
    PyModule_AddIntConstant(mod, "xplm_FlightLoop_Phase_AfterFlightModel", xplm_FlightLoop_Phase_AfterFlightModel);

    PyModule_AddIntConstant(mod, "FlightLoop_Phase_BeforeFlightModel", xplm_FlightLoop_Phase_BeforeFlightModel);
    PyModule_AddIntConstant(mod, "FlightLoop_Phase_AfterFlightModel", xplm_FlightLoop_Phase_AfterFlightModel);
  }

  /* XPLMRegisterFlightLoopCallback(flightLoopStats, -1, NULL); */

  return mod;
}



