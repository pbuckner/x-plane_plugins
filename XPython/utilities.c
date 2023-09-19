#define _GNU_SOURCE 1
#include <Python.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdbool.h>
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMUtilities.h>
#include "utils.h"

PyObject *errCallbacks;
PyObject *commandCallbackDict;  // "(OOOiO)", pluginSelf, inCommand, inHandler, inBefore, inRefcon
/* key is integer we pass as the refcon back to X-Plane with a generate callback(),
   so we know which method we should call
*/
#define CALLBACK_PLUGIN 0
#define CALLBACK_CAPSULE 1
#define CALLBACK_METHOD 2
#define CALLBACK_BEFORE 3
#define CALLBACK_REFCON 4

PyObject *commandRefcons;
PyObject *commandCapsules;
intptr_t commandCallbackCntr;

void clearInstanceCommands(PyObject *pluginSelf);

static void error_callback(const char *inMessage)
{
  //TODO: send the error only to the active plugin?
  // for now, pass to all registered
  Py_ssize_t cnt = 0;
  PyObject *pKey = NULL, *pVal = NULL;
  PyObject *msg = PyUnicode_DecodeUTF8(inMessage, strlen(inMessage), NULL);
  while(PyDict_Next(errCallbacks, &cnt, &pKey, &pVal)){
    PyObject *oRes = PyObject_CallFunctionObjArgs(pVal, msg, NULL);
    PyObject *err = PyErr_Occurred();
    if(err){
      pythonLogException();
    }else{
      Py_DECREF(oRes);
    }
  }
  Py_DECREF(msg);
}


My_DOCSTR(_speakString__doc__, "speakString", "string",
          "Display string in translucent overaly and speak string");
static PyObject *XPLMSpeakStringFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"string", NULL};
  (void) self;
  const char *inString;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "s", keywords, &inString)){
    return NULL;
  }
  XPLMSpeakString(inString);
  Py_RETURN_NONE;
}

My_DOCSTR(_getVirtualKeyDescription__doc__, "getVirtualKeyDescription", "vKey",
          "Return human-readable string describing virtual key");
static PyObject *XPLMGetVirtualKeyDescriptionFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"vKey", NULL};
  (void) self;
  int inVirtualKey;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "i", keywords, &inVirtualKey)){
    return NULL;
  }
  const char *res = XPLMGetVirtualKeyDescription(inVirtualKey);
  return PyUnicode_DecodeUTF8(res, strlen(res), NULL);
}

My_DOCSTR(_reloadScenery__doc__, "reloadScenery", "",
          "Reload current set of scenery");
static PyObject *XPLMReloadSceneryFun(PyObject *self, PyObject *args)
{
  (void)self;
  (void)args;
  XPLMReloadScenery();
  Py_RETURN_NONE;
}

My_DOCSTR(_getSystemPath__doc__, "getSystemPath", "",
          "Return full page to X-Plane folder, with trailing '/'");
static PyObject *XPLMGetSystemPathFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  char outSystemPath[1024];
  XPLMGetSystemPath(outSystemPath);

  return PyUnicode_DecodeUTF8(outSystemPath, strlen(outSystemPath), NULL);
}

My_DOCSTR(_getPrefsPath__doc__, "getPrefsPath", "",
          "Get path the *file* within X-Plane's preferences directory.");
static PyObject *XPLMGetPrefsPathFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  char outPrefsPath[1024];
  XPLMGetPrefsPath(outPrefsPath);

  return PyUnicode_DecodeUTF8(outPrefsPath, strlen(outPrefsPath), NULL);
}

My_DOCSTR(_getDirectorySeparator__doc__, "getDirectorySeparator", "",
          "Get string used for directory separator for the current platform.\n"
          "\n"
          "Don't use this, use python os.path.join() related routines.");
static PyObject *XPLMGetDirectorySeparatorFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  
  const char *res = XPLMGetDirectorySeparator();

  return PyUnicode_DecodeUTF8(res, 1, NULL);
}


My_DOCSTR(_extractFileAndPath__doc__, "extractFileAndPath", "fullPath",
          "Given a full path, separate path from file\n"
          "\n"
          "Don't use this, use os.path routines instead");
static PyObject *XPLMExtractFileAndPathFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"fullPath", NULL};
  (void) self;
  const char *inFullPathConst;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "s", keywords, &inFullPathConst)){
    return NULL;
  }
  char *inFullPath = strdup(inFullPathConst);
  const char *res = XPLMExtractFileAndPath(inFullPath);
  
  PyObject *resObj = Py_BuildValue("(ss)", res, inFullPath);
  free(inFullPath);
  return resObj;
}

My_DOCSTR(_getDirectoryContents__doc__, "getDirectoryContents", "dir, firstReturn=0, bufSize=2048, maxFiles=100",
          "Get contents (files and subdirectories) of directory\n"
          "\n"
          "Don't use this, use python os.walk() or glob.glob() instead.");
static PyObject *XPLMGetDirectoryContentsFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"dir", "firstReturn", "bufSize", "maxFiles", NULL};
  (void) self;
  const char *inDirectoryPath;
  int inFirstReturn=0;
  int inFileNameBufSize=2048;
  int inIndexCount=100;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "s|iii", keywords, &inDirectoryPath, &inFirstReturn, &inFileNameBufSize, &inIndexCount)){
    return NULL;
  }
  char *outFileNames = (char *)malloc(inFileNameBufSize);
  char **outIndices = (char **)malloc(inIndexCount * sizeof(char *));
  int outTotalFiles;
  int outReturnedFiles;

  int res = XPLMGetDirectoryContents(inDirectoryPath, inFirstReturn, outFileNames, inFileNameBufSize,
                                     outIndices, inIndexCount, &outTotalFiles, &outReturnedFiles);
  
  PyObject *namesList = PyList_New(0);
  PyObject *tmp;
  for(int i = 0; i < outReturnedFiles; ++i){
    if(outIndices[i] != NULL){
      tmp = PyUnicode_DecodeUTF8(outIndices[i], strlen(outIndices[i]), NULL);
      PyList_Append(namesList, tmp);
      Py_DECREF(tmp);
    }else{
      break;
    }
  }

  PyObject *retObj = Py_BuildValue("(iOi)", res, namesList, outTotalFiles);

  free(outFileNames);
  free(outIndices);
  return retObj;
}

My_DOCSTR(_getVersions__doc__, "getVersions", "",
          "Return tuple with (X-Plane, XPLM SDK, and hostID)\n"
          "\n"
          "Host ID is either XPlane=1 or Unknown=0");
static PyObject *XPLMGetVersionsFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  int outXPlaneVersion;
  int outXPLMVersion;
  XPLMHostApplicationID outHostID;

  XPLMGetVersions(&outXPlaneVersion, &outXPLMVersion, &outHostID);
  return Py_BuildValue("(iii)", outXPlaneVersion, outXPLMVersion, (int)outHostID);
}

My_DOCSTR(_getLanguage__doc__, "getLanguage", "",
          "Return language code the sim is running in.");
static PyObject *XPLMGetLanguageFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  
  XPLMLanguageCode res = XPLMGetLanguage();

  return PyLong_FromLong(res);
}

My_DOCSTR(_debugString__doc__, "debugString", "string",
          "Write string to 'Log.txt' file, with immediate buffer flush\n"
          "\n"
          "Use xp.systemLog() instead, to add newline and prefix with your\n"
          "plugin's name instead. Use xp.log() to write to XPPython3Log.txt file");
static PyObject *XPLMDebugStringFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"string", NULL};
  (void) self;
  const char *inString;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "s", keywords, &inString)){
    return NULL;
  }
  XPLMDebugString(inString);
  Py_RETURN_NONE;
}


//Assumption:
// Single error reporting callback per PI_* plugin!
My_DOCSTR(_setErrorCallback__doc__, "setErrorCallback", "callback",
          "Install error-reporting callback for your plugin\n"
          "\n"
          "Likely not useful for python debugging.");
static PyObject *XPLMSetErrorCallbackFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"callback", NULL};
  (void) self;

  if(PyDict_Size(errCallbacks) == 0){
    XPLMSetErrorCallback(error_callback);
  }

  PyObject *pluginSelf;
  PyObject *callback;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &callback)) {
    return NULL;
  }
  pluginSelf = get_pluginSelf();
  PyDict_SetItem(errCallbacks, pluginSelf, callback);
  Py_DECREF(pluginSelf);
  
  Py_RETURN_NONE;
}

My_DOCSTR(_findSymbol__doc__, "findSymbol", "symbol",
          "Find C-API symbol. See documentation.");
static PyObject *XPLMFindSymbolFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"symbol", NULL};
  (void) self;
  
  const char *inString;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "s", keywords, &inString)){
    return NULL;
  }
  return PyLong_FromVoidPtr(XPLMFindSymbol(inString));
}

My_DOCSTR(_loadDataFile__doc__, "loadDataFile", "fileType, path",
          "Load data file given by path\n"
          "\n"
          "fileType is:\n"
          "  DataFile_Situation   = 1\n"
          "  DataFile_ReplayMovie = 2\n"
          "Path is either absolute or relative X-Plane root.\n"
          "Returns 1 on success (file found), 0 otherwise.");
static PyObject *XPLMLoadDataFileFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"fileType", "path", NULL};
  (void) self;
  int inFileType;
  const char *inFilePath;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "is", keywords, &inFileType, &inFilePath)){
    return NULL;
  }
  int res = XPLMLoadDataFile(inFileType, inFilePath);
  return PyLong_FromLong(res);
}

My_DOCSTR(_saveDataFile__doc__, "saveDataFile", "fileType, path",
          "Saves data file to disk.\n"
          "\n"
          "fileType is:\n"
          "  DataFile_Situation   = 1\n"
          "  DataFile_ReplayMovie = 2\n"
          "Path is either absolute or relative X-Plane root.\n"
          "Returns 1 on success (file found), 0 otherwise.");
static PyObject *XPLMSaveDataFileFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"fileType", "path", NULL};
  (void) self;
  int inFileType;
  const char *inFilePath;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "is", keywords, &inFileType, &inFilePath)){
    return NULL;
  }
  int res = XPLMSaveDataFile(inFileType, inFilePath);
  return PyLong_FromLong(res);
}

static int commandCallback(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void *inRefcon)
{
  PyObject *pID = PyLong_FromVoidPtr(inRefcon);
  PyObject *pCbk = PyDict_GetItem(commandCallbackDict, pID);
  Py_DECREF(pID);
  if(pCbk == NULL){
    printf("Received unknown commandCallback refCon (%p).\n", inRefcon);
    return -1;
  }

  PyObject *arg1 = getPtrRef(inCommand, commandCapsules, commandRefName);
  PyObject *arg2 = PyLong_FromLong(inPhase);
  PyObject *oRes = PyObject_CallFunctionObjArgs(PyTuple_GetItem(pCbk, CALLBACK_METHOD), arg1, arg2, PyTuple_GetItem(pCbk, CALLBACK_REFCON), NULL);
  Py_DECREF(arg1);
  Py_DECREF(arg2);
  PyObject *err = PyErr_Occurred();
  if(err){
    pythonLog("Error in CommandCallback [%s] %s\n",
            objToStr(PyTuple_GetItem(pCbk, CALLBACK_PLUGIN)),
            objToStr(PyTuple_GetItem(pCbk, CALLBACK_METHOD)));
    /* pass error back up */
    /* 
       PyErr_SetString(err, msg);
       pythonLogException();
    */
    return 0;
  }
  if (!(oRes && PyLong_Check(oRes))) {
    pythonLog("[%s] %s CommandCallback returned '%s' rather than an integer.\n",
            objToStr(PyTuple_GetItem(pCbk, CALLBACK_PLUGIN)),
            objToStr(PyTuple_GetItem(pCbk, CALLBACK_METHOD)),
            objToStr(oRes));
    return 1;  /* return '1' to allow X-Plane to continue processing */
  }
  int res = PyLong_AsLong(oRes);
  Py_DECREF(oRes);
  return res;
}

My_DOCSTR(_findCommand__doc__, "findCommand", "name",
          "Return commandRef for named command or None");
static PyObject *XPLMFindCommandFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"name", NULL};
  (void) self;
  const char *inName;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "s", keywords, &inName)){
    return NULL;
  }
  XPLMCommandRef res = XPLMFindCommand(inName);
  return getPtrRef(res, commandCapsules, commandRefName);
}

My_DOCSTR(_commandBegin__doc__, "commandBegin", "commandRef",
          "Start execution of command specified by commandRef");
static PyObject *XPLMCommandBeginFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"commandRef", NULL};
  (void) self;
  PyObject *inCommand;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &inCommand)){
    return NULL;
  }
  XPLMCommandBegin(refToPtr(inCommand, commandRefName));
  Py_RETURN_NONE;
}

My_DOCSTR(_commandEnd__doc__, "commandEnd", "commandRef",
          "Ends execution of command specified by commandRef");
static PyObject *XPLMCommandEndFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"commandRef", NULL};
  (void) self;
  PyObject *inCommand;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &inCommand)){
    return NULL;
  }
  XPLMCommandEnd(refToPtr(inCommand, commandRefName));
  Py_RETURN_NONE;
}

My_DOCSTR(_commandOnce__doc__, "commandOnce", "commandRef",
          "Executes given commandRef, doing both CommandBegin and CommandEnd");
static PyObject *XPLMCommandOnceFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"commandRef", NULL};
  (void) self;
  PyObject *inCommand;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &inCommand)){
    return NULL;
  }
  XPLMCommandOnce(refToPtr(inCommand, commandRefName));
  Py_RETURN_NONE;
}

My_DOCSTR(_createCommand__doc__, "createCommand", "name, description=None",
          "Create a named command: You'll still need to registerCommandHandler()");
static PyObject *XPLMCreateCommandFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"name", "description", NULL};
  (void) self;
  const char *inName;
  const char *inDescription = NULL;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "s|s", keywords, &inName, &inDescription)){
    return NULL;
  }
  if (!inDescription) {
    inDescription = inName;
  }
  XPLMCommandRef res = XPLMCreateCommand(inName, inDescription);
  return getPtrRef(res, commandCapsules, commandRefName);
}

My_DOCSTR(_registerCommandHandler__doc__, "registerCommandHandler", "commandRef, callback, before=1, refCon=None",
          "Register a callback for given commandRef\n"
          "\n"
          "command callback is (commandRef, phase, refCon) and should return 0\n"
          "   to halt processing, or 1 to let X-Plane continue with other callbacks.\n"
          "   phase indicates current phase of command execution 0=Begin, 1=Continue, 2=End.\n"
          "before indicates you want to be called prior to X-Plane handling the command.");
static PyObject *XPLMRegisterCommandHandlerFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"commandRef", "callback", "before", "refCon", NULL};
  (void) self;
  PyObject *inCommand;
  PyObject *inHandler;
  int inBefore=1;
  PyObject *inRefcon=Py_None;
  PyObject *pluginSelf;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "OO|iO", keywords, &inCommand, &inHandler, &inBefore, &inRefcon)) {
    return NULL;
  }
  pluginSelf = get_pluginSelf();
  intptr_t refcon = commandCallbackCntr++;
  XPLMRegisterCommandHandler(refToPtr(inCommand, commandRefName), commandCallback, inBefore, (void *)refcon);

  /* we need a form to use as key to Dict(): as all four input values must be used, 
     we'll create a string representing the values, BUT for inrefcon, we want only
     a pointer to the object (the contents of the object may have legitimately changed.)
   */
  PyObject *inrefcon_ptr = PyLong_FromVoidPtr((void *)inRefcon);
  PyObject *bv = Py_BuildValue("(OOiO)", inCommand, inHandler, inBefore, inrefcon_ptr);
  PyObject *key = PyObject_Str(bv);

  PyObject *rc = PyLong_FromVoidPtr((void *)refcon);
  PyDict_SetItem(commandRefcons, key, rc);

  PyObject *argsObj = Py_BuildValue( "(OOOiO)", pluginSelf, inCommand, inHandler, inBefore, inRefcon);
  PyDict_SetItem(commandCallbackDict, rc, argsObj);
  Py_DECREF(argsObj);
  Py_DECREF(rc);
  Py_DECREF(key);
  Py_DECREF(bv);
  Py_DECREF(inrefcon_ptr);
  Py_RETURN_NONE;
}

My_DOCSTR(_unregisterCommandHandler__doc__, "unregisterCommandHandler", "commandRef, callback, before=1, refCon=None",
          "Unregister commandRef. Parameters must match those provided with registerCommandHandler()");
static PyObject *XPLMUnregisterCommandHandlerFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"commandRef", "callback", "before", "refCon", NULL};
  (void) self;
  PyObject *inCommand;
  PyObject *inHandler;
  int inBefore=1;
  PyObject *inRefcon=Py_None;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "OO|iO", keywords, &inCommand, &inHandler, &inBefore, &inRefcon)) {
    return NULL;
  }
  PyObject *inrefcon_ptr = PyLong_FromVoidPtr((void *)inRefcon);
  PyObject *bv = Py_BuildValue("(OOiO)", inCommand, inHandler, inBefore, inrefcon_ptr);
  PyObject *key = PyObject_Str(bv);
  PyObject *refcon = PyDict_GetItem(commandRefcons, key);  /* borrowed ref */

  if (refcon == NULL) {
    pythonLog("unregisterCommandHandler could not find command handler for %s\n", objToStr(key));
    Py_RETURN_NONE;
  }
  XPLMUnregisterCommandHandler(refToPtr(inCommand, commandRefName), commandCallback,
                               inBefore, PyLong_AsVoidPtr(refcon));
  if(PyDict_DelItem(commandRefcons, key)){
    printf("XPLMUnregisterCommandHandler: couldn't remove refcon.\n");
  }
  Py_DECREF(inrefcon_ptr);
  Py_DECREF(bv);
  Py_DECREF(key);
  if(PyDict_DelItem(commandCallbackDict, refcon)){
    printf("XPLMUnregisterCommandHandler: couldn't remove command handler.\n");
  }
  Py_RETURN_NONE;
}

void clearInstanceCommands(PyObject *pluginSelf)
{
  PyObject *key, *value;
  Py_ssize_t pos = 0;
  pythonDebug("%*s Clearing commands for [%s]", 6, " ", objDebug(pluginSelf));
  int count = 0;
  while (PyDict_Next(commandCallbackDict, &pos, &key, &value)) {
    PyObject *commandPlugin = PyTuple_GetItem(value, CALLBACK_PLUGIN);
    if (PyObject_RichCompareBool(commandPlugin, pluginSelf, Py_EQ)) {
      count ++;
      intptr_t refcon = PyLong_AsLong(key);
      pythonDebug("%*s Removing command handler for [%ld] [%s]", 8, " ", refcon, objDebug(PyTuple_GetItem(value, CALLBACK_PLUGIN)));
      XPLMUnregisterCommandHandler(refToPtr(PyTuple_GetItem(value, CALLBACK_CAPSULE), commandRefName),
                                   commandCallback,
                                   PyLong_AsLong(PyTuple_GetItem(value, CALLBACK_BEFORE)),
                                   (void *)refcon);

      PyDict_DelItem(commandCallbackDict, key);
    }
  }
  pythonDebug("%*s Cleared %d commands for [%s]", 8, " ", count, objDebug(pluginSelf));
}

static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  PyDict_Clear(errCallbacks);
  Py_DECREF(errCallbacks);
  PyDict_Clear(commandCallbackDict);
  Py_DECREF(commandCallbackDict);
  PyDict_Clear(commandRefcons);
  Py_DECREF(commandRefcons);
  PyDict_Clear(commandCapsules);
  Py_DECREF(commandCapsules);
  Py_RETURN_NONE;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
static PyMethodDef XPLMUtilitiesMethods[] = {
  {"speakString", (PyCFunction)XPLMSpeakStringFun, METH_VARARGS | METH_KEYWORDS, _speakString__doc__},
  {"XPLMSpeakString", (PyCFunction)XPLMSpeakStringFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"getVirtualKeyDescription", (PyCFunction)XPLMGetVirtualKeyDescriptionFun, METH_VARARGS | METH_KEYWORDS, _getVirtualKeyDescription__doc__},
  {"XPLMGetVirtualKeyDescription", (PyCFunction)XPLMGetVirtualKeyDescriptionFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"reloadScenery", (PyCFunction)XPLMReloadSceneryFun, METH_VARARGS, _reloadScenery__doc__},
  {"XPLMReloadScenery", (PyCFunction)XPLMReloadSceneryFun, METH_VARARGS, ""},
  {"getSystemPath", (PyCFunction)XPLMGetSystemPathFun, METH_VARARGS, _getSystemPath__doc__},
  {"XPLMGetSystemPath", (PyCFunction)XPLMGetSystemPathFun, METH_VARARGS, ""},
  {"getPrefsPath", (PyCFunction)XPLMGetPrefsPathFun, METH_VARARGS, _getPrefsPath__doc__},
  {"XPLMGetPrefsPath", (PyCFunction)XPLMGetPrefsPathFun, METH_VARARGS, ""},
  {"getDirectorySeparator", (PyCFunction)XPLMGetDirectorySeparatorFun, METH_VARARGS, _getDirectorySeparator__doc__},
  {"XPLMGetDirectorySeparator", (PyCFunction)XPLMGetDirectorySeparatorFun, METH_VARARGS, ""},
  {"extractFileAndPath", (PyCFunction)XPLMExtractFileAndPathFun, METH_VARARGS | METH_KEYWORDS, _extractFileAndPath__doc__},
  {"XPLMExtractFileAndPath", (PyCFunction)XPLMExtractFileAndPathFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"getDirectoryContents", (PyCFunction)XPLMGetDirectoryContentsFun, METH_VARARGS | METH_KEYWORDS, _getDirectoryContents__doc__},
  {"XPLMGetDirectoryContents", (PyCFunction)XPLMGetDirectoryContentsFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"getVersions", (PyCFunction)XPLMGetVersionsFun, METH_VARARGS, _getVersions__doc__},
  {"XPLMGetVersions", (PyCFunction)XPLMGetVersionsFun, METH_VARARGS, ""},
  {"getLanguage", (PyCFunction)XPLMGetLanguageFun, METH_VARARGS, _getLanguage__doc__},
  {"XPLMGetLanguage", (PyCFunction)XPLMGetLanguageFun, METH_VARARGS, ""},
  {"debugString", (PyCFunction)XPLMDebugStringFun, METH_VARARGS | METH_KEYWORDS, _debugString__doc__},
  {"XPLMDebugString", (PyCFunction)XPLMDebugStringFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"setErrorCallback", (PyCFunction)XPLMSetErrorCallbackFun, METH_VARARGS | METH_KEYWORDS, _setErrorCallback__doc__},
  {"XPLMSetErrorCallback", (PyCFunction)XPLMSetErrorCallbackFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"findSymbol", (PyCFunction)XPLMFindSymbolFun, METH_VARARGS | METH_KEYWORDS, _findSymbol__doc__},
  {"XPLMFindSymbol", (PyCFunction)XPLMFindSymbolFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"loadDataFile", (PyCFunction)XPLMLoadDataFileFun, METH_VARARGS | METH_KEYWORDS, _loadDataFile__doc__},
  {"XPLMLoadDataFile", (PyCFunction)XPLMLoadDataFileFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"saveDataFile", (PyCFunction)XPLMSaveDataFileFun, METH_VARARGS | METH_KEYWORDS, _saveDataFile__doc__},
  {"XPLMSaveDataFile", (PyCFunction)XPLMSaveDataFileFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"findCommand", (PyCFunction)XPLMFindCommandFun, METH_VARARGS | METH_KEYWORDS, _findCommand__doc__},
  {"XPLMFindCommand", (PyCFunction)XPLMFindCommandFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"commandBegin", (PyCFunction)XPLMCommandBeginFun, METH_VARARGS | METH_KEYWORDS, _commandBegin__doc__},
  {"XPLMCommandBegin", (PyCFunction)XPLMCommandBeginFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"commandEnd", (PyCFunction)XPLMCommandEndFun, METH_VARARGS | METH_KEYWORDS, _commandEnd__doc__},
  {"XPLMCommandEnd", (PyCFunction)XPLMCommandEndFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"commandOnce", (PyCFunction)XPLMCommandOnceFun, METH_VARARGS | METH_KEYWORDS, _commandOnce__doc__},
  {"XPLMCommandOnce", (PyCFunction)XPLMCommandOnceFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"createCommand", (PyCFunction)XPLMCreateCommandFun, METH_VARARGS | METH_KEYWORDS, _createCommand__doc__},
  {"XPLMCreateCommand", (PyCFunction)XPLMCreateCommandFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"registerCommandHandler", (PyCFunction)XPLMRegisterCommandHandlerFun, METH_VARARGS | METH_KEYWORDS, _registerCommandHandler__doc__},
  {"XPLMRegisterCommandHandler", (PyCFunction)XPLMRegisterCommandHandlerFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"unregisterCommandHandler", (PyCFunction)XPLMUnregisterCommandHandlerFun, METH_VARARGS | METH_KEYWORDS, _unregisterCommandHandler__doc__},
  {"XPLMUnregisterCommandHandler", (PyCFunction)XPLMUnregisterCommandHandlerFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"_cleanup", cleanup, METH_VARARGS, ""},
  {NULL, NULL, 0, NULL}
};
#pragma GCC diagnostic pop


static struct PyModuleDef XPLMUtilitiesModule = {
  PyModuleDef_HEAD_INIT,
  "XPLMUtilities",
  "Laminar documentation: \n"
  "   https://developer.x-plane.com/sdk/XPLMUtilities/\n"
  "XPPython3 documentation: \n"
  "   https://xppython3.rtfd.io/en/stable/development/modules/utilities.html",
  -1,
  XPLMUtilitiesMethods,
  NULL,
  NULL,
  NULL,
  NULL
};

PyMODINIT_FUNC
PyInit_XPLMUtilities(void)
{
  if(!(errCallbacks = PyDict_New())){
    return NULL;
  }
  PyDict_SetItemString(xppythonDicts, "errCallbacks", errCallbacks);
  if(!(commandCallbackDict = PyDict_New())){
    return NULL;
  }
  PyDict_SetItemString(xppythonDicts, "commandCallbacks", commandCallbackDict);
  if(!(commandRefcons = PyDict_New())){
    return NULL;
  }
  PyDict_SetItemString(xppythonDicts, "commandRefcons", commandRefcons);
  if(!(commandCapsules = PyDict_New())){
    return NULL;
  }
  PyDict_SetItemString(xppythonCapsules, commandRefName, commandCapsules);

  PyObject *mod = PyModule_Create(&XPLMUtilitiesModule);
  if(mod){
    PyModule_AddStringConstant(mod, "__author__", "Peter Buckner (pbuck@avnwx.com)");
    /*
     * XPLMHostApplicationID
     * 
     * The plug-in system is based on Austin's cross-platform OpenGL framework and 
     * could theoretically be adapted to  run in other apps like WorldMaker.  The 
     * plug-in system also runs against a test harness for internal development 
     * and could be adapted to another flight sim (in theory at least).  So an ID 
     * is providing allowing plug-ins to  indentify what app they are running 
     * under.                                                                      
     *
     */
    PyModule_AddIntConstant(mod, "xplm_Host_Unknown", xplm_Host_Unknown);
    PyModule_AddIntConstant(mod, "xplm_Host_XPlane", xplm_Host_XPlane);
    PyModule_AddIntConstant(mod, "Host_Unknown", xplm_Host_Unknown);
    PyModule_AddIntConstant(mod, "Host_XPlane", xplm_Host_XPlane);
    /*
     * XPLMLanguageCode
     * 
     * These enums define what language the sim is running in.  These enumerations 
     * do not imply that the sim can or does run in all of these languages; they 
     * simply provide a known encoding in the event that a given sim version is 
     * localized to a certain language.                                            
     *
     */
    PyModule_AddIntConstant(mod, "xplm_Language_Unknown", xplm_Language_Unknown);
    PyModule_AddIntConstant(mod, "xplm_Language_English", xplm_Language_English);
    PyModule_AddIntConstant(mod, "xplm_Language_French", xplm_Language_French);
    PyModule_AddIntConstant(mod, "xplm_Language_German", xplm_Language_German);
    PyModule_AddIntConstant(mod, "xplm_Language_Italian", xplm_Language_Italian);
    PyModule_AddIntConstant(mod, "xplm_Language_Spanish", xplm_Language_Spanish);
    PyModule_AddIntConstant(mod, "xplm_Language_Korean", xplm_Language_Korean);
    PyModule_AddIntConstant(mod, "xplm_Language_Russian", xplm_Language_Russian);
    PyModule_AddIntConstant(mod, "xplm_Language_Greek", xplm_Language_Greek);
    PyModule_AddIntConstant(mod, "xplm_Language_Japanese", xplm_Language_Japanese);
    PyModule_AddIntConstant(mod, "Language_Unknown", xplm_Language_Unknown);
    PyModule_AddIntConstant(mod, "Language_English", xplm_Language_English);
    PyModule_AddIntConstant(mod, "Language_French", xplm_Language_French);
    PyModule_AddIntConstant(mod, "Language_German", xplm_Language_German);
    PyModule_AddIntConstant(mod, "Language_Italian", xplm_Language_Italian);
    PyModule_AddIntConstant(mod, "Language_Spanish", xplm_Language_Spanish);
    PyModule_AddIntConstant(mod, "Language_Korean", xplm_Language_Korean);
    PyModule_AddIntConstant(mod, "Language_Russian", xplm_Language_Russian);
    PyModule_AddIntConstant(mod, "Language_Greek", xplm_Language_Greek);
    PyModule_AddIntConstant(mod, "Language_Japanese", xplm_Language_Japanese);
    PyModule_AddIntConstant(mod, "xplm_Language_Chinese", xplm_Language_Chinese);
    PyModule_AddIntConstant(mod, "Language_Chinese", xplm_Language_Chinese);

    /*
     * XPLMDataFileType
     * 
     * These enums define types of data files you can load or unload using the 
     * SDK.                                                                        
     *
     */
    PyModule_AddIntConstant(mod, "xplm_DataFile_Situation", xplm_DataFile_Situation);
    PyModule_AddIntConstant(mod, "xplm_DataFile_ReplayMovie", xplm_DataFile_ReplayMovie);

    PyModule_AddIntConstant(mod, "DataFile_Situation", xplm_DataFile_Situation);
    PyModule_AddIntConstant(mod, "DataFile_ReplayMovie", xplm_DataFile_ReplayMovie);
    
    /*
     * XPLMCommandPhase
     * 
     * The phases of a command.                                                    
     *
     */
    PyModule_AddIntConstant(mod, "xplm_CommandBegin", xplm_CommandBegin);
    PyModule_AddIntConstant(mod, "xplm_CommandContinue", xplm_CommandContinue);
    PyModule_AddIntConstant(mod, "xplm_CommandEnd", xplm_CommandEnd);

    PyModule_AddIntConstant(mod, "CommandBegin", xplm_CommandBegin);
    PyModule_AddIntConstant(mod, "CommandContinue", xplm_CommandContinue);
    PyModule_AddIntConstant(mod, "CommandEnd", xplm_CommandEnd);

  }

  return mod;
}


