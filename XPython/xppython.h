#ifndef XPPYTHON_H
#include <Python.h>
#define XPPYTHON_H
#include <pthread.h>
#include <string>
typedef struct {
  long fl_time; /* flight loop callback */
  long customw_time; /* custom widget drawing */
  long draw_time;  /* draw callback */
  std::string module_name;
} PluginStats;

extern PyObject *PythonModuleMTimes;
extern PluginStats pluginStats[];
int getPluginIndex();
void resetInternals();
PyObject *XPPythonGetDictsFun(PyObject *, PyObject*);
extern pthread_t pythonThread;
#endif
