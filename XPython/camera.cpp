#define _GNU_SOURCE 1
#include <Python.h>
#include <sys/time.h>
#include <stdio.h>
#include <unordered_map>
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMCamera.h>
#include "camera.h"
#include "utils.h"
#include "cpp_utilities.hpp"

static intptr_t camCntr;
struct CameraInfo {
  std::string module_name;
  int howLong;
  PyObject *callback;
  PyObject *refCon;
};

static std::unordered_map<intptr_t, CameraInfo> camDict;
// #define CAMERA_PLUGIN_MODULE_NAME 0
// #define CAMERA_HOW_LONG 1
// #define CAMERA_CALLBACK 2
// #define CAMERA_REFCON 3


void resetCamera(void) {
/* we don't 'reset' camera control on reload.
   Camera is controlled only by the most resent call to XPLMControlCamera.
   Previous controlCamera callbacks are removed.
 */
  XPLMCameraControlDuration duration = -1;
  if (XPLMIsCameraBeingControlled(&duration)) {
    if (duration > 0) {
      pythonDebug("     Reset --      Releasing camera");
      XPLMDontControlCamera();
    }
  }
  for (auto& pair : camDict) {
    Py_DECREF(pair.second.refCon);
    Py_DECREF(pair.second.callback);
  }
  camDict.clear();
}

static int genericCameraControl(XPLMCameraPosition_t *outCameraPosition, int inIsLosingControl, void *inRefcon)
{
  PyObject *err;
  errCheck("error before cameraControl");

  auto it = camDict.find((intptr_t)inRefcon);
  if(it == camDict.end()){
    pythonLog("Couldn't find cameraControl callback with id = %p.", inRefcon);
    return 0;
  }
  const CameraInfo& info = it->second;

  PyObject *pos;
  if(!inIsLosingControl){
    pos = PyList_New(7);
    PyList_SetItem(pos, 0, PyFloat_FromDouble(outCameraPosition->x));
    PyList_SetItem(pos, 1, PyFloat_FromDouble(outCameraPosition->y));
    PyList_SetItem(pos, 2, PyFloat_FromDouble(outCameraPosition->z));
    PyList_SetItem(pos, 3, PyFloat_FromDouble(outCameraPosition->pitch));
    PyList_SetItem(pos, 4, PyFloat_FromDouble(outCameraPosition->heading));
    PyList_SetItem(pos, 5, PyFloat_FromDouble(outCameraPosition->roll));
    PyList_SetItem(pos, 6, PyFloat_FromDouble(outCameraPosition->zoom));
  }else{
    pos = Py_None;
    Py_INCREF(pos);
  }


  set_moduleName(info.module_name);
  PyObject *fun = info.callback;
  PyObject *lc = PyLong_FromLong(inIsLosingControl);
  PyObject *refcon = info.refCon;
  PyObject *resObj = PyObject_CallFunctionObjArgs(fun, pos, lc, refcon, nullptr);  // new
  Py_DECREF(lc);

  err = PyErr_Occurred();
  char msg[1024];
  if(err){
    Py_XDECREF(resObj); // in case we got None
    Py_DECREF(pos);
    char *s = objToStr(fun);
    snprintf(msg, sizeof(msg), "Error in camera callback [%s] %s", CurrentPythonModuleName, s);
    free(s);
    PyErr_SetString(err, msg);
    pythonLogException();
    return 0;
  }

  if((outCameraPosition != nullptr) && !inIsLosingControl){
    PyObject *elem;
    if(PyList_Size(pos) != 7){
      Py_DECREF(pos);
      Py_XDECREF(resObj);
      PyErr_SetString(PyExc_RuntimeError ,"outCameraPosition must contain 7 floats.\n");
      return -1;
    }

    for(int i = 0; i < 7; ++i){
      elem = PyList_GetItem(pos, i); // borrowed
      switch(i){
        case 0:
          outCameraPosition->x = PyFloat_AsDouble(elem);
          break;
        case 1:
          outCameraPosition->y = PyFloat_AsDouble(elem);
          break;
        case 2:
          outCameraPosition->z = PyFloat_AsDouble(elem);
          break;
        case 3:
          outCameraPosition->pitch = PyFloat_AsDouble(elem);
          break;
        case 4:
          outCameraPosition->heading = PyFloat_AsDouble(elem);
          break;
        case 5:
          outCameraPosition->roll = PyFloat_AsDouble(elem);
          break;
        case 6:
          outCameraPosition->zoom = PyFloat_AsDouble(elem);
          break;
      }
    }
  }
  Py_DECREF(pos);
  int res = PyLong_AsLong(resObj);
  Py_XDECREF(resObj);
  errCheck("error at end of cameraControl");
  return res;
}

My_DOCSTR(_controlCamera__doc__, "controlCamera",
          "howLong=ControlCameraUntilViewChanges, controlFunc=None,refCon=None",
          "howLong:int=ControlCameraUntilViewChanges,"
          "controlFunc:Optional[Callable[[list[float], int, Any], int]]=None,"
          "refCon:Any=None",
          "None",
          "Reposition camera on next drawing cycle.\n"
          "  howLong: 1 = until view changes\n"
          "           2 = forever\n"
          "  controlFunc(position, isLosingControl, refCon) -> int\n"
          "      position: list of floats (or None) you'll update with new values\n"
          "                [x, y, z, pitch, heading, roll, zoom]\n"
          "      isLosingControl: 1 -> you are losing control\n"
          "      refCon: reference constant provided with 'controlCamera' call\n"
          "      Return 1 to reposition, 0 to surrender control.\n"
          "  refCon: reference constant passed to call back func().");

static PyObject *XPLMControlCameraFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("howLong"), CHAR("controlFunc"), CHAR("refCon"), nullptr};

  (void) self;
  errCheck("before ControlCamera");
  int inHowLong = xplm_ControlCameraUntilViewChanges;
  PyObject *controlFunc = Py_None;
  PyObject *refcon = Py_None;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|iOO", keywords, &inHowLong, &controlFunc, &refcon)){
    return nullptr;
  }

  if (controlFunc == Py_None) {
    PyErr_SetString(PyExc_ValueError, "Expected non-null value for func in controlCamera()\n");
    Py_RETURN_NONE;
  }

  Py_INCREF(refcon);
  Py_INCREF(controlFunc);

  camDict.emplace(++camCntr, CameraInfo {
      std::string(CurrentPythonModuleName),
      inHowLong,
      controlFunc,
      refcon});

  XPLMControlCamera(inHowLong, genericCameraControl, (void *)camCntr);
  Py_RETURN_NONE;
}

My_DOCSTR(_dontControlCamera__doc__, "dontControlCamera",
          "",
          "",
          "None",
          "Release control of camera.");
static PyObject *XPLMDontControlCameraFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  XPLMDontControlCamera();
  Py_RETURN_NONE;
}

My_DOCSTR(_isCameraBeingControlled__doc__, "isCameraBeingControlled",
          "",
          "",
          "tuple[int, int]",
          "Returns two integer tuple. (isBeingControlled, howLong).\n\n"
          "isBeingControlled is 1 when camera is being controlled, 0 otherwise.\n"
          "howLong is\n"
          "  1: Until View Changes\n"
          "  2: Forever\n"
          "  value is undefined when isBeingControlled is 0.");
static PyObject *XPLMIsCameraBeingControlledFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  XPLMCameraControlDuration dur;
  int res = XPLMIsCameraBeingControlled(&dur);
  return Py_BuildValue("(ii)", res, dur);
}

My_DOCSTR(_readCameraPosition__doc__, "readCameraPosition",
          "",
          "",
          "tuple[float, float, float, float, float, float, float]",
          "Returns tuple of seven floats:\n"
          " Index   Value   Meaning\n"
          " 0,1,2  x,y,z    Camera's position in OpenGL coordinates\n"
          " 3      pitch    In degrees, 0.0 is flat, positive for nose up.\n"
          " 4      heading  In degrees, 0.0 is true north.\n"
          " 5      roll     In degrees, 0.0 is flat, positive for roll right.\n"
          " 6      zoom     1.0 is normal, 2.0 is 2x zoom (objects appear larger)\n");
static PyObject *XPLMReadCameraPositionFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  XPLMCameraPosition_t pos;
  XPLMReadCameraPosition(&pos);
  return Py_BuildValue("(ddddddd)", pos.x, pos.y, pos.z, pos.pitch, pos.heading, pos.roll, pos.zoom);
}

static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  for (auto& pair : camDict) {
    Py_DECREF(pair.second.callback);
    Py_DECREF(pair.second.refCon);
  }
  camDict.clear();
  Py_RETURN_NONE;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
static PyMethodDef XPLMCameraMethods[] = {
  {"controlCamera", (PyCFunction)XPLMControlCameraFun, METH_VARARGS | METH_KEYWORDS, _controlCamera__doc__},
  {"XPLMControlCamera", (PyCFunction)XPLMControlCameraFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"dontControlCamera", XPLMDontControlCameraFun, METH_NOARGS, _dontControlCamera__doc__},
  {"XPLMDontControlCamera", XPLMDontControlCameraFun, METH_NOARGS, ""},
  {"isCameraBeingControlled", XPLMIsCameraBeingControlledFun, METH_NOARGS, _isCameraBeingControlled__doc__},
  {"XPLMIsCameraBeingControlled", XPLMIsCameraBeingControlledFun, METH_NOARGS, ""},
  {"readCameraPosition", XPLMReadCameraPositionFun, METH_NOARGS, _readCameraPosition__doc__},
  {"XPLMReadCameraPosition", XPLMReadCameraPositionFun, METH_NOARGS, ""},
  {"_cleanup", cleanup, METH_VARARGS, ""},
  {nullptr, nullptr, 0, nullptr}
};
#pragma GCC diagnostic pop


static struct PyModuleDef XPLMCameraModule = {
  PyModuleDef_HEAD_INIT,
  "XPLMCamera",
  "Laminar documentation: \n"
  "   https://developer.x-plane.com/sdk/XPLMCamera/\n"
  "XPPython3 documentation: \n"
  "   https://xppython3.rtfd.io/en/stable/development/modules/camera.html",
  -1,
  XPLMCameraMethods,
  nullptr,
  nullptr,
  nullptr,
  nullptr
};

PyMODINIT_FUNC
PyInit_XPLMCamera(void)
{
  // camDict is now a C++ unordered_map, no need to initialize
  PyObject *mod = PyModule_Create(&XPLMCameraModule);
  if(mod){
    PyModule_AddStringConstant(mod, "__author__", "Peter Buckner (pbuck@xppython3.org)");
    PyModule_AddIntConstant(mod, "ControlCameraUntilViewChanges", xplm_ControlCameraUntilViewChanges); //XPLMCameraControlDuration
    PyModule_AddIntConstant(mod, "xplm_ControlCameraUntilViewChanges", xplm_ControlCameraUntilViewChanges); //XPLMCameraControlDuration
    PyModule_AddIntConstant(mod, "ControlCameraForever", xplm_ControlCameraForever); //XPLMCameraControlDuration
    PyModule_AddIntConstant(mod, "xplm_ControlCameraForever", xplm_ControlCameraForever); //XPLMCameraControlDuration
  }

  return mod;
}
