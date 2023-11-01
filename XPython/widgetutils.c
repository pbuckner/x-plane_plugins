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
#include "widgetutils.h"

My_DOCSTR(_createWidgets__doc__, "createWidgets", "widgetDefs, parentID=None",
          "This does not work in X-Plane.");
static PyObject *XPUCreateWidgetsFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"widgetDefs", "parentID", "result", NULL};
  (void) self;
  PyObject *widgetDefs = NULL;
  int inCount=0;
  PyObject *paramParent=Py_None, *widgets=Py_None;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|OO", keywords, &widgetDefs, &paramParent, &widgets)){
    if(!PyArg_ParseTuple(args, "OiOO", &widgetDefs, &inCount, &paramParent, &widgets)){
      return NULL;
    }
  }
  if (widgets != Py_None && !PyList_CheckExact(widgets)) {
      PyErr_SetString(PyExc_ValueError , "createWidgets result parameter must be a list");
      return NULL;
  }
  if (widgets == Py_None) {
    widgets = PyList_New(0);
  }
    
  inCount = PySequence_Length(widgetDefs);
  if (inCount <= 0) {
    return Py_None;
  }
  XPWidgetID inParamParent = 0;
  if (paramParent != Py_None) {
    inParamParent = refToPtr(paramParent, widgetRefName);
  }

  XPWidgetID *ioWidgets = malloc(sizeof(XPWidgetID) * inCount);
  XPWidgetCreate_t *defs = malloc(sizeof(XPWidgetCreate_t) * inCount);

  if((defs == NULL) || (ioWidgets == NULL)){
    pythonLog("createWidgets, trying to create %d widgets, Out of memory\n", inCount);
    return Py_None;
  }

  int i;
  PyObject *tmpObjs[inCount];
  for(i = 0; i < inCount; ++i){
    PyObject *defListItem = PySequence_GetItem(widgetDefs, i);
    if (PySequence_Length(defListItem) != 9) {
      char *msg;
      if (-1 == asprintf(&msg, "createWidgets, widgetDefs list, definition #%d contains %lld elements, it must contain 9.\n",
                         i+1,
                         (long long)PySequence_Length(defListItem))) {
        pythonLog("Failed to allocate asprintf memory. Create Widgets failed.\n");
      }
      PyErr_SetString(PyExc_ValueError , msg);
      free(msg);
      free(ioWidgets);
      free(defs);
      return Py_None;
    }
    defs[i].left = PyLong_AsLong(PySequence_GetItem(defListItem, 0));
    defs[i].top = PyLong_AsLong(PySequence_GetItem(defListItem, 2));
    defs[i].right = PyLong_AsLong(PySequence_GetItem(defListItem, 1));
    defs[i].bottom = PyLong_AsLong(PySequence_GetItem(defListItem, 3));
    defs[i].visible = PyLong_AsLong(PySequence_GetItem(defListItem, 4));
    tmpObjs[i] = PyUnicode_AsUTF8String(PySequence_GetItem(defListItem, 5));
    char *tmp = PyBytes_AsString(tmpObjs[i]);
    defs[i].descriptor = tmp;
    defs[i].isRoot = PyLong_AsLong(PySequence_GetItem(defListItem, 6));
    defs[i].containerIndex = PyLong_AsLong(PySequence_GetItem(defListItem, 7));
    defs[i].widgetClass = PyLong_AsLong(PySequence_GetItem(defListItem, 8));
    Py_DECREF(defListItem);
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
  return widgets;
}

My_DOCSTR(_moveWidgetBy__doc__, "moveWidgetBy", "widgetID, dx=0, dy=y",
          "Move widget by amount. +x = right, +y = up");
static PyObject *XPUMoveWidgetByFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"widgetID", "dx", "dy", NULL};
  (void) self;
  XPWidgetID inWidget;
  int inDeltaX=0, inDeltaY=0;
  PyObject *widget = NULL;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|ii", keywords, &widget, &inDeltaX, &inDeltaY)){
    return NULL;
  }
  inWidget = refToPtr(widget, widgetRefName);
  XPUMoveWidgetBy(inWidget, inDeltaX, inDeltaY);
  Py_RETURN_NONE;
}

My_DOCSTR(_fixedLayout__doc__, "fixedLayout", "message, widgetID, param1, param2",
          "Use this as a widgetCallback to have child widgets maintain relative positions\n"
          "\n"
          "Seems to be completely useless with X-Plane 11.55+");
static PyObject *XPUFixedLayoutFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"message", "widgetID", "param1", "param2", NULL};
  (void) self;
  XPWidgetMessage inMessage;
  XPWidgetID inWidget;
  intptr_t inParam1, inParam2;
  PyObject *widget = NULL, *param1 = NULL, *param2 = NULL;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "iOOO", keywords, &inMessage, &widget, &param1, &param2)){
    return NULL;
  }
 
  convertMessagePythonToC(inMessage, widget, param1, param2, &inWidget, &inParam1, &inParam2);
  int res = XPUFixedLayout(inMessage, inWidget, inParam1, inParam2);
  PyObject *ret = PyLong_FromLong(res);
  return ret;
}

void convertMessagePythonToC(XPWidgetMessage msg, PyObject *widget, PyObject *param1, PyObject *param2,
                                    XPWidgetID *widget_ptr, intptr_t *param1_ptr, intptr_t *param2_ptr)
{
  errCheck("prior convertMesssagePythonToC");
  /* generically: */
  *widget_ptr = refToPtr(widget, widgetRefName);

  /* Modifications, based on message type */
  XPKeyState_t *keyState = NULL;
  XPMouseState_t *mouseState = NULL;
  XPWidgetGeometryChange_t *wChange = NULL;
  switch(msg){
  case xpMsg_KeyPress:
    /* keyState = (XPKeyState_t *)inParam1; */
    /* param1 = Py_BuildValue("(iii)", (int)keyState->key, (int)keyState->flags, */
    /*                        (int)keyState->vkey); */
    keyState = malloc(sizeof(XPKeyState_t));
    keyState->key = PyLong_AsLong(PyTuple_GetItem(param1, 0));
    keyState->flags = PyLong_AsLong(PyTuple_GetItem(param1, 1));
    keyState->vkey = PyLong_AsLong(PyTuple_GetItem(param1, 2));
    *param1_ptr = (intptr_t)keyState;
    *param2_ptr = PyLong_AsLong(param2);
    break;

  case xpMsg_MouseDown:
  case xpMsg_MouseDrag:
  case xpMsg_MouseUp:
  case xpMsg_MouseWheel:
  case xpMsg_CursorAdjust:
    /* mouseState = (XPMouseState_t *)inParam1; */
    /* param1 = Py_BuildValue("(iiii)", mouseState->x, mouseState->y, */
    /*                        mouseState->button, mouseState->delta); */
    mouseState = malloc(sizeof(XPMouseState_t));
    mouseState->x = PyLong_AsLong(PyTuple_GetItem(param1, 0));
    mouseState->y = PyLong_AsLong(PyTuple_GetItem(param1, 1));
    mouseState->button = PyLong_AsLong(PyTuple_GetItem(param1, 2));
    mouseState->delta = PyLong_AsLong(PyTuple_GetItem(param1, 3));
    *param1_ptr = (intptr_t)mouseState;
    *param2_ptr = PyLong_AsLong(param2);
    break;

  case xpMsg_Reshape:
    /* param1 =  getPtrRef((void *)inParam1, widgetIDCapsules, widgetRefName); */
    *param1_ptr = (intptr_t) refToPtr(param1, widgetRefName);/*PyCapsule_GetPointer(param1, widgetRefName);*/

    /* wChange = (XPWidgetGeometryChange_t *)inParam2; */
    /* param2 = Py_BuildValue("(iiii)", wChange->dx, wChange->dy, */
    /*                        wChange->dwidth, wChange->dheight); */
    wChange = malloc(sizeof(XPWidgetGeometryChange_t));
    wChange->dx = PyLong_AsLong(PyTuple_GetItem(param2, 0));
    wChange->dy = PyLong_AsLong(PyTuple_GetItem(param2, 1));
    wChange->dwidth = PyLong_AsLong(PyTuple_GetItem(param2, 2));
    wChange->dheight = PyLong_AsLong(PyTuple_GetItem(param2, 3));
    *param2_ptr = (intptr_t)wChange;

    break;
  case xpMsg_AcceptChild:
  case xpMsg_LoseChild:
  case xpMsg_AcceptParent:
  case xpMsg_Shown:
  case xpMsg_Hidden:
  case xpMsg_TextFieldChanged:
  case xpMsg_PushButtonPressed:
  case xpMsg_ButtonStateChanged:
  case xpMsg_ScrollBarSliderPositionChanged:
    *param1_ptr = (intptr_t) refToPtr(param1, widgetRefName);
    *param2_ptr = PyLong_AsLong(param2);
    break;
    
  case xpMsg_PropertyChanged:
    *param1_ptr = PyLong_AsLong(param1);
    // use inParam2 -- it's already python
    /* param2 = (PyObject*)inParam2; */
    *param2_ptr = *param1_ptr >= xpProperty_UserStart ? (intptr_t)param2 : PyLong_AsLong(param2);
    errCheck("Failed to convert param2 pointer for msg %d", msg);
    break;
  default:
    *param1_ptr = PyLong_Check(param1) ? PyLong_AsLong(param1) : (intptr_t) param1;
    *param2_ptr = PyLong_Check(param2) ? PyLong_AsLong(param2) : (intptr_t) param2;
    errCheck("Failed to convert param pointers for msg %d", msg);
    break;
  }
  errCheck("end convertMesssagePythonToC");
}

My_DOCSTR(_selectIfNeeded__doc__, "selectIfNeeded", "message, widgetID, param1, param2, eatClick=1",
          "Call within widget Callback to raise widget, if not already\n"
          "\n"
          "Seems completely useless with X-Plane 11.55+");
static PyObject *XPUSelectIfNeededFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"message", "widgetID", "param1", "param2", "eatClick", NULL};
  (void) self;
  XPWidgetMessage inMessage;
  XPWidgetID inWidget;
  intptr_t inParam1, inParam2;
  int inEatClick=1;
  PyObject *widget = NULL, *param1 = NULL, *param2 = NULL;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "iOOO|i", keywords, &inMessage, &widget, &param1, &param2, &inEatClick)){
    pythonLog("Failed to parse tuple in selectIfNeeded()\n");
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
    if (PyTuple_Check(param1) || PyList_Check(param1)) {
      mouseState.x = PyLong_AsLong(PySequence_GetItem(param1, 0));
      mouseState.y = PyLong_AsLong(PySequence_GetItem(param1, 1));
      mouseState.button = PyLong_AsLong(PySequence_GetItem(param1, 2));
      mouseState.delta = PyLong_AsLong(PySequence_GetItem(param1, 3));
    } else {
      pythonLog("Don't know what param1 is for message %d: %s ", inMessage, Py_TYPE(param1)->tp_name);
    }
    inParam1 = (intptr_t) &mouseState;
    inParam2 = PyLong_AsLong(param2);
    break;
  case xpMsg_Reshape:
    inParam1 = (intptr_t) refToPtr(param1, widgetRefName);
    if (PyTuple_Check(param2) || PyList_Check(param2)) {
        wChange.dx = PyLong_AsLong(PySequence_GetItem(param2, 0));
        wChange.dy = PyLong_AsLong(PySequence_GetItem(param2, 1));
        wChange.dwidth = PyLong_AsLong(PySequence_GetItem(param2, 2));
        wChange.dheight = PyLong_AsLong(PySequence_GetItem(param2, 3));
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

My_DOCSTR(_defocusKeyboard__doc__, "defocusKeyboard", "message, widgetID, param1, param2, eatClick=1",
          "Send keyboard focus back to X-Plane\n"
          "\n"
          "Seems completely useless in X-Plane 11.55");
static PyObject *XPUDefocusKeyboardFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"message", "widgetID", "param1", "param2", "eatClick", NULL};
  (void) self;
  XPWidgetMessage inMessage;
  XPWidgetID inWidget;
  intptr_t inParam1, inParam2;
  int inEatClick=1;
  PyObject *widget = NULL, *param1 = NULL, *param2 = NULL;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "iOOO|i", keywords, &inMessage, &widget, &param1, &param2, &inEatClick)){
    return NULL;
  }
  convertMessagePythonToC(inMessage, widget, param1, param2, &inWidget, &inParam1, &inParam2);

  int res = XPUDefocusKeyboard(inMessage, inWidget, inParam1, inParam2, inEatClick);
  return PyLong_FromLong(res);
}

My_DOCSTR(_dragWidget__doc__, "dragWidget", "message, widgetID, param1, param2, left, top, right, bottom",
          "Add to your widget callback to support drag areas\n"
          "\n"
          "(left, top, right, bottom) define area within your widget where, if drag\n"
          "is initiated, this callback will cause the widget to move.");
static PyObject *XPUDragWidgetFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"message", "widgetID", "param1", "param2", "left", "top", "right", "bottom", NULL};
  (void) self;
  XPWidgetMessage inMessage;
  XPWidgetID inWidget;
  intptr_t inParam1, inParam2;
  int inLeft=0, inTop=0, inRight=0, inBottom=0;
  PyObject *widget = NULL, *param1 = NULL, *param2 = NULL;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "iOOOiiii", keywords, &inMessage, &widget, &param1, &param2, &inLeft, &inTop, &inRight, &inBottom)){
    return NULL;
  }

  convertMessagePythonToC(inMessage, widget, param1, param2, &inWidget, &inParam1, &inParam2);

  int res = XPUDragWidget(inMessage, inWidget, inParam1, inParam2, inLeft, inTop, inRight, inBottom);
  return PyLong_FromLong(res);
}

static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  Py_RETURN_NONE;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
static PyMethodDef XPWidgetUtilsMethods[] = {
  {"createWidgets", (PyCFunction)XPUCreateWidgetsFun, METH_VARARGS | METH_KEYWORDS, _createWidgets__doc__},
  {"XPUCreateWidgets", (PyCFunction)XPUCreateWidgetsFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"moveWidgetBy", (PyCFunction)XPUMoveWidgetByFun, METH_VARARGS | METH_KEYWORDS, _moveWidgetBy__doc__},
  {"XPUMoveWidgetBy", (PyCFunction)XPUMoveWidgetByFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"fixedLayout", (PyCFunction)XPUFixedLayoutFun, METH_VARARGS | METH_KEYWORDS, _fixedLayout__doc__},
  {"XPUFixedLayout", (PyCFunction)XPUFixedLayoutFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"selectIfNeeded", (PyCFunction)XPUSelectIfNeededFun, METH_VARARGS | METH_KEYWORDS, _selectIfNeeded__doc__},
  {"XPUSelectIfNeeded", (PyCFunction)XPUSelectIfNeededFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"defocusKeyboard", (PyCFunction)XPUDefocusKeyboardFun, METH_VARARGS | METH_KEYWORDS, _defocusKeyboard__doc__},
  {"XPUDefocusKeyboard", (PyCFunction)XPUDefocusKeyboardFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"dragWidget", (PyCFunction)XPUDragWidgetFun, METH_VARARGS | METH_KEYWORDS, _dragWidget__doc__},
  {"XPUDragWidget", (PyCFunction)XPUDragWidgetFun, METH_VARARGS, ""},
  {"_cleanup", cleanup, METH_VARARGS, ""},
  {NULL, NULL, 0, NULL}
};
#pragma GCC diagnostic pop



static struct PyModuleDef XPWidgetUtilsModule = {
  PyModuleDef_HEAD_INIT,
  "XPWidgetUtils",
  "Laminar documentation: \n"
  "   https://developer.x-plane.com/sdk/XPWidgetUtils/\n"
  "XPPython3 documentation: \n"
  "   https://xppython3.rtfd.io/en/stable/development/modules/widgetutils.html",
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
    PyModule_AddStringConstant(mod, "__author__", "Peter Buckner (pbuck@avnwx.com)");
    PyModule_AddIntConstant(mod, "NO_PARENT", NO_PARENT);
    PyModule_AddIntConstant(mod, "PARAM_PARENT", PARAM_PARENT);

  }

  return mod;
}
  
