#pragma once

struct KeySnifferCallbackInfo {
  const char* module_name;
  PyObject *callback;
  int before;
  PyObject *refCon;
};

extern std::unordered_map<intptr_t, KeySnifferCallbackInfo> keySnifferCallbackDict;
void resetKeySnifferCallbacks(void);
PyObject *XPLMRegisterKeySnifferFun(PyObject*, PyObject*, PyObject*);
PyObject *XPLMUnregisterKeySnifferFun(PyObject*, PyObject*, PyObject*);
