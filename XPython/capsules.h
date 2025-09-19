#pragma once
#include <Python.h>
#include <string>
void deleteCapsule(PyObject *);
void *getVoidPtr(PyObject *);
void *getVoidPtr(PyObject *, std::string);
PyObject *makeCapsule(void *, std::string);
void logCapsules();
extern int pythonCapsuleRegistration;
