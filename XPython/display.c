#define _GNU_SOURCE 1
#include <Python.h>
#include <sys/time.h>
#include <stdio.h>
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMDisplay.h>
#include "utils.h"
#include "plugin_dl.h"
#include "display.h"
#include "xppythontypes.h"
#include "xppython.h"

static PyObject *drawCallbackDict;
static intptr_t drawCallbackCntr;
#define DRAW_MODULE_NAME 0
#define DRAW_CALLBACK 1 
#define DRAW_PHASE 2
#define DRAW_BEFORE 3
#define DRAW_REFCON 4  
static PyObject *drawCallbackIDDict;
static int genericDrawCallback(XPLMDrawingPhase inPhase, int inIsBefore, void *inRefcon);

static PyObject *keySniffCallbackDict;
static intptr_t keySniffCallbackCntr;
#define KEYSNIFF_MODULE_NAME 0
#define KEYSNIFF_CALLBACK 1
#define KEYSNIFF_BEFORE 2
#define KEYSNIFF_REFCON 3
static int genericKeySnifferCallback(char inChar, XPLMKeyFlags inFlags, char inVirtualKey, void *inRefcon);

static PyObject *avionicsCallbacksDict; // key is PyLong(avionicsCallbacksCntr)
static intptr_t avionicsCallbacksCntr;
#define AVIONICS_MODULE_NAME 0
#define AVIONICS_DEVICE 1
#define AVIONICS_BEFORE 2
#define AVIONICS_AFTER  3
#define AVIONICS_REFCON 4
static PyObject *avionicsCallbacksIDDict; // key is avionicsIDCapsule, value is PyLong(avionicsCallbacksCntr)

//draw, key,mouse, cursor, wheel
static PyObject *windowDict;
#define WINDOW_DRAW 0
#define WINDOW_CLICK 1
#define WINDOW_KEY 2
#define WINDOW_CURSOR 3
#define WINDOW_WHEEL 4
#define WINDOW_RIGHTCLICK 5
#define WINDOW_MODULE_NAME 6

static PyObject *hotkeyDict;
static intptr_t hotkeyCntr;
#define HOTKEY_CALLBACK 0
#define HOTKEY_REFCON 1
#define HOTKEY_MODULE_NAME 2
static PyObject *hotkeyIDDict;

static PyObject *monitorBndsCallback;

PyObject *windowIDCapsules;
const char *windowIDRef = "XPLMWindowIDRef";

static PyObject *hotkeyIDCapsules;
static const char hotkeyIDRef[] = "XPLMHotkeyID";

PyObject *avionicsIDCapsules;
const char *avionicsIDRef = "XPLMAvionicsIDRef";


void resetDrawCallbacks(void) {
  PyObject *key, *tuple;
  Py_ssize_t pos = 0;
  while(PyDict_Next(drawCallbackDict, &pos, &key, &tuple)) {
    char *moduleName = objToStr(PyTuple_GetItem(tuple, DRAW_MODULE_NAME));
    char *callback = objToStr(PyTuple_GetItem(tuple, DRAW_CALLBACK));
    pythonLog("[XPPython3] Reload --     %s - (%s)\n", moduleName, callback);

    XPLMUnregisterDrawCallback(genericDrawCallback,
                               PyLong_AsLong(PyTuple_GetItem(tuple, DRAW_PHASE)),
                               PyLong_AsLong(PyTuple_GetItem(tuple, DRAW_BEFORE)),
                               (void *)PyLong_AsLong(key));
    free(moduleName);
    free(callback);
  }
  PyDict_Clear(drawCallbackDict);
  PyDict_Clear(drawCallbackIDDict);
}

void resetKeySniffCallbacks(void) {
  PyObject *key, *tuple;
  Py_ssize_t pos = 0;
  while(PyDict_Next(keySniffCallbackDict, &pos, &key, &tuple)) {
    char *moduleName = objToStr(PyTuple_GetItem(tuple, KEYSNIFF_MODULE_NAME));
    char *callback = objToStr(PyTuple_GetItem(tuple, KEYSNIFF_CALLBACK));
    pythonLog("[XPPython3] Reload --     %s - (%s)\n", moduleName, callback);
    free(moduleName);
    free(callback);
    XPLMUnregisterKeySniffer(genericKeySnifferCallback,
                             PyLong_AsLong(PyTuple_GetItem(tuple, KEYSNIFF_BEFORE)),
                             (void *)PyLong_AsLong(key));
  }
  PyDict_Clear(keySniffCallbackDict);
}

void resetWindows(void) {
  PyObject *key, *tuple;
  Py_ssize_t pos = 0;
  while(PyDict_Next(windowDict, &pos, &key, &tuple)) {
    char *s = objToStr(PyTuple_GetItem(tuple, WINDOW_MODULE_NAME)); /* borrowed */
    pythonLog("[XPPython3] Reload --     (%s)\n", s);
    free(s);
    XPLMDestroyWindow(refToPtr(key, windowIDRef));
  }
  PyDict_Clear(windowDict);
}

static void receiveMonitorBounds(int inMonitorIndex, int inLeftBx, int inTopBx,
                                       int inRightBx, int inBottomBx, void *refcon)
{
  errCheck("Before receiveMonitorBounds");
  PyObject *pRes = PyObject_CallFunction(monitorBndsCallback, "(iiiiiO)", inMonitorIndex,
                                         inLeftBx, inTopBx, inRightBx, inBottomBx, (PyObject*)refcon);
  if (PyErr_Occurred()) {
    return;
  }
  Py_DECREF(pRes);
}

My_DOCSTR(_registerDrawCallback__doc__, "registerDrawCallback", "draw, phase=xplm_Phase_Window, after=0, refCon=None",
          "Registers  low-level drawing callback.\n"
          "\n"
          "The after parameter indicates you want to be called before (0) or after (1) phase.\n"
          "draw() callback function takes three parameters (phase, after, refCon), returning \n"
          "0 to suppress further X-Plane drawing in the phase, or 1 to allow X-Plane to finish\n"
          "(Callback's value is ignored if after=1.\n"
          "\nRegistration returns 1 on success, 0 otherwise.");
static PyObject *XPLMRegisterDrawCallbackFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  errCheck("Before registerDrawCallback");
  static char *keywords[] = {"draw", "phase", "after", "refCon", NULL};
  (void) self;
  PyObject *callback;
  int inPhase = xplm_Phase_Window;
  int inWantsBefore = 0;
  PyObject *refcon = Py_None;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|iiO", keywords, &callback, &inPhase, &inWantsBefore, &refcon)){
    return NULL;
  }
  PyObject *idx = PyLong_FromLong(++drawCallbackCntr);
  if(!idx){
    PyErr_SetString(PyExc_RuntimeError ,"Couldn't create long for drawCallbackCntr.\n");
    return NULL;
  }

  PyObject *argObj = Py_BuildValue("(sOiiO)", CurrentPythonModuleName, callback, inPhase, inWantsBefore, refcon);
  PyDict_SetItem(drawCallbackDict, idx, argObj);
  Py_DECREF(argObj);
  PyObject *tmp = PyLong_FromVoidPtr(refcon);
  PyDict_SetItem(drawCallbackIDDict, tmp, idx);
  Py_DECREF(tmp);
  
  Py_DECREF(idx);
  int res = XPLMRegisterDrawCallback(genericDrawCallback, inPhase, inWantsBefore, (void *)drawCallbackCntr);
  if(!res){
    PyErr_SetString(PyExc_RuntimeError ,"XPLMRegisterDrawCallback failed.\n");
    return NULL;
  }
  errCheck("end of registerDrawCallback");
  return PyLong_FromLong(res);
}


int genericAvionicsCallback(XPLMDeviceID inDeviceID, int inIsBefore, void *inRefcon);

My_DOCSTR(_registerAvionicsCallbacksEx__doc__, "registerAvionicsCallbacksEx", "deviceId, before=None, after=None, refCon=None",
          "Registers draw callback for particular device.\n"
          "\n"
          "Registers drawing callback(s) to enhance or replace X-Plane drawing. For\n"
          "'before' callback, return 1 to let X-Plane draw or 0 to suppress X-Plane\n"
          "drawing. Return value for 'after' callback is ignored.\n"
          "\n"
          "Upon entry, OpenGL context will be correctly set in panel coordinates for 2d drawing.\n"
          "OpenGL state (texturing, etc.) will be unknwon.\n"
          "\n"
          "Successful registration returns an AvionicsID."
         );
static PyObject *XPLMRegisterAvionicsCallbacksExFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  (void) self;
  static char *keywords[] = {"deviceId", "before", "after", "refCon", NULL};
  int deviceId;
  PyObject *beforeCallback = Py_None;
  PyObject *afterCallback = Py_None;
  PyObject *refcon = Py_None;

  if(!XPLMRegisterAvionicsCallbacksEx_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMRegisterAvionicsCallbacksEx is available only in XPLM400 and up.");
    return NULL;
  }

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "i|OOO", keywords, &deviceId, &beforeCallback, &afterCallback, &refcon)){
    return NULL;
  }

  if (beforeCallback == Py_None && afterCallback == Py_None) {
    PyErr_SetString(PyExc_RuntimeError ,"XPLMRegisterAviationCallbacksEx: Both before and after callbacks cannot be None.");
    return NULL;
  }
  PyObject *idx = PyLong_FromLong(++avionicsCallbacksCntr);
  if(!idx){
    PyErr_SetString(PyExc_RuntimeError ,"Couldn't create long.\n");
    return NULL;
  }
  PyObject *argObj = Py_BuildValue("siOOO", CurrentPythonModuleName, deviceId, beforeCallback, afterCallback, refcon);
  PyDict_SetItem(avionicsCallbacksDict, idx, argObj);
  Py_DECREF(idx);
  Py_DECREF(argObj);
    
  XPLMCustomizeAvionics_t params;
  params.structSize = sizeof(params);
  params.deviceId = deviceId;
  params.drawCallbackBefore = beforeCallback != Py_None ? genericAvionicsCallback : NULL;
  params.drawCallbackAfter = afterCallback != Py_None ? genericAvionicsCallback : NULL;
  params.refcon = (void *)avionicsCallbacksCntr;

  if (beforeCallback != Py_None) {
    Py_INCREF(beforeCallback);
  }
  if (afterCallback != Py_None) {
    Py_INCREF(afterCallback);
  }
  
  XPLMAvionicsID avionicsId = XPLMRegisterAvionicsCallbacksEx_ptr(&params);
  PyObject *aID = getPtrRef(avionicsId, avionicsIDCapsules, avionicsIDRef);
  if(!aID){
    PyErr_SetString(PyExc_RuntimeError ,"XPLMRegisterAvionicsCallbacksEx failed.\n");
    return NULL;
  }

  PyDict_SetItem(avionicsCallbacksIDDict, aID, PyLong_FromLong(avionicsCallbacksCntr));
  return aID;
}

My_DOCSTR(_unregisterAvionicsCallbacks__doc__, "unregisterAvionicsCallbacks", "avionicsId",
          "Unregisters avionics draw callback(s) associated with given avionicsId.\n"
          "\n"
          "Does not return a value."
         );
static PyObject *XPLMUnregisterAvionicsCallbacksFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"avionicsId", NULL};
  (void) self;
  PyObject *aID;

  if(!XPLMUnregisterAvionicsCallbacks_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMUnregisterAvionicsCallbacks is available only in XPLM400 and up.");
    return NULL;
  }

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &aID)) {
    return NULL;
  }
  if (aID == NULL) {
    PyErr_SetString(PyExc_RuntimeError, "XPLMUnregisterAvionicsCallback bad avionicsID\n");
    return NULL;
  }
  XPLMAvionicsID avionicsId = refToPtr(aID, avionicsIDRef);
  XPLMUnregisterAvionicsCallbacks_ptr(avionicsId);

  /* and... remove from data structures */
  removePtrRef(avionicsId, avionicsIDCapsules);
  PyDict_DelItem(avionicsCallbacksDict, PyDict_GetItem(avionicsCallbacksIDDict, aID));
  PyDict_DelItem(avionicsCallbacksIDDict, aID);
  Py_DECREF(aID);

  return Py_None;
}
          


My_DOCSTR(_registerKeySniffer__doc__, "registerKeySniffer", "sniffer, before=0, refCon=None",
          "Registers a key sniffer callba function.\n"
          "\n"
          "sniffer() callback takes four parameters (key, flags, vKey, refCon) and\n"
          "should return 0 to consume the key, 1 to pass it to next sniffer or X-Plane.\n"
          "\n"
          "before=1 will intercept keys before windows (i.e., the user may be typing in\n"
          "input field), so generally, use before=0 to sniff keys not already consumed.\n"
          "\nrefCon will be passed to your sniffer callback.");
static PyObject *XPLMRegisterKeySnifferFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  errCheck("before registerKeySniffer");
  static char *keywords[] = {"sniffer", "before", "refCon", NULL};
  (void) self;
  PyObject *callback, *refcon = Py_None;
  int inBeforeWindows=0;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|iO", keywords, &callback, &inBeforeWindows, &refcon)) {
    return NULL;
  }
    
  PyObject *idx = PyLong_FromLong(++keySniffCallbackCntr);
  if(!idx){
    PyErr_SetString(PyExc_RuntimeError ,"Couldn't create long.\n");
    return NULL;
  }

  PyObject *argObj = Py_BuildValue("(sOiO)", CurrentPythonModuleName, callback, inBeforeWindows, refcon);
  PyDict_SetItem(keySniffCallbackDict, idx, argObj);
  Py_DECREF(idx);
  Py_DECREF(argObj);
  int res = XPLMRegisterKeySniffer(genericKeySnifferCallback, inBeforeWindows, (void *)keySniffCallbackCntr);
  if(!res){
    PyErr_SetString(PyExc_RuntimeError ,"registerKeySniffer failed.\n");
    return NULL;
  }
  errCheck("at end registerKeySniffer");
  return PyLong_FromLong(res);
}


My_DOCSTR(_unregisterDrawCallback__doc__, "unregisterDrawCallback", "draw, phase=xplm_Phase_Window, after=0, refCon=None",
          "Unregisters low-level drawing callback.\n"
          "\n"
          "Parameters must match those provided  with registerDrawCallback().\n"
          "Returns 1 on success, 0 otherwise.");
static PyObject *XPLMUnregisterDrawCallbackFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"draw", "phase", "after", "refCon", NULL};
  (void) self;
  PyObject *callback, *refcon = Py_None;
  int inPhase = xplm_Phase_Window, inWantsBefore = 0;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|iiO", keywords, &callback, &inPhase, &inWantsBefore, &refcon)){
    return NULL;
  }
  PyObject *pyRefcon = PyLong_FromVoidPtr(refcon);
  PyObject *pID = PyDict_GetItem(drawCallbackIDDict, pyRefcon);
  if(pID == NULL){
    PyErr_SetString(PyExc_RuntimeError ,"XPLMUnregisterDrawCallback failed to find the callback.\n");
    Py_DECREF(pyRefcon);
    return NULL;
  }
  int res = XPLMUnregisterDrawCallback(genericDrawCallback, inPhase,
                                       inWantsBefore, PyLong_AsVoidPtr(pID));
  PyDict_DelItem(drawCallbackIDDict, pyRefcon);
  PyDict_DelItem(drawCallbackDict, pID);
  Py_DECREF(pyRefcon);

  PyObject *err = PyErr_Occurred();
  if(err){
    printf("Error occured during the XPLMUnregisterDrawCallback call:\n");
    pythonLogException();
  }
  return PyLong_FromLong(res);
}

My_DOCSTR(_unregisterKeySniffer__doc__, "unregisterKeySniffer", "sniffer, before=0, refCon=None",
          "Unregisters key sniffer.\n"
          "\n"
          "Parameters must match those provided with registerKeySniffer().\n"
          "Returns 1 on success, 0 otherwise.");
static PyObject *XPLMUnregisterKeySnifferFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"sniffer", "before", "refCon", NULL};
  (void) self;
  PyObject *callback, *refcon = Py_None;
  int inBeforeWindows = 0;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|iO", keywords, &callback, &inBeforeWindows, &refcon)) {
    return NULL;
  }

  PyObject *pKey = NULL, *pVal = NULL;
  PyObject *toDelete = NULL;
  Py_ssize_t pos = 0;
  int res = -1;
  PyObject *argObj = Py_BuildValue("sOiO", CurrentPythonModuleName, callback, inBeforeWindows, refcon);
  while(PyDict_Next(keySniffCallbackDict, &pos, &pKey, &pVal)){
    if(PyObject_RichCompareBool(pVal, argObj, Py_EQ)){
      toDelete = pKey;
      break;
    }
  }
  Py_DECREF(argObj);
  if(toDelete){
    res = XPLMUnregisterKeySniffer(genericKeySnifferCallback, 
                                   inBeforeWindows, PyLong_AsVoidPtr(toDelete));
    PyDict_DelItem(keySniffCallbackDict, toDelete);
  }

  PyObject *err = PyErr_Occurred();
  if(err){
    printf("Error occured during the XPLMUnregisterKeySnifferCallback call:\n");
    pythonLogException();
  }
  return PyLong_FromLong(res);
}


static void drawWindow(XPLMWindowID  inWindowID,
                void         *inRefcon)
{
  errCheck("prior drawWindow");
  struct timespec all_stop, all_start;
  clock_gettime(CLOCK_MONOTONIC, &all_start);
  PyObject *pID = getPtrRef(inWindowID, windowIDCapsules, windowIDRef);
  PyObject *pCbks = PyDict_GetItem(windowDict, pID);
  if(pCbks == NULL){
    printf("Unknown window passed to drawWindow (%p).\n", inWindowID);
    return;
  }
  PyObject *func = PyTuple_GetItem(pCbks, WINDOW_DRAW);
  if (func != Py_None) {
    struct timespec stop, start;
    clock_gettime(CLOCK_MONOTONIC, &start);
    set_moduleName(PyTuple_GetItem(pCbks, WINDOW_MODULE_NAME));
    PyObject *oRes = PyObject_CallFunctionObjArgs(func, pID, inRefcon, NULL);
    clock_gettime(CLOCK_MONOTONIC, &stop);
    pluginStats[getPluginIndex(PyTuple_GetItem(pCbks, WINDOW_MODULE_NAME))].draw_time += (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_nsec - start.tv_nsec) / 1000;

    if(PyErr_Occurred()) {
      pythonLogException();
    }
    Py_XDECREF(oRes);
  }
  Py_DECREF(pID);
  errCheck("end drawWindow");
  clock_gettime(CLOCK_MONOTONIC, &all_stop);
  pluginStats[0].draw_time += (all_stop.tv_sec - all_start.tv_sec) * 1000000 + (all_stop.tv_nsec - all_start.tv_nsec) / 1000;
}

static void handleKey(XPLMWindowID  inWindowID,
               char          inKey,
               XPLMKeyFlags  inFlags,
               char          inVirtualKey,
               void         *inRefcon,
               int           losingFocus)
{
  errCheck("prior handleKey");
  if (losingFocus) {
    /* If losing focus, X-Plane sends the _receiving_ windowID, rather than "self", the _losing_
     * windowID. Bug reported 10/22/2021. 
     * Confirmed by Jennifer Roberts 11/1/2021, "
     *   "This is a bug in XPWidgets -- bit it's caused by the dev being fooled by XPD-10834. Unfortunately,
     *    it can't be fixed because of the design of XPD-10834. It sounds like a fix would be a re-work of the
     *    API at some point in the future, so we do not have a time line on this."
     *
     * This is the wrong windowID, and we'll be unable to correctly pass the losingFlag
     * to the proper window's callback. (It may be received by a window outside of our plugin,
     * so we'll never know.)
     */
    return;
  }
  PyObject *pID = getPtrRef(inWindowID, windowIDCapsules, windowIDRef);
  char msg[2024];
  PyObject *pCbks = PyDict_GetItem(windowDict, pID);
  if(pCbks == NULL){
    if (inWindowID == NULL && losingFocus) {
      /* This occurs only when I have a window with keyboard focus and then
         I destroy the window (or otherwise lose focus.)
         I get callback to handle key but the window ID set zero.
         Verified this happens in simple C program, so it's not Python's fault.
         So far, it appears safe to just ignore.
         Filed with Laminar 18-May-2020 as XPD-10834
      */
      XPLMDebugString("NULL window passed to handleKey. Ignoring\n");
      errCheck("no callback, losing focus, handleKey");
      return;
    }
    char *s = objToStr(pID);
    sprintf(msg, "Unknown window passed to handleKey (%p) [%ld] -- losingFocus is %d. pID is %s\n", inWindowID, (long)inWindowID, losingFocus, s);
    free(s);
    pythonLog("%s", msg);
    return;
  }
  PyObject *arg1 = PyLong_FromLong(inKey);
  PyObject *arg2 = PyLong_FromLong(inFlags);
  PyObject *arg3 = PyLong_FromLong((unsigned int)inVirtualKey);
  PyObject *arg4 = PyLong_FromLong(losingFocus);
  /* char *s = objToStr(pID); */
  /* printf("Calling handleKey callback. inWindowID = %p, pPID = %s, losingFocus = %d\n", inWindowID, s, losingFocus); */
  /* free(s); */
  set_moduleName(PyTuple_GetItem(pCbks, WINDOW_MODULE_NAME));
  PyObject *oRes = PyObject_CallFunctionObjArgs(PyTuple_GetItem(pCbks, WINDOW_KEY), pID, arg1, arg2, arg3, inRefcon, arg4, NULL);
  Py_XDECREF(arg1);
  Py_XDECREF(arg2);
  Py_XDECREF(arg3);
  Py_XDECREF(arg4);
  Py_XDECREF(oRes);
  errCheck("end handleKey");
  Py_DECREF(pID);
}

static int handleMouseClick(XPLMWindowID     inWindowID,
                     int              x,
                     int              y,
                     XPLMMouseStatus  inMouse,
                     void            *inRefcon)
{
  errCheck("prior handleMouseClick");
  (void) inRefcon;
  PyObject *pID = getPtrRef(inWindowID, windowIDCapsules, windowIDRef);
  PyObject *pCbks = PyDict_GetItem(windowDict, pID);
  if(pCbks == NULL){
    printf("Unknown window passed to click callback (%p).\n", inWindowID);
    return 1;
  }
  PyObject *func = PyTuple_GetItem(pCbks, WINDOW_CLICK);  /* borrowed */
  set_moduleName(PyTuple_GetItem(pCbks, WINDOW_MODULE_NAME));
  if (func == Py_None) {
    /* consume click */
    errCheck("no func handleMouseClick");
    return 1;
  }

  PyObject *arg1 = PyLong_FromLong(x);
  PyObject *arg2 = PyLong_FromLong(y);
  PyObject *arg3 = PyLong_FromLong(inMouse);
  PyObject *pRes = PyObject_CallFunctionObjArgs(func, pID, arg1, arg2, arg3, inRefcon, NULL);
  PyObject *err = PyErr_Occurred();
  Py_DECREF(arg1);
  Py_DECREF(arg2);
  Py_DECREF(arg3);
  Py_DECREF(pID);
  if(err){
    pythonLogException();
    return 1;
  }
  if (!PyLong_Check(pRes)) {
    char *msg;
    char *s = objToStr(func);
    if (-1 == asprintf(&msg, "click() callback [%s] failed to return integer\n.", s)) {
      pythonLog("Failed to allocate asprintf memory for callback error.\n");
    } else {
      PyErr_SetString(PyExc_ValueError, msg);
      free(msg);
    }
    free(s);
    return 1;
  }
  int res = (int)PyLong_AsLong(pRes);
  Py_DECREF(pRes);
  errCheck("end handleMouseClick");
  return res;
}

static int handleRightClick(XPLMWindowID     inWindowID,
                     int              x,
                     int              y,
                     XPLMMouseStatus  inMouse,
                     void            *inRefcon)
{
  errCheck("prior handleRightClick");
  (void) inRefcon;
  PyObject *pID = getPtrRef(inWindowID, windowIDCapsules, windowIDRef);
  PyObject *pCbks = PyDict_GetItem(windowDict, pID);
  if(pCbks == NULL){
    printf("Unknown window passed to rightClick callback (%p).\n", inWindowID);
    return 1;
  }
  PyObject *func = PyTuple_GetItem(pCbks, WINDOW_RIGHTCLICK);
  set_moduleName(PyTuple_GetItem(pCbks, WINDOW_MODULE_NAME));
  if (func == Py_None) {
    return 1; /* consume click in window */
  }
    
  PyObject *arg1 = PyLong_FromLong(x);
  PyObject *arg2 = PyLong_FromLong(y);
  PyObject *arg3 = PyLong_FromLong(inMouse);
  PyObject *pRes = PyObject_CallFunctionObjArgs(func, pID, arg1, arg2, arg3, inRefcon, NULL);
  Py_DECREF(arg1);
  Py_DECREF(arg2);
  Py_DECREF(arg3);
  Py_DECREF(pID);
  PyObject *err = PyErr_Occurred();
  if(err){
    pythonLogException();
    return 1;
  }
  if (!PyLong_Check(pRes)) {
    char *msg;
    char *s = objToStr(func);
    if (-1 == asprintf(&msg, "rightClick() callback [%s] failed to return integer\n.", s)) {
      pythonLog("Failed to allocate asprintf memory for right click callback.\n");
    } else {
      PyErr_SetString(PyExc_ValueError, msg);
      free(msg);
    }
    free(s);
    return 1;
  }
  int res = (int)PyLong_AsLong(pRes);
  Py_DECREF(pRes);
  errCheck("end handleRightClick");
  return res;
}

static XPLMCursorStatus handleCursor(XPLMWindowID  inWindowID,
                              int           x,
                              int           y,
                              void         *inRefcon)
{
  errCheck("prior handleCursor");
  (void) inRefcon;
  PyObject *pID = getPtrRef(inWindowID, windowIDCapsules, windowIDRef);
  PyObject *pCbks = PyDict_GetItem(windowDict, pID);
  if(pCbks == NULL){
    printf("Unknown window passed to handleCursor (%p).\n", inWindowID);
    return 0;
  }
  PyObject *cbk = PyTuple_GetItem(pCbks, WINDOW_CURSOR);
  set_moduleName(PyTuple_GetItem(pCbks, WINDOW_MODULE_NAME));
  if((cbk == NULL) || (cbk == Py_None)){
    return 0;
  }
  PyObject *arg1 = PyLong_FromLong(x);
  PyObject *arg2 = PyLong_FromLong(y);
  PyObject *pRes = PyObject_CallFunctionObjArgs(cbk, pID, arg1, arg2, inRefcon, NULL);
  PyObject *err = PyErr_Occurred();
  Py_DECREF(arg1);
  Py_DECREF(arg2);
  Py_DECREF(pID);
  if(err){
    pythonLogException();
    return 0;
  }
  int res = (int)PyLong_AsLong(pRes);
  Py_DECREF(pRes);
  errCheck("end handleCursor");
  return res;
}

static int handleMouseWheel(XPLMWindowID  inWindowID,
                     int           x,
                     int           y,
                     int           wheel,
                     int           clicks,
                     void         *inRefcon)
{
  errCheck("prior handleMouseWheel");
  (void) inRefcon;
  PyObject *pID = getPtrRef(inWindowID, windowIDCapsules, windowIDRef);
  PyObject *pCbks = PyDict_GetItem(windowDict, pID);
  if(pCbks == NULL){
    printf("Unknown window passed to handleMouseWheel (%p).\n", inWindowID);
    return 1;
  }
  PyObject *cbk = PyTuple_GetItem(pCbks, WINDOW_WHEEL);
  set_moduleName(PyTuple_GetItem(pCbks, WINDOW_MODULE_NAME));
  if((cbk == NULL) || (cbk == Py_None)){
    return 1;
  }
  PyObject *arg1 = PyLong_FromLong(x);
  PyObject *arg2 = PyLong_FromLong(y);
  PyObject *arg3 = PyLong_FromLong(wheel);
  PyObject *arg4 = PyLong_FromLong(clicks);
  PyObject *pRes = PyObject_CallFunctionObjArgs(cbk, pID, arg1, arg2, arg3, arg4, inRefcon, NULL);
  PyObject *err = PyErr_Occurred();
  Py_DECREF(arg1);
  Py_DECREF(arg2);
  Py_DECREF(arg3);
  Py_DECREF(arg4);
  Py_DECREF(pID);
  if(err){
    pythonLogException();
    return 1;
  }
  int res = (int)PyLong_AsLong(pRes);
  Py_DECREF(pRes);
  errCheck("end handleMouseWheel");
  return res;
}


My_DOCSTR(_createWindowEx__doc__, "createWindowEx", "left=100, top=200, right=200, bottom=100, visible=0, draw=None, click=None, key=None, cursor=None, wheel=None, refCon=None, decoration=WindowDecorationRoundRectangle, layer=WindowLayerFloatingWindows, rightClick=None",
          "Creates modern window\n"
          "\n"
          "Callback functions are optional:\n"
          "  draw(windowID, refCon) with no return value.\n"
          "  key(windowID, key, flags, vKey, refCon, losingFocus) with no return value.\n"
          "  cursor(windowID, x, y, refCon) returns updated CursorStatus\n"
          "  click(windowID, x, y, mouseStatus, refCon) with 1= consume click, else 0.\n"
          "  rightClick(windowID, x, y, mouseStatus, refCon) with 1= consume click, else 0.\n"
          "  wheel(windowID, x, y, wheel, clicks, refCon) with 1= consume click, else 0.\n"
          "\n"
          "Returns new windowID.");
          
                                                                                       
static PyObject *XPLMCreateWindowExFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  errCheck("prior CreateWindowEx");
  static char *keywords[] = {"left", "top", "right", "bottom", "visible", "draw", "click", "key", "cursor", "wheel", "refCon", "decoration", "layer", "rightClick", NULL};
  (void) self;
  PyObject *firstObj=Py_None;
  int left=100, right=200, top=200, bottom=100;
  int visible=0;
  int decoration=xplm_WindowDecorationRoundRectangle;
  int layer=xplm_WindowLayerFloatingWindows;
  PyObject *draw=Py_None, *click=Py_None, *key=Py_None, *cursor=Py_None, *wheel=Py_None, *rightClick=Py_None, *refCon=Py_None;
  PyObject *paramsObj=Py_None;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|OiiiiOOOOOOiiO", keywords,
                                  &firstObj, &top, &right, &bottom, &visible,
                                  &draw, &click, &key, &cursor, &wheel, &refCon,
                                  &decoration, &layer, &rightClick)){
    return NULL;
  }
  if (firstObj == Py_None) {
    ; /* ``left'' remains as default value */
  } else if (PyLong_Check(firstObj)) {
    left = PyLong_AsLong(firstObj);
  } else if (PySequence_Check(firstObj)) {
    paramsObj = firstObj;
    if (PySequence_Length(paramsObj) != 14) {
      PyErr_SetString(PyExc_AttributeError ,"createWindowEx tuple did not contain 14 values\n.");
      return NULL;
    }
  } else {
    PyErr_SetString(PyExc_AttributeError ,"createWindowEx could not parse arguments.\n");
    return NULL;
  }
    
  XPLMCreateWindow_t params;
  params.structSize = sizeof(params);
  if (paramsObj != Py_None){
    PyObject *paramsTuple = PySequence_Tuple(paramsObj);
    params.left = PyLong_AsLong(PyTuple_GetItem(paramsTuple, 0));
    params.top = PyLong_AsLong(PyTuple_GetItem(paramsTuple, 1));
    params.right = PyLong_AsLong(PyTuple_GetItem(paramsTuple, 2));
    params.bottom = PyLong_AsLong(PyTuple_GetItem(paramsTuple, 3));
    params.visible = PyLong_AsLong(PyTuple_GetItem(paramsTuple, 4));
    params.decorateAsFloatingWindow = PyLong_AsLong(PyTuple_GetItem(paramsTuple, 11));
    params.layer = PyLong_AsLong(PyTuple_GetItem(paramsTuple, 12));
    params.refcon = PyTuple_GetItem(paramsTuple, 10);
    
    draw = PyTuple_GetItem(paramsTuple, 5);
    click = PyTuple_GetItem(paramsTuple, 6);
    key = PyTuple_GetItem(paramsTuple, 7);
    cursor = PyTuple_GetItem(paramsTuple, 8);
    wheel = PyTuple_GetItem(paramsTuple, 9);
    rightClick = PyTuple_GetItem(paramsTuple, 13);

    Py_DECREF(paramsTuple);
  } else {
    params.left = left;
    params.right = right;
    params.top = top;
    params.bottom = bottom;
    params.visible = visible;
    params.decorateAsFloatingWindow = decoration;
    params.layer = layer;
    params.refcon = refCon;
  }

  if (!PyCallable_Check(draw) && draw != Py_None) {
    PyErr_SetString(PyExc_ValueError ,"createWindowEx draw() is not callable.\n");
    return NULL;
  }
  if (!PyCallable_Check(click) && click != Py_None) {
    PyErr_SetString(PyExc_ValueError ,"createWindowEx click() is not callable.\n");
    return NULL;
  }
  if (!PyCallable_Check(key) && key != Py_None) {
    PyErr_SetString(PyExc_ValueError ,"createWindowEx key() is not callable.\n");
    return NULL;
  }
  
  if (!PyCallable_Check(cursor) && cursor != Py_None) {
    PyErr_SetString(PyExc_ValueError ,"createWindowEx cursor() is not callable.\n");
    return NULL;
  }
  
  if (!PyCallable_Check(wheel) && wheel != Py_None) {
    PyErr_SetString(PyExc_ValueError ,"createWindowEx wheel() is not callable.\n");
    return NULL;
  }
  
  if (!PyCallable_Check(rightClick) && rightClick != Py_None) {
    PyErr_SetString(PyExc_ValueError ,"createWindowEx rightClick() is not callable.\n");
    return NULL;
  }
  
  Py_INCREF(draw);
  Py_INCREF(click);
  Py_INCREF(key);
  Py_INCREF(cursor);
  Py_INCREF(wheel);
  Py_INCREF(rightClick);
  
  Py_INCREF(params.refcon);

  PyObject *cbkTuple = Py_BuildValue("(OOOOOOs)", draw, click, key, cursor, wheel, rightClick, CurrentPythonModuleName);
  params.drawWindowFunc = drawWindow;
  params.handleMouseClickFunc = handleMouseClick;
  params.handleKeyFunc = handleKey;
  params.handleCursorFunc = handleCursor;
  params.handleMouseWheelFunc = handleMouseWheel;
  params.handleRightClickFunc = handleRightClick;

  if(!cbkTuple){
    PyErr_SetString(PyExc_RuntimeError ,"XPLMCreateWindowEx couldn't create a callback tuple.\n");
    return NULL;
  }
  XPLMWindowID id = XPLMCreateWindowEx(&params);
  PyObject *pID = getPtrRef(id, windowIDCapsules, windowIDRef);
  PyDict_SetItem(windowDict, pID, cbkTuple);
  Py_DECREF(cbkTuple);
  errCheck("end createWindowEx");
  return pID;
}

My_DOCSTR(_createWindow__doc__, "createWindow", "left=100, top=200, right=200, bottom=100, visible=0, draw=None, key=None, mouse=None, refCon=None",
          "(Deprecated do not use)");
static PyObject *XPLMCreateWindowFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  errCheck("prior createWindow");
  static char *keywords[] = {"left", "top", "right", "bottom", "visible", "draw", "key", "mouse", "refCon", NULL};
  (void) self;
  PyObject *drawCallback=Py_None, *keyCallback=Py_None, *mouseCallback=Py_None, *refcon=Py_None;
  int left=100, top=200, right=200, bottom=100, visible=0;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|iiiiiOOOO", keywords, &left, &top, &right, &bottom, &visible, 
                                  &drawCallback, &keyCallback, &mouseCallback, &refcon)){
    return NULL;
  }

  PyObject *cbkTuple = Py_BuildValue("(OOOOOO)", drawCallback, mouseCallback, keyCallback, Py_None, Py_None, Py_None);
  if(!cbkTuple){
    PyErr_SetString(PyExc_RuntimeError ,"XPLMCreateWindow couldn't create a tuple.\n");
    return NULL;
  }

  Py_INCREF(refcon);
  XPLMWindowID id = XPLMCreateWindow(left, top, right, bottom, visible, drawWindow, handleKey, handleMouseClick, refcon);

  PyObject *pID = getPtrRef(id, windowIDCapsules, windowIDRef);
  PyDict_SetItem(windowDict, pID, cbkTuple);
  Py_DECREF(cbkTuple);
  errCheck("end createWindow");
  return pID;
}

My_DOCSTR(_destroyWindow__doc__, "destroyWindow", "windowID",
          "Destroys window. Returns None.");
static PyObject *XPLMDestroyWindowFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"windowID", NULL};
  (void) self;
  PyObject *pID;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &pID)){
    return NULL;
  }
  if(PyDict_Contains(windowDict, pID)){
    XPLMWindowID winID = refToPtr(pID, windowIDRef);
    PyObject *tmp = XPLMGetWindowRefCon(winID);
    Py_DECREF(tmp);
    XPLMDestroyWindow(winID);
    PyDict_DelItem(windowDict, pID);
  }else{
    PyErr_SetString(PyExc_RuntimeError ,"XPLMDestroyWindow couldn't find the window to destroy.\n");
    return NULL;
  }
  Py_RETURN_NONE;
}

My_DOCSTR(_getScreenSize__doc__, "getScreenSize", "", /* Documentation says, No Parameters. (&width, &height) is old C-style */
          "Returns (width, height) of screen.");
static PyObject *XPLMGetScreenSizeFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"width", "height", NULL};
  (void) self;
  (void) args;
  int w, h;
  XPLMGetScreenSize(&w, &h);
  PyObject *outWidth, *outHeight;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "OO", keywords, &outWidth, &outHeight)) {
    /* if we fail to parse, that's okay: just return the correct values */
    PyErr_Clear();
    return Py_BuildValue("(ii)", w, h);
  }
  pythonLogWarning("XPLMGetScreenSize no longer requires parameters");
  if (outWidth != Py_None)
    PyList_Append(outWidth, PyLong_FromLong(w));
  if (outHeight != Py_None)
    PyList_Append(outHeight, PyLong_FromLong(h));
  Py_RETURN_NONE;
}

My_DOCSTR(_getScreenBoundsGlobal__doc__, "getScreenBoundsGlobal", ""/* NO params */,
          "Returns (left, top, right, bottom) of screen bounds.");
static PyObject *XPLMGetScreenBoundsGlobalFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"left", "top", "right", "bottom", NULL};
  (void) self;
  (void) args;
  if(!XPLMGetScreenBoundsGlobal_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMGetScreenBoundsGlobal is available only in XPLM300 and up.");
    return NULL;
  }
  PyObject *outLeft, *outTop, *outRight, *outBottom;
  int left, top, right, bottom;
  XPLMGetScreenBoundsGlobal_ptr(&left, &top, &right, &bottom);
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OOOO", keywords, &outLeft, &outTop, &outRight, &outBottom)) {
    /* if parse error, just return the values */
    PyErr_Clear();
    return Py_BuildValue("(iiii)", left, top, right, bottom);
  }
  pythonLogWarning("XPLMGetScreenBoundsGlobal no longer requires parameters");
  if (outLeft != Py_None)
    PyList_Append(outLeft, PyLong_FromLong(left));
  if (outTop != Py_None)
    PyList_Append(outTop, PyLong_FromLong(top));
  if (outRight != Py_None)
    PyList_Append(outRight, PyLong_FromLong(right));
  if (outBottom != Py_None)
    PyList_Append(outBottom, PyLong_FromLong(left));
  Py_RETURN_NONE;
}

My_DOCSTR(_getAllMonitorBoundsGlobal__doc__, "getAllMonitorBoundsGlobal", "bounds, refCon",
          "Immediately calls bounds() callback once for each monitor to retrieve\n"
          "bounds for each monitor running full-screen simulator.\n"
          "\n"
          "Callback function is\n"
          "  bounds(index, left, top, right, bottom, refCon)");

static PyObject *XPLMGetAllMonitorBoundsGlobalFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"bounds", "refCon", NULL};
  (void) self;
  PyObject *refconObj;
  if(!XPLMGetAllMonitorBoundsGlobal_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMGetAllMonitorBoundsGlobal is available only in XPLM300 and up.");
    return NULL;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "OO", keywords, &monitorBndsCallback, &refconObj)){
    return NULL;
  }
  XPLMGetAllMonitorBoundsGlobal_ptr(receiveMonitorBounds, (void *)refconObj);
  Py_RETURN_NONE;
}

My_DOCSTR(_getAllMonitorBoundsOS__doc__, "getAllMonitorBoundsOS", "bounds, refCon",
          "Immediately calls bounds() once for each monitor known to OS.\n"
          "\n"
          "Callback function is\n"
          "  bounds(index, refCon, left, top, right, bottom, refCon)");
static PyObject *XPLMGetAllMonitorBoundsOSFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"bounds", "refCon", NULL};
  (void) self;
  PyObject *refconObj;
  if(!XPLMGetAllMonitorBoundsOS_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMGetAllMonitorBoundsOS is available only in XPLM300 and up.");
    return NULL;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "OO", keywords, &monitorBndsCallback, &refconObj)){
    return NULL;
  }
  XPLMGetAllMonitorBoundsOS_ptr(receiveMonitorBounds, (void *)refconObj);
  Py_RETURN_NONE;
}

My_DOCSTR(_getMouseLocation__doc__, "getMouseLocation", ""/* No params*/,
          "Deprecated, use getMouseLocationGlobal().");
static PyObject *XPLMGetMouseLocationFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"x", "y", NULL};
  (void) self;
  (void) args;
  pythonLogWarning("XPLMGetMouseLocation is deprecated, use XPLMGetMouseLocationGlobal.");
  int x, y;
  XPLMGetMouseLocation(&x, &y);
  PyObject *outX, *outY;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "OO", keywords, &outX, &outY)) {
    /* if parse error, just return the values */
    PyErr_Clear();
    return Py_BuildValue("(ii)", x, y);
  }
  if (outX != Py_None)
    PyList_Append(outX, PyLong_FromLong(x));
  if (outY != Py_None)
    PyList_Append(outY, PyLong_FromLong(y));
  Py_RETURN_NONE;
}

My_DOCSTR(_getMouseLocationGlobal__doc__, "getMouseLocationGlobal", ""/* no params */,
          "Returns current mouse location (x, y).");
static PyObject *XPLMGetMouseLocationGlobalFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"x", "y", NULL};
  (void) self;
  (void) args;
  int x, y;
  if(!XPLMGetMouseLocationGlobal_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMGetMouseLocationGlobal is available only in XPLM300 and up.");
    return NULL;
  }
  XPLMGetMouseLocationGlobal_ptr(&x, &y);
  PyObject *outX, *outY;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "OO", keywords, &outX, &outY)) {
    /* if parse error, just return values */
    PyErr_Clear();
    return Py_BuildValue("(ii)", x, y);
  }
  if (outX != Py_None)
    PyList_Append(outX, PyLong_FromLong(x));
  if (outY != Py_None)
    PyList_Append(outY, PyLong_FromLong(y));
  Py_RETURN_NONE;
}

My_DOCSTR(_getWindowGeometry__doc__, "getWindowGeometry", "windowID",
          "Returns window geometry (left, top, right, bottom).");
static PyObject *XPLMGetWindowGeometryFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"windowID", "left", "top", "right", "bottom", NULL};
  (void) self;
  PyObject *win;
  PyObject *outLeft, *outTop, *outRight, *outBottom;
  int returnValues = 0;
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OOOOO", keywords, &win, &outLeft, &outTop, &outRight, &outBottom)) {
    returnValues = 1;
    PyErr_Clear();
    static char *nkeywords[] = {"windowID"};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O", nkeywords, &win)) {
      return NULL;
    }
  }
  int left, top, right, bottom;
  XPLMGetWindowGeometry(refToPtr(win, windowIDRef), &left, &top, &right, &bottom);
  if (returnValues) {
    return Py_BuildValue("(iiii)", left, top, right, bottom);
  }
  pythonLogWarning("XPLMGetWindowGeometry only requires initial windowID parameter");
  if (outLeft != Py_None)
    PyList_Append(outLeft, PyLong_FromLong(left));
  if (outTop != Py_None)
    PyList_Append(outTop, PyLong_FromLong(top));
  if (outRight != Py_None)
    PyList_Append(outRight, PyLong_FromLong(right));
  if (outBottom != Py_None)
    PyList_Append(outBottom, PyLong_FromLong(bottom));
  Py_RETURN_NONE;
}
  
My_DOCSTR(_setWindowGeometry__doc__, "setWindowGeometry", "windowID, left, top, right, bottom",
          "Sets window geometry.");
static PyObject *XPLMSetWindowGeometryFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"windowID", "left", "top", "right", "bottom", NULL};
  (void) self;
  PyObject *win;
  int inLeft, inTop, inRight, inBottom;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Oiiii", keywords, &win, &inLeft, &inTop, &inRight, &inBottom)){
    return NULL;
  }
  void *inWindowID = refToPtr(win, windowIDRef);
  XPLMSetWindowGeometry(inWindowID, inLeft, inTop, inRight, inBottom);
  Py_RETURN_NONE;
}

My_DOCSTR(_getWindowGeometryOS__doc__, "getWindowGeometryOS", "windowID",
          "Returns window geometry for popped-out window (left, top, right, bottom).");
static PyObject *XPLMGetWindowGeometryOSFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"windowID", "left", "top", "right", "bottom", NULL};
  (void) self;
  PyObject *win;
  PyObject *outLeft, *outTop, *outRight, *outBottom;
  int returnValues = 0;
  if(!XPLMGetWindowGeometryOS_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMGetWindowGeometryOS is available only in XPLM300 and up.");
    return NULL;
  }
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OOOOO", keywords, &win, &outLeft, &outTop, &outRight, &outBottom)) {
    returnValues = 1;
    PyErr_Clear();
    static char *nkeywords[] = {"windowID"};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O", nkeywords, &win)) {
      return NULL;
    }
  }
  int left, top, right, bottom;
  XPLMGetWindowGeometryOS_ptr(refToPtr(win, windowIDRef), &left, &top, &right, &bottom);
  if (returnValues) {
    return Py_BuildValue("(iiii)", left, top, right, bottom);
  }
  pythonLogWarning("XPLMGetWindowGeometryOS only requires initial windowID parameter");
  if (outLeft != Py_None)
    PyList_Append(outLeft, PyLong_FromLong(left));
  if (outTop != Py_None)
    PyList_Append(outTop, PyLong_FromLong(top));
  if (outRight != Py_None)
    PyList_Append(outRight, PyLong_FromLong(right));
  if (outBottom != Py_None)
    PyList_Append(outBottom, PyLong_FromLong(bottom));
  Py_RETURN_NONE;
}

My_DOCSTR(_setWindowGeometryOS__doc__, "setWindowGeometryOS", "windowID, left, top, right, bottom",
          "Sets window geometry for popped-out window.");
static PyObject *XPLMSetWindowGeometryOSFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"windowID", "left", "top", "right", "bottom", NULL};
  (void) self;
  PyObject *win;
  int inLeft, inTop, inRight, inBottom;
  if(!XPLMSetWindowGeometryOS_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMSetWindowGeometryOS is available only in XPLM300 and up.");
    return NULL;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Oiiii", keywords, &win, &inLeft, &inTop, &inRight, &inBottom)){
    return NULL;
  }
  void *inWindowID = refToPtr(win, windowIDRef);
  XPLMSetWindowGeometryOS_ptr(inWindowID, inLeft, inTop, inRight, inBottom);
  Py_RETURN_NONE;
}

My_DOCSTR(_getWindowGeometryVR__doc__, "getWindowGeometryVR", "windowID",
          "Gets window geometry for VR window (width, height)");
static PyObject *XPLMGetWindowGeometryVRFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {NULL};
  (void) self;
  PyObject *win;
  int returnValues = 0;
  PyObject *outWidthBoxels, *outHeightBoxels;
  if(!XPLMGetWindowGeometryVR_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMGetWindowGeometryVR is available only in XPLM301 and up.");
    return NULL;
  }
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OOO", keywords, &win, &outWidthBoxels, &outHeightBoxels)) {
    returnValues = 1;
    PyErr_Clear();
    if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &win)){
      return NULL;
    }
  }
  int widthBoxels, heightBoxels;
  XPLMGetWindowGeometryVR_ptr(refToPtr(win, windowIDRef), &widthBoxels, &heightBoxels);
  if (returnValues) {
    return Py_BuildValue("(ii)", widthBoxels, heightBoxels);
  }
  pythonLogWarning("XPLMGetWindowGeometryVR only requires initial windowID parameter");
  if (outWidthBoxels != Py_None)
    PyList_Append(outWidthBoxels, PyLong_FromLong(widthBoxels));
  if (outHeightBoxels != Py_None)
    PyList_Append(outHeightBoxels, PyLong_FromLong(heightBoxels));
  Py_RETURN_NONE;
}

My_DOCSTR(_setWindowGeometryVR__doc__, "setWindowGeometryVR", "windowID, width, height",
          "Sets window geometry for VR window.");
static PyObject *XPLMSetWindowGeometryVRFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"windowID", "width", "height", NULL};
  (void) self;
  PyObject *win;
  int widthBoxels, heightBoxels;
  if(!XPLMSetWindowGeometryVR_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMSetWindowGeometryVR is available only in XPLM301 and up.");
    return NULL;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Oii", keywords, &win, &widthBoxels, &heightBoxels)){
    return NULL;
  }
  void *inWindowID = refToPtr(win, windowIDRef);
  XPLMSetWindowGeometryVR_ptr(inWindowID, widthBoxels, heightBoxels);
  Py_RETURN_NONE;
}

My_DOCSTR(_getWindowIsVisible__doc__, "getWindowIsVisible", "windowID",
          "Returns 1 if window is visible, 0 otherwise.");
static PyObject *XPLMGetWindowIsVisibleFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"windowID", NULL};
  (void) self;
  PyObject *win;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &win)){
    return NULL;
  }
  void *inWindowID = refToPtr(win, windowIDRef);
  return PyLong_FromLong(XPLMGetWindowIsVisible(inWindowID));
}

My_DOCSTR(_setWindowIsVisible__doc__, "setWindowIsVisible", "windowID, visible=1",
          "Sets window visibility. 1 indicates visible, 0 is not-visible.");
static PyObject *XPLMSetWindowIsVisibleFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"windowID", "visible=1", NULL};
  (void) self;
  PyObject *win;
  int inIsVisible = 1;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|i", keywords, &win, &inIsVisible)){
    return NULL;
  }
  XPLMWindowID inWindowID = refToPtr(win, windowIDRef);
  XPLMSetWindowIsVisible(inWindowID, inIsVisible);
  Py_RETURN_NONE;
}

My_DOCSTR(_windowIsPoppedOut__doc__, "windowIsPoppedOut", "windowID",
          "Returns 1 if window is popped-out, 0 otherwise.");
static PyObject *XPLMWindowIsPoppedOutFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"windowID", NULL};
  (void) self;
  PyObject *win;
  if(!XPLMWindowIsPoppedOut_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMWindowIsPoppedOut is available only in XPLM300 and up.");
    return NULL;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &win)){
    return NULL;
  }
  void *inWindowID = refToPtr(win, windowIDRef);
  return PyLong_FromLong(XPLMWindowIsPoppedOut_ptr(inWindowID));
}

My_DOCSTR(_windowIsInVR__doc__, "windowIsInVR", "windowID",
          "Returns 1 if window is in VR, 0 otherwise.");
static PyObject *XPLMWindowIsInVRFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"windowID", NULL};
  (void) self;
  PyObject *win;
  if(!XPLMWindowIsInVR_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMWindowIsInVR is available only in XPLM301 and up.");
    return NULL;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &win)){
    return NULL;
  }
  void *inWindowID = refToPtr(win, windowIDRef);
  return PyLong_FromLong(XPLMWindowIsInVR_ptr(inWindowID));
}

My_DOCSTR(_setWindowGravity__doc__, "setWindowGravity", "windowID, left, top, right, bottom",
          "Set window's gravity (resize window, relative screen size changes.)\n"
          "\n"
          "values typically range 0.0 - 1.0, and reflect how much the window's\n"
          "value will change relative to change in screen size.\n"
          "0.0 means zero change relative to the right side (for left/right edges)\n"
          "    and zero change relative to the bottom (for (for top/bottom edges)\n"
          "1.0 means 100% change relative to the right side / bottom");
static PyObject *XPLMSetWindowGravityFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"windowID", "left", "top", "right", "bottom", NULL};
  (void) self;
  PyObject *win;
  float inLeftGravity, inTopGravity, inRightGravity, inBottomGravity;
  if(!XPLMSetWindowGravity_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMSetWindowGravity is available only in XPLM300 and up.");
    return NULL;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Offff", keywords, &win, &inLeftGravity, &inTopGravity, &inRightGravity, &inBottomGravity)){
    return NULL;
  }
  XPLMWindowID inWindowID = refToPtr(win, windowIDRef);
  XPLMSetWindowGravity_ptr(inWindowID, inLeftGravity, inTopGravity, inRightGravity, inBottomGravity);
  Py_RETURN_NONE;
}

My_DOCSTR(_setWindowResizingLimits__doc__, "setWindowResizingLimits", "windowID, minWidth=0, minHeight=0, maxWidth=10000, maxHeight=10000",
          "Set maximum and minimum window size.");
static PyObject *XPLMSetWindowResizingLimitsFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"windowID", "minWidth", "minHeight", "maxWidth", "maxHeight", NULL};
  (void) self;
  PyObject *win;
  int inMinWidthBoxels=0, inMinHeightBoxels=0, inMaxWidthBoxels=10000, inMaxHeightBoxels=10000;
  if(!XPLMSetWindowResizingLimits_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMSetWindowResizingLimits is available only in XPLM300 and up.");
    return NULL;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|iiii", keywords, &win, &inMinWidthBoxels, &inMinHeightBoxels, &inMaxWidthBoxels, &inMaxHeightBoxels)){
    return NULL;
  }
  XPLMWindowID inWindowID = refToPtr(win, windowIDRef);
  inMinWidthBoxels = inMinWidthBoxels < 0 ? 0 : inMinWidthBoxels;
  XPLMSetWindowResizingLimits_ptr(inWindowID, inMinWidthBoxels, inMinHeightBoxels, inMaxWidthBoxels, inMaxHeightBoxels);
  Py_RETURN_NONE;
}

My_DOCSTR(_setWindowPositioningMode__doc__, "setWindowPositioningMode", "windowID, mode, index=-1",
          "Set window positioning mode:\n"
          " * WindowPositionFree\n"
          " * WindowCenterOnMonitor\n"
          " * WindowFullScreenOnMonitor\n"
          " * WindowFullScreenOnAllMonitors\n"
          " * WindowPopOut\n"
          " * WindowVR");
static PyObject *XPLMSetWindowPositioningModeFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"windowID", "mode", "index", NULL};
  (void) self;
  PyObject *win;
  int inPositioningMode, inMonitorIndex=-1;
  if(!XPLMSetWindowPositioningMode_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMSetWindowPositioningMode is available only in XPLM300 and up.");
    return NULL;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Oi|i", keywords, &win, &inPositioningMode, &inMonitorIndex)){
    return NULL;
  }
  XPLMWindowID inWindowID = refToPtr(win, windowIDRef);
  XPLMSetWindowPositioningMode_ptr(inWindowID, inPositioningMode, inMonitorIndex);
  Py_RETURN_NONE;
}

My_DOCSTR(_setWindowTitle__doc__, "setWindowTitle", "windowID, title",
          "Set window title.");
static PyObject *XPLMSetWindowTitleFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"windowID", "title", NULL};
  (void) self;
  PyObject *win;
  const char *inWindowTitle;
  if(!XPLMSetWindowTitle_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMSetWindowTitle is available only in XPLM300 and up.");
    return NULL;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Os", keywords, &win, &inWindowTitle)){
    return NULL;
  }
  XPLMWindowID inWindowID = refToPtr(win, windowIDRef);
  XPLMSetWindowTitle_ptr(inWindowID, strdup(inWindowTitle));
  Py_RETURN_NONE;
}

My_DOCSTR(_getWindowRefCon__doc__, "getWindowRefCon", "windowID",
          "Return window's reference constant refCon.");
static PyObject *XPLMGetWindowRefConFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"windowID", NULL};
  (void) self;
  PyObject *win;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &win)){
    return NULL;
  }
  XPLMWindowID inWindowID = refToPtr(win, windowIDRef);
  PyObject *res = XPLMGetWindowRefCon(inWindowID);
  // Needs to be done, because python decrefs it when the function
  //   that called us returns; otherwise all hell breaks loose!!!
  // TODO: Check no other instance of such a problem lingers in the interface!!!
  Py_INCREF(res);
  return res;
}

My_DOCSTR(_setWindowRefCon__doc__, "setWindowRefCon", "windowID, refCon",
          "Set window's reference constant refCon.");
static PyObject *XPLMSetWindowRefConFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"windowID", "refCon", NULL};
  (void) self;
  PyObject *win, *inRefcon;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "OO", keywords, &win, &inRefcon)){
    return NULL;
  }
  XPLMWindowID inWindowID = refToPtr(win, windowIDRef);
  // Decref the old refcon
  PyObject *tmp = XPLMGetWindowRefCon(inWindowID);
  Py_DECREF(tmp);
  // Make sure it stays with us
  Py_INCREF(inRefcon);
  XPLMSetWindowRefCon(inWindowID, inRefcon);
  Py_RETURN_NONE;
}

My_DOCSTR(_takeKeyboardFocus__doc__, "takeKeyboardFocus", "windowID",
          "Take keyboard focus. 0 to send focus to X-Plane.");
static PyObject *XPLMTakeKeyboardFocusFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"windowID", NULL};
  (void) self;
  PyObject *win;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &win)){
    return NULL;
  }
  // use inWindowID 0, if passed in value of 0
  XPLMWindowID inWindowID;
  if (PyLong_Check(win) && PyLong_AsLong(win) == 0) {
    inWindowID = 0;
  } else {
    inWindowID = refToPtr(win, windowIDRef);
  }
  XPLMTakeKeyboardFocus(inWindowID);
  Py_RETURN_NONE;
}

My_DOCSTR(_hasKeyboardFocus__doc__, "hasKeyboardFocus", "windowID",
          "Returns 1 if window has current keyboard focus.\n"
          "Pass 0 to query if X-Plane has current focus.");
static PyObject *XPLMHasKeyboardFocusFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"windowID", NULL};
  (void) self;
  PyObject *win;
  if(!XPLMHasKeyboardFocus_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMHasKeyboardFocus is available only in XPLM300 and up.");
    return NULL;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &win)){
    return NULL;
  }
  // use inWindowID 0, if passed in value of 0
  XPLMWindowID inWindowID;
  if (PyLong_Check(win) && PyLong_AsLong(win) == 0) {
    inWindowID = 0;
  } else {
    inWindowID = refToPtr(win, windowIDRef);
  }
  
  return PyLong_FromLong(XPLMHasKeyboardFocus_ptr(inWindowID));
}

My_DOCSTR(_bringWindowToFront__doc__, "bringWindowToFront", "windowID",
          "Bring window to front (of it's window layer).");
static PyObject *XPLMBringWindowToFrontFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"windowID", NULL};
  (void) self;
  PyObject *win;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &win)){
    return NULL;
  }
  XPLMWindowID inWindowID = refToPtr(win, windowIDRef);
  XPLMBringWindowToFront(inWindowID);
  Py_RETURN_NONE;
}

My_DOCSTR(_isWindowInFront__doc__, "isWindowInFront", "windowID",
          "Returns 1 if window is currently in the front of it's window layer).");
static PyObject *XPLMIsWindowInFrontFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"windowID", NULL};
  (void) self;
  PyObject *win;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &win)){
    return NULL;
  }
  XPLMWindowID inWindowID = refToPtr(win, windowIDRef);
  return PyLong_FromLong(XPLMIsWindowInFront(inWindowID));
}

void hotkeyCallback(void *inRefcon)
{
  errCheck("prior hotkeyCallback");
  PyObject *pRefcon = PyLong_FromVoidPtr(inRefcon);
  PyObject *pCbk = PyDict_GetItem(hotkeyDict, pRefcon);
  Py_DECREF(pRefcon);
  if(pCbk == NULL){
    printf("Unknown refcon passed to hotkeyCallback (%p).\n", inRefcon);
    return;
  }
  set_moduleName(PyTuple_GetItem(pCbk, HOTKEY_MODULE_NAME));
  PyObject *res = PyObject_CallFunctionObjArgs(PyTuple_GetItem(pCbk, HOTKEY_CALLBACK), PyTuple_GetItem(pCbk, HOTKEY_REFCON), NULL);
  PyObject *err = PyErr_Occurred();
  if(err){
    pythonDebug("exception in hotkey callback\n");
    pythonLogException();
  }
  Py_XDECREF(res);  // in case hotkey doesn't happent to return anything
  errCheck("end hotkeyCallback");
}

My_DOCSTR(_registerHotKey__doc__, "registerHotKey", "vkey, flags=0, description=\"\", hotKey=None, refCon=None",
          "Registers hot key."
          "\n"
          "Callback is hotKey(refCon), it does not need to return anything.\n"
          "Registration returns a hotKeyID, which can be used with unregisterHotKey()");
static PyObject *XPLMRegisterHotKeyFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  errCheck("prior registerHotKey");
  static char *keywords[] = {"vkey", "flags", "description", "hotKey", "refCon", NULL};
  (void) self;
  PyObject *inCallback = Py_None, *refcon = Py_None;
  int inVirtualKey, inFlags = 0;
  PyObject *hkTuple;
  const char *inDescription;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "i|isOO", keywords, &inVirtualKey, &inFlags, &inDescription, &inCallback, &refcon)){
    return NULL;
  }
  if (!PyCallable_Check(inCallback)) {
    PyErr_SetString(PyExc_ValueError ,"hotKey() not callable.\n");
    return NULL;
  }
  hkTuple = Py_BuildValue("(OOs)", inCallback, refcon, CurrentPythonModuleName);
  if(!hkTuple){
    PyErr_SetString(PyExc_RuntimeError ,"registerHotKey couldn't build value.\n");
    return NULL;
  }

  void *inRefcon = (void *)++hotkeyCntr;
  PyObject *pRefcon = PyLong_FromVoidPtr(inRefcon);  // new
  //Store the callback and original refcon
  PyDict_SetItem(hotkeyDict, pRefcon, hkTuple); // does not steal reference to hkTuple

  XPLMHotKeyID id = XPLMRegisterHotKey(inVirtualKey, inFlags, inDescription, hotkeyCallback, inRefcon);
  PyObject *pId = getPtrRef(id, hotkeyIDCapsules, hotkeyIDRef);
  //Allows me to identify my unique refcon based on hotkey id 
  PyDict_SetItem(hotkeyIDDict, pId, pRefcon);  // does not steal reference
  Py_DECREF(pRefcon);

  errCheck("end registerHotKey");
  PyObject *err = PyErr_Occurred();
  if (err) {
    pythonDebug("Error at end of registerHotKey\n");
    pythonLogException();
  }
  return pId;
} 

My_DOCSTR(_unregisterHotKey__doc__, "unregisterHotKey", "hotKeyID",
          "Unregisters hot key associated with hotKeyID.\n"
          "\n"
          "hotKeyID must be registered to this plugin using registerHotKey()\n"
          "otherwise unregistration will fail.");
static PyObject *XPLMUnregisterHotKeyFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"hotKey", NULL};
  (void) self;
  PyObject *hotKey;
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &hotKey)){
    return NULL;
  }
  PyObject *pRefcon = PyDict_GetItem(hotkeyIDDict, hotKey);
  if(pRefcon == NULL){
    PyErr_SetString(PyExc_RuntimeError ,"XPLMUnregisterHotKey couldn't find hotkey ID.\n");
    Py_RETURN_NONE;
  }
  PyObject *pCbk = PyDict_GetItem(hotkeyDict, pRefcon);
  if(pCbk == NULL){
    PyErr_SetString(PyExc_RuntimeError ,"XPLMUnregisterHotKey couldn't find refcon.\n");
    Py_RETURN_NONE;
  }
  XPLMHotKeyID *hk = refToPtr(hotKey, hotkeyIDRef);
  XPLMUnregisterHotKey(hk);
  PyDict_DelItem(hotkeyDict, pRefcon);
  PyDict_DelItem(hotkeyIDDict, hotKey);
  removePtrRef(hk, hotkeyIDCapsules);
  Py_RETURN_NONE;
} 

My_DOCSTR(_countHotKeys__doc__, "countHotKeys", "",
          "Return number of hot keys currently defined in the simulator.");
static PyObject *XPLMCountHotKeysFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  return PyLong_FromLong(XPLMCountHotKeys());
} 

My_DOCSTR(_getNthHotKey__doc__, "getNthHotKey", "index",
          "Return hotKeyID for (zero-based) Nth hot key defined in sim.");
static PyObject *XPLMGetNthHotKeyFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"index", NULL};
  (void) self;
  int inIndex;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "i", keywords, &inIndex)){
    return NULL;
  }
  return getPtrRef(XPLMGetNthHotKey(inIndex), hotkeyIDCapsules, hotkeyIDRef);
} 

My_DOCSTR(_getHotKeyInfo__doc__, "getHotKeyInfo", "hotKeyID",
          "Return object with hot key information.\n"
          "\n"
          "  .description\n"
          "  .virtualKey\n"
          "  .flags\n"
          "  .plugin");
static PyObject *XPLMGetHotKeyInfoFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  (void) self;
  PyObject *hotKey, *outVirtualKey, *outFlags, *outDescription, *outPlugin;
  int returnValues = 0;
  static char *keywords[] = {"hotKeyID", "vKey", "flags", "description", "plugin", NULL};
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "OOOOO", keywords, &hotKey, &outVirtualKey, &outFlags, &outDescription, &outPlugin)) {
    PyErr_Clear();
    returnValues = 1;
    static char *nkeywords[] = {"hotKeyID", NULL};
    if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", nkeywords, &hotKey)) {
      return NULL;
    }
  }
  XPLMHotKeyID inHotKey = refToPtr(hotKey, hotkeyIDRef);
  char virtualKey;
  XPLMKeyFlags flags;
  char description[1024];
  XPLMPluginID plugin;
  XPLMGetHotKeyInfo(inHotKey, &virtualKey, &flags, description, &plugin);
  if (returnValues) {
    return PyHotKeyInfo_New(virtualKey, flags, description, plugin);
  }
  pythonLogWarning("getHotKeyInfo only required initial hotKeyID parameter");
  if (outVirtualKey != Py_None)
    PyList_Append(outVirtualKey, PyLong_FromLong((unsigned int)virtualKey));
  if (outFlags != Py_None)
    PyList_Append(outFlags, PyLong_FromLong((unsigned int)flags));
  if (outDescription != Py_None)
    PyList_Append(outDescription, PyUnicode_FromString(description));
  if (outPlugin != Py_None)
    PyList_Append(outPlugin, PyLong_FromLong((unsigned int)plugin));
  Py_RETURN_NONE;
} 

My_DOCSTR(_setHotKeyCombination__doc__, "setHotKeyCombination", "hotKeyID, vKey, flags=0",
          "Update key combination for given hotKeyID to use vKey and flags");
static PyObject *XPLMSetHotKeyCombinationFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"hotKey", "vKey", "flags", NULL};
  (void) self;
  PyObject *hotKey;
  int inVirtualKey;
  int inFlags = 0;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Oi|i", keywords, &hotKey, &inVirtualKey, &inFlags)){
    return NULL;
  }
  void *inHotkey = refToPtr(hotKey, hotkeyIDRef);
  XPLMSetHotKeyCombination(inHotkey, (char)inVirtualKey, inFlags);
  Py_RETURN_NONE;
} 

static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  PyDict_Clear(drawCallbackDict);
  Py_DECREF(drawCallbackDict);
  PyDict_Clear(drawCallbackIDDict);
  Py_DECREF(drawCallbackIDDict);
  PyDict_Clear(keySniffCallbackDict);
  Py_DECREF(keySniffCallbackDict);
  PyDict_Clear(windowDict);
  Py_DECREF(windowDict);
  PyDict_Clear(hotkeyDict);
  Py_DECREF(hotkeyDict);
  PyDict_Clear(hotkeyIDDict);
  Py_DECREF(hotkeyIDDict);
  PyDict_Clear(avionicsCallbacksDict);
  Py_DECREF(avionicsCallbacksDict);
  PyDict_Clear(avionicsCallbacksIDDict);
  Py_DECREF(avionicsCallbacksIDDict);
  PyDict_Clear(windowIDCapsules);
  Py_DECREF(windowIDCapsules);
  PyDict_Clear(hotkeyIDCapsules);
  Py_DECREF(hotkeyIDCapsules);

  PyDict_Clear(avionicsIDCapsules);
  Py_DECREF(avionicsIDCapsules);
  Py_RETURN_NONE;
}


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
static PyMethodDef XPLMDisplayMethods[] = {
  {"registerDrawCallback", (PyCFunction)XPLMRegisterDrawCallbackFun, METH_VARARGS | METH_KEYWORDS, _registerDrawCallback__doc__},
  {"XPLMRegisterDrawCallback", (PyCFunction)XPLMRegisterDrawCallbackFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"unregisterDrawCallback", (PyCFunction)XPLMUnregisterDrawCallbackFun, METH_VARARGS | METH_KEYWORDS, _unregisterDrawCallback__doc__},
  {"XPLMUnregisterDrawCallback", (PyCFunction)XPLMUnregisterDrawCallbackFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"registerKeySniffer", (PyCFunction)XPLMRegisterKeySnifferFun, METH_VARARGS | METH_KEYWORDS, _registerKeySniffer__doc__},
  {"XPLMRegisterKeySniffer", (PyCFunction)XPLMRegisterKeySnifferFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"unregisterKeySniffer", (PyCFunction)XPLMUnregisterKeySnifferFun, METH_VARARGS | METH_KEYWORDS, _unregisterKeySniffer__doc__},
  {"XPLMUnregisterKeySniffer", (PyCFunction)XPLMUnregisterKeySnifferFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"createWindowEx", (PyCFunction)XPLMCreateWindowExFun, METH_VARARGS | METH_KEYWORDS, _createWindowEx__doc__},
  {"XPLMCreateWindowEx", (PyCFunction)XPLMCreateWindowExFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"createWindow", (PyCFunction)XPLMCreateWindowFun, METH_VARARGS | METH_KEYWORDS, _createWindow__doc__},
  {"XPLMCreateWindow", (PyCFunction)XPLMCreateWindowFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"destroyWindow", (PyCFunction)XPLMDestroyWindowFun, METH_VARARGS | METH_KEYWORDS, _destroyWindow__doc__},
  {"XPLMDestroyWindow", (PyCFunction)XPLMDestroyWindowFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"getScreenSize", (PyCFunction)XPLMGetScreenSizeFun, METH_VARARGS | METH_KEYWORDS, _getScreenSize__doc__},
  {"XPLMGetScreenSize", (PyCFunction)XPLMGetScreenSizeFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"getScreenBoundsGlobal", (PyCFunction)XPLMGetScreenBoundsGlobalFun, METH_VARARGS | METH_KEYWORDS, _getScreenBoundsGlobal__doc__},
  {"XPLMGetScreenBoundsGlobal", (PyCFunction)XPLMGetScreenBoundsGlobalFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"getAllMonitorBoundsGlobal", (PyCFunction)XPLMGetAllMonitorBoundsGlobalFun, METH_VARARGS | METH_KEYWORDS, _getAllMonitorBoundsGlobal__doc__},
  {"XPLMGetAllMonitorBoundsGlobal", (PyCFunction)XPLMGetAllMonitorBoundsGlobalFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"getAllMonitorBoundsOS", (PyCFunction)XPLMGetAllMonitorBoundsOSFun, METH_VARARGS | METH_KEYWORDS, _getAllMonitorBoundsOS__doc__},
  {"XPLMGetAllMonitorBoundsOS", (PyCFunction)XPLMGetAllMonitorBoundsOSFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"getMouseLocation", (PyCFunction)XPLMGetMouseLocationFun, METH_VARARGS | METH_KEYWORDS, _getMouseLocation__doc__},
  {"XPLMGetMouseLocation", (PyCFunction)XPLMGetMouseLocationFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"getMouseLocationGlobal", (PyCFunction)XPLMGetMouseLocationGlobalFun, METH_VARARGS | METH_KEYWORDS, _getMouseLocationGlobal__doc__},
  {"XPLMGetMouseLocationGlobal", (PyCFunction)XPLMGetMouseLocationGlobalFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"getWindowGeometry", (PyCFunction)XPLMGetWindowGeometryFun, METH_VARARGS | METH_KEYWORDS, _getWindowGeometry__doc__},
  {"XPLMGetWindowGeometry", (PyCFunction)XPLMGetWindowGeometryFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"setWindowGeometry", (PyCFunction)XPLMSetWindowGeometryFun, METH_VARARGS | METH_KEYWORDS, _setWindowGeometry__doc__},
  {"XPLMSetWindowGeometry", (PyCFunction)XPLMSetWindowGeometryFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"getWindowGeometryOS", (PyCFunction)XPLMGetWindowGeometryOSFun, METH_VARARGS | METH_KEYWORDS, _getWindowGeometryOS__doc__},
  {"XPLMGetWindowGeometryOS", (PyCFunction)XPLMGetWindowGeometryOSFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"setWindowGeometryOS", (PyCFunction)XPLMSetWindowGeometryOSFun, METH_VARARGS | METH_KEYWORDS, _setWindowGeometryOS__doc__},
  {"XPLMSetWindowGeometryOS", (PyCFunction)XPLMSetWindowGeometryOSFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"getWindowIsVisible", (PyCFunction)XPLMGetWindowIsVisibleFun, METH_VARARGS | METH_KEYWORDS, _getWindowIsVisible__doc__},
  {"XPLMGetWindowIsVisible", (PyCFunction)XPLMGetWindowIsVisibleFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"setWindowIsVisible", (PyCFunction)XPLMSetWindowIsVisibleFun, METH_VARARGS | METH_KEYWORDS, _setWindowIsVisible__doc__},
  {"XPLMSetWindowIsVisible", (PyCFunction)XPLMSetWindowIsVisibleFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"windowIsPoppedOut", (PyCFunction)XPLMWindowIsPoppedOutFun, METH_VARARGS | METH_KEYWORDS, _windowIsPoppedOut__doc__},
  {"XPLMWindowIsPoppedOut", (PyCFunction)XPLMWindowIsPoppedOutFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"setWindowGravity", (PyCFunction)XPLMSetWindowGravityFun, METH_VARARGS | METH_KEYWORDS, _setWindowGravity__doc__},
  {"XPLMSetWindowGravity", (PyCFunction)XPLMSetWindowGravityFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"setWindowResizingLimits", (PyCFunction)XPLMSetWindowResizingLimitsFun, METH_VARARGS | METH_KEYWORDS, _setWindowResizingLimits__doc__},
  {"XPLMSetWindowResizingLimits", (PyCFunction)XPLMSetWindowResizingLimitsFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"setWindowPositioningMode", (PyCFunction)XPLMSetWindowPositioningModeFun, METH_VARARGS | METH_KEYWORDS, _setWindowPositioningMode__doc__},
  {"XPLMSetWindowPositioningMode", (PyCFunction)XPLMSetWindowPositioningModeFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"setWindowTitle", (PyCFunction)XPLMSetWindowTitleFun, METH_VARARGS | METH_KEYWORDS, _setWindowTitle__doc__},
  {"XPLMSetWindowTitle", (PyCFunction)XPLMSetWindowTitleFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"getWindowRefCon", (PyCFunction)XPLMGetWindowRefConFun, METH_VARARGS | METH_KEYWORDS, _getWindowRefCon__doc__},
  {"XPLMGetWindowRefCon", (PyCFunction)XPLMGetWindowRefConFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"setWindowRefCon", (PyCFunction)XPLMSetWindowRefConFun, METH_VARARGS | METH_KEYWORDS, _setWindowRefCon__doc__},
  {"XPLMSetWindowRefCon", (PyCFunction)XPLMSetWindowRefConFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"takeKeyboardFocus", (PyCFunction)XPLMTakeKeyboardFocusFun, METH_VARARGS | METH_KEYWORDS, _takeKeyboardFocus__doc__},
  {"XPLMTakeKeyboardFocus", (PyCFunction)XPLMTakeKeyboardFocusFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"hasKeyboardFocus", (PyCFunction)XPLMHasKeyboardFocusFun, METH_VARARGS | METH_KEYWORDS, _hasKeyboardFocus__doc__},
  {"XPLMHasKeyboardFocus", (PyCFunction)XPLMHasKeyboardFocusFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"bringWindowToFront", (PyCFunction)XPLMBringWindowToFrontFun, METH_VARARGS | METH_KEYWORDS, _bringWindowToFront__doc__},
  {"XPLMBringWindowToFront", (PyCFunction)XPLMBringWindowToFrontFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"isWindowInFront", (PyCFunction)XPLMIsWindowInFrontFun, METH_VARARGS | METH_KEYWORDS, _isWindowInFront__doc__},
  {"XPLMIsWindowInFront", (PyCFunction)XPLMIsWindowInFrontFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"registerHotKey", (PyCFunction)XPLMRegisterHotKeyFun, METH_VARARGS | METH_KEYWORDS, _registerHotKey__doc__},
  {"XPLMRegisterHotKey", (PyCFunction)XPLMRegisterHotKeyFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"unregisterHotKey", (PyCFunction)XPLMUnregisterHotKeyFun, METH_VARARGS | METH_KEYWORDS, _unregisterHotKey__doc__},
  {"XPLMUnregisterHotKey", (PyCFunction)XPLMUnregisterHotKeyFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"countHotKeys", (PyCFunction)XPLMCountHotKeysFun, METH_VARARGS, _countHotKeys__doc__},
  {"XPLMCountHotKeys", (PyCFunction)XPLMCountHotKeysFun, METH_VARARGS, ""},
  {"getNthHotKey", (PyCFunction)XPLMGetNthHotKeyFun, METH_VARARGS | METH_KEYWORDS, _getNthHotKey__doc__},
  {"XPLMGetNthHotKey", (PyCFunction)XPLMGetNthHotKeyFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"getHotKeyInfo", (PyCFunction)XPLMGetHotKeyInfoFun, METH_VARARGS | METH_KEYWORDS, _getHotKeyInfo__doc__},
  {"XPLMGetHotKeyInfo", (PyCFunction)XPLMGetHotKeyInfoFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"setHotKeyCombination", (PyCFunction)XPLMSetHotKeyCombinationFun, METH_VARARGS | METH_KEYWORDS, _setHotKeyCombination__doc__},
  {"XPLMSetHotKeyCombination", (PyCFunction)XPLMSetHotKeyCombinationFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"getWindowGeometryVR", (PyCFunction)XPLMGetWindowGeometryVRFun, METH_VARARGS | METH_KEYWORDS, _getWindowGeometryVR__doc__},
  {"XPLMGetWindowGeometryVR", (PyCFunction)XPLMGetWindowGeometryVRFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"setWindowGeometryVR", (PyCFunction)XPLMSetWindowGeometryVRFun, METH_VARARGS | METH_KEYWORDS, _setWindowGeometryVR__doc__},
  {"XPLMSetWindowGeometryVR", (PyCFunction)XPLMSetWindowGeometryVRFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"windowIsInVR", (PyCFunction)XPLMWindowIsInVRFun, METH_VARARGS | METH_KEYWORDS, _windowIsInVR__doc__},
  {"XPLMWindowIsInVR", (PyCFunction)XPLMWindowIsInVRFun, METH_VARARGS | METH_KEYWORDS, ""},

  // SDK 400
  {"registerAvionicsCallbacksEx", (PyCFunction)XPLMRegisterAvionicsCallbacksExFun, METH_VARARGS | METH_KEYWORDS, _registerAvionicsCallbacksEx__doc__},
  {"XPLMRegisterAvionicsCallbacksEx", (PyCFunction)XPLMRegisterAvionicsCallbacksExFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"unregisterAvionicsCallbacks", (PyCFunction)XPLMUnregisterAvionicsCallbacksFun, METH_VARARGS | METH_KEYWORDS, _unregisterAvionicsCallbacks__doc__},
  {"XPLMUnregisterAvionicsCallbacks", (PyCFunction)XPLMUnregisterAvionicsCallbacksFun, METH_VARARGS | METH_KEYWORDS, ""},
  
  {"_cleanup", cleanup, METH_VARARGS, "cleanup"},
  {NULL, NULL, 0, NULL}
};
#pragma GCC diagnostic pop


static struct PyModuleDef XPLMDisplayModule = {
  PyModuleDef_HEAD_INIT,
  "XPLMDisplay",
  "Laminar documentation: \n"
  "   https://developer.x-plane.com/sdk/XPLMDisplay/\n"
  "XPPython3 documentation: \n"
  "   https://xppython3.rtfd.io/en/stable/development/modules/display.html",
  -1,
  XPLMDisplayMethods,
  NULL,
  NULL,
  NULL,
  NULL
};

PyMODINIT_FUNC
PyInit_XPLMDisplay(void)
{
  if(!(avionicsCallbacksDict = PyDict_New())){return NULL;}
  PyDict_SetItemString(XPY3pythonDicts, "avionicsCallbacks", avionicsCallbacksDict);

  if(!(avionicsCallbacksIDDict = PyDict_New())){return NULL;}
  PyDict_SetItemString(XPY3pythonDicts, "avionicsIDs", avionicsCallbacksIDDict);

  if(!(drawCallbackDict = PyDict_New())){return NULL;}
  PyDict_SetItemString(XPY3pythonDicts, "drawCallbacks", drawCallbackDict);

  if(!(drawCallbackIDDict = PyDict_New())){return NULL;}
  PyDict_SetItemString(XPY3pythonDicts, "drawCallbackIDs", drawCallbackIDDict);

  if(!(keySniffCallbackDict = PyDict_New())){return NULL;}
  PyDict_SetItemString(XPY3pythonDicts, "keySniffCallbacks", keySniffCallbackDict);

  if(!(windowDict = PyDict_New())){return NULL;}
  PyDict_SetItemString(XPY3pythonDicts, "windows", windowDict);

  if(!(hotkeyDict = PyDict_New())){return NULL;}
  PyDict_SetItemString(XPY3pythonDicts, "hotkeys", hotkeyDict);

  if(!(hotkeyIDDict = PyDict_New())){return NULL;}
  PyDict_SetItemString(XPY3pythonDicts, "hotkeyIDs", hotkeyIDDict);

  if(!(windowIDCapsules = PyDict_New())){return NULL;}
  PyDict_SetItemString(XPY3pythonCapsules, windowIDRef, windowIDCapsules);

  if(!(hotkeyIDCapsules = PyDict_New())){return NULL;}
  PyDict_SetItemString(XPY3pythonCapsules, hotkeyIDRef, hotkeyIDCapsules);

  if(!(avionicsIDCapsules = PyDict_New())){return NULL;}
  PyDict_SetItemString(XPY3pythonCapsules, avionicsIDRef, avionicsIDCapsules);

  PyObject *mod = PyModule_Create(&XPLMDisplayModule);
  if(mod){
    PyModule_AddStringConstant(mod, "__author__", "Peter Buckner (pbuck@avnwx.com)");
#if defined(XPLM_DEPRECATED)
    PyModule_AddIntConstant(mod, "xplm_Phase_FirstScene", xplm_Phase_FirstScene);
    PyModule_AddIntConstant(mod, "xplm_Phase_Terrain", xplm_Phase_Terrain);
    PyModule_AddIntConstant(mod, "xplm_Phase_Airports", xplm_Phase_Airports);
    PyModule_AddIntConstant(mod, "xplm_Phase_Vectors", xplm_Phase_Vectors);
    PyModule_AddIntConstant(mod, "xplm_Phase_Objects", xplm_Phase_Objects);
    PyModule_AddIntConstant(mod, "xplm_Phase_Airplanes", xplm_Phase_Airplanes);
    PyModule_AddIntConstant(mod, "xplm_Phase_LastScene", xplm_Phase_LastScene);
#endif /* XPLM_DEPRECATED */
#if defined(XPLM302)
    PyModule_AddIntConstant(mod, "xplm_Phase_Modern3D", xplm_Phase_Modern3D);
    PyModule_AddIntConstant(mod, "Phase_Modern3D", xplm_Phase_Modern3D);
#endif
    PyModule_AddIntConstant(mod, "xplm_Phase_FirstCockpit", xplm_Phase_FirstCockpit);
    PyModule_AddIntConstant(mod, "xplm_Phase_Panel", xplm_Phase_Panel);
    PyModule_AddIntConstant(mod, "xplm_Phase_Gauges", xplm_Phase_Gauges);
    PyModule_AddIntConstant(mod, "xplm_Phase_Window", xplm_Phase_Window);
    PyModule_AddIntConstant(mod, "xplm_Phase_LastCockpit", xplm_Phase_LastCockpit);

    PyModule_AddIntConstant(mod, "Phase_FirstCockpit", xplm_Phase_FirstCockpit);
    PyModule_AddIntConstant(mod, "Phase_Panel", xplm_Phase_Panel);
    PyModule_AddIntConstant(mod, "Phase_Gauges", xplm_Phase_Gauges);
    PyModule_AddIntConstant(mod, "Phase_Window", xplm_Phase_Window);
    PyModule_AddIntConstant(mod, "Phase_LastCockpit", xplm_Phase_LastCockpit);
#if defined(XPLM200)
    PyModule_AddIntConstant(mod, "xplm_Phase_LocalMap3D", xplm_Phase_LocalMap3D);
    PyModule_AddIntConstant(mod, "Phase_LocalMap3D", xplm_Phase_LocalMap3D);
#endif
#if defined(XPLM200)
    PyModule_AddIntConstant(mod, "xplm_Phase_LocalMap2D", xplm_Phase_LocalMap2D);
    PyModule_AddIntConstant(mod, "Phase_LocalMap2D", xplm_Phase_LocalMap2D);
#endif
#if defined(XPLM200)
    PyModule_AddIntConstant(mod, "xplm_Phase_LocalMapProfile", xplm_Phase_LocalMapProfile);
    PyModule_AddIntConstant(mod, "Phase_LocalMapProfile", xplm_Phase_LocalMapProfile);
#endif

    PyModule_AddIntConstant(mod, "xplm_MouseDown", xplm_MouseDown);
    PyModule_AddIntConstant(mod, "xplm_MouseDrag", xplm_MouseDrag);
    PyModule_AddIntConstant(mod, "xplm_MouseUp",   xplm_MouseUp);

    PyModule_AddIntConstant(mod, "MouseDown", xplm_MouseDown);
    PyModule_AddIntConstant(mod, "MouseDrag", xplm_MouseDrag);
    PyModule_AddIntConstant(mod, "MouseUp",   xplm_MouseUp);

    PyModule_AddIntConstant(mod, "xplm_CursorDefault", xplm_CursorDefault);
    PyModule_AddIntConstant(mod, "xplm_CursorHidden",  xplm_CursorHidden);
    PyModule_AddIntConstant(mod, "xplm_CursorArrow",   xplm_CursorArrow);
    PyModule_AddIntConstant(mod, "xplm_CursorCustom",  xplm_CursorCustom);

    PyModule_AddIntConstant(mod, "CursorDefault", xplm_CursorDefault);
    PyModule_AddIntConstant(mod, "CursorHidden",  xplm_CursorHidden);
    PyModule_AddIntConstant(mod, "CursorArrow",   xplm_CursorArrow);
    PyModule_AddIntConstant(mod, "CursorCustom",  xplm_CursorCustom);

    PyModule_AddIntConstant(mod, "xplm_WindowLayerFlightOverlay", xplm_WindowLayerFlightOverlay);
    PyModule_AddIntConstant(mod, "xplm_WindowLayerFloatingWindows", xplm_WindowLayerFloatingWindows);
    PyModule_AddIntConstant(mod, "xplm_WindowLayerModal", xplm_WindowLayerModal);
    PyModule_AddIntConstant(mod, "xplm_WindowLayerGrowlNotifications", xplm_WindowLayerGrowlNotifications);
    
    PyModule_AddIntConstant(mod, "WindowLayerFlightOverlay", xplm_WindowLayerFlightOverlay);
    PyModule_AddIntConstant(mod, "WindowLayerFloatingWindows", xplm_WindowLayerFloatingWindows);
    PyModule_AddIntConstant(mod, "WindowLayerModal", xplm_WindowLayerModal);
    PyModule_AddIntConstant(mod, "WindowLayerGrowlNotifications", xplm_WindowLayerGrowlNotifications);
    
    PyModule_AddIntConstant(mod, "xplm_WindowPositionFree", xplm_WindowPositionFree);
    PyModule_AddIntConstant(mod, "xplm_WindowCenterOnMonitor", xplm_WindowCenterOnMonitor);
    PyModule_AddIntConstant(mod, "xplm_WindowFullScreenOnMonitor", xplm_WindowFullScreenOnMonitor);
    PyModule_AddIntConstant(mod, "xplm_WindowFullScreenOnAllMonitors", xplm_WindowFullScreenOnAllMonitors);
    PyModule_AddIntConstant(mod, "xplm_WindowPopOut", xplm_WindowPopOut);
    PyModule_AddIntConstant(mod, "xplm_WindowVR", xplm_WindowVR);

    PyModule_AddIntConstant(mod, "WindowPositionFree", xplm_WindowPositionFree);
    PyModule_AddIntConstant(mod, "WindowCenterOnMonitor", xplm_WindowCenterOnMonitor);
    PyModule_AddIntConstant(mod, "WindowFullScreenOnMonitor", xplm_WindowFullScreenOnMonitor);
    PyModule_AddIntConstant(mod, "WindowFullScreenOnAllMonitors", xplm_WindowFullScreenOnAllMonitors);
    PyModule_AddIntConstant(mod, "WindowPopOut", xplm_WindowPopOut);
    PyModule_AddIntConstant(mod, "WindowVR", xplm_WindowVR);

    PyModule_AddIntConstant(mod, "xplm_WindowDecorationNone", xplm_WindowDecorationNone);
    PyModule_AddIntConstant(mod, "xplm_WindowDecorationRoundRectangle", xplm_WindowDecorationRoundRectangle);
    PyModule_AddIntConstant(mod, "xplm_WindowDecorationSelfDecorated", xplm_WindowDecorationSelfDecorated);
    PyModule_AddIntConstant(mod, "xplm_WindowDecorationSelfDecoratedResizable", xplm_WindowDecorationSelfDecoratedResizable);

    PyModule_AddIntConstant(mod, "WindowDecorationNone", xplm_WindowDecorationNone);
    PyModule_AddIntConstant(mod, "WindowDecorationRoundRectangle", xplm_WindowDecorationRoundRectangle);
    PyModule_AddIntConstant(mod, "WindowDecorationSelfDecorated", xplm_WindowDecorationSelfDecorated);
    PyModule_AddIntConstant(mod, "WindowDecorationSelfDecoratedResizable", xplm_WindowDecorationSelfDecoratedResizable);

#if defined(XPLM400)    
    PyModule_AddIntConstant(mod, "Device_GNS430_1", xplm_device_GNS430_1);
    PyModule_AddIntConstant(mod, "Device_GNS430_2", xplm_device_GNS430_2);
    PyModule_AddIntConstant(mod, "Device_GNS530_1", xplm_device_GNS530_1);
    PyModule_AddIntConstant(mod, "Device_GNS530_2", xplm_device_GNS530_2);
    PyModule_AddIntConstant(mod, "Device_CDU739_1", xplm_device_CDU739_1);
    PyModule_AddIntConstant(mod, "Device_CDU739_2", xplm_device_CDU739_2);
    PyModule_AddIntConstant(mod, "Device_G1000_PFD_1", xplm_device_G1000_PFD_1);
    PyModule_AddIntConstant(mod, "Device_G1000_PFD_2", xplm_device_G1000_PFD_2);
    PyModule_AddIntConstant(mod, "Device_G1000_MFD", xplm_device_G1000_MFD);
    PyModule_AddIntConstant(mod, "Device_CDU815_1", xplm_device_CDU815_1);
    PyModule_AddIntConstant(mod, "Device_CDU815_2", xplm_device_CDU815_2);
    PyModule_AddIntConstant(mod, "Device_Primus_PFD_1", xplm_device_Primus_PFD_1);
    PyModule_AddIntConstant(mod, "Device_Primus_PFD_2", xplm_device_Primus_PFD_2);
    PyModule_AddIntConstant(mod, "Device_Primus_MFD_1", xplm_device_Primus_MFD_1);
    PyModule_AddIntConstant(mod, "Device_Primus_MFD_2", xplm_device_Primus_MFD_2);
    PyModule_AddIntConstant(mod, "Device_Primus_MFD_3", xplm_device_Primus_MFD_3);
    PyModule_AddIntConstant(mod, "Device_Primus_RMU_1", xplm_device_Primus_RMU_1);
    PyModule_AddIntConstant(mod, "Device_Primus_RMU_2", xplm_device_Primus_RMU_2);
#else
    PyModule_AddIntConstant(mod, "Device_GNS430_1", -1);
    PyModule_AddIntConstant(mod, "Device_GNS430_2", -1);
    PyModule_AddIntConstant(mod, "Device_GNS530_1", -1);
    PyModule_AddIntConstant(mod, "Device_GNS530_2", -1);
    PyModule_AddIntConstant(mod, "Device_CDU739_1", -1);
    PyModule_AddIntConstant(mod, "Device_CDU739_2", -1);
    PyModule_AddIntConstant(mod, "Device_G1000_PFD_1", -1);
    PyModule_AddIntConstant(mod, "Device_G1000_PFD_2", -1);
    PyModule_AddIntConstant(mod, "Device_G1000_MFD", -1);
    PyModule_AddIntConstant(mod, "Device_CDU815_1", -1);
    PyModule_AddIntConstant(mod, "Device_CDU815_2", -1);
    PyModule_AddIntConstant(mod, "Device_Primus_PFD_1", -1);
    PyModule_AddIntConstant(mod, "Device_Primus_PFD_2", -1);
    PyModule_AddIntConstant(mod, "Device_Primus_MFD_1", -1);
    PyModule_AddIntConstant(mod, "Device_Primus_MFD_2", -1);
    PyModule_AddIntConstant(mod, "Device_Primus_MFD_3", -1);
    PyModule_AddIntConstant(mod, "Device_Primus_RMU_1", -1);
    PyModule_AddIntConstant(mod, "Device_Primus_RMU_2", -1);
#endif
  }

  return mod;
}

int genericAvionicsCallback(XPLMDeviceID inDeviceID, int inIsBefore, void *inRefcon)
{
  struct timespec all_stop, all_start;
  clock_gettime(CLOCK_MONOTONIC, &all_start);
  (void) inDeviceID;
  PyObject *pl = PyLong_FromVoidPtr(inRefcon);/*new*/
  PyObject *err = NULL;
  PyObject *pRes = NULL;
  int res = 0;
  if (pl == NULL) {
    pythonLog("avionicsDrawCallback, can't create PyLong.\n");
    goto cleanup;
  }

  PyObject *tup = PyDict_GetItem(avionicsCallbacksDict, pl); /*borrowed*/
  Py_DECREF(pl);
  if (!tup) {
    pythonLog("avionicsDrawCallback, got unknown inRefcon(%p)!\n", inRefcon);
    goto cleanup;
  }

  PyObject *fun = PyTuple_GetItem(tup, inIsBefore ? AVIONICS_BEFORE : AVIONICS_AFTER); /* borrowed */
  PyObject *refCon = PyTuple_GetItem(tup, AVIONICS_REFCON);/* borrowed */
  PyObject *deviceId = PyTuple_GetItem(tup, AVIONICS_DEVICE);/* borrowed -- and should match inDeviceID, which we ignore */
  set_moduleName(PyTuple_GetItem(tup, AVIONICS_MODULE_NAME));
  PyObject *isBefore = PyLong_FromLong(inIsBefore);/* new */
  struct timespec stop, start;
  clock_gettime(CLOCK_MONOTONIC, &start);
  pRes = PyObject_CallFunctionObjArgs(fun, deviceId, isBefore, refCon, NULL);
  clock_gettime(CLOCK_MONOTONIC, &stop);
  pluginStats[getPluginIndex(PyTuple_GetItem(tup, AVIONICS_MODULE_NAME))].draw_time += (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_nsec - start.tv_nsec) / 1000;
  Py_DECREF(isBefore);

  if(!pRes){
    char *s2 = objToStr(fun);
    pythonLog("[%s] Avionics Draw callback %s failed.\n", CurrentPythonModuleName, s2);
    free(s2);
    goto cleanup;
  }
  if (inIsBefore) {
    if(!PyLong_Check(pRes)){
      /* _before_ callbacks should return 1 or 0  -- _after_ callback returns are ignored */
      char *s2 = objToStr(fun);
      pythonLog("[%s] Avionics Draw callback %s returned a wrong type.\n", CurrentPythonModuleName, s2);
      free(s2);
      goto cleanup;
    }
    res = (int)PyLong_AsLong(pRes);
  }
                                   
 cleanup:
  err = PyErr_Occurred();
  if(err){
    pythonLogException();
  }
  Py_XDECREF(pRes);

  clock_gettime(CLOCK_MONOTONIC, &all_stop);
  pluginStats[0].draw_time += (all_stop.tv_sec - all_start.tv_sec) * 1000000 + (all_stop.tv_nsec - all_start.tv_nsec) / 1000;

  return res;
}

static int genericDrawCallback(XPLMDrawingPhase inPhase, int inIsBefore, void *inRefcon)
{
  struct timespec all_stop, all_start;
  clock_gettime(CLOCK_MONOTONIC, &all_start);

  PyObject *pl = NULL, *fun = NULL, *refcon = NULL, *pRes = NULL, *err = NULL;
  PyObject *tup;
  int res = 1;
  pl = PyLong_FromVoidPtr(inRefcon);
  if(pl == NULL){
    pythonLog("drawCallback, can't create PyLong.\n");
    goto cleanup;
  }else{
    tup = PyDict_GetItem(drawCallbackDict, pl);
    Py_DECREF(pl);
  }

  if(!tup){
    pythonLog("drawCallback, got unknown inRefcon (%p)!\n", inRefcon);
    goto cleanup;
  }
  fun =    PyTuple_GetItem(tup, DRAW_CALLBACK);
  refcon = PyTuple_GetItem(tup, DRAW_REFCON);
  set_moduleName(PyTuple_GetItem(tup, DRAW_MODULE_NAME));
  PyObject *inPhaseObj = PyLong_FromLong(inPhase);
  PyObject *inIsBeforeObj = PyLong_FromLong(inIsBefore);

  struct timespec stop, start;
  clock_gettime(CLOCK_MONOTONIC, &start);
  pRes = PyObject_CallFunctionObjArgs(fun, inPhaseObj, inIsBeforeObj, refcon, NULL);
  clock_gettime(CLOCK_MONOTONIC, &stop);
  pluginStats[getPluginIndex(PyTuple_GetItem(tup, DRAW_MODULE_NAME))].draw_time += (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_nsec - start.tv_nsec) / 1000;

  Py_DECREF(inPhaseObj);
  Py_DECREF(inIsBeforeObj);
  if(!pRes){
    char *s = objToStr(PyTuple_GetItem(tup, DRAW_MODULE_NAME));
    char *s2 = objToStr(fun);
    pythonLog("[%s] Draw callback %s failed.\n", s, s2);
    free(s);
    free(s2);
    goto cleanup;
  }
  if(!PyLong_Check(pRes)){
    if (inIsBefore) {
      char *s = objToStr(PyTuple_GetItem(tup, DRAW_MODULE_NAME));
      char *s2 = objToStr(fun);
      pythonLog("[%s] Draw callback %s returned a wrong type.\n", s, s2);
      free(s);
      free(s2);
      goto cleanup;
    }
    res = 0; /* if not inIsBefore, X-Plane  ignores return value & so do we */
  } else {
    res = (int)PyLong_AsLong(pRes);
  }

 cleanup:
  err = PyErr_Occurred();
  if(err){
    pythonLogException();
  }

  Py_XDECREF(pRes);
  clock_gettime(CLOCK_MONOTONIC, &all_stop);
  pluginStats[0].draw_time += (all_stop.tv_sec - all_start.tv_sec) * 1000000 + (all_stop.tv_nsec - all_start.tv_nsec) / 1000;

  return res;
}

int genericKeySnifferCallback(char inChar, XPLMKeyFlags inFlags, char inVirtualKey, void *inRefcon)
{
  PyObject *pl = NULL, *fun = NULL, *refcon = NULL, *pRes = NULL, *err = NULL;
  PyObject *tup;
  int res = 1;

  pl = PyLong_FromVoidPtr(inRefcon);
  if(pl == NULL){
    pythonLog("keySnifferCallback, can't create PyLong.\n");
    goto cleanup;
  }else{
    tup = PyDict_GetItem(keySniffCallbackDict, pl);
    Py_DECREF(pl);
  }

  if(!tup){
    pythonLog("keySninfferCallback, got unknown inRefcon (%p)!\n", inRefcon);
    goto cleanup;
  }
  fun =    PyTuple_GetItem(tup, KEYSNIFF_CALLBACK);
  PyObject *inCharObj = PyLong_FromLong(inChar);
  PyObject *inFlagsObj = PyLong_FromLong(inFlags);
  PyObject *inVirtualKeyObj = PyLong_FromLong((unsigned int)inVirtualKey);
  refcon = PyTuple_GetItem(tup, KEYSNIFF_REFCON);
  set_moduleName(PyTuple_GetItem(tup, KEYSNIFF_MODULE_NAME));
  pRes = PyObject_CallFunctionObjArgs(fun, inCharObj, inFlagsObj, inVirtualKeyObj, refcon, NULL);
  Py_DECREF(inCharObj);
  Py_DECREF(inFlagsObj);
  Py_DECREF(inVirtualKeyObj);
  if(!pRes){
    char *s2 = objToStr(fun);
    pythonLog("[%s] Key sniffer callback %s failed.\n", CurrentPythonModuleName, s2);
    free(s2);
    goto cleanup;
  }
  if(!PyLong_Check(pRes)){
    char *s2 = objToStr(fun);
    pythonLog("[%s] Key sniffer callback %s returned a wrong type.\n", CurrentPythonModuleName, s2);
    free(s2);
    goto cleanup;
  }
  res = (int)PyLong_AsLong(pRes);

 cleanup:
  err = PyErr_Occurred();
  if(err){
    pythonLogException();
  }

  Py_XDECREF(pRes);
  return res;
}
