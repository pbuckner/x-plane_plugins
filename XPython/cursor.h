#ifndef CURSOR_H
#define CURSOR_H

#if APL
#import <AppKit/NSCursor.h>
#import <AppKit/NSImage.h>
struct cursor_s {
  NSCursor	*crs;
};
#endif

#if LIN
#include <X11/Xcursor/Xcursor.h>
#include <XPLM/XPLMDataAccess.h>
#include <png.h>
struct cursor_s {
  Cursor crs;
};
#endif

#if IBM
#include <windows.h>
struct cursor_s {
  HCURSOR crs;
};
#endif


typedef struct cursor_s cursor_t;
#define MAX_CURSORS 100

#endif
