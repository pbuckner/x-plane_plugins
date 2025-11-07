#define _GNU_SOURCE 1
#include <Python.h>
#include "utils.h"
#include "cursor.h"
#include <errno.h>
#include <stdlib.h>

static cursor_t **Cursors;
static int cursorIdx = 0;

static cursor_t *cursor_read_from_file(const char *filename_png);
static void cursor_free(cursor_t *cursor);
static void cursor_make_current(cursor_t *cursor);

My_DOCSTR(_setCursor__doc__, "setCursor",
          "cursor_id=0",
          "cursor_id:int=0",
          "None",
          "Sets the mouse cursor based on enumeration.\n"
          "\n"
          "Must be called within a mouse cursor callback.");
static PyObject *setCursor(PyObject *self, PyObject *args, PyObject *kwargs)
{
  (void) self;
  static char *kwlist[] = {"cursor_id", NULL};
  int inCursorID = 0;
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|i", kwlist, &inCursorID)) {
    return NULL;
  }
  
  if (Cursors[inCursorID] == NULL) {
    PyErr_SetString(PyExc_ValueError , "setCursor: Unknown cursor enumeration.");
    return NULL;
  }
  cursor_make_current(Cursors[inCursorID]);
  Py_RETURN_NONE;
}

My_DOCSTR(_unloadCursor__doc__, "unloadCursor",
          "cursor_id=0",
          "cursor_id:int=0",
          "int",
          "Unloads cursor from memory.\n");
static PyObject *unloadCursor(PyObject *self, PyObject *args, PyObject *kwargs)
{
  (void) self;

  static char *kwlist[] = {"cursor_id", NULL};
  int inCursorID = 0;
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|i", kwlist, &inCursorID)) {
    return NULL;
  }
  if (inCursorID < MAX_CURSORS && Cursors[inCursorID] != NULL) {
    cursor_free(Cursors[inCursorID]);
    Cursors[inCursorID] = NULL;
  }
  Py_RETURN_NONE;
}

My_DOCSTR(_loadCursor__doc__, "loadCursor",
          "filename",
          "filename:str",
          "int",
          "Loads cursor from given file. Should be partial filename,\n"
          "(that is, without .png or .cur extension), and relative\n"
          "X-Plane root (e.g. 'Resources/plugins/PythonPlugins/myPlugin/hand')\n"
          "\n"
          "Multiple calls to with same filename will result in\n"
          "repeated loads. Try not to do this.\n"
          "\n"
          "Returns cursorID usable with setCursor.");
static PyObject *loadCursor(PyObject *self, PyObject *args, PyObject *kwargs)
{
  (void) self;
  static char *kwlist[] = {"filename", NULL};
  const char *cursorFileName;
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s", kwlist, &cursorFileName)) {
    return NULL;
  }
  
  if (cursorFileName[0] == '/' || cursorFileName[0] == '\\') {
    PyErr_SetString(PyExc_ValueError , "loadCursor: Bad filename");
    return NULL;
  }
  cursor_t *cursor = cursor_read_from_file(cursorFileName);
  if (cursor == NULL) {
    PyErr_SetString(PyExc_ValueError , "loadCursor: Failed to read cursor from filename.");
    return NULL;
  }

  Cursors[cursorIdx] = cursor;
  return PyLong_FromLong(cursorIdx++);
}


/*
  NEEDED for each platform:
  static cursor_t cursor_read_from_file(const char *filename);
  static void cursor_free(cursor_t *cursor);
  static void cursor_make_current(cursor_t *cursor);
 */

#if APL
static cursor_t *
cursor_read_from_file(const char *filename)
{
  cursor_t *cursor = malloc(sizeof (*cursor));
  NSString *path;
  NSImage *img;
  NSImageRep *rep;
  NSPoint p;
  
  char *filename_ext = (char *)malloc(strlen(filename) + 5);
  strcpy(filename_ext, filename);
  strcat(filename_ext, ".png");

  path = [NSString stringWithUTF8String: filename_ext];
  img = [[NSImage alloc] initWithContentsOfFile: path];
  if (img == nil) {
    pythonLog("Can't open cursor image file %s: %s", filename_ext, strerror(errno));
    free(filename_ext);
    free(cursor);
    return NULL;
  }
  rep = [[img representations] objectAtIndex: 0];
 
  p = NSMakePoint([rep pixelsWide] / 2, [rep pixelsHigh] / 2);
  cursor->crs = [[NSCursor alloc] initWithImage: img hotSpot: p];
 
  [img release];
  
  free(filename_ext);
  return cursor;
}

static void cursor_free(cursor_t *cursor) {if (cursor == NULL || cursor->crs == NULL) {return;}[cursor->crs release];free(cursor);}

static void cursor_make_current(cursor_t *cursor) {if (cursor == NULL || cursor->crs == NULL) {return;}[cursor->crs set];}

#endif

#if LIN
/* because cursors are only ever created and used from the main rendering
   thread, it is safe to use a simple unprotected global with refcount.
*/
static int dpy_refcont = 0;
static Display *dpy = NULL;
static XPLMDataRef drSystemWindow;

static cursor_t *
cursor_read_from_file(const char *filename)
{
  cursor_t *cursor;
  uint8_t *buf;
  int w, h;
  XcursorImage img = { .pixels = NULL };

  char *filename_ext = (char *)malloc(strlen(filename) + 5);
  strcpy(filename_ext, filename);
  strcat(filename_ext, ".png");

  buf = png_load_from_file_rgba(filename_ext, &w, &h);
  if (buf == NULL) {
    pythonLog("Can't open cursor image file %s", filename_ext);
    return NULL;
  }
  
  free(filename_ext);

  if (dpy_refcount == 0) {
    dpy = XOpenDisplay(NULL);
  }
  if (dpy == NULL) {
    pythonLog("Can't open X11 display for cursor");
    free(buf);
    return (NULL);
  }
  dpy_refcount++;
  
  img.size = w;
  img.width = w;
  img.height = h;
  img.xhot = w / 2;
  img.yhot = h / 2;
  img.pixels = (XcursorPixel *)buf;
  cursor = calloc(1, sizeof (*cursor));
  cursor->crs = XcursorImageLoadCursor(dpy, &img);
  
  free(buf);
  
  return (cursor);
}

void
cursor_free(cursor_t *cursor)
{
  if (cursor == NULL) {
    return;
  }
  
  XFreeCursor(dpy, cursor->crs);
  free(cursor);
  
  dpy_refcount--;

  if (dpy_refcount == 0) {
    XCloseDisplay(dpy);
    dpy = NULL;
  }
}

void
cursor_make_current(cursor_t *cursor)
{
  int win_ptr[2];
  Window win;
  
  if (cursor == NULL || dpy == NULL) {
    return;
  }

  if (XPLMGetDatavi(drSystemWindow, win_ptr, 0, 2) != 2) {
    pythonLog("Failed to get system window dataref");
    return;
  }

  memcpy(&win, win_ptr, sizeof (void *));
  
  XDefineCursor(dpy, win, cursor->crs);
  XFlush(dpy);
}  
#endif

static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  for (int i = 0; i < cursorIdx; i++) {
    cursor_free(Cursors[i]);
    Cursors[i] = NULL;
  }
  cursorIdx = 0;

  Py_RETURN_NONE;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
static PyMethodDef XPCursorMethods[] = {
  {"setCursor", (PyCFunction)setCursor, METH_VARARGS | METH_KEYWORDS, _setCursor__doc__},
  {"loadCursor", (PyCFunction)loadCursor, METH_VARARGS | METH_KEYWORDS, _loadCursor__doc__},
  {"unloadCursor", (PyCFunction)unloadCursor, METH_VARARGS | METH_KEYWORDS, _unloadCursor__doc__},
  {"_cleanup", cleanup, METH_VARARGS, ""},
  {NULL, NULL, 0, NULL}
};
#pragma GCC diagnostic pop


static struct PyModuleDef XPCursorModule = {
  PyModuleDef_HEAD_INIT,
  "XPCursor",
  "XPCursor documentation: \n"
  "   https://xppython3.rtfd.io/en/stable/development/modules/cursor.html",
  -1,
  XPCursorMethods,
  NULL,
  NULL,
  NULL,
  NULL
};

PyMODINIT_FUNC
PyInit_XPCursor(void)
{
  PyObject *mod = PyModule_Create(&XPCursorModule);

  Cursors = calloc(MAX_CURSORS, sizeof(cursor_t*));
  Cursors[cursorIdx++] = cursor_read_from_file("Resources/plugins/XPPython3/images/cursor_grab_open");

#if LIN  
  drSystemWindow = XPLMFindDataRef("sim/operation/windows/system_window_64");
#endif

  if (mod != NULL) {
    PyModule_AddStringConstant(mod, "__author__", "Peter Buckner (pbuck@xppython3.org)");
  }
  return mod;
}
