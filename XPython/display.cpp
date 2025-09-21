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
  std::string module_name;
  PyObject *callback;
  XPLMDrawingPhase phase;
  int before;
  PyObject *refCon;
};

static std::unordered_map<intptr_t, DrawCallbackInfo> drawCallbackDict;
// #define DRAW_MODULE_NAME 0
// #define DRAW_CALLBACK 1 
// #define DRAW_PHASE 2
// #define DRAW_BEFORE 3
// #define DRAW_REFCON 4  
static int genericDrawCallback(XPLMDrawingPhase inPhase, int inIsBefore, void *inRefcon);

static intptr_t keySniffCallbackCntr;
struct KeySniffInfo {
  std::string module_name;
  PyObject *callback;
  int before;
  PyObject *refCon;
};

static std::unordered_map<intptr_t, KeySniffInfo> keySniffCallbackDict;
#define KEYSNIFF_MODULE_NAME 0
#define KEYSNIFF_CALLBACK 1
#define KEYSNIFF_BEFORE 2
#define KEYSNIFF_REFCON 3
static int genericKeySnifferCallback(char inChar, XPLMKeyFlags inFlags, char inVirtualKey, void *inRefcon);

struct AvionicsCallbackInfo {
  std::string module_name;
  int deviceID;
  XPLMAvionicsID avionicsID;
  PyObject *before;
  PyObject *after;
  PyObject *refCon;
  PyObject *bezel_draw;
  PyObject *draw;
  PyObject *bezel_click;
  PyObject *bezel_rightclick;
  PyObject *bezel_scroll;
  PyObject *bezel_cursor;
  PyObject *screen_touch;
  PyObject *screen_righttouch;
  PyObject *screen_scroll;
  PyObject *screen_cursor;
  PyObject *keyboard;
  PyObject *brightness;
  int create;
};
static std::unordered_map<intptr_t, AvionicsCallbackInfo> avionicsCallbacksDict;
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

// static std::unordered_map<void*, intptr_t> avionicsCallbacksIDDict;
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



//draw, key,mouse, cursor, wheel
struct WindowInfo {
  PyObject *draw;
  PyObject *click;
  PyObject *key;
  PyObject *cursor;
  PyObject *wheel;
  PyObject *rightClick;
  std::string module_name;
};

static std::unordered_map<XPLMWindowID, WindowInfo> windowDict;
// #define WINDOW_DRAW 0
// #define WINDOW_CLICK 1
// #define WINDOW_KEY 2
// #define WINDOW_CURSOR 3
// #define WINDOW_WHEEL 4
// #define WINDOW_RIGHTCLICK 5
// #define WINDOW_MODULE_NAME 6
static void genericWindowDraw(XPLMWindowID inWindowID, void *inRefcon);
static int genericWindowMouseClick(XPLMWindowID inWindowID, int x, int y, XPLMMouseStatus inMouse, void *inRefcon);
static void genericWindowKey(XPLMWindowID, char inKey, XPLMKeyFlags inFlags, char inVirtualKey, void *inRefcon, int losingFocus);
static XPLMCursorStatus genericWindowCursor(XPLMWindowID inWindowID, int x, int y, void *inRefcon);
static int genericWindowMouseWheel(XPLMWindowID  inWindowID, int x, int y, int wheel, int clicks, void *inRefcon);
static int genericWindowRightClick(XPLMWindowID inWindowID, int x, int y, XPLMMouseStatus inMouse, void *inRefcon);

struct HotKeyInfo {
  XPLMHotKeyID hotKeyID;
  PyObject *callback;
  PyObject *refCon;
  std::string module_name;
};
static intptr_t hotkeyCounter = 0;
static std::unordered_map<intptr_t, HotKeyInfo> hotkeyDict;
static void genericHotkeyCallback(void *inRefcon);

static PyObject *monitorBndsCallback;

void resetHotKeyCallbacks(void) {
  errCheck("prior resethotkey");

  for (const auto& pair : hotkeyDict) {
    char *callback = objToStr(pair.second.callback);
    XPLMUnregisterHotKey((XPLMHotKeyID) pair.first);
    deleteCapsule(makeCapsule((XPLMHotKeyID)pair.first, "XPLMHotKeyID"));
    pythonDebug("     Reset --     %s - (%s)", pair.second.module_name.c_str(), callback);
    Py_DECREF(pair.second.refCon);
    Py_DECREF(pair.second.callback);
    free(callback);
  }
  hotkeyDict.clear();
  errCheck("post while resethotkey");

  errCheck("post reset hotkey");
}

void resetAvionicsCallbacks(void) {
  if(!XPLMUnregisterAvionicsCallbacks_ptr){
    return;
  }
  errCheck("prior resetAvionicsCallbacks");

  for (auto it = avionicsCallbacksDict.begin(); it != avionicsCallbacksDict.end();) {
    AvionicsCallbackInfo& info = it->second;
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
    if (info.create) {
      XPLMDestroyAvionics_ptr(info.avionicsID);
      errCheck("afterXPLMDestroyAvionics in reset");
    } else {
      XPLMUnregisterAvionicsCallbacks_ptr(info.avionicsID);
      errCheck("after XPLMUnregisterAvionicsCallbacks in reset");
    }
    pythonDebug("     Reset --     %s - (%d)", info.module_name.c_str(), info.avionicsID);
    deleteCapsule(makeCapsule((XPLMAvionicsID)it->first, "XPLMAvionicsID"));
  }
  avionicsCallbacksDict.clear();
}

void resetDrawCallbacks(void) {
  for (const auto& pair : drawCallbackDict) {
    char *callback = objToStr(pair.second.callback);
    pythonDebug("     Reset --     %s - (%s)", pair.second.module_name.c_str(), callback);
    free(callback);

    XPLMUnregisterDrawCallback(genericDrawCallback,
                               pair.second.phase, pair.second.before,
                               (void*)pair.first);
    Py_DECREF(pair.second.callback);
    Py_DECREF(pair.second.refCon);
  }

  drawCallbackDict.clear();
}

void resetKeySniffCallbacks(void) {
  for (const auto& pair : keySniffCallbackDict) {
    char *callback = objToStr(pair.second.callback);
    pythonDebug("     Reset --     %s - (%s)", pair.second.module_name.c_str(), callback);
    free(callback);

    XPLMUnregisterKeySniffer(genericKeySnifferCallback,
                             pair.second.before,
                             (void*)pair.first);
    Py_DECREF(pair.second.callback);
    Py_DECREF(pair.second.refCon);
  }

  keySniffCallbackDict.clear();
}

void resetWindows(void) {
  for (const auto& pair : windowDict) {
    pythonDebug("     Reset --     (%s)", pair.second.module_name.c_str());
    XPLMDestroyWindow((XPLMWindowID)pair.first);
    Py_DECREF(pair.second.draw);
    Py_DECREF(pair.second.click);
    Py_DECREF(pair.second.key);
    Py_DECREF(pair.second.cursor);
    Py_DECREF(pair.second.wheel);
    Py_DECREF(pair.second.rightClick);
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
  intptr_t idx = ++drawCallbackCntr;

  Py_INCREF(callback);
  Py_INCREF(refcon);
  drawCallbackDict[idx] = {
    .module_name = std::string(CurrentPythonModuleName),
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
          "OpenGL state (texturing, etc.) will be unknwon.\n"
          "\n"
          "Successful registration returns an AvionicsID."
         );
static PyObject *XPLMRegisterAvionicsCallbacksExFun(PyObject *self, PyObject *args, PyObject *kwargs)
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
  } else if (PyLong_Check(firstObj)) {
    deviceId = PyLong_AsLong(firstObj);
  } else if (PySequence_Check(firstObj)) {
    paramsObj = firstObj;
    if (PySequence_Length(paramsObj) != 13) {
      PyErr_SetString(PyExc_AttributeError ,"registerAvionicsEx tuple did not contain 13 values\n.");
      return nullptr;
    }
  } else {
    PyErr_SetString(PyExc_AttributeError ,"registerAvionicsEx could not parse arguments.\n");
    return nullptr;
  }

  XPLMCustomizeAvionics_t avionics_params;
  avionics_params.structSize = sizeof(avionics_params);
  if (paramsObj != Py_None) {
    PyObject *paramsTuple = PySequence_Tuple(paramsObj);
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
    avionics_params.deviceId = deviceId;
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
    .module_name = std::string(CurrentPythonModuleName),
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

My_DOCSTR(_unregisterAvionicsCallbacks__doc__, "unregisterAvionicsCallbacks",
          "avionicsId",
          "avionicsId:XPLMAvionicsID",
          "None",
          "Unregisters avionics draw callback(s) associated with given avionicsId.\n"
          "\n"
          "Does not return a value."
         );
static PyObject *XPLMUnregisterAvionicsCallbacksFun(PyObject *self, PyObject *args, PyObject *kwargs)
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
  Py_DECREF(avIDCapsule);

  int found = 0;
  for (auto& pair: avionicsCallbacksDict) {
    AvionicsCallbackInfo info = pair.second;
    if (info.avionicsID == avionicsId) {
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
      deleteCapsule(makeCapsule((XPLMAvionicsID)avionicsId, "XPLMAvionicsID"));
      avionicsCallbacksDict.erase(pair.first);
      break;
    }
  }
  if (found == 0 ) {
    PyErr_SetString(PyExc_RuntimeError, "XPLMUnregisterAvionicsCallback failed to find avionicsID\n");
    return nullptr;
  }
    
  /* and... remove from data structures */
  
  Py_RETURN_NONE;
}
          


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
static PyObject *XPLMGetAvionicsHandleFun(PyObject *self, PyObject *args, PyObject *kwargs)
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
    .module_name = std::string(CurrentPythonModuleName),
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

My_DOCSTR(_isAvionicsBound__doc__, "isAvionicsBound",
          "avionicsID",
          "avionicsID:XPLMAvionicsID",
          "int",
          "Return 1 if cockpit device with given ID is used by the current aircraft.");
static PyObject *XPLMIsAvionicsBoundFun(PyObject *self, PyObject *args, PyObject *kwargs)
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

My_DOCSTR(_isCursorOverAvionics__doc__, "isCursorOverAvionics",
          "avionicsID",
          "avionicsID:XPLMAvionicsID",
          "tuple[int, int] | None",
          "Is the cursor over the device with given avionicsID\n"
          "\n"
          "Returns tuple (x, y) with position or None.");
static PyObject *XPLMIsCursorOverAvionicsFun(PyObject *self, PyObject *args, PyObject *kwargs)
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

My_DOCSTR(_isAvionicsPopupVisible__doc__, "isAvionicsPopupVisible",
          "avionicsID",
          "avionicsID:XPLMAvionicsID",
          "int",
          "Is the popup window for the device with given avionicsID visible?\n"
          "(It may or may not be popped out into an OS window.)\n"
          "\n"
          "Returns 1 if true.");
static PyObject *XPLMIsAvionicsPopupVisibleFun(PyObject *self, PyObject *args, PyObject *kwargs)
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

My_DOCSTR(_isAvionicsPoppedOut__doc__, "isAvionicsPoppedOut",
          "avionicsID",
          "avionicsID:XPLMAvionicsID",
          "int",
          "Returns 1 (true) if the popup window for the cockpit device is popped out\n"
          "into an OS window.");
static PyObject *XPLMIsAvionicsPoppedOutFun(PyObject *self, PyObject *args, PyObject *kwargs)
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

My_DOCSTR(_hasAvionicsKeyboardFocus__doc__, "hasAvionicsKeyboardFocus",
          "avionicsID",
          "avionicsID:XPLMAvionicsID",
          "int",
          "Returns 1 (true) if cockpit device has keyboard focus.");
static PyObject *XPLMHasAvionicsKeyboardFocusFun(PyObject *self, PyObject *args, PyObject *kwargs)
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

My_DOCSTR(_avionicsNeedsDrawing__doc__, "avionicsNeedsDrawing",
          "avionicsID",
          "avionicsID:XPLMAvionicsID",
          "None",
          "Tells X-Plane that your device's screens needs to be re-drawn.\n"
          "If your device is marked for on-demand drawing, XP will call your screen\n"
          "drawing callback before drawing the next simulator frame. If your device\n"
          "is already drawn every frame, this has no effect.");
static PyObject *XPLMAvionicsNeedsDrawingFun(PyObject *self, PyObject *args, PyObject *kwargs)
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

My_DOCSTR(_popOutAvionics__doc__, "popOutAvionics",
          "avionicsID",
          "avionicsID:XPLMAvionicsID",
          "None",
          "Pops out OS window for cockpit device.");
static PyObject *XPLMPopOutAvionicsFun(PyObject *self, PyObject *args, PyObject *kwargs)
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

My_DOCSTR(_takeAvionicsKeyboardFocus__doc__, "takeAvionicsKeyboardFocus",
          "avionicsID",
          "avionicsID:XPLMAvionicsID",
          "None",
          "Sets keyboard focus to the (already) visible popup window of cockpit device.\n"
          "Does nothing if device is not visible.");
static PyObject *XPLMTakeAvionicsKeyboardFocusFun(PyObject *self, PyObject *args, PyObject *kwargs)
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

My_DOCSTR(_destroyAvionics__doc__, "destroyAvionics",
          "avionicsID",
          "avionicsID:XPLMAvionicsID",
          "None",
          "Destroys the cockpit device and deallocates its framebuffer. You should\n"
          "only ever call this for devices that you created, not stock X-Plane devices\n"
          "you have customized.");
static PyObject *XPLMDestroyAvionicsFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("avionicsID"), nullptr};
  (void) self;
  PyObject *avionicsID;
  if (!XPLMDestroyAvionics_ptr) {
    PyErr_SetString(PyExc_RuntimeError , "XPLMDestroyAvionics is available only in XPLM410 and up.");
    return nullptr;
  }

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &avionicsID)) {
    return nullptr;
  }
  void *avionics_id = getVoidPtr(avionicsID, "XPLMAvionicsID");
  XPLMDestroyAvionics_ptr(avionics_id);
  Py_RETURN_NONE;
}

My_DOCSTR(_getAvionicsBusVoltsRatio__doc__, "getAvionicsBusVoltsRatio",
          "avionicsID",
          "avionicsID:XPLMAvionicsID",
          "float",
          "Return ratio [0.0:1.0] of nominal voltage of electrical bus,\n"
          "for given avionics device. Returns -1 if device is not bound\n"
          "to the current aircraft.");
static PyObject *XPLMGetAvionicsBusVoltsRatioFun(PyObject *self, PyObject *args, PyObject *kwargs)
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
static PyObject *XPLMGetAvionicsBrightnessRheoFun(PyObject *self, PyObject *args, PyObject *kwargs)
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
static PyObject *XPLMSetAvionicsBrightnessRheoFun(PyObject *self, PyObject *args, PyObject *kwargs)
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

My_DOCSTR(_setAvionicsPopupVisible__doc__, "setAvionicsPopupVisible",
          "avionicsID, visible=1",
          "avionicsID:XPLMAvionicsID, visible:int=1",
          "None",
          "Shows (visible=1) or Hides popup window for cockpit device.");
static PyObject *XPLMSetAvionicsPopupVisibleFun(PyObject *self, PyObject *args, PyObject *kwargs)
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
static PyObject *XPLMRegisterKeySnifferFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("sniffer"), CHAR("before"), CHAR("refCon"), nullptr};

  errCheck("before registerKeySniffer");
  (void) self;
  PyObject *callback, *refcon = Py_None;
  int inBeforeWindows=0;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|iO", keywords, &callback, &inBeforeWindows, &refcon)) {
    return nullptr;
  }
    
  intptr_t idx = ++keySniffCallbackCntr;

  Py_INCREF(callback);
  Py_INCREF(refcon);
  keySniffCallbackDict[idx] = {
    .module_name = std::string(CurrentPythonModuleName),
    .callback = callback,
    .before = inBeforeWindows,
    .refCon = refcon
  };
  int res = XPLMRegisterKeySniffer(genericKeySnifferCallback, inBeforeWindows, (void *)keySniffCallbackCntr);
  if(!res){
    PyErr_SetString(PyExc_RuntimeError ,"registerKeySniffer failed.\n");
    return nullptr;
  }
  errCheck("at end registerKeySniffer");
  return PyLong_FromLong(res);
}


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
  int res = -1;
  intptr_t foundKey = 0;

  for (const auto& pair : drawCallbackDict) {
    if (pair.second.callback == callback
        && !strcmp(pair.second.module_name.c_str(), CurrentPythonModuleName)
        && pair.second.phase == inPhase
        && pair.second.before == inWantsBefore
        && pair.second.refCon == refcon) {
      foundKey = pair.first;
      res = XPLMUnregisterDrawCallback(genericDrawCallback, inPhase, inWantsBefore, (void*)pair.first);
      Py_DECREF(pair.second.callback);
      Py_DECREF(pair.second.refCon);
      drawCallbackDict.erase(pair.first);
      break;
    }
  }

  if(foundKey == 0) {
    char *s = objToStr(callback);
    pythonLog("Failed to find drawCallback entry for %s %s", CurrentPythonModuleName, s);
    free(s);
  }
  errCheck("after XPLMUnregisterDrawCallback");
  return PyLong_FromLong(res);
}

My_DOCSTR(_unregisterKeySniffer__doc__, "unregisterKeySniffer",
          "sniffer, before=0, refCon=None",
          "sniffer:Callable[[int, XPLMKeyFlags, int, Any], int], before:int=0, refCon:Any=None",
          "int",
          "Unregisters key sniffer.\n"
          "\n"
          "Parameters must match those provided with registerKeySniffer().\n"
          "Returns 1 on success, 0 otherwise.");
static PyObject *XPLMUnregisterKeySnifferFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("sniffer"), CHAR("before"), CHAR("refCon"), nullptr};
  (void) self;
  PyObject *callback, *refcon = Py_None;
  int inBeforeWindows = 0;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|iO", keywords, &callback, &inBeforeWindows, &refcon)) {
    return nullptr;
  }

  int res = -1;
  intptr_t foundKey = 0;

  for (const auto& pair : keySniffCallbackDict) {
    if(pair.second.callback == callback
       && !strcmp(pair.second.module_name.c_str(), CurrentPythonModuleName)
       && pair.second.before == inBeforeWindows
       && pair.second.refCon == refcon) {
      foundKey = pair.first;
      res = XPLMUnregisterKeySniffer(genericKeySnifferCallback,
                                     inBeforeWindows, (void*)pair.first);
      Py_DECREF(pair.second.callback);
      Py_DECREF(pair.second.refCon);
      keySniffCallbackDict.erase(pair.first);
      break;
    }
  }

  if(foundKey == 0){
    char *s = objToStr(callback);
    pythonLog("Failed to find drawCallback entry for %s %s", CurrentPythonModuleName, s);
    free(s);
  }    
  PyObject *err = PyErr_Occurred();
  if(err){
    pythonLog("Error occured during the XPLMUnregisterKeySnifferCallback call:");
    pythonLogException();
  }
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

    PyObject *oRes = PyObject_CallFunctionObjArgs(func, pID, inRefcon, nullptr);
    Py_DECREF(pID);
    clock_gettime(CLOCK_MONOTONIC, &stop);
    pluginStats[getPluginIndex()].draw_time += (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_nsec - start.tv_nsec) / 1000;

    if(PyErr_Occurred()) {
      pythonLogException();
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
    PyObject *arg3 = PyLong_FromLong((unsigned int)inVirtualKey);
    PyObject *arg4 = PyLong_FromLong(losingFocus);
    /* char *s = objToStr(pID); */
    /* printf("Calling genericWindowKey callback. inWindowID = %p, pPID = %s, losingFocus = %d\n", inWindowID, s, losingFocus); */
    /* free(s); */
    PyObject *pID = makeCapsule(inWindowID, "XPLMWindowID");
    set_moduleName(it->second.module_name);
    PyObject *oRes = PyObject_CallFunctionObjArgs(func, pID, arg1, arg2, arg3, inRefcon, arg4, nullptr);
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
  PyObject *pRes = PyObject_CallFunctionObjArgs(func, pID, arg1, arg2, arg3, inRefcon, nullptr);
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
  PyObject *pRes = PyObject_CallFunctionObjArgs(func, pID, arg1, arg2, arg3, inRefcon, nullptr);
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
  PyObject *pRes = PyObject_CallFunctionObjArgs(func, pID, arg1, arg2, inRefcon, nullptr);
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
  PyObject *pRes = PyObject_CallFunctionObjArgs(func, pID, arg1, arg2, arg3, arg4, inRefcon, nullptr);
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
  errCheck("end genericWindowMouseWheel");
  return res;
}


My_DOCSTR(_createAvionicsEx__doc__, "createAvionicsEx",
          "screenWidth=100, screenHeight=200, bezelWidth=140, bezelHeight=250, screenOffsetX=20, screenOffsetY=25, "
          "drawOnDemand=0, bezelDraw=None, screenDraw=None, bezelClick=None, bezelRightClick=None, "
          "bezelScroll=None, bezelCursor=None, screenTouch=None, screenRightTouch=None, screenScroll=None, "
          "screenCursor=None, keyboard=None, brightness=None, deviceID=\"deviceID\", deviceName=\"deviceName\", refcon=None",
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
          "\n"
          "Returns new avionicsID.");

static PyObject *XPLMCreateAvionicsExFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  (void) self;
  static char *keywords[] = {CHAR("screenWidth"), CHAR("screenHeight"), CHAR("bezelWidth"), CHAR("bezelHeight"),
                             CHAR("screenOffsetX"), CHAR("screenOffsetY"), CHAR("drawOnDemand"),
                             CHAR("bezelDraw"), CHAR("screenDraw"),
                             CHAR("bezelClick"), CHAR("bezelRightClick"), CHAR("bezelScroll"), CHAR("bezelCursor"),
                             CHAR("screenTouch"), CHAR("screenRightTouch"), CHAR("screenScroll"), CHAR("screenCursor"),
                             CHAR("keyboard"), CHAR("brightness"),
                             CHAR("deviceID"), CHAR("deviceName"), CHAR("refCon"), nullptr};


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
    *refcon=Py_None;
  char deviceIDstr[] = "deviceID";;
  char deviceName[] = "deviceName";

  /* not used with create */
  if(!XPLMCreateAvionicsEx_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMCreateAvionicsEx() is available only in XPLM410 and up.");
    return nullptr;
  }

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|OiiiiiiOOOOOOOOOOOOssO", keywords,
                                  &firstObj, &screenHeight, &bezelWidth, &bezelHeight, &screenOffsetX, &screenOffsetY,
                                  &drawOnDemand,
                                  &bezelDraw, &screenDraw,
                                  &bezelClick, &bezelRightClick, &bezelScroll, &bezelCursor,
                                  &screenTouch, &screenRightTouch, &screenScroll, &screenCursor,
                                  &keyboard, &brightness,
                                  &deviceIDstr,
                                  &deviceName,
                                  &refcon
                                  )){
    return nullptr;
  }
  if (firstObj == Py_None) {
    ; /* ``width'' remains as default value */
  } else if (PyLong_Check(firstObj)) {
    screenWidth = PyLong_AsLong(firstObj);
  } else if (PySequence_Check(firstObj)) {
    paramsObj = firstObj;
    if (PySequence_Length(paramsObj) != 22) {
      PyErr_SetString(PyExc_AttributeError ,"createAvionicsEx tuple did not contain 21 values\n.");
      return nullptr;
    }
  } else {
    PyErr_SetString(PyExc_AttributeError ,"createAvionicsEx could not parse arguments.\n");
    return nullptr;
  }

  XPLMCreateAvionics_t avionics_params;
  avionics_params.structSize = sizeof(avionics_params);
  if(paramsObj != Py_None) {
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
    Py_DECREF(paramsTuple);
  } else {
    avionics_params.screenWidth = screenWidth;
    avionics_params.screenHeight = screenHeight;
    avionics_params.bezelWidth = bezelWidth;
    avionics_params.bezelHeight = bezelHeight;
    avionics_params.screenOffsetX = screenOffsetX;
    avionics_params.screenOffsetY = screenOffsetY;
    avionics_params.drawOnDemand = drawOnDemand;
    avionics_params.deviceID = deviceIDstr;
    avionics_params.deviceName = deviceName;
  }

  if (avionics_params.bezelWidth < (avionics_params.screenWidth + avionics_params.screenOffsetX) || avionics_params.bezelHeight < (avionics_params.screenHeight + avionics_params.screenOffsetY)) {
    PyErr_SetString(PyExc_ValueError ,"createAvionicsEx() requires bezelWidth >= screenWidth + offsetX; and bezelHeight >= screenHeight + offsetY.\n");
    return nullptr;
  }

  avionics_params.refcon = (void *) ++avionicsCallbacksCntr;
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
                              bezelCursor, screenTouch, screenRightTouch, screenScroll, screenCursor, keyboard, brightness};
  for (int i=0; i< 12; i++) {
    if (callbackList[i] != Py_None) {
      if (PyCallable_Check(callbackList[i])) {
        Py_INCREF(callbackList[i]);
      } else {
        PyErr_SetString(PyExc_ValueError ,"createAvionicsEx callback is not callable.\n");
        return nullptr;
      }
    }
  }

  Py_INCREF(refcon);

  PyObject *avDictsKey = PyLong_FromLong(avionicsCallbacksCntr);
  if(!avDictsKey){
    PyErr_SetString(PyExc_RuntimeError ,"Couldn't create long.\n");
    return nullptr;
  }

  XPLMAvionicsID avionicsId = XPLMCreateAvionicsEx_ptr(&avionics_params);
  avionicsCallbacksDict[avionicsCallbacksCntr] = {
    .module_name = std::string(CurrentPythonModuleName),
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
    .create = 1
  };

  PyObject *avIDCapsule = makeCapsule(avionicsId, "XPLMAvionicsID");
  if(!avIDCapsule){
    PyErr_SetString(PyExc_RuntimeError ,"XPLMRegisterAvionicsCallbacksEx failed.\n");
    return nullptr;
  }

  Py_DECREF(avDictsKey);
  errCheck("end createAvionicsEx");
  return avIDCapsule;
}

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
  if (firstObj == Py_None) {
    ; /* ``left'' remains as default value */
  } else if (PyLong_Check(firstObj)) {
    left = PyLong_AsLong(firstObj);
  } else if (PySequence_Check(firstObj)) {
    paramsObj = firstObj;
    if (PySequence_Length(paramsObj) != 14) {
      PyErr_SetString(PyExc_AttributeError ,"createWindowEx tuple did not contain 14 values\n.");
      return nullptr;
    }
  } else {
    PyErr_SetString(PyExc_AttributeError ,"createWindowEx could not parse arguments.\n");
    return nullptr;
  }
    
  XPLMCreateWindow_t window_params;
  window_params.structSize = sizeof(window_params);
  if (paramsObj != Py_None){
    PyObject *paramsTuple = PySequence_Tuple(paramsObj);
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
    window_params.left = left;
    window_params.right = right;
    window_params.top = top;
    window_params.bottom = bottom;
    window_params.visible = visible;
    window_params.decorateAsFloatingWindow = decoration;
    window_params.layer = layer;
    window_params.refcon = refCon;
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
  PyObject *pID = makeCapsule(id, "XPLMWindowID");

  windowDict[id] = {
    .draw = draw,
    .click = click,
    .key = key,
    .cursor = cursor,
    .wheel = wheel,
    .rightClick = rightClick,
    .module_name = std::string(CurrentPythonModuleName)
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
    .module_name = std::string(CurrentPythonModuleName)
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
    PyObject *tmp = (PyObject *)XPLMGetWindowRefCon(winID);
    Py_DECREF(tmp);
    XPLMDestroyWindow(winID);
    Py_DECREF(it->second.draw);
    Py_DECREF(it->second.click);
    Py_DECREF(it->second.key);
    Py_DECREF(it->second.cursor);
    Py_DECREF(it->second.rightClick);
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
          "Bring window to front (of it's window layer).");
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
          "Returns 1 if window is currently in the front of it's window layer).");
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

static void genericHotkeyCallback(void *inRefcon)
{
  errCheck("prior hotkeyCallback");
  intptr_t id = (intptr_t) inRefcon;
  auto it = hotkeyDict.find(id);
  if(it == hotkeyDict.end()){
    pythonLog("Unknown refcon passed to hotkeyCallback (%p).", inRefcon);
    return;
  }
  set_moduleName(it->second.module_name);
  PyObject *res = PyObject_CallFunctionObjArgs(it->second.callback, it->second.refCon, nullptr);
  PyObject *err = PyErr_Occurred();
  if(err){
    pythonDebug("exception in hotkey callback\n");
    pythonLogException();
  }
  Py_XDECREF(res);  // in case hotkey doesn't happent to return anything
  errCheck("end hotkeyCallback");
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
static PyObject *XPLMRegisterHotKeyFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  errCheck("prior registerHotKey");
  static char *keywords[] = {CHAR("vkey"), CHAR("flags"), CHAR("description"), CHAR("hotKey"), CHAR("refCon"), nullptr};
  (void) self;
  PyObject *inCallback = Py_None, *refcon = Py_None;
  int inVirtualKey, inFlags = 0;
  const char *inDescription;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "i|isOO", keywords, &inVirtualKey, &inFlags, &inDescription, &inCallback, &refcon)){
    return nullptr;
  }
  if (!PyCallable_Check(inCallback)) {
    PyErr_SetString(PyExc_ValueError ,"hotKey() not callable.\n");
    return nullptr;
  }

  XPLMHotKeyID id = XPLMRegisterHotKey(inVirtualKey, inFlags, inDescription, genericHotkeyCallback, (void*)refcon);
  //Store the callback and original refcon
  Py_INCREF(inCallback);
  Py_INCREF(refcon);
  hotkeyDict[hotkeyCounter++] = {
    .hotKeyID = id,
    .callback = inCallback,
    .refCon = refcon,
    .module_name = std::string(CurrentPythonModuleName)
  };

  PyObject *hkIDCapsule = makeCapsule(id, "XPLMHotKeyID");

  errCheck("end registerHotKey");
  PyObject *err = PyErr_Occurred();
  if (err) {
    pythonDebug("Error at end of registerHotKey\n");
    pythonLogException();
  }
  return hkIDCapsule;
} 

My_DOCSTR(_unregisterHotKey__doc__, "unregisterHotKey",
          "hotKeyID",
          "hotKeyID:XPLMHotKeyID",
          "None",
          "Unregisters hot key associated with hotKeyID.\n"
          "\n"
          "hotKeyID must be registered to this plugin using registerHotKey()\n"
          "otherwise unregistration will fail.");
static PyObject *XPLMUnregisterHotKeyFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("hotKey"), nullptr};
  (void) self;
  PyObject *hkIDCapsule;
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &hkIDCapsule)){
    return nullptr;
  }

  XPLMHotKeyID hotKeyID = getVoidPtr(hkIDCapsule, "XPLMHotKeyID");

  int found = 0;
  for(auto& pair: hotkeyDict) {
    if (pair.second.hotKeyID == hotKeyID) {
      found = 1;
      XPLMUnregisterHotKey(hotKeyID);
      Py_DECREF(pair.second.callback);
      Py_DECREF(pair.second.refCon);
      hotkeyDict.erase(pair.first);
      deleteCapsule(makeCapsule(hotKeyID, "XPLMAvionicsID"));
      break;
    }
  }

  if (found == 0) {
    PyErr_SetString(PyExc_RuntimeError ,"XPLMUnregisterHotKey couldn't find hotkey ID.\n");
    return nullptr;
  }

  Py_RETURN_NONE;
} 

My_DOCSTR(_countHotKeys__doc__, "countHotKeys",
          "",
          "",
          "int",
          "Return number of hot keys currently defined in the simulator.");
static PyObject *XPLMCountHotKeysFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  return PyLong_FromLong(XPLMCountHotKeys());
} 

My_DOCSTR(_getNthHotKey__doc__, "getNthHotKey",
          "index",
          "index:int",
          "XPLMHotKeyID",
          "Return hotKeyID for (zero-based) Nth hot key defined in sim.");
static PyObject *XPLMGetNthHotKeyFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("index"), nullptr};
  (void) self;
  int inIndex;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "i", keywords, &inIndex)){
    return nullptr;
  }
  return makeCapsule(XPLMGetNthHotKey(inIndex), "XPLMHotKeyID");
} 

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
static PyObject *XPLMGetHotKeyInfoFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  (void) self;
  PyObject *hotKey, *outVirtualKey, *outFlags, *outDescription, *outPlugin;
  int returnValues = 0;
  static char *keywords[] = {CHAR("hotKeyID"), CHAR("vKey"), CHAR("flags"), CHAR("description"), CHAR("plugin"), nullptr};
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "OOOOO", keywords, &hotKey, &outVirtualKey, &outFlags, &outDescription, &outPlugin)) {
    PyErr_Clear();
    returnValues = 1;
    static char *nkeywords[] = {CHAR("hotKeyID"), nullptr};
    if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", nkeywords, &hotKey)) {
      return nullptr;
    }
  }
  XPLMHotKeyID inHotKey = getVoidPtr(hotKey, "XPLMHotKeyID");
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

My_DOCSTR(_setHotKeyCombination__doc__, "setHotKeyCombination",
          "hotKeyID, vKey, flags=0",
          "hotKeyID:XPLMHotKeyID, vKey:int, flags:XPLMKeyFlags=NoFlag",
          "None",
          "Update key combination for given hotKeyID to use vKey and flags");
static PyObject *XPLMSetHotKeyCombinationFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("hotKey"), CHAR("vKey"), CHAR("flags"), nullptr};
  (void) self;
  PyObject *hotKey;
  int inVirtualKey;
  int inFlags = 0;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Oi|i", keywords, &hotKey, &inVirtualKey, &inFlags)){
    return nullptr;
  }
  void *inHotkey = getVoidPtr(hotKey, "XPLMHotKeyID");
  XPLMSetHotKeyCombination(inHotkey, (char)inVirtualKey, inFlags);
  Py_RETURN_NONE;
} 

static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;

  for (auto& pair : drawCallbackDict) {
    DrawCallbackInfo info = pair.second;
    Py_DECREF(info.callback);
    Py_DECREF(info.refCon);
  }
  drawCallbackDict.clear();

  for (auto& pair : keySniffCallbackDict) {
    KeySniffInfo info = pair.second;
    Py_DECREF(info.callback);
    Py_DECREF(info.refCon);
  }
  keySniffCallbackDict.clear();

  for (auto& pair : windowDict) {
    XPLMWindowID winID = (XPLMWindowID) pair.first;
    WindowInfo info = pair.second;
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
    HotKeyInfo info = pair.second;
    Py_DECREF(info.callback);
    Py_DECREF(info.refCon);
  }
  hotkeyDict.clear();

  for (auto& pair : avionicsCallbacksDict) {
    AvionicsCallbackInfo info = pair.second;
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
  }

  return mod;
}

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
  deviceId = PyLong_FromLong(info.deviceID);  /* borrowed -- and should match inDeviceID, which we ignore */
  set_moduleName(info.module_name);
  isBefore = PyLong_FromLong(inIsBefore);/* new */
  struct timespec stop, start;
  clock_gettime(CLOCK_MONOTONIC, &start);
  if (fun != Py_None)
    pRes = PyObject_CallFunctionObjArgs(fun, deviceId, isBefore, refCon, nullptr);
  clock_gettime(CLOCK_MONOTONIC, &stop);
  pluginStats[getPluginIndex()].draw_time += (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_nsec - start.tv_nsec) / 1000;
  Py_DECREF(isBefore);

  if(!pRes){
    err = PyErr_Occurred();
    if (err && fun != Py_None) {
      pythonLogException();
      pythonLog("[%s] Avionics Draw function disabled. %s", CurrentPythonModuleName, objToStr(fun));
      if (inIsBefore) {
        avionicsCallbacksDict[refcon_id].before = Py_None;
      } else {
        avionicsCallbacksDict[refcon_id].after = Py_None;
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
  PyObject *inPhaseObj = PyLong_FromLong(inPhase);
  PyObject *inIsBeforeObj = PyLong_FromLong(inIsBefore);

  if (info.callback != Py_None) {
    struct timespec stop, start;
    clock_gettime(CLOCK_MONOTONIC, &start);
    pRes = PyObject_CallFunctionObjArgs(info.callback, inPhaseObj, inIsBeforeObj, info.refCon, nullptr);
    clock_gettime(CLOCK_MONOTONIC, &stop);
    pluginStats[getPluginIndex()].draw_time += (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_nsec - start.tv_nsec) / 1000;
  }

  Py_DECREF(inPhaseObj);
  Py_DECREF(inIsBeforeObj);
  if(!pRes){
    PyObject *err = PyErr_Occurred();
    if (err && info.callback != Py_None) {
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

int genericKeySnifferCallback(char inChar, XPLMKeyFlags inFlags, char inVirtualKey, void *inRefcon)
{
  int res = 1;
  intptr_t refcon_id = (intptr_t)inRefcon;
  PyObject *pRes = nullptr;
  
  auto it = keySniffCallbackDict.find(refcon_id);
  if (it == keySniffCallbackDict.end()) {
    pythonLog("keySninfferCallback, got unknown inRefcon (%p)!", inRefcon);
    return res;
  }

  const KeySniffInfo& info = it->second;
  set_moduleName(info.module_name);
  if (info.callback != Py_None) {
    PyObject *inCharObj = PyLong_FromLong(inChar);
    PyObject *inFlagsObj = PyLong_FromLong(inFlags);
    PyObject *inVirtualKeyObj = PyLong_FromLong((unsigned int)inVirtualKey);
    pRes = PyObject_CallFunctionObjArgs(info.callback, inCharObj, inFlagsObj, inVirtualKeyObj, info.refCon, nullptr);
    Py_DECREF(inCharObj);
    Py_DECREF(inFlagsObj);
    Py_DECREF(inVirtualKeyObj);
  }

  if(!pRes){
    char *s2 = objToStr(info.callback);
    pythonLog("[%s] Key sniffer callback %s failed.", CurrentPythonModuleName, s2);
    free(s2);
  } else if(PyLong_Check(pRes)){
    res = (int)PyLong_AsLong(pRes);
  } else {
    char *s2 = objToStr(info.callback);
    pythonLog("[%s] Key sniffer callback %s returned a wrong type.", CurrentPythonModuleName, s2);
    free(s2);
  }

  if(PyErr_Occurred()){
    pythonLogException();
  }

  Py_XDECREF(pRes);
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
      PyObject_CallFunctionObjArgs(info.bezel_draw,
                                   ambiantR, ambiantG, ambiantG, info.refCon, nullptr);
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
    PyObject_CallFunctionObjArgs(info.draw, info.refCon, nullptr);
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
    pRes = PyObject_CallFunctionObjArgs(info.brightness, rheoValue, ambiantBrightness, busVoltsRatio, info.refCon, nullptr);
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
    pythonLog("[%s] Avionics Brightness callback %s returned a wrong type (callbacks must return int).", CurrentPythonModuleName, s2);
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
    pRes = PyObject_CallFunctionObjArgs(info.bezel_click, x_obj, y_obj, mouse_obj, info.refCon, nullptr);
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
    pRes = PyObject_CallFunctionObjArgs(info.bezel_rightclick, x_obj, y_obj, mouse_obj, info.refCon, nullptr);
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
    pRes = PyObject_CallFunctionObjArgs(info.bezel_scroll, x_obj, y_obj, wheel_obj, click_obj, info.refCon, nullptr);
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
    clock_gettime(CLOCK_MONOTONIC, &stop);
    pluginStats[getPluginIndex()].draw_time += (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_nsec - start.tv_nsec) / 1000;
    pRes = PyObject_CallFunctionObjArgs(info.bezel_cursor, x_obj, y_obj, info.refCon, nullptr);
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
    pRes = PyObject_CallFunctionObjArgs(info.screen_touch, x_obj, y_obj, mouse_obj, info.refCon, nullptr);
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
    pRes = PyObject_CallFunctionObjArgs(info.screen_righttouch, x_obj, y_obj, mouse_obj, info.refCon, nullptr);
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
    pRes = PyObject_CallFunctionObjArgs(info.screen_scroll, x_obj, y_obj, wheel_obj, clicks_obj, info.refCon, nullptr);
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
  XPLMCursorStatus res = xplm_CursorDefault;;

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
    pRes = PyObject_CallFunctionObjArgs(info.screen_cursor, x_obj, y_obj, info.refCon, nullptr);
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
    pRes = PyObject_CallFunctionObjArgs(info.keyboard, key_obj, flags_obj, vkey_obj, info.refCon, focus_obj, nullptr);
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
