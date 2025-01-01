#ifndef WIDGETUTILS__H
#define WIDGETUTILS__H 1
#include <Python.h>
#include <Widgets/XPWidgetUtils.h>
void convertMessagePythonToC(XPWidgetMessage, PyObject *, PyObject *, PyObject *, XPWidgetID *, intptr_t *, intptr_t *);
#endif
