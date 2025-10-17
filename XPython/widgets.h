#ifndef WIDGETS__H
#define WIDGETS__H
void resetWidgets(void);
void logWidgets(PyObject *key, const char *key_s, const char *value_s);
PyObject *buildWidgetCallbackDict(void);
PyObject *buildWidgetPropertiesDict(void);
#endif
