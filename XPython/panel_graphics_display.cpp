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

/* ---- Synthetic Vision (SVT) display -------------------------------------- */

My_DOCSTR(_createSVTDisplay__doc__, "createSVTDisplay",
          "features, pilotIndex=0",
          "features:int, pilotIndex:int",
          "XPLMSVTDisplayRef",
          "Create a Synthetic Vision (SVT) display that renders a 3-D perspective view\n"
          "of terrain, runways, and optional overlays into an avionics panel. features\n"
          "is a bitwise OR of SVT_* flags (e.g. SVT_Terrain | SVT_Runways, or SVT_All);\n"
          "pilotIndex is 0 for pilot-side AHRS, 1 for copilot. Draw it with\n"
          "svtDisplayDrawIn() and free it with destroySVTDisplay().");
static PyObject *XPLMCreateSVTDisplayFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("features"), CHAR("pilotIndex"), nullptr};
  (void) self;
  int features;
  int pilotIndex = 0;
  if(!XPLMCreateSVTDisplay_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMCreateSVTDisplay is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "i|i", keywords, &features, &pilotIndex)){
    return nullptr;
  }
  XPLMCreateSVT_t params;
  params.structSize = sizeof(XPLMCreateSVT_t);
  params.features = features;
  params.pilotIndex = pilotIndex;
  XPLMSVTDisplayRef svt = XPLMCreateSVTDisplay_ptr(&params);
  if(!svt){
    PyErr_SetString(PyExc_RuntimeError , "XPLMCreateSVTDisplay failed.");
    return nullptr;
  }
  return makeCapsule(svt, SVT_CAPSULE);
}

My_DOCSTR(_destroySVTDisplay__doc__, "destroySVTDisplay",
          "svt",
          "svt:XPLMSVTDisplayRef",
          "None",
          "Destroy an SVT display created with createSVTDisplay() and free its\n"
          "resources. The handle must not be used after this call.");
static PyObject *XPLMDestroySVTDisplayFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("svt"), nullptr};
  (void) self;
  PyObject *svtCapsule;
  if(!XPLMDestroySVTDisplay_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMDestroySVTDisplay is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &svtCapsule)){
    return nullptr;
  }
  XPLMSVTDisplayRef svt = getVoidPtr(svtCapsule, SVT_CAPSULE);
  if(!svt && PyErr_Occurred()){
    return nullptr;
  }
  XPLMDestroySVTDisplay_ptr(svt);
  deleteCapsule(svtCapsule);
  Py_RETURN_NONE;
}

My_DOCSTR(_svtDisplayDrawIn__doc__, "svtDisplayDrawIn",
          "svt, features, left, top, right, bottom, dataOverrides=None",
          "svt:XPLMSVTDisplayRef, features:int, left:int, top:int, right:int, bottom:int, "
          "dataOverrides:Optional[Sequence[float]]",
          "None",
          "Render the SVT display into the panel within (left, top, right, bottom).\n"
          "Must be called from an avionics drawing callback; does nothing until terrain\n"
          "tiles finish loading. features is a bitwise OR of SVT_* flags for this draw.\n"
          "dataOverrides is None for live sim state, or a sequence of 9 values in order:\n"
          "(pitchDeg, rollDeg, headingMagDeg, magVarDeg, indicatedAltFt, baroSettingInHg,\n"
          "hsiSource, hdefDots, vdefDots).");
static PyObject *XPLMSVTDisplayDrawInFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("svt"), CHAR("features"), CHAR("left"), CHAR("top"),
                             CHAR("right"), CHAR("bottom"), CHAR("dataOverrides"), nullptr};
  (void) self;
  PyObject *svtCapsule;
  int features, left, top, right, bottom;
  PyObject *dataOverrides = Py_None;
  if(!XPLMSVTDisplayDrawIn_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMSVTDisplayDrawIn is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Oiiiii|O", keywords, &svtCapsule,
                                  &features, &left, &top, &right, &bottom, &dataOverrides)){
    return nullptr;
  }
  XPLMSVTDisplayRef svt = getVoidPtr(svtCapsule, SVT_CAPSULE);
  if(!svt && PyErr_Occurred()){
    return nullptr;
  }
  XPLMSVTCustomData_t data;
  XPLMSVTCustomData_t *dataPtr = nullptr;
  if(dataOverrides && dataOverrides != Py_None){
    PyObject *seq = PySequence_Tuple(dataOverrides);
    if(!seq){
      PyErr_SetString(PyExc_TypeError, "svtDisplayDrawIn: dataOverrides must be None or a sequence of 9 values.");
      return nullptr;
    }
    if(PyTuple_Size(seq) != 9){
      Py_DECREF(seq);
      PyErr_SetString(PyExc_ValueError, "svtDisplayDrawIn: dataOverrides must contain exactly 9 values.");
      return nullptr;
    }
    data.pitchDeg        = (float)PyFloat_AsDouble(PyTuple_GetItem(seq, 0));
    data.rollDeg         = (float)PyFloat_AsDouble(PyTuple_GetItem(seq, 1));
    data.headingMagDeg   = (float)PyFloat_AsDouble(PyTuple_GetItem(seq, 2));
    data.magVarDeg       = (float)PyFloat_AsDouble(PyTuple_GetItem(seq, 3));
    data.indicatedAltFt  = (float)PyFloat_AsDouble(PyTuple_GetItem(seq, 4));
    data.baroSettingInHg = (float)PyFloat_AsDouble(PyTuple_GetItem(seq, 5));
    data.hsiSource       = (int)PyLong_AsLong(PyTuple_GetItem(seq, 6));
    data.hdefDots        = (float)PyFloat_AsDouble(PyTuple_GetItem(seq, 7));
    data.vdefDots        = (float)PyFloat_AsDouble(PyTuple_GetItem(seq, 8));
    Py_DECREF(seq);
    if(PyErr_Occurred()){
      return nullptr;
    }
    dataPtr = &data;
  }
  XPLMSVTDisplayDrawIn_ptr(svt, features, left, top, right, bottom, dataPtr);
  Py_RETURN_NONE;
}

/* ---- base map display ---------------------------------------------------- */

My_DOCSTR(_createMapDisplay__doc__, "createMapDisplay",
          "pilotIndex=0",
          "pilotIndex:int",
          "XPLMMapDisplayRef",
          "Create a base map display (ND/MFD background) that renders terrain,\n"
          "topography, water, weather and taxi layouts with a stereographic\n"
          "projection centered on the aircraft. pilotIndex is 0 for pilot-side GPS\n"
          "position, 1 for copilot. Draw it with mapDisplayDrawIn() and free it with\n"
          "destroyMapDisplay().");
static PyObject *XPLMCreateMapDisplayFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("pilotIndex"), nullptr};
  (void) self;
  int pilotIndex = 0;
  if(!XPLMCreateMapDisplay_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMCreateMapDisplay is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|i", keywords, &pilotIndex)){
    return nullptr;
  }
  XPLMCreateMap_t params;
  params.structSize = sizeof(XPLMCreateMap_t);
  params.pilotIndex = pilotIndex;
  XPLMMapDisplayRef map = XPLMCreateMapDisplay_ptr(&params);
  if(!map){
    PyErr_SetString(PyExc_RuntimeError , "XPLMCreateMapDisplay failed.");
    return nullptr;
  }
  return makeCapsule(map, MAP_CAPSULE);
}

My_DOCSTR(_destroyMapDisplay__doc__, "destroyMapDisplay",
          "map",
          "map:XPLMMapDisplayRef",
          "None",
          "Destroy a map display created with createMapDisplay() and free its\n"
          "resources. The handle must not be used after this call.");
static PyObject *XPLMDestroyMapDisplayFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("map"), nullptr};
  (void) self;
  PyObject *mapCapsule;
  if(!XPLMDestroyMapDisplay_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMDestroyMapDisplay is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &mapCapsule)){
    return nullptr;
  }
  XPLMMapDisplayRef map = getVoidPtr(mapCapsule, MAP_CAPSULE);
  if(!map && PyErr_Occurred()){
    return nullptr;
  }
  XPLMDestroyMapDisplay_ptr(map);
  deleteCapsule(mapCapsule);
  Py_RETURN_NONE;
}

My_DOCSTR(_mapDisplayDrawIn__doc__, "mapDisplayDrawIn",
          "map, layers, left, top, right, bottom, dataOverrides=None",
          "map:XPLMMapDisplayRef, layers:int, left:int, top:int, right:int, bottom:int, "
          "dataOverrides:Optional[Sequence[float]]",
          "None",
          "Render the base map into the panel within (left, top, right, bottom). Must\n"
          "be called from an avionics drawing callback; does nothing until terrain\n"
          "tiles finish loading. layers is a bitwise OR of Map_* flags (some are\n"
          "mutually exclusive -- e.g. Map_Nexrad with Map_EGPWS or Map_IR).\n"
          "dataOverrides is None for live sim state, or a sequence of 12 values in\n"
          "order: (datLat, datLon, ctrX, ctrY, roseDiameter, mapRange, orientation,\n"
          "terrainWarn, terrainCaution, acfAlt, gearDown, trueRotation).");
static PyObject *XPLMMapDisplayDrawInFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("map"), CHAR("layers"), CHAR("left"), CHAR("top"),
                             CHAR("right"), CHAR("bottom"), CHAR("dataOverrides"), nullptr};
  (void) self;
  PyObject *mapCapsule;
  int layers, left, top, right, bottom;
  PyObject *dataOverrides = Py_None;
  if(!XPLMMapDisplayDrawIn_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMMapDisplayDrawIn is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Oiiiii|O", keywords, &mapCapsule,
                                  &layers, &left, &top, &right, &bottom, &dataOverrides)){
    return nullptr;
  }
  XPLMMapDisplayRef map = getVoidPtr(mapCapsule, MAP_CAPSULE);
  if(!map && PyErr_Occurred()){
    return nullptr;
  }
  XPLMMapCustomData_t data;
  XPLMMapCustomData_t *dataPtr = nullptr;
  if(dataOverrides && dataOverrides != Py_None){
    PyObject *seq = PySequence_Tuple(dataOverrides);
    if(!seq){
      PyErr_SetString(PyExc_TypeError, "mapDisplayDrawIn: dataOverrides must be None or a sequence of 12 values.");
      return nullptr;
    }
    if(PyTuple_Size(seq) != 12){
      Py_DECREF(seq);
      PyErr_SetString(PyExc_ValueError, "mapDisplayDrawIn: dataOverrides must contain exactly 12 values.");
      return nullptr;
    }
    data.datLat         = (float)PyFloat_AsDouble(PyTuple_GetItem(seq, 0));
    data.datLon         = (float)PyFloat_AsDouble(PyTuple_GetItem(seq, 1));
    data.ctrX           = (int)PyLong_AsLong(PyTuple_GetItem(seq, 2));
    data.ctrY           = (int)PyLong_AsLong(PyTuple_GetItem(seq, 3));
    data.roseDiameter   = (int)PyLong_AsLong(PyTuple_GetItem(seq, 4));
    data.mapRange       = (float)PyFloat_AsDouble(PyTuple_GetItem(seq, 5));
    data.orientation    = (int)PyLong_AsLong(PyTuple_GetItem(seq, 6));
    data.terrainWarn    = (float)PyFloat_AsDouble(PyTuple_GetItem(seq, 7));
    data.terrainCaution = (float)PyFloat_AsDouble(PyTuple_GetItem(seq, 8));
    data.acfAlt         = (float)PyFloat_AsDouble(PyTuple_GetItem(seq, 9));
    data.gearDown       = (int)PyLong_AsLong(PyTuple_GetItem(seq, 10));
    data.trueRotation   = (float)PyFloat_AsDouble(PyTuple_GetItem(seq, 11));
    Py_DECREF(seq);
    if(PyErr_Occurred()){
      return nullptr;
    }
    dataPtr = &data;
  }
  XPLMMapDisplayDrawIn_ptr(map, layers, left, top, right, bottom, dataPtr);
  Py_RETURN_NONE;
}


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
PyMethodDef panelGraphicsDisplayMethods[] = {
  {"createSVTDisplay", (PyCFunction)XPLMCreateSVTDisplayFun, METH_VARARGS | METH_KEYWORDS, _createSVTDisplay__doc__},
  {"XPLMCreateSVTDisplay", (PyCFunction)XPLMCreateSVTDisplayFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"destroySVTDisplay", (PyCFunction)XPLMDestroySVTDisplayFun, METH_VARARGS | METH_KEYWORDS, _destroySVTDisplay__doc__},
  {"XPLMDestroySVTDisplay", (PyCFunction)XPLMDestroySVTDisplayFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"svtDisplayDrawIn", (PyCFunction)XPLMSVTDisplayDrawInFun, METH_VARARGS | METH_KEYWORDS, _svtDisplayDrawIn__doc__},
  {"XPLMSVTDisplayDrawIn", (PyCFunction)XPLMSVTDisplayDrawInFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"createMapDisplay", (PyCFunction)XPLMCreateMapDisplayFun, METH_VARARGS | METH_KEYWORDS, _createMapDisplay__doc__},
  {"XPLMCreateMapDisplay", (PyCFunction)XPLMCreateMapDisplayFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"destroyMapDisplay", (PyCFunction)XPLMDestroyMapDisplayFun, METH_VARARGS | METH_KEYWORDS, _destroyMapDisplay__doc__},
  {"XPLMDestroyMapDisplay", (PyCFunction)XPLMDestroyMapDisplayFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"mapDisplayDrawIn", (PyCFunction)XPLMMapDisplayDrawInFun, METH_VARARGS | METH_KEYWORDS, _mapDisplayDrawIn__doc__},
  {"XPLMMapDisplayDrawIn", (PyCFunction)XPLMMapDisplayDrawInFun, METH_VARARGS | METH_KEYWORDS, ""},
  {nullptr, nullptr, 0, nullptr}
};
#pragma GCC diagnostic pop
