#define _GNU_SOURCE 1
#include <Python.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdbool.h>

#include <XPLM/XPLMDefs.h>
#include <Widgets/XPWidgetDefs.h>
#include <Widgets/XPWidgets.h>
#include <Widgets/XPWidgetUtils.h>
#include <Widgets/XPStandardWidgets.h>
#include "utils.h"

static PyObject *XPUCreateWidgetsFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *widgetDefs = NULL;
  int inCount;
  PyObject *paramParent, *widgets;
  if(!PyArg_ParseTuple(args, "OiOO", &widgetDefs, &inCount, &paramParent, &widgets)){
    return NULL;
  }
  XPWidgetID inParamParent = refToPtr(paramParent, widgetRefName);
  XPWidgetID *ioWidgets = malloc(sizeof(XPWidgetID) * inCount);
  XPWidgetCreate_t *defs = malloc(sizeof(XPWidgetCreate_t) * inCount);
  if((defs == NULL) || (ioWidgets == NULL)){
    printf("Out of memory");
    return NULL;
  }
  int i;
  PyObject *tmpObjs[inCount];
  for(i = 0; i < inCount; ++i){
    PyObject *defListItem = PyList_GetItem(widgetDefs, i);
    defs[i].left = PyLong_AsLong(PyList_GetItem(defListItem, 0));
    defs[i].top = PyLong_AsLong(PyList_GetItem(defListItem, 1));
    defs[i].right = PyLong_AsLong(PyList_GetItem(defListItem, 2));
    defs[i].bottom = PyLong_AsLong(PyList_GetItem(defListItem, 3));
    defs[i].visible = PyLong_AsLong(PyList_GetItem(defListItem, 4));
    tmpObjs[i] = PyUnicode_AsUTF8String(PyList_GetItem(defListItem, 5));
    char *tmp = PyBytes_AsString(tmpObjs[i]);
    defs[i].descriptor = tmp;
    defs[i].isRoot = PyLong_AsLong(PyList_GetItem(defListItem, 6));
    defs[i].containerIndex = PyLong_AsLong(PyList_GetItem(defListItem, 7));
    defs[i].widgetClass = PyLong_AsLong(PyList_GetItem(defListItem, 8));
  }
  XPUCreateWidgets(defs, inCount, inParamParent, ioWidgets);

  for(i = 0; i < inCount; ++i){
    Py_DECREF(tmpObjs[i]);
    PyObject *tmp = getPtrRef(ioWidgets[i], widgetIDCapsules, widgetRefName);
    PyList_Append(widgets, tmp);
    Py_DECREF(tmp);
  }
  free(ioWidgets);
  free(defs);
  Py_RETURN_NONE;
}

static PyObject *XPUMoveWidgetByFun(PyObject *self, PyObject *args)
{
  (void) self;
  XPWidgetID inWidget;
  int inDeltaX, inDeltaY;
  PyObject *widget = NULL;
  if(!PyArg_ParseTuple(args, "Oii", &widget, &inDeltaX, &inDeltaY)){
    return NULL;
  }
  inWidget = refToPtr(widget, widgetRefName);
  XPUMoveWidgetBy(inWidget, inDeltaX, inDeltaY);
  Py_RETURN_NONE;
}

static PyObject *XPUFixedLayoutFun(PyObject *self, PyObject *args)
{
  (void) self;
  XPWidgetMessage inMessage;
  XPWidgetID inWidget;
  intptr_t inParam1, inParam2;
  PyObject *widget = NULL, *param1 = NULL, *param2 = NULL;
  if(!PyArg_ParseTuple(args, "iOOO", &inMessage, &widget, &param1, &param2)){
    return NULL;
  }
  inWidget = refToPtr(widget, widgetRefName);
  inParam1 = PyLong_AsLong(param1);
  inParam2 = PyLong_AsLong(param2);
 
  int res = XPUFixedLayout(inMessage, inWidget, inParam1, inParam2);
  return PyLong_FromLong(res);
}

static PyObject *XPUSelectIfNeededFun(PyObject *self, PyObject *args)
{
  (void) self;
  XPWidgetMessage inMessage;
  XPWidgetID inWidget;
  intptr_t inParam1, inParam2;
  int inEatClick;
  PyObject *widget = NULL, *param1 = NULL, *param2 = NULL;
  if(!PyArg_ParseTuple(args, "iOOOi", &inMessage, &widget, &param1, &param2, &inEatClick)){
    fprintf(pythonLogFile, "Failed to parse tuple in XPUSelectIfNeeded()\n");
    if(PyErr_Occurred()) {
      PyErr_Print();
    }
    return NULL;
  }

  /* Incoming are messages to widgets.
     XPUSelectIfNeeded() [appears to] raise the widget if required AND THEN RESENDS the
     original message to the widget.

     For the params, we're reading a python object & we'll need to convert them (back)
     to their int form. This is the reverse of widgetCallback() in widgets.c
   */
  inWidget = refToPtr(widget, widgetRefName);
 
  XPKeyState_t keyState;
  XPMouseState_t mouseState;
  XPWidgetGeometryChange_t wChange;
  int res;
  switch (inMessage) {
  case xpMsg_KeyPress:
    keyState.key = PyLong_AsLong(PyList_GetItem(param1, 0));
    keyState.flags = PyLong_AsLong(PyList_GetItem(param1, 1));
    keyState.vkey = PyLong_AsLong(PyList_GetItem(param1, 2));
    inParam1 = (intptr_t) &keyState;
    inParam2 = PyLong_AsLong(param2);
    break;
  case xpMsg_MouseDown:
  case xpMsg_MouseDrag:
  case xpMsg_MouseUp:
  case xpMsg_MouseWheel:
  case xpMsg_CursorAdjust:
    if (PyTuple_Check(param1)) {
      mouseState.x = PyLong_AsLong(PyTuple_GetItem(param1, 0));
      mouseState.y = PyLong_AsLong(PyTuple_GetItem(param1, 1));
      mouseState.button = PyLong_AsLong(PyTuple_GetItem(param1, 2));
      mouseState.delta = PyLong_AsLong(PyTuple_GetItem(param1, 3));
    } else if (PyList_Check(param1)) {
      mouseState.x = PyLong_AsLong(PyList_GetItem(param1, 0));
      mouseState.y = PyLong_AsLong(PyList_GetItem(param1, 1));
      mouseState.button = PyLong_AsLong(PyList_GetItem(param1, 2));
      mouseState.delta = PyLong_AsLong(PyList_GetItem(param1, 3));
    } else {
      fprintf(pythonLogFile, "Don't know what param1 is for message %d: %s ", inMessage, Py_TYPE(param1)->tp_name);
    }
    inParam1 = (intptr_t) &mouseState;
    inParam2 = PyLong_AsLong(param2);
    break;
  case xpMsg_Reshape:
    inParam1 = (intptr_t) refToPtr(param1, widgetRefName);
    if (PyTuple_Check(param2)) {
        wChange.dx = PyLong_AsLong(PyTuple_GetItem(param2, 0));
        wChange.dy = PyLong_AsLong(PyTuple_GetItem(param2, 1));
        wChange.dwidth = PyLong_AsLong(PyTuple_GetItem(param2, 2));
        wChange.dheight = PyLong_AsLong(PyTuple_GetItem(param2, 3));
    } else if (PyList_Check(param2)) {
        wChange.dx = PyLong_AsLong(PyList_GetItem(param2, 0));
        wChange.dy = PyLong_AsLong(PyList_GetItem(param2, 1));
        wChange.dwidth = PyLong_AsLong(PyList_GetItem(param2, 2));
        wChange.dheight = PyLong_AsLong(PyList_GetItem(param2, 3));
    }
    inParam2 = (intptr_t) &wChange;
    break;
  case xpMsg_AcceptChild:
  case xpMsg_LoseChild:
  case xpMsg_AcceptParent:
  case xpMsg_Shown:
  case xpMsg_Hidden:
  case xpMsg_TextFieldChanged:
  case xpMsg_PushButtonPressed:
  case xpMsg_ButtonStateChanged:
    inParam1 = (intptr_t) refToPtr(param1, widgetRefName);
    inParam2 = PyLong_AsLong(param2);
    break;
  case xpMsg_PropertyChanged:
    inParam1 = PyLong_AsLong(param1);
    if (inParam1 >= xpProperty_UserStart) {
      inParam2 = (intptr_t)param2;
    } else {
      inParam2 = PyLong_AsLong(param2);
    }
    break;
  default:
    inParam1 = PyLong_AsLong(param1);
    inParam2 = PyLong_AsLong(param2);
    break;
  }
  
  res = XPUSelectIfNeeded(inMessage, inWidget, inParam1, inParam2, inEatClick);
  return PyLong_FromLong(res);
}

static PyObject *XPUDefocusKeyboardFun(PyObject *self, PyObject *args)
{
  (void) self;
  XPWidgetMessage inMessage;
  XPWidgetID inWidget;
  intptr_t inParam1, inParam2;
  int inEatClick;
  PyObject *widget = NULL, *param1 = NULL, *param2 = NULL;
  if(!PyArg_ParseTuple(args, "iOOOi", &inMessage, &widget, &param1, &param2, &inEatClick)){
    return NULL;
  }
  inWidget = refToPtr(widget, widgetRefName);
  inParam1 = PyLong_AsLong(param1);
  inParam2 = PyLong_AsLong(param2);
 
  int res = XPUDefocusKeyboard(inMessage, inWidget, inParam1, inParam2, inEatClick);
  return PyLong_FromLong(res);
}

static PyObject *XPUDragWidgetFun(PyObject *self, PyObject *args)
{
  (void) self;
  XPWidgetMessage inMessage;
  XPWidgetID inWidget;
  intptr_t inParam1, inParam2;
  int inLeft, inTop, inRight, inBottom;
  PyObject *widget = NULL, *param1 = NULL, *param2 = NULL;
  if(!PyArg_ParseTuple(args, "iOOOiiii", &inMessage, &widget, &param1, &param2, &inLeft, &inTop, &inRight, &inBottom)){
    return NULL;
  }
  inWidget = refToPtr(widget, widgetRefName);
  inParam1 = PyLong_AsLong(param1);
  inParam2 = PyLong_AsLong(param2);
 
  int res = XPUDragWidget(inMessage, inWidget, inParam1, inParam2, inLeft, inTop, inRight, inBottom);
  return PyLong_FromLong(res);
}

static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  Py_RETURN_NONE;
}

static PyMethodDef XPWidgetUtilsMethods[] = {
  {"XPUCreateWidgets", XPUCreateWidgetsFun, METH_VARARGS, ""},
  {"XPUMoveWidgetBy", XPUMoveWidgetByFun, METH_VARARGS, ""},
  {"XPUFixedLayout", XPUFixedLayoutFun, METH_VARARGS, ""},
  {"XPUSelectIfNeeded", XPUSelectIfNeededFun, METH_VARARGS, ""},
  {"XPUDefocusKeyboard", XPUDefocusKeyboardFun, METH_VARARGS, ""},
  {"XPUDragWidget", XPUDragWidgetFun, METH_VARARGS, ""},
  {"cleanup", cleanup, METH_VARARGS, ""},
  {NULL, NULL, 0, NULL}
};


static struct PyModuleDef XPWidgetUtilsModule = {
  PyModuleDef_HEAD_INIT,
  "XPWidgetUtils",
  NULL,
  -1,
  XPWidgetUtilsMethods,
  NULL,
  NULL,
  NULL,
  NULL
};

PyMODINIT_FUNC
PyInit_XPWidgetUtils(void)
{
  PyObject *mod = PyModule_Create(&XPWidgetUtilsModule);
  if(mod){
    PyModule_AddIntConstant(mod, "NO_PARENT", NO_PARENT);
    PyModule_AddIntConstant(mod, "PARAM_PARENT", PARAM_PARENT);
  }

  return mod;
}
  
