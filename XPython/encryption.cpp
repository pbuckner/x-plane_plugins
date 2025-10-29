#define GNU_SOURCE 1
#include <Python.h>
#include "utils.h"

void setEncryptionLoader()
{
  PyObject *cryptoModuleObj = PyImport_ImportModule("cryptography");
  if (! cryptoModuleObj) {
    pythonLog("[XPPython3] Cryptography package not installed, XPPython3.xpye will not be supported. See Documentation.");
    return;
  }
  Py_DECREF(cryptoModuleObj); // we needed to load it, just to verify it's available

  PyObject *mod = PyImport_ImportModule("XPPython3.xpye");
  if (mod) {
    PyObject *function = PyObject_GetAttrString(mod, "XPYEPathFinder");
    if (function) {
      PyObject *meta_path = PySys_GetObject("meta_path");
      PyList_Append(meta_path, function);
      PySys_SetObject("meta_path", meta_path);
      pythonDebug("[XPPthon3] XPYEPathFinder initialized");
    } else {
      pythonDebug("[XPPython3] Failed to initialize XPYEPathFinder.");
      Py_DECREF(mod);
    }
  } else {
    pythonDebug("[XPPython3] Failed to load XPPython3.xpye module");
  }
}
