#define _GNU_SOURCE 1
#include <Python.h>
#include <sys/time.h>
#include <stdio.h>
#include <string>
#include <unordered_map>
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMMap.h>
#include "plugin_dl.h"
#include "utils.h"
#include "map.h"
#include "capsules.h"

struct MapCallbackInfo {
  const char* module_name;     // MAP_MODULE_NAME
  XPLMMapLayerType layerType;  // MAP_LAYER
  PyObject *deleted;           // MAP_DELETE
  PyObject *prep;              // MAP_PREP
  PyObject *draw;              // MAP_DRAW
  PyObject *icon;              // MAP_ICON
  PyObject *label;             // MAP_LABEL
  int showToggle;              // MAP_TOGGLE
  PyObject *name;              // MAP_NAME
  PyObject *refCon;            // MAP_REFCON
  XPLMMapLayerID layerID;
  intptr_t refcon_id;          // The key used for lookup
};
  
#define MAP_TYPE 0
#define MAP_LAYER 1
#define MAP_DELETE 2
#define MAP_PREP 3
#define MAP_DRAW 4
#define MAP_ICON 5
#define MAP_LABEL 6
#define MAP_TOGGLE 7
#define MAP_NAME 8
#define MAP_REFCON 9

static std::unordered_map<intptr_t, MapCallbackInfo> mapCallbacks;

intptr_t mapCntr = 0;

struct MapCreateInfo {
  PyObject *callback;          // MAPCREATE_CALLBACK
  PyObject *refCon;            // MAPCREATE_REFCON
  const char* module_name;     // MAPCREATE_MODULE_NAME
  intptr_t refcon_id;          // The key used for lookup
};
static std::unordered_map<intptr_t, MapCreateInfo> mapCreateCallbacks;


intptr_t mapCreateCntr;
void resetMap(void) {
  for (const auto& pair : mapCallbacks) {
    const MapCallbackInfo& info = pair.second;
    char *moduleName = const_cast<char*>(info.module_name);
    char *layerName = objToStr(info.name);
    XPLMDestroyMapLayer(info.layerID);  // Use actual layerID, not the map key
    pythonDebug("     Reset --     %s - (%s)", moduleName, layerName);
    free(layerName);
  }

  // Clean up all MapCallbackInfo objects
  for (auto& pair : mapCallbacks) {
    Py_DECREF(pair.second.deleted);
    Py_DECREF(pair.second.prep);
    Py_DECREF(pair.second.draw);
    Py_DECREF(pair.second.icon);
    Py_DECREF(pair.second.label);
    Py_DECREF(pair.second.name);
    Py_DECREF(pair.second.refCon);
  }
  mapCallbacks.clear();

  // Clean up all MapCreateInfo objects
  for (auto& pair : mapCreateCallbacks) {
    Py_DECREF(pair.second.callback);
    Py_DECREF(pair.second.refCon);
  }
  mapCreateCallbacks.clear();
}

static inline void mapCallback(int whichCallback, XPLMMapLayerID inLayer, const float *inMapBoundsLeftTopRightBottom, float zoomRatio,
                        float mapUnitsPerUserInterfaceUnit, XPLMMapStyle mapStyle, XPLMMapProjectionID projection,
                        void *inRefcon)
{
  PyObject *layerObj, *boundsObj, *refconObj, *callback;
  intptr_t refcon_id = (intptr_t)inRefcon;
  auto it = mapCallbacks.find(refcon_id);
  if (it == mapCallbacks.end()) {
    pythonLog("Couldn't find map callback with id = %p., destroying it instead", inRefcon);
    XPLMDestroyMapLayer(inLayer);
    return;
  }

  const MapCallbackInfo& info = it->second;
  set_moduleName(info.module_name);

  if (whichCallback < MAP_DRAW || whichCallback > MAP_LABEL) {
    pythonLog("mapCallback called with bad index %d", whichCallback);
    return;
  }

  // Select the appropriate callback based on whichCallback
  switch (whichCallback) {
    case MAP_DRAW: callback = info.draw; break;
    case MAP_ICON: callback = info.icon; break;
    case MAP_LABEL: callback = info.label; break;
    default:
      pythonLog("mapCallback called with invalid callback type %d", whichCallback);
      return;
  }

  if (callback == Py_None) {
    return;
  }
  layerObj = makeCapsule(inLayer, "XPLMMapLayerID");
  PyObject *mapProjectionCapsule = makeCapsule(projection, "XPLMMapProjectionID");
  refconObj = info.refCon;
  
  boundsObj = PyTuple_New(4);
  //Steals the ref!
  PyTuple_SetItem(boundsObj, 0, PyFloat_FromDouble((double)inMapBoundsLeftTopRightBottom[0]));
  PyTuple_SetItem(boundsObj, 1, PyFloat_FromDouble((double)inMapBoundsLeftTopRightBottom[1]));
  PyTuple_SetItem(boundsObj, 2, PyFloat_FromDouble((double)inMapBoundsLeftTopRightBottom[2]));
  PyTuple_SetItem(boundsObj, 3, PyFloat_FromDouble((double)inMapBoundsLeftTopRightBottom[3]));

  PyObject *zoomRatioObj = PyFloat_FromDouble(zoomRatio);
  PyObject *mapUnitsPerUserInterfaceUnitObj = PyFloat_FromDouble(mapUnitsPerUserInterfaceUnit);
  PyObject *mapStyleObj = PyLong_FromLong(mapStyle);
  PyObject *args[] = {layerObj, boundsObj, zoomRatioObj, mapUnitsPerUserInterfaceUnitObj, mapStyleObj, mapProjectionCapsule, refconObj};
  PyObject *pRes = PyObject_Vectorcall(callback, args, 7, nullptr);
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
  Py_XDECREF(pRes);
}

static inline void genericPrepCallback(XPLMMapLayerID inLayer, const float *inMapBoundsLeftTopRightBottom,
                                           XPLMMapProjectionID projection, void *inRefcon)
{
  PyObject *layerObj, *boundsObj, *refconObj, *callback;
  intptr_t refcon_id = (intptr_t)inRefcon;
  auto it = mapCallbacks.find(refcon_id);
  if (it == mapCallbacks.end()) {
    return;
  }

  const MapCallbackInfo& info = it->second;
  callback = info.prep;
  if (callback == Py_None) {
    return;
  }

  set_moduleName(info.module_name);

  layerObj = makeCapsule(inLayer, "XPLMMapLayerID");
  PyObject *mapProjectionCapsule = makeCapsule(projection, "XPLMMapProjectionID");
  refconObj = info.refCon;
  
  boundsObj = PyTuple_New(4);
  PyTuple_SetItem(boundsObj, 0, PyFloat_FromDouble((double)inMapBoundsLeftTopRightBottom[0]));
  PyTuple_SetItem(boundsObj, 1, PyFloat_FromDouble((double)inMapBoundsLeftTopRightBottom[1]));
  PyTuple_SetItem(boundsObj, 2, PyFloat_FromDouble((double)inMapBoundsLeftTopRightBottom[2]));
  PyTuple_SetItem(boundsObj, 3, PyFloat_FromDouble((double)inMapBoundsLeftTopRightBottom[3]));

  PyObject *args[] = {layerObj, boundsObj, mapProjectionCapsule, refconObj};
  PyObject *pRes = PyObject_Vectorcall(callback, args, 4, nullptr);
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
  Py_XDECREF(pRes);
}

static inline void genericDeleteCallback(XPLMMapLayerID inLayer, void *inRefcon)
{
  PyObject *layerObj, *refconObj, *callback;
  intptr_t refcon_id = (intptr_t)inRefcon;
  auto it = mapCallbacks.find(refcon_id);
  if (it == mapCallbacks.end()) {
    printf("Couldn't find map callback with id = %p.", inRefcon);
    return;
  }

  const MapCallbackInfo& info = it->second;
  callback = info.deleted;
  if (callback == Py_None) {
    return;
  }

  set_moduleName(info.module_name);

  layerObj = makeCapsule(inLayer, "XPLMMapLayerID");
  refconObj = info.refCon;
  
  PyObject *args[] = {layerObj, refconObj};
    PyObject *pRes = PyObject_Vectorcall(callback, args, 2, nullptr);
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
  intptr_t refcon_id = (intptr_t)inRefcon;
  auto it = mapCreateCallbacks.find(refcon_id);
  if (it == mapCreateCallbacks.end()) {
    printf("Couldn't find map created callback with id = %p.", inRefcon);
    return;
  }

  const MapCreateInfo& info = it->second;
  mapTypeObj = PyUnicode_DecodeUTF8(mapType, strlen(mapType), nullptr);
  callback = info.callback;
  refconObj = info.refCon;

  set_moduleName(info.module_name);
  
  PyObject *args[] = {mapTypeObj, refconObj};
    PyObject *pRes = PyObject_Vectorcall(callback, args, 2, nullptr);
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

static void genericDrawCallback(XPLMMapLayerID inLayer, const float *inMapBoundsLeftTopRightBottom, float zoomRatio,
                        float mapUnitsPerUserInterfaceUnit, XPLMMapStyle mapStyle, XPLMMapProjectionID projection,
                        void *inRefcon)
{
  mapCallback(MAP_DRAW, inLayer, inMapBoundsLeftTopRightBottom, zoomRatio,
              mapUnitsPerUserInterfaceUnit, mapStyle, projection, inRefcon);
}

static void genericIconCallback(XPLMMapLayerID inLayer, const float *inMapBoundsLeftTopRightBottom, float zoomRatio,
                        float mapUnitsPerUserInterfaceUnit, XPLMMapStyle mapStyle, XPLMMapProjectionID projection,
                        void *inRefcon)
{
  mapCallback(MAP_ICON, inLayer, inMapBoundsLeftTopRightBottom, zoomRatio,
              mapUnitsPerUserInterfaceUnit, mapStyle, projection, inRefcon);
}

static void genericLabelCallback(XPLMMapLayerID inLayer, const float *inMapBoundsLeftTopRightBottom, float zoomRatio,
                        float mapUnitsPerUserInterfaceUnit, XPLMMapStyle mapStyle, XPLMMapProjectionID projection,
                        void *inRefcon)
{
  mapCallback(MAP_LABEL, inLayer, inMapBoundsLeftTopRightBottom, zoomRatio,
              mapUnitsPerUserInterfaceUnit, mapStyle, projection, inRefcon);
}

// Helper structures and functions for cleaner XPLMCreateMapLayerFun
struct ParsedMapParams {
    PyObject *map, *name, *deleted, *prep, *draw, *icon, *label, *refCon;
    int layerType, showToggle;
    bool freeMap, freeName;

    ParsedMapParams() : map(Py_None), name(Py_None), deleted(Py_None), prep(Py_None),
                       draw(Py_None), icon(Py_None), label(Py_None), refCon(Py_None),
                       layerType(xplm_MapLayer_Markings), showToggle(1),
                       freeMap(false), freeName(false) {}
};

// Helper 1: Parse and validate input arguments
static bool parseMapLayerInputs(PyObject *args, PyObject *kwargs, ParsedMapParams& params) {
    static char *keywords[] = {CHAR("mapType"), CHAR("layerType"), CHAR("delete"), CHAR("prep"),
                              CHAR("draw"), CHAR("icon"), CHAR("label"), CHAR("showToggle"),
                              CHAR("name"), CHAR("refCon"), nullptr};

    PyObject *firstObj = Py_None;
    PyObject *paramsObj = Py_None;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|OiOOOOOiOO", keywords,
                                    &firstObj, &params.layerType, &params.deleted, &params.prep,
                                    &params.draw, &params.icon, &params.label, &params.showToggle,
                                    &params.name, &params.refCon)) {
        return false;
    }

    // Determine if this is tuple format or keyword format
    if (PySequence_Check(firstObj)) {
        if (PySequence_Length(firstObj) > 2) {
            PyObject *item = PySequence_GetItem(firstObj, 1);
            if (PyLong_Check(item)) {
                paramsObj = firstObj;
                if (PySequence_Length(paramsObj) != 10) {
                    PyErr_SetString(PyExc_AttributeError, "createMapLayer tuple did not contain 10 values.\n");
                    Py_XDECREF(item);
                    return false;
                }
            }
            Py_XDECREF(item);
        } else {
            params.map = firstObj;
        }
    } else {
        params.map = firstObj;
    }

    // Handle legacy tuple format
    if (paramsObj != Py_None) {
      // PySequence_GetItem returns NEW references, so we need to handle the transition
      // from default Py_None to the new objects carefully
      
      // For integer fields, get temporary objects to extract values, then clean up
      PyObject *tmp = PySequence_GetItem(paramsObj, MAP_LAYER);
      params.layerType = (int)PyLong_AsLong(tmp);
      Py_DECREF(tmp);
      
      tmp = PySequence_GetItem(paramsObj, MAP_TOGGLE);
      params.showToggle = (int)PyLong_AsLong(tmp);
      Py_DECREF(tmp);
      
      /* because PySequence_GetItem returns NEW objects, we don't need to INCREF */
      params.map = PySequence_GetItem(paramsObj, MAP_TYPE);
      params.name = PySequence_GetItem(paramsObj, MAP_NAME);
      params.deleted = PySequence_GetItem(paramsObj, MAP_DELETE);
      params.prep = PySequence_GetItem(paramsObj, MAP_PREP);
      params.draw = PySequence_GetItem(paramsObj, MAP_DRAW);
      params.icon = PySequence_GetItem(paramsObj, MAP_ICON);
      params.label = PySequence_GetItem(paramsObj, MAP_LABEL);
      params.refCon = PySequence_GetItem(paramsObj, MAP_REFCON);
    } else {
      /* because ParsTupeAndKeyword returns borrowed Python Object, we need to INCREF here */
      Py_INCREF(params.map);
      Py_INCREF(params.name);
      Py_INCREF(params.deleted);
      Py_INCREF(params.prep);
      Py_INCREF(params.draw);
      Py_INCREF(params.icon);
      Py_INCREF(params.label);
      Py_INCREF(params.refCon);
    }

    return true;
}

// Helper 2: Apply defaults and prepare final parameters
static bool applyDefaultsAndPrepare(ParsedMapParams& params) {
    // Apply defaults for keyword arguments
    if (params.map == Py_None) {
        params.map = PyUnicode_DecodeUTF8(XPLM_MAP_USER_INTERFACE, strlen(XPLM_MAP_USER_INTERFACE), nullptr);
        params.freeMap = true;
    }
    if (params.name == Py_None) {
        params.name = PyUnicode_DecodeUTF8(CurrentPythonModuleName, strlen(CurrentPythonModuleName), nullptr);
        params.freeName = false;  // Don't free - will be owned by mapCallbacks
    }

    return true;
}

// Helper 3: Convert to X-Plane API structure
static bool buildXPlaneParams(const ParsedMapParams& params, intptr_t refcon_id,
                             XPLMCreateMapLayer_t& inParams, char*& tmpMap, char*& tmpLayerName,
                             PyObject*& tmpObjMap, PyObject*& tmpObjLayerName) {

    tmpObjMap = PyUnicode_AsUTF8String(params.map);
    if (!tmpObjMap) {
        return false;
    }

    tmpObjLayerName = PyUnicode_AsUTF8String(params.name);
    if (!tmpObjLayerName) {
        Py_DECREF(tmpObjMap);
        return false;
    }

    tmpMap = PyBytes_AsString(tmpObjMap);
    tmpLayerName = PyBytes_AsString(tmpObjLayerName);

    inParams = {
        .structSize = sizeof(inParams),
        .mapToCreateLayerIn = tmpMap,
        .layerType = (XPLMMapLayerType)params.layerType,
        .willBeDeletedCallback = genericDeleteCallback,
        .prepCacheCallback = genericPrepCallback,
        .drawCallback = genericDrawCallback,
        .iconCallback = genericIconCallback,
        .labelCallback = genericLabelCallback,
        .showUiToggle = params.showToggle,
        .layerName = tmpLayerName,
        .refcon = (void *)refcon_id,
    };

    return true;
}

// Helper 4: Build callback info structure
static MapCallbackInfo buildCallbackInfo(const ParsedMapParams& params, intptr_t refcon_id) {
    return MapCallbackInfo {
        .module_name = CurrentPythonModuleName,
        .layerType = (XPLMMapLayerType)params.layerType,
        .deleted = params.deleted,
        .prep = params.prep,
        .draw = params.draw,
        .icon = params.icon,
        .label = params.label,
        .showToggle = params.showToggle,
        .name = params.name,
        .refCon = params.refCon,
        .refcon_id = refcon_id,
    };
}

// Helper 5: Clean up allocated resources
static void cleanupMapParams(const ParsedMapParams& params) {
    Py_DECREF(params.map);  // Always free - not stored in mapCallbacks
    // params.name is owned by mapCallbacks, so don't DECREF (freeName is always false)
}

My_DOCSTR(_createMapLayer__doc__, "createMapLayer",
          "mapType=MAP_USER_INTERFACE, layerType=MapLayer_Markings, delete=None, prep=None, draw=None, icon=None, label=None, showToggle=1, name='', refCon=None",
          "mapType:str=MAP_USER_INTERFACE, "
          "layerType:XPLMMapLayerType=MapLayer_Markings, "
          "delete:Optional[Callable[[XPLMMapLayerID, Any], None]]=None, "
          "prep:Optional[Callable[[XPLMMapLayerID, float, XPLMMapProjectionID, Any], None]]=None, "
          "draw:Optional[Callable[[XPLMMapLayerID, float, float, float, XPLMMapStyle, XPLMMapProjectionID, Any], None]]=None, "
          "icon:Optional[Callable[[XPLMMapLayerID, float, float, float, XPLMMapStyle, XPLMMapProjectionID, Any], None]]=None, "
          "label:Optional[Callable[[XPLMMapLayerID, float, float, float, XPLMMapStyle, XPLMMapProjectionID, Any], None]]=None, "
          "showToggle:int=1, "
          "name:str=<module_name>', "
          "refCon:Any=None",
          "XPLMMapLayerID",
          "Returns layerID of newly created map layer, setting callbacks.\n"
          "\n"
          "If map does not currently exist, returns 0.");
static PyObject *XPLMCreateMapLayerFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
    (void) self;

    if (!XPLMCreateMapLayer_ptr) {
        PyErr_SetString(PyExc_RuntimeError, "XPLMCreateMapLayer is available only in XPLM300 and up.");
        return nullptr;
    }

    // Step 1: Parse and validate inputs
    ParsedMapParams params;
    if (!parseMapLayerInputs(args, kwargs, params)) {
        return nullptr;
    }

    // Step 2: Apply defaults and prepare
    if (!applyDefaultsAndPrepare(params)) {
        cleanupMapParams(params);
        return nullptr;
    }

    // Step 3: Build structures for X-Plane API
    intptr_t refcon_id = ++mapCntr;
    XPLMCreateMapLayer_t inParams;
    char *tmpMap, *tmpLayerName;
    PyObject *tmpObjMap, *tmpObjLayerName;

    if (!buildXPlaneParams(params, refcon_id, inParams, tmpMap, tmpLayerName, tmpObjMap, tmpObjLayerName)) {
        cleanupMapParams(params);
        return nullptr;
    }

    MapCallbackInfo callbackInfo = buildCallbackInfo(params, refcon_id);

    // Step 4: Call X-Plane API
    mapCallbacks[refcon_id] = callbackInfo;

    XPLMMapLayerID mapLayerID = XPLMCreateMapLayer_ptr(&inParams);
    mapCallbacks[refcon_id].layerID = mapLayerID;

    // Step 5: Handle result
    if (!mapLayerID) {
        char msg[1024];
        if (!XPLMMapExists(inParams.mapToCreateLayerIn)) {
            snprintf(msg, sizeof(msg), "Map [%s] does not (yet) exist.\n", inParams.mapToCreateLayerIn);
            PyErr_SetString(PyExc_RuntimeError, msg);
        } else {
            snprintf(msg, sizeof(msg), "Unknown map creation error.\n");
            PyErr_SetString(PyExc_ValueError, msg);
        }
        // Clean up string conversions
        Py_DECREF(tmpObjMap);
        Py_DECREF(tmpObjLayerName);
        cleanupMapParams(params);
        // Clean up PyObject* stored in mapCallbacks before erasing
        MapCallbackInfo& info = mapCallbacks[refcon_id];
        Py_DECREF(info.deleted);
        Py_DECREF(info.prep);
        Py_DECREF(info.draw);
        Py_DECREF(info.icon);
        Py_DECREF(info.label);
        Py_DECREF(info.name);
        Py_DECREF(info.refCon);
        mapCallbacks.erase(refcon_id);
        return nullptr;
    }

    // Step 6: Store callback info and return success
    // Clean up temporary string conversions
    Py_DECREF(tmpObjMap);
    Py_DECREF(tmpObjLayerName);
    cleanupMapParams(params);

    return makeCapsule(mapLayerID, "XPLMMapLayerID");
}

My_DOCSTR(_destroyMapLayer__doc__, "destroyMapLayer",
          "layerID",
          "layerID:XPLMMapLayerID",
          "int",
          "Destroys map layer given by layerID.");
static PyObject *XPLMDestroyMapLayerFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("layerID"), nullptr};
  (void) self;
  PyObject *layer;

  if(!XPLMDestroyMapLayer_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMDestroyMapLayer is available only in XPLM300 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &layer)){
    return nullptr;
  }

  XPLMMapLayerID inLayer = getVoidPtr(layer, "XPLMMapLayerID");
  int res = XPLMDestroyMapLayer_ptr(inLayer);
  if(res){
    for (auto it=mapCallbacks.begin(); it != mapCallbacks.end() ;) {
      MapCallbackInfo& info = it->second;
      if(inLayer == info.layerID) {
        Py_DECREF(info.deleted);
        Py_DECREF(info.prep);
        Py_DECREF(info.draw);
        Py_DECREF(info.icon);
        Py_DECREF(info.label);
        Py_DECREF(info.name);
        Py_DECREF(info.refCon);
        it = mapCallbacks.erase(it);
        break;
      } else {
        ++ it;
      }
    }
  }

  return PyLong_FromLong(res);
}

My_DOCSTR(_registerMapCreationHook__doc__, "registerMapCreationHook",
          "mapCreated, refCon=None",
          "mapCreated:Callable[[str, Any], None], refCon:Any=None",
          "None",
          "Registers mapCreated() callback to notify you when a map is created.\n"
          "\n"
          "Callback gets two parameters: (mapType, refCon)");
static PyObject *XPLMRegisterMapCreationHookFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("mapCreated"), CHAR("refCon"), nullptr};
  (void) self;
  PyObject *callback, *inRefCon=Py_None;
  if(!XPLMRegisterMapCreationHook_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMRegisterMapCreationHook is available only in XPLM300 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|O", keywords, &callback, &inRefCon)){
    return nullptr;
  }

  intptr_t refcon_id = ++mapCreateCntr;

  // Create MapCreateInfo struct
  MapCreateInfo createInfo;
  createInfo.callback = callback;
  createInfo.refCon = inRefCon;
  createInfo.module_name = CurrentPythonModuleName;
  createInfo.refcon_id = refcon_id;

  // Increment reference counts for PyObject fields since we're storing them
  Py_INCREF(createInfo.callback);
  Py_INCREF(createInfo.refCon);

  mapCreateCallbacks[refcon_id] = createInfo;

  XPLMRegisterMapCreationHook_ptr(mapCreatedCallback, (void*)refcon_id);
  Py_RETURN_NONE;
}

My_DOCSTR(_mapExists__doc__, "mapExists",
          "mapType",
          "mapType:str",
          "int",
          "Returns 1 if mapType exists, 0 otherwise."
          "\n"
          "mapType is either xp.MAP_USER_INTERFACE or xp.MAP_IOS");
static PyObject *XPLMMapExistsFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("mapType"), nullptr};
  (void) self;
  const char *mapType;

  if(!XPLMMapExists_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMMapExists is available only in XPLM300 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "s", keywords, &mapType)){
    return nullptr;
  }
  int res = XPLMMapExists_ptr(mapType);
  return PyLong_FromLong(res);
}

My_DOCSTR(_drawMapIconFromSheet__doc__, "drawMapIconFromSheet",
          "layerID, png, s, t, ds, dt, x, y, orientation, rotationDegrees, mapWidth",
          "layerID:XPLMMapLayerID, png:str, s:int, t:int, ds:int, dt:int, x:float, y:float, orientation:XPLMMapOrientation, rotationDegrees:float, mapWidth:float",
          "None",
          "Draws icon into map layer.\n"
          "\n"
          "Only valid within iconLayer() callback.");
static PyObject *XPLMDrawMapIconFromSheetFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("layerID"), CHAR("png"), CHAR("s"), CHAR("t"), CHAR("ds"), CHAR("dt"), CHAR("x"), CHAR("y"), CHAR("orientation"), CHAR("rotationDegrees"), CHAR("mapWidth"), nullptr};
  (void) self;
  PyObject *layerObj;
  const char *inPngPath;
  int s, t, ds, dt;
  float mapX, mapY, rotationDegrees, mapWidth;
  XPLMMapOrientation orientation;

  if(!XPLMDrawMapIconFromSheet_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMDrawMapIconFromSheet is available only in XPLM300 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Osiiiiffiff", keywords, &layerObj, &inPngPath, &s, &t, &ds, &dt, &mapX, &mapY,
                       &orientation, &rotationDegrees, &mapWidth)){
    return nullptr;
  }
  XPLMMapLayerID layer = getVoidPtr(layerObj, "XPLMMapLayerID");
  XPLMDrawMapIconFromSheet_ptr(layer, inPngPath, s, t, ds, dt, mapX, mapY,
                       orientation, rotationDegrees, mapWidth);
  Py_RETURN_NONE;
}

My_DOCSTR(_drawMapLabel__doc__, "drawMapLabel",
          "layerID, text, x, y, orientation, rotationDegrees",
          "layerID:XPLMMapLayerID, text:str, x:float, y:float, orientation:XPLMMapOrientation, rotationDegrees:float",
          "None",
          "Draws label within map layer.\n"
          "\n"
          "Only valid within labelLayer() callback.");
static PyObject *XPLMDrawMapLabelFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("layerID"), CHAR("text"), CHAR("x"), CHAR("y"), CHAR("orientation"), CHAR("rotationDegrees"), nullptr};
  (void) self;
  PyObject *layerObj;
  const char *inText;
  float mapX, mapY, rotationDegrees;
  XPLMMapOrientation orientation;

  if(!XPLMDrawMapLabel_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMDrawMapLabel is available only in XPLM300 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Osffif", keywords, &layerObj, &inText, &mapX, &mapY,
                       &orientation, &rotationDegrees)){
    return nullptr;
  }
  XPLMMapLayerID layer = getVoidPtr(layerObj, "XPLMMapLayerID");
  XPLMDrawMapLabel_ptr(layer, inText, mapX, mapY, orientation, rotationDegrees);
  Py_RETURN_NONE;
}

My_DOCSTR(_mapProject__doc__, "mapProject",
          "projection, latitude, longitude",
          "projection:XPLMMapProjectionID, latitude:float, longitude:float",
          "None | Tuple[float, float]",
          "Returns map layer (x, y) for given latitude, longitude.\n"
          "\n"
          "Only valid within map layer callbacks.");
static PyObject *XPLMMapProjectFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("projection"), CHAR("latitude"), CHAR("longitude"), CHAR("x"), CHAR("y"), nullptr};
  (void) self;
  PyObject *projectionObj, *outX, *outY;
  double latitude, longitude;
  int returnValues = 0;
  if(!XPLMMapProject_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMMapProject is available only in XPLM300 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "OddOO", keywords, &projectionObj, &latitude, &longitude, &outX, &outY)) {
    PyErr_Clear();
    returnValues = 1;
    static char *nkeywords[] = {CHAR("projection"), CHAR("latitude"), CHAR("longitude"), nullptr};
    if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Odd", nkeywords, &projectionObj, &latitude, &longitude)) {
      return nullptr;
    }
  }
  XPLMMapProjectionID projection = getVoidPtr(projectionObj, "XPLMMapProjectionID");
  float x, y;
  XPLMMapProject_ptr(projection, latitude, longitude, &x, &y);
  if (returnValues) {
    return Py_BuildValue("(ff)", x, y);
  }
  pythonLogWarning("XPLMMapProject no longer require final (x, y) parameters");

  if (outX != Py_None) {
    PyObject *floatObj = PyFloat_FromDouble((double) x);
    PyList_Append(outX, floatObj);
    Py_DECREF(floatObj);
  }
  if (outY != Py_None) {
    PyObject *floatObj = PyFloat_FromDouble((double) y);
    PyList_Append(outY, floatObj);
    Py_DECREF(floatObj);
  }
    
  Py_RETURN_NONE;
}

My_DOCSTR(_mapUnproject__doc__, "mapUnproject",
          "projection, x, y",
          "projection:XPLMMapProjectionID, x:float, y:float",
          "None | Tuple[float, float]",
          "Returns latitude, longitude for given map coordinates.\n"
          "\n"
          "Only valid within map layer callbacks.");
static PyObject *XPLMMapUnprojectFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("projection"), CHAR("x"), CHAR("y"), CHAR("latitude"), CHAR("longitude"), nullptr};
  (void) self;
  PyObject *projectionObj, *outLatitude, *outLongitude;
  float mapX, mapY;
  int returnValues = 0;

  if(!XPLMMapUnproject_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMMapUnproject is available only in XPLM300 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "OffOO", keywords, &projectionObj, &mapX, &mapY, &outLatitude, &outLongitude)){
    PyErr_Clear();
    returnValues = 1;
    static char *nkeywords[] = {CHAR("projection"), CHAR("x"), CHAR("y"), nullptr};
    if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Off", nkeywords, &projectionObj, &mapX, &mapY)){
      return nullptr;
    }
  }
  XPLMMapProjectionID projection = getVoidPtr(projectionObj, "XPLMMapProjectionID");
  double longitude, latitude;
  XPLMMapUnproject_ptr(projection, mapX, mapY, &latitude, &longitude);
  if (returnValues) 
    return Py_BuildValue("(dd)", latitude, longitude);
  pythonLogWarning("XPLMMapUnproject no longer requires final latitude, longitude parameters.");
  if (outLatitude != Py_None) {
    PyObject *floatObj = PyFloat_FromDouble(latitude);
    PyList_Append(outLatitude, floatObj);
    Py_DECREF(floatObj);
  }
  if (outLongitude != Py_None) {
    PyObject *floatObj = PyFloat_FromDouble(longitude);
    PyList_Append(outLongitude, floatObj);
    Py_DECREF(floatObj);
  }
    
  Py_RETURN_NONE;
}

My_DOCSTR(_mapScaleMeter__doc__, "mapScaleMeter",
          "projection, x, y",
          "projection:XPLMMapProjectionID, x:float, y:float",
          "float",
          "Returns number of units for 'one meter' using current projection.\n"
          "\n"
          "Only valid within map layer callbacks.");
static PyObject *XPLMMapScaleMeterFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("projection"), CHAR("x"), CHAR("y"), nullptr};
  (void) self;
  PyObject *projectionObj;
  float mapX, mapY;

  if(!XPLMMapScaleMeter_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMMapScaleMeter is available only in XPLM300 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Off", keywords, &projectionObj, &mapX, &mapY)){
    return nullptr;
  }
  XPLMMapProjectionID projection = getVoidPtr(projectionObj, "XPLMMapProjectionID");
  float res = XPLMMapScaleMeter_ptr(projection, mapX, mapY);
  return PyFloat_FromDouble(res);
}

My_DOCSTR(_mapGetNorthHeading__doc__, "mapGetNorthHeading",
          "projection, x, y",
          "projection:XPLMMapProjectionID, x:float, y:float",
          "float",
          "Returns mapping angle for map projection at point.\n"
          "\n"
          "Only valid within map layer callbacks.");
static PyObject *XPLMMapGetNorthHeadingFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("projection"), CHAR("x"), CHAR("y"), nullptr};
  (void) self;
  PyObject *projectionObj;
  float mapX, mapY;

  if(!XPLMMapGetNorthHeading_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMMapGetNorthHeading is available only in XPLM300 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Off", keywords, &projectionObj, &mapX, &mapY)){
    return nullptr;
  }
  XPLMMapProjectionID projection = getVoidPtr(projectionObj, "XPLMMapProjectionID");
  float res = XPLMMapGetNorthHeading_ptr(projection, mapX, mapY);
  return PyFloat_FromDouble(res);
}

PyObject* buildMapCallbackDict(void)
{
  PyObject *mapCallbacksDict = PyDict_New();

  for (const auto& pair : mapCallbacks) {
    intptr_t key = pair.first;
    const MapCallbackInfo& info = pair.second;

    // Create 11-element tuple containing all MapCallbackInfo elements
    PyObject *tuple = PyTuple_New(11);
    PyTuple_SetItem(tuple, 0, PyUnicode_FromString(info.module_name));       // module_name
    PyTuple_SetItem(tuple, 1, PyLong_FromLong(info.layerType));                      // layerType

    // Python callback objects - PyTuple_SetItem steals reference, so increment to keep ours
    Py_INCREF(info.deleted);
    PyTuple_SetItem(tuple, 2, info.deleted);                                         // deleted
    Py_INCREF(info.prep);
    PyTuple_SetItem(tuple, 3, info.prep);                                            // prep
    Py_INCREF(info.draw);
    PyTuple_SetItem(tuple, 4, info.draw);                                            // draw
    Py_INCREF(info.icon);
    PyTuple_SetItem(tuple, 5, info.icon);                                            // icon
    Py_INCREF(info.label);
    PyTuple_SetItem(tuple, 6, info.label);                                           // label

    PyTuple_SetItem(tuple, 7, PyLong_FromLong(info.showToggle));                     // showToggle
    Py_INCREF(info.name);
    PyTuple_SetItem(tuple, 8, info.name);                                            // name
    Py_INCREF(info.refCon);
    PyTuple_SetItem(tuple, 9, info.refCon);                                          // refCon
    PyTuple_SetItem(tuple, 10, makeCapsule(info.layerID, "XPLMMapLayerID"));         // layerID

    // Note: refcon_id is not included as it's used as the dictionary key

    // Add to dictionary with refcon_id as key
    PyObject *py_key = PyLong_FromLong(key);
    PyDict_SetItem(mapCallbacksDict, py_key, tuple);
    Py_DECREF(py_key);
    Py_DECREF(tuple);
  }

  return mapCallbacksDict;
}

static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;

  // Clean up map callbacks
  for (auto& pair : mapCallbacks) {
    Py_DECREF(pair.second.deleted);
    Py_DECREF(pair.second.prep);
    Py_DECREF(pair.second.draw);
    Py_DECREF(pair.second.icon);
    Py_DECREF(pair.second.label);
    Py_DECREF(pair.second.name);
    Py_DECREF(pair.second.refCon);
  }
  mapCallbacks.clear();

  // Clean up map creation callbacks
  for (auto& pair : mapCreateCallbacks) {
    Py_DECREF(pair.second.callback);
    Py_DECREF(pair.second.refCon);
  }
  mapCreateCallbacks.clear();

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
  {"getMapCallbackDict", (PyCFunction)buildMapCallbackDict, METH_VARARGS, "Copy of internal MapCallbackInfo"},
  {"_cleanup", cleanup, METH_VARARGS, ""},
  {nullptr, nullptr, 0, nullptr}
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
  nullptr,
  nullptr,
  nullptr,
  nullptr
};

PyMODINIT_FUNC
PyInit_XPLMMap(void)
{
  PyObject *mod = PyModule_Create(&XPLMMapModule);
  if(mod){
    PyModule_AddStringConstant(mod, "__author__", "Peter Buckner (pbuck@xppython3.org)");
    PyModule_AddIntConstant(mod, "xplm_MapStyle_VFR_Sectional", xplm_MapStyle_VFR_Sectional); // XPLMMapStyle
    PyModule_AddIntConstant(mod, "xplm_MapStyle_IFR_LowEnroute", xplm_MapStyle_IFR_LowEnroute); // XPLMMapStyle
    PyModule_AddIntConstant(mod, "xplm_MapStyle_IFR_HighEnroute", xplm_MapStyle_IFR_HighEnroute); // XPLMMapStyle
    PyModule_AddIntConstant(mod, "xplm_MapLayer_Fill", xplm_MapLayer_Fill); // XPLMMapLayerType
    PyModule_AddIntConstant(mod, "xplm_MapLayer_Markings", xplm_MapLayer_Markings); // XPLMMapLayerType
    PyModule_AddStringConstant(mod, "XPLM_MAP_USER_INTERFACE", XPLM_MAP_USER_INTERFACE);
    PyModule_AddStringConstant(mod, "XPLM_MAP_IOS", XPLM_MAP_IOS);
    PyModule_AddIntConstant(mod, "xplm_MapOrientation_Map", xplm_MapOrientation_Map); // XPLMMapOrientation
    PyModule_AddIntConstant(mod, "xplm_MapOrientation_UI", xplm_MapOrientation_UI); // XPLMMapOrientation

    PyModule_AddIntConstant(mod, "MapStyle_VFR_Sectional", xplm_MapStyle_VFR_Sectional); // XPLMMapStyle
    PyModule_AddIntConstant(mod, "MapStyle_IFR_LowEnroute", xplm_MapStyle_IFR_LowEnroute); // XPLMMapStyle
    PyModule_AddIntConstant(mod, "MapStyle_IFR_HighEnroute", xplm_MapStyle_IFR_HighEnroute); // XPLMMapStyle
    PyModule_AddIntConstant(mod, "MapLayer_Fill", xplm_MapLayer_Fill); // XPLMMapLayerType
    PyModule_AddIntConstant(mod, "MapLayer_Markings", xplm_MapLayer_Markings); // XPLMMapLayerType
    PyModule_AddStringConstant(mod, "MAP_USER_INTERFACE", XPLM_MAP_USER_INTERFACE);
    PyModule_AddStringConstant(mod, "MAP_IOS", XPLM_MAP_IOS);
    PyModule_AddIntConstant(mod, "MapOrientation_Map", xplm_MapOrientation_Map); // XPLMMapOrientation
    PyModule_AddIntConstant(mod, "MapOrientation_UI", xplm_MapOrientation_UI); // XPLMMapOrientation
  }
  return mod;
}
