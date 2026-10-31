#ifndef PANEL_GRAPHICS__H
#define PANEL_GRAPHICS__H
#include <Python.h>

/* Capsule type names shared across the panel_graphics_*.cpp files. */
#define FONT_CAPSULE "XPLMFontHandle"
#define ATLAS_CAPSULE "XPLMTextureAtlasRef"
#define RETAINED_CAPSULE "XPLMRetainedDrawing"
#define SVT_CAPSULE "XPLMSVTDisplayRef"
#define MAP_CAPSULE "XPLMMapDisplayRef"
#define TEXTURE_CAPSULE "XPLMTexture"
#define COMMAND_CAPSULE "XPLMCommandRef"
#define WINDOW_CAPSULE "XPLMWindowID"
#define AVIONICS_CAPSULE "XPLMAvionicsID"

/* Method-table fragments, one per panel_graphics_*.cpp file. PyInit_XPLMPanelGraphics
   (in panel_graphics.cpp) merges them into the module with PyModule_AddFunctions. */
extern PyMethodDef panelGraphicsPrimitivesMethods[];
extern PyMethodDef panelGraphicsFontMethods[];
extern PyMethodDef panelGraphicsTextureMethods[];
extern PyMethodDef panelGraphicsDisplayMethods[];
extern PyMethodDef panelGraphicsTouchMethods[];

/* A handle argument which is None / not a capsule must never reach the SDK. getVoidPtr() returns NULL
   (with a Python exception set if the object was not a capsule at all). Returns true, with an exception
   set, if the handle is unusable. */
static inline bool pgHandleInvalid(void *handle, const char *what)
{
  if (handle) return false;
  if (!PyErr_Occurred()) {
    PyErr_Format(PyExc_ValueError, "%s is None or no longer valid.", what);
  }
  return true;
}

/* Ownership of SDK resources created through this module, so they are released when the creating script
   stops / reloads (panel_graphics.cpp). Destroy order matters (retained drawings reference fonts and atlases),
   see pgReleaseForModule(). */
enum PGKind { PG_RETAINED, PG_SVT, PG_MAP, PG_TEXTURE, PG_ATLAS, PG_FONT };
extern void pgTrack(void *handle, PGKind kind);     /* owner is CurrentPythonModuleName */
extern void pgUntrack(void *handle);
extern void pgReleaseForModule(const char *module_name);
extern void pgReleaseAll(void);

/* Touch-event handlers (panel_graphics_touch.cpp) */
extern void touchHandlerDrop(void *handle);                       /* handle is gone: forget handler, no SDK call */
extern void touchHandlerMigrate(void *oldHandle, void *newHandle); /* avionics SDK id changed on re-register */
extern void touchHandlersRemoveForModule(const char *module_name);
extern void touchHandlersReset(void);
#endif
