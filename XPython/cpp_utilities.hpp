#include <string>
#include <vector>
#include <unordered_map>
#include <Python.h>

char **stringVectorToCharArray(const std::vector<std::string>& stringVector);
void freeCharArray(char **charArray, size_t size);
PyObject* getPtrRefCPP(void *ptr, std::unordered_map<void*, PyObject*>& dict, const char *refName);
void removePtrRefCPP(void *ptr, std::unordered_map<void *,PyObject *>& dict);
