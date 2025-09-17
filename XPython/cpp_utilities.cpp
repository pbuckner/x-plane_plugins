#include <vector>
#include <unordered_map>
#include <Python.h>
#include <string>
#include "utils.h"

static std::unordered_map<void*, PyObject*> commandCapsulesCPP;

// Function that takes vector of std::string and returns null-terminated char* array
char** stringVectorToCharArray(const std::vector<std::string>& stringVector) {
  size_t size = stringVector.size();
  
  // Allocate array of char* pointers (size + 1 for NULL terminator)
  char** result = new char*[size + 1];
  
  // Copy each string to dynamically allocated char arrays
  for (size_t i = 0; i < size; i++) {
    size_t len = stringVector[i].length() + 1; // +1 for null terminator
    result[i] = new char[len];
    strcpy(result[i], stringVector[i].c_str());
  }
  
  // Set the final element to NULL for null termination
  result[size] = nullptr;
  
  return result;
}

// Helper function to free the allocated memory
void freeCharArray(char** charArray, size_t size) {
  for (size_t i = 0; i < size; i++) {
    delete[] charArray[i];
  }
  delete[] charArray;
}

PyObject* getPtrRefCPP(void *ptr, std::unordered_map<void*, PyObject*>& dict, const char *refName)
{
  if(!ptr){
    Py_RETURN_NONE;
  }

  auto it = dict.find(ptr);
  PyObject *capsule;

  if(it == dict.end()){
    capsule = PyCapsule_New(ptr, refName, NULL);
    dict[ptr] = capsule;
  } else {
    capsule = it->second;
  }

  Py_INCREF(capsule);
  return capsule;
}

void removePtrRefCPP(void *ptr, std::unordered_map<void *,PyObject *>& dict)
{
  errCheck("prior removePtrRefCPP");
  if(!ptr) return;
  auto it = dict.find(ptr);
  if (it == dict.end()) return;

  Py_DECREF(it->second);
  dict.erase(it->first);
  errCheck("end removePtrRef");
}
