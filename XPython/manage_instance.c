#define _GNU_SOURCE 1
//Python comes first!
#include <Python.h>
#include "utils.h"
#include "manage_instances.h"
#include "manage_instance.h"
#include "menu.h"

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
        fprintf(pythonLogFile, "%s initialized.\n", name);
        fprintf(pythonLogFile, "  Name: %s\n", PyBytes_AsString(u1));
        fprintf(pythonLogFile, "  Sig:  %s\n", PyBytes_AsString(u2));
        fprintf(pythonLogFile, "  Desc: %s\n", PyBytes_AsString(u3));
        fflush(pythonLogFile);
        Py_DECREF(u1);
        Py_DECREF(u2);
        Py_DECREF(u3);

        updatePluginDict(pName, pModule, pRes, pluginInstance);
        free(name);
        return 1;
      } else {
        fprintf(pythonLogFile, "Failed to decode start information in %s\n", name);
      }
    } else {
      Py_DECREF(pRes);
      fprintf(pythonLogFile, "Unable to start plugin in file %s: XPluginStart did not return Name, Sig, and Desc.", name);
    }
  } else {
    fprintf(pythonLogFile, "XPluginStart returned NULL\n"); // NULL is error, Py_None is void, we're looking for a tuple[3]
  }
  free(name);
  if(PyErr_Occurred()) {
    pythonLogException();
  }
  return 0;
}

void updatePluginDict(PyObject *pName, PyObject *pModule, PyObject *pRes, PyObject *pluginInstance) {
  PyObject *pluginInfo = PyTuple_New(5);  /* pluginInfo is new reference */

  /* PyTuple_GetItem borrows reference, PyTuple_SetItem steals:pluginInfo now owns pRes[0] */
  PyObject *tmp;

  tmp = PyTuple_GetItem(pRes, PLUGIN_NAME);
  Py_INCREF(tmp);
  PyTuple_SetItem(pluginInfo, PLUGIN_NAME, tmp);

  tmp = PyTuple_GetItem(pRes, PLUGIN_SIGNATURE);
  Py_INCREF(tmp);
  PyTuple_SetItem(pluginInfo, PLUGIN_SIGNATURE, tmp);

  tmp = PyTuple_GetItem(pRes, PLUGIN_DESCRIPTION);
  Py_INCREF(tmp);
  PyTuple_SetItem(pluginInfo, PLUGIN_DESCRIPTION, tmp);

  PyTuple_SetItem(pluginInfo, PLUGIN_MODULE, pModule);

  PyTuple_SetItem(pluginInfo, PLUGIN_MODULE_NAME, pName);

  PyDict_SetItem(moduleDict, pluginInfo, pluginInstance);
  PyDict_SetItem(pluginDict, pluginInstance, pluginInfo);
  if(PyErr_Occurred()) {
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
    fprintf(pythonLogFile, "Could not find plugin for signature: %s\n", objToStr(signature));
    return;
  }

  PyObject *moduleName = PyTuple_GetItem(pluginInfo, PLUGIN_MODULE_NAME);
  pythonDebug("  which is module: %s", objDebug(moduleName));
  PyObject *pluginInstance = PyDict_GetItem(moduleDict, pluginInfo);
  
  xpy_disableInstance(moduleName, pluginInstance);
  xpy_stopInstance(moduleName, pluginInstance);

  /* clean up as best we can */
  /*   remove from Dicts
   */
  PyDict_DelItem(moduleDict, pluginInfo);
  PyDict_DelItem(pluginDict, pluginInstance);

  pythonDebug("Calling ReloadModule");
  PyObject *module = PyImport_ReloadModule(PyTuple_GetItem(pluginInfo, PLUGIN_MODULE));
  PyObject *err = PyErr_Occurred();
  if (err) {
    pythonLogException();
    fprintf(pythonLogFile, "[%s] Failed to reload module\n", objToStr(moduleName));
    return;
  }
  
  PyObject *pClass = PyObject_GetAttrString(module, "PythonInterface");
  if (pClass && PyCallable_Check(pClass)) {
    pythonDebug("  Getting PythonInterface class");
    pluginInstance = PyObject_CallObject(pClass, NULL);
    Py_DECREF(pClass);
    if (!pluginInstance) {
      fprintf(pythonLogFile, "[%s] Cannot restart plugin.\n", objToStr(moduleName));
      if (PyErr_Occurred()) {
        pythonLogException();
      }
      return;
    }
    if(xpy_startInstance(moduleName, module, pluginInstance)) {
      xpy_enableInstance(moduleName, pluginInstance); /* returns 1 on successful enable, 0 otherwise. */
    }
  }
}
  
PyObject *getPluginInfo(PyObject *signature){
  PyObject *pluginInfo, *pluginInstance;
  Py_ssize_t pos = 0;
  while(PyDict_Next(pluginDict, &pos, &pluginInstance, &pluginInfo)){
    if (! PyObject_RichCompareBool(PyTuple_GetItem(pluginInfo, PLUGIN_SIGNATURE), signature, Py_EQ)) {
      continue;
    }
    pythonDebug("Found plugin for signature: %s", objDebug(signature));
    return pluginInfo;
  }
  return NULL;
}

int xpy_enableInstance(PyObject *moduleName, PyObject *pluginInstance) {
  pythonDebug("Enabling instance: %s", objDebug(moduleName));
  PyObject *pRes = PyObject_CallMethod(pluginInstance, "XPluginEnable", NULL);
  if(!(pRes && PyLong_Check(pRes))){
    fprintf(pythonLogFile, "%s XPluginEnable returned '%s' rather than an integer.\n", objToStr(moduleName), objToStr(pRes));
  } else {
    pythonDebug("XPluginEnable returned %ld", PyLong_AsLong(pRes));
  }
  int ret = 0;
  if(PyErr_Occurred()) {
    pythonLogException();
    fprintf(pythonLogFile, "Error occured during the %s XPluginEnable call:\n", objToStr(moduleName));
  } else {
    ret = PyLong_AsLong(pRes) > 0 ? 1 : 0;
    Py_DECREF(pRes);
  }
  return ret;
}


void xpy_disableInstance(PyObject *moduleName, PyObject *pluginInstance) {
  pythonDebug("  Disabling instance: %s", objDebug(moduleName));
  PyObject_CallMethod(pluginInstance, "XPluginDisable", NULL);
  PyObject *err = PyErr_Occurred();

  if (err) {
    if (PyObject_HasAttrString(pluginInstance, "XPluginDisable")) {
      pythonLogException();
      fprintf(pythonLogFile, "[%s] Error occured during the XPluginDisable call:\n", objToStr(moduleName));
    } else {
      pythonDebug("  (no XPluginDisable for this module)");
      /* ignore error, if XPluginDisable is not defined in the PythonInterface class */
      PyErr_Clear();
    }
  }
}

void xpy_stopInstance(PyObject *moduleName, PyObject *pluginInstance) {
  pythonDebug("  Stopping instance: %s", objDebug(moduleName));

  int has_attr = PyObject_HasAttrString(pluginInstance, "XPluginStop");
  if (!has_attr) {
    /* ignore error, if XPluginStop is not defined in the PythonInterface class */
    pythonDebug(" (no XPluginStop for this module)");
  } else {
    PyObject *pRes = PyObject_CallMethod(pluginInstance, "XPluginStop", NULL);
    PyObject *err = PyErr_Occurred();
    if (err) {
      pythonLogException();
      fprintf(pythonLogFile, "[%s] Error occured during the XPluginStop call:\n", objToStr(moduleName));
    }
    if (pRes != Py_None && pRes != NULL) {
      fprintf(pythonLogFile, "[%s] XPluginStop returned '%s' rather than None. Value ignored\n", objToStr(moduleName), objToStr(pRes));
    }
    Py_XDECREF(pRes);
  }

  xpy_cleanUpInstance(moduleName, pluginInstance);
}

void xpy_cleanUpInstance(PyObject *moduleName, PyObject *pluginInstance) {
  pythonDebug("  Cleaning instance: %s", objDebug(moduleName));
  PyObject *pluginInfo = PyDict_GetItem(pluginDict, pluginInstance); /* borrowed */
  PyObject *err = PyErr_Occurred();
  if (err) {
    pythonLogException();
    fprintf(pythonLogFile, "Error getting pluginInfo for %s\n", objToStr(pluginInstance));
    return;
  }
  /* still need to fully remove plugin menu for this plugin -- just to be sure */
  clearInstanceMenuItems(moduleName);
  err = PyErr_Occurred();
  if (err) {
    pythonLogException();
    fprintf(pythonLogFile, "Error after clearning menus\n");
    return;
  }
  clearInstanceCommands(moduleName);
  err = PyErr_Occurred();
  if (err) {
    pythonLogException();
    fprintf(pythonLogFile, "Error after clearning commands\n");
    return;
  }
  if (PyDict_Contains(pluginDict, pluginInstance)) {
    PyDict_DelItem(pluginDict, pluginInstance);
    err = PyErr_Occurred();
    if (err) {
      pythonLogException();
      fprintf(pythonLogFile, "Error after deleting from pluginDict\n");
      return;
    }
  } else {
    fprintf(pythonLogFile, "plugin is not in plugindict!\n");
  }
  if (PyDict_Contains(moduleDict, pluginInfo)) {
    PyDict_DelItem(moduleDict, pluginInfo);
    /* Py_DECREF(pluginInfo);*/
  }
  Py_DECREF(pluginInstance);
  pythonDebug("  Cleaned instance: %s", objDebug(moduleName));
  err = PyErr_Occurred();
  if (err) {
    pythonLogException();
    fprintf(pythonLogFile, "Error inside cleanUpInstance() for %s\n", objToStr(moduleName));
  }
}
