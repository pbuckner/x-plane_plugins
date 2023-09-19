#define _GNU_SOURCE 1
#include <Python.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdbool.h>
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMMenus.h>
#include "utils.h"
#include "plugin_dl.h"

static intptr_t menuRefConCntr;
static PyObject *menuDict; // (internal) menuRefCon -> "(OsOiOO)", pluginSelf, inName, parentMenu, inParentItem, pythonHandler, menuRef);
static PyObject *menuRefDict;  // menuCapsule -> (internal) menuRefCon, key into menuDict

#define MENU_PLUGINSELF 0
#define MENU_NAME 1
#define MENU_PARENT_ID 2
#define MENU_PARENT_ITEM 3
#define MENU_CALLBACK 4
#define MENU_REFCON 5

static PyObject *menuIDCapsules;  /* XPLMMenuIDRef */
static PyObject *menuPluginIdxDict; /* plugin -> [list of Laminar menu IDs] */

static int nextXPLMMenuIdx = 0;
void clearInstanceMenuItems();

static const char menuIDRef[] = "XPLMMenuID"; 

void resetMenus() {nextXPLMMenuIdx = 0;}

static void menuHandler(void * menuRefCon, void * inItemRef)
{
  PyObject *pID = PyLong_FromVoidPtr(menuRefCon);
  if(PyErr_Occurred()) pythonLogException();
  PyObject *menuCallbackInfo = PyDict_GetItem(menuDict, pID);
  if(PyErr_Occurred()) pythonLogException();
  // pythonLog("Handling menu item: %s\n", objToStr(menuCallbackInfo));
  Py_DECREF(pID);
  if(menuCallbackInfo == NULL){
    pythonLog("Unknown callback requested in menuHandler(%p).\n", menuRefCon);
    return;
  }

  PyObject *res = PyObject_CallFunctionObjArgs(PyTuple_GetItem(menuCallbackInfo, MENU_CALLBACK),
                                        PyTuple_GetItem(menuCallbackInfo, MENU_REFCON), (PyObject*)inItemRef, NULL);
  PyObject *err = PyErr_Occurred();
  if(err){
    pythonLogException();
  }
  Py_XDECREF(res);
}

My_DOCSTR(_findPluginsMenu__doc__, "findPluginsMenu", "",
          "Returns menuID of plugins-menu.");
static PyObject *XPLMFindPluginsMenuFun(PyObject *self, PyObject *args)
{
  (void)self;
  (void)args;
  return getPtrRef(XPLMFindPluginsMenu(), menuIDCapsules, menuIDRef);
}

My_DOCSTR(_findAircraftMenu__doc__, "findAircraftMenu", "",
          "Returns menuID of currently loaded aircraft plugins menu.\n"
          "\n"
          "Note this is always 'None' for XPPython3.");
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

My_DOCSTR(_createMenu__doc__, "createMenu", "name=None, parentMenuID=None, parentItem=0, handler=None, refCon=None",
          "Creates menu, returning menuID or None on error.\n"
          "\n"
          "parentMenuID=None adds menu to PluginsMenu.\n");
static PyObject *XPLMCreateMenuFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"name", "parentMenuID", "parentItem", "handler", "refCon", NULL};
  (void)self;
  PyObject *parentMenu = Py_None, *pythonHandler = Py_None, *menuRef = Py_None;
  PyObject *idxList;
  int inParentItem = 0;
  const char *inName = NULL;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|sOiOO", keywords, &inName, &parentMenu, &inParentItem, &pythonHandler, &menuRef)){
    return NULL;
  }
  PyObject *pluginSelf = get_pluginSelf();
  // pythonLog("Creating menu for plugin self: %s, parent is: %s\n", objToStr(pluginSelf), objToStr(parentMenu));
  if (refToPtr(parentMenu, menuIDRef) == XPLMFindPluginsMenu()) {
    idxList = PyDict_GetItem(menuPluginIdxDict, pluginSelf);
    if (! idxList) {
      Py_DECREF(pluginSelf);
      // pythonLog("XPLMCreateMenu, no menu items exist.\n");
      PyErr_SetString(PyExc_IndexError, "list index out of range");
      Py_RETURN_NONE;
    }
    PyObject *xplmIndex = PyList_GetItem(idxList, inParentItem);  // throws IndexError
    // pythonLog(" Index %d -> %ld\n", inParentItem, PyLong_AsLong(xplmIndex));
    inParentItem = PyLong_AsLong(xplmIndex);
  }
  if (inName == NULL && parentMenu == Py_None) {
    inName = get_moduleName();
  }
  
  PyObject *argsObj = Py_BuildValue( "(OsOiOO)", pluginSelf, inName, parentMenu, inParentItem, pythonHandler, menuRef);

  void *menuRefCon = (void *)++menuRefConCntr;
  menuRef = PyLong_FromVoidPtr(menuRefCon);

  XPLMMenuHandler_f handler = (pythonHandler != Py_None) ? menuHandler : NULL;
  XPLMMenuID rawMenuID = XPLMCreateMenu(inName, refToPtr(parentMenu, menuIDRef),
                                        inParentItem, handler, menuRefCon);
  if(!rawMenuID){
    Py_DECREF(pluginSelf);
    Py_DECREF(menuRef);
    Py_RETURN_NONE;
  }

  if (parentMenu == Py_None) {
    /* we need to store and update index (we don't return index
       -- only the <capsuled>XPLMMenuID is returned */
    idxList = PyDict_GetItem(menuPluginIdxDict, pluginSelf);
    if (! idxList) {
      idxList = PyList_New(0);
    }
    PyList_Append(idxList, PyLong_FromLong(nextXPLMMenuIdx));
    PyDict_SetItem(menuPluginIdxDict, pluginSelf, idxList);
    // pythonLog("Create menu's idx is not provided, but next is [%d] (we don't return this)\n", nextXPLMMenuIdx);
    nextXPLMMenuIdx++;
  }
  Py_DECREF(pluginSelf);

  PyObject *menuID = getPtrRef(rawMenuID, menuIDCapsules, menuIDRef);
  PyDict_SetItem(menuDict, menuRef, argsObj);
  Py_DECREF(argsObj);
  PyDict_SetItem(menuRefDict, menuID, menuRef);
  Py_DECREF(menuRef);
  return menuID;
}

My_DOCSTR(_destroyMenu__doc__, "destroyMenu", "menuID",
          "Remove submenu from provided menuID.");
static PyObject *XPLMDestroyMenuFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"menuID", NULL};
  (void)self;
  PyObject *menuID;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &menuID)){
    return NULL;
  }
  PyObject *menuRef = PyDict_GetItem(menuRefDict, menuID);
  if(!menuRef){
    pythonLog("Unknown menuID passed to XPLMDestroyMenu, ignored.\n");
  } else {
    PyDict_DelItem(menuDict, menuRef);
    PyDict_DelItem(menuRefDict, menuID);
    

    XPLMMenuID id = refToPtr(menuID, menuIDRef);
    XPLMDestroyMenu(id);
    removePtrRef(id, menuIDCapsules);
  }
  
  Py_RETURN_NONE;
}

My_DOCSTR(_clearAllMenuItems__doc__, "clearAllMenuItems", "menuID=None",
          "Remove menu items from provided menuID, or \"all menus\", if menuID is None.");
static PyObject *XPLMClearAllMenuItemsFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"menuID", NULL};
  (void)self;
  PyObject *menuID = Py_None;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|O", keywords, &menuID)){
    return NULL;
  }
  if (menuID == Py_None || refToPtr(menuID, menuIDRef) == XPLMFindPluginsMenu()) {
    // pythonLog("Clearing top level\n");
    PyObject *pluginSelf = get_pluginSelf();
    clearInstanceMenuItems(pluginSelf);
    Py_DECREF(pluginSelf);
  } else {
    // pythonLog("Clearing item from menu: %s\n", objToStr(menuID));
    XPLMClearAllMenuItems(refToPtr(menuID, menuIDRef));
  }

  Py_RETURN_NONE;
}

void clearInstanceMenuItems(PyObject *pluginSelf) {
  /* For a particular (python) plugin:
     Get list of plugin's top-level menu indices  (--> menuPluginIdxDict[plugin])
        e.g., menuPlugIdxDict['myplugin'] = [3, 5, 6]
     ... we'll delete these, but first go through ALL
     menuPluginIdx items and shift existing indices "up" (to a small index number)
     if the existing index is greater than a index in the deletion list (e.g., '[3, 5, 6]')
   */
  pythonDebug("%*s Clearing top-level menu items for %s", 6, " ", objDebug(pluginSelf));
  if (!PyDict_Contains(menuPluginIdxDict, pluginSelf)) {
    pythonDebug("%*s No menu items for %s", 8, " ", objDebug(pluginSelf));
    return;
  }

  PyObject *localsDict = PyDict_New();
  PyDict_SetItemString(localsDict, "__builtins__", PyEval_GetBuiltins()); 
  PyDict_SetItemString(localsDict, "menuPluginIdxDict", menuPluginIdxDict);
  PyDict_SetItemString(localsDict, "plugin", pluginSelf);
  PyDict_SetItemString(localsDict, "nextXPLMMenuIdx", PyLong_FromLong(nextXPLMMenuIdx));

  /* l --> list of this plugins "laminar" menu indices, from high-to-low */
  /* iterate (again) through _all_ plugins menu indeces... 
     If we're viewing _this_ plugin, set list to []
     otherwise, decrement the (other) plugin's menu idx by one, if it's larger than
     one of the idx we're removing */
  /* return new (smaller) 'nextXPLMMenuIdx', and
     l ... the list to be deleted
  */

  PyRun_String("l=menuPluginIdxDict.get(plugin, [])\n"
               "l.reverse()\n"
               "for k,v in menuPluginIdxDict.items():\n"
               "    if k == plugin:\n"
               "        menuPluginIdxDict[k] = []\n"
               "    else:\n"
               "        for n in l:\n"
               "            menuPluginIdxDict[k] = [x if x<n else x-1 for x in menuPluginIdxDict[k]]\n"
               "nextXPLMMenuIdx = nextXPLMMenuIdx - len(l)",
               Py_file_input, localsDict, localsDict);
  nextXPLMMenuIdx = PyLong_AsLong(PyDict_GetItemString(localsDict, "nextXPLMMenuIdx"));
  PyObject *list = PyDict_GetItemString(localsDict, "l");
  pythonDebug("%*s List of menu ids for this plugin is: %s", 8, " ", objDebug(list));
  PyObject *iterator = PyObject_GetIter(list);
  PyObject *item;
  while ((item = PyIter_Next(iterator))) {
    XPLMRemoveMenuItem_ptr(XPLMFindPluginsMenu(), PyLong_AsLong(item));
    Py_DECREF(item);
  }

  Py_DECREF(iterator);
  Py_DECREF(localsDict);

  pythonDebug("%*s Clearing menu", 8, " ");
  
  PyObject *md_keys = PyDict_Keys(menuDict); /* new */
  PyObject *md_key_iterator = PyObject_GetIter(md_keys); /*new*/
  PyObject *md_key;
  while ((md_key = PyIter_Next(md_key_iterator))) {/*new*/
    PyObject *md_value = PyDict_GetItem(menuDict, md_key); /*borrowed*/
    if (PyTuple_GetItem(md_value, MENU_PLUGINSELF) == pluginSelf) {/*borrowed*/
      pythonDebug("%*s, Found menu[] for plugin, %s", 10, " ", objDebug(PyTuple_GetItem(md_value, MENU_NAME)));
      PyObject *mrd_keys = PyDict_Keys(menuRefDict); /*new*/
      PyObject *mrd_key_iterator = PyObject_GetIter(mrd_keys);/*new*/
      PyObject *mrd_key;
      while ((mrd_key = PyIter_Next(mrd_key_iterator))) {/*new*/
        PyObject *mrd_value = PyDict_GetItem(menuRefDict, mrd_key);/*borrowed*/
        if (PyLong_AsLong(mrd_value) == PyLong_AsLong(md_key)) {
          pythonDebug("%*s Clearing menu items for %ld", 10, " ", PyLong_AsLong(mrd_value));
          XPLMClearAllMenuItems(refToPtr(mrd_key, menuIDRef));
          XPLMDestroyMenu(refToPtr(mrd_key, menuIDRef));
          PyDict_DelItem(menuRefDict, mrd_key);
        }
        Py_DECREF(mrd_key);
      }
      PyDict_DelItem(menuDict, md_key);
      Py_DECREF(mrd_key_iterator);
      Py_DECREF(mrd_keys);
    }
    Py_DECREF(md_key);
  }
  Py_DECREF(md_key_iterator);
  Py_DECREF(md_keys);

  if (PyList_Size(PyDict_GetItem(menuPluginIdxDict, pluginSelf)) == 0) {
    /* no more, so remove from dict */
    PyDict_DelItem(menuPluginIdxDict, pluginSelf);
  }

  pythonDebug("%*s Cleared top-level menu items for %s", 8, " ", objDebug(pluginSelf));
}

My_DOCSTR(_appendMenuItem__doc__, "appendMenuItem", "menuID=None, name=\"Item\", refCon=None",
          "Appends new menu item to end of existing menuID.\n"
          "\n"
          "Returns index for created menu item or -1 on error.");
static PyObject *XPLMAppendMenuItemFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"menuID", "name", "refCon", "ignored", NULL};
  (void)self;
  PyObject *menuID = Py_None;
  const char *inItemName = "Item";
  int ignored;
  PyObject *inItemRef=Py_None;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|OsOi", keywords, &menuID, &inItemName, &inItemRef, &ignored)) {
    return NULL;
  }
  XPLMMenuID inMenu = menuID == Py_None ? XPLMFindPluginsMenu() : refToPtr(menuID, menuIDRef);
  int res = XPLMAppendMenuItem(inMenu, inItemName, inItemRef, 0);

  if (inMenu == XPLMFindPluginsMenu()) {
    // pythonLog("Appending to PluginsMenus: [%d], next: [%d]\n", res, nextXPLMMenuIdx);
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

My_DOCSTR(_appendMenuItemWithCommand__doc__, "appendMenuItemWithCommand", "menuID=None, name=\"Command\", commandRef=None",
          "Adds menu item to existing menuID, and executes commandRef when selected.\n"
          "\n"
          "Returns index for created menu item or -1 on error.");
static PyObject *XPLMAppendMenuItemWithCommandFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"menuID", "name", "commandRef", NULL};
  (void)self;
  PyObject *menuID = Py_None;
  const char *inItemName = "Command";
  PyObject *commandToExecute = Py_None;
  if(!XPLMAppendMenuItemWithCommand_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMAppendMenuItemWithCommand is available only in XPLM300 and up.");
    return NULL;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|OsO", keywords, &menuID, &inItemName, &commandToExecute)){
    return NULL;
  }
  
  if (commandToExecute == Py_None) {
    PyErr_SetString(PyExc_RuntimeError, "appendMenuItemWithCommand, commandRef not provided.\n");
    return NULL;
  }
  XPLMMenuID inMenu = menuID == Py_None ? XPLMFindPluginsMenu() : refToPtr(menuID, menuIDRef);
  XPLMCommandRef inCommandToExecute = (XPLMCommandRef)refToPtr(commandToExecute, commandRefName);
  int res = XPLMAppendMenuItemWithCommand_ptr(inMenu, inItemName, inCommandToExecute);
  if (inMenu == XPLMFindPluginsMenu()) {
    // pythonLog("Appending with Command to PluginsMenus: [%d], next: [%d]\n", res, nextXPLMMenuIdx);
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

My_DOCSTR(_appendMenuSeparator__doc__, "appendMenuSeparator", "menuID=None",
          "Adds separator to end of menu\n"
#if !defined(XPLM400)
          "\n"
          "Returns index of created item."
#endif
          );
static PyObject *XPLMAppendMenuSeparatorFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"menuID", NULL};
  (void)self;
  PyObject *menuID=Py_None;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|O", keywords, &menuID)){
    return NULL;
  }
  XPLMMenuID inMenu = menuID == Py_None ? XPLMFindPluginsMenu() : refToPtr(menuID, menuIDRef);

#if defined(XPLM400)  
  XPLMAppendMenuSeparator(inMenu);
#else
  int ret;
  ret = XPLMAppendMenuSeparator(inMenu);
#endif  

  if (inMenu == XPLMFindPluginsMenu()) {
    // pythonLog("Appending Separator to PluginsMenus: [%d], next: [%d]\n", ret, nextXPLMMenuIdx);
    PyObject *pluginSelf = get_pluginSelf();
    PyObject *idxList = PyDict_GetItem(menuPluginIdxDict, pluginSelf);
    if (! idxList) {
      idxList = PyList_New(0);
    }
    PyList_Append(idxList, PyLong_FromLong(nextXPLMMenuIdx));
    PyDict_SetItem(menuPluginIdxDict, pluginSelf, idxList);
    Py_DECREF(pluginSelf);
    nextXPLMMenuIdx++;
  }
#if defined(XPLM400)
  Py_RETURN_NONE;
#else  
  // Py_RETURN_NONE;  /* actually, apppendMenuSeparator __does__ return a value */
  return PyLong_FromLong(ret);
#endif
}

My_DOCSTR(_setMenuItemName__doc__, "setMenuItemName", "menuID=None, index=0, name=\"New Name\"",
          "Change the name of and existing menu item.");
static PyObject *XPLMSetMenuItemNameFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"menuID", "index", "name", NULL};
  (void)self;
  PyObject *menuID = Py_None;
  const char *inItemName = "New Name";
  int inIndex = 0;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|Ois", keywords, &menuID, &inIndex, &inItemName)){
    return NULL;
  }
  XPLMMenuID inMenu = menuID == Py_None ? XPLMFindPluginsMenu() : refToPtr(menuID, menuIDRef);
  if (inMenu == XPLMFindPluginsMenu()) {
    PyObject *pluginSelf = get_pluginSelf();
    PyObject *idxList = PyDict_GetItem(menuPluginIdxDict, pluginSelf);
    if (! idxList) {
      Py_DECREF(pluginSelf);
      // pythonLog("XPLMSetMenuItemName, no menu items exist.\n");
      PyErr_SetString(PyExc_IndexError, "list index out of range");
      Py_RETURN_NONE;
    }
    Py_DECREF(pluginSelf);

    PyObject *xplmIndex = PyList_GetItem(idxList, inIndex);  // throws IndexError
    // pythonLog(" Index %d -> %ld\n", inIndex, PyLong_AsLong(xplmIndex));
    XPLMSetMenuItemName(inMenu, PyLong_AsLong(xplmIndex), inItemName, 0);
  } else {
    XPLMSetMenuItemName(inMenu, inIndex, inItemName, 0);
  }
  Py_RETURN_NONE;
}

My_DOCSTR(_checkMenuItem__doc__, "checkMenuItem", "menuID=None, index=0, checked=Menu_Checked",
          "Set checkmark for given menu item.\n"
          "\n"
          "  Menu_NoCheck = 0\n"
          "  Menu_UnChecked = 1\n"
          "  Menu_Checked = 2"
          );
static PyObject *XPLMCheckMenuItemFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"menuID", "index", "checked", NULL};
  (void)self;
  PyObject *menuID = Py_None;
  int inIndex=0;
  int inCheck = xplm_Menu_Checked;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|Oii", keywords, &menuID, &inIndex, &inCheck)){
    return NULL;
  }
  XPLMMenuID inMenu = menuID == Py_None ? XPLMFindPluginsMenu() : refToPtr(menuID, menuIDRef);
  if (inMenu == XPLMFindPluginsMenu()) {
    PyObject *pluginSelf = get_pluginSelf();
    PyObject *idxList = PyDict_GetItem(menuPluginIdxDict, pluginSelf);
    if (! idxList) {
      Py_DECREF(pluginSelf);
      // pythonLog("XPLMCheckMenuItem, no menu items exist.\n");
      PyErr_SetString(PyExc_IndexError, "list index out of range");
      Py_RETURN_NONE;
    }
    Py_DECREF(pluginSelf);

    PyObject *xplmIndex = PyList_GetItem(idxList, inIndex);  // throws IndexError
    // pythonLog(" Index %d -> %ld\n", inIndex, PyLong_AsLong(xplmIndex));
    XPLMCheckMenuItem(inMenu, PyLong_AsLong(xplmIndex), inCheck);
  } else {
    XPLMCheckMenuItem(inMenu, inIndex, inCheck);
  }
  Py_RETURN_NONE;
}

My_DOCSTR(_checkMenuItemState__doc__, "checkMenuItemState", "menuID=None, index=0",
          "Returns menu item checked state.\n"
          "\n"
          "  Menu_NoCheck = 0\n"
          "  Menu_UnChecked = 1\n"
          "  Menu_Checked = 2"
          );
static PyObject *XPLMCheckMenuItemStateFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"menuID", "index", NULL};
  (void)self;
  PyObject *menuID=Py_None;
  int inIndex=0;
  int outCheck;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|Oi", keywords, &menuID, &inIndex)){
    return NULL;
  }
  XPLMMenuID inMenu = menuID == Py_None ? XPLMFindPluginsMenu() : refToPtr(menuID, menuIDRef);
  if (inMenu == XPLMFindPluginsMenu()) {
    PyObject *pluginSelf = get_pluginSelf();
    PyObject *idxList = PyDict_GetItem(menuPluginIdxDict, pluginSelf);
    if (! idxList) {
      Py_DECREF(pluginSelf);
      // pythonLog("XPLMCheckMenuItemState, no menu items exist.\n");
      PyErr_SetString(PyExc_IndexError, "list index out of range");
      Py_RETURN_NONE;
    }
    Py_DECREF(pluginSelf);

    PyObject *xplmIndex = PyList_GetItem(idxList, inIndex);  // throws IndexError
    // pythonLog(" Index %d -> %ld\n", inIndex, PyLong_AsLong(xplmIndex));
    // pythonLogFlush();
    XPLMCheckMenuItemState(inMenu, PyLong_AsLong(xplmIndex), &outCheck);
  } else {
    XPLMCheckMenuItemState(inMenu, inIndex, &outCheck);
  }
  return PyLong_FromLong(outCheck);
}

My_DOCSTR(_enableMenuItem__doc__, "enableMenuItem", "menuID=None, index=0, enabled=1",
          "Enables menu item\n"
          "\n"
          "Use enabled=0 to disable item, 1 to enable");
static PyObject *XPLMEnableMenuItemFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"menuID", "index", "enabled", NULL};
  (void)self;
  PyObject *menuID = Py_None;
  int inIndex=0;
  int enabled=1;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|Oii", keywords, &menuID, &inIndex, &enabled)){
    return NULL;
  }
  XPLMMenuID inMenu = menuID == Py_None ? XPLMFindPluginsMenu() : refToPtr(menuID, menuIDRef);
  if (inMenu == XPLMFindPluginsMenu()) {
    PyObject *pluginSelf = get_pluginSelf();
    PyObject *idxList = PyDict_GetItem(menuPluginIdxDict, pluginSelf);
    if (! idxList) {
      Py_DECREF(pluginSelf);
      // pythonLog("XPLMEnableMenuItem, no menu items exist.\n");
      PyErr_SetString(PyExc_IndexError, "list index out of range");
      Py_RETURN_NONE;
    }
    Py_DECREF(pluginSelf);

    PyObject *xplmIndex = PyList_GetItem(idxList, inIndex);  // throws IndexError
    // pythonLog(" Index %d -> %ld\n", inIndex, PyLong_AsLong(xplmIndex));
    // pythonLogFlush();
    XPLMEnableMenuItem(inMenu, PyLong_AsLong(xplmIndex), enabled);
  } else {
    XPLMEnableMenuItem(inMenu, inIndex, enabled);
  }
  Py_RETURN_NONE;
}

My_DOCSTR(_removeMenuItem__doc__, "removeMenuItem", "menuID=None, index=0",
          "Removes one item from menu.\n"
          "\n"
          "Note that all menu items below are moved up one index.");
static PyObject *XPLMRemoveMenuItemFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"menuID", "index", NULL};
  (void)self;
  PyObject *menuID = Py_None;
  int inIndex=0;
  if(!XPLMRemoveMenuItem_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMRemoveMenuItem is available only in XPLM210 and up.");
    return NULL;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|Oi", keywords, &menuID, &inIndex)){
    return NULL;
  }
  XPLMMenuID inMenu = menuID == Py_None ? XPLMFindPluginsMenu() : refToPtr(menuID, menuIDRef);
  if (inMenu == XPLMFindPluginsMenu()) {
    PyObject *pluginSelf = get_pluginSelf();
    PyObject *idxList = PyDict_GetItem(menuPluginIdxDict, pluginSelf);
    if (! idxList) {
      Py_DECREF(pluginSelf);
      // pythonLog("XPLMRemoveMenuItem, no menu items exist.\n");
      PyErr_SetString(PyExc_IndexError, "list index out of range");
      Py_RETURN_NONE;
    }

    PyObject *xplmIndex = PyList_GetItem(idxList, inIndex);  // throws IndexError
    
    if (!xplmIndex) {
      // pythonLog("Bad result from PyListGetItem()\n");
      Py_DECREF(pluginSelf);
      pythonLogException();
      Py_RETURN_NONE;
    }
    // pythonLog(" Index %d -> %ld\n", inIndex, PyLong_AsLong(xplmIndex));
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
    PyDict_SetItemString(localsDict, "__builtins__", PyEval_GetBuiltins()); 
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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
static PyMethodDef XPLMMenusMethods[] = {
  {"findPluginsMenu", (PyCFunction)XPLMFindPluginsMenuFun, METH_VARARGS, _findPluginsMenu__doc__},
  {"XPLMFindPluginsMenu", (PyCFunction)XPLMFindPluginsMenuFun, METH_VARARGS, ""},
  {"findAircraftMenu", (PyCFunction)XPLMFindAircraftMenuFun, METH_VARARGS, _findAircraftMenu__doc__},
  {"XPLMFindAircraftMenu", (PyCFunction)XPLMFindAircraftMenuFun, METH_VARARGS, ""},
  {"createMenu", (PyCFunction)XPLMCreateMenuFun, METH_VARARGS | METH_KEYWORDS, _createMenu__doc__},
  {"XPLMCreateMenu", (PyCFunction)XPLMCreateMenuFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"destroyMenu", (PyCFunction)XPLMDestroyMenuFun, METH_VARARGS | METH_KEYWORDS, _destroyMenu__doc__},
  {"XPLMDestroyMenu", (PyCFunction)XPLMDestroyMenuFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"clearAllMenuItems", (PyCFunction)XPLMClearAllMenuItemsFun, METH_VARARGS | METH_KEYWORDS, _clearAllMenuItems__doc__},
  {"XPLMClearAllMenuItems", (PyCFunction)XPLMClearAllMenuItemsFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"appendMenuItem", (PyCFunction)XPLMAppendMenuItemFun, METH_VARARGS | METH_KEYWORDS, _appendMenuItem__doc__},
  {"XPLMAppendMenuItem", (PyCFunction)XPLMAppendMenuItemFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"appendMenuItemWithCommand", (PyCFunction)XPLMAppendMenuItemWithCommandFun, METH_VARARGS | METH_KEYWORDS, _appendMenuItemWithCommand__doc__},
  {"XPLMAppendMenuItemWithCommand", (PyCFunction)XPLMAppendMenuItemWithCommandFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"appendMenuSeparator", (PyCFunction)XPLMAppendMenuSeparatorFun, METH_VARARGS | METH_KEYWORDS, _appendMenuSeparator__doc__},
  {"XPLMAppendMenuSeparator", (PyCFunction)XPLMAppendMenuSeparatorFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"setMenuItemName", (PyCFunction)XPLMSetMenuItemNameFun, METH_VARARGS | METH_KEYWORDS, _setMenuItemName__doc__},
  {"XPLMSetMenuItemName", (PyCFunction)XPLMSetMenuItemNameFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"checkMenuItem", (PyCFunction)XPLMCheckMenuItemFun, METH_VARARGS | METH_KEYWORDS, _checkMenuItem__doc__},
  {"XPLMCheckMenuItem", (PyCFunction)XPLMCheckMenuItemFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"checkMenuItemState", (PyCFunction)XPLMCheckMenuItemStateFun, METH_VARARGS | METH_KEYWORDS, _checkMenuItemState__doc__},
  {"XPLMCheckMenuItemState", (PyCFunction)XPLMCheckMenuItemStateFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"enableMenuItem", (PyCFunction)XPLMEnableMenuItemFun, METH_VARARGS | METH_KEYWORDS, _enableMenuItem__doc__},
  {"XPLMEnableMenuItem", (PyCFunction)XPLMEnableMenuItemFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"removeMenuItem", (PyCFunction)XPLMRemoveMenuItemFun, METH_VARARGS | METH_KEYWORDS, _removeMenuItem__doc__},
  {"XPLMRemoveMenuItem", (PyCFunction)XPLMRemoveMenuItemFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"_cleanup", cleanup, METH_VARARGS, ""},
  {NULL, NULL, 0, NULL}
};
#pragma GCC diagnostic pop


static struct PyModuleDef XPLMMenusModule = {
  PyModuleDef_HEAD_INIT,
  "XPLMMenus",
  "Laminar documentation: \n"
  "   https://developer.x-plane.com/sdk/XPLMMenus/\n"
  "XPPython3 documentation: \n"
  "   https://xppython3.rtfd.io/en/stable/development/modules/menus.html",
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
    PyModule_AddStringConstant(mod, "__author__", "Peter Buckner (pbuck@avnwx.com)");
    PyModule_AddIntConstant(mod, "xplm_Menu_NoCheck", xplm_Menu_NoCheck);
    PyModule_AddIntConstant(mod, "xplm_Menu_Unchecked", xplm_Menu_Unchecked);
    PyModule_AddIntConstant(mod, "xplm_Menu_Checked", xplm_Menu_Checked);

    PyModule_AddIntConstant(mod, "Menu_NoCheck", xplm_Menu_NoCheck);
    PyModule_AddIntConstant(mod, "Menu_Unchecked", xplm_Menu_Unchecked);
    PyModule_AddIntConstant(mod, "Menu_Checked", xplm_Menu_Checked);
  }

  return mod;
}


