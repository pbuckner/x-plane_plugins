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
  int create;
};
extern std::unordered_map<intptr_t, AvionicsCallbackInfo> avionicsCallbacksDict;

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
extern PyObject *XPLMTakeAvionicsKeyboardFocusFun(PyObject*, PyObject*, PyObject*);
extern PyObject *XPLMDestroyAvionicsFun(PyObject*, PyObject*, PyObject*);
extern PyObject *XPLMCreateAvionicsExFun(PyObject*, PyObject*, PyObject*);
extern PyObject *XPLMGetAvionicsBrightnessRheoFun(PyObject*, PyObject*, PyObject*);
extern PyObject *XPLMSetAvionicsBrightnessRheoFun(PyObject*, PyObject*, PyObject*);
extern PyObject *XPLMGetAvionicsBusVoltsRatioFun(PyObject*, PyObject*, PyObject*);

extern void resetAvionicsCallbacks(void);
