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
#include "display_keysniffer.h"
#include "display_hotkey.h"
#include "display_avionics.h"
#include "processing.h"
#include "utils.h"
#include "camera.h"
#include "capsules.h"
#include "data_access.h"
#include "instance.h"
#include "map.h"
#include "utilities.h"
#include "widgets.h"
#include "xppython.h"
#include "manage_instance.h"
#include "manage_instances.h"

PyObject *XPY3pythonDicts = nullptr, *XPY3pythonCapsules = nullptr;
PyObject *PythonModuleMTimes = nullptr;
extern const char *pythonPluginVersion, *pythonPluginsPath, *pythonInternalPluginsPath;
static PyObject *getExecutable(void);
static PyObject *buildPluginInfoDict(void);
PluginStats pluginStats[512];
static int numPlugins = 0;

// Constant strings for plugin stats (initialized once in PyInit_XPPython)
static PyObject *stats_fl_string = nullptr;
static PyObject *stats_customw_string = nullptr;
static PyObject *stats_draw_string = nullptr;


int getPluginIndex()
{
  /* add check for 'max plugins', so we don't die on multiple reloads with lots of python plugins? */
  if (numPlugins == 0) {
    pluginStats[numPlugins].module_name = nullptr;
    pluginStats[numPlugins].fl_time = pluginStats[numPlugins].customw_time = pluginStats[numPlugins].draw_time = 0;
    numPlugins++;
  }

  for (int i = 1; i < numPlugins; i++) {
    if(0 == strcmp(pluginStats[i].module_name, CurrentPythonModuleName)) return i;
  }

  pluginStats[numPlugins].module_name = CurrentPythonModuleName;
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
  static char *keywords[] = {CHAR("signature"), nullptr};
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

  PyObject *py_fltime, *py_drawtime, *py_customwtime;

  for(int i=0; i< numPlugins; i++) {
    PyObject *plugin_info = PyDict_New();

    py_fltime = PyLong_FromLong(pluginStats[i].fl_time);
    py_drawtime = PyLong_FromLong(pluginStats[i].draw_time);
    py_customwtime = PyLong_FromLong(pluginStats[i].customw_time);

    PyDict_SetItem(plugin_info, stats_fl_string, py_fltime);
    PyDict_SetItem(plugin_info, stats_draw_string, py_drawtime);
    PyDict_SetItem(plugin_info, stats_customw_string, py_customwtime);
    Py_DECREF(py_fltime);
    Py_DECREF(py_drawtime);
    Py_DECREF(py_customwtime);

    if (i == 0) {
      PyDict_SetItem(info, Py_None, plugin_info);
    } else {
      PyObject *module_key = PyUnicode_FromString(pluginStats[i].module_name);
      PyDict_SetItem(info, module_key, plugin_info);
      Py_DECREF(module_key);
    }

    Py_DECREF(plugin_info);
    pluginStats[i].fl_time = pluginStats[i].customw_time = pluginStats[i].draw_time = 0;
  }

  return info;
}

My_DOCSTR(_getSelfName__doc__, "getSelfModuleName",
          "",
          "",
          "str",
          "Return the string name of the python module of the\n"
          "currently executing python plugin.");
static PyObject *XPGetSelfModuleNameFun(PyObject *self, PyObject *args)
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
PyObject *XPPythonGetDictsFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  Py_INCREF(XPY3pythonDicts);

  PyObject *temp;

  temp = buildAvionicsCallbackDict();
  PyDict_SetItemString(XPY3pythonDicts, "avionicsCallbacks", temp);
  Py_DECREF(temp);

  temp = buildCapsuleDict();
  PyDict_SetItemString(XPY3pythonDicts, "capsules", temp);
  Py_DECREF(temp);

  temp = buildCommandCallbackDict();
  PyDict_SetItemString(XPY3pythonDicts, "commandCallbacks", temp);
  Py_DECREF(temp);

  temp = buildDataRefCallbackDict();
  PyDict_SetItemString(XPY3pythonDicts, "dataRefCallbacks", temp);
  Py_DECREF(temp);

  temp = buildDrawCallbackDict();
  PyDict_SetItemString(XPY3pythonDicts, "drawCallbacks", temp);
  Py_DECREF(temp);

  temp = buildErrorCallbackDict();
  PyDict_SetItemString(XPY3pythonDicts, "errorCallbacks", temp);
  Py_DECREF(temp);

  temp = buildFlightLoopCallbackDict();
  PyDict_SetItemString(XPY3pythonDicts, "flightLoopCallbacks", temp);
  Py_DECREF(temp);

  temp = buildHotKeyCallbackDict();
  PyDict_SetItemString(XPY3pythonDicts, "hotKeyCallbacks", temp);
  Py_DECREF(temp);

  temp = buildKeySnifferCallbackDict();
  PyDict_SetItemString(XPY3pythonDicts, "keySnifferCallbacks", temp);
  Py_DECREF(temp);

  temp = buildMapCallbackDict();
  PyDict_SetItemString(XPY3pythonDicts, "mapCallbacks", temp);
  Py_DECREF(temp);

  temp = buildMenuCallbackDict();
  PyDict_SetItemString(XPY3pythonDicts, "menuCallbacks", temp);
  Py_DECREF(temp);

  temp = buildPluginInfoDict();
  PyDict_SetItemString(XPY3pythonDicts, "plugins", temp);
  Py_DECREF(temp);

  temp = buildSharedDataRefCallbackDict();
  PyDict_SetItemString(XPY3pythonDicts, "sharedDataRefCallbacks", temp);
  Py_DECREF(temp);

  temp = buildWidgetCallbackDict();
  PyDict_SetItemString(XPY3pythonDicts, "widgetCallbacks", temp);
  Py_DECREF(temp);

  temp = buildWidgetPropertiesDict();
  PyDict_SetItemString(XPY3pythonDicts, "widgetProperties", temp);
  Py_DECREF(temp);

  temp = buildWindowCallbackDict();
  PyDict_SetItemString(XPY3pythonDicts, "windowCallbacks", temp);
  Py_DECREF(temp);

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

My_DOCSTR(_getCapsuleDict__doc__, "getCapsuleDict",
          "",
          "",
          "dict",
          "Returns internal dictionary of Capsules\n"
          "\n"
          "Intended for debugging only");
static PyObject *XPGetCapsuleDictFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  return buildCapsuleDict();
}

static PyObject* buildPluginInfoDict(void)
{
  PyObject *pluginDict = PyDict_New();
  if (!pluginDict) {
    return nullptr;
  }

  for (const auto& [pluginInstance, pluginInfo] : XPY3pluginInfoDict) {
    // Add name
    PyObject *name = PyUnicode_FromString(pluginInfo.name.c_str());
    PyObject *signature = PyUnicode_FromString(pluginInfo.signature.c_str());
    PyObject *description = PyUnicode_FromString(pluginInfo.description.c_str());
    PyObject *module_name = PyUnicode_FromString(pluginInfo.module_name);
    PyObject *disabled = pluginInfo.disabled ? Py_True : Py_False;
    if (!name || !signature || !description || !module_name) {
      Py_XDECREF(name);
      Py_XDECREF(signature);
      Py_XDECREF(description);
      Py_XDECREF(module_name);
      Py_DECREF(pluginDict);
      return nullptr;
    }

    Py_INCREF(disabled);

    PyObject *pluginTuple = PyTuple_Pack(5,
                                         module_name,
                                         name,
                                         signature,
                                         description,
                                         disabled);

    // PyTuple_Pack increfs its arguments, so we can release our refs now
    Py_DECREF(name);
    Py_DECREF(signature);
    Py_DECREF(description);
    Py_DECREF(module_name);
    Py_DECREF(disabled);

    if (!pluginTuple) {
      Py_DECREF(pluginDict);
      return nullptr;
    }

    // Add to dictionary
    if (PyDict_SetItem(pluginDict, pluginInstance, pluginTuple) < 0) {
      Py_DECREF(pluginTuple);
      Py_DECREF(pluginDict);
      return nullptr;
    }

    Py_DECREF(pluginTuple);
  }

  return pluginDict;
}

My_DOCSTR(_getCapsulePtr__doc__, "getCapsulePtr",
          "capsule_type, capsule",
          "capsule_type:str, capsule:Any",
          "int",
          "Dereference a capsule to retrieve internal C language pointer\n"
          "\n"
          "Intended for debugging only");

static PyObject *XPGetCapsulePtrFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *capsule;
  const char *capsule_type;
  if(!PyArg_ParseTuple(args, "sO", &capsule_type, &capsule)) {
    return nullptr;
  }
  /* pythonLog("Capsule Name: %s", PyCapsule_GetName(capsule)); */
  /* pythonLog("Capsule Context: %p", PyCapsule_GetContext(capsule)); */
  /* pythonLog("Capsule Pointer: %p", PyCapsule_GetPointer(capsule, capsule_type)); */

  return PyLong_FromVoidPtr(getVoidPtr(capsule, capsule_type));
  //return PyLong_FromVoidPtr(PyCapsule_GetPointer(capsule, capsule_type));
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
  pythonDebug("     Reset --   g) Remove KeySniffer callbacks");
  resetKeySnifferCallbacks();
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
  {"getCapsuleDict", (PyCFunction)XPGetCapsuleDictFun, METH_VARARGS, _getCapsuleDict__doc__},
  {"XPGetCapsuleDict", (PyCFunction)XPGetCapsuleDictFun, METH_VARARGS, ""},
  {"getCapsulePtr", (PyCFunction)XPGetCapsulePtrFun, METH_VARARGS, _getCapsulePtr__doc__},
  {"XPGetCapsulePtr", (PyCFunction)XPGetCapsulePtrFun, METH_VARARGS, ""},
  {"log", (PyCFunction)XPPythonLogFun, METH_VARARGS, _pythonLog__doc__},
  {"XPPythonLog", (PyCFunction)XPPythonLogFun, METH_VARARGS, ""},
  {"XPSystemLog", XPSystemLogFun, METH_VARARGS, ""},
  {"systemLog", XPSystemLogFun, METH_VARARGS, _pythonSystemLog__doc__},
  {"sys_log", XPSystemLogFun, METH_VARARGS, _pythonSystemLog__doc__},
  {"getPluginStats", (PyCFunction)XPGetPluginStats, METH_VARARGS | METH_KEYWORDS, _getPluginStats__doc__},
  {"XPGetPluginStats", (PyCFunction)XPGetPluginStats, METH_VARARGS | METH_KEYWORDS, ""},
  {"reloadPlugin", (PyCFunction)XPReloadPlugin, METH_VARARGS | METH_KEYWORDS, _reloadPlugin__doc__},
  {"XPReloadPlugin", (PyCFunction)XPReloadPlugin, METH_VARARGS | METH_KEYWORDS, ""},
  {"getSelfModuleName", (PyCFunction)XPGetSelfModuleNameFun, METH_VARARGS | METH_KEYWORDS, _getSelfName__doc__},
  {"XPGetSelfModuleName", (PyCFunction)XPGetSelfModuleNameFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"getPluginDict", (PyCFunction)buildPluginInfoDict, METH_VARARGS, "Copy of internal PluginInfo"},
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

  // Initialize constant strings for plugin stats
  stats_fl_string = PyUnicode_FromString("fl");
  stats_customw_string = PyUnicode_FromString("customw");
  stats_draw_string = PyUnicode_FromString("draw");

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
