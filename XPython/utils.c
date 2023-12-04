#define _GNU_SOURCE 1
#include <Python.h>
#include <assert.h>
#if LIN || APL
#include <execinfo.h>
#endif
#include <frameobject.h>
#include <sys/time.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdio.h>
#include "utils.h"
#include "ini_file.h"
#include <XPLM/XPLMUtilities.h>

const char *objRefName = "XPLMObjectRef";
const char *commandRefName = "XPLMCommandRef";
const char *widgetRefName = "XPLMWidgetID";

char CurrentPythonModuleName[512] = "Main";

void c_backtrace();
int pythonWarnings = 1;  /* 1= issue warnings, 0= do not */
int pythonDebugs = 0; /* 1= issue DEBUG messages, 0= do not */
int pythonFlushLog = 0; /* 1= flush python log after every wrinte, 0= buffer writes */
int pythonCapsuleRegistration = 0;  /* 1= log registration of all capsules (REQUIRES ERRCHECK) */
FILE *pythonLog_fp = NULL;

#define CAPSULE_CAPSULE 0
#define CAPSULE_MODULE 1

PyObject *get_pythonline();

void setLogFile(void) {
  /* Log file name is (in order of preference):
     1) environment variable: XPPYTHON3_LOG
     2) config file value: xppython3.ini [Main].log_file_name
     3) compiled default: XPPython3Log.txt
     * If value is 'Log.txt' (exactly) we'll send messages via XPLMDebugMessage()
       to Log.txt.
     * If _set_ value is null (XPPTHON3_LOG=, or [Main].log_file_name=),
       OR we cannot find/open the file.
       We'll use stdout

     File Preservation is:
     1) environment variable: XPPYTHON3_PRESERVE (if defined, it's true)
     2) config file value: xppython3.ini [Main].log_file_preserve (any non-zero value is true)
     3) compiled default: False

     If we cannot open log file, it is set to stdout
  */
  
  static char *ENV_logFileVar = "XPPYTHON3_LOG";  // set this environment to override logFileName
  static char *ENV_logPreserve = "XPPYTHON3_PRESERVE";  // DO NOT truncate XPPython log on startup. If set, we preserve, if unset, we truncate
  static char *logFileName = "XPPython3Log.txt";
  char *log;
  pythonFlushLog = xpy_config_get_int("[Main].flush_log");/* 0= off, 1= on */
  log = xpy_config_get("[Main].log_file_name");
  if (log != NULL) {
    logFileName = log;
  }
  log = getenv(ENV_logFileVar);
  if(log != NULL){
    logFileName = log;
  }
  
  char *msg;
  int preserve = 0;
  if (0 == strcmp(logFileName, "Log.txt")) {     /* Special case, combine python log an Log.txt */
    pythonLog_fp = NULL;
  } else { /* Not 'Log.txt'... -- try to open provided logFileName */
    preserve = getenv(ENV_logPreserve) != NULL || xpy_config_get_int("[Main].log_file_preserve") != 0;
    pythonLog_fp = fopen(logFileName, preserve ? "a" : "w");
    if(pythonLog_fp == NULL) {
      preserve = 0;
      pythonLog_fp = stdout;
      logFileName = "standard out";
    }
  }

  if (-1 == asprintf(&msg, "[XPPython3] Starting %s (compiled: %0x)... Logging to %s%s\n",
                     pythonPluginVersion, PY_VERSION_HEX, logFileName, preserve ? "+" : "")) {
    pythonLog("Failed to allocate asprintf memory, failed to start.");
  }
  XPLMDebugString(msg);
  free(msg);
}


void pythonLogClose(void) {
  if (pythonLog_fp) {
    fclose(pythonLog_fp);
  } /* else... we're writing to DebugString, which is already flushed each time */
}
  
void pythonLogFlush(void) {
  if (pythonLog_fp) {
    fflush(pythonLog_fp);
  } /* else... we're writing to DebugString, which is already flushed each time */
}

void pythonLog(const char *fmt, ...) {
  /* do not include terminating newline */
    char *msg;
    va_list ap;
    va_start(ap, fmt);
    if (-1 == vasprintf(&msg, fmt, ap)) {
      msg = "Failed to allocation vasprintf memory in pythonDebug";
      if (pythonLog_fp) {
        fprintf(pythonLog_fp, "%s\n", msg);
        if (pythonFlushLog) {
          fflush(pythonLog_fp);
        }
      } else {
        XPLMDebugString(msg);
        XPLMDebugString("\n");
      }
      return;
    }
    va_end(ap);
    if (pythonLog_fp) {
      fprintf(pythonLog_fp, "%s\n", msg);
      pythonLogFlush();
    } else {
      XPLMDebugString(msg);
      XPLMDebugString("\n");
    }
    free(msg);
}

void pythonDebug(const char *fmt, ...) {
  /* do not include terminating newline */
  if (pythonDebugs) {
    char *msg;
    va_list ap;
    va_start(ap, fmt);
    if (-1 == vasprintf(&msg, fmt, ap)) {
      msg = "Failed to allocation vasprintf memory in pythonDebug";
      pythonLog("%s", msg);
      return;
    }
    va_end(ap);
    pythonLog("DEBUG>> %s", msg);
    free(msg);
  }
}

void pythonLogWarning(const char *msg) {
  if (pythonWarnings) {
    PyObject *python_line = get_pythonline();
    char *s = objToStr(python_line);
    pythonLog("WARNING>> %s: %s", s, msg);
    free(s);
  }
}

void dbg(const char *msg){
  printf("Going to check %s\n", msg);
  PyObject *err = PyErr_Occurred();
  if(err){
    printf("Error occured during the %s call:\n", msg);
    pythonLogException();
  }
}


char * objDebug(PyObject *item) {
  /* Same as objToStr, but is a no-op if pythonDebugs not set. (saves us from work and memory) */
  if (pythonDebugs) {
    // returns char * pointer to something in heap
    PyObject *pyAsStr = PyObject_Str(item); // new object
    PyObject *pyBytes = PyUnicode_AsEncodedString(pyAsStr, "utf-8", "replace"); // new object
    
    PyObject *err = PyErr_Occurred();
    if(err){
      pythonLog("[XPPython3] Error occured during objToStr");
      pythonLogException();
      return strdup("<Error>");
    }
    char *res = PyBytes_AsString(pyBytes);  //borrowed (from pyBytes)
    res = strdup(res); // allocated on heap
    Py_DECREF(pyAsStr);
    Py_DECREF(pyBytes);
    return res;
  }
  return "";
}

void pythonLogException()
{
  PyObject *ptype, *pvalue, *ptraceback;
  int full_traceback = 0;  /* set to '1' if we're able to do a full traceback */
  char *foo;

  PyErr_Fetch(&ptype, &pvalue, &ptraceback);
  PyErr_NormalizeException(&ptype, &pvalue, &ptraceback);
  if (!ptype && !pvalue && !ptraceback) {
    return; /* no current exception */
  }
  PyErr_Clear();

  if (ptraceback != NULL) {
    PyException_SetTraceback(pvalue, ptraceback);
  }

  PyObject *module = get_moduleName_p();

  /* First, attempt to fully format exception with traceback
     If the _fails_ then we'll print the Exception type & value below.
   */
  PyObject *tb_module_name = PyUnicode_DecodeFSDefault("traceback"); /* new */
  PyObject *tb_module = PyImport_Import(tb_module_name); /* new */
  Py_DECREF(tb_module_name);
  if (tb_module != NULL) {
    PyObject *fmt_exception = PyObject_GetAttrString(tb_module, "format_exception");
    if (fmt_exception && PyCallable_Check(fmt_exception)) {
      PyObject *vals;
      if (pvalue && ptraceback) {
        vals = PyObject_CallFunctionObjArgs(fmt_exception, ptype, pvalue, ptraceback, NULL);
      } else if (pvalue) {
        pythonDebug("(logging exception without traceback)\n");
        PyObject *fmt_exception_only = PyObject_GetAttrString(tb_module, "format_exception_only");
        vals = PyObject_CallFunctionObjArgs(fmt_exception_only, ptype, pvalue, NULL);
        Py_DECREF(fmt_exception_only);
      } else {
        pythonDebug("(logging exception without traceback or pvalue)\n");
        vals = PyObject_CallFunctionObjArgs(fmt_exception, ptype, pvalue, NULL);
      }
      if (vals == NULL) {
        if(PyErr_Occurred()) {
          pythonLog("[XPPython3] Unable to format exception");
          PyErr_Print();
        }
      } else {
        PyObject *localsDict = PyDict_New();
        PyDict_SetItemString(localsDict, "__builtins__", PyEval_GetBuiltins()); 
        PyDict_SetItemString(localsDict, "vals", vals);
        PyDict_SetItemString(localsDict, "module", module);
        MyPyRun_String("ret = []\n"
                       "[ret.extend(x.split('\\n')) for x in vals]\n"
                       "ret = '\\n'.join(['EXCEPTION>> [{}] {}'.format(module or 'Main', x) for x in ret])\n",
                       Py_file_input, localsDict, localsDict);
        PyObject *tb_string = PyDict_GetItemString(localsDict, "ret"); /* borrowed */
        char *s = objToStr(tb_string);
        Py_DECREF(localsDict);
        pythonLog( "%s", s);
        free(s);
        full_traceback = 1;
        Py_DECREF(vals);
      }
      Py_DECREF(fmt_exception);
    }
  }

  if (!full_traceback) {
    /* Failed to print full traceback. Print what we can. */
    foo = objToStr(ptype);
    pythonLog("EXCEPTION>> [%s] type: %s", CurrentPythonModuleName, foo);
    free(foo);
    foo = objToStr(pvalue);
    pythonLog("EXCEPTION>> [%s] value: %s", CurrentPythonModuleName, foo);
    free(foo);
  }

  c_backtrace();

  Py_XDECREF(ptype);
  Py_XDECREF(pvalue);
  Py_XDECREF(ptraceback);
  Py_XDECREF(module);
}

char * objToStr(PyObject *item) {
  errCheck("prior objToStr");
  // returns char * pointer to something in heap
  PyObject *pyAsStr = PyObject_Str(item); // new object
  PyObject *pyBytes = PyUnicode_AsEncodedString(pyAsStr, "utf-8", "replace"); // new object

  PyObject *err = PyErr_Occurred();
  if(err){
    pythonLog("[XPPython3] Error occured during objToStr");
    pythonLogException();
    return strdup("<Error>");
  }
  char *res = PyBytes_AsString(pyBytes);  //borrowed (from pyBytes)
  res = strdup(res); // allocated on heap
  Py_DECREF(pyAsStr);
  Py_DECREF(pyBytes);
  errCheck("end objToStr");
  return res;
}
  
void set_moduleName(PyObject *name) {
  /* takes Python String */
  char *tmp = objToStr(name);
  strcpy(CurrentPythonModuleName, tmp);
  free(tmp);
}

PyObject *get_moduleName_p() {
  PyObject *pModule = PyUnicode_FromString(CurrentPythonModuleName);
  Py_INCREF(pModule);
  return pModule;
}

#if !defined(Py_LIMITED_API)
PyObject *get_pythonline() {
  // returns heap-allocated PyObject (or Py_RETURN_NONE)
  PyGILState_STATE gilState = PyGILState_Ensure();
  PyThreadState *tstate = PyThreadState_Get();
  PyObject *last_filenameObj = Py_None;
  int line = 0;
#if PY_VERSION_HEX > 0x030b0000
  if (NULL != tstate && NULL != PyThreadState_GetFrame(tstate)) {
    PyFrameObject *frame = PyThreadState_GetFrame(tstate);
    if (frame) {
      last_filenameObj = PyFrame_GetCode(frame)->co_filename;
      // line = PyCode_Addr2Line(frame->f_code, frame->f_lasti);  /* This is not available on window version */
      line = PyFrame_GetLineNumber(frame);
    }
  }
#else
    if (NULL != tstate && NULL != tstate->frame) {
    PyFrameObject *frame = tstate->frame;
    if (frame) {
      last_filenameObj = frame->f_code->co_filename;
      // line = PyCode_Addr2Line(frame->f_code, frame->f_lasti);  /* This is not available on window version */
      line = frame->f_lineno;
    }
  }
#endif
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
#endif

// To avoid Python code messing with raw pointers (when passed
//   in using PyLong_FromVoidPtr), these are hidden in the capsules.

// Can be used where no callbacks are involved in passing the capsule
PyObject *getPtrRefOneshot(void *ptr, const char *refName)
{
  if(!ptr){
    Py_RETURN_NONE;
  }
  errCheck("prior getPtrRefOneshot: %s", refName);
  PyObject *ret = PyCapsule_New(ptr, refName, NULL);
  errCheck("end getPtrRefOneShot");
  return ret;
}

PyObject *getPtrRef(void *ptr, PyObject *dict, const char *refName)
{
  /* converts a 'C' pointer to a Capsule reference of <refName> type,
     IF, pointer has been convereted before, we return the given Capsule.
     ELSE, we create a new capsule, storing info in dict, for later retrieval.
  */
  if(!ptr){
    Py_RETURN_NONE;
  }
  // Check if the refernece is known
  errCheck("prior getPtrRef %s", refName);
  PyObject *key = PyLong_FromVoidPtr(ptr);
#if ERRCHECK
  PyObject *tuple = PyDict_GetItemWithError(dict, key); /* borrowed, NULL w/out exception, if not found */
  PyObject *capsule = tuple == NULL ? NULL : PyTuple_GetItem(tuple, CAPSULE_CAPSULE);
#else
  PyObject *capsule = PyDict_GetItemWithError(dict, key); /* borrowed */
#endif
  if(capsule == NULL){
    // New ref, register it
    capsule = getPtrRefOneshot(ptr, refName);
#if ERRCHECK
    if (pythonCapsuleRegistration) {
      char *res_s = objToStr(capsule);
      pythonLog("  (%s registering %s for %p)", CurrentPythonModuleName, res_s, ptr);
      free(res_s);
    }
#endif
#if ERRCHECK
    PyObject *tuple = PyTuple_Pack(2, capsule, get_moduleName_p());
    PyDict_SetItem(dict, key, tuple);
#else
    PyDict_SetItem(dict, key, capsule);
#endif
  }
  Py_INCREF(capsule);
  errCheck("end getPtrRef");
  return capsule;
}

void *refToPtr(PyObject *ref, const char *refName)
{
  errCheck("prior refToPtr %s", refName);
  /* XPLMWidgetID can be 0, refering to underlying X-Plane window, need to keep that */
  if (ref == Py_None) return NULL;
  if (refName == NULL) {
    refName = PyCapsule_GetName(ref);
    errCheck("Bad PyCapsule_GetName()");
  }
  if (!strcmp(widgetRefName, refName) && PyLong_Check(ref) && PyLong_AsLong(ref) == 0) {
    return NULL;
  }

  void *ptr = PyCapsule_GetPointer(ref, refName);
  if (pythonDebugs) {
    PyObject *err = PyErr_Occurred();
    if(err){
      pythonLogException();
      char *s = objToStr(ref);
      pythonDebug("Failed to convert '%s' capsule (%s) to pointer\n", refName, s);
      char msg[1024];
      sprintf(msg, "Failed to convert '%s' capsule (%s) to pointer\n", refName, s);
      PyErr_SetString(PyExc_ValueError , msg);
      free(s);
      return NULL;
    }
  }
  errCheck("end refToPtr");
  return ptr;
}

void removePtrRef(void *ptr, PyObject *dict)
{
  errCheck("prior removePtrRef");
  if(!ptr){
    return;
  }
  PyObject *key = PyLong_FromVoidPtr(ptr);
  PyDict_DelItem(dict, key);
  Py_DECREF(key);
  errCheck("end removePtrRef");
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


void MyPyRun_String(const char *str, int start, PyObject *globals, PyObject *locals) {
  PyObject *codeObj = Py_CompileString(str, "<input>", start);
  PyEval_EvalCode(codeObj, globals, locals);
  Py_DECREF(codeObj);
}

void MyPyRun_File(FILE *fp, const char *filename, int start, PyObject *globals, PyObject *locals) {
  (void) filename;  /* ?? */
  char *buffer = 0;
  long length;
  if(fp) {
    fseek(fp, 0, SEEK_END);
    length = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    buffer = malloc(length+1);
    if (buffer) {
      fread(buffer, 1, length, fp);
      buffer[length] = '\0';
    }
    if (buffer) {
      MyPyRun_String(buffer, start, globals, locals);
    }
  }
}

void c_backtrace() {
#if LIN || APL
  printf("Backtrace due to python exception:\n");
  void *callstack[128];
  int i, frames = backtrace(callstack, 128);
  char ** strs = backtrace_symbols(callstack, frames);
  for (i = 0; i < frames; ++i) {
    printf("%s\n", strs[i]);
  }
  free(strs);
  assert (1 == 2);
#endif
}

void errCheck_f(const char *fmt, ...) {
  /* (you should NOT include terminating new line!) */
  char *msg;
  va_list ap;
  PyObject *err = PyErr_Occurred();
  if (err) {
    va_start(ap, fmt);
    if (-1 == vasprintf(&msg, fmt, ap)) {
      msg = "Failed to allocation vasprintf memory in errCheck";
      pythonLog(msg);
      return;
    }
    va_end(ap);
    
    pythonLog("%s", msg);
    pythonLogException();
  }
}
