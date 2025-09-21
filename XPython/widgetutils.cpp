#define _GNU_SOURCE 1
#include <Python.h>
#include <sys/time.h>
#include <stdio.h>

#include <XPLM/XPLMDefs.h>
#include <Widgets/XPWidgetDefs.h>
#include <Widgets/XPWidgets.h>
#include <Widgets/XPWidgetUtils.h>
#include <Widgets/XPStandardWidgets.h>
#include "utils.h"
#include "widgets.h"
#include "widgetutils.h"
#include "capsules.h"
#include "cpp_utilities.hpp"

My_DOCSTR(_createWidgets__doc__, "createWidgets",
          "widgetDefs, parentID=None",
          "widgetDefs:Sequence[Sequence[Any]], parentID:Optional[XPWidgetID]",
          "None | list[XPWidgetID]",
          "This does not work in X-Plane.");
static PyObject *XPUCreateWidgetsFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  char *keywords[] = {CHAR("widgetDefs"), CHAR("parentID"), CHAR("result"), nullptr};
  (void) self;
  PyObject *widgetDefs = nullptr;
  int inCount=0;
  PyObject *paramParent=Py_None, *widgets=Py_None;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|OO", keywords, &widgetDefs, &paramParent, &widgets)){
    if(!PyArg_ParseTuple(args, "OiOO", &widgetDefs, &inCount, &paramParent, &widgets)){
      return nullptr;
    }
  }
  if (widgets != Py_None && !PyList_CheckExact(widgets)) {
      PyErr_SetString(PyExc_ValueError , "createWidgets result parameter must be a list");
      return nullptr;
  }
  if (widgets == Py_None) {
    widgets = PyList_New(0);
  }
    
  inCount = PySequence_Length(widgetDefs);
  if (inCount <= 0) {
    Py_RETURN_NONE;
  }
  XPWidgetID inParamParent = 0;
  if (paramParent != Py_None) {
    inParamParent = getVoidPtr(paramParent, "XPWidgetID");
  }

  XPWidgetID *ioWidgets = (XPWidgetID *)malloc(sizeof(XPWidgetID) * inCount);
  XPWidgetCreate_t *defs = (XPWidgetCreate_t *)malloc(sizeof(XPWidgetCreate_t) * inCount);

  if((defs == nullptr) || (ioWidgets == nullptr)){
    pythonLog("createWidgets, trying to create %d widgets, Out of memory", inCount);
    Py_RETURN_NONE;
  }

  int i;
  PyObject **tmpObjs = (PyObject**)malloc(sizeof(PyObject *) * inCount);
  for(i = 0; i < inCount; ++i){
    PyObject *defListItem = PySequence_GetItem(widgetDefs, i);
    if (PySequence_Length(defListItem) != 9) {
      char *msg;
      if (-1 == asprintf(&msg, "createWidgets, widgetDefs list, definition #%d contains %lld elements, it must contain 9.\n",
                         i+1,
                         (long long)PySequence_Length(defListItem))) {
        pythonLog("Failed to allocate asprintf memory. Create Widgets failed.");
      }
      PyErr_SetString(PyExc_ValueError , msg);
      free(msg);
      free(ioWidgets);
      free(defs);
      Py_RETURN_NONE;
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
    PyObject *tmp = makeCapsule(ioWidgets[i], "XPWidgetID");
    PyList_Append(widgets, tmp);
    Py_DECREF(tmp);
  }
  free(ioWidgets);
  free(defs);
  return widgets;
}

My_DOCSTR(_moveWidgetBy__doc__, "moveWidgetBy",
          "widgetID, dx=0, dy=0",
          "widgetID:XPWidgetID, dx:int=0, dy:int=0",
          "None",
          "Move widget by amount. +x = right, +y = up");
static PyObject *XPUMoveWidgetByFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  char *keywords[] = {CHAR("widgetID"), CHAR("dx"), CHAR("dy"), nullptr};
  (void) self;
  XPWidgetID inWidget;
  int inDeltaX=0, inDeltaY=0;
  PyObject *widget = nullptr;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|ii", keywords, &widget, &inDeltaX, &inDeltaY)){
    return nullptr;
  }
  inWidget = getVoidPtr(widget, "XPWidgetID");
  XPUMoveWidgetBy(inWidget, inDeltaX, inDeltaY);
  Py_RETURN_NONE;
}

My_DOCSTR(_fixedLayout__doc__, "fixedLayout",
          "message, widgetID, param1, param2",
          "message:XPWidgetMessage, widgetID:XPWidgetID, param1:int, param2:int",
          "int",
          "Use this as a widgetCallback to have child widgets maintain relative positions\n"
          "\n"
          "Seems to be completely useless with X-Plane 11.55+");
static PyObject *XPUFixedLayoutFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  char *keywords[] = {CHAR("message"), CHAR("widgetID"), CHAR("param1"), CHAR("param2"), nullptr};
  (void) self;
  XPWidgetMessage inMessage;
  XPWidgetID inWidget;
  intptr_t inParam1, inParam2;
  PyObject *widget = nullptr, *param1 = nullptr, *param2 = nullptr;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "iOOO", keywords, &inMessage, &widget, &param1, &param2)){
    return nullptr;
  }

  convertMessagePythonToC(inMessage, widget, param1, param2, &inWidget, &inParam1, &inParam2);
  int res = XPUFixedLayout(inMessage, inWidget, inParam1, inParam2);
  PyObject *ret = PyLong_FromLong(res);
  return ret;
}

void convertMessagePythonToC(XPWidgetMessage msg, PyObject *widget, PyObject *param1, PyObject *param2,
                                    XPWidgetID *widget_ptr, intptr_t *param1_ptr, intptr_t *param2_ptr)
{
  /* generically: */
  *widget_ptr = getVoidPtr(widget, "XPWidgetID");

  /* Modifications, based on message type */
  XPKeyState_t *keyState = nullptr;
  XPMouseState_t *mouseState = nullptr;
  XPWidgetGeometryChange_t *wChange = nullptr;
  switch(msg){
  case xpMsg_KeyPress:
    /* keyState = (XPKeyState_t *)inParam1; */
    /* param1 = Py_BuildValue("(iii)", (int)keyState->key, (int)keyState->flags, */
    /*                        (int)keyState->vkey); */
    keyState = (XPKeyState_t *)malloc(sizeof(XPKeyState_t));
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
    mouseState = (XPMouseState_t *)malloc(sizeof(XPMouseState_t));
    mouseState->x = PyLong_AsLong(PyTuple_GetItem(param1, 0));
    mouseState->y = PyLong_AsLong(PyTuple_GetItem(param1, 1));
    mouseState->button = PyLong_AsLong(PyTuple_GetItem(param1, 2));
    mouseState->delta = PyLong_AsLong(PyTuple_GetItem(param1, 3));
    *param1_ptr = (intptr_t)mouseState;
    *param2_ptr = PyLong_AsLong(param2);
    break;

  case xpMsg_Reshape:
    *param1_ptr = (intptr_t) getVoidPtr(param1, "XPWidgetID");

    /* wChange = (XPWidgetGeometryChange_t *)inParam2; */
    /* param2 = Py_BuildValue("(iiii)", wChange->dx, wChange->dy, */
    /*                        wChange->dwidth, wChange->dheight); */
    wChange = (XPWidgetGeometryChange_t *)malloc(sizeof(XPWidgetGeometryChange_t));
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
    *param1_ptr = (intptr_t) getVoidPtr(param1, "XPWidgetID");
    *param2_ptr = PyLong_AsLong(param2);
    break;
    
  case xpMsg_PropertyChanged:
    *param1_ptr = PyLong_AsLong(param1);
    // use inParam2 -- it's already python, if Property > UserStart
    *param2_ptr = *param1_ptr >= xpProperty_UserStart ? (intptr_t)param2 : PyLong_AsLong(param2);
    errCheck("Failed to convert param2 pointer for msg %d", msg);
    break;
  default:
    *param1_ptr = PyCapsule_CheckExact(param1) ? (intptr_t) getVoidPtr(param1) : PyLong_AsLong(param1);
    *param2_ptr = PyCapsule_CheckExact(param2) ? (intptr_t) getVoidPtr(param2) : PyLong_AsLong(param2);
    errCheck("Failed to convert param pointers for msg %d", msg);
    break;
  }
  errCheck("end convertMesssagePythonToC");
}

My_DOCSTR(_selectIfNeeded__doc__, "selectIfNeeded",
          "message, widgetID, param1, param2, eatClick=1",
          "message:XPWidgetMessage, widgetID:XPWidgetID, param1:int, param2:int, eatClick:int=1",
          "int",
          "Call within widget Callback to raise widget, if not already\n"
          "\n"
          "Seems completely useless with X-Plane 11.55+");
static PyObject *XPUSelectIfNeededFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  errCheck("prior selectIfNeeded");
  char *keywords[] = {CHAR("message"), CHAR("widgetID"), CHAR("param1"), CHAR("param2"), CHAR("eatClick"), nullptr};
  (void) self;
  XPWidgetMessage inMessage;
  XPWidgetID inWidget;
  intptr_t inParam1, inParam2;
  int inEatClick=1;
  PyObject *widget = nullptr, *param1 = nullptr, *param2 = nullptr;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "iOOO|i", keywords, &inMessage, &widget, &param1, &param2, &inEatClick)){
    pythonLog("Failed to parse tuple in selectIfNeeded()");
    return nullptr;
  }

  /* Incoming are messages to widgets.
     XPUSelectIfNeeded() [appears to] raise the widget if required AND THEN RESENDS the
     original message to the widget.

     For the params, we're reading a python object & we'll need to convert them (back)
     to their int form. This is the reverse of widgetCallback() in widgets.c
   */
  inWidget = getVoidPtr(widget, "XPWidgetID");
  errCheck("selectifneeded inWidget");
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
    if (inMessage == xpMsg_CursorAdjust) {
      inParam2 = PyLong_AsLong(PySequence_GetItem(param2, 0));
    } else {
      inParam2 = PyLong_AsLong(param2);
    }
    break;
  case xpMsg_Reshape:
    inParam1 = (intptr_t) getVoidPtr(param1, "XPWidgetID");
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
    inParam1 = (intptr_t) getVoidPtr(param1, "XPWidgetID");
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
  
  errCheck("selectIfNeeded before XPU()");
  res = XPUSelectIfNeeded(inMessage, inWidget, inParam1, inParam2, inEatClick);
  errCheck("end selectIfNeeded ");
  return PyLong_FromLong(res);
}

My_DOCSTR(_defocusKeyboard__doc__, "defocusKeyboard",
          "message, widgetID, param1, param2, eatClick=1",
          "message:XPWidgetMessage, widgetID:XPWidgetID, param1:int, param2:int, eatClick:int=1",
          "int",
          "Send keyboard focus back to X-Plane\n"
          "\n"
          "Seems completely useless in X-Plane 11.55");
static PyObject *XPUDefocusKeyboardFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  char *keywords[] = {CHAR("message"), CHAR("widgetID"), CHAR("param1"), CHAR("param2"), CHAR("eatClick"), nullptr};
  (void) self;
  XPWidgetMessage inMessage;
  XPWidgetID inWidget;
  intptr_t inParam1, inParam2;
  int inEatClick=1;
  PyObject *widget = nullptr, *param1 = nullptr, *param2 = nullptr;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "iOOO|i", keywords, &inMessage, &widget, &param1, &param2, &inEatClick)){
    return nullptr;
  }
  convertMessagePythonToC(inMessage, widget, param1, param2, &inWidget, &inParam1, &inParam2);

  int res = XPUDefocusKeyboard(inMessage, inWidget, inParam1, inParam2, inEatClick);
  return PyLong_FromLong(res);
}

My_DOCSTR(_dragWidget__doc__, "dragWidget",
          "message, widgetID, param1, param2, left, top, right, bottom",
          "message:XPWidgetMessage, widgetID:XPWidgetID, param1:int, param2:int, left:int, top:int, right:int, bottom:int",
          "int",
          "Add to your widget callback to support drag areas\n"
          "\n"
          "(left, top, right, bottom) define area within your widget where, if drag\n"
          "is initiated, this callback will cause the widget to move.");
static PyObject *XPUDragWidgetFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  char *keywords[] = {CHAR("message"), CHAR("widgetID"), CHAR("param1"), CHAR("param2"), CHAR("left"), CHAR("top"), CHAR("right"), CHAR("bottom"), nullptr};
  (void) self;
  XPWidgetMessage inMessage;
  XPWidgetID inWidget;
  intptr_t inParam1, inParam2;
  int inLeft=0, inTop=0, inRight=0, inBottom=0;
  PyObject *widget = nullptr, *param1 = nullptr, *param2 = nullptr;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "iOOOiiii", keywords, &inMessage, &widget, &param1, &param2, &inLeft, &inTop, &inRight, &inBottom)){
    return nullptr;
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
  {nullptr, nullptr, 0, nullptr}
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
  nullptr,
  nullptr,
  nullptr,
  nullptr
};

PyMODINIT_FUNC
PyInit_XPWidgetUtils(void)
{
  PyObject *mod = PyModule_Create(&XPWidgetUtilsModule);
  if(mod){
    PyModule_AddStringConstant(mod, "__author__", "Peter Buckner (pbuck@xppython3.org)");
    PyModule_AddIntConstant(mod, "NO_PARENT", NO_PARENT);
    PyModule_AddIntConstant(mod, "PARAM_PARENT", PARAM_PARENT);

  }

  return mod;
}
  
