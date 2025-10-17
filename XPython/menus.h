#ifndef MENUS__H
#define MENUS__H
#include <Python.h>
void resetMenus(void);
void clearInstanceMenuItems(char *);
PyObject* buildMenuCallbackDict(void);
#endif
