#define _GNU_SOURCE 1
#include <Python.h>
#include <sys/time.h>
#include <stdio.h>
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMMap.h>
#include "plugin_dl.h"
#include "utils.h"
#include "map.h"

static PyObject *mapDict;  /* {
                                mapDictKey1: [0] module, [1] type ...,
                                mapDictKey2: [0] module, [1] type ...,
                              } */
#define MAP_MODULE_NAME 0
#define MAP_TYPE 1
#define MAP_LAYER 2
#define MAP_DELETE 3
#define MAP_PREP 4
#define MAP_DRAW 5
#define MAP_ICON 6
#define MAP_LABEL 7
#define MAP_TOGGLE 8
#define MAP_NAME 9
#define MAP_REFCON 10
intptr_t mapCntr;

static PyObject *mapRefDict; /* {
                                  <mapLayerIDCapsule1> : mapDictKey1
                                  <mapLayerIDCapsule2> : mapDictKey2,
                                }
                             */


static PyObject *mapCreateDict; /* {
                                     mapCreateCntr1: [0]callback, [1]refCon, [2]module,
                                     mapCreateCntr2: [0]callback, [1]refCon, [2]module,
                                   } */

#define MAPCREATE_CALLBACK 0
#define MAPCREATE_REFCON 1
#define MAPCREATE_MODULE_NAME 2

intptr_t mapCreateCntr;
PyObject *mapLayerIDCapsules;
PyObject *mapProjectionCapsule;

static const char layerIDRefName[] = "XPLMMapLayerID";
static const char projectionRefName[] = "XPLMMapProjectionID";


void resetMap(void) {
  PyObject *key, *tuple;
  Py_ssize_t pos = 0;
  while(PyDict_Next(mapDict, &pos, &key, &tuple)) {
    char *moduleName = objToStr(PyTuple_GetItem(tuple, MAP_MODULE_NAME));
    char *layerName = objToStr(PyTuple_GetItem(tuple, MAP_NAME));
    XPLMMapLayerID inLayer = PyLong_AsVoidPtr(key);
    XPLMDestroyMapLayer(inLayer);
    pythonLog("[XPPython3] Reset --     %s - (%s)\n", moduleName, layerName);
    free(moduleName);
    free(layerName);
  }
  PyDict_Clear(mapDict);
  PyDict_Clear(mapCreateDict);
  PyDict_Clear(mapRefDict);
  
}

static inline void mapCallback(int whichCallback, XPLMMapLayerID inLayer, const float *inMapBoundsLeftTopRightBottom, float zoomRatio,
                        float mapUnitsPerUserInterfaceUnit, XPLMMapStyle mapStyle, XPLMMapProjectionID projection,
                        void *inRefcon)
{
  PyObject *layerObj, *boundsObj, *refconObj, *callback;
  PyObject *pRefCon = PyLong_FromVoidPtr(inRefcon);
  PyObject *callbackInfo = PyDict_GetItem(mapDict, pRefCon);
  Py_DECREF(pRefCon);
  if(callbackInfo == NULL){
    printf("Couldn't find map callback with id = %p., destroying it insted", inRefcon); 
    XPLMDestroyMapLayer(inLayer);
    return;
  }

  set_moduleName(PyTuple_GetItem(callbackInfo, MAP_MODULE_NAME));

  if (whichCallback < MAP_DRAW || whichCallback > MAP_LABEL) {
    pythonLog("mapCallback called with bad index %d\n", whichCallback);
    return;
  }

  callback = PyTuple_GetItem(callbackInfo, whichCallback);
  if (callback == Py_None) {
    return;
  }
  layerObj = getPtrRef(inLayer, mapLayerIDCapsules, layerIDRefName);
  mapProjectionCapsule = getPtrRefOneshot(projection, projectionRefName);
  refconObj = PyTuple_GetItem(callbackInfo, MAP_REFCON);
  
  boundsObj = PyTuple_New(4);
  //Steals the ref!
  PyTuple_SetItem(boundsObj, 0, PyFloat_FromDouble((double)inMapBoundsLeftTopRightBottom[0]));
  PyTuple_SetItem(boundsObj, 1, PyFloat_FromDouble((double)inMapBoundsLeftTopRightBottom[1]));
  PyTuple_SetItem(boundsObj, 2, PyFloat_FromDouble((double)inMapBoundsLeftTopRightBottom[2]));
  PyTuple_SetItem(boundsObj, 3, PyFloat_FromDouble((double)inMapBoundsLeftTopRightBottom[3]));

  PyObject *zoomRatioObj = PyFloat_FromDouble(zoomRatio);
  PyObject *mapUnitsPerUserInterfaceUnitObj = PyFloat_FromDouble(mapUnitsPerUserInterfaceUnit);
  PyObject *mapStyleObj = PyLong_FromLong(mapStyle);
  PyObject *pRes = PyObject_CallFunctionObjArgs(callback, layerObj, boundsObj, zoomRatioObj,
                                         mapUnitsPerUserInterfaceUnitObj, mapStyleObj, mapProjectionCapsule, refconObj,NULL);
  if(!pRes){
    PyObject *err = PyErr_Occurred();
    if(err){
      pythonLogException();
    }
  }
  Py_DECREF(zoomRatioObj);
  Py_DECREF(mapUnitsPerUserInterfaceUnitObj);
  Py_DECREF(mapStyleObj);
  Py_DECREF(boundsObj);
  Py_DECREF(layerObj);
  Py_DECREF(mapProjectionCapsule);
  mapProjectionCapsule = NULL;
  Py_XDECREF(pRes);
}

static inline void mapPrepareCacheCallback(XPLMMapLayerID inLayer, const float *inMapBoundsLeftTopRightBottom,
                                           XPLMMapProjectionID projection, void *inRefcon)
{
  PyObject *layerObj, *boundsObj, *refconObj, *callback;
  PyObject *ref = PyLong_FromVoidPtr(inRefcon);
  PyObject *callbackInfo = PyDict_GetItem(mapDict, ref);
  Py_DECREF(ref);
  if(callbackInfo == NULL){
    return;
  }
  callback = PyTuple_GetItem(callbackInfo, MAP_PREP);
  if (callback == Py_None) {
    return;
  }
  set_moduleName(PyTuple_GetItem(callbackInfo, MAP_MODULE_NAME));

  layerObj = getPtrRef(inLayer, mapLayerIDCapsules, layerIDRefName);
  mapProjectionCapsule = getPtrRefOneshot(projection, projectionRefName);
  refconObj = PyTuple_GetItem(callbackInfo, MAP_REFCON);
  
  boundsObj = PyTuple_New(4);
  PyTuple_SetItem(boundsObj, 0, PyFloat_FromDouble((double)inMapBoundsLeftTopRightBottom[0]));
  PyTuple_SetItem(boundsObj, 1, PyFloat_FromDouble((double)inMapBoundsLeftTopRightBottom[1]));
  PyTuple_SetItem(boundsObj, 2, PyFloat_FromDouble((double)inMapBoundsLeftTopRightBottom[2]));
  PyTuple_SetItem(boundsObj, 3, PyFloat_FromDouble((double)inMapBoundsLeftTopRightBottom[3]));

  PyObject *pRes = PyObject_CallFunctionObjArgs(callback, layerObj, boundsObj, mapProjectionCapsule, refconObj, NULL);
  if(!pRes){
    printf("MapPrepareCacheCallback callback failed.\n");
    PyObject *err = PyErr_Occurred();
    if(err){
      pythonLogException();
    }
  }
  Py_DECREF(boundsObj);
  Py_DECREF(layerObj);
  Py_DECREF(mapProjectionCapsule);
  mapProjectionCapsule = NULL;
  Py_XDECREF(pRes);
}

static inline void mapWillBeDeletedCallback(XPLMMapLayerID inLayer, void *inRefcon)
{
  PyObject *layerObj, *refconObj, *callback;
  PyObject *ref = PyLong_FromVoidPtr(inRefcon);
  PyObject *callbackInfo = PyDict_GetItem(mapDict, ref);
  Py_DECREF(ref);
  if(callbackInfo == NULL){
    printf("Couldn't find map callback with id = %p.", inRefcon); 
    return;
  }

  callback = PyTuple_GetItem(callbackInfo, MAP_DELETE);
  if (callback == Py_None) {
    return;
  }

  set_moduleName(PyTuple_GetItem(callbackInfo, MAP_MODULE_NAME));

  layerObj = getPtrRef(inLayer, mapLayerIDCapsules, layerIDRefName);
  refconObj = PyTuple_GetItem(callbackInfo, MAP_REFCON);
  
  PyObject *pRes = PyObject_CallFunctionObjArgs(callback, layerObj, refconObj, NULL);
  if(!pRes){
    printf("MapWillBeDeletedCallback callback failed.\n");
    PyObject *err = PyErr_Occurred();
    if(err){
      pythonLogException();
    }
  }
  Py_DECREF(layerObj);
  Py_XDECREF(pRes);
}

static inline void mapCreatedCallback(const char *mapType, void *inRefcon)
{
  PyObject *mapTypeObj, *refconObj, *callback;
  PyObject *ref = PyLong_FromVoidPtr(inRefcon);
  PyObject *callbackInfo = PyDict_GetItem(mapCreateDict, ref);
  Py_DECREF(ref);
  if(callbackInfo == NULL){
    printf("Couldn't find map created callback with id = %p.", inRefcon); 
    return;
  }
  mapTypeObj = PyUnicode_DecodeUTF8(mapType, strlen(mapType), NULL);
  callback = PyTuple_GetItem(callbackInfo, MAPCREATE_CALLBACK);
  refconObj = PyTuple_GetItem(callbackInfo, MAPCREATE_REFCON);
  set_moduleName(PyTuple_GetItem(callbackInfo, MAPCREATE_MODULE_NAME));
  
  PyObject *pRes = PyObject_CallFunctionObjArgs(callback, mapTypeObj, refconObj, NULL);
  if(!pRes){
    printf("mapCreatedCallback callback failed.\n");
    PyObject *err = PyErr_Occurred();
    if(err){
      pythonLogException();
    }
  }
  Py_DECREF(mapTypeObj);
  Py_XDECREF(pRes);
}

static void mapDrawingCallback(XPLMMapLayerID inLayer, const float *inMapBoundsLeftTopRightBottom, float zoomRatio,
                        float mapUnitsPerUserInterfaceUnit, XPLMMapStyle mapStyle, XPLMMapProjectionID projection,
                        void *inRefcon)
{
  mapCallback(MAP_DRAW, inLayer, inMapBoundsLeftTopRightBottom, zoomRatio,
              mapUnitsPerUserInterfaceUnit, mapStyle, projection, inRefcon);
}

static void mapIconDrawingCallback(XPLMMapLayerID inLayer, const float *inMapBoundsLeftTopRightBottom, float zoomRatio,
                        float mapUnitsPerUserInterfaceUnit, XPLMMapStyle mapStyle, XPLMMapProjectionID projection,
                        void *inRefcon)
{
  mapCallback(MAP_ICON, inLayer, inMapBoundsLeftTopRightBottom, zoomRatio,
              mapUnitsPerUserInterfaceUnit, mapStyle, projection, inRefcon);
}

static void mapLabelDrawingCallback(XPLMMapLayerID inLayer, const float *inMapBoundsLeftTopRightBottom, float zoomRatio,
                        float mapUnitsPerUserInterfaceUnit, XPLMMapStyle mapStyle, XPLMMapProjectionID projection,
                        void *inRefcon)
{
  mapCallback(MAP_LABEL, inLayer, inMapBoundsLeftTopRightBottom, zoomRatio,
              mapUnitsPerUserInterfaceUnit, mapStyle, projection, inRefcon);
}

My_DOCSTR(_createMapLayer__doc__, "createMapLayer", "mapType=MAP_USER_INTERFACE, layerType=MapLayer_Markings, delete=None, prep=None, draw=None, icon=None, label=None, showToggle=1, name=\"\", refCon=None",
          "Returns layerID of newly created map layer, setting callbacks.\n"
          "\n"
          "If map does not currently exist, returns 0.");
static PyObject *XPLMCreateMapLayerFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"mapType", "layerType", "delete", "prep", "draw", "icon", "label", "showToggle", "name", "refCon", NULL};
  (void) self;
  PyObject *map=Py_None, *delete=Py_None, *prep=Py_None, *draw=Py_None, *icon=Py_None, *label=Py_None, *refCon=Py_None;
  PyObject *firstObj=Py_None;
  int layerType=xplm_MapLayer_Markings, showToggle=1;
  PyObject *name=Py_None;

  PyObject *paramsObj=Py_None;

  if(!XPLMCreateMapLayer_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMCreateMapLayer is available only in XPLM300 and up.");
    return NULL;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|OiOOOOOiOO", keywords, &firstObj, &layerType, &delete, &prep, &draw, &icon, &label, &showToggle, &name, &refCon)){
    return NULL;
  }
  if (firstObj == Py_None) {
    ;
  } else if (PySequence_Check(firstObj)) {
    /* First items is sequence... BUT, first item could be mapType which is a string... which is a sequence
     * So, IF the first item is a sequence, we check to see if element #1 of THAT sequence is
     * an integer... If it is, then this appears to be a single-argument call, where a
     * list of values is passed (with integer 'layerType' is passed as element #1)
     * If _not_ integer, we assume first element is mapType (... a string).
     */
    if (PySequence_Length(firstObj) > 2) {
      PyObject *item = PySequence_GetItem(firstObj, 1);
      if (PyLong_Check(item)) {
        paramsObj = firstObj;
        if (PySequence_Length(paramsObj) != 10) {
          PyErr_SetString(PyExc_AttributeError, "createMapLayer tuple did not contain 10 values.\n");
          return NULL;
        }
      }
      Py_XDECREF(item);
    } else {
      map = firstObj;
    }
  } else {
    map = firstObj;
  }

  XPLMCreateMapLayer_t inParams;
  inParams.structSize = sizeof(inParams);
  PyObject *tmpObjMap;
  char *tmpMap;
  PyObject *tmpObjLayerName;
  char *tmpLayerName;
  PyObject *paramsTuple;
  if (paramsObj == Py_None) {
    if (map == Py_None) {
      map = PyUnicode_DecodeUTF8(XPLM_MAP_USER_INTERFACE, strlen(XPLM_MAP_USER_INTERFACE), NULL);
    }
    if (name == Py_None) {
      name = PyUnicode_DecodeUTF8("", 0, NULL);
    }
    paramsTuple = Py_BuildValue("(sOiOOOOOiOO)", CurrentPythonModuleName, map, layerType, delete, prep, draw, icon, label, showToggle, name, refCon);
  } else {
    PyObject *paramsList = PySequence_List(paramsObj);
    PyObject *tmp = Py_BuildValue("[s]", CurrentPythonModuleName);
    PyObject *concat = PySequence_Concat(tmp, paramsList);
    Py_DECREF(tmp);
    Py_DECREF(paramsList);
    paramsTuple = PySequence_Tuple(concat);
    Py_DECREF(concat);
  }

  tmpObjMap = PyUnicode_AsUTF8String(PyTuple_GetItem(paramsTuple, MAP_TYPE));
  tmpMap = PyBytes_AsString(tmpObjMap);
  
  if (PyErr_Occurred()) {
    Py_DECREF(tmpObjMap);
    Py_DECREF(paramsTuple);
    return NULL;
  }
  inParams.mapToCreateLayerIn = tmpMap;
  inParams.layerType = PyLong_AsLong(PyTuple_GetItem(paramsTuple, MAP_LAYER));
  inParams.showUiToggle = PyLong_AsLong(PyTuple_GetItem(paramsTuple, MAP_TOGGLE));
  
  tmpObjLayerName = PyUnicode_AsUTF8String(PyTuple_GetItem(paramsTuple, MAP_NAME));
  tmpLayerName = PyBytes_AsString(tmpObjLayerName);
  
  if (PyErr_Occurred()) {
    Py_DECREF(tmpObjMap);
    Py_DECREF(tmpObjLayerName);
    Py_DECREF(paramsTuple);
    return NULL;
  }

  inParams.refcon = (void *)++mapCntr;
  inParams.layerName = tmpLayerName;
  inParams.mapToCreateLayerIn = tmpMap;
  inParams.willBeDeletedCallback = mapWillBeDeletedCallback;
  inParams.prepCacheCallback = mapPrepareCacheCallback;
  inParams.drawCallback = mapDrawingCallback;
  inParams.iconCallback = mapIconDrawingCallback;
  inParams.labelCallback = mapLabelDrawingCallback;

  /* !!!!
   * XPLMCreateMapLayer() will immediately call prep_cache, so
   * make sure we set mapDict prior to call.
   */
  PyObject *mapDictKey = PyLong_FromVoidPtr(inParams.refcon);

  PyDict_SetItem(mapDict, mapDictKey, paramsTuple);

  XPLMMapLayerID mapLayerID = XPLMCreateMapLayer_ptr(&inParams);
  if(!mapLayerID){
    char msg[1024];
    if (!XPLMMapExists(inParams.mapToCreateLayerIn)) {
      sprintf(msg, "Map [%s] does not (yet) exist.\n", inParams.mapToCreateLayerIn);
      PyErr_SetString(PyExc_RuntimeError, msg);
    } else {
      sprintf(msg, "Unknown map creation error.\n");
      PyErr_SetString(PyExc_ValueError, msg);
    }
    PyDict_DelItem(mapDict, mapDictKey);
    Py_DECREF(mapDictKey);
    Py_DECREF(tmpObjMap);
    Py_DECREF(tmpObjLayerName);
    Py_DECREF(paramsTuple);
    return NULL;
  }
  PyObject *mapLayerCapsule = getPtrRef(mapLayerID, mapLayerIDCapsules, layerIDRefName);
  PyDict_SetItem(mapRefDict, mapLayerCapsule, mapDictKey);
  Py_DECREF(tmpObjMap);
  Py_DECREF(tmpObjLayerName);
  Py_DECREF(paramsTuple);
  Py_DECREF(mapDictKey);
  return mapLayerCapsule;
}

My_DOCSTR(_destroyMapLayer__doc__, "destroyMapLayer", "layerID",
          "Destroys map layer given by layerID.");
static PyObject *XPLMDestroyMapLayerFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"layerID", NULL};
  (void) self;
  PyObject *layer;

  if(!XPLMDestroyMapLayer_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMDestroyMapLayer is available only in XPLM300 and up.");
    return NULL;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &layer)){
    return NULL;
  }

  XPLMMapLayerID inLayer = refToPtr(layer, layerIDRefName);
  int res = XPLMDestroyMapLayer_ptr(inLayer);
  if(res){
    PyObject *pRefCon = PyDict_GetItem(mapRefDict, layer);
    PyDict_DelItem(mapDict, pRefCon);
    PyDict_DelItem(mapRefDict, layer);
    removePtrRef(inLayer, mapLayerIDCapsules);
  }

  return PyLong_FromLong(res);
}

My_DOCSTR(_registerMapCreationHook__doc__, "registerMapCreationHook", "mapCreated, refCon=None",
          "Registers mapCreated() callback to notify you when a map is created.\n"
          "\n"
          "Callback gets two parameters: (mapType, refCon)");
static PyObject *XPLMRegisterMapCreationHookFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"mapCreated", "refCon", NULL};
  (void) self;
  PyObject *callback, *inRefCon=Py_None;
  if(!XPLMRegisterMapCreationHook_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMRegisterMapCreationHook is available only in XPLM300 and up.");
    return NULL;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|O", keywords, &callback, &inRefCon)){
    return NULL;
  }

  PyObject *argObj = Py_BuildValue("(OOs)", callback, inRefCon, CurrentPythonModuleName);

  void *refcon = (void *)++mapCreateCntr;
  PyObject *refconObj = PyLong_FromVoidPtr(refcon);
  PyDict_SetItem(mapCreateDict, refconObj, argObj);
  Py_DECREF(argObj);
  Py_DECREF(refconObj);
  XPLMRegisterMapCreationHook_ptr(mapCreatedCallback, refcon);
  Py_RETURN_NONE;
}

My_DOCSTR(_mapExists__doc__, "mapExists", "mapType",
          "Returns 1 if mapType exists, 0 otherwise."
          "\n"
          "mapType is either xp.MAP_USER_INTERFACE or xp.MAP_IOS");
static PyObject *XPLMMapExistsFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"mapType", NULL};
  (void) self;
  const char *mapType;

  if(!XPLMMapExists_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMMapExists is available only in XPLM300 and up.");
    return NULL;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "s", keywords, &mapType)){
    return NULL;
  }
  int res = XPLMMapExists_ptr(mapType);
  return PyLong_FromLong(res);
}

My_DOCSTR(_drawMapIconFromSheet__doc__, "drawMapIconFromSheet", "layerID, png, s, t, ds, dt, x, y, orientation, rotationDegrees, mapWidth",
          "Draws icon into map layer.\n"
          "\n"
          "Only valid within iconLayer() callback.");
static PyObject *XPLMDrawMapIconFromSheetFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"layerID", "png", "s", "t", "ds", "dt", "x", "y", "orientation", "rotationDegrees", "mapWidth", NULL};
  (void) self;
  PyObject *layerObj;
  const char *inPngPath;
  int s, t, ds, dt;
  float mapX, mapY, rotationDegrees, mapWidth;
  XPLMMapOrientation orientation;

  if(!XPLMDrawMapIconFromSheet_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMDrawMapIconFromSheet is available only in XPLM300 and up.");
    return NULL;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Osiiiiffiff", keywords, &layerObj, &inPngPath, &s, &t, &ds, &dt, &mapX, &mapY,
                       &orientation, &rotationDegrees, &mapWidth)){
    return NULL;
  }
  XPLMMapLayerID layer = refToPtr(layerObj, layerIDRefName);
  XPLMDrawMapIconFromSheet_ptr(layer, inPngPath, s, t, ds, dt, mapX, mapY,
                       orientation, rotationDegrees, mapWidth);
  Py_RETURN_NONE;
}

My_DOCSTR(_drawMapLabel__doc__, "drawMapLabel", "layerID, text, x, y, orientation, rotationDegrees",
          "Draws label within map layer.\n"
          "\n"
          "Only valid within labelLayer() callback.");
static PyObject *XPLMDrawMapLabelFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"layerID", "text", "x", "y", "orientation", "rotationDegrees", NULL};
  (void) self;
  PyObject *layerObj;
  const char *inText;
  float mapX, mapY, rotationDegrees;
  XPLMMapOrientation orientation;

  if(!XPLMDrawMapLabel_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMDrawMapLabel is available only in XPLM300 and up.");
    return NULL;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Osffif", keywords, &layerObj, &inText, &mapX, &mapY,
                       &orientation, &rotationDegrees)){
    return NULL;
  }
  XPLMMapLayerID layer = refToPtr(layerObj, layerIDRefName);
  XPLMDrawMapLabel_ptr(layer, inText, mapX, mapY, orientation, rotationDegrees);
  Py_RETURN_NONE;
}

My_DOCSTR(_mapProject__doc__, "mapProject", "projection, latitude, longitude",
          "Returns map layer (x, y) for given latitude, longitude.\n"
          "\n"
          "Only valid within map layer callbacks.");
static PyObject *XPLMMapProjectFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"projection", "latitude", "longitude", "x", "y", NULL};
  (void) self;
  PyObject *projectionObj, *outX, *outY;
  double latitude, longitude;
  int returnValues = 0;
  if(!XPLMMapProject_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMMapProject is available only in XPLM300 and up.");
    return NULL;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "OddOO", keywords, &projectionObj, &latitude, &longitude, &outX, &outY)) {
    PyErr_Clear();
    returnValues = 1;
    static char *nkeywords[] = {"projection", "latitude", "longitude", NULL};
    if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Odd", nkeywords, &projectionObj, &latitude, &longitude)) {
      return NULL;
    }
  }
  XPLMMapProjectionID projection = refToPtr(projectionObj, projectionRefName);
  float x, y;
  XPLMMapProject_ptr(projection, latitude, longitude, &x, &y);
  if (returnValues) {
    return Py_BuildValue("ff", x, y);
  }
  pythonLogWarning("XPLMMapProject no longer require final (x, y) parameters");

  if (outX != Py_None)
    PyList_Append(outX, PyFloat_FromDouble((double) x));
  if (outY != Py_None)
    PyList_Append(outY, PyFloat_FromDouble((double) y));
    
  Py_RETURN_NONE;
}

My_DOCSTR(_mapUnproject__doc__, "mapUnproject", "projection, x, y",
          "Returns latitude, longitude for given map coordinates.\n"
          "\n"
          "Only valid within map layer callbacks.");
static PyObject *XPLMMapUnprojectFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"projection", "x", "y", "latitude", "longitude", NULL};
  (void) self;
  PyObject *projectionObj, *outLatitude, *outLongitude;
  float mapX, mapY;
  int returnValues = 0;

  if(!XPLMMapUnproject_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMMapUnproject is available only in XPLM300 and up.");
    return NULL;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "OffOO", keywords, &projectionObj, &mapX, &mapY, &outLatitude, &outLongitude)){
    PyErr_Clear();
    returnValues = 1;
    static char *nkeywords[] = {"projection", "x", "y", NULL};
    if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Off", nkeywords, &projectionObj, &mapX, &mapY)){
      return NULL;
    }
  }
  XPLMMapProjectionID projection = refToPtr(projectionObj, projectionRefName);
  double longitude, latitude;
  XPLMMapUnproject_ptr(projection, mapX, mapY, &latitude, &longitude);
  if (returnValues) 
    return Py_BuildValue("dd", latitude, longitude);
  pythonLogWarning("XPLMMapUnproject no longer requires final latitude, longitude parameters.");
  if (outLatitude != Py_None)
    PyList_Append(outLatitude, PyFloat_FromDouble(latitude));
  if (outLongitude != Py_None)
    PyList_Append(outLongitude, PyFloat_FromDouble(longitude));
    
  Py_RETURN_NONE;
}

My_DOCSTR(_mapScaleMeter__doc__, "mapScaleMeter", "projection, x, y",
          "Returns number of units for \"one meter\" using current projection.\n"
          "\n"
          "Only valid within map layer callbacks.");
static PyObject *XPLMMapScaleMeterFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"projection", "x", "y", NULL};
  (void) self;
  PyObject *projectionObj;
  float mapX, mapY;

  if(!XPLMMapScaleMeter_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMMapScaleMeter is available only in XPLM300 and up.");
    return NULL;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Off", keywords, &projectionObj, &mapX, &mapY)){
    return NULL;
  }
  XPLMMapProjectionID projection = refToPtr(projectionObj, projectionRefName);
  float res = XPLMMapScaleMeter_ptr(projection, mapX, mapY);
  return PyFloat_FromDouble(res);
}

My_DOCSTR(_mapGetNorthHeading__doc__, "mapGetNorthHeading", "projection, x, y",
          "Returns mapping angle for map projection at point.\n"
          "\n"
          "Only valid within map layer callbacks.");
static PyObject *XPLMMapGetNorthHeadingFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"projection", "x", "y", NULL};
  (void) self;
  PyObject *projectionObj;
  float mapX, mapY;

  if(!XPLMMapGetNorthHeading_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMMapGetNorthHeading is available only in XPLM300 and up.");
    return NULL;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Off", keywords, &projectionObj, &mapX, &mapY)){
    return NULL;
  }
  XPLMMapProjectionID projection = refToPtr(projectionObj, projectionRefName);
  float res = XPLMMapGetNorthHeading_ptr(projection, mapX, mapY);
  return PyFloat_FromDouble(res);
}

static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  PyDict_Clear(mapDict);
  Py_DECREF(mapDict);
  PyDict_Clear(mapRefDict);
  Py_DECREF(mapRefDict);
  PyDict_Clear(mapCreateDict);
  Py_DECREF(mapCreateDict);
  PyDict_Clear(mapLayerIDCapsules);
  Py_DECREF(mapLayerIDCapsules);
  Py_RETURN_NONE;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
static PyMethodDef XPLMMapMethods[] = {
  {"createMapLayer", (PyCFunction)XPLMCreateMapLayerFun, METH_VARARGS | METH_KEYWORDS, _createMapLayer__doc__},
  {"XPLMCreateMapLayer", (PyCFunction)XPLMCreateMapLayerFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"destroyMapLayer", (PyCFunction)XPLMDestroyMapLayerFun, METH_VARARGS | METH_KEYWORDS, _destroyMapLayer__doc__},
  {"XPLMDestroyMapLayer", (PyCFunction)XPLMDestroyMapLayerFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"registerMapCreationHook", (PyCFunction)XPLMRegisterMapCreationHookFun, METH_VARARGS | METH_KEYWORDS, _registerMapCreationHook__doc__},
  {"XPLMRegisterMapCreationHook", (PyCFunction)XPLMRegisterMapCreationHookFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"mapExists", (PyCFunction)XPLMMapExistsFun, METH_VARARGS | METH_KEYWORDS, _mapExists__doc__},
  {"XPLMMapExists", (PyCFunction)XPLMMapExistsFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"drawMapIconFromSheet", (PyCFunction)XPLMDrawMapIconFromSheetFun, METH_VARARGS | METH_KEYWORDS, _drawMapIconFromSheet__doc__},
  {"XPLMDrawMapIconFromSheet", (PyCFunction)XPLMDrawMapIconFromSheetFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"drawMapLabel", (PyCFunction)XPLMDrawMapLabelFun, METH_VARARGS | METH_KEYWORDS, _drawMapLabel__doc__},
  {"XPLMDrawMapLabel", (PyCFunction)XPLMDrawMapLabelFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"mapProject", (PyCFunction)XPLMMapProjectFun, METH_VARARGS | METH_KEYWORDS, _mapProject__doc__},
  {"XPLMMapProject", (PyCFunction)XPLMMapProjectFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"mapUnproject", (PyCFunction)XPLMMapUnprojectFun, METH_VARARGS | METH_KEYWORDS, _mapUnproject__doc__},
  {"XPLMMapUnproject", (PyCFunction)XPLMMapUnprojectFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"mapScaleMeter", (PyCFunction)XPLMMapScaleMeterFun, METH_VARARGS | METH_KEYWORDS, _mapScaleMeter__doc__},
  {"XPLMMapScaleMeter", (PyCFunction)XPLMMapScaleMeterFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"mapGetNorthHeading", (PyCFunction)XPLMMapGetNorthHeadingFun, METH_VARARGS | METH_KEYWORDS, _mapGetNorthHeading__doc__},
  {"XPLMMapGetNorthHeading", (PyCFunction)XPLMMapGetNorthHeadingFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"_cleanup", cleanup, METH_VARARGS, ""},
  {NULL, NULL, 0, NULL}
};
#pragma GCC diagnostic pop


static struct PyModuleDef XPLMMapModule = {
  PyModuleDef_HEAD_INIT,
  "XPLMMap",
  "Laminar documentation: \n"
  "   https://developer.x-plane.com/sdk/XPLMMap/\n"
  "XPPython3 documentation: \n"
  "   https://xppython3.rtfd.io/en/stable/development/modules/map.html",
  -1,
  XPLMMapMethods,
  NULL,
  NULL,
  NULL,
  NULL
};

PyMODINIT_FUNC
PyInit_XPLMMap(void)
{
  if(!(mapCreateDict = PyDict_New())){
    return NULL;
  }
  PyDict_SetItemString(XPY3pythonDicts, "mapCreates", mapCreateDict);
  if(!(mapRefDict = PyDict_New())){
    return NULL;
  }
  PyDict_SetItemString(XPY3pythonDicts, "mapRefs", mapRefDict);
  if(!(mapDict = PyDict_New())){
    return NULL;
  }
  PyDict_SetItemString(XPY3pythonDicts, "maps", mapDict);
  if(!(mapLayerIDCapsules = PyDict_New())){
    return NULL;
  }
  PyDict_SetItemString(XPY3pythonCapsules, layerIDRefName, mapLayerIDCapsules);

  PyObject *mod = PyModule_Create(&XPLMMapModule);
  if(mod){
    PyModule_AddStringConstant(mod, "__author__", "Peter Buckner (pbuck@avnwx.com)");
    PyModule_AddIntConstant(mod, "xplm_MapStyle_VFR_Sectional", xplm_MapStyle_VFR_Sectional);
    PyModule_AddIntConstant(mod, "xplm_MapStyle_IFR_LowEnroute", xplm_MapStyle_IFR_LowEnroute);
    PyModule_AddIntConstant(mod, "xplm_MapStyle_IFR_HighEnroute", xplm_MapStyle_IFR_HighEnroute);
    PyModule_AddIntConstant(mod, "xplm_MapLayer_Fill", xplm_MapLayer_Fill);
    PyModule_AddIntConstant(mod, "xplm_MapLayer_Markings", xplm_MapLayer_Markings);
    PyModule_AddStringConstant(mod, "XPLM_MAP_USER_INTERFACE", XPLM_MAP_USER_INTERFACE);
    PyModule_AddStringConstant(mod, "XPLM_MAP_IOS", XPLM_MAP_IOS);
    PyModule_AddIntConstant(mod, "xplm_MapOrientation_Map", xplm_MapOrientation_Map);
    PyModule_AddIntConstant(mod, "xplm_MapOrientation_UI", xplm_MapOrientation_UI);

    PyModule_AddIntConstant(mod, "MapStyle_VFR_Sectional", xplm_MapStyle_VFR_Sectional);
    PyModule_AddIntConstant(mod, "MapStyle_IFR_LowEnroute", xplm_MapStyle_IFR_LowEnroute);
    PyModule_AddIntConstant(mod, "MapStyle_IFR_HighEnroute", xplm_MapStyle_IFR_HighEnroute);
    PyModule_AddIntConstant(mod, "MapLayer_Fill", xplm_MapLayer_Fill);
    PyModule_AddIntConstant(mod, "MapLayer_Markings", xplm_MapLayer_Markings);
    PyModule_AddStringConstant(mod, "MAP_USER_INTERFACE", XPLM_MAP_USER_INTERFACE);
    PyModule_AddStringConstant(mod, "MAP_IOS", XPLM_MAP_IOS);
    PyModule_AddIntConstant(mod, "MapOrientation_Map", xplm_MapOrientation_Map);
    PyModule_AddIntConstant(mod, "MapOrientation_UI", xplm_MapOrientation_UI);
  }
  return mod;
}
