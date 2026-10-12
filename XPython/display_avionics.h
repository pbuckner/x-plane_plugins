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
};
extern std::unordered_map<intptr_t, AvionicsCallbackInfo> avionicsCallbacksDict;

/* Avionics handle indirection
   ---------------------------
   Python never sees the SDK's XPLMAvionicsID. The "XPLMAvionicsID" capsule we hand out wraps a pointer to an
   AvionicsHandle, which in turn holds the current SDK value. If X-Plane ever gives us a different
   XPLMAvionicsID for the same logical device (e.g., on re-registering), we update `sdk` via
   avionicsRebind() and every capsule already held by Python stays valid.

   Every function taking a Python XPLMAvionicsID must convert with avionicsSdkId() first. */
struct AvionicsHandle {
  XPLMAvionicsID sdk;
  int deviceID;
};
/* New reference to the capsule for this SDK id (creating the AvionicsHandle if needed). None if sdk is NULL. */
extern PyObject *makeAvionicsCapsule(XPLMAvionicsID sdk, int deviceID);
/* New reference to the existing capsule for this SDK id, or None if we have none (used from X-Plane callbacks). */
extern PyObject *avionicsCapsuleForSdk(XPLMAvionicsID sdk);
/* Python capsule -> SDK id. Returns NULL _with a Python exception set_ if capsule is None, not an
   XPLMAvionicsID capsule, or refers to a handle which has been released. */
extern XPLMAvionicsID avionicsSdkId(PyObject *capsule);
/* Point an existing handle at a new SDK id; also updates avionicsCallbacksDict entries. */
extern void avionicsRebind(PyObject *capsule, XPLMAvionicsID newSdk);
/* Invalidate the handle for this SDK id and delete its capsule. Safe to call if there is none. */
extern void releaseAvionicsHandle(XPLMAvionicsID sdk);

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
