#pragma once
#include <Python.h>
#include <string>
#include <unordered_map>
void deleteCapsule(PyObject *);
void *getVoidPtr(PyObject *);
void *getVoidPtr(PyObject *, std::string);
PyObject *makeCapsule(void *, std::string);
void logCapsules();
extern std::unordered_map <void *, PyObject*> CapsuleDict;  // {ptr: capsule}
extern int pythonCapsuleRegistration;
