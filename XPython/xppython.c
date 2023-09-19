#define _GNU_SOURCE 1
#include <Python.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdbool.h>
#include <structmember.h>
#include "xppythontypes.h"
#include <XPLM/XPLMUtilities.h>
#include <XPLM/XPLMNavigation.h>
#include "utils.h"
#include "xppython.h"
#include "manage_instance.h"

PyObject *xppythonDicts = NULL, *xppythonCapsules = NULL;
PyObject *PythonModuleMTimes = NULL;
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
    if(pluginStats[i].pluginInstance == pluginInstance) {
      return i;
    }
  }
  pluginStats[numPlugins].pluginInstance = pluginInstance;
  pluginStats[numPlugins].fl_time = pluginStats[numPlugins].customw_time = pluginStats[numPlugins].draw_time = 0;
  return numPlugins++;
}

PyObject *getExecutable()
/* get value for executable to be 'python' rather than 'X-Plane'.
   This will enable subprocess and multiprocessing to work as
   anticipated
*/
{
  PyObject *exec_prefix = PySys_GetObject("exec_prefix");
#if LIN || APL
  PyObject *executable = PyUnicode_FromFormat("%S/bin/python" PYTHONVERSION, exec_prefix);
#endif
#if IBM
  PyObject *executable = PyUnicode_FromFormat("%S\\pythonw.exe", exec_prefix);
#endif
  return executable;
}





My_DOCSTR(_reloadPlugin__doc__, "reloadPlugin", "signature",
          "Reload (python) plugin with provided signature\n"
          "\n"
          "Plugin will be disabled, stopped, reloaded, then\n"
          "started and enabled.");
static PyObject *XPReloadPlugin(PyObject *self, PyObject *args, PyObject *kwargs)
{
  (void) self;
  static char *kwlist[] = {"signature", NULL};
  PyObject *signature;
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O", kwlist, &signature)) {
    return NULL;
  }
  xpy_reloadInstance(signature);
  Py_RETURN_NONE;
}


My_DOCSTR(_getPluginStats__doc__, "getPluginStats", "",
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

My_DOCSTR(_pythonGetDicts__doc__, "pythonGetDicts", "",
          "Return dictionary of internal xppython3 dictionaries\n"
          "\n"
          "See documentation, intended for debugging only");
static PyObject *XPPythonGetDictsFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  Py_INCREF(xppythonDicts);
  return xppythonDicts;
}

My_DOCSTR(_pythonLog__doc__, "log", "",
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
      char *moduleName = get_moduleName();
      pythonLog("[%s] %s\n", moduleName, inString);
      free(moduleName);
    } else {
      flush = 1;
    }
  }
  if (flush || pythonFlushLog) {
    pythonLogFlush();
  }
  Py_RETURN_NONE;
}

My_DOCSTR(_pythonSystemLog__doc__, "systemLog", "",
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
      char *moduleName = get_moduleName();
      char *msg;
      if (-1 == asprintf(&msg, "[XP3: %s] %s\n", moduleName, inString)) {
        pythonLog("Failed to allocate memory for asprintf syslog.\n");
      } else {
        XPLMDebugString(msg);
        free(msg);
      }
      free(moduleName);
    } else {
      /* DebugString already, always flushes, so ignore empty prints */
    }
  }
  Py_RETURN_NONE;
}

My_DOCSTR(_pythonGetCapsules__doc__, "pythonGetCapsules", "",
          "Returns internal dictionary of Capsules\n"
          "\n"
          "Intended for debugging only");
static PyObject *XPPythonGetCapsulesFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  Py_INCREF(xppythonCapsules);
  return xppythonCapsules;
}

My_DOCSTR(_derefCapsule__doc__, "derefCapsule", "capsule_type, capsule",
          "Dereference a capsule to retrieve internal C language pointer\n"
          "\n"
          "Intended for debugging only");

static PyObject *XPPythonDerefCapsuleFun(PyObject *self, PyObject *args)
{
  (void) self;
  const char *capsule_type;
  PyObject *capsule;
  if(!PyArg_ParseTuple(args, "sO", &capsule_type, &capsule)) {
    return NULL;
  }
  /* pythonLog("Capsule Name: %s\n", PyCapsule_GetName(capsule)); */
  /* pythonLog("Capsule Context: %p\n", PyCapsule_GetContext(capsule)); */
  /* pythonLog("Capsule Pointer: %p\n", PyCapsule_GetPointer(capsule, capsule_type)); */

  return PyLong_FromVoidPtr(PyCapsule_GetPointer(capsule, capsule_type));
}

static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  PyDict_Clear(xppythonCapsules);
  Py_DECREF(xppythonCapsules);
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
  {"_cleanup", cleanup, METH_VARARGS, ""},
  {NULL, NULL, 0, NULL}
};
#pragma GCC diagnostic pop


static struct PyModuleDef XPPythonModule = {
  PyModuleDef_HEAD_INIT,
  "XPPython",
  "XPPython3 documentation: \n"
  "   https://xppython3.rtfd.io/en/stable/development/modules/kpython.html",
  -1,
  XPPythonMethods,
  NULL,
  NULL,
  NULL,
  NULL
};

PyMODINIT_FUNC
PyInit_XPPython(void)
{
  if (PyType_Ready(&HotKeyInfoType) < 0)
    return NULL;
  if (PyType_Ready(&ProbeInfoType) < 0)
    return NULL;
  if (PyType_Ready(&DataRefInfoType) < 0)
    return NULL;
  if (PyType_Ready(&WeatherInfoType) < 0)
    return NULL;
  if (PyType_Ready(&WeatherInfoCloudsType) < 0)
    return NULL;
  if (PyType_Ready(&WeatherInfoWindsType) < 0)
    return NULL;
  if (PyType_Ready(&PluginInfoType) < 0)
    return NULL;
  if (PyType_Ready(&TrackMetricsType) < 0)
    return NULL;
  if (PyType_Ready(&NavAidInfoType) < 0)
    return NULL;
  if (PyType_Ready(&FMSEntryInfoType) < 0)
    return NULL;

  PyObject *mod = PyModule_Create(&XPPythonModule);
  PythonModuleMTimes = PyDict_New();

  if (mod != NULL) {
    PyModule_AddStringConstant(mod, "__author__", "Peter Buckner (pbuck@avnwx.com)");
    PyModule_AddStringConstant(mod, "VERSION", pythonPluginVersion);
    PyModule_AddStringConstant(mod, "PLUGINSPATH", pythonPluginsPath);
    PyModule_AddStringConstant(mod, "INTERNALPLUGINSPATH", pythonInternalPluginsPath);
    PyModule_AddIntConstant(mod, "pythonDebugLevel", pythonDebugs);
    PyModule_AddObject(mod, "pythonExecutable", getExecutable());
    PyModule_AddObject(mod, "HotKeyInfo", (PyObject *) &HotKeyInfoType);
    PyModule_AddObject(mod, "ProbeInfo", (PyObject *) &ProbeInfoType);
    PyModule_AddObject(mod, "DataRefInfo", (PyObject *) &DataRefInfoType);
    PyModule_AddObject(mod, "WeatherInfo", (PyObject *) &WeatherInfoType);
    PyModule_AddObject(mod, "WeatherInfoClouds", (PyObject *) &WeatherInfoCloudsType);
    PyModule_AddObject(mod, "WeatherInfoWinds", (PyObject *) &WeatherInfoWindsType);
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
