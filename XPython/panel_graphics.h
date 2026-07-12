#ifndef PANEL_GRAPHICS__H
#define PANEL_GRAPHICS__H
#include <Python.h>

/* Capsule type names shared across the panel_graphics_*.cpp files. */
#define FONT_CAPSULE "XPLMFontHandle"
#define ATLAS_CAPSULE "XPLMTextureAtlasRef"
#define RETAINED_CAPSULE "XPLMRetainedDrawing_t"
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
#endif
