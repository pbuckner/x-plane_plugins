#pragma once
#include <Python.h>
#include "XPLM/XPLMDisplay.h"

struct AvionicsCallbackInfo {
  const char* module_name;
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
  PyObject *browserLoadFinished;
  PyObject *browserLoadError;
  int create;
  int handleOnly;  /* getAvionicsHandle() stub: no callbacks, never registered with X-Plane */
};
extern std::unordered_map<intptr_t, AvionicsCallbackInfo> avionicsCallbacksDict;

/* Avionics handle indirection
   ---------------------------
   Python never sees the SDK's XPLMAvionicsID. The "XPLMAvionicsID" capsule we hand out wraps an opaque,
   never-reused token which maps to a key in avionicsCallbacksDict; the SDK value lives in that entry's
   `avionicsID`. There is one dict entry (and so one capsule) per _registrant_: each Python script which calls
   registerAvionicsCallbacksEx() / getAvionicsHandle() for a device gets its own, even though they all
   resolve to the same SDK XPLMAvionicsID. If X-Plane hands us a different SDK value (e.g., after we
   re-register a device), only the dict entries are updated and every capsule held by Python stays valid.

   Every function taking a Python XPLMAvionicsID must convert with avionicsSdkId() first. */

/* New reference to the capsule for this dict key (creating the token on first use). */
extern PyObject *makeAvionicsCapsule(intptr_t key);
/* New reference to the existing capsule for this dict key, or None if there is none. */
extern PyObject *avionicsCapsuleForKey(intptr_t key);
/* New reference to the capsule of the created device with this SDK id (or, failing that, any entry with it), or None. */
extern PyObject *avionicsCapsuleForSdk(XPLMAvionicsID sdk);
/* Python capsule -> dict key. Returns 0 _with a Python exception set_ if capsule is None, not an
   XPLMAvionicsID capsule, or refers to a handle which has been released. */
extern intptr_t avionicsKey(PyObject *capsule);
/* Python capsule -> SDK id. Returns NULL _with a Python exception set_ on failure (see avionicsKey). */
extern XPLMAvionicsID avionicsSdkId(PyObject *capsule);
/* Invalidate the handle for this dict key and delete its capsule. Safe to call if there is none. */
extern void releaseAvionicsHandle(intptr_t key);
/* Drop every avionics registration made by this script -- built-in device callbacks and any custom devices
   it created (called when the script is cleaned up). */
extern void removeAvionicsRegistrantsForModule(const char *module_name);

extern PyObject* XPLMRegisterAvionicsCallbacksExFun(PyObject*, PyObject*, PyObject*);
extern PyObject* XPLMUnregisterAvionicsCallbacksFun(PyObject*, PyObject*, PyObject*);
extern PyObject *XPLMAvionicsNeedsDrawingFun(PyObject*, PyObject*, PyObject*);
extern PyObject *XPLMGetAvionicsHandleFun(PyObject*, PyObject*, PyObject*);
extern PyObject *XPLMHasAvionicsKeyboardFocusFun(PyObject*, PyObject*, PyObject*);
extern PyObject *XPLMIsAvionicsBoundFun(PyObject*, PyObject*, PyObject*);
extern PyObject *XPLMIsAvionicsPoppedOutFun(PyObject*, PyObject*, PyObject*);
extern PyObject *XPLMIsAvionicsPopupVisibleFun(PyObject*, PyObject*, PyObject*);
extern PyObject *XPLMIsCursorOverAvionicsFun(PyObject*, PyObject*, PyObject*);
extern PyObject *XPLMPopOutAvionicsFun(PyObject*, PyObject*, PyObject*);
extern PyObject *XPLMSetAvionicsPopupVisibleFun(PyObject*, PyObject*, PyObject*);
extern PyObject *XPLMIsAvionicsMappedToVRFun(PyObject*, PyObject*, PyObject*);
extern PyObject *XPLMSetAvionicsMappedToVRFun(PyObject*, PyObject*, PyObject*);
extern PyObject *XPLMTakeAvionicsKeyboardFocusFun(PyObject*, PyObject*, PyObject*);
extern PyObject *XPLMDestroyAvionicsFun(PyObject*, PyObject*, PyObject*);
extern PyObject *XPLMCreateAvionicsExFun(PyObject*, PyObject*, PyObject*);
extern PyObject *XPLMGetAvionicsBrightnessRheoFun(PyObject*, PyObject*, PyObject*);
extern PyObject *XPLMSetAvionicsBrightnessRheoFun(PyObject*, PyObject*, PyObject*);
extern PyObject *XPLMGetAvionicsBusVoltsRatioFun(PyObject*, PyObject*, PyObject*);
extern PyObject *XPLMAvionicsSetURLFun(PyObject*, PyObject*, PyObject*);
extern PyObject *XPLMAvionicsRefreshFun(PyObject*, PyObject*, PyObject*);
extern PyObject *XPLMAvionicsInjectScriptFun(PyObject*, PyObject*, PyObject*);
extern PyObject *XPLMAvionicsAddBrowserFunctionFun(PyObject*, PyObject*, PyObject*);
extern PyObject *XPLMSetObjectAvionicsFun(PyObject*, PyObject*, PyObject*);
extern PyObject *XPLMClearObjectAvionicsFun(PyObject*, PyObject*, PyObject*);

extern void resetAvionicsCallbacks(void);
extern PyMethodDef displayAvionicsMethods[];
