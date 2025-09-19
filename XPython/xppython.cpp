#define _GNU_SOURCE 1
#include <Python.h>
#include <pthread.h>
#include <math.h>
#include <sys/time.h>
#include <stdio.h>
#include <structmember.h>
#include "xppythontypes.h"
#include <XPLM/XPLMUtilities.h>
#include <XPLM/XPLMNavigation.h>
#include <XPLM/XPLMProcessing.h>
#include <vector>
#include <string>
#include "menus.h"
#include "display.h"
#include "processing.h"
#include "utils.h"
#include "camera.h"
#include "data_access.h"
#include "instance.h"
#include "map.h"
#include "utilities.h"
#include "widgets.h"
#include "xppython.h"
#include "manage_instance.h"
#include "cpp_utilities.hpp"

PyObject *XPY3pythonDicts = nullptr, *XPY3pythonCapsules = nullptr;
PyObject *PythonModuleMTimes = nullptr;
extern const char *pythonPluginVersion, *pythonPluginsPath, *pythonInternalPluginsPath;
static PyObject *getExecutable(void);

PluginStats pluginStats[512];
static int numPlugins = 0;


int getPluginIndex(PyObject *pluginInstance)
{
  /* add check for 'max plugins', so we don't die on multiple reloads with lots of python plugins? */
  if (numPlugins == 0) {
    pluginStats[numPlugins].pluginInstance = Py_None; /* slot zero is for 'all' */
    pluginStats[numPlugins].fl_time = pluginStats[numPlugins].customw_time = pluginStats[numPlugins].draw_time = 0;
    numPlugins++;
  }
  for (int i = 0; i < numPlugins; i++) {
    if(pluginStats[i].pluginInstance != nullptr && PyObject_RichCompareBool(pluginStats[i].pluginInstance, pluginInstance, Py_EQ)) {
      return i;
    }
  }

  pluginStats[numPlugins].pluginInstance = pluginInstance;
  Py_INCREF(pluginInstance);
  pluginStats[numPlugins].fl_time = pluginStats[numPlugins].customw_time = pluginStats[numPlugins].draw_time = 0;
  return numPlugins++;
}

PyObject *getExecutable()
/* get value for executable to be 'python' rather than 'X-Plane'.
   This will enable subprocess and multiprocessing to work as
   anticipated
*/
{
  PyObject *py_executable = PySys_GetObject("executable");
  PyObject *executable = PyUnicode_FromFormat("%S", py_executable);
  return executable;
}





My_DOCSTR(_reloadPlugin__doc__, "reloadPlugin",
          "signature",
          "signature:str",
          "None",
          "Reload (python) plugin with provided signature\n"
          "\n"
          "Plugin will be disabled, stopped, reloaded, then\n"
          "started and enabled.");
static PyObject *XPReloadPlugin(PyObject *self, PyObject *args, PyObject *kwargs)
{
  (void) self;
  std::vector<std::string> params = {"signature"};
  char **keywords = stringVectorToCharArray(params);
  PyObject *signature;
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &signature)) {
    return nullptr;
  }
  xpy_reloadInstance(signature);
  Py_RETURN_NONE;
}


My_DOCSTR(_getPluginStats__doc__, "getPluginStats",
          "",
          "",
          "dict",
          "Return dictionary of python plugin performance statistics\n"
          "\n"
          "Keys are different python plugins, with key=None being\n"
          "overall performance of XPPython3 plugin.");
static PyObject *XPGetPluginStats(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  PyObject *info = PyDict_New();
  
  PyObject *fl_string = PyUnicode_FromString("fl");
  PyObject *customw_string = PyUnicode_FromString("customw");
  PyObject *draw_string = PyUnicode_FromString("draw");
  PyObject *py_fltime, *py_drawtime, *py_customwtime;

  for(int i=0; i< numPlugins; i++) {
    PyObject *plugin_info = PyDict_New();

    py_fltime = PyLong_FromLong(pluginStats[i].fl_time);
    py_drawtime = PyLong_FromLong(pluginStats[i].draw_time);
    py_customwtime = PyLong_FromLong(pluginStats[i].customw_time);

    PyDict_SetItem(plugin_info, fl_string, py_fltime);
    PyDict_SetItem(plugin_info, draw_string, py_drawtime);
    PyDict_SetItem(plugin_info, customw_string, py_customwtime);
    Py_DECREF(py_fltime);
    Py_DECREF(py_drawtime);
    Py_DECREF(py_customwtime);

    PyDict_SetItem(info, pluginStats[i].pluginInstance, plugin_info);
    
    Py_DECREF(plugin_info);
    pluginStats[i].fl_time = pluginStats[i].customw_time = pluginStats[i].draw_time = 0;
  }

  Py_DECREF(fl_string);
  Py_DECREF(draw_string);
  Py_DECREF(customw_string);

  Py_INCREF(info);
  return info;
}

My_DOCSTR(_getSelfName__doc__, "getSelfName",
          "",
          "",
          "str",
          "Return the string name of the python module of the\n"
          "currently executing python plugin.");
static PyObject *XPGetSelfNameFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  return PyUnicode_FromString(CurrentPythonModuleName);
}

My_DOCSTR(_pythonGetDicts__doc__, "pythonGetDicts",
          "",
          "",
          "dict",
          "Return dictionary of internal xppython3 dictionaries\n"
          "\n"
          "See documentation, intended for debugging only");
static PyObject *XPPythonGetDictsFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  Py_INCREF(XPY3pythonDicts);
  //PyDict_SetItemString(XPY3pythonDicts, "commandCallbacks", buildCommandCallbacksDict())
  return XPY3pythonDicts;
}

My_DOCSTR(_pythonLog__doc__, "log",
          "s",
          "s:Optional[str]=None",
          "None",
          "Log string to XPPython3log.txt file. Flush buffer, if no string is provided.");
static PyObject *XPPythonLogFun(PyObject *self, PyObject *args)
{
  (void) self;
  const char *inString;
  int flush = 0;
  if(!PyArg_ParseTuple(args, "s", &inString)) {
    /* don't bother sending error */
    PyErr_Clear();
    flush = 1;
  } else {
    if (strlen(inString)) {
      pythonLog("[%s] %s", pthread_equal(pythonThread, pthread_self()) ? CurrentPythonModuleName : "Thread", inString);
    } else {
      flush = 1;
    }
  }
  if (flush || pythonFlushLog) {
    pythonLogFlush();
  }
  Py_RETURN_NONE;
}

My_DOCSTR(_pythonSystemLog__doc__, "systemLog",
          "s",
          "s:Optional[str]=None",
          "None",
          "Log string to system log file, Log.txt, with newline appended, flushing buffer.");
static PyObject *XPSystemLogFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  const char *inString;
  if(!PyArg_ParseTuple(args, "s", &inString)) {
    /* don't bother sending error */
    PyErr_Clear();
  } else {
    if (strlen(inString)) {
      char *msg;
      float t = XPLMGetElapsedTime();
      if (-1 == asprintf(&msg, "%d:%02d:%06.3f XP3: [%s] %s\n",
                         (int) (t / 3600.0),
                         (int) (t / 60.0),
                         fmod(t, 60.0),
                         pthread_equal(pythonThread, pthread_self()) ? CurrentPythonModuleName : "Thread",
                         inString)) {
        pythonLog("Failed to allocate memory for asprintf syslog.");
      } else {
        XPLMDebugString(msg);
        free(msg);
      }
    } else {
      /* DebugString already, always flushes, so ignore empty prints */
    }
  }
  Py_RETURN_NONE;
}

My_DOCSTR(_pythonGetCapsules__doc__, "pythonGetCapsules",
          "",
          "",
          "dict",
          "Returns internal dictionary of Capsules\n"
          "\n"
          "Intended for debugging only");
static PyObject *XPPythonGetCapsulesFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  Py_INCREF(XPY3pythonCapsules);
  return XPY3pythonCapsules;
}

My_DOCSTR(_derefCapsule__doc__, "derefCapsule",
          "capsule_type, capsule",
          "capsule_type:str, capsule:Any",
          "int",
          "Dereference a capsule to retrieve internal C language pointer\n"
          "\n"
          "Intended for debugging only");

static PyObject *XPPythonDerefCapsuleFun(PyObject *self, PyObject *args)
{
  (void) self;
  const char *capsule_type;
  PyObject *capsule;
  if(!PyArg_ParseTuple(args, "sO", &capsule_type, &capsule)) {
    return nullptr;
  }
  /* pythonLog("Capsule Name: %s", PyCapsule_GetName(capsule)); */
  /* pythonLog("Capsule Context: %p", PyCapsule_GetContext(capsule)); */
  /* pythonLog("Capsule Pointer: %p", PyCapsule_GetPointer(capsule, capsule_type)); */

  return PyLong_FromVoidPtr(PyCapsule_GetPointer(capsule, capsule_type));
}

static void resetCapsules() {
  PyObject *keys = PyDict_Keys(XPY3pythonCapsules);
  PyObject *iterator = PyObject_GetIter(keys);
  PyObject *key;
  while((key = PyIter_Next(iterator))) {
    PyObject *item = PyDict_GetItem(XPY3pythonCapsules, key); /* borrowed */
    int size = PyDict_Size(item);
    if (size > 0) {
      char *s = objToStr(key);
      pythonDebug("     Reset --     %d %s Capsules", size, s);
      free(s);
    }
    PyDict_Clear(item);
    Py_DECREF(key);
  }
  Py_XDECREF(iterator);
  Py_DECREF(keys);
  PyDict_Clear(XPY3pythonCapsules);
}

void resetInternals() {
  pythonDebug("     Reset --   a) Clear Menus");
  resetMenus();
  pythonDebug("     Reset --   b) Cancel FlightLoops");
  resetFlightLoops();
  pythonDebug("     Reset --   c) Remove Windows");
  resetWindows();
  pythonDebug("     Reset --   d) Remove Commands");
  resetCommands();
  pythonDebug("     Reset --   e) Remove Widgets");
  resetWidgets();
  pythonDebug("     Reset --   f) Remove Direct Draw callbacks");
  errCheck("pre drawCallbacks");
  resetDrawCallbacks();
  errCheck("post drawCallbacks");
  pythonDebug("     Reset --   g) Remove KeySniff callbacks");
  resetKeySniffCallbacks();
  pythonDebug("     Reset --   h) Remove HotKey callbacks");
  resetHotKeyCallbacks();
  errCheck("post resetHotKey");
  pythonDebug("     Reset --   i) Remove Avionics callbacks");
  errCheck("pre resetAvionicsKey");
  resetAvionicsCallbacks();
  pythonDebug("     Reset --   j) Release Camera");
  errCheck("pre camera");
  resetCamera();
  pythonDebug("     Reset --   k) Reset Map");
  resetMap();
  /* resetPlanes -- do something with 'acquirePlanes?'
     doesn't seem to matter... yet. On reload,
     plugin is able to re-acquire plane & I haven't quite
     figured out a way to break it... so don't do anything

     TODO: keep pointer to _current_python_plugin_ which has
     acquired planes -- other python plugins which attempt
     to acquire, return 0(?), ... allowing callback to
     occur for 'other' python plugins?
  */
  pythonDebug("     Reset --   l) Reset registered datarefs");
  resetDataRefs();
  pythonDebug("     Reset --   m) Reset Instanced Objects");
  resetInstances();

  pythonDebug("     Reset --   x) Clear Capsules");
  resetCapsules();
}

static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  PyDict_Clear(XPY3pythonCapsules);
  Py_DECREF(XPY3pythonCapsules);
  Py_RETURN_NONE;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
static PyMethodDef XPPythonMethods[] = {
  {"pythonGetDicts", (PyCFunction)XPPythonGetDictsFun, METH_VARARGS, _pythonGetDicts__doc__},
  {"XPPythonGetDicts", (PyCFunction)XPPythonGetDictsFun, METH_VARARGS, ""},
  {"pythonGetCapsules", (PyCFunction)XPPythonGetCapsulesFun, METH_VARARGS, _pythonGetCapsules__doc__},
  {"XPPythonGetCapsules", (PyCFunction)XPPythonGetCapsulesFun, METH_VARARGS, ""},
  {"derefCapsule", (PyCFunction)XPPythonDerefCapsuleFun, METH_VARARGS, _derefCapsule__doc__},
  {"XPPythonDerefCapsule", (PyCFunction)XPPythonDerefCapsuleFun, METH_VARARGS, ""},
  {"log", (PyCFunction)XPPythonLogFun, METH_VARARGS, _pythonLog__doc__},
  {"XPPythonLog", (PyCFunction)XPPythonLogFun, METH_VARARGS, ""},
  {"XPSystemLog", XPSystemLogFun, METH_VARARGS, ""},
  {"systemLog", XPSystemLogFun, METH_VARARGS, _pythonSystemLog__doc__},
  {"sys_log", XPSystemLogFun, METH_VARARGS, _pythonSystemLog__doc__},
  {"getPluginStats", (PyCFunction)XPGetPluginStats, METH_VARARGS | METH_KEYWORDS, _getPluginStats__doc__},
  {"XPGetPluginStats", (PyCFunction)XPGetPluginStats, METH_VARARGS | METH_KEYWORDS, ""},
  {"reloadPlugin", (PyCFunction)XPReloadPlugin, METH_VARARGS | METH_KEYWORDS, _reloadPlugin__doc__},
  {"XPReloadPlugin", (PyCFunction)XPReloadPlugin, METH_VARARGS | METH_KEYWORDS, ""},
  {"getSelfName", (PyCFunction)XPGetSelfNameFun, METH_VARARGS | METH_KEYWORDS, _getSelfName__doc__},
  {"XPGetSelfName", (PyCFunction)XPGetSelfNameFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"_cleanup", cleanup, METH_VARARGS, ""},
  {nullptr, nullptr, 0, nullptr}
};
#pragma GCC diagnostic pop


static struct PyModuleDef XPPythonModule = {
  PyModuleDef_HEAD_INIT,
  "XPPython",
  "XPPython3 documentation: \n"
  "   https://xppython3.rtfd.io/en/stable/development/modules/xppython.html",
  -1,
  XPPythonMethods,
  nullptr,
  nullptr,
  nullptr,
  nullptr
};

PyMODINIT_FUNC
PyInit_XPPython(void)
{
  if (PyType_Ready(&HotKeyInfoType) < 0)
    return nullptr;
  if (PyType_Ready(&ProbeInfoType) < 0)
    return nullptr;
  if (PyType_Ready(&DataRefInfoType) < 0)
    return nullptr;
  if (PyType_Ready(&WeatherInfoType) < 0)
    return nullptr;
  if (PyType_Ready(&WeatherInfoCloudsType) < 0)
    return nullptr;
  if (PyType_Ready(&WeatherInfoWindsType) < 0)
    return nullptr;
  if (PyType_Ready(&PluginInfoType) < 0)
    return nullptr;
  if (PyType_Ready(&TrackMetricsType) < 0)
    return nullptr;
  if (PyType_Ready(&NavAidInfoType) < 0)
    return nullptr;
  if (PyType_Ready(&FMSEntryInfoType) < 0)
    return nullptr;

  PyObject *mod = PyModule_Create(&XPPythonModule);
  PythonModuleMTimes = PyDict_New();

  if (mod != nullptr) {
    PyModule_AddStringConstant(mod, "__author__", "Peter Buckner (pbuck@xppython3.org)");
    PyModule_AddStringConstant(mod, "VERSION", pythonPluginVersion);
    PyModule_AddStringConstant(mod, "PLUGINSPATH", pythonPluginsPath);
    PyModule_AddStringConstant(mod, "INTERNALPLUGINSPATH", pythonInternalPluginsPath);
    PyModule_AddIntConstant(mod, "pythonDebugLevel", pythonDebugs);
    PyModule_AddObject(mod, "pythonExecutable", getExecutable()); // str
    PyModule_AddObject(mod, "HotKeyInfo", (PyObject *) &HotKeyInfoType);
    PyModule_AddObject(mod, "XPLMProbeInfo_t", (PyObject *) &ProbeInfoType);
    PyModule_AddObject(mod, "XPLMDataRefInfo_t", (PyObject *) &DataRefInfoType);
    PyModule_AddObject(mod, "XPLMWeatherInfo_t", (PyObject *) &WeatherInfoType);
    PyModule_AddObject(mod, "XPLMWeatherInfoClouds_t", (PyObject *) &WeatherInfoCloudsType);
    PyModule_AddObject(mod, "XPLMWeatherInfoWinds_t", (PyObject *) &WeatherInfoWindsType);
    PyModule_AddObject(mod, "PluginInfo", (PyObject *) &PluginInfoType);
    PyModule_AddObject(mod, "NavAidInfo", (PyObject *) &NavAidInfoType);
    PyModule_AddObject(mod, "FMSEntryInfo", (PyObject *) &FMSEntryInfoType);
    PyModule_AddObject(mod, "TrackMetrics", (PyObject *) &TrackMetricsType);
    PyModule_AddObject(mod, "ModuleMTimes", PythonModuleMTimes);
  }
  Py_INCREF(&HotKeyInfoType);
  Py_INCREF(&ProbeInfoType);
  Py_INCREF(&DataRefInfoType);
  Py_INCREF(&WeatherInfoType);
  Py_INCREF(&WeatherInfoCloudsType);
  Py_INCREF(&WeatherInfoWindsType);
  Py_INCREF(&PluginInfoType);
  Py_INCREF(&NavAidInfoType);
  Py_INCREF(&FMSEntryInfoType);
  Py_INCREF(&TrackMetricsType);

  return mod;
}
