//Python comes first!
#include <Python.h>
#include <XPLM/XPLMPlanes.h>
#include "manage_instances.h"
#include "load_modules.h"
#include "manage_instance.h"
#include "utils.h"

PyObject *moduleDict;
PyObject *pluginDict;
PyObject *aircraftPlugins; /* [instance, instance, ] */
PyObject *sceneryPlugins; /* [instance, instance, ] */

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
  xpy_loadModules(pythonInternalPluginsPath, "XPPython3", "^I_PI_.*\\.py$", NULL);
  pythonDebug("  STARTED INTERNAL plugins.");
}

void xpy_startInstances(int include_aircraft)
/* find, load, start global, scenery and (optionally) aircraft plugins */
{
  pythonDebug("STARTING Global plugins...");
  xpy_loadModules(pythonPluginsPath, "PythonPlugins", "^PI_.*\\.py$", NULL);
  pythonDebug("  STARTED Global plugins.");

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
    fprintf(pythonLogFile, "No user aircraft selected, skipping startAircraftPlugins()\n");
    return;
  }
  char *tmp = strrchr(outPath, '/');
  *tmp = '\0';
  // pythonDebug("Will look for Aircraft PI files in %s/plugins/PythonPlugins\n", outPath);
  char *plugins_path;
  asprintf(&plugins_path, "%s/plugins/PythonPlugins", outPath);

  /* 'Aircraft' has to be in syspath, and get aircraft_path_rel*/
  PyObject *localsDict = PyDict_New();
  PyDict_SetItemString(localsDict, "__builtins__", PyEval_GetBuiltins());
  PyDict_SetItemString(localsDict, "aircraft_path", PyUnicode_FromString(outPath));
  PyRun_String("import os\n"
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
  
  char *package = objToStr(PyDict_GetItemString(localsDict, "package"));
  Py_DECREF(localsDict);
  /* ... need to know these are 'aircraft' plugins, so I can remove them later!!! */
  pythonDebug("STARTING Aircraft plugins.");
  xpy_loadModules(plugins_path, package, "^PI_.*\\.py$", aircraftPlugins);
  pythonDebug("  STARTED Aircraft plugins.");
  free(package);
  free(plugins_path);
}

void xpy_startSceneryPlugins()
{
  pythonDebug("STARTING Scenery plugins...");
  /* Loads and calls XPluginStart, does not call Enable */
  PyObject *localsDict = PyDict_New();
  PyDict_SetItemString(localsDict, "__builtins__", PyEval_GetBuiltins());
  PyRun_String("import glob\n"
               "import os\n"
               "import sys\n"
               "import xp\n"
               "import re\n"
               "from pathlib import Path\n"
               "packages = []\n"
               "with open(os.path.join(xp.getSystemPath() + 'Custom Scenery', 'scenery_packs.ini'), 'r') as fp:\n"
               "    for idx, line in enumerate(fp.readlines()):\n"
               "        m = re.match('SCENERY_PACK[\\s]+(.+)/', line)\n"
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
      // fprintf(pythonLogFile, "path: %s, package: %s\n", path, package);
      xpy_loadModules(path, package, "^PI_.*\\.py$", sceneryPlugins);
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
  pythonDebug("  STARTED Scenery plugins.");
}

/****************************************************************
 * "ENABLE"
 * Send XPlaneEnable
 ****************************************************************/

void xpy_enableInstances() {
  /* Enables all instances (everything in moduleDict).
     Normally, aircraft plugins are intialized when aircraft is loaded, not
     at startup, so here, IF we see an existing aircraft instance we'll disable and top _it_
     and attempt to start and enable for current aircraft
     NOTE: THIS SEEMS CONVOLUTED.
   */
  PyObject *pluginInfo, *pluginInstance;
  Py_ssize_t pos = 0;
  pythonDebug("ENABLING Global Plugins");
  while(PyDict_Next(moduleDict, &pos, &pluginInfo, &pluginInstance)){
    if (PySequence_Contains(aircraftPlugins, pluginInstance) || PySequence_Contains(sceneryPlugins, pluginInstance)) {
      continue;
    }
    PyObject *pModuleName = PyTuple_GetItem(pluginInfo, PLUGIN_MODULE_NAME);
    xpy_enableInstance(pModuleName, pluginInstance); /* returns 1 on successful enable, 0 otherwise. */
  }
  fflush(pythonLogFile);
  pythonDebug("  ENABLED Global Plugins");

  xpy_enableSceneryPlugins();

  /* IF EXISTING aircraft, enable it them also */
  char outFileName[512];
  char outPath[1024];
  XPLMGetNthAircraftModel(0, outFileName, outPath);
  if (strlen(outFileName)) {
    // pythonDebug("(Loading already existing user aircraft.)");
    if (PyList_Size(aircraftPlugins) > 0) {
      pythonDebug("DISABLING Aircraft plugins");
      disablePluginList(aircraftPlugins);
      pythonDebug("  DISABLE AircraftPlugins");
      pythonDebug("STOPPING Aircraft plugins");
      stopPluginList(aircraftPlugins);
      pythonDebug("  STOPPED AircraftPlugins");
    }
    xpy_startAircraftPlugins();  /* start and enable will be called */
    xpy_enableAircraftPlugins();
  } else {
    // pythonDebug("(No existing user aircraft on startup.)");
  }
  fflush(pythonLogFile);
}

static void enablePluginList(PyObject *pluginList)
{
  /* will enable _all_ plugins in provided list */
  PyObject *iterator = PyObject_GetIter(pluginList);
  PyObject *pluginInstance;
  if (iterator != NULL) {
    while((pluginInstance = PyIter_Next(iterator))) {
      PyObject *moduleName = PyTuple_GetItem(PyDict_GetItem(pluginDict, pluginInstance), PLUGIN_MODULE_NAME);
      xpy_enableInstance(moduleName, pluginInstance);
      Py_DECREF(pluginInstance); /* PyInter_Next() returns new */
    }
    Py_DECREF(iterator);
  }
}

void xpy_enableSceneryPlugins()
{
  pythonDebug("ENABLING Scenery Plugins");
  enablePluginList(sceneryPlugins);
  pythonDebug("  ENABLED Scenery Plugins");
}

void xpy_enableAircraftPlugins()
{
  pythonDebug("ENABLING Aircraft plugins: %s", objDebug(aircraftPlugins));
  enablePluginList(aircraftPlugins);
  pythonDebug("  ENABLED Aircraft plugins");
}

/****************************************************************
 * "DISABLE"
 * Send XPlaneDisable to all plugins (everything in moduleDict)
 ****************************************************************/

void xpy_disableInstances()
{
  PyObject *pluginInfo, *pluginInstance;
  Py_ssize_t pos = 0;
  pythonDebug("DISABLING  Global plugins...");
  while(PyDict_Next(moduleDict, &pos, &pluginInfo, &pluginInstance)){
    PyObject *moduleName = PyTuple_GetItem(pluginInfo, PLUGIN_MODULE_NAME);
    if (PySequence_Contains(aircraftPlugins, pluginInstance) || PySequence_Contains(sceneryPlugins, pluginInstance)) {
      continue;
    }
    xpy_disableInstance(moduleName, pluginInstance);
  }
  pythonDebug("  DISABLED Global plugins.");

  xpy_disableSceneryPlugins();
  xpy_disableAircraftPlugins();
}

static void disablePluginList(PyObject *pluginList) {
  /* XPluginDisable() for plugins in list, does not remove items from list */
  PyObject *iterator = PyObject_GetIter(pluginList);
  PyObject *pluginInstance;
  if (iterator != NULL) {
    while((pluginInstance = PyIter_Next(iterator))) {
      PyObject *moduleName = PyTuple_GetItem(PyDict_GetItem(pluginDict, pluginInstance), PLUGIN_MODULE_NAME);
      xpy_disableInstance(moduleName, pluginInstance);
      Py_DECREF(pluginInstance);
    }
    Py_DECREF(iterator);
  }
}

void xpy_disableSceneryPlugins()
{
  pythonDebug("DISABLING Scenery plugins.");
  disablePluginList(sceneryPlugins);
  pythonDebug("  DISABLED Scenery plugins.");
}

void xpy_disableAircraftPlugins()
{
  pythonDebug("DISABLING Aircraft plugins: %s", objDebug(aircraftPlugins));
  disablePluginList(aircraftPlugins);
  pythonDebug("  DISABLED Aircraft plugins");
}

/****************************************************************
 * "STOP"
 * Send XPlaneStop to all plugins (everything in moduleDict)
 *  cleanup
 ****************************************************************/

void xpy_stopInstances()
{
  pythonDebug("STOPPING Global plugins...");

  PyObject *md_keys = PyDict_Keys(moduleDict); /* new */
  PyObject *md_key_iterator = PyObject_GetIter(md_keys); /*new*/
  PyObject *pluginInfo; /* key is pluginInfo */
  while ((pluginInfo = PyIter_Next(md_key_iterator))) {/*new*/
    PyObject *pluginInstance = PyDict_GetItem(moduleDict, pluginInfo); /*borrowed*/
    PyObject *pModuleName = PyTuple_GetItem(pluginInfo, PLUGIN_MODULE_NAME);
    if (PySequence_Contains(aircraftPlugins, pluginInstance) || PySequence_Contains(sceneryPlugins, pluginInstance)) {
      continue;
    }
    xpy_stopInstance(pModuleName, pluginInstance);
    Py_DECREF(pluginInfo);
  }
  Py_DECREF(md_key_iterator);
  Py_DECREF(md_keys);

  /* Py_ssize_t pos = 0; */
  /* while(PyDict_Next(moduleDict, &pos, &pluginInfo, &pluginInstance)){ */
  /*   PyObject *pModuleName = PyTuple_GetItem(pluginInfo, PLUGIN_MODULE_NAME); */
  /*   if (PySequence_Contains(aircraftPlugins, pluginInstance) || PySequence_Contains(sceneryPlugins, pluginInstance)) { */
  /*     continue; */
  /*   } */
  /*   xpy_stopInstance(pModuleName, pluginInstance); */
  /* } */

  pythonDebug("  STOPPED Global plugins.");
  
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
      PyObject *moduleName = PyTuple_GetItem(PyDict_GetItem(pluginDict, pluginInstance), PLUGIN_MODULE_NAME);
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
  pythonDebug("STOPPING Aircraft plugins: %s", objDebug(aircraftPlugins));
  stopPluginList(aircraftPlugins);
  pythonDebug("  STOPPED Aircraft plugins");
}

void xpy_stopSceneryPlugins()
{
  pythonDebug("STOPPING Scenery plugins.");
  stopPluginList(sceneryPlugins);
  pythonDebug("  STOPPED Scenery plugins.");
}
