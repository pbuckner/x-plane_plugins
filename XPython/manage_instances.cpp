//Python comes first!
#include <Python.h>
#include <XPLM/XPLMPlanes.h>
#include "manage_instances.h"
#include "load_modules.h"
#include "manage_instance.h"
#include <unordered_map>
#include "utils.h"

PyObject *XPY3aircraftPlugins; /* [instance, instance, ] */
PyObject *XPY3sceneryPlugins; /* [instance, instance, ] */

static void stopPluginList(PyObject *);
static void disablePluginList(PyObject *);
static void enablePluginList(PyObject *);

static void xpy_startInternalPlugins(void);
static void xpy_enableInternalPlugins(void);
static void xpy_disableInternalPlugins(void);
static void xpy_stopInternalPlugins(void);

static void xpy_startGlobalPlugins(void);
static void xpy_enableGlobalPlugins(void);
static void xpy_disableGlobalPlugins(void);
static void xpy_stopGlobalPlugins(void);

std::unordered_map<PyObject *, PluginInfo> XPY3pluginInfoDict;

/****************************************************************
 * "START"
 * Find, load and send "XPluginStart"
 *   path is set, internal dictionaries are updated
 ****************************************************************/

void xpy_startInternalPlugins()
{
  pythonDebug("STARTING Internal plugins...");
  xpy_loadModules(pythonInternalPluginsPath, "XPPython3", "^I_PI_[^.]*\\.py$", nullptr, PLUGIN_INTERNAL);
  pythonDebug("STARTED INTERNAL plugins.");
}

void xpy_startGlobalPlugins()
{
  pythonDebug("STARTING Global plugins...");
  xpy_loadModules(pythonPluginsPath, "PythonPlugins", "^PI_[^.]*\\.py$", nullptr, PLUGIN_GLOBAL);
  pythonDebug("STARTED Global plugins.");
}

void xpy_enableInternalPlugins()
{
  pythonDebug("ENABLING Internal plugins");
  for (auto& [pluginInstance, info] : XPY3pluginInfoDict) {
    if (info.plugin_type != PLUGIN_INTERNAL) {
      continue;
    }
    set_moduleName(info.module_name);
    if (0 == xpy_enableInstance(pluginInstance)) {
      info.disabled = true;
    }
  }
  pythonDebug("ENABLED Internal plugins");
}

void xpy_disableInternalPlugins()
{
  pythonDebug("DISABLING Internal plugins");
  for (const auto& [pluginInstance, info] : XPY3pluginInfoDict) {
    if (info.plugin_type != PLUGIN_INTERNAL) {
      continue;
    }
    if (!info.disabled) {
      set_moduleName(info.module_name);
      xpy_disableInstance(pluginInstance);
    }
  }
  pythonDebug("DISABLED Internal plugins");
}

void xpy_stopInternalPlugins()
{
  pythonDebug("STOPPING Internal plugins");
  for (auto it = XPY3pluginInfoDict.begin(); it != XPY3pluginInfoDict.end(); ) {
    if (it->second.plugin_type != PLUGIN_INTERNAL) {
      ++it;
      continue;
    }
    set_moduleName(it->second.module_name);
    PyObject *pluginInstance = it->first;
    ++it;
    xpy_stopInstance(pluginInstance);
    set_moduleName(XPPython3ModuleName);
  }
  pythonDebug("STOPPED Internal plugins");
}


void xpy_startInstances(int include_aircraft)
/* find, load, start global, scenery and (optionally) aircraft plugins */
{
  xpy_startInternalPlugins();
  xpy_startGlobalPlugins();
  xpy_startSceneryPlugins();

  if (include_aircraft) {
    xpy_startAircraftPlugins();
  }
}

void xpy_startAircraftPlugins()
{
  /* find, load, start and enable _only_ aircraft plugin for User Aircraft */
  char outFileName[512];
  char outPath[1024];
  XPLMGetNthAircraftModel(0, outFileName, outPath);
  if (!strlen(outPath)) {
    pythonLog("No user aircraft selected, skipping startAircraftPlugins()");
    return;
  }
  char *tmp = strrchr(outPath, '/');
  *tmp = '\0';
  // pythonDebug("Will look for Aircraft PI files in %s/plugins/PythonPlugins\n", outPath);
  char *plugins_path;
  if (-1 == asprintf(&plugins_path, "%s/plugins/PythonPlugins", outPath)) {
    pythonLog("Failed to allocate memory with asprintf. Unable to start.");
  }

  /* 'Aircraft' has to be in syspath, and get aircraft_path_rel*/
  pythonDebug("STARTING Aircraft plugins.");
  PyObject *localsDict = PyDict_New();
  PyDict_SetItemString(localsDict, "__builtins__", PyEval_GetBuiltins());
  PyDict_SetItemString(localsDict, "aircraft_path", PyUnicode_FromString(outPath));
  MyPyRun_String("import os\n"
                 "import sys\n"
                 "import xp\n"
                 "aircraft_root = os.path.join(xp.getSystemPath(), 'Aircraft')\n"
                 "if aircraft_root not in sys.path:\n"
                 "    sys.path.insert(0, aircraft_root)\n"
                 "aircraft_path_rel = os.path.relpath(aircraft_path, start=aircraft_root)\n"
                 "if aircraft_path not in sys.path:\n"
                 "    sys.path.insert(0, aircraft_path)\n"
                 "package = (aircraft_path_rel + '/plugins/PythonPlugins').replace('/', '.').replace('\\\\', '.')\n",
                 Py_file_input, localsDict, localsDict);
  
  PyObject *package = PyDict_GetItemString(localsDict, "package");
  if (package == nullptr) {
    pythonLog("[XPPython3] Failed to load aircraft package. Likely missing or bad xp.py, %p", package);
    pythonLogFlush();
  }
  if(PyErr_Occurred()) {
    pythonLogException();
    pythonDebug("FAILED to start aircraft plugin.");
    return;
  }

  char *package_str = objToStr(package);
  Py_DECREF(localsDict);
  /* ... need to know these are 'aircraft' plugins, so I can remove them later!!! */
  xpy_loadModules(plugins_path, package_str, "^PI_[^.]*\\.py$", XPY3aircraftPlugins, PLUGIN_AIRCRAFT);
  pythonDebug("STARTED Aircraft plugins.");
  free(package_str);
  free(plugins_path);
}

void xpy_startSceneryPlugins()
{
  pythonDebug("STARTING Scenery plugins...");
  /* Loads and calls XPluginStart, does not call Enable */
  PyObject *localsDict = PyDict_New();
  PyDict_SetItemString(localsDict, "__builtins__", PyEval_GetBuiltins());
  MyPyRun_String("import glob\n"
                 "import os\n"
                 "import sys\n"
                 "import xp\n"
                 "import re\n"
                 "from pathlib import Path\n"
                 "packages = []\n"
                 "with open(os.path.join(xp.getSystemPath() + 'Custom Scenery', 'scenery_packs.ini'), 'r') as fp:\n"
                 "    for idx, line in enumerate(fp.readlines()):\n"
                 "        m = re.match(r'SCENERY_PACK[\\s]+(.+)/', line)\n"
                 "        if not m:\n"
                 "            continue\n"
                 "        scenery_package_directory = m.group(1)\n"
                 "        if scenery_package_directory.startswith('Custom Scenery'):\n"
                 "            scenery_package_directory = xp.getSystemPath() + scenery_package_directory\n"
                 "        x = os.path.join(scenery_package_directory, 'plugins/PythonPlugins')\n"
                 "        p = Path(os.path.normpath(x))\n"
                 "        path_component = str(Path().joinpath(*p.parts[:-3]))\n"
                 "        if path_component not in sys.path:\n"
                 "            sys.path.insert(0, path_component)\n"
                 "        package = '.'.join([os.path.split(scenery_package_directory)[-1], 'plugins', 'PythonPlugins'])\n"
                 "        if glob.glob(x + '/PI_*.py'):\n"
                 "            packages.append([x, package])\n",
                 Py_file_input, localsDict, localsDict);
  
  PyObject *packages = PyDict_GetItemString(localsDict, "packages");
  if (packages == nullptr) {
    pythonLog("[XPPython3] Failed to load scenery packages. Likely missing or bad xp.py, %p", packages);
    pythonLogFlush();
  }
  if(PyErr_Occurred()) {
    pythonLogException();
    pythonDebug("FAILED to start scenery plugins.");
    return;
  }

  PyObject *iterator = PyObject_GetIter(packages);
  PyObject *packageInfo;

  if(PyErr_Occurred()) {
    pythonLogException();
  }

  #define MODULE_PATH 0
  #define MODULE_PACKAGE 1
  if (iterator != nullptr) {
    while((packageInfo = PyIter_Next(iterator))) {
      const char *path = PyUnicode_AsUTF8(PyList_GetItem(packageInfo, MODULE_PATH));
      const char *package = PyUnicode_AsUTF8(PyList_GetItem(packageInfo, MODULE_PACKAGE));
      // pythonLog("path: %s, package: %s", path, package);
      xpy_loadModules(path, package, "^PI_[^.]*\\.py$", XPY3sceneryPlugins, PLUGIN_SCENERY);
      Py_DECREF(packageInfo);
    }
    Py_DECREF(iterator);
  }
  Py_DECREF(localsDict);
  if(PyErr_Occurred()) {
    pythonLogException();
  }
  pythonDebug("STARTED Scenery plugins.");
}

/****************************************************************
 * "ENABLE"
 * Send XPlaneEnable
 ****************************************************************/

void xpy_enableInstances() {
  /* Enables all instances (everything in XPY3pluginInfoDict).
     Normally, aircraft plugins are initialized when aircraft is loaded, not
     at startup, so here, IF we see an existing aircraft instance we'll disable and stop _it_
     and attempt to start and enable for current aircraft
     NOTE: THIS SEEMS CONVOLUTED.
   */
  xpy_enableInternalPlugins();
  xpy_enableGlobalPlugins();
  xpy_enableSceneryPlugins();

  /* IF EXISTING aircraft, enable it them also */
  char outFileName[512];
  char outPath[1024];
  XPLMGetNthAircraftModel(0, outFileName, outPath);
  if (strlen(outFileName)) {
    // pythonDebug("(Loading already existing user aircraft.)");
    if (PyList_Size(XPY3aircraftPlugins) > 0) {
      pythonDebug("DISABLING Aircraft plugins");
      disablePluginList(XPY3aircraftPlugins);
      pythonDebug("DISABLED AircraftPlugins");
      pythonDebug("STOPPING Aircraft plugins");
      stopPluginList(XPY3aircraftPlugins);
      pythonDebug("STOPPED AircraftPlugins");
    }
    xpy_startAircraftPlugins();  /* start and enable will be called */
    xpy_enableAircraftPlugins();
  } else {
    // pythonDebug("(No existing user aircraft on startup.)");
  }
  pythonLogFlush();
}

static void enablePluginList(PyObject *pluginList)
{
  /* will enable _all_ plugins in provided list */
  PyObject *iterator = PyObject_GetIter(pluginList);
  PyObject *pluginInstance;
  if (iterator != nullptr) {
    while((pluginInstance = PyIter_Next(iterator))) {
      auto pluginIt = XPY3pluginInfoDict.find(pluginInstance);
      if (pluginIt != XPY3pluginInfoDict.end()) {
        const PluginInfo& info = pluginIt->second;
        set_moduleName(info.module_name);
        xpy_enableInstance(pluginInstance);
      }
      Py_DECREF(pluginInstance); /* PyInter_Next() returns new */
    }
    Py_DECREF(iterator);
  }
}

void xpy_enableGlobalPlugins()
{
  pythonDebug("ENABLING Global Plugins");
  for (auto& [pluginInstance, info] : XPY3pluginInfoDict) {
    if (info.plugin_type != PLUGIN_GLOBAL) {
      continue;
    }
    
    set_moduleName(info.module_name);
    if (0 == xpy_enableInstance(pluginInstance)) {
      /* returns 1 on successful enable, 0 otherwise. */
      info.disabled = true;
    };
  }
  pythonLogFlush();
  pythonDebug("ENABLED Global Plugins");
}

void xpy_enableSceneryPlugins()
{
  pythonDebug("ENABLING Scenery Plugins");
  enablePluginList(XPY3sceneryPlugins);
  pythonDebug("ENABLED Scenery Plugins");
}

void xpy_enableAircraftPlugins()
{
  pythonDebug("ENABLING Aircraft plugins: %s", objDebug(XPY3aircraftPlugins));
  enablePluginList(XPY3aircraftPlugins);
  pythonDebug("ENABLED Aircraft plugins");
}

/****************************************************************
 * "DISABLE"
 * Send XPlaneDisable to all plugins (everything in XPmoduleDict)
 ****************************************************************/

void xpy_disableGlobalPlugins() {
  pythonDebug("DISABLING  Global plugins...");
  for (const auto& [pluginInstance, info] : XPY3pluginInfoDict) {
    
    if (info.plugin_type != PLUGIN_GLOBAL) {
      continue;
    }
    
    if (!info.disabled) {
      set_moduleName(info.module_name);
      xpy_disableInstance(pluginInstance);
    }
  }
  pythonDebug("DISABLED Global plugins.");
}

void xpy_disableInstances()
{
  xpy_disableAircraftPlugins();
  xpy_disableGlobalPlugins();
  xpy_disableSceneryPlugins();
  xpy_disableInternalPlugins();
}

static void disablePluginList(PyObject *pluginList) {
  /* XPluginDisable() for plugins in list, does not remove items from list */
  PyObject *iterator = PyObject_GetIter(pluginList);
  PyObject *pluginInstance;
  if (iterator != nullptr) {
    while((pluginInstance = PyIter_Next(iterator))) {
      auto pluginIt = XPY3pluginInfoDict.find(pluginInstance);
      if (pluginIt != XPY3pluginInfoDict.end()) {
        const PluginInfo& info = pluginIt->second;
        if (!info.disabled) {
          set_moduleName(info.module_name);
          xpy_disableInstance(pluginInstance);
        }
      }
      Py_DECREF(pluginInstance);
    }
    Py_DECREF(iterator);
  }
}

void xpy_disableSceneryPlugins()
{
  pythonDebug("DISABLING Scenery plugins.");
  disablePluginList(XPY3sceneryPlugins);
  pythonDebug("DISABLED Scenery plugins.");
}

void xpy_disableAircraftPlugins()
{
  pythonDebug("DISABLING Aircraft plugins: %s", objDebug(XPY3aircraftPlugins));
  disablePluginList(XPY3aircraftPlugins);
  pythonDebug("DISABLED Aircraft plugins");
}

/****************************************************************
 * "STOP"
 * Send XPlaneStop to all plugins (everything in XPmoduleDict)
 *  cleanup
 ****************************************************************/

void xpy_stopInstances()
{
  xpy_stopGlobalPlugins();
  xpy_stopInternalPlugins();
  
  xpy_stopSceneryPlugins();
  xpy_stopAircraftPlugins();
}

static void xpy_stopGlobalPlugins() {
  pythonDebug("STOPPING Global plugins...");
  
  for (auto it = XPY3pluginInfoDict.begin(); it != XPY3pluginInfoDict.end(); ) {
    if (it->second.plugin_type != PLUGIN_GLOBAL) {
      ++it;
      continue;
    }
    set_moduleName(it->second.module_name);
    PyObject *pluginInstance = it->first;
    ++it; // advance iterator BEFORE calling xpy_stopInstance, since it modifies the Dict
    xpy_stopInstance(pluginInstance);
  }
  pythonDebug("STOPPED Global plugins.");
}

static void stopPluginList(PyObject *pluginList) {
  /* XPluginStop() for plugins in list,
     clears menu for each plugin,
     removes plugin from dictionary,
     empties the list
  */
  PyObject *pluginInstance;
  PyObject *iterator = PyObject_GetIter(pluginList);
  if (iterator != nullptr) {
    while((pluginInstance = PyIter_Next(iterator))) {
      auto pluginIt = XPY3pluginInfoDict.find(pluginInstance);
      if (pluginIt != XPY3pluginInfoDict.end()) {
        const PluginInfo& info = pluginIt->second;
        set_moduleName(info.module_name);
        xpy_stopInstance(pluginInstance);
      }
      /* xpy_cleanUpInstance(moduleName, pluginInstance); STOP also calls cleanup*/
      Py_DECREF(pluginInstance);
    }
    Py_DECREF(iterator);
  }
  PyList_SetSlice(pluginList, 0, PyList_Size(pluginList), nullptr);
}

void xpy_stopAircraftPlugins()
{
  pythonDebug("STOPPING Aircraft plugins: %s", objDebug(XPY3aircraftPlugins));
  stopPluginList(XPY3aircraftPlugins);
  pythonDebug("STOPPED Aircraft plugins");
}

void xpy_stopSceneryPlugins()
{
  pythonDebug("STOPPING Scenery plugins.");
  stopPluginList(XPY3sceneryPlugins);
  pythonDebug("STOPPED Scenery plugins.");
}
 
