#define _GNU_SOURCE 1
//Python comes first!
#include <Python.h>
#include <dirent.h>
#include <regex.h>
#include "manage_instance.h"
#include "load_modules.h"

#include "utils.h"

static PyObject *loadPIClass(const char *fname, PluginType plugin_type);

void xpy_loadModules(const char *path, const char *package, const char *pattern, PyObject* pluginList, PluginType plugin_type)
{
  //Scan current directory for the plugin modules, loads and calls XPluginStart()
  set_moduleName(XPPython3ModuleName);
  DIR *dir = opendir(path);
  PyObject *pluginInstance;
  if(dir == nullptr){
    pythonLog("[XPPython3] Scanning for plugins in '%s': directory not found.", path);
    pythonLogFlush();
    return;
  }
  struct dirent *de;
  regex_t rex;
  if(regcomp(&rex, pattern, REG_NOSUB) == 0){
    while((de = readdir(dir))){
      // pythonDebug("Checking file name %s against pattern %s", de->d_name, pattern);
      if(regexec(&rex, de->d_name, 0, nullptr, 0) == 0 && strstr(de->d_name, "flymake.py") == nullptr){
        char *modName = strdup(de->d_name);
        if(modName){
          modName[strlen(de->d_name) - 3] = '\0';
          char *pkgModName;
          if (-1 == asprintf(&pkgModName, "%s.%s", package, modName)) {
            pythonLog("[XPPython3] Failed to allocate memory for package module name");
            continue;
          }
          /* We want to load as part of packages
             "XPPython3.I_PI_<plugin>.py"
             "PythonPlugins.PI_<plugin>.py"
             "Laminar Research.Baron B58.plugins.PythonPlugions.PI_<plugin>.py"
          */
          set_moduleName(pkgModName);
          pluginInstance = loadPIClass(pkgModName, plugin_type);
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


static PyObject *loadPIClass(const char *fname, PluginType plugin_type)
/* Load "PythonInterface" class in provided file (fname)
 * and call XPluginStart
 *
 * Updates XPY3pluginDict with information about
 * loaded plugin.
 *
 * Returns pluginInstance on success (or nullptr)
 */
{
  PyObject *err;
  pythonDebug("loading module '%s'", fname);
  PyObject *module_name_p = PyUnicode_DecodeFSDefault(fname);
  if (!module_name_p) {
    pythonLog("[XPPython3] Problem decoding the filename %s.", fname);
    if(PyErr_Occurred()) {
      pythonLogException();
    }
    return nullptr;
  }
    
  PyObject *sys_modules = PySys_GetObject("modules"); /* borrowed */
  int already_loaded = (PyDict_Contains(sys_modules, module_name_p) == 1);
  PyObject *module_p = PyImport_Import(module_name_p); /* returns new reference */
  err = PyErr_Occurred();
  if (err){
    pythonLogException();
    pythonDebug("Error occured during import of %s", fname);
    char *s = objToStr(err);
    pythonDebug("%s\n^^^^", s);
    free(s);
    return nullptr;
  }

  PyObject *module2start_p;
  
  /* if it's already loaded, we reload the module before trying to start it */
  char *s;
  if (already_loaded) {
    module2start_p = PyImport_ReloadModule(module_p);
    if (PyErr_Occurred()){
      pythonLogException();
      pythonLog("[XPPython3] Problem reloading module '%s'.", fname);
      return nullptr;
    }
    s = objToStr(module2start_p);
    pythonDebug("reloaded '%s'", s);
    Py_DECREF(module_p);
    
  } else {
    s = objToStr(module_p);
    pythonDebug("loaded '%s'", s);
    module2start_p = module_p;
  }
  free(s);
  
  /* If module has PythonInterface, we get an instance of it... */
  PyObject *pClass = PyObject_GetAttrString(module2start_p, "PythonInterface");
  if (pClass && PyCallable_Check(pClass)) {
    PyObject *pluginInstance = PyObject_CallObject(pClass, nullptr);
    if (PyErr_Occurred()){
      pythonLogException();
      pythonLog("[XPPython3] Problem loading PythonInterface object in %s.", fname);
      return nullptr;
    }
    Py_DECREF(pClass);

    /* ... and START it, return instance on success, nullptr on failure */
    /*     xpy_startInstance will update PluginDict */
    return xpy_startInstance(module2start_p, pluginInstance, plugin_type) ? pluginInstance : nullptr;

  } 

  pythonDebug(" . Failed to get callable PythonInterface class");
  Py_DECREF(module_name_p);
  Py_DECREF(module2start_p);
  pythonLog("[XPPython3] Problem getting PythonInterface class in %s.", fname);

  if(PyErr_Occurred()) {
    pythonLogException();
  }
  return nullptr;
}

