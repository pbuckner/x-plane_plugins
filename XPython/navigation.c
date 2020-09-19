#define _GNU_SOURCE 1
#include <Python.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdbool.h>
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMNavigation.h>
#include "utils.h"
#include "xppythontypes.h"

static PyObject *XPLMGetFirstNavAidFun(PyObject *self, PyObject *args)
{
  (void)self;
  (void)args;
  return PyLong_FromLong(XPLMGetFirstNavAid());
}

static PyObject *XPLMGetNextNavAidFun(PyObject *self, PyObject *args)
{
  (void)self;
  int inNavAidRef;
  if(!PyArg_ParseTuple(args, "i", &inNavAidRef)){
    return NULL;
  }
  return PyLong_FromLong(XPLMGetNextNavAid(inNavAidRef));
}

static PyObject *XPLMFindFirstNavAidOfTypeFun(PyObject *self, PyObject *args)
{
  (void)self;
  int inType;
  if(!PyArg_ParseTuple(args, "i", &inType)){
    return NULL;
  }
  return PyLong_FromLong(XPLMFindFirstNavAidOfType(inType));
}

static PyObject *XPLMFindLastNavAidOfTypeFun(PyObject *self, PyObject *args)
{
  (void)self;
  int inType;
  if(!PyArg_ParseTuple(args, "i", &inType)){
    return NULL;
  }
  return PyLong_FromLong(XPLMFindLastNavAidOfType(inType));
}

static PyObject *XPLMFindNavAidFun(PyObject *self, PyObject *args)
{
  (void)self;
  const char *inNameFragment = NULL;
  const char *inIDFragment = NULL;
  PyObject *objLat, *objLon, *objFreq;
  float lat, *inLat = NULL;
  float lon, *inLon = NULL;
  int frequency, *inFrequency = NULL;
  int inType;
  if(!PyArg_ParseTuple(args, "zzOOOi", &inNameFragment, &inIDFragment, &objLat, &objLon, &objFreq, &inType)){
    return NULL;
  }
  if(objLat != Py_None){
    lat = PyFloat_AsDouble(objLat);
    inLat = &lat;
  }
  if(objLon != Py_None){
    lon = PyFloat_AsDouble(objLon);
    inLon = &lon;
  }
  if(objFreq != Py_None){
    frequency = PyLong_AsLong(objFreq);
    inFrequency = &frequency;
  }
  return PyLong_FromLong(XPLMFindNavAid(inNameFragment, inIDFragment, inLat, inLon, inFrequency, inType));
}

static PyObject *XPLMGetNavAidInfoFun(PyObject *self, PyObject *args)
{
  (void)self;
  XPLMNavRef inRef;
  PyObject *outType, *outLatitude, *outLongitude, *outHeight, *outFrequency, *outHeading, *outID, *outName, *outReg;
  int returnValues = 0;
  if(!PyArg_ParseTuple(args, "iOOOOOOOOO", &inRef, &outType, &outLatitude, &outLongitude, &outHeight, &outFrequency, &outHeading, &outID, &outName, &outReg)){
    PyErr_Clear();
    returnValues = 1;
    if(!PyArg_ParseTuple(args, "i", &inRef)){
      return NULL;
    }
  }
  XPLMNavType type;
  float latitude = 0, longitude = 0, height = 0, heading = 0;
  int frequency = 0;
  char ID[512];
  char name[512];
  char reg[512];
  XPLMGetNavAidInfo(inRef, &type, &latitude, &longitude, &height, &frequency, &heading, ID, name, reg);
  if (returnValues) {
    return PyNavAidInfo_New(type, latitude, longitude, height, frequency, heading, ID, name, (int)reg[0]);
  }
  pythonLogWarning("XPLMGetNavAidInfo only requires navRef parameter");
  if (outType != Py_None)
    PyList_Append(outType, PyLong_FromLong(type));
  if (outLatitude != Py_None)
    PyList_Append(outLatitude, PyFloat_FromDouble((double)latitude));
  if (outLongitude != Py_None)
    PyList_Append(outLongitude, PyFloat_FromDouble((double)longitude));
  if (outHeight != Py_None)
    PyList_Append(outHeight, PyFloat_FromDouble((double)height));
  if (outHeading != Py_None)
    PyList_Append(outHeading, PyFloat_FromDouble((double)heading));
  if (outFrequency != Py_None)
    PyList_Append(outFrequency, PyLong_FromLong(frequency));
  if (outID != Py_None)
    PyList_Append(outID, PyUnicode_FromString(ID));
  if (outName != Py_None)
    PyList_Append(outName, PyUnicode_FromString(name));
  if (outReg != Py_None)
    PyList_Append(outReg, PyLong_FromLong((long)reg[0]));
  Py_RETURN_NONE;
}

static PyObject *XPLMCountFMSEntriesFun(PyObject *self, PyObject *args)
{
  (void)self;
  (void)args;
  return PyLong_FromLong(XPLMCountFMSEntries());
}

static PyObject *XPLMGetDisplayedFMSEntryFun(PyObject *self, PyObject *args)
{
  (void)self;
  (void)args;
  return PyLong_FromLong(XPLMGetDisplayedFMSEntry());
}

static PyObject *XPLMGetDestinationFMSEntryFun(PyObject *self, PyObject *args)
{
  (void)self;
  (void)args;
  return PyLong_FromLong(XPLMGetDestinationFMSEntry());
}

static PyObject *XPLMSetDisplayedFMSEntryFun(PyObject *self, PyObject *args)
{
  (void)self;
  int inIndex;
  if(!PyArg_ParseTuple(args, "i", &inIndex)){
    return NULL;
  }
  XPLMSetDisplayedFMSEntry(inIndex);
  Py_RETURN_NONE;
}

static PyObject *XPLMSetDestinationFMSEntryFun(PyObject *self, PyObject *args)
{
  (void)self;
  int inIndex;
  if(!PyArg_ParseTuple(args, "i", &inIndex)){
    return NULL;
  }
  XPLMSetDestinationFMSEntry(inIndex);
  Py_RETURN_NONE;
}

static PyObject *XPLMGetFMSEntryInfoFun(PyObject *self, PyObject *args)
{
  (void)self;
  int inIndex;
  PyObject *outType, *outID, *outRef, *outAltitude, *outLat, *outLon;
  int returnValues = 0;
  if(!PyArg_ParseTuple(args, "iOOOOOO", &inIndex, &outType, &outID, &outRef, &outAltitude, &outLat, &outLon)){
    PyErr_Clear();
    if(!PyArg_ParseTuple(args, "i", &inIndex)){
      return NULL;
    }
    returnValues = 1;
  }

  XPLMNavType type;
  float lat, lon;
  char ID[512];
  XPLMNavRef ref;
  int altitude;

  XPLMGetFMSEntryInfo(inIndex, &type, ID, &ref, &altitude, &lat, &lon);
  if (returnValues)
    return PyFMSEntryInfo_New(type, ID, ref, altitude, lat, lon);
  pythonLogWarning("XPLMGetFMSEntryInfo only requires initial index parameter");
  if (outType != Py_None)
    PyList_Append(outType, PyLong_FromLong(type));
  if (outID != Py_None)
    PyList_Append(outID, PyUnicode_FromString(ID));
  if (outRef != Py_None)
    PyList_Append(outRef, PyLong_FromLong(ref));
  if (outAltitude != Py_None)
    PyList_Append(outAltitude, PyLong_FromLong(altitude));
  if (outLat != Py_None)
    PyList_Append(outLat, PyFloat_FromDouble((double) lat));
  if (outLon != Py_None)
    PyList_Append(outLon, PyFloat_FromDouble((double) lon));
  Py_RETURN_NONE;
}

static PyObject *XPLMSetFMSEntryInfoFun(PyObject *self, PyObject *args)
{
  (void)self;
  int inIndex;
  XPLMNavRef inRef;
  int inAltitude;
  if(!PyArg_ParseTuple(args, "iii", &inIndex, &inRef, &inAltitude)){
    return NULL;
  }
  XPLMSetFMSEntryInfo(inIndex, inRef, inAltitude);
  Py_RETURN_NONE;
}

static PyObject *XPLMSetFMSEntryLatLonFun(PyObject *self, PyObject *args)
{
  (void)self;
  int inIndex;
  float inLat;
  float inLon;
  int inAltitude;
  if(!PyArg_ParseTuple(args, "iffi", &inIndex, &inLat, &inLon, &inAltitude)){
    return NULL;
  }
  XPLMSetFMSEntryLatLon(inIndex, inLat, inLon, inAltitude);
  Py_RETURN_NONE;
}

static PyObject *XPLMClearFMSEntryFun(PyObject *self, PyObject *args)
{
  (void)self;
  int inIndex;
  if(!PyArg_ParseTuple(args, "i", &inIndex)){
    return NULL;
  }
  XPLMClearFMSEntry(inIndex);
  Py_RETURN_NONE;
}

static PyObject *XPLMGetGPSDestinationTypeFun(PyObject *self, PyObject *args)
{
  (void)self;
  (void)args;
  return PyLong_FromLong(XPLMGetGPSDestinationType());
}

static PyObject *XPLMGetGPSDestinationFun(PyObject *self, PyObject *args)
{
  (void)self;
  (void)args;
  return PyLong_FromLong(XPLMGetGPSDestination());
}

static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  Py_RETURN_NONE;
}

static PyMethodDef XPLMNavigationMethods[] = {
  {"XPLMGetFirstNavAid", XPLMGetFirstNavAidFun, METH_VARARGS, ""},
  {"XPLMGetNextNavAid", XPLMGetNextNavAidFun, METH_VARARGS, ""},
  {"XPLMFindFirstNavAidOfType", XPLMFindFirstNavAidOfTypeFun, METH_VARARGS, ""},
  {"XPLMFindLastNavAidOfType", XPLMFindLastNavAidOfTypeFun, METH_VARARGS, ""},
  {"XPLMFindNavAid", XPLMFindNavAidFun, METH_VARARGS, ""},
  {"XPLMGetNavAidInfo", XPLMGetNavAidInfoFun, METH_VARARGS, ""},
  {"XPLMCountFMSEntries", XPLMCountFMSEntriesFun, METH_VARARGS, ""},
  {"XPLMGetDisplayedFMSEntry", XPLMGetDisplayedFMSEntryFun, METH_VARARGS, ""},
  {"XPLMGetDestinationFMSEntry", XPLMGetDestinationFMSEntryFun, METH_VARARGS, ""},
  {"XPLMSetDisplayedFMSEntry", XPLMSetDisplayedFMSEntryFun, METH_VARARGS, ""},
  {"XPLMSetDestinationFMSEntry", XPLMSetDestinationFMSEntryFun, METH_VARARGS, ""},
  {"XPLMGetFMSEntryInfo", XPLMGetFMSEntryInfoFun, METH_VARARGS, ""},
  {"XPLMSetFMSEntryInfo", XPLMSetFMSEntryInfoFun, METH_VARARGS, ""},
  {"XPLMSetFMSEntryLatLon", XPLMSetFMSEntryLatLonFun, METH_VARARGS, ""},
  {"XPLMClearFMSEntry", XPLMClearFMSEntryFun, METH_VARARGS, ""},
  {"XPLMGetGPSDestinationType", XPLMGetGPSDestinationTypeFun, METH_VARARGS, ""},
  {"XPLMGetGPSDestination", XPLMGetGPSDestinationFun, METH_VARARGS, ""},
  {"cleanup", cleanup, METH_VARARGS, ""},
  {NULL, NULL, 0, NULL}
};

static struct PyModuleDef XPLMNavigationModule = {
  PyModuleDef_HEAD_INIT,
  "XPLMNavigation",
  NULL,
  -1,
  XPLMNavigationMethods,
  NULL,
  NULL,
  NULL,
  NULL
};

PyMODINIT_FUNC
PyInit_XPLMNavigation(void)
{
  PyObject *mod = PyModule_Create(&XPLMNavigationModule);
  if(mod){
    PyModule_AddIntConstant(mod, "xplm_Nav_Unknown", xplm_Nav_Unknown);
    PyModule_AddIntConstant(mod, "xplm_Nav_Airport", xplm_Nav_Airport);
    PyModule_AddIntConstant(mod, "xplm_Nav_NDB", xplm_Nav_NDB);
    PyModule_AddIntConstant(mod, "xplm_Nav_VOR", xplm_Nav_VOR);
    PyModule_AddIntConstant(mod, "xplm_Nav_ILS", xplm_Nav_ILS);
    PyModule_AddIntConstant(mod, "xplm_Nav_Localizer", xplm_Nav_Localizer);
    PyModule_AddIntConstant(mod, "xplm_Nav_GlideSlope", xplm_Nav_GlideSlope);
    PyModule_AddIntConstant(mod, "xplm_Nav_OuterMarker", xplm_Nav_OuterMarker);
    PyModule_AddIntConstant(mod, "xplm_Nav_MiddleMarker", xplm_Nav_MiddleMarker);
    PyModule_AddIntConstant(mod, "xplm_Nav_InnerMarker", xplm_Nav_InnerMarker);
    PyModule_AddIntConstant(mod, "xplm_Nav_Fix", xplm_Nav_Fix);
    PyModule_AddIntConstant(mod, "xplm_Nav_DME", xplm_Nav_DME);
    PyModule_AddIntConstant(mod, "xplm_Nav_LatLon", xplm_Nav_LatLon);
    
    PyModule_AddIntConstant(mod, "XPLM_NAV_NOT_FOUND", XPLM_NAV_NOT_FOUND);
  }

  return mod;
}


