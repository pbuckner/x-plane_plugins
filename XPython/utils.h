#ifndef UTILS__H
#include <Python.h>
#include <string>
#include <unordered_set>
#define UTILS__H

#define CHAR(x) const_cast<char*>(x)

#define My_DOCSTR(name,method,parms,params_doc,return,str) static const char name[] = method"($module, /, " parms ")\n--\n\n" str
extern PyObject *XPY3pythonDicts;
extern PyObject *XPY3pythonCapsules;
extern int pythonDebugs;
extern int pythonWarnings;
extern int pythonStats;
extern int pythonFlushLog;
extern int pythonCapsuleRegistration;
extern char *CurrentPythonModuleName;
extern char *XPPython3ModuleName;

// String interning pool - stores module names with stable pointers
// Strings in unordered_set don't move on rehash, so c_str() pointers remain valid
extern std::unordered_set<std::string> moduleNamePool;

// Intern a module name string - returns stable pointer that won't be invalidated
// Multiple calls with same string return same pointer (pointer equality works)
const char* intern_moduleName(const std::string& name);
const char* intern_moduleName(const char* name);

// Clear the pool (only called on complete shutdown)
void cleanup_moduleNamePool();

// Pre-allocated Python objects for performance optimization
extern PyObject *PHASE_OBJECTS[8];    // Drawing phases 0-7
extern PyObject *BOOL_OBJECTS[2];     // Boolean values 0,1

void dbg(const char *msg);
void pythonLogException(void);

void set_moduleName(const std::string&);
void set_moduleName(char *);
void set_moduleName(const char*);
PyObject *get_moduleName_p();
PyObject *get_pythonline();
char *objToStr(PyObject *item);
char *objDebug(PyObject *item);
void pythonLogWarning(const char *msg);
void pythonDebug(const char *fmt, ...);
void setLogFile(void);
void pythonLog(const char *fmt, ...);
void pythonLogRaw(const char *msg);
void pythonLogFlush(void);
void pythonLogClose(void);

extern const char *pythonPluginVersion;
extern const char *pythonPluginsPath;
extern const char *pythonInternalPluginsPath;
void MyPyRun_String(const char *str, int start, PyObject *globals, PyObject *locals);
void MyPyRun_File(FILE *fp, const char *filename, int start, PyObject *globals, PyObject *locals);
void xpy_assert(bool);
void errCheck_f(const char *fmt, ...);
#if ERRCHECK
#define errCheck(fmt, ...) {errCheck_f(fmt, ##__VA_ARGS__);}
#else
#define errCheck(fmt, ...) {;}
#endif
// Pre-allocated object management
void initPreallocatedObjects(void);
void cleanupPreallocatedObjects(void);
#endif
