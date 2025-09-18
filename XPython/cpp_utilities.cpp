#define _GNU_SOURCE 1
#include <vector>
#include <string>
#include "cpp_utilities.hpp"

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
