#define _GNU_SOURCE 1
#include <Python.h>
#include <sys/time.h>
#include <stdio.h>
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMCamera.h>
#include "camera.h"
#include "utils.h"

static intptr_t camCntr;
static PyObject *camDict;
#define CAMERA_PLUGIN_MODULE_NAME 0
#define CAMERA_HOW_LONG 1
#define CAMERA_CALLBACK 2
#define CAMERA_REFCON 3


void resetCamera(void) {
/* we don't 'reset' camera control on reload.
   Camera is controlled only by the most resent call to XPLMControlCamera.
   Previous controlCamera callbacks are removed.
 */
  XPLMCameraControlDuration duration = -1;
  if (XPLMIsCameraBeingControlled(&duration)) {
    if (duration > 0) {
      pythonLog("[XPPython3] Reset --      Releasing camera\n");
      XPLMDontControlCamera();
    }
  }
  PyDict_Clear(camDict);
}
static int genericCameraControl(XPLMCameraPosition_t *outCameraPosition, int inIsLosingControl, void *inRefcon)
{
  PyObject *err;
  errCheck("error before cameraControl");
  PyObject *ref = PyLong_FromVoidPtr(inRefcon); // new
  PyObject *callbackInfo = PyDict_GetItem(camDict, ref); // borrowed
  Py_DECREF(ref);
  if(callbackInfo == NULL){
    printf("Couldn't find cameraControl callback with id = %p.", inRefcon); 
    return 0;
  }

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


  set_moduleName(PyTuple_GetItem(callbackInfo, CAMERA_PLUGIN_MODULE_NAME));
  PyObject *fun = PyTuple_GetItem(callbackInfo, CAMERA_CALLBACK);
  PyObject *lc = PyLong_FromLong(inIsLosingControl);
  PyObject *refcon = PyTuple_GetItem(callbackInfo, CAMERA_REFCON);
  PyObject *resObj = PyObject_CallFunctionObjArgs(fun, pos, lc, refcon, NULL);  // new
  Py_DECREF(lc);

  err = PyErr_Occurred();
  char msg[1024];
  if(err){
    Py_XDECREF(resObj); // in case we got None
    Py_DECREF(pos);
    char *s = objToStr(fun);
    sprintf(msg, "Error in camera callback [%s] %s", CurrentPythonModuleName, s);
    free(s);
    PyErr_SetString(err, msg);
    pythonLogException();
    return 0;
  }

  if((outCameraPosition != NULL) && !inIsLosingControl){
    PyObject *elem;
    if(PyList_Size(pos) != 7){
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

My_DOCSTR(_controlCamera__doc__, "controlCamera", "howLong=ControlCameraUntilViewChanges, controlFunc=None, refCon=None",
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
  (void) self;
  errCheck("before ControlCamera");
  int inHowLong = xplm_ControlCameraUntilViewChanges;
  PyObject *controlFunc = Py_None;
  PyObject *refcon = Py_None;
  static char *keywords[] = {"howLong", "controlFunc", "refCon", NULL};
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|iOO", keywords, &inHowLong, &controlFunc, &refcon)){
    return NULL;
  }

  if (controlFunc == Py_None) {
    Py_DECREF(controlFunc);
    Py_DECREF(refcon);
    PyErr_SetString(PyExc_ValueError, "Expected non-null value for func in controlCamera()\n");
    Py_RETURN_NONE;
  }
  void *inRefcon = (void *)++camCntr;
  PyObject *refconObj = PyLong_FromVoidPtr(inRefcon);
  PyObject *argsObj = Py_BuildValue("(siOO)", CurrentPythonModuleName, inHowLong, controlFunc, refcon);

  PyDict_SetItem(camDict, refconObj, argsObj);
  Py_DECREF(argsObj);
  Py_DECREF(refconObj);
  XPLMControlCamera(inHowLong, genericCameraControl, inRefcon);
  Py_RETURN_NONE;
}

My_DOCSTR(_dontControlCamera__doc__, "dontControlCamera", "", "Release control of camera.");
static PyObject *XPLMDontControlCameraFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  XPLMDontControlCamera();
  Py_RETURN_NONE;
}

My_DOCSTR(_isCameraBeingControlled__doc__, "isCameraBeingControlled", "",
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

My_DOCSTR(_readCameraPosition__doc__, "readCameraPosition", "",
          "Returns list of seven floats:\n"
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
  PyDict_Clear(camDict);
  Py_DECREF(camDict);
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
  {NULL, NULL, 0, NULL}
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
  NULL,
  NULL,
  NULL,
  NULL
};

PyMODINIT_FUNC
PyInit_XPLMCamera(void)
{
  if(!(camDict = PyDict_New())){
    return NULL;
  }
  PyObject *mod = PyModule_Create(&XPLMCameraModule);
  if(mod){
    PyModule_AddStringConstant(mod, "__author__", "Peter Buckner (pbuck@avnwx.com)");
    PyModule_AddIntConstant(mod, "ControlCameraUntilViewChanges", xplm_ControlCameraUntilViewChanges);
    PyModule_AddIntConstant(mod, "xplm_ControlCameraUntilViewChanges", xplm_ControlCameraUntilViewChanges);
    PyModule_AddIntConstant(mod, "ControlCameraForever", xplm_ControlCameraForever);
    PyModule_AddIntConstant(mod, "xplm_ControlCameraForever", xplm_ControlCameraForever);
  }

  return mod;
}




