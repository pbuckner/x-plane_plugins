#define _GNU_SOURCE 1
#include <Python.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMDataAccess.h>
#include <XPLM/XPLMUtilities.h>
#include "utils.h"
#include "data_access.h"
#include "xppythontypes.h"
#include "capsules.h"
#include "plugin_dl.h"

struct SharedInfo {
  const char* module_name;
  std::string name;
  XPLMDataTypeID data_type;
  PyObject *callback;
  PyObject *refCon;
};

static std::unordered_map<intptr_t, SharedInfo> sharedCallbacks;
static void genericSharedDataChanged(void *inRefcon);

static intptr_t sharedCntr = 0;

struct DataRefInfo {
  const char* module_name;
  std::string data_name;
  XPLMDataTypeID data_type;
  int is_writable;
  PyObject *read_int;
  PyObject *write_int;
  PyObject *read_float;
  PyObject *write_float;
  PyObject *read_double;
  PyObject *write_double;
  PyObject *read_int_array;
  PyObject *write_int_array;
  PyObject *read_float_array;
  PyObject *write_float_array;
  PyObject *read_data;
  PyObject *write_data;
  PyObject *read_refCon;
  PyObject *write_refCon;
  XPLMDataRef dataRef;
};
static std::unordered_map<intptr_t, DataRefInfo> accessorCallbacks;
static intptr_t accessorCntr = 0;

void resetDataRefs(void) {
  for (auto& pair: accessorCallbacks) {
    const DataRefInfo& info = pair.second;
    XPLMUnregisterDataAccessor(info.dataRef);
    pythonDebug("     Reset --     %s - (%s)", info.module_name, info.data_name.c_str());

    // Clean up Python object references
    Py_DECREF(info.read_int);
    Py_DECREF(info.write_int);
    Py_DECREF(info.read_float);
    Py_DECREF(info.write_float);
    Py_DECREF(info.read_double);
    Py_DECREF(info.write_double);
    Py_DECREF(info.read_int_array);
    Py_DECREF(info.write_int_array);
    Py_DECREF(info.read_float_array);
    Py_DECREF(info.write_float_array);
    Py_DECREF(info.read_data);
    Py_DECREF(info.write_data);
    Py_DECREF(info.read_refCon);
    Py_DECREF(info.write_refCon);
  }
  errCheck("post while resetdataref");
  accessorCallbacks.clear();
  errCheck("post reset dataref");

  for (auto& it : sharedCallbacks) {
    SharedInfo& info = it.second;
    pythonDebug("     Reset --     %s - shared (%s)", info.module_name, info.name.c_str());
    int ret = XPLMUnshareData(info.name.c_str(), info.data_type, genericSharedDataChanged, (void*)it.first);
    if (!ret) {
      pythonLog("***** failed to find data to unshare!!");
    }

    // Clean up Python object references
    Py_DECREF(info.callback);
    Py_DECREF(info.refCon);
  }
  errCheck("post while reset shared data");
  sharedCallbacks.clear();
}

static inline XPLMDataRef drefFromObj(PyObject *obj)
{
  /* provides error checking for valid (i.e., not unregistered) dataRef */
  XPLMDataRef ret = (XPLMDataRef)getVoidPtr(obj, "XPLMDataRef");
  if (! ret) {
    PyErr_SetString(PyExc_ValueError, "invalid dataRef");
  }
  return ret;
}

My_DOCSTR(_findDataRef__doc__, "findDataRef",
          "name",
          "name:str",
          "XPLMDataRef",
          "Looks up string name of data ref and returns dataRef code\n"
          "to be used with get and set data ref functions,\n"
          "or None, if name cannot be found.");
static PyObject *XPLMFindDataRefFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  (void) self;
  static char *keywords[] = {CHAR("name"), nullptr};
  const char *inDataRefName;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "s", keywords, &inDataRefName)){
    return nullptr;
  }
  XPLMDataRef ref = XPLMFindDataRef(inDataRefName);
  if(ref){
    return makeCapsule(ref, "XPLMDataRef");
  }else{
    Py_RETURN_NONE;
  }
}

My_DOCSTR(_canWriteDataRef__doc__, "canWriteDataRef",
          "dataRef",
          "dataRef:XPLMDataRef",
          "bool",
          "Returns True if dataRef is writable, False otherwise. Also\n"
          "returns False if provided dataRef is None.");
static PyObject *XPLMCanWriteDataRefFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("dataRef"), nullptr};
 (void) self;
  PyObject *dataRef;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &dataRef)){
    return nullptr;
  }
  XPLMDataRef inDataRef = drefFromObj(dataRef);
  if(XPLMCanWriteDataRef(inDataRef)){
    Py_RETURN_TRUE;
  }else{
    Py_RETURN_FALSE;
  }
}

My_DOCSTR(_isDataRefGood__doc__, "isDataRefGood",
          "dataRef",
          "dataRef:XPLMDataRef",
          "bool",
          "(Deprecated, do not use.)");
static PyObject *XPLMIsDataRefGoodFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("dataRef"), nullptr};
  (void) self;
  PyObject *dataRef;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &dataRef)){
    return nullptr;
  }
  XPLMDataRef inDataRef = drefFromObj(dataRef);
  if (XPLMIsDataRefGood(inDataRef)){
    Py_RETURN_TRUE;
  } else {
    Py_RETURN_FALSE;
  }
}

My_DOCSTR(_getDataRefTypes__doc__, "getDataRefTypes",
          "dataRef",
          "dataRef:XPLMDataRef",
          "int",
          "Returns or'd values of data type(s) supported by dataRef.\n"
          "   1 Type_Int\n"
          "   2 Type_Float\n"
          "   4 Type_Double\n"
          "   8 Type_FloatArray\n"
          "  16 Type_IntArray\n"
          "  32 Type_Data\n");
static PyObject *XPLMGetDataRefTypesFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("dataRef"), nullptr};
  (void) self;
  PyObject *dataRef;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &dataRef)){
    return nullptr;
  }
  XPLMDataRef inDataRef = drefFromObj(dataRef);
  return PyLong_FromLong(XPLMGetDataRefTypes(inDataRef));
}

My_DOCSTR(_getDatai__doc__, "getDatai",
          "dataRef",
          "dataRef:XPLMDataRef",
          "int",
          "Returns integer value for dataRef.");
static PyObject *XPLMGetDataiFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("dataRef"), nullptr};
  (void) self;
  PyObject *dataRef;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &dataRef)){
    return nullptr;
  }
  XPLMDataRef inDataRef = drefFromObj(dataRef);
  return PyLong_FromLong(XPLMGetDatai(inDataRef));
}

My_DOCSTR(_setDatai__doc__, "setDatai",
          "dataRef, value=0",
          "dataRef:XPLMDataRef, value:int=0",
          "None",
          "Sets integer value for dataRef.");
static PyObject *XPLMSetDataiFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("dataRef"), CHAR("value"), nullptr};
  (void) self;
  PyObject *dataRef;
  int inValue = 0;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|i", keywords, &dataRef, &inValue)){
    return nullptr;
  }
  XPLMDataRef inDataRef = drefFromObj(dataRef);
  XPLMSetDatai(inDataRef, inValue);
  Py_RETURN_NONE;
}

My_DOCSTR(_getDataf__doc__, "getDataf",
          "dataRef",
          "dataRef:XPLMDataRef",
          "float",
          "Returns float value for dataRef.");
static PyObject *XPLMGetDatafFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("dataRef"), nullptr};
  (void) self;
  PyObject *dataRef;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &dataRef)){
    return nullptr;
  }
  XPLMDataRef inDataRef = drefFromObj(dataRef);
  return PyFloat_FromDouble(XPLMGetDataf(inDataRef));
}

My_DOCSTR(_setDataf__doc__, "setDataf",
          "dataRef, value=0.0",
          "dataRef:XPLMDataRef, value:float=0.0",
          "None",
          "Sets float value for dataRef.");
static PyObject *XPLMSetDatafFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("dataRef"), CHAR("value"), nullptr};
  (void) self;
  PyObject *dataRef;
  float inValue = 0.0;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|f", keywords, &dataRef, &inValue)){
    return nullptr;
  }
  XPLMDataRef inDataRef = drefFromObj(dataRef);
  XPLMSetDataf(inDataRef, inValue);
  Py_RETURN_NONE;
}

My_DOCSTR(_getDatad__doc__, "getDatad",
          "dataRef",
          "dataRef:XPLMDataRef",
          "float",
          "Returns double value for dataRef (as a python float)");
static PyObject *XPLMGetDatadFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("dataRef"), nullptr};
  (void) self;
  PyObject *dataRef;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &dataRef)){
    return nullptr;
  }
  XPLMDataRef inDataRef = drefFromObj(dataRef);
  return PyFloat_FromDouble(XPLMGetDatad(inDataRef));
}

My_DOCSTR(_setDatad__doc__, "setDatad",
          "dataRef, value=0.0",
          "dataRef:XPLMDataRef, value:float=0.0",
          "None",
          "Sets double value for dataRef.");
static PyObject *XPLMSetDatadFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("dataRef"), CHAR("value"), nullptr};
  (void) self;
  PyObject *dataRef;
  double inValue = 0.0;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|d", keywords, &dataRef, &inValue)){
    return nullptr;
  }
  XPLMDataRef inDataRef = drefFromObj(dataRef);
  XPLMSetDatad(inDataRef, inValue);
  Py_RETURN_NONE;
}

My_DOCSTR(_getDatavi__doc__, "getDatavi",
          "dataRef, values=None, offset=0, count=-1",
          "dataRef:XPLMDataRef, values:Optional[list[int]]=None, offset:int=0, count:int=-1",
          "int",
          "Get integer array value for dataRef.\n"
          "\n"
          "If values is None, return number of elements in the array (only).\n"
          "Otherwise, values should be a list into which will be copied elements\n"
          "from the dataRef, starting at offset, and continuing for count # of elements.\n"
          "If count is negative, or unspecified, all elements (relative offset) are copied.\n"
          "\n"
          "Returns the number of elements copied.");
static PyObject *XPLMGetDataviFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("dataRef"), CHAR("values"), CHAR("offset"), CHAR("count"), nullptr};
  (void) self;
  PyObject *drefObj;
  PyObject *outValuesObj=Py_None;
  int *outValues = nullptr;
  int inOffset=0, inMax=-1;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|Oii", keywords, &drefObj, &outValuesObj, &inOffset, &inMax)){
    return nullptr;
  }
  XPLMDataRef inDataRef= drefFromObj(drefObj);
  if(outValuesObj && (outValuesObj != Py_None)){
    if(!PyList_Check(outValuesObj)){
      PyErr_SetString(PyExc_TypeError, "getDatavi expects list or None as the values parameter.");
      return nullptr;
    }
    if (inMax <= 0) {
      inMax = XPLMGetDatavi(inDataRef, nullptr, 0, 0);
    }
    if(inMax > 0){
      outValues = (int *)malloc(inMax * sizeof(int));
    }else{
      PyErr_SetString(PyExc_RuntimeError, "getDatavi count value must be positive.");
      return nullptr;
    }
  }
  
  int res = XPLMGetDatavi(inDataRef, outValues, inOffset, inMax);
  if(outValues != nullptr){
    if(PyList_Size(outValuesObj) > 0){
      PySequence_DelSlice(outValuesObj, 0, PyList_Size(outValuesObj));
    }
    //If res < inMax, copy only res elements;
    //  if inMax < res, copy only inMax elements
    for(int i = 0; i < (res < inMax ? res : inMax); ++i){
      PyObject *tmp = PyLong_FromLong(outValues[i]);
      PyList_Append(outValuesObj, tmp);
      Py_DECREF(tmp);
    }
    free(outValues);
  }
  return PyLong_FromLong(res);
}

My_DOCSTR(_setDatavi__doc__, "setDatavi",
          "dataRef, values, offset=0, count=-1",
          "dataRef:XPLMDataRef, values:list[int], offset:int=0, count:int=-1",
          "None",
          "Set integer array value for dataRef.\n"
          "\n"
          "values is a list of integers, to be written into dataRef starting\n"
          "at offset. Up to count values are written.\n"
          "\n"
          "If count is negative (or not provided), all values in the list are copied.\n"
          "It is an error for count to be greater than the length of the list.\n"
          "\n"
          "No return value.");
static PyObject *XPLMSetDataviFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("dataRef"), CHAR("values"), CHAR("offset"), CHAR("count"), nullptr};
  (void) self;
  PyObject *drefObj;
  PyObject *inValuesObj;
  int *inValues = nullptr;
  int inOffset=0, inCount=-1;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "OO|ii", keywords, &drefObj, &inValuesObj, &inOffset, &inCount)){
    return nullptr;
  }
  XPLMDataRef inDataRef= drefFromObj(drefObj);
  if(!PySequence_Check(inValuesObj)){
    PyErr_SetString(PyExc_TypeError, "setDatavi expects list as the values parameter.");
    return nullptr;
  }
  if(inCount > 0 && PySequence_Length(inValuesObj) < inCount){
    PyErr_SetString(PyExc_RuntimeError, "setDatavi list too short for provided count.");
    return nullptr;
  }
  if (inCount <= 0) {
    inCount = PySequence_Length(inValuesObj);
  }

  inValues = (int *)malloc(inCount * sizeof(int));
  /* PyObject *tup = PySequence_Tuple(inValuesObj); */
  for(int i = 0; i < inCount; ++i){
    PyObject *f = PySequence_GetItem(inValuesObj, i);
    inValues[i] = PyLong_AsLong(f);
    Py_DECREF(f);
  }
  /* Py_DECREF(tup); */
  
  XPLMSetDatavi(inDataRef, inValues, inOffset, inCount);
  free(inValues);
  Py_RETURN_NONE;
}

My_DOCSTR(_getDatavf__doc__, "getDatavf",
          "dataRef, values=None, offset=0, count=-1",
          "dataRef:XPLMDataRef, values:Optional[list[float]]=None, offset:int=0, count:int=-1",
          "int",
          "Get float array value for dataRef.\n"
          "\n"
          "If values is None, return number of elements in the array (only).\n"
          "Otherwise, values should be a list into which will be copied elements\n"
          "from the dataRef, starting at offset, and continuing for count # of elements.\n"
          "If count is negative, or unspecified, all elements (relative offset) are copied.\n"
          "\n"
          "Returns the number of elements copied.");
static PyObject *XPLMGetDatavfFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("dataRef"), CHAR("values"), CHAR("offset"), CHAR("count"), nullptr};
  (void) self;
  PyObject *drefObj;
  PyObject *outValuesObj=Py_None;
  float *outValues = nullptr;
  int inOffset=0, inMax=-1;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|Oii", keywords, &drefObj, &outValuesObj, &inOffset, &inMax)){
    return nullptr;
  }
  XPLMDataRef inDataRef = drefFromObj(drefObj);
  if(outValuesObj && (outValuesObj != Py_None)){
    if(!PyList_Check(outValuesObj)){
      PyErr_SetString(PyExc_TypeError, "getDatavf expects list or None as the values parameter.");
      return nullptr;
    }
    if (inMax <= 0) {
      inMax = XPLMGetDatavf(inDataRef, nullptr, 0, 0);
    }
    if(inMax > 0){
      outValues = (float *)malloc(inMax * sizeof(float));
    }else{
      pythonLog("getdatavf count value must be positive");
      PyErr_SetString(PyExc_RuntimeError, "getDatavf count value must be positive.");
      return nullptr;
    }
  }
  
  int res = XPLMGetDatavf(inDataRef, outValues, inOffset, inMax);

  if(outValues != nullptr){
    if(PyList_Size(outValuesObj) > 0){
      PySequence_DelSlice(outValuesObj, 0, PyList_Size(outValuesObj));
    }
    //If res < inMax, copy only res elements;
    //  if inMax < res, copy only inMax elements
    for(int i = 0; i < (res < inMax ? res : inMax); ++i){
      PyObject *tmp = PyFloat_FromDouble(outValues[i]);
      PyList_Append(outValuesObj, tmp);
      Py_DECREF(tmp);
    }
    free(outValues);
  }
  PyObject *err = PyErr_Occurred();
  if(err) {
    char *s = objToStr(err);
    pythonLog("Error has already occurred in getDatavf: %s", s);
    free(s);
  }
  return PyLong_FromLong(res);
}

My_DOCSTR(_setDatavf__doc__, "setDatavf",
          "dataRef, values, offset=0, count=-1",
          "dataRef:XPLMDataRef, values:list[float], offset:int=0, count:int=-1",
          "None",
          "Set float array value for dataRef.\n"
          "\n"
          "values is a list of floats, to be written into dataRef starting\n"
          "at offset. Up to count values are written.\n"
          "\n"
          "If count is negative (or not provided), all values in the list are copied.\n"
          "It is an error for count to be greater than the length of the list.\n"
          "\n"
          "No return value.");
static PyObject *XPLMSetDatavfFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("dataRef"), CHAR("values"), CHAR("offset"), CHAR("count"), nullptr};
  (void) self;
  PyObject *drefObj;
  PyObject *inValuesObj;
  float *inValues = nullptr;
  int inOffset=0, inCount=-1;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "OO|ii", keywords, &drefObj, &inValuesObj, &inOffset, &inCount)){
    return nullptr;
  }
  XPLMDataRef inDataRef = drefFromObj(drefObj);
  if(!PySequence_Check(inValuesObj)){
    PyErr_SetString(PyExc_TypeError, "setDatavf expects list as the values parameter.");
    return nullptr;
  }
  if(inCount > 0 && PySequence_Length(inValuesObj) < inCount){
    PyErr_SetString(PyExc_RuntimeError, "setDatavf list too short for provided count.");
    return nullptr;
  }
  if (inCount <= 0) {
    inCount = PySequence_Length(inValuesObj);
  }

  inValues = (float *)malloc(inCount * sizeof(float));
  /* PyObject *tup = PySequence_Tuple(inValuesObj);*/
  for(int i = 0; i < inCount; ++i){
    PyObject *f = PySequence_GetItem(inValuesObj, i);
    inValues[i] = PyFloat_AsDouble(f);
    Py_DECREF(f);
  }
  /* Py_DECREF(tup); */
  
  XPLMSetDatavf(inDataRef, inValues, inOffset, inCount);
  free(inValues);
  Py_RETURN_NONE;
}


My_DOCSTR(_getDatab__doc__, "getDatab",
          "dataRef, values=None, offset=0, count=-1",
          "dataRef:XPLMDataRef, values:Optional[list[int]]=None, offset:int=0, count:int=-1",
          "int",
          "Get byte array value for dataRef.\n"
          "\n"
          "If values is None, return number of elements in the array (only).\n"
          "Otherwise, values should be a list into which will be copied elements\n"
          "from the dataRef, starting at offset, and continuing for count # of elements.\n"
          "If count is negative, or unspecified, all elements (relative offset) are copied.\n"
          "\n"
          "See also getDatas()."
          "\n"
          "Returns the number of elements copied.");
static PyObject *XPLMGetDatabFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("dataRef"), CHAR("values"), CHAR("offset"), CHAR("count"), nullptr};
  (void) self;
  PyObject *drefObj;
  PyObject *outValuesObj = Py_None;
  uint8_t *outValues = nullptr;
  int inOffset=0, inMax=-1;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|Oii", keywords, &drefObj, &outValuesObj, &inOffset, &inMax)){
    return nullptr;
  }
  XPLMDataRef inDataRef= drefFromObj(drefObj);
  if(outValuesObj && (outValuesObj != Py_None)){
    if(!PyList_Check(outValuesObj)){
      PyErr_SetString(PyExc_TypeError, "getDatab expects list or None as the values parameter.");
      return nullptr;
    }
    if (inMax <= 0) {
      inMax = XPLMGetDatab(inDataRef, nullptr, 0, 0);
    }
    if(inMax > 0){
      outValues = (uint8_t *)malloc(inMax * sizeof(uint8_t));
    }else{
      PyErr_SetString(PyExc_RuntimeError, "getDatab count value must be positive.");
      return nullptr;
    }
  }
  
  int res = XPLMGetDatab(inDataRef, outValues, inOffset, inMax);
  if(outValues != nullptr){
    if(PyList_Size(outValuesObj) > 0){
      PySequence_DelSlice(outValuesObj, 0, PyList_Size(outValuesObj));
    }
    //If res < inMax, copy only res elements;
    //  if inMax < res, copy only inMax elements
    for(int i = 0; i < (res < inMax ? res : inMax); ++i){
      PyObject *tmp = PyLong_FromLong(outValues[i]);
      PyList_Append(outValuesObj, tmp);
      Py_DECREF(tmp);
    }
    free(outValues);
  }
  return PyLong_FromLong(res);
}

My_DOCSTR(_setDatab__doc__, "setDatab",
          "dataRef, values, offset=0, count=-1",
          "dataRef:XPLMDataRef, values:list[int] | bytes, offset:int=0, count:int=-1",
          "None",
          "Set byte array value for dataRef.\n"
          "\n"
          "values is a list of bytes, to be written into dataRef starting\n"
          "at offset. Up to count values are written.\n"
          "\n"
          "If count is negative (or not provided), all values in the list are copied.\n"
          "It is an error for count to be greater than the length of the list.\n"
          "\n"
          "See also setDatas()."
          "\n"
          "No return value.");
static PyObject *XPLMSetDatabFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("dataRef"), CHAR("values"), CHAR("offset"), CHAR("count"), nullptr};
  (void) self;
  PyObject *drefObj;
  PyObject *inValuesObj;
  uint8_t *inValues = nullptr;
  int inOffset=0, inCount=-1;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "OO|ii", keywords, &drefObj, &inValuesObj, &inOffset, &inCount)){
    return nullptr;
  }
  XPLMDataRef inDataRef= drefFromObj(drefObj);
  if(!PySequence_Check(inValuesObj)){
    PyErr_SetString(PyExc_TypeError, "setDatab expects list as the values parameter.");
    return nullptr;
  }
  if(inCount > 0 && PySequence_Length(inValuesObj) < inCount){
    PyErr_SetString(PyExc_RuntimeError, "setDatab list too short.");
    return nullptr;
  }
  if (inCount <= 0) {
    inCount = PySequence_Length(inValuesObj);
  }
  
  inValues = (uint8_t *)malloc(inCount * sizeof(uint8_t));
  /* PyObject *tup = PySequence_Tuple(inValuesObj); */
  for(int i = 0; i < inCount; ++i){
    PyObject *f = PySequence_GetItem(inValuesObj, i);
    inValues[i] = PyLong_AsLong(f);
    Py_DECREF(f);
  }
  /* Py_DECREF(tup); */
  
  XPLMSetDatab(inDataRef, inValues, inOffset, inCount);
  free(inValues);
  Py_RETURN_NONE;
}

My_DOCSTR(_getDatas__doc__, "getDatas",
          "dataRef, offset=0, count=-1",
          "dataRef:XPLMDataRef, offset:int=0, count:int=-1",
          "str | None",
          "Returns string value for dataRef.\n"
          "\n"
          "String is the first null-terminated sequence found in the byte-array\n"
          "dataRef, starting at offset. If count is given, string returned is\n"
          "restricted to count length (not including a null byte).\n"
          "\n"
          "Note not all byte-array dataRefs are strings: be sure the requested\n"
          "dataRef is storing character information. Otherwise use getDatab().");
static PyObject *XPLMGetDatasFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("dataRef"), CHAR("offset"), CHAR("count"), nullptr};
  (void) self;
  PyObject *drefObj;
  char *outValues = nullptr;
  int inOffset=0, inMax=-1;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|ii", keywords, &drefObj, &inOffset, &inMax)){
    return nullptr;
  }
  XPLMDataRef inDataRef= drefFromObj(drefObj);
  if (inMax <= 0) {
    inMax = XPLMGetDatab(inDataRef, nullptr, 0, 0);
  }
  if(inMax > 0){
    outValues = (char *)malloc(inMax * sizeof(char));
  }else{
    PyErr_SetString(PyExc_RuntimeError, "getDatas count value must be positive.");
    return nullptr;
  }
  
  int res = XPLMGetDatab(inDataRef, outValues, inOffset, inMax);
  if(outValues != nullptr){
    size_t len = strlen(outValues);
    len = len > (size_t) res ? (size_t) res : len;  // in case returned outValues doesn't end with '\0' 
    PyObject *string = PyUnicode_FromStringAndSize(outValues, len);
    free(outValues);
    Py_INCREF(string);
    return string;
  }
  Py_RETURN_NONE;
}

My_DOCSTR(_setDatas__doc__, "setDatas",
          "dataRef, value, offset=0, count=-1",
          "dataRef:XPLMDataRef, value:str, offset:int=0, count:int=-1",
          "None",
          "Set byte array to string value for dataRef.\n"
          "\n"
          "value is a python unicode string (capable of being encoded as 'UTF-8').\n"
          "String is written into the dataRef starting at offset. Up to count\n"
          "characters are written. If count is more than len(value), the written\n"
          "values are padded with zeros ('\\x00') up to count.\n"
          "\n"
          "If count is negative (or not provided), value is copied AND the\n"
          "remaining length of the dataRef is zero-filled. Use count to limit\n"
          "the amount of padding.\n"
          "\n"
          "If len(value) is greater than existing dataRef value, and count is not\n"
          "specified, the underlying dataRef is NOT extended to accommodate the\n"
          "full string. Instead the string is copied upto the end of the existing\n"
          "data. To extend the underlying dataRef, provide a larger value for count.\n"
          "\n"
          "Caution: extend dataRef only if the underlying dataRef is implemented in \n"
          "python. Attempting to extend non-python dataRefs will cause the sim to\n"
          "crash.\n"
          "\n"
          "No return value.");

static PyObject *XPLMSetDatasFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("dataRef"), CHAR("value"), CHAR("offset"), CHAR("count"), nullptr};
  (void) self;
  PyObject *drefObj;
  PyObject *inValueObj;
  const char *inValue = nullptr;
  int inOffset=0, inCount=-1;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "OO|ii", keywords, &drefObj, &inValueObj, &inOffset, &inCount)){
    return nullptr;
  }
  XPLMDataRef inDataRef= drefFromObj(drefObj);

  if(!PyUnicode_Check(inValueObj)){
    PyErr_SetString(PyExc_TypeError, "setDatas expects unicode string as the value parameter.");
    return nullptr;
  }
  /* if(inCount > 0 && PyUnicode_GetLength(inValueObj) < inCount){ */
  /*   PyErr_SetString(PyExc_RuntimeError, "setDatab value string too short for provided count."); */
  /*   return nullptr; */
  /* } */
  char *buffer;
  int bufferSize;
  if (inCount <= 0) {
    /* if count isn't provided, we'll need to zero-fill to the end */
    int dataRefLength = XPLMGetDatab(inDataRef, nullptr, 0, 0);
    /* inCount = PyUnicode_GetLength(inValueObj);*/
    bufferSize = dataRefLength - inOffset;
  } else {
    bufferSize = inCount;
  }
  buffer = (char *)malloc(bufferSize);

  inValue = objToStr(inValueObj);
  //inValue = PyUnicode_AsUTF8AndSize(inValueObj, nullptr); /* docs say I'm not responsible for de-allocation of inValue */
  strncpy(buffer, inValue, bufferSize);
  XPLMSetDatab(inDataRef, (void *)buffer, inOffset, bufferSize);
  free((void *)inValue);
  free(buffer);
  Py_RETURN_NONE;
}

static int getDatai(void *inRefcon)
{
  intptr_t refcon_id = (intptr_t)inRefcon;
  auto it = accessorCallbacks.find(refcon_id);
  if (it == accessorCallbacks.end()) {
    pythonLog("dataAccessor refCon passed to getDatai (%p).", inRefcon);
    return -1;
  }

  DataRefInfo& info = it->second;
  if (info.read_int == Py_None) {
    pythonLog("getDatai callback not defined.");
    return 0;
  }

  set_moduleName(info.module_name);
  PyObject *args[] = {info.read_refCon};
  PyObject *oRes = PyObject_Vectorcall(info.read_int, args, 1, nullptr);

  if(PyErr_Occurred()) {
    /* ... If error occurs within the callback function
       simply allow the error to pass through, but DON'T
       attempt to use or DECREF the return from the function */

    /* vvvvv previously we'd catch and alter the error message vvvvv */
    /* char msg[1024]; */
    /* sprintf(msg, "[%s] Error in getDatai callback %s", */
    /*         objToStr(PyTuple_GetItem(pCbks, PLUGINSELF)), */
    /*         objToStr(oFun)); */
    /* PyErr_SetString(err, msg); */
    /* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */
    return 0;
  }

  int res = 0;
  if (oRes != Py_None) {
    res = PyLong_AsLong(oRes);
  }
  Py_DECREF(oRes);
  PyObject *err = PyErr_Occurred();
  if(err) {
    /* we report this error, because the 'error' is we can't convert
       the results, but the function itself appeared to execute without error */
    char msg[1024];
    char *s = objToStr(info.read_int);
    snprintf(msg, sizeof(msg), "[%s] getDatai callback %s failed to return a int / long", info.module_name, s);
    free(s);
    PyErr_SetString(err, msg);
  }
  return res;
}

static void setDatai(void *inRefcon, int inValue)
{
  intptr_t refcon_id = (intptr_t)inRefcon;
  auto it = accessorCallbacks.find(refcon_id);
  if (it == accessorCallbacks.end()) {
    pythonLog("Unknown dataAccessor refCon passed to setDatai (%p).", inRefcon);
    return;
  }

  DataRefInfo& info = it->second;
  if (info.write_int == Py_None) {
    pythonLog("setDatai callback not defined.");
    return;
  }

  PyObject *oArg2 = PyLong_FromLong(inValue);
  set_moduleName(info.module_name);
  PyObject *args[] = {info.write_refCon, oArg2};
  PyObject *oRes = PyObject_Vectorcall(info.write_int, args, 2, nullptr);

  if(PyErr_Occurred()){
    /* ... If error occurs within the callback function
       simply allow the error to pass through, but DON'T
       attempt to use or DECREF the return from the function */
    return;
  }
  Py_DECREF(oArg2);
  Py_XDECREF(oRes);
  return;
}

static float getDataf(void *inRefcon)
{
  intptr_t refcon_id = (intptr_t)inRefcon;
  auto it = accessorCallbacks.find(refcon_id);
  if (it == accessorCallbacks.end()) {
    pythonLog("Unknown dataAccessor refCon passed to getDataf (%p).", inRefcon);
    return -1;
  }

  DataRefInfo& info = it->second;
  if (info.read_float == Py_None) {
    pythonLog("getDataf callback not defined.");
    return 0.0;
  }

  set_moduleName(info.module_name);
  PyObject *args[] = {info.read_refCon};
  PyObject *oRes = PyObject_Vectorcall(info.read_float, args, 1, nullptr);

  if(PyErr_Occurred()) {
    return 0.0;
  }

  float res = 0.0;
  if (oRes != Py_None) {
    res = PyFloat_AsDouble(oRes);
  }
  Py_DECREF(oRes);

  PyObject *err = PyErr_Occurred();
  if(err) {
    char msg[1024];
    char *s = objToStr(info.read_float);
    snprintf(msg, sizeof(msg), "[%s] getDataf callback %s failed to return a float", CurrentPythonModuleName, s);
    free(s);
    PyErr_SetString(err, msg);
  }
  return res;
}

static void setDataf(void *inRefcon, float inValue)
{
  intptr_t refcon_id = (intptr_t)inRefcon;
  auto it = accessorCallbacks.find(refcon_id);
  if (it == accessorCallbacks.end()) {
    pythonLog("Unknown dataAccessor refCon passed to setDataf (%p).", inRefcon);
    return;
  }

  DataRefInfo& info = it->second;
  if (info.write_float == Py_None) {
    pythonLog("setDataf callback not defined.");
    return;
  }

  PyObject *oArg2 = PyFloat_FromDouble((double)inValue);
  set_moduleName(info.module_name);
  PyObject *args[] = {info.write_refCon, oArg2};
  PyObject *oRes = PyObject_Vectorcall(info.write_float, args, 2, nullptr);

  if(PyErr_Occurred()){
    return;
  }
  Py_DECREF(oArg2);
  Py_XDECREF(oRes);
  return;
}

static double getDatad(void *inRefcon)
{
  intptr_t refcon_id = (intptr_t)inRefcon;
  auto it = accessorCallbacks.find(refcon_id);
  if (it == accessorCallbacks.end()) {
    pythonLog("Unknown dataAccessor refCon passed to getDatad (%p).", inRefcon);
    return -1;
  }

  DataRefInfo& info = it->second;
  if (info.read_double == Py_None) {
    pythonLog("getDatad callback not defined.");
    return 0.0;
  }

  set_moduleName(info.module_name);
  PyObject *args[] = {info.read_refCon};
  PyObject *oRes = PyObject_Vectorcall(info.read_double, args, 1, nullptr);

  if(PyErr_Occurred()) {
    return 0.0;
  }

  double res = 0.0;
  if (oRes != Py_None) {
     res = PyFloat_AsDouble(oRes);
  }
  Py_DECREF(oRes);

  PyObject *err = PyErr_Occurred();
  if(err){
    char msg[1024];
    char *s = objToStr(info.read_double);
    snprintf(msg, sizeof(msg), "[%s] getDatad callback %s failed to return a float", CurrentPythonModuleName, s);
    free(s);
    PyErr_SetString(err, msg);
  }
  return res;
}

static void setDatad(void *inRefcon, double inValue)
{
  intptr_t refcon_id = (intptr_t)inRefcon;
  auto it = accessorCallbacks.find(refcon_id);
  if (it == accessorCallbacks.end()) {
    pythonLog("Unknown dataAccessor refCon passed to setDatad (%p).", inRefcon);
    return;
  }

  DataRefInfo& info = it->second;
  if (info.write_double == Py_None) {
    pythonLog("setDatad callback not defined.");
    return;
  }

  PyObject *oArg2 = PyFloat_FromDouble(inValue);
  set_moduleName(info.module_name);
  PyObject *args[] = {info.write_refCon, oArg2};
  PyObject *oRes = PyObject_Vectorcall(info.write_double, args, 2, nullptr);
  if(PyErr_Occurred()){
    return;
  }
  Py_DECREF(oArg2);
  Py_XDECREF(oRes);
  return;
}

static int getDatavi(void *inRefcon, int *outValues, int inOffset, int inMax)
{
  intptr_t refcon_id = (intptr_t)inRefcon;
  auto it = accessorCallbacks.find(refcon_id);
  if (it == accessorCallbacks.end()) {
    pythonLog("Unknown dataAccessor refCon passed to getDatavi (%p).", inRefcon);
    return -1;
  }

  DataRefInfo& info = it->second;
  if (info.read_int_array == Py_None) {
    pythonLog("getDatavi callback not defined.");
    return 0;
  }
  PyObject *oArg2 = PyLong_FromLong(inOffset);
  PyObject *oArg3 = PyLong_FromLong(inMax);
  PyObject *outValuesObj;
  if(outValues != nullptr){
    outValuesObj = PyList_New(0);
  }else{
    outValuesObj = Py_None;
    Py_INCREF(outValuesObj);
  }

  set_moduleName(info.module_name);
  PyObject *args[] = {info.read_refCon, outValuesObj, oArg2, oArg3};
  PyObject *oRes = PyObject_Vectorcall(info.read_int_array, args, 4, nullptr);
  PyObject *err = PyErr_Occurred();
  if(err) {
    char *s = objToStr(info.read_int_array);
    char *s2 = objToStr(err);
    pythonLog("[%s] getDatavi callback %s failed with %s.", info.module_name, s, s2);
    free(s);
    free(s2);
    pythonLogException(); /* because if we don't clear it here, it will get reported by the "next"
                             python call, which is likely completely unrelated to this error (because
                             this is being executed in a callback.
                             Side-effect is PrintEx() also clears the error.*/
    return 0;
  }
  Py_DECREF(oArg2);
  Py_DECREF(oArg3);

  int res = 0;
  if (oRes != Py_None) {
    res = PyLong_AsLong(oRes);
  }
  Py_DECREF(oRes);

  err = PyErr_Occurred();
  if (err) {
    char msg[1024];
    char *s = objToStr(info.read_int_array);
    snprintf(msg, sizeof(msg), "[%s] getDatavi callback %s failed to return an int", CurrentPythonModuleName, s);
    free(s);
    PyErr_SetString(err, msg);
  } else {
    if(outValuesObj != Py_None){
      for(int i = 0; i < res; ++i){
        PyObject *item = PyList_GetItem(outValuesObj, i); /* GetItem borrows */
        if (item == 0 || item == nullptr || item == Py_None) {
          outValues[i] = 0;
        } else {
          outValues[i] = PyLong_AsLong(item);
        }
      }
    }
  }
  Py_DECREF(outValuesObj);
  return res;
}

static void setDatavi(void *inRefcon, int *inValues, int inOffset, int inCount)
{
  intptr_t refcon_id = (intptr_t)inRefcon;
  auto it = accessorCallbacks.find(refcon_id);
  if (it == accessorCallbacks.end()) {
    pythonLog("Unknown dataAccessor refCon passed to setDatavi (%p).", inRefcon);
    return;
  }

  DataRefInfo& info = it->second;
  PyObject *inValuesObj = PyList_New(0);
  for(int i = 0; i < inCount; ++i){
    PyObject *tmp = PyLong_FromLong(inValues[i]);
    PyList_Append(inValuesObj, tmp);
    Py_DECREF(tmp);
  }
  PyObject *err = PyErr_Occurred();
  set_moduleName(info.module_name);
  if(err){
    char msg[1024];
    char *s = objToStr(info.write_int_array);
    snprintf(msg, sizeof(msg), "[%s] setDatavi error getting input longs: %s", info.module_name, s);
    free(s);
    PyErr_SetString(err, msg);
    return;
  }

  if (info.write_int_array == Py_None) {
    pythonLog("setDatavi callback not defined.");
    return;
  }
  PyObject *oArg2 = PyLong_FromLong(inOffset);
  PyObject *oArg3 = PyLong_FromLong(inCount);
  if(PyErr_Occurred()){
    return;
  }
  PyObject *args[] = {info.write_refCon, inValuesObj, oArg2, oArg3};
  PyObject *oRes = PyObject_Vectorcall(info.write_int_array, args, 4, nullptr);
  Py_DECREF(oArg2);
  Py_DECREF(oArg3);
  Py_DECREF(inValuesObj);

  if(PyErr_Occurred()){
    return;
  }

  Py_DECREF(oRes);
  return;
}

static int getDatavf(void *inRefcon, float *outValues, int inOffset, int inMax)
{
  intptr_t refcon_id = (intptr_t)inRefcon;
  auto it = accessorCallbacks.find(refcon_id);
  if (it == accessorCallbacks.end()) {
    pythonLog("Unknown dataAccessor refCon passed to getDatavf (%p).", inRefcon);
    return -1;
  }

  DataRefInfo& info = it->second;
  if (info.read_float_array == Py_None) {
    pythonLog("getDatavf callback not defined.");
    return 0;
  }
  PyObject *oArg2 = PyLong_FromLong(inOffset);
  PyObject *oArg3 = PyLong_FromLong(inMax);
  PyObject *outValuesObj;
  if(outValues != nullptr){
    outValuesObj = PyList_New(0);
  }else{
    outValuesObj = Py_None;
    Py_INCREF(outValuesObj);
  }

  set_moduleName(info.module_name);
  PyObject *args[] = {info.read_refCon, outValuesObj, oArg2, oArg3};
  PyObject *oRes = PyObject_Vectorcall(info.read_float_array, args, 4, nullptr);
  PyObject *err = PyErr_Occurred();
  if(err) {
    char *s = objToStr(info.read_float_array);
    char *s2 = objToStr(err);
    pythonLog("[%s] getDatavf callback %s failed with %s.", info.module_name, s, s2);
    free(s);
    free(s2);
    pythonLogException(); /* because if we don't clear it here, it will get reported by the "next"
                             python call, which is likely completely unrelated to this error (because
                             this is being executed in a callback.
                             Side-effect is PrintEx() also clears the error.*/
    return 0;
  }
  Py_DECREF(oArg2);
  Py_DECREF(oArg3);

  int res = 0;
  if (oRes != Py_None) {
    res = PyLong_AsLong(oRes);
  }

  Py_DECREF(oRes);

  err = PyErr_Occurred();
  if(err) {
    char msg[1024];
    char *s2 = objToStr(info.read_float_array);
    snprintf(msg, sizeof(msg), "[%s] getDatavf callback %s failed to get length of data", CurrentPythonModuleName, s2);
    free(s2);
    PyErr_SetString(err, msg);
  } else {
    if(outValuesObj != Py_None){
      for(int i = 0; i < res; ++i){
        PyObject *item = PyList_GetItem(outValuesObj, i);  /* GetItem borrows */
        err = PyErr_Occurred();
        if (err){
          pythonLog("Failed to get #%d from returned values", i);
          return i;
        }
        if (item == 0 || item == nullptr || item == Py_None) {
          outValues[i] = 0.0;
        } else {
          outValues[i] = PyFloat_AsDouble(item);
        }
      }
    }
  }
  Py_DECREF(outValuesObj);
  return res;
}

static void setDatavf(void *inRefcon, float *inValues, int inOffset, int inCount)
{
  intptr_t refcon_id = (intptr_t)inRefcon;
  auto it = accessorCallbacks.find(refcon_id);
  if (it == accessorCallbacks.end()) {
    pythonLog("Unknown dataAccessor refCon passed to setDatavf (%p).", inRefcon);
    return;
  }

  DataRefInfo& info = it->second;
  PyObject *inValuesObj = PyList_New(0);
  for(int i = 0; i < inCount; ++i){
    PyObject *tmp = PyFloat_FromDouble(inValues[i]);
    PyList_Append(inValuesObj, tmp);
    Py_DECREF(tmp);
  }
  PyObject *err = PyErr_Occurred();
  if(err){
    char msg[1024];
    char *s2 = objToStr(info.write_float_array);
    snprintf(msg, sizeof(msg), "[%s] setDatavf error getting input floats %s", info.module_name, s2);
    free(s2);
    PyErr_SetString(err, msg);
    return;
  }

  if (info.write_float_array == Py_None) {
    pythonLog("setDatavf callback not defined.");
    return;
  }
  PyObject *oArg2 = PyLong_FromLong(inOffset);
  PyObject *oArg3 = PyLong_FromLong(inCount);
  if(PyErr_Occurred()) {
    return;
  }
  set_moduleName(info.module_name);
  PyObject *args[] = {info.write_refCon, inValuesObj, oArg2, oArg3};
  PyObject *oRes = PyObject_Vectorcall(info.write_float_array, args, 4, nullptr);
  Py_DECREF(oArg2);
  Py_DECREF(oArg3);
  Py_DECREF(inValuesObj);

  if(PyErr_Occurred()) {
    return;
  }

  Py_DECREF(oRes);
  return;
}

static int getDatab(void *inRefcon, void *outValue, int inOffset, int inMax)
{
  intptr_t refcon_id = (intptr_t)inRefcon;
  auto it = accessorCallbacks.find(refcon_id);
  if (it == accessorCallbacks.end()) {
    pythonLog("Unknown dataAccessor refCon passed to getDatab (%p).", inRefcon);
    return -1;
  }

  DataRefInfo& info = it->second;
  if (info.read_data == Py_None) {
    pythonLog("getDatab callback not defined.");
    return 0;
  }
  PyObject *oArg2 = PyLong_FromLong(inOffset);
  PyObject *oArg3 = PyLong_FromLong(inMax);
  PyObject *outValuesObj;
  if(outValue != nullptr){
    outValuesObj = PyList_New(0);
  }else{
    outValuesObj = Py_None;
    Py_INCREF(outValuesObj);
  }
  set_moduleName(info.module_name);
  PyObject *args[] = {info.read_refCon, outValuesObj, oArg2, oArg3};
  PyObject *oRes = PyObject_Vectorcall(info.read_data, args, 4, nullptr);
  PyObject *err = PyErr_Occurred();
  if(err) {
    char *s = objToStr(info.read_data);
    char *s2 = objToStr(err);
    pythonLog("[%s] getDatab callback %s failed with %s.", info.module_name, s, s2);
    free(s);
    free(s2);
    pythonLogException(); /* because if we don't clear it here, it will get reported by the "next"
                             python call, which is likely completely unrelated to this error (because
                             this is being executed in a callback.
                             Side-effect is PrintEx() also clears the error.*/
    return 0;
  }
  Py_DECREF(oArg2);
  Py_DECREF(oArg3);

  int res = 0;
  if (oRes != Py_None) {
    res = PyLong_AsLong(oRes);
  }

  Py_DECREF(oRes);

  err = PyErr_Occurred();
  if(err){
    char msg[1024];
    char *s = objToStr(info.read_data);
    snprintf(msg, sizeof(msg), "[%s] getData callback %s failed to get length of data", CurrentPythonModuleName, s);
    PyErr_SetString(err, msg);
  } else {
    if(outValuesObj != Py_None){
      uint8_t *pOutValue = (uint8_t *)outValue;
      for(int i = 0; i < res; ++i){
        pOutValue[i] = PyLong_AsLong(PyList_GetItem(outValuesObj, i));
        err = PyErr_Occurred();
        if(err){
          char msg[1024];
          char *s = objToStr(info.read_data);
          snprintf(msg, sizeof(msg), "[%s] getDatab callback %s failed to return valid data", CurrentPythonModuleName, s);
          free(s);
          PyErr_SetString(err, msg);
          return 0;
        }
      }
    }
  }
  Py_DECREF(outValuesObj);
  return res;
}

static void setDatab(void *inRefcon, void *inValue, int inOffset, int inCount)
{
  intptr_t refcon_id = (intptr_t)inRefcon;
  auto it = accessorCallbacks.find(refcon_id);
  if (it == accessorCallbacks.end()) {
    pythonLog("Unknown dataAccessor refCon passed to setDatab (%p).", inRefcon);
    return;
  }

  DataRefInfo& info = it->second;
  set_moduleName(info.module_name);

  PyObject *inValuesObj = PyList_New(0);
  uint8_t *pInValue = (uint8_t *)inValue;
  for(int i = 0; i < inCount; ++i){
    PyObject *tmp = PyLong_FromLong(pInValue[i]);
    PyList_Append(inValuesObj, tmp);
    Py_DECREF(tmp);
  }
  PyObject *err = PyErr_Occurred();
  if(err){
    char msg[1024];
    char *s = objToStr(info.write_data);
    snprintf(msg, sizeof(msg), "[%s] setDatab error getting input data %s", info.module_name, s);
    free(s);
    PyErr_SetString(err, msg);
    return;
  }

  if (info.write_data == Py_None) {
    pythonLog("setDatab callback not defined.");
    return;
  }
  PyObject *oArg2 = PyLong_FromLong(inOffset);
  PyObject *oArg3 = PyLong_FromLong(inCount);
  if(PyErr_Occurred()){
    return;
  }
  PyObject *args[] = {info.write_refCon, inValuesObj, oArg2, oArg3};
  PyObject *oRes = PyObject_Vectorcall(info.write_data, args, 4, nullptr);
  Py_DECREF(oArg2);
  Py_DECREF(oArg3);
  Py_DECREF(inValuesObj);
  if(PyErr_Occurred()){
    return;
  }

  Py_DECREF(oRes);
  return;
}


My_DOCSTR(_registerDataAccessor__doc__, "registerDataAccessor",
          "name, dataType=Type_Unknown, writable=-1, readInt=None, writeInt=None, readFloat=None, writeFloat=None, readDouble=None, writeDouble=None, readIntArray=None, writeIntArray=None, readFloatArray=None, writeFloatArray=None, readData=None, writeData=None, readRefCon=None, writeRefCon=None",
          "name:str, dataType:int=Type_Unknown, writable:int=-1, "
          "readInt:Optional[Callable[[Any], int]]=None, writeInt:Optional[Callable[[Any, int], None]]=None, "
          "readFloat:Optional[Callable[[Any], float]]=None, writeFloat:Optional[Callable[[Any, float], None]]=None, "
          "readDouble:Optional[Callable[[Any], float]]=None, writeDouble:Optional[Callable[[Any, float], None]]=None, "
          "readIntArray:Optional[Callable[[Any, list[int], int, int], int]]=None, writeIntArray:Optional[Callable[[Any, list[int], int, int], None]]=None, "
          "readFloatArray:Optional[Callable[[Any, list[float], int, int], int]]=None, writeFloatArray:Optional[Callable[[Any, list[float], int, int], None]]=None, "
          "readData:Optional[Callable[[Any, list[int], int, int], int]]=None, writeData:Optional[Callable[[Any, list[int], int, int], None]]=None, "
          "readRefCon:Any=None, writeRefCon:Any=None",
          "XPLMDataRef",
          "Register data accessors for provided string name.\n"
          "\n"
          "Provide one or more read/write callback functions which implement\n"
          "get/set access. If dataType is Type_Unknown, or writable is -1, we'll\n"
          "calculate their value to match provided callbacks.\n\n"
          "Two optional refCon are available, to be passed to your get/set functions.\n"
          "\n"
          "* Scalar get callback functions take single (refCon) parameter \n"
          "  and return the value.\n"
          "* Scalar set callback functions take (refCon, value) parameters \n"
          "  with no return.\n"
          "* Vector gets take (refCon, values, offset, count), return # elements\n"
          "  copied into values.\n"
          "* Vector sets take (refCon, values, offset, count), with no return\n");
          
          
static PyObject *XPLMRegisterDataAccessorFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("name"), CHAR("dataType"), CHAR("writable"), CHAR("readInt"), CHAR("writeInt"),
                             CHAR("readFloat"), CHAR("writeFloat"), CHAR("readDouble"), CHAR("writeDouble"),
                             CHAR("readIntArray"), CHAR("writeIntArray"), CHAR("readFloatArray"), CHAR("writeFloatArray"),
                             CHAR("readData"), CHAR("writeData"),
                             CHAR("readRefCon"), CHAR("writeRefCon"), nullptr};
  (void)self;
  const char *inDataName;
  int inDataType=xplmType_Unknown, inIsWritable=-1;
  PyObject *ri=Py_None, *wi=Py_None, *rf=Py_None, *wf=Py_None, *rd=Py_None, *wd=Py_None, *rai=Py_None, *wai=Py_None, *raf=Py_None,
    *waf=Py_None, *rab=Py_None, *wab=Py_None, *rRef=Py_None, *wRef=Py_None;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "s|iiOOOOOOOOOOOOOO", keywords, &inDataName, &inDataType, &inIsWritable,
                                  &ri, &wi, &rf, &wf, &rd, &wd, &rai, &wai, &raf, &waf, &rab, &wab, &rRef, &wRef)) {
    return nullptr;
  }
  if (inIsWritable == -1) {
    inIsWritable = (wi != Py_None || wf != Py_None || wd != Py_None || wai != Py_None || waf != Py_None || wab != Py_None) ? 1 : 0;
  }
  if (inDataType == xplmType_Unknown) {
    inDataType |= (ri != Py_None || wi != Py_None) ? xplmType_Int : 0;
    inDataType |= (rf != Py_None || wf != Py_None) ? xplmType_Float : 0;
    inDataType |= (rd != Py_None || wd != Py_None) ? xplmType_Double : 0;
    inDataType |= (rai != Py_None || wai != Py_None) ? xplmType_IntArray : 0;
    inDataType |= (raf != Py_None || waf != Py_None) ? xplmType_FloatArray : 0;
    inDataType |= (rab != Py_None || wab != Py_None) ? xplmType_Data : 0;
    if (inDataType == xplmType_Unknown) {
      PyErr_SetString(PyExc_ValueError, "Could not determine dataType value for dataRef");
    }
  }

  intptr_t refcon_id = accessorCntr++;
  void *refcon = (void *)refcon_id;

  XPLMDataRef res = XPLMRegisterDataAccessor(
                                          inDataName,
                                          inDataType,
                                          inIsWritable,
                                          getDatai,   setDatai,
                                          getDataf,   setDataf,
                                          getDatad,   setDatad,
                                          getDatavi,  setDatavi,
                                          getDatavf,  setDatavf,
                                          getDatab,   setDatab,
                                          refcon,     refcon);

  // Increment references for all PyObject* members
  Py_INCREF(ri); Py_INCREF(wi); Py_INCREF(rf); Py_INCREF(wf);
  Py_INCREF(rd); Py_INCREF(wd); Py_INCREF(rai); Py_INCREF(wai);
  Py_INCREF(raf); Py_INCREF(waf); Py_INCREF(rab); Py_INCREF(wab);
  Py_INCREF(rRef); Py_INCREF(wRef);

  accessorCallbacks[refcon_id] = {
    .module_name = CurrentPythonModuleName,
    .data_name = inDataName,
    .data_type = inDataType,
    .is_writable = inIsWritable,
    .read_int = ri,
    .write_int = wi,
    .read_float = rf,
    .write_float = wf,
    .read_double = rd,
    .write_double = wd,
    .read_int_array = rai,
    .write_int_array = wai,
    .read_float_array = raf,
    .write_float_array = waf,
    .read_data = rab,
    .write_data = wab,
    .read_refCon = rRef,
    .write_refCon = wRef,
    .dataRef = res
  };

  PyObject *dataRefCapsule = makeCapsule(res, "XPLMDataRef");
  PyObject *accessorDictKey = PyLong_FromVoidPtr(refcon);
  Py_DECREF(accessorDictKey);
  return dataRefCapsule;
}

My_DOCSTR(_unregisterDataAccessor__doc__, "unregisterDataAccessor",
          "accessor",
          "accessor:XPLMDataRef",
          "None",
          "Unregisters data accessor.");
static PyObject *XPLMUnregisterDataAccessorFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("accessor"), nullptr};
  (void)self;
  PyObject *drefObj;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &drefObj)) {
    return nullptr;
  }
  PyObject *pluginSelf = get_moduleName_p();
  intptr_t accessor_id = -1;
  for (auto& pair : accessorCallbacks) {
    if (pair.second.dataRef == getVoidPtr(drefObj, "XPLMDataRef")) {
      accessor_id = pair.first;
    }
  }
  if (accessor_id == -1) {
    Py_DECREF(pluginSelf);
    pythonLog("XPLMUnregisterDataref: No such dataref registered!\n");
    Py_RETURN_NONE;
  }

  //  intptr_t accessor_id = (intptr_t)PyLong_AsVoidPtr(refconObj);
  auto it = accessorCallbacks.find(accessor_id);
  if (it == accessorCallbacks.end()) {
    Py_DECREF(pluginSelf);
    pythonLog("XPLMUnregisterDataref: No such refcon registered!\n");
    Py_RETURN_NONE;
  }

  DataRefInfo& info = it->second;
  PyObject *registerer = PyUnicode_FromString(info.module_name);
  if(PyObject_RichCompareBool(pluginSelf, registerer, Py_NE)) {
    Py_DECREF(registerer);
    Py_DECREF(pluginSelf);
    pythonLog("XPLMUnregisterDataref: Don't unregister dataref you didn't register!!\n");
    Py_RETURN_NONE;
  }
  Py_DECREF(pluginSelf);
  Py_DECREF(registerer);

  // Clean up Python object references
  Py_DECREF(info.read_int);
  Py_DECREF(info.write_int);
  Py_DECREF(info.read_float);
  Py_DECREF(info.write_float);
  Py_DECREF(info.read_double);
  Py_DECREF(info.write_double);
  Py_DECREF(info.read_int_array);
  Py_DECREF(info.write_int_array);
  Py_DECREF(info.read_float_array);
  Py_DECREF(info.write_float_array);
  Py_DECREF(info.read_data);
  Py_DECREF(info.write_data);
  Py_DECREF(info.read_refCon);
  Py_DECREF(info.write_refCon);

  XPLMUnregisterDataAccessor(info.dataRef);
  accessorCallbacks.erase(it);
  deleteCapsule(drefObj);
  Py_RETURN_NONE;
}

static void genericSharedDataChanged(void *inRefcon)
{
  intptr_t refcon_id = (intptr_t)inRefcon;
  auto it = sharedCallbacks.find(refcon_id);
  if (it == sharedCallbacks.end()) {
    pythonLog("Shared data callback called with wrong inRefcon: %p\n", inRefcon);
    return;
  }

  SharedInfo& info = it->second;
  set_moduleName(info.module_name);

  if (info.callback != Py_None) {
    PyObject *args[] = {info.refCon};
    PyObject *oRes = PyObject_Vectorcall(info.callback, args, 1, nullptr);
    PyObject *err = PyErr_Occurred();
    if(err){
      char msg[1024];
      char *s2 = objToStr(info.callback);
      snprintf(msg, sizeof(msg), "[%s] Error in genericSharedDataChanged callback %s", info.module_name, s2);
      free(s2);
      pythonLog("%s", msg);
      pythonLogFlush();
      PyErr_SetString(err, msg);
      return;
    }
    Py_DECREF(oRes);
  }
}

My_DOCSTR(_shareData__doc__, "shareData",
          "name, dataType, dataChanged=None, refCon=None",
          "name:str, dataType:int, "
          "dataChanged:Optional[Callable[[Any], None]]=None, refCon:Any=None",
          "int",
          "Create shared data ref with provided name and dataType.\n"
          "\n"
          "Optionally provide a callback function which will be called whenever\n"
          "this data ref has been changed.\n"
          "\nCallback takes single (refCon) parameter\n"
          "\nReturns 1 on success 0 otherwise.");
static PyObject *XPLMShareDataFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("name"), CHAR("dataType"), CHAR("dataChanged"), CHAR("refCon"), nullptr};
  (void) self;
  const char *inDataName;
  XPLMDataTypeID inDataType = xplmType_Unknown;
  PyObject *inNotificationFunc=Py_None, *inNotificationRefcon=Py_None;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "si|OO", keywords, &inDataName, &inDataType, &inNotificationFunc, &inNotificationRefcon)) {
    return nullptr;
  }
  intptr_t refcon_id = sharedCntr++;
  void *refcon = (void *)refcon_id;
  int res = XPLMShareData(inDataName, inDataType, genericSharedDataChanged, refcon);
  if(res != 1){
    return PyLong_FromLong(res);
  }

  Py_INCREF(inNotificationFunc);
  Py_INCREF(inNotificationRefcon);

  sharedCallbacks.emplace(refcon_id, SharedInfo {
    CurrentPythonModuleName,
    inDataName,
    inDataType,
    inNotificationFunc,
    inNotificationRefcon});

  return PyLong_FromLong(res);
}

My_DOCSTR(_countDataRefs__doc__, "countDataRefs",
          "",
          "",
          "int",
          "Returns the total number of datarefs that have been registered in X-Plane.");
static PyObject *XPLMCountDataRefsFun(PyObject *self, PyObject *args)
{
  (void)self;
  (void)args;
  if(!XPLMCountDataRefs_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMCountDataRefs is available only in XPLM400 and up and requires at least X-Plane v12.04.");
    return nullptr;
  }
  return PyLong_FromLong(XPLMCountDataRefs_ptr());
}

My_DOCSTR(_getDataRefsByIndex__doc__, "getDataRefsByIndex",
          "offset=0, count=1",
          "offset:int=0, count:int=1",
          "None | list[XPLMDataRef]",
          "Returns list of dataRefs, each similar to return from xp.findDataRef().\n"
          "Use xp.getDataRefInfo() to access information about the dataref.\n"
          "As a special case, count=-1 returns all datarefs starting from offset to the end.\n"
          "\n"
          "CAUTION: requesting datarefs greater than countDataRefs() returns garbage. If you\n"
          "try to use these, you may crash the sim." );
static PyObject *XPLMGetDataRefsByIndexFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("offset"), CHAR("count"), nullptr};
  (void)self;
  int offset = 0;
  int count = 1;

  if(!XPLMGetDataRefsByIndex_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMGetDataRefsByIndex is available only in XPLM400 and up and requires at least X-Plane v12.04.");
    return nullptr;
  }

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|ii", keywords, &offset, &count)) {
    return nullptr;
  }
  if (offset < 0) {
    PyErr_SetString(PyExc_ValueError, "invalid offset, too low");
    Py_RETURN_NONE;
  }
  if (count < 0) {
    count = XPLMCountDataRefs_ptr() - offset;
    if (count < 0) {
      PyErr_SetString(PyExc_ValueError, "invalid offset, too high");
      Py_RETURN_NONE;
    }
  }
  if (count + offset > XPLMCountDataRefs_ptr()) {
      PyErr_SetString(PyExc_ValueError, "invalid count and offset, too high");
      Py_RETURN_NONE;
  }

  XPLMDataRef *outDataRefs = (XPLMDataRef *)malloc(count * sizeof(XPLMDataRef));
  XPLMGetDataRefsByIndex_ptr(offset, count, outDataRefs);
  PyObject *outValuesObj = PyList_New(0);
  for(int i = 0; i < count; i++) {
    if (outDataRefs[i] == nullptr) {
      /* bail.. the only time this (should) happen is if we're asked to get more dataRefs than there
         are in the system. We'll get garbage data which (often) includes
      */
      free(outDataRefs);
      Py_RETURN_NONE;
    }
    PyList_Append(outValuesObj, makeCapsule(outDataRefs[i], "XPLMDataRef"));
  }
  free(outDataRefs);
  return outValuesObj;
}

My_DOCSTR(_getDataRefInfo__doc__, "getDataRefInfo",
          "dataRef",
          "dataRef:XPLMDataRef",
          "XPLMDataRefInfo_t",
          "Return DataRefInfo object for provided dataRef.\n"
          "\n"
          "XPLMDataRefInfo_t object is .name, .type, .writable, .owner\n"
          "  recall type is a bitfield, see xp.getDataRefTypes()");
static PyObject *XPLMGetDataRefInfoFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("dataRef"), nullptr};
  (void) self;

  if(!XPLMGetDataRefInfo_ptr){
    PyErr_SetString(PyExc_RuntimeError, "XPLMGetDataRefInfo is available only in XPLM400 and up and requires at least X-Plane v12.04.");
    return nullptr;
  }

  PyObject *dataRef;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &dataRef)){
    return nullptr;
  }
  XPLMDataRef inDataRef = drefFromObj(dataRef);
  XPLMDataRefInfo_t outInfo;
  outInfo.structSize = sizeof(XPLMDataRefInfo_t);
  XPLMGetDataRefInfo_ptr(inDataRef, &outInfo);
  return PyDataRefInfo_New(outInfo.name, outInfo.type, outInfo.writable, outInfo.owner);
}

My_DOCSTR(_unshareData__doc__, "unshareData",
          "name, dataType, dataChanged=None, refCon=None",
          "name:str, dataType:int, dataChanged:Optional[Callable[[Any], None]]=None, refCon:Any=None",
          "int",
          "Unshare data. If dataChanged function was provided with initial shareData()\n"
          "the callback will no longer be called on data changes.\n"
          "All parameter values must match those provided with shareData()\n"
          "in order to be successful."
          "\n"
          "Returns 1 on success, 0 otherwise");
static PyObject *XPLMUnshareDataFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("name"), CHAR("dataType"), CHAR("dataChanged"), CHAR("refCon"), nullptr};
  (void) self;
  const char *inDataName = nullptr;
  XPLMDataTypeID inDataType = xplmType_Unknown;
  PyObject *callbackObj = Py_None;
  PyObject *refconObj = Py_None;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "si|OO", keywords, &inDataName, &inDataType, &callbackObj, &refconObj)) {
    return nullptr;
  }

  PyObject *pluginSelf = get_moduleName_p();
  const char* target_module = CurrentPythonModuleName;
  intptr_t target_refcon_id = -1;

  for (auto it = sharedCallbacks.begin(); it != sharedCallbacks.end(); ++it) {
    SharedInfo& info = it->second;

    // only look for things this plugin is sharing...
    if (0 != strcmp(info.module_name, target_module)) continue;
    // Look for inDataName match
    if (info.name != std::string(inDataName)) continue;
    if (info.data_type != inDataType) continue;
    if (info.callback != callbackObj) continue;
    if (info.refCon != refconObj) continue;

    target_refcon_id = it->first;
    break;
  }

  Py_DECREF(pluginSelf);

  if (target_refcon_id != -1) {
    // Found and deleting unshared
    auto it = sharedCallbacks.find(target_refcon_id);
    if (it != sharedCallbacks.end()) {
      SharedInfo& info = it->second;
      int res = XPLMUnshareData(inDataName, inDataType, genericSharedDataChanged, (void*)target_refcon_id);

      // Clean up Python object references
      Py_DECREF(info.callback);
      Py_DECREF(info.refCon);

      sharedCallbacks.erase(it);
      return PyLong_FromLong(res);
    }
  }

  // Couldn't find the right shared data...
  return PyLong_FromLong(0);
}


static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  // Clean up accessor callbacks
  for (auto& pair : accessorCallbacks) {
    DataRefInfo& info = pair.second;
    Py_DECREF(info.read_int);
    Py_DECREF(info.write_int);
    Py_DECREF(info.read_float);
    Py_DECREF(info.write_float);
    Py_DECREF(info.read_double);
    Py_DECREF(info.write_double);
    Py_DECREF(info.read_int_array);
    Py_DECREF(info.write_int_array);
    Py_DECREF(info.read_float_array);
    Py_DECREF(info.write_float_array);
    Py_DECREF(info.read_data);
    Py_DECREF(info.write_data);
    Py_DECREF(info.read_refCon);
    Py_DECREF(info.write_refCon);
    XPLMUnregisterDataAccessor(info.dataRef);
  }
  accessorCallbacks.clear();

  // Clean up shared callbacks
  for (auto& pair : sharedCallbacks) {
    Py_DECREF(pair.second.callback);
    Py_DECREF(pair.second.refCon);
  }
  sharedCallbacks.clear();

  Py_RETURN_NONE;
}

PyObject* buildDataRefCallbackDict(void)
{
  PyObject *dataRefs = PyDict_New();

  for (const auto& pair : accessorCallbacks) {
    intptr_t key = pair.first;
    const DataRefInfo& info = pair.second;

    // Create 19-element tuple containing all DataRefInfo elements
    PyObject *tuple = PyTuple_New(19);
    PyTuple_SetItem(tuple, 0, PyUnicode_FromString(info.module_name));        // module_name
    PyTuple_SetItem(tuple, 1, PyUnicode_FromString(info.data_name.c_str()));          // data_name
    PyTuple_SetItem(tuple, 2, PyLong_FromLong(info.data_type));                       // data_type
    PyTuple_SetItem(tuple, 3, PyLong_FromLong(info.is_writable));                     // is_writable

    // Python callback objects (increment reference count)
    Py_INCREF(info.read_int);
    PyTuple_SetItem(tuple, 4, info.read_int);                                         // read_int
    Py_INCREF(info.write_int);
    PyTuple_SetItem(tuple, 5, info.write_int);                                        // write_int
    Py_INCREF(info.read_float);
    PyTuple_SetItem(tuple, 6, info.read_float);                                       // read_float
    Py_INCREF(info.write_float);
    PyTuple_SetItem(tuple, 7, info.write_float);                                      // write_float
    Py_INCREF(info.read_double);
    PyTuple_SetItem(tuple, 8, info.read_double);                                      // read_double
    Py_INCREF(info.write_double);
    PyTuple_SetItem(tuple, 9, info.write_double);                                     // write_double
    Py_INCREF(info.read_int_array);
    PyTuple_SetItem(tuple, 10, info.read_int_array);                                  // read_int_array
    Py_INCREF(info.write_int_array);
    PyTuple_SetItem(tuple, 11, info.write_int_array);                                 // write_int_array
    Py_INCREF(info.read_float_array);
    PyTuple_SetItem(tuple, 12, info.read_float_array);                                // read_float_array
    Py_INCREF(info.write_float_array);
    PyTuple_SetItem(tuple, 13, info.write_float_array);                               // write_float_array
    Py_INCREF(info.read_data);
    PyTuple_SetItem(tuple, 14, info.read_data);                                       // read_data
    Py_INCREF(info.write_data);
    PyTuple_SetItem(tuple, 15, info.write_data);                                      // write_data
    Py_INCREF(info.read_refCon);
    PyTuple_SetItem(tuple, 16, info.read_refCon);                                     // read_refCon
    Py_INCREF(info.write_refCon);
    PyTuple_SetItem(tuple, 17, info.write_refCon);                                    // write_refCon

    PyObject *capsule = makeCapsule(info.dataRef, "XPLMDataRef");
    PyTuple_SetItem(tuple, 18, capsule);

    // Note: info.dataRef (XPLMDataRef) is not included as it's a C pointer type

    // Add to dictionary with accessorCntr as key
    PyObject *py_key = PyLong_FromLong(key);
    PyDict_SetItem(dataRefs, py_key, tuple);
    Py_DECREF(py_key);
    Py_DECREF(tuple);
  }

  return dataRefs;
}

PyObject* buildSharedDataRefCallbackDict(void)
{
  PyObject *sharedDataRefs = PyDict_New();

  for (const auto& pair : sharedCallbacks) {
    intptr_t key = pair.first;
    const SharedInfo& info = pair.second;

    // Create 5-element tuple containing all SharedInfo elements
    PyObject *tuple = PyTuple_New(5);
    PyTuple_SetItem(tuple, 0, PyUnicode_FromString(info.module_name));        // module_name
    PyTuple_SetItem(tuple, 1, PyUnicode_FromString(info.name.c_str()));               // name
    PyTuple_SetItem(tuple, 2, PyLong_FromLong(info.data_type));                       // data_type

    // Python callback objects (increment reference count)
    Py_INCREF(info.callback);
    PyTuple_SetItem(tuple, 3, info.callback);                                         // callback
    Py_INCREF(info.refCon);
    PyTuple_SetItem(tuple, 4, info.refCon);                                           // refCon

    // Add to dictionary with sharedCntr as key
    PyObject *py_key = PyLong_FromLong(key);
    PyDict_SetItem(sharedDataRefs, py_key, tuple);
    Py_DECREF(py_key);
    Py_DECREF(tuple);
  }

  return sharedDataRefs;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
static PyMethodDef XPLMDataAccessMethods[] = {
  {"findDataRef", (PyCFunction)XPLMFindDataRefFun, METH_VARARGS | METH_KEYWORDS, _findDataRef__doc__},
  {"XPLMFindDataRef", (PyCFunction)XPLMFindDataRefFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"canWriteDataRef", (PyCFunction)XPLMCanWriteDataRefFun, METH_VARARGS | METH_KEYWORDS, _canWriteDataRef__doc__},
  {"XPLMCanWriteDataRef", (PyCFunction)XPLMCanWriteDataRefFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"isDataRefGood", (PyCFunction)XPLMIsDataRefGoodFun, METH_VARARGS | METH_KEYWORDS, _isDataRefGood__doc__},
  {"XPLMIsDataRefGood", (PyCFunction)XPLMIsDataRefGoodFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"getDataRefTypes", (PyCFunction)XPLMGetDataRefTypesFun, METH_VARARGS | METH_KEYWORDS, _getDataRefTypes__doc__},
  {"XPLMGetDataRefTypes", (PyCFunction)XPLMGetDataRefTypesFun, METH_VARARGS | METH_KEYWORDS, "Check dataref type"},
  {"getDatai", (PyCFunction)XPLMGetDataiFun, METH_VARARGS | METH_KEYWORDS, _getDatai__doc__},
  {"XPLMGetDatai", (PyCFunction)XPLMGetDataiFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"setDatai", (PyCFunction)XPLMSetDataiFun, METH_VARARGS | METH_KEYWORDS, _setDatai__doc__},
  {"XPLMSetDatai", (PyCFunction)XPLMSetDataiFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"getDataf", (PyCFunction)XPLMGetDatafFun, METH_VARARGS | METH_KEYWORDS, _getDataf__doc__},
  {"XPLMGetDataf", (PyCFunction)XPLMGetDatafFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"setDataf", (PyCFunction)XPLMSetDatafFun, METH_VARARGS | METH_KEYWORDS, _setDataf__doc__},
  {"XPLMSetDataf", (PyCFunction)XPLMSetDatafFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"getDatad", (PyCFunction)XPLMGetDatadFun, METH_VARARGS | METH_KEYWORDS, _getDatad__doc__},
  {"XPLMGetDatad", (PyCFunction)XPLMGetDatadFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"setDatad", (PyCFunction)XPLMSetDatadFun, METH_VARARGS | METH_KEYWORDS, _setDatad__doc__},
  {"XPLMSetDatad", (PyCFunction)XPLMSetDatadFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"getDatavi", (PyCFunction)XPLMGetDataviFun, METH_VARARGS | METH_KEYWORDS, _getDatavi__doc__},
  {"XPLMGetDatavi", (PyCFunction)XPLMGetDataviFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"setDatavi", (PyCFunction)XPLMSetDataviFun, METH_VARARGS | METH_KEYWORDS, _setDatavi__doc__},
  {"XPLMSetDatavi", (PyCFunction)XPLMSetDataviFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"getDatavf", (PyCFunction)XPLMGetDatavfFun, METH_VARARGS | METH_KEYWORDS, _getDatavf__doc__},
  {"XPLMGetDatavf", (PyCFunction)XPLMGetDatavfFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"setDatavf", (PyCFunction)XPLMSetDatavfFun, METH_VARARGS | METH_KEYWORDS, _setDatavf__doc__},
  {"XPLMSetDatavf", (PyCFunction)XPLMSetDatavfFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"getDatab", (PyCFunction)XPLMGetDatabFun, METH_VARARGS | METH_KEYWORDS, _getDatab__doc__},
  {"getDatas", (PyCFunction)XPLMGetDatasFun, METH_VARARGS | METH_KEYWORDS, _getDatas__doc__},
  {"XPLMGetDatab", (PyCFunction)XPLMGetDatabFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"setDatab", (PyCFunction)XPLMSetDatabFun, METH_VARARGS | METH_KEYWORDS, _setDatab__doc__},
  {"setDatas", (PyCFunction)XPLMSetDatasFun, METH_VARARGS | METH_KEYWORDS, _setDatas__doc__},
  {"XPLMSetDatab", (PyCFunction)XPLMSetDatabFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"registerDataAccessor", (PyCFunction)XPLMRegisterDataAccessorFun, METH_VARARGS | METH_KEYWORDS, _registerDataAccessor__doc__},
  {"XPLMRegisterDataAccessor", (PyCFunction)XPLMRegisterDataAccessorFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"unregisterDataAccessor", (PyCFunction)XPLMUnregisterDataAccessorFun, METH_VARARGS | METH_KEYWORDS, _unregisterDataAccessor__doc__},
  {"XPLMUnregisterDataAccessor", (PyCFunction)XPLMUnregisterDataAccessorFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"shareData", (PyCFunction)XPLMShareDataFun, METH_VARARGS | METH_KEYWORDS, _shareData__doc__},
  {"XPLMShareData", (PyCFunction)XPLMShareDataFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"unshareData", (PyCFunction)XPLMUnshareDataFun, METH_VARARGS | METH_KEYWORDS, _unshareData__doc__},
  {"XPLMUnshareData", (PyCFunction)XPLMUnshareDataFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"countDataRefs", (PyCFunction)XPLMCountDataRefsFun, METH_VARARGS, _countDataRefs__doc__},
  {"XPLMCountDataRefs", (PyCFunction)XPLMCountDataRefsFun, METH_VARARGS, ""},
  {"getDataRefsByIndex", (PyCFunction)XPLMGetDataRefsByIndexFun, METH_VARARGS | METH_KEYWORDS, _getDataRefsByIndex__doc__},
  {"XPLMGetDataRefsByIndex", (PyCFunction)XPLMGetDataRefsByIndexFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"getDataRefInfo", (PyCFunction)XPLMGetDataRefInfoFun, METH_VARARGS | METH_KEYWORDS, _getDataRefInfo__doc__},
  {"XPLMGetDataRefInfo", (PyCFunction)XPLMGetDataRefInfoFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"getDataRefCallbackDict", (PyCFunction)buildDataRefCallbackDict, METH_VARARGS, "Copy of internal DataRefInfo"},
  {"getSharedDataRefCallbackDict", (PyCFunction)buildSharedDataRefCallbackDict, METH_VARARGS, "Copy of internal Shared DataRefs Dict"},
  {"_cleanup", cleanup, METH_VARARGS, ""},
  {nullptr, nullptr, 0, nullptr}
};
#pragma GCC diagnostic pop

static struct PyModuleDef XPLMDataAccessModule = {
  PyModuleDef_HEAD_INIT,
  "XPLMDataAccess",
  "Laminar documentation: \n"
  "   https://developer.x-plane.com/sdk/XPLMDataAccess/\n"
  "XPPython3 documentation: \n"
  "   https://xppython3.rtfd.io/en/stable/development/modules/dataaccess.html",
  -1,
  XPLMDataAccessMethods,
  nullptr,
  nullptr,
  nullptr,
  nullptr
};

PyMODINIT_FUNC
PyInit_XPLMDataAccess(void)
{
  PyObject *mod = PyModule_Create(&XPLMDataAccessModule);
  if(mod){
    PyModule_AddStringConstant(mod, "__author__", "Peter Buckner (pbuck@xppython3.org)");
    PyModule_AddIntConstant(mod, "xplmType_Unknown", xplmType_Unknown); // XPLMDataTypeID
    PyModule_AddIntConstant(mod, "xplmType_Int", xplmType_Int);  // XPLMDataTypeID
    PyModule_AddIntConstant(mod, "xplmType_Float", xplmType_Float);  // XPLMDataTypeID
    PyModule_AddIntConstant(mod, "xplmType_Double", xplmType_Double);  // XPLMDataTypeID
    PyModule_AddIntConstant(mod, "xplmType_FloatArray", xplmType_FloatArray);  // XPLMDataTypeID
    PyModule_AddIntConstant(mod, "xplmType_IntArray", xplmType_IntArray);  // XPLMDataTypeID
    PyModule_AddIntConstant(mod, "xplmType_Data", xplmType_Data);  // XPLMDataTypeID
    PyModule_AddIntConstant(mod, "Type_Unknown", xplmType_Unknown);  // XPLMDataTypeID
    PyModule_AddIntConstant(mod, "Type_Int", xplmType_Int);  // XPLMDataTypeID
    PyModule_AddIntConstant(mod, "Type_Float", xplmType_Float);  // XPLMDataTypeID
    PyModule_AddIntConstant(mod, "Type_Double", xplmType_Double);  // XPLMDataTypeID
    PyModule_AddIntConstant(mod, "Type_FloatArray", xplmType_FloatArray);  // XPLMDataTypeID
    PyModule_AddIntConstant(mod, "Type_IntArray", xplmType_IntArray);  // XPLMDataTypeID
    PyModule_AddIntConstant(mod, "Type_Data", xplmType_Data);  // XPLMDataTypeID
  }

  return mod;
}


