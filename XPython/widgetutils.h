#ifndef _WIDGETUTILS
#define _WIDGETUTILS
#include <Python.h>
#include <Widgets/XPWidgetUtils.h>
void convertMessagePythonToC(XPWidgetMessage, PyObject *, PyObject *, PyObject *, XPWidgetID *, intptr_t *, intptr_t *);
#endif
