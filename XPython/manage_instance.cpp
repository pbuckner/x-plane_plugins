#define _GNU_SOURCE 1
//Python comes first!
#include <Python.h>
#include "utils.h"
#include "manage_instances.h"
#include "manage_instance.h"
#include "menus.h"
#include "utilities.h"

static void updatePluginDict(PyObject*,PyObject*, PyObject*);
static PyObject *getPluginInstanceBySignature(PyObject *);

int xpy_startInstance(PyObject *pModule, PyObject* pluginInstance) {
  /* Start loaded instance, update  XP3pluginDict with information   */
  PyObject *pRes = PyObject_CallMethod(pluginInstance, "XPluginStart", nullptr);
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

        updatePluginDict(pModule, pRes, pluginInstance);
        return 1;
      } else {
        pythonLog("[XPPython3] Failed to decode start information in %s", CurrentPythonModuleName);
      }
    } else {
      Py_DECREF(pRes);
      pythonLog("[XPPython3] Unable to start plugin in file %s: XPluginStart did not return Name, Sig, and Desc.", CurrentPythonModuleName);
    }
  } else {
    pythonLog("[%s] XPluginStart returned nullptr", CurrentPythonModuleName); // nullptr is error, Py_None is void, we're looking for a tuple[3]
  }
  if(PyErr_Occurred()) {
    pythonLogException();
  }
  return 0;
}

void updatePluginDict(PyObject * /* pModule */, PyObject *pRes, PyObject *pluginInstance) {
  PluginInfoDict pluginInfo;

  // Extract strings from Python tuple
  char *name = objToStr(PyTuple_GetItem(pRes, PLUGIN_NAME));
  char *signature = objToStr(PyTuple_GetItem(pRes, PLUGIN_SIGNATURE));
  char *description = objToStr(PyTuple_GetItem(pRes, PLUGIN_DESCRIPTION));

  pluginInfo.name = name;
  pluginInfo.signature = signature;
  pluginInfo.description = description;
  pluginInfo.module_name = CurrentPythonModuleName;
  pluginInfo.disabled = false;

  free(name);
  free(signature);
  free(description);

  Py_INCREF(pluginInstance);
  XPY3pluginInfoDict[pluginInstance] = pluginInfo;
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
  PyObject *pluginInstance = getPluginInstanceBySignature(signature);
  if (!pluginInstance) {
    char *s = objToStr(signature);
    pythonLog("[XPPython3] Could not find plugin for signature: %s", s);
    free(s);
    return;
  }

  auto pluginIt = XPY3pluginInfoDict.find(pluginInstance);
  if (pluginIt == XPY3pluginInfoDict.end()) {
    pythonLog("[XPPython3] Could not find plugin info for instance");
    return;
  }

  const PluginInfoDict& pluginInfo = pluginIt->second;
  set_moduleName(pluginInfo.module_name);
  pythonDebug("  which is module: %s", CurrentPythonModuleName);

  if (!pluginInfo.disabled) {
    xpy_disableInstance(pluginInstance);
  }
  xpy_stopInstance(pluginInstance);

  /* clean up as best we can */
  /*   remove from Dicts
   */
    XPY3pluginInfoDict.erase(pluginInstance);

  pythonDebug("Calling ReloadModule");
  // For now, we'll use PyImport_ImportModule to reload by name
  // This is a simplification - the original code used the module object from pluginInfo
  PyObject *module = PyImport_ImportModule(pluginInfo.module_name.c_str());
  PyObject *err = PyErr_Occurred();
  if (err) {
    pythonLogException();
    pythonLog("[XPPython3] Failed to reload module '%s'", CurrentPythonModuleName);
    return;
  }

  PyObject *pClass = PyObject_GetAttrString(module, "PythonInterface");
  if (pClass && PyCallable_Check(pClass)) {
    pythonDebug("  Getting PythonInterface class");
    pluginInstance = PyObject_CallObject(pClass, nullptr);
    Py_DECREF(pClass);
    if (!pluginInstance) {
      pythonLog("[XPPython3] Cannot restart plugin '%s'.", CurrentPythonModuleName);
      if (PyErr_Occurred()) {
        pythonLogException();
      }
      return;
    }
    if(xpy_startInstance(module, pluginInstance)) {
      /* on reload, we attempt to reload _all_ plugins, even if they'd previously
         request to be disabled */
      if (0 == xpy_enableInstance(pluginInstance)) {
        /* we could not enable the plugin (or plugin requested not to be enabled
           Mark it as disabled, so we'll not sent messages (or Disable) to it later
        */
        auto newPluginIt = XPY3pluginInfoDict.find(pluginInstance);
        if (newPluginIt != XPY3pluginInfoDict.end()) {
          newPluginIt->second.disabled = true;
        }
      } else {
        auto newPluginIt = XPY3pluginInfoDict.find(pluginInstance);
        if (newPluginIt != XPY3pluginInfoDict.end()) {
          newPluginIt->second.disabled = false;
        }
      }
    }
  }
}
  
PyObject *getPluginInstanceBySignature(PyObject *signature){
  char *signatureStr = objToStr(signature);
  for (const auto& [pluginInstance, pluginInfo] : XPY3pluginInfoDict) {
    if (pluginInfo.signature == signatureStr) {
      pythonDebug("Found plugin for signature: %s", signatureStr);
      free(signatureStr);
      return pluginInstance;
    }
  }
  free(signatureStr);
  return nullptr;
}

int xpy_enableInstance(PyObject *pluginInstance) {
  pythonDebug("  Enabling instance: %s", CurrentPythonModuleName);
  PyObject *pRes = PyObject_CallMethod(pluginInstance, "XPluginEnable", nullptr);

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


void xpy_disableInstance(PyObject *pluginInstance) {
  pythonDebug("  Disabling instance: %s", CurrentPythonModuleName);
  PyObject_CallMethod(pluginInstance, "XPluginDisable", nullptr);
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

void xpy_cleanUpInstance(PyObject *pluginInstance) {
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
  clearInstanceMenuItems(CurrentPythonModuleName);
  err = PyErr_Occurred();
  if (err) {
    pythonLogException();
    pythonLog("[XPPython3] Error after clearning menus for %s", CurrentPythonModuleName);
    return;
  }
  clearInstanceCommands(CurrentPythonModuleName);
  err = PyErr_Occurred();
  if (err) {
    pythonLogException();
    pythonLog("[XPPython3] Error after clearning commands for %s", CurrentPythonModuleName);
    return;
  }
  auto pluginIt = XPY3pluginInfoDict.find(pluginInstance);
  if (pluginIt != XPY3pluginInfoDict.end()) {
    XPY3pluginInfoDict.erase(pluginIt);
  } else {
    pythonLog("[XPPython3] Plugin is not in plugindict! %s", CurrentPythonModuleName);
  }
  Py_DECREF(pluginInstance);
  pythonDebug("%*s Cleaned instance: %s", 6, " ", CurrentPythonModuleName);
  err = PyErr_Occurred();
  if (err) {
    pythonLogException();
    pythonLog("[XPPython3] Error inside cleanUpInstance() for %s", CurrentPythonModuleName);
  }
}

void xpy_stopInstance(PyObject *pluginInstance) {
  pythonDebug("%*s Stopping instance: %s", 2, " ", CurrentPythonModuleName);

  int has_attr = PyObject_HasAttrString(pluginInstance, "XPluginStop");
  if (!has_attr) {
    /* ignore error, if XPluginStop is not defined in the PythonInterface class */
    pythonDebug("%*s (no XPluginStop for this module)", 4, " ");
  } else {
    PyObject *pRes = PyObject_CallMethod(pluginInstance, "XPluginStop", nullptr);
    PyObject *err = PyErr_Occurred();
    if (err) {
      pythonLogException();
      pythonLog("[XPPython3] Error occurred during call to %s XPluginStop", CurrentPythonModuleName);
    }
    if (pRes != Py_None && pRes != nullptr) {
      pythonDebug("XPluginStop for %s returned '%s' rather than None. Value ignored\n", CurrentPythonModuleName, objDebug(pRes));
    }
    Py_XDECREF(pRes);
  }

  xpy_cleanUpInstance(pluginInstance);
}

