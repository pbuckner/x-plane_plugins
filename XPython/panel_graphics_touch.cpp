#define _GNU_SOURCE 1
#define PY_SSIZE_T_CLEAN   /* required for 'y#' (bytes+length) formats below */
#include <Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdint>
#include <unordered_map>
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMPanelGraphics.h>
#include "utils.h"
#include "panel_graphics.h"
#include "plugin_dl.h"
#include "capsules.h"

/* ---- touch zones --------------------------------------------------------- */

/* One Python touch-event handler registered on a window or avionics device.
   A pointer to this struct is handed to X-Plane as the C `ref`, so the
   trampoline gets it back directly; the dict (keyed by the window/avionics
   handle) exists only so we can free/replace a handler on re-registration. */
struct TouchHandlerInfo {
  const char *module_name;
  PyObject *callback;
  PyObject *refcon;
};
static std::unordered_map<void *, TouchHandlerInfo *> touchHandlerDict;

static void touchEventCallback(int identifier, XPLMMouseStatus status, int x, int y,
                               int dx, int dy, int button, void *ref)
{
  TouchHandlerInfo *info = (TouchHandlerInfo *)ref;
  if(!info || !info->callback || info->callback == Py_None){
    return;
  }
  set_moduleName(info->module_name);
  PyObject *args[] = {
    PyLong_FromLong(identifier),
    PyLong_FromLong(status),
    PyLong_FromLong(x),
    PyLong_FromLong(y),
    PyLong_FromLong(dx),
    PyLong_FromLong(dy),
    PyLong_FromLong(button),
    info->refcon
  };
  PyObject *pRes = PyObject_Vectorcall(info->callback, args, 8, nullptr);
  for(int i = 0; i < 7; i++){
    Py_XDECREF(args[i]);
  }
  if(!pRes && PyErr_Occurred()){
    pythonLogException();
    char *s2 = objToStr(info->callback);
    pythonLog("[%s] Touch event callback %s failed.", CurrentPythonModuleName, s2);
    free(s2);
  }
  Py_XDECREF(pRes);
}

/* Drop any existing handler registered on `handle`; if `handler` is not None,
   store a new TouchHandlerInfo and return it (to hand to the SDK as `ref`),
   otherwise return nullptr (meaning: unregister). */
static TouchHandlerInfo *prepareTouchHandler(void *handle, PyObject *handler, PyObject *refcon)
{
  auto it = touchHandlerDict.find(handle);
  if(it != touchHandlerDict.end()){
    TouchHandlerInfo *old = it->second;
    Py_XDECREF(old->callback);
    Py_XDECREF(old->refcon);
    free(old);
    touchHandlerDict.erase(it);
  }
  if(!handler || handler == Py_None){
    return nullptr;
  }
  TouchHandlerInfo *info = (TouchHandlerInfo *)malloc(sizeof(TouchHandlerInfo));
  info->module_name = CurrentPythonModuleName;
  Py_INCREF(handler);
  info->callback = handler;
  Py_INCREF(refcon);
  info->refcon = refcon;
  touchHandlerDict[handle] = info;
  return info;
}

My_DOCSTR(_accumulateTouchZone__doc__, "accumulateTouchZone",
          "type, left, top, right, bottom, command=None, identifier=0",
          "type:int, left:int, top:int, right:int, bottom:int, "
          "command:Optional[XPLMCommandRef], identifier:int",
          "int",
          "Register an interactive touch zone for the current frame. Call this every\n"
          "frame from your avionics drawing callback for each region. type is one of\n"
          "TouchZone_Nothing, TouchZone_Command (fires command on press/release), or\n"
          "TouchZone_Identifier (delivers events to the handler set with\n"
          "avionicsSetTouchEventHandler/windowSetTouchEventHandler, tagged with\n"
          "identifier). Returns True while the zone is being pressed/held (use it for\n"
          "pressed-state feedback). Zones registered later win overlaps.");
static PyObject *XPLMAccumulateTouchZoneFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("type"), CHAR("left"), CHAR("top"), CHAR("right"),
                             CHAR("bottom"), CHAR("command"), CHAR("identifier"), nullptr};
  (void) self;
  int type, left, top, right, bottom;
  PyObject *commandObj = Py_None;
  int identifier = 0;
  if(!XPLMAccumulateTouchZone_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMAccumulateTouchZone is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "iiiii|Oi", keywords, &type,
                                  &left, &top, &right, &bottom, &commandObj, &identifier)){
    return nullptr;
  }
  XPLMTouchZoneSpec_t spec;
  spec.structSize = sizeof(XPLMTouchZoneSpec_t);
  spec.type = type;
  spec.command = nullptr;
  spec.identifier = identifier;
  spec.left = left;
  spec.top = top;
  spec.right = right;
  spec.bottom = bottom;
  if(commandObj && commandObj != Py_None){
    spec.command = getVoidPtr(commandObj, COMMAND_CAPSULE);
    if(!spec.command && PyErr_Occurred()){
      return nullptr;
    }
  }
  int held = XPLMAccumulateTouchZone_ptr(&spec);
  return PyBool_FromLong(held);
}

My_DOCSTR(_avionicsSetTouchEventHandler__doc__, "avionicsSetTouchEventHandler",
          "avionic, handler, refcon=None",
          "avionic:XPLMAvionicsID, handler:Optional[Callable], refcon:Any",
          "None",
          "Register handler to receive touch events for TouchZone_Identifier zones on\n"
          "the given avionics device. handler is called as handler(identifier, status,\n"
          "x, y, dx, dy, button, refcon). Pass handler=None to remove the current\n"
          "handler.");
static PyObject *XPLMAvionicsSetTouchEventHandlerFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("avionic"), CHAR("handler"), CHAR("refcon"), nullptr};
  (void) self;
  PyObject *avionicCapsule;
  PyObject *handler;
  PyObject *refcon = Py_None;
  if(!XPLMAvionicsSetTouchEventHandler_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMAvionicsSetTouchEventHandler is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "OO|O", keywords, &avionicCapsule, &handler, &refcon)){
    return nullptr;
  }
  XPLMAvionicsID avionic = getVoidPtr(avionicCapsule, AVIONICS_CAPSULE);
  if(!avionic && PyErr_Occurred()){
    return nullptr;
  }
  TouchHandlerInfo *info = prepareTouchHandler(avionic, handler, refcon);
  XPLMAvionicsSetTouchEventHandler_ptr(avionic, info ? touchEventCallback : nullptr, info);
  Py_RETURN_NONE;
}

My_DOCSTR(_windowSetTouchEventHandler__doc__, "windowSetTouchEventHandler",
          "window, handler, refcon=None",
          "window:XPLMWindowID, handler:Optional[Callable], refcon:Any",
          "None",
          "Register handler to receive touch events for TouchZone_Identifier zones on\n"
          "the given window. handler is called as handler(identifier, status, x, y, dx,\n"
          "dy, button, refcon). Pass handler=None to remove the current handler.");
static PyObject *XPLMWindowSetTouchEventHandlerFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("window"), CHAR("handler"), CHAR("refcon"), nullptr};
  (void) self;
  PyObject *windowCapsule;
  PyObject *handler;
  PyObject *refcon = Py_None;
  if(!XPLMWindowSetTouchEventHandler_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMWindowSetTouchEventHandler is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "OO|O", keywords, &windowCapsule, &handler, &refcon)){
    return nullptr;
  }
  XPLMWindowID window = getVoidPtr(windowCapsule, WINDOW_CAPSULE);
  if(!window && PyErr_Occurred()){
    return nullptr;
  }
  TouchHandlerInfo *info = prepareTouchHandler(window, handler, refcon);
  XPLMWindowSetTouchEventHandler_ptr(window, info ? touchEventCallback : nullptr, info);
  Py_RETURN_NONE;
}


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
PyMethodDef panelGraphicsTouchMethods[] = {
  {"accumulateTouchZone", (PyCFunction)XPLMAccumulateTouchZoneFun, METH_VARARGS | METH_KEYWORDS, _accumulateTouchZone__doc__},
  {"XPLMAccumulateTouchZone", (PyCFunction)XPLMAccumulateTouchZoneFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"avionicsSetTouchEventHandler", (PyCFunction)XPLMAvionicsSetTouchEventHandlerFun, METH_VARARGS | METH_KEYWORDS, _avionicsSetTouchEventHandler__doc__},
  {"XPLMAvionicsSetTouchEventHandler", (PyCFunction)XPLMAvionicsSetTouchEventHandlerFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"windowSetTouchEventHandler", (PyCFunction)XPLMWindowSetTouchEventHandlerFun, METH_VARARGS | METH_KEYWORDS, _windowSetTouchEventHandler__doc__},
  {"XPLMWindowSetTouchEventHandler", (PyCFunction)XPLMWindowSetTouchEventHandlerFun, METH_VARARGS | METH_KEYWORDS, ""},
  {nullptr, nullptr, 0, nullptr}
};
#pragma GCC diagnostic pop
