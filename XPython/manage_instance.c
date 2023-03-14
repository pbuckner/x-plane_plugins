#define _GNU_SOURCE 1
//Python comes first!
#include <Python.h>
#include "utils.h"
#include "manage_instances.h"
#include "manage_instance.h"
#include "menus.h"

extern void clearInstanceMenuItems();
extern void clearInstanceCommands();
static void updatePluginDict(PyObject*, PyObject*,PyObject*, PyObject*);
static PyObject *getPluginInfo(PyObject *);

int xpy_startInstance(PyObject *pName, PyObject *pModule, PyObject* pluginInstance) {  
  /* Start loaded instance, update moduleDict and pluginDict with information
   */
  PyObject *pRes = PyObject_CallMethod(pluginInstance, "XPluginStart", NULL);
  PyObject *err = PyErr_Occurred();
  if (err) {
    pythonLogException();
    fprintf(pythonLogFile, "[%s] Failed to start, error in XPluginStart\n", objToStr(pName));
    return 0;
  }
  char *name = objToStr(pName);

  pythonDebug("Starting instance %s", name);
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
        fprintf(pythonLogFile, "[XPPython3] %s initialized.\n", name);
        fprintf(pythonLogFile, "[XPPython3]  Name: %s\n", PyBytes_AsString(u1));
        fprintf(pythonLogFile, "[XPPython3]  Sig:  %s\n", PyBytes_AsString(u2));
        fprintf(pythonLogFile, "[XPPython3]  Desc: %s\n", PyBytes_AsString(u3));
        fflush(pythonLogFile);
        Py_DECREF(u1);
        Py_DECREF(u2);
        Py_DECREF(u3);

        updatePluginDict(pName, pModule, pRes, pluginInstance);
        free(name);
        return 1;
      } else {
        fprintf(pythonLogFile, "[XPPython3] Failed to decode start information in %s\n", name);
      }
    } else {
      Py_DECREF(pRes);
      fprintf(pythonLogFile, "[XPPython3] Unable to start plugin in file %s: XPluginStart did not return Name, Sig, and Desc.", name);
    }
  } else {
    fprintf(pythonLogFile, "[%s] XPluginStart returned NULL\n", name); // NULL is error, Py_None is void, we're looking for a tuple[3]
  }
  free(name);
  if(PyErr_Occurred()) {
    pythonLogException();
  }
  return 0;
}

void updatePluginDict(PyObject *pName, PyObject *pModule, PyObject *pRes, PyObject *pluginInstance) {
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

  Py_INCREF(pName);
  PyList_SetItem(pluginInfo, PLUGIN_MODULE_NAME, pName);
  Py_INCREF(Py_False);
  PyList_SetItem(pluginInfo, PLUGIN_DISABLED, Py_False);

  Py_INCREF(pluginInstance);
  PyDict_SetItem(moduleDict, pName, pluginInstance);
  Py_INCREF(pluginInfo);
  PyDict_SetItem(pluginDict, pluginInstance, pluginInfo);
  if(PyErr_Occurred()) {
    fprintf(pythonLogFile, "Error while updating plugin dict\n");
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
    fprintf(pythonLogFile, "[XPPython3] Could not find plugin for signature: %s\n", objToStr(signature));
    return;
  }

  PyObject *moduleName = PyList_GetItem(pluginInfo, PLUGIN_MODULE_NAME);
  pythonDebug("  which is module: %s", objDebug(moduleName));
  PyObject *pluginInstance = PyDict_GetItem(moduleDict, moduleName);
  
  if (PyList_GetItem(pluginInfo, PLUGIN_DISABLED) == Py_False) {
    xpy_disableInstance(moduleName, pluginInstance);
  }
  xpy_stopInstance(moduleName, pluginInstance);

  /* clean up as best we can */
  /*   remove from Dicts
   */
  PyDict_DelItem(moduleDict, moduleName);
  PyDict_DelItem(pluginDict, pluginInstance);

  pythonDebug("Calling ReloadModule");
  PyObject *module = PyImport_ReloadModule(PyList_GetItem(pluginInfo, PLUGIN_MODULE));
  PyObject *err = PyErr_Occurred();
  if (err) {
    pythonLogException();
    fprintf(pythonLogFile, "[XPPython3] Failed to reload module '%s'\n", objToStr(moduleName));
    return;
  }
  
  PyObject *pClass = PyObject_GetAttrString(module, "PythonInterface");
  if (pClass && PyCallable_Check(pClass)) {
    pythonDebug("  Getting PythonInterface class");
    pluginInstance = PyObject_CallObject(pClass, NULL);
    Py_DECREF(pClass);
    if (!pluginInstance) {
      fprintf(pythonLogFile, "[XPPython3] Cannot restart plugin '%s'.\n", objToStr(moduleName));
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
  while(PyDict_Next(pluginDict, &pos, &pluginInstance, &pluginInfo)){
    if (! PyObject_RichCompareBool(PyList_GetItem(pluginInfo, PLUGIN_SIGNATURE), signature, Py_EQ)) {
      continue;
    }
    pythonDebug("Found plugin for signature: %s", objDebug(signature));
    return pluginInfo;
  }
  return NULL;
}

int xpy_enableInstance(PyObject *moduleName, PyObject *pluginInstance) {
  pythonDebug("  Enabling instance: %s", objDebug(moduleName));
  PyObject *pRes = PyObject_CallMethod(pluginInstance, "XPluginEnable", NULL);

  if(PyErr_Occurred()) {
    pythonLogException();
    fprintf(pythonLogFile, "[XPPython3] Error occured during call to %s XPluginEnable\n", objToStr(moduleName));
    return 0;
  }

  if(!(pRes && PyLong_Check(pRes))){
    fprintf(pythonLogFile, "[XPPython3] %s XPluginEnable returned '%s' rather than an integer.\n", objToStr(moduleName), objToStr(pRes));
  } else {
    pythonDebug("    %s XPluginEnable returned %ld", objDebug(moduleName), PyLong_AsLong(pRes));
  }

  int ret = PyLong_AsLong(pRes) > 0 ? 1 : 0;
  if (ret == 0) {
    fprintf(pythonLogFile, "[XPPython3] %s not enabled.\n", objToStr(moduleName));
  }
  Py_DECREF(pRes);
  return ret;
}


void xpy_disableInstance(PyObject *moduleName, PyObject *pluginInstance) {
  pythonDebug("  Disabling instance: %s", objDebug(moduleName));
  PyObject_CallMethod(pluginInstance, "XPluginDisable", NULL);
  PyObject *err = PyErr_Occurred();

  if (err) {
    if (PyObject_HasAttrString(pluginInstance, "XPluginDisable")) {
      pythonLogException();
      fprintf(pythonLogFile, "[XPPython3] Error occured during call to %s XPluginDisable\n", objToStr(moduleName));
    } else {
      pythonDebug("  (no XPluginDisable for %s module)", objDebug(moduleName));
      /* ignore error, if XPluginDisable is not defined in the PythonInterface class */
      PyErr_Clear();
    }
  }
}

void xpy_stopInstance(PyObject *moduleName, PyObject *pluginInstance) {
  pythonDebug("%*s Stopping instance: %s", 2, " ", objDebug(moduleName));

  int has_attr = PyObject_HasAttrString(pluginInstance, "XPluginStop");
  if (!has_attr) {
    /* ignore error, if XPluginStop is not defined in the PythonInterface class */
    pythonDebug("%*s (no XPluginStop for this module)", 4, " ");
  } else {
    PyObject *pRes = PyObject_CallMethod(pluginInstance, "XPluginStop", NULL);
    PyObject *err = PyErr_Occurred();
    if (err) {
      pythonLogException();
      fprintf(pythonLogFile, "[XPPython3] Error occurred during call to %s XPluginStop\n", objToStr(moduleName));
    }
    if (pRes != Py_None && pRes != NULL) {
      pythonDebug("XPluginStop for %s returned '%s' rather than None. Value ignored\n", objDebug(moduleName), objDebug(pRes));
    }
    Py_XDECREF(pRes);
  }

  xpy_cleanUpInstance(moduleName, pluginInstance);
}

void xpy_cleanUpInstance(PyObject *moduleName, PyObject *pluginInstance) {
  pythonDebug("%*s Cleaning instance: %s", 4, " ", objDebug(moduleName));
  PyObject *err = PyErr_Occurred();
  if (err) {
    pythonLogException();
    fprintf(pythonLogFile, "[XPPython3] Error getting pluginInfo for %s\n", objToStr(pluginInstance));
    return;
  }
  /* still need to fully remove plugin menu for this plugin -- just to be sure */
  clearInstanceMenuItems(moduleName);
  err = PyErr_Occurred();
  if (err) {
    pythonLogException();
    fprintf(pythonLogFile, "[XPPython3] Error after clearning menus for %s\n", objToStr(moduleName));
    return;
  }
  clearInstanceCommands(moduleName);
  err = PyErr_Occurred();
  if (err) {
    pythonLogException();
    fprintf(pythonLogFile, "[XPPython3] Error after clearning commands for %s\n", objToStr(moduleName));
    return;
  }
  if (PyDict_Contains(pluginDict, pluginInstance)) {
    PyDict_DelItem(pluginDict, pluginInstance);
    err = PyErr_Occurred();
    if (err) {
      pythonLogException();
      fprintf(pythonLogFile, "[XPPython3] Error after deleting %s from pluginDict.\n", objToStr(moduleName));
      return;
    }
  } else {
    fprintf(pythonLogFile, "[XPPython3] Plugin is not in plugindict! %s\n", objToStr(moduleName));
  }
  if (PyDict_Contains(moduleDict, moduleName)) {
    PyDict_DelItem(moduleDict, moduleName);
  }
  Py_DECREF(pluginInstance);
  pythonDebug("%*s Cleaned instance: %s", 6, " ", objDebug(moduleName));
  err = PyErr_Occurred();
  if (err) {
    pythonLogException();
    fprintf(pythonLogFile, "[XPPython3] Error inside cleanUpInstance() for %s\n", objToStr(moduleName));
  }
}
