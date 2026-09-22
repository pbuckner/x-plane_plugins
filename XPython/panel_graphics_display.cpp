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

/* Read an integer-valued element out of a dataOverrides sequence, accepting
   either an int or a float.

   Every other element of these sequences is a float, and callers typically
   build them straight from datarefs -- which hand back floats -- so demanding
   an exact int for the one or two integer fields makes the whole sequence
   awkward to construct and fails with a confusing "'float' object cannot be
   interpreted as an integer". Floats are truncated toward zero, matching the C
   cast the value would have received anyway.

   Returns 0 with a Python exception set on failure; callers already test
   PyErr_Occurred() after filling the struct, so that is picked up there. */
static int seqInt(PyObject *seq, Py_ssize_t idx)
{
  PyObject *item = PyTuple_GetItem(seq, idx);   /* borrowed ref, NULL if out of range */
  if(!item){
    return 0;
  }
  if(PyFloat_Check(item)){
    return (int)PyFloat_AsDouble(item);
  }
  return (int)PyLong_AsLong(item);
}

My_DOCSTR(_createSVTDisplay__doc__, "createSVTDisplay",
          "pilotIndex=0, pixelsPerDegree=14",
          "pilotIndex:int, pixelsPerDegree:float",
          "XPLMSVTDisplayRef",
          "Create a Synthetic Vision (SVT) display that renders a 3-D perspective view\n"
          "of terrain, runways, and optional overlays into an avionics panel.\n"
          "pilotIndex is 0 for pilot-side AHRS, 1 for copilot. \n"
          "pixelsPerDegree is vertical scale of the 3-d view, center of display. Must be >0\n"
          "G1000 PFD uses 14. Draw it with\n"
          "svtDisplayDrawIn() and free it with destroySVTDisplay().");
static PyObject *XPLMCreateSVTDisplayFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("pilotIndex"), CHAR("pixelsPerDegree"), nullptr};
  (void) self;
  int pilotIndex = 0;
  float pixelsPerDegree = 14.0;
  if(!XPLMCreateSVTDisplay_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMCreateSVTDisplay is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|if", keywords, &pilotIndex, & pixelsPerDegree)){
    return nullptr;
  }
  XPLMCreateSVT_t params;
  params.structSize = sizeof(XPLMCreateSVT_t);
  params.pilotIndex = pilotIndex;
  params.pixelsPerDegree = pixelsPerDegree;
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
    data.hsiSource       = seqInt(seq, 6);
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

/* Shared by mapDisplayDrawIn and the projection routines: they must all be
   handed the SAME description of the map, or the projection you query is not
   the projection you drew. */
static void fillMapDrawInfo(XPLMMapDrawInfo_t *info, int layers, int left, int top, int right, int bottom)
{
  memset(info, 0, sizeof(*info));
  info->structSize = sizeof(XPLMMapDrawInfo_t);
  info->layers = (XPLMMapLayers)layers;
  info->left = left;
  info->top = top;
  info->right = right;
  info->bottom = bottom;
}

/* Returns 1 on success (with *dataPtr left null when the caller passed None),
   0 with a Python exception set on failure. */
static int parseMapCustomData(PyObject *dataOverrides, const char *fname,
                              XPLMMapCustomData_t *data, XPLMMapCustomData_t **dataPtr)
{
  *dataPtr = nullptr;
  if(!dataOverrides || dataOverrides == Py_None){
    return 1;
  }
  memset(data, 0, sizeof(*data));
  data->structSize = sizeof(XPLMMapCustomData_t);

  PyObject *seq = PySequence_Tuple(dataOverrides);
  if(!seq){
    PyErr_Format(PyExc_TypeError, "%s: dataOverrides must be None or a sequence of 12 or 15 values.", fname);
    return 0;
  }
  Py_ssize_t n = PyTuple_Size(seq);
  if(n != 12 && n != 15){
    Py_DECREF(seq);
    PyErr_Format(PyExc_ValueError, "%s: dataOverrides must contain 12 or 15 values.", fname);
    return 0;
  }
  data->datLat         = (float)PyFloat_AsDouble(PyTuple_GetItem(seq, 0));
  data->datLon         = (float)PyFloat_AsDouble(PyTuple_GetItem(seq, 1));
  data->centerX        = seqInt(seq, 2);
  data->centerY        = seqInt(seq, 3);
  data->roseRadius     = seqInt(seq, 4);
  data->mapRange       = (float)PyFloat_AsDouble(PyTuple_GetItem(seq, 5));
  data->orientation    = seqInt(seq, 6);
  data->terrainWarn    = (float)PyFloat_AsDouble(PyTuple_GetItem(seq, 7));
  data->terrainCaution = (float)PyFloat_AsDouble(PyTuple_GetItem(seq, 8));
  data->acfAlt         = (float)PyFloat_AsDouble(PyTuple_GetItem(seq, 9));
  data->gearDown       = seqInt(seq, 10);
  data->trueRotation   = (float)PyFloat_AsDouble(PyTuple_GetItem(seq, 11));
  /* XPLM440 EGPWS fields -- default to 0 when the caller passes only 12. */
  data->nearestRwyElev  = n == 15 ? (float)PyFloat_AsDouble(PyTuple_GetItem(seq, 12)) : 0.0f;
  data->egpwsBrightness = n == 15 ? (float)PyFloat_AsDouble(PyTuple_GetItem(seq, 13)) : 0.0f;
  data->egpwsStyle      = n == 15 ? (XPLMEGPWSStyle)seqInt(seq, 14) : xplm_EGPWS_Style_Blocky;
  Py_DECREF(seq);
  if(PyErr_Occurred()){
    return 0;
  }
  *dataPtr = data;
  return 1;
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
          "dataOverrides is None for live sim state, or a sequence of 12 or 15\n"
          "values in order: (datLat, datLon, centerX, centerY, roseRadius, mapRange,\n"
          "orientation, terrainWarn, terrainCaution, acfAlt, gearDown, trueRotation\n"
          "[, nearestRwyElev, egpwsBrightness, egpwsStyle]). The last three (EGPWS)\n"
          "are XPLM440 additions; omit them (pass 12) to leave them at 0.\n"
          "roseRadius is center-to-rose in pixels and mapRange is that same distance\n"
          "in nautical miles; trueRotation is the true heading that points up.");
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
  if(!parseMapCustomData(dataOverrides, "mapDisplayDrawIn", &data, &dataPtr)){
    return nullptr;
  }
  XPLMMapDrawInfo_t info;
  fillMapDrawInfo(&info, layers, left, top, right, bottom);
  XPLMMapDisplayDrawIn_ptr(map, &info, dataPtr);
  Py_RETURN_NONE;
}

My_DOCSTR(_mapDisplayProject__doc__, "mapDisplayProject",
          "map, layers, left, top, right, bottom, latitude, longitude, dataOverrides=None",
          "map:XPLMMapDisplayRef, layers:int, left:int, top:int, right:int, bottom:int, "
          "latitude:float, longitude:float, dataOverrides:Optional[Sequence[float]]",
          "tuple[float, float] | None",
          "Convert a latitude/longitude into an (x, y) position in panel coordinates,\n"
          "for the map described by these arguments. Inverse of mapDisplayUnproject().\n"
          "Pass the SAME arguments you draw the map with and you get the projection\n"
          "that draw call produces, whether you call this before or after\n"
          "mapDisplayDrawIn(). Unlike mapDisplayDrawIn() this need not be called from\n"
          "a drawing callback -- a click handler or flight loop is equally valid.\n"
          "Returns None if terrain tiles have not loaded or the point is not on this map.");
static PyObject *XPLMMapDisplayProjectFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("map"), CHAR("layers"), CHAR("left"), CHAR("top"),
                             CHAR("right"), CHAR("bottom"), CHAR("latitude"), CHAR("longitude"),
                             CHAR("dataOverrides"), nullptr};
  (void) self;
  PyObject *mapCapsule;
  int layers, left, top, right, bottom;
  double latitude, longitude;
  PyObject *dataOverrides = Py_None;
  if(!XPLMMapDisplayProject_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMMapDisplayProject is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Oiiiiidd|O", keywords, &mapCapsule,
                                  &layers, &left, &top, &right, &bottom,
                                  &latitude, &longitude, &dataOverrides)){
    return nullptr;
  }
  XPLMMapDisplayRef map = getVoidPtr(mapCapsule, MAP_CAPSULE);
  if(!map && PyErr_Occurred()){
    return nullptr;
  }
  XPLMMapCustomData_t data;
  XPLMMapCustomData_t *dataPtr = nullptr;
  if(!parseMapCustomData(dataOverrides, "mapDisplayProject", &data, &dataPtr)){
    return nullptr;
  }
  XPLMMapDrawInfo_t info;
  fillMapDrawInfo(&info, layers, left, top, right, bottom);
  float outX = 0.0f, outY = 0.0f;
  if(!XPLMMapDisplayProject_ptr(map, &info, dataPtr, latitude, longitude, &outX, &outY)){
    Py_RETURN_NONE;
  }
  return Py_BuildValue("(ff)", outX, outY);
}

My_DOCSTR(_mapDisplayUnproject__doc__, "mapDisplayUnproject",
          "map, layers, left, top, right, bottom, x, y, dataOverrides=None",
          "map:XPLMMapDisplayRef, layers:int, left:int, top:int, right:int, bottom:int, "
          "x:float, y:float, dataOverrides:Optional[Sequence[float]]",
          "tuple[float, float] | None",
          "Convert an (x, y) position in panel coordinates back into a\n"
          "(latitude, longitude), for the map described by these arguments. Inverse of\n"
          "mapDisplayProject(). Use it to turn a touch or click on your map into a\n"
          "place in the world. Need not be called from a drawing callback.\n"
          "Returns None if terrain tiles have not loaded or the point does not\n"
          "correspond to anywhere on the earth.");
static PyObject *XPLMMapDisplayUnprojectFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("map"), CHAR("layers"), CHAR("left"), CHAR("top"),
                             CHAR("right"), CHAR("bottom"), CHAR("x"), CHAR("y"),
                             CHAR("dataOverrides"), nullptr};
  (void) self;
  PyObject *mapCapsule;
  int layers, left, top, right, bottom;
  float x, y;
  PyObject *dataOverrides = Py_None;
  if(!XPLMMapDisplayUnproject_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMMapDisplayUnproject is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Oiiiiiff|O", keywords, &mapCapsule,
                                  &layers, &left, &top, &right, &bottom,
                                  &x, &y, &dataOverrides)){
    return nullptr;
  }
  XPLMMapDisplayRef map = getVoidPtr(mapCapsule, MAP_CAPSULE);
  if(!map && PyErr_Occurred()){
    return nullptr;
  }
  XPLMMapCustomData_t data;
  XPLMMapCustomData_t *dataPtr = nullptr;
  if(!parseMapCustomData(dataOverrides, "mapDisplayUnproject", &data, &dataPtr)){
    return nullptr;
  }
  XPLMMapDrawInfo_t info;
  fillMapDrawInfo(&info, layers, left, top, right, bottom);
  double outLatitude = 0.0, outLongitude = 0.0;
  if(!XPLMMapDisplayUnproject_ptr(map, &info, dataPtr, x, y, &outLatitude, &outLongitude)){
    Py_RETURN_NONE;
  }
  return Py_BuildValue("(dd)", outLatitude, outLongitude);
}

My_DOCSTR(_mapDisplayScaleMeter__doc__, "mapDisplayScaleMeter",
          "map, layers, left, top, right, bottom, x, y, dataOverrides=None",
          "map:XPLMMapDisplayRef, layers:int, left:int, top:int, right:int, bottom:int, "
          "x:float, y:float, dataOverrides:Optional[Sequence[float]]",
          "float",
          "Return how many pixels correspond to one meter at (x, y) on the map\n"
          "described by these arguments. Use it to size symbols and range rings so\n"
          "they stay correct as the range changes. Returns 0.0 if terrain tiles have\n"
          "not loaded yet.");
static PyObject *XPLMMapDisplayScaleMeterFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("map"), CHAR("layers"), CHAR("left"), CHAR("top"),
                             CHAR("right"), CHAR("bottom"), CHAR("x"), CHAR("y"),
                             CHAR("dataOverrides"), nullptr};
  (void) self;
  PyObject *mapCapsule;
  int layers, left, top, right, bottom;
  float x, y;
  PyObject *dataOverrides = Py_None;
  if(!XPLMMapDisplayScaleMeter_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMMapDisplayScaleMeter is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Oiiiiiff|O", keywords, &mapCapsule,
                                  &layers, &left, &top, &right, &bottom,
                                  &x, &y, &dataOverrides)){
    return nullptr;
  }
  XPLMMapDisplayRef map = getVoidPtr(mapCapsule, MAP_CAPSULE);
  if(!map && PyErr_Occurred()){
    return nullptr;
  }
  XPLMMapCustomData_t data;
  XPLMMapCustomData_t *dataPtr = nullptr;
  if(!parseMapCustomData(dataOverrides, "mapDisplayScaleMeter", &data, &dataPtr)){
    return nullptr;
  }
  XPLMMapDrawInfo_t info;
  fillMapDrawInfo(&info, layers, left, top, right, bottom);
  return PyFloat_FromDouble(XPLMMapDisplayScaleMeter_ptr(map, &info, dataPtr, x, y));
}

My_DOCSTR(_mapDisplayGetNorthHeading__doc__, "mapDisplayGetNorthHeading",
          "map, layers, left, top, right, bottom, x, y, dataOverrides=None",
          "map:XPLMMapDisplayRef, layers:int, left:int, top:int, right:int, bottom:int, "
          "x:float, y:float, dataOverrides:Optional[Sequence[float]]",
          "float",
          "Return the heading, in degrees clockwise from straight up on the display,\n"
          "at which true north lies at (x, y) on the map described by these arguments.\n"
          "ADD it to a true heading to get the angle to draw that heading at. Accounts\n"
          "both for the map's own rotation and for the projection's convergence, which\n"
          "tilts north away from vertical as you move away from the map's center.\n"
          "Returns 0.0 if terrain tiles have not loaded yet.");
static PyObject *XPLMMapDisplayGetNorthHeadingFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("map"), CHAR("layers"), CHAR("left"), CHAR("top"),
                             CHAR("right"), CHAR("bottom"), CHAR("x"), CHAR("y"),
                             CHAR("dataOverrides"), nullptr};
  (void) self;
  PyObject *mapCapsule;
  int layers, left, top, right, bottom;
  float x, y;
  PyObject *dataOverrides = Py_None;
  if(!XPLMMapDisplayGetNorthHeading_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMMapDisplayGetNorthHeading is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Oiiiiiff|O", keywords, &mapCapsule,
                                  &layers, &left, &top, &right, &bottom,
                                  &x, &y, &dataOverrides)){
    return nullptr;
  }
  XPLMMapDisplayRef map = getVoidPtr(mapCapsule, MAP_CAPSULE);
  if(!map && PyErr_Occurred()){
    return nullptr;
  }
  XPLMMapCustomData_t data;
  XPLMMapCustomData_t *dataPtr = nullptr;
  if(!parseMapCustomData(dataOverrides, "mapDisplayGetNorthHeading", &data, &dataPtr)){
    return nullptr;
  }
  XPLMMapDrawInfo_t info;
  fillMapDrawInfo(&info, layers, left, top, right, bottom);
  return PyFloat_FromDouble(XPLMMapDisplayGetNorthHeading_ptr(map, &info, dataPtr, x, y));
}

My_DOCSTR(_mapDisplayGetTerrainAltitudes__doc__, "mapDisplayGetTerrainAltitudes",
          "map",
          "map:XPLMMapDisplayRef",
          "tuple[float, float] | None",
          "Return (minAltitude, maxAltitude) -- the lowest and highest altitude (feet)\n"
          "shown on the map's EGPWS terrain display. Altitudes are only available if\n"
          "the map was drawn with the Map_EGPWS layer; if not, returns None. Must be\n"
          "called from an avionics drawing callback.");
static PyObject *XPLMMapDisplayGetTerrainAltitudesFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("map"), nullptr};
  (void) self;
  PyObject *mapCapsule;
  if(!XPLMMapDisplayGetTerrainAltitudes_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMMapDisplayGetTerrainAltitudes is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &mapCapsule)){
    return nullptr;
  }
  XPLMMapDisplayRef map = getVoidPtr(mapCapsule, MAP_CAPSULE);
  if(!map && PyErr_Occurred()){
    return nullptr;
  }
  float minAlt = 0.0f, maxAlt = 0.0f;
  if(!XPLMMapDisplayGetTerrainAltitudes_ptr(map, &minAlt, &maxAlt)){
    Py_RETURN_NONE;
  }
  return Py_BuildValue("(ff)", minAlt, maxAlt);
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
  {"mapDisplayProject", (PyCFunction)XPLMMapDisplayProjectFun, METH_VARARGS | METH_KEYWORDS, _mapDisplayProject__doc__},
  {"XPLMMapDisplayProject", (PyCFunction)XPLMMapDisplayProjectFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"mapDisplayUnproject", (PyCFunction)XPLMMapDisplayUnprojectFun, METH_VARARGS | METH_KEYWORDS, _mapDisplayUnproject__doc__},
  {"XPLMMapDisplayUnproject", (PyCFunction)XPLMMapDisplayUnprojectFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"mapDisplayScaleMeter", (PyCFunction)XPLMMapDisplayScaleMeterFun, METH_VARARGS | METH_KEYWORDS, _mapDisplayScaleMeter__doc__},
  {"XPLMMapDisplayScaleMeter", (PyCFunction)XPLMMapDisplayScaleMeterFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"mapDisplayGetNorthHeading", (PyCFunction)XPLMMapDisplayGetNorthHeadingFun, METH_VARARGS | METH_KEYWORDS, _mapDisplayGetNorthHeading__doc__},
  {"XPLMMapDisplayGetNorthHeading", (PyCFunction)XPLMMapDisplayGetNorthHeadingFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"mapDisplayGetTerrainAltitudes", (PyCFunction)XPLMMapDisplayGetTerrainAltitudesFun, METH_VARARGS | METH_KEYWORDS, _mapDisplayGetTerrainAltitudes__doc__},
  {"XPLMMapDisplayGetTerrainAltitudes", (PyCFunction)XPLMMapDisplayGetTerrainAltitudesFun, METH_VARARGS | METH_KEYWORDS, ""},
  {nullptr, nullptr, 0, nullptr}
};
#pragma GCC diagnostic pop
