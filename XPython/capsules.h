#pragma once
#include <Python.h>
#include <string>
#include <unordered_map>
void deleteCapsule(PyObject *);
void deleteCapsuleByPtr(void *, const char *);
void *getVoidPtr(PyObject *);
void *getVoidPtr(PyObject *, std::string);
PyObject* buildCapsuleDict(void);
PyObject *makeCapsule(void *, std::string);
void logCapsules();
extern std::unordered_map <void *, PyObject*> CapsuleDict;  // {ptr: capsule}
extern int pythonCapsuleRegistration;
