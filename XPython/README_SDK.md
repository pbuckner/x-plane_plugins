# XPPython3 Plugin Development
See also [XPPython3 Plugin README INSTALL](Resources/plugins/XPPython3/README_INSTALL.md).

To develop python3 plugins, you'll need to have the XPPython3 plugin installed first (and python3, of course).

Additionally, you may find these resources helpful (though not required):

### Python Stubs [Stubs.zip](https://github.com/pbuckner/x-plane_plugins/raw/master/XPython/stubs.zip)
Because the XPLM* modules are contained within a shared library, they are not useful for support tools such as pylint. For this reason, we've included a set of stubs. Include the stub directory as part of your python path (during development) and pylint will be useful.  

For example, include this in `~/.pylintrc`:

    [MASTER]                                                                                    
    init-hook="import sys;sys.path.extend(['.', '/path_to_stubs/PythonStubs'])"

### Examples [Examples.zip](https://github.com/pbuckner/x-plane_plugins/raw/master/XPython/examples.zip) 
Python code exercising each interface is in [XPython/examples](XPython/examples) directory, organized primarily one example file per module. You can copy them into your Resources/plugins/PythonPlugins directory to have them executed by X-Plane.



Plugin Development
------------------
Python coding errors / Exceptions will appear in XPPython3.Log. Error in some calls
to XP interface don't cause exceptions but rather just terminate the plugin's method
(e.g., terminate the flightloop) with no further message.

If you write bad python code, or use Python2 rather than Python3 syntax, you'll see the exception
in XPPython3.log.

Note we set feature NativePaths ON (i.e.,  XPLMEnableFeature("XPLM_USE_NATIVE_PATHS", 1))
Python2 plugin allowed you to be selected. XPPython3 doesn't. You should be using
native paths rather than legacy paths. (You can use python to convert from one
to the other if it becomes necessary.)

Same with NativeWidgetWindows, "XPLM_USE_NATIVE_WIDGET_WINDOWS", we enable it at startup
and disallow plugins disabling it.

## CHANGES from Python2 SDK
* 303 is base version (X-Plane 11.50). There is no attempt to implement deprecated API
  or test on earlier versions
* XPLM_USE_NATIVE_PATHS is automatically set on startup. We never use legacy paths,
  so XPLMGetSystemPath() will return '/' separated results. If you need legacy paths,
  use python within your plugin to convert.
* XPLM_USE_NATIVE_WIDGET_WINDOWS is automatically set on startup.
* It is a FATAL ERROR to attempt to disable either NATIVE_PATHS or NATIVE_WIDGET_WINDOWS features
* REMOVAL of passing 'self' as first parameter in callback registrations
  We're using stack manipulation to do that for you instead.

    ```XPLMLoadObjectAsync(path, callback, refcon)```
    not  
    ```XPLMLoadObjectAsync(self, path, callback, refcon)```

* XPGetWidgetWithFocus() returns None, instead of 0, if no widgets have focus

* All routines return "PyCapsules" rather than ints for XPLMWindowIDRef, XPLMHotkeyIDRef, XPLMMenuIDRef, XPLMWidgetID

* PI_GetMouseState() no longer supported: you'll now get tuple (x, y, button, delta) as param1 in widget callbacks
  (for xpMsg_MouseDown, xpMsg_MouseDrag, xpMsg_MouseUp, xpMsp_CursorAdjust)
* PI_GetKeyState() no longer supported: you'll now get tuple (key, flags, vkey) as param1 in widget callbacks
  (for xpMsg_KeyPress)
* PI_GetWidgetGeometry() no longer supported: you'll now get tuple (dx, dy, dwidth, dheight) as param1 in widget callbacks
  (for xpMsg_Reshape)

## Change "out" parameters. Instead, return values directly:
display:  
```
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
```
graphics:  
```
  XPLMGenerateTextureNumber(outTextureIDs, inCount) ->  
  outTextureIDs = XPLMGenerateTextureNumber(inCount)  
  
  XPLMGetFontDimensions(inFontID, outCharWidth, outCharHeight, outDigitsOnly) ->  
  width, height, digitsOnly = XPLMGetFontDimensions(inFontID)  
```  
map:  
```
  XPLMMapProject(projection, latitude, longitude, outX, outY) ->  
  x, y = XPLMMapProject(projection, latitude, longitude)  
  
  XPLMMapUnProject(production, mapX, mapY, outLatitude, outLongitude) ->  
  lat, lon = XPLMMapUnProject(production, mapX, mapY)
```  
uigraphics:  
```
  XPGetWindowDefaultDimensions(inStyle, outWidth, outHeight) ->  
  width, height = XPGetWindowDefaultDimensions(inStyle)  
  
  XPGetElementDefaultDimensions(inStyle, outWidth, outHeight, outCanBeLit) ->  
  width, height, canBeLit = XPGetElementDefaultDimensions(inStyle)  
  
  XPGetTrackDefaultDimensions(inStyle, outWidth, outCanBeLit) ->  
  width, canBeLit = XPGetTrackDefaultDimensions(inStyle)  
  
  XPGetTrackMetrics(inX1, inY1, inX2, inY2, inMin, inMax, inValue, inTrackStyle, outIsVertical, outDownBtnSize, outDownPageSize, outThumbSize, outUpPageSize, outUpBtnSiz) ->  
  TrackMetrics = XPGetTrackMetrics(inX1, inY1, inX2, inY2, inMin, inMax, inValue, inTrackStyle)
```  
navigation:  
```
  XPLMGetNavAidInfo(inRef, outType, outLatitude, outLogitude, outHeight, outFrequency, outHeading, outID, outName, outReg) ->  
  NavAidInfo = XPLMGetNavAidInfo(inRef)  
  
  XPLMGetFMSEntryInfo(inIndex, outType, outID, outRef, outAltitude, outLat, outLon) ->  
  FMSEntryInfo = XPLMGetFMSEntryInfo(inIndex)  
```
plugin:  
```
  XPLMGetPluginInfo(inPluginID, outName, outFilePath, outSignature, outDescription) ->  
  PluginInfo = XPLMGetPluginInfo(inPluginID)
```  
scenery:
```
  XPLMProbeTerrainXYZ(inProbe, inX, inY, inZ, outInfo) ->  
  ProbeInfo = XPLMProbeTerrainXYZ(inProbe, inX, inY, inZ)  
```  
widgets:  
```
  XPGetWidgetGeometry(inWidget, outLeft, outTop, outRight, outBottom) ->  
  left, top, right, bottom = XPGetWidgetGeometry(inWidget)  
  
  XPGetWidgetExposedGeometry(inWidgetID, outLeft, outTop, outRight, outBottom) ->  
  left, top, right, bottom = XPGetWidgetExposedGeometry(inWidgetID)  
  
  XPGetWidgetDescriptor(inWidget, outDescriptor, inMaxDescLength) ->  
  descriptor = XPGetWidgetDescriptor(inWidget)
```  

## Callbacks and Refcons
Many functions include a "reference constant" (refCon) as an input parameter.
In every case, this reference constanct can be any python object (int, float, dict,
etc.) and will be passed to your callback function. If the object is mutable
(e.g., dict, list), you'll be able to change its value within, or outside of the
callback and that new value will be maintained.

In a few cases, the reference constant MUST BE an object which remains in scope
because the same object MUST BE used during a subsequent lookup. For example,

### WORKS (the same object)
```
     self.refCon = {'a': 1}
     XPLMDisplay.XPLMRegisterDrawCallback(..., self.refCon)
     ...
     XPLMDisplay.XPLMUnregisterDrawCallback(..., self.refCon)
```
### DOES NOT WORK (equivalent, but not the same object)
```
     XPLMDisplay.XPLMRegisterDrawCallback(..., {'a': 1})
     ...
     XPLMDisplay.XPLMUnregisterDrawCallback(..., {'a': 1})
```     

If no subsequent lookup is required, then there is no requirement to have
the object stay in scope

### RefCon REQUIRES SAME OBJECT:
```
      XPLMDisplay: XPLMRegisterDrawCallback + XPLMUnregisterDrawCallback
      XPLMDisplay: XPLMRegisterKeySniffer + XPLMUnregisterKeySniffer
      XPLMProcessing: XPLMRegisterFlightLoopCallback + XPLMUnregisterFlightLoopCallback
      XPLMUtilities: XPLMRegisterCommandHandler + XPLMUnregisterCommandHandler      
```

### RefCon CAN BE ANYTHING:
```
      XPLMDataAccess: XPLMRegisterDataAccessor
      XPLMDisplay: XPLMRegisterHotKey
      XPLMDisplay: XPLMCreateWindowEX (mousewheel, mouseclick, rightclick, cursor functions)
      XPLMDisplay: XPLMSetWindowRefCon
      XPLMMap: XPLMRegisterMapCreationHook
      XPLMPlanes: XPLMPlanesAvailable
      XPLMPlanes: XPLMAcquirePlanes
      XPLMScenery: XPLMLoadObjectAsync
```

## NEW
  XPLMUtilities.XPLMPythonGetDicts()
    returns dictionary of internal plugin python dictionaries. The Plugin already stores lists of
    items registered by each (XPython3) plugin. You may be able to use these, read-only,
    rather than maintaining your own list of things you've registered.
    Documentation of these dicts are TBD.


