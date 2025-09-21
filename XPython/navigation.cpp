#define _GNU_SOURCE 1
#include <Python.h>
#include <sys/time.h>
#include <stdio.h>
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMNavigation.h>
#include <vector>
#include <string>
#include "plugin_dl.h"
#include "utils.h"
#include "xppythontypes.h"
#include "cpp_utilities.hpp"

My_DOCSTR(_getFirstNavAid__doc__, "getFirstNavAid",
          "",
          "",
          "int",
          "Returns navRef of first entry in navaid database.");
static PyObject *XPLMGetFirstNavAidFun(PyObject *self, PyObject *args)
{
  (void)self;
  (void)args;
  return PyLong_FromLong(XPLMGetFirstNavAid());
}

My_DOCSTR(_getNextNavAid__doc__, "getNextNavAid",
          "navRef",
          "navRef:XPLMNavRef",
          "int",
          "Returns next navRef after the provided value.");
static PyObject *XPLMGetNextNavAidFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("navRef"), nullptr};
  (void)self;
  int inNavAidRef;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "i", keywords, &inNavAidRef)){
    return nullptr;
  }
  return PyLong_FromLong(XPLMGetNextNavAid(inNavAidRef));
}

My_DOCSTR(_findFirstNavAidOfType__doc__, "findFirstNavAidOfType",
          "navType",
          "navType:XPLMNavType",
          "int",
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
          "  Nav_LatLon      =2048\n"
          "  Nav_TACAN       =4096\n"
          );
static PyObject *XPLMFindFirstNavAidOfTypeFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("navType"), nullptr};
  (void)self;
  int inType;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "i", keywords, &inType)){
    return nullptr;
  }
  return PyLong_FromLong(XPLMFindFirstNavAidOfType(inType));
}

My_DOCSTR(_findLastNavAidOfType__doc__, "findLastNavAidOfType",
          "navType",
          "navType:XPLMNavType",
          "int",
          "Returns navRef of last navAid of given type.");
static PyObject *XPLMFindLastNavAidOfTypeFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("navType"), nullptr};
  (void)self;
  int inType;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "i", keywords, &inType)){
    return nullptr;
  }
  return PyLong_FromLong(XPLMFindLastNavAidOfType(inType));
}

My_DOCSTR(_findNavAid__doc__, "findNavAid",
          "name=None, navAidID=None, lat=None, lon=None, freq=None, navType=0xffffffff",
          "name:Optional[str], navAidID:Optional[str], lat:Optional[float], "
          "lon:Optional[float], freq:Optional[int], navType:XPLMNavType=Nav_Any",
          "XPLMNavRef",
          "Returns navRef of last navAid matching information.\n"
          "\n"
          "name and navAidID are case-sensitive and will match a fragment of the actual value.\n"
          "freq is an integer, 100x the real frequency value (eg. 137.75 -> 13775) except for NDB.\n"
          "navType=0xffffffff will match any type value");
static PyObject *XPLMFindNavAidFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("name"), CHAR("navAidID"), CHAR("lat"), CHAR("lon"), CHAR("freq"), CHAR("navType"), nullptr};
  (void)self;
  const char *inNameFragment = nullptr;
  const char *inIDFragment = nullptr;
  PyObject *objLat=Py_None, *objLon=Py_None, *objFreq=Py_None;
  float lat, *inLat = nullptr;
  float lon, *inLon = nullptr;
  int frequency, *inFrequency = nullptr;
  int inType=0xffffff;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|zzOOOi", keywords, &inNameFragment, &inIDFragment, &objLat, &objLon, &objFreq, &inType)){
    return nullptr;
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

My_DOCSTR(_getNavAidInfo__doc__, "getNavAidInfo",
          "navRef",
          "navRef:XPLMNavRef",
          "NavAidInfo",
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
          " .reg        # =1 if navaid is within local 'region' of loaded DSFs");
static PyObject *XPLMGetNavAidInfoFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("navRef"), nullptr};
  (void)self;
  XPLMNavRef inRef;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "i", keywords, &inRef)){
    return nullptr;
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

My_DOCSTR(_countFMSEntries__doc__, "countFMSEntries",
          "",
          "",
          "int",
          "Returns number of FMS Entries");
static PyObject *XPLMCountFMSEntriesFun(PyObject *self, PyObject *args)
{
  (void)self;
  (void)args;
  return PyLong_FromLong(XPLMCountFMSEntries());
}

My_DOCSTR(_getDisplayedFMSEntry__doc__, "getDisplayedFMSEntry",
          "",
          "",
          "int",
          "Returns index number of currently displayed FMS entry.");
static PyObject *XPLMGetDisplayedFMSEntryFun(PyObject *self, PyObject *args)
{
  (void)self;
  (void)args;
  return PyLong_FromLong(XPLMGetDisplayedFMSEntry());
}

My_DOCSTR(_getDestinationFMSEntry__doc__, "getDestinationFMSEntry",
          "",
          "",
          "int",
          "Returns index number of destination FMS entry.");
static PyObject *XPLMGetDestinationFMSEntryFun(PyObject *self, PyObject *args)
{
  (void)self;
  (void)args;
  return PyLong_FromLong(XPLMGetDestinationFMSEntry());
}

My_DOCSTR(_setDisplayedFMSEntry__doc__, "setDisplayedFMSEntry",
          "index",
          "index:int",
          "None",
          "Sets index number for FMS Entry to be displayed.");
static PyObject *XPLMSetDisplayedFMSEntryFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("index"), nullptr};
  (void)self;
  int inIndex;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "i", keywords, &inIndex)){
    return nullptr;
  }
  XPLMSetDisplayedFMSEntry(inIndex);
  Py_RETURN_NONE;
}

My_DOCSTR(_setDestinationFMSEntry__doc__, "setDestinationFMSEntry",
          "index",
          "index:int",
          "None",
          "Sets index number for FMS Entry to become the current destination.");
static PyObject *XPLMSetDestinationFMSEntryFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("index"), nullptr};
  (void)self;
  int inIndex;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "i", keywords, &inIndex)){
    return nullptr;
  }
  XPLMSetDestinationFMSEntry(inIndex);
  Py_RETURN_NONE;
}

My_DOCSTR(_getFMSEntryInfo__doc__, "getFMSEntryInfo",
          "index",
          "index:int",
          "FMSEntryInfo",
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
  static char *keywords[] = {CHAR("index"), nullptr};
  (void)self;
  int inIndex;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "i", keywords, &inIndex)){
    return nullptr;
  }

  XPLMNavType type;
  float lat, lon;
  char ID[512];
  XPLMNavRef ref;
  int altitude;

  XPLMGetFMSEntryInfo(inIndex, &type, ID, &ref, &altitude, &lat, &lon);
  return PyFMSEntryInfo_New(type, ID, ref, altitude, lat, lon);
}

My_DOCSTR(_setFMSEntryInfo__doc__, "setFMSEntryInfo",
          "index, navRef, altitude=0",
          "index:int, navRef:XPLMNavRef, altitude:int=0",
          "None",
          "Set given FMS Entry to provided navRef and altitude (feet)");
static PyObject *XPLMSetFMSEntryInfoFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("index"), CHAR("navRef"), CHAR("altitude"), nullptr};
  (void)self;
  int inIndex;
  XPLMNavRef inRef;
  int inAltitude=0;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "ii|i", keywords, &inIndex, &inRef, &inAltitude)){
    return nullptr;
  }
  XPLMSetFMSEntryInfo(inIndex, inRef, inAltitude);
  Py_RETURN_NONE;
}

My_DOCSTR(_setFMSEntryLatLon__doc__, "setFMSEntryLatLon",
          "index, lat, lon, altitude=0",
          "index:int, lat:float, lon:float, altitude:int=0",
          "None",
          "Set given FMS Entry to provided (lat, lon) and altitude(feet).");
static PyObject *XPLMSetFMSEntryLatLonFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("index"), CHAR("lat"), CHAR("lon"), CHAR("altitude"), nullptr};
  (void)self;
  int inIndex;
  float inLat;
  float inLon;
  int inAltitude =0;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "iff|i", keywords, &inIndex, &inLat, &inLon, &inAltitude)){
    return nullptr;
  }
  XPLMSetFMSEntryLatLon(inIndex, inLat, inLon, inAltitude);
  Py_RETURN_NONE;
}

My_DOCSTR(_clearFMSEntry__doc__, "clearFMSEntry",
          "index",
          "index:int",
          "None",
          "Clear given FMS entry.");
static PyObject *XPLMClearFMSEntryFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("index"), nullptr};
  (void)self;
  int inIndex;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "i", keywords, &inIndex)){
    return nullptr;
  }
  XPLMClearFMSEntry(inIndex);
  Py_RETURN_NONE;
}

My_DOCSTR(_getGPSDestinationType__doc__, "getGPSDestinationType",
          "",
          "",
          "int",
          "Return NavType of current GPS Destination.");
static PyObject *XPLMGetGPSDestinationTypeFun(PyObject *self, PyObject *args)
{
  (void)self;
  (void)args;
  return PyLong_FromLong(XPLMGetGPSDestinationType());
}

My_DOCSTR(_getGPSDestination__doc__, "getGPSDestination",
          "",
          "",
          "int",
          "Return navRef of current GPS Destination.");
static PyObject *XPLMGetGPSDestinationFun(PyObject *self, PyObject *args)
{
  (void)self;
  (void)args;
  return PyLong_FromLong(XPLMGetGPSDestination());
}

/* SDK410 FMSFlightPlan routines */
My_DOCSTR(_countFMSFlightPlanEntries__doc__, "countFMSFlightPlanEntries",
          "flightPlan",
          "flightPlan:XPLMNavFlightPlan",
          "int",
          "Returns number of FMS Entries in given flightplan:\n"
          "  Fpl_Pilot_Primary\n"
          "  Fpl_CoPilot_Primary\n"
          "  Fpl_Pilot_Approach\n"
          "  Fpl_CoPilot_Approach\n"
          "  Fpl_Pilot_Temporary\n"
          "  Fpl_CoPilot_Temporary\n"
);
static PyObject *XPLMCountFMSFlightPlanEntriesFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("flightPlan"), nullptr};
  (void)self;
  int inFlightPlan;
  if (!XPLMCountFMSFlightPlanEntries_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMCountFMSFlightPlanEntries is available only in XPLM410 and up.");
    return nullptr;
  }

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "i", keywords, &inFlightPlan)){
    return nullptr;
  }

  return PyLong_FromLong(XPLMCountFMSFlightPlanEntries_ptr(inFlightPlan));
}


My_DOCSTR(_getDisplayedFMSFlightPlanEntry__doc__, "getDisplayedFMSFlightPlanEntry",
          "flightPlan",
          "flightPlan:XPLMNavFlightPlan",
          "int",
          "Returns index number of the entry the pilot is viewing.");
static PyObject *XPLMGetDisplayedFMSFlightPlanEntryFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("flightPlan"), nullptr};
  (void)self;
  if (!XPLMGetDisplayedFMSFlightPlanEntry_ptr) {
    PyErr_SetString(PyExc_RuntimeError, "XPLMGetDisplayedFMSFlightPlanEntry is available only in XPLM410 and up.");
    return nullptr;
  }

  int inFlightPlan;

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "i", keywords, &inFlightPlan)){
    return nullptr;
  }
  return PyLong_FromLong(XPLMGetDisplayedFMSFlightPlanEntry_ptr(inFlightPlan));

}

My_DOCSTR(_getDestinationFMSFlightPlanEntry__doc__, "getDestinationFMSFlightPlanEntry",
          "flightPlan",
          "flightPlan:XPLMNavFlightPlan",
          "int",
          "Returns the index number of the flight plan destination.");
static PyObject *XPLMGetDestinationFMSFlightPlanEntryFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("flightPlan"), nullptr};
  (void)self;
  if (!XPLMGetDestinationFMSFlightPlanEntry_ptr) {
    PyErr_SetString(PyExc_RuntimeError, "XPLMGetDestinationFMSFlightPlanEntry is available only in XPLM410 and up.");
    return nullptr;
  }
  int inFlightPlan;

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "i", keywords, &inFlightPlan)){
    return nullptr;
  }
  return PyLong_FromLong(XPLMGetDestinationFMSFlightPlanEntry_ptr(inFlightPlan));
}

My_DOCSTR(_setDisplayedFMSFlightPlanEntry__doc__, "setDisplayedFMSFlightPlanEntry",
          "flightPlan, index",
          "flightPlan:XPLMNavFlightPlan, index:int",
          "None",
          "Sets index number for FMS Entry to be displayed for this flight plan.");
static PyObject *XPLMSetDisplayedFMSFlightPlanEntryFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("flightPlan"), CHAR("index"), nullptr};
  (void)self;
  if (!XPLMSetDisplayedFMSFlightPlanEntry_ptr) {
    PyErr_SetString(PyExc_RuntimeError, "XPLMSetDisplayedFMSFlightPlanEntry is available only in XPLM410 and up.");
    return nullptr;
  }
  int inFlightPlan;
  int index;

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "ii", keywords, &inFlightPlan, &index)){
    return nullptr;
  }

  XPLMSetDisplayedFMSFlightPlanEntry_ptr(inFlightPlan, index);
  Py_RETURN_NONE;
}

My_DOCSTR(_setDestinationFMSFlightPlanEntry__doc__, "setDestinationFMSFlightPlanEntry",
          "flightPlan, index",
          "flightPlan:XPLMNavFlightPlan, index:int",
          "None",
          "Sets index number for the FMS Entry to become the destination. The track\n"
          "is from the n-1'th point to this point.");
static PyObject *XPLMSetDestinationFMSFlightPlanEntryFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("flightPlan"), CHAR("index"), nullptr};
  (void)self;
  if (!XPLMSetDestinationFMSFlightPlanEntry_ptr) {
    PyErr_SetString(PyExc_RuntimeError, "XPLMSetDestinationFMSFlightPlanEntry is available only in XPLM410 and up.");
    return nullptr;
  }
  int inFlightPlan;
  int index;

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "ii", keywords, &inFlightPlan, &index)){
    return nullptr;
  }
  XPLMSetDestinationFMSFlightPlanEntry_ptr(inFlightPlan, index);
  Py_RETURN_NONE;
}

My_DOCSTR(_setDirectToFMSFlightPlanEntry__doc__, "setDirectToFMSFlightPlanEntry",
          "flightPlan, index",
          "flightPlan:XPLMNavFlightPlan, index:int",
          "None",
          "Sets the entry the FMS is flying toward. The track is from the current\n"
          "position directly to this point, ignoring the point before it in the flight plan.");
static PyObject *XPLMSetDirectToFMSFlightPlanEntryFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("flightPlan"), CHAR("index"), nullptr};
  (void)self;
  if (!XPLMSetDirectToFMSFlightPlanEntry_ptr) {
    PyErr_SetString(PyExc_RuntimeError, "XPLMSetDirectToFMSFlightPlanEntry is available only in XPLM410 and up.");
    return nullptr;
  }

  int inFlightPlan;
  int index;

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "ii", keywords, &inFlightPlan, &index)){
    return nullptr;
  }

  XPLMSetDirectToFMSFlightPlanEntry_ptr(inFlightPlan, index);
  Py_RETURN_NONE;
}

My_DOCSTR(_getFMSFlightPlanEntryInfo__doc__, "getFMSFlightPlanEntryInfo",
          "flightPlan, index",
          "flightPlan:XPLMNavFlightPlan, index:int",
          "FMSEntryInfo",
          "Returns FMSEntryInfo object for give FMS index for this flightplan.\n"
          "\n"
          "Attributes are:\n"
          " .type      # a NavType\n"
          " .navAidID\n"
          " .ref       # navRef (use as input to getNavAidInfo())\n"
          " .altitude  # (in feet)\n"
          " .lat\n"
          " .lon");
static PyObject *XPLMGetFMSFlightPlanEntryInfoFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("flightPlan"), CHAR("index"), nullptr};
  (void)self;
  if (!XPLMGetFMSFlightPlanEntryInfo_ptr) {
    PyErr_SetString(PyExc_RuntimeError, "XPLMGetFMSFlightPlanEntryInfo is available only in XPLM410 and up.");
    return nullptr;
  }

  int inFlightPlan;
  int index;

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "ii", keywords, &inFlightPlan, &index)){
    return nullptr;
  }
  XPLMNavType type = xplm_Nav_Unknown;
  float lat, lon;
  char ID[512];
  XPLMNavRef ref = XPLM_NAV_NOT_FOUND; /* due to bug prior to 11.31, reference is not set, when no data is available, so always pre-set this */
  int altitude;
  
  XPLMGetFMSFlightPlanEntryInfo_ptr(inFlightPlan, index, &type, ID, &ref, &altitude, &lat, &lon);
  return PyFMSEntryInfo_New(type, ID, ref, altitude, lat, lon);
}

My_DOCSTR(_setFMSFlightPlanEntryInfo__doc__, "setFMSFlightPlanEntryInfo",
          "flightPlan, index, navRef, altitude=0",
          "flightPlan:XPLMNavFlightPlan, index:int, navRef:XPLMNavRef, altitude:int=0",
          "None",
          "Sets given FMS entry to provided navRef and altitude (feet).");
static PyObject *XPLMSetFMSFlightPlanEntryInfoFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("flightPlan"), CHAR("index"), CHAR("navRef"), CHAR("altitude"), nullptr};
  (void)self;
  if (!XPLMSetFMSFlightPlanEntryInfo_ptr) {
    PyErr_SetString(PyExc_RuntimeError, "XPLMSetFMSFlightPlanEntryInfo is available only in XPLM410 and up.");
    return nullptr;
  }

  int inFlightPlan;
  int index;
  XPLMNavRef inRef;
  int inAltitude=0;

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "iii|i", keywords, &inFlightPlan, &index, &inRef, &inAltitude)){
    return nullptr;
  }
  XPLMSetFMSFlightPlanEntryInfo_ptr(inFlightPlan, index, inRef, inAltitude);
  Py_RETURN_NONE;

}

My_DOCSTR(_setFMSFlightPlanEntryLatLon__doc__, "setFMSFlightPlanEntryLatLon",
          "flightPlan, index, lat, lon, altitude=0",
          "flightPlan:XPLMNavFlightPlan, index:int, lat:float, lon:float, altitude:int=0",
          "None",
          "Set given FMS Entry to provided (lat, lon) and altitude(feet).");
static PyObject *XPLMSetFMSFlightPlanEntryLatLonFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("flightPlan"), CHAR("index"), CHAR("lat"), CHAR("lon"), CHAR("altitude"), nullptr};
  (void)self;
  if (!XPLMSetFMSFlightPlanEntryLatLon_ptr) {
    PyErr_SetString(PyExc_RuntimeError, "XPLMSetFMSFlightPlanEntryLatLon is available only in XPLM410 and up.");
    return nullptr;
  }

  int index;
  float inLat;
  float inLon;
  int inAltitude =0;
  int inFlightPlan;

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "iiff|i", keywords, &inFlightPlan, &index, &inLat, &inLon, &inAltitude)){
    return nullptr;
  }
  XPLMSetFMSFlightPlanEntryLatLon_ptr(inFlightPlan, index, inLat, inLon, inAltitude);
  Py_RETURN_NONE;
}

My_DOCSTR(_setFMSFlightPlanEntryLatLonWithId__doc__, "setFMSFlightPlanEntryLatLonWithId",
          "flightPlan, index, lat, lon, altitude=0, ID=None",
          "flightPlan:XPLMNavFlightPlan, index:int, lat:float, lon:float, altitude:int=0, ID:str=None",
          "None",
          "Sets entry in the FMS to a lat/lon entry, with the given coordinates\n"
          "and display ID for the waypoint.");
static PyObject *XPLMSetFMSFlightPlanEntryLatLonWithIdFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("flightPlan"), CHAR("index"), CHAR("lat"), CHAR("lon"), CHAR("altitude"), CHAR("ID"), nullptr};
  (void)self;
  if (!XPLMSetFMSFlightPlanEntryLatLonWithId_ptr) {
    PyErr_SetString(PyExc_RuntimeError, "XPLMSetFMSFlightPlanEntryLatLonWithId is available only in XPLM410 and up.");
    return nullptr;
  }

  int index;
  float inLat;
  float inLon;
  int inAltitude =0;
  int inFlightPlan;
  const char *inID = "Lat/Lon";

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "iiff|iz", keywords, &inFlightPlan, &index, &inLat, &inLon, &inAltitude, &inID)){
    return nullptr;
  }
  unsigned int length = strlen(inID);
  XPLMSetFMSFlightPlanEntryLatLonWithId_ptr(inFlightPlan, index, inLat, inLon, inAltitude, inID, length);
  Py_RETURN_NONE;
}

My_DOCSTR(_clearFMSFlightPlanEntry__doc__, "clearFMSFlightPlanEntry",
          "flightPlan, index",
          "flightPlan:XPLMNavFlightPlan, index:int",
          "None",
          "Clears given FMS Entry.");
static PyObject *XPLMClearFMSFlightPlanEntryFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("flightPlan"), CHAR("index"), nullptr};
  (void)self;
  if (!XPLMClearFMSFlightPlanEntry_ptr) {
    PyErr_SetString(PyExc_RuntimeError, "XPLMClearFMSFlightPlanEntry is available only in XPLM410 and up.");
    return nullptr;
  }

  int inFlightPlan;
  int index;

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "ii", keywords, &inFlightPlan, &index)){
    return nullptr;
  }
  XPLMClearFMSFlightPlanEntry_ptr(inFlightPlan, index);
  Py_RETURN_NONE;
}

My_DOCSTR(_loadFMSFlightPlan__doc__, "loadFMSFlightPlan",
          "device, plan",
          "device:int, plan:str",
          "None",
          "Loads provided flightplan into pilot (device=0) or co-pilot (device=1) unit.");
static PyObject *XPLMLoadFMSFlightPlanFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("device"), CHAR("plan"), nullptr};
  (void)self;
  if (!XPLMLoadFMSFlightPlan_ptr) {
    PyErr_SetString(PyExc_RuntimeError, "XPLMLoadFMSFlightPlan is available only in XPLM410 and up.");
    return nullptr;
  }

  int device;
  char *flightPlan;

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "iz", keywords, &device, &flightPlan)){
    return nullptr;
  }
  XPLMLoadFMSFlightPlan_ptr(device, flightPlan, strlen(flightPlan));
  Py_RETURN_NONE;
}

My_DOCSTR(_saveFMSFlightPlan__doc__, "saveFMSFlightPlan",
          "device=0",
          "device:Optional[int]=0",
          "str",
          "Returns X-Plane 11 formatted flightplan from FMS or GPS.\n"
          "Use device=0 for pilot, device=1 for co-pilot side unit.");
static PyObject *XPLMSaveFMSFlightPlanFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("device"), nullptr};
  (void)self;
  if (!XPLMSaveFMSFlightPlan_ptr) {
    PyErr_SetString(PyExc_RuntimeError, "XPLMSaveFMSFlightPlan is available only in XPLM410 and up.");
    return nullptr;
  }
  int device=0;

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|i", keywords, &device)){
    return nullptr;
  }

  char *buffer;
  int buffer_size = 1000;
  while (1) {
    buffer = (char *)malloc(buffer_size);
    int ret = XPLMSaveFMSFlightPlan_ptr(device, buffer, buffer_size);
    if (ret < buffer_size) break;
    buffer_size = buffer_size * 2;
    free(buffer);
  }
  PyObject *flightPlan = PyUnicode_FromString(buffer);
  Py_INCREF(flightPlan);
  free(buffer);
  return flightPlan;
}

/* ------ cleanup ------- */

static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  Py_RETURN_NONE;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
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
  // SDK 410
  {"countFMSFlightPlanEntries", (PyCFunction)XPLMCountFMSFlightPlanEntriesFun, METH_VARARGS | METH_KEYWORDS, _countFMSFlightPlanEntries__doc__},
  {"XPLMCountFMSFlightPlanEntries", (PyCFunction)XPLMCountFMSFlightPlanEntriesFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"getDisplayedFMSFlightPlanEntry", (PyCFunction)XPLMGetDisplayedFMSFlightPlanEntryFun, METH_VARARGS | METH_KEYWORDS, _getDisplayedFMSFlightPlanEntry__doc__},
  {"XPLMGetDisplayedFMSFlightPlanEntry", (PyCFunction)XPLMGetDisplayedFMSFlightPlanEntryFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"getDestinationFMSFlightPlanEntry", (PyCFunction)XPLMGetDestinationFMSFlightPlanEntryFun, METH_VARARGS | METH_KEYWORDS, _getDestinationFMSFlightPlanEntry__doc__},
  {"XPLMGetDestinationFMSFlightPlanEntry", (PyCFunction)XPLMGetDestinationFMSFlightPlanEntryFun, METH_VARARGS | METH_KEYWORDS, ""},  
  {"setDisplayedFMSFlightPlanEntry", (PyCFunction)XPLMSetDisplayedFMSFlightPlanEntryFun, METH_VARARGS | METH_KEYWORDS, _setDisplayedFMSFlightPlanEntry__doc__},
  {"XPLMSetDisplayedFMSFlightPlanEntry", (PyCFunction)XPLMSetDisplayedFMSFlightPlanEntryFun, METH_VARARGS | METH_KEYWORDS, ""},  
  {"setDestinationFMSFlightPlanEntry", (PyCFunction)XPLMSetDestinationFMSFlightPlanEntryFun, METH_VARARGS | METH_KEYWORDS, _setDestinationFMSFlightPlanEntry__doc__},
  {"XPLMSetDestinationFMSFlightPlanEntry", (PyCFunction)XPLMSetDestinationFMSFlightPlanEntryFun, METH_VARARGS | METH_KEYWORDS, ""},  
  {"setDirectToFMSFlightPlanEntry", (PyCFunction)XPLMSetDirectToFMSFlightPlanEntryFun, METH_VARARGS | METH_KEYWORDS, _setDirectToFMSFlightPlanEntry__doc__},
  {"XPLMSetDirectToFMSFlightPlanEntry", (PyCFunction)XPLMSetDirectToFMSFlightPlanEntryFun, METH_VARARGS | METH_KEYWORDS, ""},  
  {"getFMSFlightPlanEntryInfo", (PyCFunction)XPLMGetFMSFlightPlanEntryInfoFun, METH_VARARGS | METH_KEYWORDS, _getFMSFlightPlanEntryInfo__doc__},
  {"XPLMGetFMSFlightPlanEntryInfo", (PyCFunction)XPLMGetFMSFlightPlanEntryInfoFun, METH_VARARGS | METH_KEYWORDS, ""},  
  {"setFMSFlightPlanEntryInfo", (PyCFunction)XPLMSetFMSFlightPlanEntryInfoFun, METH_VARARGS | METH_KEYWORDS, _setFMSFlightPlanEntryInfo__doc__},
  {"XPLMSetFMSFlightPlanEntryInfo", (PyCFunction)XPLMSetFMSFlightPlanEntryInfoFun, METH_VARARGS | METH_KEYWORDS, ""},  
  {"setFMSFlightPlanEntryLatLon", (PyCFunction)XPLMSetFMSFlightPlanEntryLatLonFun, METH_VARARGS | METH_KEYWORDS, _setFMSFlightPlanEntryLatLon__doc__},
  {"XPLMSetFMSFlightPlanEntryLatLon", (PyCFunction)XPLMSetFMSFlightPlanEntryLatLonFun, METH_VARARGS | METH_KEYWORDS, ""},  
  {"setFMSFlightPlanEntryLatLonWithId", (PyCFunction)XPLMSetFMSFlightPlanEntryLatLonWithIdFun, METH_VARARGS | METH_KEYWORDS, _setFMSFlightPlanEntryLatLonWithId__doc__},
  {"XPLMSetFMSFlightPlanEntryLatLonWithId", (PyCFunction)XPLMSetFMSFlightPlanEntryLatLonWithIdFun, METH_VARARGS | METH_KEYWORDS, ""},  
  {"clearFMSFlightPlanEntry", (PyCFunction)XPLMClearFMSFlightPlanEntryFun, METH_VARARGS | METH_KEYWORDS, _clearFMSFlightPlanEntry__doc__},
  {"XPLMClearFMSFlightPlanEntry", (PyCFunction)XPLMClearFMSFlightPlanEntryFun, METH_VARARGS | METH_KEYWORDS, ""},  
  {"loadFMSFlightPlan", (PyCFunction)XPLMLoadFMSFlightPlanFun, METH_VARARGS | METH_KEYWORDS, _loadFMSFlightPlan__doc__},
  {"XPLMLoadFMSFlightPlan", (PyCFunction)XPLMLoadFMSFlightPlanFun, METH_VARARGS | METH_KEYWORDS, ""},  
  {"saveFMSFlightPlan", (PyCFunction)XPLMSaveFMSFlightPlanFun, METH_VARARGS | METH_KEYWORDS, _saveFMSFlightPlan__doc__},
  {"XPLMSaveFMSFlightPlan", (PyCFunction)XPLMSaveFMSFlightPlanFun, METH_VARARGS | METH_KEYWORDS, ""},  


  {"_cleanup", cleanup, METH_VARARGS, ""},
  {nullptr, nullptr, 0, nullptr}
};
#pragma GCC diagnostic pop


static struct PyModuleDef XPLMNavigationModule = {
  PyModuleDef_HEAD_INIT,
  "XPLMNavigation",
  "Laminar documentation: \n"
  "   https://developer.x-plane.com/sdk/XPLMNavigation/\n"
  "XPPython3 documentation: \n"
  "   https://xppython3.rtfd.io/en/stable/development/modules/navigation.html",
  -1,
  XPLMNavigationMethods,
  nullptr,
  nullptr,
  nullptr,
  nullptr
};

PyMODINIT_FUNC
PyInit_XPLMNavigation(void)
{
  PyObject *mod = PyModule_Create(&XPLMNavigationModule);
  if(mod){
    PyModule_AddStringConstant(mod, "__author__", "Peter Buckner (pbuck@xppython3.org)");
    PyModule_AddIntConstant(mod, "xplm_Nav_Any", 0xffffffff); //XPLMNavType
    PyModule_AddIntConstant(mod, "xplm_Nav_Unknown", xplm_Nav_Unknown); // XPLMNavType
    PyModule_AddIntConstant(mod, "xplm_Nav_Airport", xplm_Nav_Airport); // XPLMNavType
    PyModule_AddIntConstant(mod, "xplm_Nav_NDB", xplm_Nav_NDB); // XPLMNavType
    PyModule_AddIntConstant(mod, "xplm_Nav_VOR", xplm_Nav_VOR); // XPLMNavType
    PyModule_AddIntConstant(mod, "xplm_Nav_ILS", xplm_Nav_ILS); // XPLMNavType
    PyModule_AddIntConstant(mod, "xplm_Nav_Localizer", xplm_Nav_Localizer); // XPLMNavType
    PyModule_AddIntConstant(mod, "xplm_Nav_GlideSlope", xplm_Nav_GlideSlope); // XPLMNavType
    PyModule_AddIntConstant(mod, "xplm_Nav_OuterMarker", xplm_Nav_OuterMarker); // XPLMNavType
    PyModule_AddIntConstant(mod, "xplm_Nav_MiddleMarker", xplm_Nav_MiddleMarker); // XPLMNavType
    PyModule_AddIntConstant(mod, "xplm_Nav_InnerMarker", xplm_Nav_InnerMarker); // XPLMNavType
    PyModule_AddIntConstant(mod, "xplm_Nav_Fix", xplm_Nav_Fix); // XPLMNavType
    PyModule_AddIntConstant(mod, "xplm_Nav_DME", xplm_Nav_DME); // XPLMNavType
    PyModule_AddIntConstant(mod, "xplm_Nav_LatLon", xplm_Nav_LatLon); // XPLMNavType
    PyModule_AddIntConstant(mod, "xplm_Nav_TACAN", xplm_Nav_TACAN); // XPLMNavType
    
    PyModule_AddIntConstant(mod, "XPLM_NAV_NOT_FOUND", XPLM_NAV_NOT_FOUND);

    PyModule_AddIntConstant(mod, "Nav_Any", 0xffffffff); //XPLMNavType
    PyModule_AddIntConstant(mod, "Nav_Unknown", xplm_Nav_Unknown); // XPLMNavType
    PyModule_AddIntConstant(mod, "Nav_Airport", xplm_Nav_Airport); // XPLMNavType
    PyModule_AddIntConstant(mod, "Nav_NDB", xplm_Nav_NDB); // XPLMNavType
    PyModule_AddIntConstant(mod, "Nav_VOR", xplm_Nav_VOR); // XPLMNavType
    PyModule_AddIntConstant(mod, "Nav_ILS", xplm_Nav_ILS); // XPLMNavType
    PyModule_AddIntConstant(mod, "Nav_Localizer", xplm_Nav_Localizer); // XPLMNavType
    PyModule_AddIntConstant(mod, "Nav_GlideSlope", xplm_Nav_GlideSlope); // XPLMNavType
    PyModule_AddIntConstant(mod, "Nav_OuterMarker", xplm_Nav_OuterMarker); // XPLMNavType
    PyModule_AddIntConstant(mod, "Nav_MiddleMarker", xplm_Nav_MiddleMarker); // XPLMNavType
    PyModule_AddIntConstant(mod, "Nav_InnerMarker", xplm_Nav_InnerMarker); // XPLMNavType
    PyModule_AddIntConstant(mod, "Nav_Fix", xplm_Nav_Fix); // XPLMNavType
    PyModule_AddIntConstant(mod, "Nav_DME", xplm_Nav_DME); // XPLMNavType
    PyModule_AddIntConstant(mod, "Nav_LatLon", xplm_Nav_LatLon); // XPLMNavType
    PyModule_AddIntConstant(mod, "Nav_TACAN", xplm_Nav_TACAN); // XPLMNavType

    PyModule_AddIntConstant(mod, "Fpl_Pilot_Primary", xplm_Fpl_Pilot_Primary);  // XPLMNavFlightPlan
    PyModule_AddIntConstant(mod, "Fpl_CoPilot_Primary", xplm_Fpl_CoPilot_Primary); // XPLMNavFlightPlan
    PyModule_AddIntConstant(mod, "Fpl_Pilot_Approach", xplm_Fpl_Pilot_Approach); // XPLMNavFlightPlan
    PyModule_AddIntConstant(mod, "Fpl_CoPilot_Approach", xplm_Fpl_CoPilot_Approach); // XPLMNavFlightPlan
    PyModule_AddIntConstant(mod, "Fpl_Pilot_Temporary", xplm_Fpl_Pilot_Temporary); // XPLMNavFlightPlan
    PyModule_AddIntConstant(mod, "Fpl_CoPilot_Temporary", xplm_Fpl_CoPilot_Temporary); // XPLMNavFlightPlan
    
    PyModule_AddIntConstant(mod, "xplm_Fpl_Pilot_Primary", xplm_Fpl_Pilot_Primary);  // XPLMNavFlightPlan
    PyModule_AddIntConstant(mod, "xplm_Fpl_CoPilot_Primary", xplm_Fpl_CoPilot_Primary); // XPLMNavFlightPlan
    PyModule_AddIntConstant(mod, "xplm_Fpl_Pilot_Approach", xplm_Fpl_Pilot_Approach); // XPLMNavFlightPlan
    PyModule_AddIntConstant(mod, "xplm_Fpl_CoPilot_Approach", xplm_Fpl_CoPilot_Approach); // XPLMNavFlightPlan
    PyModule_AddIntConstant(mod, "xplm_Fpl_Pilot_Temporary", xplm_Fpl_Pilot_Temporary); // XPLMNavFlightPlan
    PyModule_AddIntConstant(mod, "xplm_Fpl_CoPilot_Temporary", xplm_Fpl_CoPilot_Temporary); // XPLMNavFlightPlan
    
    PyModule_AddIntConstant(mod, "NAV_NOT_FOUND", XPLM_NAV_NOT_FOUND);
  }

  return mod;
}
