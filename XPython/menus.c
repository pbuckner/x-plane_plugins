#define _GNU_SOURCE 1
#include <Python.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdbool.h>
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMMenus.h>
#include "utils.h"
#include "plugin_dl.h"

static intptr_t menuCntr;
static PyObject *menuDict;
static PyObject *menuRefDict;
static PyObject *menuIDCapsules;
static PyObject *menuPluginIdxDict;
static int nextXPLMMenuIdx = 0;

static const char menuIDRef[] = "XPLMMenuIDRef"; 

static void menuHandler(void * inMenuRef, void * inItemRef)
{
  PyObject *pID = PyLong_FromVoidPtr(inMenuRef);
  PyObject *menuCallbackInfo = PyDict_GetItem(menuDict, pID);
  Py_DECREF(pID);
  if(menuCallbackInfo == NULL){
    fprintf(pythonLogFile, "Unknown callback requested in menuHandler(%p).\n", inMenuRef);
    return;
  }
  PyObject *res = PyObject_CallFunctionObjArgs(PyTuple_GetItem(menuCallbackInfo, 4),
                                        PyTuple_GetItem(menuCallbackInfo, 5), (PyObject*)inItemRef, NULL);
  PyObject *err = PyErr_Occurred();
  if(err){
    fprintf(pythonLogFile, "Error occured during the menuHandler callback(inMenuRef = %p):\n", inMenuRef);
    PyErr_Print();
  }
  Py_XDECREF(res);
}

static PyObject *XPLMFindPluginsMenuFun(PyObject *self, PyObject *args)
{
  (void)self;
  (void)args;
  return getPtrRef(XPLMFindPluginsMenu(), menuIDCapsules, menuIDRef);
}

static PyObject *XPLMFindAircraftMenuFun(PyObject *self, PyObject *args)
{
  (void)self;
  (void)args;
  if(!XPLMFindAircraftMenu_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMFindAircraftMenu is available only in XPLM300 and up.");
    return NULL;
  }
  return getPtrRef(XPLMFindAircraftMenu_ptr(), menuIDCapsules, menuIDRef);
}

static PyObject *XPLMCreateMenuFun(PyObject *self, PyObject *args)
{
  (void)self;
  PyObject *parentMenu = NULL, *pythonHandler = NULL, *menuRef = NULL;
  PyObject *pluginSelf;
  int inParentItem;
  const char *inName;
  if(!PyArg_ParseTuple(args, "OsOiOO", &pluginSelf, &inName, &parentMenu, &inParentItem, &pythonHandler, &menuRef)){
    PyErr_Clear();
    if(!PyArg_ParseTuple(args, "sOiOO", &inName, &parentMenu, &inParentItem, &pythonHandler, &menuRef)){
      return NULL;
    }
  } else {
    pythonLogWarning("'self' deprecated as first parameter of XPLMCreateMenu");
  }
  pluginSelf = get_pluginSelf();
  PyObject *argsObj = Py_BuildValue( "(OsOiOO)", pluginSelf, inName, parentMenu, inParentItem, pythonHandler, menuRef);

  void *inMenuRef = (void *)++menuCntr;
  menuRef = PyLong_FromVoidPtr(inMenuRef);

  XPLMMenuHandler_f handler = (pythonHandler != Py_None) ? menuHandler : NULL;
  XPLMMenuID rawMenuID = XPLMCreateMenu(inName, refToPtr(parentMenu, menuIDRef),
                                        inParentItem, handler, inMenuRef);
  if(!rawMenuID){
    Py_DECREF(menuRef);
    Py_RETURN_NONE;
  }

  if (parentMenu == Py_None) {
    /* we need to store and update index (we don't return index
       -- only the <capsuled>XPLMMenuID is returned */
    PyObject *idxList = PyDict_GetItem(menuPluginIdxDict, pluginSelf);
    if (! idxList) {
      idxList = PyList_New(0);
    }
    PyList_Append(idxList, PyLong_FromLong(nextXPLMMenuIdx));
    PyDict_SetItem(menuPluginIdxDict, pluginSelf, idxList);
    // fprintf(pythonLogFile, "Create menu's idx is %d (we don't return this)\n", nextXPLMMenuIdx);
    nextXPLMMenuIdx++;
  }

  PyObject *menuID = getPtrRef(rawMenuID, menuIDCapsules, menuIDRef);
  PyDict_SetItem(menuDict, menuRef, argsObj);
  Py_DECREF(argsObj);
  PyDict_SetItem(menuRefDict, menuID, menuRef);
  Py_DECREF(menuRef);
  return menuID;
}

static PyObject *XPLMDestroyMenuFun(PyObject *self, PyObject *args)
{
  (void)self;
  PyObject *menuID, *pluginSelf;
  if(!PyArg_ParseTuple(args, "OO", &pluginSelf, &menuID)){
    PyErr_Clear();
    if(!PyArg_ParseTuple(args, "O", &menuID)){
      return NULL;
    }
  } else {
    pythonLogWarning("'self' deprecated as first parameter of XPLMDestroyMenu");
  }
  PyObject *menuRef = PyDict_GetItem(menuRefDict, menuID);
  if(!menuRef){
    fprintf(pythonLogFile, "Unknown menuID passed to XPLMDestroyMenu, ignored.\n");
  } else {
    PyDict_DelItem(menuDict, menuRef);
    PyDict_DelItem(menuRefDict, menuID);
    XPLMMenuID id = refToPtr(menuID, menuIDRef);
    XPLMDestroyMenu(id);
    removePtrRef(id, menuIDCapsules);
  }
  
  Py_RETURN_NONE;
}

static PyObject *XPLMClearAllMenuItemsFun(PyObject *self, PyObject *args)
{
  (void)self;
  PyObject *menuID;
  if(!PyArg_ParseTuple(args, "O", &menuID)){
    return NULL;
  }
  if (refToPtr(menuID, menuIDRef) == XPLMFindPluginsMenu()) {
    // fprintf(pythonLogFile, "Clearing top level\n");
    PyObject *pluginSelf = get_pluginSelf();
    PyObject *localsDict = PyDict_New();
    PyDict_SetItemString(localsDict, "m", menuPluginIdxDict);
    PyDict_SetItemString(localsDict, "p", pluginSelf);
    PyDict_SetItemString(localsDict, "idx", PyLong_FromLong(nextXPLMMenuIdx));
    
    PyRun_String("l=m[p]\nl.reverse()\nfor k,v in m.items():\n    if k == p:\n        m[k] = []\n    else:\n        for n in l:\n            m[k] = [x if x<n else x-1 for x in m[k]]\nidx = idx - len(l)", Py_file_input, localsDict, localsDict);
    nextXPLMMenuIdx = PyLong_AsLong(PyDict_GetItemString(localsDict, "idx"));
    PyObject *list = PyDict_GetItemString(localsDict, "l");
    // fprintf(pythonLogFile, "Updated next is %d\n", nextXPLMMenuIdx);

    PyObject *iterator = PyObject_GetIter(list);
    PyObject *item;
    while ((item = PyIter_Next(iterator))) {
      XPLMRemoveMenuItem_ptr(refToPtr(menuID, menuIDRef), PyLong_AsLong(item));
      Py_DECREF(item);
    }
    Py_DECREF(iterator);
    Py_DECREF(localsDict);
    Py_DECREF(pluginSelf);
  } else {
    XPLMClearAllMenuItems(refToPtr(menuID, menuIDRef));
  }

  Py_RETURN_NONE;
}

static PyObject *XPLMAppendMenuItemFun(PyObject *self, PyObject *args)
{
  (void)self;
  PyObject *menuID;
  const char *inItemName;
  int ignored;
  PyObject *inItemRef;
  if(!PyArg_ParseTuple(args, "OsOi", &menuID, &inItemName, &inItemRef, &ignored)) {
    PyErr_Clear();
    if(!PyArg_ParseTuple(args, "OsO", &menuID, &inItemName, &inItemRef)) {
      return NULL;
    }
  } else {
    pythonLogWarning("final parameter after itemRef is ignored for XPLMAppendMenuItem");
  }
  XPLMMenuID inMenu = refToPtr(menuID, menuIDRef);
  int res = XPLMAppendMenuItem(inMenu, inItemName, inItemRef, 0);

  if (inMenu == XPLMFindPluginsMenu()) {
    // fprintf(pythonLogFile, "Appending to PluginsMenus\n");
    PyObject *pluginSelf = get_pluginSelf();
    PyObject *idxList = PyDict_GetItem(menuPluginIdxDict, pluginSelf);
    if (! idxList) {
      idxList = PyList_New(0);
    }
    PyList_Append(idxList, PyLong_FromLong(nextXPLMMenuIdx));
    PyDict_SetItem(menuPluginIdxDict, pluginSelf, idxList);
    Py_DECREF(pluginSelf);
    int pluginIdx = PyList_GET_SIZE(idxList) - 1;
    nextXPLMMenuIdx++;
    return PyLong_FromLong(pluginIdx);
  }

  return PyLong_FromLong(res);
}

static PyObject *XPLMAppendMenuItemWithCommandFun(PyObject *self, PyObject *args)
{
  (void)self;
  PyObject *menuID;
  const char *inItemName;
  PyObject *commandToExecute;
  if(!XPLMAppendMenuItemWithCommand_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMAppendMenuItemWithCommand is available only in XPLM300 and up.");
    return NULL;
  }
  if(!PyArg_ParseTuple(args, "OsO", &menuID, &inItemName, &commandToExecute)){
    return NULL;
  }
  XPLMMenuID inMenu = refToPtr(menuID, menuIDRef);
  XPLMCommandRef inCommandToExecute = (XPLMCommandRef)refToPtr(commandToExecute, commandRefName);
  int res = XPLMAppendMenuItemWithCommand_ptr(inMenu, inItemName, inCommandToExecute);
  if (inMenu == XPLMFindPluginsMenu()) {
    //fprintf(pythonLogFile, "Appending with Command to PluginsMenus\n");
    PyObject *pluginSelf = get_pluginSelf();
    PyObject *idxList = PyDict_GetItem(menuPluginIdxDict, pluginSelf);
    if (! idxList) {
      idxList = PyList_New(0);
    }
    PyList_Append(idxList, PyLong_FromLong(nextXPLMMenuIdx));
    PyDict_SetItem(menuPluginIdxDict, pluginSelf, idxList);
    Py_DECREF(pluginSelf);
    int pluginIdx = PyList_GET_SIZE(idxList) - 1;
    nextXPLMMenuIdx++;
    return PyLong_FromLong(pluginIdx);
  }
  return PyLong_FromLong(res);
}

static PyObject *XPLMAppendMenuSeparatorFun(PyObject *self, PyObject *args)
{
  (void)self;
  PyObject *menuID;
  if(!PyArg_ParseTuple(args, "O", &menuID)){
    return NULL;
  }
  XPLMAppendMenuSeparator(refToPtr(menuID, menuIDRef));
  XPLMMenuID inMenu = refToPtr(menuID, menuIDRef);
  if (inMenu == XPLMFindPluginsMenu()) {
    // fprintf(pythonLogFile, "Appending Separator to PluginsMenus\n");
    PyObject *pluginSelf = get_pluginSelf();
    PyObject *idxList = PyDict_GetItem(menuPluginIdxDict, pluginSelf);
    if (! idxList) {
      idxList = PyList_New(0);
    }
    PyList_Append(idxList, PyLong_FromLong(nextXPLMMenuIdx));
    PyDict_SetItem(menuPluginIdxDict, pluginSelf, idxList);
    Py_DECREF(pluginSelf);
    int pluginIdx = PyList_GET_SIZE(idxList) - 1;
    nextXPLMMenuIdx++;
  }
  Py_RETURN_NONE;
}

static PyObject *XPLMSetMenuItemNameFun(PyObject *self, PyObject *args)
{
  (void)self;
  PyObject *menuID;
  const char *inItemName;
  int inIndex;
  int ignored;
  if(!PyArg_ParseTuple(args, "Oisi", &menuID, &inIndex, &inItemName, &ignored)){
    PyErr_Clear();
    if(!PyArg_ParseTuple(args, "Ois", &menuID, &inIndex, &inItemName)){
      return NULL;
    }
  } else {
    pythonLogWarning("final parameter after name is ignored for XPLMSetMenuItemName");
  }
  XPLMMenuID inMenu = refToPtr(menuID, menuIDRef);
  if (inMenu == XPLMFindPluginsMenu()) {
    PyObject *pluginSelf = get_pluginSelf();
    PyObject *idxList = PyDict_GetItem(menuPluginIdxDict, pluginSelf);
    if (! idxList) {
      Py_DECREF(pluginSelf);
      fprintf(pythonLogFile, "XPLMSetMenuItemName, no menu items exist.\n");
      Py_RETURN_NONE;
    }
    Py_DECREF(pluginSelf);

    PyObject *xplmIndex = PyList_GetItem(idxList, inIndex);  // throws IndexError
    // fprintf(pythonLogFile, " Index %d -> %ld\n", inIndex, PyLong_AsLong(xplmIndex));
    // fflush(pythonLogFile);
    XPLMSetMenuItemName(inMenu, PyLong_AsLong(xplmIndex), inItemName, 0);
  } else {
    XPLMSetMenuItemName(inMenu, inIndex, inItemName, 0);
  }
  Py_RETURN_NONE;
}

static PyObject *XPLMCheckMenuItemFun(PyObject *self, PyObject *args)
{
  (void)self;
  PyObject *menuID;
  int inIndex;
  int inCheck;
  if(!PyArg_ParseTuple(args, "Oii", &menuID, &inIndex, &inCheck)){
    return NULL;
  }
  XPLMMenuID inMenu = refToPtr(menuID, menuIDRef);
  if (inMenu == XPLMFindPluginsMenu()) {
    PyObject *pluginSelf = get_pluginSelf();
    PyObject *idxList = PyDict_GetItem(menuPluginIdxDict, pluginSelf);
    if (! idxList) {
      Py_DECREF(pluginSelf);
      fprintf(pythonLogFile, "XPLMCheckMenuItem, no menu items exist.\n");
      Py_RETURN_NONE;
    }
    Py_DECREF(pluginSelf);

    PyObject *xplmIndex = PyList_GetItem(idxList, inIndex);  // throws IndexError
    // fprintf(pythonLogFile, " Index %d -> %ld\n", inIndex, PyLong_AsLong(xplmIndex));
    // fflush(pythonLogFile);
    XPLMCheckMenuItem(inMenu, PyLong_AsLong(xplmIndex), inCheck);
  } else {
    XPLMCheckMenuItem(inMenu, inIndex, inCheck);
  }
  Py_RETURN_NONE;
}

static PyObject *XPLMCheckMenuItemStateFun(PyObject *self, PyObject *args)
{
  (void)self;
  PyObject *menuID;
  int inIndex;
  int outCheck;
  if(!PyArg_ParseTuple(args, "Oi", &menuID, &inIndex)){
    return NULL;
  }
  XPLMMenuID inMenu = refToPtr(menuID, menuIDRef);
  if (inMenu == XPLMFindPluginsMenu()) {
    PyObject *pluginSelf = get_pluginSelf();
    PyObject *idxList = PyDict_GetItem(menuPluginIdxDict, pluginSelf);
    if (! idxList) {
      Py_DECREF(pluginSelf);
      fprintf(pythonLogFile, "XPLMCheckMenuItemState, no menu items exist.\n");
      Py_RETURN_NONE;
    }
    Py_DECREF(pluginSelf);

    PyObject *xplmIndex = PyList_GetItem(idxList, inIndex);  // throws IndexError
    // fprintf(pythonLogFile, " Index %d -> %ld\n", inIndex, PyLong_AsLong(xplmIndex));
    // fflush(pythonLogFile);
    XPLMCheckMenuItemState(inMenu, PyLong_AsLong(xplmIndex), &outCheck);
  } else {
    XPLMCheckMenuItemState(inMenu, inIndex, &outCheck);
  }
  return PyLong_FromLong(outCheck);
}

static PyObject *XPLMEnableMenuItemFun(PyObject *self, PyObject *args)
{
  (void)self;
  PyObject *menuID;
  int inIndex;
  int enabled;
  if(!PyArg_ParseTuple(args, "Oii", &menuID, &inIndex, &enabled)){
    return NULL;
  }
  XPLMMenuID inMenu = refToPtr(menuID, menuIDRef);
  if (inMenu == XPLMFindPluginsMenu()) {
    PyObject *pluginSelf = get_pluginSelf();
    PyObject *idxList = PyDict_GetItem(menuPluginIdxDict, pluginSelf);
    if (! idxList) {
      Py_DECREF(pluginSelf);
      fprintf(pythonLogFile, "XPLMEnableMenuItem, no menu items exist.\n");
      Py_RETURN_NONE;
    }
    Py_DECREF(pluginSelf);

    PyObject *xplmIndex = PyList_GetItem(idxList, inIndex);  // throws IndexError
    // fprintf(pythonLogFile, " Index %d -> %ld\n", inIndex, PyLong_AsLong(xplmIndex));
    // fflush(pythonLogFile);
    XPLMEnableMenuItem(inMenu, PyLong_AsLong(xplmIndex), enabled);
  } else {
    XPLMEnableMenuItem(inMenu, inIndex, enabled);
  }
  Py_RETURN_NONE;
}

static PyObject *XPLMRemoveMenuItemFun(PyObject *self, PyObject *args)
{
  (void)self;
  PyObject *menuID;
  int inIndex;
  if(!XPLMRemoveMenuItem_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMRemoveMenuItem is available only in XPLM210 and up.");
    return NULL;
  }
  if(!PyArg_ParseTuple(args, "Oi", &menuID, &inIndex)){
    return NULL;
  }
  XPLMMenuID inMenu = refToPtr(menuID, menuIDRef);
  if (inMenu == XPLMFindPluginsMenu()) {
    PyObject *pluginSelf = get_pluginSelf();
    PyObject *idxList = PyDict_GetItem(menuPluginIdxDict, pluginSelf);
    if (! idxList) {
      Py_DECREF(pluginSelf);
      fprintf(pythonLogFile, "XPLMRemoveMenuItem, no menu items exist.\n");
      Py_RETURN_NONE;
    }

    PyObject *xplmIndex = PyList_GetItem(idxList, inIndex);  // throws IndexError
    
    if (!xplmIndex) {
      fprintf(pythonLogFile, "Bad result from PyListGetItem()\n");
      Py_DECREF(pluginSelf);
      PyErr_Print();
      Py_RETURN_NONE;
    }
    // fprintf(pythonLogFile, " Index %d -> %ld\n", inIndex, PyLong_AsLong(xplmIndex));
    // fflush(pythonLogFile);
    XPLMRemoveMenuItem_ptr(inMenu, PyLong_AsLong(xplmIndex));
    nextXPLMMenuIdx--;

    /* first delete the one we're removing */
    /* Now, go through _all_ lists, and any xplmIndex > 'xplmIndex' we need to reduce by one
       to "shift down":

       xpmlIndex = menuPluginIdxDict[pluginSelf].pop(inIndex)
       for k, l in menuPluginIdxDict.items():
           menuPluginIdxDict[k] = [(x if x < xplmIndex else x - 1) for x in l]

    */
    PyObject *localsDict = PyDict_New();
    PyDict_SetItemString(localsDict, "m", menuPluginIdxDict);
    PyDict_SetItemString(localsDict, "p", pluginSelf);
    PyDict_SetItemString(localsDict, "x", PyLong_FromLong(inIndex));

    PyRun_String("xplm=m[p].pop(x)\nfor k,l in m.items():\n    m[k]=[(x if x<xplm else x-1) for x in l]", Py_file_input, localsDict, localsDict);
    Py_DECREF(localsDict);
    Py_DECREF(pluginSelf);
  } else {
    XPLMRemoveMenuItem_ptr(inMenu, inIndex);
  }
  Py_RETURN_NONE;
}

static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  PyDict_Clear(menuDict);
  Py_DECREF(menuDict);
  PyDict_Clear(menuRefDict);
  Py_DECREF(menuRefDict);
  PyDict_Clear(menuIDCapsules);
  Py_DECREF(menuIDCapsules);
  Py_RETURN_NONE;
}

static PyMethodDef XPLMMenusMethods[] = {
  {"XPLMFindPluginsMenu", XPLMFindPluginsMenuFun, METH_VARARGS, ""},
  {"XPLMFindAircraftMenu", XPLMFindAircraftMenuFun, METH_VARARGS, ""},
  {"XPLMCreateMenu", XPLMCreateMenuFun, METH_VARARGS, ""},
  {"XPLMDestroyMenu", XPLMDestroyMenuFun, METH_VARARGS, ""},
  {"XPLMClearAllMenuItems", XPLMClearAllMenuItemsFun, METH_VARARGS, ""},
  {"XPLMAppendMenuItem", XPLMAppendMenuItemFun, METH_VARARGS, ""},
  {"XPLMAppendMenuItemWithCommand", XPLMAppendMenuItemWithCommandFun, METH_VARARGS, ""},
  {"XPLMAppendMenuSeparator", XPLMAppendMenuSeparatorFun, METH_VARARGS, ""},
  {"XPLMSetMenuItemName", XPLMSetMenuItemNameFun, METH_VARARGS, ""},
  {"XPLMCheckMenuItem", XPLMCheckMenuItemFun, METH_VARARGS, ""},
  {"XPLMCheckMenuItemState", XPLMCheckMenuItemStateFun, METH_VARARGS, ""},
  {"XPLMEnableMenuItem", XPLMEnableMenuItemFun, METH_VARARGS, ""},
  {"XPLMRemoveMenuItem", XPLMRemoveMenuItemFun, METH_VARARGS, ""},
  {"cleanup", cleanup, METH_VARARGS, ""},
  {NULL, NULL, 0, NULL}
};

static struct PyModuleDef XPLMMenusModule = {
  PyModuleDef_HEAD_INIT,
  "XPLMMenus",
  NULL,
  -1,
  XPLMMenusMethods,
  NULL,
  NULL,
  NULL,
  NULL
};

PyMODINIT_FUNC
PyInit_XPLMMenus(void)
{
  if(!(menuDict = PyDict_New())){
    return NULL;
  }
  PyDict_SetItemString(xppythonDicts, "menus", menuDict);
  if(!(menuRefDict = PyDict_New())){
    return NULL;
  }
  PyDict_SetItemString(xppythonDicts, "menuRefs", menuRefDict);
  if(!(menuIDCapsules = PyDict_New())){
    return NULL;
  }
  PyDict_SetItemString(xppythonCapsules, menuIDRef, menuIDCapsules);
  if(!(menuPluginIdxDict = PyDict_New())){
    return NULL;
  }
  PyDict_SetItemString(xppythonDicts, "menuPluginIdx", menuPluginIdxDict);

  PyObject *mod = PyModule_Create(&XPLMMenusModule);
  if(mod){
    PyModule_AddIntConstant(mod, "xplm_Menu_NoCheck", xplm_Menu_NoCheck);
    PyModule_AddIntConstant(mod, "xplm_Menu_Unchecked", xplm_Menu_Unchecked);
    PyModule_AddIntConstant(mod, "xplm_Menu_Checked", xplm_Menu_Checked);

  }

  return mod;
}


