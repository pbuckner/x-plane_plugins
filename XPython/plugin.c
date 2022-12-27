//Python comes first!
#include <Python.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdbool.h>
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMUtilities.h>
#include <XPLM/XPLMPlugin.h>
#include <sys/types.h>
#include <dlfcn.h>
#if LIN
/* to get strlcat() */
#include <bsd/string.h>
#endif

#include "menus.h"
#include "utils.h"
#include "plugin_dl.h"
#include "manage_instances.h"
#include "load_modules.h"
#include "xppython.h"
#include "ini_file.h"

/*************************************
 * Python plugin upgrade for Python 3
 *   Michal        f.josef@email.cz (uglyDwarf on x-plane.org)
 *   Peter Buckner pbuck@avnwx.com (pbuckner on x-plane.org) 
 *
 * Upgraded from original Python2 version by
 *   Sandy Barbour (on x-plane.org)
 */

/**********************
 * Plugin configuration
 */
const char *pythonPluginsPath = "Resources/plugins/PythonPlugins";
const char *pythonInternalPluginsPath = "Resources/plugins/XPPython3";

static const char *pythonPluginName = "XPPython3";
const char *pythonPluginVersion = XPPYTHON3VERSION " - for Python " PYTHONVERSION;
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

static int loadPythonLibrary(void);
static FILE *getLogFile(void);
static void setSysPath(void);
static void handleConfigFile(void);
static void reloadSysModules(void);
static void initMtimes(void);

time_t SymStartTime = 0;

FILE *pythonLogFile;

PLUGIN_API int XPluginEnable(void);
PLUGIN_API void XPluginDisable(void);

static bool xpy3_disabled = true;
static bool xpy3_started = false;

// extern int allErrorsEncountered;
static PyObject *loggerModuleObj;
static void *pythonHandle = NULL;

extern PyMODINIT_FUNC PyInit_XPLMDefs(void);
extern PyMODINIT_FUNC PyInit_XPLMDisplay(void);
extern PyMODINIT_FUNC PyInit_XPLMGraphics(void);
extern PyMODINIT_FUNC PyInit_XPLMDataAccess(void);
extern PyMODINIT_FUNC PyInit_XPLMUtilities(void);
extern PyMODINIT_FUNC PyInit_XPLMScenery(void);
extern PyMODINIT_FUNC PyInit_XPLMMenus(void);
extern PyMODINIT_FUNC PyInit_XPLMNavigation(void);
extern PyMODINIT_FUNC PyInit_XPLMPlugin(void);
extern PyMODINIT_FUNC PyInit_XPLMPlanes(void);
extern PyMODINIT_FUNC PyInit_XPLMProcessing(void);
extern PyMODINIT_FUNC PyInit_XPLMCamera(void);
extern PyMODINIT_FUNC PyInit_XPWidgetDefs(void);
extern PyMODINIT_FUNC PyInit_XPWidgets(void);
extern PyMODINIT_FUNC PyInit_XPStandardWidgets(void);
extern PyMODINIT_FUNC PyInit_XPUIGraphics(void);
extern PyMODINIT_FUNC PyInit_XPWidgetUtils(void);
extern PyMODINIT_FUNC PyInit_XPLMInstance(void);
extern PyMODINIT_FUNC PyInit_XPLMMap(void);
/* extern PyMODINIT_FUNC PyInit_SBU(void); */
extern PyMODINIT_FUNC PyInit_XPPython(void);
extern PyMODINIT_FUNC PyInit_XPythonLogWriter(void);

int initPython(void){
  /* Initalize Python and internal modules
   * return 0: success, otherwise: fail... all failures are fatal */

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
  /* XPythonLogger
     It had to be treated differently from other python modules because we
     wanted it imported first and unloaded last */
  PyImport_AppendInittab("XPythonLogger", PyInit_XPythonLogWriter);
  
  /* PyImport_AppendInittab("SandyBarbourUtilities", PyInit_SBU);  -- Python2 stuff, for which there is no xppython3 equivalent */
  
  Py_Initialize();
  if(!Py_IsInitialized()){
    fprintf(pythonLogFile, "[XPPython3] Failed to initialize Python.\n");
    fflush(pythonLogFile);
    return -1;
  }

  char *msg;
  PyObject *runtime_version = PySys_GetObject("version_info"); /* borrowed */
  int major = PyLong_AsLong(PyTuple_GetItem(runtime_version, 0)); /* borrowed */
  int minor = PyLong_AsLong(PyTuple_GetItem(runtime_version, 1)); /* borrowed */
  int micro = PyLong_AsLong(PyTuple_GetItem(runtime_version, 2)); /* borrowed */

#if IBM
  if (major == 3 && minor == 11 && micro == 1) {
    fprintf(pythonLogFile, "Python v3.11.1 is not supported on Windows, use v3.10.x, v3.11.0, or v3.11.2+ \n");
    fflush(pythonLogFile);
    return -1;
  }
#endif

  asprintf(&msg, "[XPPython3] Python runtime initialized %d.%d.%d\n", major, minor, micro);
  XPLMDebugString(msg);
  free(msg);

  loggerModuleObj = PyImport_ImportModule("XPythonLogger");

  setSysPath();
  /***********************
   * Init internal dictionaries
   */
  xppythonDicts = PyDict_New();
  xppythonCapsules = PyDict_New();
  moduleDict = PyDict_New();
  pluginDict = PyDict_New();
  aircraftPlugins = PyList_New(0);
  sceneryPlugins = PyList_New(0);

  if (! (xppythonDicts || xppythonCapsules || moduleDict || pluginDict || aircraftPlugins || sceneryPlugins)) {
    fprintf(pythonLogFile, "[XPPython3] Failed to allocate internal data structures. Fatal Error.\n");
    return -1;
  }

  PyDict_SetItemString(xppythonDicts, "plugins", pluginDict);
  PyDict_SetItemString(xppythonDicts, "modules", moduleDict);

  return 0;
}


static int startPython(void)
/* Start Python and all plugin instances
 * return 0: success, otherwise fail... all failures are fatal */
{
  pythonDebug("Calling startPython()");
  if (xpy3_started) {
    pythonDebug("Error, python already started");
    return 0;
  }

  loadSDKFunctions();

  if(initPython()) {
    fprintf(pythonLogFile, "[XPPython3] Failed to start python, fatal error.\n");
    return -1;
  }

  xpy_startInternalInstances();
  #define EXCLUDE_AIRCRAFT 0
  xpy_startInstances(EXCLUDE_AIRCRAFT);
  xpy3_started = true;
  return 1;
}


static int stopPython(void)
{
  if(!xpy3_started) return -1;

  xpy_stopInstances();

  if (pythonDebugs) {
    char *dicts [] = {"plugins", "modules", "accessors", "drefs", "sharedDrefs", "drawCallbacks",
      "drawCallbackIDs", "keySniffCallbacks", "windows", "hotkeys", "hotkeyIDs", "mapCreates", "mapRefs", "maps",
      "menus", "menuRefs", "menuPluginIdx", "errCallbacks", "commandCallbacks", "commandRefcons",
      "widgetCallbacks", "widgetProperties", NULL};
    char **dict_ptr = dicts;
    while(*dict_ptr != NULL) {
      if (PyDict_Size(PyDict_GetItemString(xppythonDicts, *dict_ptr))) {
        pythonDebug("{%s}: %s", *dict_ptr, objDebug(PyDict_GetItemString(xppythonDicts, *dict_ptr)));
      }
      ++dict_ptr;
    }
  }
              
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
                  "XPLMMap", "XPLMDataAccess", /*"SandyBarbourUtilities", */ "XPPython", NULL};
  char **mod_ptr = mods;

  while(*mod_ptr != NULL){
    PyObject *mod = PyImport_ImportModule(*mod_ptr);
    fflush(stdout);
    if (PyErr_Occurred()) {
      fprintf(pythonLogFile, "[XPPython3] Failed during stop of internal module %s\n", *mod_ptr);
      pythonLogException();
      return 1;
    }
      
    if(mod){
      PyObject *pRes = PyObject_CallMethod(mod, "_cleanup", NULL);
      if (PyErr_Occurred() ) {
        fprintf(pythonLogFile, "[XPPython3] Failed during cleanup of internal module %s\n", *mod_ptr);
        pythonLogException();
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
  xpy3_started = false;
  return 0;
}

PLUGIN_API int XPluginStart(char *outName, char *outSig, char *outDesc)
{
  if (XPLMFindPluginBySignature("sandybarbour.projects.pythoninterface") != XPLM_NO_PLUGIN_ID) {
    XPLMDebugString("[XPPython3] FATAL ERROR: XPPython3 Detected python2 PythonInterface plugin. These plugins are incompatible\n");
    return 0;
  }

  pythonLogFile = getLogFile();

  char *c_time_string;
  SymStartTime = time(NULL);
  c_time_string = ctime(&SymStartTime);

  fprintf(pythonLogFile, "[%s] Version %s Started -- %s\n", pythonPluginName, pythonPluginVersion, c_time_string);

  if (loadPythonLibrary() == -1) {return 0;}

  fflush(pythonLogFile);
  strcpy(outName, pythonPluginName);
  strcpy(outSig, pythonPluginSig);
  strcpy(outDesc, pythonPluginDesc);

  if (XPLMHasFeature("XPLM_USE_NATIVE_PATHS")) {
    XPLMEnableFeature("XPLM_USE_NATIVE_PATHS", 1);
  } else {
    fprintf(pythonLogFile, "[XPPython3] Warning: XPLM_USE_NATIVE_PATHS not enabled. Using Legacy paths.\n");
  }
  if (XPLMHasFeature("XPLM_USE_NATIVE_WIDGET_WINDOWS")) {
    XPLMEnableFeature("XPLM_USE_NATIVE_WIDGET_WINDOWS", 1);
  } else {
    fprintf(pythonLogFile, "[XPPython3] Warning: XPLM_USE_NATIVE_WIDGET_WINDOWS not enabled. Using Legacy windows.\n");
  }

  handleConfigFile();

  /* Create Commands */
  disableScripts = XPLMCreateCommand(pythonDisableCommand, "Disable all running scripts");
  enableScripts = XPLMCreateCommand(pythonEnableCommand, "Enable all scripts");
  reloadScripts = XPLMCreateCommand(pythonReloadCommand, "Reload all scripts");

  XPLMRegisterCommandHandler(disableScripts, commandHandler, 1, (void *)0);
  XPLMRegisterCommandHandler(enableScripts, commandHandler, 1, (void *)1);
  XPLMRegisterCommandHandler(reloadScripts, commandHandler, 1, (void *)2);

  pythonDebug("Registered commands");

  if(startPython() == -1) return 0;

  return 1;
}


PLUGIN_API void XPluginStop(void)
{
  stopPython();
  XPLMUnregisterCommandHandler(disableScripts, commandHandler, 1, (void *)0);
  XPLMUnregisterCommandHandler(enableScripts, commandHandler, 1, (void *)1);
  XPLMUnregisterCommandHandler(reloadScripts, commandHandler, 1, (void *)2);
  /* if(allErrorsEncountered){ */
  /*   fprintf(pythonLogFile, "Total errors encountered: %d\n", allErrorsEncountered); */
  /* } */
  char *c_time_string;
  time_t current_time;
  current_time = time(NULL);
  c_time_string = ctime(&current_time);

  fprintf(pythonLogFile, "[%s] Stopped. %s\n", pythonPluginName, c_time_string);
  fclose(pythonLogFile);
}

static int commandHandler(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void *inRefcon)
{
  (void) inRefcon;
  if(inPhase != xplm_CommandBegin){
    return 0;
  }
  if(inCommand == disableScripts){
    if (! xpy3_disabled) {
      XPluginDisable();
      xpy3_disabled = true;
      fprintf(pythonLogFile, "[XPPython3] Disabled scripts.\n");
    } else {
      fprintf(pythonLogFile, "XPPython3: already disabled.\n");
    }
  }else if(inCommand == enableScripts){
    if (xpy3_disabled) {
      XPluginEnable();
      fprintf(pythonLogFile, "[XPPython3] Enabled scripts.\n");
    } else {
      fprintf(pythonLogFile, "XPPython3: already enabled.\n");
    }
  }else if(inCommand == reloadScripts){
    if (! xpy3_disabled) {
      char *c_time_string;
      time_t current_time;
      current_time = time(NULL);
      c_time_string = ctime(&current_time);
      fprintf(pythonLogFile, "[XPPython3] Reloading Scripts.======= %s\n", c_time_string);
      fprintf(pythonLogFile, "[XPPython3] Reload -- 1) Disable existing scripts.=======\n");
      xpy_disableInstances();/* Internal, PythonPlugins, scenery, aircraft */
      fprintf(pythonLogFile, "[XPPython3] Reload -- 2) Stop existing scripts.=======\n");
      xpy_stopInstances();   /* Internal, PythonPlugins, scenery, aircraft */
      fprintf(pythonLogFile, "[XPPython3] Reload -- 3) Reset Menu.=======\n");
      resetMenus();
      fprintf(pythonLogFile, "[XPPython3] Reload -- 4) Reload Config file.=======\n");
      handleConfigFile();
      fprintf(pythonLogFile, "[XPPython3] Reload -- 5) Determine changed python modules.=======\n");
      reloadSysModules();
      fprintf(pythonLogFile, "[XPPython3] Reload -- 6) Restart Internal Python plugins.=======\n");
      xpy_startInternalInstances(); /* Internal */
      fprintf(pythonLogFile, "[XPPython3] Reload -- 7) Restart PythonPlugins (Scenery and aircraft) plugins.=======\n");
      xpy_startInstances(1); /* PythonPlugins, scenery, aircraft */
      fprintf(pythonLogFile, "[XPPython3] Reload -- 8) Enable PythonPlugins.=======\n");
      xpy_enableInstances(); /* Internal, PythonPlugins, scenery, aircraft */
      fprintf(pythonLogFile, "[XPPython3] Reloaded Scripts.=======\n");
      xpy3_disabled = false;
    } else {
      fprintf(pythonLogFile, "XPPython3 is disabled, cannot reload.\n");
    }
  }
  fflush(pythonLogFile);
  return 0;
}


PLUGIN_API int XPluginEnable(void)
{
  xpy3_disabled = false;
  xpy_enableInstances();
  initMtimes();
  return 1;
}


PLUGIN_API void XPluginDisable(void)
{
  if(xpy3_disabled) return;
  xpy_disableInstances();
}


PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFromWho, long inMessage, void *inParam)
{
  if(xpy3_disabled) return;

  PyObject *pluginInfo, *pluginInstance, *pRes;
  Py_ssize_t pos = 0;
  PyObject *param;
  param = PyLong_FromLong((long)inParam);
  pythonDebug("XPPython3 received message, forwarding to all plugins: From: %d, Msg: %ld, inParam: %ld",
              inFromWho, inMessage, (long)inParam);

  while(PyDict_Next(moduleDict, &pos, &pluginInfo, &pluginInstance)){
    char *moduleName = objToStr(PyTuple_GetItem(pluginInfo, PLUGIN_MODULE_NAME));
    pRes = PyObject_CallMethod(pluginInstance, "XPluginReceiveMessage", "ilO", inFromWho, inMessage, param);

    PyObject *err = PyErr_Occurred();
    if (err) {
      if (PyObject_HasAttrString(pluginInstance, "XPluginReceiveMessage")) {
        fprintf(pythonLogFile, "[%s] Error occured during the XPluginReceiveMessage call:\n", moduleName);
        pythonLogException();
      } else {
        /* ignore error, if XPluginReceiveMessage is not defined in the PythonInterface class */
        PyErr_Clear();
      }
    } else {
      if (pRes != Py_None) {
        fprintf(pythonLogFile, "[%s] XPluginReceiveMessage returned '%s' rather than None. Value ignored\n", moduleName, objToStr(pRes));
      }
      Py_DECREF(pRes);
    }
    free(moduleName);
  }
  if (inMessage == XPLM_MSG_SCENERY_LOADED && XPLMGetCycleNumber() != 0){
    /* we'll get SCENERY_LOADED with Cycle# = 0 on startup, but we'll have already loaded the 
       scenery plugins (this matches XP behavior). We ignore if cycle == 0, because we don't want
       to disable and reload them. */
    xpy_disableSceneryPlugins();
    xpy_stopSceneryPlugins();
    xpy_startSceneryPlugins();
    xpy_enableSceneryPlugins();
  }
  if (inMessage == XPLM_MSG_PLANE_LOADED && inParam == XPLM_USER_AIRCRAFT) {
    xpy_startAircraftPlugins();
    xpy_enableAircraftPlugins();
  }
  if (inMessage == XPLM_MSG_PLANE_UNLOADED && inParam == XPLM_USER_AIRCRAFT && XPLMGetCycleNumber() != 0) {
    xpy_disableAircraftPlugins();
    xpy_stopAircraftPlugins();
  }
  Py_DECREF(param);
}

static int loadPythonLibrary(void) 
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

     The _reason_ we have to do this is a python problem which (still) exists on linux (November 2022)
     Essential, the python shared libs (say _ssl.so) don't look within the python libpythonX.X.so shared
     lib, so the imported shared lib will fail with something like:
      ImportError: /usr/lib/python/lib-dynload/_sso.so: undefined symbol: PyEx_ValueError

     See https://mail.python.org/pipermail/new-bugs-announce/2008-November/003322.html
  */
#if LIN
  char *suffix = "so";
  char *path = "";
#endif
#if APL
  char *suffix = "dylib";
  char *path = "/Library/Frameworks/Python.framework/Versions/" PYTHONVERSION "/lib/";
#endif
  char library[1024];
  sprintf(library, "%slibpython%sm.%s", path, PYTHONVERSION, suffix);
  pythonHandle = dlopen(library, RTLD_LAZY | RTLD_GLOBAL);
  if (!pythonHandle) {
    sprintf(library, "%slibpython%sm.%s.1", path, PYTHONVERSION, suffix);
    pythonHandle = dlopen(library, RTLD_LAZY | RTLD_GLOBAL);
  }
  if (!pythonHandle) {
    sprintf(library, "%slibpython%sm.%s.1.0", path, PYTHONVERSION, suffix);
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
    sprintf(library, "%slibpython%s.%s.1.0", path, PYTHONVERSION, suffix);
    pythonHandle = dlopen(library, RTLD_LAZY | RTLD_GLOBAL);
  }
  if (!pythonHandle) {
    fprintf(pythonLogFile, "Unable to find python shared library '%slibpython%s.%s'\n", path, PYTHONVERSION, suffix);
    fflush(pythonLogFile);
    return -1;
  }
  fprintf(pythonLogFile, "[XPPython3] Python shared library loaded: %s\n", library);
  fflush(pythonLogFile);
#endif
  return 0;
}

static FILE *getLogFile(void) {
  static char *ENV_logFileVar = "XPPYTHON3_LOG";  // set this environment to override logFileName
  static char *ENV_logPreserve = "XPPYTHON3_PRESERVE";  // DO NOT truncate XPPython log on startup. If set, we preserve, if unset, we truncate
  static char *logFileName = "XPPython3Log.txt";
  char *log;
  FILE *fp;
  log = getenv(ENV_logFileVar);
  if(log != NULL){
    logFileName = log;
  }
  if (getenv(ENV_logPreserve) != NULL) {
    printf("Preserving log file\n"); fflush(stdout);
    fp = fopen(logFileName, "a");
  } else {
    fp = fopen(logFileName, "w");
  }
  char *msg;
  if(fp == NULL){
    fp = stdout;
    asprintf(&msg, "[XPPython3] Starting %s (compiled: %0x)... Logging to standard out\n",
             pythonPluginVersion, PY_VERSION_HEX);
  } else {
    asprintf(&msg, "[XPPython3] Starting %s (compiled: %0x)... Logging to %s\n",
             pythonPluginVersion, PY_VERSION_HEX, logFileName);
  }
  XPLMDebugString(msg);
  free(msg);
  return fp;
}

static void setSysPath(void) {
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
}

static void handleConfigFile(void) {  /* Find and handle config.ini file */
  XPLMGetPrefsPath(xpy_ini_file);
  XPLMExtractFileAndPath(xpy_ini_file);
#if LIN || APL
  strlcat(xpy_ini_file, "/xppython3.ini", 512);
#endif
#if IBM
  strcat_s(xpy_ini_file, 512, "/xppython3.ini");
#endif
  pythonDebugs = xpy_config_get_int("[Main].debug");
  pythonWarnings = xpy_config_get_int("[Main].warning");
#ifndef Py_LIMITED_API
  Py_VerboseFlag = xpy_config_get_int("[Main].py_verbose");/* 0= off, 1= each file as loaded, 2= each file that is checked when searching for module */
#endif
  pythonFlushLog = xpy_config_get_int("[Main].flush_log");/* 0= off, 1= on */
  pythonDebug("Read config file: %s", xpy_ini_file);
}

static void initMtimes(void) {
  PythonModuleMTimes = PyDict_New();
  /* Initials mtime for imports */
  PyObject *localsDict = PyDict_New();
  PyDict_SetItemString(localsDict, "__builtins__", PyEval_GetBuiltins());
  PyDict_SetItemString(localsDict, "mtimes", PythonModuleMTimes);
  PyRun_String("import sys\n"
               "import os\n"
               "import importlib\n"
               "for mod in list(sys.modules.values()):\n"
               "  if mod and hasattr(mod, '__file__') and mod.__file__:\n"
               "    try:\n"
               "      mtime = os.stat(mod.__file__).st_mtime\n"
               "    except (OSError, IOError) as e:\n"
               "      continue\n"
               "    if mod.__file__.endswith('.pyc') and os.path.exists(mod.__file__[:-1]):\n"
               "      mtime = max(mtime, os.stat(mod.__file__[:-1]).st_mtime)\n"
               "    if mod not in mtimes:\n"
               "      mtimes [mod] = mtime\n",
               Py_file_input, localsDict, localsDict);
  Py_DECREF(localsDict);
}

static void reloadSysModules(void) {
  PyObject *localsDict = PyDict_New();
  PyDict_SetItemString(localsDict, "__builtins__", PyEval_GetBuiltins());
  PyDict_SetItemString(localsDict, "mtimes", PythonModuleMTimes);
  PyDict_SetItemString(localsDict, "sym_start", PyLong_FromLong(SymStartTime));
  /* reload_unknown... if sys.modules has a module for which we don't know the mtime,
   *                   do we reload it anyway? Py_True
   */
  PyDict_SetItemString(localsDict, "reload_unknown", Py_True);
  PyRun_String("import sys\n"
               "import os\n"
               "import importlib\n"
               "result = []\n"
               "for mod in list(sys.modules.values()):\n"
               "  if mod and hasattr(mod, '__file__') and mod.__file__:\n" /* some don't have __file__, some __file__ are None */
               "    try:\n"
               "      mtime = os.stat(mod.__file__).st_mtime\n"  /* if it's simple foo.py */
               "    except (OSError, IOError) as e:\n"
               "      result.append(f'Opps for {mod.__file__}: {e}')\n"
               "      continue\n"
               "    if mod.__file__.endswith('.pyc') and os.path.exists(mod.__file__[:-1]):\n"  /* if there is (also) a pyc */
               "      mtime = max(mtime, os.stat(mod.__file__[:-1]).st_mtime)\n"
               "    if (not reload_unknown and mod not in mtimes) or (mtime <= sym_start and mod not in mtimes):\n"
               "      mtimes [mod] = mtime\n"
               "    elif (reload_unknown and not mod in mtimes and mtime > sym_start)  or mtimes[mod] < mtime:\n"
               "      mtimes[mod] = mtime\n"
               "      if hasattr(mod, '__spec__') and hasattr(mod.__spec__, 'origin') and mod.__spec__.origin in ('frozen', 'builtin'):\n"
               "        continue\n"
               "      try:\n"
               "        result.append(mod.__file__)\n"
               "        importlib.reload(mod)\n"
               "        mtimes[mod] = mtime\n"
               "      except ImportError:\n"
               "        result.append(f'>> Failed to reload {mod.__file__}')\n"
               "        pass\n"
               "      except Exception as e:\n"
               "        result.append(f'>> Failed reloading {mod.__file__}: {e}')\n"
               "result = '\\n  > '.join(result)\n",
               Py_file_input, localsDict, localsDict);
  PyObject *result = PyDict_GetItemString(localsDict, "result");
  PyObject *err = PyErr_Occurred();
  if (err) {
    fprintf(pythonLogFile, "[XPPython3] Error occured during the reload of modules.\n");
    pythonLogException();
  }
  if (PyUnicode_GetLength(result) > 2) {
    fprintf(pythonLogFile, " Module(s) reloaded:  \n  > %s\n", objToStr(result));
  }
  Py_DECREF(localsDict);
}
