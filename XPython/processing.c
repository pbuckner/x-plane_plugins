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
static PyObject *flDict;
static PyObject *flRevDict;
static PyObject *flIDDict;

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
                                               PyTuple_GetItem(callbackInfo, 3), NULL);
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
    PyErr_Print();
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

static PyObject *XPLMGetElapsedTimeFun(PyObject *self, PyObject *args)
{
  (void)self;
  (void)args;
  float res = XPLMGetElapsedTime();
  return PyFloat_FromDouble(res);
}

static PyObject *XPLMGetCycleNumberFun(PyObject *self, PyObject *args)
{
  (void)self;
  (void)args;
  int res = XPLMGetCycleNumber();
  return PyLong_FromLong(res);
}



static PyObject *XPLMRegisterFlightLoopCallbackFun(PyObject* self, PyObject *args)
{
  (void)self;
  PyObject *pluginSelf, *callback, *refcon;
  float inInterval;
  if (!PyArg_ParseTuple(args, "OOfO", &pluginSelf, &callback, &inInterval, &refcon)){
    PyErr_Clear();
    if (!PyArg_ParseTuple(args, "OfO", &callback, &inInterval, &refcon)){
      return NULL;
    }
  } else {
    pythonLogWarning("'self' deprecated as first parameter of XPLMRegisterFlightLoopCallback");
  }
  pluginSelf = get_pluginSelf();
  void *inRefcon = (void *)++flCntr;

  PyObject *id = PyLong_FromVoidPtr(inRefcon);
  //I don't like this at all...
  PyObject *refconAddr = PyLong_FromVoidPtr(refcon);
  PyObject *argObj = Py_BuildValue("(OOfO)", pluginSelf, callback, inInterval, refcon);
  PyObject *revId = Py_BuildValue("(OOO)", pluginSelf, callback, refconAddr);
  PyDict_SetItem(flDict, id, argObj);
  PyDict_SetItem(flRevDict, revId, id);
  Py_XDECREF(revId);
  Py_XDECREF(id);
  Py_DECREF(refconAddr);
  Py_DECREF(argObj);
  XPLMRegisterFlightLoopCallback(flightLoopCallback, inInterval, inRefcon);
  Py_RETURN_NONE;
}

static PyObject *XPLMUnregisterFlightLoopCallbackFun(PyObject *self, PyObject *args)
{
  (void)self;
  PyObject *pluginSelf, *callback, *refcon;
  if (!PyArg_ParseTuple(args, "OOO", &pluginSelf, &callback, &refcon)) {
    PyErr_Clear();
    if (!PyArg_ParseTuple(args, "OO", &callback, &refcon)) {
      return NULL;
    }
  } else {
    pythonLogWarning("'self' deprecated as first parameter of XPLMUnregisterFlightLoopCallback");
  }
  pluginSelf = get_pluginSelf();
  PyObject *refconAddr = PyLong_FromVoidPtr(refcon);
  PyObject *revId = Py_BuildValue("(OOO)", pluginSelf, callback, refconAddr);
  PyObject *id = PyDict_GetItem(flRevDict, revId);
  if(id == NULL){
    Py_DECREF(revId);
    Py_DECREF(refconAddr);
    Py_DECREF(pluginSelf);
    printf("Couldn't find the id of the requested callback.\n");
    return NULL;
  }
  PyDict_DelItem(flRevDict, revId);
  XPLMUnregisterFlightLoopCallback(flightLoopCallback, PyLong_AsVoidPtr(id));
  PyDict_DelItem(flDict, id);
  Py_DECREF(revId);
  Py_DECREF(refconAddr);
  Py_DECREF(pluginSelf);
  Py_RETURN_NONE;
}

static PyObject *XPLMSetFlightLoopCallbackIntervalFun(PyObject *self, PyObject *args)
{
  (void)self;
  PyObject *pluginSelf, *callback, *refcon;
  float inInterval;
  int inRelativeToNow;
  if(!PyArg_ParseTuple(args, "OOfiO", &pluginSelf, &callback, &inInterval, &inRelativeToNow, &refcon)) {
    PyErr_Clear();
    if(!PyArg_ParseTuple(args, "OfiO", &callback, &inInterval, &inRelativeToNow, &refcon)) {
      return NULL;
    }
  } else {
    pythonLogWarning("'self' deprecated as first parameter of XPLMSetFlightLoopCallbackInterval");
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

static PyObject *XPLMCreateFlightLoopFun(PyObject* self, PyObject *args)
{
  (void)self;
  PyObject *pluginSelf, *param_seq;
  if(!XPLMCreateFlightLoop_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMCreateFlightLoop is available only in XPLM210 and up.");
    return NULL;
  }
  if(!PyArg_ParseTuple(args, "OO", &pluginSelf, &param_seq)) {
    PyErr_Clear();
    if(!PyArg_ParseTuple(args, "O", &param_seq)) {
      return NULL;
    }
  } else {
    pythonLogWarning("'self' deprecated as first parameter of XPLMCreateFlightLoop");
  }
  pluginSelf = get_pluginSelf();
  PyObject *params = PySequence_Tuple(param_seq);
  XPLMCreateFlightLoop_t fl;
  fl.structSize = sizeof(fl);
  fl.phase = PyLong_AsLong(PyTuple_GetItem(params, 0));
  fl.callbackFunc = flightLoopCallback;
  fl.refcon = (void *)++flCntr;
  
  XPLMFlightLoopID res = XPLMCreateFlightLoop_ptr(&fl);

  PyObject *id = PyLong_FromVoidPtr(fl.refcon);
  PyObject *argObj = Py_BuildValue("(OOfO)", pluginSelf, PyTuple_GetItem(params, 1),
                                             -1.0, PyTuple_GetItem(params, 2));
  PyDict_SetItem(flDict, id, argObj);
  Py_XDECREF(argObj);
  //we need to uniquely identify the id of the callback based on the caller and inRefcon
  PyObject *resObj = getPtrRefOneshot(res, flIDRef);
  PyDict_SetItem(flRevDict, resObj, id);
  Py_XDECREF(id);
  return resObj;
}

static PyObject *XPLMDestroyFlightLoopFun(PyObject *self, PyObject *args)
{
  (void)self;
  PyObject *revId, *pluginSelf;
  if(!XPLMDestroyFlightLoop_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMDestroyFlightLoop is available only in XPLM210 and up.");
    return NULL;
  }
  if(!PyArg_ParseTuple(args, "OO", &pluginSelf, &revId)){
    PyErr_Clear();
    if(!PyArg_ParseTuple(args, "O", &revId)){
      return NULL;
    }
  } else {
    pythonLogWarning("'self' deprecated as first parameter of XPLMDestroyFlightLoop");
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

static PyObject *XPLMScheduleFlightLoopFun(PyObject *self, PyObject*args)
{
  (void)self;
  PyObject *flightLoopID, *pluginSelf;
  float inInterval;
  int inRelativeToNow;
  if(!XPLMScheduleFlightLoop_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMScheduleFlightLoop is available only in XPLM210 and up.");
    return NULL;
  }
  if(!PyArg_ParseTuple(args, "OOfi", &pluginSelf, &flightLoopID, &inInterval, &inRelativeToNow)){
    PyErr_Clear();
    if(!PyArg_ParseTuple(args, "Ofi", &flightLoopID, &inInterval, &inRelativeToNow)){
      return NULL;
    }
  } else {
    pythonLogWarning("'self' deprecated as first parameter of XPLMScheduleFlightLoop");
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
  PyDict_Clear(flIDDict);
  Py_DECREF(flIDDict);
  Py_RETURN_NONE;
}


static PyMethodDef XPLMProcessingMethods[] = {
  {"XPLMGetElapsedTime", XPLMGetElapsedTimeFun, METH_VARARGS, ""},
  {"XPLMGetCycleNumber", XPLMGetCycleNumberFun, METH_VARARGS, ""},
  {"XPLMRegisterFlightLoopCallback", XPLMRegisterFlightLoopCallbackFun, METH_VARARGS, ""},
  {"XPLMUnregisterFlightLoopCallback", XPLMUnregisterFlightLoopCallbackFun, METH_VARARGS, ""},
  {"XPLMSetFlightLoopCallbackInterval", XPLMSetFlightLoopCallbackIntervalFun, METH_VARARGS, ""},
  {"XPLMCreateFlightLoop", XPLMCreateFlightLoopFun, METH_VARARGS, ""},
  {"XPLMDestroyFlightLoop", XPLMDestroyFlightLoopFun, METH_VARARGS, ""},
  {"XPLMScheduleFlightLoop", XPLMScheduleFlightLoopFun, METH_VARARGS, ""},
  {"cleanup", cleanup, METH_VARARGS, ""},
  {NULL, NULL, 0, NULL}
};

static struct PyModuleDef XPLMProcessingModule = {
  PyModuleDef_HEAD_INIT,
  "XPLMProcessing",
  NULL,
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
  if(!(flIDDict = PyDict_New())){
    return NULL;
  }
  PyObject *mod = PyModule_Create(&XPLMProcessingModule);
  if(mod){
    PyModule_AddIntConstant(mod, "xplm_FlightLoop_Phase_BeforeFlightModel", xplm_FlightLoop_Phase_BeforeFlightModel);
    PyModule_AddIntConstant(mod, "xplm_FlightLoop_Phase_AfterFlightModel", xplm_FlightLoop_Phase_AfterFlightModel);
  }

  /* XPLMRegisterFlightLoopCallback(flightLoopStats, -1, NULL); */

  return mod;
}



