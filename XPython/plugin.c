//Python comes first!
#include <Python.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdbool.h>
#include <dirent.h>
#include <dlfcn.h>
#include <XPLM/XPLMDefs.h>

#include <XPLM/XPLMPlugin.h>
#include <sys/types.h>
#include <regex.h>

#include "menus.h"
#include "utils.h"
#include "plugin_dl.h"

/*************************************
 * Python plugin upgrade for Python 3
 *   Michal        f.josef@email.cz (uglyDwarf on x-plane.org)
 *   Peter Buckner pbuck@avnwx.com (pbuckner on x-plane.org) 
 *
 * Upgraded from original Python2 version by
 *   Sandy Barbour (on x-plane.org)
 */

extern void clearAllMenuItems();
/**********************
 * Plugin configuration
 */
static char *logFileName = "XPPython3Log.txt";
static char *ENV_logFileVar = "XPPYTHON3_LOG";  // set this environment to override logFileName
static char *ENV_logPreserve = "XPPYTHON3_PRESERVE";  // DO NOT truncate XPPython log on startup. If set, we preserve, if unset, we truncate

const char *pythonPluginsPath = "Resources/plugins/PythonPlugins";
const char *pythonInternalPluginsPath = "Resources/plugins/XPPython3";

static const char *pythonPluginName = "XPPython3";
const char *pythonPluginVersion = XPPYTHON3VERSION " - for Python " PYTHONVERSION;
/* 10a: update sys.path to INSERT rather than APPEND plugin paths */
const char *pythonPluginSig  = "xppython3.main";
static const char *pythonPluginDesc = "X-Plane interface for Python 3";
static const char *pythonDisableCommand = "XPPython3/disableScripts";
static const char *pythonEnableCommand = "XPPython3/enableScripts";
static const char *pythonReloadCommand = "XPPython3/reloadScripts";
/**********************/
static XPLMCommandRef disableScripts;
static XPLMCommandRef enableScripts;
static XPLMCommandRef reloadScripts;

static int commandHandler(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void *inRefcon);

static int loadPythonLibrary();
static void loadAircraftPlugins();
static void stopPluginList(PyObject *);
static void disablePluginList(PyObject *);
static void enablePluginList(PyObject *);
static void loadSceneryPlugins();
static PyObject *loadPIClass();

PLUGIN_API int XPluginEnable(void);
PLUGIN_API void XPluginDisable(void);

PyMODINIT_FUNC PyInit_XPLMDefs(void);
PyMODINIT_FUNC PyInit_XPLMDisplay(void);
PyMODINIT_FUNC PyInit_XPLMGraphics(void);
PyMODINIT_FUNC PyInit_XPLMDataAccess(void);
PyMODINIT_FUNC PyInit_XPLMUtilities(void);
PyMODINIT_FUNC PyInit_XPLMScenery(void);
PyMODINIT_FUNC PyInit_XPLMMenus(void);
PyMODINIT_FUNC PyInit_XPLMNavigation(void);
PyMODINIT_FUNC PyInit_XPLMPlugin(void);
PyMODINIT_FUNC PyInit_XPLMPlanes(void);
PyMODINIT_FUNC PyInit_XPLMProcessing(void);
PyMODINIT_FUNC PyInit_XPLMCamera(void);
PyMODINIT_FUNC PyInit_XPWidgetDefs(void);
PyMODINIT_FUNC PyInit_XPWidgets(void);
PyMODINIT_FUNC PyInit_XPStandardWidgets(void);
PyMODINIT_FUNC PyInit_XPUIGraphics(void);
PyMODINIT_FUNC PyInit_XPWidgetUtils(void);
PyMODINIT_FUNC PyInit_XPLMInstance(void);
PyMODINIT_FUNC PyInit_XPLMMap(void);
PyMODINIT_FUNC PyInit_SBU(void);
PyMODINIT_FUNC PyInit_XPPython(void);

FILE *pythonLogFile;
static bool disabled;
static int allErrorsEncountered;

static PyObject *logWriterWrite(PyObject *self, PyObject *args)
{
  (void) self;
  char *msg;
  if(!PyArg_ParseTuple(args, "s", &msg)){
    return NULL;
  }
  //printf("%s", msg);
  fprintf(pythonLogFile, "%s", msg);
  fflush(pythonLogFile);
  Py_RETURN_NONE;
}

static PyObject *logWriterFlush(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  fflush(pythonLogFile);
  Py_RETURN_NONE;
}

static PyObject *logWriterAddAllErrors(PyObject *self, PyObject *args)
{
  (void) self;
  int errs;
  if(!PyArg_ParseTuple(args, "i", &errs)){
    return NULL;
  }
  printf("Adding %d errors...\n", errs);
  allErrorsEncountered += errs;
  Py_RETURN_NONE;
}


static PyMethodDef logWriterMethods[] = {
  {"write", logWriterWrite, METH_VARARGS, ""},
  {"flush", logWriterFlush, METH_VARARGS, ""},
  {"addAllErrors", logWriterAddAllErrors, METH_VARARGS, ""},
  {NULL, NULL, 0, NULL}
};

static struct PyModuleDef XPythonLogWriterModule = {
  PyModuleDef_HEAD_INIT,
  "XPythonLogWriter",
  NULL,
  -1,
  logWriterMethods,
  NULL,
  NULL,
  NULL,
  NULL
};

PyMODINIT_FUNC
PyInit_XPythonLogWriter(void)
{
  PyObject *mod = PyModule_Create(&XPythonLogWriterModule);
  if(mod){
    PySys_SetObject("stdout", mod);
    PySys_SetObject("stderr", mod);
  }

  return mod;
};

static PyObject *moduleDict;  /* info -> instance */
static PyObject *pluginDict;  /* instance -> info */
#define PLUGIN_NAME 0
#define PLUGIN_SIGNATURE 1
#define PLUGIN_DESCRIPTION 2
#define PLUGIN_MODULE_NAME 3

static PyObject *loggerModuleObj;
static PyObject *aircraftPlugins; /* [instance, instance, ] */
static PyObject *sceneryPlugins; /* [instance, instance, ] */
static void *pythonHandle = NULL;

int initPython(void){
  // setbuf(stdout, NULL);  // for debugging, it removes stdout buffering

  PyImport_AppendInittab("XPPython", PyInit_XPPython);
  PyImport_AppendInittab("XPLMDefs", PyInit_XPLMDefs);
  PyImport_AppendInittab("XPLMDisplay", PyInit_XPLMDisplay);
  PyImport_AppendInittab("XPLMGraphics", PyInit_XPLMGraphics);
  PyImport_AppendInittab("XPLMDataAccess", PyInit_XPLMDataAccess);
  PyImport_AppendInittab("XPLMUtilities", PyInit_XPLMUtilities);
  PyImport_AppendInittab("XPLMScenery", PyInit_XPLMScenery);
  PyImport_AppendInittab("XPLMMenus", PyInit_XPLMMenus);
  PyImport_AppendInittab("XPLMNavigation", PyInit_XPLMNavigation);
  PyImport_AppendInittab("XPLMPlugin", PyInit_XPLMPlugin);
  PyImport_AppendInittab("XPLMPlanes", PyInit_XPLMPlanes);
  PyImport_AppendInittab("XPLMProcessing", PyInit_XPLMProcessing);
  PyImport_AppendInittab("XPLMCamera", PyInit_XPLMCamera);
  PyImport_AppendInittab("XPWidgetDefs", PyInit_XPWidgetDefs);
  PyImport_AppendInittab("XPWidgets", PyInit_XPWidgets);
  PyImport_AppendInittab("XPStandardWidgets", PyInit_XPStandardWidgets);
  PyImport_AppendInittab("XPUIGraphics", PyInit_XPUIGraphics);
  PyImport_AppendInittab("XPWidgetUtils", PyInit_XPWidgetUtils);
  PyImport_AppendInittab("XPLMInstance", PyInit_XPLMInstance);
  PyImport_AppendInittab("XPLMMap", PyInit_XPLMMap);
  PyImport_AppendInittab("XPythonLogger", PyInit_XPythonLogWriter);
  PyImport_AppendInittab("SandyBarbourUtilities", PyInit_SBU);
  
  Py_Initialize();
  if(!Py_IsInitialized()){
    fprintf(pythonLogFile, "Failed to initialize Python.\n");
    fflush(pythonLogFile);
    return -1;
  }

  //get the plugin directory into the python's path
  loggerModuleObj = PyImport_ImportModule("XPythonLogger");

  PyObject *path = PySys_GetObject("path"); //Borrowed!
  PyObject *pathStrObj = NULL, *absolutePathStrObj = NULL, *xPlaneDirObj = NULL;

  char x_plane_dir[512];
  XPLMGetSystemPath(x_plane_dir);
  xPlaneDirObj = PyUnicode_DecodeUTF8(x_plane_dir, strlen(x_plane_dir), NULL);

  /* Resources/plugins */
  pathStrObj = PyUnicode_DecodeUTF8(pythonPluginsPath, (strrchr(pythonPluginsPath, '/') - pythonPluginsPath), NULL);
  absolutePathStrObj = PyUnicode_Concat(xPlaneDirObj, pathStrObj);
  PyList_Insert(path, 0, absolutePathStrObj);
  Py_DECREF(pathStrObj);
  Py_DECREF(absolutePathStrObj);

  /* Resources/plugins/XPPython3 */
  pathStrObj = PyUnicode_DecodeUTF8(pythonInternalPluginsPath, strlen(pythonInternalPluginsPath), NULL);
  absolutePathStrObj = PyUnicode_Concat(xPlaneDirObj, pathStrObj);
  PyList_Insert(path, 0, absolutePathStrObj);
  Py_DECREF(pathStrObj);
  Py_DECREF(absolutePathStrObj);

  /* Resources/plugins/PythonPlugins */
  pathStrObj = PyUnicode_DecodeUTF8(pythonPluginsPath, strlen(pythonPluginsPath), NULL);
  absolutePathStrObj = PyUnicode_Concat(xPlaneDirObj, pathStrObj);
  PyList_Insert(path, 0, absolutePathStrObj);
  Py_DECREF(pathStrObj);
  Py_DECREF(absolutePathStrObj);

  Py_DECREF(xPlaneDirObj);

  xppythonDicts = PyDict_New();
  Py_INCREF(xppythonDicts);
  xppythonCapsules = PyDict_New();
  Py_INCREF(xppythonCapsules);

  PyObject *cryptographyModuleObj = PyImport_ImportModule("cryptography");
  if (!cryptographyModuleObj) {
    fprintf(pythonLogFile, "[XPPython3] Cryptography package not installed, XPPython3.xpyce will not be supported. See Documentation.\n");
  } else {
    Py_DECREF(cryptographyModuleObj);
    char *xpyceModule = "XPPython3.xpyce";
    PyObject *pModule = PyImport_ImportModule(xpyceModule);
    if (pModule) {
      PyObject *pFunction = PyObject_GetAttrString(pModule, "XPYCEPathFinder");
      if (pFunction) {
        PyObject *meta_path = PySys_GetObject("meta_path");
        PyList_Insert(meta_path, 0, pFunction);
        PySys_SetObject("meta_path", meta_path);
        fprintf(pythonLogFile, "%s loader initialized.\n", xpyceModule);
      } else {
        fprintf(pythonLogFile, "Failed to intialize %s PathFinder.\n", xpyceModule);
      }
    } else {
      fprintf(pythonLogFile, "Failed to load %s.\n", xpyceModule);
    }
  }

  moduleDict = PyDict_New();
  if (!moduleDict) {
    pythonDebug("Failed to allocation moduleDict");
  }
  pluginDict = PyDict_New();
  if (!pluginDict ) {
    pythonDebug("Failed to allocate pluginDict");
  }
  aircraftPlugins = PyList_New(0);
  if(!aircraftPlugins) {
    pythonDebug("Failed to allocate aircraftplugins");
  }
  sceneryPlugins = PyList_New(0);
  if(!sceneryPlugins) {
    pythonDebug("Failed to allocate sceneryplugins");
  }
  PyDict_SetItemString(xppythonDicts, "plugins", pluginDict);
  PyDict_SetItemString(xppythonDicts, "modules", moduleDict);
  return 0;
}


PyObject *loadPIClass(const char *fname)
{
  PyObject *pName = NULL, *pModule = NULL, *pClass = NULL,
    *pluginInstance = NULL, *pRes = NULL;

  pName = PyUnicode_DecodeFSDefault(fname);
  if(pName == NULL){
    fprintf(pythonLogFile, "Problem decoding the filename.\n");
    goto cleanup;
  }
  pModule = PyImport_Import(pName);
  
  Py_DECREF(pName);
  if(pModule == NULL){
    goto cleanup;
  }

  pClass = PyObject_GetAttrString(pModule, "PythonInterface");
  if(pClass == NULL){
    goto cleanup;
  }
  if(!PyCallable_Check(pClass)){
    goto cleanup;
  }
  //trying to get an object constructed
  pluginInstance = PyObject_CallObject(pClass, NULL);

  if(pluginInstance == NULL){
    goto cleanup;
  }
  pRes = PyObject_CallMethod(pluginInstance, "XPluginStart", NULL);
  if(pRes == NULL){
    fprintf(pythonLogFile, "XPluginStart returned NULL\n"); // NULL is error, Py_None is void, we're looking for a tuple[3]
    goto cleanup;
  }
  if(!(PyTuple_Check(pRes) && (PyTuple_Size(pRes) == 3) &&
      PyUnicode_Check(PyTuple_GetItem(pRes, PLUGIN_NAME)) &&
      PyUnicode_Check(PyTuple_GetItem(pRes, PLUGIN_SIGNATURE)) &&
      PyUnicode_Check(PyTuple_GetItem(pRes, PLUGIN_DESCRIPTION)))){
    fprintf(pythonLogFile, "Unable to start plugin in file %s: XPluginStart did not return Name, Sig, and Desc.", fname);
    goto cleanup;
  }
  
  PyObject *u1 = NULL, *u2 = NULL, *u3 = NULL;

  u1 = PyUnicode_AsUTF8String(PyTuple_GetItem(pRes, PLUGIN_NAME));
  u2 = PyUnicode_AsUTF8String(PyTuple_GetItem(pRes, PLUGIN_SIGNATURE));
  u3 = PyUnicode_AsUTF8String(PyTuple_GetItem(pRes, PLUGIN_DESCRIPTION));
  if(u1 && u2 && u3){
    fprintf(pythonLogFile, "%s initialized.\n", fname);
    fprintf(pythonLogFile, "  Name: %s\n", PyBytes_AsString(u1));
    fprintf(pythonLogFile, "  Sig:  %s\n", PyBytes_AsString(u2));
    fprintf(pythonLogFile, "  Desc: %s\n", PyBytes_AsString(u3));
    fflush(pythonLogFile);
  }
  Py_DECREF(u1);
  Py_DECREF(u2);
  Py_DECREF(u3);

  PyObject *pluginInfo = PyTuple_New(4);  /* pluginInfo is new reference */

  /* PyTuple_GetItem borrows reference, PyTuple_SetItem steals:pluginInfo now owns pRes[0] */
  PyObject *tmp = PyTuple_GetItem(pRes, PLUGIN_NAME);
  Py_INCREF(tmp);
  PyTuple_SetItem(pluginInfo, PLUGIN_NAME, tmp);

  tmp = PyTuple_GetItem(pRes, PLUGIN_SIGNATURE);
  Py_INCREF(tmp);
  PyTuple_SetItem(pluginInfo, PLUGIN_SIGNATURE, tmp);

  tmp = PyTuple_GetItem(pRes, PLUGIN_DESCRIPTION);
  Py_INCREF(tmp);
  PyTuple_SetItem(pluginInfo, PLUGIN_DESCRIPTION, tmp);

  tmp = PyUnicode_FromString(fname); /* Module.class */
  Py_INCREF(tmp);
  PyTuple_SetItem(pluginInfo, PLUGIN_MODULE_NAME, tmp);

  PyDict_SetItem(moduleDict, pluginInfo, pluginInstance); // does not steal reference. We don't need pluginInfo again, so decref
  PyDict_SetItem(pluginDict, pluginInstance, pluginInfo);
  /* Because we put pluginInfo into pluginDict, we need to NOT DECREF
     Py_DECREF(pluginInfo);
  */

 cleanup:
  if(PyErr_Occurred()) {
    PyErr_Print();
  }

  // use XDECREF rather than DECREF, because we may hit this section via goto cleanup error
  Py_XDECREF(pRes);
  Py_XDECREF(pModule);
  Py_XDECREF(pClass);
  return pluginInstance;
}

void loadModules(const char *path, const char *package, const char *pattern, PyObject* pluginList)
{
  //Scan current directory for the plugin modules, loads and calls XPluginStart()
  DIR *dir = opendir(path);
  PyObject *pluginInstance;
  if(dir == NULL){
    fprintf(pythonLogFile, "Looking in '%s' to scan for plugins: directory not found.\n", path);
    fflush(pythonLogFile);
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
          if (pluginInstance && pluginList){
            PyList_Append(pluginList, pluginInstance);
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

static bool pythonStarted;

static int startPython(void)
{
  pythonDebug("Calling startPython...");
  if(pythonStarted){
    return 0;
  }
  pythonDebug("loadAllFunctions startPython...");
  loadAllFunctions();
  pythonDebug("allFunctions Loaded...");
  if(initPython()) {
    fprintf(pythonLogFile, "Failed to start python\n");
    fflush(pythonLogFile);
    return -1;
  }

  // Load internal stuff
  // XPLMDebugString("STARTING Internal plugins...\n");
  pythonDebug("STARTING Internal plugins...");
  loadModules(pythonInternalPluginsPath, "XPPython3", "^I_PI_.*\\.py$", NULL);
  pythonDebug("Modules loaded");
  //XPLMDebugString("  STARTED Internal plugins Started.\n");
  pythonDebug("  STARTED INTERNAL plugins.");
  // Load modules
  //XPLMDebugString("STARTING Global plugins...\n");
  pythonDebug("STARTING Global plugins...");
  loadModules(pythonPluginsPath, "PythonPlugins", "^PI_.*\\.py$", NULL);
  //XPLMDebugString("  STARTED Global plugins Started.\n");
  pythonDebug("  STARTED Global plugins.");
  // Load Scenery
  //XPLMDebugString("STARTING Scenery plugins...\n");
  pythonDebug("STARTING Scenery plugins...");
  loadSceneryPlugins();
  //XPLMDebugString("  STARTED Scenery plugins Started.\n");
  pythonDebug("  STARTED Scenery plugins.");
  pythonStarted = true;
  return 1;
}

static void loadSceneryPlugins()
{
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
    PyErr_Print();
  }

  #define MODULE_PATH 0
  #define MODULE_PACKAGE 1
  if (iterator != NULL) {
    while((packageInfo = PyIter_Next(iterator))) {
      char *path = objToStr(PyList_GetItem(packageInfo, MODULE_PATH));
      char *package = objToStr(PyList_GetItem(packageInfo, MODULE_PACKAGE));
      // fprintf(pythonLogFile, "path: %s, package: %s\n", path, package);
      loadModules(path, package, "^PI_.*\\.py$", sceneryPlugins);
      free(path);
      free(package);
      Py_DECREF(packageInfo);
    }
    Py_DECREF(iterator);
  }
  Py_DECREF(localsDict);
  if(PyErr_Occurred()) {
    PyErr_Print();
  }
}

static int stopPython(void)
{
  if(!pythonStarted){
    return 0;
  }
  PyObject *pluginInfo, *pluginInstance;
  Py_ssize_t pos = 0;

  pythonDebug("STOPPING Global plugins...");
  while(PyDict_Next(moduleDict, &pos, &pluginInfo, &pluginInstance)){
    char *moduleName = objToStr(PyTuple_GetItem(pluginInfo, PLUGIN_MODULE_NAME));
    if (PySequence_Contains(aircraftPlugins, pluginInstance) || PySequence_Contains(sceneryPlugins, pluginInstance)) {
      continue;
    }
    PyObject *pRes = PyObject_CallMethod(pluginInstance, "XPluginStop", NULL); // should return void, so we should see Py_None
    if(pRes != Py_None) {
      fprintf(pythonLogFile, "%s XPluginStop returned '%s' rather than None.\n", moduleName, objToStr(pRes));
    }
    if(PyErr_Occurred()){
      fprintf(pythonLogFile, "Error occured during the %s XPluginStop call:\n", moduleName);
      PyErr_Print();
    }else{
      Py_DECREF(pRes);
    }
  }
  pythonDebug("  STOPPED Global plugins.");
  
  pythonDebug("STOPPING Scenery plugins");
  stopPluginList(sceneryPlugins);
  pythonDebug("  STOPPED Scenery plugins");

  pythonDebug("STOPPING Aircraft plugins");
  stopPluginList(aircraftPlugins);
  pythonDebug("  STOPPED Aircraft plugins");

  XPLMClearAllMenuItems(XPLMFindPluginsMenu());

  PyDict_Clear(moduleDict);
  PyDict_Clear(pluginDict);
  PyList_SetSlice(aircraftPlugins, 0, PyList_Size(aircraftPlugins), NULL);
  PyList_SetSlice(sceneryPlugins, 0, PyList_Size(sceneryPlugins), NULL);

  PyDict_DelItemString(xppythonDicts, "modules");
  PyDict_DelItemString(xppythonDicts, "plugins");

  Py_DECREF(moduleDict);
  Py_DECREF(pluginDict);
  
  // Invoke cleanup method of all built-in modules
  char *mods[] = {"XPLMDefs", "XPLMDisplay", "XPLMGraphics", "XPLMUtilities", "XPLMScenery", "XPLMMenus",
                  "XPLMNavigation", "XPLMPlugin", "XPLMPlanes", "XPLMProcessing", "XPLMCamera", "XPWidgetDefs",
                  "XPWidgets", "XPStandardWidgets", "XPUIGraphics", "XPWidgetUtils", "XPLMInstance",
                  "XPLMMap", "XPLMDataAccess", "SandyBarbourUtilities", "XPPython", NULL};
  char **mod_ptr = mods;

  while(*mod_ptr != NULL){
    PyObject *mod = PyImport_ImportModule(*mod_ptr);
    fflush(stdout);
    if (PyErr_Occurred()) {
      fprintf(pythonLogFile, "XPlugin Failed during stop of internal module %s\n", *mod_ptr);
      PyErr_Print();
      return 1;
    }
      
    if(mod){
      PyObject *pRes = PyObject_CallMethod(mod, "cleanup", NULL);
      if (PyErr_Occurred() ) {
        fprintf(pythonLogFile, "XPlugin Failed during cleanup of internal module %s\n", *mod_ptr);
        PyErr_Print();
        return 1;
      }
        
      Py_DECREF(pRes);
      Py_DECREF(mod);
    }
    ++mod_ptr;
  }
  Py_DECREF(loggerModuleObj);
  Py_Finalize();
  if (pythonHandle) {
    dlclose(pythonHandle);
  }
  pythonStarted = false;
  return 0;
}

PLUGIN_API int XPluginStart(char *outName, char *outSig, char *outDesc)
{
  if (XPLMFindPluginBySignature("sandybarbour.projects.pythoninterface") != XPLM_NO_PLUGIN_ID) {
    XPLMDebugString("FATAL ERROR: XPPython3 Detected python2 PythonInterface plugin. These plugins are incompatible\n");
    return 0;
  }
  char *log;
  log = getenv(ENV_logFileVar);
  if(log != NULL){
    logFileName = log;
  }
  if (getenv(ENV_logPreserve) != NULL) {
    printf("Preserving log file\n"); fflush(stdout);
    pythonLogFile = fopen(logFileName, "a");
  } else {
    pythonLogFile = fopen(logFileName, "w");
  }
  if(pythonLogFile == NULL){
    pythonLogFile = stdout;
  }
  if(loadPythonLibrary() == -1) {
    fprintf(pythonLogFile, "Failed to open python shared library.\n");
    fflush(pythonLogFile);
    return 0;
  }

  fprintf(pythonLogFile, "%s version %s Started.\n", pythonPluginName, pythonPluginVersion);
  fflush(pythonLogFile);
  strcpy(outName, pythonPluginName);
  strcpy(outSig, pythonPluginSig);
  strcpy(outDesc, pythonPluginDesc);

  if (XPLMHasFeature("XPLM_USE_NATIVE_PATHS")) {
    XPLMEnableFeature("XPLM_USE_NATIVE_PATHS", 1);
  } else {
    fprintf(pythonLogFile, "Warning: XPLM_USE_NATIVE_PATHS not enabled. Using Legacy paths.\n");
  }
  if (XPLMHasFeature("XPLM_USE_NATIVE_WIDGET_WINDOWS")) {
    XPLMEnableFeature("XPLM_USE_NATIVE_WIDGET_WINDOWS", 1);
  } else {
    fprintf(pythonLogFile, "Warning: XPLM_USE_NATIVE_WIDGET_WINDOWS not enabled. Using Legacy windows.\n");
  }
  disableScripts = XPLMCreateCommand(pythonDisableCommand, "Disable all running scripts");
  enableScripts = XPLMCreateCommand(pythonEnableCommand, "Enable all scripts");
  reloadScripts = XPLMCreateCommand(pythonReloadCommand, "Reload all scripts");

  XPLMRegisterCommandHandler(disableScripts, commandHandler, 1, (void *)0);
  XPLMRegisterCommandHandler(enableScripts, commandHandler, 1, (void *)1);
  XPLMRegisterCommandHandler(reloadScripts, commandHandler, 1, (void *)2);

  pythonDebug("commands created");
  if(startPython() == -1) {
    fprintf(pythonLogFile, "Failed to start python, exiting.\n");
    fflush(pythonLogFile);
    return 0;
  }
  return 1;
}


PLUGIN_API void XPluginStop(void)
{
  stopPython();
  XPLMUnregisterCommandHandler(disableScripts, commandHandler, 1, (void *)0);
  XPLMUnregisterCommandHandler(enableScripts, commandHandler, 1, (void *)1);
  XPLMUnregisterCommandHandler(reloadScripts, commandHandler, 1, (void *)2);
  if(allErrorsEncountered){
    fprintf(pythonLogFile, "Total errors encountered: %d\n", allErrorsEncountered);
  }
  fprintf(pythonLogFile, "%s Stopped.\n", pythonPluginName);
  fclose(pythonLogFile);
}

static int commandHandler(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void *inRefcon)
{
  (void) inRefcon;
  if(inPhase != xplm_CommandBegin){
    return 0;
  }
  if(inCommand == disableScripts){
    if (! disabled) {
      XPluginDisable();
      disabled = true;
      fprintf(pythonLogFile, "XPPython: Disabled scripts.\n");
    } else {
      fprintf(pythonLogFile, "XPPython already disabled.\n");
    }
  }else if(inCommand == enableScripts){
    if (disabled) {
      disabled = false;
      XPluginEnable();
      fprintf(pythonLogFile, "XPPython: Enabled scripts.\n");
    } else {
      fprintf(pythonLogFile, "XPPython already enabled.\n");
    }
  }else if(inCommand == reloadScripts){
    if (! disabled) {
      XPluginDisable();
    }
    stopPython();
    fprintf(pythonLogFile, "XPPython: Reloading scripts.\n");
    disabled = 0;
    resetMenus();
    startPython();
    XPluginEnable();
  }
  fflush(pythonLogFile);
  return 0;
}


PLUGIN_API int XPluginEnable(void)
{
  PyObject *pluginInfo, *pluginInstance, *pRes;
  Py_ssize_t pos = 0;
  if(disabled){
    return 1;
  }

  pythonDebug("ENABLING Global Plugins");
  while(PyDict_Next(moduleDict, &pos, &pluginInfo, &pluginInstance)){
    if (PySequence_Contains(aircraftPlugins, pluginInstance) || PySequence_Contains(sceneryPlugins, pluginInstance)) {
      continue;
    }
    char *moduleName = objToStr(PyTuple_GetItem(pluginInfo, PLUGIN_MODULE_NAME));
    pRes = PyObject_CallMethod(pluginInstance, "XPluginEnable", NULL);
    if(!(pRes && PyLong_Check(pRes))){
      fprintf(pythonLogFile, "%s XPluginEnable returned '%s' rather than an integer.\n", moduleName, objToStr(pRes));
    }else{
      //printf("XPluginEnable returned %ld\n", PyLong_AsLong(pRes));
    }
    if(PyErr_Occurred()) {
      fprintf(pythonLogFile, "Error occured during the %s XPluginEnable call:\n", moduleName);
      PyErr_Print();
    }else{
      Py_DECREF(pRes);
    }
  }
  fflush(pythonLogFile);
  pythonDebug("  ENABLED Global Plugins");
  pythonDebug("ENABLING Scenery Plugins");
  enablePluginList(sceneryPlugins);
  pythonDebug("  ENABLED Scenery Plugins");
  /* IF EXISTING aircraft, enable it them also */
  char outFileName[512];
  char outPath[1024];
  XPLMGetNthAircraftModel(0, outFileName, outPath);
  if (strlen(outFileName)) {
    // pythonDebug("(Loading already existing user aircraft.)");
    if (PyList_Size(aircraftPlugins) > 0) {
      pythonDebug("STOPPING Aircraft plugins");
      stopPluginList(aircraftPlugins);
      pythonDebug("  STOPPED AircraftPlugins");
    }
    loadAircraftPlugins();  /* start and enable will be called */
  } else {
    // pythonDebug("(No existing user aircraft on startup.)");
  }
  fflush(pythonLogFile);

  return 1;
}


static void disablePluginList(PyObject *pluginList) {
  /* XPluginDisable() for plugins in list, does not remove items from list */
  PyObject *iterator = PyObject_GetIter(pluginList);
  PyObject *pluginInstance, *pRes;
  if (iterator != NULL) {
    while((pluginInstance = PyIter_Next(iterator))) {
      PyObject *pluginInfo = PyDict_GetItem(pluginDict, pluginInstance);
      char *moduleName = objToStr(PyTuple_GetItem(pluginInfo, PLUGIN_MODULE_NAME));
      pRes = PyObject_CallMethod(pluginInstance, "XPluginDisable", NULL);
      if(pRes != Py_None) {
        fprintf(pythonLogFile, "%s XPluginDisable returned '%s' rather than None.\n", moduleName, objToStr(pRes));
      }
      if(PyErr_Occurred()) {
        fprintf(pythonLogFile, "Error occured during the %s XPluginDisable call:\n", moduleName);
        PyErr_Print();
      }else{
        Py_DECREF(pRes);
      }
      Py_DECREF(pluginInstance);
    }
    Py_DECREF(iterator);
  }
}

static void stopPluginList(PyObject *pluginList) {
  /* XPluginStop() for plugins in list, 
     clears menu for each plugin,
     removes plugin from dictionary,
     empties the list
  */
  PyObject *iterator = PyObject_GetIter(pluginList);
  PyObject *pluginInstance, *pRes;
  iterator = PyObject_GetIter(pluginList);
  if (iterator != NULL) {
    while((pluginInstance = PyIter_Next(iterator))) {
      PyObject *pluginInfo = PyDict_GetItem(pluginDict, pluginInstance);
      char *moduleName = objToStr(PyTuple_GetItem(pluginInfo, PLUGIN_MODULE_NAME));
      pRes = PyObject_CallMethod(pluginInstance, "XPluginStop", NULL);
      if(pRes != Py_None) {
        fprintf(pythonLogFile, "%s XPluginStop returned '%s' rather than None.\n", moduleName, objToStr(pRes));
      }
      if(PyErr_Occurred()) {
        fprintf(pythonLogFile, "Error occured during the %s XPluginStop call:\n", moduleName);
        PyErr_Print();
      }else{
        Py_DECREF(pRes);
      }

      /* still need to fully remove plugin menu for this plugin -- just to be sure */
      PyObject *mod = PyTuple_GetItem(pluginInfo, PLUGIN_MODULE_NAME);
      clearAllMenuItems(mod);
      Py_DECREF(mod);

      PyDict_DelItem(pluginDict, pluginInstance);
      PyDict_DelItem(moduleDict, pluginInfo);
      Py_DECREF(pluginInfo);
      Py_DECREF(pluginInstance);
    }
    Py_DECREF(iterator);
  }
  PyList_SetSlice(pluginList, 0, PyList_Size(pluginList), NULL);
}

PLUGIN_API void XPluginDisable(void)
{
  PyObject *pluginInfo, *pluginInstance, *pRes;
  Py_ssize_t pos = 0;
  if(disabled){
    return;
  }

  pythonDebug("DISABLING  Global plugins...");
  while(PyDict_Next(moduleDict, &pos, &pluginInfo, &pluginInstance)){
    char *moduleName = objToStr(PyTuple_GetItem(pluginInfo, PLUGIN_MODULE_NAME));
    if (PySequence_Contains(aircraftPlugins, pluginInstance) || PySequence_Contains(sceneryPlugins, pluginInstance)) {
      continue;
    }
    pRes = PyObject_CallMethod(pluginInstance, "XPluginDisable", NULL);
    if(pRes != Py_None) {
      fprintf(pythonLogFile, "%s XPluginDisable returned '%s' rather than None.\n", moduleName, objToStr(pRes));
    }
    if(PyErr_Occurred()) {
      fprintf(pythonLogFile, "Error occured during the %s XPluginDisable call:\n", moduleName);
      PyErr_Print();
    }else{
      Py_DECREF(pRes);
    }
  }
  pythonDebug("  DISABLED Global plugins.");
  pythonDebug("DISABLING Scenery plugins.");
  disablePluginList(sceneryPlugins);
  pythonDebug("  DISABLED Scenery plugins.");
  pythonDebug("DISABLING Aircraft plugins.");
  disablePluginList(aircraftPlugins);
  pythonDebug("  DISABLED Aircraft plugins.");
}

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFromWho, long inMessage, void *inParam)
{
  PyObject *pluginInfo, *pluginInstance, *pRes;
  Py_ssize_t pos = 0;
  PyObject *param;
  if(disabled){
    return;
  }
  param = PyLong_FromLong((long)inParam);
  /* printf("XPPython3 received message, which we'll try to send to all plugins: From: %d, Msg: %ld, inParam: %ld\n", */
  /*        inFromWho, inMessage, (long)inParam); */
  while(PyDict_Next(moduleDict, &pos, &pluginInfo, &pluginInstance)){
    char *moduleName = objToStr(PyTuple_GetItem(pluginInfo, PLUGIN_MODULE_NAME));
    pRes = PyObject_CallMethod(pluginInstance, "XPluginReceiveMessage", "ilO", inFromWho, inMessage, param);
    if (pRes != Py_None) {
      fprintf(pythonLogFile, "%s XPluginReceiveMessage didn't return None.\n", moduleName);
    }
    if(PyErr_Occurred()) {
      fprintf(pythonLogFile, "Error occured during the %s XPluginReceiveMessage call:\n", moduleName);
      PyErr_Print();
    }else{
      Py_DECREF(pRes);
    }
  }
  if (inMessage == XPLM_MSG_SCENERY_LOADED && XPLMGetCycleNumber() != 0){
    /* we'll get SCENERY_LOADED with Cycle# = 0 on startup, but we'll have already loaded the 
       scenery plugins (this matches XP behavior). We ignore if cycle == 0, because we don't want
       to disable and reload them. */
    pythonDebug("DISABLING Scenery plugins.");
    disablePluginList(sceneryPlugins);
    pythonDebug("  DISABLED Scenery plugins.");
    pythonDebug("STOPPING Scenery plugins.");
    stopPluginList(sceneryPlugins);
    pythonDebug("  STOPPED Scenery plugins.");
    pythonDebug("STARTING Scenery plugins.");
    loadSceneryPlugins();
    pythonDebug("  STARTED Scenery plugins.");
    pythonDebug("ENABLING Scenery plugins.");
    enablePluginList(sceneryPlugins);
    pythonDebug("  ENABLED Scenery plugins.");
  }
  if (inMessage == XPLM_MSG_PLANE_LOADED && inParam == XPLM_USER_AIRCRAFT) {
    loadAircraftPlugins();  /* start and enable are called */
  }
  if (inMessage == XPLM_MSG_PLANE_UNLOADED && inParam == XPLM_USER_AIRCRAFT && XPLMGetCycleNumber() != 0) {
    pythonDebug("DISABLING Aircraft plugins");
    disablePluginList(aircraftPlugins);
    pythonDebug("  DISABLED Aircraft plugins");
    pythonDebug("STOPPING Aircraft plugins");
    stopPluginList(aircraftPlugins);
    pythonDebug("  STOPPED Aircraft plugins");
  }
  Py_DECREF(param);
}

static void loadAircraftPlugins() {
  /* find, load, start and enable aircraft plugin for User Aircraft */
  char outFileName[512];
  char outPath[1024];
  XPLMGetNthAircraftModel(0, outFileName, outPath);
  if (!strlen(outPath)) {
    fprintf(pythonLogFile, "No user aircraft selected, skipping loadAircraftPlugins()\n");
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
  loadModules(plugins_path, package, "^PI_.*\\.py$", aircraftPlugins);
  pythonDebug("  STARTED Aircraft plugins.");
  free(package);
  free(plugins_path);

  pythonDebug("ENABLING Aircraft plugins");
  enablePluginList(aircraftPlugins);
  pythonDebug("  ENABLED Aircraft plugins");
  return;
}

void enablePluginList(PyObject *pluginList) {
  PyObject *iterator = PyObject_GetIter(pluginList);
  PyObject *pluginInstance, *pRes;
  if (iterator != NULL) {
    while((pluginInstance = PyIter_Next(iterator))) {
      PyObject *pluginInfo = PyDict_GetItem(pluginDict, pluginInstance);
      char *moduleName = objToStr(PyTuple_GetItem(pluginInfo, PLUGIN_MODULE_NAME));
      pRes = PyObject_CallMethod(pluginInstance, "XPluginEnable", NULL);
      if(!(pRes && PyLong_Check(pRes))){
        fprintf(pythonLogFile, "%s XPluginEnable returned '%s' rather than an integer.\n", moduleName, objToStr(pRes));
      }else{
        //printf("XPluginEnable returned %ld\n", PyLong_AsLong(pRes));
      }
      if(PyErr_Occurred()) {
        fprintf(pythonLogFile, "Error occured during the %s XPluginEnable call:\n", moduleName);
        PyErr_Print();
      }else{
        Py_DECREF(pRes);
      }
      Py_DECREF(pluginInstance);
    }
    Py_DECREF(iterator);
  }
}

int loadPythonLibrary() 
{
#if LIN || APL
  /* Prefered library is simple .so:
      libpython3.8.so
     But, that's usually a link to a versioned .so and sometimes, that
     link hasn't been created, so we'll also look for that:
      libpython3.8.so.1
     (there could be more versions, but that seems unlikely for most consumers)

     Now, prior to 3.8, the library name included 'm' to indicate it includes pymalloc, which
     which is prefered, so we look for those FIRST, and if not found, look for
     libraries without the 'm'.
  */
#if LIN
  char *suffix = "so";
  char *path = "";
#endif
#if APL
  char *suffix = "dylib";
  char *path = "/Library/Frameworks/Python.framework/Versions/" PYTHONVERSION "/lib/";
#endif
  char *library;
  asprintf(&library, "%slibpython%sm.%s", path, PYTHONVERSION, suffix);
  pythonHandle = dlopen(library, RTLD_LAZY | RTLD_GLOBAL);
  if (!pythonHandle) {
    sprintf(library, "%slibpython%sm.%s.1", path, PYTHONVERSION, suffix);
    pythonHandle = dlopen(library, RTLD_LAZY | RTLD_GLOBAL);
  }
  if (!pythonHandle) {
    sprintf(library, "%slibpython%s.%s", path, PYTHONVERSION, suffix);
    pythonHandle = dlopen(library, RTLD_LAZY | RTLD_GLOBAL);
  }
  if (!pythonHandle) {
    sprintf(library, "%slibpython%s.%s.1", path, PYTHONVERSION, suffix);
    pythonHandle = dlopen(library, RTLD_LAZY | RTLD_GLOBAL);
  }
  if (!pythonHandle) {
    fprintf(pythonLogFile, "Unable to find python shared library '%slibpython%s.%s'\n", path, PYTHONVERSION, suffix);
    fflush(pythonLogFile);
    return -1;
  }
  fprintf(pythonLogFile, "Python shared library loaded: %s\n", library);
  fflush(pythonLogFile);
  free(library);
#endif
  return 0;
}
