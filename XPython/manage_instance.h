#ifndef MANAGE_INSTANCE__H
#define MANAGE_INSTANCE__H
#include "manage_instances.h"

void xpy_reloadInstance(PyObject *signature);
int xpy_startInstance(PyObject *module, PyObject *pluginInstance, PluginType plugin_type);
void xpy_stopInstance(PyObject *pluginInstance);
void xpy_cleanUpInstance(PyObject *pluginInstance);
void xpy_disableInstance(PyObject *pluginInstance);
int xpy_enableInstance(PyObject *pluginInstance);
#endif

