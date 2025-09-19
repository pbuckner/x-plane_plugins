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
   (somewhat) typed version, we pass a capsule: it wrappes the (void *) pointer
   and includes a visible type.

   XPLMMenuID id;  // XPLMMenuID is defined as (void *)

   PyObject *capsule = makeCapsule(id, "XPLMMenuID");
   
 */


static std::unordered_map <void *, PyObject*> CapsuleDict;  // {ptr: capsule}

void deleteCapsule(PyObject *capsule)
{
  /* Clear context and remove from CapsuleDict */

  /* replaces removePtrRef(ptr, dict) with deleteCapsule(capsule) */

  const char *name = PyCapsule_GetName(capsule);
  void *context = PyCapsule_GetContext(capsule);
  void *ptr = PyCapsule_GetPointer(capsule, name);

  auto it = CapsuleDict.find(ptr);
  if (it == CapsuleDict.end()) {
    pythonLog("Unable to find capsule %s:%s %p in CapsuleDict", (char *)context, name, ptr);
    return;
  }

  free((char*)name);
  free(context);
  CapsuleDict.erase(it->first);
}

void *getVoidPtr(PyObject *capsule, std::string name)
{
  /* return (void *) ptr, stored within capsule */

  /* replaces refToPtr(PyObject *ref, const char *name) */
  errCheck("prior getVoidPtr %s", name.c_str());
  if (capsule == Py_None) return NULL;
  if (name == "") {
    name = PyCapsule_GetName(capsule);
    errCheck("Bad PyCapsule_GetName()");
  } else if (pythonDebugs) {
    if(name != PyCapsule_GetName(capsule)) {
      pythonLog("getVoidPtr handed a %s capsule, but trying to get one %s", PyCapsule_GetName(capsule), name.c_str());
      pythonLog("Capsule created by %s", (char *)PyCapsule_GetContext(capsule));
    }
  }
  if (PyLong_Check(capsule) && PyLong_AsLong(capsule) == 0 && !strcmp(name.c_str(), "XPWidgetID")) {
    /* XPWidgetID can be 0, refering to underlying X-Plane window, need to keep that */
    return NULL;
  }

  if (pythonDebugs) {
    PyObject *err = PyErr_Occurred();
    if(err){
      pythonLogException();
      char *s = objToStr(capsule);
      pythonDebug("Failed to convert '%s' capsule (%s) to pointer\n", name.c_str(), s);
      char msg[1024];
      snprintf(msg, sizeof(msg), "Failed to convert '%s' capsule (%s) to pointer\n", name.c_str(), s);
      PyErr_SetString(PyExc_ValueError , msg);
      free(s);
      return NULL;
    }
  }

  void *ptr = PyCapsule_GetPointer(capsule, name.c_str());
  errCheck("getVoidPtr failed for %s", name.c_str());
  return ptr;
}

void *getVoidPtr(PyObject *capsule) { return getVoidPtr(capsule, "");}

std::vector<std::string> CapsuleTypes = {
  "FMOD_CHANNEL", "FMOD_CHANNELGROUP", "FMOD_STUDIO_SYSTEM",
  "XPLMAvionicsID", "XPLMCommandRef", "XPLMDataRef", "XPLMFlightLoopID",
  "XPLMHotKeyID", "XPLMInstanceRef", "XPLMMapLayerID", "XPLMMapProjectionID",
  "XPLMMenuID", "XPLMObjectRef", "XPLMProbeRef", "XPLMWindowID", "XPWidgetID",            
};

PyObject *makeCapsule(void *ptr, std::string name)
{
  /* looks in hash table for ptr:
     a) returns already defined capsule (with Py_INCREF)
        if exists; otherwise
     b) creates capsule, stores in has and returns.
  */

  /* replaces getPtrRef(ptr, dict, name) with makeCapsule(ptr, name) */

#if ERRCHECK
  if (std::find(CapsuleTypes.begin(), CapsuleTypes.end(), name) == CapsuleTypes.end()) {
    pythonLog("Trying to create unknown Capsule type %s", name.c_str());
    return nullptr;
  }
#endif

  if(!ptr) Py_RETURN_NONE;

  errCheck("prior to makeCapsule for %s", name.c_str());

  PyObject *capsule;
  auto it = CapsuleDict.find(ptr);
  if (it == CapsuleDict.end()) {
    capsule = PyCapsule_New(ptr, strdup(name.c_str()), NULL); // the 'name' must outlive the capsule
    if (pythonCapsuleRegistration) {
      pythonLog("Capsule: New      %s > %s at: %s", CurrentPythonModuleName, name.c_str(),
                objToStr(get_pythonline()));
    }
    if (pythonDebugs) {
      char *context;
      if(-1 != asprintf(&context, "[%s] %s", CurrentPythonModuleName, objToStr(get_pythonline()))) {
        PyCapsule_SetContext(capsule, (void *)context);
      }
    }
    CapsuleDict[ptr] = capsule;
  } else {
    capsule = it->second;
    // if (pythonCapsuleRegistration) {
    //   char *context = (char *)PyCapsule_GetContext(capsule);
    //   pythonLog("Capsule: Existing %s [%s] > %s ", CurrentPythonModuleName, context, name.c_str());
    // }
  }
  Py_INCREF(capsule);
  return capsule;
}

// // Can be used where no callbacks are involved in passing the capsule
// PyObject *getPtrRefOneshot(void *ptr, const char *refName)
// {
//   if(!ptr){
//     Py_RETURN_NONE;
//   }
//   errCheck("prior getPtrRefOneshot: %s", refName);
//   PyObject *ret = PyCapsule_New(ptr, refName, NULL);
//   errCheck("end getPtrRefOneShot");
//   return ret;
// }
