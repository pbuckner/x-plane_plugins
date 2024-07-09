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
    pythonLog("[XPPython3] Scanning for plugins in '%s': directory not found.", path);
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
            pythonLog("[XPPython3] Failed to load pluginInstance for '%s'", pkgModName);
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
 * Updates XPY3moduleDict and XPpluginDict with informaiton about
 * loaded plugin.
 *
 * Returns pluginInstance on success (or NULL)
 */
{
  int already_loaded = 0;
  PyObject *err;
  pythonDebug("loading module '%s'", fname);
  PyObject *module_name_p = PyUnicode_DecodeFSDefault(fname);
  if (module_name_p) {
    set_moduleName(module_name_p);
    PyObject *sys_modules = PySys_GetObject("modules"); /* borrowed */
    already_loaded = (PyDict_Contains(sys_modules, module_name_p) == 1);
    PyObject *module_p = PyImport_Import(module_name_p); /* returns new reference */
    err = PyErr_Occurred();
    if (err){
      pythonLogException();
      pythonDebug("Error occured during import of %s", fname);
      char *s = objToStr(err);
      pythonDebug("%s\n^^^^", s);
      free(s);
    }
    if (module_p) {
      PyObject *module2_p;
      char *s;
      if (already_loaded) {
        module2_p = PyImport_ReloadModule(module_p);
        if (PyErr_Occurred()){
          pythonLogException();
          pythonLog("[XPPython3] Problem reloading module '%s'.", fname);
          return NULL;
        }
        s = objDebug(module2_p);
        pythonDebug("reloaded '%s'", s);
        Py_DECREF(module_p);
      } else {
        s = objDebug(module_p);
        pythonDebug("loaded '%s'", s);
        module2_p = module_p;
      }
      if (pythonDebugs) free(s); /* because if not debug, objDebug() doesn't allocate */
        
      PyObject *pClass = PyObject_GetAttrString(module2_p, "PythonInterface");
      if (pClass && PyCallable_Check(pClass)) {
        PyObject *pluginInstance = PyObject_CallObject(pClass, NULL);
        if (PyErr_Occurred()){
          pythonLogException();
          pythonLog("[XPPython3] Problem loading PythonInterface object in %s.", fname);
          return NULL;
        }
        Py_DECREF(pClass);
        if (pluginInstance) {
          return xpy_startInstance(module_name_p, module2_p, pluginInstance) ? pluginInstance : NULL;
        }
      } else {
        pythonDebug(" . Failed to get callable PythonInterface class");
        Py_DECREF(module_name_p);
        Py_DECREF(module2_p);
        pythonLog("[XPPython3] Problem getting PythonInterface class in %s.", fname);
      }
    } else {
      Py_DECREF(module_name_p);
      pythonLog("[XPPython3] Problem importing module for %s.", fname);
    }
  } else {
    pythonLog("[XPPython3] Problem decoding the filename %s.", fname);
  }
  if(PyErr_Occurred()) {
    pythonLogException();
  }
  return NULL;
}

