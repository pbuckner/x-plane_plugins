#ifndef WINDOWS__H
#include <Python.h>
#define WINDOWS__H
void resetWindows(void);
void resetDrawCallbacks(void);
PyObject* buildDrawCallbackDict(void);
PyObject* buildWindowCallbackDict(void);
PyObject* buildAvionicsCallbackDict(void);
PyObject* buildKeySnifferCallbackDict(void);
PyObject* buildHotKeyCallbackDict(void);
#include <unordered_map>
extern std::unordered_map<void*, PyObject*> windowIDCapsules;
#endif
