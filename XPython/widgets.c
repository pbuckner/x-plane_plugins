#define _GNU_SOURCE 1
#include <Python.h>
#include <sys/time.h>
#include <stdio.h>

#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMDisplay.h>
#include <Widgets/XPWidgetDefs.h>
#include <Widgets/XPWidgets.h>
#include <Widgets/XPStandardWidgets.h>
#include "plugin_dl.h"
#include "utils.h"
#include "widgetutils.h"
#include "widgets.h"
#include "xppython.h"

static PyObject *widgetCallbackDict; // _key_ is widget, value = callbackList
#define WIDGET_CALLBACK 0
#define WIDGET_MODULE_NAME 1

static PyObject *widgetPropertyDict; // _key_ is (Widget, Property), _value_ is PyObject*
#define WIDGETPROPERTY_WIDGET 0
#define WIDGETPROPERTY_PROPERTY 1

void resetWidgets(void) {
  PyObject *keys = PyDict_Keys(widgetIDCapsules);
  PyObject *iterator = PyObject_GetIter(keys); /* new */
  PyObject *capsuleDictKey;
  while((capsuleDictKey = PyIter_Next(iterator))) {  /* new */
    PyObject *capsuleInfo = PyDict_GetItem(widgetIDCapsules, capsuleDictKey);
    PyObject *capsule = capsuleInfo;
    char *moduleName = "";
#if ERRCHECK
    capsule = PyTuple_GetItem(capsuleInfo, 0);
    moduleName = objToStr(PyTuple_GetItem(capsuleInfo, 1));
#endif    
    char *s1 = objToStr(capsule);
    pythonDebug("     Reset --      %s  %s", moduleName, s1);
    free(s1);
#if ERRCHECK
    free(moduleName);
#endif
    XPDestroyWidget(refToPtr(capsule, widgetRefName), 0);
    Py_DECREF(capsuleDictKey);
  }
  Py_DECREF(iterator);
  Py_XDECREF(keys);
  PyDict_Clear(widgetIDCapsules);
  PyDict_Clear(widgetCallbackDict);
  PyDict_Clear(widgetPropertyDict);
}

static void clearChildrenXPWidgetData(PyObject *widget);
static void clearXPWidgetData(PyObject *widget);
PyObject *widgetIDCapsules;

int widgetCallback(XPWidgetMessage inMessage, XPWidgetID inWidget, intptr_t inParam1, intptr_t inParam2)
{
  PyObject *err;
  errCheck("Error prior to widgetCallback");
  // struct timespec stop, start;
  struct timespec all_stop, all_start;
  clock_gettime(CLOCK_MONOTONIC, &all_start);

  PyObject *widget = getPtrRef(inWidget, widgetIDCapsules, widgetRefName);
  errCheck("Error after getPtrRef");
  
  PyObject *param1, *param2;
  XPKeyState_t *keyState;
  XPMouseState_t *mouseState;
  XPWidgetGeometryChange_t *wChange;
  param1 = PyLong_FromLong(inParam1);
  param2 = PyLong_FromLong(inParam2);
  switch(inMessage){
  case xpMsg_KeyPress:
    keyState = (XPKeyState_t *)inParam1;
    param1 = Py_BuildValue("(iii)", (int)keyState->key, (int)keyState->flags,
                           (int)keyState->vkey);
    break;
  case xpMsg_MouseDown:
  case xpMsg_MouseDrag:
  case xpMsg_MouseUp:
  case xpMsg_MouseWheel:
  case xpMsg_CursorAdjust:
    mouseState = (XPMouseState_t *)inParam1;
    param1 = Py_BuildValue("(iiii)", mouseState->x, mouseState->y,
                           mouseState->button, mouseState->delta);
    break;
  case xpMsg_Reshape:
    param1 =  getPtrRef((void *)inParam1, widgetIDCapsules, widgetRefName);
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
    param1 =  getPtrRef((void *)inParam1, widgetIDCapsules, widgetRefName);
    break;
    
  case xpMsg_PropertyChanged:
    if (inParam1 >= xpProperty_UserStart) {
      // use inParam2 -- it's already python
      param2 = (PyObject*)inParam2;
    }
    break;
  default: // intentionally empty
    break;
  }

  errCheck("Error after message parse for msg %d", inMessage);
  
  PyObject *callbackList = PyDict_GetItem(widgetCallbackDict, widget);
  if(callbackList == NULL){
    /* we'll get an xpMsg_Create that we can't handle from a CustomWidget (because the widgetCallbackDict
       isn't populated yet). Ignore the message (CreateCustomWidget() below will send it again!)
       If not xpMsg_Create, write error.
     */
    if (inMessage != xpMsg_Create && inMessage != xpMsg_AcceptParent) {
      pythonLog("Couldn't find the callback list for widget ID %p. for message %d", inWidget, inMessage);
    }
    Py_DECREF(widget);
    Py_DECREF(param1);
    Py_DECREF(param2);
    err = PyErr_Occurred();
    if(err){
      pythonLog("Error after message where no callbackList for msg %d", inMessage);
      pythonLogException();
    }
    return 0;
  }

  Py_ssize_t i;
  int res;
  PyObject *callback;
  for(i = 0; i < PyList_Size(callbackList); ++i){
    err = PyErr_Occurred();
    if(err){
      pythonLog("Error in widget callbacklist [%d] for msg %d", i, inMessage);
      pythonLogException();
    }
  
    callback = PyTuple_GetItem(PyList_GetItem(callbackList, i), WIDGET_CALLBACK);
    set_moduleName(PyTuple_GetItem(PyList_GetItem(callbackList, i), WIDGET_MODULE_NAME));
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
      PyObject *resObj = PyObject_CallFunctionObjArgs(callback, inMessageObj, widget, param1, param2, NULL);
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
        *(XPLMCursorStatus *)inParam2 = (int)PyLong_AsLong(param2);
      }
      break;
    }
    //pluginStats[getPluginIndex(pluginSelf)].customw_time += (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_nsec - start.tv_nsec) / 1000;
  }

  err = PyErr_Occurred();
  if(err){
    pythonLog("Error in widget post callbacklist for msg %d", inMessage);
    pythonLogException();
  }

  if(inMessage == xpMsg_Destroy){
    PyDict_DelItem(widgetCallbackDict, widget);
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
  PyObject *module_name_p = PyUnicode_FromString(CurrentPythonModuleName);
  pluginStats[getPluginIndex(module_name_p)].customw_time += (all_stop.tv_sec - all_start.tv_sec) * 1000000 + (all_stop.tv_nsec - all_start.tv_nsec) / 1000;
  Py_DECREF(module_name_p);
  pluginStats[0].customw_time += (all_stop.tv_sec - all_start.tv_sec) * 1000000 + (all_stop.tv_nsec - all_start.tv_nsec) / 1000;
  err = PyErr_Occurred();
  if(err){
    pythonLog("Error add end of widget callback for msg %d", inMessage);
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
  static char *keywords[] = {"left", "top", "right", "bottom", "visible", "descriptor", "isRoot", "container", "widgetClass", NULL};
  (void) self;
  int inLeft, inTop, inRight, inBottom, inVisible, inIsRoot;
  const char *inDescriptor;
  PyObject *container;
  XPWidgetClass inClass;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "iiiiisiOi", keywords, &inLeft, &inTop, &inRight, &inBottom, &inVisible, &inDescriptor, &inIsRoot,
                                         &container, &inClass)){
    return NULL;
  }
  // use inContainer 0, if passed in value of 0
  XPWidgetID inContainer;
  if (inIsRoot > 0) {
    inContainer = 0;
  } else if ((PyLong_Check(container) && PyLong_AsLong(container) == 0) || container == Py_None) {
    inContainer = 0;
  } else {
    inContainer = refToPtr(container, widgetRefName);
  }

  XPWidgetID res = XPCreateWidget(inLeft, inTop, inRight, inBottom, inVisible, inDescriptor, inIsRoot, inContainer, inClass);
  PyObject *ret = getPtrRef(res, widgetIDCapsules, widgetRefName);
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
  static char *keywords[] = {"left", "top", "right", "bottom", "visible", "descriptor", "isRoot", "container", "callback", NULL};
  (void) self;
  int inLeft, inTop, inRight, inBottom, inVisible, inIsRoot;
  const char *inDescriptor;
  PyObject *container;
  PyObject *inCallback;
  errCheck("error at start of CreateCustomWidget");
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "iiiiisiOO", keywords, &inLeft, &inTop, &inRight, &inBottom, &inVisible, &inDescriptor,
                       &inIsRoot, &container, &inCallback)){
    return NULL;
  }
  // use inContainer 0, if passed in value of 0
  XPWidgetID inContainer;
  if (inIsRoot > 0) {
    inContainer = 0;
  } else if ((PyLong_Check(container) && PyLong_AsLong(container) == 0) || container == Py_None) {
    inContainer = 0;
  } else {
    inContainer = refToPtr(container, widgetRefName);
  }

  /* vvvvvvvvvvvvvvvvvv widgetCallback will be immediately called with Create msg BUT
       widgetCallbackDict does not yet have entry for this widget, so the create msg
       will not be received by this function. 
       So... we populate the dict and then call SendMessageToWidget directly!
   */
  XPWidgetID res = XPCreateCustomWidget(inLeft, inTop, inRight, inBottom, inVisible, inDescriptor, inIsRoot,
                                        inContainer, widgetCallback);
  PyObject *resObj = getPtrRef(res, widgetIDCapsules, widgetRefName);
  PyObject *callbackList = PyList_New(0);
  PyObject *tup = Py_BuildValue("(Os)", inCallback, CurrentPythonModuleName);
  Py_INCREF(inCallback);
  PyList_Insert(callbackList, 0, tup);
  Py_DECREF(tup);
  PyDict_SetItem(widgetCallbackDict, resObj, callbackList);
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
  static char *keywords[] = {"widgetID", "destroyChildren", NULL};
  (void) self;
  PyObject *widget;
  int inDestroyChildren=1;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|i", keywords, &widget, &inDestroyChildren)){
    return NULL;
  }
  XPWidgetID wid = refToPtr(widget, widgetRefName);
  XPDestroyWidget(wid, inDestroyChildren);
  if (inDestroyChildren) {
    clearChildrenXPWidgetData(widget);
  }
  clearXPWidgetData(widget);
  Py_RETURN_NONE;
}


static void clearChildrenXPWidgetData(PyObject *widget) {
  XPWidgetID wid = refToPtr(widget, widgetRefName);
  int numChildren = XPCountChildWidgets(wid);
  pythonDebug("Clearing children xp widget data [%d] for %s", numChildren, objDebug(widget));
  for (int i=0; i<numChildren; i++) {
    PyObject *child_widget = getPtrRef(XPGetNthChildWidget(wid, i), widgetIDCapsules, widgetRefName);
    clearChildrenXPWidgetData(child_widget);
    clearXPWidgetData(child_widget);
  }
}

static void clearXPWidgetData(PyObject *widget) {
  pythonDebug(" ... clearing xp widgetdata for %s", objDebug(widget));
  XPWidgetID wid = refToPtr(widget, widgetRefName);
  if (PyDict_GetItem(widgetCallbackDict, widget)) {
    pythonDebug(" widget found in callback dict");
    PyDict_DelItem(widgetCallbackDict, widget);
  }

  /* PropertyDict key is tuple: (<capsule> prop id) */
  PyObject *keys = PyDict_Keys(widgetPropertyDict); /* new */
  PyObject *key_iterator = PyObject_GetIter(keys); /*new*/
  PyObject *key;
  while ((key = PyIter_Next(key_iterator))) {/*new*/
    if (PyObject_RichCompareBool(PyTuple_GetItem(key, WIDGETPROPERTY_WIDGET), widget, Py_EQ)) {/*tuple borrowed*/
      pythonDebug("   deleting widget property: %s", objDebug(PyTuple_GetItem(key, WIDGETPROPERTY_PROPERTY)));
      PyDict_DelItem(widgetPropertyDict, key);
    }
    Py_DECREF(key);
  }
  Py_DECREF(key_iterator);
  Py_DECREF(keys);

  removePtrRef(wid, widgetIDCapsules);
}

My_DOCSTR(_sendMessageToWidget__doc__, "sendMessageToWidget",
          "widgetID, message, dispatchMode=1, param1=0, param2=0",
          "widgetID:XPWidgetID, message:XPWidgetMessage | int, dispatchMode:XPDispatchMode=Mode_UpChain, param1:int=0, param2:int=0",
          "int",
          "dispatchMode default is Mode_UpChain");
static PyObject *XPSendMessageToWidgetFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  errCheck("prior sendMessageToWidget");
  static char *keywords[] = {"widgetID", "message", "dispatchMode", "param1", "param2", NULL};
  (void) self;
  PyObject *widget, *param1=Py_None, *param2=Py_None;
  int inMessage, inMode=xpMode_UpChain;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Oi|iOO", keywords, &widget, &inMessage, &inMode, &param1, &param2)){
    errCheck("Failed to parse tuple sendMessage");
    return NULL;
  }
  errCheck("sendMessage post parse");
  XPWidgetID inWidget = refToPtr(widget, widgetRefName);
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
  static char *keywords[] = {"widgetID", "container", NULL};
  (void) self;
  PyObject *subWidget, *container=Py_None;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|O", keywords, &subWidget, &container)){
    return NULL;
  }
  if (container == Py_None) {
    container = PyLong_FromLong(0);
  }
  XPPlaceWidgetWithin(refToPtr(subWidget, widgetRefName), refToPtr(container, widgetRefName));
  Py_RETURN_NONE;
}

My_DOCSTR(_countChildWidgets__doc__, "countChildWidgets",
          "widgetID",
          "widgetID:XPWidgetID",
          "int",
          "Return number of child widgets for this widgetID");
static PyObject *XPCountChildWidgetsFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"widgetID", NULL};
  (void) self;
  PyObject *widget;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &widget)){
    return NULL;
  }
  int res = XPCountChildWidgets(refToPtr(widget, widgetRefName));
  return PyLong_FromLong(res);
}

My_DOCSTR(_getNthChildWidget__doc__, "getNthChildWidget",
          "widgetID, index",
          "widgetID:XPWidgetID, index:int",
          "XPWidgetID",
          "Return widgetID of 0-based nth child");
static PyObject *XPGetNthChildWidgetFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"widgetID", "index", NULL};
  (void) self;
  PyObject *widget;
  int inIndex;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Oi", keywords, &widget, &inIndex)){
    return NULL;
  }
  XPWidgetID res = XPGetNthChildWidget(refToPtr(widget, widgetRefName), inIndex);
  return getPtrRef(res, widgetIDCapsules, widgetRefName);
}

My_DOCSTR(_getParentWidget__doc__, "getParentWidget",
          "widgetID",
          "widgetID:XPWidgetID",
          "XPWidgetID",
          "Return widgetID for parent (i.e., container) of this widgetID");
static PyObject *XPGetParentWidgetFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"widgetID", NULL};
  (void) self;
  PyObject *widget;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &widget)){
    return NULL;
  }
  XPWidgetID res = XPGetParentWidget(refToPtr(widget, widgetRefName));
  return getPtrRef(res, widgetIDCapsules, widgetRefName);
}

My_DOCSTR(_showWidget__doc__, "showWidget",
          "widgetID",
          "widgetID:XPWidgetID",
          "None",
          "Make widget visible.");
static PyObject *XPShowWidgetFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"widgetID", NULL};
  (void) self;
  PyObject *widget;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &widget)){
    return NULL;
  }
  XPShowWidget(refToPtr(widget, widgetRefName));
  Py_RETURN_NONE;
}

My_DOCSTR(_hideWidget__doc__, "hideWidget",
          "widgetID",
          "widgetID:XPWidgetID",
          "None",
          "Hide widget");
static PyObject *XPHideWidgetFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"widgetID", NULL};
  (void) self;
  PyObject *widget;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &widget)){
    return NULL;
  }
  XPHideWidget(refToPtr(widget, widgetRefName));
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
  static char *keywords[] = {"widgetID", NULL};
  (void) self;
  PyObject *widget;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &widget)){
    return NULL;
  }
  int res = XPIsWidgetVisible(refToPtr(widget, widgetRefName));
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
  static char *keywords[] = {"widgetID", NULL};
  (void) self;
  PyObject *widget;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &widget)){
    return NULL;
  }
  XPWidgetID res = XPFindRootWidget(refToPtr(widget, widgetRefName));
  return(getPtrRef(res, widgetIDCapsules, widgetRefName));
}

My_DOCSTR(_bringRootWidgetToFront__doc__, "bringRootWidgetToFront",
          "widgetID",
          "widgetID:XPWidgetID",
          "None",
          "Make whole widget hierarchy containing widgetID to the front");
static PyObject *XPBringRootWidgetToFrontFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"widgetID", NULL};
  (void) self;
  PyObject *widget;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &widget)){
    return NULL;
  }
  XPBringRootWidgetToFront(refToPtr(widget, widgetRefName));
  Py_RETURN_NONE;
}

My_DOCSTR(_isWidgetInFront__doc__, "isWidgetInFront",
          "widgetID",
          "widgetID:XPWidgetID",
          "int",
          "Return 1 if widget's hierarchy is front most.");
static PyObject *XPIsWidgetInFrontFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"widgetID", NULL};
  (void) self;
  PyObject *widget;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &widget)){
    return NULL;
  }
  int res = XPIsWidgetInFront(refToPtr(widget, widgetRefName));
  return(PyLong_FromLong(res));
}

My_DOCSTR(_getWidgetGeometry__doc__, "getWidgetGeometry",
          "widgetID",
          "widgetID:XPWidgetID",
          "tuple[int, int, int, int]",
          "Return bounding box (left, top, right, bottom) of widgetID");
static PyObject *XPGetWidgetGeometryFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"widgetID", NULL};
  (void) self;
  PyObject *widget;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &widget)){
    return NULL;
  }
  int left, top, right, bottom;
  XPGetWidgetGeometry(refToPtr(widget, widgetRefName), &left, &top, &right, &bottom);
  return Py_BuildValue("(iiii)", left, top, right, bottom);
}

My_DOCSTR(_setWidgetGeometry__doc__, "setWidgetGeometry",
          "widgetID, left, top, right, bottom",
          "widgetID:XPWidgetID, left:int, top:int, right:int, bottom:int",
          "None",
          "Set bounding box for widgetID");
static PyObject *XPSetWidgetGeometryFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"widgetID", "left", "top", "right", "bottom", NULL};
  (void) self;
  PyObject *widget;
  int inLeft, inTop, inRight, inBottom;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Oiiii", keywords, &widget, &inLeft, &inTop, &inRight, &inBottom)){
    return NULL;
  }
  XPSetWidgetGeometry(refToPtr(widget, widgetRefName), inLeft, inTop, inRight, inBottom);
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
  static char *keywords[] = {"container", "xOffset", "yOffset", "recursive", "visibleOnly", NULL};
  (void) self;
  PyObject *container;
  int xOffset, yOffset, recursive=1, visibleOnly=1;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Oii|ii", keywords, &container, &xOffset, &yOffset, &recursive, &visibleOnly)){
    return NULL;
  }
  XPWidgetID res = XPGetWidgetForLocation(refToPtr(container, widgetRefName), xOffset, yOffset, recursive, visibleOnly);
  return getPtrRef(res, widgetIDCapsules, widgetRefName);
}

My_DOCSTR(_getWidgetExposedGeometry__doc__, "getWidgetExposedGeometry",
          "widgetID",
          "widgetID:XPWidgetID",
          "tuple[int, int, int, int]",
          "Return (left, top, right, bottom) of widget's exposed geometry");
static PyObject *XPGetWidgetExposedGeometryFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"widgetID", NULL};
  (void) self;
  PyObject *widget;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &widget)){
    return NULL;
  }
  int left, top, right, bottom;
  XPGetWidgetExposedGeometry(refToPtr(widget, widgetRefName), &left, &top, &right, &bottom);
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
  static char *keywords[] = {"widgetID", "descriptor", NULL};
  (void) self;
  PyObject *widget;
  const char *inDescriptor;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Os", keywords, &widget, &inDescriptor)){
    return NULL;
  }
  XPSetWidgetDescriptor(refToPtr(widget, widgetRefName), inDescriptor);
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
  static char *keywords[] = {"widgetID", NULL};
  (void) self;
  PyObject *widget;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &widget)){
    return NULL;
  }
  int res;
  int length = XPGetWidgetDescriptor(refToPtr(widget, widgetRefName), NULL, 0);
  char buffer[length + 1];
  res = XPGetWidgetDescriptor(refToPtr(widget, widgetRefName), buffer, length);
  if (res > length) {
    printf("Warning: xppython descriptor for widget exceeds buffer size\n");
  }
  buffer[res] = '\0';
  errCheck("end getWidgetDescriptor");
  return PyUnicode_FromString(buffer);
}

My_DOCSTR(_getWidgetUnderlyingWindow__doc__, "getWidgetUnderlyingWindow",
          "widgetID",
          "widgetID:XPWidgetID",
          "XPLMWindowID",
          "Return windowID of window underlying widget");
static PyObject *XPGetWidgetUnderlyingWindowFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"widgetID", NULL};
  (void)self;
  PyObject *widget;
  if(!XPGetWidgetUnderlyingWindow_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPGetWidgetUnderlyingWindow is available only in XPLM301 and up.");
    return NULL;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &widget)){
    return NULL;
  }
  XPLMWindowID res = XPGetWidgetUnderlyingWindow_ptr(refToPtr(widget, widgetRefName));
  return getPtrRef(res, windowIDCapsules, windowIDRef);
}


My_DOCSTR(_setWidgetProperty__doc__, "setWidgetProperty",
          "widgetID, propertyID, value=None",
          "widgetID:XPWidgetID, propertyID:XPWidgetPropertyID | int, value:Optional[Any]",
          "None",
          "Set widget property to value");
static PyObject *XPSetWidgetPropertyFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"widgetID", "propertyID", "value", NULL};
  (void) self;
  PyObject *widget, *value=Py_None;
  int property;
  PyObject *err;
  errCheck("Error prior to  start of setWidgetProperty");
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Oi|O", keywords, &widget, &property, &value)){
    errCheck("Failed to parse setWidgetProperty");
    return NULL;
  }
  XPWidgetPropertyID inProperty = property;
  if (property >= xpProperty_UserStart) {
    PyObject *key = Py_BuildValue("(Oi)", widget, property);
    errCheck("Failed to build setWidgetProperty");
    PyObject *prevValueObj = PyDict_GetItem(widgetPropertyDict, key);
    PyDict_SetItem(widgetPropertyDict, key, value);
    Py_INCREF(value);

    int comparison = 0; /* false */
    if (prevValueObj != NULL) {
      /* value value, do comparison */
      comparison = PyObject_RichCompareBool(value, prevValueObj, Py_EQ);
    }
    if (comparison == 0) {
      /* not found, or they're different */
      XPSendMessageToWidget(refToPtr(widget, widgetRefName), xpMsg_PropertyChanged, xpMode_Direct, property, (intptr_t) value);
      errCheck("after comparison == 0 setwidgetproperty");
    }
  } else {
    XPSetWidgetProperty(refToPtr(widget, widgetRefName), inProperty, value == Py_None ? 0: PyLong_AsLong(value));
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
  static char *keywords[] = {"widgetID", "propertyID", "exists", NULL};
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
      return NULL;
    }
    exception_on_error = 1;
  }
  XPWidgetPropertyID inProperty = property;
  int inExists;

  PyObject *resObj;
  if (property >= xpProperty_UserStart) {
    PyObject *key = Py_BuildValue("(Oi)", widget, property);
    resObj = PyDict_GetItem(widgetPropertyDict, key);
    if (resObj == NULL) {
      /* not found, return 0 */
      resObj = PyLong_FromLong(0);
      inExists = 0;
    } else {
      Py_INCREF(resObj);
      inExists = 1;
    }
    Py_DECREF(key);
  } else {
    intptr_t res = XPGetWidgetProperty(refToPtr(widget, widgetRefName), inProperty, &inExists);
    resObj = PyLong_FromLong(res);
  }
    
  if (exception_on_error && !inExists) {
    PyErr_SetString(PyExc_ValueError, "Widget does not have this property");
    return NULL;
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
  static char *keywords[] = {"widgetID", NULL};
  (void) self;
  PyObject *widget;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &widget)){
    return NULL;
  }
  XPWidgetID res = XPSetKeyboardFocus(refToPtr(widget, widgetRefName));
  PyObject *resObj = getPtrRef(res, widgetIDCapsules, widgetRefName);
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
  static char *keywords[] = {"widgetID", NULL};
  (void) self;
  PyObject *widget;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &widget)){
    return NULL;
  }
  XPLoseKeyboardFocus(refToPtr(widget, widgetRefName));
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
  PyObject *resObj = getPtrRef(res, widgetIDCapsules, widgetRefName);
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
  static char *keywords[] = {"widget", "callback", NULL};
  (void) self;
  PyObject *widget, *callback;
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OO", keywords, &widget, &callback)){
    return NULL;
  }
  PyObject *callbackList = PyDict_GetItem(widgetCallbackDict, widget);
  PyObject *tup = Py_BuildValue("(Os)", callback, CurrentPythonModuleName);
  Py_INCREF(callback);
  if(callbackList == NULL){
    callbackList = PyList_New(0);
    PyList_Append(callbackList, tup);
    PyDict_SetItem(widgetCallbackDict, widget, callbackList);
    //register only the first time
    XPAddWidgetCallback(refToPtr(widget, widgetRefName), widgetCallback);
  }else{
    PyList_Insert(callbackList, 0, tup);
  }
  Py_DECREF(tup);
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
  static char *keywords[] = {"widgetID", NULL};
  (void)self;
  int inWidgetClass; 
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "i", keywords, &inWidgetClass)){
    return NULL;
  }
  XPWidgetFunc_t res = XPGetWidgetClassFunc(inWidgetClass);
  return PyLong_FromVoidPtr(res);
}

static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  PyDict_Clear(widgetCallbackDict);
  Py_DECREF(widgetCallbackDict);
  PyDict_Clear(widgetPropertyDict);
  Py_DECREF(widgetPropertyDict);
  PyDict_Clear(widgetIDCapsules);
  Py_DECREF(widgetIDCapsules);
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
  {"_cleanup", cleanup, METH_VARARGS, ""},
  {NULL, NULL, 0, NULL}
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
  NULL,
  NULL,
  NULL,
  NULL
};

PyMODINIT_FUNC
PyInit_XPWidgets(void)
{
  if(!(widgetCallbackDict = PyDict_New())){
    return NULL;
  }
  PyDict_SetItemString(XPY3pythonDicts, "widgetCallbacks", widgetCallbackDict);
  if(!(widgetPropertyDict = PyDict_New())){
    return NULL;
  }
  PyDict_SetItemString(XPY3pythonDicts, "widgetProperties", widgetPropertyDict);
  if(!(widgetIDCapsules = PyDict_New())){
    return NULL;
  }
  PyDict_SetItemString(XPY3pythonCapsules, widgetRefName, widgetIDCapsules);
  PyObject *mod = PyModule_Create(&XPWidgetsModule);
  if(mod){
    PyModule_AddStringConstant(mod, "__author__", "Peter Buckner (pbuck@xppython3.org)");
  }

  return mod;
}



