#define _GNU_SOURCE 1
#include <Python.h>
#include <frameobject.h>
#include <sys/time.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>
#include "utils.h"

const char *objRefName = "XPLMObjectRef";
const char *commandRefName = "XPLMCommandRef";
const char *widgetRefName = "XPLMWidgetID";

int pythonWarnings = 1;  /* 1= issue warnings, 0= do not */
int pythonDebugs = 0; /* 1= issue DEBUG messages, 0= do not */

PyObject *get_pythonline();

void pythonDebug(const char *fmt, ...) {
  //PyObject *err = PyErr_Occurred();
  bool err = 0;
  if (err || pythonDebugs) {
    Py_VerboseFlag = 0;/* 0= off, 1= each file as loaded, 2= each file that is checked when searching for module */
    char *msg;
    va_list ap;
    va_start(ap, fmt);
    vasprintf(&msg, fmt, ap);
    va_end(ap);
    if (err) {
      fprintf(pythonLogFile, "PyErr occurred: check log\n");
      fflush(pythonLogFile);
      PyErr_Print();
    }
    fprintf(pythonLogFile, "DEBUG>> %s\n", msg);
    fflush(pythonLogFile);
    free(msg);
  }
}

void pythonLogWarning(const char *msg) {
  if (pythonWarnings) {
    PyObject *python_line = get_pythonline();
    fprintf(pythonLogFile, "WARNING>> %s: %s\n", objToStr(python_line), msg);
    fflush(pythonLogFile);
  }
}

void dbg(const char *msg){
  printf("Going to check %s\n", msg);
  PyObject *err = PyErr_Occurred();
  if(err){
    printf("Error occured during the %s call:\n", msg);
    PyErr_Print();
  }
}

char * objToStr(PyObject *item) {
  // returns char * pointer to something in heap
  PyObject *pyAsStr = PyObject_Str(item); // new object
  PyObject *pyBytes = PyUnicode_AsEncodedString(pyAsStr, "utf-8", "replace"); // new object
  char *res = PyBytes_AsString(pyBytes);  //borrowed (from pyBytes)
  res = strdup(res); // allocated on heap
  Py_DECREF(pyAsStr);
  Py_DECREF(pyBytes);
  return res;
}
  
float getFloatFromTuple(PyObject *seq, Py_ssize_t i)
{
  return PyFloat_AsDouble(PyTuple_GetItem(seq, i));
}

long getLongFromTuple(PyObject *seq, Py_ssize_t i)
{
  return PyLong_AsLong(PyTuple_GetItem(seq, i));
}

PyObject *get_module() {
  PyGILState_STATE gilState = PyGILState_Ensure();
  PyThreadState *tstate = PyThreadState_Get();
  PyFrameObject *last_frame = NULL, *frame = NULL;
  PyObject *moduleName = Py_None;
  if (NULL != tstate && NULL != tstate->frame) {
    frame = tstate->frame;
    while (NULL != frame) {
      last_frame = frame;
      frame = frame->f_back;
    }
  }
  if (last_frame) {
    moduleName = PyDict_GetItemString(last_frame->f_globals, "__name__");
  }
  PyGILState_Release(gilState);
  return moduleName;
}
  
char *get_moduleName() {
  return objToStr(get_module());
}
  
PyObject *get_pluginSelf() {
  return get_module();
}

PyObject *get_pythonline() {
  // returns heap-allocated PyObject (or Py_RETURN_NONE)
  PyGILState_STATE gilState = PyGILState_Ensure();
  PyThreadState *tstate = PyThreadState_Get();
  PyObject *last_filenameObj = Py_None;
  int line = 0;
  if (NULL != tstate && NULL != tstate->frame) {
    PyFrameObject *frame = tstate->frame;
    if (frame) {
      last_filenameObj = frame->f_code->co_filename;
      // line = PyCode_Addr2Line(frame->f_code, frame->f_lasti);  /* This is not available on window version */
      line = frame->f_lineno;
    }
  }

  char *last_filename = objToStr(last_filenameObj); // allocates new string on heap
  char *token = strrchr(last_filename, '/');
  if (token == NULL) {
    token = strrchr(last_filename, '\\');
    if (token == NULL) {
      token = strrchr(last_filename, ':');
    }
  }
  PyGILState_Release(gilState);
  if (token) {
    char msg[1024];
    sprintf(msg, "%s:%d", ++token, line);
    PyObject *ret = PyUnicode_FromString(msg); // return new item, we then free the char*
    free(last_filename);
    return ret;
  }
  free(last_filename);
  Py_RETURN_NONE;
}

// To avoid Python code messing with raw pointers (when passed
//   in using PyLong_FromVoidPtr), these are hidden in the capsules.

// Can be used where no callbacks are involved in passing the capsule
PyObject *getPtrRefOneshot(void *ptr, const char *refName)
{
  if(ptr){
    return PyCapsule_New(ptr, refName, NULL);
  }else{
    Py_RETURN_NONE;
  }
}

PyObject *getPtrRef(void *ptr, PyObject *dict, const char *refName)
{
  if(!ptr){
    Py_RETURN_NONE;
  }
  // Check if the refernece is known
  PyObject *key = PyLong_FromVoidPtr(ptr);
  PyObject *res = PyDict_GetItem(dict, key);
  if(res == NULL){
    // New ref, register it
    res = getPtrRefOneshot(ptr, refName);
    PyDict_SetItem(dict, key, res);
  }
  Py_INCREF(res);
  return res;
}

void *refToPtr(PyObject *ref, const char *refName)
{
  /* XPLMWidgetID can be 0, refering to underlying X-Plane window, need to keep that */
  if (ref == Py_None || (!strcmp(widgetRefName, refName) && PyLong_Check(ref) && PyLong_AsLong(ref) == 0)){
    return NULL;
  }else{
    return PyCapsule_GetPointer(ref, refName);
  }
}

void removePtrRef(void *ptr, PyObject *dict)
{
  if(!ptr){
    return;
  }
  PyObject *key = PyLong_FromVoidPtr(ptr);
  PyDict_DelItem(dict, key);
  Py_DECREF(key);
}

/* char *get_module(PyThreadState *tstate) { */
/*   /\* returns filename of top most frame -- this will be the Plugin's file *\/ */
/*   char *last_filename = "[unknown]"; */
/*   if (NULL != tstate && NULL != tstate->frame) { */
/*     PyFrameObject *frame = tstate->frame; */
    
/*     while (NULL != frame) { */
/*       // int line = frame->f_lineno; */
/*       /\* */
/*         frame->f_lineno will not always return the correct line number */
/*         you need to call PyCode_Addr2Line(). */
/*       *\/ */
/*       // int line = PyCode_Addr2Line(frame->f_code, frame->f_lasti); */
/*       PyObject *temp_bytes = PyUnicode_AsEncodedString(frame->f_code->co_filename, "utf-8", "replace"); */
/*       const char *filename = PyBytes_AsString(temp_bytes); */
/*       filename = strdup(filename); */
/*       last_filename = strdup(filename); */
/*       Py_DECREF(temp_bytes); */

/*       temp_bytes = PyUnicode_AsEncodedString(frame->f_code->co_name, "utf-8", "replace"); */
/*       const char *funcname = PyBytes_AsString(temp_bytes); */
/*       funcname = strdup(funcname); */
/*       Py_DECREF(temp_bytes); */
/*       frame = frame->f_back; */
/*     } */
/*   } */

/*   char *token = strrchr(last_filename, '/'); */
/*   if (token == NULL) { */
/*     token = strrchr(last_filename, '\\'); */
/*     if (token == NULL) { */
/*       token = strrchr(last_filename, ':'); */
/*     } */
/*   } */
/*   if (token) { */
/*     return (++token); */
/*   } */
/*   return "Unknown"; */
/* } */


