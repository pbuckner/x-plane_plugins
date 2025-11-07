#define _GNU_SOURCE 1
#include <Python.h>
#include <sys/time.h>
#include <stdio.h>
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMMenus.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include "utils.h"
#include "plugin_dl.h"
#include "menus.h"
#include "capsules.h"

static intptr_t menuRefConCntr;
struct MenuInfo {
  std::string name;
  const char* module_name;
  XPLMMenuID parent_id;
  int parent_item;
  PyObject *callback;
  PyObject *refCon;
  PyObject *capsule;
};

static std::unordered_map<intptr_t, MenuInfo> menuCallbacks;
static std::unordered_map<XPLMMenuID, intptr_t> menuRefMap;  // XPLMMenuID -> refcon (key into menuCallbacks)
static std::unordered_map<std::string, std::vector<int>> menuPluginIdxMap; // plugin -> [list of Laminar menu IDs]

static int nextXPLMMenuIdx = 0;

void resetMenus() {
  /* NOTE: stopping an instance, which is done (also) on reload
     calls xpy_cleanUpInstance, which already clears menus
     clears menuPluginIdxMap, menuDict, and menuRefMap
  */
  nextXPLMMenuIdx = 0;
  if (menuPluginIdxMap.size() > 0) {
    pythonLog("OOPS, menuPluginIdxMap is %zu", menuPluginIdxMap.size());
  }
  if (menuCallbacks.size() > 0) {
    pythonLog("OOPS, menuCallbacks is %zu", menuCallbacks.size());
  }
  if (menuRefMap.size() > 0) {
    pythonLog("OOPS, menuRefMap is %zu", menuRefMap.size());
  }
}

static void genericMenuHandler(void * menuRefCon, void * inItemRef)
{
  errCheck("prior genericMenuHandler");
  intptr_t refcon_id = (intptr_t)menuRefCon;
  auto it = menuCallbacks.find(refcon_id);
  if (it == menuCallbacks.end()) {
    pythonLog("Unknown callback requested in genericMenuHandler(%p).", menuRefCon);
    return;
  }

  MenuInfo& info = it->second;
  set_moduleName(info.module_name);

  PyObject *args[] = {info.refCon, (PyObject*)inItemRef};
    PyObject *res = PyObject_Vectorcall(info.callback, args, 2, nullptr);
  errCheck("end genericMenuHandler");
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
  PyObject *parentMenu = Py_None, *pythonHandler = Py_None, *refCon = Py_None;
  int inParentItem = 0;
  const char *inName = nullptr;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|sOiOO", keywords, &inName, &parentMenu, &inParentItem, &pythonHandler, &refCon)){
    return nullptr;
  }
  std::string moduleName_str(CurrentPythonModuleName);
  if (getVoidPtr(parentMenu, "XPLMMenuID") == XPLMFindPluginsMenu()) {
    auto it = menuPluginIdxMap.find(moduleName_str);
    if (it == menuPluginIdxMap.end() || inParentItem >= (int)it->second.size()) {
      // pythonLog("XPLMCreateMenu, no menu items exist.");
      PyErr_SetString(PyExc_IndexError, "list index out of range");
      Py_RETURN_NONE;
    }
    inParentItem = it->second[inParentItem];
  }
  if (inName == nullptr && parentMenu == Py_None) {
    inName = CurrentPythonModuleName;
  }
  
  intptr_t refcon_id = ++menuRefConCntr;

  XPLMMenuHandler_f handler = (pythonHandler != Py_None) ? genericMenuHandler : nullptr;
  XPLMMenuID rawMenuID = XPLMCreateMenu(inName, getVoidPtr(parentMenu, "XPLMMenuID"),
                                        inParentItem, handler, (void*)refcon_id);
  if(!rawMenuID){
    Py_RETURN_NONE;
  }

  PyObject *menuID = makeCapsule(rawMenuID, "XPLMMenuID");

  // Store menu info in C++ map
  menuCallbacks[refcon_id] = {
    .name = std::string(inName ? inName : ""),
    .module_name = CurrentPythonModuleName,
    .parent_id = getVoidPtr(parentMenu, "XPLMMenuID"),
    .parent_item = inParentItem,
    .callback = pythonHandler,
    .refCon = refCon,
    .capsule = menuID
  };
  if (pythonHandler != Py_None) {
    Py_INCREF(pythonHandler);
  }
  Py_INCREF(menuID);
  Py_INCREF(refCon);

  if (parentMenu == Py_None) {
    /* we need to store and update index (we don't return index
       -- only the <capsuled>XPLMMenuID is returned */
    menuPluginIdxMap[moduleName_str].push_back(nextXPLMMenuIdx);
    // pythonLog("Create menu's idx is not provided, but next is [%d] (we don't return this)", nextXPLMMenuIdx);
    nextXPLMMenuIdx++;
  }

  menuRefMap[rawMenuID] = refcon_id;
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
  XPLMMenuID id = getVoidPtr(menuID, "XPLMMenuID");
  auto refMap_it = menuRefMap.find(id);
  if(refMap_it == menuRefMap.end()){
    pythonLog("Unknown menuID passed to XPLMDestroyMenu, ignored.");
  } else {
    intptr_t refcon_id = refMap_it->second;
    auto it = menuCallbacks.find(refcon_id);
    if (it != menuCallbacks.end()) {
      if (it->second.callback != Py_None) {
        Py_DECREF(it->second.callback);
      }
      Py_DECREF(it->second.capsule);
      Py_DECREF(it->second.refCon);
      menuCallbacks.erase(it);
    }

    menuRefMap.erase(refMap_it);

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
    clearInstanceMenuItems(CurrentPythonModuleName);
  } else {
    /* char *s = objToStr(menuID); */
    /* pythonLog("Clearing item from menu: %s", s); */
    /* free(s); */
    XPLMClearAllMenuItems(getVoidPtr(menuID, "XPLMMenuID"));
  }

  Py_RETURN_NONE;
}

void clearInstanceMenuItems(char *moduleName) {
  /* For a particular (python) plugin:
     Get list of plugin's top-level menu indices  (--> menuPluginIdxMap[plugin])
        e.g., menuPluginIdxMap["myplugin"] = [3, 5, 6]
     ... we'll delete these, but first go through ALL
     menuPluginIdx items and shift existing indices "up" (to a small index number)
     if the existing index is greater than a index in the deletion list (e.g., '[3, 5, 6]')
   */
  pythonDebug("%*s Clearing top-level menu items for %s", 6, " ", moduleName);

  std::string moduleName_str(moduleName);
  auto it = menuPluginIdxMap.find(moduleName_str);
  if (it == menuPluginIdxMap.end()) {
    pythonDebug("%*s No menu items for %s", 8, " ", moduleName);
    return;
  }

  /* l --> list of this plugins "laminar" menu indices, from high-to-low */
  std::vector<int> l = it->second;
  std::reverse(l.begin(), l.end());

  pythonDebug("%*s List of menu ids for this plugin has %zu items", 8, " ", l.size());

  /* iterate through _all_ plugins menu indices...
     If we're viewing _this_ plugin, set list to []
     otherwise, decrement the (other) plugin's menu idx by one, if it's larger than
     one of the idx we're removing */
  for (auto& [key, indices] : menuPluginIdxMap) {
    if (key == moduleName_str) {
      indices.clear();
    } else {
      for (int n : l) {
        for (int& x : indices) {
          if (x >= n) x--;
        }
      }
    }
  }

  // Update nextXPLMMenuIdx
  nextXPLMMenuIdx = nextXPLMMenuIdx - l.size();

  // Remove menu items from X-Plane
  for (int idx : l) {
    XPLMRemoveMenuItem_ptr(XPLMFindPluginsMenu(), idx);
  }

  pythonDebug("%*s Clearing menu", 8, " ");

  for (auto it = menuCallbacks.begin(); it != menuCallbacks.end();) {
    MenuInfo& info = it->second;
    if (0 == strcmp(info.module_name, moduleName)) {
      pythonDebug("%*s, Found menu[] for plugin, %s", 10, " ", info.name.c_str());

      // Find corresponding menuRefMap entry and remove it
      XPLMMenuID menuID = nullptr;
      for (auto refMap_it = menuRefMap.begin(); refMap_it != menuRefMap.end(); ++refMap_it) {
        if (refMap_it->second == it->first) {
          menuID = refMap_it->first;
          pythonDebug("%*s Clearing menu items for %lld", 10, " ", it->first);
          XPLMClearAllMenuItems(menuID);
          XPLMDestroyMenu(menuID);
          menuRefMap.erase(refMap_it);
          break;
        }
      }

      // Clean up Python objects
      if (info.callback != Py_None) {
        Py_DECREF(info.callback);
      }
      Py_DECREF(info.capsule);
      Py_DECREF(info.refCon);
      it = menuCallbacks.erase(it);
    } else {
      ++it;
    }
  }

  // Remove from menuPluginIdxMap if empty
  auto idx_it = menuPluginIdxMap.find(moduleName_str);
  if (idx_it != menuPluginIdxMap.end() && idx_it->second.empty()) {
    menuPluginIdxMap.erase(idx_it);
  }
  pythonDebug("%*s Cleared top-level menu items for %s", 8, " ", moduleName);
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
    std::string moduleName_str(CurrentPythonModuleName);
    menuPluginIdxMap[moduleName_str].push_back(nextXPLMMenuIdx);
    int pluginIdx = menuPluginIdxMap[moduleName_str].size() - 1;
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
    std::string moduleName_str(CurrentPythonModuleName);
    menuPluginIdxMap[moduleName_str].push_back(nextXPLMMenuIdx);
    int pluginIdx = menuPluginIdxMap[moduleName_str].size() - 1;
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
    std::string moduleName_str(CurrentPythonModuleName);
    menuPluginIdxMap[moduleName_str].push_back(nextXPLMMenuIdx);
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
          "Change the name of an existing menu item.");
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
    std::string moduleName_str(CurrentPythonModuleName);
    auto it = menuPluginIdxMap.find(moduleName_str);
    if (it == menuPluginIdxMap.end() || inIndex >= (int)it->second.size()) {
      // pythonLog("XPLMSetMenuItemName, no menu items exist.");
      PyErr_SetString(PyExc_IndexError, "list index out of range");
      Py_RETURN_NONE;
    }
    // pythonLog(" Index %d -> %d", inIndex, it->second[inIndex]);
    XPLMSetMenuItemName(inMenu, it->second[inIndex], inItemName, 0);
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
          "  Menu_Unchecked = 1\n"
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
    std::string moduleName_str(CurrentPythonModuleName);
    auto it = menuPluginIdxMap.find(moduleName_str);
    if (it == menuPluginIdxMap.end() || inIndex >= (int)it->second.size()) {
      // pythonLog("XPLMCheckMenuItem, no menu items exist.");
      PyErr_SetString(PyExc_IndexError, "list index out of range");
      Py_RETURN_NONE;
    }
    // pythonLog(" Index %d -> %d", inIndex, it->second[inIndex]);
    XPLMCheckMenuItem(inMenu, it->second[inIndex], inCheck);
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
          "  Menu_Unchecked = 1\n"
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
    std::string moduleName_str(CurrentPythonModuleName);
    auto it = menuPluginIdxMap.find(moduleName_str);
    if (it == menuPluginIdxMap.end() || inIndex >= (int)it->second.size()) {
      // pythonLog("XPLMCheckMenuItemState, no menu items exist.");
      PyErr_SetString(PyExc_IndexError, "list index out of range");
      Py_RETURN_NONE;
    }
    // pythonLog(" Index %d -> %d", inIndex, it->second[inIndex]);
    XPLMCheckMenuItemState(inMenu, it->second[inIndex], &outCheck);
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
    std::string moduleName_str(CurrentPythonModuleName);
    auto it = menuPluginIdxMap.find(moduleName_str);
    if (it == menuPluginIdxMap.end() || inIndex >= (int)it->second.size()) {
      // pythonLog("XPLMEnableMenuItem, no menu items exist.");
      PyErr_SetString(PyExc_IndexError, "list index out of range");
      Py_RETURN_NONE;
    }
    // pythonLog(" Index %d -> %d", inIndex, it->second[inIndex]);
    XPLMEnableMenuItem(inMenu, it->second[inIndex], enabled);
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
    std::string moduleName_str(CurrentPythonModuleName);
    auto it = menuPluginIdxMap.find(moduleName_str);
    if (it == menuPluginIdxMap.end() || inIndex >= (int)it->second.size()) {
      // pythonLog("XPLMRemoveMenuItem, no menu items exist.");
      PyErr_SetString(PyExc_IndexError, "list index out of range");
      Py_RETURN_NONE;
    }

    // Get the XPLM index we're removing
    int xplmIndex = it->second[inIndex];

    // pythonLog(" Index %d -> %d", inIndex, xplmIndex);
    XPLMRemoveMenuItem_ptr(inMenu, xplmIndex);
    nextXPLMMenuIdx--;

    /* First delete the one we're removing from this plugin's list */
    it->second.erase(it->second.begin() + inIndex);

    /* Now, go through _all_ lists, and any index >= xplmIndex needs to be decremented
       to "shift down" after the removal */
    for (auto& [key, indices] : menuPluginIdxMap) {
      for (int& x : indices) {
        if (x > xplmIndex) x--;
      }
    }
  } else {
    XPLMRemoveMenuItem_ptr(inMenu, inIndex);
  }
  Py_RETURN_NONE;
}

PyObject* buildMenuCallbackDict(void)
{
  PyObject *dict = PyDict_New();
  if (!dict) {
    return nullptr;
  }

  bool error_occurred = false;

  for (const auto& pair : menuCallbacks) {
    const MenuInfo& info = pair.second;
    intptr_t refcon_id = pair.first;

    // Initialize all pointers to nullptr
    PyObject *key = nullptr;
    PyObject *module_name_p = nullptr;
    PyObject *name_p = nullptr;
    PyObject *parent_id_p = nullptr;
    PyObject *parent_item_p = nullptr;
    PyObject *value = nullptr;

    // Create all Python objects
    key = PyLong_FromLong(refcon_id);
    if (!key) {
      error_occurred = true;
      goto cleanup_iteration;
    }

    module_name_p = PyUnicode_FromString(info.module_name);
    if (!module_name_p) {
      error_occurred = true;
      goto cleanup_iteration;
    }

    name_p = PyUnicode_FromString(info.name.c_str());
    if (!name_p) {
      error_occurred = true;
      goto cleanup_iteration;
    }

    parent_id_p = PyLong_FromVoidPtr(info.parent_id);
    if (!parent_id_p) {
      error_occurred = true;
      goto cleanup_iteration;
    }

    parent_item_p = PyLong_FromLong(info.parent_item);
    if (!parent_item_p) {
      error_occurred = true;
      goto cleanup_iteration;
    }

    // Build tuple: (module_name, name, parent_id, parent_item, callback, refCon, capsule)
    value = PyTuple_New(7);
    if (!value) {
      error_occurred = true;
      goto cleanup_iteration;
    }

    // Set tuple items (PyTuple_SetItem steals references)
    PyTuple_SetItem(value, 0, module_name_p);         // steals ref
    module_name_p = nullptr; // Mark as stolen

    PyTuple_SetItem(value, 1, name_p);                // steals ref
    name_p = nullptr; // Mark as stolen

    PyTuple_SetItem(value, 2, parent_id_p);           // steals ref
    parent_id_p = nullptr; // Mark as stolen

    PyTuple_SetItem(value, 3, parent_item_p);         // steals ref
    parent_item_p = nullptr; // Mark as stolen

    Py_INCREF(info.callback);                         // increment for tuple
    PyTuple_SetItem(value, 4, info.callback);         // steals ref

    Py_INCREF(info.refCon);                           // increment for tuple
    PyTuple_SetItem(value, 5, info.refCon);           // steals ref

    Py_INCREF(info.capsule);                          // increment for tuple
    PyTuple_SetItem(value, 6, info.capsule);          // steals ref

    // Add to dictionary (PyDict_SetItem does NOT steal references)
    if (PyDict_SetItem(dict, key, value) < 0) {
      error_occurred = true;
      goto cleanup_iteration;
    }

    // Clean up our references for this iteration
    Py_DECREF(key);
    Py_DECREF(value);
    continue;

cleanup_iteration:
    Py_XDECREF(key);
    Py_XDECREF(module_name_p);
    Py_XDECREF(name_p);
    Py_XDECREF(parent_id_p);
    Py_XDECREF(parent_item_p);
    Py_XDECREF(value);
    break;
  }

  if (error_occurred) {
    Py_DECREF(dict);
    return nullptr;
  }

  return dict;
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
    Py_DECREF(pair.second.capsule);
    Py_DECREF(pair.second.refCon);
  }
  menuCallbacks.clear();
  menuRefMap.clear();
  menuPluginIdxMap.clear();

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
  {"getMenuCallbackDict", (PyCFunction)buildMenuCallbackDict, METH_VARARGS, "Copy of internal MenuCallbackInfo"},
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
  // All menu tracking now uses C++ unordered_map, no Python dicts needed

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


