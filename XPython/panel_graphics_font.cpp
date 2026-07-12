#define _GNU_SOURCE 1
#define PY_SSIZE_T_CLEAN   /* required for 'y#' (bytes+length) formats below */
#include <Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdint>
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMPanelGraphics.h>
#include "utils.h"
#include "panel_graphics.h"
#include "plugin_dl.h"
#include "capsules.h"

/* ---- Fonts ---- */

My_DOCSTR(_createFont__doc__, "createFont",
          "charset",
          "charset:int",
          "XPLMFontHandle",
          "Create a new font handle for the given character set (one of\n"
          "CharSetDigits, CharSetASCII, CharSetUnicode). Add one or more TrueType\n"
          "faces with fontAddFace() before drawing, and destroyFont() when done.");
static PyObject *XPLMCreateFontFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("charset"), nullptr};
  (void) self;
  int charset;
  if(!XPLMCreateFont_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMCreateFont is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "i", keywords, &charset)){
    return nullptr;
  }
  XPLMFontHandle font = XPLMCreateFont_ptr((XPLMCharSet_t)charset);
  if(!font){
    PyErr_SetString(PyExc_RuntimeError , "XPLMCreateFont failed.");
    return nullptr;
  }
  return makeCapsule(font, FONT_CAPSULE);
}

My_DOCSTR(_destroyFont__doc__, "destroyFont",
          "font",
          "font:XPLMFontHandle",
          "None",
          "Destroy a font handle created with createFont() and free its resources.");
static PyObject *XPLMDestroyFontFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("font"), nullptr};
  (void) self;
  PyObject *fontCapsule;
  if(!XPLMDestroyFont_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMDestroyFont is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &fontCapsule)){
    return nullptr;
  }
  XPLMFontHandle font = getVoidPtr(fontCapsule, FONT_CAPSULE);
  XPLMDestroyFont_ptr(font);
  deleteCapsule(fontCapsule);
  Py_RETURN_NONE;
}

My_DOCSTR(_fontAddFace__doc__, "fontAddFace",
          "font, ttf_path",
          "font:XPLMFontHandle, ttf_path:str",
          "None",
          "Add a TrueType (.ttf/.otf) face to a font handle. Multiple faces may be\n"
          "added to provide fallback glyphs, searched in the order added.");
static PyObject *XPLMFontAddFaceFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("font"), CHAR("ttf_path"), nullptr};
  (void) self;
  PyObject *fontCapsule;
  const char *ttfPath;
  if(!XPLMFontAddFace_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMFontAddFace is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Os", keywords, &fontCapsule, &ttfPath)){
    return nullptr;
  }
  XPLMFontHandle font = getVoidPtr(fontCapsule, FONT_CAPSULE);
  XPLMFontAddFace_ptr(font, ttfPath);
  Py_RETURN_NONE;
}

My_DOCSTR(_fontGetMetrics__doc__, "fontGetMetrics",
          "font, fontSize",
          "font:XPLMFontHandle, fontSize:float",
          "tuple[float, float, float]",
          "Return (lineHeight, lineAscent, lineDescent) in pixels for the font at\n"
          "the given size.");
static PyObject *XPLMFontGetMetricsFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("font"), CHAR("fontSize"), nullptr};
  (void) self;
  PyObject *fontCapsule;
  float fontSize;
  if(!XPLMFontGetMetrics_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMFontGetMetrics is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Of", keywords, &fontCapsule, &fontSize)){
    return nullptr;
  }
  XPLMFontHandle font = getVoidPtr(fontCapsule, FONT_CAPSULE);
  XPLMFontMetrics_t metrics;
  metrics.structSize = sizeof(XPLMFontMetrics_t);
  XPLMFontGetMetrics_ptr(font, fontSize, &metrics);
  return Py_BuildValue("(fff)", metrics.lineHeight, metrics.lineAscent, metrics.lineDescent);
}

My_DOCSTR(_fontMeasureString__doc__, "fontMeasureString",
          "font, fontSize, string",
          "font:XPLMFontHandle, fontSize:float, string:str",
          "float",
          "Return the width in pixels the string would occupy if drawn. The string\n"
          "is not drawn.");
static PyObject *XPLMFontMeasureStringFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("font"), CHAR("fontSize"), CHAR("string"), nullptr};
  (void) self;
  PyObject *fontCapsule;
  float fontSize;
  const char *string;
  if(!XPLMFontMeasureString_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMFontMeasureString is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Ofs", keywords, &fontCapsule, &fontSize, &string)){
    return nullptr;
  }
  XPLMFontHandle font = getVoidPtr(fontCapsule, FONT_CAPSULE);
  return PyFloat_FromDouble(XPLMFontMeasureString_ptr(font, fontSize, string));
}

My_DOCSTR(_fontGetLineCount__doc__, "fontGetLineCount",
          "font, fontSize, string, width",
          "font:XPLMFontHandle, fontSize:float, string:str, width:float",
          "int",
          "Return how many lines the string would occupy if word-wrapped to width.");
static PyObject *XPLMFontGetLineCountFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("font"), CHAR("fontSize"), CHAR("string"), CHAR("width"), nullptr};
  (void) self;
  PyObject *fontCapsule;
  float fontSize, width;
  const char *string;
  if(!XPLMFontGetLineCount_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMFontGetLineCount is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Ofsf", keywords, &fontCapsule, &fontSize, &string, &width)){
    return nullptr;
  }
  XPLMFontHandle font = getVoidPtr(fontCapsule, FONT_CAPSULE);
  return PyLong_FromLong(XPLMFontGetLineCount_ptr(font, fontSize, string, width));
}

My_DOCSTR(_fontFitForward__doc__, "fontFitForward",
          "font, fontSize, string, width",
          "font:XPLMFontHandle, fontSize:float, string:str, width:float",
          "int",
          "Return the number of characters from the start of the string that fit\n"
          "within width, measured left to right.");
static PyObject *XPLMFontFitForwardFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("font"), CHAR("fontSize"), CHAR("string"), CHAR("width"), nullptr};
  (void) self;
  PyObject *fontCapsule;
  float fontSize, width;
  const char *string;
  if(!XPLMFontFitForward_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMFontFitForward is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Ofsf", keywords, &fontCapsule, &fontSize, &string, &width)){
    return nullptr;
  }
  XPLMFontHandle font = getVoidPtr(fontCapsule, FONT_CAPSULE);
  return PyLong_FromLong(XPLMFontFitForward_ptr(font, fontSize, string, width));
}

My_DOCSTR(_fontFitReverse__doc__, "fontFitReverse",
          "font, fontSize, string, width",
          "font:XPLMFontHandle, fontSize:float, string:str, width:float",
          "int",
          "Return the START INDEX of the longest suffix that fits within width,\n"
          "measured right to left; the fitting tail is string[index:]. (The SDK\n"
          "header calls this a character count, but it returns an index: 0 when\n"
          "the whole string fits, len(string) when nothing fits. Trailing-char\n"
          "count = len(string) - index.)");
static PyObject *XPLMFontFitReverseFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("font"), CHAR("fontSize"), CHAR("string"), CHAR("width"), nullptr};
  (void) self;
  PyObject *fontCapsule;
  float fontSize, width;
  const char *string;
  if(!XPLMFontFitReverse_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMFontFitReverse is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Ofsf", keywords, &fontCapsule, &fontSize, &string, &width)){
    return nullptr;
  }
  XPLMFontHandle font = getVoidPtr(fontCapsule, FONT_CAPSULE);
  return PyLong_FromLong(XPLMFontFitReverse_ptr(font, fontSize, string, width));
}

My_DOCSTR(_fontDrawString__doc__, "fontDrawString",
          "font, color, fontSize, x, y, string, justification",
          "font:XPLMFontHandle, color:int, fontSize:float, x:float, y:float, string:str, justification:int",
          "None",
          "Draw a string at (x, y) (the baseline anchor) with the given font, size,\n"
          "packed color, and justification (JustLeft, JustCenter, JustRight).");
static PyObject *XPLMFontDrawStringFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("font"), CHAR("color"), CHAR("fontSize"), CHAR("x"), CHAR("y"),
                             CHAR("string"), CHAR("justification"), nullptr};
  (void) self;
  PyObject *fontCapsule;
  unsigned long color;
  float fontSize, x, y;
  const char *string;
  int justification;
  if(!XPLMFontDrawString_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMFontDrawString is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Okfffsi", keywords,
                                  &fontCapsule, &color, &fontSize, &x, &y, &string, &justification)){
    return nullptr;
  }
  XPLMFontHandle font = getVoidPtr(fontCapsule, FONT_CAPSULE);
  XPLMFontDrawString_ptr(font, (uint32_t)color, fontSize, x, y, string, (XPLMJustification_t)justification);
  Py_RETURN_NONE;
}

My_DOCSTR(_fontDrawStringFixedSpacing__doc__, "fontDrawStringFixedSpacing",
          "font, color, fontSize, x, y, string, fixedSpacing, justification",
          "font:XPLMFontHandle, color:int, fontSize:float, x:float, y:float, string:str, fixedSpacing:int, justification:int",
          "None",
          "Draw a string using fixed per-character spacing (in pixels) instead of\n"
          "the font's natural proportional spacing. Useful for numeric readouts.");
static PyObject *XPLMFontDrawStringFixedSpacingFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("font"), CHAR("color"), CHAR("fontSize"), CHAR("x"), CHAR("y"),
                             CHAR("string"), CHAR("fixedSpacing"), CHAR("justification"), nullptr};
  (void) self;
  PyObject *fontCapsule;
  unsigned long color;
  float fontSize, x, y;
  const char *string;
  int fixedSpacing, justification;
  if(!XPLMFontDrawStringFixedSpacing_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMFontDrawStringFixedSpacing is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Okfffsii", keywords,
                                  &fontCapsule, &color, &fontSize, &x, &y, &string, &fixedSpacing, &justification)){
    return nullptr;
  }
  XPLMFontHandle font = getVoidPtr(fontCapsule, FONT_CAPSULE);
  XPLMFontDrawStringFixedSpacing_ptr(font, (uint32_t)color, fontSize, x, y, string, fixedSpacing,
                                     (XPLMJustification_t)justification);
  Py_RETURN_NONE;
}

My_DOCSTR(_fontDrawStringWordWrapped__doc__, "fontDrawStringWordWrapped",
          "font, color, fontSize, x, y, string, wrapWidth, justification",
          "font:XPLMFontHandle, color:int, fontSize:float, x:float, y:float, string:str, wrapWidth:int, justification:int",
          "None",
          "Draw a string with automatic word wrapping at wrapWidth pixels. Lines\n"
          "stack downward from the initial y position by the font's line height.");
static PyObject *XPLMFontDrawStringWordWrappedFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("font"), CHAR("color"), CHAR("fontSize"), CHAR("x"), CHAR("y"),
                             CHAR("string"), CHAR("wrapWidth"), CHAR("justification"), nullptr};
  (void) self;
  PyObject *fontCapsule;
  unsigned long color;
  float fontSize, x, y;
  const char *string;
  int wrapWidth, justification;
  if(!XPLMFontDrawStringWordWrapped_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMFontDrawStringWordWrapped is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Okfffsii", keywords,
                                  &fontCapsule, &color, &fontSize, &x, &y, &string, &wrapWidth, &justification)){
    return nullptr;
  }
  XPLMFontHandle font = getVoidPtr(fontCapsule, FONT_CAPSULE);
  XPLMFontDrawStringWordWrapped_ptr(font, (uint32_t)color, fontSize, x, y, string, wrapWidth,
                                    (XPLMJustification_t)justification);
  Py_RETURN_NONE;
}

My_DOCSTR(_fontDrawStringRotated__doc__, "fontDrawStringRotated",
          "font, color, fontSize, x, y, string, angle, justification",
          "font:XPLMFontHandle, color:int, fontSize:float, x:float, y:float, string:str, angle:float, justification:int",
          "None",
          "Draw a string rotated by angle degrees (positive counterclockwise)\n"
          "around the (x, y) anchor point.");
static PyObject *XPLMFontDrawStringRotatedFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("font"), CHAR("color"), CHAR("fontSize"), CHAR("x"), CHAR("y"),
                             CHAR("string"), CHAR("angle"), CHAR("justification"), nullptr};
  (void) self;
  PyObject *fontCapsule;
  unsigned long color;
  float fontSize, x, y, angle;
  const char *string;
  int justification;
  if(!XPLMFontDrawStringRotated_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMFontDrawStringRotated is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Okfffsfi", keywords,
                                  &fontCapsule, &color, &fontSize, &x, &y, &string, &angle, &justification)){
    return nullptr;
  }
  XPLMFontHandle font = getVoidPtr(fontCapsule, FONT_CAPSULE);
  XPLMFontDrawStringRotated_ptr(font, (uint32_t)color, fontSize, x, y, string, angle,
                                (XPLMJustification_t)justification);
  Py_RETURN_NONE;
}


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
PyMethodDef panelGraphicsFontMethods[] = {
  {"createFont", (PyCFunction)XPLMCreateFontFun, METH_VARARGS | METH_KEYWORDS, _createFont__doc__},
  {"XPLMCreateFont", (PyCFunction)XPLMCreateFontFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"destroyFont", (PyCFunction)XPLMDestroyFontFun, METH_VARARGS | METH_KEYWORDS, _destroyFont__doc__},
  {"XPLMDestroyFont", (PyCFunction)XPLMDestroyFontFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"fontAddFace", (PyCFunction)XPLMFontAddFaceFun, METH_VARARGS | METH_KEYWORDS, _fontAddFace__doc__},
  {"XPLMFontAddFace", (PyCFunction)XPLMFontAddFaceFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"fontGetMetrics", (PyCFunction)XPLMFontGetMetricsFun, METH_VARARGS | METH_KEYWORDS, _fontGetMetrics__doc__},
  {"XPLMFontGetMetrics", (PyCFunction)XPLMFontGetMetricsFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"fontMeasureString", (PyCFunction)XPLMFontMeasureStringFun, METH_VARARGS | METH_KEYWORDS, _fontMeasureString__doc__},
  {"XPLMFontMeasureString", (PyCFunction)XPLMFontMeasureStringFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"fontGetLineCount", (PyCFunction)XPLMFontGetLineCountFun, METH_VARARGS | METH_KEYWORDS, _fontGetLineCount__doc__},
  {"XPLMFontGetLineCount", (PyCFunction)XPLMFontGetLineCountFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"fontFitForward", (PyCFunction)XPLMFontFitForwardFun, METH_VARARGS | METH_KEYWORDS, _fontFitForward__doc__},
  {"XPLMFontFitForward", (PyCFunction)XPLMFontFitForwardFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"fontFitReverse", (PyCFunction)XPLMFontFitReverseFun, METH_VARARGS | METH_KEYWORDS, _fontFitReverse__doc__},
  {"XPLMFontFitReverse", (PyCFunction)XPLMFontFitReverseFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"fontDrawString", (PyCFunction)XPLMFontDrawStringFun, METH_VARARGS | METH_KEYWORDS, _fontDrawString__doc__},
  {"XPLMFontDrawString", (PyCFunction)XPLMFontDrawStringFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"fontDrawStringFixedSpacing", (PyCFunction)XPLMFontDrawStringFixedSpacingFun, METH_VARARGS | METH_KEYWORDS, _fontDrawStringFixedSpacing__doc__},
  {"XPLMFontDrawStringFixedSpacing", (PyCFunction)XPLMFontDrawStringFixedSpacingFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"fontDrawStringWordWrapped", (PyCFunction)XPLMFontDrawStringWordWrappedFun, METH_VARARGS | METH_KEYWORDS, _fontDrawStringWordWrapped__doc__},
  {"XPLMFontDrawStringWordWrapped", (PyCFunction)XPLMFontDrawStringWordWrappedFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"fontDrawStringRotated", (PyCFunction)XPLMFontDrawStringRotatedFun, METH_VARARGS | METH_KEYWORDS, _fontDrawStringRotated__doc__},
  {"XPLMFontDrawStringRotated", (PyCFunction)XPLMFontDrawStringRotatedFun, METH_VARARGS | METH_KEYWORDS, ""},
  {nullptr, nullptr, 0, nullptr}
};
#pragma GCC diagnostic pop
