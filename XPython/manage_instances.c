//Python comes first!
#include <Python.h>
#include <XPLM/XPLMPlanes.h>
#include "manage_instances.h"
#include "load_modules.h"
#include "manage_instance.h"
#include "utils.h"

PyObject *XPY3moduleDict;
PyObject *XPY3pluginDict;
PyObject *XPY3aircraftPlugins; /* [instance, instance, ] */
PyObject *XPY3sceneryPlugins; /* [instance, instance, ] */

static void stopPluginList(PyObject *);
static void disablePluginList(PyObject *);
static void enablePluginList(PyObject *);

/****************************************************************
 * "START"
 * Find, load and send "XPluginStart"
 *   path is set, internal dictionaries are updated
 ****************************************************************/

void xpy_startInternalInstances()
/* find, load, start INTERNAL plugins */
{
  pythonDebug("STARTING Internal plugins...");
  xpy_loadModules(pythonInternalPluginsPath, "XPPython3", "^I_PI_[^.]*\\.py$", NULL);
  pythonDebug("STARTED INTERNAL plugins.");
}

void xpy_startInstances(int include_aircraft)
/* find, load, start global, scenery and (optionally) aircraft plugins */
{
  pythonDebug("STARTING Global plugins...");
  xpy_loadModules(pythonPluginsPath, "PythonPlugins", "^PI_[^.]*\\.py$", NULL);
  pythonDebug("STARTED Global plugins.");

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
  if (package == NULL) {
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
  xpy_loadModules(plugins_path, package_str, "^PI_[^.]*\\.py$", XPY3aircraftPlugins);
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
  if (packages == NULL) {
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
  if (iterator != NULL) {
    while((packageInfo = PyIter_Next(iterator))) {
      char *path = objToStr(PyList_GetItem(packageInfo, MODULE_PATH));
      char *package = objToStr(PyList_GetItem(packageInfo, MODULE_PACKAGE));
      // pythonLog("path: %s, package: %s", path, package);
      xpy_loadModules(path, package, "^PI_[^.]*\\.py$", XPY3sceneryPlugins);
      free(path);
      free(package);
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
  /* Enables all instances (everything in XPY3moduleDict).
     Normally, aircraft plugins are intialized when aircraft is loaded, not
     at startup, so here, IF we see an existing aircraft instance we'll disable and stop _it_
     and attempt to start and enable for current aircraft
     NOTE: THIS SEEMS CONVOLUTED.
   */
  PyObject *pluginInfo, *pModuleName, *pluginInstance;
  Py_ssize_t pos = 0;
  pythonDebug("ENABLING Global Plugins");
  while(PyDict_Next(XPY3moduleDict, &pos, &pModuleName, &pluginInstance)){
    if (PySequence_Contains(XPY3aircraftPlugins, pluginInstance) || PySequence_Contains(XPY3sceneryPlugins, pluginInstance)) {
      continue;
    }
    
    pluginInfo = PyDict_GetItem(XPY3pluginDict, pluginInstance);
    if (0 == xpy_enableInstance(pModuleName, pluginInstance)) {
      /* returns 1 on successful enable, 0 otherwise. */
      PyList_SetItem(pluginInfo, PLUGIN_DISABLED, Py_True);
    }; 
  }
  pythonLogFlush();
  pythonDebug("ENABLED Global Plugins");

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
  if (iterator != NULL) {
    while((pluginInstance = PyIter_Next(iterator))) {
      PyObject *moduleName = PyList_GetItem(PyDict_GetItem(XPY3pluginDict, pluginInstance), PLUGIN_MODULE_NAME);
      xpy_enableInstance(moduleName, pluginInstance);
      Py_DECREF(pluginInstance); /* PyInter_Next() returns new */
    }
    Py_DECREF(iterator);
  }
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

void xpy_disableInstances()
{
  PyObject *pluginInfo, *pluginInstance, *pModuleName;
  Py_ssize_t pos = 0;
  pythonDebug("DISABLING  Global plugins...");
  while(PyDict_Next(XPY3moduleDict, &pos, &pModuleName, &pluginInstance)){
    if (PySequence_Contains(XPY3aircraftPlugins, pluginInstance) || PySequence_Contains(XPY3sceneryPlugins, pluginInstance)) {
      continue;
    }
    pluginInfo = PyDict_GetItem(XPY3pluginDict, pluginInstance); /* borrowed */
    if (PyList_GetItem(pluginInfo, PLUGIN_DISABLED) == Py_False) {
      xpy_disableInstance(pModuleName, pluginInstance);
    }
  }
  pythonDebug("DISABLED Global plugins.");
  xpy_disableSceneryPlugins();
  xpy_disableAircraftPlugins();
}

static void disablePluginList(PyObject *pluginList) {
  /* XPluginDisable() for plugins in list, does not remove items from list */
  PyObject *iterator = PyObject_GetIter(pluginList);
  PyObject *pluginInstance;
  if (iterator != NULL) {
    while((pluginInstance = PyIter_Next(iterator))) {
      PyObject *pluginInfo = PyDict_GetItem(XPY3pluginDict, pluginInstance);
      PyObject *moduleName = PyList_GetItem(pluginInfo, PLUGIN_MODULE_NAME);
      if (PyList_GetItem(pluginInfo, PLUGIN_DISABLED) == Py_False) {
        xpy_disableInstance(moduleName, pluginInstance);
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
  pythonDebug("STOPPING Global plugins...");

  PyObject *md_keys = PyDict_Keys(XPY3moduleDict); /* new */
  PyObject *md_key_iterator = PyObject_GetIter(md_keys); /*new*/
  PyObject *pModuleName; /* key is moduleName */
  while ((pModuleName = PyIter_Next(md_key_iterator))) {/*new*/
    PyObject *pluginInstance = PyDict_GetItem(XPY3moduleDict, pModuleName); /* borrowed */
    if (PySequence_Contains(XPY3aircraftPlugins, pluginInstance) || PySequence_Contains(XPY3sceneryPlugins, pluginInstance)) {
      continue;
    }
    xpy_stopInstance(pModuleName, pluginInstance);
    Py_DECREF(pModuleName);
  }
  Py_DECREF(md_key_iterator);
  Py_DECREF(md_keys);

  pythonDebug("STOPPED Global plugins.");
  
  xpy_stopSceneryPlugins();
  xpy_stopAircraftPlugins();
}

static void stopPluginList(PyObject *pluginList) {
  /* XPluginStop() for plugins in list, 
     clears menu for each plugin,
     removes plugin from dictionary,
     empties the list
  */
  PyObject *pluginInstance;
  PyObject *iterator = PyObject_GetIter(pluginList);
  if (iterator != NULL) {
    while((pluginInstance = PyIter_Next(iterator))) {
      PyObject *moduleName = PyList_GetItem(PyDict_GetItem(XPY3pluginDict, pluginInstance), PLUGIN_MODULE_NAME);
      xpy_stopInstance(moduleName, pluginInstance);
      /* xpy_cleanUpInstance(moduleName, pluginInstance); STOP also calls cleanup*/
      Py_DECREF(pluginInstance);
    }
    Py_DECREF(iterator);
  }
  PyList_SetSlice(pluginList, 0, PyList_Size(pluginList), NULL);
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
