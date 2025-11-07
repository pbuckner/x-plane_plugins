#define GNU_SOURCE 1
#include <Python.h>
#include "utils.h"

void setEncryptionLoader()
{
  PyObject *cryptoModuleObj = PyImport_ImportModule("cryptography");
  if (! cryptoModuleObj) {
    pythonLog("[XPPython3] Cryptography package not installed, XPPython3.xpyce will not be supported. See Documentation.");
    return;
  }
  Py_DECREF(cryptoModuleObj); // we needed to load it, just to verify it's available

  const char *xpyce = "XPPython3.xpyce";
  PyObject *mod = PyImport_ImportModule(xpyce);
  if (mod) {
    PyObject *function = PyObject_GetAttrString(mod, "XPYCEPathFinder");
    if (function) {
      PyObject *meta_path = PySys_GetObject("meta_path");
      PyList_Append(meta_path, function);
      PySys_SetObject("meta_path", meta_path);
      pythonDebug("[XPPthon3] XPYCEPathFinder initialized");
    } else {
      pythonDebug("[XPPython3] Failed to initialize XPYCEPathFinder.");
      Py_DECREF(mod);
    }
  } else {
    pythonDebug("[XPPython3] Failed to load %s module", xpyce);
  }
}
