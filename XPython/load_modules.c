#define _GNU_SOURCE 1
//Python comes first!
#include <Python.h>
#include <dirent.h>
#include <regex.h>
#include "manage_instance.h"
#include "load_modules.h"

#include "utils.h"

static PyObject *loadPIClass(const char *fname);

void xpy_loadModules(const char *path, const char *package, const char *pattern, PyObject* pluginList)
{
  //Scan current directory for the plugin modules, loads and calls XPluginStart()
  DIR *dir = opendir(path);
  PyObject *pluginInstance;
  if(dir == NULL){
    pythonLog("[XPPython3] Scanning for plugins in '%s': directory not found.\n", path);
    pythonLogFlush();
    return;
  }
  struct dirent *de;
  regex_t rex;
  if(regcomp(&rex, pattern, REG_NOSUB) == 0){
    while((de = readdir(dir))){
      // pythonDebug("Checking file name %s against pattern %s", de->d_name, pattern);
      if(regexec(&rex, de->d_name, 0, NULL, 0) == 0 && strstr(de->d_name, "flymake.py") == NULL){
        char *modName = strdup(de->d_name);
        if(modName){
          modName[strlen(de->d_name) - 3] = '\0';
          char *pkgModName = malloc(strlen(modName) + strlen(package) + 2);
          strcpy(pkgModName, package);
          strcat(pkgModName, ".");
          strcat(pkgModName, modName);
          /* We want to load as part of packages
             "XPPython3.I_PI_<plugin>.py"
             "PythonPlugins.PI_<plugin>.py"
             "Laminar Research.Baron B58.plugins.PythonPlugions.PI_<plugin>.py"
          */
          pluginInstance = loadPIClass(pkgModName);
          if (pluginInstance) {
            if (pluginList) {
              /* we're putting the resulting plugins into a list... */
              PyList_Append(pluginList, pluginInstance);
            }
          } else {
            pythonLog("[XPPython3] Failed to load pluginInstance for '%s'\n", pkgModName);
          }
          free(pkgModName);
        }
        free(modName);
      }
    }
    regfree(&rex);
    closedir(dir);
  }
}


static PyObject *loadPIClass(const char *fname)
/* Load "PythonInterface" class in provided file (fname)
 * and call XPluginStart
 * 
 * Updates moduleDict and pluginDict with informaiton about
 * loaded plugin.
 *
 * Returns pluginInstance on success (or NULL)
 */
{
  int already_loaded = 0;
  PyObject *err;
  pythonDebug(" . Loading class in '%s'", fname);
  PyObject *pName = PyUnicode_DecodeFSDefault(fname);
  if (pName) {
    PyObject *sys_modules = PySys_GetObject("modules"); /* borrowed */
    already_loaded = (PyDict_Contains(sys_modules, pName) == 1);
    PyObject *pModule1 = PyImport_Import(pName); /* returns new reference */
    err = PyErr_Occurred();
    if (err){
      pythonLogException();
      pythonDebug("Error occured during import of %s", fname);
      pythonDebug("%s\n^^^^", objToStr(err));
    }
    if (pModule1) {
      PyObject *pModule;
      if (already_loaded) {
        pModule = PyImport_ReloadModule(pModule1);
        pythonDebug(" . Module reloaded '%s'", objDebug(pModule));
        Py_DECREF(pModule1);
      } else {
        pythonDebug(" . Module loaded '%s'", objDebug(pModule1));
        pModule = pModule1;
      }
        
      PyObject *pClass = PyObject_GetAttrString(pModule, "PythonInterface");
      if (pClass && PyCallable_Check(pClass)) {
        PyObject *pluginInstance = PyObject_CallObject(pClass, NULL);
        if (PyErr_Occurred()){
          pythonLogException();
          pythonLog("[XPPython3] Problem loading PythonInterface object in %s.\n", fname);
          return NULL;
        }
        Py_DECREF(pClass);
        if (pluginInstance) {
          return xpy_startInstance(pName, pModule, pluginInstance) ? pluginInstance : NULL;
        }
      } else {
        pythonDebug(" . Failed to get callable PythonInterface class");
        Py_DECREF(pName);
        Py_DECREF(pModule);
        pythonLog("[XPPython3] Problem getting PythonInterface class in %s.\n", fname);
      }
    } else {
      Py_DECREF(pName);
      pythonLog("[XPPython3] Problem importing module for %s.\n", fname);
    }
  } else {
    pythonLog("[XPPython3] Problem decoding the filename %s.\n", fname);
  }
  if(PyErr_Occurred()) {
    pythonLogException();
  }
  return NULL;
}

