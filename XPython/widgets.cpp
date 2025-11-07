#define _GNU_SOURCE 1
#include <Python.h>
#include <sys/time.h>
#include <stdio.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMDisplay.h>
#include <Widgets/XPWidgetDefs.h>
#include <Widgets/XPWidgets.h>
#include <Widgets/XPStandardWidgets.h>
#include "plugin_dl.h"
#include "utils.h"
#include "widgetutils.h"
#include "widgets.h"
#include "display.h"
#include "xppython.h"
#include "capsules.h"

struct WidgetCallbackInfo {
  std::vector<PyObject*> callbacks;
  const char* module_name;
};

static std::unordered_map<XPWidgetID, WidgetCallbackInfo> widgetCallbacks;

// Custom hash and equality functors for widget property keys
struct WidgetPropertyHash {
    std::size_t operator()(const std::pair<PyObject*, int>& p) const {
        return std::hash<void*>{}(p.first) ^ (std::hash<int>{}(p.second) << 1);
    }
};

struct WidgetPropertyEqual {
    bool operator()(const std::pair<PyObject*, int>& a, const std::pair<PyObject*, int>& b) const {
        return PyObject_RichCompareBool(a.first, b.first, Py_EQ) == 1 && a.second == b.second;
    }
};

static std::unordered_map<std::pair<PyObject*, int>, PyObject*, WidgetPropertyHash, WidgetPropertyEqual> widgetPropertyDict;

static std::unordered_set<XPWidgetID> widgetSet;

void resetWidgets(void) {
  /* Destroy all tracked widgets */
  for(auto it = widgetSet.begin(); it != widgetSet.end();) {
    XPDestroyWidget(*it, 0);
    it = widgetSet.erase(it);
  }
  for (auto& pair : widgetCallbacks) {
    for (PyObject* callback : pair.second.callbacks) {
      Py_DECREF(callback);
    }
  }
  widgetCallbacks.clear();
  for (auto& pair : widgetPropertyDict) {
    Py_DECREF(pair.second);
  }
  widgetPropertyDict.clear();
}

static void clearChildrenXPWidgetData(PyObject *widget);
static void clearXPWidgetData(PyObject *widget);
int widgetCallback(XPWidgetMessage inMessage, XPWidgetID inWidget, intptr_t inParam1, intptr_t inParam2)
{
  PyObject *err;
  errCheck("Error prior to widgetCallback");
  // struct timespec stop, start;
  struct timespec all_stop, all_start;
  clock_gettime(CLOCK_MONOTONIC, &all_start);

  PyObject *widget = makeCapsule(inWidget, "XPWidgetID");

  auto it = widgetCallbacks.find(inWidget);
  if(it == widgetCallbacks.end()){
    /* we'll get an xpMsg_Create that we can't handle from a CustomWidget (because the widgetCallbacks
       isn't populated yet). Ignore the message (CreateCustomWidget() below will send it again!)
       If not xpMsg_Create, write error.
     */
    if (inMessage != xpMsg_Create && inMessage != xpMsg_AcceptParent) {
      pythonLog("Couldn't find the callback list for widget ID %p. for message %d", inWidget, inMessage);
    }
    Py_DECREF(widget);
    err = PyErr_Occurred();
    if(err){
      pythonLog("Error after message where no callbackList for msg %d", inMessage);
      pythonLogException();
    }
    return 0;
  }

  WidgetCallbackInfo& callbackInfo = it->second;
  set_moduleName(callbackInfo.module_name);
  int pluginIndex = getPluginIndex(); // gotta 'save' this for stats below
  errCheck("Error after makeCapsule");
  
  PyObject *param1, *param2;
  XPKeyState_t *keyState;
  XPMouseState_t *mouseState;
  XPWidgetGeometryChange_t *wChange;

  switch(inMessage){
  case xpMsg_KeyPress:
    keyState = (XPKeyState_t *)inParam1;
    param1 = Py_BuildValue("(iii)", (int)keyState->key, (int)keyState->flags,
                           (int)keyState->vkey);
    param2 = PyLong_FromLong(inParam2);
    break;
  case xpMsg_MouseDown:
  case xpMsg_MouseDrag:
  case xpMsg_MouseUp:
  case xpMsg_MouseWheel:
  case xpMsg_CursorAdjust:
    mouseState = (XPMouseState_t *)inParam1;
    param1 = Py_BuildValue("(iiii)", mouseState->x, mouseState->y,
                           mouseState->button, mouseState->delta);
    param2 = PyLong_FromLong(inParam2);
    if (inMessage == xpMsg_CursorAdjust) {
      PyObject *val = param2;
      param2 = PyList_New(1);
      PyList_SetItem(param2, 0, val);
    }
    break;
  case xpMsg_Reshape:
    param1 =  makeCapsule((void *)inParam1, "XPWidgetID");
    wChange = (XPWidgetGeometryChange_t *)inParam2;
    param2 = Py_BuildValue("(iiii)", wChange->dx, wChange->dy,
                           wChange->dwidth, wChange->dheight);
    break;
  case xpMsg_AcceptChild:
  case xpMsg_LoseChild:
  case xpMsg_AcceptParent:
  case xpMsg_Shown:
  case xpMsg_Hidden:
  case xpMsg_TextFieldChanged:
  case xpMsg_PushButtonPressed:
  case xpMsg_ButtonStateChanged:
  case xpMsg_ScrollBarSliderPositionChanged:
    param1 =  makeCapsule((void *)inParam1, "XPWidgetID");
    param2 = PyLong_FromLong(inParam2);
    break;
    
  case xpMsg_PropertyChanged:
    param1 = PyLong_FromLong(inParam1);
    if (inParam1 >= xpProperty_UserStart) {
      // use inParam2 -- it's already python
      param2 = (PyObject*)inParam2;
    } else {
      param2 = PyLong_FromLong(inParam2);
    }
    break;
  default: // all other messages
    param1 = PyLong_FromLong(inParam1);
    param2 = PyLong_FromLong(inParam2);
    break;
  }

  errCheck("Error after message parse for msg %d", inMessage);

  /* as we prepare to call the C or Python callback function
     we _save_ CurrentPythonModuleName. We know it's supposed
     to be the same value (point to same location) before
     and after any call.

     However, we also know that this may result in a destroy
     message being sent. The destroy will (eventually) come back
     here and delete the callbackInfo.module_name, negating
     CurrentPythonmoduleName which may be pointing to memory
     within callbackInfo hash.

     Once I'm in Destroy, it's (may be) late. If a plugin calls
     Destroy directly, I can catch it -- CurrentPythonModuleName
     is the calling plugin, so save and restore works.

     But, if I'm here, the Destroy message may be sent to a widget
     _but_not_directly_using_my XPLMDestroyFun, so I've not been
     about to save/restore it.
     
  */

  int res;
  for(size_t i = 0; i < callbackInfo.callbacks.size(); ++i){
    err = PyErr_Occurred();
    if(err){
      pythonLog("Error in widget callbacklist [%zu] for msg %d", i, inMessage);
      pythonLogException();
    }

    PyObject *callback = callbackInfo.callbacks[i];
    //Have to differentiate between python callbacks and "binary" function callbacks
    // (like the ones returned by XPGetWidgetClassFunc)
    if(PyLong_Check(callback)){
      XPWidgetFunc_t cFunc = (XPWidgetFunc_t)PyLong_AsVoidPtr(callback);
      // clock_gettime(CLOCK_MONOTONIC, &start);
      res = cFunc(inMessage, inWidget, inParam1, inParam2);
      // clock_gettime(CLOCK_MONOTONIC, &stop);
    }else{
      PyObject *inMessageObj = PyLong_FromLong(inMessage);
      // clock_gettime(CLOCK_MONOTONIC, &start);
      PyObject *args[] = {inMessageObj, widget, param1, param2};
      PyObject *resObj = PyObject_Vectorcall(callback, args, 4, nullptr);
      // clock_gettime(CLOCK_MONOTONIC, &stop);
      Py_DECREF(inMessageObj);
      if(!resObj || resObj == Py_None){
        char *s2 = objToStr(callback);
        pythonLog("[%s] Widget Callback function %s did not return a value", CurrentPythonModuleName, s2);
        free(s2);
        break;
      }
      res = PyLong_AsLong(resObj);
      Py_DECREF(resObj);
    }
    if(res != 0){
      if(inMessage == xpMsg_CursorAdjust){
        *(XPLMCursorStatus *)inParam2 = (int)PyLong_AsLong(PyList_GetItem(param2, 0));
      }
      break;
    }
  }
  err = PyErr_Occurred();
  if(err){
    pythonLog("Error in widget post callbacklist for msg %d", inMessage);
    pythonLogException();
  }

  if(inMessage == xpMsg_Destroy){
    auto it = widgetCallbacks.find(inWidget);
    if (it != widgetCallbacks.end()) {
      for (PyObject* callback : it->second.callbacks) {
        Py_DECREF(callback);
      }
      widgetCallbacks.erase(it);
    }
  }

  Py_DECREF(widget);
  Py_DECREF(param1);
  if (inMessage != xpMsg_PropertyChanged || inParam1 < xpProperty_UserStart) {
    Py_DECREF(param2);
  }
  clock_gettime(CLOCK_MONOTONIC, &all_stop);
  /* ( we get better times, per plugin, when we include the full execution: just recording
     callback execution misses much of what the custom widget actually processes. Sadly
     it appears we cannot match what Laminar is collecting internally.
  */
  pluginStats[pluginIndex].customw_time += (all_stop.tv_sec - all_start.tv_sec) * 1000000 + (all_stop.tv_nsec - all_start.tv_nsec) / 1000;
  pluginStats[0].customw_time += (all_stop.tv_sec - all_start.tv_sec) * 1000000 + (all_stop.tv_nsec - all_start.tv_nsec) / 1000;
  err = PyErr_Occurred();
  if(err){
    pythonLog("Error at end of widget callback for msg %d", inMessage);
    pythonLogException();
  }
  return res;
}



My_DOCSTR(_createWidget__doc__, "createWidget",
          "left, top, right, bottom, visible, descriptor, isRoot, container, widgetClass",
          "left:int, top:int, right:int, bottom:int, visible:int, descriptor:str, isRoot:int, "
          "container:XPWidgetID | Literal[0], widgetClass:XPWidgetClass",
          "XPWidgetID",
          "Create widget of class at location\n"
          "\n"
          "isRoot=1 if widget is a root widget, container is None or widgetID of parent widget\n"
          "widgetClass is one of predefined classes:\n"
          "  WidgetClass_MainWindow\n"
          "  WidgetClass_SubWindow\n"
          "  WidgetClass_Button\n"
          "  WidgetClass_TextField\n"
          "  WidgetClass_ScrollBar\n"
          "  WidgetClass_Caption\n"
          "  WidgetClass_GeneralGraphics\n"
          "  WidgetClass_Progress\n"
          "Returns created widgetID"
          );
static PyObject *XPCreateWidgetFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  errCheck("prior createWidget");
  static char *keywords[] = {CHAR("left"), CHAR("top"), CHAR("right"), CHAR("bottom"), CHAR("visible"), CHAR("descriptor"), CHAR("isRoot"), CHAR("container"), CHAR("widgetClass"), nullptr};
  (void) self;
  int inLeft, inTop, inRight, inBottom, inVisible, inIsRoot;
  const char *inDescriptor;
  PyObject *container;
  XPWidgetClass inClass;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "iiiiisiOi", keywords, &inLeft, &inTop, &inRight, &inBottom, &inVisible, &inDescriptor, &inIsRoot,
                                         &container, &inClass)){
    return nullptr;
  }
  // use inContainer 0, if passed in value of 0
  XPWidgetID inContainer;
  if (inIsRoot > 0) {
    inContainer = 0;
  } else if ((PyLong_Check(container) && PyLong_AsLong(container) == 0) || container == Py_None) {
    inContainer = 0;
  } else {
    inContainer = getVoidPtr(container, "XPWidgetID");
  }

  XPWidgetID res = XPCreateWidget(inLeft, inTop, inRight, inBottom, inVisible, inDescriptor, inIsRoot, inContainer, inClass);

  // Add to widget tracking set
  widgetSet.insert(res);

  PyObject *ret = makeCapsule(res, "XPWidgetID");
  errCheck("end createWidget");
  return ret;
}

My_DOCSTR(_createCustomWidget__doc__, "createCustomWidget",
          "left, top, right, bottom, visible, descriptor, isRoot, container, callback",
          "left:int, top:int, right:int, bottom:int, visible:int, descriptor:str, isRoot:int, container:Optional[XPWidgetID], "
          "callback:Callable[[XPWidgetMessage, XPWidgetID, int, int], int]",
          "XPWidgetID",
          "Create widget at location, with custom callback\n"
          "\n"
          "callback is (message, widget, param1, param2) returning 1 if you've handled\n"
          "   the message, 0 otherwise.\n"
          "Returns created widgetID");
static PyObject *XPCreateCustomWidgetFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("left"), CHAR("top"), CHAR("right"), CHAR("bottom"), CHAR("visible"), CHAR("descriptor"), CHAR("isRoot"), CHAR("container"), CHAR("callback"), nullptr};
  (void) self;
  int inLeft, inTop, inRight, inBottom, inVisible, inIsRoot;
  const char *inDescriptor;
  PyObject *container;
  PyObject *inCallback;
  errCheck("error at start of CreateCustomWidget");
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "iiiiisiOO", keywords, &inLeft, &inTop, &inRight, &inBottom, &inVisible, &inDescriptor,
                       &inIsRoot, &container, &inCallback)){
    return nullptr;
  }
  // use inContainer 0, if passed in value of 0
  XPWidgetID inContainer;
  if (inIsRoot > 0) {
    inContainer = 0;
  } else if ((PyLong_Check(container) && PyLong_AsLong(container) == 0) || container == Py_None) {
    inContainer = 0;
  } else {
    inContainer = getVoidPtr(container, "XPWidgetID");
  }

  /* vvvvvvvvvvvvvvvvvv widgetCallback will be immediately called with Create msg BUT
       widgetCallbacks does not yet have entry for this widget, so the create msg
       will not be received by this function.
       So... we populate the map and then call SendMessageToWidget directly!
   */
  XPWidgetID res = XPCreateCustomWidget(inLeft, inTop, inRight, inBottom, inVisible, inDescriptor, inIsRoot,
                                        inContainer, widgetCallback);

  // Add to widget tracking set
  widgetSet.insert(res);

  PyObject *resObj = makeCapsule(res, "XPWidgetID");

  WidgetCallbackInfo callbackInfo;
  callbackInfo.module_name = CurrentPythonModuleName;
  callbackInfo.callbacks.push_back(inCallback);
  Py_INCREF(inCallback);
  widgetCallbacks[res] = std::move(callbackInfo);
  errCheck("error near end of CreateCustomWidget");
  XPSendMessageToWidget(res, xpMsg_Create, xpMode_Direct, 0, 0);
  XPSendMessageToWidget(res, xpMsg_AcceptParent, xpMode_Direct, (intptr_t)inContainer, 0);
  errCheck("error at end of CreateCustomWidget");
  return resObj;
}

My_DOCSTR(_destroyWidget__doc__, "destroyWidget",
          "widgetID, destroyChildren=1",
          "widgetID:XPWidgetID, destroyChildren:int=1",
          "None",
          "Destroys widgetID and (optionally) all children.");
static PyObject *XPDestroyWidgetFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("widgetID"), CHAR("destroyChildren"), nullptr};
  (void) self;
  PyObject *widget;
  int inDestroyChildren=1;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|i", keywords, &widget, &inDestroyChildren)){
    return nullptr;
  }
  XPWidgetID wid = getVoidPtr(widget, "XPWidgetID");
  if (wid == nullptr) return nullptr;
  widgetSet.erase(wid);
  XPDestroyWidget(wid, inDestroyChildren);
  if (inDestroyChildren) {
    clearChildrenXPWidgetData(widget);
  }
  clearXPWidgetData(widget);
  Py_RETURN_NONE;
}


static void clearChildrenXPWidgetData(PyObject *widget) {
  XPWidgetID wid = getVoidPtr(widget, "XPWidgetID");
  int numChildren = XPCountChildWidgets(wid);
  pythonDebug("Clearing children xp widget data [%d] for %s", numChildren, objDebug(widget));
  for (int i=0; i<numChildren; i++) {
    PyObject *child_widget = makeCapsule(XPGetNthChildWidget(wid, i), "XPWidgetID");
    clearChildrenXPWidgetData(child_widget);
    clearXPWidgetData(child_widget);
  }
}

static void clearXPWidgetData(PyObject *widget) {
  pythonDebug(" ... clearing xp widgetdata for %s", objDebug(widget));
  XPWidgetID wid = getVoidPtr(widget, "XPWidgetID");
  auto it = widgetCallbacks.find(wid);
  if (it != widgetCallbacks.end()) {
    pythonDebug(" widget found in callback dict");
    for (PyObject* callback : it->second.callbacks) {
      Py_DECREF(callback);
    }
    widgetCallbacks.erase(it);
  }

  /* Remove all properties for this widget */
  for (auto it = widgetPropertyDict.begin(); it != widgetPropertyDict.end();) {
    if (PyObject_RichCompareBool(it->first.first, widget, Py_EQ) == 1) {
      pythonDebug("   deleting widget property: %d", it->first.second);
      Py_DECREF(it->second);
      it = widgetPropertyDict.erase(it);
    } else {
      ++it;
    }
  }
}

My_DOCSTR(_sendMessageToWidget__doc__, "sendMessageToWidget",
          "widgetID, message, dispatchMode=1, param1=0, param2=0",
          "widgetID:XPWidgetID, message:XPWidgetMessage | int, dispatchMode:XPDispatchMode=Mode_UpChain, param1:int=0, param2:int=0",
          "int",
          "dispatchMode default is Mode_UpChain");
static PyObject *XPSendMessageToWidgetFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  errCheck("prior sendMessageToWidget");
  static char *keywords[] = {CHAR("widgetID"), CHAR("message"), CHAR("dispatchMode"), CHAR("param1"), CHAR("param2"), nullptr};
  (void) self;
  PyObject *widget, *param1=Py_None, *param2=Py_None;
  int inMessage, inMode=xpMode_UpChain;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Oi|iOO", keywords, &widget, &inMessage, &inMode, &param1, &param2)){
    errCheck("Failed to parse tuple sendMessage");
    return nullptr;
  }
  errCheck("sendMessage post parse");
  XPWidgetID inWidget = getVoidPtr(widget, "XPWidgetID");
  intptr_t inParam1;
  intptr_t inParam2;
  errCheck("before convertMessage");
  convertMessagePythonToC(inMessage, widget, param1, param2, &inWidget, &inParam1, &inParam2);
  errCheck("after convertMessage");
  int res = XPSendMessageToWidget(inWidget, inMessage, inMode, inParam1, inParam2);
  errCheck("end sendMessageToWidget");
  return PyLong_FromLong(res);
}

My_DOCSTR(_placeWidgetWithin__doc__, "placeWidgetWithin",
          "widgetID, container=0",
          "widgetID:XPWidgetID, container:XPWidgetID | Literal[0] = 0",
          "None",
          "Change container widget for widgetID to container (widgetID)");
static PyObject *XPPlaceWidgetWithinFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("widgetID"), CHAR("container"), nullptr};
  (void) self;
  PyObject *subWidget, *container=Py_None;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|O", keywords, &subWidget, &container)){
    return nullptr;
  }
  if (PyLong_Check(container) && PyLong_AsLong(container) == 0) {
    container = Py_None;
  }
  XPPlaceWidgetWithin(getVoidPtr(subWidget, "XPWidgetID"), container == Py_None ? (void *) 0 : getVoidPtr(container, "XPWidgetID"));
  Py_RETURN_NONE;
}

My_DOCSTR(_countChildWidgets__doc__, "countChildWidgets",
          "widgetID",
          "widgetID:XPWidgetID",
          "int",
          "Return number of child widgets for this widgetID");
static PyObject *XPCountChildWidgetsFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("widgetID"), nullptr};
  (void) self;
  PyObject *widget;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &widget)){
    return nullptr;
  }
  int res = XPCountChildWidgets(getVoidPtr(widget, "XPWidgetID"));
  return PyLong_FromLong(res);
}

My_DOCSTR(_getNthChildWidget__doc__, "getNthChildWidget",
          "widgetID, index",
          "widgetID:XPWidgetID, index:int",
          "XPWidgetID",
          "Return widgetID of 0-based nth child");
static PyObject *XPGetNthChildWidgetFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("widgetID"), CHAR("index"), nullptr};
  (void) self;
  PyObject *widget;
  int inIndex;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Oi", keywords, &widget, &inIndex)){
    return nullptr;
  }
  XPWidgetID res = XPGetNthChildWidget(getVoidPtr(widget, "XPWidgetID"), inIndex);
  return makeCapsule(res, "XPWidgetID");
}

My_DOCSTR(_getParentWidget__doc__, "getParentWidget",
          "widgetID",
          "widgetID:XPWidgetID",
          "XPWidgetID",
          "Return widgetID for parent (i.e., container) of this widgetID");
static PyObject *XPGetParentWidgetFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("widgetID"), nullptr};
  (void) self;
  PyObject *widget;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &widget)){
    return nullptr;
  }
  XPWidgetID res = XPGetParentWidget(getVoidPtr(widget, "XPWidgetID"));
  return makeCapsule(res, "XPWidgetID");
}

My_DOCSTR(_showWidget__doc__, "showWidget",
          "widgetID",
          "widgetID:XPWidgetID",
          "None",
          "Make widget visible.");
static PyObject *XPShowWidgetFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("widgetID"), nullptr};
  (void) self;
  PyObject *widget;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &widget)){
    return nullptr;
  }
  XPShowWidget(getVoidPtr(widget, "XPWidgetID"));
  Py_RETURN_NONE;
}

My_DOCSTR(_hideWidget__doc__, "hideWidget",
          "widgetID",
          "widgetID:XPWidgetID",
          "None",
          "Hide widget");
static PyObject *XPHideWidgetFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("widgetID"), nullptr};
  (void) self;
  PyObject *widget;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &widget)){
    return nullptr;
  }
  XPHideWidget(getVoidPtr(widget, "XPWidgetID"));
  Py_RETURN_NONE;
}

My_DOCSTR(_isWidgetVisible__doc__, "isWidgetVisible",
          "widgetID",
          "widgetID:XPWidgetID",
          "int",
          "Return 1 if widget is visible\n"
          "\n"
          "Widget must be itself visible and contained in visible parent.\n"
          "Note if widget is outside of parent's geometry it may be clipped\n"
          "being reported 'visible' yet still not seen by user.");
static PyObject *XPIsWidgetVisibleFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("widgetID"), nullptr};
  (void) self;
  PyObject *widget;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &widget)){
    return nullptr;
  }
  int res = XPIsWidgetVisible(getVoidPtr(widget, "XPWidgetID"));
  return(PyLong_FromLong(res));
}

My_DOCSTR(_findRootWidget__doc__, "findRootWidget",
          "widgetID",
          "widgetID:XPWidgetID",
          "XPWidgetID",
          "Return top-most widget container for given widgetID\n"
          "\n"
          "If widget is root widget, it will return itself.");
static PyObject *XPFindRootWidgetFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("widgetID"), nullptr};
  (void) self;
  PyObject *widget;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &widget)){
    return nullptr;
  }
  XPWidgetID res = XPFindRootWidget(getVoidPtr(widget, "XPWidgetID"));
  return makeCapsule(res, "XPWidgetID");
}

My_DOCSTR(_bringRootWidgetToFront__doc__, "bringRootWidgetToFront",
          "widgetID",
          "widgetID:XPWidgetID",
          "None",
          "Make whole widget hierarchy containing widgetID to the front");
static PyObject *XPBringRootWidgetToFrontFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("widgetID"), nullptr};
  (void) self;
  PyObject *widget;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &widget)){
    return nullptr;
  }
  XPBringRootWidgetToFront(getVoidPtr(widget, "XPWidgetID"));
  Py_RETURN_NONE;
}

My_DOCSTR(_isWidgetInFront__doc__, "isWidgetInFront",
          "widgetID",
          "widgetID:XPWidgetID",
          "int",
          "Return 1 if widget's hierarchy is front most.");
static PyObject *XPIsWidgetInFrontFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("widgetID"), nullptr};
  (void) self;
  PyObject *widget;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &widget)){
    return nullptr;
  }
  int res = XPIsWidgetInFront(getVoidPtr(widget, "XPWidgetID"));
  return(PyLong_FromLong(res));
}

My_DOCSTR(_getWidgetGeometry__doc__, "getWidgetGeometry",
          "widgetID",
          "widgetID:XPWidgetID",
          "tuple[int, int, int, int]",
          "Return bounding box (left, top, right, bottom) of widgetID");
static PyObject *XPGetWidgetGeometryFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("widgetID"), nullptr};
  (void) self;
  PyObject *widget;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &widget)){
    return nullptr;
  }
  int left, top, right, bottom;
  XPGetWidgetGeometry(getVoidPtr(widget, "XPWidgetID"), &left, &top, &right, &bottom);
  return Py_BuildValue("(iiii)", left, top, right, bottom);
}

My_DOCSTR(_setWidgetGeometry__doc__, "setWidgetGeometry",
          "widgetID, left, top, right, bottom",
          "widgetID:XPWidgetID, left:int, top:int, right:int, bottom:int",
          "None",
          "Set bounding box for widgetID");
static PyObject *XPSetWidgetGeometryFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("widgetID"), CHAR("left"), CHAR("top"), CHAR("right"), CHAR("bottom"), nullptr};
  (void) self;
  PyObject *widget;
  int inLeft, inTop, inRight, inBottom;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Oiiii", keywords, &widget, &inLeft, &inTop, &inRight, &inBottom)){
    return nullptr;
  }
  XPSetWidgetGeometry(getVoidPtr(widget, "XPWidgetID"), inLeft, inTop, inRight, inBottom);
  Py_RETURN_NONE;
}

My_DOCSTR(_getWidgetForLocation__doc__, "getWidgetForLocation",
          "container, xOffset, yOffset, recursive=1, visibleOnly=1",
          "container:XPWidgetID, xOffset:int, yOffset:int, recursive:int=1, visibleOnly:int=1",
          "XPWidgetID",
          "Return widgetID of the child widget within the container widget at offset\n"
          "\n"
          "offsets are global coordinates, not relative bounding box of container.\n"
          "recursive=1 indicates find 'deepest' child widget\n"
          "visibleOnly=1 indicates only visible widgets are considered");
static PyObject *XPGetWidgetForLocationFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("container"), CHAR("xOffset"), CHAR("yOffset"), CHAR("recursive"), CHAR("visibleOnly"), nullptr};
  (void) self;
  PyObject *container;
  int xOffset, yOffset, recursive=1, visibleOnly=1;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Oii|ii", keywords, &container, &xOffset, &yOffset, &recursive, &visibleOnly)){
    return nullptr;
  }
  XPWidgetID res = XPGetWidgetForLocation(getVoidPtr(container, "XPWidgetID"), xOffset, yOffset, recursive, visibleOnly);
  return makeCapsule(res, "XPWidgetID");
}

My_DOCSTR(_getWidgetExposedGeometry__doc__, "getWidgetExposedGeometry",
          "widgetID",
          "widgetID:XPWidgetID",
          "tuple[int, int, int, int]",
          "Return (left, top, right, bottom) of widget's exposed geometry");
static PyObject *XPGetWidgetExposedGeometryFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("widgetID"), nullptr};
  (void) self;
  PyObject *widget;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &widget)){
    return nullptr;
  }
  int left, top, right, bottom;
  XPGetWidgetExposedGeometry(getVoidPtr(widget, "XPWidgetID"), &left, &top, &right, &bottom);
  return Py_BuildValue("(iiii)", left, top, right, bottom);
}

My_DOCSTR(_setWidgetDescriptor__doc__, "setWidgetDescriptor",
          "widgetID, descriptor",
          "widgetID:XPWidgetID, descriptor:str",
          "None",
          "Set widget's descriptor string");
static PyObject *XPSetWidgetDescriptorFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  errCheck("Prior setwidgetdescriptor");
  static char *keywords[] = {CHAR("widgetID"), CHAR("descriptor"), nullptr};
  (void) self;
  PyObject *widget;
  const char *inDescriptor;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Os", keywords, &widget, &inDescriptor)){
    return nullptr;
  }
  XPSetWidgetDescriptor(getVoidPtr(widget, "XPWidgetID"), inDescriptor);
  errCheck("end setwidgetdescriptor");
  Py_RETURN_NONE;
}

My_DOCSTR(_getWidgetDescriptor__doc__, "getWidgetDescriptor",
          "widgetID",
          "widgetID:XPWidgetID",
          "str",
          "Returns widget's descriptor string");
static PyObject *XPGetWidgetDescriptorFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  errCheck("prior getWidgetDescriptor");
  static char *keywords[] = {CHAR("widgetID"), nullptr};
  (void) self;
  PyObject *widget;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &widget)){
    return nullptr;
  }
  int res;
  int length = XPGetWidgetDescriptor(getVoidPtr(widget, "XPWidgetID"), nullptr, 0);
  char *buffer = (char *)malloc(length + 1);
  res = XPGetWidgetDescriptor(getVoidPtr(widget, "XPWidgetID"), buffer, length);
  if (res > length) {
    printf("Warning: xppython descriptor for widget exceeds buffer size\n");
  }
  buffer[res] = '\0';
  errCheck("end getWidgetDescriptor");
  PyObject *ret = PyUnicode_FromString(buffer);
  free(buffer);
  return ret;
}

My_DOCSTR(_getWidgetUnderlyingWindow__doc__, "getWidgetUnderlyingWindow",
          "widgetID",
          "widgetID:XPWidgetID",
          "XPLMWindowID",
          "Return windowID of window underlying widget");
static PyObject *XPGetWidgetUnderlyingWindowFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("widgetID"), nullptr};
  (void)self;
  PyObject *widget;
  if(!XPGetWidgetUnderlyingWindow_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPGetWidgetUnderlyingWindow is available only in XPLM301 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &widget)){
    return nullptr;
  }
  XPLMWindowID res = XPGetWidgetUnderlyingWindow_ptr(getVoidPtr(widget, "XPWidgetID"));
  return makeCapsule(res, "XPLMWindowID");
}


My_DOCSTR(_setWidgetProperty__doc__, "setWidgetProperty",
          "widgetID, propertyID, value=None",
          "widgetID:XPWidgetID, propertyID:XPWidgetPropertyID | int, value:Optional[Any]",
          "None",
          "Set widget property to value");
static PyObject *XPSetWidgetPropertyFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("widgetID"), CHAR("propertyID"), CHAR("value"), nullptr};
  (void) self;
  PyObject *widget, *value=Py_None;
  int property;
  PyObject *err;
  errCheck("Error prior to  start of setWidgetProperty");
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Oi|O", keywords, &widget, &property, &value)){
    errCheck("Failed to parse setWidgetProperty");
    return nullptr;
  }
  XPWidgetPropertyID inProperty = property;
  if (property >= xpProperty_UserStart) {
    std::pair<PyObject*, int> key = std::make_pair(widget, property);
    errCheck("Failed to build setWidgetProperty");
    auto it = widgetPropertyDict.find(key);
    PyObject *prevValueObj = (it != widgetPropertyDict.end()) ? it->second : nullptr;

    int comparison = 0; /* false */
    if (prevValueObj != nullptr) {
      /* previous value exists, do comparison */
      if (prevValueObj == value) {
        comparison = 1;  // Same object, definitely equal
      } else {
        comparison = PyObject_RichCompareBool(value, prevValueObj, Py_EQ) == 1 ? 1 : 0;
      }
      Py_DECREF(prevValueObj); /* remove old value */
    }

    Py_INCREF(value); /* add new value */
    widgetPropertyDict[key] = value;

    if (comparison != 1) {
      /* not found, or they're different */
      XPSendMessageToWidget(getVoidPtr(widget, "XPWidgetID"), xpMsg_PropertyChanged, xpMode_Direct, property, (intptr_t) value);
      errCheck("after comparison == 0 setwidgetproperty");
    }
  } else {
    XPSetWidgetProperty(getVoidPtr(widget, "XPWidgetID"), inProperty, value == Py_None ? 0: PyLong_AsLong(value));
    err = PyErr_Occurred();
    if(err){
      char *s = objToStr(value);
      pythonLog("Error trying to set widget property %d with value %s", inProperty, s);
      free(s);
    }
  }
  errCheck("Error prior to  start of setWidgetProperty");
  Py_RETURN_NONE;
}

My_DOCSTR(_getWidgetProperty__doc__, "getWidgetProperty",
          "widgetID, propertyID, exists=-1",
          "widgetID:XPWidgetID, propertyID:XPWidgetPropertyID | int, exists:int=-1",
          "Any",
          "Returns widget's property value. For XP properties, it is a long, for\n"
          "User-defined properties, it can be any python object.\n"
          "\n"
          "Raise ValueError if exists=-1 and property does not exist / has not been set\n"
          "Set exists=None if you don't care if property exists (value will be 0)\n"
          "Set exists to a list object, and we'll set it to [1,] if property exists, \n"
          "  [0, ] otherwise");
static PyObject *XPGetWidgetPropertyFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("widgetID"), CHAR("propertyID"), CHAR("exists"), nullptr};
  (void)self;
  PyObject *widget, *exists=Py_None;
  int property;
  int exception_on_error = 0;
  PyObject *err;
  err = PyErr_Occurred();
  if (err) {
    pythonLog("error prior to getWidgetProperty");
    pythonLogException();
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "OiO", keywords, &widget, &property, &exists)){
    PyErr_Clear();

    if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Oi|O", keywords, &widget, &property, &exists)){
      return nullptr;
    }
    exception_on_error = 1;
  }
  XPWidgetPropertyID inProperty = property;
  int inExists;

  PyObject *resObj;
  if (property >= xpProperty_UserStart) {
    std::pair<PyObject*, int> key = std::make_pair(widget, property);
    auto it = widgetPropertyDict.find(key);
    if (it == widgetPropertyDict.end()) {
      /* not found, return 0 */
      resObj = PyLong_FromLong(0);
      inExists = 0;
    } else {
      resObj = it->second;
      Py_INCREF(resObj);
      inExists = 1;
    }
  } else {
    intptr_t res = XPGetWidgetProperty(getVoidPtr(widget, "XPWidgetID"), inProperty, &inExists);
    resObj = PyLong_FromLong(res);
  }
    
  if (exception_on_error && !inExists) {
    PyErr_SetString(PyExc_ValueError, "Widget does not have this property");
    return nullptr;
  }
  if(exists != Py_None && !exception_on_error) {
    PyObject *e = PyLong_FromLong(inExists);
    PyList_Insert(exists, 0, e);
    Py_DECREF(e);
  }
  
  err = PyErr_Occurred();
  if (err) {
    pythonLog("error at end of getWidgetProperty");
    pythonLogException();
  }
  return resObj;
}

My_DOCSTR(_setKeyboardFocus__doc__, "setKeyboardFocus",
          "widgetID",
          "widgetID:XPWidgetID",
          "int",
          "Set keyboard focus to widgetID");
static PyObject *XPSetKeyboardFocusFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("widgetID"), nullptr};
  (void) self;
  PyObject *widget;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &widget)){
    return nullptr;
  }
  XPWidgetID res;
  if (PyLong_Check(widget) && PyLong_AsLong(widget) == 0) {
    res = XPSetKeyboardFocus(0);
  } else {
     res = XPSetKeyboardFocus(getVoidPtr(widget, "XPWidgetID"));
  }
  PyObject *resObj = makeCapsule(res, "XPWidgetID");
  if (resObj == Py_None) {
    return PyLong_FromLong(0);
  }
  return resObj;
}

My_DOCSTR(_loseKeyboardFocus__doc__, "loseKeyboardFocus",
          "widgetID",
          "widgetID:XPWidgetID",
          "None",
          "Cause widgetID to lose keyboard focus");
static PyObject *XPLoseKeyboardFocusFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("widgetID"), nullptr};
  (void) self;
  PyObject *widget;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &widget)){
    return nullptr;
  }
  XPLoseKeyboardFocus(getVoidPtr(widget, "XPWidgetID"));
  Py_RETURN_NONE;
}

My_DOCSTR(_getWidgetWithFocus__doc__, "getWidgetWithFocus",
          "",
          "",
          "XPWidgetID",
          "Return widgetID with current focus. 0=X-Plane has focus.");
static PyObject *XPGetWidgetWithFocusFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  XPWidgetID res = XPGetWidgetWithFocus();
  PyObject *resObj = makeCapsule(res, "XPWidgetID");
  return resObj;
}

//Since we have only one callback available, we'll have to handle this
//  ourselves...

My_DOCSTR(_addWidgetCallback__doc__, "addWidgetCallback",
          "widgetID, callback",
          "widgetID:XPWidgetID, callback:Callable[[XPWidgetMessage, XPWidgetID, int, int], int]",
          "None",
          "Add callback to widgetID\n"
          "\n"
          "Callback has signature (message, widgetID, param1, param2). See\n"
          "createCustomWidget()");
static PyObject *XPAddWidgetCallbackFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("widget"), CHAR("callback"), nullptr};
  (void) self;
  PyObject *widget, *callback;
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OO", keywords, &widget, &callback)){
    return nullptr;
  }
  XPWidgetID widgetPtr = getVoidPtr(widget, "XPWidgetID");
  auto it = widgetCallbacks.find(widgetPtr);
  Py_INCREF(callback);

  if(it == widgetCallbacks.end()){
    WidgetCallbackInfo callbackInfo;
    callbackInfo.module_name = CurrentPythonModuleName;
    callbackInfo.callbacks.push_back(callback);
    widgetCallbacks[widgetPtr] = std::move(callbackInfo);
    //register only the first time
    XPAddWidgetCallback(widgetPtr, widgetCallback);
  }else{
    it->second.callbacks.insert(it->second.callbacks.begin(), callback);
  }
  Py_RETURN_NONE;
}

My_DOCSTR(_getWidgetClassFunc__doc__, "getWidgetClassFunc",
          "widgetID",
          "widgetID:XPWidgetID",
          "int",
          "Given widgetClass, return underlying function.\n"
          "\n"
          "Not useful with python. Use addWidgetCalback() instead.");
static PyObject *XPGetWidgetClassFuncFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("widgetID"), nullptr};
  (void)self;
  int inWidgetClass;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "i", keywords, &inWidgetClass)){
    return nullptr;
  }
  XPWidgetFunc_t res = XPGetWidgetClassFunc(inWidgetClass);
  return PyLong_FromVoidPtr((void*)res);
}

My_DOCSTR(_getWidgetCallbackDict__doc__, "getWidgetCallbackDict",
          "",
          "",
          "dict[XPWidgetID, tuple[str, list]]",
          "Returns dictionary of widget callbacks.\n"
          "\n"
          "Keys are XPWidgetID capsules, values are tuples of (module_name, [callbacks])");
static PyObject *XPGetWidgetCallbackDictFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  return buildWidgetCallbackDict();
}

My_DOCSTR(_getWidgetPropertiesDict__doc__, "getWidgetPropertiesDict",
          "",
          "",
          "dict[XPWidgetID, dict[int, Any]]",
          "Returns dictionary of widget properties.\n"
          "\n"
          "Keys are XPWidgetID capsules, values are dicts of {propertyID: value}");
static PyObject *XPGetWidgetPropertiesDictFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  return buildWidgetPropertiesDict();
}

static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  for (auto& pair : widgetCallbacks) {
    for (PyObject* callback : pair.second.callbacks) {
      Py_DECREF(callback);
    }
  }
  widgetCallbacks.clear();
  for (auto& pair : widgetPropertyDict) {
    Py_DECREF(pair.second);
  }
  widgetPropertyDict.clear();
  Py_RETURN_NONE;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
static PyMethodDef XPWidgetsMethods[] = {
  {"createWidget", (PyCFunction)XPCreateWidgetFun, METH_VARARGS | METH_KEYWORDS, _createWidget__doc__},
  {"XPCreateWidget", (PyCFunction)XPCreateWidgetFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"createCustomWidget", (PyCFunction)XPCreateCustomWidgetFun, METH_VARARGS | METH_KEYWORDS, _createCustomWidget__doc__},
  {"XPCreateCustomWidget", (PyCFunction)XPCreateCustomWidgetFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"destroyWidget", (PyCFunction)XPDestroyWidgetFun, METH_VARARGS | METH_KEYWORDS, _destroyWidget__doc__},
  {"XPDestroyWidget", (PyCFunction)XPDestroyWidgetFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"sendMessageToWidget", (PyCFunction)XPSendMessageToWidgetFun, METH_VARARGS | METH_KEYWORDS, _sendMessageToWidget__doc__},
  {"XPSendMessageToWidget", (PyCFunction)XPSendMessageToWidgetFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"placeWidgetWithin", (PyCFunction)XPPlaceWidgetWithinFun, METH_VARARGS | METH_KEYWORDS, _placeWidgetWithin__doc__},
  {"XPPlaceWidgetWithin", (PyCFunction)XPPlaceWidgetWithinFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"countChildWidgets", (PyCFunction)XPCountChildWidgetsFun, METH_VARARGS | METH_KEYWORDS, _countChildWidgets__doc__},
  {"XPCountChildWidgets", (PyCFunction)XPCountChildWidgetsFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"getNthChildWidget", (PyCFunction)XPGetNthChildWidgetFun, METH_VARARGS | METH_KEYWORDS, _getNthChildWidget__doc__},
  {"XPGetNthChildWidget", (PyCFunction)XPGetNthChildWidgetFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"getParentWidget", (PyCFunction)XPGetParentWidgetFun, METH_VARARGS | METH_KEYWORDS, _getParentWidget__doc__},
  {"XPGetParentWidget", (PyCFunction)XPGetParentWidgetFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"showWidget", (PyCFunction)XPShowWidgetFun, METH_VARARGS | METH_KEYWORDS, _showWidget__doc__},
  {"XPShowWidget", (PyCFunction)XPShowWidgetFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"hideWidget", (PyCFunction)XPHideWidgetFun, METH_VARARGS | METH_KEYWORDS, _hideWidget__doc__},
  {"XPHideWidget", (PyCFunction)XPHideWidgetFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"isWidgetVisible", (PyCFunction)XPIsWidgetVisibleFun, METH_VARARGS | METH_KEYWORDS, _isWidgetVisible__doc__},
  {"XPIsWidgetVisible", (PyCFunction)XPIsWidgetVisibleFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"findRootWidget", (PyCFunction)XPFindRootWidgetFun, METH_VARARGS | METH_KEYWORDS, _findRootWidget__doc__},
  {"XPFindRootWidget", (PyCFunction)XPFindRootWidgetFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"bringRootWidgetToFront", (PyCFunction)XPBringRootWidgetToFrontFun, METH_VARARGS | METH_KEYWORDS, _bringRootWidgetToFront__doc__},
  {"XPBringRootWidgetToFront", (PyCFunction)XPBringRootWidgetToFrontFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"isWidgetInFront", (PyCFunction)XPIsWidgetInFrontFun, METH_VARARGS | METH_KEYWORDS, _isWidgetInFront__doc__},
  {"XPIsWidgetInFront", (PyCFunction)XPIsWidgetInFrontFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"getWidgetGeometry", (PyCFunction)XPGetWidgetGeometryFun, METH_VARARGS | METH_KEYWORDS, _getWidgetGeometry__doc__},
  {"XPGetWidgetGeometry", (PyCFunction)XPGetWidgetGeometryFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"setWidgetGeometry", (PyCFunction)XPSetWidgetGeometryFun, METH_VARARGS | METH_KEYWORDS, _setWidgetGeometry__doc__},
  {"XPSetWidgetGeometry", (PyCFunction)XPSetWidgetGeometryFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"getWidgetForLocation", (PyCFunction)XPGetWidgetForLocationFun, METH_VARARGS | METH_KEYWORDS, _getWidgetForLocation__doc__},
  {"XPGetWidgetForLocation", (PyCFunction)XPGetWidgetForLocationFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"getWidgetExposedGeometry", (PyCFunction)XPGetWidgetExposedGeometryFun, METH_VARARGS | METH_KEYWORDS, _getWidgetExposedGeometry__doc__},
  {"XPGetWidgetExposedGeometry", (PyCFunction)XPGetWidgetExposedGeometryFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"setWidgetDescriptor", (PyCFunction)XPSetWidgetDescriptorFun, METH_VARARGS | METH_KEYWORDS, _setWidgetDescriptor__doc__},
  {"XPSetWidgetDescriptor", (PyCFunction)XPSetWidgetDescriptorFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"getWidgetDescriptor", (PyCFunction)XPGetWidgetDescriptorFun, METH_VARARGS | METH_KEYWORDS, _getWidgetDescriptor__doc__},
  {"XPGetWidgetDescriptor", (PyCFunction)XPGetWidgetDescriptorFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"getWidgetUnderlyingWindow", (PyCFunction)XPGetWidgetUnderlyingWindowFun, METH_VARARGS | METH_KEYWORDS, _getWidgetUnderlyingWindow__doc__},
  {"XPGetWidgetUnderlyingWindow", (PyCFunction)XPGetWidgetUnderlyingWindowFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"setWidgetProperty", (PyCFunction)XPSetWidgetPropertyFun, METH_VARARGS | METH_KEYWORDS, _setWidgetProperty__doc__},
  {"XPSetWidgetProperty", (PyCFunction)XPSetWidgetPropertyFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"getWidgetProperty", (PyCFunction)XPGetWidgetPropertyFun, METH_VARARGS | METH_KEYWORDS, _getWidgetProperty__doc__},
  {"XPGetWidgetProperty", (PyCFunction)XPGetWidgetPropertyFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"setKeyboardFocus", (PyCFunction)XPSetKeyboardFocusFun, METH_VARARGS | METH_KEYWORDS, _setKeyboardFocus__doc__},
  {"XPSetKeyboardFocus", (PyCFunction)XPSetKeyboardFocusFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"loseKeyboardFocus", (PyCFunction)XPLoseKeyboardFocusFun, METH_VARARGS | METH_KEYWORDS, _loseKeyboardFocus__doc__},
  {"XPLoseKeyboardFocus", (PyCFunction)XPLoseKeyboardFocusFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"getWidgetWithFocus", (PyCFunction)XPGetWidgetWithFocusFun, METH_VARARGS, _getWidgetWithFocus__doc__},
  {"XPGetWidgetWithFocus", (PyCFunction)XPGetWidgetWithFocusFun, METH_VARARGS, ""},
  {"addWidgetCallback", (PyCFunction)XPAddWidgetCallbackFun, METH_VARARGS | METH_KEYWORDS, _addWidgetCallback__doc__},
  {"XPAddWidgetCallback", (PyCFunction)XPAddWidgetCallbackFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"getWidgetClassFunc", (PyCFunction)XPGetWidgetClassFuncFun, METH_VARARGS | METH_KEYWORDS, _getWidgetClassFunc__doc__},
  {"XPGetWidgetClassFunc", (PyCFunction)XPGetWidgetClassFuncFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"getWidgetCallbackDict", (PyCFunction)XPGetWidgetCallbackDictFun, METH_VARARGS, _getWidgetCallbackDict__doc__},
  {"getWidgetPropertiesDict", (PyCFunction)XPGetWidgetPropertiesDictFun, METH_VARARGS, _getWidgetPropertiesDict__doc__},
  {"_cleanup", cleanup, METH_VARARGS, ""},
  {nullptr, nullptr, 0, nullptr}
};
#pragma GCC diagnostic pop

static struct PyModuleDef XPWidgetsModule = {
  PyModuleDef_HEAD_INIT,
  "XPWidgets",
  "Laminar documentation: \n"
  "   https://developer.x-plane.com/sdk/XPWidgets/\n"
  "XPPython3 documentation: \n"
  "   https://xppython3.rtfd.io/en/stable/development/modules/widgets.html",
  -1,
  XPWidgetsMethods,
  nullptr,
  nullptr,
  nullptr,
  nullptr
};

PyMODINIT_FUNC
PyInit_XPWidgets(void)
{
  // widgetCallbacks is now a C++ unordered_map, no need to initialize as PyDict
  // widgetPropertyDict is now a C++ unordered_map, no need to initialize as PyDict
  // widgetIDCapsules is now a C++ unordered_map, no need to initialize as PyDict
  PyObject *mod = PyModule_Create(&XPWidgetsModule);
  if(mod){
    PyModule_AddStringConstant(mod, "__author__", "Peter Buckner (pbuck@xppython3.org)");
  }

  return mod;
}


PyObject *buildWidgetCallbackDict(void) {
  PyObject *dict = PyDict_New();
  if (!dict) return nullptr;

  for (auto& pair : widgetCallbacks) {
    PyObject *widget_capsule = makeCapsule(pair.first, "XPWidgetID");
    if (!widget_capsule) {
      Py_DECREF(dict);
      return nullptr;
    }

    // Build list of callbacks
    PyObject *callback_list = PyList_New(pair.second.callbacks.size());
    if (!callback_list) {
      Py_DECREF(widget_capsule);
      Py_DECREF(dict);
      return nullptr;
    }

    for (size_t i = 0; i < pair.second.callbacks.size(); ++i) {
      PyObject *callback = pair.second.callbacks[i];
      Py_INCREF(callback);
      PyList_SetItem(callback_list, i, callback);
    }

    // Build tuple: (module_name, [callbacks])
    PyObject *value_tuple = Py_BuildValue("(sO)",
                                          pair.second.module_name,
                                          callback_list);
    Py_DECREF(callback_list);

    if (!value_tuple) {
      Py_DECREF(widget_capsule);
      Py_DECREF(dict);
      return nullptr;
    }

    if (PyDict_SetItem(dict, widget_capsule, value_tuple) < 0) {
      Py_DECREF(value_tuple);
      Py_DECREF(widget_capsule);
      Py_DECREF(dict);
      return nullptr;
    }

    Py_DECREF(value_tuple);
    Py_DECREF(widget_capsule);
  }

  return dict;
}

PyObject *buildWidgetPropertiesDict(void) {
  PyObject *outer_dict = PyDict_New();
  if (!outer_dict) return nullptr;

  for (auto& pair : widgetPropertyDict) {
    PyObject *widget = pair.first.first;  // The widget capsule
    int propertyID = pair.first.second;   // The property ID
    PyObject *value = pair.second;        // The property value

    // Check if this widget already has an entry in outer dict
    PyObject *inner_dict = PyDict_GetItem(outer_dict, widget);  // borrowed ref

    if (!inner_dict) {
      // Create new inner dict for this widget
      inner_dict = PyDict_New();
      if (!inner_dict) {
        Py_DECREF(outer_dict);
        return nullptr;
      }

      // Add to outer dict
      if (PyDict_SetItem(outer_dict, widget, inner_dict) < 0) {
        Py_DECREF(inner_dict);
        Py_DECREF(outer_dict);
        return nullptr;
      }

      Py_DECREF(inner_dict);  // PyDict_SetItem increfs, so we can release our ref

      // Re-borrow the reference for consistency
      inner_dict = PyDict_GetItem(outer_dict, widget);
    }

    // Add property to inner dict
    PyObject *propID_obj = PyLong_FromLong(propertyID);
    if (!propID_obj) {
      Py_DECREF(outer_dict);
      return nullptr;
    }

    Py_INCREF(value);  // We're returning a reference to this value
    if (PyDict_SetItem(inner_dict, propID_obj, value) < 0) {
      Py_DECREF(value);
      Py_DECREF(propID_obj);
      Py_DECREF(outer_dict);
      return nullptr;
    }

    Py_DECREF(propID_obj);
  }

  return outer_dict;
}

void logWidgets(PyObject *key, const char *key_s, const char * value_s) {
  (void) key;
  /* get all widget capsules */
  for (auto& pair : CapsuleDict) {
    PyObject *capsule = pair.second;
    if (! strcmp(PyCapsule_GetName(capsule), "XPWidgetID")) {
      char *moduleName = (char *)PyCapsule_GetContext(capsule);
      pythonLog("  %s / %s:%s%s", key_s, moduleName, strlen(value_s) > 10 ? "\n    " : " ", value_s);
    }
  }
}
