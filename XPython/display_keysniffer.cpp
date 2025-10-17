#define _GNU_SOURCE 1
#include <Python.h>
#include <string>
#include <unordered_map>
#include <XPLM/XPLMDisplay.h>
#include "utils.h"
#include "display_keysniffer.h"

static intptr_t keySnifferCallbackCntr;

std::unordered_map<intptr_t, KeySnifferCallbackInfo> keySnifferCallbackDict;
#define KEYSNIFFER_MODULE_NAME 0
#define KEYSNIFFER_CALLBACK 1
#define KEYSNIFFER_BEFORE 2
#define KEYSNIFFER_REFCON 3


static int genericKeySnifferCallback(char inChar, XPLMKeyFlags inFlags, char inVirtualKey, void *inRefcon);

void resetKeySnifferCallbacks(void) {
  for (const auto& pair : keySnifferCallbackDict) {
    char *callback = objToStr(pair.second.callback);
    pythonDebug("     Reset --     %s - (%s)", pair.second.module_name, callback);
    free(callback);

    XPLMUnregisterKeySniffer(genericKeySnifferCallback,
                             pair.second.before,
                             (void*)pair.first);
    Py_DECREF(pair.second.callback);
    Py_DECREF(pair.second.refCon);
  }

  keySnifferCallbackDict.clear();
}

PyObject *XPLMRegisterKeySnifferFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("sniffer"), CHAR("before"), CHAR("refCon"), nullptr};

  errCheck("before registerKeySniffer");
  (void) self;
  PyObject *callback, *refcon = Py_None;
  int inBeforeWindows=0;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|iO", keywords, &callback, &inBeforeWindows, &refcon)) {
    return nullptr;
  }
    
  intptr_t idx = ++keySnifferCallbackCntr;

  int res = XPLMRegisterKeySniffer(genericKeySnifferCallback, inBeforeWindows, (void *)idx);
  if(!res){
    PyErr_SetString(PyExc_RuntimeError ,"registerKeySniffer failed.\n");
    return nullptr;
  }

  Py_INCREF(callback);
  Py_INCREF(refcon);
  keySnifferCallbackDict[idx] = {
    .module_name = CurrentPythonModuleName,
    .callback = callback,
    .before = inBeforeWindows,
    .refCon = refcon
  };
  errCheck("at end registerKeySniffer");
  return PyLong_FromLong(res);
}


PyObject *XPLMUnregisterKeySnifferFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("sniffer"), CHAR("before"), CHAR("refCon"), nullptr};
  (void) self;
  PyObject *callback, *refcon = Py_None;
  int inBeforeWindows = 0;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|iO", keywords, &callback, &inBeforeWindows, &refcon)) {
    return nullptr;
  }

  int res = -1;
  intptr_t foundKey = 0;

  for (auto it = keySnifferCallbackDict.begin(); it != keySnifferCallbackDict.end(); ) {
    KeySnifferCallbackInfo& info = it->second;
    if(info.before == inBeforeWindows
       && 0 == strcmp(info.module_name, CurrentPythonModuleName)
       && PyObject_RichCompareBool(info.callback, callback, Py_EQ) == 1
       && PyObject_RichCompareBool(info.refCon, refcon, Py_EQ) == 1) {
      foundKey = it->first;
      res = XPLMUnregisterKeySniffer(genericKeySnifferCallback,
                                     inBeforeWindows, (void*)it->first);
      Py_DECREF(info.callback);
      Py_DECREF(info.refCon);
      it = keySnifferCallbackDict.erase(it);
      break;
    } else {
      ++it;
    }
  }

  if(foundKey == 0){
    char *s = objToStr(callback);
    pythonLog("Failed to find keySnifferCallback entry for %s %s", CurrentPythonModuleName, s);
    free(s);
  }    
  PyObject *err = PyErr_Occurred();
  if(err){
    pythonLog("Error occured during the XPLMUnregisterKeySnifferCallback call:");
    pythonLogException();
  }
  return PyLong_FromLong(res);
}


int genericKeySnifferCallback(char inChar, XPLMKeyFlags inFlags, char inVirtualKey, void *inRefcon)
{
  int res = 1;
  intptr_t refcon_id = (intptr_t)inRefcon;
  PyObject *pRes = nullptr;
  
  auto it = keySnifferCallbackDict.find(refcon_id);
  if (it == keySnifferCallbackDict.end()) {
    pythonLog("keySninfferCallback, got unknown inRefcon (%p)!", inRefcon);
    return res;
  }

  const KeySnifferCallbackInfo& info = it->second;
  set_moduleName(info.module_name);
  if (info.callback != Py_None) {
    PyObject *inCharObj = PyLong_FromLong(inChar);
    PyObject *inFlagsObj = PyLong_FromLong(inFlags);
    PyObject *inVirtualKeyObj = PyLong_FromLong((unsigned int)(inVirtualKey & 0xff));  // XPD-17397
    PyObject *args[] = {inCharObj, inFlagsObj, inVirtualKeyObj, info.refCon};
    pRes = PyObject_Vectorcall(info.callback, args, 4, nullptr);
    Py_DECREF(inCharObj);
    Py_DECREF(inFlagsObj);
    Py_DECREF(inVirtualKeyObj);
  }

  if(!pRes){
    char *s2 = objToStr(info.callback);
    pythonLog("[%s] Key sniffer callback %s failed.", CurrentPythonModuleName, s2);
    free(s2);
  } else if(PyLong_Check(pRes)){
    res = (int)PyLong_AsLong(pRes);
  } else {
    char *s2 = objToStr(info.callback);
    pythonLog("[%s] Key sniffer callback %s returned a wrong type.", CurrentPythonModuleName, s2);
    free(s2);
  }

  if(PyErr_Occurred()){
    pythonLogException();
  }

  Py_XDECREF(pRes);
  return res;
}

