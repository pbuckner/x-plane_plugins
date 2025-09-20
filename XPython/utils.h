#ifndef UTILS__H
#include <Python.h>
#include <string>
#define UTILS__H

#define My_DOCSTR(name,method,parms,params_doc,return,str) static const char name[] = method"($module, /, " parms ")\n--\n\n" str
extern PyObject *XPY3pythonDicts;
extern PyObject *XPY3pythonCapsules;
extern int pythonDebugs;
extern int pythonWarnings;
extern int pythonStats;
extern int pythonFlushLog;
extern int pythonCapsuleRegistration;
extern char CurrentPythonModuleName[];

void dbg(const char *msg);
void pythonLogException(void);

void set_moduleName(PyObject *);
void set_moduleName(std::string);
void set_moduleName(char *);
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
void errCheck_f(const char *fmt, ...);
#define errCheck(fmt, ...) do { if (ERRCHECK) errCheck_f(fmt, ##__VA_ARGS__); } while(0)
#endif
