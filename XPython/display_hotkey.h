#pragma once
#include <Python.h>
#include "XPLM/XPLMDisplay.h"

struct HotKeyCallbackInfo {
  XPLMHotKeyID hotKeyID;
  PyObject *callback;
  PyObject *refCon;
  const char* module_name;
};

extern std::unordered_map<intptr_t, HotKeyCallbackInfo> hotkeyDict;
void resetHotKeyCallbacks(void);
extern PyObject* XPLMRegisterHotKeyFun(PyObject*, PyObject*, PyObject *);
extern PyObject* XPLMUnregisterHotKeyFun(PyObject*, PyObject*, PyObject *);
extern PyObject* XPLMCountHotKeysFun(PyObject*, PyObject*);
extern PyObject* XPLMGetNthHotKeyFun(PyObject*, PyObject*, PyObject *);
extern PyObject* XPLMGetHotKeyInfoFun(PyObject*, PyObject*, PyObject *);
extern PyObject* XPLMSetHotKeyCombinationFun(PyObject*, PyObject*, PyObject *);
