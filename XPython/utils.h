#ifndef UTILS__H
#define UTILS__H

#define My_DOCSTR(name,method,parms,str) static const char name[] = method"($module, /, "parms")\n--\n\n"str
extern const char *objRefName;
extern const char *commandRefName;
extern const char *widgetRefName;
extern const char *windowIDRef;
extern PyObject *widgetIDCapsules;
extern PyObject *windowIDCapsules;
extern PyObject *xppythonDicts;
extern PyObject *xppythonCapsules;
extern int pythonDebugs;
extern int pythonWarnings;
extern int pythonFlushLog;

void dbg(const char *msg);
float getFloatFromTuple(PyObject *seq, Py_ssize_t i);
long getLongFromTuple(PyObject *seq, Py_ssize_t i);
void pythonLogException(void);

PyObject *getPtrRef(void *ptr, PyObject *dict, const char *refName);
PyObject *getPtrRefOneshot(void *ptr, const char *refName);
void *refToPtr(PyObject *ref, const char *refName);
void removePtrRef(void *ptr, PyObject *dict);
char *get_moduleName();
PyObject *get_pluginSelf();
char *objToStr(PyObject *item);
char *objDebug(PyObject *item);
void pythonLogWarning(const char *msg);
void pythonDebug(const char *fmt, ...);
void setLogFile(void);
void pythonLog(const char *fmt, ...);
void pythonLogFlush(void);
void pythonLogClose(void);

extern const char *pythonPluginVersion;
extern const char *pythonPluginsPath;
extern const char *pythonInternalPluginsPath;
void MyPyRun_String(const char *str, int start, PyObject *globals, PyObject *locals);
void errCheck_f(const char *fmt, ...);
#define errCheck(fmt, ...) do { if (ERRCHECK) errCheck_f(fmt, ##__VA_ARGS__); } while(0)
#endif
