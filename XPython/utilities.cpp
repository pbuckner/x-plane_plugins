#define _GNU_SOURCE 1
#include <Python.h>
#include <sys/time.h>
#include <stdio.h>
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMUtilities.h>
#include <unordered_map>
#include <string>
#include <vector>
#include "utils.h"
#include "utilities.h"
#include "capsules.h"

struct ErrorCallbackInfo {
    PyObject* callback;
    const char* module_name;  // Points into moduleNamePool (interned string)
};

struct CommandCallbackInfo {
    XPLMCommandRef command;
    PyObject* callback;
    int before;
    PyObject* refcon;
    const char* module_name;  // Points into moduleNamePool (interned string)
    intptr_t refcon_id;
};

static std::unordered_map<std::string, ErrorCallbackInfo> errorCallbacks;

static std::unordered_map<intptr_t, CommandCallbackInfo> commandCallbacks;
static intptr_t commandCallbackCounter = 0;

void resetCommands(void) {
  /* commands are reset by clearInstanceCommands */
  for (auto& pair: commandCallbacks) {
    Py_DECREF(pair.second.callback);
    Py_DECREF(pair.second.refcon);
  }
  commandCallbacks.clear();
}

std::string MostRecentCallbackMessageXPPython3;

static void error_callback(const char *inMessage)
{
  //TODO: send the error only to the active plugin?
  // for now, pass to all registered


  pythonLog("Error callback called with message %s", inMessage);
  MostRecentCallbackMessageXPPython3 = inMessage;


  PyObject *msg = PyUnicode_DecodeUTF8(inMessage, strlen(inMessage), nullptr);
  
  for (const auto& pair : errorCallbacks) {
    const ErrorCallbackInfo& info = pair.second;
    set_moduleName(info.module_name);
    PyObject *args[] = {msg};
    PyObject *oRes = PyObject_Vectorcall(info.callback, args, 1, nullptr);
    PyObject *err = PyErr_Occurred();
    if(err){
      pythonLogException();
    }else{
      Py_DECREF(oRes);
    }
  }
  Py_DECREF(msg);
}


My_DOCSTR(_speakString__doc__, "speakString",
          "string",
          "string:str",
          "None",
          "Display string in translucent overlay and speak string");
static PyObject *XPLMSpeakStringFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("string"), nullptr};

  (void) self;
  const char *inString;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "s", keywords, &inString)){
    return nullptr;
  }
  XPLMSpeakString(inString);
  Py_RETURN_NONE;
}

My_DOCSTR(_getVirtualKeyDescription__doc__, "getVirtualKeyDescription",
          "vKey",
          "vKey:int",
          "str",
          "Return human-readable string describing virtual key");
static PyObject *XPLMGetVirtualKeyDescriptionFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  (void) self;
  int inVirtualKey;
  static char *keywords[] = {CHAR("vKey"), nullptr};
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "i", keywords, &inVirtualKey)){
    return nullptr;
  }
  const char *res = XPLMGetVirtualKeyDescription(inVirtualKey);
  return PyUnicode_DecodeUTF8(res, strlen(res), nullptr);
}

My_DOCSTR(_reloadScenery__doc__, "reloadScenery",
          "",
          "",
          "None",
          "Reload current set of scenery");
static PyObject *XPLMReloadSceneryFun(PyObject *self, PyObject *args)
{
  (void)self;
  (void)args;
  XPLMReloadScenery();
  Py_RETURN_NONE;
}

My_DOCSTR(_getSystemPath__doc__, "getSystemPath",
          "",
          "",
          "str",
          "Return full path to X-Plane folder, with trailing '/'");
static PyObject *XPLMGetSystemPathFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  char outSystemPath[1024];
  XPLMGetSystemPath(outSystemPath);

  return PyUnicode_DecodeUTF8(outSystemPath, strlen(outSystemPath), nullptr);
}

My_DOCSTR(_getPrefsPath__doc__, "getPrefsPath",
          "",
          "",
          "str",
          "Get path the *file* within X-Plane's preferences directory.");
static PyObject *XPLMGetPrefsPathFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  char outPrefsPath[1024];
  XPLMGetPrefsPath(outPrefsPath);

  return PyUnicode_DecodeUTF8(outPrefsPath, strlen(outPrefsPath), nullptr);
}

My_DOCSTR(_getDirectorySeparator__doc__, "getDirectorySeparator",
          "",
          "",
          "str",
          "Get string used for directory separator for the current platform.\n"
          "\n"
          "Don't use this, use python os.path.join() related routines.");
static PyObject *XPLMGetDirectorySeparatorFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  
  const char *res = XPLMGetDirectorySeparator();

  return PyUnicode_DecodeUTF8(res, 1, nullptr);
}


My_DOCSTR(_extractFileAndPath__doc__, "extractFileAndPath",
          "fullPath",
          "fullPath:str",
          "tuple[str, str]",
          "Given a full path, separate path from file\n"
          "\n"
          "Don't use this, use os.path routines instead");
static PyObject *XPLMExtractFileAndPathFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("fullPath"), nullptr};
  (void) self;
  const char *inFullPathConst;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "s", keywords, &inFullPathConst)){
    return nullptr;
  }
  char *inFullPath = strdup(inFullPathConst);
  const char *res = XPLMExtractFileAndPath(inFullPath);
  
  PyObject *resObj = Py_BuildValue("(ss)", res, inFullPath);
  free(inFullPath);
  return resObj;
}

My_DOCSTR(_getDirectoryContents__doc__, "getDirectoryContents",
          "dir, firstReturn=0, bufSize=2048, maxFiles=100",
          "dir:str, firstReturn:int=0, bufSize:int=2048, maxFiles:int=100",
          "tuple[int, list[str], int]",
          "Get contents (files and subdirectories) of directory\n"
          "\n"
          "Don't use this, use python os.walk() or glob.glob() instead.");
static PyObject *XPLMGetDirectoryContentsFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("dir"), CHAR("firstReturn"), CHAR("bufSize"), CHAR("maxFiles"), nullptr};
  (void) self;
  const char *inDirectoryPath;
  int inFirstReturn=0;
  int inFileNameBufSize=2048;
  int inIndexCount=100;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "s|iii", keywords, &inDirectoryPath, &inFirstReturn, &inFileNameBufSize, &inIndexCount)){
    return nullptr;
  }
  char *outFileNames = static_cast<char*>(malloc(inFileNameBufSize));
  char **outIndices = static_cast<char**>(malloc(inIndexCount * sizeof(char *)));
  int outTotalFiles;
  int outReturnedFiles;

  int res = XPLMGetDirectoryContents(inDirectoryPath, inFirstReturn, outFileNames, inFileNameBufSize,
                                     outIndices, inIndexCount, &outTotalFiles, &outReturnedFiles);
  
  PyObject *namesList = PyList_New(0);
  PyObject *tmp;
  for(int i = 0; i < outReturnedFiles; ++i){
    if(outIndices[i] != nullptr){
      tmp = PyUnicode_DecodeUTF8(outIndices[i], strlen(outIndices[i]), nullptr);
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

My_DOCSTR(_getVersions__doc__, "getVersions",
          "",
          "",
          "tuple[int, int, int]",
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

My_DOCSTR(_getLanguage__doc__, "getLanguage",
          "",
          "",
          "int",
          "Return language code the sim is running in.");
static PyObject *XPLMGetLanguageFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  
  XPLMLanguageCode res = XPLMGetLanguage();

  return PyLong_FromLong(res);
}

My_DOCSTR(_debugString__doc__, "debugString",
          "string",
          "string:str",
          "None",
          "Write string to 'Log.txt' file, with immediate buffer flush\n"
          "\n"
          "Use xp.systemLog() instead, to add newline and prefix with your\n"
          "plugin's name instead. Use xp.log() to write to XPPython3Log.txt file");
static PyObject *XPLMDebugStringFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("string"), nullptr};
  (void) self;
  const char *inString;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "s", keywords, &inString)){
    return nullptr;
  }
  XPLMDebugString(inString);
  Py_RETURN_NONE;
}


//Assumption:
// Single error reporting callback per PI_* plugin!
My_DOCSTR(_setErrorCallback__doc__, "setErrorCallback",
          "callback",
          "callback:Callable[[str], None]",
          "None",
          "Install error-reporting callback for your plugin\n"
          "\n"
          "Likely not useful for python debugging.");
static PyObject *XPLMSetErrorCallbackFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("callback"), nullptr};
  (void) self;

  if(errorCallbacks.empty()){
    XPLMSetErrorCallback(error_callback);
  }

  PyObject *callback;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &callback)) {
    return nullptr;
  }

  // Remove old callback if exists (using CurrentPythonModuleName which is already interned)
  auto it = errorCallbacks.find(CurrentPythonModuleName);
  if (it != errorCallbacks.end()) {
    Py_DECREF(it->second.callback);
  }

  // Add new callback
  Py_INCREF(callback);
  errorCallbacks[CurrentPythonModuleName] = {
    .callback = callback,
    .module_name = CurrentPythonModuleName
  };
  
  Py_RETURN_NONE;
}

My_DOCSTR(_findSymbol__doc__, "findSymbol",
          "symbol",
          "symbol:str",
          "int",
          "Find C-API symbol. See documentation.");
static PyObject *XPLMFindSymbolFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("symbol"), nullptr};
  (void) self;

  const char *inString;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "s", keywords, &inString)){
    return nullptr;
  }
  return PyLong_FromVoidPtr(XPLMFindSymbol(inString));
}

My_DOCSTR(_loadDataFile__doc__, "loadDataFile",
          "fileType, path",
          "fileType:XPLMDataFileType, path:str",
          "int",
          "Load data file given by path\n"
          "\n"
          "fileType is:\n"
          "  DataFile_Situation   = 1\n"
          "  DataFile_ReplayMovie = 2\n"
          "Path is either absolute or relative X-Plane root.\n"
          "Returns 1 on success (file found), 0 otherwise.");
static PyObject *XPLMLoadDataFileFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("fileType"), CHAR("path"), nullptr};
  (void) self;
  int inFileType;
  const char *inFilePath;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "is", keywords, &inFileType, &inFilePath)){
    return nullptr;
  }
  int res = XPLMLoadDataFile(inFileType, inFilePath);
  return PyLong_FromLong(res);
}

My_DOCSTR(_saveDataFile__doc__, "saveDataFile",
          "fileType, path",
          "fileType:XPLMDataFileType, path:str",
          "int",
          "Saves data file to disk.\n"
          "\n"
          "fileType is:\n"
          "  DataFile_Situation   = 1\n"
          "  DataFile_ReplayMovie = 2\n"
          "Path is either absolute or relative X-Plane root.\n"
          "Returns 1 on success (file found), 0 otherwise.");
static PyObject *XPLMSaveDataFileFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("fileType"), CHAR("path"), nullptr};
  (void) self;
  int inFileType;
  const char *inFilePath;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "is", keywords, &inFileType, &inFilePath)){
    return nullptr;
  }
  int res = XPLMSaveDataFile(inFileType, inFilePath);
  return PyLong_FromLong(res);
}

static int genericCommandCallback(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void *inRefcon)
{
  intptr_t refcon_id = (intptr_t)inRefcon;
  auto it = commandCallbacks.find(refcon_id);
  if (it == commandCallbacks.end()) {
    printf("Received unknown commandCallback refCon (%p).\n", inRefcon);
    return -1;
  }
  
  CommandCallbackInfo& info = it->second;
  set_moduleName(info.module_name);

  PyObject *arg1 = makeCapsule(inCommand, "XPLMCommandRef");
  PyObject *arg2 = PyLong_FromLong(inPhase);
  PyObject *args[] = {arg1, arg2, info.refcon};
    PyObject *oRes = PyObject_Vectorcall(info.callback, args, 3, nullptr);
  Py_DECREF(arg1);
  Py_DECREF(arg2);
  PyObject *err = PyErr_Occurred();
  if(err){
    char *s2 = objToStr(info.callback);
    pythonLog("Error in CommandCallback [%s] %s", info.module_name, s2);
    free(s2);
    return 0;
  }
  if (!(oRes && PyLong_Check(oRes))) {
    char *s2 = objToStr(info.callback);
    char *s3 = objToStr(oRes);
    pythonLog("[%s] %s CommandCallback returned '%s' rather than an integer.", info.module_name, s2, s3);
    free(s2);
    free(s3);
    return 1;
  }
  int res = PyLong_AsLong(oRes);
  Py_DECREF(oRes);
  return res;
}

My_DOCSTR(_findCommand__doc__, "findCommand",
          "name",
          "name:str",
          "XPLMCommandRef",
          "Return commandRef for named command or None");
static PyObject *XPLMFindCommandFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("name"), nullptr};
  (void) self;
  const char *inName;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "s", keywords, &inName)){
    return nullptr;
  }
  XPLMCommandRef res = XPLMFindCommand(inName);
  return makeCapsule(res, "XPLMCommandRef");
}

My_DOCSTR(_commandBegin__doc__, "commandBegin",
          "commandRef",
          "commandRef:XPLMCommandRef",
          "None",
          "Start execution of command specified by commandRef");
static PyObject *XPLMCommandBeginFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("commandRef"), nullptr};
  (void) self;
  PyObject *inCommand;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &inCommand)){
    return nullptr;
  }
  XPLMCommandBegin(getVoidPtr(inCommand, "XPLMCommandRef"));
  Py_RETURN_NONE;
}

My_DOCSTR(_commandEnd__doc__, "commandEnd",
          "commandRef",
          "commandRef:XPLMCommandRef",
          "None",
          "Ends execution of command specified by commandRef");
static PyObject *XPLMCommandEndFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("commandRef"), nullptr};
  (void) self;
  PyObject *inCommand;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &inCommand)){
    return nullptr;
  }
  XPLMCommandEnd(getVoidPtr(inCommand, "XPLMCommandRef"));
  Py_RETURN_NONE;
}

My_DOCSTR(_commandOnce__doc__, "commandOnce",
          "commandRef",
          "commandRef:XPLMCommandRef",
          "None",
          "Executes given commandRef, doing both CommandBegin and CommandEnd");
static PyObject *XPLMCommandOnceFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("commandRef"), nullptr};
  (void) self;
  PyObject *inCommand;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &inCommand)){
    return nullptr;
  }
  XPLMCommandOnce(getVoidPtr(inCommand, "XPLMCommandRef"));
  Py_RETURN_NONE;
}

My_DOCSTR(_createCommand__doc__, "createCommand",
          "name, description=None",
          "name:str, description:Optional[str]=None",
          "XPLMCommandRef",
          "Create a named command: You'll still need to registerCommandHandler()");
static PyObject *XPLMCreateCommandFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("name"), CHAR("description"), nullptr};
  (void) self;
  const char *inName;
  const char *inDescription = nullptr;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "s|s", keywords, &inName, &inDescription)){
    return nullptr;
  }
  if (!inDescription) {
    inDescription = inName;
  }
  XPLMCommandRef res = XPLMCreateCommand(inName, inDescription);
  return makeCapsule(res, "XPLMCommandRef");
}

My_DOCSTR(_registerCommandHandler__doc__, "registerCommandHandler",
          "commandRef, callback, before=1, refCon=None",
          "commandRef:XPLMCommandRef, callback:Callable[[XPLMCommandRef, XPLMCommandPhase, Any], int], before:int=1, refCon:Any=None",
          "None",
          "Register a callback for given commandRef\n"
          "\n"
          "command callback is (commandRef, phase, refCon) and should return 0\n"
          "   to halt processing, or 1 to let X-Plane continue with other callbacks.\n"
          "   phase indicates current phase of command execution 0=Begin, 1=Continue, 2=End.\n"
          "before indicates you want to be called prior to X-Plane handling the command.");
static PyObject *XPLMRegisterCommandHandlerFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("commandRef"), CHAR("callback"), CHAR("before"), CHAR("refCon"), nullptr};
  (void) self;
  PyObject *inCommand;
  PyObject *inHandler;
  int inBefore=1;
  PyObject *inRefcon=Py_None;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "OO|iO", keywords, &inCommand, &inHandler, &inBefore, &inRefcon)) {
    return nullptr;
  }
  intptr_t refcon = commandCallbackCounter++;
  XPLMCommandRef commandRef = getVoidPtr(inCommand, "XPLMCommandRef");
  XPLMRegisterCommandHandler(commandRef, genericCommandCallback, inBefore, (void *)refcon);
  commandCallbacks[refcon] = {
    .command = commandRef,
    .callback = inHandler,
    .before = inBefore,
    .refcon = inRefcon,
    .module_name = CurrentPythonModuleName,
    .refcon_id = refcon
  };
  Py_INCREF(inHandler);
  Py_INCREF(inRefcon);
  
  Py_RETURN_NONE;
}

My_DOCSTR(_unregisterCommandHandler__doc__, "unregisterCommandHandler",
          "commandRef, callback, before=1, refCon=None",
          "commandRef:XPLMCommandRef, callback:Callable[[XPLMCommandRef, XPLMCommandPhase, Any], int], before:int=1, refCon:Any=None",
          "None",
          "Unregister commandRef. Parameters must match those provided with registerCommandHandler()");
static PyObject *XPLMUnregisterCommandHandlerFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("commandRef"), CHAR("callback"), CHAR("before"), CHAR("refCon"), nullptr};
  (void) self;
  PyObject *inCommand;
  PyObject *inHandler;
  int inBefore=1;
  PyObject *inRefcon=Py_None;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "OO|iO", keywords, &inCommand, &inHandler, &inBefore, &inRefcon)) {
    return nullptr;
  }
  
  XPLMCommandRef commandRef = getVoidPtr(inCommand, "XPLMCommandRef");
  
  bool found = false;
  for (auto it = commandCallbacks.begin(); it != commandCallbacks.end();) {
    CommandCallbackInfo& info = it->second;
    if (info.command == commandRef &&
        info.before == inBefore &&
        1 == PyObject_RichCompareBool(info.callback, inHandler, Py_EQ) &&
        1 == PyObject_RichCompareBool(info.refcon, inRefcon, Py_EQ)) {
        
        XPLMUnregisterCommandHandler(info.command, genericCommandCallback,
                                   info.before, (void*)it->first);
        Py_DECREF(info.callback);
        Py_DECREF(info.refcon);
        deleteCapsule(inCommand);
        it = commandCallbacks.erase(it);
        found = true;
        break;
    } else {
      ++ it;
    }
  }
  if (!found) {
    pythonLog("Warning: [%s] Attempted to unregister command handler that was not found", CurrentPythonModuleName);
  }
  
  Py_RETURN_NONE;
}

PyObject* buildCommandCallbackDict(void)
{
  PyObject *dict = PyDict_New();
  if (!dict) {
    return nullptr;
  }
  
  bool error_occurred = false;
  
  for (const auto& pair : commandCallbacks) {
    const CommandCallbackInfo& info = pair.second;
    intptr_t refcon_id = pair.first;
    
    // Initialize all pointers to nullptr
    PyObject *key = nullptr;
    PyObject *command_capsule = nullptr;
    PyObject *module_name_p = nullptr;
    PyObject *before = nullptr;
    PyObject *value = nullptr;
    
    // Create all Python objects
    key = PyLong_FromLong(refcon_id);
    if (!key) {
      error_occurred = true;
      goto cleanup_iteration;
    }
    
    command_capsule = makeCapsule(info.command, "XPLMCommandRef");
    if (!command_capsule) {
      error_occurred = true;
      goto cleanup_iteration;
    }

    module_name_p = PyUnicode_FromString(info.module_name);
    if (!module_name_p) {
      error_occurred = true;
      goto cleanup_iteration;
    }
    
    before = PyLong_FromLong(info.before);
    if (!before) {
      error_occurred = true;
      goto cleanup_iteration;
    }
    
    // Build tuple: (command, callback, before, refcon, module_name)

    value = PyTuple_Pack(5,
                         module_name_p,
                         command_capsule,
                         info.callback,
                         before,
                         info.refcon);
    if (!value) {
      error_occurred = true;
      goto cleanup_iteration;
    }
    
    command_capsule = nullptr; // Mark as stolen
    Py_INCREF(info.callback);                       // increment for tuple
    before = nullptr; // Mark as stolen
    Py_INCREF(info.refcon);                         // increment for tuple
    module_name_p = nullptr; // Mark as stolen
    
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
    // Clean up any non-nullptr objects that weren't stolen
    if (key) Py_DECREF(key);
    if (command_capsule) Py_DECREF(command_capsule);
    if (module_name_p) Py_DECREF(module_name_p);
    if (before) Py_DECREF(before);
    if (value) Py_DECREF(value);
    break; // Exit the loop on error
  }
  
  if (error_occurred) {
    Py_DECREF(dict);
    return nullptr;
  }
  
  return dict;
}

PyObject* buildErrorCallbackDict(void)
{
  PyObject *dict = PyDict_New();
  if (!dict) {
    return nullptr;
  }

  bool error_occurred = false;

  for (const auto& pair : errorCallbacks) {
    const ErrorCallbackInfo& info = pair.second;
    const std::string& module_name = pair.first;

    // Initialize all pointers to nullptr
    PyObject *key = nullptr;
    PyObject *value = nullptr;
    PyObject *module_name_p = nullptr;

    // Create all Python objects
    key = PyUnicode_FromString(module_name.c_str());
    if (!key) {
      error_occurred = true;
      goto cleanup_iteration;
    }

    module_name_p = PyUnicode_FromString(info.module_name);
    if (!module_name_p) {
      error_occurred = true;
      goto cleanup_iteration;
    }

    // Build tuple: (callback, module_name)
    value = PyTuple_New(2);
    if (!value) {
      error_occurred = true;
      goto cleanup_iteration;
    }

    // Set tuple items (PyTuple_SetItem steals references)
    Py_INCREF(info.callback);                       // increment for tuple
    PyTuple_SetItem(value, 0, info.callback);       // steals ref

    PyTuple_SetItem(value, 1, module_name_p);       // steals ref
    module_name_p = nullptr; // Mark as stolen

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
    // Clean up any non-nullptr objects that weren't stolen
    if (key) Py_DECREF(key);
    if (module_name_p) Py_DECREF(module_name_p);
    if (value) Py_DECREF(value);
    break; // Exit the loop on error
  }

  if (error_occurred) {
    Py_DECREF(dict);
    return nullptr;
  }

  return dict;
}

void clearInstanceCommands(char *module_name)
{
  pythonDebug("%*s Clearing commands for [%s]", 6, " ", module_name);
  int count = 0;
  
  for (auto it = commandCallbacks.begin(); it != commandCallbacks.end();) {
    CommandCallbackInfo& info = it->second;
    if (0 == strcmp(info.module_name, module_name)) {
      count++;
      pythonDebug("%*s Removing command handler for [%lld] [%s]", 8, " ", info.refcon_id, info.module_name);
      
      XPLMUnregisterCommandHandler(info.command, genericCommandCallback,
                                   info.before, (void*)info.refcon_id);
      Py_DECREF(info.callback);
      Py_DECREF(info.refcon);
      it = commandCallbacks.erase(it);
    } else {
      ++it;
    }
  }
  
  pythonDebug("%*s Cleared %d commands for [%s]", 8, " ", count, module_name);
}

static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  
  // Clean up error callbacks
  for (auto& pair : errorCallbacks) {
    Py_DECREF(pair.second.callback);
  }
  errorCallbacks.clear();
  
  // Clean up command callbacks
  for (auto& pair : commandCallbacks) {
    Py_DECREF(pair.second.callback);
    Py_DECREF(pair.second.refcon);
  }
  commandCallbacks.clear();

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
  {"getCommandCallbackDict", (PyCFunction)buildCommandCallbackDict, METH_VARARGS, "Copy of internal CommandCallbackInfo"},
  {"getErrorCallbackDict", (PyCFunction)buildErrorCallbackDict, METH_VARARGS, "Copy of internal ErrorCallbackInfo"},
  {"_cleanup", cleanup, METH_VARARGS, ""},
  {nullptr, nullptr, 0, nullptr}
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
  nullptr,
  nullptr,
  nullptr,
  nullptr
};

PyMODINIT_FUNC
PyInit_XPLMUtilities(void)
{
  PyObject *mod = PyModule_Create(&XPLMUtilitiesModule);
  if(mod){
    PyModule_AddStringConstant(mod, "__author__", "Peter Buckner (pbuck@xppython3.org)");
    PyModule_AddIntConstant(mod, "xplm_Host_Unknown", xplm_Host_Unknown); // XPLMHostApplicationID
    PyModule_AddIntConstant(mod, "xplm_Host_XPlane", xplm_Host_XPlane); // XPLMHostApplicationID
    PyModule_AddIntConstant(mod, "Host_Unknown", xplm_Host_Unknown); // XPLMHostApplicationID
    PyModule_AddIntConstant(mod, "Host_XPlane", xplm_Host_XPlane); // XPLMHostApplicationID

    PyModule_AddIntConstant(mod, "xplm_Language_Unknown", xplm_Language_Unknown); // XPLMLanguageCode
    PyModule_AddIntConstant(mod, "xplm_Language_English", xplm_Language_English); // XPLMLanguageCode
    PyModule_AddIntConstant(mod, "xplm_Language_French", xplm_Language_French); // XPLMLanguageCode
    PyModule_AddIntConstant(mod, "xplm_Language_German", xplm_Language_German); // XPLMLanguageCode
    PyModule_AddIntConstant(mod, "xplm_Language_Italian", xplm_Language_Italian); // XPLMLanguageCode
    PyModule_AddIntConstant(mod, "xplm_Language_Spanish", xplm_Language_Spanish); // XPLMLanguageCode
    PyModule_AddIntConstant(mod, "xplm_Language_Korean", xplm_Language_Korean); // XPLMLanguageCode
    PyModule_AddIntConstant(mod, "xplm_Language_Russian", xplm_Language_Russian); // XPLMLanguageCode
    PyModule_AddIntConstant(mod, "xplm_Language_Greek", xplm_Language_Greek); // XPLMLanguageCode
    PyModule_AddIntConstant(mod, "xplm_Language_Japanese", xplm_Language_Japanese); // XPLMLanguageCode
    PyModule_AddIntConstant(mod, "Language_Unknown", xplm_Language_Unknown); // XPLMLanguageCode
    PyModule_AddIntConstant(mod, "Language_English", xplm_Language_English); // XPLMLanguageCode
    PyModule_AddIntConstant(mod, "Language_French", xplm_Language_French); // XPLMLanguageCode
    PyModule_AddIntConstant(mod, "Language_German", xplm_Language_German); // XPLMLanguageCode
    PyModule_AddIntConstant(mod, "Language_Italian", xplm_Language_Italian); // XPLMLanguageCode
    PyModule_AddIntConstant(mod, "Language_Spanish", xplm_Language_Spanish); // XPLMLanguageCode
    PyModule_AddIntConstant(mod, "Language_Korean", xplm_Language_Korean); // XPLMLanguageCode
    PyModule_AddIntConstant(mod, "Language_Russian", xplm_Language_Russian); // XPLMLanguageCode
    PyModule_AddIntConstant(mod, "Language_Greek", xplm_Language_Greek); // XPLMLanguageCode
    PyModule_AddIntConstant(mod, "Language_Japanese", xplm_Language_Japanese); // XPLMLanguageCode
    PyModule_AddIntConstant(mod, "xplm_Language_Chinese", xplm_Language_Chinese); // XPLMLanguageCode
    PyModule_AddIntConstant(mod, "Language_Chinese", xplm_Language_Chinese); // XPLMLanguageCode
    PyModule_AddIntConstant(mod, "xplm_Language_Ukrainian", xplm_Language_Ukrainian); // XPLMLanguageCode
    PyModule_AddIntConstant(mod, "Language_Ukrainian", xplm_Language_Ukrainian); // XPLMLanguageCode

    PyModule_AddIntConstant(mod, "xplm_DataFile_Situation", xplm_DataFile_Situation); // XPLMDataFileType
    PyModule_AddIntConstant(mod, "xplm_DataFile_ReplayMovie", xplm_DataFile_ReplayMovie); // XPLMDataFileType
    PyModule_AddIntConstant(mod, "DataFile_Situation", xplm_DataFile_Situation); // XPLMDataFileType
    PyModule_AddIntConstant(mod, "DataFile_ReplayMovie", xplm_DataFile_ReplayMovie); // XPLMDataFileType
    
    PyModule_AddIntConstant(mod, "xplm_CommandBegin", xplm_CommandBegin); // XPLMCommandPhase
    PyModule_AddIntConstant(mod, "xplm_CommandContinue", xplm_CommandContinue); // XPLMCommandPhase
    PyModule_AddIntConstant(mod, "xplm_CommandEnd", xplm_CommandEnd); // XPLMCommandPhase
    PyModule_AddIntConstant(mod, "CommandBegin", xplm_CommandBegin); // XPLMCommandPhase
    PyModule_AddIntConstant(mod, "CommandContinue", xplm_CommandContinue); // XPLMCommandPhase
    PyModule_AddIntConstant(mod, "CommandEnd", xplm_CommandEnd); // XPLMCommandPhase

  }

  if(errorCallbacks.empty()){
    XPLMSetErrorCallback(error_callback);
  }

  return mod;
}
