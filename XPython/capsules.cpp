#define _GNU_SOURCE 1
#include <Python.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <algorithm>
#include "utils.h"
#include <time.h>

/* Capsules
   --------
   We often need to pass a (void *) ptr to/from python. In order to pass a
   (somewhat) typed version, we pass a capsule: it wraps the (void *) pointer
   and includes a visible type.

   XPLMMenuID id;  // XPLMMenuID is defined as (void *)

   PyObject *capsule = makeCapsule(id, "XPLMMenuID");
   
 */


std::unordered_map <void *, PyObject*> CapsuleDict;  // {ptr: capsule}

PyObject* buildCapsuleDict(void)
{
  PyObject *capsules = PyDict_New();

  for (const auto& pair : CapsuleDict) {
    void *ptr = pair.first;
    PyObject *capsule = pair.second;
    const char *name = PyCapsule_GetName(capsule);
    char *context = (char *)PyCapsule_GetContext(capsule);

    PyObject *tuple = PyTuple_New(3);
    PyTuple_SetItem(tuple, 0, PyUnicode_FromString(context ? context : ""));
    PyTuple_SetItem(tuple, 1, capsule);
    PyTuple_SetItem(tuple, 2, PyUnicode_FromString(name ? name : ""));
    Py_INCREF(capsule);
    PyObject *pkey = PyLong_FromVoidPtr(ptr);
    PyDict_SetItem(capsules, pkey, tuple);
    Py_DECREF(pkey);
    Py_DECREF(tuple);
  }
  return capsules;
}

void deleteCapsule(PyObject *capsule)
{
  /* Clear context and remove from CapsuleDict */

  /* replaces removePtrRef(ptr, dict) with deleteCapsule(capsule) */

  const char *name = nullptr;
  void *context = nullptr;
  if (pythonDebugs) {
    name = PyCapsule_GetName(capsule);
    context = PyCapsule_GetContext(capsule);
  }

  void *ptr = PyCapsule_GetPointer(capsule, name);

  auto it = CapsuleDict.find(ptr);
  if (it == CapsuleDict.end()) {
    pythonLog("Unable to find capsule %s:%s %p in CapsuleDict", context ? (char *)context : "", name ? name : "", ptr);
    return;
  }

  if(name) {
    free((char*)name);
  }
  if (context) {
    free(context);
  }
  CapsuleDict.erase(it);
}

void deleteCapsuleByPtr(void *ptr, const char *name)
{
  /* Delete capsule directly by pointer, avoiding temporary capsule creation */
  auto it = CapsuleDict.find(ptr);
  if (it == CapsuleDict.end()) {
    pythonLog("Unable to find capsule for pointer %p in CapsuleDict", ptr);
    return;
  }

  PyObject *capsule = it->second;
  void *context = nullptr;
  if (pythonDebugs) {
    const char *capsule_name = PyCapsule_GetName(capsule);

    // Verify the capsule type matches what caller expects
    if (pythonDebugs) {
      if (strcmp(capsule_name, name) != 0) {
        pythonLog("deleteCapsuleByPtr type mismatch: expected %s but found %s for pointer %p",
                  name, capsule_name, ptr);
        return;
      }
      context = PyCapsule_GetContext(capsule);
      
    }
    if (capsule_name) {
      free((char*)capsule_name);
    }
    if (context) {
      free(context);
    }
  }
  CapsuleDict.erase(it);
}

void *getVoidPtr(PyObject *capsule, std::string name)
{
  /* return (void *) ptr, stored within capsule */
  /* If pythonDebugs, we'll check it's a capsule with a valid name
     or raise TypeErrors
     Otherwise, we return as fast as we can.
  */

  if (!pythonDebugs) {
    return capsule == Py_None ? nullptr : PyCapsule_GetPointer(capsule, nullptr);
      // ? nullptr
      // : (name == ""
      //    ? PyCapsule_GetPointer(capsule, PyCapsule_GetName(capsule))
      //    : PyCapsule_GetPointer(capsule, name.c_str()));
  }

  errCheck("prior getVoidPtr %s", name.c_str());
  if (capsule == Py_None) return nullptr;
  if (! PyCapsule_CheckExact(capsule)) {
    PyErr_SetString(PyExc_ValueError, "getVoidPtr handed something, not a capsule");
    return nullptr;
  }
  if (name == "") {
    /* No name give, so extract it */
    name = PyCapsule_GetName(capsule);
    errCheck("Bad PyCapsule_GetName()");
  } else {
    /* name give, if it DOES NOT match capsule's name we do more checking and raise a TypeError */
    if(name != PyCapsule_GetName(capsule)) {
      bool valid = true;
      std::string capsule_name = PyCapsule_GetName(capsule);
      for (char c: capsule_name) {
        if (static_cast<unsigned char>(c) > 127 || static_cast<unsigned char>(c) < 65) {
          valid = false;
          break;
        }
      }
      if (! valid) {
        PyErr_SetString(PyExc_ValueError, "getVoidPtr handed something, not a capsule");
        return nullptr;
      } else {
        char *msg;
        if (-1 != asprintf(&msg, "[%s] getVoidPtr handed a %s capsule but needed a %s",
                           CurrentPythonModuleName, capsule_name.c_str(), name.c_str())) {
          pythonLog("%s", msg);
          PyErr_SetString(PyExc_TypeError, msg);
          free(msg);
        } else {
          pythonLog("getVoidPtr got wrong capsule type");
          PyErr_SetString(PyExc_TypeError, "getVoidPtr got wrong capsule type");
        }
        void *capsule_context = PyCapsule_GetContext(capsule);
        if(capsule_context) {
          pythonLog("Capsule created by %s", capsule_context);
        }
        return nullptr;
      }
    }
  }

  if (PyLong_Check(capsule) && PyLong_AsLong(capsule) == 0 && !strcmp(name.c_str(), "XPWidgetID")) {
    /* XPWidgetID can be 0, refering to underlying X-Plane window, need to keep that, without error */
    return nullptr;
  }

  /* Final error check... just to be sure */
  PyObject *err = PyErr_Occurred();
  if(err){
    pythonLogException();
    char *s = objToStr(capsule);
    pythonDebug("Failed to convert '%s' capsule (%s) to pointer\n", name.c_str(), s);
    char msg[1024];
    snprintf(msg, sizeof(msg), "[%s] Failed to convert '%s' capsule (%s) to pointer\n", CurrentPythonModuleName, name.c_str(), s);
    PyErr_SetString(PyExc_ValueError , msg);
    free(s);
    return nullptr;
  }

  /* get and return pointer */
  void *ptr = PyCapsule_GetPointer(capsule, name.c_str());
  errCheck("getVoidPtr failed for %s", name.c_str());
  return ptr;
}

void *getVoidPtr(PyObject *capsule) { return getVoidPtr(capsule, "");}

std::vector<std::string> CapsuleTypes = {
  "FMOD_CHANNEL", "FMOD_CHANNELGROUP", "FMOD_STUDIO_SYSTEM",
  "XPLMAvionicsID",
  "XPLMCommandRef", // simple ID
  "XPLMDataRef",  "XPLMFlightLoopID",  "XPLMHotKeyID",  "XPLMInstanceRef",  "XPLMMapLayerID",
  "XPLMMapProjectionID",  "XPLMMenuID",  "XPLMObjectRef", "XPLMProbeRef", "XPLMWindowID", "XPWidgetID",            
};

PyObject *makeCapsule(void *ptr, std::string name)
{
  /* looks in hash table for ptr:
     a) returns already defined capsule (with Py_INCREF)
        if exists; otherwise
     b) creates capsule, stores in hash and returns.
  */

  /* replaces getPtrRef(ptr, dict, name) with makeCapsule(ptr, name) */

#if ERRCHECK
  if (std::find(CapsuleTypes.begin(), CapsuleTypes.end(), name) == CapsuleTypes.end()) {
    pythonLog("Trying to create unknown Capsule type %s", name.c_str());
    PyErr_SetString(PyExc_TypeError, "Unknown capsule type requested");
    return nullptr;
  }
#endif

  if(!ptr) Py_RETURN_NONE;

  errCheck("prior to makeCapsule for %s", name.c_str());

  PyObject *capsule;
  auto it = CapsuleDict.find(ptr);
  if (it == CapsuleDict.end()) {
    if (pythonDebugs) {
      capsule = PyCapsule_New(ptr, strdup(name.c_str()), nullptr); // the 'name' must outlive the capsule
      if (pythonCapsuleRegistration) {
        pythonLog("Capsule: New      %s > %s at: %s", CurrentPythonModuleName, name.c_str(),
                  objToStr(get_pythonline()));
      }
      char *context;
      if(-1 != asprintf(&context, "[%s] %s", CurrentPythonModuleName, objToStr(get_pythonline()))) {
        PyCapsule_SetContext(capsule, (void *)context);
      }
    } else {
      capsule = PyCapsule_New(ptr, nullptr, nullptr); // if not pythonDebugs, we pass only generic pointers
    }
    CapsuleDict[ptr] = capsule;
  } else {
    capsule = it->second;
    if (pythonDebugs) {
      if (PyCapsule_GetName(capsule) != name) {
        pythonLog("Found capsule, but existing type '%s' doesn't match requested type '%s'", PyCapsule_GetName(capsule), name.c_str());
        PyErr_SetString(PyExc_TypeError, "mismatch in requested capsule type");
        return nullptr;
      }
      // /* this is pretty noisy, so don't normally support this reporting: */
      // if (pythonCapsuleRegistration) {
      //   char *context = (char *)PyCapsule_GetContext(capsule);
      //   pythonLog("Capsule: Existing %s [%s] > %s ", CurrentPythonModuleName, context, name.c_str());
      // }
    }
  }
  Py_INCREF(capsule);
  return capsule;
}

void logCapsules()
{
  for(auto& item : CapsuleDict) {
    PyObject *capsule = item.second;
    const char *name = PyCapsule_GetName(capsule);
    void *context = PyCapsule_GetContext(capsule);
    void *ptr = PyCapsule_GetPointer(capsule, name);

    pythonLog("%-20s %p: %s", name ? name : "", ptr, context ? (char *)context : "");
  }
}
