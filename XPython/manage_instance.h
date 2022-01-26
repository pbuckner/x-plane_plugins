#ifndef MANAGE_INSTANCE__H
#define MANAGE_INSTANCE__H
void xpy_reloadInstance(PyObject *signature);
int xpy_startInstance(PyObject *moduleName, PyObject *module, PyObject *pluginInstance);
void xpy_stopInstance(PyObject *moduleName, PyObject *pluginInstance);
void xpy_cleanUpInstance(PyObject *moduleName, PyObject *pluginInstance);
void xpy_disableInstance(PyObject *moduleName, PyObject *pluginInstance);
int xpy_enableInstance(PyObject *moduleName, PyObject *pluginInstance);
#endif

