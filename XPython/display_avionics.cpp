#include <Python.h>
#include <string>
#include <unordered_map>
#include <XPLM/XPLMDisplay.h>
#include "display_avionics.h"
#include "plugin_dl.h"
#include "utils.h"
#include "xppython.h"
#include "capsules.h"
#include <stdio.h>

std::unordered_map<intptr_t, AvionicsCallbackInfo> avionicsCallbacksDict;
static intptr_t avionicsCallbacksCntr;

#define AVIONICS_MODULE_NAME 0
#define AVIONICS_DEVICE 1
#define AVIONICS_BEFORE 2      // only used for customize
#define AVIONICS_AFTER  3      // only used for customize
#define AVIONICS_REFCON 4
#define AVIONICS_BEZEL_DRAW 5          // create
#define AVIONICS_DRAW 6                // create
#define AVIONICS_BEZEL_CLICK 7         // create
#define AVIONICS_BEZEL_RIGHTCLICK 8    // create
#define AVIONICS_BEZEL_SCROLL 9        // create
#define AVIONICS_BEZEL_CURSOR 10       // create
#define AVIONICS_SCREEN_TOUCH 11       // create
#define AVIONICS_SCREEN_RIGHTTOUCH 12  // create
#define AVIONICS_SCREEN_SCROLL 13      // create
#define AVIONICS_SCREEN_CURSOR 14      // create
#define AVIONICS_KEYBOARD 15           // create
#define AVIONICS_BRIGHTNESS 16         // create
#define AVIONICS_CREATE 17             // both... if this item is CREATED we'll need to DESTROY. Otherwise it's REGISTERED.

static int genericAvionicsCallback(XPLMDeviceID inDeviceID, int inIsBefore, void *inRefcon);
static void genericAvionicsBezelDraw(float inAmbiantR, float inAmbiantG, float inAmbiantB,  void *inRefcon);
static void genericAvionicsScreenDraw(void *inRefcon);
static int genericAvionicsBezelClick(int x, int y, XPLMMouseStatus inMouse, void *inRefcon);
static int genericAvionicsBezelRightClick(int x, int y, XPLMMouseStatus inMouse, void *inRefcon);
static int genericAvionicsBezelScroll(int x, int y, int wheel, int clicks, void *inRefcon);
static XPLMCursorStatus genericAvionicsBezelCursor(int x, int y, void *inRefcon);
static int genericAvionicsScreenTouch(int x, int y, int inMouse, void *inRefcon);
static int genericAvionicsScreenRightTouch(int x, int y, int inMouse, void *inRefcon);
static int genericAvionicsScreenScroll(int x, int y, int wheel, int clicks, void *inRefcon);
static XPLMCursorStatus genericAvionicsScreenCursor(int x, int y, void *inRefcon);
static int genericAvionicsKeyboard(char inKey, XPLMKeyFlags inFlags, char inVirtualKey, void *inRefcon, int losingFocus);
static float genericAvionicsBrightness(float inRheoValue, float inAmbiantBrightness, float inBusVoltsRatio, void *inRefcon);

static int genericAvionicsCallback(XPLMDeviceID inDeviceID, int inIsBefore, void *inRefcon)
{
  struct timespec all_stop, all_start;
  clock_gettime(CLOCK_MONOTONIC, &all_start);
  (void) inDeviceID;
  intptr_t refcon_id = (intptr_t)inRefcon;
  PyObject *err = nullptr;
  PyObject *pRes = nullptr;
  PyObject *fun = nullptr;
  PyObject *refCon = nullptr;
  PyObject *deviceId = nullptr;
  PyObject *isBefore = nullptr;
  int res = 0;

  auto it = avionicsCallbacksDict.find(refcon_id);
  if (it == avionicsCallbacksDict.end()) {
    pythonLog("avionicsDrawCallback, got unknown inRefcon(%p)!", inRefcon);
    return res;
  }

  AvionicsCallbackInfo& info = it->second;

  fun = inIsBefore ? info.before : info.after;
  refCon = info.refCon;
  deviceId = PyLong_FromLong(info.deviceID);  /* new -- and should match inDeviceID, which we ignore */
  set_moduleName(info.module_name);
  isBefore = PyLong_FromLong(inIsBefore);/* new */
  struct timespec stop, start;
  clock_gettime(CLOCK_MONOTONIC, &start);
  if (fun != Py_None) {
    PyObject *args[] = {deviceId, isBefore, refCon};
    pRes = PyObject_Vectorcall(fun, args, 3, nullptr);
  }
  clock_gettime(CLOCK_MONOTONIC, &stop);
  pluginStats[getPluginIndex()].draw_time += (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_nsec - start.tv_nsec) / 1000;
  Py_DECREF(isBefore);
  Py_DECREF(deviceId);

  if(!pRes){
    err = PyErr_Occurred();
    if (err && fun != Py_None) {
      pythonLogException();
      pythonLog("[%s] Avionics Draw function disabled. %s", CurrentPythonModuleName, objToStr(fun));
      if (inIsBefore) {
        info.before = Py_None;
      } else {
        info.after = Py_None;
      }
    }
  } else if (inIsBefore) {
    if(!PyLong_Check(pRes)){
      /* _before_ callbacks should return 1 or 0  -- _after_ callback returns are ignored */
      char *s2 = objToStr(fun);
      pythonLog("[%s] Avionics Draw callback %s returned a wrong type ('before' callbacks must return int).", CurrentPythonModuleName, s2);
      free(s2);
    } else {
      res = (int)PyLong_AsLong(pRes);
    }
  }
                                   
  err = PyErr_Occurred();
  if(err){
    pythonLogException();
  }
  Py_XDECREF(pRes);

  clock_gettime(CLOCK_MONOTONIC, &all_stop);
  pluginStats[0].draw_time += (all_stop.tv_sec - all_start.tv_sec) * 1000000 + (all_stop.tv_nsec - all_start.tv_nsec) / 1000;

  return res;
}

static void genericAvionicsBezelDraw(float inAmbiantR, float inAmbiantG, float inAmbiantB, void *inRefcon) {
  struct timespec all_stop, all_start;
  clock_gettime(CLOCK_MONOTONIC, &all_start);
  auto it = avionicsCallbacksDict.find((intptr_t)inRefcon);
  if (it == avionicsCallbacksDict.end()) {
    pythonLog("avionicsBezelDraw, got unknown inRefcon(%p)!", inRefcon);
  } else {
    struct timespec stop, start;
    const AvionicsCallbackInfo& info = it->second;
    set_moduleName(info.module_name);
    if (info.bezel_draw != Py_None) {
      PyObject *ambiantR = PyFloat_FromDouble(inAmbiantR);
      PyObject *ambiantG = PyFloat_FromDouble(inAmbiantG);
      PyObject *ambiantB = PyFloat_FromDouble(inAmbiantB);
      clock_gettime(CLOCK_MONOTONIC, &start);
      PyObject *args[] = {ambiantR, ambiantG, ambiantB, info.refCon};
      PyObject_Vectorcall(info.bezel_draw, args, 4, nullptr);
      clock_gettime(CLOCK_MONOTONIC, &stop);
      Py_DECREF(ambiantR);
      Py_DECREF(ambiantG);
      Py_DECREF(ambiantB);
      pluginStats[getPluginIndex()].draw_time += (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_nsec - start.tv_nsec) / 1000;
    }
    
    PyObject *err = PyErr_Occurred();
    if(err && info.bezel_draw != Py_None){
      pythonLogException();
      pythonLog("[%s] Avionics Bezel Draw function disabled. %s", CurrentPythonModuleName, objToStr(info.bezel_draw));
      avionicsCallbacksDict[(intptr_t)inRefcon].bezel_draw = Py_None;
    }
  }
  clock_gettime(CLOCK_MONOTONIC, &all_stop);
  pluginStats[0].draw_time += (all_stop.tv_sec - all_start.tv_sec) * 1000000 + (all_stop.tv_nsec - all_start.tv_nsec) / 1000;
}

static void genericAvionicsScreenDraw(void *inRefcon)  {
  struct timespec all_stop, all_start;
  clock_gettime(CLOCK_MONOTONIC, &all_start);

  auto it = avionicsCallbacksDict.find((intptr_t)inRefcon);
  if (it == avionicsCallbacksDict.end()) {
    pythonLog("avionicsScreenDraw, got unknown inRefcon(%p)!", inRefcon);
    return;
  }

  AvionicsCallbackInfo& info  = it->second;
  set_moduleName(info.module_name);
  if (info.draw != Py_None) {
    struct timespec stop, start;
    clock_gettime(CLOCK_MONOTONIC, &start);
    PyObject *args[] = {info.refCon};
    PyObject_Vectorcall(info.draw, args, 1, nullptr);
    clock_gettime(CLOCK_MONOTONIC, &stop);
    pluginStats[getPluginIndex()].draw_time += (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_nsec - start.tv_nsec) / 1000;
  }
  
  PyObject *err = PyErr_Occurred();
  if(err && info.draw != Py_None){
    pythonLogException();
    pythonLog("[%s] ScreenDraw function disabled. %s", CurrentPythonModuleName, objToStr(info.draw));
    avionicsCallbacksDict[(intptr_t)inRefcon].draw = Py_None;
  }
  clock_gettime(CLOCK_MONOTONIC, &all_stop);
  pluginStats[0].draw_time += (all_stop.tv_sec - all_start.tv_sec) * 1000000 + (all_stop.tv_nsec - all_start.tv_nsec) / 1000;
}

static float genericAvionicsBrightness(float inRheoValue, float inAmbiantBrightness, float inBusVoltsRatio, void *inRefcon)  {
  struct timespec all_stop, all_start;
  clock_gettime(CLOCK_MONOTONIC, &all_start);
  PyObject *pRes = nullptr;
  float res = inRheoValue * inAmbiantBrightness; /* ... the default behavior */

  auto it = avionicsCallbacksDict.find((intptr_t)inRefcon);
  if (it == avionicsCallbacksDict.end()) {
    pythonLog("avionicsBrightness, got unknown inRefcon(%p)!", inRefcon);
    return res;
  }

  AvionicsCallbackInfo& info = it->second;
  set_moduleName(info.module_name);
  if (info.brightness != Py_None) {
    struct timespec stop, start;
    clock_gettime(CLOCK_MONOTONIC, &start);
    PyObject * rheoValue = PyFloat_FromDouble(inRheoValue);
    PyObject * ambiantBrightness = PyFloat_FromDouble(inAmbiantBrightness);
    PyObject * busVoltsRatio = PyFloat_FromDouble(inBusVoltsRatio);
    PyObject *args[] = {rheoValue, ambiantBrightness, busVoltsRatio, info.refCon};
    pRes = PyObject_Vectorcall(info.brightness, args, 4, nullptr);
    clock_gettime(CLOCK_MONOTONIC, &stop);
    pluginStats[getPluginIndex()].draw_time += (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_nsec - start.tv_nsec) / 1000;
    Py_DECREF(rheoValue);
    Py_DECREF(ambiantBrightness);
    Py_DECREF(busVoltsRatio);
  }

  if(!pRes){
    PyObject *err = PyErr_Occurred();
    if (err && info.brightness != Py_None) {
      pythonLogException();
      pythonLog("[%s] Avionics Brightness callback %s failed.", CurrentPythonModuleName, objToStr(info.brightness));
      avionicsCallbacksDict[(intptr_t) inRefcon].brightness = Py_None;
    }
  } else if (PyFloat_Check(pRes)) {
    res = (float)PyFloat_AsDouble(pRes);
  } else {
    char *s2 = objToStr(info.brightness);
    pythonLog("[%s] Avionics Brightness callback %s returned a wrong type (callbacks must return float).", CurrentPythonModuleName, s2);
    free(s2);
  }

  Py_XDECREF(pRes);
  clock_gettime(CLOCK_MONOTONIC, &all_stop);
  pluginStats[0].draw_time += (all_stop.tv_sec - all_start.tv_sec) * 1000000 + (all_stop.tv_nsec - all_start.tv_nsec) / 1000;
  return res;
}

static int genericAvionicsBezelClick(int x, int y, XPLMMouseStatus inMouse, void *inRefcon) {
  struct timespec all_stop, all_start;
  clock_gettime(CLOCK_MONOTONIC, &all_start);
  PyObject *pRes = nullptr;
  int res = 0;
  
  auto it = avionicsCallbacksDict.find((intptr_t)inRefcon);
  if(it == avionicsCallbacksDict.end()) {
    pythonLog("avionicsBezelClick, got unknown inRefcon(%p)!", inRefcon);
    return res;
  }

  AvionicsCallbackInfo& info = it->second;
  set_moduleName(info.module_name);
  
  if (info.bezel_click != Py_None) {
    PyObject* x_obj = PyLong_FromLong(x);
    PyObject* y_obj = PyLong_FromLong(y);
    PyObject* mouse_obj = PyLong_FromLong(inMouse);
    
    struct timespec stop, start;
    clock_gettime(CLOCK_MONOTONIC, &start);
    PyObject *args[] = {x_obj, y_obj, mouse_obj, info.refCon};
    pRes = PyObject_Vectorcall(info.bezel_click, args, 4, nullptr);
    clock_gettime(CLOCK_MONOTONIC, &stop);
    pluginStats[getPluginIndex()].draw_time += (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_nsec - start.tv_nsec) / 1000;
    Py_DECREF(x_obj);
    Py_DECREF(y_obj);
    Py_DECREF(mouse_obj);
  }
  
  if(!pRes) {
    PyObject *err = PyErr_Occurred();
    if (err && info.bezel_click != Py_None) {
      pythonLogException();
      char *s2 = objToStr(info.bezel_click);
      pythonLog("[%s] Avionics Bezel Click callback %s failed.", CurrentPythonModuleName, s2);
      free(s2);
      avionicsCallbacksDict[(intptr_t) inRefcon].bezel_click = Py_None;
    }
  } else if(PyLong_Check(pRes)){
    res = (int)PyLong_AsLong(pRes);
  } else {
    char *s2 = objToStr(info.bezel_click);
    pythonLog("[%s] Avionics Bezel Click callback %s returned a wrong type (callbacks must return int).", CurrentPythonModuleName, s2);
    free(s2);
  }
                                   
  Py_XDECREF(pRes);

  clock_gettime(CLOCK_MONOTONIC, &all_stop);
  pluginStats[0].draw_time += (all_stop.tv_sec - all_start.tv_sec) * 1000000 + (all_stop.tv_nsec - all_start.tv_nsec) / 1000;

  return res;
}

static int genericAvionicsBezelRightClick(int x, int y, XPLMMouseStatus inMouse, void *inRefcon) {
  struct timespec all_stop, all_start;
  clock_gettime(CLOCK_MONOTONIC, &all_start);
  int res = 0;
  
  auto it = avionicsCallbacksDict.find((intptr_t)inRefcon);
  if(it == avionicsCallbacksDict.end()) {
    pythonLog("avionicsBezelRightclick, got unknown inRefcon(%p)!", inRefcon);
    return res;
  }
  AvionicsCallbackInfo& info = it->second;
  set_moduleName(info.module_name);
  
  PyObject *pRes = nullptr;
  if (info.bezel_rightclick != Py_None) {
    PyObject* x_obj = PyLong_FromLong(x);
    PyObject* y_obj = PyLong_FromLong(y);
    PyObject* mouse_obj = PyLong_FromLong(inMouse);
    struct timespec stop, start;
    clock_gettime(CLOCK_MONOTONIC, &start);
    PyObject *args[] = {x_obj, y_obj, mouse_obj, info.refCon};
    pRes = PyObject_Vectorcall(info.bezel_rightclick, args, 4, nullptr);
    clock_gettime(CLOCK_MONOTONIC, &stop);
    pluginStats[getPluginIndex()].draw_time += (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_nsec - start.tv_nsec) / 1000;
    Py_DECREF(x_obj);
    Py_DECREF(y_obj);
    Py_DECREF(mouse_obj);
  }
  
  if(!pRes) {
    PyObject *err = PyErr_Occurred();
    if (err && info.bezel_rightclick != Py_None) {
      pythonLogException();
      char *s2 = objToStr(info.bezel_rightclick);
      pythonLog("[%s] Avionics Bezel Rightclick callback %s failed.", CurrentPythonModuleName, s2);
      free(s2);
      avionicsCallbacksDict[(intptr_t) inRefcon].bezel_rightclick = Py_None;
    }
  } else if(PyLong_Check(pRes)){
    res = (int)PyLong_AsLong(pRes);
  } else {
    char *s2 = objToStr(info.bezel_rightclick);
    pythonLog("[%s] Avionics Bezel Rightclick callback %s returned a wrong type (callbacks must return int).", CurrentPythonModuleName, s2);
    free(s2);
  }                                   
  Py_XDECREF(pRes);

  clock_gettime(CLOCK_MONOTONIC, &all_stop);
  pluginStats[0].draw_time += (all_stop.tv_sec - all_start.tv_sec) * 1000000 + (all_stop.tv_nsec - all_start.tv_nsec) / 1000;

  return res;
}

static int genericAvionicsBezelScroll(int x, int y, int wheel, int clicks, void *inRefcon) {
  struct timespec all_stop, all_start;
  clock_gettime(CLOCK_MONOTONIC, &all_start);
  int res = 0;

  auto it = avionicsCallbacksDict.find((intptr_t) inRefcon);
  if (it == avionicsCallbacksDict.end()) {
    pythonLog("avionicsBezelScroll, got unknown inRefcon(%p)!", inRefcon);
    return res;
  }
  PyObject *pRes = nullptr;
  AvionicsCallbackInfo& info = it->second;
  set_moduleName(info.module_name);

  if (info.bezel_scroll != Py_None) {
    PyObject* x_obj = PyLong_FromLong(x);
    PyObject* y_obj = PyLong_FromLong(y);
    PyObject* wheel_obj = PyLong_FromLong(wheel);
    PyObject* click_obj = PyLong_FromLong(clicks);
    struct timespec stop, start;
    clock_gettime(CLOCK_MONOTONIC, &start);
    PyObject *args[] = {x_obj, y_obj, wheel_obj, click_obj, info.refCon};
    pRes = PyObject_Vectorcall(info.bezel_scroll, args, 5, nullptr);
    clock_gettime(CLOCK_MONOTONIC, &stop);
    pluginStats[getPluginIndex()].draw_time += (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_nsec - start.tv_nsec) / 1000;
    Py_DECREF(x_obj);
    Py_DECREF(y_obj);
    Py_DECREF(wheel_obj);
    Py_DECREF(click_obj);
  }

  if(!pRes){
    PyObject *err = PyErr_Occurred();
    if (err && info.bezel_scroll != Py_None) {
      pythonLogException();
      char *s2 = objToStr(info.bezel_scroll);
      pythonLog("[%s] Avionics Bezel Scroll callback %s failed.", CurrentPythonModuleName, s2);
      free(s2);
      avionicsCallbacksDict[(intptr_t)inRefcon].bezel_scroll = Py_None;
    }
  } else if(PyLong_Check(pRes)){
    res = (int)PyLong_AsLong(pRes);
  } else {
    char *s2 = objToStr(info.bezel_scroll);
    pythonLog("[%s] Avionics Bezel Scroll callback %s returned a wrong type (callbacks must return int).", CurrentPythonModuleName, s2);
    free(s2);
  }
                                   
  Py_XDECREF(pRes);

  clock_gettime(CLOCK_MONOTONIC, &all_stop);
  pluginStats[0].draw_time += (all_stop.tv_sec - all_start.tv_sec) * 1000000 + (all_stop.tv_nsec - all_start.tv_nsec) / 1000;

  return res;
}

static XPLMCursorStatus genericAvionicsBezelCursor(int x, int y, void *inRefcon) {
  struct timespec all_stop, all_start;
  clock_gettime(CLOCK_MONOTONIC, &all_start);
  PyObject *pRes = nullptr;
  XPLMCursorStatus res = 0;

  auto it = avionicsCallbacksDict.find((intptr_t)inRefcon);
  if (it == avionicsCallbacksDict.end()) {
    pythonLog("avionicsBezelCursor, got unknown inRefcon(%p)!", inRefcon);
    return res;
  }
  AvionicsCallbackInfo& info = it->second;
  set_moduleName(info.module_name);

  if (info.bezel_cursor != Py_None) {
    PyObject* x_obj = PyLong_FromLong(x);
    PyObject* y_obj = PyLong_FromLong(y);
    struct timespec stop, start;
    clock_gettime(CLOCK_MONOTONIC, &start);
    PyObject *args[] = {x_obj, y_obj, info.refCon};
    pRes = PyObject_Vectorcall(info.bezel_cursor, args, 3, nullptr);
    clock_gettime(CLOCK_MONOTONIC, &stop);
    pluginStats[getPluginIndex()].draw_time += (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_nsec - start.tv_nsec) / 1000;
    Py_DECREF(x_obj);
    Py_DECREF(y_obj);
  }

  if(!pRes){
    PyObject *err = PyErr_Occurred();
    if (err && info.bezel_cursor != Py_None) {
      pythonLogException();
      char *s2 = objToStr(info.bezel_cursor);
      pythonLog("[%s] Avionics Bezel Cursor callback %s failed.", CurrentPythonModuleName, s2);
      free(s2);
      avionicsCallbacksDict[(intptr_t)inRefcon].bezel_cursor = Py_None;
    }
  } else if(PyLong_Check(pRes)){
    res = (XPLMCursorStatus)PyLong_AsLong(pRes);
  } else {
    char *s2 = objToStr(info.bezel_cursor);
    pythonLog("[%s] Avionics Bezel Cursor callback %s returned a wrong type (callbacks must return int).", CurrentPythonModuleName, s2);
    free(s2);
  }
                                   
  Py_XDECREF(pRes);

  clock_gettime(CLOCK_MONOTONIC, &all_stop);
  pluginStats[0].draw_time += (all_stop.tv_sec - all_start.tv_sec) * 1000000 + (all_stop.tv_nsec - all_start.tv_nsec) / 1000;

  return res;
}

static int genericAvionicsScreenTouch(int x, int y, int inMouse, void *inRefcon) {
  int res = 0;
  struct timespec all_stop, all_start;
  clock_gettime(CLOCK_MONOTONIC, &all_start);

  auto it = avionicsCallbacksDict.find((intptr_t)inRefcon);
  if (it == avionicsCallbacksDict.end()) {
    pythonLog("avionicsScreenTouch, got unknown inRefcon(%p)!", inRefcon);
    return res;
  }
  PyObject *pRes = nullptr;
  AvionicsCallbackInfo& info = it->second;
  set_moduleName(info.module_name);

  if (info.screen_touch != Py_None) {
    struct timespec stop, start;
    clock_gettime(CLOCK_MONOTONIC, &start);
    PyObject* x_obj = PyLong_FromLong(x);
    PyObject* y_obj = PyLong_FromLong(y);
    PyObject* mouse_obj = PyLong_FromLong(inMouse);
    PyObject *args[] = {x_obj, y_obj, mouse_obj, info.refCon};
    pRes = PyObject_Vectorcall(info.screen_touch, args, 4, nullptr);
    clock_gettime(CLOCK_MONOTONIC, &stop);
    pluginStats[getPluginIndex()].draw_time += (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_nsec - start.tv_nsec) / 1000;
    Py_DECREF(x_obj);
    Py_DECREF(y_obj);
    Py_DECREF(mouse_obj);
  }

  if(!pRes){
    PyObject *err = PyErr_Occurred();
    if (err && info.screen_touch != Py_None) {
      pythonLogException();
      char *s2 = objToStr(info.screen_touch);
      pythonLog("[%s] Avionics Screen Touch callback %s failed.", CurrentPythonModuleName, s2);
      free(s2);
      avionicsCallbacksDict[(intptr_t)inRefcon].screen_touch = Py_None;
    }
  } else if(PyLong_Check(pRes)){
    res = (int)PyLong_AsLong(pRes);
  } else {
    char *s2 = objToStr(info.screen_touch);
    pythonLog("[%s] Avionics Screen Touch callback %s returned a wrong type (callbacks must return int).", CurrentPythonModuleName, s2);
    free(s2);
  }
                                   
  Py_XDECREF(pRes);

  clock_gettime(CLOCK_MONOTONIC, &all_stop);
  pluginStats[0].draw_time += (all_stop.tv_sec - all_start.tv_sec) * 1000000 + (all_stop.tv_nsec - all_start.tv_nsec) / 1000;

  return res;
}

static int genericAvionicsScreenRightTouch(int x, int y, int inMouse, void *inRefcon) {
  struct timespec all_stop, all_start;
  clock_gettime(CLOCK_MONOTONIC, &all_start);
  int res = 0;

  auto it = avionicsCallbacksDict.find((intptr_t) inRefcon);
  if (it == avionicsCallbacksDict.end()) {
    pythonLog("avionicsScreenRightTouch, got unknown inRefcon(%p)!", inRefcon);
    return res;
  }
  PyObject *pRes = nullptr;
  AvionicsCallbackInfo& info = it->second;
  set_moduleName(info.module_name);

  if (info.screen_righttouch != Py_None) {
    PyObject* x_obj = PyLong_FromLong(x);
    PyObject* y_obj = PyLong_FromLong(y);
    PyObject* mouse_obj = PyLong_FromLong(inMouse);
    struct timespec stop, start;
    clock_gettime(CLOCK_MONOTONIC, &start);
    PyObject *args[] = {x_obj, y_obj, mouse_obj, info.refCon};
    pRes = PyObject_Vectorcall(info.screen_righttouch, args, 4, nullptr);
    clock_gettime(CLOCK_MONOTONIC, &stop);
    pluginStats[getPluginIndex()].draw_time += (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_nsec - start.tv_nsec) / 1000;
    Py_DECREF(x_obj);
    Py_DECREF(y_obj);
    Py_DECREF(mouse_obj);
  }

  if(!pRes){
    PyObject *err = PyErr_Occurred();
    if (err && info.screen_righttouch != Py_None) {
      pythonLogException();
      char *s2 = objToStr(info.screen_righttouch);
      pythonLog("[%s] Avionics Screen Right Touch callback %s failed.", CurrentPythonModuleName, s2);
      free(s2);
      avionicsCallbacksDict[(intptr_t)inRefcon].screen_righttouch = Py_None;
    }
  } else if(PyLong_Check(pRes)){
    res = (int)PyLong_AsLong(pRes);
  } else {
    char *s2 = objToStr(info.screen_righttouch);
    pythonLog("[%s] Avionics Screen Right Touch callback %s returned a wrong type (callbacks must return int).", CurrentPythonModuleName, s2);
    free(s2);
  }
                                   
  Py_XDECREF(pRes);

  clock_gettime(CLOCK_MONOTONIC, &all_stop);
  pluginStats[0].draw_time += (all_stop.tv_sec - all_start.tv_sec) * 1000000 + (all_stop.tv_nsec - all_start.tv_nsec) / 1000;

  return res;
}

static int genericAvionicsScreenScroll(int x, int y, int wheel, int clicks, void *inRefcon) {
  struct timespec all_stop, all_start;
  clock_gettime(CLOCK_MONOTONIC, &all_start);
  int res = 0;

  auto it = avionicsCallbacksDict.find((intptr_t) inRefcon);
  if(it == avionicsCallbacksDict.end()) {
    pythonLog("avionicsScreenScroll, got unknown inRefcon(%p)!", inRefcon);
    return res;
  }

  PyObject *pRes = nullptr;
  AvionicsCallbackInfo& info = it->second;
  set_moduleName(info.module_name);

  if(info.screen_scroll != Py_None) {
    PyObject* x_obj = PyLong_FromLong(x);
    PyObject* y_obj = PyLong_FromLong(y);
    PyObject* wheel_obj = PyLong_FromLong(wheel);
    PyObject* clicks_obj = PyLong_FromLong(clicks);
    struct timespec stop, start;
    clock_gettime(CLOCK_MONOTONIC, &start);
    PyObject *args[] = {x_obj, y_obj, wheel_obj, clicks_obj, info.refCon};
    pRes = PyObject_Vectorcall(info.screen_scroll, args, 5, nullptr);
    clock_gettime(CLOCK_MONOTONIC, &stop);
    pluginStats[getPluginIndex()].draw_time += (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_nsec - start.tv_nsec) / 1000;
    Py_DECREF(x_obj);
    Py_DECREF(y_obj);
    Py_DECREF(wheel_obj);
    Py_DECREF(clicks_obj);
  }

  if(!pRes){
    PyObject *err = PyErr_Occurred();
    if(err && info.screen_scroll != Py_None) {
      pythonLogException();
      char *s2 = objToStr(info.screen_scroll);
      pythonLog("[%s] Avionics Screen Scroll callback %s failed.", CurrentPythonModuleName, s2);
      free(s2);
      avionicsCallbacksDict[(intptr_t)inRefcon].screen_scroll = Py_None;
    }
  } else if(PyLong_Check(pRes)){
    res = (int)PyLong_AsLong(pRes);
  } else {
    char *s2 = objToStr(info.screen_scroll);
    pythonLog("[%s] Avionics Screen Scroll callback %s returned a wrong type (callbacks must return int).", CurrentPythonModuleName, s2);
    free(s2);
  }
                                   
  Py_XDECREF(pRes);

  clock_gettime(CLOCK_MONOTONIC, &all_stop);
  pluginStats[0].draw_time += (all_stop.tv_sec - all_start.tv_sec) * 1000000 + (all_stop.tv_nsec - all_start.tv_nsec) / 1000;

  return res;
}

static XPLMCursorStatus genericAvionicsScreenCursor(int x, int y, void *inRefcon) {
  struct timespec all_stop, all_start;
  clock_gettime(CLOCK_MONOTONIC, &all_start);
  XPLMCursorStatus res = xplm_CursorDefault;

  auto it = avionicsCallbacksDict.find((intptr_t) inRefcon);
  if(it == avionicsCallbacksDict.end()) {
    pythonLog("avionicsScreenCursor, got unknown inRefcon(%p)!", inRefcon);
    return res;
  }

  AvionicsCallbackInfo& info = it->second;

  PyObject *pRes = nullptr;
  set_moduleName(info.module_name);

  if (info.screen_cursor != Py_None) {
    PyObject* x_obj = PyLong_FromLong(x);
    PyObject* y_obj = PyLong_FromLong(y);
    struct timespec stop, start;
    clock_gettime(CLOCK_MONOTONIC, &start);
    PyObject *args[] = {x_obj, y_obj, info.refCon};
    pRes = PyObject_Vectorcall(info.screen_cursor, args, 3, nullptr);
    clock_gettime(CLOCK_MONOTONIC, &stop);
    pluginStats[getPluginIndex()].draw_time += (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_nsec - start.tv_nsec) / 1000;
    Py_DECREF(x_obj);
    Py_DECREF(y_obj);
  } 

  if(!pRes){
    PyObject *err = PyErr_Occurred();
    if(err && info.screen_cursor != Py_None) {
      pythonLogException();
      char *s2 = objToStr(info.screen_cursor);
      pythonLog("[%s] Avionics Screen Cursor callback %s failed.", CurrentPythonModuleName, s2);
      free(s2);
      avionicsCallbacksDict[(intptr_t) inRefcon].screen_cursor = Py_None;
    }
  } else if(PyLong_Check(pRes)){
    res = (XPLMCursorStatus)PyLong_AsLong(pRes);
  } else {
    char *s2 = objToStr(info.screen_cursor);
    pythonLog("[%s] Avionics Screen Cursor callback %s returned a wrong type (callbacks must return int).", CurrentPythonModuleName, s2);
    free(s2);
  }
                                   
  Py_XDECREF(pRes);

  clock_gettime(CLOCK_MONOTONIC, &all_stop);
  pluginStats[0].draw_time += (all_stop.tv_sec - all_start.tv_sec) * 1000000 + (all_stop.tv_nsec - all_start.tv_nsec) / 1000;
  return res;
}

static int genericAvionicsKeyboard(char inKey, XPLMKeyFlags inFlags, char inVirtualKey, void *inRefcon, int losingFocus) {
  struct timespec all_stop, all_start;
  clock_gettime(CLOCK_MONOTONIC, &all_start);
  PyObject *pRes = nullptr;
  int res = 1;

  auto it = avionicsCallbacksDict.find((intptr_t) inRefcon);
  if (it == avionicsCallbacksDict.end()) {
    pythonLog("avionicsKeyboard, got unknown inRefcon(%p)!", inRefcon);
    return res;
  }

  AvionicsCallbackInfo& info = it->second;
  set_moduleName(info.module_name);
  
  if (info.keyboard != Py_None) {
    struct timespec stop, start;
    clock_gettime(CLOCK_MONOTONIC, &start);
    PyObject *key_obj = PyLong_FromLong(inKey);
    PyObject *flags_obj = PyLong_FromLong(inFlags);
    PyObject *vkey_obj = PyLong_FromLong(inVirtualKey);
    PyObject *focus_obj = PyLong_FromLong(losingFocus);
    PyObject *args[] = {key_obj, flags_obj, vkey_obj, info.refCon, focus_obj};
    pRes = PyObject_Vectorcall(info.keyboard, args, 5, nullptr);
    clock_gettime(CLOCK_MONOTONIC, &stop);
    pluginStats[getPluginIndex()].draw_time += (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_nsec - start.tv_nsec) / 1000;
    Py_DECREF(key_obj);
    Py_DECREF(flags_obj);
    Py_DECREF(vkey_obj);
    Py_DECREF(focus_obj);
  }

  if(!pRes){
    PyObject *err = PyErr_Occurred();
    if (err) {
      char *s2 = objToStr(info.keyboard);
      pythonLog("[%s] Avionics Keyboard callback %s failed.", CurrentPythonModuleName, s2);
      free(s2);
      avionicsCallbacksDict[(intptr_t)inRefcon].keyboard = Py_None;
    }
  } else if(PyLong_Check(pRes)){
    res = (int)PyLong_AsLong(pRes);
  } else {
    char *s2 = objToStr(info.keyboard);
    pythonLog("[%s] Avionics Keyboard callback %s returned a wrong type (callbacks must return int).", CurrentPythonModuleName, s2);
    free(s2);
  }

  Py_XDECREF(pRes);

  clock_gettime(CLOCK_MONOTONIC, &all_stop);
  pluginStats[0].draw_time += (all_stop.tv_sec - all_start.tv_sec) * 1000000 + (all_stop.tv_nsec - all_start.tv_nsec) / 1000;
  return res;
}

PyObject *XPLMRegisterAvionicsCallbacksExFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  (void) self;
  static char *keywords[] = {CHAR("deviceId"), CHAR("before"), CHAR("after"), CHAR("refCon"),
                             CHAR("bezelClick"), CHAR("bezelRightClick"), CHAR("bezelScroll"), CHAR("bezelCursor"),
                             CHAR("screenTouch"), CHAR("screenRightTouch"), CHAR("screenScroll"), CHAR("screenCursor"),
                             CHAR("keyboard"), nullptr};
  int deviceId=0;

  PyObject *firstObj=Py_None, *paramsObj = Py_None;

  PyObject
    *before=Py_None,
    *after=Py_None,
    *screenDraw=Py_None,
    *bezelDraw=Py_None,
    *bezelClick=Py_None,
    *bezelRightClick=Py_None,
    *bezelScroll=Py_None,
    *bezelCursor=Py_None,
    *screenTouch=Py_None,
    *screenRightTouch=Py_None,
    *screenScroll=Py_None,
    *screenCursor=Py_None,
    *keyboard=Py_None,
    *refcon=Py_None;

  if(!XPLMRegisterAvionicsCallbacksEx_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMRegisterAvionicsCallbacksEx is available only in XPLM400 and up.");
    return nullptr;
  }

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|OOOOOOOOOOOOO", keywords,
                                  &firstObj, &before, &after, &refcon,
                                  &bezelClick, &bezelRightClick, &bezelScroll, &bezelCursor,
                                  &screenTouch, &screenRightTouch, &screenScroll, &screenCursor,
                                  &keyboard)){
    return nullptr;
  }

  /* if (beforeCallback == Py_None && afterCallback == Py_None) { */
  /*   PyErr_SetString(PyExc_RuntimeError ,"XPLMRegisterAviationCallbacksEx: Both before and after callbacks cannot be None."); */
  /*   return nullptr; */
  /* } */

  if (firstObj == Py_None) {
    /* deviceID is _required_, so if firstObj isn't specified, this is simply an error */
    return nullptr;
  }

  XPLMCustomizeAvionics_t avionics_params;
  avionics_params.structSize = sizeof(avionics_params);

  if (PyLong_Check(firstObj)) {
    deviceId = PyLong_AsLong(firstObj);
    avionics_params.deviceId = deviceId;
  } else if (PySequence_Check(firstObj)) {
    paramsObj = firstObj;
    if (PySequence_Length(paramsObj) != 13) {
      PyErr_SetString(PyExc_AttributeError ,"registerAvionicsEx tuple did not contain 13 values\n.");
      return nullptr;
    }
    PyObject *paramsTuple = PySequence_Tuple(paramsObj);
    // when paramsObj is not None, I know it's a sequence with 13 elements, due to checks above
    avionics_params.deviceId = PyLong_AsLong(PyTuple_GetItem(paramsTuple, 0));
    before = PyTuple_GetItem(paramsTuple, 1);
    after = PyTuple_GetItem(paramsTuple, 2);
    refcon = PyTuple_GetItem(paramsTuple, 3);
    bezelClick = PyTuple_GetItem(paramsTuple, 4);
    bezelRightClick = PyTuple_GetItem(paramsTuple, 5);
    bezelScroll = PyTuple_GetItem(paramsTuple, 6);
    bezelCursor = PyTuple_GetItem(paramsTuple, 7);
    screenTouch = PyTuple_GetItem(paramsTuple, 8);
    screenRightTouch = PyTuple_GetItem(paramsTuple, 9);
    screenScroll = PyTuple_GetItem(paramsTuple, 10);
    screenCursor = PyTuple_GetItem(paramsTuple, 11);
    keyboard = PyTuple_GetItem(paramsTuple, 12);
    Py_DECREF(paramsTuple);
  } else {
    PyErr_SetString(PyExc_AttributeError ,"registerAvionicsEx could not parse arguments.\n");
    return nullptr;
  }

  avionics_params.refcon = (void *)++avionicsCallbacksCntr;
  avionics_params.drawCallbackBefore = before != Py_None ? genericAvionicsCallback : nullptr;
  avionics_params.drawCallbackAfter = after != Py_None ? genericAvionicsCallback : nullptr;
  avionics_params.bezelClickCallback = bezelClick != Py_None ? genericAvionicsBezelClick : nullptr;
  avionics_params.bezelRightClickCallback = bezelRightClick != Py_None ? genericAvionicsBezelRightClick : nullptr;
  avionics_params.bezelScrollCallback = bezelScroll != Py_None ? genericAvionicsBezelScroll : nullptr;
  avionics_params.bezelCursorCallback = bezelCursor != Py_None ? genericAvionicsBezelCursor : nullptr;
  avionics_params.screenTouchCallback = screenTouch != Py_None ? genericAvionicsScreenTouch : nullptr;
  avionics_params.screenRightTouchCallback = screenRightTouch != Py_None ? genericAvionicsScreenRightTouch : nullptr;
  avionics_params.screenScrollCallback = screenScroll != Py_None ? genericAvionicsScreenScroll : nullptr;
  avionics_params.screenCursorCallback = screenCursor != Py_None ? genericAvionicsScreenCursor : nullptr;
  avionics_params.keyboardCallback = keyboard != Py_None ? genericAvionicsKeyboard : nullptr;

  PyObject *callbackList[] = {before, after, bezelDraw, screenDraw, bezelClick, bezelRightClick, bezelScroll,
                              bezelCursor, screenTouch, screenRightTouch, screenScroll, screenCursor, keyboard};
  for (int i=0; i< 13; i++) {
    if (callbackList[i] != Py_None) {
      if (PyCallable_Check(callbackList[i])) {
        Py_INCREF(callbackList[i]);
      } else {
        PyErr_SetString(PyExc_ValueError ,"registerAvionicsEx callback is not callable.\n");
        return nullptr;
      }
    }
  }

  Py_INCREF(refcon);
  
  XPLMAvionicsID avionicsId = XPLMRegisterAvionicsCallbacksEx_ptr(&avionics_params);

  avionicsCallbacksDict[(intptr_t)avionics_params.refcon] = {
    .module_name = CurrentPythonModuleName,
    .deviceID = deviceId,
    .avionicsID = avionicsId,
    .before = before,
    .after = after,
    .refCon = refcon,
    .bezel_draw = bezelDraw,
    .draw = screenDraw,
    .bezel_click = bezelClick,
    .bezel_rightclick = bezelRightClick,
    .bezel_scroll = bezelScroll,
    .bezel_cursor = bezelCursor,
    .screen_touch = screenTouch,
    .screen_righttouch = screenRightTouch,
    .screen_scroll = screenScroll,
    .screen_cursor = screenCursor,
    .keyboard = keyboard,
    .brightness = Py_None,
    .create = 0
  };

  PyObject *avIDCapsule = makeCapsule(avionicsId, "XPLMAvionicsID");
  if(!avIDCapsule){
    PyErr_SetString(PyExc_RuntimeError ,"XPLMRegisterAvionicsCallbacksEx failed.\n");
    return nullptr;
  }

  return avIDCapsule;
}
PyObject *XPLMUnregisterAvionicsCallbacksFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("avionicsId"), nullptr};
  (void) self;
  PyObject *avIDCapsule;

  if(!XPLMUnregisterAvionicsCallbacks_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMUnregisterAvionicsCallbacks is available only in XPLM400 and up.");
    return nullptr;
  }

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &avIDCapsule)) {
    return nullptr;
  }
  if (avIDCapsule == nullptr) {
    PyErr_SetString(PyExc_RuntimeError, "XPLMUnregisterAvionicsCallback bad avionicsID\n");
    return nullptr;
  }
  XPLMAvionicsID avionicsId = getVoidPtr(avIDCapsule, "XPLMAvionicsID");

  int found = 0;
  for (auto it = avionicsCallbacksDict.begin(); it != avionicsCallbacksDict.end(); ){
    AvionicsCallbackInfo info = it->second;
    if (info.avionicsID == avionicsId) {
      if (info.create != 0) {
        pythonLog("Error: trying to unregister a Custom avionics device. Use destroyAvionics() instead.");
        ++it;
        continue;
      }
      found = 1;
      XPLMUnregisterAvionicsCallbacks_ptr(avionicsId);
      Py_DECREF(info.before);
      Py_DECREF(info.after);
      Py_DECREF(info.bezel_draw);
      Py_DECREF(info.draw);
      Py_DECREF(info.bezel_click);
      Py_DECREF(info.bezel_rightclick);
      Py_DECREF(info.bezel_scroll);
      Py_DECREF(info.bezel_cursor);
      Py_DECREF(info.screen_touch);
      Py_DECREF(info.screen_righttouch);
      Py_DECREF(info.screen_scroll);
      Py_DECREF(info.screen_cursor);
      Py_DECREF(info.keyboard);
      Py_DECREF(info.brightness);
      Py_DECREF(avIDCapsule);
      deleteCapsule(avIDCapsule);
      it = avionicsCallbacksDict.erase(it);
      break;
    } else {
      ++it;
    }
  }
  if (found == 0 ) {
    PyErr_SetString(PyExc_RuntimeError, "XPLMUnregisterAvionicsCallback failed to find avionicsID\n");
    return nullptr;
  }
  Py_RETURN_NONE;
}
PyObject *XPLMGetAvionicsHandleFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("deviceID"), nullptr};
  (void) self;
  int deviceID;
  if (!XPLMGetAvionicsHandle_ptr) {
    PyErr_SetString(PyExc_RuntimeError , "XPLMGetAvionicsHandle is available only in XPLM410 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "i", keywords, &deviceID)) {
    return nullptr;
  }

  PyObject *avDictsKey = PyLong_FromLong(++avionicsCallbacksCntr);
  if(!avDictsKey){
    PyErr_SetString(PyExc_RuntimeError ,"Couldn't create long.\n");
    return nullptr;
  }

  XPLMAvionicsID avionicsId = XPLMGetAvionicsHandle_ptr(deviceID);
  if (avionicsId == nullptr) {
    PyErr_SetString(PyExc_RuntimeError ,"XPLMGetAvionicsHandle() returned nullptr. (bug filed)\n");
    return nullptr;
  }    

  PyObject *avIDCapsule = makeCapsule(avionicsId, "XPLMAvionicsID");
  if(!avIDCapsule){
    PyErr_SetString(PyExc_RuntimeError ,"XPLMGetAvionicsHandle failed.\n");
    return nullptr;
  }

  avionicsCallbacksDict[avionicsCallbacksCntr] = {
    .module_name = CurrentPythonModuleName,
    .deviceID = deviceID,
    .avionicsID = avionicsId,
    .before = Py_None,
    .after = Py_None,
    .refCon = Py_None,
    .bezel_draw = Py_None,
    .draw = Py_None,
    .bezel_click = Py_None,
    .bezel_rightclick = Py_None,
    .bezel_scroll = Py_None,
    .bezel_cursor = Py_None,
    .screen_touch = Py_None,
    .screen_righttouch = Py_None,
    .screen_scroll = Py_None,
    .screen_cursor = Py_None,
    .keyboard = Py_None,
    .brightness = Py_None,
    .create = 0
  };

  Py_DECREF(avDictsKey);
  return avIDCapsule;
}

PyObject *XPLMIsAvionicsBoundFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("avionicsID"), nullptr};
  (void) self;
  PyObject *avionicsID;
  if (!XPLMIsAvionicsBound_ptr) {
    PyErr_SetString(PyExc_RuntimeError , "XPLMIsAvionicsBound is available only in XPLM410 and up.");
    return nullptr;
  }

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &avionicsID)) {
    return nullptr;
  }
  void *avionics_id = getVoidPtr(avionicsID, "XPLMAvionicsID");
  int ret = XPLMIsAvionicsBound_ptr(avionics_id);
  return PyLong_FromLong(ret);
}

PyObject *XPLMIsCursorOverAvionicsFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("avionicsID"), nullptr};
  (void) self;
  PyObject *avionicsID;
  if (!XPLMIsCursorOverAvionics_ptr) {
    PyErr_SetString(PyExc_RuntimeError , "XPLMIsCursorOverAvionics is available only in XPLM410 and up.");
    return nullptr;
  }

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &avionicsID)) {
    return nullptr;
  }
  void *avionics_id = getVoidPtr(avionicsID, "XPLMAvionicsID");
  int x=0, y=0;
  int ret = XPLMIsCursorOverAvionics_ptr(avionics_id, &x, &y);
  if (!ret) {
    Py_RETURN_NONE;
  }

  PyObject *tuple = PyTuple_New(2);
  PyTuple_SetItem(tuple, 0, PyLong_FromLong(x));
  PyTuple_SetItem(tuple, 1, PyLong_FromLong(y));
  Py_INCREF(tuple);
  return tuple;
}
PyObject *XPLMIsAvionicsPopupVisibleFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("avionicsID"), nullptr};
  (void) self;
  PyObject *avionicsID;
  if (!XPLMIsAvionicsPopupVisible_ptr) {
    PyErr_SetString(PyExc_RuntimeError , "XPLMIsAvionicsPopupVisible is available only in XPLM410 and up.");
    return nullptr;
  }

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &avionicsID)) {
    return nullptr;
  }
  void *avionics_id = getVoidPtr(avionicsID, "XPLMAvionicsID");
  return PyLong_FromLong(XPLMIsAvionicsPopupVisible_ptr(avionics_id));
}

PyObject *XPLMIsAvionicsPoppedOutFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("avionicsID"), nullptr};
  (void) self;
  PyObject *avionicsID;
  if (!XPLMIsAvionicsPoppedOut_ptr) {
    PyErr_SetString(PyExc_RuntimeError , "XPLMIsAvionicsPoppedOut is available only in XPLM410 and up.");
    return nullptr;
  }

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &avionicsID)) {
    return nullptr;
  }
  void *avionics_id = getVoidPtr(avionicsID, "XPLMAvionicsID");
  return PyLong_FromLong(XPLMIsAvionicsPoppedOut_ptr(avionics_id));
}
PyObject *XPLMHasAvionicsKeyboardFocusFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("avionicsID"), nullptr};
  (void) self;
  PyObject *avionicsID;
  if (!XPLMHasAvionicsKeyboardFocus_ptr) {
    PyErr_SetString(PyExc_RuntimeError , "XPLMHasAvionicsKeyboardFocus is available only in XPLM410 and up.");
    return nullptr;
  }

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &avionicsID)) {
    return nullptr;
  }
  void *avionics_id = getVoidPtr(avionicsID, "XPLMAvionicsID");
  return PyLong_FromLong(XPLMHasAvionicsKeyboardFocus_ptr(avionics_id));
}
PyObject *XPLMAvionicsNeedsDrawingFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("avionicsID"), nullptr};
  (void) self;
  PyObject *avionicsID;
  if (!XPLMAvionicsNeedsDrawing_ptr) {
    PyErr_SetString(PyExc_RuntimeError , "XPLMAvionicsNeedsDrawing is available only in XPLM410 and up.");
    return nullptr;
  }

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &avionicsID)) {
    return nullptr;
  }
  void *avionics_id = getVoidPtr(avionicsID, "XPLMAvionicsID");
  XPLMAvionicsNeedsDrawing_ptr(avionics_id);
  Py_RETURN_NONE;
}
PyObject *XPLMPopOutAvionicsFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("avionicsID"), nullptr};
  (void) self;
  PyObject *avionicsID;
  if (!XPLMPopOutAvionics_ptr) {
    PyErr_SetString(PyExc_RuntimeError , "XPLMPopOutAvionics is available only in XPLM410 and up.");
    return nullptr;
  }

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &avionicsID)) {
    return nullptr;
  }
  void *avionics_id = getVoidPtr(avionicsID, "XPLMAvionicsID");
  XPLMPopOutAvionics_ptr(avionics_id);
  Py_RETURN_NONE;
}
PyObject *XPLMTakeAvionicsKeyboardFocusFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("avionicsID"), nullptr};
  (void) self;
  PyObject *avionicsID;
  if (!XPLMTakeAvionicsKeyboardFocus_ptr) {
    PyErr_SetString(PyExc_RuntimeError , "XPLMTakeAvionicsKeyboardFocus is available only in XPLM410 and up.");
    return nullptr;
  }

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &avionicsID)) {
    return nullptr;
  }
  void *avionics_id = getVoidPtr(avionicsID, "XPLMAvionicsID");
  XPLMTakeAvionicsKeyboardFocus_ptr(avionics_id);
  Py_RETURN_NONE;
}
PyObject *XPLMDestroyAvionicsFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("avionicsID"), nullptr};
  (void) self;
  PyObject *avIDCapsule;
  if (!XPLMDestroyAvionics_ptr) {
    PyErr_SetString(PyExc_RuntimeError , "XPLMDestroyAvionics is available only in XPLM410 and up.");
    return nullptr;
  }

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &avIDCapsule)) {
    return nullptr;
  }
  XPLMAvionicsID avionicsId = getVoidPtr(avIDCapsule, "XPLMAvionicsID");
  
  int found = 0;
  for (auto it = avionicsCallbacksDict.begin(); it != avionicsCallbacksDict.end(); ){
    AvionicsCallbackInfo info = it->second;
    if (info.avionicsID == avionicsId) {
      if (info.create != 1) {
        pythonLog("Error: trying to destroy a built-in avionics device. Use unregisterAvionicsCallbacks() instead.");
        ++it;
        continue;
      }
      found = 1;
      XPLMDestroyAvionics_ptr(avionicsId);
      Py_DECREF(info.before);
      Py_DECREF(info.after);
      Py_DECREF(info.bezel_draw);
      Py_DECREF(info.draw);
      Py_DECREF(info.bezel_click);
      Py_DECREF(info.bezel_rightclick);
      Py_DECREF(info.bezel_scroll);
      Py_DECREF(info.bezel_cursor);
      Py_DECREF(info.screen_touch);
      Py_DECREF(info.screen_righttouch);
      Py_DECREF(info.screen_scroll);
      Py_DECREF(info.screen_cursor);
      Py_DECREF(info.keyboard);
      Py_DECREF(info.brightness);
      Py_DECREF(avIDCapsule);
      deleteCapsule(avIDCapsule);
      it = avionicsCallbacksDict.erase(it);
      break;
    } else {
      ++it;
    }
  }
  if (found == 0 ) {
    PyErr_SetString(PyExc_RuntimeError, "XPLMDestroyAvionics failed to find avionicsID\n");
    return nullptr;
  }
  Py_RETURN_NONE;
}
PyObject *XPLMGetAvionicsBusVoltsRatioFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("avionicsID"), nullptr};
  (void) self;
  PyObject *avionicsID;
  if (!XPLMGetAvionicsBusVoltsRatio_ptr) {
    PyErr_SetString(PyExc_RuntimeError, "XPLMGetAvionicsBusVoltsRatio is available only in XPLM410 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &avionicsID)) {
    return nullptr;
  }

  void *avionics_id = getVoidPtr(avionicsID, "XPLMAvionicsID");
  return PyFloat_FromDouble(XPLMGetAvionicsBusVoltsRatio_ptr(avionics_id));
}
PyObject *XPLMGetAvionicsBrightnessRheoFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("avionicsID"), nullptr};
  (void) self;
  PyObject *avionicsID;
  if (!XPLMGetAvionicsBrightnessRheo_ptr) {
    PyErr_SetString(PyExc_RuntimeError, "XPLMGetAvionicsBrightnessRheo is available only in XPLM410 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &avionicsID)) {
    return nullptr;
  }

  void *avionics_id = getVoidPtr(avionicsID, "XPLMAvionicsID");
  return PyFloat_FromDouble(XPLMGetAvionicsBrightnessRheo_ptr(avionics_id));
}
PyObject *XPLMSetAvionicsBrightnessRheoFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("avionicsID"), CHAR("brightness"), nullptr};
  (void) self;
  PyObject *avionicsID;
  float brightness=1.0;
  if (!XPLMSetAvionicsBrightnessRheo_ptr) {
    PyErr_SetString(PyExc_RuntimeError, "XPLMSetAvionicsBrightnessRheo is available only in XPLM410 and up.");
    return nullptr;
  }

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|f", keywords, &avionicsID, &brightness)) {
    return nullptr;
  }

  void *avionics_id = getVoidPtr(avionicsID, "XPLMAvionicsID");
  XPLMSetAvionicsBrightnessRheo_ptr(avionics_id, brightness);
  Py_RETURN_NONE;
}
PyObject *XPLMSetAvionicsPopupVisibleFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("avionicsID"), CHAR("visible"), nullptr};
  (void) self;
  PyObject *avionicsID;
  int visible=1;
  if (!XPLMSetAvionicsPopupVisible_ptr) {
    PyErr_SetString(PyExc_RuntimeError , "XPLMSetAvionicsPopupVisible is available only in XPLM410 and up.");
    return nullptr;
  }

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|i", keywords, &avionicsID, &visible)) {
    return nullptr;
  }
  void *avionics_id = getVoidPtr(avionicsID, "XPLMAvionicsID");
  XPLMSetAvionicsPopupVisible_ptr(avionics_id, visible);
  Py_RETURN_NONE;
}

PyObject *XPLMIsAvionicsMappedToVRFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("avionicsID"), nullptr};
  (void) self;
  PyObject *avionicsID;
  if (!XPLMIsAvionicsMappedToVR_ptr) {
    PyErr_SetString(PyExc_RuntimeError , "XPLMIsAvionicsMappedToVR is available only in XPLM440 and up.");
    return nullptr;
  }

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &avionicsID)) {
    return nullptr;
  }
  void *avionics_id = getVoidPtr(avionicsID, "XPLMAvionicsID");
  return PyLong_FromLong(XPLMIsAvionicsMappedToVR_ptr(avionics_id));
}

PyObject *XPLMSetAvionicsMappedToVRFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("avionicsID"), CHAR("mapped"), nullptr};
  (void) self;
  PyObject *avionicsID;
  int mapped=1;
  if (!XPLMSetAvionicsMappedToVR_ptr) {
    PyErr_SetString(PyExc_RuntimeError , "XPLMSetAvionicsMappedToVR is available only in XPLM440 and up.");
    return nullptr;
  }

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|i", keywords, &avionicsID, &mapped)) {
    return nullptr;
  }
  void *avionics_id = getVoidPtr(avionicsID, "XPLMAvionicsID");
  XPLMSetAvionicsMappedToVR_ptr(avionics_id, mapped);
  Py_RETURN_NONE;
}

#if defined(XPLM440)
/* Browser functions registered with XPLMAvionicsAddBrowserFunction. The X-Plane
   refcon is an incrementing counter used to look up the real Python callback + refcon.
   Kept separate from the window-side dict in display.cpp. */
struct AvionicsBrowserFunctionInfo {
  PyObject *callback;
  PyObject *refCon;
  const char* module_name;
};
std::unordered_map<intptr_t, AvionicsBrowserFunctionInfo> avionicsBrowserFunctionDict;
static intptr_t avionicsBrowserFunctionCntr;

/* browserLoadFinished/browserLoadError callbacks: like the generic avionics draw
   callbacks, inRefcon is the avionicsCallbacksDict counter, not a PyObject*. */
static void genericAvionicsBrowserLoadFinished(XPLMAvionicsID inAvionics, const char *inURL, void *inRefcon)
{
  // NOTE!!! sometimes this fires with url=about:blank, you must check the URL!!
  errCheck("prior genericAvionicsBrowserLoadFinished");
  auto it = avionicsCallbacksDict.find((intptr_t)inRefcon);
  if(it == avionicsCallbacksDict.end()){
    pythonLog("Unknown refcon passed to genericAvionicsBrowserLoadFinished (%p).", inRefcon);
    return;
  }
  AvionicsCallbackInfo& info = it->second;
  if (info.browserLoadFinished != Py_None) {
    PyObject *pID = makeCapsule(inAvionics, "XPLMAvionicsID");
    PyObject *pURL = inURL ? PyUnicode_FromString(inURL) : (Py_INCREF(Py_None), Py_None);
    set_moduleName(info.module_name);

    PyObject *args[] = {pID, pURL, info.refCon};
    PyObject *oRes = PyObject_Vectorcall(info.browserLoadFinished, args, 3, nullptr);
    Py_DECREF(pID);
    Py_DECREF(pURL);

    if(PyErr_Occurred()) {
      pythonLogException();
      pythonLog("disabling avionics browserLoadFinished function");
      info.browserLoadFinished = Py_None;
    }
    Py_XDECREF(oRes);
  }
  errCheck("end genericAvionicsBrowserLoadFinished");
}

static void genericAvionicsBrowserLoadError(XPLMAvionicsID inAvionics, const char *inURL, const char *inError, void *inRefcon)
{
  errCheck("prior genericAvionicsBrowserLoadError");
  auto it = avionicsCallbacksDict.find((intptr_t)inRefcon);
  if(it == avionicsCallbacksDict.end()){
    pythonLog("Unknown refcon passed to genericAvionicsBrowserLoadError (%p).", inRefcon);
    return;
  }
  AvionicsCallbackInfo& info = it->second;
  if (info.browserLoadError != Py_None) {
    PyObject *pID = makeCapsule(inAvionics, "XPLMAvionicsID");
    PyObject *pURL = inURL ? PyUnicode_FromString(inURL) : (Py_INCREF(Py_None), Py_None);
    PyObject *pErr = inError ? PyUnicode_FromString(inError) : (Py_INCREF(Py_None), Py_None);
    set_moduleName(info.module_name);

    PyObject *args[] = {pID, pURL, pErr, info.refCon};
    PyObject *oRes = PyObject_Vectorcall(info.browserLoadError, args, 4, nullptr);
    Py_DECREF(pID);
    Py_DECREF(pURL);
    Py_DECREF(pErr);

    if(PyErr_Occurred()) {
      pythonLogException();
      pythonLog("disabling avionics browserLoadError function");
      info.browserLoadError = Py_None;
    }
    Py_XDECREF(oRes);
  }
  errCheck("end genericAvionicsBrowserLoadError");
}

static const char *genericAvionicsBrowserCallback(XPLMAvionicsID inAvionics, const char *inJSON, void *inRefcon)
{
  errCheck("prior genericAvionicsBrowserCallback");
  /* The only sanctioned way to return a string is via XPLMReturnString, which copies
     into the host-managed slot. Return an empty string on any error. */
  const char *result = XPLMReturnString_ptr ? XPLMReturnString_ptr("") : "";
  auto it = avionicsBrowserFunctionDict.find((intptr_t)inRefcon);
  if(it == avionicsBrowserFunctionDict.end()){
    pythonLog("Unknown refcon passed to genericAvionicsBrowserCallback (%p).", inRefcon);
    return result;
  }
  PyObject *func = it->second.callback;
  PyObject *pID = makeCapsule(inAvionics, "XPLMAvionicsID");
  PyObject *pJSON = inJSON ? PyUnicode_FromString(inJSON) : (Py_INCREF(Py_None), Py_None);
  set_moduleName(it->second.module_name);

  PyObject *args[] = {pID, pJSON, it->second.refCon};
  PyObject *oRes = PyObject_Vectorcall(func, args, 3, nullptr);
  Py_DECREF(pID);
  Py_DECREF(pJSON);

  if(PyErr_Occurred()) {
    pythonLogException();
  } else if (oRes && oRes != Py_None) {
    PyObject *str = PyObject_Str(oRes);
    if (str) {
      const char *tmp = PyUnicode_AsUTF8(str);
      if (tmp && XPLMReturnString_ptr) {
        result = XPLMReturnString_ptr(tmp);    // copies tmp into host slot while str is alive
      }
      Py_DECREF(str);
    } else {
      PyErr_Clear();
    }
  }
  Py_XDECREF(oRes);
  errCheck("end genericAvionicsBrowserCallback");
  return result;
}
#endif /* XPLM440 */

PyObject *XPLMCreateAvionicsExFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  (void) self;
  static char *keywords[] = {CHAR("screenWidth"), CHAR("screenHeight"), CHAR("bezelWidth"), CHAR("bezelHeight"),
                             CHAR("screenOffsetX"), CHAR("screenOffsetY"), CHAR("drawOnDemand"),
                             CHAR("bezelDraw"), CHAR("screenDraw"),
                             CHAR("bezelClick"), CHAR("bezelRightClick"), CHAR("bezelScroll"), CHAR("bezelCursor"),
                             CHAR("screenTouch"), CHAR("screenRightTouch"), CHAR("screenScroll"), CHAR("screenCursor"),
                             CHAR("keyboard"), CHAR("brightness"),
                             CHAR("deviceID"), CHAR("deviceName"), CHAR("refCon"),
                             CHAR("contentType"), CHAR("windowWithChrome"),
                             CHAR("browserLoadFinished"), CHAR("browserLoadError"), nullptr};


  /*float brt(float rheo, float photo_cell, float bus_ratio) {
    "default" behavior.
    return photo_cell * rheo;
    }
  */

  int screenWidth=100;
  int screenHeight=200;
  int bezelWidth=140;
  int bezelHeight=250;
  int screenOffsetX=20;
  int screenOffsetY=25;
  int drawOnDemand=0;
  /* SDK440: how the device screen is drawn. Default to the legacy OpenGL bridge
     for back-compat; pass xp.WindowContentTypePanelGraphics to use panel-graphics
     calls (XPLMPolygon, SVT/map DrawIn, ...) from the screen/bezel callbacks. */
  int contentType=xplm_WindowContentTypeOpenGL;
  int windowWithChrome=0;

  PyObject *firstObj=Py_None, *paramsObj=Py_None;

  PyObject
    *screenDraw=Py_None,
    *bezelDraw=Py_None,
    *bezelClick=Py_None,
    *bezelRightClick=Py_None,
    *bezelScroll=Py_None,
    *bezelCursor=Py_None,
    *screenTouch=Py_None,
    *screenRightTouch=Py_None,
    *screenScroll=Py_None,
    *screenCursor=Py_None,
    *keyboard=Py_None,
    *brightness=Py_None,
    *browserLoadFinished=Py_None,
    *browserLoadError=Py_None,
    *refcon=Py_None;
  char *deviceIDstr = nullptr;
  char *deviceName = nullptr;

  if(!XPLMCreateAvionicsEx_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMCreateAvionicsEx() is available only in XPLM410 and up.");
    return nullptr;
  }

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|OiiiiiiOOOOOOOOOOOOssOiiOO", keywords,
                                  &firstObj, &screenHeight, &bezelWidth, &bezelHeight, &screenOffsetX, &screenOffsetY,
                                  &drawOnDemand,
                                  &bezelDraw, &screenDraw,
                                  &bezelClick, &bezelRightClick, &bezelScroll, &bezelCursor,
                                  &screenTouch, &screenRightTouch, &screenScroll, &screenCursor,
                                  &keyboard, &brightness,
                                  &deviceIDstr,
                                  &deviceName,
                                  &refcon,
                                  &contentType,
                                  &windowWithChrome,
                                  &browserLoadFinished,
                                  &browserLoadError
                                  )){
    return nullptr;
  }
  XPLMCreateAvionics_t avionics_params;
  avionics_params.structSize = sizeof(avionics_params);

  if (firstObj == Py_None || PyLong_Check(firstObj)) {
    screenWidth = firstObj == Py_None ? 100 : PyLong_AsLong(firstObj);
    avionics_params.screenWidth = screenWidth;
    avionics_params.screenHeight = screenHeight;
    avionics_params.bezelWidth = bezelWidth;
    avionics_params.bezelHeight = bezelHeight;
    avionics_params.screenOffsetX = screenOffsetX;
    avionics_params.screenOffsetY = screenOffsetY;
    avionics_params.drawOnDemand = drawOnDemand;
    if (deviceIDstr == nullptr) {
      asprintf(&deviceIDstr, "deviceID-%ld", (long)avionicsCallbacksCntr + 1);  // we ++ it below...
    }
    if (deviceName == nullptr) {
      asprintf(&deviceName, "deviceName-%ld", (long)avionicsCallbacksCntr + 1);  // we ++ it below...
    }
    avionics_params.deviceID = strdup(deviceIDstr);
    avionics_params.deviceName = strdup(deviceName);
  } else if (PySequence_Check(firstObj)) {
    paramsObj = firstObj;
    if (PySequence_Length(paramsObj) != 24) {
      PyErr_SetString(PyExc_AttributeError ,"createAvionicsEx tuple did not contain 24 values\n.");
      return nullptr;
    }
    PyObject *paramsTuple = PySequence_Tuple(paramsObj);
    avionics_params.screenWidth = PyLong_AsLong(PyTuple_GetItem(paramsTuple, 0));
    avionics_params.screenHeight = PyLong_AsLong(PyTuple_GetItem(paramsTuple, 1));
    avionics_params.bezelWidth = PyLong_AsLong(PyTuple_GetItem(paramsTuple, 2));
    avionics_params.bezelHeight = PyLong_AsLong(PyTuple_GetItem(paramsTuple, 3));
    avionics_params.screenOffsetX = PyLong_AsLong(PyTuple_GetItem(paramsTuple, 4));
    avionics_params.screenOffsetY = PyLong_AsLong(PyTuple_GetItem(paramsTuple, 5));
    avionics_params.drawOnDemand = PyLong_AsLong(PyTuple_GetItem(paramsTuple, 6));
    avionics_params.deviceID = objToStr(PyTuple_GetItem(paramsTuple, 19));
    avionics_params.deviceName = objToStr(PyTuple_GetItem(paramsTuple, 20));

    bezelDraw = PyTuple_GetItem(paramsTuple, 7);
    screenDraw = PyTuple_GetItem(paramsTuple, 8);
    bezelClick = PyTuple_GetItem(paramsTuple, 9);
    bezelRightClick = PyTuple_GetItem(paramsTuple, 10);
    bezelScroll = PyTuple_GetItem(paramsTuple, 11);
    bezelCursor = PyTuple_GetItem(paramsTuple, 12);
    screenTouch = PyTuple_GetItem(paramsTuple, 13);
    screenRightTouch = PyTuple_GetItem(paramsTuple, 14);
    screenScroll = PyTuple_GetItem(paramsTuple, 15);
    screenCursor = PyTuple_GetItem(paramsTuple, 16);
    keyboard = PyTuple_GetItem(paramsTuple, 17);
    brightness = PyTuple_GetItem(paramsTuple, 18);

    refcon = PyTuple_GetItem(paramsTuple, 21);
    browserLoadFinished = PyTuple_GetItem(paramsTuple, 22);
    browserLoadError = PyTuple_GetItem(paramsTuple, 23);
    Py_DECREF(paramsTuple);

  } else {
    PyErr_SetString(PyExc_AttributeError ,"createAvionicsEx could not parse arguments.\n");
    return nullptr;
  }

  if (avionics_params.bezelWidth < (avionics_params.screenWidth + avionics_params.screenOffsetX) || avionics_params.bezelHeight < (avionics_params.screenHeight + avionics_params.screenOffsetY)) {
    PyErr_SetString(PyExc_ValueError ,"createAvionicsEx() requires bezelWidth >= screenWidth + offsetX; and bezelHeight >= screenHeight + offsetY.\n");
    free((void *)avionics_params.deviceID);
    free((void *)avionics_params.deviceName);
    return nullptr;
  }

  avionics_params.refcon = (void *) ++avionicsCallbacksCntr;
#if defined(XPLM440)
  avionics_params.contentType = (XPLMWindowContentType) contentType;
  avionics_params.windowWithChrome = windowWithChrome;
  avionics_params.browserLoadFinishedFunc = browserLoadFinished != Py_None ? genericAvionicsBrowserLoadFinished : nullptr;
  avionics_params.browserLoadErrorFunc = browserLoadError != Py_None ? genericAvionicsBrowserLoadError : nullptr;
#else
  (void) contentType;
  (void) windowWithChrome;
#endif
  avionics_params.bezelDrawCallback = bezelDraw != Py_None ? genericAvionicsBezelDraw : nullptr;
  avionics_params.drawCallback = screenDraw != Py_None ? genericAvionicsScreenDraw : nullptr;
  avionics_params.bezelClickCallback = bezelClick != Py_None ? genericAvionicsBezelClick : nullptr;
  avionics_params.bezelRightClickCallback = bezelRightClick != Py_None ? genericAvionicsBezelRightClick : nullptr;
  avionics_params.bezelScrollCallback = bezelScroll != Py_None ? genericAvionicsBezelScroll : nullptr;
  avionics_params.bezelCursorCallback = bezelCursor != Py_None ? genericAvionicsBezelCursor : nullptr;
  avionics_params.screenTouchCallback = screenTouch != Py_None ? genericAvionicsScreenTouch : nullptr;
  avionics_params.screenRightTouchCallback = screenRightTouch != Py_None ? genericAvionicsScreenRightTouch : nullptr;
  avionics_params.screenScrollCallback = screenScroll != Py_None ? genericAvionicsScreenScroll : nullptr;
  avionics_params.screenCursorCallback = screenCursor != Py_None ? genericAvionicsScreenCursor : nullptr;
  avionics_params.keyboardCallback = keyboard != Py_None ? genericAvionicsKeyboard : nullptr;
  avionics_params.brightnessCallback = brightness != Py_None ? genericAvionicsBrightness : nullptr;

  /* check which are callable & Py_INCREF as appropriate */
  PyObject *callbackList[] = {bezelDraw, screenDraw, bezelClick, bezelRightClick, bezelScroll,
                              bezelCursor, screenTouch, screenRightTouch, screenScroll, screenCursor, keyboard, brightness,
                              browserLoadFinished, browserLoadError};
  for (int i=0; i< 14; i++) {
    if (callbackList[i] != Py_None) {
      if (PyCallable_Check(callbackList[i])) {
        Py_INCREF(callbackList[i]);
      } else {
        PyErr_SetString(PyExc_ValueError ,"createAvionicsEx callback is not callable.\n");
        free((void *)avionics_params.deviceID);
        free((void *)avionics_params.deviceName);
        return nullptr;
      }
    }
  }

  Py_INCREF(refcon);

  PyObject *avDictsKey = PyLong_FromLong(avionicsCallbacksCntr);
  if(!avDictsKey){
    PyErr_SetString(PyExc_RuntimeError ,"Couldn't create long.\n");
    free((void *)avionics_params.deviceID);
    free((void *)avionics_params.deviceName);
    return nullptr;
  }

  XPLMAvionicsID avionicsId = XPLMCreateAvionicsEx_ptr(&avionics_params);
  avionicsCallbacksDict[avionicsCallbacksCntr] = {
    .module_name = CurrentPythonModuleName,
    .deviceID = 0,
    .avionicsID = avionicsId,
    .before = Py_None,
    .after = Py_None,
    .refCon = refcon,
    .bezel_draw = bezelDraw,
    .draw = screenDraw,
    .bezel_click = bezelClick,
    .bezel_rightclick = bezelRightClick,
    .bezel_scroll = bezelScroll,
    .bezel_cursor = bezelCursor,
    .screen_touch = screenTouch,
    .screen_righttouch = screenRightTouch,
    .screen_scroll = screenScroll,
    .screen_cursor = screenCursor,
    .keyboard = keyboard,
    .brightness = brightness,
    .browserLoadFinished = browserLoadFinished,
    .browserLoadError = browserLoadError,
    .create = 1
  };

  PyObject *avIDCapsule = makeCapsule(avionicsId, "XPLMAvionicsID");
  if(!avIDCapsule){
    PyErr_SetString(PyExc_RuntimeError ,"XPLMRegisterAvionicsCallbacksEx failed.\n");
    free((void *)avionics_params.deviceID);
    free((void *)avionics_params.deviceName);
    return nullptr;
  }

  Py_DECREF(avDictsKey);
  errCheck("end createAvionicsEx");
  free((void *)avionics_params.deviceID);
  free((void *)avionics_params.deviceName);
  return avIDCapsule;
}

PyObject *XPLMAvionicsSetURLFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("avionicsID"), CHAR("url"), nullptr};
  (void) self;
  PyObject *avionicsID;
  const char *inURL;
  if(!XPLMAvionicsSetURL_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMAvionicsSetURL is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Os", keywords, &avionicsID, &inURL)){
    return nullptr;
  }
  void *avionics_id = getVoidPtr(avionicsID, "XPLMAvionicsID");
  XPLMAvionicsSetURL_ptr(avionics_id, inURL);
  Py_RETURN_NONE;
}

PyObject *XPLMAvionicsRefreshFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("avionicsID"), CHAR("ignoreCache"), nullptr};
  (void) self;
  PyObject *avionicsID;
  int inIgnoreCache = 0;
  if(!XPLMAvionicsRefresh_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMAvionicsRefresh is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|i", keywords, &avionicsID, &inIgnoreCache)){
    return nullptr;
  }
  void *avionics_id = getVoidPtr(avionicsID, "XPLMAvionicsID");
  XPLMAvionicsRefresh_ptr(avionics_id, inIgnoreCache);
  Py_RETURN_NONE;
}

PyObject *XPLMAvionicsInjectScriptFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("avionicsID"), CHAR("script"), nullptr};
  (void) self;
  PyObject *avionicsID;
  const char *inScript;
  if(!XPLMAvionicsInjectScript_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMAvionicsInjectScript is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Os", keywords, &avionicsID, &inScript)){
    return nullptr;
  }
  void *avionics_id = getVoidPtr(avionicsID, "XPLMAvionicsID");
  XPLMAvionicsInjectScript_ptr(avionics_id, inScript);
  Py_RETURN_NONE;
}

PyObject *XPLMAvionicsAddBrowserFunctionFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("avionicsID"), CHAR("name"), CHAR("function"), CHAR("refCon"), nullptr};
  (void) self;
  PyObject *avionicsID;
  const char *inName;
  PyObject *function;
  PyObject *refCon = Py_None;
  if(!XPLMAvionicsAddBrowserFunction_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMAvionicsAddBrowserFunction is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "OsO|O", keywords, &avionicsID, &inName, &function, &refCon)){
    return nullptr;
  }
  if(!PyCallable_Check(function)){
    PyErr_SetString(PyExc_ValueError ,"avionicsAddBrowserFunction function is not callable.\n");
    return nullptr;
  }
#if defined(XPLM440)
  void *avionics_id = getVoidPtr(avionicsID, "XPLMAvionicsID");
  intptr_t idx = ++avionicsBrowserFunctionCntr;
  Py_INCREF(function);
  Py_INCREF(refCon);
  avionicsBrowserFunctionDict[idx] = {
    .callback = function,
    .refCon = refCon,
    .module_name = CurrentPythonModuleName
  };
  XPLMAvionicsAddBrowserFunction_ptr(avionics_id, inName, genericAvionicsBrowserCallback, (void *)idx);
#endif
  Py_RETURN_NONE;
}

PyObject *XPLMSetObjectAvionicsFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("object"), CHAR("avionicsID"), nullptr};
  (void) self;
  PyObject *object, *avionicsID;
  if(!XPLMSetObjectAvionics_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMSetObjectAvionics is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "OO", keywords, &object, &avionicsID)){
    return nullptr;
  }
  XPLMObjectRef inObject = getVoidPtr(object, "XPLMObjectRef");
  void *avionics_id = getVoidPtr(avionicsID, "XPLMAvionicsID");
  int res = XPLMSetObjectAvionics_ptr(inObject, avionics_id);
  return PyLong_FromLong(res);
}

PyObject *XPLMClearObjectAvionicsFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("object"), CHAR("avionicsID"), nullptr};
  (void) self;
  PyObject *object, *avionicsID;
  if(!XPLMClearObjectAvionics_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMClearObjectAvionics is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "OO", keywords, &object, &avionicsID)){
    return nullptr;
  }
  XPLMObjectRef inObject = getVoidPtr(object, "XPLMObjectRef");
  void *avionics_id = getVoidPtr(avionicsID, "XPLMAvionicsID");
  XPLMClearObjectAvionics_ptr(inObject, avionics_id);
  Py_RETURN_NONE;
}

void resetAvionicsCallbacks(void) {
  if(!XPLMUnregisterAvionicsCallbacks_ptr){
    return;
  }
  errCheck("prior resetAvionicsCallbacks");

  for (auto& pair : avionicsCallbacksDict) {
    AvionicsCallbackInfo& info = pair.second;
    Py_DECREF(info.before);
    Py_DECREF(info.after);
    Py_DECREF(info.refCon);
    Py_DECREF(info.bezel_draw);
    Py_DECREF(info.draw);
    Py_DECREF(info.bezel_click);
    Py_DECREF(info.bezel_rightclick);
    Py_DECREF(info.bezel_scroll);
    Py_DECREF(info.bezel_cursor);
    Py_DECREF(info.screen_touch);
    Py_DECREF(info.screen_righttouch);
    Py_DECREF(info.screen_scroll);
    Py_DECREF(info.screen_cursor);
    Py_DECREF(info.keyboard);
    Py_DECREF(info.brightness);
    Py_DECREF(info.browserLoadFinished);
    Py_DECREF(info.browserLoadError);
    if (info.create) {
      XPLMDestroyAvionics_ptr(info.avionicsID);
      errCheck("afterXPLMDestroyAvionics in reset");
    } else {
      XPLMUnregisterAvionicsCallbacks_ptr(info.avionicsID);
      errCheck("after XPLMUnregisterAvionicsCallbacks in reset");
    }
    pythonDebug("     Reset --     %s - (%d)", info.module_name, info.avionicsID);
    deleteCapsuleByPtr((void*)info.avionicsID, "XPLMAvionicsID");
  }
  avionicsCallbacksDict.clear();

#if defined(XPLM440)
  for (auto& pair : avionicsBrowserFunctionDict) {
    Py_DECREF(pair.second.callback);
    Py_DECREF(pair.second.refCon);
  }
  avionicsBrowserFunctionDict.clear();
#endif
}



/* ---------------------------------------------------------------------------
   Method-table fragment merged into the XPLMDisplay module by
   PyInit_XPLMDisplay() in display.cpp. Docstrings live here, beside the
   functions they document.
   --------------------------------------------------------------------------- */
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

My_DOCSTR(_isAvionicsMappedToVR__doc__, "isAvionicsMappedToVR",
          "avionicsID",
          "avionicsID:XPLMAvionicsID",
          "int",
          "Return 1 if the cockpit device with given ID is mapped into VR.");

My_DOCSTR(_setAvionicsMappedToVR__doc__, "setAvionicsMappedToVR",
          "avionicsID, mapped=1",
          "avionicsID:XPLMAvionicsID, mapped:int=1",
          "None",
          "Map (mapped=1) or unmap the cockpit device with given ID into VR.");

My_DOCSTR(_createAvionicsEx__doc__, "createAvionicsEx",
          "screenWidth=100, screenHeight=200, bezelWidth=140, bezelHeight=250, screenOffsetX=20, screenOffsetY=25, "
          "drawOnDemand=0, bezelDraw=None, screenDraw=None, bezelClick=None, bezelRightClick=None, "
          "bezelScroll=None, bezelCursor=None, screenTouch=None, screenRightTouch=None, screenScroll=None, "
          "screenCursor=None, keyboard=None, brightness=None, deviceID=\"deviceID-<num>\", deviceName=\"deviceName-<num>\", refcon=None, "
          "contentType=WindowContentTypeOpenGL, windowWithChrome=0, browserLoadFinished=None, browserLoadError=None",
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
          "refCon: Any = None, "
          "contentType: XPLMWindowContentType = WindowContentTypeOpenGL, windowWithChrome: int = 0, "
          "browserLoadFinished: Optional[Callable[[XPLMAvionicsID, str, Any], None]] = None, "
          "browserLoadError: Optional[Callable[[XPLMAvionicsID, str, str, Any], None]] = None,"
          ,
          "XPLMAvionicsID",
          "Creates glass cockpit device for 3D cockpit.\n"
          "With 12.0 you needed to call this within your XPluginStart callback\n"
          "to ensure your texture would be ready. Since 12.1, you may call this\n"
          "at anytime and X-Plane will retroactively map your display to it.\n"
          "Note, if not specified, we create a \"unique\" deviceID and deviceName,\n"
          "as deviceID *must* be unique.\n"
          "\n"
          "For contentType=WindowContentTypeBrowser (XPLM440), the device screen is a\n"
          "CEF web view driven with avionicsSetURL()/avionicsRefresh()/etc.;\n"
          "browserLoadFinished(avionicsID, url, refCon) fires when the main frame\n"
          "finishes loading (which also happens for rendered HTTP error pages -- treat\n"
          "browserLoadError as authoritative and ignore url='about:blank'), and\n"
          "browserLoadError(avionicsID, url, error, refCon) fires on a network-level\n"
          "navigation failure.\n"
          "\n"
          "Returns new avionicsID.");

My_DOCSTR(_avionicsSetURL__doc__, "avionicsSetURL",
          "avionicsID, url",
          "avionicsID:XPLMAvionicsID, url:str",
          "None",
          "Load a URL into a browser-content-type avionics device.\n"
          "Safe to call immediately after createAvionicsEx; the load is queued\n"
          "until the browser is ready. Subsequent calls replace the page.");

My_DOCSTR(_avionicsRefresh__doc__, "avionicsRefresh",
          "avionicsID, ignoreCache=0",
          "avionicsID:XPLMAvionicsID, ignoreCache:int=0",
          "None",
          "Reload the current URL in a browser-content-type avionics device.\n"
          "Pass ignoreCache=1 to bypass the HTTP cache (shift-reload).");

My_DOCSTR(_avionicsInjectScript__doc__, "avionicsInjectScript",
          "avionicsID, script",
          "avionicsID:XPLMAvionicsID, script:str",
          "None",
          "Execute a JavaScript snippet in a browser device's main frame.\n"
          "The script runs once with access to the same `xplane.*` namespace\n"
          "exposed to the page. If injected before the page finishes loading,\n"
          "it may run against an empty document.");

My_DOCSTR(_avionicsAddBrowserFunction__doc__, "avionicsAddBrowserFunction",
          "avionicsID, name, function, refCon=None",
          "avionicsID:XPLMAvionicsID, name:str, function:Callable[[XPLMAvionicsID, str, Any], Optional[str]], refCon:Any=None",
          "None",
          "Register a callback the browser page can invoke as `xplane.<name>(arg)`.\n"
          "function(avionicsID, json, refCon) is called with the JSON argument string;\n"
          "its return value (a str, parsed as JSON) resolves the page's Promise.");

My_DOCSTR(_setObjectAvionics__doc__, "setObjectAvionics",
          "object, avionicsID",
          "object:XPLMObjectRef, avionicsID:XPLMAvionicsID",
          "int",
          "Glue a device you created with createAvionicsEx() onto a loaded 3D\n"
          "object, so the device's screen draws on that object (e.g. one drawn\n"
          "in the world via the instancing API).\n"
          "\n"
          "The object must declare an ATTR_cockpit_device with the same deviceID\n"
          "string passed to createAvionicsEx(). You may only bind devices you\n"
          "created yourself.\n"
          "\n"
          "Returns 1 if the object had a matching device screen and the binding\n"
          "succeeded, 0 otherwise.");

My_DOCSTR(_clearObjectAvionics__doc__, "clearObjectAvionics",
          "object, avionicsID",
          "object:XPLMObjectRef, avionicsID:XPLMAvionicsID",
          "None",
          "Remove a binding previously made with setObjectAvionics(), restoring\n"
          "the object's device screen to black. Bindings are also cleared\n"
          "automatically when you destroy the device with destroyAvionics().");

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
PyMethodDef displayAvionicsMethods[] = {
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
  {"isAvionicsMappedToVR", (PyCFunction)XPLMIsAvionicsMappedToVRFun, METH_VARARGS | METH_KEYWORDS, _isAvionicsMappedToVR__doc__},
  {"XPLMIsAvionicsMappedToVR", (PyCFunction)XPLMIsAvionicsMappedToVRFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"setAvionicsMappedToVR", (PyCFunction)XPLMSetAvionicsMappedToVRFun, METH_VARARGS | METH_KEYWORDS, _setAvionicsMappedToVR__doc__},
  {"XPLMSetAvionicsMappedToVR", (PyCFunction)XPLMSetAvionicsMappedToVRFun, METH_VARARGS | METH_KEYWORDS, ""},
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
  // SDK 440
  {"avionicsSetURL", (PyCFunction)XPLMAvionicsSetURLFun, METH_VARARGS | METH_KEYWORDS, _avionicsSetURL__doc__},
  {"XPLMAvionicsSetURL", (PyCFunction)XPLMAvionicsSetURLFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"avionicsRefresh", (PyCFunction)XPLMAvionicsRefreshFun, METH_VARARGS | METH_KEYWORDS, _avionicsRefresh__doc__},
  {"XPLMAvionicsRefresh", (PyCFunction)XPLMAvionicsRefreshFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"avionicsInjectScript", (PyCFunction)XPLMAvionicsInjectScriptFun, METH_VARARGS | METH_KEYWORDS, _avionicsInjectScript__doc__},
  {"XPLMAvionicsInjectScript", (PyCFunction)XPLMAvionicsInjectScriptFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"avionicsAddBrowserFunction", (PyCFunction)XPLMAvionicsAddBrowserFunctionFun, METH_VARARGS | METH_KEYWORDS, _avionicsAddBrowserFunction__doc__},
  {"XPLMAvionicsAddBrowserFunction", (PyCFunction)XPLMAvionicsAddBrowserFunctionFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"setObjectAvionics", (PyCFunction)XPLMSetObjectAvionicsFun, METH_VARARGS | METH_KEYWORDS, _setObjectAvionics__doc__},
  {"XPLMSetObjectAvionics", (PyCFunction)XPLMSetObjectAvionicsFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"clearObjectAvionics", (PyCFunction)XPLMClearObjectAvionicsFun, METH_VARARGS | METH_KEYWORDS, _clearObjectAvionics__doc__},
  {"XPLMClearObjectAvionics", (PyCFunction)XPLMClearObjectAvionicsFun, METH_VARARGS | METH_KEYWORDS, ""},
  {nullptr, nullptr, 0, nullptr}
};
#pragma GCC diagnostic pop
