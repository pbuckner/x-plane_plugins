#include <string>
#include <vector>
char **stringVectorToCharArray(const std::vector<std::string>& stringVector);
void freeCharArray(char **charArray, size_t size);
