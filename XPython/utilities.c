#define _GNU_SOURCE 1
#include <Python.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdbool.h>
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMUtilities.h>
#include "utils.h"

PyObject *errCallbacks;
PyObject *commandCallbacks;
PyObject *commandRefcons;
PyObject *commandCapsules;
intptr_t commandCallbackCntr;

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
      PyErr_Print();
    }else{
      Py_DECREF(oRes);
    }
  }
  Py_DECREF(msg);
}


#if defined(XPLM_DEPRECATED)
static PyObject *XPLMSimulateKeyPressFun(PyObject *self, PyObject *args)
{
  (void) self;
  int inKeyType, inKey;
  if(!PyArg_ParseTuple(args, "ii", &inKeyType, &inKey)){
    return NULL;
  }
  XPLMSimulateKeyPress(inKeyType, inKey);
  Py_RETURN_NONE;
}
#endif



static PyObject *XPLMSpeakStringFun(PyObject *self, PyObject *args)
{
  (void) self;
  const char *inString;
  if(!PyArg_ParseTuple(args, "s", &inString)){
    return NULL;
  }
  XPLMSpeakString(inString);
  Py_RETURN_NONE;
}

#if defined(XPLM_DEPRECATED)
static PyObject *XPLMCommandKeyStrokeFun(PyObject *self, PyObject *args)
{
  (void) self;
  int inKey;
  if(!PyArg_ParseTuple(args, "i", &inKey)){
    return NULL;
  }
  XPLMCommandKeyStroke(inKey);
  Py_RETURN_NONE;
}
#endif

#if defined(XPLM_DEPRECATED)
static PyObject *XPLMCommandButtonPressFun(PyObject *self, PyObject *args)
{
  (void) self;
  int inButton;
  if(!PyArg_ParseTuple(args, "i", &inButton)){
    return NULL;
  }
  XPLMCommandButtonPress(inButton);
  Py_RETURN_NONE;
}
#endif

#if defined(XPLM_DEPRECATED)
static PyObject *XPLMCommandButtonReleaseFun(PyObject *self, PyObject *args)
{
  (void) self;
  int inButton;
  if(!PyArg_ParseTuple(args, "i", &inButton)){
    return NULL;
  }
  XPLMCommandButtonRelease(inButton);
  Py_RETURN_NONE;
}
#endif

static PyObject *XPLMGetVirtualKeyDescriptionFun(PyObject *self, PyObject *args)
{
  (void) self;
  int inVirtualKey;
  if(!PyArg_ParseTuple(args, "i", &inVirtualKey)){
    return NULL;
  }
  const char *res = XPLMGetVirtualKeyDescription(inVirtualKey);
  return PyUnicode_DecodeUTF8(res, strlen(res), NULL);
}

static PyObject *XPLMReloadSceneryFun(PyObject *self, PyObject *args)
{
  (void)self;
  (void)args;
  XPLMReloadScenery();
  Py_RETURN_NONE;
}

static PyObject *XPLMGetSystemPathFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  char outSystemPath[1024];
  XPLMGetSystemPath(outSystemPath);

  return PyUnicode_DecodeUTF8(outSystemPath, strlen(outSystemPath), NULL);
}

static PyObject *XPLMGetPrefsPathFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  char outPrefsPath[1024];
  XPLMGetPrefsPath(outPrefsPath);

  return PyUnicode_DecodeUTF8(outPrefsPath, strlen(outPrefsPath), NULL);
}

static PyObject *XPLMGetDirectorySeparatorFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  
  const char *res = XPLMGetDirectorySeparator();

  return PyUnicode_DecodeUTF8(res, 1, NULL);
}


static PyObject *XPLMExtractFileAndPathFun(PyObject *self, PyObject *args)
{
  (void) self;
  const char *inFullPathConst;
  if(!PyArg_ParseTuple(args, "s", &inFullPathConst)){
    return NULL;
  }
  char *inFullPath = strdup(inFullPathConst);
  const char *res = XPLMExtractFileAndPath(inFullPath);
  
  PyObject *resObj = Py_BuildValue("(ss)", res, inFullPath);
  free(inFullPath);
  return resObj;
}

static PyObject *XPLMGetDirectoryContentsFun(PyObject *self, PyObject *args)
{
  (void) self;
  const char *inDirectoryPath;
  int inFirstReturn;
  int inFileNameBufSize;
  int inIndexCount;
  if(!PyArg_ParseTuple(args, "siii", &inDirectoryPath, &inFirstReturn, &inFileNameBufSize, &inIndexCount)){
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

#if defined(XPLM_DEPRECATED)
static PyObject *XPLMInitializedFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  
  int res = XPLMInitialized();

  return PyLong_FromLong(res);
}
#endif

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

static PyObject *XPLMGetLanguageFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  
  XPLMLanguageCode res = XPLMGetLanguage();

  return PyLong_FromLong(res);
}

static PyObject *XPLMDebugStringFun(PyObject *self, PyObject *args)
{
  (void) self;
  const char *inString;
  if(!PyArg_ParseTuple(args, "s", &inString)){
    return NULL;
  }
  XPLMDebugString(inString);
  Py_RETURN_NONE;
}


//Assumption:
// Single error reporting callback per PI_* plugin!
static PyObject *XPLMSetErrorCallbackFun(PyObject *self, PyObject *args)
{
  (void) self;

  if(PyDict_Size(errCallbacks) == 0){
    XPLMSetErrorCallback(error_callback);
  }

  PyObject *pluginSelf;
  PyObject *callback;
  if(!PyArg_ParseTuple(args, "OO", &pluginSelf, &callback)) {
    PyErr_Clear();
    if(!PyArg_ParseTuple(args, "O", &callback)) {
      return NULL;
    }
  } else {
    pythonLogWarning("'self' deprecated as first parameter of XPLMSetErrorCallback");
  }
  pluginSelf = get_pluginSelf();
  PyDict_SetItem(errCallbacks, pluginSelf, callback);
  Py_DECREF(pluginSelf);
  
  Py_RETURN_NONE;
}

static PyObject *XPLMFindSymbolFun(PyObject *self, PyObject *args)
{
  (void) self;
  
  const char *inString;
  if(!PyArg_ParseTuple(args, "s", &inString)){
    return NULL;
  }
  return PyLong_FromVoidPtr(XPLMFindSymbol(inString));
}

static PyObject *XPLMLoadDataFileFun(PyObject *self, PyObject *args)
{
  (void) self;
  int inFileType;
  const char *inFilePath;
  if(!PyArg_ParseTuple(args, "is", &inFileType, &inFilePath)){
    return NULL;
  }
  int res = XPLMLoadDataFile(inFileType, inFilePath);
  return PyLong_FromLong(res);
}

static PyObject *XPLMSaveDataFileFun(PyObject *self, PyObject *args)
{
  (void) self;
  int inFileType;
  const char *inFilePath;
  if(!PyArg_ParseTuple(args, "is", &inFileType, &inFilePath)){
    return NULL;
  }
  int res = XPLMSaveDataFile(inFileType, inFilePath);
  return PyLong_FromLong(res);
}

static int commandCallback(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void *inRefcon)
{
  PyObject *pID = PyLong_FromVoidPtr(inRefcon);
  PyObject *pCbk = PyDict_GetItem(commandCallbacks, pID);
  Py_DECREF(pID);
  if(pCbk == NULL){
    printf("Received unknown commandCallback refCon (%p).\n", inRefcon);
    return -1;
  }
  //0 - self, 1 - callback, 2 - refcon
  PyObject *arg1 = getPtrRef(inCommand, commandCapsules, commandRefName);
  PyObject *arg2 = PyLong_FromLong(inPhase);
  PyObject *oRes = PyObject_CallFunctionObjArgs(PyTuple_GetItem(pCbk, 2), arg1, arg2, PyTuple_GetItem(pCbk, 4), NULL);
  Py_DECREF(arg1);
  Py_DECREF(arg2);
  PyObject *err = PyErr_Occurred();
  char msg[1024];
  if(err){
    fprintf(pythonLogFile, "Error in CommandCallback [%s] %s\n",
            objToStr(PyTuple_GetItem(pCbk, 0)),
            objToStr(PyTuple_GetItem(pCbk, 2)));
    /* pass error back up */
    /* 
       PyErr_SetString(err, msg);
       PyErr_Print();
    */
    return 0;
  }
  int res = PyLong_AsLong(oRes);
  err = PyErr_Occurred();
  if(err){
    sprintf(msg, "Expected integer for return from CommandCallback [%s] %s",
            objToStr(PyTuple_GetItem(pCbk, 0)),
            objToStr(PyTuple_GetItem(pCbk, 2)));
    PyErr_SetString(err, msg);
    PyErr_Print();
  }
  Py_DECREF(oRes);
  return res;
}

static PyObject *XPLMFindCommandFun(PyObject *self, PyObject *args)
{
  (void) self;
  const char *inName;
  if(!PyArg_ParseTuple(args, "s", &inName)){
    return NULL;
  }
  XPLMCommandRef res = XPLMFindCommand(inName);
  return getPtrRef(res, commandCapsules, commandRefName);
}

static PyObject *XPLMCommandBeginFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *inCommand;
  if(!PyArg_ParseTuple(args, "O", &inCommand)){
    return NULL;
  }
  XPLMCommandBegin(refToPtr(inCommand, commandRefName));
  Py_RETURN_NONE;
}

static PyObject *XPLMCommandEndFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *inCommand;
  if(!PyArg_ParseTuple(args, "O", &inCommand)){
    return NULL;
  }
  XPLMCommandEnd(refToPtr(inCommand, commandRefName));
  Py_RETURN_NONE;
}

static PyObject *XPLMCommandOnceFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *inCommand;
  if(!PyArg_ParseTuple(args, "O", &inCommand)){
    return NULL;
  }
  XPLMCommandOnce(refToPtr(inCommand, commandRefName));
  Py_RETURN_NONE;
}

static PyObject *XPLMCreateCommandFun(PyObject *self, PyObject *args)
{
  (void) self;
  const char *inName;
  const char *inDescription;
  if(!PyArg_ParseTuple(args, "ss", &inName, &inDescription)){
    return NULL;
  }
  XPLMCommandRef res = XPLMCreateCommand(inName, inDescription);
  return getPtrRef(res, commandCapsules, commandRefName);
}

static PyObject *XPLMRegisterCommandHandlerFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *inCommand;
  PyObject *inHandler;
  int inBefore;
  PyObject *inRefcon;
  PyObject *pluginSelf;
  if(!PyArg_ParseTuple(args, "OOOiO", &pluginSelf, &inCommand, &inHandler, &inBefore, &inRefcon)) {
    PyErr_Clear();
    if(!PyArg_ParseTuple(args, "OOiO", &inCommand, &inHandler, &inBefore, &inRefcon)) {
      return NULL;
    }
  } else {
    pythonLogWarning("'self' deprecated as first parameter of XPLMRegisterCommandHandler");
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
  PyDict_SetItem(commandCallbacks, rc, argsObj);
  Py_DECREF(argsObj);
  Py_DECREF(rc);
  Py_DECREF(key);
  Py_DECREF(bv);
  Py_DECREF(inrefcon_ptr);
  Py_RETURN_NONE;
}

static PyObject *XPLMUnregisterCommandHandlerFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *inCommand;
  PyObject *inHandler;
  int inBefore;
  PyObject *inRefcon;
  PyObject *pluginSelf;
  if(!PyArg_ParseTuple(args, "OOOiO", &pluginSelf, &inCommand, &inHandler, &inBefore, &inRefcon)) {
    PyErr_Clear();
    if(!PyArg_ParseTuple(args, "OOiO", &inCommand, &inHandler, &inBefore, &inRefcon)) {
      return NULL;
    }
  } else {
    pythonLogWarning("'self' deprecated as first parameter of XPLMUnregisterCommandHandler");
  }
  PyObject *inrefcon_ptr = PyLong_FromVoidPtr((void *)inRefcon);
  PyObject *bv = Py_BuildValue("(OOiO)", inCommand, inHandler, inBefore, inrefcon_ptr);
  PyObject *key = PyObject_Str(bv);
  PyObject *refcon = PyDict_GetItem(commandRefcons, key);  /* borrowed ref */
  XPLMUnregisterCommandHandler(refToPtr(inCommand, commandRefName), commandCallback,
                               inBefore, PyLong_AsVoidPtr(refcon));
  if(PyDict_DelItem(commandRefcons, key)){
    printf("XPLMUnregisterCommandHandler: couldn't remove refcon.\n");
  }
  Py_DECREF(inrefcon_ptr);
  Py_DECREF(bv);
  Py_DECREF(key);
  if(PyDict_DelItem(commandCallbacks, refcon)){
    printf("XPLMUnregisterCommandHandler: couldn't remove command handler.\n");
  }
  Py_RETURN_NONE;
}

static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  PyDict_Clear(errCallbacks);
  Py_DECREF(errCallbacks);
  PyDict_Clear(commandCallbacks);
  Py_DECREF(commandCallbacks);
  PyDict_Clear(commandRefcons);
  Py_DECREF(commandRefcons);
  PyDict_Clear(commandCapsules);
  Py_DECREF(commandCapsules);
  Py_RETURN_NONE;
}

static PyMethodDef XPLMUtilitiesMethods[] = {
#if defined(XPLM_DEPRECATED)
  {"XPLMSimulateKeyPress", XPLMSimulateKeyPressFun, METH_VARARGS, "DEPRECATED"},
#endif
  {"XPLMSpeakString", XPLMSpeakStringFun, METH_VARARGS, ""},
#if defined(XPLM_DEPRECATED)
  {"XPLMCommandKeyStroke", XPLMCommandKeyStrokeFun, METH_VARARGS, "DEPRECATED"},
  {"XPLMCommandButtonPress", XPLMCommandButtonPressFun, METH_VARARGS, "DEPRECATED"},
  {"XPLMCommandButtonRelease", XPLMCommandButtonReleaseFun, METH_VARARGS, "DEPRECATED"},
#endif
  {"XPLMGetVirtualKeyDescription", XPLMGetVirtualKeyDescriptionFun, METH_VARARGS, ""},
  {"XPLMReloadScenery", XPLMReloadSceneryFun, METH_VARARGS, ""},
  {"XPLMGetSystemPath", XPLMGetSystemPathFun, METH_VARARGS, ""},
  {"XPLMGetPrefsPath", XPLMGetPrefsPathFun, METH_VARARGS, ""},
  {"XPLMGetDirectorySeparator", XPLMGetDirectorySeparatorFun, METH_VARARGS, ""},
  {"XPLMExtractFileAndPath", XPLMExtractFileAndPathFun, METH_VARARGS, ""},
  {"XPLMGetDirectoryContents", XPLMGetDirectoryContentsFun, METH_VARARGS, ""},
#if defined(XPLM_DEPRECATED)
  {"XPLMInitialized", XPLMInitializedFun, METH_VARARGS, "DEPRECATED"},
#endif
  {"XPLMGetVersions", XPLMGetVersionsFun, METH_VARARGS, ""},
  {"XPLMGetLanguage", XPLMGetLanguageFun, METH_VARARGS, ""},
  {"XPLMDebugString", XPLMDebugStringFun, METH_VARARGS, ""},
  {"XPLMSetErrorCallback", XPLMSetErrorCallbackFun, METH_VARARGS, ""},
  {"XPLMFindSymbol", XPLMFindSymbolFun, METH_VARARGS, ""},
  {"XPLMLoadDataFile", XPLMLoadDataFileFun, METH_VARARGS, ""},
  {"XPLMSaveDataFile", XPLMSaveDataFileFun, METH_VARARGS, ""},
  {"XPLMFindCommand", XPLMFindCommandFun, METH_VARARGS, ""},
  {"XPLMCommandBegin", XPLMCommandBeginFun, METH_VARARGS, ""},
  {"XPLMCommandEnd", XPLMCommandEndFun, METH_VARARGS, ""},
  {"XPLMCommandOnce", XPLMCommandOnceFun, METH_VARARGS, ""},
  {"XPLMCreateCommand", XPLMCreateCommandFun, METH_VARARGS, ""},
  {"XPLMRegisterCommandHandler", XPLMRegisterCommandHandlerFun, METH_VARARGS, ""},
  {"XPLMUnregisterCommandHandler", XPLMUnregisterCommandHandlerFun, METH_VARARGS, ""},
  {"cleanup", cleanup, METH_VARARGS, ""},
  {NULL, NULL, 0, NULL}
};

static struct PyModuleDef XPLMUtilitiesModule = {
  PyModuleDef_HEAD_INIT,
  "XPLMUtilities",
  NULL,
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
  if(!(commandCallbacks = PyDict_New())){
    return NULL;
  }
  PyDict_SetItemString(xppythonDicts, "commandCallbacks", commandCallbacks);
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
#if defined(XPLM300)
    PyModule_AddIntConstant(mod, "xplm_Language_Chinese", xplm_Language_Chinese);
#endif

    /*
     * XPLMDataFileType
     * 
     * These enums define types of data files you can load or unload using the 
     * SDK.                                                                        
     *
     */
    PyModule_AddIntConstant(mod, "xplm_DataFile_Situation", xplm_DataFile_Situation);
    PyModule_AddIntConstant(mod, "xplm_DataFile_ReplayMovie", xplm_DataFile_ReplayMovie);

    
    /*
     * XPLMCommandPhase
     * 
     * The phases of a command.                                                    
     *
     */
    PyModule_AddIntConstant(mod, "xplm_CommandBegin", xplm_CommandBegin);
    PyModule_AddIntConstant(mod, "xplm_CommandContinue", xplm_CommandContinue);
    PyModule_AddIntConstant(mod, "xplm_CommandEnd", xplm_CommandEnd);

  }

  return mod;
}


