//Python comes first!
#include <Python.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdbool.h>
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMUtilities.h>
#include <XPLM/XPLMPlugin.h>
#include <sys/types.h>
#include <dlfcn.h>
#include <wchar.h>
#if LIN
/* to get strlcat() */
#include <bsd/string.h>
#endif

#include "utils.h"
#include "plugin_dl.h"
#include "manage_instances.h"
#include "load_modules.h"
#include "xppython.h"
#include "ini_file.h"

#if !defined(Py_LIMITED_API)
static int pythonVerbose = 0;
#endif

/*************************************
 * Python plugin upgrade for Python 3
 *   Michal        f.josef@email.cz (uglyDwarf on x-plane.org)
 *   Peter Buckner pbuck@xppython3.org (pbuckner on x-plane.org) 
 *
 * Upgraded from original Python2 version by
 *   Sandy Barbour (on x-plane.org)
 */

/**********************
 * Plugin configuration
 */
const char *pythonPluginsPath = "Resources/plugins/PythonPlugins";
const char *pythonInternalPluginsPath = "Resources/plugins/XPPython3";
const char *pythonInternalHooksPath =   "Resources/plugins/XPPython3/hooks";

static const char *pythonPluginName = "XPPython3";
const char *pythonPluginVersion = XPPYTHON3VERSION;
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

static int loadLocalPythonLibrary(void);
static void setSysPath(void);
static void handleConfigFile(void);
static void reloadSysModules(void);
static void initMtimes(void);

time_t SymStartTime = 0;

PLUGIN_API int XPluginEnable(void);
PLUGIN_API void XPluginDisable(void);

static bool xpy3_disabled = true;
static bool xpy3_started = false;

// extern int allErrorsEncountered;
static PyObject *loggerModuleObj;
static void *pythonHandle = NULL;

pthread_t pythonThread = (pthread_t) NULL;

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
extern PyMODINIT_FUNC PyInit_XPLMSound(void);
extern PyMODINIT_FUNC PyInit_XPWidgetDefs(void);
extern PyMODINIT_FUNC PyInit_XPWidgets(void);
extern PyMODINIT_FUNC PyInit_XPLMWeather(void);
extern PyMODINIT_FUNC PyInit_XPStandardWidgets(void);
extern PyMODINIT_FUNC PyInit_XPUIGraphics(void);
extern PyMODINIT_FUNC PyInit_XPWidgetUtils(void);
extern PyMODINIT_FUNC PyInit_XPLMInstance(void);
extern PyMODINIT_FUNC PyInit_XPLMMap(void);
/* extern PyMODINIT_FUNC PyInit_SBU(void); */
extern PyMODINIT_FUNC PyInit_XPPython(void);
extern PyMODINIT_FUNC PyInit_XPCursor(void);
extern PyMODINIT_FUNC PyInit_XPythonLogWriter(void);

static void
debugErrorCallback(const char *inMessage) {
  pythonLog("[XPPython3] Error Callback: %s", inMessage);
  pythonLogFlush();
};

int initPython(void){
  /* Initalize Python and internal modules
   * return 0: success, otherwise: fail... all failures are fatal */

  printf("getting pthread_self\n");
  pythonThread = pthread_self();
  printf("got pthread_self\n");
  
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
  PyImport_AppendInittab("XPLMWeather", PyInit_XPLMWeather);
  PyImport_AppendInittab("XPLMSound", PyInit_XPLMSound);
  PyImport_AppendInittab("XPCursor", PyInit_XPCursor);
  /* XPythonLogger
     It had to be treated differently from other python modules because we
     wanted it imported first and unloaded last */
  PyImport_AppendInittab("XPythonLogger", PyInit_XPythonLogWriter);
  
  /* PyImport_AppendInittab("SandyBarbourUtilities", PyInit_SBU);  -- Python2 stuff, for which there is no xppython3 equivalent */
  
#if defined(Py_LIMITED_API)
  Py_Initialize();
#else
  PyPreConfig preconfig;
  PyPreConfig_InitIsolatedConfig(&preconfig);
  //preconfig.utf8_mode = 1; ... this seems to cause Windows filename errors
  PyStatus status = Py_PreInitialize(&preconfig);
  if (PyStatus_Exception(status)) {
    pythonLog("preinit status is error");
    Py_ExitStatusException(status);
  }

  PyConfig config;
  PyConfig_InitIsolatedConfig(&config);

  char executable[512];
  XPLMGetSystemPath(executable);
  /* config.home seems to be set properly in the executables (?) or other tricks
     such as patchelf / install_name_tool, so I don't appear to need to set
     it explicitly
  */
#if APL
  /* config.home = wcsdup(L"Resources/plugins/XPPython3/mac_x64/python" PYTHONVERSION); */
  strcat(executable, "Resources/plugins/XPPython3/mac_x64/python" PYTHONVERSION "/Resources/Python.app/Contents/MacOS/Python");
#elif IBM
  /* config.home = wcsdup(L"Resources/plugins/XPPython3/win_x64"); */
  strcat(executable, "Resources/plugins/XPPython3/win_x64/pythonw.exe");
#elif LIN
  /* config.home = wcsdup(L"Resources/plugins/XPPython3/lin_x64/python" PYTHONVERSION); */
  strcat(executable, "Resources/plugins/XPPython3/lin_x64/python" PYTHONVERSION "/bin/python" PYTHONVERSION);
  /* setenv("PYOPENGL_PLATFORM", "egl", 1);*/
  setenv("PYOPENGL_PLATFORM", "glx", 1); /* to get around Wayland... */
#endif  

  status = PyConfig_SetBytesString(&config, &config.executable, strdup(executable));
  if (PyStatus_Exception(status)) {
    pythonLog("SetBytesString status is error");
    Py_ExitStatusException(status);
  }
  if (pythonVerbose) {
    config.verbose = pythonVerbose;
  }
  status = Py_InitializeFromConfig(&config);
  if (PyStatus_Exception(status)) {
    pythonLog("InitializeFrom Config status is error");
    Py_ExitStatusException(status);
  }
  PyConfig_Clear(&config);
#endif
  if(!Py_IsInitialized()){
    pythonLog("[XPPython3] Failed to initialize Python.");
    pythonLogFlush();
    return -1;
  }

  char *msg;
  PyObject *runtime_version = PySys_GetObject("version_info"); /* borrowed */
  int major = PyLong_AsLong(PyTuple_GetItem(runtime_version, 0)); /* borrowed */
  int minor = PyLong_AsLong(PyTuple_GetItem(runtime_version, 1)); /* borrowed */
  int micro = PyLong_AsLong(PyTuple_GetItem(runtime_version, 2)); /* borrowed */

#if IBM
  if (major == 3 && minor == 11 && micro == 1) {
    pythonLog("Python v3.11.1 is not supported on Windows, use v3.10.x, v3.11.0, or v3.11.2+ ");
    pythonLogFlush();
    return -1;
  }
#endif

  if (-1 == asprintf(&msg, "[XPPython3] Python runtime initialized %d.%d.%d\n", major, minor, micro)) {
    pythonLog("[XPPython3] Failed to allocate asprintf memory, cannot initialize.");
    return -1;
  }
  XPLMDebugString(msg);
  if (msg) free(msg);

  loggerModuleObj = PyImport_ImportModule("XPythonLogger");

  setSysPath();
  /***********************
   * Init internal dictionaries
   */
  XPY3pythonDicts = PyDict_New();
  XPY3pythonCapsules = PyDict_New();
  XPY3moduleDict = PyDict_New();
  XPY3pluginDict = PyDict_New();
  XPY3aircraftPlugins = PyList_New(0);
  XPY3sceneryPlugins = PyList_New(0);

  if (! (XPY3pythonDicts && XPY3pythonCapsules && XPY3moduleDict && XPY3pluginDict && XPY3aircraftPlugins && XPY3sceneryPlugins)) {
    pythonLog("[XPPython3] Failed to allocate internal data structures. Fatal Error.");
    return -1;
  }

  PyDict_SetItemString(XPY3pythonDicts, "plugins", XPY3pluginDict);
  PyDict_SetItemString(XPY3pythonDicts, "modules", XPY3moduleDict);

  if (ERRCHECK || pythonDebugs) {
    /* if beta/ERRCHECK or if user has enabled pythonDebugs, set a defaut ErrorCallback */
    pythonLog("[XPPython3] Enabling XP Error Callback -- XP SDK API errors will be directed to python log");
    XPLMSetErrorCallback(debugErrorCallback);
  }
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
    pythonLog("[XPPython3] Failed to start python, fatal error.");
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
    /* Print out currently "undeleted" objects... ideally, these would be cleaned up by the owing python plugins
       This is output only -- we're not deleting them here.
     */
    pythonLog("Undeleted items: begin vvvvv");
    char *dicts [] = {"plugins", "modules", "accessors", "drefs", "sharedDrefs", "drawCallbacks",
      "keySniffCallbacks", "windows", "hotkeys", "hotkeyIDs", "mapCreates", "mapRefs", "maps",
      "menus", "menuRefs", "menuPluginIdx", "errCallbacks", "commandCallbacks", "commandRevDict",
      "widgetCallbacks", "widgetProperties", "flightLoops", "flightLoopIDs", NULL};
    char **dict_ptr = dicts;
    while(*dict_ptr != NULL) {
      PyObject *dict = PyDict_GetItemString(XPY3pythonDicts, *dict_ptr); // borrowed
      if (PyDict_Size(dict) > 0) {
        pythonLog("{%s}: [%d]", *dict_ptr, PyDict_Size(dict));
        Py_ssize_t pos = 0;
        PyObject *key,  *value;
        while(PyDict_Next(dict, &pos, &key, &value)){
          char *key_s = objToStr(key);
          char *value_s = objToStr(value);
          if (! strcmp(*dict_ptr, "widgetProperties") ) {
#if ERRCHECK
            /* widgetProperites key is Tuple<capsule, propID> */
            PyObject *capsule = PyTuple_GetItem(key, 0);
            /* widgetIDCapsules is <PyLong *ptr> : (<capsule> <module>)
               So we have to iterate through all items to find the module */
            Py_ssize_t capsule_pos = 0;
            PyObject *capsule_key, *capsule_value;
            PyObject *module = NULL;
            while(PyDict_Next(widgetIDCapsules, &capsule_pos, &capsule_key, &capsule_value)) {
              if (capsule == PyTuple_GetItem(capsule_value, 0)) {
                module = PyTuple_GetItem(capsule_value, 1);
                break;
              }
            }
            char *module_s = objToStr(module);
            pythonLog("  %s / %s:%s%s", key_s, module_s, strlen(value_s) > 10 ? "\n    " : " ", value_s);
            free(module_s);
#else
            pythonLog("  %s:%s %s", key_s, strlen(value_s) > 10 ? "\n    " : " ", value_s);
#endif            
          } else {
            pythonLog("  %s:%s %s", key_s, strlen(value_s) > 10 ? "\n    " : " ", value_s);
          }
          free(key_s);
          free(value_s);
        }
      }
      ++dict_ptr;
    }
    pythonLog("Undeleted items: end   ^^^^");
  }
              
  XPLMClearAllMenuItems(XPLMFindPluginsMenu());

  PyDict_Clear(XPY3moduleDict);
  PyDict_Clear(XPY3pluginDict);
  PyList_SetSlice(XPY3aircraftPlugins, 0, PyList_Size(XPY3aircraftPlugins), NULL);
  PyList_SetSlice(XPY3sceneryPlugins, 0, PyList_Size(XPY3sceneryPlugins), NULL);

  PyDict_DelItemString(XPY3pythonDicts, "modules");
  PyDict_DelItemString(XPY3pythonDicts, "plugins");

  Py_DECREF(XPY3moduleDict);
  Py_DECREF(XPY3pluginDict);
  
  // Invoke cleanup method of all built-in modules
  char *mods[] = {"XPLMDefs", "XPLMDisplay", "XPLMGraphics", "XPLMUtilities", "XPLMScenery", "XPLMMenus",
                  "XPLMNavigation", "XPLMPlugin", "XPLMPlanes", "XPLMProcessing", "XPLMCamera", "XPWidgetDefs",
                  "XPWidgets", "XPStandardWidgets", "XPUIGraphics", "XPWidgetUtils", "XPLMInstance",
                  "XPLMMap", "XPLMDataAccess", "XPLMWeather", "XPLMSound", /*"SandyBarbourUtilities", */ "XPCursor", "XPPython", NULL};
  char **mod_ptr = mods;

  while(*mod_ptr != NULL){
    PyObject *mod = PyImport_ImportModule(*mod_ptr);
    fflush(stdout);
    if (PyErr_Occurred()) {
      pythonLog("[XPPython3] Failed during stop of internal module %s", *mod_ptr);
      pythonLogException();
      return 1;
    }
      
    if(mod){
      set_moduleName(PyUnicode_FromString(*mod_ptr));
      PyObject *pRes = PyObject_CallMethod(mod, "_cleanup", NULL);
      if (PyErr_Occurred() ) {
        pythonLog("[XPPython3] Failed during cleanup of internal module %s", *mod_ptr);
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
  if (Py_IsInitialized()) {
    XPLMDebugString("[XPPython3] already initialized\n");
    return 0;
  } else if (XPLMFindPluginBySignature("xppython3.main") != XPLM_NO_PLUGIN_ID) {
    XPLMDebugString("[XPPython3] already running. Using that instance instead.\n");
    return 0;
  }
  if (XPLMFindPluginBySignature("sandybarbour.projects.pythoninterface") != XPLM_NO_PLUGIN_ID) {
    XPLMDebugString("[XPPython3] WARNING: XPPython3 Detected python2 PythonInterface plugin. These plugins have compatibility issues.\n");
  }

  if (XPLMHasFeature("XPLM_USE_NATIVE_PATHS")) {
    XPLMEnableFeature("XPLM_USE_NATIVE_PATHS", 1);
  } else {
    XPLMDebugString("[XPPython3] WARNING: XPLM_USE_NATIVE_PATHS not enabled. Using Legacy paths.\n");
  }

  handleConfigFile();

  setLogFile();

  SymStartTime = time(NULL);
  
  pythonLog("[%s] Version %s Started -- %.24s", pythonPluginName, pythonPluginVersion, ctime(&SymStartTime));

  if (loadLocalPythonLibrary() == -1) {return 0;}

  pythonLogFlush();
  strcpy(outName, pythonPluginName);
  strcpy(outSig, pythonPluginSig);
  strcpy(outDesc, pythonPluginDesc);

  if (XPLMHasFeature("XPLM_USE_NATIVE_WIDGET_WINDOWS")) {
    XPLMEnableFeature("XPLM_USE_NATIVE_WIDGET_WINDOWS", 1);
  } else {
    pythonLog("[XPPython3] Warning: XPLM_USE_NATIVE_WIDGET_WINDOWS not enabled. Using Legacy windows.");
  }
  if (XPLMHasFeature("XPLM_WANTS_DATAREF_NOTIFICATIONS")) {
    XPLMEnableFeature("XPLM_WANTS_DATAREF_NOTIFICATIONS", 1);
  }

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
  /*   pythonLog("Total errors encountered: %d", allErrorsEncountered); */
  /* } */

  time_t current_time = time(NULL);
  pythonLog("[%s] Stopped. %.24s", pythonPluginName, ctime(&current_time));
  pythonLogClose();
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
      pythonLog("[XPPython3] Disabled scripts.");
    } else {
      pythonLog("XPPython3: already disabled.");
    }
  }else if(inCommand == enableScripts){
    if (xpy3_disabled) {
      XPluginEnable();
      pythonLog("[XPPython3] Enabled scripts.");
    } else {
      pythonLog("XPPython3: already enabled.");
    }
  }else if(inCommand == reloadScripts){
    if (! xpy3_disabled) {
      time_t current_time = time(NULL);
      pythonLog("[XPPython3] Reloading Scripts.======= %.24s", ctime(&current_time));
      pythonLog("[XPPython3] Reload -- 1) Disable existing scripts.=======");
      xpy_disableInstances();/* Internal, PythonPlugins, scenery, aircraft */
      pythonLog("[XPPython3] Reload -- 2) Stop existing scripts.=======");
      xpy_stopInstances();   /* Internal, PythonPlugins, scenery, aircraft */
      pythonLog("[XPPython3] Reload -- 3) Reset Menu, callbacks and datastructures.=======");
      resetInternals();
      pythonLog("[XPPython3] Reload -- 4) Reload Config file.=======");
      handleConfigFile();
      pythonLog("[XPPython3] Reload -- 5) Determine changed python modules.=======");
      reloadSysModules();
      pythonLog("[XPPython3] Reload -- 6) Restart Internal Python plugins.=======");
      xpy_startInternalInstances(); /* Internal */
      pythonLog("[XPPython3] Reload -- 7) Restart PythonPlugins (and Scenery) plugins.=======");
      xpy_startInstances(EXCLUDE_AIRCRAFT); /* PythonPlugins, scenery. aircraft will be restarted and enabled in enableInstances*/
      pythonLog("[XPPython3] Reload -- 8) Enable PythonPlugins (also starting and enabling Aircraft plugins if exist.=======");
      xpy_enableInstances(); /* Internal, PythonPlugins, scenery, aircraft */
      pythonLog("[XPPython3] Reloaded Scripts.=======");
      xpy3_disabled = false;
    } else {
      pythonLog("XPPython3 is disabled, cannot reload.");
    }
  }
  pythonLogFlush();
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

  PyObject *pluginInfo, *pluginInstance, *pRes, *pModuleName;
  Py_ssize_t pos = 0;
  PyObject *param;
  errCheck("receiveMessage start");
  param = PyLong_FromLong((long)inParam);
  pythonDebug("XPPython3 received message, forwarding to all plugins: From: %d, Msg: %ld, inParam: %ld",
              inFromWho, inMessage, (long)inParam);
  if (inMessage == XPLM_MSG_DATAREFS_ADDED && (long) inParam > 100000) {
    /* bug -- inParam is sent as pointer to value, rather than value itself. We'll
       convert to value and send _that_ to python plugins */
    pythonDebug("...Sending %ld instead. See bug XPD-13931\n", *(long*)inParam);
    if (*(long*)inParam > 100000) {
      pythonLog("Totally bogus value sent with XPLM_MSG_DATAREFS_ADDED (%ld). See bug XPD-13931", (long)inParam);
      /* ... passing original value for param -- what else can I do?  */
    } else {
      param = PyLong_FromLong(*(long*)inParam);
    }
  }

  if (inMessage == XPLM_MSG_PLANE_LOADED && inParam == XPLM_USER_AIRCRAFT) {
    xpy_startAircraftPlugins();
    xpy_enableAircraftPlugins();
  }

  while(PyDict_Next(XPY3moduleDict, &pos, &pModuleName, &pluginInstance)){
    pluginInfo = PyDict_GetItem(XPY3pluginDict, pluginInstance);
    if (PyList_GetItem(pluginInfo, PLUGIN_DISABLED) == Py_True) {
      continue;
    }
    set_moduleName(pModuleName);
    errCheck("before sending to XPluginReceiveMessage");
    pRes = PyObject_CallMethod(pluginInstance, "XPluginReceiveMessage", "ilO", inFromWho, inMessage, param);

    PyObject *err = PyErr_Occurred();
    if (err) {
      if (PyObject_HasAttrString(pluginInstance, "XPluginReceiveMessage")) {
        pythonLog("[%s] Error occured during the XPluginReceiveMessage call:", CurrentPythonModuleName);
        pythonLogException();
        PyErr_SetRaisedException(err);
      } else {
        /* ignore error, if XPluginReceiveMessage is not defined in the PythonInterface class */
        PyErr_Clear();
      }
    } else {
      if (pRes != Py_None) {
        char *s = objToStr(pRes);
        pythonLog("[%s] XPluginReceiveMessage returned '%s' rather than None. Value ignored", CurrentPythonModuleName, s);
        free(s);
      }
      Py_DECREF(pRes);
    }
  }
  errCheck("before sending all XPluginReceiveMessage");
  if (inMessage == XPLM_MSG_SCENERY_LOADED && XPLMGetCycleNumber() != 0){
    /* we'll get SCENERY_LOADED with Cycle# = 0 on startup, but we'll have already loaded the 
       scenery plugins (this matches XP behavior). We ignore if cycle == 0, because we don't want
       to disable and reload them. */
    xpy_disableSceneryPlugins();
    xpy_stopSceneryPlugins();
    xpy_startSceneryPlugins();
    xpy_enableSceneryPlugins();
  }
  if (inMessage == XPLM_MSG_PLANE_UNLOADED && inParam == XPLM_USER_AIRCRAFT && XPLMGetCycleNumber() != 0) {
    xpy_disableAircraftPlugins();
    xpy_stopAircraftPlugins();
  }
  errCheck("finish sending all XPluginReceiveMessage");
  Py_DECREF(param);
}

static int loadLocalPythonLibrary(void)
{
/*
  The _reason_ we have to do this is a python problem which (still) exists on linux (November 2022)
  Essential, the python shared libs (say _ssl.so) don't look within the python libpythonX.X.so shared
  lib, so the imported shared lib will fail with something like:
     ImportError: /usr/lib/python/lib-dynload/_sso.so: undefined symbol: PyEx_ValueError

  See https://mail.python.org/pipermail/new-bugs-announce/2008-November/003322.html
*/
#if APL || LIN
#if APL
  char *suffix = "dylib";
  char *path = "Resources/plugins/XPPython3/mac_x64/python" PYTHONVERSION "/lib/";
#elif LIN
  char *suffix = "so";
  char *path = "Resources/plugins/XPPython3/lin_x64/python" PYTHONVERSION "/lib/";
#endif  
  char library[1024];
  sprintf(library, "%slibpython%s.%s", path, PYTHONVERSION, suffix);
  pythonHandle = dlopen(library, RTLD_LAZY | RTLD_GLOBAL);
  if (!pythonHandle) {
    char cwd[1000];
    if (!getcwd(cwd, sizeof(cwd))) {
      strcpy(cwd, "[unknown current working directory]");
    }
    pythonLog("Unable to find python shared library '%s' from %s: %s", library, cwd, dlerror());
    pythonLogFlush();
    return -1;
  }
  pythonLog("[XPPython3] Python shared library loaded: '%s'", library);
#endif
  return 0;
}

static void setSysPath(void) {
  PyObject *path = PySys_GetObject("path"); //Borrowed!
  PyObject *pathStrObj = NULL, *absolutePathStrObj = NULL, *xPlaneDirObj = NULL;

  char x_plane_dir[512];
  XPLMGetSystemPath(x_plane_dir);
  xPlaneDirObj = PyUnicode_FromString(x_plane_dir);  // new

  /* Resources/plugins */
  pathStrObj = PyUnicode_DecodeUTF8(pythonPluginsPath, (strrchr(pythonPluginsPath, '/') - pythonPluginsPath), NULL); // new
  absolutePathStrObj = PyUnicode_Concat(xPlaneDirObj, pathStrObj);  // new
  PyList_Insert(path, 0, absolutePathStrObj);
  Py_DECREF(pathStrObj);
  Py_DECREF(absolutePathStrObj);

  /* Resources/plugins/XPPython3 */
  pathStrObj = PyUnicode_DecodeUTF8(pythonInternalPluginsPath, strlen(pythonInternalPluginsPath), NULL); // new
  absolutePathStrObj = PyUnicode_Concat(xPlaneDirObj, pathStrObj);  // new
  PyList_Insert(path, 0, absolutePathStrObj);
  Py_DECREF(pathStrObj);
  Py_DECREF(absolutePathStrObj);

  /* Resources/plugins/PythonPlugins */
  pathStrObj = PyUnicode_DecodeUTF8(pythonPluginsPath, strlen(pythonPluginsPath), NULL); // new
  absolutePathStrObj = PyUnicode_Concat(xPlaneDirObj, pathStrObj);  // new
  PyList_Insert(path, 0, absolutePathStrObj);
  Py_DECREF(pathStrObj);
  Py_DECREF(absolutePathStrObj);

  Py_DECREF(xPlaneDirObj);
}

static void handleConfigFile(void) {  /* Find and handle config.ini file */
  XPLMGetPrefsPath(xpy_ini_file);
  XPLMExtractFileAndPath(xpy_ini_file);
#if LIN || APL
  if (strlcat(xpy_ini_file, "/xppython3.ini", 512) >= 512) {
    pythonLog("[XPPython3] Config file path too long");
    return;
  }
#elif IBM
  if (strcat_s(xpy_ini_file, 512, "/xppython3.ini") != 0) {
    pythonLog("[XPPython3] Failed to concatenate config file path");
    return;
  }
#endif
  pythonDebugs = xpy_config_get_int("[Main].debug");
  pythonWarnings = xpy_config_get_int("[Main].warning");
  pythonStats = xpy_config_get_int_default("[Main].stats", 1);
#if !defined(Py_LIMITED_API)
  pythonVerbose = xpy_config_get_int("[Main].py_verbose");/* 0= off, 1= each file as loaded, 2= each file that is checked when searching for module */
#endif
  pythonDebug("Read config file: %s", xpy_ini_file);
  pythonCapsuleRegistration = xpy_config_get_int("[Main].capsule_registrations");  /* 0= off, 1= log every Capsule registration */
}

static void initMtimes(void) {
  PythonModuleMTimes = PyDict_New();
  /* Initials mtime for imports */
  PyObject *localsDict = PyDict_New();
  PyDict_SetItemString(localsDict, "__builtins__", PyEval_GetBuiltins());
  PyDict_SetItemString(localsDict, "mtimes", PythonModuleMTimes);
  MyPyRun_String("import sys\n"
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
  char filename[1024];
  strcpy(filename, pythonInternalHooksPath);
  strcat(filename, "/");
  strcat(filename, "xp_reloader.py");
  FILE *fp = fopen(filename, "rb");  /* python says, use binary mode so newlines work */
  if (!fp) {
    pythonLog("Cannot find reloader script in %s", filename);
    return;
  }

  PyObject *localsDict = PyDict_New();
  PyDict_SetItemString(localsDict, "__builtins__", PyEval_GetBuiltins());
  PyDict_SetItemString(localsDict, "mtimes", PythonModuleMTimes);
  PyDict_SetItemString(localsDict, "sym_start", PyLong_FromLong(SymStartTime));
  /* reload_unknown... if sys.modules has a module for which we don't know the mtime,
   *                   do we reload it anyway? Py_True
   */
  strcpy(CurrentPythonModuleName, "XPPython3");
  PyDict_SetItemString(localsDict, "reload_unknown", Py_True);

  MyPyRun_File(fp, "xp_reloader.py", Py_file_input, localsDict, localsDict);
  fclose(fp);
  PyObject *result = PyDict_GetItemString(localsDict, "result");
  PyObject *err = PyErr_Occurred();
  if (err) {
    pythonLog("[XPPython3] Error occured during the reload of modules.");
    pythonLogException();
  }
  if (PyUnicode_GetLength(result) > 2) {
    char *s = objToStr(result);
    pythonLog(" Module(s) reloaded:  \n  > %s", s);
    free(s);
  }
  Py_DECREF(localsDict);
}
