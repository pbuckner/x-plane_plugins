#ifndef XPPYTHON_H
#include <Python.h>
#define XPPYTHON_H
#include <pthread.h>
typedef struct {
  long fl_time; /* flight loop callback */
  long customw_time; /* custom widget drawing */
  long draw_time;  /* draw callback */
  PyObject *pluginInstance;
} PluginStats;

extern PyObject *PythonModuleMTimes;
extern PluginStats pluginStats[];
int getPluginIndex(PyObject *);
void resetInternals();
extern pthread_t pythonThread;
#endif
