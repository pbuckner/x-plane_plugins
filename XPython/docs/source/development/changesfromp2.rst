Changes from Python2 SDK
========================

.. Note:: If you didn't do plugin development using Python2, this shoud not interest you.
   
* X-Plane SDK303 is base version (X-Plane 11.50). There is no attempt to implement deprecated API
  or test on earlier versions.

* :code:`XPLM_USE_NATIVE_PATHS` is automatically set on startup. We never use legacy paths,
  so XPLMGetSystemPath() will return '/' separated results. If you need legacy paths,
  use python within your plugin to convert.

* :code:`XPLM_USE_NATIVE_WIDGET_WINDOWS` is automatically set on startup, and disallow plugins disabling it (for python).

  .. Warning:: It is a FATAL ERROR to attempt to disable either XPLM_USE_NATIVE_PATHS or XPLM_USE_NATIVE_WIDGET_WINDOWS features.

* All routines return "PyCapsules" rather than ints for XPLMWindowIDRef, XPLMHotkeyIDRef, XPLMMenuIDRef, XPLMWidgetID. For
  the most part, you should not care.

* PI_GetMouseState() no longer required: you'll now get tuple (x, y, button, delta) as param1 in widget callbacks
  (for xpMsg_MouseDown, xpMsg_MouseDrag, xpMsg_MouseUp, xpMsp_CursorAdjust) and you can use that directly rather than
  passing it through PI_GetKeyState(). However, for backward compatibility, PI_GetMouseState() has been implemented
  as a no-op, so you can still use it.

* PI_GetKeyState() no longer required: you'll now get tuple (key, flags, vkey) as param1 in widget callbacks
  (for xpMsg_KeyPress) and you can use that directly rather than passing it through PI_GetKeyState(). However,
  for backward compatibility, PI_GetKeyState() has been implemented as a no-op, so you can still use it.

* PI_GetWidgetGeometry() no longer supported: you'll now get tuple (dx, dy, dwidth, dheight) as param1 in widget callbacks
  (for xpMsg_Reshape)

* XPGetWidgetWithFocus() returns None, instead of 0, if no widgets have focus


See also :doc:`portingNotes`.

.. note::
   For the most part, the old interface (from Sandy's python2 PythonInterface plugin) will still work, as
   we've implemented backward-compatible hooks. You **may** use the simplified interface described next,
   and/or continue to use the older API (they're compatible with each other).

   For example, these all work::

     # PythonInterface style:
     outWidth = []
     outHeight = []
     XPLMDisplay.XPLMGetScreenSize(outWidth, outHeight)
     print("Screen is {}x{}".format(outWidth[0], outHeight[0])

     # XPPython3 style:
     width, height = XPLMDisplay.XPLMGetScreenSize()
     print("Screen is {}x{}".format(width, height)

     # XPPython3 'xp' style:
     widgth, height = xp.getScreenSize()
     print("Screen is {}x{}".format(width, height)
     
     

Simplified Interfaces
---------------------

* **DEPRECATED** passing 'self' as first parameter in callback registrations (required with Python2, not with Python3)

  We're using stack manipulation to do that for you instead.

  ::

    XPLMLoadObjectAsync(path, callback, refcon)
    - not -
    XPLMLoadObjectAsync(self, path, callback, refcon)

* **DEPRECATED** extraneous parameters

  Python make string handling and memory management vastly simpler than C / C++, so we've dropped some
  API parameters which are no longer critical:  

  graphics::

    XPLMMeasureString(fontId, string)  # No longer support a third parameter "inNumChars" 

  menus::

    XPLMSetMenuItemName(...)  # Dropped the final parameter which was called "deprecatedAndIgnored"  
    XPLMAppendMenuItem(...)   # Dropped the final parameter which was called "deprecatedAndIgnored"  

  widgets::

    XPLMGetWidgetDescriptor(widgetID)
      We've dropped the `inMaxDescLength` and will return the full descriptor (up to 2048 characters)

  
* **DEPRECATED** use of "out" parameters. Instead, return values directly where we can. Often
  this means you don't need to pass in a list as a parameter to retrieve the results.

  Old::
    
    outWidth = []
    outHeight = []
    XPLMGetScreenSize(outWidth, outHeight)
    print("Screen is {}x{}".format(outWidth[0], outHeight[0])
  
  New::

    width, height = XPLMGetScreenSize()
    print("Screen is {}x{}".format(width, height)
     

  XPLMDisplay::

    XPLMGetScreenSize(outWidth, outHeight) ->  
    width, height = XPLMGetScreenSize()  

    XPLMGetMouseLocationGlobal(outX, outY) ->  
    x, y = XPLMGetMouseLocationGlobal()  

    XPLMGetMouseLocation(outX, outY) ->  
    x, y = XPLMGetMouseLocation()  
  
    XPLMGetWindowGeometry(inWindowID, outLeft, outTop, outRight, outBottom) ->  
    left, top, right, bottom = XPLMGetWindowGeometry(inWindowID)  
  
    XPLMGetWindowGeometryOS(inWindowID, outLeft, outTop, outRight, outBottom) ->  
    left, top, right, bottom = XPLMGetWindowGeometryOS(inWindowID)  
  
    XPLMGetScreenBoundsGlobal(outLeft, outTop, outRight, outBottom) ->  
    left, top, right, bottom = XPLMGetScreenBoundsGlobal()  
  
    XPLMGetWindowGeometryVR(inWindowID, outWidthBoxels, outHeightBoxels) ->  
    width, height = XPLMGetWindowGeometryVR(inWindowID)  
  
    XPLMGetHotKeyInfo(inHotKey, outVirtualKey, outFlags, outDescription, outPlugin) ->  
    HotKeyInfo = XPLMGetHotKeyInfo(inHotKey)

  XPLMGraphics::

    XPLMGenerateTextureNumber(outTextureIDs, inCount) ->  
    outTextureIDs = XPLMGenerateTextureNumber(inCount)  
  
    XPLMGetFontDimensions(inFontID, outCharWidth, outCharHeight, outDigitsOnly) ->  
    width, height, digitsOnly = XPLMGetFontDimensions(inFontID)  

  XPLMMap::

    XPLMMapProject(projection, latitude, longitude, outX, outY) ->  
    x, y = XPLMMapProject(projection, latitude, longitude)  
  
    XPLMMapUnProject(production, mapX, mapY, outLatitude, outLongitude) ->  
    lat, lon = XPLMMapUnProject(production, mapX, mapY)

  XPUIGraphics::

    XPGetWindowDefaultDimensions(inStyle, outWidth, outHeight) ->  
    width, height = XPGetWindowDefaultDimensions(inStyle)  
  
    XPGetElementDefaultDimensions(inStyle, outWidth, outHeight, outCanBeLit) ->  
    width, height, canBeLit = XPGetElementDefaultDimensions(inStyle)  
  
    XPGetTrackDefaultDimensions(inStyle, outWidth, outCanBeLit) ->  
    width, canBeLit = XPGetTrackDefaultDimensions(inStyle)  
  
    XPGetTrackMetrics(inX1, inY1, inX2, inY2, inMin, inMax, inValue, inTrackStyle, outIsVertical, outDownBtnSize, outDownPageSize, outThumbSize, outUpPageSize, outUpBtnSiz) ->  
    TrackMetrics = XPGetTrackMetrics(inX1, inY1, inX2, inY2, inMin, inMax, inValue, inTrackStyle)

  XPLMNavigation::

    XPLMGetNavAidInfo(inRef, outType, outLatitude, outLogitude, outHeight, outFrequency, outHeading, outID, outName, outReg) ->  
    NavAidInfo = XPLMGetNavAidInfo(inRef)  
  
    XPLMGetFMSEntryInfo(inIndex, outType, outID, outRef, outAltitude, outLat, outLon) ->  
    FMSEntryInfo = XPLMGetFMSEntryInfo(inIndex)  

  XPLMPlugin::

    XPLMGetPluginInfo(inPluginID, outName, outFilePath, outSignature, outDescription) ->  
    PluginInfo = XPLMGetPluginInfo(inPluginID)

  XPLMScenery::

    XPLMProbeTerrainXYZ(inProbe, inX, inY, inZ, outInfo) ->  
    ProbeInfo = XPLMProbeTerrainXYZ(inProbe, inX, inY, inZ)  

  XPWidgets::

    XPGetWidgetGeometry(inWidget, outLeft, outTop, outRight, outBottom) ->  
    left, top, right, bottom = XPGetWidgetGeometry(inWidget)  
  
    XPGetWidgetExposedGeometry(inWidgetID, outLeft, outTop, outRight, outBottom) ->  
    left, top, right, bottom = XPGetWidgetExposedGeometry(inWidgetID)  
  
    XPGetWidgetDescriptor(inWidget, outDescriptor, inMaxDescLength) ->  
    descriptor = XPGetWidgetDescriptor(inWidget)


New XPPython3 Constants
-----------------------

:data:`xp.VERSION`
      Version number of XPPython3  

:data:`xp.PLUGINSPATH`
      Full path to where XPPython3 Plugins are installed: <XP>/Resources/plugins/PythonPlugins  

:data:`xp.INTERNALPLUGINSPATH`
      Full path to where XPPython3 Internal Plugins are installed: <XP>/Resources/XPPython3  

New XPPython3 Functions
-----------------------

:py:func:`xp.pythonGetDicts()`
  Returns dictionary of internal plugin python dictionaries. The Plugin already stores lists of
  items registered by each (XPython3) plugin. You may be able to use these, read-only,
  rather than maintaining your own list of things you've registered.
    
  For now, this internal dictionaries are partially documented in :doc:`xppythondicts`, though you can probably divine their use.

:py:func:`xp.pythonGetCapsules()`
  Returns a dictionary of internal plugin capsules (essentially these are registered WidgetIDs)
  Not sure they'll be of any real value to SDK users.

