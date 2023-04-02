#define _GNU_SOURCE 1
#include <Python.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdbool.h>

#include <XPLM/XPLMDefs.h>
#include <Widgets/XPWidgetDefs.h>

static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  Py_RETURN_NONE;
}

static PyObject *PI_GetMouseStateFun(PyObject *self, PyObject *args) {
  (void) self;
  PyObject *param1;
  if(!PyArg_ParseTuple(args, "O", &param1)) {
    return NULL;
  }
  return param1;
}

static PyObject *PI_GetKeyStateFun(PyObject *self, PyObject *args) {
  (void) self;
  PyObject *param1;
  if(!PyArg_ParseTuple(args, "O", &param1)) {
    return NULL;
  }
  return param1;
}

static PyMethodDef XPWidgetDefsMethods[] = {
  {"PI_GetMouseState", PI_GetMouseStateFun, METH_VARARGS, ""},
  {"PI_GetKeyState", PI_GetKeyStateFun, METH_VARARGS, ""},
  {"_cleanup", cleanup, METH_VARARGS, ""},
  {NULL, NULL, 0, NULL}
};

static struct PyModuleDef XPWidgetDefsModule = {
  PyModuleDef_HEAD_INIT,
  "XPWidgetDefs",
  "Laminar documentation: \n"
  "   https://developer.x-plane.com/sdk/XPWidgetDefs/\n"
  "XPPython3 documentation: \n"
  "   https://xppython3.rtfd.io/en/stable/development/modules/widgetdefs.html",
  -1,
  XPWidgetDefsMethods,
  NULL,
  NULL,
  NULL,
  NULL
};

PyMODINIT_FUNC
PyInit_XPWidgetDefs(void)
{
  PyObject *mod = PyModule_Create(&XPWidgetDefsModule);
  if(mod){
    PyModule_AddStringConstant(mod, "__author__", "Peter Buckner (pbuck@avnwx.com)");
    PyModule_AddIntConstant(mod, "xpProperty_Refcon", xpProperty_Refcon);
    PyModule_AddIntConstant(mod, "xpProperty_Dragging", xpProperty_Dragging);
    PyModule_AddIntConstant(mod, "xpProperty_DragXOff", xpProperty_DragXOff);
    PyModule_AddIntConstant(mod, "xpProperty_DragYOff", xpProperty_DragYOff);
    PyModule_AddIntConstant(mod, "xpProperty_Hilited", xpProperty_Hilited);
    PyModule_AddIntConstant(mod, "xpProperty_Object", xpProperty_Object);
    PyModule_AddIntConstant(mod, "xpProperty_Clip", xpProperty_Clip);
    PyModule_AddIntConstant(mod, "xpProperty_Enabled", xpProperty_Enabled);
    PyModule_AddIntConstant(mod, "xpProperty_UserStart", xpProperty_UserStart);

    PyModule_AddIntConstant(mod, "Property_Refcon", xpProperty_Refcon);
    PyModule_AddIntConstant(mod, "Property_Dragging", xpProperty_Dragging);
    PyModule_AddIntConstant(mod, "Property_DragXOff", xpProperty_DragXOff);
    PyModule_AddIntConstant(mod, "Property_DragYOff", xpProperty_DragYOff);
    PyModule_AddIntConstant(mod, "Property_Hilited", xpProperty_Hilited);
    PyModule_AddIntConstant(mod, "Property_Object", xpProperty_Object);
    PyModule_AddIntConstant(mod, "Property_Clip", xpProperty_Clip);
    PyModule_AddIntConstant(mod, "Property_Enabled", xpProperty_Enabled);
    PyModule_AddIntConstant(mod, "Property_UserStart", xpProperty_UserStart);

    PyModule_AddIntConstant(mod, "xpMode_Direct", xpMode_Direct);
    PyModule_AddIntConstant(mod, "xpMode_UpChain", xpMode_UpChain);
    PyModule_AddIntConstant(mod, "xpMode_Recursive", xpMode_Recursive);
    PyModule_AddIntConstant(mod, "xpMode_DirectAllCallbacks", xpMode_DirectAllCallbacks);
    PyModule_AddIntConstant(mod, "xpMode_Once", xpMode_Once);

    PyModule_AddIntConstant(mod, "Mode_Direct", xpMode_Direct);
    PyModule_AddIntConstant(mod, "Mode_UpChain", xpMode_UpChain);
    PyModule_AddIntConstant(mod, "Mode_Recursive", xpMode_Recursive);
    PyModule_AddIntConstant(mod, "Mode_DirectAllCallbacks", xpMode_DirectAllCallbacks);
    PyModule_AddIntConstant(mod, "Mode_Once", xpMode_Once);

    PyModule_AddIntConstant(mod, "xpWidgetClass_None", xpWidgetClass_None);
    PyModule_AddIntConstant(mod, "WidgetClass_None", xpWidgetClass_None);

    PyModule_AddIntConstant(mod, "xpMsg_None", xpMsg_None);
    PyModule_AddIntConstant(mod, "xpMsg_Create", xpMsg_Create);
    PyModule_AddIntConstant(mod, "xpMsg_Destroy", xpMsg_Destroy);
    PyModule_AddIntConstant(mod, "xpMsg_Paint", xpMsg_Paint);
    PyModule_AddIntConstant(mod, "xpMsg_Draw", xpMsg_Draw);
    PyModule_AddIntConstant(mod, "xpMsg_KeyPress", xpMsg_KeyPress);
    PyModule_AddIntConstant(mod, "xpMsg_KeyTakeFocus", xpMsg_KeyTakeFocus);
    PyModule_AddIntConstant(mod, "xpMsg_KeyLoseFocus", xpMsg_KeyLoseFocus);
    PyModule_AddIntConstant(mod, "xpMsg_MouseDown", xpMsg_MouseDown);
    PyModule_AddIntConstant(mod, "xpMsg_MouseDrag", xpMsg_MouseDrag);
    PyModule_AddIntConstant(mod, "xpMsg_MouseUp", xpMsg_MouseUp);
    PyModule_AddIntConstant(mod, "xpMsg_Reshape", xpMsg_Reshape);
    PyModule_AddIntConstant(mod, "xpMsg_ExposedChanged", xpMsg_ExposedChanged);
    PyModule_AddIntConstant(mod, "xpMsg_AcceptChild", xpMsg_AcceptChild);
    PyModule_AddIntConstant(mod, "xpMsg_LoseChild", xpMsg_LoseChild);
    PyModule_AddIntConstant(mod, "xpMsg_AcceptParent", xpMsg_AcceptParent);
    PyModule_AddIntConstant(mod, "xpMsg_Shown", xpMsg_Shown);
    PyModule_AddIntConstant(mod, "xpMsg_Hidden", xpMsg_Hidden);
    PyModule_AddIntConstant(mod, "xpMsg_DescriptorChanged", xpMsg_DescriptorChanged);
    PyModule_AddIntConstant(mod, "xpMsg_PropertyChanged", xpMsg_PropertyChanged);
    PyModule_AddIntConstant(mod, "xpMsg_MouseWheel", xpMsg_MouseWheel);
    PyModule_AddIntConstant(mod, "xpMsg_CursorAdjust", xpMsg_CursorAdjust);
    PyModule_AddIntConstant(mod, "xpMsg_UserStart", xpMsg_UserStart);

    PyModule_AddIntConstant(mod, "Msg_None", xpMsg_None);
    PyModule_AddIntConstant(mod, "Msg_Create", xpMsg_Create);
    PyModule_AddIntConstant(mod, "Msg_Destroy", xpMsg_Destroy);
    PyModule_AddIntConstant(mod, "Msg_Paint", xpMsg_Paint);
    PyModule_AddIntConstant(mod, "Msg_Draw", xpMsg_Draw);
    PyModule_AddIntConstant(mod, "Msg_KeyPress", xpMsg_KeyPress);
    PyModule_AddIntConstant(mod, "Msg_KeyTakeFocus", xpMsg_KeyTakeFocus);
    PyModule_AddIntConstant(mod, "Msg_KeyLoseFocus", xpMsg_KeyLoseFocus);
    PyModule_AddIntConstant(mod, "Msg_MouseDown", xpMsg_MouseDown);
    PyModule_AddIntConstant(mod, "Msg_MouseDrag", xpMsg_MouseDrag);
    PyModule_AddIntConstant(mod, "Msg_MouseUp", xpMsg_MouseUp);
    PyModule_AddIntConstant(mod, "Msg_Reshape", xpMsg_Reshape);
    PyModule_AddIntConstant(mod, "Msg_ExposedChanged", xpMsg_ExposedChanged);
    PyModule_AddIntConstant(mod, "Msg_AcceptChild", xpMsg_AcceptChild);
    PyModule_AddIntConstant(mod, "Msg_LoseChild", xpMsg_LoseChild);
    PyModule_AddIntConstant(mod, "Msg_AcceptParent", xpMsg_AcceptParent);
    PyModule_AddIntConstant(mod, "Msg_Shown", xpMsg_Shown);
    PyModule_AddIntConstant(mod, "Msg_Hidden", xpMsg_Hidden);
    PyModule_AddIntConstant(mod, "Msg_DescriptorChanged", xpMsg_DescriptorChanged);
    PyModule_AddIntConstant(mod, "Msg_PropertyChanged", xpMsg_PropertyChanged);
    PyModule_AddIntConstant(mod, "Msg_MouseWheel", xpMsg_MouseWheel);
    PyModule_AddIntConstant(mod, "Msg_CursorAdjust", xpMsg_CursorAdjust);
    PyModule_AddIntConstant(mod, "Msg_UserStart", xpMsg_UserStart);

  }
  return mod;
}

