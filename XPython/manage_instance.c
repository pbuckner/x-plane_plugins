#define _GNU_SOURCE 1
//Python comes first!
#include <Python.h>
#include "utils.h"
#include "manage_instances.h"
#include "manage_instance.h"
#include "menus.h"

extern void clearInstanceMenuItems(PyObject*);
extern void clearInstanceCommands(PyObject*);
static void updatePluginDict(PyObject*, PyObject*,PyObject*, PyObject*);
static PyObject *getPluginInfo(PyObject *);

int xpy_startInstance(PyObject *module_name_p, PyObject *pModule, PyObject* pluginInstance) {  
  /* Start loaded instance, update XPY3moduleDict and XP3pluginDict with information
   */
  set_moduleName(module_name_p);
  PyObject *pRes = PyObject_CallMethod(pluginInstance, "XPluginStart", NULL);
  PyObject *err = PyErr_Occurred();
  if (err) {
    pythonLogException();
    pythonLog("[%s] Failed to start, error in XPluginStart", CurrentPythonModuleName);
    return 0;
  }

  pythonDebug("Starting instance %s", CurrentPythonModuleName);
  if (pRes) {
    if(PyTuple_Check(pRes) &&
       PyTuple_Size(pRes) == 3 &&
       PyUnicode_Check(PyTuple_GetItem(pRes, PLUGIN_NAME)) &&
       PyUnicode_Check(PyTuple_GetItem(pRes, PLUGIN_SIGNATURE)) &&
       PyUnicode_Check(PyTuple_GetItem(pRes, PLUGIN_DESCRIPTION))) {
      PyObject *u1 = PyUnicode_AsUTF8String(PyTuple_GetItem(pRes, PLUGIN_NAME));
      PyObject *u2 = PyUnicode_AsUTF8String(PyTuple_GetItem(pRes, PLUGIN_SIGNATURE));
      PyObject *u3 = PyUnicode_AsUTF8String(PyTuple_GetItem(pRes, PLUGIN_DESCRIPTION));
      if(u1 && u2 && u3){
        pythonLog("[XPPython3] %s initialized.", CurrentPythonModuleName);
        pythonLog("[XPPython3]  Name: %s", PyBytes_AsString(u1));
        pythonLog("[XPPython3]  Sig:  %s", PyBytes_AsString(u2));
        pythonLog("[XPPython3]  Desc: %s", PyBytes_AsString(u3));
        pythonLogFlush();
        Py_DECREF(u1);
        Py_DECREF(u2);
        Py_DECREF(u3);

        updatePluginDict(module_name_p, pModule, pRes, pluginInstance);
        return 1;
      } else {
        pythonLog("[XPPython3] Failed to decode start information in %s", CurrentPythonModuleName);
      }
    } else {
      Py_DECREF(pRes);
      pythonLog("[XPPython3] Unable to start plugin in file %s: XPluginStart did not return Name, Sig, and Desc.", CurrentPythonModuleName);
    }
  } else {
    pythonLog("[%s] XPluginStart returned NULL", CurrentPythonModuleName); // NULL is error, Py_None is void, we're looking for a tuple[3]
  }
  if(PyErr_Occurred()) {
    pythonLogException();
  }
  return 0;
}

void updatePluginDict(PyObject *module_name_p, PyObject *pModule, PyObject *pRes, PyObject *pluginInstance) {
  PyObject *pluginInfo = PyList_New(6);  /* pluginInfo is new reference */
  PyObject *tmp;

  tmp = PyTuple_GetItem(pRes, PLUGIN_NAME);
  Py_INCREF(tmp);
  PyList_SetItem(pluginInfo, PLUGIN_NAME, tmp);

  tmp = PyTuple_GetItem(pRes, PLUGIN_SIGNATURE);
  Py_INCREF(tmp);
  PyList_SetItem(pluginInfo, PLUGIN_SIGNATURE, tmp);

  tmp = PyTuple_GetItem(pRes, PLUGIN_DESCRIPTION);
  Py_INCREF(tmp);
  PyList_SetItem(pluginInfo, PLUGIN_DESCRIPTION, tmp);

  Py_INCREF(pModule);
  PyList_SetItem(pluginInfo, PLUGIN_MODULE, pModule);

  Py_INCREF(module_name_p);
  PyList_SetItem(pluginInfo, PLUGIN_MODULE_NAME, module_name_p);
  Py_INCREF(Py_False);
  PyList_SetItem(pluginInfo, PLUGIN_DISABLED, Py_False);

  Py_INCREF(pluginInstance);
  PyDict_SetItem(XPY3moduleDict, module_name_p, pluginInstance);
  Py_INCREF(pluginInfo);
  PyDict_SetItem(XPY3pluginDict, pluginInstance, pluginInfo);
  if(PyErr_Occurred()) {
    pythonLog("Error while updating plugin dict");
    pythonLogException();
  }
}


void xpy_reloadInstance(PyObject *signature) {
  /* given signature, reload the plugin:
     1) convert signature to instance
     2) "Disable"
     3) "Stop"
     4) clean up as best we can
     5) python reload module
     6) "Start" instance
     7) "Enable" instance
  */

  pythonDebug("Reloading instance with signature: %s", objDebug(signature));
  PyObject *pluginInfo = getPluginInfo(signature);
  if (!pluginInfo) {
    char *s = objToStr(signature);
    pythonLog("[XPPython3] Could not find plugin for signature: %s", s);
    free(s);
    return;
  }

  PyObject *moduleName = PyList_GetItem(pluginInfo, PLUGIN_MODULE_NAME);
  set_moduleName(moduleName);
  pythonDebug("  which is module: %s", objDebug(moduleName));
  PyObject *pluginInstance = PyDict_GetItem(XPY3moduleDict, moduleName);
  
  if (PyList_GetItem(pluginInfo, PLUGIN_DISABLED) == Py_False) {
    xpy_disableInstance(moduleName, pluginInstance);
  }
  xpy_stopInstance(moduleName, pluginInstance);

  /* clean up as best we can */
  /*   remove from Dicts
   */
  PyDict_DelItem(XPY3moduleDict, moduleName);
  PyDict_DelItem(XPY3pluginDict, pluginInstance);

  pythonDebug("Calling ReloadModule");
  PyObject *module = PyImport_ReloadModule(PyList_GetItem(pluginInfo, PLUGIN_MODULE));
  PyObject *err = PyErr_Occurred();
  if (err) {
    pythonLogException();
    char *s = objToStr(moduleName);
    pythonLog("[XPPython3] Failed to reload module '%s'", s);
    free(s);
    return;
  }
  
  PyObject *pClass = PyObject_GetAttrString(module, "PythonInterface");
  if (pClass && PyCallable_Check(pClass)) {
    pythonDebug("  Getting PythonInterface class");
    pluginInstance = PyObject_CallObject(pClass, NULL);
    Py_DECREF(pClass);
    if (!pluginInstance) {
      char *s = objToStr(moduleName);
      pythonLog("[XPPython3] Cannot restart plugin '%s'.", s);
      free(s);
      if (PyErr_Occurred()) {
        pythonLogException();
      }
      return;
    }
    if(xpy_startInstance(moduleName, module, pluginInstance)) {
      /* on reload, we attempt to reload _all_ plugins, even if they'd previously
         request to be disabled */
      if (0 == xpy_enableInstance(moduleName, pluginInstance)) {
        /* we could not enable the plugin (or plugin requested not to be enabled
           Mark it as disabled, so we'll not sent messages (or Disable) to it later
        */
        PyList_SetItem(pluginInfo, PLUGIN_DISABLED, Py_True);
      } else {
        PyList_SetItem(pluginInfo, PLUGIN_DISABLED, Py_False);
      }
    }
  }
}
  
PyObject *getPluginInfo(PyObject *signature){
  PyObject *pluginInfo, *pluginInstance;
  Py_ssize_t pos = 0;
  while(PyDict_Next(XPY3pluginDict, &pos, &pluginInstance, &pluginInfo)){
    if (! PyObject_RichCompareBool(PyList_GetItem(pluginInfo, PLUGIN_SIGNATURE), signature, Py_EQ)) {
      continue;
    }
    pythonDebug("Found plugin for signature: %s", objDebug(signature));
    return pluginInfo;
  }
  return NULL;
}

int xpy_enableInstance(PyObject *moduleName, PyObject *pluginInstance) {
  set_moduleName(moduleName);
  pythonDebug("  Enabling instance: %s", CurrentPythonModuleName);
  PyObject *pRes = PyObject_CallMethod(pluginInstance, "XPluginEnable", NULL);

  if(PyErr_Occurred()) {
    pythonLogException();
    pythonLog("[XPPython3] Error occured during call to %s XPluginEnable", CurrentPythonModuleName);
    return 0;
  }

  if(!(pRes && PyLong_Check(pRes))){
    char *s = objToStr(pRes);
    pythonLog("[XPPython3] %s XPluginEnable returned '%s' rather than an integer.", CurrentPythonModuleName, s);
    free(s);
  } else {
    pythonDebug("    %s XPluginEnable returned %ld", CurrentPythonModuleName, PyLong_AsLong(pRes));
  }

  int ret = PyLong_AsLong(pRes) > 0 ? 1 : 0;
  if (ret == 0) {
    pythonLog("[XPPython3] %s not enabled.", CurrentPythonModuleName);
  }
  Py_DECREF(pRes);
  return ret;
}


void xpy_disableInstance(PyObject *moduleName, PyObject *pluginInstance) {
  set_moduleName(moduleName);
  pythonDebug("  Disabling instance: %s", CurrentPythonModuleName);
  PyObject_CallMethod(pluginInstance, "XPluginDisable", NULL);
  PyObject *err = PyErr_Occurred();

  if (err) {
    PyErr_Clear();
    if (PyObject_HasAttrString(pluginInstance, "XPluginDisable")) {
      pythonLogException();
      pythonLog("[XPPython3] Error occured during call to %s XPluginDisable", CurrentPythonModuleName);
      PyErr_SetRaisedException(err);
    } else {
      pythonDebug("  (no XPluginDisable for %s module)", CurrentPythonModuleName);
      /* ignore error, if XPluginDisable is not defined in the PythonInterface class */
    }
  }
}

void xpy_stopInstance(PyObject *moduleName, PyObject *pluginInstance) {
  set_moduleName(moduleName);
  pythonDebug("%*s Stopping instance: %s", 2, " ", CurrentPythonModuleName);

  int has_attr = PyObject_HasAttrString(pluginInstance, "XPluginStop");
  if (!has_attr) {
    /* ignore error, if XPluginStop is not defined in the PythonInterface class */
    pythonDebug("%*s (no XPluginStop for this module)", 4, " ");
  } else {
    PyObject *pRes = PyObject_CallMethod(pluginInstance, "XPluginStop", NULL);
    PyObject *err = PyErr_Occurred();
    if (err) {
      pythonLogException();
      char *s = objToStr(moduleName);
      pythonLog("[XPPython3] Error occurred during call to %s XPluginStop", s);
      free(s);
    }
    if (pRes != Py_None && pRes != NULL) {
      pythonDebug("XPluginStop for %s returned '%s' rather than None. Value ignored\n", CurrentPythonModuleName, objDebug(pRes));
    }
    Py_XDECREF(pRes);
  }

  xpy_cleanUpInstance(moduleName, pluginInstance);
}

void xpy_cleanUpInstance(PyObject *moduleName, PyObject *pluginInstance) {
  set_moduleName(moduleName);
  pythonDebug("%*s Cleaning instance: %s", 4, " ", CurrentPythonModuleName);
  PyObject *err = PyErr_Occurred();
  if (err) {
    pythonLogException();
    char *s = objToStr(pluginInstance);
    pythonLog("[XPPython3] Error getting pluginInfo for %s", s);
    free(s);
    return;
  }
  /* still need to fully remove plugin menu for this plugin -- just to be sure */
  clearInstanceMenuItems(moduleName);
  err = PyErr_Occurred();
  if (err) {
    pythonLogException();
    pythonLog("[XPPython3] Error after clearning menus for %s", CurrentPythonModuleName);
    return;
  }
  clearInstanceCommands(moduleName);
  err = PyErr_Occurred();
  if (err) {
    pythonLogException();
    pythonLog("[XPPython3] Error after clearning commands for %s", CurrentPythonModuleName);
    return;
  }
  if (PyDict_Contains(XPY3pluginDict, pluginInstance)) {
    PyDict_DelItem(XPY3pluginDict, pluginInstance);
    err = PyErr_Occurred();
    if (err) {
      pythonLogException();
      pythonLog("[XPPython3] Error after deleting %s from XPpluginDict.", CurrentPythonModuleName);
      return;
    }
  } else {
    pythonLog("[XPPython3] Plugin is not in plugindict! %s", CurrentPythonModuleName);
  }
  if (PyDict_Contains(XPY3moduleDict, moduleName)) {
    PyDict_DelItem(XPY3moduleDict, moduleName);
  }
  Py_DECREF(pluginInstance);
  pythonDebug("%*s Cleaned instance: %s", 6, " ", CurrentPythonModuleName);
  err = PyErr_Occurred();
  if (err) {
    pythonLogException();
    pythonLog("[XPPython3] Error inside cleanUpInstance() for %s", CurrentPythonModuleName);
  }
}
