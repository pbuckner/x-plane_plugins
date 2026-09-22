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


/* ---- Transform stack ---- */

My_DOCSTR(_transformPush__doc__, "transformPush",
          "",
          "",
          "None",
          "Save the current transformation matrix onto the transform stack.");
static PyObject *XPLMTransformPushFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  (void) self; (void) args; (void) kwargs;
  if(!XPLMTransformPush_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMTransformPush is available only in XPLM440 and up.");
    return nullptr;
  }
  XPLMTransformPush_ptr();
  Py_RETURN_NONE;
}

My_DOCSTR(_transformPop__doc__, "transformPop",
          "",
          "",
          "None",
          "Restore the transformation matrix from the top of the transform stack.");
static PyObject *XPLMTransformPopFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  (void) self; (void) args; (void) kwargs;
  if(!XPLMTransformPop_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMTransformPop is available only in XPLM440 and up.");
    return nullptr;
  }
  XPLMTransformPop_ptr();
  Py_RETURN_NONE;
}

My_DOCSTR(_transformTranslate__doc__, "transformTranslate",
          "dx, dy",
          "dx:float, dy:float",
          "None",
          "Offset all subsequent drawing by (dx, dy) pixels.");
static PyObject *XPLMTransformTranslateFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("dx"), CHAR("dy"), nullptr};
  (void) self;
  float dx, dy;
  if(!XPLMTransformTranslate_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMTransformTranslate is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "ff", keywords, &dx, &dy)){
    return nullptr;
  }
  XPLMTransformTranslate_ptr(dx, dy);
  Py_RETURN_NONE;
}

My_DOCSTR(_transformRotate__doc__, "transformRotate",
          "centerX=0, centerY=0, angle=0",
          "centerX:float, centerY:float, angle:float",
          "None",
          "Rotate subsequent drawing around (centerX, centerY) counter-clockwise by angle degrees.");
static PyObject *XPLMTransformRotateFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("centerX"), CHAR("centerY"), CHAR("angle"), nullptr};
  (void) self;
  float centerX=0.0, centerY=0.0, angle=0.0;
  if(!XPLMTransformRotate_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMTransformRotate is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|fff", keywords, &centerX, &centerY, &angle)){
    return nullptr;
  }
  XPLMTransformRotate_ptr(centerX, centerY, angle);
  Py_RETURN_NONE;
}

My_DOCSTR(_transformScale__doc__, "transformScale",
          "scaleX=1, scaleY=1",
          "scaleX:float, scaleY:float",
          "None",
          "Scale subsequent drawing by (scaleX, scaleY) relative to the origin.");
static PyObject *XPLMTransformScaleFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("scaleX"), CHAR("scaleY"), nullptr};
  (void) self;
  float scaleX=1.0, scaleY=1.0;
  if(!XPLMTransformScale_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMTransformScale is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|ff", keywords, &scaleX, &scaleY)){
    return nullptr;
  }
  XPLMTransformScale_ptr(scaleX, scaleY);
  Py_RETURN_NONE;
}

/* ---- Scissor stack ---- */

My_DOCSTR(_scissorPush__doc__, "scissorPush",
          "",
          "",
          "None",
          "Save the current scissor rectangle onto the scissor stack.");
static PyObject *XPLMScissorPushFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  (void) self; (void) args; (void) kwargs;
  if(!XPLMScissorPush_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMScissorPush is available only in XPLM440 and up.");
    return nullptr;
  }
  XPLMScissorPush_ptr();
  Py_RETURN_NONE;
}

My_DOCSTR(_scissorPop__doc__, "scissorPop",
          "",
          "",
          "None",
          "Restore the scissor rectangle from the top of the scissor stack.");
static PyObject *XPLMScissorPopFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  (void) self; (void) args; (void) kwargs;
  if(!XPLMScissorPop_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMScissorPop is available only in XPLM440 and up.");
    return nullptr;
  }
  XPLMScissorPop_ptr();
  Py_RETURN_NONE;
}

My_DOCSTR(_scissorSet__doc__, "scissorSet",
          "left, top, right, bottom",
          "left:int, top:int, right:int, bottom:int",
          "None",
          "Set an absolute scissor rectangle; only pixels inside are drawn.");
static PyObject *XPLMScissorSetFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("left"), CHAR("top"), CHAR("right"), CHAR("bottom"), nullptr};
  (void) self;
  int left, top, right, bottom;
  if(!XPLMScissorSet_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMScissorSet is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "iiii", keywords, &left, &top, &right, &bottom)){
    return nullptr;
  }
  XPLMScissorSet_ptr(left, top, right, bottom);
  Py_RETURN_NONE;
}

My_DOCSTR(_scissorIntersect__doc__, "scissorIntersect",
          "left, top, right, bottom",
          "left:int, top:int, right:int, bottom:int",
          "None",
          "Intersect the current scissor box with the absolute rectangle given by\n"
          "edges (left, top, right, bottom) in panel pixels; the result is their\n"
          "overlap (drawable area only shrinks).");
static PyObject *XPLMScissorIntersectFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("left"), CHAR("top"), CHAR("right"), CHAR("bottom"), nullptr};
  (void) self;
  int left, top, right, bottom;
  if(!XPLMScissorIntersect_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMScissorIntersect is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "iiii", keywords, &left, &top, &right, &bottom)){
    return nullptr;
  }
  XPLMScissorIntersect_ptr(left, top, right, bottom);
  Py_RETURN_NONE;
}

/* ---- Stencil mask ---- */

My_DOCSTR(_beginSetupStencilMask__doc__, "beginSetupStencilMask",
          "bits, mask",
          "bits:int, mask:int",
          "None",
          "Begin stencil mask setup; subsequent drawing writes to the stencil buffer.\n"
          "Parameters are limited to 8-bit quantities."
          );
static PyObject *XPLMBeginSetupStencilMaskFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("bits"), CHAR("mask"), nullptr};
  (void) self;
  unsigned int bits, mask;
  if(!XPLMBeginSetupStencilMask_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMBeginSetupStencilMask is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "II", keywords, &bits, &mask)){
    return nullptr;
  }
  XPLMBeginSetupStencilMask_ptr(bits, mask);
  Py_RETURN_NONE;
}

My_DOCSTR(_endSetupStencilMask__doc__, "endSetupStencilMask",
          "",
          "",
          "None",
          "End stencil mask setup; subsequent drawing renders to the screen again.");
static PyObject *XPLMEndSetupStencilMaskFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  (void) self; (void) args; (void) kwargs;
  if(!XPLMEndSetupStencilMask_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMEndSetupStencilMask is available only in XPLM440 and up.");
    return nullptr;
  }
  XPLMEndSetupStencilMask_ptr();
  Py_RETURN_NONE;
}

My_DOCSTR(_useStencilMask__doc__, "useStencilMask",
          "bits=0, mask=0",
          "bits:int, mask:int",
          "None",
          "Activate stencil testing for subsequent drawing.");
static PyObject *XPLMUseStencilMaskFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("bits"), CHAR("mask"), nullptr};
  (void) self;
  unsigned int bits=0, mask=0;
  if(!XPLMUseStencilMask_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMUseStencilMask is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|II", keywords, &bits, &mask)){
    return nullptr;
  }
  XPLMUseStencilMask_ptr(bits, mask);
  Py_RETURN_NONE;
}

My_DOCSTR(_clearStencilMask__doc__, "clearStencilMask",
          "",
          "",
          "None",
          "Clear the stencil buffer and disable stencil testing.");
static PyObject *XPLMClearStencilMaskFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  (void) self; (void) args; (void) kwargs;
  if(!XPLMClearStencilMask_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMClearStencilMask is available only in XPLM440 and up.");
    return nullptr;
  }
  XPLMClearStencilMask_ptr();
  Py_RETURN_NONE;
}

/* ---- Retained drawing ---- */

/* Tracks whether a beginRetainedDrawing() recording is currently open, so we can
   reject nested begins and unbalanced ends from Python before they reach the SDK
   (the header warns recording sessions must not be nested). Single main thread,
   so a plain bool is sufficient. */
static bool retainedDrawingActive = false;

My_DOCSTR(_beginRetainedDrawing__doc__, "beginRetainedDrawing",
          "",
          "",
          "None",
          "Begin recording panel-graphics commands into a retained drawing. All\n"
          "panel-graphics calls made until endRetainedDrawing() are captured instead\n"
          "of being drawn immediately. Recording sessions must not be nested.");
static PyObject *XPLMBeginRetainedDrawingFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  (void) self; (void) args; (void) kwargs;
  if(!XPLMBeginRetainedDrawing_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMBeginRetainedDrawing is available only in XPLM440 and up.");
    return nullptr;
  }
  if(retainedDrawingActive){
    PyErr_SetString(PyExc_RuntimeError, "beginRetainedDrawing: a retained-drawing recording is already active (sessions must not be nested).");
    return nullptr;
  }
  XPLMBeginRetainedDrawing_ptr();
  retainedDrawingActive = true;
  Py_RETURN_NONE;
}

My_DOCSTR(_endRetainedDrawing__doc__, "endRetainedDrawing",
          "",
          "",
          "XPLMRetainedDrawing",
          "End the recording started by beginRetainedDrawing() and return an opaque\n"
          "handle to the captured commands. Replay it with drawRetained() and free it\n"
          "with destroyRetainedDrawing(). If a font or texture atlas used during\n"
          "recording is destroyed, you must destroy the retained drawing as well --\n"
          "replaying it afterwards references invalid resources.");
static PyObject *XPLMEndRetainedDrawingFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  (void) self; (void) args; (void) kwargs;
  if(!XPLMEndRetainedDrawing_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMEndRetainedDrawing is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!retainedDrawingActive){
    PyErr_SetString(PyExc_RuntimeError, "endRetainedDrawing: no retained-drawing recording is active (call beginRetainedDrawing() first).");
    return nullptr;
  }
  XPLMRetainedDrawing_t drawing = XPLMEndRetainedDrawing_ptr();
  retainedDrawingActive = false;
  return makeCapsule(drawing, RETAINED_CAPSULE);
}

My_DOCSTR(_drawRetained__doc__, "drawRetained",
          "drawing",
          "drawing:XPLMRetainedDrawing",
          "None",
          "Replay a retained drawing captured with endRetainedDrawing(). May be\n"
          "called any number of times per frame and across frames to redraw the same\n"
          "content cheaply.");
static PyObject *XPLMDrawRetainedFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("drawing"), nullptr};
  (void) self;
  PyObject *drawingCapsule;
  if(!XPLMDrawRetained_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMDrawRetained is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &drawingCapsule)){
    return nullptr;
  }
  XPLMRetainedDrawing_t drawing = getVoidPtr(drawingCapsule, RETAINED_CAPSULE);
  if(!drawing && PyErr_Occurred()){
    return nullptr;
  }
  XPLMDrawRetained_ptr(drawing);
  Py_RETURN_NONE;
}

My_DOCSTR(_destroyRetainedDrawing__doc__, "destroyRetainedDrawing",
          "drawing",
          "drawing:XPLMRetainedDrawing",
          "None",
          "Destroy a retained drawing captured with endRetainedDrawing() and free its\n"
          "resources. The handle must not be used after this call.");
static PyObject *XPLMDestroyRetainedDrawingFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("drawing"), nullptr};
  (void) self;
  PyObject *drawingCapsule;
  if(!XPLMDestroyRetainedDrawing_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMDestroyRetainedDrawing is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &drawingCapsule)){
    return nullptr;
  }
  XPLMRetainedDrawing_t drawing = getVoidPtr(drawingCapsule, RETAINED_CAPSULE);
  if(!drawing && PyErr_Occurred()){
    return nullptr;
  }
  XPLMDestroyRetainedDrawing_ptr(drawing);
  deleteCapsule(drawingCapsule);
  Py_RETURN_NONE;
}


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
static PyMethodDef XPLMPanelGraphicsMethods[] = {
  {"transformPush", (PyCFunction)XPLMTransformPushFun, METH_VARARGS | METH_KEYWORDS, _transformPush__doc__},
  {"XPLMTransformPush", (PyCFunction)XPLMTransformPushFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"transformPop", (PyCFunction)XPLMTransformPopFun, METH_VARARGS | METH_KEYWORDS, _transformPop__doc__},
  {"XPLMTransformPop", (PyCFunction)XPLMTransformPopFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"transformTranslate", (PyCFunction)XPLMTransformTranslateFun, METH_VARARGS | METH_KEYWORDS, _transformTranslate__doc__},
  {"XPLMTransformTranslate", (PyCFunction)XPLMTransformTranslateFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"transformRotate", (PyCFunction)XPLMTransformRotateFun, METH_VARARGS | METH_KEYWORDS, _transformRotate__doc__},
  {"XPLMTransformRotate", (PyCFunction)XPLMTransformRotateFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"transformScale", (PyCFunction)XPLMTransformScaleFun, METH_VARARGS | METH_KEYWORDS, _transformScale__doc__},
  {"XPLMTransformScale", (PyCFunction)XPLMTransformScaleFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"scissorPush", (PyCFunction)XPLMScissorPushFun, METH_VARARGS | METH_KEYWORDS, _scissorPush__doc__},
  {"XPLMScissorPush", (PyCFunction)XPLMScissorPushFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"scissorPop", (PyCFunction)XPLMScissorPopFun, METH_VARARGS | METH_KEYWORDS, _scissorPop__doc__},
  {"XPLMScissorPop", (PyCFunction)XPLMScissorPopFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"scissorSet", (PyCFunction)XPLMScissorSetFun, METH_VARARGS | METH_KEYWORDS, _scissorSet__doc__},
  {"XPLMScissorSet", (PyCFunction)XPLMScissorSetFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"scissorIntersect", (PyCFunction)XPLMScissorIntersectFun, METH_VARARGS | METH_KEYWORDS, _scissorIntersect__doc__},
  {"XPLMScissorIntersect", (PyCFunction)XPLMScissorIntersectFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"beginSetupStencilMask", (PyCFunction)XPLMBeginSetupStencilMaskFun, METH_VARARGS | METH_KEYWORDS, _beginSetupStencilMask__doc__},
  {"XPLMBeginSetupStencilMask", (PyCFunction)XPLMBeginSetupStencilMaskFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"endSetupStencilMask", (PyCFunction)XPLMEndSetupStencilMaskFun, METH_VARARGS | METH_KEYWORDS, _endSetupStencilMask__doc__},
  {"XPLMEndSetupStencilMask", (PyCFunction)XPLMEndSetupStencilMaskFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"useStencilMask", (PyCFunction)XPLMUseStencilMaskFun, METH_VARARGS | METH_KEYWORDS, _useStencilMask__doc__},
  {"XPLMUseStencilMask", (PyCFunction)XPLMUseStencilMaskFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"clearStencilMask", (PyCFunction)XPLMClearStencilMaskFun, METH_VARARGS | METH_KEYWORDS, _clearStencilMask__doc__},
  {"XPLMClearStencilMask", (PyCFunction)XPLMClearStencilMaskFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"beginRetainedDrawing", (PyCFunction)XPLMBeginRetainedDrawingFun, METH_VARARGS | METH_KEYWORDS, _beginRetainedDrawing__doc__},
  {"XPLMBeginRetainedDrawing", (PyCFunction)XPLMBeginRetainedDrawingFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"endRetainedDrawing", (PyCFunction)XPLMEndRetainedDrawingFun, METH_VARARGS | METH_KEYWORDS, _endRetainedDrawing__doc__},
  {"XPLMEndRetainedDrawing", (PyCFunction)XPLMEndRetainedDrawingFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"drawRetained", (PyCFunction)XPLMDrawRetainedFun, METH_VARARGS | METH_KEYWORDS, _drawRetained__doc__},
  {"XPLMDrawRetained", (PyCFunction)XPLMDrawRetainedFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"destroyRetainedDrawing", (PyCFunction)XPLMDestroyRetainedDrawingFun, METH_VARARGS | METH_KEYWORDS, _destroyRetainedDrawing__doc__},
  {"XPLMDestroyRetainedDrawing", (PyCFunction)XPLMDestroyRetainedDrawingFun, METH_VARARGS | METH_KEYWORDS, ""},
  {nullptr, nullptr, 0, nullptr}
};
#pragma GCC diagnostic pop

static struct PyModuleDef XPLMPanelGraphicsModule = {
  PyModuleDef_HEAD_INIT,
  "XPLMPanelGraphics",
  "Laminar documentation: \n"
  "   https://developer.x-plane.com/sdk/XPLMPanelGraphics/\n"
  "XPPython3 documentation: \n"
  "   https://xppython3.rtfd.io/en/stable/development/modules/panelgraphics.html",
  -1,
  XPLMPanelGraphicsMethods,
  nullptr,
  nullptr,
  nullptr,
  nullptr
};

PyMODINIT_FUNC
PyInit_XPLMPanelGraphics(void)
{
  PyObject *mod = PyModule_Create(&XPLMPanelGraphicsModule);
  if(mod){
    PyModule_AddStringConstant(mod, "__author__", "Peter Buckner (pbuck@xppython3.org)");

    /* Merge the method-table fragments defined in the sibling
       panel_graphics_*.cpp files. */
    PyModule_AddFunctions(mod, panelGraphicsPrimitivesMethods);
    PyModule_AddFunctions(mod, panelGraphicsFontMethods);
    PyModule_AddFunctions(mod, panelGraphicsTextureMethods);
    PyModule_AddFunctions(mod, panelGraphicsDisplayMethods);
    PyModule_AddFunctions(mod, panelGraphicsTouchMethods);

    /* XPLMLineCap_t */
    PyModule_AddIntConstant(mod, "xplm_LineCapButt", xplm_LineCapButt);
    PyModule_AddIntConstant(mod, "xplm_LineCapRound", xplm_LineCapRound);
    PyModule_AddIntConstant(mod, "xplm_LineCapSquare", xplm_LineCapSquare);
    PyModule_AddIntConstant(mod, "LineCapButt", xplm_LineCapButt);
    PyModule_AddIntConstant(mod, "LineCapRound", xplm_LineCapRound);
    PyModule_AddIntConstant(mod, "LineCapSquare", xplm_LineCapSquare);

    /* XPLMCharSet_t */
    PyModule_AddIntConstant(mod, "xplm_CharSetDigits", xplm_CharSetDigits);
    PyModule_AddIntConstant(mod, "xplm_CharSetASCII", xplm_CharSetASCII);
    PyModule_AddIntConstant(mod, "xplm_CharSetUnicode", xplm_CharSetUnicode);
    PyModule_AddIntConstant(mod, "CharSetDigits", xplm_CharSetDigits);
    PyModule_AddIntConstant(mod, "CharSetASCII", xplm_CharSetASCII);
    PyModule_AddIntConstant(mod, "CharSetUnicode", xplm_CharSetUnicode);

    /* XPLMTextureSource */
    PyModule_AddIntConstant(mod, "xplm_Texture_WeatherRadar1", xplm_Texture_WeatherRadar1);
    PyModule_AddIntConstant(mod, "xplm_Texture_WeatherRadar2", xplm_Texture_WeatherRadar2);
    PyModule_AddIntConstant(mod, "Texture_WeatherRadar1", xplm_Texture_WeatherRadar1);
    PyModule_AddIntConstant(mod, "Texture_WeatherRadar2", xplm_Texture_WeatherRadar2);

    /* XPLMSVTFeatures */
    PyModule_AddIntConstant(mod, "xplm_SVT_Terrain", xplm_SVT_Terrain);
    PyModule_AddIntConstant(mod, "xplm_SVT_Runways", xplm_SVT_Runways);
    PyModule_AddIntConstant(mod, "xplm_SVT_Obstacles", xplm_SVT_Obstacles);
    PyModule_AddIntConstant(mod, "xplm_SVT_FlightPath", xplm_SVT_FlightPath);
    PyModule_AddIntConstant(mod, "xplm_SVT_Traffic", xplm_SVT_Traffic);
    PyModule_AddIntConstant(mod, "xplm_SVT_AirportSigns", xplm_SVT_AirportSigns);
    PyModule_AddIntConstant(mod, "xplm_SVT_ILSHoops", xplm_SVT_ILSHoops);
    PyModule_AddIntConstant(mod, "xplm_SVT_HorizonHeading", xplm_SVT_HorizonHeading);
    PyModule_AddIntConstant(mod, "xplm_SVT_All", xplm_SVT_All);
    PyModule_AddIntConstant(mod, "SVT_Terrain", xplm_SVT_Terrain);
    PyModule_AddIntConstant(mod, "SVT_Runways", xplm_SVT_Runways);
    PyModule_AddIntConstant(mod, "SVT_Obstacles", xplm_SVT_Obstacles);
    PyModule_AddIntConstant(mod, "SVT_FlightPath", xplm_SVT_FlightPath);
    PyModule_AddIntConstant(mod, "SVT_Traffic", xplm_SVT_Traffic);
    PyModule_AddIntConstant(mod, "SVT_AirportSigns", xplm_SVT_AirportSigns);
    PyModule_AddIntConstant(mod, "SVT_ILSHoops", xplm_SVT_ILSHoops);
    PyModule_AddIntConstant(mod, "SVT_HorizonHeading", xplm_SVT_HorizonHeading);
    PyModule_AddIntConstant(mod, "SVT_All", xplm_SVT_All);

    /* XPLMMapLayers */
    PyModule_AddIntConstant(mod, "xplm_Map_Nexrad", xplm_Map_Nexrad);
    PyModule_AddIntConstant(mod, "xplm_Map_IR", xplm_Map_IR);
    PyModule_AddIntConstant(mod, "xplm_Map_Topo", xplm_Map_Topo);
    PyModule_AddIntConstant(mod, "xplm_Map_Terrain", xplm_Map_Terrain);
    PyModule_AddIntConstant(mod, "xplm_Map_Water", xplm_Map_Water);
    PyModule_AddIntConstant(mod, "xplm_Map_EGPWS", xplm_Map_EGPWS);
    PyModule_AddIntConstant(mod, "xplm_Map_raw_elev", xplm_Map_raw_elev);
    PyModule_AddIntConstant(mod, "xplm_Map_safe_taxi", xplm_Map_safe_taxi);
    PyModule_AddIntConstant(mod, "Map_Nexrad", xplm_Map_Nexrad);
    PyModule_AddIntConstant(mod, "Map_IR", xplm_Map_IR);
    PyModule_AddIntConstant(mod, "Map_Topo", xplm_Map_Topo);
    PyModule_AddIntConstant(mod, "Map_Terrain", xplm_Map_Terrain);
    PyModule_AddIntConstant(mod, "Map_Water", xplm_Map_Water);
    PyModule_AddIntConstant(mod, "Map_EGPWS", xplm_Map_EGPWS);
    PyModule_AddIntConstant(mod, "Map_raw_elev", xplm_Map_raw_elev);
    PyModule_AddIntConstant(mod, "Map_safe_taxi", xplm_Map_safe_taxi);

    PyModule_AddIntConstant(mod, "xplm_EGPWS_Style_Blocky", xplm_EGPWS_Style_Blocky); //XPLMEGPWSStyle
    PyModule_AddIntConstant(mod, "xplm_EGPWS_Style_Smooth", xplm_EGPWS_Style_Smooth); //XPLMEGPWSStyle
    PyModule_AddIntConstant(mod, "EGPWS_Style_Blocky", xplm_EGPWS_Style_Blocky); //XPLMEGPWSStyle
    PyModule_AddIntConstant(mod, "EGPWS_Style_Smooth", xplm_EGPWS_Style_Smooth); //XPLMEGPWSStyle

    /* XPLMTouchZone */
    PyModule_AddIntConstant(mod, "xplm_TouchZone_Nothing", xplm_TouchZone_Nothing);
    PyModule_AddIntConstant(mod, "xplm_TouchZone_Command", xplm_TouchZone_Command);
    PyModule_AddIntConstant(mod, "xplm_TouchZone_Identifier", xplm_TouchZone_Identifier);
    PyModule_AddIntConstant(mod, "TouchZone_Nothing", xplm_TouchZone_Nothing);
    PyModule_AddIntConstant(mod, "TouchZone_Command", xplm_TouchZone_Command);
    PyModule_AddIntConstant(mod, "TouchZone_Identifier", xplm_TouchZone_Identifier);

    /* XPLMJustification_t */
    PyModule_AddIntConstant(mod, "xplm_JustLeft", xplm_JustLeft);
    PyModule_AddIntConstant(mod, "xplm_JustCenter", xplm_JustCenter);
    PyModule_AddIntConstant(mod, "xplm_JustRight", xplm_JustRight);
    PyModule_AddIntConstant(mod, "JustLeft", xplm_JustLeft);
    PyModule_AddIntConstant(mod, "JustCenter", xplm_JustCenter);
    PyModule_AddIntConstant(mod, "JustRight", xplm_JustRight);
  }
  return mod;
}
