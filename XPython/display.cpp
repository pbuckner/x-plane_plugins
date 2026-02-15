#define _GNU_SOURCE 1
#include <Python.h>
#include <sys/time.h>
#include <stdio.h>
#include <unordered_map>
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMDisplay.h>
#include "utils.h"
#include "plugin_dl.h"
#include "display.h"
#include "display_keysniffer.h"
#include "display_hotkey.h"
#include "display_avionics.h"
#include "xppythontypes.h"
#include "xppython.h"
#include "capsules.h"

static intptr_t drawCallbackCntr;
/* the 'key' to drawCallbackDict, we'll use as the refcon for genericDrawCallback function.
   When generic function is _called_ by X-Plane, we:
   1) take provided refcon
   2) lookup info in drawCallbackDict to get "real" callback & refcon
   Registered as
     XPLMRegisterDrawCallback(generic, phase, before, <drawCallbackCntr>)

   For unregister, we need to unregister the generic & pass in the value for drawCallbackCntr we sent
   to X-Plane for registration. We don't have that value, but we are provided same info as with registration,
   so we go through drawCallbackDict, [(key, value), (key, value)], comparing value with tuple of provided
   info and on match, use the <key> as the drawCallbackCntr

   Unregistered as:
      XPLMUnregisterDrawCallback(generic, phase, before, <drawCallbackCntr>)
*/
struct DrawCallbackInfo {
  const char* module_name;
  PyObject *callback;
  XPLMDrawingPhase phase;
  int before;
  PyObject *refCon;
};

static std::unordered_map<intptr_t, DrawCallbackInfo> drawCallbackDict;
static int genericDrawCallback(XPLMDrawingPhase inPhase, int inIsBefore, void *inRefcon);


//draw, key,mouse, cursor, wheel
struct WindowCallbackInfo {
  PyObject *draw;
  PyObject *click;
  PyObject *key;
  PyObject *cursor;
  PyObject *wheel;
  PyObject *rightClick;
  PyObject *refCon;
  const char* module_name;
};

static std::unordered_map<XPLMWindowID, WindowCallbackInfo> windowDict;
static void genericWindowDraw(XPLMWindowID inWindowID, void *inRefcon);
static int genericWindowMouseClick(XPLMWindowID inWindowID, int x, int y, XPLMMouseStatus inMouse, void *inRefcon);
static void genericWindowKey(XPLMWindowID, char inKey, XPLMKeyFlags inFlags, char inVirtualKey, void *inRefcon, int losingFocus);
static XPLMCursorStatus genericWindowCursor(XPLMWindowID inWindowID, int x, int y, void *inRefcon);
static int genericWindowMouseWheel(XPLMWindowID  inWindowID, int x, int y, int wheel, int clicks, void *inRefcon);
static int genericWindowRightClick(XPLMWindowID inWindowID, int x, int y, XPLMMouseStatus inMouse, void *inRefcon);

static PyObject *monitorBndsCallback;

void resetDrawCallbacks(void) {
  for (const auto& pair : drawCallbackDict) {
    char *callback = objToStr(pair.second.callback);
    pythonDebug("     Reset --     %s - (%s)", pair.second.module_name, callback);
    free(callback);

    XPLMUnregisterDrawCallback(genericDrawCallback,
                               pair.second.phase, pair.second.before,
                               (void*)pair.first);
    Py_DECREF(pair.second.callback);
    Py_DECREF(pair.second.refCon);
  }

  drawCallbackDict.clear();
}

void resetWindows(void) {
  for (const auto& pair : windowDict) {
    pythonDebug("     Reset --     (%s)", pair.second.module_name);
    XPLMDestroyWindow((XPLMWindowID)pair.first);
    Py_DECREF(pair.second.draw);
    Py_DECREF(pair.second.click);
    Py_DECREF(pair.second.key);
    Py_DECREF(pair.second.cursor);
    Py_DECREF(pair.second.wheel);
    Py_DECREF(pair.second.rightClick);
    Py_DECREF(pair.second.refCon);
  }
  windowDict.clear();
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

My_DOCSTR(_registerDrawCallback__doc__, "registerDrawCallback",
          "draw, phase=Phase_Window, after=0, refCon=None",
          "draw:Callable[[XPLMDrawingPhase, int, Any], None | int], phase:XPLMDrawingPhase=Phase_Window, after:int=0, refCon:Any=None",
          "int",
          "Registers  low-level drawing callback.\n"
          "\n"
          "The after parameter indicates you want to be called before (0) or after (1) phase.\n"
          "draw() callback function takes three parameters (phase, after, refCon), returning \n"
          "0 to suppress further X-Plane drawing in the phase, or 1 to allow X-Plane to finish.\n"
          "(Callback's value is ignored if after=1).\n"
          "\nRegistration returns 1 on success, 0 otherwise.");
static PyObject *XPLMRegisterDrawCallbackFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("draw"), CHAR("phase"), CHAR("after"), CHAR("refCon"), nullptr};

  errCheck("Before registerDrawCallback");
  (void) self;
  PyObject *callback;
  int inPhase = xplm_Phase_Window;
  int inWantsBefore = 0;
  PyObject *refcon = Py_None;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|iiO", keywords, &callback, &inPhase, &inWantsBefore, &refcon)){
    return nullptr;
  }
  if (!PyCallable_Check(callback)) {
    PyErr_SetString(PyExc_TypeError ,"XPLMRegisterDrawCallback callback is not Callable.\n");
    return nullptr;
  }

  intptr_t idx = ++drawCallbackCntr;

  Py_INCREF(callback);
  Py_INCREF(refcon);
  drawCallbackDict[idx] = {
    .module_name = CurrentPythonModuleName,
    .callback = callback,
    .phase = inPhase,
    .before = inWantsBefore,
    .refCon = refcon
  };

  int res = XPLMRegisterDrawCallback(genericDrawCallback, inPhase, inWantsBefore, (void *)drawCallbackCntr);
  if(!res){
    PyErr_SetString(PyExc_RuntimeError ,"XPLMRegisterDrawCallback failed.\n");
    return nullptr;
  }
  errCheck("end of registerDrawCallback");
  return PyLong_FromLong(res);
}


My_DOCSTR(_registerAvionicsCallbacksEx__doc__, "registerAvionicsCallbacksEx",
          "deviceId, before=None, after=None, refCon=None, bezelClick=None, bezelRightClick=None, "
          "bezelScroll=None, bezelCursor=None, screenTouch=None, screenRightTouch=None, screenScroll=None, "
          "screenCursor=None, keyboard=None",
          "deviceId:XPLMDeviceID, "
          "before:Optional[Callable[[XPLMDeviceID, int, Any], int]]=None, "
          "after:Optional[Callable[[XPLMDeviceID, int, Any], int]]=None, "
          "refCon:Any=None,"
          "bezelClick:Optional[Callable[[int, int, XPLMMouseStatus, Any], int]]=None, "
          "bezelRightClick:Optional[Callable[[int, int, XPLMMouseStatus, Any], int]]=None, "
          "bezelScroll:Optional[Callable[[int, int, int, int, Any], int]]=None, "
          "bezelCursor:Optional[Callable[[int, int, Any], XPLMCursorStatus]]=None, "
          "screenTouch:Optional[Callable[[int, int, XPLMMouseStatus, Any], int]]=None, "
          "screenRightTouch:Optional[Callable[[int, int, XPLMMouseStatus, Any], int]]=None, "
          "screenScroll:Optional[Callable[[int, int, int, int, Any], int]]=None, "
          "screenCursor:Optional[Callable[[int, int, Any], XPLMCursorStatus]]=None, "
          "keyboard:Optional[Callable[[int, XPLMKeyFlags, int, Any, int], int]]=None",
          "XPLMAvionicsID",
          "Registers draw callback for particular device.\n"
          "\n"
          "Registers drawing callback(s) to enhance or replace X-Plane drawing. For\n"
          "'before' callback, return 1 to let X-Plane draw or 0 to suppress X-Plane\n"
          "drawing. Return value for 'after' callback is ignored.\n"
          "\n"
          "Upon entry, OpenGL context will be correctly set in panel coordinates for 2d drawing.\n"
          "OpenGL state (texturing, etc.) will be unknown.\n"
          "\n"
          "Successful registration returns an AvionicsID."
         );

My_DOCSTR(_unregisterAvionicsCallbacks__doc__, "unregisterAvionicsCallbacks",
          "avionicsId",
          "avionicsId:XPLMAvionicsID",
          "None",
          "Unregisters avionics draw callback(s) associated with given avionicsId.\n"
          "\n"
          "Does not return a value."
         );
          


My_DOCSTR(_getAvionicsHandle__doc__, "getAvionicsHandle",
          "deviceID",
          "deviceID:int",
          "XPLMAvionicsID",
          "Registers no callbacks for a cockpit device, but returns a\n"
          "handle which allows you to interact using other Avionics Device\n"
          "API. Use this if you do not wish to intercept drawing, clicks, or\n"
          "touches but want to interact with its popup programmatically.\n"
          "\n"
          "Returns XPLMAvionicsID."
          );

My_DOCSTR(_isAvionicsBound__doc__, "isAvionicsBound",
          "avionicsID",
          "avionicsID:XPLMAvionicsID",
          "int",
          "Return 1 if cockpit device with given ID is used by the current aircraft.");

My_DOCSTR(_isCursorOverAvionics__doc__, "isCursorOverAvionics",
          "avionicsID",
          "avionicsID:XPLMAvionicsID",
          "tuple[int, int] | None",
          "Is the cursor over the device with given avionicsID\n"
          "\n"
          "Returns tuple (x, y) with position or None.");


My_DOCSTR(_isAvionicsPopupVisible__doc__, "isAvionicsPopupVisible",
          "avionicsID",
          "avionicsID:XPLMAvionicsID",
          "int",
          "Is the popup window for the device with given avionicsID visible?\n"
          "(It may or may not be popped out into an OS window.)\n"
          "\n"
          "Returns 1 if true.");

My_DOCSTR(_isAvionicsPoppedOut__doc__, "isAvionicsPoppedOut",
          "avionicsID",
          "avionicsID:XPLMAvionicsID",
          "int",
          "Returns 1 (true) if the popup window for the cockpit device is popped out\n"
          "into an OS window.");


My_DOCSTR(_hasAvionicsKeyboardFocus__doc__, "hasAvionicsKeyboardFocus",
          "avionicsID",
          "avionicsID:XPLMAvionicsID",
          "int",
          "Returns 1 (true) if cockpit device has keyboard focus.");


My_DOCSTR(_avionicsNeedsDrawing__doc__, "avionicsNeedsDrawing",
          "avionicsID",
          "avionicsID:XPLMAvionicsID",
          "None",
          "Tells X-Plane that your device's screens needs to be re-drawn.\n"
          "If your device is marked for on-demand drawing, XP will call your screen\n"
          "drawing callback before drawing the next simulator frame. If your device\n"
          "is already drawn every frame, this has no effect.");


My_DOCSTR(_popOutAvionics__doc__, "popOutAvionics",
          "avionicsID",
          "avionicsID:XPLMAvionicsID",
          "None",
          "Pops out OS window for cockpit device.");

My_DOCSTR(_takeAvionicsKeyboardFocus__doc__, "takeAvionicsKeyboardFocus",
          "avionicsID",
          "avionicsID:XPLMAvionicsID",
          "None",
          "Sets keyboard focus to the (already) visible popup window of cockpit device.\n"
          "Does nothing if device is not visible.");

My_DOCSTR(_destroyAvionics__doc__, "destroyAvionics",
          "avionicsID",
          "avionicsID:XPLMAvionicsID",
          "None",
          "Destroys the cockpit device and deallocates its framebuffer. You should\n"
          "only ever call this for devices that you created, not stock X-Plane devices\n"
          "you have customized.");

My_DOCSTR(_getAvionicsBusVoltsRatio__doc__, "getAvionicsBusVoltsRatio",
          "avionicsID",
          "avionicsID:XPLMAvionicsID",
          "float",
          "Return ratio [0.0:1.0] of nominal voltage of electrical bus,\n"
          "for given avionics device. Returns -1 if device is not bound\n"
          "to the current aircraft.");

My_DOCSTR(_getAvionicsBrightnessRheo__doc__, "getAvionicsBrightnessRheo",
          "avionicsID",
          "avionicsID:XPLMAvionicsID",
          "float",
          "Returns brightness setting between 0 and 1 for the screen of\n"
          "this cockpit device.\n"
          "If the device is bound to current aircraft, this is equivalent\n"
          "to 'sim/cockpit2/switches/instrument_brightness_ratio[]' dataref\n"
          "with the correct array slot for the bound device.\n"
          "If the device is not bound, it returns brightness ratio for the\n"
          "device alone.");

My_DOCSTR(_setAvionicsBrightnessRheo__doc__, "setAvionicsBrightnessRheo",
          "avionicsID, brightness=1.0",
          "avionicsID:XPLMAvionicsID, brightness:float=1.0",
          "None",
          "Sets brightness setting between 0 and 1 for the screen of\n"
          "this cockpit device.\n\n"
          "If the device is bound to current aircraft, this is equivalent\n"
          "to 'sim/cockpit2/switches/instrument_brightness_ratio[]' dataref\n"
          "with the correct array slot for the bound device.\n\n"
          "If the device is not bound, it sets brightness rheostat for the\n"
          "device alone, even though not connected to the dataref.");

My_DOCSTR(_setAvionicsPopupVisible__doc__, "setAvionicsPopupVisible",
          "avionicsID, visible=1",
          "avionicsID:XPLMAvionicsID, visible:int=1",
          "None",
          "Shows (visible=1) or Hides popup window for cockpit device.");


My_DOCSTR(_unregisterDrawCallback__doc__, "unregisterDrawCallback",
          "draw, phase=Phase_Window, after=0, refCon=None",
          "draw:Callable[[XPLMDrawingPhase, int, Any], None], phase:XPLMDrawingPhase=Phase_Window, after:int=0, refCon:Any=None",
          "int",
          "Unregisters low-level drawing callback.\n"
          "\n"
          "Parameters must match those provided  with registerDrawCallback().\n"
          "Returns 1 on success, 0 otherwise.");
static PyObject *XPLMUnregisterDrawCallbackFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("draw"), CHAR("phase"), CHAR("after"), CHAR("refCon"), nullptr};
  (void) self;
  PyObject *callback;
  int inPhase = xplm_Phase_Window;
  int inWantsBefore = 0;
  PyObject *refcon = Py_None;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|iiO", keywords, &callback, &inPhase, &inWantsBefore, &refcon)){
    return nullptr;
  }
  int res = 0;
  intptr_t foundKey = 0;

  for (auto it = drawCallbackDict.begin(); it != drawCallbackDict.end(); ) {
    DrawCallbackInfo &info = it->second;
    if (info.phase == inPhase
        && info.before == inWantsBefore
        && 0 == strcmp(info.module_name, CurrentPythonModuleName)
        && PyObject_RichCompareBool(info.callback, callback, Py_EQ)
        && PyObject_RichCompareBool(info.refCon, refcon, Py_EQ)) {
      foundKey = it->first;
      res = XPLMUnregisterDrawCallback(genericDrawCallback, inPhase, inWantsBefore, (void*)it->first);
      Py_DECREF(info.callback);
      Py_DECREF(info.refCon);
      it = drawCallbackDict.erase(it);
      break;
    } else {
      ++it;
    }
  }

  if(foundKey == 0) {
    char *s = objToStr(callback);
    pythonLog("Failed to find matching drawCallback entry for %s %s", CurrentPythonModuleName, s);
    free(s);
  }
  errCheck("after XPLMUnregisterDrawCallback");
  return PyLong_FromLong(res);
}

static void genericWindowDraw(XPLMWindowID  inWindowID,
                void         *inRefcon)
{
  errCheck("prior genericWindowDraw");
  struct timespec all_stop, all_start;
  clock_gettime(CLOCK_MONOTONIC, &all_start);
  auto it = windowDict.find(inWindowID);
  if(it == windowDict.end()){
    pythonLog("Unknown window passed to genericWindowDraw (%p).", inWindowID);
    return;
  }
  PyObject *func = it->second.draw;
  if (func != Py_None) {
    struct timespec stop, start;
    clock_gettime(CLOCK_MONOTONIC, &start);
    PyObject *pID = makeCapsule(inWindowID, "XPLMWindowID");
    set_moduleName(it->second.module_name);

    PyObject *args[] = {pID, (PyObject*)inRefcon};
    PyObject *oRes = PyObject_Vectorcall(func, args, 2, nullptr);
    Py_DECREF(pID);
    clock_gettime(CLOCK_MONOTONIC, &stop);
    pluginStats[getPluginIndex()].draw_time += (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_nsec - start.tv_nsec) / 1000;

    if(PyErr_Occurred()) {
      pythonLogException();
      pythonLog("disabling draw function");
      it->second.draw = Py_None;
    }
    Py_XDECREF(oRes);
  }
  errCheck("end genericWindowDraw");
  clock_gettime(CLOCK_MONOTONIC, &all_stop);
  pluginStats[0].draw_time += (all_stop.tv_sec - all_start.tv_sec) * 1000000 + (all_stop.tv_nsec - all_start.tv_nsec) / 1000;
}

static void genericWindowKey(XPLMWindowID  inWindowID,
               char          inKey,
               XPLMKeyFlags  inFlags,
               char          inVirtualKey,
               void         *inRefcon,
               int           losingFocus)
{
  errCheck("prior genericWindowKey");
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
  char msg[2024];
  auto it = windowDict.find(inWindowID);
  if(it == windowDict.end()){
    if (inWindowID == nullptr && losingFocus) {
      /* This occurs only when I have a window with keyboard focus and then
         I destroy the window (or otherwise lose focus.)
         I get callback to handle key but the window ID set zero.
         Verified this happens in simple C program, so it's not Python's fault.
         So far, it appears safe to just ignore.
         Filed with Laminar 18-May-2020 as XPD-10834
      */
      XPLMDebugString("nullptr window passed to genericWindowKey. Ignoring\n");
      errCheck("no callback, losing focus, genericWindowKey");
      return;
    }
    snprintf(msg, sizeof(msg), "Unknown window passed to genericWindowKey (%p) -- losingFocus is %d.\n", inWindowID, losingFocus);
    pythonLog("%s", msg);
    return;
  }
  PyObject *func = it->second.key;
  if (func != Py_None) {
    PyObject *arg1 = PyLong_FromLong(inKey);
    PyObject *arg2 = PyLong_FromLong(inFlags);
    PyObject *arg3 = PyLong_FromLong((unsigned int)(inVirtualKey &  0xff));  // XPD-17397 
    PyObject *arg4 = PyLong_FromLong(losingFocus);
    /* char *s = objToStr(pID); */
    /* printf("Calling genericWindowKey callback. inWindowID = %p, pPID = %s, losingFocus = %d\n", inWindowID, s, losingFocus); */
    /* free(s); */
    PyObject *pID = makeCapsule(inWindowID, "XPLMWindowID");
    set_moduleName(it->second.module_name);
    PyObject *args[] = {pID, arg1, arg2, arg3, (PyObject*)inRefcon, arg4};
    PyObject *oRes = PyObject_Vectorcall(func, args, 6, nullptr);
    if (PyErr_Occurred()) {
      pythonLogException();
      pythonLog("disabling key function");
      it->second.key = Py_None;
    }
      
    Py_DECREF(pID);
    Py_XDECREF(arg1);
    Py_XDECREF(arg2);
    Py_XDECREF(arg3);
    Py_XDECREF(arg4);
    Py_XDECREF(oRes);
  }
  errCheck("end genericWindowKey");
}

static int genericWindowMouseClick(XPLMWindowID     inWindowID,
                     int              x,
                     int              y,
                     XPLMMouseStatus  inMouse,
                     void            *inRefcon)
{
  errCheck("prior genericWindowMouseClick");
  (void) inRefcon;
  auto it = windowDict.find(inWindowID);
  if(it == windowDict.end()){
    pythonLog("Unknown window passed to click callback (%p).", inWindowID);
    return 1;
  }
  PyObject *func = it->second.click;
  if (func == Py_None) {
    /* consume click */
    errCheck("no func genericWindowMouseClick");
    return 1;
  }

  set_moduleName(it->second.module_name);
  PyObject *arg1 = PyLong_FromLong(x);
  PyObject *arg2 = PyLong_FromLong(y);
  PyObject *arg3 = PyLong_FromLong(inMouse);
  PyObject *pID = makeCapsule(inWindowID, "XPLMWindowID");
  PyObject *args[] = {pID, arg1, arg2, arg3, (PyObject*)inRefcon};
    PyObject *pRes = PyObject_Vectorcall(func, args, 5, nullptr);
  PyObject *err = PyErr_Occurred();
  Py_DECREF(arg1);
  Py_DECREF(arg2);
  Py_DECREF(arg3);
  Py_DECREF(pID);
  if(err){
    pythonLogException();
    pythonLog("disabling click function");
    it->second.click = Py_None;
    return 1;
  }
  if (!PyLong_Check(pRes)) {
    char *msg;
    char *s = objToStr(func);
    if (-1 == asprintf(&msg, "click() callback [%s] failed to return integer\n.", s)) {
      pythonLog("Failed to allocate asprintf memory for callback error.");
    } else {
      PyErr_SetString(PyExc_ValueError, msg);
      free(msg);
    }
    free(s);
    return 1;
  }
  int res = (int)PyLong_AsLong(pRes);
  Py_DECREF(pRes);
  errCheck("end genericWindowMouseClick");
  return res;
}

static int genericWindowRightClick(XPLMWindowID     inWindowID,
                     int              x,
                     int              y,
                     XPLMMouseStatus  inMouse,
                     void            *inRefcon)
{
  errCheck("prior genericWindowRightClick");
  (void) inRefcon;
  auto it = windowDict.find(inWindowID);
  if(it == windowDict.end()){
    pythonLog("Unknown window passed to rightClick callback (%p).", inWindowID);
    return 1;
  }
  PyObject *func = it->second.rightClick;
  if (func == Py_None) {
    return 1; /* consume click in window */
  }

  set_moduleName(it->second.module_name);
  PyObject *pID = makeCapsule(inWindowID, "XPLMWindowID");
  PyObject *arg1 = PyLong_FromLong(x);
  PyObject *arg2 = PyLong_FromLong(y);
  PyObject *arg3 = PyLong_FromLong(inMouse);
  PyObject *args[] = {pID, arg1, arg2, arg3, (PyObject*)inRefcon};
    PyObject *pRes = PyObject_Vectorcall(func, args, 5, nullptr);
  Py_DECREF(arg1);
  Py_DECREF(arg2);
  Py_DECREF(arg3);
  Py_DECREF(pID);
  PyObject *err = PyErr_Occurred();
  if(err){
    pythonLogException();
    pythonLog("Disabling right-click callback.");
    it->second.rightClick = Py_None;
    return 1;
  }
  if (!PyLong_Check(pRes)) {
    char *msg;
    char *s = objToStr(func);
    if (-1 == asprintf(&msg, "rightClick() callback [%s] failed to return integer\n.", s)) {
      pythonLog("Failed to allocate asprintf memory for right click callback.");
    } else {
      PyErr_SetString(PyExc_ValueError, msg);
      free(msg);
    }
    free(s);
    return 1;
  }
  int res = (int)PyLong_AsLong(pRes);
  Py_DECREF(pRes);
  errCheck("end genericWindowRightClick");
  return res;
}

static XPLMCursorStatus genericWindowCursor(XPLMWindowID  inWindowID,
                              int           x,
                              int           y,
                              void         *inRefcon)
{
  errCheck("prior genericWindowCursor");
  (void) inRefcon;
  auto it = windowDict.find(inWindowID);
  if(it == windowDict.end()){
    pythonLog("Unknown window passed to genericWindowCursor (%p).", inWindowID);
    return 0;
  }
  PyObject *func = it->second.cursor;
  set_moduleName(it->second.module_name);
  if((func == nullptr) || (func == Py_None)){
    return 0;
  }

  PyObject *pID = makeCapsule(inWindowID, "XPLMWindowID");
  PyObject *arg1 = PyLong_FromLong(x);
  PyObject *arg2 = PyLong_FromLong(y);
  PyObject *args[] = {pID, arg1, arg2, (PyObject*)inRefcon};
    PyObject *pRes = PyObject_Vectorcall(func, args, 4, nullptr);
  PyObject *err = PyErr_Occurred();
  Py_DECREF(arg1);
  Py_DECREF(arg2);
  Py_DECREF(pID);
  if(err){
    pythonLogException();
    pythonLog("Disabling cursor function");
    it->second.cursor = Py_None;
    return 0;
  }
  int res = (int)PyLong_AsLong(pRes);
  Py_DECREF(pRes);
  errCheck("end genericWindowCursor");
  return res;
}

static int genericWindowMouseWheel(XPLMWindowID  inWindowID,
                     int           x,
                     int           y,
                     int           wheel,
                     int           clicks,
                     void         *inRefcon)
{
  errCheck("prior genericWindowMouseWheel");
  (void) inRefcon;
  auto it = windowDict.find(inWindowID);
  if(it == windowDict.end()){
    pythonLog("Unknown window passed to genericWindowMouseWheel (%p).", inWindowID);
    return 1;
  }
  PyObject *func = it->second.wheel;
  set_moduleName(it->second.module_name);
  if((func == nullptr) || (func == Py_None)){
    return 1;
  }

  PyObject *pID = makeCapsule(inWindowID, "XPLMWindowID");
  PyObject *arg1 = PyLong_FromLong(x);
  PyObject *arg2 = PyLong_FromLong(y);
  PyObject *arg3 = PyLong_FromLong(wheel);
  PyObject *arg4 = PyLong_FromLong(clicks);
  PyObject *args[] = {pID, arg1, arg2, arg3, arg4, (PyObject*)inRefcon};
    PyObject *pRes = PyObject_Vectorcall(func, args, 6, nullptr);
  PyObject *err = PyErr_Occurred();
  Py_DECREF(arg1);
  Py_DECREF(arg2);
  Py_DECREF(arg3);
  Py_DECREF(arg4);
  Py_DECREF(pID);
  if(err){
    pythonLogException();
    pythonLog("Disabling wheel callback");
    it->second.wheel = Py_None;
    return 1;
  }
  int res = (int)PyLong_AsLong(pRes);
  Py_DECREF(pRes);
  errCheck("end genericWindowMouseWheel");
  return res;
}


My_DOCSTR(_createAvionicsEx__doc__, "createAvionicsEx",
          "screenWidth=100, screenHeight=200, bezelWidth=140, bezelHeight=250, screenOffsetX=20, screenOffsetY=25, "
          "drawOnDemand=0, bezelDraw=None, screenDraw=None, bezelClick=None, bezelRightClick=None, "
          "bezelScroll=None, bezelCursor=None, screenTouch=None, screenRightTouch=None, screenScroll=None, "
          "screenCursor=None, keyboard=None, brightness=None, deviceID=\"deviceID-<num>\", deviceName=\"deviceName-<num>\", refcon=None",
          "screenWidth: int = 100, screenHeight: int = 200, bezelWidth:int = 140, bezelHeight: int = 250, "
          "screenOffsetX: int = 20, screenOffsetY: int = 25, drawOnDemand: int = 0, "
          "bezelDraw: Optional[Callable[[float, float, float, Any], None]] = None, "
          "screenDraw: Optional[Callable[[Any], None]] = None, "
          "bezelClick: Optional[Callable[[int, int, XPLMMouseStatus, Any], int]] = None, "
          "bezelRightClick: Optional[Callable[[int, int, XPLMMouseStatus, Any], int]] = None, "
          "bezelScroll: Optional[Callable[[int, int, int, int, Any], int]] = None, "
          "bezelCursor: Optional[Callable[[int, int, Any], XPLMCursorStatus]] = None, "
          "screenTouch: Optional[Callable[[int, int, XPLMMouseStatus, Any], int]] = None, "
          "screenRightTouch: Optional[Callable[[int, int, XPLMMouseStatus, Any], int]] = None, "
          "screenScroll: Optional[Callable[[int, int, int, int, Any], int]] = None, "
          "screenCursor: Optional[Callable[[int, int, Any], XPLMCursorStatus]] = None, "
          "keyboard: Optional[Callable[[int, XPLMKeyFlags, int, Any, int], int]] = None, "
          "brightness: Optional[Callable[[float, float, float, float], float]] = None, "
          "deviceID: str = \"deviceID\", deviceName: str= \"deviceName\", "
          "refCon: Any = None,"
          ,
          "XPLMAvionicsID",
          "Creates glass cockpit device for 3D cockpit.\n"
          "With 12.0 you needed to call this within your XPluginStart callback\n"
          "to ensure your texture would be ready. Since 12.1, you may call this\n"
          "at anytime and X-Plane will retroactively map your display to it.\n"
          "Note, if not specified, we create a \"unique\" deviceID and deviceName,\n"
          "as deviceID *must* be unique.\n"
          "\n"
          "Returns new avionicsID.");


My_DOCSTR(_createWindowEx__doc__, "createWindowEx",
          "left=100, top=200, right=200, bottom=100, visible=0, draw=None, click=None, key=None, cursor=None, wheel=None, refCon=None, decoration=WindowDecorationRoundRectangle, layer=WindowLayerFloatingWindows, rightClick=None",
          "left:int=100, top:int=200, right:int=200, bottom:int=100,"
          "visible:int=0, "
          "draw:Optional[Callable[[XPLMWindowID, Any], None]]=None,"
          "click:Optional[Callable[[XPLMWindowID, int, int, XPLMMouseStatus, Any], int]]=None, "
          "key:Optional[Callable[[XPLMWindowID, int, int, int, Any, int], None]]=None, "
          "cursor:Optional[Callable[[XPLMWindowID, int, int, Any], XPLMCursorStatus]]=None, "
          "wheel:Optional[Callable[[XPLMWindowID, int, int, int, int, Any], int]]=None, "
          "refCon:Any=None, decoration:XPLMWindowDecoration=WindowDecorationRoundRectangle, "
          "layer:XPLMWindowLayer=WindowLayerFloatingWindows, "
          "rightClick:Optional[Callable[[XPLMWindowID, int, int, XPLMMouseStatus, Any], int]]=None, ",
          "XPLMWindowID",
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
  static char *keywords[] = {CHAR("left"), CHAR("top"), CHAR("right"), CHAR("bottom"), CHAR("visible"), CHAR("draw"), CHAR("click"), CHAR("key"), CHAR("cursor"),
                             CHAR("wheel"), CHAR("refCon"), CHAR("decoration"), CHAR("layer"), CHAR("rightClick"), nullptr};
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
    return nullptr;
  }

  XPLMCreateWindow_t window_params;
  window_params.structSize = sizeof(window_params);

  if (firstObj == Py_None|| PyLong_Check(firstObj)) {
    left = firstObj == Py_None ? 100 : PyLong_AsLong(firstObj);
    window_params.left = left;
    window_params.right = right;
    window_params.top = top;
    window_params.bottom = bottom;
    window_params.visible = visible;
    window_params.decorateAsFloatingWindow = decoration;
    window_params.layer = layer;
    window_params.refcon = refCon;
  } else if (PySequence_Check(firstObj)) {
    paramsObj = firstObj;
    if (PySequence_Length(paramsObj) != 14) {
      PyErr_SetString(PyExc_AttributeError ,"createWindowEx tuple did not contain 14 values\n.");
      return nullptr;
    }
    PyObject *paramsTuple = PySequence_Tuple(paramsObj);
    if (!paramsTuple) {
      pythonLogException();
      return nullptr;
    }
    window_params.left = PyLong_AsLong(PyTuple_GetItem(paramsTuple, 0));
    window_params.top = PyLong_AsLong(PyTuple_GetItem(paramsTuple, 1));
    window_params.right = PyLong_AsLong(PyTuple_GetItem(paramsTuple, 2));
    window_params.bottom = PyLong_AsLong(PyTuple_GetItem(paramsTuple, 3));
    window_params.visible = PyLong_AsLong(PyTuple_GetItem(paramsTuple, 4));
    window_params.decorateAsFloatingWindow = PyLong_AsLong(PyTuple_GetItem(paramsTuple, 11));
    window_params.layer = PyLong_AsLong(PyTuple_GetItem(paramsTuple, 12));
    window_params.refcon = PyTuple_GetItem(paramsTuple, 10);
    
    draw = PyTuple_GetItem(paramsTuple, 5);
    click = PyTuple_GetItem(paramsTuple, 6);
    key = PyTuple_GetItem(paramsTuple, 7);
    cursor = PyTuple_GetItem(paramsTuple, 8);
    wheel = PyTuple_GetItem(paramsTuple, 9);
    rightClick = PyTuple_GetItem(paramsTuple, 13);

    Py_DECREF(paramsTuple);
  } else {
    PyErr_SetString(PyExc_AttributeError ,"createWindowEx could not parse arguments.\n");
    return nullptr;
  }
    
  if (!PyCallable_Check(draw) && draw != Py_None) {
    PyErr_SetString(PyExc_ValueError ,"createWindowEx draw() is not callable.\n");
    return nullptr;
  }
  if (!PyCallable_Check(click) && click != Py_None) {
    PyErr_SetString(PyExc_ValueError ,"createWindowEx click() is not callable.\n");
    return nullptr;
  }
  if (!PyCallable_Check(key) && key != Py_None) {
    PyErr_SetString(PyExc_ValueError ,"createWindowEx key() is not callable.\n");
    return nullptr;
  }
  
  if (!PyCallable_Check(cursor) && cursor != Py_None) {
    PyErr_SetString(PyExc_ValueError ,"createWindowEx cursor() is not callable.\n");
    return nullptr;
  }
  
  if (!PyCallable_Check(wheel) && wheel != Py_None) {
    PyErr_SetString(PyExc_ValueError ,"createWindowEx wheel() is not callable.\n");
    return nullptr;
  }
  
  if (!PyCallable_Check(rightClick) && rightClick != Py_None) {
    PyErr_SetString(PyExc_ValueError ,"createWindowEx rightClick() is not callable.\n");
    return nullptr;
  }
  
  Py_INCREF(draw);
  Py_INCREF(click);
  Py_INCREF(key);
  Py_INCREF(cursor);
  Py_INCREF(wheel);
  Py_INCREF(rightClick);
  
  Py_INCREF(window_params.refcon);

  window_params.drawWindowFunc = genericWindowDraw;
  window_params.handleMouseClickFunc = genericWindowMouseClick;
  window_params.handleKeyFunc = genericWindowKey;
  window_params.handleCursorFunc = genericWindowCursor;
  window_params.handleMouseWheelFunc = genericWindowMouseWheel;
  window_params.handleRightClickFunc = genericWindowRightClick;

  XPLMWindowID id = XPLMCreateWindowEx(&window_params);
  if (id == nullptr) {
    PyErr_SetString(PyExc_RuntimeError ,"Failed to createWindowEx.\n");
    Py_DECREF(draw);
    Py_DECREF(click);
    Py_DECREF(key);
    Py_DECREF(cursor);
    Py_DECREF(wheel);
    Py_DECREF(rightClick);
    Py_DECREF(window_params.refcon);
    return nullptr;
  }
    
  PyObject *pID = makeCapsule(id, "XPLMWindowID");

  windowDict[id] = {
    .draw = draw,
    .click = click,
    .key = key,
    .cursor = cursor,
    .wheel = wheel,
    .rightClick = rightClick,
    .refCon = refCon,
    .module_name = CurrentPythonModuleName
  };

  errCheck("end createWindowEx");
  return pID;
}

My_DOCSTR(_createWindow__doc__, "createWindow",
          "left=100, top=200, right=200, bottom=100, visible=0, draw=None, key=None, mouse=None, refCon=None",
          "left:int=100, top:int=200, right:int=200, bottom:int=100, visible:int=0,"
          "draw:Optional[Callable[[XPLMWindowID, Any], None]]=None,"
          "key:Optional[Callable[[XPLMWindowID, int, int, int, Any, int], None]]=None, "
          "mouse:Optional[Callable[[XPLMWindowID, int, int, XPLMMouseStatus, Any], int]]=None, "
          "refCon:Any=None",
          "XPLMWindowID",
          "(Deprecated do not use)");
static PyObject *XPLMCreateWindowFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  errCheck("prior createWindow");
  static char *keywords[] = {CHAR("left"), CHAR("top"), CHAR("right"), CHAR("bottom"), CHAR("visible"), CHAR("draw"), CHAR("key"), CHAR("mouse"), CHAR("refCon"), nullptr};
  (void) self;
  PyObject *drawCallback=Py_None, *keyCallback=Py_None, *mouseCallback=Py_None, *refcon=Py_None;
  int left=100, top=200, right=200, bottom=100, visible=0;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|iiiiiOOOO", keywords, &left, &top, &right, &bottom, &visible,
                                  &drawCallback, &keyCallback, &mouseCallback, &refcon)){
    return nullptr;
  }

  Py_INCREF(refcon);
  XPLMWindowID id = XPLMCreateWindow(left, top, right, bottom, visible, genericWindowDraw, genericWindowKey, genericWindowMouseClick, refcon);

  PyObject *pID = makeCapsule(id, "XPLMWindowID");
  windowDict[id] = {
    .draw = drawCallback,
    .click = mouseCallback,
    .key = keyCallback,
    .cursor = Py_None,
    .wheel = Py_None,
    .rightClick = Py_None,
    .refCon = refcon,
    .module_name = CurrentPythonModuleName
  };
  errCheck("end createWindow");
  return pID;
}

My_DOCSTR(_destroyWindow__doc__, "destroyWindow",
          "windowID",
          "windowID:XPLMWindowID",
          "None",
          "Destroys window. Returns None.");
static PyObject *XPLMDestroyWindowFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("windowID"), nullptr};
  (void) self;
  PyObject *pID;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &pID)){
    return nullptr;
  }
  XPLMWindowID winID = getVoidPtr(pID, "XPLMWindowID");
  auto it = windowDict.find(winID);
  if(it != windowDict.end()){
    XPLMDestroyWindow(winID);
    Py_DECREF(it->second.draw);
    Py_DECREF(it->second.click);
    Py_DECREF(it->second.key);
    Py_DECREF(it->second.cursor);
    Py_DECREF(it->second.wheel);
    Py_DECREF(it->second.rightClick);
    Py_DECREF(it->second.refCon);
    windowDict.erase(winID);
  }else{
    PyErr_SetString(PyExc_RuntimeError ,"XPLMDestroyWindow couldn't find the window to destroy.\n");
    return nullptr;
  }
  Py_RETURN_NONE;
}

My_DOCSTR(_getScreenSize__doc__, "getScreenSize",
          "",
          "",
          "tuple[int, int]",
          "Returns (width, height) of screen.");
static PyObject *XPLMGetScreenSizeFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("width"), CHAR("height"), nullptr};
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

My_DOCSTR(_getScreenBoundsGlobal__doc__, "getScreenBoundsGlobal",
          "",
          "",
          "tuple[int, int, int, int]",
          "Returns (left, top, right, bottom) of screen bounds.");
static PyObject *XPLMGetScreenBoundsGlobalFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("left"), CHAR("top"), CHAR("right"), CHAR("bottom"), nullptr};
  (void) self;
  (void) args;
  if(!XPLMGetScreenBoundsGlobal_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMGetScreenBoundsGlobal is available only in XPLM300 and up.");
    return nullptr;
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

My_DOCSTR(_getAllMonitorBoundsGlobal__doc__, "getAllMonitorBoundsGlobal",
          "bounds, refCon",
          "bounds:Callable[[int, int, int, int, int, Any], None], refCon:Any",
          "None",
          "Immediately calls bounds() callback once for each monitor to retrieve\n"
          "bounds for each monitor running full-screen simulator.\n"
          "\n"
          "Callback function is\n"
          "  bounds(index, left, top, right, bottom, refCon)");

static PyObject *XPLMGetAllMonitorBoundsGlobalFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("bounds"), CHAR("refCon"), nullptr};
  (void) self;
  PyObject *refconObj;
  if(!XPLMGetAllMonitorBoundsGlobal_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMGetAllMonitorBoundsGlobal is available only in XPLM300 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "OO", keywords, &monitorBndsCallback, &refconObj)){
    return nullptr;
  }
  XPLMGetAllMonitorBoundsGlobal_ptr(receiveMonitorBounds, (void *)refconObj);
  Py_RETURN_NONE;
}

My_DOCSTR(_getAllMonitorBoundsOS__doc__, "getAllMonitorBoundsOS",
          "bounds, refCon",
          "bounds:Callable[[int, int, int, int, int, Any], None], refCon:Any",
          "None",
          "Immediately calls bounds() once for each monitor known to OS.\n"
          "\n"
          "Callback function is\n"
          "  bounds(index, refCon, left, top, right, bottom, refCon)");
static PyObject *XPLMGetAllMonitorBoundsOSFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("bounds"), CHAR("refCon"), nullptr};
  (void) self;
  PyObject *refconObj;
  if(!XPLMGetAllMonitorBoundsOS_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMGetAllMonitorBoundsOS is available only in XPLM300 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "OO", keywords, &monitorBndsCallback, &refconObj)){
    return nullptr;
  }
  XPLMGetAllMonitorBoundsOS_ptr(receiveMonitorBounds, (void *)refconObj);
  Py_RETURN_NONE;
}

My_DOCSTR(_getMouseLocation__doc__, "getMouseLocation",
          "",
          "",
          "None | tuple[int, int]",
          "Deprecated, use getMouseLocationGlobal().");
static PyObject *XPLMGetMouseLocationFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("x"), CHAR("y"), nullptr};
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

My_DOCSTR(_getMouseLocationGlobal__doc__, "getMouseLocationGlobal",
          "",
          "",
          "None | tuple[int, int]",
          "Returns current mouse location (x, y).");
static PyObject *XPLMGetMouseLocationGlobalFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("x"), CHAR("y"), nullptr};
  (void) self;
  (void) args;
  int x, y;
  if(!XPLMGetMouseLocationGlobal_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMGetMouseLocationGlobal is available only in XPLM300 and up.");
    return nullptr;
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

My_DOCSTR(_getWindowGeometry__doc__, "getWindowGeometry",
          "windowID",
          "windowID:XPLMWindowID",
          "None | tuple[int, int, int, int]",
          "Returns window geometry (left, top, right, bottom).");
static PyObject *XPLMGetWindowGeometryFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("windowID"), CHAR("left"), CHAR("top"), CHAR("right"), CHAR("bottom"), nullptr};
  (void) self;
  PyObject *win;
  PyObject *outLeft, *outTop, *outRight, *outBottom;
  int returnValues = 0;
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OOOOO", keywords, &win, &outLeft, &outTop, &outRight, &outBottom)) {
    returnValues = 1;
    PyErr_Clear();
    static char *nkeywords[] = {CHAR("windowID"), nullptr};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O", nkeywords, &win)) {
      return nullptr;
    }
  }
  int left, top, right, bottom;
  XPLMGetWindowGeometry(getVoidPtr(win, "XPLMWindowID"), &left, &top, &right, &bottom);
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

My_DOCSTR(_getAvionicsGeometry__doc__, "getAvionicsGeometry",
          "avionicsID",
          "avionicsID:XPLMAvionicsID",
          "tuple[int, int, int, int]",
          "Returns geometry (left, top, right, bottom) of popup window\n"
          "in X-Plane coordinate system.");
static PyObject *XPLMGetAvionicsGeometryFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("avionicsID"), nullptr};
  (void) self;
  PyObject *avionicsID;

  if (!XPLMGetAvionicsGeometry_ptr) {
    PyErr_SetString(PyExc_RuntimeError , "XPLMGetAvionicsGeometry is available only in XPLM410 and up.");
    return nullptr;
  }
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &avionicsID)) {
    return nullptr;
  }
  int left=0, top=0, right=0, bottom=0;
  XPLMGetAvionicsGeometry_ptr(getVoidPtr(avionicsID, "XPLMAvionicsID"), &left, &top, &right, &bottom);
  return Py_BuildValue("(iiii)", left, top, right, bottom);
}
  
My_DOCSTR(_setWindowGeometry__doc__, "setWindowGeometry",
          "windowID, left, top, right, bottom",
          "windowID:XPLMWindowID, left:int, top:int, right:int, bottom:int",
          "None",
          "Sets window geometry.");
static PyObject *XPLMSetWindowGeometryFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("windowID"), CHAR("left"), CHAR("top"), CHAR("right"), CHAR("bottom"), nullptr};
  (void) self;
  PyObject *win;
  int inLeft, inTop, inRight, inBottom;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Oiiii", keywords, &win, &inLeft, &inTop, &inRight, &inBottom)){
    return nullptr;
  }
  void *inWindowID = getVoidPtr(win, "XPLMWindowID");
  XPLMSetWindowGeometry(inWindowID, inLeft, inTop, inRight, inBottom);
  Py_RETURN_NONE;
}

My_DOCSTR(_setAvionicsGeometry__doc__, "setAvionicsGeometry",
          "avionicsID, left, top, right, bottom",
          "avionicsID:XPLMAvionicsID, left:int, top:int, right:int, bottom:int",
          "None",
          "Sets window geometry for cockpit device's popup window in X-Plane coordinates.");
static PyObject *XPLMSetAvionicsGeometryFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("avionicsID"), CHAR("left"), CHAR("top"), CHAR("right"), CHAR("bottom"), nullptr};
  (void) self;
  PyObject *avionicsID;
  int inLeft, inTop, inRight, inBottom;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Oiiii", keywords, &avionicsID, &inLeft, &inTop, &inRight, &inBottom)){
    return nullptr;
  }
  if(!XPLMSetAvionicsGeometry_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMSetAvionicsGeometry is available only in XPLM410 and up.");
    return nullptr;
  }
    
  void *inAvionicsID = getVoidPtr(avionicsID, "XPLMAvionicsID");
  XPLMSetAvionicsGeometry_ptr(inAvionicsID, inLeft, inTop, inRight, inBottom);
  Py_RETURN_NONE;
}

My_DOCSTR(_getWindowGeometryOS__doc__, "getWindowGeometryOS",
          "windowID",
          "windowID:XPLMWindowID",
          "None | tuple[int, int, int, int]",
          "Returns window geometry for popped-out window (left, top, right, bottom).");
static PyObject *XPLMGetWindowGeometryOSFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("windowID"), CHAR("left"), CHAR("top"), CHAR("right"), CHAR("bottom"), nullptr};
  (void) self;
  PyObject *win;
  PyObject *outLeft, *outTop, *outRight, *outBottom;
  int returnValues = 0;
  if(!XPLMGetWindowGeometryOS_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMGetWindowGeometryOS is available only in XPLM300 and up.");
    return nullptr;
  }
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OOOOO", keywords, &win, &outLeft, &outTop, &outRight, &outBottom)) {
    returnValues = 1;
    PyErr_Clear();
    static char *nkeywords[] = {CHAR("windowID"), nullptr};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O", nkeywords, &win)) {
      return nullptr;
    }
  }
  int left, top, right, bottom;
  XPLMGetWindowGeometryOS_ptr(getVoidPtr(win, "XPLMWindowID"), &left, &top, &right, &bottom);
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

My_DOCSTR(_getAvionicsGeometryOS__doc__, "getAvionicsGeometryOS",
          "avionicsID",
          "avionicsID:XPLMAvionicsID",
          "tuple[int, int, int, int]",
          "Returns window geometry for popped-out avionics device (left, top, right, bottom).");
static PyObject *XPLMGetAvionicsGeometryOSFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("avionicsID"), nullptr};
  (void) self;
  PyObject *avionicsID;
  if(!XPLMGetAvionicsGeometryOS_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMGetAvionicsGeometryOS is available only in XPLM410 and up.");
    return nullptr;
  }
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &avionicsID)) {
    return nullptr;
  }
  int left=0, top=0, right=0, bottom=0;
  XPLMGetAvionicsGeometryOS_ptr(getVoidPtr(avionicsID, "XPLMAvionicsID"), &left, &top, &right, &bottom);
  return Py_BuildValue("(iiii)", left, top, right, bottom);
}

My_DOCSTR(_setAvionicsGeometryOS__doc__, "setAvionicsGeometryOS",
          "avionicsID, left, top, right, bottom",
          "avionicsID:XPLMAvionicsID, left:int, top:int, right:int, bottom:int",
          "None",
          "Sets window geometry for popped-out avionics.");
static PyObject *XPLMSetAvionicsGeometryOSFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("avionicsID"), CHAR("left"), CHAR("top"), CHAR("right"), CHAR("bottom"), nullptr};
  (void) self;
  PyObject *avionicsID;
  int inLeft, inTop, inRight, inBottom;
  if(!XPLMSetAvionicsGeometryOS_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMSetAvionicsGeometryOS is available only in XPLM410 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Oiiii", keywords, &avionicsID, &inLeft, &inTop, &inRight, &inBottom)){
    return nullptr;
  }
  XPLMSetAvionicsGeometryOS_ptr(getVoidPtr(avionicsID, "XPLMAvionicsID"), inLeft, inTop, inRight, inBottom);
  Py_RETURN_NONE;
}

My_DOCSTR(_setWindowGeometryOS__doc__, "setWindowGeometryOS",
          "windowID, left, top, right, bottom",
          "windowID:XPLMWindowID, left:int, top:int, right:int, bottom:int",
          "None",
          "Sets window geometry for popped-out window.");
static PyObject *XPLMSetWindowGeometryOSFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("windowID"), CHAR("left"), CHAR("top"), CHAR("right"), CHAR("bottom"), nullptr};
  (void) self;
  PyObject *win;
  int inLeft, inTop, inRight, inBottom;
  if(!XPLMSetWindowGeometryOS_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMSetWindowGeometryOS is available only in XPLM300 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Oiiii", keywords, &win, &inLeft, &inTop, &inRight, &inBottom)){
    return nullptr;
  }
  void *inWindowID = getVoidPtr(win, "XPLMWindowID");
  XPLMSetWindowGeometryOS_ptr(inWindowID, inLeft, inTop, inRight, inBottom);
  Py_RETURN_NONE;
}

My_DOCSTR(_getWindowGeometryVR__doc__, "getWindowGeometryVR",
          "windowID",
          "windowID:XPLMWindowID",
          "None | tuple[int, int]",
          "Gets window geometry for VR window (width, height)");
static PyObject *XPLMGetWindowGeometryVRFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {nullptr};
  (void) self;
  PyObject *win;
  int returnValues = 0;
  PyObject *outWidthBoxels, *outHeightBoxels;
  if(!XPLMGetWindowGeometryVR_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMGetWindowGeometryVR is available only in XPLM301 and up.");
    return nullptr;
  }
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OOO", keywords, &win, &outWidthBoxels, &outHeightBoxels)) {
    returnValues = 1;
    PyErr_Clear();
    if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &win)){
      return nullptr;
    }
  }
  int widthBoxels, heightBoxels;
  XPLMGetWindowGeometryVR_ptr(getVoidPtr(win, "XPLMWindowID"), &widthBoxels, &heightBoxels);
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

My_DOCSTR(_setWindowGeometryVR__doc__, "setWindowGeometryVR",
          "windowID, width, height",
          "windowID:XPLMWindowID, width:int, height:int",
          "None",
          "Sets window geometry for VR window.");
static PyObject *XPLMSetWindowGeometryVRFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("windowID"), CHAR("width"), CHAR("height"), nullptr};
  (void) self;
  PyObject *win;
  int widthBoxels, heightBoxels;
  if(!XPLMSetWindowGeometryVR_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMSetWindowGeometryVR is available only in XPLM301 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Oii", keywords, &win, &widthBoxels, &heightBoxels)){
    return nullptr;
  }
  void *inWindowID = getVoidPtr(win, "XPLMWindowID");
  XPLMSetWindowGeometryVR_ptr(inWindowID, widthBoxels, heightBoxels);
  Py_RETURN_NONE;
}

My_DOCSTR(_getWindowIsVisible__doc__, "getWindowIsVisible",
          "windowID",
          "windowID:XPLMWindowID",
          "int",
          "Returns 1 if window is visible, 0 otherwise.");
static PyObject *XPLMGetWindowIsVisibleFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("windowID"), nullptr};
  (void) self;
  PyObject *win;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &win)){
    return nullptr;
  }
  void *inWindowID = getVoidPtr(win, "XPLMWindowID");
  return PyLong_FromLong(XPLMGetWindowIsVisible(inWindowID));
}

My_DOCSTR(_setWindowIsVisible__doc__, "setWindowIsVisible",
          "windowID, visible=1",
          "windowID:XPLMWindowID, visible:int=1",
          "None",
          "Sets window visibility. 1 indicates visible, 0 is not-visible.");
static PyObject *XPLMSetWindowIsVisibleFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("windowID"), CHAR("visible"), nullptr};
  (void) self;
  PyObject *win;
  int inIsVisible = 1;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|i", keywords, &win, &inIsVisible)){
    return nullptr;
  }
  XPLMWindowID inWindowID = getVoidPtr(win, "XPLMWindowID");
  XPLMSetWindowIsVisible(inWindowID, inIsVisible);
  Py_RETURN_NONE;
}

My_DOCSTR(_windowIsPoppedOut__doc__, "windowIsPoppedOut",
          "windowID",
          "windowID:XPLMWindowID",
          "int",
          "Returns 1 if window is popped-out, 0 otherwise.");
static PyObject *XPLMWindowIsPoppedOutFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("windowID"), nullptr};
  (void) self;
  PyObject *win;
  if(!XPLMWindowIsPoppedOut_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMWindowIsPoppedOut is available only in XPLM300 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &win)){
    return nullptr;
  }
  void *inWindowID = getVoidPtr(win, "XPLMWindowID");
  return PyLong_FromLong(XPLMWindowIsPoppedOut_ptr(inWindowID));
}

My_DOCSTR(_windowIsInVR__doc__, "windowIsInVR",
          "windowID",
          "windowID:XPLMWindowID",
          "int",
          "Returns 1 if window is in VR, 0 otherwise.");
static PyObject *XPLMWindowIsInVRFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("windowID"), nullptr};
  (void) self;
  PyObject *win;
  if(!XPLMWindowIsInVR_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMWindowIsInVR is available only in XPLM301 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &win)){
    return nullptr;
  }
  void *inWindowID = getVoidPtr(win, "XPLMWindowID");
  return PyLong_FromLong(XPLMWindowIsInVR_ptr(inWindowID));
}

My_DOCSTR(_setWindowGravity__doc__, "setWindowGravity",
          "windowID, left, top, right, bottom",
          "windowID:XPLMWindowID, left:int, top:int, right:int, bottom:int",
          "None",
          "Set window's gravity (resize window, relative screen size changes.)\n"
          "\n"
          "values typically range 0.0 - 1.0, and reflect how much the window's\n"
          "value will change relative to change in screen size.\n"
          "0.0 means zero change relative to the right side (for left/right edges)\n"
          "    and zero change relative to the bottom (for (for top/bottom edges)\n"
          "1.0 means 100% change relative to the right side / bottom");
static PyObject *XPLMSetWindowGravityFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("windowID"), CHAR("left"), CHAR("top"), CHAR("right"), CHAR("bottom"), nullptr};
  (void) self;
  PyObject *win;
  float inLeftGravity, inTopGravity, inRightGravity, inBottomGravity;
  if(!XPLMSetWindowGravity_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMSetWindowGravity is available only in XPLM300 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Offff", keywords, &win, &inLeftGravity, &inTopGravity, &inRightGravity, &inBottomGravity)){
    return nullptr;
  }
  XPLMWindowID inWindowID = getVoidPtr(win, "XPLMWindowID");
  XPLMSetWindowGravity_ptr(inWindowID, inLeftGravity, inTopGravity, inRightGravity, inBottomGravity);
  Py_RETURN_NONE;
}

My_DOCSTR(_setWindowResizingLimits__doc__, "setWindowResizingLimits",
          "windowID, minWidth=0, minHeight=0, maxWidth=10000, maxHeight=10000",
          "windowID:XPLMWindowID, minWidth:int=0, minHeight:int=0, maxWidth:int=10000, maxHeight:int=10000",
          "None",
          "Set maximum and minimum window size.");
static PyObject *XPLMSetWindowResizingLimitsFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("windowID"), CHAR("minWidth"), CHAR("minHeight"), CHAR("maxWidth"), CHAR("maxHeight"), nullptr};
  (void) self;
  PyObject *win;
  int inMinWidthBoxels=0, inMinHeightBoxels=0, inMaxWidthBoxels=10000, inMaxHeightBoxels=10000;
  if(!XPLMSetWindowResizingLimits_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMSetWindowResizingLimits is available only in XPLM300 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|iiii", keywords, &win, &inMinWidthBoxels, &inMinHeightBoxels, &inMaxWidthBoxels, &inMaxHeightBoxels)){
    return nullptr;
  }
  XPLMWindowID inWindowID = getVoidPtr(win, "XPLMWindowID");
  inMinWidthBoxels = inMinWidthBoxels < 0 ? 0 : inMinWidthBoxels;
  XPLMSetWindowResizingLimits_ptr(inWindowID, inMinWidthBoxels, inMinHeightBoxels, inMaxWidthBoxels, inMaxHeightBoxels);
  Py_RETURN_NONE;
}

My_DOCSTR(_setWindowPositioningMode__doc__, "setWindowPositioningMode",
          "windowID, mode, index=-1",
          "windowID:XPLMWindowID, mode:XPLMWindowPositioningMode, index:int=-1",
          "None",
          "Set window positioning mode:\n"
          " * WindowPositionFree\n"
          " * WindowCenterOnMonitor\n"
          " * WindowFullScreenOnMonitor\n"
          " * WindowFullScreenOnAllMonitors\n"
          " * WindowPopOut\n"
          " * WindowVR");
static PyObject *XPLMSetWindowPositioningModeFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("windowID"), CHAR("mode"), CHAR("index"), nullptr};
  (void) self;
  PyObject *win;
  int inPositioningMode, inMonitorIndex=-1;
  if(!XPLMSetWindowPositioningMode_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMSetWindowPositioningMode is available only in XPLM300 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Oi|i", keywords, &win, &inPositioningMode, &inMonitorIndex)){
    return nullptr;
  }
  XPLMWindowID inWindowID = getVoidPtr(win, "XPLMWindowID");
  XPLMSetWindowPositioningMode_ptr(inWindowID, inPositioningMode, inMonitorIndex);
  Py_RETURN_NONE;
}

My_DOCSTR(_setWindowTitle__doc__, "setWindowTitle",
          "windowID, title",
          "windowID:XPLMWindowID, title:str",
          "None",
          "Set window title.");
static PyObject *XPLMSetWindowTitleFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("windowID"), CHAR("title"), nullptr};
  (void) self;
  PyObject *win;
  const char *inWindowTitle;
  if(!XPLMSetWindowTitle_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMSetWindowTitle is available only in XPLM300 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Os", keywords, &win, &inWindowTitle)){
    return nullptr;
  }
  XPLMWindowID inWindowID = getVoidPtr(win, "XPLMWindowID");
  XPLMSetWindowTitle_ptr(inWindowID, strdup(inWindowTitle));
  Py_RETURN_NONE;
}

My_DOCSTR(_getWindowRefCon__doc__, "getWindowRefCon",
          "windowID",
          "windowID:XPLMWindowID",
          "Any",
          "Return window's reference constant refCon.");
static PyObject *XPLMGetWindowRefConFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("windowID"), nullptr};
  (void) self;
  PyObject *win;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &win)){
    return nullptr;
  }
  XPLMWindowID inWindowID = getVoidPtr(win, "XPLMWindowID");
  PyObject *res = (PyObject *)XPLMGetWindowRefCon(inWindowID);
  // Needs to be done, because python decrefs it when the function
  //   that called us returns; otherwise all hell breaks loose!!!
  // TODO: Check no other instance of such a problem lingers in the interface!!!
  Py_INCREF(res);
  return res;
}

My_DOCSTR(_setWindowRefCon__doc__, "setWindowRefCon",
          "windowID, refCon",
          "windowID:XPLMWindowID, refCon:Any",
          "None",
          "Set window's reference constant refCon.");
static PyObject *XPLMSetWindowRefConFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("windowID"), CHAR("refCon"), nullptr};
  (void) self;
  PyObject *win, *inRefcon;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "OO", keywords, &win, &inRefcon)){
    return nullptr;
  }
  XPLMWindowID inWindowID = getVoidPtr(win, "XPLMWindowID");
  // Decref the old refcon
  PyObject *tmp = (PyObject *)XPLMGetWindowRefCon(inWindowID);
  Py_DECREF(tmp);
  // Make sure it stays with us
  Py_INCREF(inRefcon);
  XPLMSetWindowRefCon(inWindowID, inRefcon);
  Py_RETURN_NONE;
}

My_DOCSTR(_takeKeyboardFocus__doc__, "takeKeyboardFocus",
          "windowID",
          "windowID:XPLMWindowID",
          "None",
          "Take keyboard focus. 0 to send focus to X-Plane.");
static PyObject *XPLMTakeKeyboardFocusFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("windowID"), nullptr};
  (void) self;
  PyObject *win;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &win)){
    return nullptr;
  }
  // use inWindowID 0, if passed in value of 0
  XPLMWindowID inWindowID;
  if (PyLong_Check(win) && PyLong_AsLong(win) == 0) {
    inWindowID = 0;
  } else {
    inWindowID = getVoidPtr(win, "XPLMWindowID");
  }
  XPLMTakeKeyboardFocus(inWindowID);
  Py_RETURN_NONE;
}

My_DOCSTR(_hasKeyboardFocus__doc__, "hasKeyboardFocus",
          "windowID",
          "windowID: XPLMWindowID",
          "int",
          "Returns 1 if window has current keyboard focus.\n"
          "Pass 0 to query if X-Plane has current focus.");
static PyObject *XPLMHasKeyboardFocusFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("windowID"), nullptr};
  (void) self;
  PyObject *win;
  if(!XPLMHasKeyboardFocus_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMHasKeyboardFocus is available only in XPLM300 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &win)){
    return nullptr;
  }
  // use inWindowID 0, if passed in value of 0
  XPLMWindowID inWindowID;
  if (PyLong_Check(win) && PyLong_AsLong(win) == 0) {
    inWindowID = 0;
  } else {
    inWindowID = getVoidPtr(win, "XPLMWindowID");
  }
  
  return PyLong_FromLong(XPLMHasKeyboardFocus_ptr(inWindowID));
}

My_DOCSTR(_bringWindowToFront__doc__, "bringWindowToFront",
          "windowID",
          "windowID:XPLMWindowID",
          "None",
          "Bring window to front (of its window layer).");
static PyObject *XPLMBringWindowToFrontFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("windowID"), nullptr};
  (void) self;
  PyObject *win;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &win)){
    return nullptr;
  }
  XPLMWindowID inWindowID = getVoidPtr(win, "XPLMWindowID");
  XPLMBringWindowToFront(inWindowID);
  Py_RETURN_NONE;
}

My_DOCSTR(_isWindowInFront__doc__, "isWindowInFront",
          "windowID",
          "windowID:XPLMWindowID",
          "int",
          "Returns 1 if window is currently in the front of its window layer).");
static PyObject *XPLMIsWindowInFrontFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("windowID"), nullptr};
  (void) self;
  PyObject *win;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &win)){
    return nullptr;
  }
  XPLMWindowID inWindowID = getVoidPtr(win, "XPLMWindowID");
  return PyLong_FromLong(XPLMIsWindowInFront(inWindowID));
}


My_DOCSTR(_registerHotKey__doc__, "registerHotKey",
          "vkey, flags=0, description='', hotKey=None, refCon=None",
          "vkey:int, flags:XPLMKeyFlags=NoFlag, description:str='', "
          "hotKey:Optional[Callable[[Any], None]]=None, "
          "refCon:Any=None",
          "XPLMHotKeyID",
          "Registers hot key."
          "\n"
          "Callback is hotKey(refCon), it does not need to return anything.\n"
          "Registration returns a hotKeyID, which can be used with unregisterHotKey()");
static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;

  for (auto& pair : drawCallbackDict) {
    DrawCallbackInfo& info = pair.second;
    Py_DECREF(info.callback);
    Py_DECREF(info.refCon);
  }
  drawCallbackDict.clear();

  for (auto& pair : keySnifferCallbackDict) {
    KeySnifferCallbackInfo& info = pair.second;
    Py_DECREF(info.callback);
    Py_DECREF(info.refCon);
  }
  keySnifferCallbackDict.clear();

  for (auto& pair : windowDict) {
    XPLMWindowID winID = (XPLMWindowID) pair.first;
    WindowCallbackInfo& info = pair.second;
    PyObject *tmp = (PyObject *)XPLMGetWindowRefCon(winID);
    Py_DECREF(tmp);
    XPLMDestroyWindow(winID);
    Py_DECREF(info.draw);
    Py_DECREF(info.click);
    Py_DECREF(info.key);
    Py_DECREF(info.cursor);
    Py_DECREF(info.rightClick);
  }
  windowDict.clear();

  for (auto& pair : hotkeyDict) {
    HotKeyCallbackInfo& info = pair.second;
    Py_DECREF(info.callback);
    Py_DECREF(info.refCon);
  }
  hotkeyDict.clear();

  for (auto& pair : avionicsCallbacksDict) {
    AvionicsCallbackInfo& info = pair.second;
    if(info.before != Py_None) Py_DECREF(info.before);
    if(info.after != Py_None) Py_DECREF(info.after);
    if(info.refCon != Py_None) Py_DECREF(info.refCon);
    if(info.bezel_draw != Py_None) Py_DECREF(info.bezel_draw);
    if(info.draw != Py_None) Py_DECREF(info.draw);
    if(info.bezel_click != Py_None) Py_DECREF(info.bezel_click);
    if(info.bezel_rightclick != Py_None) Py_DECREF(info.bezel_rightclick);
    if(info.bezel_scroll != Py_None) Py_DECREF(info.bezel_scroll);
    if(info.bezel_cursor != Py_None) Py_DECREF(info.bezel_cursor);
    if(info.screen_touch != Py_None) Py_DECREF(info.screen_touch);
    if(info.screen_righttouch != Py_None) Py_DECREF(info.screen_righttouch);
    if(info.screen_scroll != Py_None) Py_DECREF(info.screen_scroll);
    if(info.screen_cursor != Py_None) Py_DECREF(info.screen_cursor);
    if(info.keyboard != Py_None) Py_DECREF(info.keyboard);
    if(info.brightness != Py_None) Py_DECREF(info.brightness);
  }
  avionicsCallbacksDict.clear();
  Py_RETURN_NONE;
}


My_DOCSTR(_registerKeySniffer__doc__, "registerKeySniffer",
          "sniffer, before=0, refCon=None",
          "sniffer:Callable[[int, XPLMKeyFlags, int, Any], int], before:int=0, refCon:Any=None",
          "int",
          "Registers a key sniffer callback function.\n"
          "\n"
          "sniffer() callback takes four parameters (key, flags, vKey, refCon) and\n"
          "should return 0 to consume the key, 1 to pass it to next sniffer or X-Plane.\n"
          "\n"
          "before=1 will intercept keys before windows (i.e., the user may be typing in\n"
          "input field), so generally, use before=0 to sniff keys not already consumed.\n"
          "\nrefCon will be passed to your sniffer callback.");

My_DOCSTR(_unregisterKeySniffer__doc__, "unregisterKeySniffer",
          "sniffer, before=0, refCon=None",
          "sniffer:Callable[[int, XPLMKeyFlags, int, Any], int], before:int=0, refCon:Any=None",
          "int",
          "Unregisters key sniffer.\n"
          "\n"
          "Parameters must match those provided with registerKeySniffer().\n"
          "Returns 1 on success, 0 otherwise.");
My_DOCSTR(_unregisterHotKey__doc__, "unregisterHotKey",
          "hotKeyID",
          "hotKeyID:XPLMHotKeyID",
          "None",
          "Unregisters hot key associated with hotKeyID.\n"
          "\n"
          "hotKeyID must be registered to this plugin using registerHotKey()\n"
          "otherwise unregistration will fail.");
My_DOCSTR(_countHotKeys__doc__, "countHotKeys",
          "",
          "",
          "int",
          "Return number of hot keys currently defined in the simulator.");
My_DOCSTR(_getNthHotKey__doc__, "getNthHotKey",
          "index",
          "index:int",
          "XPLMHotKeyID",
          "Return hotKeyID for (zero-based) Nth hot key defined in sim.");
My_DOCSTR(_getHotKeyInfo__doc__, "getHotKeyInfo",
          "hotKeyID",
          "hotKeyID:XPLMHotKeyID",
          "None | HotKeyInfo",
          "Return object with hot key information.\n"
          "\n"
          "  .description\n"
          "  .virtualKey\n"
          "  .flags\n"
          "  .plugin");
My_DOCSTR(_setHotKeyCombination__doc__, "setHotKeyCombination",
          "hotKeyID, vKey, flags=0",
          "hotKeyID:XPLMHotKeyID, vKey:int, flags:XPLMKeyFlags=NoFlag",
          "None",
          "Update key combination for given hotKeyID to use vKey and flags");

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
  {"getAvionicsGeometry", (PyCFunction)XPLMGetAvionicsGeometryFun, METH_VARARGS | METH_KEYWORDS, _getAvionicsGeometry__doc__},
  {"XPLMGetAvionicsGeometry", (PyCFunction)XPLMGetAvionicsGeometryFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"setAvionicsGeometry", (PyCFunction)XPLMSetAvionicsGeometryFun, METH_VARARGS | METH_KEYWORDS, _setAvionicsGeometry__doc__},
  {"XPLMSetAvionicsGeometry", (PyCFunction)XPLMSetAvionicsGeometryFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"getAvionicsGeometryOS", (PyCFunction)XPLMGetAvionicsGeometryOSFun, METH_VARARGS | METH_KEYWORDS, _getAvionicsGeometryOS__doc__},
  {"XPLMGetAvionicsGeometryOS", (PyCFunction)XPLMGetAvionicsGeometryOSFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"setAvionicsGeometryOS", (PyCFunction)XPLMSetAvionicsGeometryOSFun, METH_VARARGS | METH_KEYWORDS, _setAvionicsGeometryOS__doc__},
  {"XPLMSetAvionicsGeometryOS", (PyCFunction)XPLMSetAvionicsGeometryOSFun, METH_VARARGS | METH_KEYWORDS, ""},
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
  // SDK 410
  {"getAvionicsHandle", (PyCFunction)XPLMGetAvionicsHandleFun, METH_VARARGS | METH_KEYWORDS, _getAvionicsHandle__doc__},
  {"XPLMGetAvionicsHandle", (PyCFunction)XPLMGetAvionicsHandleFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"isAvionicsBound", (PyCFunction)XPLMIsAvionicsBoundFun, METH_VARARGS | METH_KEYWORDS, _isAvionicsBound__doc__},
  {"XPLMIsAvionicsBound", (PyCFunction)XPLMIsAvionicsBoundFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"isCursorOverAvionics", (PyCFunction)XPLMIsCursorOverAvionicsFun, METH_VARARGS | METH_KEYWORDS, _isCursorOverAvionics__doc__},
  {"XPLMIsCursorOverAvionics", (PyCFunction)XPLMIsCursorOverAvionicsFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"isAvionicsPopupVisible", (PyCFunction)XPLMIsAvionicsPopupVisibleFun, METH_VARARGS | METH_KEYWORDS, _isAvionicsPopupVisible__doc__},
  {"XPLMIsAvionicsPopupVisible", (PyCFunction)XPLMIsAvionicsPopupVisibleFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"isAvionicsPoppedOut", (PyCFunction)XPLMIsAvionicsPoppedOutFun, METH_VARARGS | METH_KEYWORDS, _isAvionicsPoppedOut__doc__},
  {"XPLMIsAvionicsPoppedOut", (PyCFunction)XPLMIsAvionicsPoppedOutFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"hasAvionicsKeyboardFocus", (PyCFunction)XPLMHasAvionicsKeyboardFocusFun, METH_VARARGS | METH_KEYWORDS, _hasAvionicsKeyboardFocus__doc__},
  {"XPLMHasAvionicsKeyboardFocus", (PyCFunction)XPLMHasAvionicsKeyboardFocusFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"avionicsNeedsDrawing", (PyCFunction)XPLMAvionicsNeedsDrawingFun, METH_VARARGS | METH_KEYWORDS, _avionicsNeedsDrawing__doc__},
  {"XPLMAvionicsNeedsDrawing", (PyCFunction)XPLMAvionicsNeedsDrawingFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"setAvionicsPopupVisible", (PyCFunction)XPLMSetAvionicsPopupVisibleFun, METH_VARARGS | METH_KEYWORDS, _setAvionicsPopupVisible__doc__},
  {"XPLMSetAvionicsPopupVisible", (PyCFunction)XPLMSetAvionicsPopupVisibleFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"popOutAvionics", (PyCFunction)XPLMPopOutAvionicsFun, METH_VARARGS | METH_KEYWORDS, _popOutAvionics__doc__},
  {"XPLMPopOutAvionics", (PyCFunction)XPLMPopOutAvionicsFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"takeAvionicsKeyboardFocus", (PyCFunction)XPLMTakeAvionicsKeyboardFocusFun, METH_VARARGS | METH_KEYWORDS, _takeAvionicsKeyboardFocus__doc__},
  {"XPLMTakeAvionicsKeyboardFocus", (PyCFunction)XPLMTakeAvionicsKeyboardFocusFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"destroyAvionics", (PyCFunction)XPLMDestroyAvionicsFun, METH_VARARGS | METH_KEYWORDS, _destroyAvionics__doc__},
  {"XPLMDestroyAvionics", (PyCFunction)XPLMDestroyAvionicsFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"createAvionicsEx", (PyCFunction)XPLMCreateAvionicsExFun, METH_VARARGS | METH_KEYWORDS, _createAvionicsEx__doc__},
  {"XPLMCreateAvionicsEx", (PyCFunction)XPLMCreateAvionicsExFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"setAvionicsBrightnessRheo", (PyCFunction)XPLMSetAvionicsBrightnessRheoFun, METH_VARARGS | METH_KEYWORDS, _setAvionicsBrightnessRheo__doc__},
  {"XPLMSetAvionicsBrightnessRheo", (PyCFunction)XPLMSetAvionicsBrightnessRheoFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"getAvionicsBrightnessRheo", (PyCFunction)XPLMGetAvionicsBrightnessRheoFun, METH_VARARGS | METH_KEYWORDS, _getAvionicsBrightnessRheo__doc__},
  {"XPLMGetAvionicsBrightnessRheo", (PyCFunction)XPLMGetAvionicsBrightnessRheoFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"getAvionicsBusVoltsRatio", (PyCFunction)XPLMGetAvionicsBusVoltsRatioFun, METH_VARARGS | METH_KEYWORDS, _getAvionicsBusVoltsRatio__doc__},
  {"XPLMGetAvionicsBusVoltsRatio", (PyCFunction)XPLMGetAvionicsBusVoltsRatioFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"getDrawCallbackDict", (PyCFunction)buildDrawCallbackDict, METH_VARARGS, "Copy of internal DrawCallbackInfo"},
  {"getWindowCallbackDict", (PyCFunction)buildWindowCallbackDict, METH_VARARGS, "Copy of internal WindowCallbackInfo"},
  {"getAvionicsCallbackDict", (PyCFunction)buildAvionicsCallbackDict, METH_VARARGS, "Copy of internal AvionicsCallbackInfo"},
  {"getKeySnifferCallbackDict", (PyCFunction)buildKeySnifferCallbackDict, METH_VARARGS, "Copy of internal KeySnifferCallbackInfo"},
  {"getHotKeyCallbackDict", (PyCFunction)buildHotKeyCallbackDict, METH_VARARGS, "Copy of internal HotKeyCallbackInfo"},
  {"_cleanup", cleanup, METH_VARARGS, "cleanup"},
  {nullptr, nullptr, 0, nullptr}
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
  nullptr,
  nullptr,
  nullptr,
  nullptr
};

PyMODINIT_FUNC
PyInit_XPLMDisplay(void)
{
  PyObject *mod = PyModule_Create(&XPLMDisplayModule);
  if(mod){
    PyModule_AddStringConstant(mod, "__author__", "Peter Buckner (pbuck@xppython3.org)");
#if defined(XPLM_DEPRECATED)
    PyModule_AddIntConstant(mod, "xplm_Phase_FirstScene", xplm_Phase_FirstScene); //XPLMDrawingPhase
    PyModule_AddIntConstant(mod, "xplm_Phase_Terrain", xplm_Phase_Terrain); //XPLMDrawingPhase
    PyModule_AddIntConstant(mod, "xplm_Phase_Airports", xplm_Phase_Airports); //XPLMDrawingPhase
    PyModule_AddIntConstant(mod, "xplm_Phase_Vectors", xplm_Phase_Vectors); //XPLMDrawingPhase
    PyModule_AddIntConstant(mod, "xplm_Phase_Objects", xplm_Phase_Objects); //XPLMDrawingPhase
    PyModule_AddIntConstant(mod, "xplm_Phase_Airplanes", xplm_Phase_Airplanes); //XPLMDrawingPhase
    PyModule_AddIntConstant(mod, "xplm_Phase_LastScene", xplm_Phase_LastScene); //XPLMDrawingPhase
#endif /* XPLM_DEPRECATED */
#if defined(XPLM302)
    PyModule_AddIntConstant(mod, "xplm_Phase_Modern3D", xplm_Phase_Modern3D); //XPLMDrawingPhase
    PyModule_AddIntConstant(mod, "Phase_Modern3D", xplm_Phase_Modern3D); //XPLMDrawingPhase
#endif
    PyModule_AddIntConstant(mod, "xplm_Phase_FirstCockpit", xplm_Phase_FirstCockpit); //XPLMDrawingPhase
    PyModule_AddIntConstant(mod, "xplm_Phase_Panel", xplm_Phase_Panel); //XPLMDrawingPhase
    PyModule_AddIntConstant(mod, "xplm_Phase_Gauges", xplm_Phase_Gauges); //XPLMDrawingPhase
    PyModule_AddIntConstant(mod, "xplm_Phase_Window", xplm_Phase_Window); //XPLMDrawingPhase
    PyModule_AddIntConstant(mod, "xplm_Phase_LastCockpit", xplm_Phase_LastCockpit); //XPLMDrawingPhase

    PyModule_AddIntConstant(mod, "Phase_FirstCockpit", xplm_Phase_FirstCockpit); //XPLMDrawingPhase
    PyModule_AddIntConstant(mod, "Phase_Panel", xplm_Phase_Panel); //XPLMDrawingPhase
    PyModule_AddIntConstant(mod, "Phase_Gauges", xplm_Phase_Gauges); //XPLMDrawingPhase
    PyModule_AddIntConstant(mod, "Phase_Window", xplm_Phase_Window); //XPLMDrawingPhase
    PyModule_AddIntConstant(mod, "Phase_LastCockpit", xplm_Phase_LastCockpit); //XPLMDrawingPhase
#if defined(XPLM200)
    PyModule_AddIntConstant(mod, "xplm_Phase_LocalMap3D", xplm_Phase_LocalMap3D); //XPLMDrawingPhase
    PyModule_AddIntConstant(mod, "Phase_LocalMap3D", xplm_Phase_LocalMap3D); //XPLMDrawingPhase
#endif
#if defined(XPLM200)
    PyModule_AddIntConstant(mod, "xplm_Phase_LocalMap2D", xplm_Phase_LocalMap2D); //XPLMDrawingPhase
    PyModule_AddIntConstant(mod, "Phase_LocalMap2D", xplm_Phase_LocalMap2D); //XPLMDrawingPhase
#endif
#if defined(XPLM200)
    PyModule_AddIntConstant(mod, "xplm_Phase_LocalMapProfile", xplm_Phase_LocalMapProfile); //XPLMDrawingPhase
    PyModule_AddIntConstant(mod, "Phase_LocalMapProfile", xplm_Phase_LocalMapProfile); //XPLMDrawingPhase
#endif

    /* mouse status has been moved to defs.c in SDK410, but we'll keep it here, also, so older code continues to work */
    PyModule_AddIntConstant(mod, "xplm_MouseDown", xplm_MouseDown); // XPLMMouseStatus
    PyModule_AddIntConstant(mod, "xplm_MouseDrag", xplm_MouseDrag); // XPLMMouseStatus
    PyModule_AddIntConstant(mod, "xplm_MouseUp",   xplm_MouseUp); // XPLMMouseStatus

    PyModule_AddIntConstant(mod, "MouseDown", xplm_MouseDown); // XPLMMouseStatus
    PyModule_AddIntConstant(mod, "MouseDrag", xplm_MouseDrag); // XPLMMouseStatus
    PyModule_AddIntConstant(mod, "MouseUp",   xplm_MouseUp); // XPLMMouseStatus
    /* ^^^^^ */

    PyModule_AddIntConstant(mod, "xplm_CursorDefault", xplm_CursorDefault); //XPLMCursorStatus
    PyModule_AddIntConstant(mod, "xplm_CursorHidden",  xplm_CursorHidden); //XPLMCursorStatus
    PyModule_AddIntConstant(mod, "xplm_CursorArrow",   xplm_CursorArrow); //XPLMCursorStatus
    PyModule_AddIntConstant(mod, "xplm_CursorCustom",  xplm_CursorCustom); //XPLMCursorStatus

    PyModule_AddIntConstant(mod, "CursorDefault", xplm_CursorDefault); //XPLMCursorStatus
    PyModule_AddIntConstant(mod, "CursorHidden",  xplm_CursorHidden); //XPLMCursorStatus
    PyModule_AddIntConstant(mod, "CursorArrow",   xplm_CursorArrow); //XPLMCursorStatus
    PyModule_AddIntConstant(mod, "CursorCustom",  xplm_CursorCustom); //XPLMCursorStatus

    PyModule_AddIntConstant(mod, "xplm_WindowLayerFlightOverlay", xplm_WindowLayerFlightOverlay); //XPLMWindowLayer
    PyModule_AddIntConstant(mod, "xplm_WindowLayerFloatingWindows", xplm_WindowLayerFloatingWindows); //XPLMWindowLayer
    PyModule_AddIntConstant(mod, "xplm_WindowLayerModal", xplm_WindowLayerModal); //XPLMWindowLayer
    PyModule_AddIntConstant(mod, "xplm_WindowLayerGrowlNotifications", xplm_WindowLayerGrowlNotifications); //XPLMWindowLayer
    
    PyModule_AddIntConstant(mod, "WindowLayerFlightOverlay", xplm_WindowLayerFlightOverlay); //XPLMWindowLayer
    PyModule_AddIntConstant(mod, "WindowLayerFloatingWindows", xplm_WindowLayerFloatingWindows); //XPLMWindowLayer
    PyModule_AddIntConstant(mod, "WindowLayerModal", xplm_WindowLayerModal); //XPLMWindowLayer
    PyModule_AddIntConstant(mod, "WindowLayerGrowlNotifications", xplm_WindowLayerGrowlNotifications); //XPLMWindowLayer
    
    PyModule_AddIntConstant(mod, "xplm_WindowPositionFree", xplm_WindowPositionFree); // XPLMWindowPositioningMode
    PyModule_AddIntConstant(mod, "xplm_WindowCenterOnMonitor", xplm_WindowCenterOnMonitor); // XPLMWindowPositioningMode
    PyModule_AddIntConstant(mod, "xplm_WindowFullScreenOnMonitor", xplm_WindowFullScreenOnMonitor); // XPLMWindowPositioningMode
    PyModule_AddIntConstant(mod, "xplm_WindowFullScreenOnAllMonitors", xplm_WindowFullScreenOnAllMonitors); // XPLMWindowPositioningMode
    PyModule_AddIntConstant(mod, "xplm_WindowPopOut", xplm_WindowPopOut); // XPLMWindowPositioningMode
    PyModule_AddIntConstant(mod, "xplm_WindowVR", xplm_WindowVR); // XPLMWindowPositioningMode

    PyModule_AddIntConstant(mod, "WindowPositionFree", xplm_WindowPositionFree); // XPLMWindowPositioningMode
    PyModule_AddIntConstant(mod, "WindowCenterOnMonitor", xplm_WindowCenterOnMonitor); // XPLMWindowPositioningMode
    PyModule_AddIntConstant(mod, "WindowFullScreenOnMonitor", xplm_WindowFullScreenOnMonitor); // XPLMWindowPositioningMode
    PyModule_AddIntConstant(mod, "WindowFullScreenOnAllMonitors", xplm_WindowFullScreenOnAllMonitors); // XPLMWindowPositioningMode
    PyModule_AddIntConstant(mod, "WindowPopOut", xplm_WindowPopOut); // XPLMWindowPositioningMode
    PyModule_AddIntConstant(mod, "WindowVR", xplm_WindowVR); // XPLMWindowPositioningMode

    PyModule_AddIntConstant(mod, "xplm_WindowDecorationNone", xplm_WindowDecorationNone); // XPLMWindowDecoration
    PyModule_AddIntConstant(mod, "xplm_WindowDecorationRoundRectangle", xplm_WindowDecorationRoundRectangle); // XPLMWindowDecoration
    PyModule_AddIntConstant(mod, "xplm_WindowDecorationSelfDecorated", xplm_WindowDecorationSelfDecorated); // XPLMWindowDecoration
    PyModule_AddIntConstant(mod, "xplm_WindowDecorationSelfDecoratedResizable", xplm_WindowDecorationSelfDecoratedResizable); // XPLMWindowDecoration

    PyModule_AddIntConstant(mod, "WindowDecorationNone", xplm_WindowDecorationNone); // XPLMWindowDecoration
    PyModule_AddIntConstant(mod, "WindowDecorationRoundRectangle", xplm_WindowDecorationRoundRectangle); // XPLMWindowDecoration
    PyModule_AddIntConstant(mod, "WindowDecorationSelfDecorated", xplm_WindowDecorationSelfDecorated); // XPLMWindowDecoration
    PyModule_AddIntConstant(mod, "WindowDecorationSelfDecoratedResizable", xplm_WindowDecorationSelfDecoratedResizable); // XPLMWindowDecoration

#if defined(XPLM400)    
    PyModule_AddIntConstant(mod, "Device_GNS430_1", xplm_device_GNS430_1); // XPLMDeviceID
    PyModule_AddIntConstant(mod, "Device_GNS430_2", xplm_device_GNS430_2); // XPLMDeviceID
    PyModule_AddIntConstant(mod, "Device_GNS530_1", xplm_device_GNS530_1); // XPLMDeviceID
    PyModule_AddIntConstant(mod, "Device_GNS530_2", xplm_device_GNS530_2); // XPLMDeviceID
    PyModule_AddIntConstant(mod, "Device_CDU739_1", xplm_device_CDU739_1); // XPLMDeviceID
    PyModule_AddIntConstant(mod, "Device_CDU739_2", xplm_device_CDU739_2); // XPLMDeviceID
    PyModule_AddIntConstant(mod, "Device_G1000_PFD_1", xplm_device_G1000_PFD_1); // XPLMDeviceID
    PyModule_AddIntConstant(mod, "Device_G1000_PFD_2", xplm_device_G1000_PFD_2); // XPLMDeviceID
    PyModule_AddIntConstant(mod, "Device_G1000_MFD", xplm_device_G1000_MFD); // XPLMDeviceID
    PyModule_AddIntConstant(mod, "Device_CDU815_1", xplm_device_CDU815_1); // XPLMDeviceID
    PyModule_AddIntConstant(mod, "Device_CDU815_2", xplm_device_CDU815_2); // XPLMDeviceID
    PyModule_AddIntConstant(mod, "Device_Primus_PFD_1", xplm_device_Primus_PFD_1); // XPLMDeviceID
    PyModule_AddIntConstant(mod, "Device_Primus_PFD_2", xplm_device_Primus_PFD_2); // XPLMDeviceID
    PyModule_AddIntConstant(mod, "Device_Primus_MFD_1", xplm_device_Primus_MFD_1); // XPLMDeviceID
    PyModule_AddIntConstant(mod, "Device_Primus_MFD_2", xplm_device_Primus_MFD_2); // XPLMDeviceID
    PyModule_AddIntConstant(mod, "Device_Primus_MFD_3", xplm_device_Primus_MFD_3); // XPLMDeviceID
    PyModule_AddIntConstant(mod, "Device_Primus_RMU_1", xplm_device_Primus_RMU_1); // XPLMDeviceID
    PyModule_AddIntConstant(mod, "Device_Primus_RMU_2", xplm_device_Primus_RMU_2); // XPLMDeviceID
#else
    PyModule_AddIntConstant(mod, "Device_GNS430_1", -1); // XPLMDeviceID
    PyModule_AddIntConstant(mod, "Device_GNS430_2", -1); // XPLMDeviceID
    PyModule_AddIntConstant(mod, "Device_GNS530_1", -1); // XPLMDeviceID
    PyModule_AddIntConstant(mod, "Device_GNS530_2", -1); // XPLMDeviceID
    PyModule_AddIntConstant(mod, "Device_CDU739_1", -1); // XPLMDeviceID
    PyModule_AddIntConstant(mod, "Device_CDU739_2", -1); // XPLMDeviceID
    PyModule_AddIntConstant(mod, "Device_G1000_PFD_1", -1); // XPLMDeviceID
    PyModule_AddIntConstant(mod, "Device_G1000_PFD_2", -1); // XPLMDeviceID
    PyModule_AddIntConstant(mod, "Device_G1000_MFD", -1); // XPLMDeviceID
    PyModule_AddIntConstant(mod, "Device_CDU815_1", -1); // XPLMDeviceID
    PyModule_AddIntConstant(mod, "Device_CDU815_2", -1); // XPLMDeviceID
    PyModule_AddIntConstant(mod, "Device_Primus_PFD_1", -1); // XPLMDeviceID
    PyModule_AddIntConstant(mod, "Device_Primus_PFD_2", -1); // XPLMDeviceID
    PyModule_AddIntConstant(mod, "Device_Primus_MFD_1", -1); // XPLMDeviceID
    PyModule_AddIntConstant(mod, "Device_Primus_MFD_2", -1); // XPLMDeviceID
    PyModule_AddIntConstant(mod, "Device_Primus_MFD_3", -1); // XPLMDeviceID
    PyModule_AddIntConstant(mod, "Device_Primus_RMU_1", -1); // XPLMDeviceID
    PyModule_AddIntConstant(mod, "Device_Primus_RMU_2", -1); // XPLMDeviceID
#endif
#if defined(XPLM411)
    PyModule_AddIntConstant(mod, "Device_MCDU_1", xplm_device_MCDU_1); // XPLMDeviceID
    PyModule_AddIntConstant(mod, "Device_MCDU_2", xplm_device_MCDU_2); // XPLMDeviceID
#else
    PyModule_AddIntConstant(mod, "Device_MCDU_1", -1); // XPLMDeviceID
    PyModule_AddIntConstant(mod, "Device_MCDU_2", -1); // XPLMDeviceID
#endif
#if defined(XPLM430)
    PyModule_AddIntConstant(mod, "Device_MCDU_3", xplm_device_MCDU_3); // XPLMDeviceID
#else
    PyModule_AddIntConstant(mod, "Device_MCDU_3", -1); // XPLMDeviceID
#endif    
  }

  return mod;
}

static int genericDrawCallback(XPLMDrawingPhase inPhase, int inIsBefore, void *inRefcon)
{
  struct timespec all_stop, all_start;
  clock_gettime(CLOCK_MONOTONIC, &all_start);

  PyObject *pRes = nullptr;
  int res = 1;

  auto it = drawCallbackDict.find((intptr_t)inRefcon);
  if (it == drawCallbackDict.end()) {
    pythonLog("Unknown refcon passed to genericDrawCallback");
    return res;
  }

  DrawCallbackInfo& info = it->second;
  set_moduleName(info.module_name);
  PyObject *inPhaseObj = (inPhase >= 0 && inPhase < 8) ?
                        PHASE_OBJECTS[inPhase] : PyLong_FromLong(inPhase);
  PyObject *inIsBeforeObj = BOOL_OBJECTS[inIsBefore ? 1 : 0];

  if (info.callback != Py_None) {
    /* if the user's draw callback fails, for any reason, we report the error
     * and set _our_ info.callback function to None.
     * So, we will continue to have X-Plane call _this_ genericDrawcallback, but
     * we'll _not_ call the user callback
     */
    struct timespec stop, start;
    clock_gettime(CLOCK_MONOTONIC, &start);
    PyObject *args[] = {inPhaseObj, inIsBeforeObj, info.refCon};
    pRes = PyObject_Vectorcall(info.callback, args, 3, nullptr);
    clock_gettime(CLOCK_MONOTONIC, &stop);
    pluginStats[getPluginIndex()].draw_time += (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_nsec - start.tv_nsec) / 1000;
  }

  if (inPhase < 0 || inPhase >= 8) Py_DECREF(inPhaseObj);
  // No DECREF for inIsBeforeObj (borrowed reference)
  if(!pRes){
    PyObject *err = PyErr_Occurred();
    if (err && info.callback != Py_None) {
      // (only report if callback isn't None as otherwise we'll not stop reporting it..
      pythonLogException();
      pythonLog("[%s] Draw callback %s failed.", CurrentPythonModuleName, objToStr(info.callback));
      drawCallbackDict[(intptr_t)inRefcon].callback = Py_None;
    }
  } else if(PyLong_Check(pRes)){
    res = (int)PyLong_AsLong(pRes);
  } else {
    if (inIsBefore) {
      char *s2 = objToStr(info.callback);
      pythonLog("[%s] Draw callback %s returned a wrong type.", CurrentPythonModuleName, s2);
      free(s2);
    } else {
      res = 0; /* if not inIsBefore, X-Plane  ignores return value & so do we */
    }
  }

  Py_XDECREF(pRes);
  clock_gettime(CLOCK_MONOTONIC, &all_stop);
  pluginStats[0].draw_time += (all_stop.tv_sec - all_start.tv_sec) * 1000000 + (all_stop.tv_nsec - all_start.tv_nsec) / 1000;

  return res;
}

PyObject* buildDrawCallbackDict(void)
{
  PyObject *dict = PyDict_New();
  if (!dict) {
    return nullptr;
  }

  bool error_occurred = false;

  for (const auto& pair : drawCallbackDict) {
    const DrawCallbackInfo& info = pair.second;
    intptr_t refcon_id = pair.first;

    // Initialize all pointers to nullptr
    PyObject *key = nullptr;
    PyObject *module_name_p = nullptr;
    PyObject *phase = nullptr;
    PyObject *before = nullptr;
    PyObject *value = nullptr;

    // Create all Python objects
    key = PyLong_FromLong(refcon_id);
    if (!key) {
      error_occurred = true;
      goto cleanup_iteration;
    }

    module_name_p = PyUnicode_FromString(info.module_name);
    if (!module_name_p) {
      error_occurred = true;
      goto cleanup_iteration;
    }

    phase = PyLong_FromLong(info.phase);
    if (!phase) {
      error_occurred = true;
      goto cleanup_iteration;
    }

    before = PyLong_FromLong(info.before);
    if (!before) {
      error_occurred = true;
      goto cleanup_iteration;
    }

    // Build tuple: (module_name, callback, phase, before, refCon)
    value = PyTuple_New(5);
    if (!value) {
      error_occurred = true;
      goto cleanup_iteration;
    }

    // Set tuple items (PyTuple_SetItem steals references)
    PyTuple_SetItem(value, 0, module_name_p);         // steals ref
    module_name_p = nullptr; // Mark as stolen

    Py_INCREF(info.callback);                         // increment for tuple
    PyTuple_SetItem(value, 1, info.callback);         // steals ref

    PyTuple_SetItem(value, 2, phase);                 // steals ref
    phase = nullptr; // Mark as stolen

    PyTuple_SetItem(value, 3, before);                // steals ref
    before = nullptr; // Mark as stolen

    Py_INCREF(info.refCon);                           // increment for tuple
    PyTuple_SetItem(value, 4, info.refCon);           // steals ref

    // Add to dictionary (PyDict_SetItem does NOT steal references)
    if (PyDict_SetItem(dict, key, value) < 0) {
      error_occurred = true;
      goto cleanup_iteration;
    }

    // Clean up our references for this iteration
    Py_DECREF(key);
    Py_DECREF(value);
    continue;

cleanup_iteration:
    // Clean up any non-nullptr objects that weren't stolen
    if (key) Py_DECREF(key);
    if (module_name_p) Py_DECREF(module_name_p);
    if (phase) Py_DECREF(phase);
    if (before) Py_DECREF(before);
    if (value) Py_DECREF(value);
    break; // Exit the loop on error
  }

  if (error_occurred) {
    Py_DECREF(dict);
    return nullptr;
  }

  return dict;
}

PyObject* buildWindowCallbackDict(void)
{
  PyObject *dict = PyDict_New();
  if (!dict) {
    return nullptr;
  }

  bool error_occurred = false;

  for (const auto& pair : windowDict) {
    const WindowCallbackInfo& info = pair.second;
    XPLMWindowID window_id = pair.first;

    // Initialize all pointers to nullptr
    PyObject *key = nullptr;
    PyObject *window_id_capsule = nullptr;
    PyObject *module_name_p = nullptr;
    PyObject *value = nullptr;

    // Create all Python objects
    window_id_capsule = makeCapsule(window_id, "XPLMWindowID");
    if (!window_id_capsule) {
      error_occurred = true;
      goto cleanup_iteration;
    }

    key = window_id_capsule;
    Py_INCREF(key); // Since we're using it as both key and in tuple

    module_name_p = PyUnicode_FromString(info.module_name);
    if (!module_name_p) {
      error_occurred = true;
      goto cleanup_iteration;
    }

    // Build tuple: (module_name, draw, click, key, cursor, wheel, rightClick, refCon)
    value = PyTuple_New(8);
    if (!value) {
      error_occurred = true;
      goto cleanup_iteration;
    }

    // Set tuple items (PyTuple_SetItem steals references)
    PyTuple_SetItem(value, 0, module_name_p);           // steals ref
    module_name_p = nullptr; // Mark as stolen

    Py_INCREF(info.draw);                               // increment for tuple
    PyTuple_SetItem(value, 1, info.draw);               // steals ref

    Py_INCREF(info.click);                              // increment for tuple
    PyTuple_SetItem(value, 2, info.click);              // steals ref

    Py_INCREF(info.key);                                // increment for tuple
    PyTuple_SetItem(value, 3, info.key);                // steals ref

    Py_INCREF(info.cursor);                             // increment for tuple
    PyTuple_SetItem(value, 4, info.cursor);             // steals ref

    Py_INCREF(info.wheel);                              // increment for tuple
    PyTuple_SetItem(value, 5, info.wheel);              // steals ref

    Py_INCREF(info.rightClick);                         // increment for tuple
    PyTuple_SetItem(value, 6, info.rightClick);         // steals ref

    Py_INCREF(info.refCon);                             // increment for tuple
    PyTuple_SetItem(value, 7, info.refCon);             // steals ref

    // Add to dictionary (PyDict_SetItem does NOT steal references)
    if (PyDict_SetItem(dict, key, value) < 0) {
      error_occurred = true;
      goto cleanup_iteration;
    }

    // Clean up our references for this iteration
    Py_DECREF(key);
    Py_DECREF(value);
    continue;

cleanup_iteration:
    // Clean up any non-nullptr objects that weren't stolen
    if (key) Py_DECREF(key);
    if (module_name_p) Py_DECREF(module_name_p);
    if (value) Py_DECREF(value);
    break; // Exit the loop on error
  }

  if (error_occurred) {
    Py_DECREF(dict);
    return nullptr;
  }

  return dict;
}

PyObject* buildAvionicsCallbackDict(void)
{
  PyObject *dict = PyDict_New();
  if (!dict) {
    return nullptr;
  }

  for (const auto& pair : avionicsCallbacksDict) {
    const AvionicsCallbackInfo& info = pair.second;
    intptr_t refcon_id = pair.first;

    PyObject *key = PyLong_FromLong(refcon_id);
    if (!key) continue;

    PyObject *value = PyTuple_New(19);
    if (!value) {
      Py_DECREF(key);
      continue;
    }

    PyObject *module_name_p = PyUnicode_FromString(info.module_name);
    PyObject *device_id = PyLong_FromLong(info.deviceID);
    PyObject *avionics_id_capsule = makeCapsule(info.avionicsID, "XPLMAvionicsID");
    PyObject *create = PyLong_FromLong(info.create);

    PyTuple_SetItem(value, 0, module_name_p);
    PyTuple_SetItem(value, 1, device_id);
    PyTuple_SetItem(value, 2, avionics_id_capsule);

    Py_INCREF(info.before);
    PyTuple_SetItem(value, 3, info.before);
    Py_INCREF(info.after);
    PyTuple_SetItem(value, 4, info.after);
    Py_INCREF(info.refCon);
    PyTuple_SetItem(value, 5, info.refCon);
    Py_INCREF(info.bezel_draw);
    PyTuple_SetItem(value, 6, info.bezel_draw);
    Py_INCREF(info.draw);
    PyTuple_SetItem(value, 7, info.draw);
    Py_INCREF(info.bezel_click);
    PyTuple_SetItem(value, 8, info.bezel_click);
    Py_INCREF(info.bezel_rightclick);
    PyTuple_SetItem(value, 9, info.bezel_rightclick);
    Py_INCREF(info.bezel_scroll);
    PyTuple_SetItem(value, 10, info.bezel_scroll);
    Py_INCREF(info.bezel_cursor);
    PyTuple_SetItem(value, 11, info.bezel_cursor);
    Py_INCREF(info.screen_touch);
    PyTuple_SetItem(value, 12, info.screen_touch);
    Py_INCREF(info.screen_righttouch);
    PyTuple_SetItem(value, 13, info.screen_righttouch);
    Py_INCREF(info.screen_scroll);
    PyTuple_SetItem(value, 14, info.screen_scroll);
    Py_INCREF(info.screen_cursor);
    PyTuple_SetItem(value, 15, info.screen_cursor);
    Py_INCREF(info.keyboard);
    PyTuple_SetItem(value, 16, info.keyboard);
    Py_INCREF(info.brightness);
    PyTuple_SetItem(value, 17, info.brightness);

    PyTuple_SetItem(value, 18, create);

    PyDict_SetItem(dict, key, value);
    Py_DECREF(key);
    Py_DECREF(value);
  }

  return dict;
}

PyObject* buildKeySnifferCallbackDict(void)
{
  PyObject *dict = PyDict_New();
  if (!dict) {
    return nullptr;
  }

  for (const auto& pair : keySnifferCallbackDict) {
    const KeySnifferCallbackInfo& info = pair.second;
    intptr_t refcon_id = pair.first;

    PyObject *key = PyLong_FromLong(refcon_id);
    if (!key) continue;

    PyObject *value = PyTuple_New(4);
    if (!value) {
      Py_DECREF(key);
      continue;
    }

    PyObject *module_name_p = PyUnicode_FromString(info.module_name);
    PyObject *before = PyLong_FromLong(info.before);

    PyTuple_SetItem(value, 0, module_name_p);
    Py_INCREF(info.callback);
    PyTuple_SetItem(value, 1, info.callback);
    PyTuple_SetItem(value, 2, before);
    Py_INCREF(info.refCon);
    PyTuple_SetItem(value, 3, info.refCon);

    PyDict_SetItem(dict, key, value);
    Py_DECREF(key);
    Py_DECREF(value);
  }

  return dict;
}

PyObject* buildHotKeyCallbackDict(void)
{
  PyObject *dict = PyDict_New();
  if (!dict) {
    return nullptr;
  }

  for (const auto& pair : hotkeyDict) {
    const HotKeyCallbackInfo& info = pair.second;
    intptr_t refcon_id = pair.first;

    PyObject *key = PyLong_FromLong(refcon_id);
    if (!key) continue;

    PyObject *value = PyTuple_New(4);
    if (!value) {
      Py_DECREF(key);
      continue;
    }

    PyObject *hotkey_id_capsule = makeCapsule(info.hotKeyID, "XPLMHotKeyID");
    PyObject *module_name_p = PyUnicode_FromString(info.module_name);

    PyTuple_SetItem(value, 0, module_name_p);
    PyTuple_SetItem(value, 1, hotkey_id_capsule);
    PyTuple_SetItem(value, 2, info.callback);
    PyTuple_SetItem(value, 3, info.refCon);

    Py_INCREF(info.callback);
    Py_INCREF(info.refCon);

    PyDict_SetItem(dict, key, value);
    Py_DECREF(key);
    Py_DECREF(value);
  }

  return dict;
}
