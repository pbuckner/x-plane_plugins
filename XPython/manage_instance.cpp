#define _GNU_SOURCE 1
//Python comes first!
#include <Python.h>
#include "utils.h"
#include "manage_instances.h"
#include "manage_instance.h"
#include "menus.h"
#include "utilities.h"

static void updatePluginDict(PluginInfo *pluginInfo, PyObject*,PyObject*, PyObject*, PluginType);
static PyObject *getPluginInstanceBySignature(PyObject *);

int xpy_startInstance(PyObject *pModule, PyObject* pluginInstance, PluginType plugin_type) {
  /* Start loaded instance, update  XP3pluginDict with information   */

  /* start with a nearly empty struct -- we need the stable module_name */
  PluginInfo *pluginInfo = (PluginInfo*)malloc(sizeof(PluginInfo));
  pluginInfo->module_name = CurrentPythonModuleName;  // Already interned via set_moduleName()

  PyObject *pRes = PyObject_CallMethod(pluginInstance, "XPluginStart", nullptr);
  PyObject *err = PyErr_Occurred();
  if (err) {
    pythonLogException();
    pythonLog("[%s] Failed to start, error in XPluginStart", CurrentPythonModuleName);
    return 0;
  }

  pythonDebug("Starting instance %s", CurrentPythonModuleName);
  if (pRes) {
    PyObject *name = PyTuple_GetItem(pRes, PLUGIN_NAME);
    PyObject *sig  = PyTuple_GetItem(pRes, PLUGIN_SIGNATURE);
    PyObject *desc = PyTuple_GetItem(pRes, PLUGIN_DESCRIPTION);
    if(PyTuple_Check(pRes) && PyTuple_Size(pRes) == 3 && PyUnicode_Check(name) && PyUnicode_Check(sig) && PyUnicode_Check(desc)) {
      PyObject *u1 = PyUnicode_AsUTF8String(name);
      PyObject *u2 = PyUnicode_AsUTF8String(sig);
      PyObject *u3 = PyUnicode_AsUTF8String(desc);
      if(u1 && u2 && u3){
        pythonLog("[XPPython3] %s initialized.", CurrentPythonModuleName);
        pythonLog("[XPPython3]  Name: %s", PyBytes_AsString(u1));
        pythonLog("[XPPython3]  Sig:  %s", PyBytes_AsString(u2));
        pythonLog("[XPPython3]  Desc: %s", PyBytes_AsString(u3));
        pythonLogFlush();
        Py_DECREF(u1);
        Py_DECREF(u2);
        Py_DECREF(u3);

        updatePluginDict(pluginInfo, pModule, pRes, pluginInstance, plugin_type);
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

void updatePluginDict(PluginInfo *pluginInfo, PyObject * /* pModule */, PyObject *pRes, PyObject *pluginInstance, PluginType plugin_type) {

  // Extract strings from Python tuple
  const char *name = PyUnicode_AsUTF8(PyTuple_GetItem(pRes, PLUGIN_NAME));
  const char *signature = PyUnicode_AsUTF8(PyTuple_GetItem(pRes, PLUGIN_SIGNATURE));
  const char *description = PyUnicode_AsUTF8(PyTuple_GetItem(pRes, PLUGIN_DESCRIPTION));

  pluginInfo->name = name ? name : "";
  pluginInfo->signature = signature ? signature : "";
  pluginInfo->description = description ? description : "";
  //  pluginInfo.module_name = std::string(CurrentPythonModuleName);
  pluginInfo->disabled = false;
  pluginInfo->plugin_type = plugin_type;

  //set_moduleName(pluginInfo.module_name);
  Py_INCREF(pluginInstance);
  XPY3pluginInfoDict[pluginInstance] = *pluginInfo;
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
    const char *s = PyUnicode_AsUTF8(signature);
    pythonLog("[XPPython3] Could not find plugin for signature: %s", s ? s : "");
    return;
  }

  auto pluginIt = XPY3pluginInfoDict.find(pluginInstance);
  if (pluginIt == XPY3pluginInfoDict.end()) {
    pythonLog("[XPPython3] Could not find plugin info for instance");
    return;
  }

  const PluginInfo& pluginInfo = pluginIt->second;
  PluginType saved_type = pluginInfo.plugin_type;
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
  PyObject *module = PyImport_ImportModule(pluginInfo.module_name);
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
    if(xpy_startInstance(module, pluginInstance, saved_type)) {
      /* on reload, we attempt to reload _all_ plugins, even if they'd previously
         request to be disabled */
      auto newPluginIt = XPY3pluginInfoDict.find(pluginInstance);
      if (newPluginIt != XPY3pluginInfoDict.end()) {
        /* we could not enable the plugin (or plugin requested not to be enabled
           Mark it as disabled, so we'll not send messages (or Disable) to it later
        */
        newPluginIt->second.disabled = (0 == xpy_enableInstance(pluginInstance));
      }
    }
  }
}
  
PyObject *getPluginInstanceBySignature(PyObject *signature){
  const char *signatureStr = PyUnicode_AsUTF8(signature);
  for (const auto& [pluginInstance, pluginInfo] : XPY3pluginInfoDict) {
    if (pluginInfo.signature == signatureStr) {
      pythonDebug("Found plugin for signature: %s", signatureStr);
      return pluginInstance;
    }
  }
  return nullptr;
}

int xpy_enableInstance(PyObject *pluginInstance) {
  pythonDebug("  Enabling instance: %s", CurrentPythonModuleName);
  PyObject *pRes = PyObject_CallMethod(pluginInstance, "XPluginEnable", nullptr);

  if(PyErr_Occurred()) {
    pythonLogException();
    pythonLog("[XPPython3] Error occurred during call to %s XPluginEnable", CurrentPythonModuleName);
    return 0;
  }

  if(!(pRes && PyLong_Check(pRes))){
    pythonLog("[XPPython3] %s XPluginEnable returned '%s' rather than an integer.", CurrentPythonModuleName, PyUnicode_AsUTF8(pRes));
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
      pythonLog("[XPPython3] Error occurred during call to %s XPluginDisable", CurrentPythonModuleName);
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
    pythonLog("[XPPython3] Error getting pluginInfo for %s", PyUnicode_AsUTF8(pluginInstance));
    return;
  }
  /* still need to fully remove plugin menu for this plugin -- just to be sure */
  clearInstanceMenuItems(CurrentPythonModuleName);
  err = PyErr_Occurred();
  if (err) {
    pythonLogException();
    pythonLog("[XPPython3] Error after clearing menus for %s", CurrentPythonModuleName);
    return;
  }
  clearInstanceCommands(CurrentPythonModuleName);
  err = PyErr_Occurred();
  if (err) {
    pythonLogException();
    pythonLog("[XPPython3] Error after clearing commands for %s", CurrentPythonModuleName);
    return;
  }
  pythonDebug("%*s Cleaned instance: %s", 6, " ", CurrentPythonModuleName);
  /* NOTE 'CurrentPythonModuleName' will no longer be valued after erase */
  auto pluginIt = XPY3pluginInfoDict.find(pluginInstance);
  if (pluginIt != XPY3pluginInfoDict.end()) {
    XPY3pluginInfoDict.erase(pluginIt);
  } else {
    pythonLog("[XPPython3] Plugin is not in plugindict!");
  }
  Py_DECREF(pluginInstance);
  err = PyErr_Occurred();
  if (err) {
    pythonLogException();
    pythonLog("[XPPython3] Error inside cleanUpInstance()");
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

