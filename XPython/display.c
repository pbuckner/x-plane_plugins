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
static PyObject *drawCallbackDict;  /* {
                                         drawCallbackCntr1: ([0]module, [1]callback, [2]phase, [3]before, [4]refcon1)
                                         drawCallbackCntr2: ([0]module, [1]callback, [2]phase, [3]before, [4]refcon2)
                                       } */
#define DRAW_MODULE_NAME 0
#define DRAW_CALLBACK 1 
#define DRAW_PHASE 2
#define DRAW_BEFORE 3
#define DRAW_REFCON 4  
static int genericDrawCallback(XPLMDrawingPhase inPhase, int inIsBefore, void *inRefcon);

static intptr_t keySniffCallbackCntr;
static PyObject *keySniffCallbackDict;
#define KEYSNIFF_MODULE_NAME 0
#define KEYSNIFF_CALLBACK 1
#define KEYSNIFF_BEFORE 2
#define KEYSNIFF_REFCON 3
static int genericKeySnifferCallback(char inChar, XPLMKeyFlags inFlags, char inVirtualKey, void *inRefcon);

static PyObject *avionicsCallbacksDict; // key is PyLong(avionicsCallbacksCntr)
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
static PyObject *avionicsCallbacksIDDict; // key is avionicsIDCapsule, value is PyLong(avionicsCallbacksCntr)
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
static PyObject *windowDict;  /* key is windowID ... getPtrRef(id, windowIDCapsules, windowIDRef); */
#define WINDOW_DRAW 0
#define WINDOW_CLICK 1
#define WINDOW_KEY 2
#define WINDOW_CURSOR 3
#define WINDOW_WHEEL 4
#define WINDOW_RIGHTCLICK 5
#define WINDOW_MODULE_NAME 6
static void genericWindowDraw(XPLMWindowID inWindowID, void *inRefcon);
static int genericWindowMouseClick(XPLMWindowID inWindowID, int x, int y, XPLMMouseStatus inMouse, void *inRefcon);
static void genericWindowKey(XPLMWindowID, char inKey, XPLMKeyFlags inFlags, char inVirtualKey, void *inRefcon, int losingFocus);
static XPLMCursorStatus genericWindowCursor(XPLMWindowID inWindowID, int x, int y, void *inRefcon);
static int genericWindowMouseWheel(XPLMWindowID  inWindowID, int x, int y, int wheel, int clicks, void *inRefcon);
static int genericWindowRightClick(XPLMWindowID inWindowID, int x, int y, XPLMMouseStatus inMouse, void *inRefcon);


static PyObject *hotkeyDict;  /* {
                                   hotkeyCntr1:  ([0]callback, [1]refcon, [2]module) )
                                   hotkeyCntr2:  ([0]callback, [1]refcon, [2]module) )
                                 } */
static intptr_t hotkeyCntr;
#define HOTKEY_CALLBACK 0
#define HOTKEY_REFCON 1
#define HOTKEY_MODULE_NAME 2
static PyObject *hotkeyIDDict;  /* {
                                     <capsule HotKeyID1> : hotkeyCntr1
                                     <capsule HotKeyID2> : hotkeyCntr2
                                   } */
static void genericHotkeyCallback(void *inRefcon);

static PyObject *monitorBndsCallback;

PyObject *windowIDCapsules; /* cannot be static as it's used by widgets */
const char *windowIDRef = "XPLMWindowID";

static PyObject *hotkeyIDCapsules;
static const char hotkeyIDRef[] = "XPLMHotkeyID";

static PyObject *avionicsIDCapsules;
static const char avionicsIDRef[] = "XPLMAvionicsID";


void resetHotKeyCallbacks(void) {
  errCheck("prior resethotkey");
  PyObject *hotkeyID, *hotkeyDictKey;
  Py_ssize_t pos = 0;
  while(PyDict_Next(hotkeyIDDict, &pos, &hotkeyID, &hotkeyDictKey)) {
    PyObject *tuple = PyDict_GetItem(hotkeyDict, hotkeyDictKey);
    char *moduleName = objToStr(PyTuple_GetItem(tuple, HOTKEY_MODULE_NAME));
    char *callback = objToStr(PyTuple_GetItem(tuple, HOTKEY_CALLBACK));
    XPLMUnregisterHotKey((XPLMHotKeyID) refToPtr(hotkeyID, hotkeyIDRef));
    pythonDebug("     Reset --     %s - (%s)", moduleName, callback);
    free(moduleName);
    free(callback);
  }
  errCheck("post while resethotkey");
  PyDict_Clear(hotkeyDict);
  PyDict_Clear(hotkeyIDDict);
  errCheck("post reset hotkey");
}

void resetAvionicsCallbacks(void) {
  if(!XPLMUnregisterAvionicsCallbacks_ptr){
    return;
  }
  PyObject *avIDCapsule, *avDictKey;
  Py_ssize_t pos = 0;
  errCheck("prior resetAvionicsCallbacks");
  while(PyDict_Next(avionicsCallbacksIDDict, &pos, &avIDCapsule, &avDictKey)) {
    PyObject *tuple = PyDict_GetItem(avionicsCallbacksDict, avDictKey);
    if (tuple != NULL) {
      char *moduleName = objToStr(PyTuple_GetItem(tuple, AVIONICS_MODULE_NAME));
      char *callback_before = objToStr(PyTuple_GetItem(tuple, AVIONICS_BEFORE));
      char *callback_after = objToStr(PyTuple_GetItem(tuple, AVIONICS_AFTER));
      XPLMAvionicsID avionicsId = refToPtr(avIDCapsule, avionicsIDRef);
      if (PyTuple_GetItem(tuple, AVIONICS_CREATE)) {
        XPLMDestroyAvionics_ptr(avionicsId);
        errCheck("after XPLMDestroyAvionics in reset");
      } else {
        XPLMUnregisterAvionicsCallbacks_ptr(avionicsId);
        errCheck("after XPLMUnregisterAvioniccCallbacks in reset");
      }
      pythonDebug("     Reset --     %s - (%s)", moduleName, callback_before, callback_after);
      free(moduleName);
      free(callback_before);
      free(callback_after);
    } /* else... this ID has no callbacks (possible if just XPLMGetAvionicsHandle() was called) */
  }

  /****** call DestroyAvionics for all non-stock avionics devices *******/

  PyDict_Clear(avionicsCallbacksIDDict);
  PyDict_Clear(avionicsCallbacksDict);
}

void resetDrawCallbacks(void) {
  PyObject *key, *tuple;
  Py_ssize_t pos = 0;
  while(PyDict_Next(drawCallbackDict, &pos, &key, &tuple)) {
    char *moduleName = objToStr(PyTuple_GetItem(tuple, DRAW_MODULE_NAME));
    char *callback = objToStr(PyTuple_GetItem(tuple, DRAW_CALLBACK));
    pythonDebug("     Reset --     %s - (%s)", moduleName, callback);

    XPLMUnregisterDrawCallback(genericDrawCallback,
                               PyLong_AsLong(PyTuple_GetItem(tuple, DRAW_PHASE)),
                               PyLong_AsLong(PyTuple_GetItem(tuple, DRAW_BEFORE)),
                               PyLong_AsVoidPtr(key));
    free(moduleName);
    free(callback);
  }
  PyDict_Clear(drawCallbackDict);
  /* PyDict_Clear(drawCallbackIDDict); */
}

void resetKeySniffCallbacks(void) {
  PyObject *key, *tuple;
  Py_ssize_t pos = 0;
  while(PyDict_Next(keySniffCallbackDict, &pos, &key, &tuple)) {
    char *moduleName = objToStr(PyTuple_GetItem(tuple, KEYSNIFF_MODULE_NAME));
    char *callback = objToStr(PyTuple_GetItem(tuple, KEYSNIFF_CALLBACK));
    pythonDebug("     Reset --     %s - (%s)", moduleName, callback);
    free(moduleName);
    free(callback);
    XPLMUnregisterKeySniffer(genericKeySnifferCallback,
                             PyLong_AsLong(PyTuple_GetItem(tuple, KEYSNIFF_BEFORE)),
                             PyLong_AsVoidPtr(key));
  }
  PyDict_Clear(keySniffCallbackDict);
}

void resetWindows(void) {
  PyObject *key, *tuple;
  Py_ssize_t pos = 0;
  while(PyDict_Next(windowDict, &pos, &key, &tuple)) {
    char *s = objToStr(PyTuple_GetItem(tuple, WINDOW_MODULE_NAME)); /* borrowed */
    pythonDebug("     Reset --     (%s)", s);
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
  Py_DECREF(idx);
  int res = XPLMRegisterDrawCallback(genericDrawCallback, inPhase, inWantsBefore, (void *)drawCallbackCntr);
  if(!res){
    PyErr_SetString(PyExc_RuntimeError ,"XPLMRegisterDrawCallback failed.\n");
    return NULL;
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
  static char *keywords[] = {"deviceId", "before", "after", "refCon",
                             "bezelClick", "bezelRightClick", "bezelScroll", "bezelCursor",
                             "screenTouch", "screenRightTouch", "screenScroll", "screenCursor",
                             "keyboard", NULL};
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

  /* not used with register */
  (void) screenDraw;
  (void) bezelDraw;
  
  if(!XPLMRegisterAvionicsCallbacksEx_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMRegisterAvionicsCallbacksEx is available only in XPLM400 and up.");
    return NULL;
  }

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|OOOOOOOOOOOOO", keywords,
                                  &firstObj, &before, &after, &refcon,
                                  &bezelClick, &bezelRightClick, &bezelScroll, &bezelCursor,
                                  &screenTouch, &screenRightTouch, &screenScroll, &screenCursor,
                                  &keyboard)){
    return NULL;
  }

  /* if (beforeCallback == Py_None && afterCallback == Py_None) { */
  /*   PyErr_SetString(PyExc_RuntimeError ,"XPLMRegisterAviationCallbacksEx: Both before and after callbacks cannot be None."); */
  /*   return NULL; */
  /* } */

  if (firstObj == Py_None) {
    /* deviceID is _required_, so if firstObj isn't specified, this is simply an error */
    return NULL;
  } else if (PyLong_Check(firstObj)) {
    deviceId = PyLong_AsLong(firstObj);
  } else if (PySequence_Check(firstObj)) {
    paramsObj = firstObj;
    if (PySequence_Length(paramsObj) != 13) {
      PyErr_SetString(PyExc_AttributeError ,"registerAvionicsEx tuple did not contain 13 values\n.");
      return NULL;
    }
  } else {
    PyErr_SetString(PyExc_AttributeError ,"registerAvionicsEx could not parse arguments.\n");
    return NULL;
  }

  XPLMCustomizeAvionics_t params;
  params.structSize = sizeof(params);
  if (paramsObj != Py_None) {
    PyObject *paramsTuple = PySequence_Tuple(paramsObj);
    params.deviceId = PyLong_AsLong(PyTuple_GetItem(paramsTuple, 0));
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
    params.deviceId = deviceId;
  }

  params.refcon = (void *)++avionicsCallbacksCntr;
  params.drawCallbackBefore = before != Py_None ? genericAvionicsCallback : NULL;
  params.drawCallbackAfter = after != Py_None ? genericAvionicsCallback : NULL;
  params.bezelClickCallback = bezelClick != Py_None ? genericAvionicsBezelClick : NULL;
  params.bezelRightClickCallback = bezelRightClick != Py_None ? genericAvionicsBezelRightClick : NULL;
  params.bezelScrollCallback = bezelScroll != Py_None ? genericAvionicsBezelScroll : NULL;
  params.bezelCursorCallback = bezelCursor != Py_None ? genericAvionicsBezelCursor : NULL;
  params.screenTouchCallback = screenTouch != Py_None ? genericAvionicsScreenTouch : NULL;
  params.screenRightTouchCallback = screenRightTouch != Py_None ? genericAvionicsScreenRightTouch : NULL;
  params.screenScrollCallback = screenScroll != Py_None ? genericAvionicsScreenScroll : NULL;
  params.screenCursorCallback = screenCursor != Py_None ? genericAvionicsScreenCursor : NULL;
  params.keyboardCallback = keyboard != Py_None ? genericAvionicsKeyboard : NULL;

  PyObject *callbackList[] = {before, after, bezelDraw, screenDraw, bezelClick, bezelRightClick, bezelScroll,
                              bezelCursor, screenTouch, screenRightTouch, screenScroll, screenCursor, keyboard};
  for (int i=0; i< 13; i++) {
    if (callbackList[i] != Py_None) {
      if (PyCallable_Check(callbackList[i])) {
        Py_INCREF(callbackList[i]);
      } else {
        PyErr_SetString(PyExc_ValueError ,"registerAvionicsEx callback is not callable.\n");
        return NULL;
      }
    }
  }

  
  Py_INCREF(refcon);
  
  PyObject *avDictsKey = PyLong_FromLong(avionicsCallbacksCntr);
  if(!avDictsKey){
    PyErr_SetString(PyExc_RuntimeError ,"Couldn't create long.\n");
    return NULL;
  }
  PyObject *cbkTuple = Py_BuildValue("(siOOOOOOOOOOOOOOd)",
                                     CurrentPythonModuleName, deviceId, before, after,
                                     refcon, bezelDraw, screenDraw,
                                     bezelClick, bezelRightClick, bezelScroll, bezelCursor,
                                     screenTouch, screenRightTouch, screenScroll, screenCursor,
                                     keyboard, 0);
  if(!cbkTuple){
    PyErr_SetString(PyExc_RuntimeError ,"XPLMRegisterAvionicsCallbacksEx couldn't create a callback tuple.\n");
    return NULL;
  }

  PyDict_SetItem(avionicsCallbacksDict, avDictsKey, cbkTuple);
    
  XPLMAvionicsID avionicsId = XPLMRegisterAvionicsCallbacksEx_ptr(&params);
  PyObject *avIDCapsule = getPtrRef(avionicsId, avionicsIDCapsules, avionicsIDRef);
  if(!avIDCapsule){
    PyErr_SetString(PyExc_RuntimeError ,"XPLMRegisterAvionicsCallbacksEx failed.\n");
    return NULL;
  }
  PyDict_SetItem(avionicsCallbacksIDDict, avIDCapsule, avDictsKey);

  Py_DECREF(cbkTuple);
  Py_DECREF(avDictsKey);
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
  static char *keywords[] = {"avionicsId", NULL};
  (void) self;
  PyObject *avIDCapsule;

  if(!XPLMUnregisterAvionicsCallbacks_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMUnregisterAvionicsCallbacks is available only in XPLM400 and up.");
    return NULL;
  }

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &avIDCapsule)) {
    return NULL;
  }
  if (avIDCapsule == NULL) {
    PyErr_SetString(PyExc_RuntimeError, "XPLMUnregisterAvionicsCallback bad avionicsID\n");
    return NULL;
  }
  XPLMAvionicsID avionicsId = refToPtr(avIDCapsule, avionicsIDRef);
  XPLMUnregisterAvionicsCallbacks_ptr(avionicsId);

  /* and... remove from data structures */
  PyDict_DelItem(avionicsCallbacksDict, PyDict_GetItem(avionicsCallbacksIDDict, avIDCapsule));
  PyDict_DelItem(avionicsCallbacksIDDict, avIDCapsule);
  Py_DECREF(avIDCapsule);
  removePtrRef(avionicsId, avionicsIDCapsules);

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
  static char *keywords[] = {"deviceID", NULL};
  (void) self;
  int deviceID;
  if (!XPLMGetAvionicsHandle_ptr) {
    PyErr_SetString(PyExc_RuntimeError , "XPLMGetAvionicsHandle is available only in XPLM410 and up.");
    return NULL;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "i", keywords, &deviceID)) {
    return NULL;
  }

  PyObject *avDictsKey = PyLong_FromLong(++avionicsCallbacksCntr);
  if(!avDictsKey){
    PyErr_SetString(PyExc_RuntimeError ,"Couldn't create long.\n");
    return NULL;
  }

  XPLMAvionicsID avionicsId = XPLMGetAvionicsHandle_ptr(deviceID);
  if (avionicsId == NULL) {
    PyErr_SetString(PyExc_RuntimeError ,"XPLMGetAvionicsHandle() returned NULL. (bug filed)\n");
    return NULL;
  }    

  PyObject *avIDCapsule = getPtrRef(avionicsId, avionicsIDCapsules, avionicsIDRef);
  if(!avIDCapsule){
    PyErr_SetString(PyExc_RuntimeError ,"XPLMGetAvionicsHandle failed.\n");
    return NULL;
  }
  PyDict_SetItem(avionicsCallbacksIDDict, avIDCapsule, avDictsKey);
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
  static char *keywords[] = {"avionicsID", NULL};
  (void) self;
  PyObject *avionicsID;
  if (!XPLMIsAvionicsBound_ptr) {
    PyErr_SetString(PyExc_RuntimeError , "XPLMIsAvionicsBound is available only in XPLM410 and up.");
    return NULL;
  }

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &avionicsID)) {
    return NULL;
  }
  void *avionics_id = refToPtr(avionicsID, avionicsIDRef);
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
  static char *keywords[] = {"avionicsID", NULL};
  (void) self;
  PyObject *avionicsID;
  if (!XPLMIsCursorOverAvionics_ptr) {
    PyErr_SetString(PyExc_RuntimeError , "XPLMIsCursorOverAvionics is available only in XPLM410 and up.");
    return NULL;
  }

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &avionicsID)) {
    return NULL;
  }
  void *avionics_id = refToPtr(avionicsID, avionicsIDRef);
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
  static char *keywords[] = {"avionicsID", NULL};
  (void) self;
  PyObject *avionicsID;
  if (!XPLMIsAvionicsPopupVisible_ptr) {
    PyErr_SetString(PyExc_RuntimeError , "XPLMIsAvionicsPopupVisible is available only in XPLM410 and up.");
    return NULL;
  }

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &avionicsID)) {
    return NULL;
  }
  void *avionics_id = refToPtr(avionicsID, avionicsIDRef);
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
  static char *keywords[] = {"avionicsID", NULL};
  (void) self;
  PyObject *avionicsID;
  if (!XPLMIsAvionicsPoppedOut_ptr) {
    PyErr_SetString(PyExc_RuntimeError , "XPLMIsAvionicsPoppedOut is available only in XPLM410 and up.");
    return NULL;
  }

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &avionicsID)) {
    return NULL;
  }
  void *avionics_id = refToPtr(avionicsID, avionicsIDRef);
  return PyLong_FromLong(XPLMIsAvionicsPoppedOut_ptr(avionics_id));
}

My_DOCSTR(_hasAvionicsKeyboardFocus__doc__, "hasAvionicsKeyboardFocus",
          "avionicsID",
          "avionicsID:XPLMAvionicsID",
          "int",
          "Returns 1 (true) if cockpit device has keyboard focus.");
static PyObject *XPLMHasAvionicsKeyboardFocusFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"avionicsID", NULL};
  (void) self;
  PyObject *avionicsID;
  if (!XPLMHasAvionicsKeyboardFocus_ptr) {
    PyErr_SetString(PyExc_RuntimeError , "XPLMHasAvionicsKeyboardFocus is available only in XPLM410 and up.");
    return NULL;
  }

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &avionicsID)) {
    return NULL;
  }
  void *avionics_id = refToPtr(avionicsID, avionicsIDRef);
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
  static char *keywords[] = {"avionicsID", NULL};
  (void) self;
  PyObject *avionicsID;
  if (!XPLMAvionicsNeedsDrawing_ptr) {
    PyErr_SetString(PyExc_RuntimeError , "XPLMAvionicsNeedsDrawing is available only in XPLM410 and up.");
    return NULL;
  }

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &avionicsID)) {
    return NULL;
  }
  void *avionics_id = refToPtr(avionicsID, avionicsIDRef);
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
  static char *keywords[] = {"avionicsID", NULL};
  (void) self;
  PyObject *avionicsID;
  if (!XPLMPopOutAvionics_ptr) {
    PyErr_SetString(PyExc_RuntimeError , "XPLMPopOutAvionics is available only in XPLM410 and up.");
    return NULL;
  }

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &avionicsID)) {
    return NULL;
  }
  void *avionics_id = refToPtr(avionicsID, avionicsIDRef);
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
  static char *keywords[] = {"avionicsID", NULL};
  (void) self;
  PyObject *avionicsID;
  if (!XPLMTakeAvionicsKeyboardFocus_ptr) {
    PyErr_SetString(PyExc_RuntimeError , "XPLMTakeAvionicsKeyboardFocus is available only in XPLM410 and up.");
    return NULL;
  }

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &avionicsID)) {
    return NULL;
  }
  void *avionics_id = refToPtr(avionicsID, avionicsIDRef);
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
  static char *keywords[] = {"avionicsID", NULL};
  (void) self;
  PyObject *avionicsID;
  if (!XPLMDestroyAvionics_ptr) {
    PyErr_SetString(PyExc_RuntimeError , "XPLMDestroyAvionics is available only in XPLM410 and up.");
    return NULL;
  }

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &avionicsID)) {
    return NULL;
  }
  void *avionics_id = refToPtr(avionicsID, avionicsIDRef);
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
  static char *keywords[] = {"avionicsID", NULL};
  (void) self;
  PyObject *avionicsID;
  if (!XPLMGetAvionicsBusVoltsRatio_ptr) {
    PyErr_SetString(PyExc_RuntimeError, "XPLMGetAvionicsBusVoltsRatio is available only in XPLM410 and up.");
    return NULL;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &avionicsID)) {
    return NULL;
  }

  void *avionics_id = refToPtr(avionicsID, avionicsIDRef);
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
  static char *keywords[] = {"avionicsID", NULL};
  (void) self;
  PyObject *avionicsID;
  if (!XPLMGetAvionicsBrightnessRheo_ptr) {
    PyErr_SetString(PyExc_RuntimeError, "XPLMGetAvionicsBrightnessRheo is available only in XPLM410 and up.");
    return NULL;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &avionicsID)) {
    return NULL;
  }

  void *avionics_id = refToPtr(avionicsID, avionicsIDRef);
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
  static char *keywords[] = {"avionicsID", "brightness", NULL};
  (void) self;
  PyObject *avionicsID;
  float brightness=1.0;
  if (!XPLMSetAvionicsBrightnessRheo_ptr) {
    PyErr_SetString(PyExc_RuntimeError, "XPLMSetAvionicsBrightnessRheo is available only in XPLM410 and up.");
    return NULL;
  }

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|f", keywords, &avionicsID, &brightness)) {
    return NULL;
  }

  void *avionics_id = refToPtr(avionicsID, avionicsIDRef);
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
  static char *keywords[] = {"avionicsID", "visible", NULL};
  (void) self;
  PyObject *avionicsID;
  int visible=1;
  if (!XPLMSetAvionicsPopupVisible_ptr) {
    PyErr_SetString(PyExc_RuntimeError , "XPLMSetAvionicsPopupVisible is available only in XPLM410 and up.");
    return NULL;
  }

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|i", keywords, &avionicsID, &visible)) {
    return NULL;
  }
  void *avionics_id = refToPtr(avionicsID, avionicsIDRef);
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
  static char *keywords[] = {"draw", "phase", "after", "refCon", NULL};
  (void) self;
  PyObject *callback;
  int inPhase = xplm_Phase_Window;
  int inWantsBefore = 0;
  PyObject *refcon = Py_None;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|iiO", keywords, &callback, &inPhase, &inWantsBefore, &refcon)){
    return NULL;
  }
  PyObject *argObj = Py_BuildValue("(sOiiO)", CurrentPythonModuleName, callback, inPhase, inWantsBefore, refcon);
  
  PyObject *items = PyDict_Items(drawCallbackDict);  /* new [(key, value), (key, value)] */
  PyObject *iterator = PyObject_GetIter(items); /* new */
  PyObject *item;
  PyObject *key;
  PyObject *value;
  int res = -1;
  while((item = PyIter_Next(iterator))) {  /* new */
    key = PyTuple_GetItem(item, 0);        /* borrowed */
    value = PyTuple_GetItem(item, 1);      /* borrowed */
    Py_DECREF(item);
    if (PyObject_RichCompareBool(value, argObj, Py_EQ)) {
      res = XPLMUnregisterDrawCallback(genericDrawCallback, inPhase, inWantsBefore, PyLong_AsVoidPtr(key));
      break;
    }
  }
  Py_DECREF(argObj);
  if (res == 1) {
    PyDict_DelItem(drawCallbackDict, key);
  } else {
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


static void genericWindowDraw(XPLMWindowID  inWindowID,
                void         *inRefcon)
{
  errCheck("prior genericWindowDraw");
  struct timespec all_stop, all_start;
  clock_gettime(CLOCK_MONOTONIC, &all_start);
  PyObject *pID = getPtrRef(inWindowID, windowIDCapsules, windowIDRef);
  PyObject *pCbks = PyDict_GetItem(windowDict, pID);
  if(pCbks == NULL){
    printf("Unknown window passed to genericWindowDraw (%p).", inWindowID);
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
      XPLMDebugString("NULL window passed to genericWindowKey. Ignoring\n");
      errCheck("no callback, losing focus, genericWindowKey");
      return;
    }
    char *s = objToStr(pID);
    sprintf(msg, "Unknown window passed to genericWindowKey (%p) [%ld] -- losingFocus is %d. pID is %s\n", inWindowID, (long)inWindowID, losingFocus, s);
    free(s);
    pythonLog("%s", msg);
    return;
  }
  PyObject *arg1 = PyLong_FromLong(inKey);
  PyObject *arg2 = PyLong_FromLong(inFlags);
  PyObject *arg3 = PyLong_FromLong((unsigned int)inVirtualKey);
  PyObject *arg4 = PyLong_FromLong(losingFocus);
  /* char *s = objToStr(pID); */
  /* printf("Calling genericWindowKey callback. inWindowID = %p, pPID = %s, losingFocus = %d\n", inWindowID, s, losingFocus); */
  /* free(s); */
  set_moduleName(PyTuple_GetItem(pCbks, WINDOW_MODULE_NAME));
  PyObject *oRes = PyObject_CallFunctionObjArgs(PyTuple_GetItem(pCbks, WINDOW_KEY), pID, arg1, arg2, arg3, inRefcon, arg4, NULL);
  Py_XDECREF(arg1);
  Py_XDECREF(arg2);
  Py_XDECREF(arg3);
  Py_XDECREF(arg4);
  Py_XDECREF(oRes);
  errCheck("end genericWindowKey");
  Py_DECREF(pID);
}

static int genericWindowMouseClick(XPLMWindowID     inWindowID,
                     int              x,
                     int              y,
                     XPLMMouseStatus  inMouse,
                     void            *inRefcon)
{
  errCheck("prior genericWindowMouseClick");
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
    errCheck("no func genericWindowMouseClick");
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
  PyObject *pID = getPtrRef(inWindowID, windowIDCapsules, windowIDRef);
  PyObject *pCbks = PyDict_GetItem(windowDict, pID);
  if(pCbks == NULL){
    printf("Unknown window passed to genericWindowCursor (%p).\n", inWindowID);
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
  PyObject *pID = getPtrRef(inWindowID, windowIDCapsules, windowIDRef);
  PyObject *pCbks = PyDict_GetItem(windowDict, pID);
  if(pCbks == NULL){
    printf("Unknown window passed to genericWindowMouseWheel (%p).\n", inWindowID);
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
  static char *keywords[] = {"screenWidth", "screenHeight", "bezelWidth", "bezelHeight",
    "screenOffsetX", "screenOffsetY", "drawOnDemand",
    "bezelDraw", "screenDraw",
    "bezelClick", "bezelRightClick", "bezelScroll", "bezelCursor",
    "screenTouch", "screenRightTouch", "screenScroll", "screenCursor",
    "keyboard", "brightness",
    "deviceID", "deviceName", "refCon", NULL};


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
  char *deviceIDstr = "deviceID";
  char *deviceName= "deviceName";

  /* not used with create */

  if(!XPLMCreateAvionicsEx_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMCreateAvionicsEx() is available only in XPLM410 and up.");
    return NULL;
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
    return NULL;
  }
  if (firstObj == Py_None) {
    ; /* ``width'' remains as default value */
  } else if (PyLong_Check(firstObj)) {
    screenWidth = PyLong_AsLong(firstObj);
  } else if (PySequence_Check(firstObj)) {
    paramsObj = firstObj;
    if (PySequence_Length(paramsObj) != 22) {
      PyErr_SetString(PyExc_AttributeError ,"createAvionicsEx tuple did not contain 21 values\n.");
      return NULL;
    }
  } else {
    PyErr_SetString(PyExc_AttributeError ,"createAvionicsEx could not parse arguments.\n");
    return NULL;
  }

  XPLMCreateAvionics_t params;
  params.structSize = sizeof(params);
  if(paramsObj != Py_None) {
    PyObject *paramsTuple = PySequence_Tuple(paramsObj);
    params.screenWidth = PyLong_AsLong(PyTuple_GetItem(paramsTuple, 0));
    params.screenHeight = PyLong_AsLong(PyTuple_GetItem(paramsTuple, 1));
    params.bezelWidth = PyLong_AsLong(PyTuple_GetItem(paramsTuple, 2));
    params.bezelHeight = PyLong_AsLong(PyTuple_GetItem(paramsTuple, 3));
    params.screenOffsetX = PyLong_AsLong(PyTuple_GetItem(paramsTuple, 4));
    params.screenOffsetY = PyLong_AsLong(PyTuple_GetItem(paramsTuple, 5));
    params.drawOnDemand = PyLong_AsLong(PyTuple_GetItem(paramsTuple, 6));
    params.deviceID = objToStr(PyTuple_GetItem(paramsTuple, 19));
    params.deviceName = objToStr(PyTuple_GetItem(paramsTuple, 20));

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
    params.screenWidth = screenWidth;
    params.screenHeight = screenHeight;
    params.bezelWidth = bezelWidth;
    params.bezelHeight = bezelHeight;
    params.screenOffsetX = screenOffsetX;
    params.screenOffsetY = screenOffsetY;
    params.drawOnDemand = drawOnDemand;
    params.deviceID = deviceIDstr;
    params.deviceName = deviceName;
  }

  if (params.bezelWidth < (params.screenWidth + params.screenOffsetX) || params.bezelHeight < (params.screenHeight + params.screenOffsetY)) {
    PyErr_SetString(PyExc_ValueError ,"createAvionicsEx() requires bezelWidth >= screenWidth + offsetX; and bezelHeight >= screenHeight + offsetY.\n");
    return NULL;
  }

  params.refcon = (void *) ++avionicsCallbacksCntr;
  params.bezelDrawCallback = bezelDraw != Py_None ? genericAvionicsBezelDraw : NULL;
  params.drawCallback = screenDraw != Py_None ? genericAvionicsScreenDraw : NULL;
  params.bezelClickCallback = bezelClick != Py_None ? genericAvionicsBezelClick : NULL;
  params.bezelRightClickCallback = bezelRightClick != Py_None ? genericAvionicsBezelRightClick : NULL;
  params.bezelScrollCallback = bezelScroll != Py_None ? genericAvionicsBezelScroll : NULL;
  params.bezelCursorCallback = bezelCursor != Py_None ? genericAvionicsBezelCursor : NULL;
  params.screenTouchCallback = screenTouch != Py_None ? genericAvionicsScreenTouch : NULL;
  params.screenRightTouchCallback = screenRightTouch != Py_None ? genericAvionicsScreenRightTouch : NULL;
  params.screenScrollCallback = screenScroll != Py_None ? genericAvionicsScreenScroll : NULL;
  params.screenCursorCallback = screenCursor != Py_None ? genericAvionicsScreenCursor : NULL;
  params.keyboardCallback = keyboard != Py_None ? genericAvionicsKeyboard : NULL;
  params.brightnessCallback = brightness != Py_None ? genericAvionicsBrightness : NULL;

  /* check which are callable & Py_INCREF as appropriate */
  PyObject *callbackList[] = {bezelDraw, screenDraw, bezelClick, bezelRightClick, bezelScroll,
                              bezelCursor, screenTouch, screenRightTouch, screenScroll, screenCursor, keyboard, brightness};
  for (int i=0; i< 12; i++) {
    if (callbackList[i] != Py_None) {
      if (PyCallable_Check(callbackList[i])) {
        Py_INCREF(callbackList[i]);
      } else {
        PyErr_SetString(PyExc_ValueError ,"createAvionicsEx callback is not callable.\n");
        return NULL;
      }
    }
  }

  Py_INCREF(refcon);

  PyObject *avDictsKey = PyLong_FromLong(avionicsCallbacksCntr);
  if(!avDictsKey){
    PyErr_SetString(PyExc_RuntimeError ,"Couldn't create long.\n");
    return NULL;
  }
  PyObject *cbkTuple = Py_BuildValue("(siOOOOOOOOOOOOOOOd)",
                                     CurrentPythonModuleName, 0, Py_None, Py_None,  /* ... deviceID, before, after callbacks */
                                     refcon, bezelDraw, screenDraw,
                                     bezelClick, bezelRightClick, bezelScroll, bezelCursor,
                                     screenTouch, screenRightTouch, screenScroll, screenCursor,
                                     keyboard, brightness, 1);
  if(!cbkTuple){
    PyErr_SetString(PyExc_RuntimeError ,"XPLMCreateAvionicsEx couldn't create a callback tuple.\n");
    return NULL;
  }

  PyDict_SetItem(avionicsCallbacksDict, avDictsKey, cbkTuple);

  XPLMAvionicsID avionicsId = XPLMCreateAvionicsEx_ptr(&params);
  PyObject *avIDCapsule = getPtrRef(avionicsId, avionicsIDCapsules, avionicsIDRef);
  if(!avIDCapsule){
    PyErr_SetString(PyExc_RuntimeError ,"XPLMRegisterAvionicsCallbacksEx failed.\n");
    return NULL;
  }
  PyDict_SetItem(avionicsCallbacksIDDict, avIDCapsule, avDictsKey);

  Py_DECREF(cbkTuple);
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
  params.drawWindowFunc = genericWindowDraw;
  params.handleMouseClickFunc = genericWindowMouseClick;
  params.handleKeyFunc = genericWindowKey;
  params.handleCursorFunc = genericWindowCursor;
  params.handleMouseWheelFunc = genericWindowMouseWheel;
  params.handleRightClickFunc = genericWindowRightClick;

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
  XPLMWindowID id = XPLMCreateWindow(left, top, right, bottom, visible, genericWindowDraw, genericWindowKey, genericWindowMouseClick, refcon);

  PyObject *pID = getPtrRef(id, windowIDCapsules, windowIDRef);
  PyDict_SetItem(windowDict, pID, cbkTuple);
  Py_DECREF(cbkTuple);
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

My_DOCSTR(_getScreenSize__doc__, "getScreenSize",
          "",
          "",
          "tuple[int, int]",
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

My_DOCSTR(_getScreenBoundsGlobal__doc__, "getScreenBoundsGlobal",
          "",
          "",
          "tuple[int, int, int, int]",
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

My_DOCSTR(_getMouseLocation__doc__, "getMouseLocation",
          "",
          "",
          "None | tuple[int, int]",
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

My_DOCSTR(_getMouseLocationGlobal__doc__, "getMouseLocationGlobal",
          "",
          "",
          "None | tuple[int, int]",
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

My_DOCSTR(_getWindowGeometry__doc__, "getWindowGeometry",
          "windowID",
          "windowID:XPLMWindowID",
          "None | tuple[int, int, int, int]",
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
    static char *nkeywords[] = {"windowID", NULL};
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

My_DOCSTR(_getAvionicsGeometry__doc__, "getAvionicsGeometry",
          "avionicsID",
          "avionicsID:XPLMAvionicsID",
          "tuple[int, int, int, int]",
          "Returns geometry (left, top, right, bottom) of popup window\n"
          "in X-Plane coordinate system.");
static PyObject *XPLMGetAvionicsGeometryFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"avionicsID", NULL};
  (void) self;
  PyObject *avionicsID;

  if (!XPLMGetAvionicsGeometry_ptr) {
    PyErr_SetString(PyExc_RuntimeError , "XPLMGetAvionicsGeometry is available only in XPLM410 and up.");
    return NULL;
  }
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &avionicsID)) {
    return NULL;
  }
  int left=0, top=0, right=0, bottom=0;
  XPLMGetAvionicsGeometry_ptr(refToPtr(avionicsID, avionicsIDRef), &left, &top, &right, &bottom);
  return Py_BuildValue("(iiii)", left, top, right, bottom);
}
  
My_DOCSTR(_setWindowGeometry__doc__, "setWindowGeometry",
          "windowID, left, top, right, bottom",
          "windowID:XPLMWindowID, left:int, top:int, right:int, bottom:int",
          "None",
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

My_DOCSTR(_setAvionicsGeometry__doc__, "setAvionicsGeometry",
          "avionicsID, left, top, right, bottom",
          "avionicsID:XPLMAvionicsID, left:int, top:int, right:int, bottom:int",
          "None",
          "Sets window geometry for cockpit device's popup window in X-Plane coordinates.");
static PyObject *XPLMSetAvionicsGeometryFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"avionicsID", "left", "top", "right", "bottom", NULL};
  (void) self;
  PyObject *avionicsID;
  int inLeft, inTop, inRight, inBottom;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Oiiii", keywords, &avionicsID, &inLeft, &inTop, &inRight, &inBottom)){
    return NULL;
  }
  if(!XPLMSetAvionicsGeometry_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMSetAvionicsGeometry is available only in XPLM410 and up.");
    return NULL;
  }
    
  void *inAvionicsID = refToPtr(avionicsID, avionicsIDRef);
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
    static char *nkeywords[] = {"windowID", NULL};
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

My_DOCSTR(_getAvionicsGeometryOS__doc__, "getAvionicsGeometryOS",
          "avionicsID",
          "avionicsID:XPLMAvionicsID",
          "tuple[int, int, int, int]",
          "Returns window geometry for popped-out avionics device (left, top, right, bottom).");
static PyObject *XPLMGetAvionicsGeometryOSFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"avionicsID", NULL};
  (void) self;
  PyObject *avionicsID;
  if(!XPLMGetAvionicsGeometryOS_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMGetAvionicsGeometryOS is available only in XPLM410 and up.");
    return NULL;
  }
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &avionicsID)) {
    return NULL;
  }
  int left=0, top=0, right=0, bottom=0;
  XPLMGetAvionicsGeometryOS_ptr(refToPtr(avionicsID, avionicsIDRef), &left, &top, &right, &bottom);
  return Py_BuildValue("(iiii)", left, top, right, bottom);
}

My_DOCSTR(_setAvionicsGeometryOS__doc__, "setAvionicsGeometryOS",
          "avionicsID, left, top, right, bottom",
          "avionicsID:XPLMAvionicsID, left:int, top:int, right:int, bottom:int",
          "None",
          "Sets window geometry for popped-out avionics.");
static PyObject *XPLMSetAvionicsGeometryOSFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"avionicsID", "left", "top", "right", "bottom", NULL};
  (void) self;
  PyObject *avionicsID;
  int inLeft, inTop, inRight, inBottom;
  if(!XPLMSetAvionicsGeometryOS_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMSetAvionicsGeometryOS is available only in XPLM410 and up.");
    return NULL;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Oiiii", keywords, &avionicsID, &inLeft, &inTop, &inRight, &inBottom)){
    return NULL;
  }
  XPLMSetAvionicsGeometryOS_ptr(refToPtr(avionicsID, avionicsIDRef), inLeft, inTop, inRight, inBottom);
  Py_RETURN_NONE;
}

My_DOCSTR(_setWindowGeometryOS__doc__, "setWindowGeometryOS",
          "windowID, left, top, right, bottom",
          "windowID:XPLMWindowID, left:int, top:int, right:int, bottom:int",
          "None",
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

My_DOCSTR(_getWindowGeometryVR__doc__, "getWindowGeometryVR",
          "windowID",
          "windowID:XPLMWindowID",
          "None | tuple[int, int]",
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

My_DOCSTR(_setWindowGeometryVR__doc__, "setWindowGeometryVR",
          "windowID, width, height",
          "windowID:XPLMWindowID, width:int, height:int",
          "None",
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

My_DOCSTR(_getWindowIsVisible__doc__, "getWindowIsVisible",
          "windowID",
          "windowID:XPLMWindowID",
          "int",
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

My_DOCSTR(_setWindowIsVisible__doc__, "setWindowIsVisible",
          "windowID, visible=1",
          "windowID:XPLMWindowID, visible:int=1",
          "None",
          "Sets window visibility. 1 indicates visible, 0 is not-visible.");
static PyObject *XPLMSetWindowIsVisibleFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"windowID", "visible", NULL};
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

My_DOCSTR(_windowIsPoppedOut__doc__, "windowIsPoppedOut",
          "windowID",
          "windowID:XPLMWindowID",
          "int",
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

My_DOCSTR(_windowIsInVR__doc__, "windowIsInVR",
          "windowID",
          "windowID:XPLMWindowID",
          "int",
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

My_DOCSTR(_setWindowResizingLimits__doc__, "setWindowResizingLimits",
          "windowID, minWidth=0, minHeight=0, maxWidth=10000, maxHeight=10000",
          "windowID:XPLMWindowID, minWidth:int=0, minHeight:int=0, maxWidth:int=10000, maxHeight:int=10000",
          "None",
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

My_DOCSTR(_setWindowTitle__doc__, "setWindowTitle",
          "windowID, title",
          "windowID:XPLMWindowID, title:str",
          "None",
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

My_DOCSTR(_getWindowRefCon__doc__, "getWindowRefCon",
          "windowID",
          "windowID:XPLMWindowID",
          "Any",
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

My_DOCSTR(_setWindowRefCon__doc__, "setWindowRefCon",
          "windowID, refCon",
          "windowID:XPLMWindowID, refCon:Any",
          "None",
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

My_DOCSTR(_takeKeyboardFocus__doc__, "takeKeyboardFocus",
          "windowID",
          "windowID:XPLMWindowID",
          "None",
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

My_DOCSTR(_hasKeyboardFocus__doc__, "hasKeyboardFocus",
          "windowID",
          "windowID: XPLMWindowID",
          "int",
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

My_DOCSTR(_bringWindowToFront__doc__, "bringWindowToFront",
          "windowID",
          "windowID:XPLMWindowID",
          "None",
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

My_DOCSTR(_isWindowInFront__doc__, "isWindowInFront",
          "windowID",
          "windowID:XPLMWindowID",
          "int",
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

static void genericHotkeyCallback(void *inRefcon)
{
  errCheck("prior hotkeyCallback");
  PyObject *pHotkeyDictKey = PyLong_FromVoidPtr(inRefcon);
  PyObject *hotkeyInfo = PyDict_GetItem(hotkeyDict, pHotkeyDictKey);
  Py_DECREF(pHotkeyDictKey);
  if(hotkeyInfo == NULL){
    printf("Unknown refcon passed to hotkeyCallback (%p).\n", inRefcon);
    return;
  }
  set_moduleName(PyTuple_GetItem(hotkeyInfo, HOTKEY_MODULE_NAME));
  PyObject *res = PyObject_CallFunctionObjArgs(PyTuple_GetItem(hotkeyInfo, HOTKEY_CALLBACK),
                                               PyTuple_GetItem(hotkeyInfo, HOTKEY_REFCON),
                                               NULL);
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

  PyObject *hkDictKey = PyLong_FromVoidPtr((void *)++hotkeyCntr);  // new
  //Store the callback and original refcon
  PyDict_SetItem(hotkeyDict, hkDictKey, hkTuple); // does not steal reference to hkTuple

  XPLMHotKeyID id = XPLMRegisterHotKey(inVirtualKey, inFlags, inDescription, genericHotkeyCallback, PyLong_AsVoidPtr(hkDictKey));
  PyObject *hkIDCapsule = getPtrRef(id, hotkeyIDCapsules, hotkeyIDRef);
  //Allows me to identify my unique refcon based on hotkey id 
  PyDict_SetItem(hotkeyIDDict, hkIDCapsule, hkDictKey);  // does not steal reference
  Py_DECREF(hkDictKey);

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
  static char *keywords[] = {"hotKey", NULL};
  (void) self;
  PyObject *hkIDCapsule;
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &hkIDCapsule)){
    return NULL;
  }
  PyObject *hkDictKey = PyDict_GetItem(hotkeyIDDict, hkIDCapsule);
  if(hkDictKey == NULL){
    PyErr_SetString(PyExc_RuntimeError ,"XPLMUnregisterHotKey couldn't find hotkey ID.\n");
    Py_RETURN_NONE;
  }
  PyObject *hkTuple = PyDict_GetItem(hotkeyDict, hkDictKey);
  if(hkTuple == NULL){
    PyErr_SetString(PyExc_RuntimeError ,"XPLMUnregisterHotKey couldn't find refcon.\n");
    Py_RETURN_NONE;
  }
  XPLMHotKeyID *hk = refToPtr(hkIDCapsule, hotkeyIDRef);
  XPLMUnregisterHotKey(hk);
  PyDict_DelItem(hotkeyDict, hkDictKey);
  PyDict_DelItem(hotkeyIDDict, hkIDCapsule);
  removePtrRef(hk, hotkeyIDCapsules);
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
  static char *keywords[] = {"index", NULL};
  (void) self;
  int inIndex;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "i", keywords, &inIndex)){
    return NULL;
  }
  return getPtrRef(XPLMGetNthHotKey(inIndex), hotkeyIDCapsules, hotkeyIDRef);
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

My_DOCSTR(_setHotKeyCombination__doc__, "setHotKeyCombination",
          "hotKeyID, vKey, flags=0",
          "hotKeyID:XPLMHotKeyID, vKey:int, flags:XPLMKeyFlags=NoFlag",
          "None",
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
  /* PyDict_Clear(drawCallbackIDDict); */
  /* Py_DECREF(drawCallbackIDDict); */
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
#if defined(XPLM410)
    PyModule_AddIntConstant(mod, "Device_MCDU_1", 18); // XPLMDeviceID
    PyModule_AddIntConstant(mod, "Device_MCDU_2", 19); // XPLMDeviceID
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
  PyObject *pl = PyLong_FromVoidPtr(inRefcon);/*new*/
  PyObject *err = NULL;
  PyObject *pRes = NULL;
  int res = 0;
  if (pl == NULL) {
    pythonLog("avionicsDrawCallback, can't create PyLong.");
    goto cleanup;
  }

  PyObject *tup = PyDict_GetItem(avionicsCallbacksDict, pl); /*borrowed*/
  if (!tup) {
    pythonLog("avionicsDrawCallback, got unknown inRefcon(%p)!", inRefcon);
    goto cleanup;
  }

  PyObject *fun = PyTuple_GetItem(tup, inIsBefore ? AVIONICS_BEFORE : AVIONICS_AFTER); /* borrowed */
  PyObject *refCon = PyTuple_GetItem(tup, AVIONICS_REFCON);/* borrowed */
  PyObject *deviceId = PyTuple_GetItem(tup, AVIONICS_DEVICE);/* borrowed -- and should match inDeviceID, which we ignore */
  set_moduleName(PyTuple_GetItem(tup, AVIONICS_MODULE_NAME));
  PyObject *isBefore = PyLong_FromLong(inIsBefore);/* new */
  struct timespec stop, start;
  clock_gettime(CLOCK_MONOTONIC, &start);
  if (fun != Py_None)
    pRes = PyObject_CallFunctionObjArgs(fun, deviceId, isBefore, refCon, NULL);
  clock_gettime(CLOCK_MONOTONIC, &stop);
  pluginStats[getPluginIndex(PyTuple_GetItem(tup, AVIONICS_MODULE_NAME))].draw_time += (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_nsec - start.tv_nsec) / 1000;
  Py_DECREF(isBefore);

  if(!pRes){
    err = PyErr_Occurred();
    if (err && fun != Py_None) {
      pythonLogException();
      pythonLog("[%s] Avionics Draw function disabled. %s", CurrentPythonModuleName, objToStr(fun));
      PyTuple_SetItem(tup, inIsBefore ? AVIONICS_BEFORE : AVIONICS_AFTER, Py_None);
      PyDict_SetItem(avionicsCallbacksDict, pl, tup);
    }
    goto cleanup;
  }
  if (inIsBefore) {
    if(!PyLong_Check(pRes)){
      /* _before_ callbacks should return 1 or 0  -- _after_ callback returns are ignored */
      char *s2 = objToStr(fun);
      pythonLog("[%s] Avionics Draw callback %s returned a wrong type ('before' callbacks must return int).", CurrentPythonModuleName, s2);
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
  Py_XDECREF(pl);

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
    pythonLog("drawCallback, can't create PyLong.");
    goto cleanup;
  }else{
    tup = PyDict_GetItem(drawCallbackDict, pl);
  }

  if(!tup){
    pythonLog("drawCallback, got unknown inRefcon (%p)!", inRefcon);
    goto cleanup;
  }
  fun =    PyTuple_GetItem(tup, DRAW_CALLBACK);
  refcon = PyTuple_GetItem(tup, DRAW_REFCON);
  set_moduleName(PyTuple_GetItem(tup, DRAW_MODULE_NAME));
  PyObject *inPhaseObj = PyLong_FromLong(inPhase);
  PyObject *inIsBeforeObj = PyLong_FromLong(inIsBefore);

  struct timespec stop, start;
  clock_gettime(CLOCK_MONOTONIC, &start);
  if (fun != Py_None)
    pRes = PyObject_CallFunctionObjArgs(fun, inPhaseObj, inIsBeforeObj, refcon, NULL);
  clock_gettime(CLOCK_MONOTONIC, &stop);
  pluginStats[getPluginIndex(PyTuple_GetItem(tup, DRAW_MODULE_NAME))].draw_time += (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_nsec - start.tv_nsec) / 1000;

  Py_DECREF(inPhaseObj);
  Py_DECREF(inIsBeforeObj);
  if(!pRes){
    err = PyErr_Occurred();
    if (err && fun != Py_None) {
      pythonLogException();
      pythonLog("[%s] Draw callback %s failed.", CurrentPythonModuleName, objToStr(fun));
      PyTuple_SetItem(tup, DRAW_CALLBACK, Py_None);
      PyDict_SetItem(drawCallbackDict, pl, tup);
    }
    goto cleanup;
  }
  if(!PyLong_Check(pRes)){
    if (inIsBefore) {
      char *s = objToStr(PyTuple_GetItem(tup, DRAW_MODULE_NAME));
      char *s2 = objToStr(fun);
      pythonLog("[%s] Draw callback %s returned a wrong type.", s, s2);
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

  Py_XDECREF(pl);
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
    pythonLog("keySnifferCallback, can't create PyLong.");
    goto cleanup;
  }else{
    tup = PyDict_GetItem(keySniffCallbackDict, pl);
    Py_DECREF(pl);
  }

  if(!tup){
    pythonLog("keySninfferCallback, got unknown inRefcon (%p)!", inRefcon);
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
    pythonLog("[%s] Key sniffer callback %s failed.", CurrentPythonModuleName, s2);
    free(s2);
    goto cleanup;
  }
  if(!PyLong_Check(pRes)){
    char *s2 = objToStr(fun);
    pythonLog("[%s] Key sniffer callback %s returned a wrong type.", CurrentPythonModuleName, s2);
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

static void genericAvionicsBezelDraw(float inAmbiantR, float inAmbiantG, float inAmbiantB, void *inRefcon) {
  struct timespec all_stop, all_start;
  clock_gettime(CLOCK_MONOTONIC, &all_start);
  PyObject *pl = PyLong_FromVoidPtr(inRefcon);/*new*/
  PyObject *err = NULL;
  PyObject *fun = Py_None;
  if (pl == NULL) {
    pythonLog("avionicsBezelDraw, can't create PyLong.");
    goto cleanup;
  }

  PyObject *tup = PyDict_GetItem(avionicsCallbacksDict, pl); /*borrowed*/
  if (!tup) {
    pythonLog("avionicsBezelDraw, got unknown inRefcon(%p)!", inRefcon);
    goto cleanup;
  }

  fun = PyTuple_GetItem(tup, AVIONICS_BEZEL_DRAW); /* borrowed */
  PyObject *refCon = PyTuple_GetItem(tup, AVIONICS_REFCON);/* borrowed */
  set_moduleName(PyTuple_GetItem(tup, AVIONICS_MODULE_NAME));
  struct timespec stop, start;
  clock_gettime(CLOCK_MONOTONIC, &start);
  if (fun != Py_None)
    PyObject_CallFunctionObjArgs(fun,
                                 PyFloat_FromDouble(inAmbiantR),
                                 PyFloat_FromDouble(inAmbiantG),
                                 PyFloat_FromDouble(inAmbiantB), refCon, NULL);
  clock_gettime(CLOCK_MONOTONIC, &stop);
  pluginStats[getPluginIndex(PyTuple_GetItem(tup, AVIONICS_MODULE_NAME))].draw_time += (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_nsec - start.tv_nsec) / 1000;

 cleanup:
  err = PyErr_Occurred();
  if(err && fun != Py_None){
    pythonLogException();
    pythonLog("[%s] Avionics Bezel Draw function disabled. %s", CurrentPythonModuleName, objToStr(fun));
    PyTuple_SetItem(tup, AVIONICS_BEZEL_DRAW, Py_None);
    PyDict_SetItem(avionicsCallbacksDict, pl, tup);
  }

  Py_XDECREF(pl);
  clock_gettime(CLOCK_MONOTONIC, &all_stop);
  pluginStats[0].draw_time += (all_stop.tv_sec - all_start.tv_sec) * 1000000 + (all_stop.tv_nsec - all_start.tv_nsec) / 1000;
}

static void genericAvionicsScreenDraw(void *inRefcon)  {
  struct timespec all_stop, all_start;
  clock_gettime(CLOCK_MONOTONIC, &all_start);
  PyObject *pl = PyLong_FromVoidPtr(inRefcon);/*new*/
  PyObject *err = NULL;
  PyObject *fun = Py_None;
  if (pl == NULL) {
    pythonLog("genericAvionicsScreenDraw, can't create PyLong.");
    goto cleanup;
  }

  PyObject *tup = PyDict_GetItem(avionicsCallbacksDict, pl); /*borrowed*/
  if (!tup) {
    pythonLog("genericAvionicsScreenDraw, got unknown inRefcon(%p)!", inRefcon);
    goto cleanup;
  }

  fun = PyTuple_GetItem(tup, AVIONICS_DRAW); /* borrowed */
  PyObject *refCon = PyTuple_GetItem(tup, AVIONICS_REFCON);/* borrowed */
  set_moduleName(PyTuple_GetItem(tup, AVIONICS_MODULE_NAME));
  struct timespec stop, start;
  clock_gettime(CLOCK_MONOTONIC, &start);
  if (fun != Py_None)
    PyObject_CallFunctionObjArgs(fun, refCon, NULL);
  clock_gettime(CLOCK_MONOTONIC, &stop);
  pluginStats[getPluginIndex(PyTuple_GetItem(tup, AVIONICS_MODULE_NAME))].draw_time += (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_nsec - start.tv_nsec) / 1000;

 cleanup:
  err = PyErr_Occurred();
  if(err && fun != Py_None){
    pythonLogException();
    pythonLog("[%s] ScreenDraw function disabled. %s", CurrentPythonModuleName, objToStr(fun));
    PyTuple_SetItem(tup, AVIONICS_DRAW, Py_None);
    PyDict_SetItem(avionicsCallbacksDict, pl, tup);
  }

  Py_XDECREF(pl);
  clock_gettime(CLOCK_MONOTONIC, &all_stop);
  pluginStats[0].draw_time += (all_stop.tv_sec - all_start.tv_sec) * 1000000 + (all_stop.tv_nsec - all_start.tv_nsec) / 1000;
}

static float genericAvionicsBrightness(float inRheoValue, float inAmbiantBrightness, float inBusVoltsRatio, void *inRefcon)  {
  struct timespec all_stop, all_start;
  clock_gettime(CLOCK_MONOTONIC, &all_start);
  PyObject *err = NULL;
  PyObject *pRes = NULL;
  PyObject *pl = PyLong_FromVoidPtr(inRefcon);/*new*/
  float res = inRheoValue * inAmbiantBrightness; /* ... the default behavior */

  if (pl == NULL) {
    pythonLog("genericAvionicsBrightness, can't create PyLong.");
    goto cleanup;
  }

  PyObject *tup = PyDict_GetItem(avionicsCallbacksDict, pl); /*borrowed*/
  if (!tup) {
    pythonLog("genericAvionicsBrightness, got unknown inRefcon(%p)!", inRefcon);
    goto cleanup;
  }

  PyObject *fun = PyTuple_GetItem(tup, AVIONICS_BRIGHTNESS); /* borrowed */
  PyObject *refCon = PyTuple_GetItem(tup, AVIONICS_REFCON);/* borrowed */

  set_moduleName(PyTuple_GetItem(tup, AVIONICS_MODULE_NAME));
  struct timespec stop, start;
  clock_gettime(CLOCK_MONOTONIC, &start);
  if (fun != Py_None)
    pRes = PyObject_CallFunctionObjArgs(fun,
                                        PyFloat_FromDouble(inRheoValue),
                                        PyFloat_FromDouble(inAmbiantBrightness),
                                        PyFloat_FromDouble(inBusVoltsRatio),
                                        refCon, NULL);
  clock_gettime(CLOCK_MONOTONIC, &stop);
  pluginStats[getPluginIndex(PyTuple_GetItem(tup, AVIONICS_MODULE_NAME))].draw_time += (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_nsec - start.tv_nsec) / 1000;

  if(!pRes){
    err = PyErr_Occurred();
    if (err && fun != Py_None) {
      pythonLogException();
      pythonLog("[%s] Avionics Brightness callback %s failed.", CurrentPythonModuleName, objToStr(fun));
      PyTuple_SetItem(tup, AVIONICS_BRIGHTNESS, Py_None);
      PyDict_SetItem(avionicsCallbacksDict, pl, tup);
    }
    goto cleanup;
  }
  if(!PyFloat_Check(pRes)){
    char *s2 = objToStr(fun);
    pythonLog("[%s] Avionics Brightness callback %s returned a wrong type (callbacks must return int).", CurrentPythonModuleName, s2);
    free(s2);
    goto cleanup;
  }
  res = (float)PyFloat_AsDouble(pRes);

 cleanup:
  err = PyErr_Occurred();
  if(err){
    pythonLogException();
  }

  Py_XDECREF(pl);
  clock_gettime(CLOCK_MONOTONIC, &all_stop);
  pluginStats[0].draw_time += (all_stop.tv_sec - all_start.tv_sec) * 1000000 + (all_stop.tv_nsec - all_start.tv_nsec) / 1000;
  return res;
}

static int genericAvionicsBezelClick(int x, int y, XPLMMouseStatus inMouse, void *inRefcon) {
  struct timespec all_stop, all_start;
  clock_gettime(CLOCK_MONOTONIC, &all_start);
  PyObject *pl = PyLong_FromVoidPtr(inRefcon);/*new*/
  PyObject *err = NULL;
  PyObject *pRes = NULL;
  int res = 0;
  if (pl == NULL) {
    pythonLog("avionicsBezelClick, can't create PyLong.");
    goto cleanup;
  }

  PyObject *tup = PyDict_GetItem(avionicsCallbacksDict, pl); /*borrowed*/
  Py_DECREF(pl);
  if (!tup) {
    pythonLog("avionicsBezelClick, got unknown inRefcon(%p)!", inRefcon);
    goto cleanup;
  }
  PyObject *fun = PyTuple_GetItem(tup, AVIONICS_BEZEL_CLICK); /* borrowed */
  PyObject *refCon = PyTuple_GetItem(tup, AVIONICS_REFCON);/* borrowed */
  set_moduleName(PyTuple_GetItem(tup, AVIONICS_MODULE_NAME));

  struct timespec stop, start;
  clock_gettime(CLOCK_MONOTONIC, &start);
  pRes = PyObject_CallFunctionObjArgs(fun, PyLong_FromLong(x), PyLong_FromLong(y), PyLong_FromLong(inMouse), refCon, NULL);
  clock_gettime(CLOCK_MONOTONIC, &stop);
  pluginStats[getPluginIndex(PyTuple_GetItem(tup, AVIONICS_MODULE_NAME))].draw_time += (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_nsec - start.tv_nsec) / 1000;

  if(!pRes){
    char *s2 = objToStr(fun);
    pythonLog("[%s] Avionics Bezel Click callback %s failed.", CurrentPythonModuleName, s2);
    free(s2);
    goto cleanup;
  }
  if(!PyLong_Check(pRes)){
    char *s2 = objToStr(fun);
    pythonLog("[%s] Avionics Bezel Click callback %s returned a wrong type (callbacks must return int).", CurrentPythonModuleName, s2);
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

  clock_gettime(CLOCK_MONOTONIC, &all_stop);
  pluginStats[0].draw_time += (all_stop.tv_sec - all_start.tv_sec) * 1000000 + (all_stop.tv_nsec - all_start.tv_nsec) / 1000;

  return res;
}

static int genericAvionicsBezelRightClick(int x, int y, XPLMMouseStatus inMouse, void *inRefcon) {
  struct timespec all_stop, all_start;
  clock_gettime(CLOCK_MONOTONIC, &all_start);
  PyObject *pl = PyLong_FromVoidPtr(inRefcon);/*new*/
  PyObject *err = NULL;
  PyObject *pRes = NULL;
  int res = 0;
  if (pl == NULL) {
    pythonLog("avionicsBezelRightClick, can't create PyLong.");
    goto cleanup;
  }

  PyObject *tup = PyDict_GetItem(avionicsCallbacksDict, pl); /*borrowed*/
  Py_DECREF(pl);
  if (!tup) {
    pythonLog("avionicsBezelRightClick, got unknown inRefcon(%p)!", inRefcon);
    goto cleanup;
  }
  PyObject *fun = PyTuple_GetItem(tup, AVIONICS_BEZEL_RIGHTCLICK); /* borrowed */
  PyObject *refCon = PyTuple_GetItem(tup, AVIONICS_REFCON);/* borrowed */
  set_moduleName(PyTuple_GetItem(tup, AVIONICS_MODULE_NAME));

  struct timespec stop, start;
  clock_gettime(CLOCK_MONOTONIC, &start);
  pRes = PyObject_CallFunctionObjArgs(fun, PyLong_FromLong(x), PyLong_FromLong(y), PyLong_FromLong(inMouse), refCon, NULL);
  clock_gettime(CLOCK_MONOTONIC, &stop);
  pluginStats[getPluginIndex(PyTuple_GetItem(tup, AVIONICS_MODULE_NAME))].draw_time += (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_nsec - start.tv_nsec) / 1000;

  if(!pRes){
    char *s2 = objToStr(fun);
    pythonLog("[%s] Avionics Bezel Right Click callback %s failed.", CurrentPythonModuleName, s2);
    free(s2);
    goto cleanup;
  }
  if(!PyLong_Check(pRes)){
    char *s2 = objToStr(fun);
    pythonLog("[%s] Avionics Bezel Right Click callback %s returned a wrong type (callbacks must return int).", CurrentPythonModuleName, s2);
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

  clock_gettime(CLOCK_MONOTONIC, &all_stop);
  pluginStats[0].draw_time += (all_stop.tv_sec - all_start.tv_sec) * 1000000 + (all_stop.tv_nsec - all_start.tv_nsec) / 1000;

  return res;
}

static int genericAvionicsBezelScroll(int x, int y, int wheel, int clicks, void *inRefcon) {
  struct timespec all_stop, all_start;
  clock_gettime(CLOCK_MONOTONIC, &all_start);
  PyObject *pl = PyLong_FromVoidPtr(inRefcon);/*new*/
  PyObject *err = NULL;
  PyObject *pRes = NULL;
  int res = 0;
  if (pl == NULL) {
    pythonLog("avionicsBezelScroll, can't create PyLong.");
    goto cleanup;
  }

  PyObject *tup = PyDict_GetItem(avionicsCallbacksDict, pl); /*borrowed*/
  Py_DECREF(pl);
  if (!tup) {
    pythonLog("avionicsBezelScroll, got unknown inRefcon(%p)!", inRefcon);
    goto cleanup;
  }
  PyObject *fun = PyTuple_GetItem(tup, AVIONICS_BEZEL_SCROLL); /* borrowed */
  PyObject *refCon = PyTuple_GetItem(tup, AVIONICS_REFCON);/* borrowed */
  set_moduleName(PyTuple_GetItem(tup, AVIONICS_MODULE_NAME));

  struct timespec stop, start;
  clock_gettime(CLOCK_MONOTONIC, &start);
  pRes = PyObject_CallFunctionObjArgs(fun, PyLong_FromLong(x), PyLong_FromLong(y), PyLong_FromLong(wheel),
                                      PyLong_FromLong(clicks), refCon, NULL);
  clock_gettime(CLOCK_MONOTONIC, &stop);
  pluginStats[getPluginIndex(PyTuple_GetItem(tup, AVIONICS_MODULE_NAME))].draw_time += (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_nsec - start.tv_nsec) / 1000;

  if(!pRes){
    char *s2 = objToStr(fun);
    pythonLog("[%s] Avionics Bezel Scroll callback %s failed.", CurrentPythonModuleName, s2);
    free(s2);
    goto cleanup;
  }
  if(!PyLong_Check(pRes)){
    char *s2 = objToStr(fun);
    pythonLog("[%s] Avionics Bezel Scroll callback %s returned a wrong type (callbacks must return int).", CurrentPythonModuleName, s2);
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

  clock_gettime(CLOCK_MONOTONIC, &all_stop);
  pluginStats[0].draw_time += (all_stop.tv_sec - all_start.tv_sec) * 1000000 + (all_stop.tv_nsec - all_start.tv_nsec) / 1000;

  return res;
}

static XPLMCursorStatus genericAvionicsBezelCursor(int x, int y, void *inRefcon) {
  struct timespec all_stop, all_start;
  clock_gettime(CLOCK_MONOTONIC, &all_start);
  PyObject *pl = PyLong_FromVoidPtr(inRefcon);/*new*/
  PyObject *err = NULL;
  PyObject *pRes = NULL;
  XPLMCursorStatus res = 0;
  if (pl == NULL) {
    pythonLog("avionicsBezelCursor, can't create PyLong.");
    goto cleanup;
  }

  PyObject *tup = PyDict_GetItem(avionicsCallbacksDict, pl); /*borrowed*/
  Py_DECREF(pl);
  if (!tup) {
    pythonLog("avionicsBezelCursor, got unknown inRefcon(%p)!", inRefcon);
    goto cleanup;
  }
  PyObject *fun = PyTuple_GetItem(tup, AVIONICS_BEZEL_CURSOR); /* borrowed */
  PyObject *refCon = PyTuple_GetItem(tup, AVIONICS_REFCON);/* borrowed */
  set_moduleName(PyTuple_GetItem(tup, AVIONICS_MODULE_NAME));

  struct timespec stop, start;
  clock_gettime(CLOCK_MONOTONIC, &start);
  pRes = PyObject_CallFunctionObjArgs(fun, PyLong_FromLong(x), PyLong_FromLong(y), refCon, NULL);
  clock_gettime(CLOCK_MONOTONIC, &stop);
  pluginStats[getPluginIndex(PyTuple_GetItem(tup, AVIONICS_MODULE_NAME))].draw_time += (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_nsec - start.tv_nsec) / 1000;

  if(!pRes){
    char *s2 = objToStr(fun);
    pythonLog("[%s] Avionics Bezel Cursor callback %s failed.", CurrentPythonModuleName, s2);
    free(s2);
    goto cleanup;
  }
  if(!PyLong_Check(pRes)){
    char *s2 = objToStr(fun);
    pythonLog("[%s] Avionics Bezel Cursor callback %s returned a wrong type (callbacks must return int).", CurrentPythonModuleName, s2);
    free(s2);
    goto cleanup;
  }
  res = (XPLMCursorStatus)PyLong_AsLong(pRes);
                                   
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

static int genericAvionicsScreenTouch(int x, int y, int inMouse, void *inRefcon) {
  struct timespec all_stop, all_start;
  clock_gettime(CLOCK_MONOTONIC, &all_start);
  PyObject *pl = PyLong_FromVoidPtr(inRefcon);/*new*/
  PyObject *err = NULL;
  PyObject *pRes = NULL;
  int res = 0;
  if (pl == NULL) {
    pythonLog("avionicsScreenTouch, can't create PyLong.");
    goto cleanup;
  }

  PyObject *tup = PyDict_GetItem(avionicsCallbacksDict, pl); /*borrowed*/
  Py_DECREF(pl);
  if (!tup) {
    pythonLog("avionicsScreenTouch, got unknown inRefcon(%p)!", inRefcon);
    goto cleanup;
  }
  PyObject *fun = PyTuple_GetItem(tup, AVIONICS_SCREEN_TOUCH); /* borrowed */
  PyObject *refCon = PyTuple_GetItem(tup, AVIONICS_REFCON);/* borrowed */
  set_moduleName(PyTuple_GetItem(tup, AVIONICS_MODULE_NAME));

  struct timespec stop, start;
  clock_gettime(CLOCK_MONOTONIC, &start);
  pRes = PyObject_CallFunctionObjArgs(fun, PyLong_FromLong(x), PyLong_FromLong(y), PyLong_FromLong(inMouse), refCon, NULL);
  clock_gettime(CLOCK_MONOTONIC, &stop);
  pluginStats[getPluginIndex(PyTuple_GetItem(tup, AVIONICS_MODULE_NAME))].draw_time += (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_nsec - start.tv_nsec) / 1000;

  if(!pRes){
    char *s2 = objToStr(fun);
    pythonLog("[%s] Avionics Screen Touch callback %s failed.", CurrentPythonModuleName, s2);
    free(s2);
    goto cleanup;
  }
  if(!PyLong_Check(pRes)){
    char *s2 = objToStr(fun);
    pythonLog("[%s] Avionics Screen Touch callback %s returned a wrong type (callbacks must return int).", CurrentPythonModuleName, s2);
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

  clock_gettime(CLOCK_MONOTONIC, &all_stop);
  pluginStats[0].draw_time += (all_stop.tv_sec - all_start.tv_sec) * 1000000 + (all_stop.tv_nsec - all_start.tv_nsec) / 1000;

  return res;
}

static int genericAvionicsScreenRightTouch(int x, int y, int inMouse, void *inRefcon) {
  struct timespec all_stop, all_start;
  clock_gettime(CLOCK_MONOTONIC, &all_start);
  PyObject *pl = PyLong_FromVoidPtr(inRefcon);/*new*/
  PyObject *err = NULL;
  PyObject *pRes = NULL;
  int res = 0;
  if (pl == NULL) {
    pythonLog("avionicsScreenRightTouch, can't create PyLong.");
    goto cleanup;
  }

  PyObject *tup = PyDict_GetItem(avionicsCallbacksDict, pl); /*borrowed*/
  Py_DECREF(pl);
  if (!tup) {
    pythonLog("avionicsScreenRightTouch, got unknown inRefcon(%p)!", inRefcon);
    goto cleanup;
  }
  PyObject *fun = PyTuple_GetItem(tup, AVIONICS_SCREEN_RIGHTTOUCH); /* borrowed */
  PyObject *refCon = PyTuple_GetItem(tup, AVIONICS_REFCON);/* borrowed */
  set_moduleName(PyTuple_GetItem(tup, AVIONICS_MODULE_NAME));

  struct timespec stop, start;
  clock_gettime(CLOCK_MONOTONIC, &start);
  pRes = PyObject_CallFunctionObjArgs(fun, PyLong_FromLong(x), PyLong_FromLong(y), PyLong_FromLong(inMouse), refCon, NULL);
  clock_gettime(CLOCK_MONOTONIC, &stop);
  pluginStats[getPluginIndex(PyTuple_GetItem(tup, AVIONICS_MODULE_NAME))].draw_time += (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_nsec - start.tv_nsec) / 1000;

  if(!pRes){
    char *s2 = objToStr(fun);
    pythonLog("[%s] Avionics Screen Right Touch callback %s failed.", CurrentPythonModuleName, s2);
    free(s2);
    goto cleanup;
  }
  if(!PyLong_Check(pRes)){
    char *s2 = objToStr(fun);
    pythonLog("[%s] Avionics Screen Right Touch callback %s returned a wrong type (callbacks must return int).", CurrentPythonModuleName, s2);
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

  clock_gettime(CLOCK_MONOTONIC, &all_stop);
  pluginStats[0].draw_time += (all_stop.tv_sec - all_start.tv_sec) * 1000000 + (all_stop.tv_nsec - all_start.tv_nsec) / 1000;

  return res;
}

static int genericAvionicsScreenScroll(int x, int y, int wheel, int clicks, void *inRefcon) {
  struct timespec all_stop, all_start;
  clock_gettime(CLOCK_MONOTONIC, &all_start);
  PyObject *pl = PyLong_FromVoidPtr(inRefcon);/*new*/
  PyObject *err = NULL;
  PyObject *pRes = NULL;
  int res = 0;
  if (pl == NULL) {
    pythonLog("avionicsScreenScroll, can't create PyLong.");
    goto cleanup;
  }

  PyObject *tup = PyDict_GetItem(avionicsCallbacksDict, pl); /*borrowed*/
  Py_DECREF(pl);
  if (!tup) {
    pythonLog("avionicsScreenScroll, got unknown inRefcon(%p)!", inRefcon);
    goto cleanup;
  }
  PyObject *fun = PyTuple_GetItem(tup, AVIONICS_SCREEN_SCROLL); /* borrowed */
  PyObject *refCon = PyTuple_GetItem(tup, AVIONICS_REFCON);/* borrowed */
  set_moduleName(PyTuple_GetItem(tup, AVIONICS_MODULE_NAME));

  struct timespec stop, start;
  clock_gettime(CLOCK_MONOTONIC, &start);
  pRes = PyObject_CallFunctionObjArgs(fun, PyLong_FromLong(x), PyLong_FromLong(y),
                                      PyLong_FromLong(wheel), PyLong_FromLong(clicks), refCon, NULL);
  clock_gettime(CLOCK_MONOTONIC, &stop);
  pluginStats[getPluginIndex(PyTuple_GetItem(tup, AVIONICS_MODULE_NAME))].draw_time += (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_nsec - start.tv_nsec) / 1000;

  if(!pRes){
    char *s2 = objToStr(fun);
    pythonLog("[%s] Avionics Screen Scroll callback %s failed.", CurrentPythonModuleName, s2);
    free(s2);
    goto cleanup;
  }
  if(!PyLong_Check(pRes)){
    char *s2 = objToStr(fun);
    pythonLog("[%s] Avionics Screen Scroll callback %s returned a wrong type (callbacks must return int).", CurrentPythonModuleName, s2);
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

  clock_gettime(CLOCK_MONOTONIC, &all_stop);
  pluginStats[0].draw_time += (all_stop.tv_sec - all_start.tv_sec) * 1000000 + (all_stop.tv_nsec - all_start.tv_nsec) / 1000;

  return res;
}

static XPLMCursorStatus genericAvionicsScreenCursor(int x, int y, void *inRefcon) {
  struct timespec all_stop, all_start;
  clock_gettime(CLOCK_MONOTONIC, &all_start);
  PyObject *pl = PyLong_FromVoidPtr(inRefcon);/*new*/
  PyObject *err = NULL;
  PyObject *pRes = NULL;
  XPLMCursorStatus res = 0;
  if (pl == NULL) {
    pythonLog("avionicsScreenCursor, can't create PyLong.");
    goto cleanup;
  }

  PyObject *tup = PyDict_GetItem(avionicsCallbacksDict, pl); /*borrowed*/
  Py_DECREF(pl);
  if (!tup) {
    pythonLog("avionicsScreenCursor, got unknown inRefcon(%p)!", inRefcon);
    goto cleanup;
  }
  PyObject *fun = PyTuple_GetItem(tup, AVIONICS_SCREEN_CURSOR); /* borrowed */
  PyObject *refCon = PyTuple_GetItem(tup, AVIONICS_REFCON);/* borrowed */
  set_moduleName(PyTuple_GetItem(tup, AVIONICS_MODULE_NAME));

  struct timespec stop, start;
  clock_gettime(CLOCK_MONOTONIC, &start);
  pRes = PyObject_CallFunctionObjArgs(fun, PyLong_FromLong(x), PyLong_FromLong(y), refCon, NULL);
  clock_gettime(CLOCK_MONOTONIC, &stop);
  pluginStats[getPluginIndex(PyTuple_GetItem(tup, AVIONICS_MODULE_NAME))].draw_time += (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_nsec - start.tv_nsec) / 1000;

  if(!pRes){
    char *s2 = objToStr(fun);
    pythonLog("[%s] Avionics Screen Cursor callback %s failed.", CurrentPythonModuleName, s2);
    free(s2);
    goto cleanup;
  }
  if(!PyLong_Check(pRes)){
    char *s2 = objToStr(fun);
    pythonLog("[%s] Avionics Screen Cursor callback %s returned a wrong type (callbacks must return int).", CurrentPythonModuleName, s2);
    free(s2);
    goto cleanup;
  }
  res = (XPLMCursorStatus)PyLong_AsLong(pRes);
                                   
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

static int genericAvionicsKeyboard(char inKey, XPLMKeyFlags inFlags, char inVirtualKey, void *inRefcon, int losingFocus) {
  struct timespec all_stop, all_start;
  clock_gettime(CLOCK_MONOTONIC, &all_start);
  PyObject *pl = PyLong_FromVoidPtr(inRefcon);/*new*/
  PyObject *err = NULL;
  PyObject *pRes = NULL;
  int res = 0;
  if (pl == NULL) {
    pythonLog("avionicsKeyboard, can't create PyLong.");
    goto cleanup;
  }

  PyObject *tup = PyDict_GetItem(avionicsCallbacksDict, pl); /*borrowed*/
  Py_DECREF(pl);
  if (!tup) {
    pythonLog("avionicsKeyboard, got unknown inRefcon(%p)!", inRefcon);
    goto cleanup;
  }
  PyObject *fun = PyTuple_GetItem(tup, AVIONICS_KEYBOARD); /* borrowed */
  PyObject *refCon = PyTuple_GetItem(tup, AVIONICS_REFCON);/* borrowed */
  set_moduleName(PyTuple_GetItem(tup, AVIONICS_MODULE_NAME));

  struct timespec stop, start;
  clock_gettime(CLOCK_MONOTONIC, &start);
  pRes = PyObject_CallFunctionObjArgs(fun,
                                      PyLong_FromLong(inKey), PyLong_FromLong(inFlags),
                                      PyLong_FromLong(inVirtualKey), refCon, PyLong_FromLong(losingFocus), NULL);
  clock_gettime(CLOCK_MONOTONIC, &stop);
  pluginStats[getPluginIndex(PyTuple_GetItem(tup, AVIONICS_MODULE_NAME))].draw_time += (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_nsec - start.tv_nsec) / 1000;

  if(!pRes){
    char *s2 = objToStr(fun);
    pythonLog("[%s] Avionics Keyboard callback %s failed.", CurrentPythonModuleName, s2);
    free(s2);
    goto cleanup;
  }
  if(!PyLong_Check(pRes)){
    char *s2 = objToStr(fun);
    pythonLog("[%s] Avionics Keyboard callback %s returned a wrong type (callbacks must return int).", CurrentPythonModuleName, s2);
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

  clock_gettime(CLOCK_MONOTONIC, &all_stop);
  pluginStats[0].draw_time += (all_stop.tv_sec - all_start.tv_sec) * 1000000 + (all_stop.tv_nsec - all_start.tv_nsec) / 1000;
  return res;
}
