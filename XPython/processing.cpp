#define _GNU_SOURCE 1
#include <Python.h>
#include <sys/time.h>
#include <stdio.h>
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMProcessing.h>
#include <vector>
#include <string>
#include <unordered_map>
#include "plugin_dl.h"
#include "utils.h"
#include "xppython.h"
#include "xppythontypes.h"
#include "processing.h"
#include "capsules.h"
#include "cpp_utilities.hpp"

static intptr_t flCntr;
/* the 'key' to flDict, we use as the refCon for genericFlightLoopCallback function.
   When genericFlightLoopCallback function is _called_ by X-Plane, we:
   1) take provided refCon
   2) lookup info in flDict to get "real" callback & refcon
*/
   
struct FlightLoopDict {
  PyObject *callback;
  PyObject *refCon;
  std::string module_name;
  int loop_type;
};
static std::unordered_map<intptr_t, FlightLoopDict> flightLoopCallbacks;

/* For "new style" flight looks, we also record XPLMFlightLoopID capsule.
   Note that internally, X-Plane will always call our genericFlightLoop with <flCntr> as the refCon,
   so we can do a flDict[<flCntr>] lookup to get the info: we don't need FlightLoopID.

   But, for schedule and destroy flight loop, user provides us with <capsuleFlightLoopID>,
   so we need to determine which <flCntr> is correct for this <capsule>.

   We'll do a traversal of flIDDict, comparing capsules, to retrieve <flCntr>.

   (Why traversal? We need to go both directions:
   for reset  <fnCntr>                    -> <capsule XPLMFlightLoopID>
   for destroy <capsule XPLMFlightLoopID> -> <fnCntr>
   Really, could just use list of two-tuples, I suppose
*/
static PyObject *flIDDict;  /* {
                                  flCntr: <capsule XPLMFlightLoopID>
                               } */

static float genericFlightLoopCallbackStats(float inElapsedSinceLastCall, float inElapsedTimeSinceLastFlightLoop, 
                                       int counter, void * inRefcon);
static float genericFlightLoopCallbackNoStats(float inElapsedSinceLastCall, float inElapsedTimeSinceLastFlightLoop, 
                                       int counter, void * inRefcon);

void resetFlightLoops()
{
  /* flightLoopCallbacks & flIDDict:
   * unregisters all flight loops by going through flightLoopCallbacks
   * Then, at the end, clears both containers
   */
  for (auto& pair : flightLoopCallbacks) {
    FlightLoopDict& info = pair.second;
    pythonDebug("     Reset --     (%s)", info.module_name.c_str());

    if (info.loop_type == 0) {
      XPLMUnregisterFlightLoopCallback(pythonStats ? genericFlightLoopCallbackStats : genericFlightLoopCallbackNoStats,
                                       (void*)pair.first);
    } else {
      PyObject *pKey = PyLong_FromLong(pair.first);
      XPLMDestroyFlightLoop(getVoidPtr(PyDict_GetItem(flIDDict, pKey), "XPLMFlightLoopID"));
      deleteCapsule(pKey);
      Py_DECREF(pKey);
    }
    Py_DECREF(info.callback);
    Py_DECREF(info.refCon);
  }
  flightLoopCallbacks.clear();
  PyDict_Clear(flIDDict);
}

static float genericFlightLoopCallbackStats(float inElapsedSinceLastCall, float inElapsedTimeSinceLastFlightLoop,
                                int counter, void * inRefcon)
{
  errCheck("prior flightLoop");
  struct timespec all_stop, all_start;
  clock_gettime(CLOCK_MONOTONIC, &all_start);

  intptr_t refcon_id = (intptr_t)inRefcon;
  auto it = flightLoopCallbacks.find(refcon_id);
  if (it == flightLoopCallbacks.end()) {
    pythonDebug("Unknown flightLoop callback requested! (inRefcon = %p). Disabling it.\n", inRefcon);
    return 0.0;
  }
  FlightLoopDict& flInfo = it->second;
  errCheck("post callbackInfo");
  PyObject *inElapsedSinceLastCallObj = PyFloat_FromDouble(inElapsedSinceLastCall);
  PyObject *inElapsedTimeSinceLastFlightLoopObj = PyFloat_FromDouble(inElapsedTimeSinceLastFlightLoop);
  PyObject *counterObj = PyLong_FromLong(counter);

  /* vvvvvvvvvvvvvvvvv */
  struct timespec stop, start;
  clock_gettime(CLOCK_MONOTONIC, &start);

  PyObject *module_name_obj = PyUnicode_FromString(flInfo.module_name.c_str());
  set_moduleName(module_name_obj);
  Py_DECREF(module_name_obj);

  PyObject *res = PyObject_CallFunctionObjArgs(flInfo.callback, inElapsedSinceLastCallObj,
                                               inElapsedTimeSinceLastFlightLoopObj, counterObj,
                                               flInfo.refCon, nullptr);
  clock_gettime(CLOCK_MONOTONIC, &stop);
  PyObject *module_name_for_stats = PyUnicode_FromString(flInfo.module_name.c_str());
  pluginStats[getPluginIndex(module_name_for_stats)].fl_time += (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_nsec - start.tv_nsec) / 1000;
  Py_DECREF(module_name_for_stats);
  /* ^^^^^^^^^^^^^^^^^ */
  
  float tmp;
  PyObject *err = PyErr_Occurred();
  Py_DECREF(inElapsedSinceLastCallObj);
  Py_DECREF(inElapsedTimeSinceLastFlightLoopObj);
  Py_DECREF(counterObj);
  if(err){
    pythonLogException();
    char *s = objToStr(flInfo.callback);
    pythonLog("[%s]: %s Error occured during the flightLoop callback (inRefcon = %p), disabling:",
              CurrentPythonModuleName, s, inRefcon);
    free(s);
    errCheck("opps, trying to log error failed");
    tmp = 0.0f;
  } else if (PyFloat_Check(res)) {
    tmp = PyFloat_AsDouble(res);
  } else if (PyLong_Check(res)) {
    tmp = PyLong_AsDouble(res);
  } else {
    char *s = objToStr(flInfo.callback);
    char *s2 = objToStr(res); 
    pythonLog("[%s]: %s Error occured during the flightLoop callback (inRefcon = %p), disabling: Bad return value '%s'",
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

static float genericFlightLoopCallbackNoStats(float inElapsedSinceLastCall, float inElapsedTimeSinceLastFlightLoop,
                                              int counter, void * inRefcon)
{
  intptr_t refcon_id = (intptr_t)inRefcon;
  auto it = flightLoopCallbacks.find(refcon_id);
  if (it == flightLoopCallbacks.end()) {
    pythonDebug("Unknown flightLoop callback requested! (inRefcon = %p). Disabling it.\n", inRefcon);
    return 0.0;
  }
  FlightLoopDict& flInfo = it->second;
  PyObject *inElapsedSinceLastCallObj = PyFloat_FromDouble(inElapsedSinceLastCall);
  PyObject *inElapsedTimeSinceLastFlightLoopObj = PyFloat_FromDouble(inElapsedTimeSinceLastFlightLoop);
  PyObject *counterObj = PyLong_FromLong(counter);

  PyObject *res = PyObject_CallFunctionObjArgs(flInfo.callback, inElapsedSinceLastCallObj,
                                               inElapsedTimeSinceLastFlightLoopObj, counterObj,
                                               flInfo.refCon, nullptr);
  float tmp;
  PyObject *err = PyErr_Occurred();
  Py_DECREF(inElapsedSinceLastCallObj);
  Py_DECREF(inElapsedTimeSinceLastFlightLoopObj);
  Py_DECREF(counterObj);
  if(err){
    pythonLogException();
    char *s = objToStr(flInfo.callback);
    pythonLog("[%s]: %s Error occured during the flightLoop callback (inRefcon = %p), disabling:",
              CurrentPythonModuleName, s, inRefcon);
    free(s);
    errCheck("opps, trying to log error failed");
    tmp = 0.0f;
  } else if (PyFloat_Check(res)) {
    tmp = PyFloat_AsDouble(res);
  } else if (PyLong_Check(res)) {
    tmp = PyLong_AsDouble(res);
  } else {
    char *s = objToStr(flInfo.callback);
    char *s2 = objToStr(res); 
    pythonLog("[%s]: %s Error occured during the flightLoop callback (inRefcon = %p), disabling: Bad return value '%s'",
              CurrentPythonModuleName, s, inRefcon, s2);
    free(s);
    free(s2);
    errCheck("opps, trying to log 'else' error failed");
    tmp = 0.0f;
  }
  Py_XDECREF(res);

  return tmp;
}

My_DOCSTR(_getElapsedTime__doc__, "getElapsedTime",
          "",
          "",
          "float",
          "Return elapsed time since sim started.");
static PyObject *XPLMGetElapsedTimeFun(PyObject *self, PyObject *args)
{
  (void)self;
  (void)args;
  float res = XPLMGetElapsedTime();
  return PyFloat_FromDouble(res);
}

My_DOCSTR(_getCycleNumber__doc__, "getCycleNumber",
          "",
          "",
          "int",
          "Get cycle number, increased for each cycle computed by sim.");
static PyObject *XPLMGetCycleNumberFun(PyObject *self, PyObject *args)
{
  (void)self;
  (void)args;
  int res = XPLMGetCycleNumber();
  return PyLong_FromLong(res);
}



My_DOCSTR(_registerFlightLoopCallback__doc__, "registerFlightLoopCallback",
          "callback, interval=0.0, refCon=None",
          "callback:Callable[[float, float, int, Any], float], interval:float=0.0, refCon:Any=None",
          "None",
          "Register flight loop callback.\n"
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
   */

  std::vector<std::string> params = {"callback", "interval", "refCon"};
  char **keywords = stringVectorToCharArray(params);
  (void)self;
  PyObject *callback=Py_None, *refCon=Py_None;
  float interval=0.0;

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|fO", keywords, &callback, &interval, &refCon)){
    freeCharArray(keywords, params.size());
    return nullptr;
  }
  freeCharArray(keywords, params.size());

  intptr_t refcon = ++flCntr;
  flightLoopCallbacks[refcon] = {
    .callback = callback,
    .refCon = refCon,
    .module_name = std::string(CurrentPythonModuleName),
    .loop_type = 0
  };
  Py_INCREF(callback);
  Py_INCREF(refCon);
  XPLMRegisterFlightLoopCallback(pythonStats ? genericFlightLoopCallbackStats : genericFlightLoopCallbackNoStats,
                                 interval, (void*)refcon);
  Py_RETURN_NONE;
}


My_DOCSTR(_unregisterFlightLoopCallback__doc__, "unregisterFlightLoopCallback",
          "callback, refCon=None",
          "callback:Callable[[float, float, int, Any], float], refCon:Any=None",
          "None",
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
  std::vector<std::string> params = {"callback", "refCon"};
  char **keywords = stringVectorToCharArray(params);
  (void)self;
  PyObject *callback, *refcon=Py_None;
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|O", keywords, &callback, &refcon)) {
    freeCharArray(keywords, params.size());
    return nullptr;
  }
  freeCharArray(keywords, params.size());

  PyObject *module_name_p = get_moduleName_p();
  intptr_t found_refcon = 0;
  bool found = false;

  for (auto it = flightLoopCallbacks.begin(); it != flightLoopCallbacks.end(); ++it) {
    FlightLoopDict& info = it->second;
    PyObject *t_moduleName = PyUnicode_FromString(info.module_name.c_str());

    if (PyObject_RichCompareBool(info.refCon, refcon, Py_EQ)
        && PyObject_RichCompareBool(info.callback, callback, Py_EQ)
        && PyObject_RichCompareBool(t_moduleName, module_name_p, Py_EQ)) {
      found_refcon = it->first;
      found = true;
      Py_DECREF(t_moduleName);
      break;
    }
    Py_DECREF(t_moduleName);
  }
  Py_DECREF(module_name_p);

  if (!found) {
    PyErr_SetString(PyExc_ValueError , "unregisterFlightLoopCallback: Unknown flight loop callback.");
    return nullptr;
  }

  auto it = flightLoopCallbacks.find(found_refcon);
  if (it != flightLoopCallbacks.end()) {
    Py_DECREF(it->second.callback);
    Py_DECREF(it->second.refCon);
    flightLoopCallbacks.erase(it);
  }

  XPLMUnregisterFlightLoopCallback(pythonStats ? genericFlightLoopCallbackStats : genericFlightLoopCallbackNoStats,
                                   (void*)found_refcon);
  Py_RETURN_NONE;
}


My_DOCSTR(_setFlightLoopCallbackInterval__doc__, "setFlightLoopCallbackInterval",
          "callback, interval=0.0, relativeToNow=1, refCon=None",
          "callback:Callable[[float, float, int, Any], float], interval:float=0.0, relativeToNow:int=1, refCon:Any=None",
          "None",
          "Change interval associated with callback, refCon.\n"
          "\n"
          "Must have been previously registered with registerFlightLoopCallback()");
static PyObject *XPLMSetFlightLoopCallbackIntervalFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  std::vector<std::string> params = {"callback", "interval", "relativeToNow", "refCon"};
  char **keywords = stringVectorToCharArray(params);
  (void)self;
  PyObject *callback, *refcon=Py_None;
  float inInterval=0.0;
  int inRelativeToNow=1;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|fiO", keywords, &callback, &inInterval, &inRelativeToNow, &refcon)) {
    freeCharArray(keywords, params.size());
    return nullptr;
  }
  freeCharArray(keywords, params.size());

  PyObject *module_name_p = get_moduleName_p();
  intptr_t found_refcon = 0;
  bool found = false;

  for (auto it = flightLoopCallbacks.begin(); it != flightLoopCallbacks.end(); ++it) {
    FlightLoopDict& info = it->second;
    PyObject *t_moduleName = PyUnicode_FromString(info.module_name.c_str());

    if (PyObject_RichCompareBool(info.refCon, refcon, Py_EQ)
        && PyObject_RichCompareBool(info.callback, callback, Py_EQ)
        && PyObject_RichCompareBool(t_moduleName, module_name_p, Py_EQ)) {
      found_refcon = it->first;
      found = true;
      Py_DECREF(t_moduleName);
      break;
    }
    Py_DECREF(t_moduleName);
  }
  Py_DECREF(module_name_p);

  if (!found) {
    PyErr_SetString(PyExc_ValueError , "setFlightLoopCallbackInterval: Unknown FlightLoopID");
    return nullptr;
  }

  XPLMSetFlightLoopCallbackInterval(pythonStats ? genericFlightLoopCallbackStats : genericFlightLoopCallbackNoStats,
                                    inInterval, inRelativeToNow, (void*)found_refcon);
  Py_RETURN_NONE;
}

My_DOCSTR(_createFlightLoop__doc__, "createFlightLoop",
          "callback, phase=0, refCon=None",
          "callback:Callable[[float, float, int, Any], float], "
          "phase:XPLMFlightLoopPhaseType=FlightLoop_Phase_BeforeFlightModel, refCon:Any=None",
          "XPLMFlightLoopID",
          "Create flight loop, returning flightLoopID\n"
          "\n"
          "Callback take (sinceLast, elapsedTime, counter, refCon)\n"
          "returning interval (0=stop, >0 seconds, <0 flightloops)\n"
          "phase is 0=before or 1=After flight model integration");
static PyObject *XPLMCreateFlightLoopFun(PyObject* self, PyObject *args, PyObject *kwargs)
{
  std::vector<std::string> params = {"callback", "phase", "refCon"};
  char **keywords = stringVectorToCharArray(params);
  (void)self;
  PyObject *firstObj=Py_None;
  PyObject *refCon=Py_None;
  PyObject *callback=Py_None;
  int phase=0;

  if(!XPLMCreateFlightLoop_ptr){
    freeCharArray(keywords, params.size());
    PyErr_SetString(PyExc_RuntimeError , "XPLMCreateFlightLoop is available only in XPLM210 and up.");
    return nullptr;
  }

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|iO", keywords, &firstObj, &phase, &refCon)) {
    freeCharArray(keywords, params.size());
    return nullptr;
  }
  freeCharArray(keywords, params.size());

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

  fl.callbackFunc = pythonStats ? genericFlightLoopCallbackStats : genericFlightLoopCallbackNoStats;
  intptr_t refcon = ++flCntr;
  PyObject *flDictKeyObj = PyLong_FromLong(refcon);
  fl.refcon = (void*)refcon;

  XPLMFlightLoopID flightLoopID = XPLMCreateFlightLoop_ptr(&fl);

  flightLoopCallbacks[refcon] = {
    .callback = callback,
    .refCon = refCon,
    .module_name = std::string(CurrentPythonModuleName),
    .loop_type = 1
  };
  Py_INCREF(callback);
  Py_INCREF(refCon);

  PyObject *flightLoopIDObj = makeCapsule(flightLoopID, "XPLMFlightLoopID");
  PyDict_SetItem(flIDDict, flDictKeyObj, flightLoopIDObj);
  Py_DECREF(flDictKeyObj);
  return flightLoopIDObj;
}

My_DOCSTR(_isFlightLoopValid__doc__, "isFlightLoopValid",
          "flightLoopID",
          "flightLoopID:XPLMFlightLoopID",
          "bool",
          "Return True if flightLoopID exists and is valid: it may or may not be scheduled.");
static PyObject *isFlightLoopValidFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  std::vector<std::string> params = {"flightLoopID"};
  char **keywords = stringVectorToCharArray(params);
  (void) self;
  PyObject *revId;
  if(!XPLMDestroyFlightLoop_ptr){
    freeCharArray(keywords, params.size());
    PyErr_SetString(PyExc_RuntimeError , "isFlightLoopValid is available only in XPLM210 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &revId)){
    freeCharArray(keywords, params.size());
    PyErr_SetString(PyExc_ValueError , "xp.isFlightLoopValid: missing flightLoopID.");
    return nullptr;
  }
  freeCharArray(keywords, params.size());
  PyObject *pKey, *pValue;
  Py_ssize_t pos = 0;
  while(PyDict_Next(flIDDict, &pos, &pKey, &pValue)){
    if (getVoidPtr(pValue, "XPLMFlightLoopID") == getVoidPtr(revId, "XPLMFlightLoopID")) {
      return Py_True;
    }
  }
  return Py_False;
}

My_DOCSTR(_destroyFlightLoop__doc__, "destroyFlightLoop",
          "flightLoopID",
          "flightLoopID:XPLMFlightLoopID",
          "None",
          "Destroys flight loop previously created by createFlightLoop()");
static PyObject *XPLMDestroyFlightLoopFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  std::vector<std::string> params = {"flightLoopID"};
  char **keywords = stringVectorToCharArray(params);
  (void)self;
  PyObject *revId;
  if(!XPLMDestroyFlightLoop_ptr){
    freeCharArray(keywords, params.size());
    PyErr_SetString(PyExc_RuntimeError , "XPLMDestroyFlightLoop is available only in XPLM210 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &revId)){
    freeCharArray(keywords, params.size());
    PyErr_SetString(PyExc_ValueError , "xp.destroyFlightLoop: missing flightLoopID.");
    return nullptr;
  }
  freeCharArray(keywords, params.size());
  PyObject *pKey, *pValue;
  Py_ssize_t pos = 0;
  PyObject *found = Py_None;
  while(PyDict_Next(flIDDict, &pos, &pKey, &pValue)){
    if (getVoidPtr(pValue, "XPLMFlightLoopID") == getVoidPtr(revId, "XPLMFlightLoopID")) {
      found = pKey;
      break;
    }
  }
  if (found == Py_None) {
    PyErr_SetString(PyExc_ValueError , "destroyFlightLoop: Unknown FlightLoopID");
    return nullptr;
  }
  XPLMDestroyFlightLoop_ptr(getVoidPtr(revId, "XPLMFlightLoopID"));
  PyDict_DelItem(flIDDict, found);
  flightLoopCallbacks.erase((intptr_t)found);
  deleteCapsule(revId);
  Py_RETURN_NONE;
}

My_DOCSTR(_scheduleFlightLoop__doc__, "scheduleFlightLoop",
          "flightLoopID, interval=0.0, relativeToNow=1",
          "flightLoopID:XPLMFlightLoopID, interval:float=0.0, relativeToNow:int=1",
          "None",
          "Change interval associated with flight loop\n"
          "\n"
          "  0= stop\n"
          "  >0 seconds\n"
          "  <0 flightLoops\n"
          "If relativeToNow is 1, interval is relative to now, duh, otherwise\n"
          "interval is relative to previous callback execution.");
static PyObject *XPLMScheduleFlightLoopFun(PyObject *self, PyObject*args, PyObject *kwargs)
{
  std::vector<std::string> params = {"flightLoopID", "interval", "relativeToNow"};
  char **keywords = stringVectorToCharArray(params);
  (void)self;
  PyObject *flightLoopID;
  float inInterval=0.0;
  int inRelativeToNow=1;
  if(!XPLMScheduleFlightLoop_ptr){
    freeCharArray(keywords, params.size());
    PyErr_SetString(PyExc_RuntimeError , "XPLMScheduleFlightLoop is available only in XPLM210 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|fi", keywords, &flightLoopID, &inInterval, &inRelativeToNow)){
    freeCharArray(keywords, params.size());
    PyErr_SetString(PyExc_TypeError, "scheduleFlightLoop signature is (flightLoopID: XPLMFlightLoopID, interval: Float=0.0, relativeToNow: int=1).");
    return nullptr;
  }
  freeCharArray(keywords, params.size());
  XPLMFlightLoopID inFlightLoopID = getVoidPtr(flightLoopID, "XPLMFlightLoopID");
  if (inFlightLoopID == nullptr) {
    PyErr_SetString(PyExc_ValueError, "scheduleFlightLoop: bad flightLoopID.");
    return nullptr;
  }
  XPLMScheduleFlightLoop_ptr(inFlightLoopID, inInterval, inRelativeToNow);
  Py_RETURN_NONE;
}

static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  for (auto& pair : flightLoopCallbacks) {
    Py_DECREF(pair.second.callback);
    Py_DECREF(pair.second.refCon);
  }
  flightLoopCallbacks.clear();
  PyDict_Clear(flIDDict);
  Py_DECREF(flIDDict);
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
  {"isFlightLoopValid", (PyCFunction)isFlightLoopValidFun, METH_VARARGS | METH_KEYWORDS, _isFlightLoopValid__doc__},
  {"_cleanup", cleanup, METH_VARARGS, ""},
  {nullptr, nullptr, 0, nullptr}
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
  nullptr,
  nullptr,
  nullptr,
  nullptr
};

PyMODINIT_FUNC
PyInit_XPLMProcessing(void)
{
  if(!(flIDDict = PyDict_New())){
    return nullptr;
  }
  PyDict_SetItemString(XPY3pythonDicts, "flightLoopIDs", flIDDict);
  PyObject *mod = PyModule_Create(&XPLMProcessingModule);
  if(mod){
    PyModule_AddStringConstant(mod, "__author__", "Peter Buckner (pbuck@xppython3.org)");
    PyModule_AddIntConstant(mod, "xplm_FlightLoop_Phase_BeforeFlightModel", xplm_FlightLoop_Phase_BeforeFlightModel); // XPLMFlightLoopPhaseType
    PyModule_AddIntConstant(mod, "xplm_FlightLoop_Phase_AfterFlightModel", xplm_FlightLoop_Phase_AfterFlightModel); // XPLMFlightLoopPhaseType

    PyModule_AddIntConstant(mod, "FlightLoop_Phase_BeforeFlightModel", xplm_FlightLoop_Phase_BeforeFlightModel); // XPLMFlightLoopPhaseType
    PyModule_AddIntConstant(mod, "FlightLoop_Phase_AfterFlightModel", xplm_FlightLoop_Phase_AfterFlightModel); // XPLMFlightLoopPhaseType
  }

  /* XPLMRegisterFlightLoopCallback(flightLoopStats, -1, nullptr); */

  return mod;
}



