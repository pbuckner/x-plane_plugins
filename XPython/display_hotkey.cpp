#include <Python.h>
#include <unordered_map>
#include <XPLM/XPLMDisplay.h>
#include "utils.h"
#include "display_hotkey.h"
#include "capsules.h"
#include "xppythontypes.h"

static intptr_t hotkeyCounter = 0;
std::unordered_map<intptr_t, HotKeyCallbackInfo> hotkeyDict;
static void genericHotkeyCallback(void *inRefcon);

void resetHotKeyCallbacks(void) {
  errCheck("prior resethotkey");

  for (const auto& pair : hotkeyDict) {
    char *callback = objToStr(pair.second.callback);
    XPLMUnregisterHotKey((XPLMHotKeyID) pair.second.hotKeyID);
    deleteCapsuleByPtr((void*)pair.first, "XPLMHotKeyID");
    pythonDebug("     Reset --     %s - (%s)", pair.second.module_name, callback);
    Py_DECREF(pair.second.refCon);
    Py_DECREF(pair.second.callback);
    free(callback);
  }
  hotkeyDict.clear();
  errCheck("post while resethotkey");

  errCheck("post reset hotkey");
}

static void genericHotkeyCallback(void *inRefcon)
{
  errCheck("prior hotkeyCallback");
  intptr_t id = (intptr_t) inRefcon;
  auto it = hotkeyDict.find(id);
  if(it == hotkeyDict.end()){
    pythonLog("Unknown refcon passed to hotkeyCallback (%p).", inRefcon);
    return;
  }
  set_moduleName(it->second.module_name);
  PyObject *args[] = {it->second.refCon};
    PyObject *res = PyObject_Vectorcall(it->second.callback, args, 1, nullptr);
  PyObject *err = PyErr_Occurred();
  if(err){
    pythonDebug("exception in hotkey callback\n");
    pythonLogException();
  }
  Py_XDECREF(res);  // in case hotkey doesn't happen to return anything
  errCheck("end hotkeyCallback");
}

PyObject *XPLMRegisterHotKeyFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  errCheck("prior registerHotKey");
  static char *keywords[] = {CHAR("vkey"), CHAR("flags"), CHAR("description"), CHAR("hotKey"), CHAR("refCon"), nullptr};
  (void) self;
  PyObject *inCallback = Py_None, *refcon = Py_None;
  int inVirtualKey, inFlags = 0;
  const char *inDescription;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "i|isOO", keywords, &inVirtualKey, &inFlags, &inDescription, &inCallback, &refcon)){
    return nullptr;
  }
  if (!PyCallable_Check(inCallback)) {
    PyErr_SetString(PyExc_ValueError ,"hotKey() not callable.\n");
    return nullptr;
  }

  // ( it appears this call cannot fail... you'll always get and ID )
  XPLMHotKeyID id = XPLMRegisterHotKey(inVirtualKey, inFlags, inDescription, genericHotkeyCallback, (void*)hotkeyCounter);
  
  //Store the callback and original refcon
  Py_INCREF(inCallback);
  Py_INCREF(refcon);
  hotkeyDict[hotkeyCounter++] = {
    .hotKeyID = id,
    .callback = inCallback,
    .refCon = refcon,
    .module_name = CurrentPythonModuleName
  };

  PyObject *hkIDCapsule = makeCapsule(id, "XPLMHotKeyID");

  errCheck("end registerHotKey");
  PyObject *err = PyErr_Occurred();
  if (err) {
    pythonDebug("Error at end of registerHotKey\n");
    pythonLogException();
  }
  return hkIDCapsule;
} 

PyObject *XPLMUnregisterHotKeyFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("hotKey"), nullptr};
  (void) self;
  PyObject *hkIDCapsule;
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &hkIDCapsule)){
    return nullptr;
  }

  XPLMHotKeyID hotKeyID = getVoidPtr(hkIDCapsule, "XPLMHotKeyID");

  int found = 0;
  for(auto it = hotkeyDict.begin(); it != hotkeyDict.end(); ) {
    HotKeyCallbackInfo& info = it->second;
    if (info.hotKeyID == hotKeyID) {
      found = 1;
      XPLMUnregisterHotKey(hotKeyID);
      Py_DECREF(info.callback);
      Py_DECREF(info.refCon);
      deleteCapsule(hkIDCapsule);
      it = hotkeyDict.erase(it);
      break;
    } else {
      ++ it;
    }
  }

  if (found == 0) {
    PyErr_SetString(PyExc_RuntimeError ,"XPLMUnregisterHotKey couldn't find hotkey ID.\n");
    return nullptr;
  }

  Py_RETURN_NONE;
} 

PyObject *XPLMCountHotKeysFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  return PyLong_FromLong(XPLMCountHotKeys());
} 

PyObject *XPLMGetNthHotKeyFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("index"), nullptr};
  (void) self;
  int inIndex;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "i", keywords, &inIndex)){
    return nullptr;
  }
  return makeCapsule(XPLMGetNthHotKey(inIndex), "XPLMHotKeyID");
} 

PyObject *XPLMGetHotKeyInfoFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  (void) self;
  PyObject *hotKey, *outVirtualKey, *outFlags, *outDescription, *outPlugin;
  int returnValues = 0;
  static char *keywords[] = {CHAR("hotKeyID"), CHAR("vKey"), CHAR("flags"), CHAR("description"), CHAR("plugin"), nullptr};
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "OOOOO", keywords, &hotKey, &outVirtualKey, &outFlags, &outDescription, &outPlugin)) {
    PyErr_Clear();
    returnValues = 1;
    static char *nkeywords[] = {CHAR("hotKeyID"), nullptr};
    if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", nkeywords, &hotKey)) {
      return nullptr;
    }
  }
  XPLMHotKeyID inHotKey = getVoidPtr(hotKey, "XPLMHotKeyID");
  char virtualKey;
  XPLMKeyFlags flags;
  char description[1024];
  XPLMPluginID plugin;
  XPLMGetHotKeyInfo(inHotKey, &virtualKey, &flags, description, &plugin);
  if (returnValues) {
    return PyHotKeyInfo_New(virtualKey, flags, description, plugin);
  }
  pythonLogWarning("getHotKeyInfo only required initial hotKeyID parameter");
  if (outVirtualKey != Py_None) {
    PyObject *obj = PyLong_FromLong((unsigned int)virtualKey);
    PyList_Append(outVirtualKey, obj);
    Py_DECREF(obj);
  }
  if (outFlags != Py_None) {
    PyObject *obj = PyLong_FromLong((unsigned int)flags);
    PyList_Append(outFlags, obj);
    Py_DECREF(obj);
  }
  if (outDescription != Py_None) {
    PyObject *obj = PyUnicode_FromString(description);
    PyList_Append(outDescription, obj);
    Py_DECREF(obj);
  }
  if (outPlugin != Py_None) {
    PyObject *obj = PyLong_FromLong((unsigned int)plugin);
    PyList_Append(outPlugin, obj);
    Py_DECREF(obj);
  }
  Py_RETURN_NONE;
} 

PyObject *XPLMSetHotKeyCombinationFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("hotKey"), CHAR("vKey"), CHAR("flags"), nullptr};
  (void) self;
  PyObject *hotKey;
  int inVirtualKey;
  int inFlags = 0;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Oi|i", keywords, &hotKey, &inVirtualKey, &inFlags)){
    return nullptr;
  }
  void *inHotkey = getVoidPtr(hotKey, "XPLMHotKeyID");
  XPLMSetHotKeyCombination(inHotkey, (char)inVirtualKey, inFlags);
  Py_RETURN_NONE;
}


/* ---------------------------------------------------------------------------
   Method-table fragment merged into the XPLMDisplay module by
   PyInit_XPLMDisplay() in display.cpp. Docstrings live here, beside the
   functions they document.
   --------------------------------------------------------------------------- */
My_DOCSTR(_registerHotKey__doc__, "registerHotKey",
          "vkey, flags=0, description='', hotKey=None, refCon=None",
          "vkey:int, flags:XPLMKeyFlags=NoFlag, description:str='', "
          "hotKey:Optional[Callable[[Any], None]]=None, "
          "refCon:Any=None",
          "XPLMHotKeyID",
          "Registers hot key."
          "\n"
          "Callback is hotKey(refCon), it does not need to return anything.\n"
          "Registration returns a hotKeyID, which can be used with unregisterHotKey()");

My_DOCSTR(_unregisterHotKey__doc__, "unregisterHotKey",
          "hotKeyID",
          "hotKeyID:XPLMHotKeyID",
          "None",
          "Unregisters hot key associated with hotKeyID.\n"
          "\n"
          "hotKeyID must be registered to this plugin using registerHotKey()\n"
          "otherwise unregistration will fail.");

My_DOCSTR(_countHotKeys__doc__, "countHotKeys",
          "",
          "",
          "int",
          "Return number of hot keys currently defined in the simulator.");

My_DOCSTR(_getNthHotKey__doc__, "getNthHotKey",
          "index",
          "index:int",
          "XPLMHotKeyID",
          "Return hotKeyID for (zero-based) Nth hot key defined in sim.");

My_DOCSTR(_getHotKeyInfo__doc__, "getHotKeyInfo",
          "hotKeyID",
          "hotKeyID:XPLMHotKeyID",
          "None | HotKeyInfo",
          "Return object with hot key information.\n"
          "\n"
          "  .description\n"
          "  .virtualKey\n"
          "  .flags\n"
          "  .plugin");

My_DOCSTR(_setHotKeyCombination__doc__, "setHotKeyCombination",
          "hotKeyID, vKey, flags=0",
          "hotKeyID:XPLMHotKeyID, vKey:int, flags:XPLMKeyFlags=NoFlag",
          "None",
          "Update key combination for given hotKeyID to use vKey and flags");

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
PyMethodDef displayHotkeyMethods[] = {
  {"registerHotKey", (PyCFunction)XPLMRegisterHotKeyFun, METH_VARARGS | METH_KEYWORDS, _registerHotKey__doc__},
  {"XPLMRegisterHotKey", (PyCFunction)XPLMRegisterHotKeyFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"unregisterHotKey", (PyCFunction)XPLMUnregisterHotKeyFun, METH_VARARGS | METH_KEYWORDS, _unregisterHotKey__doc__},
  {"XPLMUnregisterHotKey", (PyCFunction)XPLMUnregisterHotKeyFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"countHotKeys", (PyCFunction)XPLMCountHotKeysFun, METH_VARARGS, _countHotKeys__doc__},
  {"XPLMCountHotKeys", (PyCFunction)XPLMCountHotKeysFun, METH_VARARGS, ""},
  {"getNthHotKey", (PyCFunction)XPLMGetNthHotKeyFun, METH_VARARGS | METH_KEYWORDS, _getNthHotKey__doc__},
  {"XPLMGetNthHotKey", (PyCFunction)XPLMGetNthHotKeyFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"getHotKeyInfo", (PyCFunction)XPLMGetHotKeyInfoFun, METH_VARARGS | METH_KEYWORDS, _getHotKeyInfo__doc__},
  {"XPLMGetHotKeyInfo", (PyCFunction)XPLMGetHotKeyInfoFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"setHotKeyCombination", (PyCFunction)XPLMSetHotKeyCombinationFun, METH_VARARGS | METH_KEYWORDS, _setHotKeyCombination__doc__},
  {"XPLMSetHotKeyCombination", (PyCFunction)XPLMSetHotKeyCombinationFun, METH_VARARGS | METH_KEYWORDS, ""},
  {nullptr, nullptr, 0, nullptr}
};
#pragma GCC diagnostic pop
