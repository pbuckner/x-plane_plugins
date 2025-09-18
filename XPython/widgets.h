#ifndef WIDGETS__H
#define WIDGETS__H
void resetWidgets(void);
void logWidgets(PyObject *key, char *key_s, char *value_s);
#include <unordered_map>
extern std::unordered_map<void*, PyObject*> widgetIDCapsules;
#endif
