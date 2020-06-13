#define _GNU_SOURCE 1
#include <Python.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdbool.h>
#include <structmember.h>
#include "xppythontypes.h"
#include "utils.h"

extern const char *pythonPluginVersion, *pythonPluginsPath, *pythonInternalPluginsPath;

typedef struct {
  PyObject_HEAD
  int virtualKey;
  int flags;
  PyObject *description;
  int plugin;
} HotKeyInfoObject;

static PyObject *
HotKeyInfo_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
  (void) args;
  (void) kwds;
  HotKeyInfoObject *self;
  self = (HotKeyInfoObject *) type->tp_alloc(type, 0);
  if (self != NULL) {
    self->virtualKey = 0;
    self->flags = 0;
    self->description = PyUnicode_FromString("");
    if (self->description == NULL) {
      Py_DECREF(self);
      return NULL;
    }
    self->plugin = 0;PyUnicode_FromString("");
  }
  return (PyObject *) self;
}

static int
HotKeyInfo_traverse(HotKeyInfoObject *self, visitproc visit, void *arg)
{
  Py_VISIT(self->description);
  return 0;
}

static int
HotKeyInfo_clear(HotKeyInfoObject *self)
{
  Py_CLEAR(self->description);
  return 0;
}
    
static void
HotKeyInfo_dealloc(HotKeyInfoObject *self)
{
  PyObject_GC_UnTrack(self);
  HotKeyInfo_clear(self);
  Py_TYPE(self)->tp_free((PyObject *) self);
}

static int
HotKeyInfo_init(HotKeyInfoObject *self, PyObject *args, PyObject *kwds)
{
  static char *kwlist[] = {"virtualKey", "flags", "description", "plugin", NULL};
  PyObject *description = NULL, *tmp;
  if (!PyArg_ParseTupleAndKeywords(args, kwds, "|iiUi", kwlist,
                                   &self->virtualKey, &self->flags, &description, &self->plugin))
        return -1;
    if (description) {
        tmp = self->description;
        Py_INCREF(description);
        self->description = description;
        Py_XDECREF(tmp);
    }
    return 0;
}

static PyMemberDef HotKeyInfo_members[] = {
    {"virtualKey", T_INT, offsetof(HotKeyInfoObject, virtualKey), 0, "virtual key code"},
    {"flags", T_INT, offsetof(HotKeyInfoObject, flags), 0, "XPLMKeyFlags"},
    {"description", T_OBJECT_EX, offsetof(HotKeyInfoObject, description), 0, "Description"},
    {"plugin", T_INT, offsetof(HotKeyInfoObject, plugin), 0, "XPLMPluginID"},
    {NULL}  /* Sentinel */
};

static PyTypeObject HotKeyInfoType = {
                                      PyVarObject_HEAD_INIT(NULL, 0)
                                      .tp_name = "xppython3.HotKeyInfo",
                                      .tp_doc = "HotKeyInfo",
                                      .tp_basicsize = sizeof(HotKeyInfoObject),
                                      .tp_itemsize = 0,
                                      .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_GC,
                                      .tp_new = HotKeyInfo_new,
                                      .tp_init = (initproc) HotKeyInfo_init,
                                      .tp_dealloc = (destructor) HotKeyInfo_dealloc,
                                      .tp_traverse = (traverseproc) HotKeyInfo_traverse,
                                      .tp_clear = (inquiry) HotKeyInfo_clear,
                                      .tp_members = HotKeyInfo_members,

};


PyObject *
PyHotKeyInfo_New(int virtualKey, int flags, char *description, int plugin)
{
  PyObject *argsList = Py_BuildValue("iisi", virtualKey, flags, description, plugin);
  printf("argslist is %s\n", objToStr(argsList));
  PyObject *obj = PyObject_CallObject((PyObject *) &HotKeyInfoType, argsList);
  printf("obj is %s\n", objToStr(obj));
  Py_DECREF(argsList);
  return (PyObject*)obj;
}


/* extern PyObject *widgetCallbackDict; */

/* static PyObject *XPPythonGetDictsFun(PyObject *self, PyObject *args) */
/* { */
/*   (void) self; */
/*   (void) args; */
/*   /\* */
/*      "cam":            one for each camera controller */
/*        key:  idx, */
/*        val: tuple(<module_filename>,       "PI_Display.py" */
/*                   howLong, */
/*                   controlFunc, */
/*                   refcon */
/*                   ) */
/*      "module":         one for each loaded Python Plugin module */
/*        key: tuple(<name>, <sig>, <description>, <module_name>)  ("Display regression test", "XPPython3.Display", "This is ...", "PI_Display") */
/*        val: <PythonInterface object at 0x...> */
  
/*      "fl":   one for each registered flight loop */
/*        key: idx, */
/*        val: tuple(<module_filename>,       "PI_Display.py" */
/*                   <bound method <> of <PythonInterface object at 0x...>>, */
/*                   <interval>,              -1.0 */
/*                   <referenceConstant>      "my flight loop" */
/*                  ) */
/*      "flRev": */
/*        key: tuple(<module_filename>,       "PI_Display.py" */
/*                   <bound method <> of <PythonInterface object at 0x...>>, */
/*                   <slot>                   24704555319,  (the reference constant address) */
/*        val: idx into callbackInfo          1 */
  
/*      "keySniffCallback": one for each registered key sniffer */
/*        key: idx, */
/*        value: tuple(<module_filename>,     "PI_Display.py" */
/*                     <bound method <> of PythonInterface object at 0x...>>, */
/*                     <beforeWindows>        1, */
/*                     <referenceConstant>    ["booya, ] */
  
/*       "window": one for each registered window */
/*         key: windowID,                      14055678383, */
/*         val: tuple(<bound method <drawWindowFunc>       of PythonInterface object at 0x...>>, */
/*                    <bound method <handleMouseClickFunc> of PythonInterface object at 0x...>>, */
/*                    <bound method <handleKeyFunc>        of PythonInterface object at 0x...>>, */
/*                    <bound method <handleCursorFunc>     of PythonInterface object at 0x...>>, */
/*                    <bound method <handleMouseWheelFunc> of PythonInterface object at 0x...>>, */
/*                    <bound method <handleRightClickFunc> of PythonInterface object at 0x...>>, */
/*   *\/ */

/*   PyObject *ret = PyDict_New(); */
/*   if(PyDict_Check(widgetCallbackDict)) { */
/*     PyDict_SetItemString(ret, "widgetCallback", PyDict_Copy(widgetCallbackDict)); //widgets */
/*   } else { */
/*     PyDict_SetItemString(ret, "widgetCallback", PyDict_New()); */
/*   } */
  
/*   if(PyDict_Check(moduleDict)) { */
/*     PyDict_SetItemString(ret, "module", PyDict_Copy(moduleDict)); //plugins */
/*   } else { */
/*     PyDict_SetItemString(ret, "module", PyDict_New()); */
/*   } */
/*   if(feDict && PyDict_Check(feDict)){ */
/*     PyDict_SetItemString(ret, "fe", PyDict_Copy(feDict)); // plugins */
/*   } else { */
/*     PyDict_SetItemString(ret, "fe", PyDict_New()); */
/*   } */
/*   if(windowDict && PyDict_Check(windowDict)){ */
/*     PyDict_SetItemString(ret, "window", PyDict_Copy(windowDict)); // display */
/*   } else { */
/*     PyDict_SetItemString(ret, "window", PyDict_New()); */
/*   } */
/*   if(hotkeyDict && PyDict_Check(hotkeyDict)){ */
/*     PyDict_SetItemString(ret, "hotkey", PyDict_Copy(hotkeyDict)); // display */
/*   } else { */
/*     PyDict_SetItemString(ret, "hotkey", PyDict_New()); */
/*   } */
/*   if(hotkeyIDDict && PyDict_Check(hotkeyIDDict)){ */
/*     PyDict_SetItemString(ret, "hotkeyID", PyDict_Copy(hotkeyIDDict)); // display */
/*   } else { */
/*     PyDict_SetItemString(ret, "hotkeyID", PyDict_New()); */
/*   } */
/*   if(drawCallbackDict && PyDict_Check(drawCallbackDict)){ */
/*     PyDict_SetItemString(ret, "drawCallback", PyDict_Copy(drawCallbackDict)); // display */
/*   } else { */
/*     PyDict_SetItemString(ret, "drawCallback", PyDict_New()); */
/*   } */
/*   if(drawCallbackIDDict && PyDict_Check(drawCallbackIDDict)){ */
/*     PyDict_SetItemString(ret, "drawCallbackID", PyDict_Copy(drawCallbackIDDict)); // display */
/*   } else { */
/*     PyDict_SetItemString(ret, "drawCallbackID", PyDict_New()); */
/*   } */
/*   if(keySniffCallbackDict && PyDict_Check(keySniffCallbackDict)){ */
/*     PyDict_SetItemString(ret, "keySniffCallback", PyDict_Copy(keySniffCallbackDict)); // display */
/*   } else { */
/*     PyDict_SetItemString(ret, "keySniffCallback", PyDict_New()); */
/*   } */
/*   if(menuDict && PyDict_Check(menuDict)){ */
/*     PyDict_SetItemString(ret, "menuDict", PyDict_Copy(menuDict));  // menut */
/*   } else { */
/*     PyDict_SetItemString(ret, "menu", PyDict_New()); */
/*   } */
/*   if(menuRefDict && PyDict_Check(menuRefDict)){ */
/*     PyDict_SetItemString(ret, "menuRefDict", PyDict_Copy(menuRefDict)); //menu */
/*   } else { */
/*     PyDict_SetItemString(ret, "menuRef", PyDict_New()); */
/*   } */
/*   if(camDict && PyDict_Check(camDict)){ */
/*     PyDict_SetItemString(ret, "cam", PyDict_Copy(camDict)); //camera */
/*   } else { */
/*     PyDict_SetItemString(ret, "cam", PyDict_New()); */
/*   } */
/*   if(accessorDict && PyDict_Check(accessorDict)){ */
/*     PyDict_SetItemString(ret, "accessor", PyDict_Copy(accessorDict));  //data_access */
/*   } else { */
/*     PyDict_SetItemString(ret, "accessor", PyDict_New()); */
/*   } */
/*   if(drefDict && PyDict_Check(drefDict)){ */
/*     PyDict_SetItemString(ret, "dref", PyDict_Copy(drefDict)); // data_access */
/*   } else { */
/*     PyDict_SetItemString(ret, "dref", PyDict_New()); */
/*   } */
/*   if(sharedDict && PyDict_Check(sharedDict)){ */
/*     PyDict_SetItemString(ret, "shared", PyDict_Copy(sharedDict)); //data_access */
/*   } else { */
/*     PyDict_SetItemString(ret, "shared", PyDict_New()); */
/*   } */
/*   if(availableDict && PyDict_Check(availableDict)){ */
/*     PyDict_SetItemString(ret, "available", PyDict_Copy(availableDict)); //planes */
/*   } else { */
/*     PyDict_SetItemString(ret, "available", PyDict_New()); */
/*   } */
/*   if(flDict && PyDict_Check(flDict)){ */
/*     PyDict_SetItemString(ret, "fl", PyDict_Copy(flDict));  //processing */
/*   } else { */
/*     PyDict_SetItemString(ret, "fl", PyDict_New()); */
/*   } */
/*   if(flRevDict && PyDict_Check(flRevDict)){ */
/*     PyDict_SetItemString(ret, "flRev", PyDict_Copy(flRevDict));//processing */
/*   } else { */
/*     PyDict_SetItemString(ret, "flRev", PyDict_New()); */
/*   } */
/*   if(flIDDict && PyDict_Check(flIDDict)){ */
/*     PyDict_SetItemString(ret, "flID", PyDict_Copy(flIDDict));//procesing */
/*   } else { */
/*     PyDict_SetItemString(ret, "flID", PyDict_New()); */
/*   } */
/*   if(loaderDict && PyDict_Check(loaderDict)){ */
/*     PyDict_SetItemString(ret, "loader", PyDict_Copy(loaderDict)); // scenery */
/*   } else { */
/*     PyDict_SetItemString(ret, "loader", PyDict_New()); */
/*   } */
/*   if(libEnumDict && PyDict_Check(libEnumDict)){ */
/*     PyDict_SetItemString(ret, "libEnum", PyDict_Copy(libEnumDict)); // scenery */
/*   } else { */
/*     PyDict_SetItemString(ret, "libEnum", PyDict_New()); */
/*   } */
/*   return ret; */
/* } */


static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  Py_RETURN_NONE;
}

static PyMethodDef XPPythonMethods[] = {
  /* {"XPPythonGetDicts", XPPythonGetDictsFun, METH_VARARGS, ""}, */
  {"cleanup", cleanup, METH_VARARGS, ""},
  {NULL, NULL, 0, NULL}
};

static struct PyModuleDef XPPythonModule = {
  PyModuleDef_HEAD_INIT,
  "XPPython",
  NULL,
  -1,
  XPPythonMethods,
  NULL,
  NULL,
  NULL,
  NULL
};

PyMODINIT_FUNC
PyInit_XPPython(void)
{
  if (PyType_Ready(&HotKeyInfoType) < 0)
    return NULL;

  PyObject *mod = PyModule_Create(&XPPythonModule);
  if (mod != NULL) {
    PyModule_AddStringConstant(mod, "VERSION", pythonPluginVersion);
    PyModule_AddStringConstant(mod, "PLUGINSPATH", pythonPluginsPath);
    PyModule_AddStringConstant(mod, "INTERNALPLUGINSPATH", pythonInternalPluginsPath);
    PyModule_AddObject(mod, "HotKeyInfo", (PyObject *) &HotKeyInfoType);
  }
  Py_INCREF(&HotKeyInfoType);
  

  return mod;
}
