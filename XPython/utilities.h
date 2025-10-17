#ifndef UTILITIES__H
#define UTILITIES__H
void resetCommands(void);
void clearInstanceCommands(char *module_name);
PyObject* buildCommandCallbackDict(void);
PyObject* buildErrorCallbackDict(void);
#endif
