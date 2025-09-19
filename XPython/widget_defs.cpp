#define _GNU_SOURCE 1
#include <Python.h>
#include <sys/time.h>
#include <stdio.h>

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
    return nullptr;
  }
  return param1;
}

static PyObject *PI_GetKeyStateFun(PyObject *self, PyObject *args) {
  (void) self;
  PyObject *param1;
  if(!PyArg_ParseTuple(args, "O", &param1)) {
    return nullptr;
  }
  return param1;
}

static PyMethodDef XPWidgetDefsMethods[] = {
  {"PI_GetMouseState", PI_GetMouseStateFun, METH_VARARGS, ""},
  {"PI_GetKeyState", PI_GetKeyStateFun, METH_VARARGS, ""},
  {"_cleanup", cleanup, METH_VARARGS, ""},
  {nullptr, nullptr, 0, nullptr}
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
  nullptr,
  nullptr,
  nullptr,
  nullptr
};

PyMODINIT_FUNC
PyInit_XPWidgetDefs(void)
{
  PyObject *mod = PyModule_Create(&XPWidgetDefsModule);
  if(mod){
    PyModule_AddStringConstant(mod, "__author__", "Peter Buckner (pbuck@xppython3.org)");
    PyModule_AddIntConstant(mod, "xpProperty_Refcon", xpProperty_Refcon); // XPWidgetPropertyID
    PyModule_AddIntConstant(mod, "xpProperty_Dragging", xpProperty_Dragging); // XPWidgetPropertyID
    PyModule_AddIntConstant(mod, "xpProperty_DragXOff", xpProperty_DragXOff); // XPWidgetPropertyID
    PyModule_AddIntConstant(mod, "xpProperty_DragYOff", xpProperty_DragYOff); // XPWidgetPropertyID
    PyModule_AddIntConstant(mod, "xpProperty_Hilited", xpProperty_Hilited); // XPWidgetPropertyID
    PyModule_AddIntConstant(mod, "xpProperty_Object", xpProperty_Object); // XPWidgetPropertyID
    PyModule_AddIntConstant(mod, "xpProperty_Clip", xpProperty_Clip); // XPWidgetPropertyID
    PyModule_AddIntConstant(mod, "xpProperty_Enabled", xpProperty_Enabled); // XPWidgetPropertyID
    PyModule_AddIntConstant(mod, "xpProperty_UserStart", xpProperty_UserStart); // XPWidgetPropertyID

    PyModule_AddIntConstant(mod, "Property_Refcon", xpProperty_Refcon); // XPWidgetPropertyID
    PyModule_AddIntConstant(mod, "Property_Dragging", xpProperty_Dragging); // XPWidgetPropertyID
    PyModule_AddIntConstant(mod, "Property_DragXOff", xpProperty_DragXOff); // XPWidgetPropertyID
    PyModule_AddIntConstant(mod, "Property_DragYOff", xpProperty_DragYOff); // XPWidgetPropertyID
    PyModule_AddIntConstant(mod, "Property_Hilited", xpProperty_Hilited); // XPWidgetPropertyID
    PyModule_AddIntConstant(mod, "Property_Object", xpProperty_Object); // XPWidgetPropertyID
    PyModule_AddIntConstant(mod, "Property_Clip", xpProperty_Clip); // XPWidgetPropertyID
    PyModule_AddIntConstant(mod, "Property_Enabled", xpProperty_Enabled); // XPWidgetPropertyID
    PyModule_AddIntConstant(mod, "Property_UserStart", xpProperty_UserStart); // XPWidgetPropertyID

    PyModule_AddIntConstant(mod, "xpMode_Direct", xpMode_Direct);  // XPDispatchMode
    PyModule_AddIntConstant(mod, "xpMode_UpChain", xpMode_UpChain);  // XPDispatchMode
    PyModule_AddIntConstant(mod, "xpMode_Recursive", xpMode_Recursive);  // XPDispatchMode
    PyModule_AddIntConstant(mod, "xpMode_DirectAllCallbacks", xpMode_DirectAllCallbacks);  // XPDispatchMode
    PyModule_AddIntConstant(mod, "xpMode_Once", xpMode_Once);  // XPDispatchMode

    PyModule_AddIntConstant(mod, "Mode_Direct", xpMode_Direct);  // XPDispatchMode
    PyModule_AddIntConstant(mod, "Mode_UpChain", xpMode_UpChain);  // XPDispatchMode
    PyModule_AddIntConstant(mod, "Mode_Recursive", xpMode_Recursive);  // XPDispatchMode
    PyModule_AddIntConstant(mod, "Mode_DirectAllCallbacks", xpMode_DirectAllCallbacks);  // XPDispatchMode
    PyModule_AddIntConstant(mod, "Mode_Once", xpMode_Once);  // XPDispatchMode

    PyModule_AddIntConstant(mod, "xpWidgetClass_None", xpWidgetClass_None);
    PyModule_AddIntConstant(mod, "WidgetClass_None", xpWidgetClass_None);

    PyModule_AddIntConstant(mod, "xpMsg_None", xpMsg_None); // XPWidgetMessage
    PyModule_AddIntConstant(mod, "xpMsg_Create", xpMsg_Create); // XPWidgetMessage
    PyModule_AddIntConstant(mod, "xpMsg_Destroy", xpMsg_Destroy); // XPWidgetMessage
    PyModule_AddIntConstant(mod, "xpMsg_Paint", xpMsg_Paint); // XPWidgetMessage
    PyModule_AddIntConstant(mod, "xpMsg_Draw", xpMsg_Draw); // XPWidgetMessage
    PyModule_AddIntConstant(mod, "xpMsg_KeyPress", xpMsg_KeyPress); // XPWidgetMessage
    PyModule_AddIntConstant(mod, "xpMsg_KeyTakeFocus", xpMsg_KeyTakeFocus); // XPWidgetMessage
    PyModule_AddIntConstant(mod, "xpMsg_KeyLoseFocus", xpMsg_KeyLoseFocus); // XPWidgetMessage
    PyModule_AddIntConstant(mod, "xpMsg_MouseDown", xpMsg_MouseDown); // XPWidgetMessage
    PyModule_AddIntConstant(mod, "xpMsg_MouseDrag", xpMsg_MouseDrag); // XPWidgetMessage
    PyModule_AddIntConstant(mod, "xpMsg_MouseUp", xpMsg_MouseUp); // XPWidgetMessage
    PyModule_AddIntConstant(mod, "xpMsg_Reshape", xpMsg_Reshape); // XPWidgetMessage
    PyModule_AddIntConstant(mod, "xpMsg_ExposedChanged", xpMsg_ExposedChanged); // XPWidgetMessage
    PyModule_AddIntConstant(mod, "xpMsg_AcceptChild", xpMsg_AcceptChild); // XPWidgetMessage
    PyModule_AddIntConstant(mod, "xpMsg_LoseChild", xpMsg_LoseChild); // XPWidgetMessage
    PyModule_AddIntConstant(mod, "xpMsg_AcceptParent", xpMsg_AcceptParent); // XPWidgetMessage
    PyModule_AddIntConstant(mod, "xpMsg_Shown", xpMsg_Shown); // XPWidgetMessage
    PyModule_AddIntConstant(mod, "xpMsg_Hidden", xpMsg_Hidden); // XPWidgetMessage
    PyModule_AddIntConstant(mod, "xpMsg_DescriptorChanged", xpMsg_DescriptorChanged); // XPWidgetMessage
    PyModule_AddIntConstant(mod, "xpMsg_PropertyChanged", xpMsg_PropertyChanged); // XPWidgetMessage
    PyModule_AddIntConstant(mod, "xpMsg_MouseWheel", xpMsg_MouseWheel); // XPWidgetMessage
    PyModule_AddIntConstant(mod, "xpMsg_CursorAdjust", xpMsg_CursorAdjust); // XPWidgetMessage
    PyModule_AddIntConstant(mod, "xpMsg_UserStart", xpMsg_UserStart); // XPWidgetMessage

    PyModule_AddIntConstant(mod, "Msg_None", xpMsg_None); // XPWidgetMessage
    PyModule_AddIntConstant(mod, "Msg_Create", xpMsg_Create); // XPWidgetMessage
    PyModule_AddIntConstant(mod, "Msg_Destroy", xpMsg_Destroy); // XPWidgetMessage
    PyModule_AddIntConstant(mod, "Msg_Paint", xpMsg_Paint); // XPWidgetMessage
    PyModule_AddIntConstant(mod, "Msg_Draw", xpMsg_Draw); // XPWidgetMessage
    PyModule_AddIntConstant(mod, "Msg_KeyPress", xpMsg_KeyPress); // XPWidgetMessage
    PyModule_AddIntConstant(mod, "Msg_KeyTakeFocus", xpMsg_KeyTakeFocus); // XPWidgetMessage
    PyModule_AddIntConstant(mod, "Msg_KeyLoseFocus", xpMsg_KeyLoseFocus); // XPWidgetMessage
    PyModule_AddIntConstant(mod, "Msg_MouseDown", xpMsg_MouseDown); // XPWidgetMessage
    PyModule_AddIntConstant(mod, "Msg_MouseDrag", xpMsg_MouseDrag); // XPWidgetMessage
    PyModule_AddIntConstant(mod, "Msg_MouseUp", xpMsg_MouseUp); // XPWidgetMessage
    PyModule_AddIntConstant(mod, "Msg_Reshape", xpMsg_Reshape); // XPWidgetMessage
    PyModule_AddIntConstant(mod, "Msg_ExposedChanged", xpMsg_ExposedChanged); // XPWidgetMessage
    PyModule_AddIntConstant(mod, "Msg_AcceptChild", xpMsg_AcceptChild); // XPWidgetMessage
    PyModule_AddIntConstant(mod, "Msg_LoseChild", xpMsg_LoseChild); // XPWidgetMessage
    PyModule_AddIntConstant(mod, "Msg_AcceptParent", xpMsg_AcceptParent); // XPWidgetMessage
    PyModule_AddIntConstant(mod, "Msg_Shown", xpMsg_Shown); // XPWidgetMessage
    PyModule_AddIntConstant(mod, "Msg_Hidden", xpMsg_Hidden); // XPWidgetMessage
    PyModule_AddIntConstant(mod, "Msg_DescriptorChanged", xpMsg_DescriptorChanged); // XPWidgetMessage
    PyModule_AddIntConstant(mod, "Msg_PropertyChanged", xpMsg_PropertyChanged); // XPWidgetMessage
    PyModule_AddIntConstant(mod, "Msg_MouseWheel", xpMsg_MouseWheel); // XPWidgetMessage
    PyModule_AddIntConstant(mod, "Msg_CursorAdjust", xpMsg_CursorAdjust); // XPWidgetMessage
    PyModule_AddIntConstant(mod, "Msg_UserStart", xpMsg_UserStart); // XPWidgetMessage
  }
  return mod;
}

