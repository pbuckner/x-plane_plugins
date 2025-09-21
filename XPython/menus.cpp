#define _GNU_SOURCE 1
#include <Python.h>
#include <sys/time.h>
#include <stdio.h>
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMMenus.h>
#include <string>
#include <vector>
#include <unordered_map>
#include "utils.h"
#include "plugin_dl.h"
#include "menus.h"
#include "capsules.h"

static intptr_t menuRefConCntr;
struct MenuInfo {
  std::string name;
  std::string module_name;
  XPLMMenuID parent_id;
  int parent_item;
  PyObject *callback;
  PyObject *refCon;
};

static std::unordered_map<intptr_t, MenuInfo> menuCallbacks;
static PyObject *menuRefDict;  // menuCapsule -> (internal) menuRefCon, key into menuCallbacks

static std::unordered_map<void*, PyObject*> menuCapsules;
static PyObject *menuPluginIdxDict; /* plugin -> [list of Laminar menu IDs] */

static int nextXPLMMenuIdx = 0;

void resetMenus() {
  /* NOTE: stopping an instance, which is done (also) on reload
     calls xpy_cleanUpInstance, which already clears menus
     clears menuPluginIdxDict, menuDict, and menuRefDict
  */
  nextXPLMMenuIdx = 0;
  if (PyDict_Size(menuPluginIdxDict) > 0) {
    pythonLog("OOPS, menuPluginDict is %d", PyDict_Size(menuPluginIdxDict));
  }
  if (menuCallbacks.size() > 0) {
    pythonLog("OOPS, menuCallbacks is %zu", menuCallbacks.size());
  }
  if (PyDict_Size(menuRefDict) > 0) {
    pythonLog("OOPS, menuRefDict is %d", PyDict_Size(menuRefDict));
  }
}

static void menuHandler(void * menuRefCon, void * inItemRef)
{
  errCheck("prior menuHandler");
  intptr_t refcon_id = (intptr_t)menuRefCon;
  auto it = menuCallbacks.find(refcon_id);
  if (it == menuCallbacks.end()) {
    pythonLog("Unknown callback requested in menuHandler(%p).", menuRefCon);
    return;
  }

  MenuInfo& info = it->second;
  set_moduleName(info.module_name);

  PyObject *res = PyObject_CallFunctionObjArgs(info.callback, info.refCon, (PyObject*)inItemRef, nullptr);
  errCheck("end menuHandler");
  Py_XDECREF(res);
}

My_DOCSTR(_findPluginsMenu__doc__, "findPluginsMenu",
          "",
          "",
          "XPLMMenuID",
          "Returns menuID of plugins-menu.");
static PyObject *XPLMFindPluginsMenuFun(PyObject *self, PyObject *args)
{
  (void)self;
  (void)args;
  errCheck("prior findPluginsMenu");
  PyObject *ret = makeCapsule(XPLMFindPluginsMenu(), "XPLMMenuID");
  errCheck("end findPluginsMenu");
  return ret;
}

My_DOCSTR(_findAircraftMenu__doc__, "findAircraftMenu",
          "",
          "",
          "XPLMMenuID",
          "Returns menuID of currently loaded aircraft plugins menu.\n"
          "\n"
          "Note this is always 'None' for XPPython3.");
static PyObject *XPLMFindAircraftMenuFun(PyObject *self, PyObject *args)
{
  (void)self;
  (void)args;
  if(!XPLMFindAircraftMenu_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMFindAircraftMenu is available only in XPLM300 and up.");
    return nullptr;
  }
  return makeCapsule(XPLMFindAircraftMenu_ptr(), "XPLMMenuID");
}

My_DOCSTR(_createMenu__doc__, "createMenu",
          "name=None, parentMenuID=None, parentItem=0, handler=None, refCon=None",
          "name:Optional[str]=None, parentMenuID:Optional[XPLMMenuID]=None, parentItem:Optional[int]=0, "
          "handler:Optional[Callable[[Any, Any], None]]=None, refCon:Optional[Any]=None",
          "None | XPLMMenuID",
          "Creates menu, returning menuID or None on error.\n"
          "\n"
          "parentMenuID=None adds menu to PluginsMenu.\n");
static PyObject *XPLMCreateMenuFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  errCheck("prior CreateMenu");
  static char *keywords[] = {CHAR("name"), CHAR("parentMenuID"), CHAR("parentItem"), CHAR("handler"), CHAR("refCon"), nullptr};
  (void)self;
  PyObject *parentMenu = Py_None, *pythonHandler = Py_None, *menuRef = Py_None;
  PyObject *idxList;
  int inParentItem = 0;
  const char *inName = nullptr;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|sOiOO", keywords, &inName, &parentMenu, &inParentItem, &pythonHandler, &menuRef)){
    return nullptr;
  }
  PyObject *pluginSelf = get_moduleName_p();
  /* char *s = objToStr(pluginSelf); */
  /* char *s2 = objToStr(parentMenu); */
  /* pythonLog("Creating menu for plugin self: %s, parent is: %s", s, s2); */
  /* free(s); */
  /* free(s2); */
  if (getVoidPtr(parentMenu, "XPLMMenuID") == XPLMFindPluginsMenu()) {
    idxList = PyDict_GetItem(menuPluginIdxDict, pluginSelf);
    if (! idxList) {
      Py_DECREF(pluginSelf);
      // pythonLog("XPLMCreateMenu, no menu items exist.");
      PyErr_SetString(PyExc_IndexError, "list index out of range");
      Py_RETURN_NONE;
    }
    PyObject *xplmIndex = PyList_GetItem(idxList, inParentItem);  // throws IndexError
    // pythonLog(" Index %d -> %ld", inParentItem, PyLong_AsLong(xplmIndex));
    inParentItem = PyLong_AsLong(xplmIndex);
  }
  if (inName == nullptr && parentMenu == Py_None) {
    inName = CurrentPythonModuleName;
  }
  
  intptr_t refcon = ++menuRefConCntr;
  menuRef = PyLong_FromVoidPtr((void*)refcon);

  XPLMMenuHandler_f handler = (pythonHandler != Py_None) ? menuHandler : nullptr;
  XPLMMenuID rawMenuID = XPLMCreateMenu(inName, getVoidPtr(parentMenu, "XPLMMenuID"),
                                        inParentItem, handler, (void*)refcon);
  if(!rawMenuID){
    Py_DECREF(pluginSelf);
    Py_DECREF(menuRef);
    Py_RETURN_NONE;
  }

  // Store menu info in C++ map
  menuCallbacks[refcon] = {
    .name = std::string(inName ? inName : ""),
    .module_name = std::string(CurrentPythonModuleName),
    .parent_id = getVoidPtr(parentMenu, "XPLMMenuID"),
    .parent_item = inParentItem,
    .callback = pythonHandler,
    .refCon = menuRef
  };
  if (pythonHandler != Py_None) {
    Py_INCREF(pythonHandler);
  }
  Py_INCREF(menuRef);

  if (parentMenu == Py_None) {
    /* we need to store and update index (we don't return index
       -- only the <capsuled>XPLMMenuID is returned */
    idxList = PyDict_GetItem(menuPluginIdxDict, pluginSelf);
    if (! idxList) {
      idxList = PyList_New(0);
    }
    PyList_Append(idxList, PyLong_FromLong(nextXPLMMenuIdx));
    PyDict_SetItem(menuPluginIdxDict, pluginSelf, idxList);
    // pythonLog("Create menu's idx is not provided, but next is [%d] (we don't return this)", nextXPLMMenuIdx);
    nextXPLMMenuIdx++;
  }
  Py_DECREF(pluginSelf);

  PyObject *menuID = makeCapsule(rawMenuID, "XPLMMenuID");
  PyDict_SetItem(menuRefDict, menuID, menuRef);
  Py_DECREF(menuRef);
  errCheck("end CreateMenu");
  return menuID;
}

My_DOCSTR(_destroyMenu__doc__, "destroyMenu",
          "menuID",
          "menuID:XPLMMenuID",
          "None",
          "Remove submenu from provided menuID.");
static PyObject *XPLMDestroyMenuFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("menuID"), nullptr};
  (void)self;
  PyObject *menuID;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &menuID)){
    return nullptr;
  }
  PyObject *menuRef = PyDict_GetItem(menuRefDict, menuID);
  if(!menuRef){
    pythonLog("Unknown menuID passed to XPLMDestroyMenu, ignored.");
  } else {
    // Remove from menuCallbacks using menuRef as key
    intptr_t refcon_id = (intptr_t)PyLong_AsVoidPtr(menuRef);
    auto it = menuCallbacks.find(refcon_id);
    if (it != menuCallbacks.end()) {
      if (it->second.callback != Py_None) {
        Py_DECREF(it->second.callback);
      }
      Py_DECREF(it->second.refCon);
      menuCallbacks.erase(it);
    }

    PyDict_DelItem(menuRefDict, menuID);

    XPLMMenuID id = getVoidPtr(menuID, "XPLMMenuID");
    XPLMDestroyMenu(id);
    deleteCapsule(menuID);
  }
  
  Py_RETURN_NONE;
}

My_DOCSTR(_clearAllMenuItems__doc__, "clearAllMenuItems",
          "menuID=None",
          "menuID:Optional[XPLMMenuID]",
          "None",
          "Remove menu items from provided menuID, or 'all menus', if menuID is None.");
static PyObject *XPLMClearAllMenuItemsFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("menuID"), nullptr};
  (void)self;
  PyObject *menuID = Py_None;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|O", keywords, &menuID)){
    return nullptr;
  }
  if (menuID == Py_None || getVoidPtr(menuID, "XPLMMenuID") == XPLMFindPluginsMenu()) {
    // pythonLog("Clearing top level");
    PyObject *pluginSelf = get_moduleName_p();
    clearInstanceMenuItems(pluginSelf);
    Py_DECREF(pluginSelf);
  } else {
    /* char *s = objToStr(menuID); */
    /* pythonLog("Clearing item from menu: %s", s); */
    /* free(s); */
    XPLMClearAllMenuItems(getVoidPtr(menuID, "XPLMMenuID"));
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
  if (!menuPluginIdxDict) return;
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

  MyPyRun_String("l=menuPluginIdxDict.get(plugin, [])\n"
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

  char *target_module = objToStr(pluginSelf);
  std::string target_module_str(target_module);
  free(target_module);

  for (auto it = menuCallbacks.begin(); it != menuCallbacks.end();) {
    MenuInfo& info = it->second;
    if (info.module_name == target_module_str) {
      pythonDebug("%*s, Found menu[] for plugin, %s", 10, " ", info.name.c_str());

      // Find corresponding menuRefDict entry and remove it
      PyObject *mrd_keys = PyDict_Keys(menuRefDict);
      PyObject *mrd_key_iterator = PyObject_GetIter(mrd_keys);
      PyObject *mrd_key;
      while ((mrd_key = PyIter_Next(mrd_key_iterator))) {
        PyObject *mrd_value = PyDict_GetItem(menuRefDict, mrd_key);
        if (PyLong_AsVoidPtr(mrd_value) == (void*)it->first) {
          pythonDebug("%*s Clearing menu items for %ld", 10, " ", it->first);
          XPLMClearAllMenuItems(getVoidPtr(mrd_key, "XPLMMenuID"));
          XPLMDestroyMenu(getVoidPtr(mrd_key, "XPLMMenuID"));
          PyDict_DelItem(menuRefDict, mrd_key);
        }
        Py_DECREF(mrd_key);
      }
      Py_DECREF(mrd_key_iterator);
      Py_DECREF(mrd_keys);

      // Clean up Python objects
      if (info.callback != Py_None) {
        Py_DECREF(info.callback);
      }
      Py_DECREF(info.refCon);
      it = menuCallbacks.erase(it);
    } else {
      ++it;
    }
  }

  if (PyList_Size(PyDict_GetItem(menuPluginIdxDict, pluginSelf)) == 0) {
    /* no more, so remove from dict */
    PyDict_DelItem(menuPluginIdxDict, pluginSelf);
  }

  pythonDebug("%*s Cleared top-level menu items for %s", 8, " ", objDebug(pluginSelf));
}

My_DOCSTR(_appendMenuItem__doc__, "appendMenuItem",
          "menuID=None, name='Item', refCon=None",
          "menuID:Optional[XPLMMenuID]=None, name:str='Item', refCon:Any=None",
          "int",
          "Appends new menu item to end of existing menuID.\n"
          "\n"
          "Returns index for created menu item or -1 on error.");
static PyObject *XPLMAppendMenuItemFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  errCheck("prior appendMenuItem");
  static char *keywords[] = {CHAR("menuID"), CHAR("name"), CHAR("refCon"), CHAR("ignored"), nullptr};
  (void)self;
  PyObject *menuID = Py_None;
  const char *inItemName = "Item";
  int ignored;
  PyObject *inItemRef=Py_None;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|OsOi", keywords, &menuID, &inItemName, &inItemRef, &ignored)) {
    return nullptr;
  }
  XPLMMenuID inMenu = menuID == Py_None ? XPLMFindPluginsMenu() : getVoidPtr(menuID, "XPLMMenuID");
  int res = XPLMAppendMenuItem(inMenu, inItemName, inItemRef, 0);

  if (inMenu == XPLMFindPluginsMenu()) {
    // pythonLog("Appending to PluginsMenus: [%d], next: [%d]", res, nextXPLMMenuIdx);
    PyObject *pluginSelf = get_moduleName_p();
    PyObject *idxList = PyDict_GetItem(menuPluginIdxDict, pluginSelf);
    if (! idxList) {
      idxList = PyList_New(0);
    }
    PyList_Append(idxList, PyLong_FromLong(nextXPLMMenuIdx));
    PyDict_SetItem(menuPluginIdxDict, pluginSelf, idxList);
    Py_DECREF(pluginSelf);
    int pluginIdx = PyList_Size(idxList) - 1;
    nextXPLMMenuIdx++;
    return PyLong_FromLong(pluginIdx);
  }

  errCheck("end appendMenuItem");
  return PyLong_FromLong(res);
}

My_DOCSTR(_appendMenuItemWithCommand__doc__, "appendMenuItemWithCommand",
          "menuID=None, name='Command', commandRef=None",
          "menuID:Optional[XPLMMenuID]=None, name:str='Command', commandRef:Any=None",
          "int",
          "Adds menu item to existing menuID, and executes commandRef when selected.\n"
          "\n"
          "Returns index for created menu item or -1 on error.");
static PyObject *XPLMAppendMenuItemWithCommandFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  errCheck("prior appendMenuItemWithCommand");
  static char *keywords[] = {CHAR("menuID"), CHAR("name"), CHAR("commandRef"), nullptr};
  (void)self;
  PyObject *menuID = Py_None;
  const char *inItemName = "Command";
  PyObject *commandToExecute = Py_None;
  if(!XPLMAppendMenuItemWithCommand_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMAppendMenuItemWithCommand is available only in XPLM300 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|OsO", keywords, &menuID, &inItemName, &commandToExecute)){
    return nullptr;
  }
  
  if (commandToExecute == Py_None) {
    PyErr_SetString(PyExc_RuntimeError, "appendMenuItemWithCommand, commandRef not provided.\n");
    return nullptr;
  }
  XPLMMenuID inMenu = menuID == Py_None ? XPLMFindPluginsMenu() : getVoidPtr(menuID, "XPLMMenuID");
  XPLMCommandRef inCommandToExecute = (XPLMCommandRef)getVoidPtr(commandToExecute, "XPLMCommandRef");
  int res = XPLMAppendMenuItemWithCommand_ptr(inMenu, inItemName, inCommandToExecute);
  if (inMenu == XPLMFindPluginsMenu()) {
    // pythonLog("Appending with Command to PluginsMenus: [%d], next: [%d]", res, nextXPLMMenuIdx);
    PyObject *pluginSelf = get_moduleName_p();
    PyObject *idxList = PyDict_GetItem(menuPluginIdxDict, pluginSelf);
    if (! idxList) {
      idxList = PyList_New(0);
    }
    PyList_Append(idxList, PyLong_FromLong(nextXPLMMenuIdx));
    PyDict_SetItem(menuPluginIdxDict, pluginSelf, idxList);
    Py_DECREF(pluginSelf);
    int pluginIdx = PyList_Size(idxList) - 1;
    nextXPLMMenuIdx++;
    return PyLong_FromLong(pluginIdx);
  }
  errCheck("end appendMenuItemWithCommand");
  return PyLong_FromLong(res);
}

My_DOCSTR(_appendMenuSeparator__doc__, "appendMenuSeparator",
          "menuID=None",
          "menuID:XPLMMenuID=None",
          "None | int",
          "Adds separator to end of menu.\n"
          "\n"
          "Returns index of created item for XP11, None for XP12"
          );
static PyObject *XPLMAppendMenuSeparatorFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("menuID"), nullptr};
  (void)self;
  PyObject *menuID=Py_None;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|O", keywords, &menuID)){
    return nullptr;
  }
  XPLMMenuID inMenu = menuID == Py_None ? XPLMFindPluginsMenu() : getVoidPtr(menuID, "XPLMMenuID");

#if defined(XPLM400)  
  XPLMAppendMenuSeparator(inMenu);
#else
  int ret;
  ret = XPLMAppendMenuSeparator(inMenu);
#endif  

  if (inMenu == XPLMFindPluginsMenu()) {
    // pythonLog("Appending Separator to PluginsMenus: [%d], next: [%d]", ret, nextXPLMMenuIdx);
    PyObject *pluginSelf = get_moduleName_p();
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

My_DOCSTR(_setMenuItemName__doc__, "setMenuItemName",
          "menuID=None, index=0, name='New Name'",
          "menuID:Optional[XPLMMenuID]=None, index:int=0, name:str='New Name'",
          "None",
          "Change the name of and existing menu item.");
static PyObject *XPLMSetMenuItemNameFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("menuID"), CHAR("index"), CHAR("name"), nullptr};
  (void)self;
  PyObject *menuID = Py_None;
  const char *inItemName = "New Name";
  int inIndex = 0;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|Ois", keywords, &menuID, &inIndex, &inItemName)){
    return nullptr;
  }
  XPLMMenuID inMenu = menuID == Py_None ? XPLMFindPluginsMenu() : getVoidPtr(menuID, "XPLMMenuID");
  if (inMenu == XPLMFindPluginsMenu()) {
    PyObject *pluginSelf = get_moduleName_p();
    PyObject *idxList = PyDict_GetItem(menuPluginIdxDict, pluginSelf);
    if (! idxList) {
      Py_DECREF(pluginSelf);
      // pythonLog("XPLMSetMenuItemName, no menu items exist.");
      PyErr_SetString(PyExc_IndexError, "list index out of range");
      Py_RETURN_NONE;
    }
    Py_DECREF(pluginSelf);

    PyObject *xplmIndex = PyList_GetItem(idxList, inIndex);  // throws IndexError
    // pythonLog(" Index %d -> %ld", inIndex, PyLong_AsLong(xplmIndex));
    XPLMSetMenuItemName(inMenu, PyLong_AsLong(xplmIndex), inItemName, 0);
  } else {
    XPLMSetMenuItemName(inMenu, inIndex, inItemName, 0);
  }
  Py_RETURN_NONE;
}

My_DOCSTR(_checkMenuItem__doc__, "checkMenuItem",
          "menuID=None, index=0, checked=Menu_Checked",
          "menuID:Optional[XPLMMenuID]=None, index:int=0, checked:XPLMMenuCheck=Menu_Checked",
          "None",
          "Set checkmark for given menu item.\n"
          "\n"
          "  Menu_NoCheck = 0\n"
          "  Menu_UnChecked = 1\n"
          "  Menu_Checked = 2"
          );
static PyObject *XPLMCheckMenuItemFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("menuID"), CHAR("index"), CHAR("checked"), nullptr};
  (void)self;
  PyObject *menuID = Py_None;
  int inIndex=0;
  int inCheck = xplm_Menu_Checked;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|Oii", keywords, &menuID, &inIndex, &inCheck)){
    return nullptr;
  }
  XPLMMenuID inMenu = menuID == Py_None ? XPLMFindPluginsMenu() : getVoidPtr(menuID, "XPLMMenuID");
  if (inMenu == XPLMFindPluginsMenu()) {
    PyObject *pluginSelf = get_moduleName_p();
    PyObject *idxList = PyDict_GetItem(menuPluginIdxDict, pluginSelf);
    if (! idxList) {
      Py_DECREF(pluginSelf);
      // pythonLog("XPLMCheckMenuItem, no menu items exist.");
      PyErr_SetString(PyExc_IndexError, "list index out of range");
      Py_RETURN_NONE;
    }
    Py_DECREF(pluginSelf);

    PyObject *xplmIndex = PyList_GetItem(idxList, inIndex);  // throws IndexError
    // pythonLog(" Index %d -> %ld", inIndex, PyLong_AsLong(xplmIndex));
    XPLMCheckMenuItem(inMenu, PyLong_AsLong(xplmIndex), inCheck);
  } else {
    XPLMCheckMenuItem(inMenu, inIndex, inCheck);
  }
  Py_RETURN_NONE;
}

My_DOCSTR(_checkMenuItemState__doc__, "checkMenuItemState",
          "menuID=None, index=0",
          "menuID:Optional[XPLMMenuID], index:int=0",
          "int",
          "Returns menu item checked state.\n"
          "\n"
          "  Menu_NoCheck = 0\n"
          "  Menu_UnChecked = 1\n"
          "  Menu_Checked = 2"
          );
static PyObject *XPLMCheckMenuItemStateFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("menuID"), CHAR("index"), nullptr};
  (void)self;
  PyObject *menuID=Py_None;
  int inIndex=0;
  int outCheck;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|Oi", keywords, &menuID, &inIndex)){
    return nullptr;
  }
  XPLMMenuID inMenu = menuID == Py_None ? XPLMFindPluginsMenu() : getVoidPtr(menuID, "XPLMMenuID");
  if (inMenu == XPLMFindPluginsMenu()) {
    PyObject *pluginSelf = get_moduleName_p();
    PyObject *idxList = PyDict_GetItem(menuPluginIdxDict, pluginSelf);
    if (! idxList) {
      Py_DECREF(pluginSelf);
      // pythonLog("XPLMCheckMenuItemState, no menu items exist.");
      PyErr_SetString(PyExc_IndexError, "list index out of range");
      Py_RETURN_NONE;
    }
    Py_DECREF(pluginSelf);

    PyObject *xplmIndex = PyList_GetItem(idxList, inIndex);  // throws IndexError
    // pythonLog(" Index %d -> %ld", inIndex, PyLong_AsLong(xplmIndex));
    // pythonLogFlush();
    XPLMCheckMenuItemState(inMenu, PyLong_AsLong(xplmIndex), &outCheck);
  } else {
    XPLMCheckMenuItemState(inMenu, inIndex, &outCheck);
  }
  return PyLong_FromLong(outCheck);
}

My_DOCSTR(_enableMenuItem__doc__, "enableMenuItem",
          "menuID=None, index=0, enabled=1",
          "menuID:Optional[XPLMMenuID]=None, index:int=0, enabled:int=1",
          "None",
          "Enables menu item\n"
          "\n"
          "Use enabled=0 to disable item, 1 to enable");
static PyObject *XPLMEnableMenuItemFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("menuID"), CHAR("index"), CHAR("enabled"), nullptr};
  (void)self;
  PyObject *menuID = Py_None;
  int inIndex=0;
  int enabled=1;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|Oii", keywords, &menuID, &inIndex, &enabled)){
    return nullptr;
  }
  XPLMMenuID inMenu = menuID == Py_None ? XPLMFindPluginsMenu() : getVoidPtr(menuID, "XPLMMenuID");
  if (inMenu == XPLMFindPluginsMenu()) {
    PyObject *pluginSelf = get_moduleName_p();
    PyObject *idxList = PyDict_GetItem(menuPluginIdxDict, pluginSelf);
    if (! idxList) {
      Py_DECREF(pluginSelf);
      // pythonLog("XPLMEnableMenuItem, no menu items exist.");
      PyErr_SetString(PyExc_IndexError, "list index out of range");
      Py_RETURN_NONE;
    }
    Py_DECREF(pluginSelf);

    PyObject *xplmIndex = PyList_GetItem(idxList, inIndex);  // throws IndexError
    // pythonLog(" Index %d -> %ld", inIndex, PyLong_AsLong(xplmIndex));
    // pythonLogFlush();
    XPLMEnableMenuItem(inMenu, PyLong_AsLong(xplmIndex), enabled);
  } else {
    XPLMEnableMenuItem(inMenu, inIndex, enabled);
  }
  Py_RETURN_NONE;
}

My_DOCSTR(_removeMenuItem__doc__, "removeMenuItem",
          "menuID=None, index=0",
          "menuID:Optional[XPLMMenuID]=None, index:int=0",
          "None",
          "Removes one item from menu.\n"
          "\n"
          "Note that all menu items below are moved up one index.");
static PyObject *XPLMRemoveMenuItemFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("menuID"), CHAR("index"), nullptr};
  (void)self;
  PyObject *menuID = Py_None;
  int inIndex=0;
  if(!XPLMRemoveMenuItem_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMRemoveMenuItem is available only in XPLM210 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|Oi", keywords, &menuID, &inIndex)){
    return nullptr;
  }
  XPLMMenuID inMenu = menuID == Py_None ? XPLMFindPluginsMenu() : getVoidPtr(menuID, "XPLMMenuID");
  if (inMenu == XPLMFindPluginsMenu()) {
    PyObject *pluginSelf = get_moduleName_p();
    PyObject *idxList = PyDict_GetItem(menuPluginIdxDict, pluginSelf);
    if (! idxList) {
      Py_DECREF(pluginSelf);
      // pythonLog("XPLMRemoveMenuItem, no menu items exist.");
      PyErr_SetString(PyExc_IndexError, "list index out of range");
      Py_RETURN_NONE;
    }

    PyObject *xplmIndex = PyList_GetItem(idxList, inIndex);  // throws IndexError
    
    if (!xplmIndex) {
      // pythonLog("Bad result from PyListGetItem()");
      Py_DECREF(pluginSelf);
      pythonLogException();
      Py_RETURN_NONE;
    }
    // pythonLog(" Index %d -> %ld", inIndex, PyLong_AsLong(xplmIndex));
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

    MyPyRun_String("xplm=m[p].pop(x)\nfor k,l in m.items():\n    m[k]=[(x if x<xplm else x-1) for x in l]", Py_file_input, localsDict, localsDict);
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

  // Clean up menu callbacks
  for (auto& pair : menuCallbacks) {
    if (pair.second.callback != Py_None) {
      Py_DECREF(pair.second.callback);
    }
    Py_DECREF(pair.second.refCon);
  }
  menuCallbacks.clear();

  PyDict_Clear(menuRefDict);
  Py_DECREF(menuRefDict);
  for (auto& pair : menuCapsules) {
    Py_DECREF(pair.second);
  }
  menuCapsules.clear();
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
  {nullptr, nullptr, 0, nullptr}
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
  nullptr,
  nullptr,
  nullptr,
  nullptr
};

PyMODINIT_FUNC
PyInit_XPLMMenus(void)
{
  // menuCallbacks is now a C++ unordered_map, no need to initialize
  // PyDict_SetItemString(XPY3pythonDicts, "menus", menuCallbacks);
  if(!(menuRefDict = PyDict_New())){
    return nullptr;
  }
  PyDict_SetItemString(XPY3pythonDicts, "menuRefs", menuRefDict);
  // menuCapsules is now a C++ unordered_map, no need to initialize
  // PyDict_SetItemString(XPY3pythonCapsules, menuIDRef, menuCapsules);
  if(!(menuPluginIdxDict = PyDict_New())){
    return nullptr;
  }
  PyDict_SetItemString(XPY3pythonDicts, "menuPluginIdx", menuPluginIdxDict);

  PyObject *mod = PyModule_Create(&XPLMMenusModule);
  if(mod){
    PyModule_AddStringConstant(mod, "__author__", "Peter Buckner (pbuck@xppython3.org)");
    PyModule_AddIntConstant(mod, "xplm_Menu_NoCheck", xplm_Menu_NoCheck); // XPLMMenuCheck
    PyModule_AddIntConstant(mod, "xplm_Menu_Unchecked", xplm_Menu_Unchecked); // XPLMMenuCheck
    PyModule_AddIntConstant(mod, "xplm_Menu_Checked", xplm_Menu_Checked); // XPLMMenuCheck

    PyModule_AddIntConstant(mod, "Menu_NoCheck", xplm_Menu_NoCheck); // XPLMMenuCheck
    PyModule_AddIntConstant(mod, "Menu_Unchecked", xplm_Menu_Unchecked); // XPLMMenuCheck
    PyModule_AddIntConstant(mod, "Menu_Checked", xplm_Menu_Checked); // XPLMMenuCheck
  }

  return mod;
}


