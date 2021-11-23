#define _GNU_SOURCE 1
#include <Python.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdbool.h>
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMNavigation.h>
#include "utils.h"
#include "xppythontypes.h"

My_DOCSTR(_getFirstNavAid__doc__, "getFirstNavAid", "",
          "Returns navRef of first entry in navaid database.");
static PyObject *XPLMGetFirstNavAidFun(PyObject *self, PyObject *args)
{
  (void)self;
  (void)args;
  return PyLong_FromLong(XPLMGetFirstNavAid());
}

My_DOCSTR(_getNextNavAid__doc__, "getNextNavAid", "navRef",
          "Returns next navRef after the provided value.");
static PyObject *XPLMGetNextNavAidFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"navRef", NULL};
  (void)self;
  int inNavAidRef;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "i", keywords, &inNavAidRef)){
    return NULL;
  }
  return PyLong_FromLong(XPLMGetNextNavAid(inNavAidRef));
}

My_DOCSTR(_findFirstNavAidOfType__doc__, "findFirstNavAidOfType", "navType",
          "Returns navRef of first navAid of given type.\n"
          "\n"
          "Types are:\n"
          "  Nav_Unknown        =0\n"
          "  Nav_Airport        =1\n"
          "  Nav_NDB            =2\n"
          "  Nav_VOR            =4\n"
          "  Nav_ILS            =8\n"
          "  Nav_Localizer     =16\n"
          "  Nav_GlideSlope    =32\n"
          "  Nav_OuterMarker   =64\n"
          "  Nav_MiddleMarker =128\n"
          "  Nav_InnerMarker  =256\n"
          "  Nav_Fix          =512\n"
          "  Nav_DME         =1024\n"
          "  Nav_LatLon      =2048"
          );
static PyObject *XPLMFindFirstNavAidOfTypeFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"navType", NULL};
  (void)self;
  int inType;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "i", keywords, &inType)){
    return NULL;
  }
  return PyLong_FromLong(XPLMFindFirstNavAidOfType(inType));
}

My_DOCSTR(_findLastNavAidOfType__doc__, "findLastNavAidOfType", "navType",
          "Returns navRef of last navAid of given type.");
static PyObject *XPLMFindLastNavAidOfTypeFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"navType", NULL};
  (void)self;
  int inType;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "i", keywords, &inType)){
    return NULL;
  }
  return PyLong_FromLong(XPLMFindLastNavAidOfType(inType));
}

My_DOCSTR(_findNavAid__doc__, "findNavAid", "name=None, navAidID=None, lat=None, lon=None, freq=None, navType=-1",
          "Returns navRef of last navAid matching information.\n"
          "\n"
          "name and navAidID are case-sensitive and will match a fragment of the actual value.\n"
          "freq is an integer, 100x the real frequency value (eg. 137.75 -> 13775) except for NDB.\n"
          "navType=-1 will match any value");
static PyObject *XPLMFindNavAidFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"name", "navAidID", "lat", "lon", "freq", "navType", NULL};
  (void)self;
  const char *inNameFragment = NULL;
  const char *inIDFragment = NULL;
  PyObject *objLat=Py_None, *objLon=Py_None, *objFreq=Py_None;
  float lat, *inLat = NULL;
  float lon, *inLon = NULL;
  int frequency, *inFrequency = NULL;
  int inType=-1;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|zzOOOi", keywords, &inNameFragment, &inIDFragment, &objLat, &objLon, &objFreq, &inType)){
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
  if (inType == -1) {
    inType = 0xffffffff;
  }
  return PyLong_FromLong(XPLMFindNavAid(inNameFragment, inIDFragment, inLat, inLon, inFrequency, inType));
}

My_DOCSTR(_getNavAidInfo__doc__, "getNavAidInfo", "navRef",
          "Returns NavAidInfo object for given navRef\n"
          "\n"
          "Attibutes are:\n"
          " .type        # a NavType\n"
          " .latitude\n"
          " .longitude\n"
          " .height     # in meters\n"
          " .frequency  # integer, for NDB, value is exact, otherwise devide by 100.0\n"
          " .heading    # See documentation for glideslope headings\n"
          " .navAidID\n"
          " .reg        # =1 if navaid is within local \"region\" of loaded DSFs");
static PyObject *XPLMGetNavAidInfoFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"navRef", NULL};
  (void)self;
  XPLMNavRef inRef;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "i", keywords, &inRef)){
    return NULL;
  }
  XPLMNavType type;
  float latitude = 0, longitude = 0, height = 0, heading = 0;
  int frequency = 0;
  char ID[512];
  char name[512];
  char reg[512];
  XPLMGetNavAidInfo(inRef, &type, &latitude, &longitude, &height, &frequency, &heading, ID, name, reg);
  return PyNavAidInfo_New(type, latitude, longitude, height, frequency, heading, ID, name, (int)reg[0]);
}

My_DOCSTR(_countFMSEntries__doc__, "countFMSEntries", "",
          "Returns number of FMS Entries");
static PyObject *XPLMCountFMSEntriesFun(PyObject *self, PyObject *args)
{
  (void)self;
  (void)args;
  return PyLong_FromLong(XPLMCountFMSEntries());
}

My_DOCSTR(_getDisplayedFMSEntry__doc__, "getDisplayedFMSEntry", "",
          "Returns index number of currently displayed FMS entry.");
static PyObject *XPLMGetDisplayedFMSEntryFun(PyObject *self, PyObject *args)
{
  (void)self;
  (void)args;
  return PyLong_FromLong(XPLMGetDisplayedFMSEntry());
}

My_DOCSTR(_getDestinationFMSEntry__doc__, "getDestinationFMSEntry", "",
          "Returns index number of destination FMS entry.");
static PyObject *XPLMGetDestinationFMSEntryFun(PyObject *self, PyObject *args)
{
  (void)self;
  (void)args;
  return PyLong_FromLong(XPLMGetDestinationFMSEntry());
}

My_DOCSTR(_setDisplayedFMSEntry__doc__, "setDisplayedFMSEntry", "index",
          "Sets index number for FMS Entry to be displayed.");
static PyObject *XPLMSetDisplayedFMSEntryFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"index", NULL};
  (void)self;
  int inIndex;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "i", keywords, &inIndex)){
    return NULL;
  }
  XPLMSetDisplayedFMSEntry(inIndex);
  Py_RETURN_NONE;
}

My_DOCSTR(_setDestinationFMSEntry__doc__, "setDestinationFMSEntry", "index",
          "Sets index number for FMS Entry to become the current destination.");
static PyObject *XPLMSetDestinationFMSEntryFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"index", NULL};
  (void)self;
  int inIndex;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "i", keywords, &inIndex)){
    return NULL;
  }
  XPLMSetDestinationFMSEntry(inIndex);
  Py_RETURN_NONE;
}

My_DOCSTR(_getFMSEntryInfo__doc__, "getFMSEntryInfo", "index",
          "Return FMSEntryInfo object for given FMS Entry index.\n"
          "\n"
          "Attributes are:\n"
          " .type      # a NavType\n"
          " .navAidID\n"
          " .ref       # navRef (use as input to getNavAidInfo())\n"
          " .altitude  # (in feet)\n"
          " .lat\n"
          " .lon");
static PyObject *XPLMGetFMSEntryInfoFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"index", NULL};
  (void)self;
  int inIndex;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "i", keywords, &inIndex)){
    return NULL;
  }

  XPLMNavType type;
  float lat, lon;
  char ID[512];
  XPLMNavRef ref;
  int altitude;

  XPLMGetFMSEntryInfo(inIndex, &type, ID, &ref, &altitude, &lat, &lon);
  return PyFMSEntryInfo_New(type, ID, ref, altitude, lat, lon);
}

My_DOCSTR(_setFMSEntryInfo__doc__, "setFMSEntryInfo", "index, navRef, altitude=0",
          "Set given FMS Entry to provided navRef and altitude (feet)");
static PyObject *XPLMSetFMSEntryInfoFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"index", "navRef", "altitude", NULL};
  (void)self;
  int inIndex;
  XPLMNavRef inRef;
  int inAltitude=0;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "ii|i", keywords, &inIndex, &inRef, &inAltitude)){
    return NULL;
  }
  XPLMSetFMSEntryInfo(inIndex, inRef, inAltitude);
  Py_RETURN_NONE;
}

My_DOCSTR(_setFMSEntryLatLon__doc__, "setFMSEntryLatLon", "index, lat, lon, altitude=0",
          "Set given FMS Entry to provided (lat, lon) and altitude(feet).");
static PyObject *XPLMSetFMSEntryLatLonFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"index", "lat", "lon", "altitude", NULL};
  (void)self;
  int inIndex;
  float inLat;
  float inLon;
  int inAltitude =0;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "iff|i", keywords, &inIndex, &inLat, &inLon, &inAltitude)){
    return NULL;
  }
  XPLMSetFMSEntryLatLon(inIndex, inLat, inLon, inAltitude);
  Py_RETURN_NONE;
}

My_DOCSTR(_clearFMSEntry__doc__, "clearFMSEntry", "index",
          "Clear given FMS entry.");
static PyObject *XPLMClearFMSEntryFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"index", NULL};
  (void)self;
  int inIndex;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "i", keywords, &inIndex)){
    return NULL;
  }
  XPLMClearFMSEntry(inIndex);
  Py_RETURN_NONE;
}

My_DOCSTR(_getGPSDestinationType__doc__, "getGPSDestinationType", "",
          "Return NavType of current GPS Destination.");
static PyObject *XPLMGetGPSDestinationTypeFun(PyObject *self, PyObject *args)
{
  (void)self;
  (void)args;
  return PyLong_FromLong(XPLMGetGPSDestinationType());
}

My_DOCSTR(_getGPSDestination__doc__, "getGPSDestination", "",
          "Return navRef of current GPS Destination.");
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
  {"getFirstNavAid", (PyCFunction)XPLMGetFirstNavAidFun, METH_VARARGS, _getFirstNavAid__doc__},
  {"XPLMGetFirstNavAid", (PyCFunction)XPLMGetFirstNavAidFun, METH_VARARGS, ""},
  {"getNextNavAid", (PyCFunction)XPLMGetNextNavAidFun, METH_VARARGS | METH_KEYWORDS, _getNextNavAid__doc__},
  {"XPLMGetNextNavAid", (PyCFunction)XPLMGetNextNavAidFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"findFirstNavAidOfType", (PyCFunction)XPLMFindFirstNavAidOfTypeFun, METH_VARARGS | METH_KEYWORDS, _findFirstNavAidOfType__doc__},
  {"XPLMFindFirstNavAidOfType", (PyCFunction)XPLMFindFirstNavAidOfTypeFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"findLastNavAidOfType", (PyCFunction)XPLMFindLastNavAidOfTypeFun, METH_VARARGS | METH_KEYWORDS, _findLastNavAidOfType__doc__},
  {"XPLMFindLastNavAidOfType", (PyCFunction)XPLMFindLastNavAidOfTypeFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"findNavAid", (PyCFunction)XPLMFindNavAidFun, METH_VARARGS | METH_KEYWORDS, _findNavAid__doc__},
  {"XPLMFindNavAid", (PyCFunction)XPLMFindNavAidFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"getNavAidInfo", (PyCFunction)XPLMGetNavAidInfoFun, METH_VARARGS | METH_KEYWORDS, _getNavAidInfo__doc__},
  {"XPLMGetNavAidInfo", (PyCFunction)XPLMGetNavAidInfoFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"countFMSEntries", (PyCFunction)XPLMCountFMSEntriesFun, METH_VARARGS, _countFMSEntries__doc__},
  {"XPLMCountFMSEntries", (PyCFunction)XPLMCountFMSEntriesFun, METH_VARARGS, ""},
  {"getDisplayedFMSEntry", (PyCFunction)XPLMGetDisplayedFMSEntryFun, METH_VARARGS, _getDisplayedFMSEntry__doc__},
  {"XPLMGetDisplayedFMSEntry", (PyCFunction)XPLMGetDisplayedFMSEntryFun, METH_VARARGS, ""},
  {"getDestinationFMSEntry", (PyCFunction)XPLMGetDestinationFMSEntryFun, METH_VARARGS, _getDestinationFMSEntry__doc__},
  {"XPLMGetDestinationFMSEntry", (PyCFunction)XPLMGetDestinationFMSEntryFun, METH_VARARGS, ""},
  {"setDisplayedFMSEntry", (PyCFunction)XPLMSetDisplayedFMSEntryFun, METH_VARARGS | METH_KEYWORDS, _setDisplayedFMSEntry__doc__},
  {"XPLMSetDisplayedFMSEntry", (PyCFunction)XPLMSetDisplayedFMSEntryFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"setDestinationFMSEntry", (PyCFunction)XPLMSetDestinationFMSEntryFun, METH_VARARGS | METH_KEYWORDS, _setDestinationFMSEntry__doc__},
  {"XPLMSetDestinationFMSEntry", (PyCFunction)XPLMSetDestinationFMSEntryFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"getFMSEntryInfo", (PyCFunction)XPLMGetFMSEntryInfoFun, METH_VARARGS | METH_KEYWORDS, _getFMSEntryInfo__doc__},
  {"XPLMGetFMSEntryInfo", (PyCFunction)XPLMGetFMSEntryInfoFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"setFMSEntryInfo", (PyCFunction)XPLMSetFMSEntryInfoFun, METH_VARARGS | METH_KEYWORDS, _setFMSEntryInfo__doc__},
  {"XPLMSetFMSEntryInfo", (PyCFunction)XPLMSetFMSEntryInfoFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"setFMSEntryLatLon", (PyCFunction)XPLMSetFMSEntryLatLonFun, METH_VARARGS | METH_KEYWORDS, _setFMSEntryLatLon__doc__},
  {"XPLMSetFMSEntryLatLon", (PyCFunction)XPLMSetFMSEntryLatLonFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"clearFMSEntry", (PyCFunction)XPLMClearFMSEntryFun, METH_VARARGS | METH_KEYWORDS, _clearFMSEntry__doc__},
  {"XPLMClearFMSEntry", (PyCFunction)XPLMClearFMSEntryFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"getGPSDestinationType", (PyCFunction)XPLMGetGPSDestinationTypeFun, METH_VARARGS, _getGPSDestinationType__doc__},
  {"XPLMGetGPSDestinationType", (PyCFunction)XPLMGetGPSDestinationTypeFun, METH_VARARGS, ""},
  {"getGPSDestination", (PyCFunction)XPLMGetGPSDestinationFun, METH_VARARGS, _getGPSDestination__doc__},
  {"XPLMGetGPSDestination", (PyCFunction)XPLMGetGPSDestinationFun, METH_VARARGS, ""},
  {"_cleanup", cleanup, METH_VARARGS, ""},
  {NULL, NULL, 0, NULL}
};

static struct PyModuleDef XPLMNavigationModule = {
  PyModuleDef_HEAD_INIT,
  "XPLMNavigation",
  "Laminar documentation: \n"
  "   https://developer.x-plane.com/sdk/XPLMNavigation/\n"
  "XPPython3 documentation: \n"
  "   https://xppython3.rtfd.io/en/stable/development/modules/navigation.html",
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
    PyModule_AddStringConstant(mod, "__author__", "Peter Buckner (xppython3@avnwx.com)");
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

    PyModule_AddIntConstant(mod, "Nav_Unknown", xplm_Nav_Unknown);
    PyModule_AddIntConstant(mod, "Nav_Airport", xplm_Nav_Airport);
    PyModule_AddIntConstant(mod, "Nav_NDB", xplm_Nav_NDB);
    PyModule_AddIntConstant(mod, "Nav_VOR", xplm_Nav_VOR);
    PyModule_AddIntConstant(mod, "Nav_ILS", xplm_Nav_ILS);
    PyModule_AddIntConstant(mod, "Nav_Localizer", xplm_Nav_Localizer);
    PyModule_AddIntConstant(mod, "Nav_GlideSlope", xplm_Nav_GlideSlope);
    PyModule_AddIntConstant(mod, "Nav_OuterMarker", xplm_Nav_OuterMarker);
    PyModule_AddIntConstant(mod, "Nav_MiddleMarker", xplm_Nav_MiddleMarker);
    PyModule_AddIntConstant(mod, "Nav_InnerMarker", xplm_Nav_InnerMarker);
    PyModule_AddIntConstant(mod, "Nav_Fix", xplm_Nav_Fix);
    PyModule_AddIntConstant(mod, "Nav_DME", xplm_Nav_DME);
    PyModule_AddIntConstant(mod, "Nav_LatLon", xplm_Nav_LatLon);
    
    PyModule_AddIntConstant(mod, "NAV_NOT_FOUND", XPLM_NAV_NOT_FOUND);
  }

  return mod;
}


