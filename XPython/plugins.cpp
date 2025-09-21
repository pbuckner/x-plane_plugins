#define _GNU_SOURCE 1
#include <Python.h>
#include <sys/time.h>
#include <stdio.h>
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMPlugin.h>
#include <vector>
#include <string>
#include <unordered_map>
#include "utils.h"
#include "xppythontypes.h"

My_DOCSTR(_getMyID__doc__, "getMyID",
          "",
          "",
          "int",
          "Returns plugin ID of calling plugin.\n"
          "\n"
          "For XPPython3, this is *always* the ID of the XPPython3 plugin\n"
          "not any particular python plugin.");
static PyObject *XPLMGetMyIDFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  return PyLong_FromLong(XPLMGetMyID());
}

My_DOCSTR(_countPlugins__doc__, "countPlugins",
          "",
          "",
          "int",
          "Return total number of (non-python) plugins");
static PyObject *XPLMCountPluginsFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  return PyLong_FromLong(XPLMCountPlugins());
}

My_DOCSTR(_getNthPlugin__doc__, "getNthPlugin",
          "index",
          "index:int",
          "XPLMPluginID",
          "Return the ID of a (non-python) plugin by index.");
static PyObject *XPLMGetNthPluginFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("index"), nullptr};
  (void) self;
  int inIndex;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "i", keywords, &inIndex)){
    return nullptr;
  }
  return PyLong_FromLong(XPLMGetNthPlugin(inIndex));
}

My_DOCSTR(_findPluginByPath__doc__, "findPluginByPath",
          "path",
          "path:str",
          "XPLMPluginID",
          "Return pluginID of (non-python) plugin whose file exists at path.\n"
          "\n"
          "Path must be absolute.");
static PyObject *XPLMFindPluginByPathFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("path"), nullptr};
  (void) self;
  const char *inPath;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "s", keywords, &inPath)){
    return nullptr;
  }
  return PyLong_FromLong(XPLMFindPluginByPath(inPath));
}

My_DOCSTR(_findPluginBySignature__doc__, "findPluginBySignature",
          "signature",
          "signature:str",
          "XPLMPluginID",
          "Return the pluginID of the (non-python) plugin whose signature matches.");
static PyObject *XPLMFindPluginBySignatureFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("signature"), nullptr};
  (void) self;
  const char *inSignature;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "s", keywords, &inSignature)){
    return nullptr;
  }
  return PyLong_FromLong(XPLMFindPluginBySignature(inSignature));
}

My_DOCSTR(_getPluginInfo__doc__, "getPluginInfo",
          "pluginID",
          "pluginID:XPLMPluginID",
          "PluginInfo",
          "Return information about plugin.\n"
          "\n"
          "Return value is an object with attributes:\n"
          "  .name\n"
          "  .filePath\n"
          "  .signature\n"
          "  .description");
static PyObject *XPLMGetPluginInfoFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("pluginID"), nullptr};
  (void) self;
  int inPluginID;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "i", keywords, &inPluginID)){
    return nullptr;
  }
  char name[512];
  char filePath[512];
  char signature[512];
  char description[512];
  XPLMGetPluginInfo(inPluginID, name, filePath, signature, description);
  return PyPluginInfo_New(name, filePath, signature, description);
}

My_DOCSTR(_isPluginEnabled__doc__, "isPluginEnabled",
          "pluginID",
          "pluginID:XPLMPluginID",
          "int",
          "Return 1 if plugin is enabled, 0 otherwise");
static PyObject *XPLMIsPluginEnabledFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("pluginID"), nullptr};
  (void) self;
  int inPluginID;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "i", keywords, &inPluginID)){
    return nullptr;
  }
  return PyLong_FromLong(XPLMIsPluginEnabled(inPluginID));
}

My_DOCSTR(_enablePlugin__doc__, "enablePlugin",
          "pluginID",
          "pluginID:XPLMPluginID",
          "int",
          "Enables plugin.");
static PyObject *XPLMEnablePluginFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("pluginID"), nullptr};
  (void) self;
  int inPluginID;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "i", keywords, &inPluginID)){
    return nullptr;
  }
  return PyLong_FromLong(XPLMEnablePlugin(inPluginID));
}

My_DOCSTR(_disablePlugin__doc__, "disablePlugin",
          "pluginID",
          "pluginID:XPLMPluginID",
          "None",
          "Disables plugin");
static PyObject *XPLMDisablePluginFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("pluginID"), nullptr};
  (void) self;
  int inPluginID;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "i", keywords, &inPluginID)){
    return nullptr;
  }
  XPLMDisablePlugin(inPluginID);
  Py_RETURN_NONE;
}

My_DOCSTR(_reloadPlugins__doc__, "reloadPlugins",
          "",
          "",
          "None",
          "Reload *all* plugins.\n"
          "\n"
          "Likely crashes the sim. DO NOT USE.");
static PyObject *XPLMReloadPluginsFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  XPLMReloadPlugins();
  Py_RETURN_NONE;
}

My_DOCSTR(_sendMessageToPlugin__doc__, "sendMessageToPlugin",
          "pluginID, message, param=None",
          "pluginID:XPLMPluginID, message:int, param:Optional[Any]",
          "None",
          "Send message to plugin\n"
          "\n"
          "Messages sent to XPPython3 plugin will be forwarded to all\n"
          "python plugins.");
static PyObject *XPLMSendMessageToPluginFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("pluginID"), CHAR("message"), CHAR("param"), nullptr};
  (void) self;
  long inPluginID;
  long inMessage;
  PyObject* inParam = Py_None;
  PyObject *err;
  err = PyErr_Occurred();
  if (err) {
    pythonLog("Error prior to send message to plugin");
    pythonLogException();
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "ll|O", keywords, &inPluginID, &inMessage, &inParam)){
    return nullptr;
  }
  void *msgParam;
  if (inParam == Py_None) {
    msgParam = nullptr;
    XPLMSendMessageToPlugin(inPluginID, inMessage, msgParam);
  } else if (PyLong_Check(inParam)) {
    msgParam = PyLong_AsVoidPtr(inParam);
    XPLMSendMessageToPlugin(inPluginID, inMessage, msgParam);
  } else if (PyUnicode_Check(inParam)) {
    msgParam = objToStr(inParam);
    XPLMSendMessageToPlugin(inPluginID, inMessage, msgParam);
    free(msgParam);
  } else {
    char *s = objToStr(PyObject_Type(inParam));
    pythonLog("Unknown data type %s for XPLMSendMessageToPlugin(... inParam). Cannot convert", s);
    free(s);
  }
  err = PyErr_Occurred();
  if (err) {
    pythonLog("Error at end of send message to plugin");
    pythonLogException();
  }
  Py_RETURN_NONE;
}

My_DOCSTR(_hasFeature__doc__, "hasFeature",
          "feature",
          "feature:str",
          "int",
          "Return 1 if X-Plane supports feature.");
static PyObject *XPLMHasFeatureFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("feature"), nullptr};
  (void) self;
  const char *inFeature;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "s", keywords, &inFeature)){
    return nullptr;
  }
  return PyLong_FromLong(XPLMHasFeature(inFeature));
}

My_DOCSTR(_isFeatureEnabled__doc__, "isFeatureEnabled",
          "feature",
          "feature:str",
          "int",
          "Returns 1 if feature is currently enabled for your plugin.");
static PyObject *XPLMIsFeatureEnabledFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("feature"), nullptr};
  (void) self;
  const char *inFeature;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "s", keywords, &inFeature)){
    return nullptr;
  }
  return PyLong_FromLong(XPLMIsFeatureEnabled(inFeature));
}

My_DOCSTR(_enableFeature__doc__, "enableFeature",
          "feature, enable=1",
          "feature:str, enable:int=1",
          "None",
          "Enables / disables indicated feature for this plugin.");
static PyObject *XPLMEnableFeatureFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("feature"), CHAR("enable"), nullptr};
  (void) self;
  const char *inFeature;
  int inEnable=1;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "s|i", keywords, &inFeature, &inEnable)){
    return nullptr;
  }
  if (!inEnable && ! (strcmp(inFeature, "XPLM_USE_NATIVE_PATHS") &&
                      strcmp(inFeature, "XPLM_USE_NATIVE_WIDGET_WINDOWS") &&
                      strcmp(inFeature, "XPLM_WANTS_DATAREF_NOTIFICATIONS"))) {
    PyErr_SetString(PyExc_RuntimeError, "An XPPython3 plugin is attempting to disable NATIVE_PATHS, NATIVE_WIDGET_WINDOWS or DATAREF_NOTIFICATIONS feature, this is not allowed");
  } else {
    XPLMEnableFeature(inFeature, inEnable);
  }
  Py_RETURN_NONE;
}

struct FeatureDict {
  PyObject *callback;
  PyObject *refCon;
  std::string module_name;
};

static std::unordered_map<intptr_t, FeatureDict> featureCallbacks;
static intptr_t feCntr;

static void featureEnumerator(const char *inFeature, void *inRef)
{
  intptr_t refcon_id = (intptr_t)inRef;
  auto it = featureCallbacks.find(refcon_id);
  if (it == featureCallbacks.end()) {
    printf("Unknown feature enumeration callback requested! (inFeature = '%s' inRef = %p)\n", inFeature, inRef);
    return;
  }

  FeatureDict& info = it->second;
  set_moduleName(info.module_name);

  PyObject *inFeatureObj = PyUnicode_FromString(inFeature);
  PyObject *res = PyObject_CallFunctionObjArgs(info.callback, inFeatureObj, info.refCon, nullptr);
  PyObject *err = PyErr_Occurred();
  Py_DECREF(inFeatureObj);
  if(err){
    printf("Error occured during the feature enumeration callback(inFeature = '%s' inRef = %p):\n", inFeature, inRef);
    pythonLogException();
  }
  Py_XDECREF(res);
}


My_DOCSTR(_enumerateFeatures__doc__, "enumerateFeatures",
          "enumerator, refCon=None",
          "enumerator:Callable[[str, Any], None], refCon:Any=None",
          "None",
          "Enumerate supported features\n"
          "\n"
          "You callback takes (name, refCon) as parameters");
static PyObject *XPLMEnumerateFeaturesFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("enumerator"), CHAR("refCon"), nullptr};
  (void) self;
  PyObject *fun;
  PyObject *refCon=Py_None;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|O", keywords, &fun, &refCon)) {
    return nullptr;
  }
  intptr_t refcon = ++feCntr;
  featureCallbacks[refcon] = {
    .callback = fun,
    .refCon = refCon,
    .module_name = std::string(CurrentPythonModuleName)
  };
  Py_INCREF(fun);
  Py_INCREF(refCon);
  XPLMEnumerateFeatures(featureEnumerator, (void*)refcon);
  Py_RETURN_NONE;
}

static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  for (auto& pair : featureCallbacks) {
    Py_DECREF(pair.second.callback);
    Py_DECREF(pair.second.refCon);
  }
  featureCallbacks.clear();
  Py_RETURN_NONE;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
static PyMethodDef XPLMPluginMethods[] = {
  {"getMyID", (PyCFunction)XPLMGetMyIDFun, METH_VARARGS, _getMyID__doc__},
  {"XPLMGetMyID", (PyCFunction)XPLMGetMyIDFun, METH_VARARGS, ""},
  {"countPlugins", (PyCFunction)XPLMCountPluginsFun, METH_VARARGS, _countPlugins__doc__},
  {"XPLMCountPlugins", (PyCFunction)XPLMCountPluginsFun, METH_VARARGS, ""},
  {"getNthPlugin", (PyCFunction)XPLMGetNthPluginFun, METH_VARARGS | METH_KEYWORDS, _getNthPlugin__doc__},
  {"XPLMGetNthPlugin", (PyCFunction)XPLMGetNthPluginFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"findPluginByPath", (PyCFunction)XPLMFindPluginByPathFun, METH_VARARGS | METH_KEYWORDS, _findPluginByPath__doc__},
  {"XPLMFindPluginByPath", (PyCFunction)XPLMFindPluginByPathFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"findPluginBySignature", (PyCFunction)XPLMFindPluginBySignatureFun, METH_VARARGS | METH_KEYWORDS, _findPluginBySignature__doc__},
  {"XPLMFindPluginBySignature", (PyCFunction)XPLMFindPluginBySignatureFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"getPluginInfo", (PyCFunction)XPLMGetPluginInfoFun, METH_VARARGS | METH_KEYWORDS, _getPluginInfo__doc__},
  {"XPLMGetPluginInfo", (PyCFunction)XPLMGetPluginInfoFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"isPluginEnabled", (PyCFunction)XPLMIsPluginEnabledFun, METH_VARARGS | METH_KEYWORDS, _isPluginEnabled__doc__},
  {"XPLMIsPluginEnabled", (PyCFunction)XPLMIsPluginEnabledFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"enablePlugin", (PyCFunction)XPLMEnablePluginFun, METH_VARARGS | METH_KEYWORDS, _enablePlugin__doc__},
  {"XPLMEnablePlugin", (PyCFunction)XPLMEnablePluginFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"disablePlugin", (PyCFunction)XPLMDisablePluginFun, METH_VARARGS | METH_KEYWORDS, _disablePlugin__doc__},
  {"XPLMDisablePlugin", (PyCFunction)XPLMDisablePluginFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"reloadPlugins", (PyCFunction)XPLMReloadPluginsFun, METH_VARARGS, _reloadPlugins__doc__},
  {"XPLMReloadPlugins", (PyCFunction)XPLMReloadPluginsFun, METH_VARARGS, ""},
  {"sendMessageToPlugin", (PyCFunction)XPLMSendMessageToPluginFun, METH_VARARGS | METH_KEYWORDS, _sendMessageToPlugin__doc__},
  {"XPLMSendMessageToPlugin", (PyCFunction)XPLMSendMessageToPluginFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"hasFeature", (PyCFunction)XPLMHasFeatureFun, METH_VARARGS | METH_KEYWORDS, _hasFeature__doc__},
  {"XPLMHasFeature", (PyCFunction)XPLMHasFeatureFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"isFeatureEnabled", (PyCFunction)XPLMIsFeatureEnabledFun, METH_VARARGS | METH_KEYWORDS, _isFeatureEnabled__doc__},
  {"XPLMIsFeatureEnabled", (PyCFunction)XPLMIsFeatureEnabledFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"enableFeature", (PyCFunction)XPLMEnableFeatureFun, METH_VARARGS | METH_KEYWORDS, _enableFeature__doc__},
  {"XPLMEnableFeature", (PyCFunction)XPLMEnableFeatureFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"enumerateFeatures", (PyCFunction)XPLMEnumerateFeaturesFun, METH_VARARGS | METH_KEYWORDS, _enumerateFeatures__doc__},
  {"XPLMEnumerateFeatures", (PyCFunction)XPLMEnumerateFeaturesFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"_cleanup", cleanup, METH_VARARGS, ""},
  {nullptr, nullptr, 0, nullptr}
};
#pragma GCC diagnostic pop


static struct PyModuleDef XPLMPluginModule = {
  PyModuleDef_HEAD_INIT,
  "XPLMPlugin",
  "Laminar documentation: \n"
  "   https://developer.x-plane.com/sdk/XPLMPlugin/\n"
  "XPPython3 documentation: \n"
  "   https://xppython3.rtfd.io/en/stable/development/modules/plugin.html",
  -1,
  XPLMPluginMethods,
  nullptr,
  nullptr,
  nullptr,
  nullptr
};

PyMODINIT_FUNC
PyInit_XPLMPlugin(void)
{
  PyObject *mod = PyModule_Create(&XPLMPluginModule);
  if(mod){
    PyModule_AddStringConstant(mod, "__author__", "Peter Buckner (pbuck@xppython3.org)");
    PyModule_AddIntConstant(mod, "XPLM_MSG_PLANE_CRASHED", XPLM_MSG_PLANE_CRASHED);
    PyModule_AddIntConstant(mod, "XPLM_MSG_PLANE_LOADED", XPLM_MSG_PLANE_LOADED);
    PyModule_AddIntConstant(mod, "XPLM_MSG_AIRPORT_LOADED", XPLM_MSG_AIRPORT_LOADED);
    PyModule_AddIntConstant(mod, "XPLM_MSG_SCENERY_LOADED", XPLM_MSG_SCENERY_LOADED);
    PyModule_AddIntConstant(mod, "XPLM_MSG_AIRPLANE_COUNT_CHANGED", XPLM_MSG_AIRPLANE_COUNT_CHANGED);
    PyModule_AddIntConstant(mod, "XPLM_MSG_PLANE_UNLOADED", XPLM_MSG_PLANE_UNLOADED);
    PyModule_AddIntConstant(mod, "XPLM_MSG_WILL_WRITE_PREFS", XPLM_MSG_WILL_WRITE_PREFS);
    PyModule_AddIntConstant(mod, "XPLM_MSG_LIVERY_LOADED", XPLM_MSG_LIVERY_LOADED);
    PyModule_AddIntConstant(mod, "XPLM_MSG_ENTERED_VR", XPLM_MSG_ENTERED_VR);
    PyModule_AddIntConstant(mod, "XPLM_MSG_EXITING_VR", XPLM_MSG_EXITING_VR);
    PyModule_AddIntConstant(mod, "XPLM_MSG_RELEASE_PLANES", XPLM_MSG_RELEASE_PLANES);

    PyModule_AddIntConstant(mod, "MSG_PLANE_CRASHED", XPLM_MSG_PLANE_CRASHED);
    PyModule_AddIntConstant(mod, "MSG_PLANE_LOADED", XPLM_MSG_PLANE_LOADED);
    PyModule_AddIntConstant(mod, "MSG_AIRPORT_LOADED", XPLM_MSG_AIRPORT_LOADED);
    PyModule_AddIntConstant(mod, "MSG_SCENERY_LOADED", XPLM_MSG_SCENERY_LOADED);
    PyModule_AddIntConstant(mod, "MSG_AIRPLANE_COUNT_CHANGED", XPLM_MSG_AIRPLANE_COUNT_CHANGED);
    PyModule_AddIntConstant(mod, "MSG_PLANE_UNLOADED", XPLM_MSG_PLANE_UNLOADED);
    PyModule_AddIntConstant(mod, "MSG_WILL_WRITE_PREFS", XPLM_MSG_WILL_WRITE_PREFS);
    PyModule_AddIntConstant(mod, "MSG_LIVERY_LOADED", XPLM_MSG_LIVERY_LOADED);
    PyModule_AddIntConstant(mod, "MSG_ENTERED_VR", XPLM_MSG_ENTERED_VR);
    PyModule_AddIntConstant(mod, "MSG_EXITING_VR", XPLM_MSG_EXITING_VR);
    PyModule_AddIntConstant(mod, "MSG_RELEASE_PLANES", XPLM_MSG_RELEASE_PLANES);

    PyModule_AddIntConstant(mod, "MsgPlaneCrashed", XPLM_MSG_PLANE_CRASHED);
    PyModule_AddIntConstant(mod, "MsgPlaneLoaded", XPLM_MSG_PLANE_LOADED);
    PyModule_AddIntConstant(mod, "MsgAirportLoaded", XPLM_MSG_AIRPORT_LOADED);
    PyModule_AddIntConstant(mod, "MsgSceneryLoaded", XPLM_MSG_SCENERY_LOADED);
    PyModule_AddIntConstant(mod, "MsgAirplaneCountChanged", XPLM_MSG_AIRPLANE_COUNT_CHANGED);
    PyModule_AddIntConstant(mod, "MsgPlaneUnloaded", XPLM_MSG_PLANE_UNLOADED);
    PyModule_AddIntConstant(mod, "MsgWillWritePrefs", XPLM_MSG_WILL_WRITE_PREFS);
    PyModule_AddIntConstant(mod, "MsgLivery_Loaded", XPLM_MSG_LIVERY_LOADED);
    PyModule_AddIntConstant(mod, "MsgEnteredVr", XPLM_MSG_ENTERED_VR);
    PyModule_AddIntConstant(mod, "MsgExitingVr", XPLM_MSG_EXITING_VR);
    PyModule_AddIntConstant(mod, "MsgReleasePlanes", XPLM_MSG_RELEASE_PLANES);
#if defined(XPLM400)    
    PyModule_AddIntConstant(mod, "MSG_FMOD_BANK_LOADED", XPLM_MSG_FMOD_BANK_LOADED);
    PyModule_AddIntConstant(mod, "MSG_FMOD_BANK_UNLOADING", XPLM_MSG_FMOD_BANK_UNLOADING);
    PyModule_AddIntConstant(mod, "MSG_DATAREFS_ADDED", XPLM_MSG_DATAREFS_ADDED);

    PyModule_AddIntConstant(mod, "MsgFmodBankLoaded", XPLM_MSG_FMOD_BANK_LOADED);
    PyModule_AddIntConstant(mod, "MsgFmodBankUnloading", XPLM_MSG_FMOD_BANK_UNLOADING);
    PyModule_AddIntConstant(mod, "MsgDatarefs_Added", XPLM_MSG_DATAREFS_ADDED);
#else
    PyModule_AddIntConstant(mod, "MSG_FMOD_BANK_LOADED", -1);
    PyModule_AddIntConstant(mod, "MSG_FMOD_BANK_UNLOADING", -1);
    PyModule_AddIntConstant(mod, "MSG_DATAREFS_ADDED", -1);

    PyModule_AddIntConstant(mod, "MsgFmodBankLoaded", -1);
    PyModule_AddIntConstant(mod, "MsgFmodBankUnloading", -1);
    PyModule_AddIntConstant(mod, "MsgDatarefsAdded", -1);
#endif
  }
  return mod;
}

