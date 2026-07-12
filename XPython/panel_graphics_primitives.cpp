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

/* XPLMPanelGraphics primitive drawing calls: color packing, the line / polygon /
   quad-strip families (plain, WithWidth, stipple, and per-vertex-color 'c'
   variants), and the (x, y) / (x, y, color) vertex parsers they share. The
   method-table fragment below is merged into the XPLMPanelGraphics module by
   PyInit_XPLMPanelGraphics in panel_graphics.cpp. */

/* Parse a Python sequence of (x, y) pairs into a malloc'd XPLMVertex_t array.
   Caller must free() the result. On error, sets an exception and returns nullptr.
   An empty sequence returns a non-null pointer with *outCount == 0. */
static XPLMVertex_t *parseVertices(PyObject *verticesObj, Py_ssize_t *outCount)
{
  *outCount = 0;
  PyObject *seq = PySequence_Fast(verticesObj, "vertices must be a sequence of (x, y) pairs");
  if(!seq){
    return nullptr;
  }
  Py_ssize_t count = PySequence_Fast_GET_SIZE(seq);
  XPLMVertex_t *vertices = (XPLMVertex_t *)malloc((count ? count : 1) * sizeof(XPLMVertex_t));
  if(!vertices){
    Py_DECREF(seq);
    PyErr_NoMemory();
    return nullptr;
  }
  for(Py_ssize_t i = 0; i < count; i++){
    PyObject *itemSeq = PySequence_Fast(PySequence_Fast_GET_ITEM(seq, i), "each vertex must be an (x, y) pair");
    if(!itemSeq || PySequence_Fast_GET_SIZE(itemSeq) < 2){
      Py_XDECREF(itemSeq);
      free(vertices);
      Py_DECREF(seq);
      PyErr_SetString(PyExc_ValueError, "each vertex must be an (x, y) pair");
      return nullptr;
    }
    vertices[i].x = (float)PyFloat_AsDouble(PySequence_Fast_GET_ITEM(itemSeq, 0));
    vertices[i].y = (float)PyFloat_AsDouble(PySequence_Fast_GET_ITEM(itemSeq, 1));
    Py_DECREF(itemSeq);
  }
  Py_DECREF(seq);
  if(PyErr_Occurred()){
    free(vertices);
    return nullptr;
  }
  *outCount = count;
  return vertices;
}

/* Parse a Python sequence of (x, y, color) triples into a malloc'd
   XPLMVertexColor_t array. Caller must free() the result. */
static XPLMVertexColor_t *parseVertexColors(PyObject *verticesObj, Py_ssize_t *outCount)
{
  *outCount = 0;
  PyObject *seq = PySequence_Fast(verticesObj, "vertices must be a sequence of (x, y, color) triples");
  if(!seq){
    return nullptr;
  }
  Py_ssize_t count = PySequence_Fast_GET_SIZE(seq);
  XPLMVertexColor_t *vertices = (XPLMVertexColor_t *)malloc((count ? count : 1) * sizeof(XPLMVertexColor_t));
  if(!vertices){
    Py_DECREF(seq);
    PyErr_NoMemory();
    return nullptr;
  }
  for(Py_ssize_t i = 0; i < count; i++){
    PyObject *itemSeq = PySequence_Fast(PySequence_Fast_GET_ITEM(seq, i), "each vertex must be an (x, y, color) triple");
    if(!itemSeq || PySequence_Fast_GET_SIZE(itemSeq) < 3){
      Py_XDECREF(itemSeq);
      free(vertices);
      Py_DECREF(seq);
      PyErr_SetString(PyExc_ValueError, "each vertex must be an (x, y, color) triple");
      return nullptr;
    }
    vertices[i].x = (float)PyFloat_AsDouble(PySequence_Fast_GET_ITEM(itemSeq, 0));
    vertices[i].y = (float)PyFloat_AsDouble(PySequence_Fast_GET_ITEM(itemSeq, 1));
    vertices[i].color = (uint32_t)PyLong_AsUnsignedLong(PySequence_Fast_GET_ITEM(itemSeq, 2));
    Py_DECREF(itemSeq);
  }
  Py_DECREF(seq);
  if(PyErr_Occurred()){
    free(vertices);
    return nullptr;
  }
  *outCount = count;
  return vertices;
}

My_DOCSTR(_makeColor__doc__, "makeColor",
          "red, green, blue, alpha",
          "red:float, green:float, blue:float, alpha:float",
          "int",
          "Pack four float color components (each 0.0-1.0, clamped) into a single\n"
          "packed color value (ABGR) for use with the panel graphics routines.");
static PyObject *XPLMMakeColorFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("red"), CHAR("green"), CHAR("blue"), CHAR("alpha"), nullptr};
  (void) self;
  float red, green, blue, alpha;
  if(!XPLMMakeColor_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMMakeColor is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "ffff", keywords, &red, &green, &blue, &alpha)){
    return nullptr;
  }
  return PyLong_FromUnsignedLong(XPLMMakeColor_ptr(red, green, blue, alpha));
}

/* ---- Geometry primitives taking (color, vertices) ---- */

My_DOCSTR(_lines__doc__, "lines",
          "color, vertices",
          "color:int, vertices:Sequence[tuple[float, float]]",
          "None",
          "Draw disconnected line segments. Each consecutive pair of vertices forms\n"
          "one segment.");
static PyObject *XPLMLinesFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("color"), CHAR("vertices"), nullptr};
  (void) self;
  unsigned long color;
  PyObject *verticesObj;
  if(!XPLMLines_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMLines is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "kO", keywords, &color, &verticesObj)){
    return nullptr;
  }
  Py_ssize_t count;
  XPLMVertex_t *v = parseVertices(verticesObj, &count);
  if(!v){
    return nullptr;
  }
  XPLMLines_ptr((uint32_t)color, v, (int)count);
  free(v);
  Py_RETURN_NONE;
}

My_DOCSTR(_linesWithWidth__doc__, "linesWithWidth",
          "color, lineWidth, vertices",
          "color:int, lineWidth:float, vertices:Sequence[tuple[float, float]]",
          "None",
          "Draw disconnected line segments with the given line width.");
static PyObject *XPLMLinesWithWidthFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("color"), CHAR("lineWidth"), CHAR("vertices"), nullptr};
  (void) self;
  unsigned long color;
  float lineWidth;
  PyObject *verticesObj;
  if(!XPLMLinesWithWidth_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMLinesWithWidth is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "kfO", keywords, &color, &lineWidth, &verticesObj)){
    return nullptr;
  }
  Py_ssize_t count;
  XPLMVertex_t *v = parseVertices(verticesObj, &count);
  if(!v){
    return nullptr;
  }
  XPLMLinesWithWidth_ptr((uint32_t)color, lineWidth, v, (int)count);
  free(v);
  Py_RETURN_NONE;
}

My_DOCSTR(_lineStrip__doc__, "lineStrip",
          "color, vertices",
          "color:int, vertices:Sequence[tuple[float, float]]",
          "None",
          "Draw a connected line strip; the last vertex is not closed back.");
static PyObject *XPLMLineStripFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("color"), CHAR("vertices"), nullptr};
  (void) self;
  unsigned long color;
  PyObject *verticesObj;
  if(!XPLMLineStrip_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMLineStrip is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "kO", keywords, &color, &verticesObj)){
    return nullptr;
  }
  Py_ssize_t count;
  XPLMVertex_t *v = parseVertices(verticesObj, &count);
  if(!v){
    return nullptr;
  }
  XPLMLineStrip_ptr((uint32_t)color, v, (int)count);
  free(v);
  Py_RETURN_NONE;
}

My_DOCSTR(_lineStripWithWidth__doc__, "lineStripWithWidth",
          "color, lineWidth, vertices",
          "color:int, lineWidth:float, vertices:Sequence[tuple[float, float]]",
          "None",
          "Draw a connected line strip with the given line width.");
static PyObject *XPLMLineStripWithWidthFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("color"), CHAR("lineWidth"), CHAR("vertices"), nullptr};
  (void) self;
  unsigned long color;
  float lineWidth;
  PyObject *verticesObj;
  if(!XPLMLineStripWithWidth_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMLineStripWithWidth is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "kfO", keywords, &color, &lineWidth, &verticesObj)){
    return nullptr;
  }
  Py_ssize_t count;
  XPLMVertex_t *v = parseVertices(verticesObj, &count);
  if(!v){
    return nullptr;
  }
  XPLMLineStripWithWidth_ptr((uint32_t)color, lineWidth, v, (int)count);
  free(v);
  Py_RETURN_NONE;
}

My_DOCSTR(_lineLoop__doc__, "lineLoop",
          "color, vertices",
          "color:int, vertices:Sequence[tuple[float, float]]",
          "None",
          "Draw a closed line loop; the last vertex connects back to the first.");
static PyObject *XPLMLineLoopFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("color"), CHAR("vertices"), nullptr};
  (void) self;
  unsigned long color;
  PyObject *verticesObj;
  if(!XPLMLineLoop_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMLineLoop is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "kO", keywords, &color, &verticesObj)){
    return nullptr;
  }
  Py_ssize_t count;
  XPLMVertex_t *v = parseVertices(verticesObj, &count);
  if(!v){
    return nullptr;
  }
  XPLMLineLoop_ptr((uint32_t)color, v, (int)count);
  free(v);
  Py_RETURN_NONE;
}

My_DOCSTR(_lineLoopWithWidth__doc__, "lineLoopWithWidth",
          "color, lineWidth, vertices",
          "color:int, lineWidth:float, vertices:Sequence[tuple[float, float]]",
          "None",
          "Draw a closed line loop with the given line width.");
static PyObject *XPLMLineLoopWithWidthFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("color"), CHAR("lineWidth"), CHAR("vertices"), nullptr};
  (void) self;
  unsigned long color;
  float lineWidth;
  PyObject *verticesObj;
  if(!XPLMLineLoopWithWidth_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMLineLoopWithWidth is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "kfO", keywords, &color, &lineWidth, &verticesObj)){
    return nullptr;
  }
  Py_ssize_t count;
  XPLMVertex_t *v = parseVertices(verticesObj, &count);
  if(!v){
    return nullptr;
  }
  XPLMLineLoopWithWidth_ptr((uint32_t)color, lineWidth, v, (int)count);
  free(v);
  Py_RETURN_NONE;
}

My_DOCSTR(_polygon__doc__, "polygon",
          "color, vertices",
          "color:int, vertices:Sequence[tuple[float, float]]",
          "None",
          "Draw a filled convex polygon (at least 3 vertices).");
static PyObject *XPLMPolygonFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("color"), CHAR("vertices"), nullptr};
  (void) self;
  unsigned long color;
  PyObject *verticesObj;
  if(!XPLMPolygon_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMPolygon is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "kO", keywords, &color, &verticesObj)){
    return nullptr;
  }
  Py_ssize_t count;
  XPLMVertex_t *v = parseVertices(verticesObj, &count);
  if(!v){
    return nullptr;
  }
  XPLMPolygon_ptr((uint32_t)color, v, (int)count);
  free(v);
  Py_RETURN_NONE;
}

My_DOCSTR(_polygonWithWidth__doc__, "polygonWithWidth",
          "color, lineWidth, vertices",
          "color:int, lineWidth:float, vertices:Sequence[tuple[float, float]]",
          "None",
          "Draw a filled convex polygon with the given outline width.");
static PyObject *XPLMPolygonWithWidthFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("color"), CHAR("lineWidth"), CHAR("vertices"), nullptr};
  (void) self;
  unsigned long color;
  float lineWidth;
  PyObject *verticesObj;
  if(!XPLMPolygonWithWidth_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMPolygonWithWidth is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "kfO", keywords, &color, &lineWidth, &verticesObj)){
    return nullptr;
  }
  Py_ssize_t count;
  XPLMVertex_t *v = parseVertices(verticesObj, &count);
  if(!v){
    return nullptr;
  }
  XPLMPolygonWithWidth_ptr((uint32_t)color, lineWidth, v, (int)count);
  free(v);
  Py_RETURN_NONE;
}

My_DOCSTR(_quadstrip__doc__, "quadstrip",
          "color, vertices",
          "color:int, vertices:Sequence[tuple[float, float]]",
          "None",
          "Draw a series of connected filled quadrilaterals (count even, >= 4).");
static PyObject *XPLMQuadstripFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("color"), CHAR("vertices"), nullptr};
  (void) self;
  unsigned long color;
  PyObject *verticesObj;
  if(!XPLMQuadstrip_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMQuadstrip is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "kO", keywords, &color, &verticesObj)){
    return nullptr;
  }
  Py_ssize_t count;
  XPLMVertex_t *v = parseVertices(verticesObj, &count);
  if(!v){
    return nullptr;
  }
  XPLMQuadstrip_ptr((uint32_t)color, v, (int)count);
  free(v);
  Py_RETURN_NONE;
}

My_DOCSTR(_quadstripWithWidth__doc__, "quadstripWithWidth",
          "color, lineWidth, vertices",
          "color:int, lineWidth:float, vertices:Sequence[tuple[float, float]]",
          "None",
          "Draw a quad strip with the given outline width.");
static PyObject *XPLMQuadstripWithWidthFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("color"), CHAR("lineWidth"), CHAR("vertices"), nullptr};
  (void) self;
  unsigned long color;
  float lineWidth;
  PyObject *verticesObj;
  if(!XPLMQuadstripWithWidth_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMQuadstripWithWidth is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "kfO", keywords, &color, &lineWidth, &verticesObj)){
    return nullptr;
  }
  Py_ssize_t count;
  XPLMVertex_t *v = parseVertices(verticesObj, &count);
  if(!v){
    return nullptr;
  }
  XPLMQuadstripWithWidth_ptr((uint32_t)color, lineWidth, v, (int)count);
  free(v);
  Py_RETURN_NONE;
}

/* ---- Stipple (dashed) variants taking (color, vertices, dashLength, lineWidth) ---- */

My_DOCSTR(_linesStipple__doc__, "linesStipple",
          "color, vertices, dashLength, lineWidth",
          "color:int, vertices:Sequence[tuple[float, float]], dashLength:float, lineWidth:float",
          "None",
          "Draw disconnected dashed line segments.");
static PyObject *XPLMLinesStippleFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("color"), CHAR("vertices"), CHAR("dashLength"), CHAR("lineWidth"), nullptr};
  (void) self;
  unsigned long color;
  PyObject *verticesObj;
  float dashLength, lineWidth;
  if(!XPLMLinesStipple_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMLinesStipple is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "kOff", keywords, &color, &verticesObj, &dashLength, &lineWidth)){
    return nullptr;
  }
  Py_ssize_t count;
  XPLMVertex_t *v = parseVertices(verticesObj, &count);
  if(!v){
    return nullptr;
  }
  XPLMLinesStipple_ptr((uint32_t)color, v, (int)count, dashLength, lineWidth);
  free(v);
  Py_RETURN_NONE;
}

My_DOCSTR(_lineStripStipple__doc__, "lineStripStipple",
          "color, vertices, dashLength, lineWidth",
          "color:int, vertices:Sequence[tuple[float, float]], dashLength:float, lineWidth:float",
          "None",
          "Draw a connected dashed line strip.");
static PyObject *XPLMLineStripStippleFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("color"), CHAR("vertices"), CHAR("dashLength"), CHAR("lineWidth"), nullptr};
  (void) self;
  unsigned long color;
  PyObject *verticesObj;
  float dashLength, lineWidth;
  if(!XPLMLineStripStipple_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMLineStripStipple is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "kOff", keywords, &color, &verticesObj, &dashLength, &lineWidth)){
    return nullptr;
  }
  Py_ssize_t count;
  XPLMVertex_t *v = parseVertices(verticesObj, &count);
  if(!v){
    return nullptr;
  }
  XPLMLineStripStipple_ptr((uint32_t)color, v, (int)count, dashLength, lineWidth);
  free(v);
  Py_RETURN_NONE;
}

My_DOCSTR(_lineLoopStipple__doc__, "lineLoopStipple",
          "color, vertices, dashLength, lineWidth",
          "color:int, vertices:Sequence[tuple[float, float]], dashLength:float, lineWidth:float",
          "None",
          "Draw a closed dashed line loop.");
static PyObject *XPLMLineLoopStippleFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("color"), CHAR("vertices"), CHAR("dashLength"), CHAR("lineWidth"), nullptr};
  (void) self;
  unsigned long color;
  PyObject *verticesObj;
  float dashLength, lineWidth;
  if(!XPLMLineLoopStipple_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMLineLoopStipple is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "kOff", keywords, &color, &verticesObj, &dashLength, &lineWidth)){
    return nullptr;
  }
  Py_ssize_t count;
  XPLMVertex_t *v = parseVertices(verticesObj, &count);
  if(!v){
    return nullptr;
  }
  XPLMLineLoopStipple_ptr((uint32_t)color, v, (int)count, dashLength, lineWidth);
  free(v);
  Py_RETURN_NONE;
}

/* ---- Per-vertex-color variants taking (vertices) ---- */

My_DOCSTR(_linesc__doc__, "linesc",
          "vertices",
          "vertices:Sequence[tuple[float, float, int]]",
          "None",
          "Draw disconnected line segments with per-vertex colors.");
static PyObject *XPLMLinescFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("vertices"), nullptr};
  (void) self;
  PyObject *verticesObj;
  if(!XPLMLinesc_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMLinesc is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &verticesObj)){
    return nullptr;
  }
  Py_ssize_t count;
  XPLMVertexColor_t *v = parseVertexColors(verticesObj, &count);
  if(!v){
    return nullptr;
  }
  XPLMLinesc_ptr(v, (int)count);
  free(v);
  Py_RETURN_NONE;
}

My_DOCSTR(_lineStripc__doc__, "lineStripc",
          "vertices",
          "vertices:Sequence[tuple[float, float, int]]",
          "None",
          "Draw a connected line strip with per-vertex colors.");
static PyObject *XPLMLineStripcFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("vertices"), nullptr};
  (void) self;
  PyObject *verticesObj;
  if(!XPLMLineStripc_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMLineStripc is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &verticesObj)){
    return nullptr;
  }
  Py_ssize_t count;
  XPLMVertexColor_t *v = parseVertexColors(verticesObj, &count);
  if(!v){
    return nullptr;
  }
  XPLMLineStripc_ptr(v, (int)count);
  free(v);
  Py_RETURN_NONE;
}

My_DOCSTR(_lineLoopc__doc__, "lineLoopc",
          "vertices",
          "vertices:Sequence[tuple[float, float, int]]",
          "None",
          "Draw a closed line loop with per-vertex colors.");
static PyObject *XPLMLineLoopcFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("vertices"), nullptr};
  (void) self;
  PyObject *verticesObj;
  if(!XPLMLineLoopc_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMLineLoopc is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &verticesObj)){
    return nullptr;
  }
  Py_ssize_t count;
  XPLMVertexColor_t *v = parseVertexColors(verticesObj, &count);
  if(!v){
    return nullptr;
  }
  XPLMLineLoopc_ptr(v, (int)count);
  free(v);
  Py_RETURN_NONE;
}

My_DOCSTR(_polygonc__doc__, "polygonc",
          "vertices",
          "vertices:Sequence[tuple[float, float, int]]",
          "None",
          "Draw a filled convex polygon with per-vertex colors.");
static PyObject *XPLMPolygoncFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("vertices"), nullptr};
  (void) self;
  PyObject *verticesObj;
  if(!XPLMPolygonc_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMPolygonc is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &verticesObj)){
    return nullptr;
  }
  Py_ssize_t count;
  XPLMVertexColor_t *v = parseVertexColors(verticesObj, &count);
  if(!v){
    return nullptr;
  }
  XPLMPolygonc_ptr(v, (int)count);
  free(v);
  Py_RETURN_NONE;
}

My_DOCSTR(_quadstripc__doc__, "quadstripc",
          "vertices",
          "vertices:Sequence[tuple[float, float, int]]",
          "None",
          "Draw a quad strip with per-vertex colors.");
static PyObject *XPLMQuadstripcFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("vertices"), nullptr};
  (void) self;
  PyObject *verticesObj;
  if(!XPLMQuadstripc_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMQuadstripc is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &verticesObj)){
    return nullptr;
  }
  Py_ssize_t count;
  XPLMVertexColor_t *v = parseVertexColors(verticesObj, &count);
  if(!v){
    return nullptr;
  }
  XPLMQuadstripc_ptr(v, (int)count);
  free(v);
  Py_RETURN_NONE;
}

/* ---- Per-vertex-color width variants taking (lineWidth, vertices) ---- */

My_DOCSTR(_linescWithWidth__doc__, "linescWithWidth",
          "lineWidth, vertices",
          "lineWidth:float, vertices:Sequence[tuple[float, float, int]]",
          "None",
          "Draw disconnected line segments with per-vertex colors and line width.");
static PyObject *XPLMLinescWithWidthFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("lineWidth"), CHAR("vertices"), nullptr};
  (void) self;
  float lineWidth;
  PyObject *verticesObj;
  if(!XPLMLinescWithWidth_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMLinescWithWidth is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "fO", keywords, &lineWidth, &verticesObj)){
    return nullptr;
  }
  Py_ssize_t count;
  XPLMVertexColor_t *v = parseVertexColors(verticesObj, &count);
  if(!v){
    return nullptr;
  }
  XPLMLinescWithWidth_ptr(lineWidth, v, (int)count);
  free(v);
  Py_RETURN_NONE;
}

My_DOCSTR(_lineStripcWithWidth__doc__, "lineStripcWithWidth",
          "lineWidth, vertices",
          "lineWidth:float, vertices:Sequence[tuple[float, float, int]]",
          "None",
          "Draw a connected line strip with per-vertex colors and line width.");
static PyObject *XPLMLineStripcWithWidthFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("lineWidth"), CHAR("vertices"), nullptr};
  (void) self;
  float lineWidth;
  PyObject *verticesObj;
  if(!XPLMLineStripcWithWidth_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMLineStripcWithWidth is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "fO", keywords, &lineWidth, &verticesObj)){
    return nullptr;
  }
  Py_ssize_t count;
  XPLMVertexColor_t *v = parseVertexColors(verticesObj, &count);
  if(!v){
    return nullptr;
  }
  XPLMLineStripcWithWidth_ptr(lineWidth, v, (int)count);
  free(v);
  Py_RETURN_NONE;
}

My_DOCSTR(_lineLoopcWithWidth__doc__, "lineLoopcWithWidth",
          "lineWidth, vertices",
          "lineWidth:float, vertices:Sequence[tuple[float, float, int]]",
          "None",
          "Draw a closed line loop with per-vertex colors and line width.");
static PyObject *XPLMLineLoopcWithWidthFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("lineWidth"), CHAR("vertices"), nullptr};
  (void) self;
  float lineWidth;
  PyObject *verticesObj;
  if(!XPLMLineLoopcWithWidth_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMLineLoopcWithWidth is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "fO", keywords, &lineWidth, &verticesObj)){
    return nullptr;
  }
  Py_ssize_t count;
  XPLMVertexColor_t *v = parseVertexColors(verticesObj, &count);
  if(!v){
    return nullptr;
  }
  XPLMLineLoopcWithWidth_ptr(lineWidth, v, (int)count);
  free(v);
  Py_RETURN_NONE;
}

My_DOCSTR(_polygoncWithWidth__doc__, "polygoncWithWidth",
          "lineWidth, vertices",
          "lineWidth:float, vertices:Sequence[tuple[float, float, int]]",
          "None",
          "Draw a filled convex polygon with per-vertex colors and outline width.");
static PyObject *XPLMPolygoncWithWidthFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("lineWidth"), CHAR("vertices"), nullptr};
  (void) self;
  float lineWidth;
  PyObject *verticesObj;
  if(!XPLMPolygoncWithWidth_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMPolygoncWithWidth is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "fO", keywords, &lineWidth, &verticesObj)){
    return nullptr;
  }
  Py_ssize_t count;
  XPLMVertexColor_t *v = parseVertexColors(verticesObj, &count);
  if(!v){
    return nullptr;
  }
  XPLMPolygoncWithWidth_ptr(lineWidth, v, (int)count);
  free(v);
  Py_RETURN_NONE;
}

My_DOCSTR(_quadstripcWithWidth__doc__, "quadstripcWithWidth",
          "lineWidth, vertices",
          "lineWidth:float, vertices:Sequence[tuple[float, float, int]]",
          "None",
          "Draw a quad strip with per-vertex colors and outline width.");
static PyObject *XPLMQuadstripcWithWidthFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("lineWidth"), CHAR("vertices"), nullptr};
  (void) self;
  float lineWidth;
  PyObject *verticesObj;
  if(!XPLMQuadstripcWithWidth_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMQuadstripcWithWidth is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "fO", keywords, &lineWidth, &verticesObj)){
    return nullptr;
  }
  Py_ssize_t count;
  XPLMVertexColor_t *v = parseVertexColors(verticesObj, &count);
  if(!v){
    return nullptr;
  }
  XPLMQuadstripcWithWidth_ptr(lineWidth, v, (int)count);
  free(v);
  Py_RETURN_NONE;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
PyMethodDef panelGraphicsPrimitivesMethods[] = {
  {"makeColor", (PyCFunction)XPLMMakeColorFun, METH_VARARGS | METH_KEYWORDS, _makeColor__doc__},
  {"XPLMMakeColor", (PyCFunction)XPLMMakeColorFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"lines", (PyCFunction)XPLMLinesFun, METH_VARARGS | METH_KEYWORDS, _lines__doc__},
  {"XPLMLines", (PyCFunction)XPLMLinesFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"linesWithWidth", (PyCFunction)XPLMLinesWithWidthFun, METH_VARARGS | METH_KEYWORDS, _linesWithWidth__doc__},
  {"XPLMLinesWithWidth", (PyCFunction)XPLMLinesWithWidthFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"linesStipple", (PyCFunction)XPLMLinesStippleFun, METH_VARARGS | METH_KEYWORDS, _linesStipple__doc__},
  {"XPLMLinesStipple", (PyCFunction)XPLMLinesStippleFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"linesc", (PyCFunction)XPLMLinescFun, METH_VARARGS | METH_KEYWORDS, _linesc__doc__},
  {"XPLMLinesc", (PyCFunction)XPLMLinescFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"linescWithWidth", (PyCFunction)XPLMLinescWithWidthFun, METH_VARARGS | METH_KEYWORDS, _linescWithWidth__doc__},
  {"XPLMLinescWithWidth", (PyCFunction)XPLMLinescWithWidthFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"lineStrip", (PyCFunction)XPLMLineStripFun, METH_VARARGS | METH_KEYWORDS, _lineStrip__doc__},
  {"XPLMLineStrip", (PyCFunction)XPLMLineStripFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"lineStripWithWidth", (PyCFunction)XPLMLineStripWithWidthFun, METH_VARARGS | METH_KEYWORDS, _lineStripWithWidth__doc__},
  {"XPLMLineStripWithWidth", (PyCFunction)XPLMLineStripWithWidthFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"lineStripc", (PyCFunction)XPLMLineStripcFun, METH_VARARGS | METH_KEYWORDS, _lineStripc__doc__},
  {"XPLMLineStripc", (PyCFunction)XPLMLineStripcFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"lineStripcWithWidth", (PyCFunction)XPLMLineStripcWithWidthFun, METH_VARARGS | METH_KEYWORDS, _lineStripcWithWidth__doc__},
  {"XPLMLineStripcWithWidth", (PyCFunction)XPLMLineStripcWithWidthFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"lineStripStipple", (PyCFunction)XPLMLineStripStippleFun, METH_VARARGS | METH_KEYWORDS, _lineStripStipple__doc__},
  {"XPLMLineStripStipple", (PyCFunction)XPLMLineStripStippleFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"lineLoop", (PyCFunction)XPLMLineLoopFun, METH_VARARGS | METH_KEYWORDS, _lineLoop__doc__},
  {"XPLMLineLoop", (PyCFunction)XPLMLineLoopFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"lineLoopWithWidth", (PyCFunction)XPLMLineLoopWithWidthFun, METH_VARARGS | METH_KEYWORDS, _lineLoopWithWidth__doc__},
  {"XPLMLineLoopWithWidth", (PyCFunction)XPLMLineLoopWithWidthFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"lineLoopc", (PyCFunction)XPLMLineLoopcFun, METH_VARARGS | METH_KEYWORDS, _lineLoopc__doc__},
  {"XPLMLineLoopc", (PyCFunction)XPLMLineLoopcFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"lineLoopcWithWidth", (PyCFunction)XPLMLineLoopcWithWidthFun, METH_VARARGS | METH_KEYWORDS, _lineLoopcWithWidth__doc__},
  {"XPLMLineLoopcWithWidth", (PyCFunction)XPLMLineLoopcWithWidthFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"lineLoopStipple", (PyCFunction)XPLMLineLoopStippleFun, METH_VARARGS | METH_KEYWORDS, _lineLoopStipple__doc__},
  {"XPLMLineLoopStipple", (PyCFunction)XPLMLineLoopStippleFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"polygon", (PyCFunction)XPLMPolygonFun, METH_VARARGS | METH_KEYWORDS, _polygon__doc__},
  {"XPLMPolygon", (PyCFunction)XPLMPolygonFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"polygonWithWidth", (PyCFunction)XPLMPolygonWithWidthFun, METH_VARARGS | METH_KEYWORDS, _polygonWithWidth__doc__},
  {"XPLMPolygonWithWidth", (PyCFunction)XPLMPolygonWithWidthFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"polygonc", (PyCFunction)XPLMPolygoncFun, METH_VARARGS | METH_KEYWORDS, _polygonc__doc__},
  {"XPLMPolygonc", (PyCFunction)XPLMPolygoncFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"polygoncWithWidth", (PyCFunction)XPLMPolygoncWithWidthFun, METH_VARARGS | METH_KEYWORDS, _polygoncWithWidth__doc__},
  {"XPLMPolygoncWithWidth", (PyCFunction)XPLMPolygoncWithWidthFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"quadstrip", (PyCFunction)XPLMQuadstripFun, METH_VARARGS | METH_KEYWORDS, _quadstrip__doc__},
  {"XPLMQuadstrip", (PyCFunction)XPLMQuadstripFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"quadstripWithWidth", (PyCFunction)XPLMQuadstripWithWidthFun, METH_VARARGS | METH_KEYWORDS, _quadstripWithWidth__doc__},
  {"XPLMQuadstripWithWidth", (PyCFunction)XPLMQuadstripWithWidthFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"quadstripc", (PyCFunction)XPLMQuadstripcFun, METH_VARARGS | METH_KEYWORDS, _quadstripc__doc__},
  {"XPLMQuadstripc", (PyCFunction)XPLMQuadstripcFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"quadstripcWithWidth", (PyCFunction)XPLMQuadstripcWithWidthFun, METH_VARARGS | METH_KEYWORDS, _quadstripcWithWidth__doc__},
  {"XPLMQuadstripcWithWidth", (PyCFunction)XPLMQuadstripcWithWidthFun, METH_VARARGS | METH_KEYWORDS, ""},
  {nullptr, nullptr, 0, nullptr}
};
#pragma GCC diagnostic pop
