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
    pythonLog("Error occurred during the XPLMUnregisterKeySnifferCallback call:");
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
    pythonLog("keySnifferCallback, got unknown inRefcon (%p)!", inRefcon);
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



/* ---------------------------------------------------------------------------
   Method-table fragment merged into the XPLMDisplay module by
   PyInit_XPLMDisplay() in display.cpp. Docstrings live here, beside the
   functions they document.
   --------------------------------------------------------------------------- */
My_DOCSTR(_registerKeySniffer__doc__, "registerKeySniffer",
          "sniffer, before=0, refCon=None",
          "sniffer:Callable[[int, XPLMKeyFlags, int, Any], int], before:int=0, refCon:Any=None",
          "int",
          "Registers a key sniffer callback function.\n"
          "\n"
          "sniffer() callback takes four parameters (key, flags, vKey, refCon) and\n"
          "should return 0 to consume the key, 1 to pass it to next sniffer or X-Plane.\n"
          "\n"
          "before=1 will intercept keys before windows (i.e., the user may be typing in\n"
          "input field), so generally, use before=0 to sniff keys not already consumed.\n"
          "\nrefCon will be passed to your sniffer callback.");

My_DOCSTR(_unregisterKeySniffer__doc__, "unregisterKeySniffer",
          "sniffer, before=0, refCon=None",
          "sniffer:Callable[[int, XPLMKeyFlags, int, Any], int], before:int=0, refCon:Any=None",
          "int",
          "Unregisters key sniffer.\n"
          "\n"
          "Parameters must match those provided with registerKeySniffer().\n"
          "Returns 1 on success, 0 otherwise.");

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
PyMethodDef displayKeysnifferMethods[] = {
  {"registerKeySniffer", (PyCFunction)XPLMRegisterKeySnifferFun, METH_VARARGS | METH_KEYWORDS, _registerKeySniffer__doc__},
  {"XPLMRegisterKeySniffer", (PyCFunction)XPLMRegisterKeySnifferFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"unregisterKeySniffer", (PyCFunction)XPLMUnregisterKeySnifferFun, METH_VARARGS | METH_KEYWORDS, _unregisterKeySniffer__doc__},
  {"XPLMUnregisterKeySniffer", (PyCFunction)XPLMUnregisterKeySnifferFun, METH_VARARGS | METH_KEYWORDS, ""},
  {nullptr, nullptr, 0, nullptr}
};
#pragma GCC diagnostic pop
