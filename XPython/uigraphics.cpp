#define _GNU_SOURCE 1
#include <Python.h>
#include <sys/time.h>
#include <stdio.h>

#include <XPLM/XPLMDefs.h>
#include <Widgets/XPWidgetDefs.h>
#include <Widgets/XPUIGraphics.h>
#include "utils.h"
#include "xppythontypes.h"
#include "cpp_utilities.hpp"

My_DOCSTR(_drawWindow__doc__, "drawWindow",
          "left, bottom, right, top, style=1",
          "left:int, bottom:int, right:int, top:int, style:XPWindowStyle=Window_MainWindow",
          "None",
          "Draw window at location");
static PyObject *XPDrawWindowFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  std::vector<std::string> params = {"left", "bottom", "right", "top", "style"};
  char **keywords = stringVectorToCharArray(params);
  (void) self;
  int inX1, inY1, inX2, inY2, inStyle=xpWindow_MainWindow;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "iiii|i", keywords, &inX1, &inY1, &inX2, &inY2, &inStyle)){
    freeCharArray(keywords, params.size());
    return NULL;
  }
  freeCharArray(keywords, params.size());
  XPDrawWindow(inX1, inY1, inX2, inY2, inStyle);
  Py_RETURN_NONE;
}

My_DOCSTR(_getWindowDefaultDimensions__doc__, "getWindowDefaultDimensions",
          "style=1",
          "style:XPWindowStyle=Window_MainWindow",
          "tuple[int, int]",
          "Default dimension for indicated style\n"
          "\n"
          "Returns (width, height)");
static PyObject *XPGetWindowDefaultDimensionsFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  std::vector<std::string> params = {"style"};
  char **keywords = stringVectorToCharArray(params);
  (void) self;
  int inStyle=xpWindow_MainWindow, width, height;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|i", keywords, &inStyle)) {
    freeCharArray(keywords, params.size());
    return NULL;
  }
  freeCharArray(keywords, params.size());
  XPGetWindowDefaultDimensions(inStyle, &width, &height);
  return Py_BuildValue("(ii)", width, height);
}

My_DOCSTR(_drawElement__doc__, "drawElement",
          "left, bottom, right, top, style, lit=0",
          "left:int, bottom:int, right:int, top:int, style:XPElementStyle, lit:int=0",
          "None",
          "Draw element, possibly lit, at location.");
static PyObject *XPDrawElementFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  std::vector<std::string> params = {"left", "bottom", "right", "top", "style", "lit"};
  char **keywords = stringVectorToCharArray(params);
  (void) self;
  int inX1, inY1, inX2, inY2, inStyle, inLit=0;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "iiiii|i", keywords, &inX1, &inY1, &inX2, &inY2, &inStyle, &inLit)){
    freeCharArray(keywords, params.size());
    return NULL;
  }
  freeCharArray(keywords, params.size());
  XPDrawElement(inX1, inY1, inX2, inY2, inStyle, inLit);
  Py_RETURN_NONE;
}

My_DOCSTR(_getElementDefaultDimensions__doc__, "getElementDefaultDimensions",
          "style",
          "style:XPElementStyle",
          "tuple[int, int, int]",
          "Default dimension for indicated element\n"
          "\n"
          "Returns (width, height, canBeLit)");
static PyObject *XPGetElementDefaultDimensionsFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  std::vector<std::string> params = {"style"};
  char **keywords = stringVectorToCharArray(params);
  (void) self;
  int inStyle, width, height, canBeLit;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "i", keywords, &inStyle)) {
    freeCharArray(keywords, params.size());
    return NULL;
  }
  freeCharArray(keywords, params.size());
  XPGetElementDefaultDimensions(inStyle, &width, &height, &canBeLit);
  return Py_BuildValue("(iii)", width, height, canBeLit);
}


My_DOCSTR(_drawTrack__doc__, "drawTrack",
          "left, bottom, right, top, minValue, maxValue, value, style, lit=0",
          "left:int, bottom:int, right:int, top:int, minValue:int, maxValue:int, value:int, style:XPTrackStyle, lit:int=0",
          "None",
          "Draw track at location, with min/max values and current value.\n"
          "\n"
          "Track may be 'reversed' if minValue > maxValue.\n"
          "Styles are:\n"
          "  Track_Scrollbar=0\n"
          "  Track_Slider   =1\n"
          "  Track_Progress =2");
static PyObject *XPDrawTrackFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  std::vector<std::string> params = {"left", "bottom", "right", "top", "minValue", "maxValue", "value", "style", "lit"};
  char **keywords = stringVectorToCharArray(params);
  (void) self;
  int inX1, inY1, inX2, inY2, inMin, inMax, inValue, inStyle, inLit=0;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "iiiiiiii|i", keywords, &inX1, &inY1, &inX2, &inY2, &inMin, &inMax, &inValue, &inStyle, &inLit)){
    freeCharArray(keywords, params.size());
    return NULL;
  }
  freeCharArray(keywords, params.size());
  XPDrawTrack(inX1, inY1, inX2, inY2, inMin, inMax, inValue, inStyle, inLit);
  Py_RETURN_NONE;
}

My_DOCSTR(_getTrackDefaultDimensions__doc__, "getTrackDefaultDimensions",
          "style",
          "style:XPTrackStyle",
          "tuple[int, int]",
          "Default dimension for indicated track style\n"
          "\n"
          "Returns (width, canBeLit)");
static PyObject *XPGetTrackDefaultDimensionsFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  std::vector<std::string> params = {"style"};
  char **keywords = stringVectorToCharArray(params);
  (void) self;
  int inStyle, width, canBeLit;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "i", keywords, &inStyle)) {
    freeCharArray(keywords, params.size());
    return NULL;
  }
  freeCharArray(keywords, params.size());
  XPGetTrackDefaultDimensions(inStyle, &width, &canBeLit);
  return Py_BuildValue("(ii)", width, canBeLit);
}


My_DOCSTR(_getTrackMetrics__doc__, "getTrackMetrics",
          "left, bottom, right, top, minValue, maxValue, value, style",
          "left:int, bottom:int, right:int, top:int, minValue:int, maxValue:int, value:int, style:XPTrackStyle",
          "TrackMetrics",
          "Return object with metrics about track\n"
          "\n"
          "Object attributes are:\n"
          "  .isVertical\n"
          "  .downBtnSize\n"
          "  .downPageSize\n"
          "  .thumbSize\n"
          "  .upPageSize\n"
          "  .upBtnSize");
static PyObject *XPGetTrackMetricsFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  std::vector<std::string> params = {"left", "bottom", "right", "top", "minValue", "maxValue", "value", "style"};
  char **keywords = stringVectorToCharArray(params);
  (void) self;
  int inX1, inY1, inX2, inY2, inMin, inMax, inValue, inStyle;
  int outIsVertical, outDownBtnSize, outDownPageSize, outThumbSize, outUpPageSize, outUpBtnSize;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "iiiiiiii", keywords, &inX1, &inY1, &inX2, &inY2, &inMin, &inMax, &inValue, &inStyle)){
    freeCharArray(keywords, params.size());
    return NULL;
  }
  freeCharArray(keywords, params.size());
  XPGetTrackMetrics(inX1, inY1, inX2, inY2, inMin, inMax, inValue, inStyle, &outIsVertical, &outDownBtnSize,
                    &outDownPageSize, &outThumbSize, &outUpPageSize, &outUpBtnSize);
  return PyTrackMetrics_New(outIsVertical, outDownBtnSize, outDownPageSize, outThumbSize, outUpPageSize, outUpBtnSize);
}

static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  Py_RETURN_NONE;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
static PyMethodDef XPUIGraphicsMethods[] = {
  {"drawWindow", (PyCFunction)XPDrawWindowFun, METH_VARARGS | METH_KEYWORDS, _drawWindow__doc__},
  {"XPDrawWindow", (PyCFunction)XPDrawWindowFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"getWindowDefaultDimensions", (PyCFunction)XPGetWindowDefaultDimensionsFun, METH_VARARGS | METH_KEYWORDS, _getWindowDefaultDimensions__doc__},
  {"XPGetWindowDefaultDimensions", (PyCFunction)XPGetWindowDefaultDimensionsFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"drawElement", (PyCFunction)XPDrawElementFun, METH_VARARGS | METH_KEYWORDS, _drawElement__doc__},
  {"XPDrawElement", (PyCFunction)XPDrawElementFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"getElementDefaultDimensions", (PyCFunction)XPGetElementDefaultDimensionsFun, METH_VARARGS | METH_KEYWORDS, _getElementDefaultDimensions__doc__},
  {"XPGetElementDefaultDimensions", (PyCFunction)XPGetElementDefaultDimensionsFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"drawTrack", (PyCFunction)XPDrawTrackFun, METH_VARARGS | METH_KEYWORDS, _drawTrack__doc__},
  {"XPDrawTrack", (PyCFunction)XPDrawTrackFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"getTrackDefaultDimensions", (PyCFunction)XPGetTrackDefaultDimensionsFun, METH_VARARGS | METH_KEYWORDS, _getTrackDefaultDimensions__doc__},
  {"XPGetTrackDefaultDimensions", (PyCFunction)XPGetTrackDefaultDimensionsFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"getTrackMetrics", (PyCFunction)XPGetTrackMetricsFun, METH_VARARGS | METH_KEYWORDS, _getTrackMetrics__doc__},
  {"XPGetTrackMetrics", (PyCFunction)XPGetTrackMetricsFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"_cleanup", cleanup, METH_VARARGS, ""},
  {NULL, NULL, 0, NULL}
};
#pragma GCC diagnostic pop



static struct PyModuleDef XPUIGraphicsModule = {
  PyModuleDef_HEAD_INIT,
  "XPUIGraphics",
  "Laminar documentation: \n"
  "   https://developer.x-plane.com/sdk/XPUIGraphics/\n"
  "XPPython3 documentation: \n"
  "   https://xppython3.rtfd.io/en/stable/development/modules/uigraphics.html",
  -1,
  XPUIGraphicsMethods,
  NULL,
  NULL,
  NULL,
  NULL
};

PyMODINIT_FUNC
PyInit_XPUIGraphics(void)
{
  PyObject *mod = PyModule_Create(&XPUIGraphicsModule);
  if(mod){
    PyModule_AddStringConstant(mod, "__author__", "Peter Buckner (pbuck@xppython3.org)");
    PyModule_AddIntConstant(mod, "xpWindow_Help", xpWindow_Help); // XPWindowStyle
    PyModule_AddIntConstant(mod, "xpWindow_MainWindow", xpWindow_MainWindow); // XPWindowStyle
    PyModule_AddIntConstant(mod, "xpWindow_SubWindow", xpWindow_SubWindow); // XPWindowStyle
    PyModule_AddIntConstant(mod, "xpWindow_Screen", xpWindow_Screen); // XPWindowStyle
    PyModule_AddIntConstant(mod, "xpWindow_ListView", xpWindow_ListView); // XPWindowStyle
    PyModule_AddIntConstant(mod, "xpElement_TextField", xpElement_TextField); // XPElementStyle
    PyModule_AddIntConstant(mod, "xpElement_CheckBox", xpElement_CheckBox); // XPElementStyle
    PyModule_AddIntConstant(mod, "xpElement_CheckBoxLit", xpElement_CheckBoxLit); // XPElementStyle
    PyModule_AddIntConstant(mod, "xpElement_WindowCloseBox", xpElement_WindowCloseBox); // XPElementStyle
    PyModule_AddIntConstant(mod, "xpElement_WindowCloseBoxPressed", xpElement_WindowCloseBoxPressed); // XPElementStyle
    PyModule_AddIntConstant(mod, "xpElement_PushButton", xpElement_PushButton); // XPElementStyle
    PyModule_AddIntConstant(mod, "xpElement_PushButtonLit", xpElement_PushButtonLit); // XPElementStyle
    PyModule_AddIntConstant(mod, "xpElement_OilPlatform", xpElement_OilPlatform); // XPElementStyle
    PyModule_AddIntConstant(mod, "xpElement_OilPlatformSmall", xpElement_OilPlatformSmall); // XPElementStyle
    PyModule_AddIntConstant(mod, "xpElement_Ship", xpElement_Ship); // XPElementStyle
    PyModule_AddIntConstant(mod, "xpElement_ILSGlideScope", xpElement_ILSGlideScope); // XPElementStyle
    PyModule_AddIntConstant(mod, "xpElement_MarkerLeft", xpElement_MarkerLeft); // XPElementStyle
    PyModule_AddIntConstant(mod, "xpElement_Airport", xpElement_Airport); // XPElementStyle
    PyModule_AddIntConstant(mod, "xpElement_Waypoint", xpElement_Waypoint); // XPElementStyle
    PyModule_AddIntConstant(mod, "xpElement_NDB", xpElement_NDB); // XPElementStyle
    PyModule_AddIntConstant(mod, "xpElement_VOR", xpElement_VOR); // XPElementStyle
    PyModule_AddIntConstant(mod, "xpElement_RadioTower", xpElement_RadioTower); // XPElementStyle
    PyModule_AddIntConstant(mod, "xpElement_AircraftCarrier", xpElement_AircraftCarrier); // XPElementStyle
    PyModule_AddIntConstant(mod, "xpElement_Fire", xpElement_Fire); // XPElementStyle
    PyModule_AddIntConstant(mod, "xpElement_MarkerRight", xpElement_MarkerRight); // XPElementStyle
    PyModule_AddIntConstant(mod, "xpElement_CustomObject", xpElement_CustomObject); // XPElementStyle
    PyModule_AddIntConstant(mod, "xpElement_CoolingTower", xpElement_CoolingTower); // XPElementStyle
    PyModule_AddIntConstant(mod, "xpElement_SmokeStack", xpElement_SmokeStack); // XPElementStyle
    PyModule_AddIntConstant(mod, "xpElement_Building", xpElement_Building); // XPElementStyle
    PyModule_AddIntConstant(mod, "xpElement_PowerLine", xpElement_PowerLine); // XPElementStyle
    PyModule_AddIntConstant(mod, "xpElement_CopyButtons", xpElement_CopyButtons); // XPElementStyle
    PyModule_AddIntConstant(mod, "xpElement_CopyButtonsWithEditingGrid", xpElement_CopyButtonsWithEditingGrid); // XPElementStyle
    PyModule_AddIntConstant(mod, "xpElement_EditingGrid", xpElement_EditingGrid); // XPElementStyle
    PyModule_AddIntConstant(mod, "xpElement_ScrollBar", xpElement_ScrollBar); // XPElementStyle
    PyModule_AddIntConstant(mod, "xpElement_VORWithCompassRose", xpElement_VORWithCompassRose); // XPElementStyle
    PyModule_AddIntConstant(mod, "xpElement_Zoomer", xpElement_Zoomer); // XPElementStyle
    PyModule_AddIntConstant(mod, "xpElement_TextFieldMiddle", xpElement_TextFieldMiddle); // XPElementStyle
    PyModule_AddIntConstant(mod, "xpElement_LittleDownArrow", xpElement_LittleDownArrow); // XPElementStyle
    PyModule_AddIntConstant(mod, "xpElement_LittleUpArrow", xpElement_LittleUpArrow); // XPElementStyle
    PyModule_AddIntConstant(mod, "xpElement_WindowDragBar", xpElement_WindowDragBar); // XPElementStyle
    PyModule_AddIntConstant(mod, "xpElement_WindowDragBarSmooth", xpElement_WindowDragBarSmooth); // XPElementStyle
    PyModule_AddIntConstant(mod, "xpTrack_ScrollBar", xpTrack_ScrollBar); // XPTrackStyle
    PyModule_AddIntConstant(mod, "xpTrack_Slider", xpTrack_Slider); // XPTrackStyle
    PyModule_AddIntConstant(mod, "xpTrack_Progress", xpTrack_Progress); // XPTrackStyle

    PyModule_AddIntConstant(mod, "Window_Help", xpWindow_Help); // XPWindowStyle
    PyModule_AddIntConstant(mod, "Window_MainWindow", xpWindow_MainWindow); // XPWindowStyle
    PyModule_AddIntConstant(mod, "Window_SubWindow", xpWindow_SubWindow); // XPWindowStyle
    PyModule_AddIntConstant(mod, "Window_Screen", xpWindow_Screen); // XPWindowStyle
    PyModule_AddIntConstant(mod, "Window_ListView", xpWindow_ListView); // XPWindowStyle
    PyModule_AddIntConstant(mod, "Element_TextField", xpElement_TextField); // XPElementStyle
    PyModule_AddIntConstant(mod, "Element_CheckBox", xpElement_CheckBox); // XPElementStyle
    PyModule_AddIntConstant(mod, "Element_CheckBoxLit", xpElement_CheckBoxLit); // XPElementStyle
    PyModule_AddIntConstant(mod, "Element_WindowCloseBox", xpElement_WindowCloseBox); // XPElementStyle
    PyModule_AddIntConstant(mod, "Element_WindowCloseBoxPressed", xpElement_WindowCloseBoxPressed); // XPElementStyle
    PyModule_AddIntConstant(mod, "Element_PushButton", xpElement_PushButton); // XPElementStyle
    PyModule_AddIntConstant(mod, "Element_PushButtonLit", xpElement_PushButtonLit); // XPElementStyle
    PyModule_AddIntConstant(mod, "Element_OilPlatform", xpElement_OilPlatform); // XPElementStyle
    PyModule_AddIntConstant(mod, "Element_OilPlatformSmall", xpElement_OilPlatformSmall); // XPElementStyle
    PyModule_AddIntConstant(mod, "Element_Ship", xpElement_Ship); // XPElementStyle
    PyModule_AddIntConstant(mod, "Element_ILSGlideScope", xpElement_ILSGlideScope); // XPElementStyle
    PyModule_AddIntConstant(mod, "Element_MarkerLeft", xpElement_MarkerLeft); // XPElementStyle
    PyModule_AddIntConstant(mod, "Element_Airport", xpElement_Airport); // XPElementStyle
    PyModule_AddIntConstant(mod, "Element_Waypoint", xpElement_Waypoint); // XPElementStyle
    PyModule_AddIntConstant(mod, "Element_NDB", xpElement_NDB); // XPElementStyle
    PyModule_AddIntConstant(mod, "Element_VOR", xpElement_VOR); // XPElementStyle
    PyModule_AddIntConstant(mod, "Element_RadioTower", xpElement_RadioTower); // XPElementStyle
    PyModule_AddIntConstant(mod, "Element_AircraftCarrier", xpElement_AircraftCarrier); // XPElementStyle
    PyModule_AddIntConstant(mod, "Element_Fire", xpElement_Fire); // XPElementStyle
    PyModule_AddIntConstant(mod, "Element_MarkerRight", xpElement_MarkerRight); // XPElementStyle
    PyModule_AddIntConstant(mod, "Element_CustomObject", xpElement_CustomObject); // XPElementStyle
    PyModule_AddIntConstant(mod, "Element_CoolingTower", xpElement_CoolingTower); // XPElementStyle
    PyModule_AddIntConstant(mod, "Element_SmokeStack", xpElement_SmokeStack); // XPElementStyle
    PyModule_AddIntConstant(mod, "Element_Building", xpElement_Building); // XPElementStyle
    PyModule_AddIntConstant(mod, "Element_PowerLine", xpElement_PowerLine); // XPElementStyle
    PyModule_AddIntConstant(mod, "Element_CopyButtons", xpElement_CopyButtons); // XPElementStyle
    PyModule_AddIntConstant(mod, "Element_CopyButtonsWithEditingGrid", xpElement_CopyButtonsWithEditingGrid); // XPElementStyle
    PyModule_AddIntConstant(mod, "Element_EditingGrid", xpElement_EditingGrid); // XPElementStyle
    PyModule_AddIntConstant(mod, "Element_ScrollBar", xpElement_ScrollBar); // XPElementStyle
    PyModule_AddIntConstant(mod, "Element_VORWithCompassRose", xpElement_VORWithCompassRose); // XPElementStyle
    PyModule_AddIntConstant(mod, "Element_Zoomer", xpElement_Zoomer); // XPElementStyle
    PyModule_AddIntConstant(mod, "Element_TextFieldMiddle", xpElement_TextFieldMiddle); // XPElementStyle
    PyModule_AddIntConstant(mod, "Element_LittleDownArrow", xpElement_LittleDownArrow); // XPElementStyle
    PyModule_AddIntConstant(mod, "Element_LittleUpArrow", xpElement_LittleUpArrow); // XPElementStyle
    PyModule_AddIntConstant(mod, "Element_WindowDragBar", xpElement_WindowDragBar); // XPElementStyle
    PyModule_AddIntConstant(mod, "Element_WindowDragBarSmooth", xpElement_WindowDragBarSmooth); // XPElementStyle
    PyModule_AddIntConstant(mod, "Track_ScrollBar", xpTrack_ScrollBar); // XPTrackStyle
    PyModule_AddIntConstant(mod, "Track_Slider", xpTrack_Slider); // XPTrackStyle
    PyModule_AddIntConstant(mod, "Track_Progress", xpTrack_Progress); // XPTrackStyle
  }

  return mod;
}
 

