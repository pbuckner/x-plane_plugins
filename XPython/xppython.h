#ifndef XPPYTHON_H
#define XPPYTHON_H
typedef struct {
  long fl_time; /* flight loop callback */
  long customw_time; /* custom widget drawing */
  long draw_time;  /* draw callback */
  PyObject *pluginInstance;
} PluginStats;

extern PluginStats pluginStats[];
int getPluginIndex(PyObject *);
#endif
