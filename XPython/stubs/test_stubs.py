from typing import Any, Dict, List, Union, Tuple
import xp
from xp_hinting import XPLMWidgetID, XPWindowID, XPLMCommandRef, XPLMMenuID, XPLMProbeRef, XPLMObjectRef, XPLMNavRef, XPLMMapLayerID, XPLMMapProjectionID
a = xp.createWidget(0, 0, 0, 0, 0, "foobar", 0, 0, 0)
b = xp.createWidget(0, 0, 0, 0, 0, "foobar", 0, a, 0)
xp.destroyWidget(b)
left, top, right, bottom, visible, isRoot = (0, 0, 0, 0, 0, 0)
destroyChildren = 0
param1, param2 = None, None
descriptor = 'abc'
container = XPLMWidgetID(0)
widgetClass = 0
widgetID = XPLMWidgetID(0)
index = 0
message = 0
dispatchMode = 0
xOffset, yOffset = 0, 0
value = None
exists = 1
propertyID = 0
recursive = 0
visibleOnly = 1
inWidgetClass = 0
title = "Title"
path = "/foo/bar"
signature = "foo.bar"
enabled = 1

def widgetCallback(a: int, widgetID: XPLMWidgetID, param1: Any, param2: Any) -> int:
    return int()


# XPWidgets
xp.createWidget(left, top, right, bottom, visible, descriptor, isRoot, container, widgetClass)
xp.createCustomWidget(left, top, right, bottom, visible, descriptor, isRoot, container, widgetCallback)
xp.destroyWidget(widgetID, destroyChildren)
xp.sendMessageToWidget(widgetID, message, dispatchMode, param1, param2)
xp.placeWidgetWithin(widgetID, container)
xp.countChildWidgets(widgetID)
xp.getNthChildWidget(widgetID, index) 
xp.getParentWidget(widgetID)
xp.showWidget(widgetID)
xp.hideWidget(widgetID)
xp.isWidgetVisible(widgetID) 
xp.findRootWidget(widgetID) 
xp.bringRootWidgetToFront(widgetID) 
xp.isWidgetInFront(widgetID) 
xp.getWidgetGeometry(widgetID) 
xp.setWidgetGeometry(widgetID, left, top, right, bottom) 
xp.getWidgetForLocation(widgetID, xOffset, yOffset, recursive, visibleOnly) 
xp.getWidgetExposedGeometry(widgetID) 
xp.setWidgetDescriptor(widgetID, descriptor) 
xp.getWidgetDescriptor(widgetID) 
xp.getWidgetUnderlyingWindow(widgetID) 
xp.setWidgetProperty(widgetID, propertyID, value) 
xp.getWidgetProperty(widgetID, propertyID, -1)
xp.setKeyboardFocus(widgetID) 
xp.loseKeyboardFocus(widgetID) 
xp.getWidgetWithFocus() 
xp.addWidgetCallback(widgetID, widgetCallback)
xp.getWidgetClassFunc(inWidgetClass)

# XPLMDataAccess
name = "abc"
dataRef = 0
dataType = 0
writable = 1
refCon: Dict[Any, Any] = {}
offset = 0
count = 1
dataAccessor = 1
xp.unshareData("hello", 1)
xp.findDataRef(name)
xp.getDataRefTypes(dataRef)
xp.getDatai(dataRef)
xp.setDatai(dataRef, 0)
xp.getDataf(dataRef)
xp.setDataf(dataRef, 0.0)
xp.getDatas(dataRef, offset, count)
xp.setDatas(dataRef, descriptor, offset, count)
xp.getDatad(dataRef)
xp.setDatad(dataRef, 0.0)
xp.getDatavi(dataRef, [0, 1], offset, count)
xp.setDatavi(dataRef, [0, 1], offset, count)
xp.getDatavf(dataRef, [0.0, 1.0], offset, count)
xp.setDatavf(dataRef, [0.0, 1.0], offset, count)
xp.getDatab(dataRef, [0, 1], offset, count)
xp.setDatab(dataRef, [0, 1], offset, count)

def getDatai_f(refCon: Any) -> int:
    return int()

def setDatai_f(refCon: Any, value: int = 0) -> None:
    return

def getDataf_f(refCon: Any) -> float:
    return float()

def setDataf_f(refCon: Any, value: float = 0.0) -> None:
    return

def getDatad_f(refCon: Any) -> float:
    return float()

def setDatad_f(refCon: Any, value: float = 0.0) -> None:
    return

def getDatavi_f(refCon: Any, value: Union[None, List[int, ]] = None, offset: int = 0, count: int = 0) -> int:
    return int()

def setDatavi_f(refCon: Any, value: Union[None, list[int, ]] = None, offset: int = 0, count: int = 0) -> None:
    return

def getDatavf_f(refCon: Any, value: Union[None, List[float, ]] = None, offset: int = 0, count: int = 0) -> int:
    return int()

def setDatavf_f(refCon: Any, value: Union[None, List[float, ]], offset: int = 0, count: int = 0) -> None:
    return

def getDatab_f(refCon: Any, value: Union[None, List[int, ]], offset: int = 0, count: int = 0) -> int:
    return int()

def setDatab_f(refCon: Any, value: Union[None, List[int, ]], offset: int = 0, count: int = 0) -> None:
    return

xp.registerDataAccessor(name, dataType, writable,
                        getDatai_f,
                        setDatai_f,
                        getDataf_f,
                        setDataf_f,
                        getDatad_f,
                        setDatad_f,
                        getDatavi_f,
                        setDatavi_f,
                        getDatavf_f,
                        setDatavf_f,
                        getDatab_f,
                        setDatab_f,
                        refCon, refCon)
xp.unregisterDataAccessor(dataAccessor)


def dataChanged_f(refCon: Any) -> None:
    return


xp.shareData(name, dataType, dataChanged_f, refCon)
xp.unshareData(name, dataType, dataChanged_f, refCon)

# XPLMCamera
howLong = 0


def cameraControl_f(outCameraPosition: List[int, ], inIsLosingControl: bool, refCon: Any) -> int:
    return int()


xp.controlCamera(howLong, cameraControl_f, refCon)
xp.dontControlCamera()
xp.isCameraBeingControlled()
xp.readCameraPosition()

# XPLMDisplay
windowID = XPWindowID(0)


def drawCallback(phase:int, after: int, refCon: Any) -> int:
    return int()


xp.registerDrawCallback(drawCallback, 0, 0, refCon)
xp.unregisterDrawCallback(drawCallback, 0, 0, refCon)


def drawWindowCallback(windowID: XPWindowID, refCon: Any) -> None:
    return


def clickWindowCallback(windowID: XPWindowID, x: int, y: int, status: int, refCon: Any) -> int:
    return int()


def keyWindowCallback(windowID: XPWindowID, key: int, flags: int, vKey: int, refCon: Any, losingFocus: int) -> None:
    return


def cursorWindowCallback(windowID: XPWindowID, x: int, y: int, refCon: Any) -> int:
    return int()


def wheelWindowCallback(windowID: XPWindowID, x: int, y: int, wheel: int, clicks: int, refCon: Any) -> int:
    return int()


def rightWindowCallback(windowID: XPWindowID, x: int, y: int, status: int, refCon: Any) -> int:
    return int()


xp.createWindowEx(100, 200, 200, 100, 0,
                  drawWindowCallback,
                  clickWindowCallback,
                  keyWindowCallback,
                  cursorWindowCallback,
                  wheelWindowCallback,
                  refCon,
                  1,
                  1,
                  rightWindowCallback)
xp.destroyWindow(windowID)
xp.getScreenSize()
xp.getScreenBoundsGlobal()


def bounds(index: int, left: int, top: int, right: int, bottom: int, refCon: Any) -> None:
    return


xp.getAllMonitorBoundsGlobal(bounds, refCon)
xp.getAllMonitorBoundsOS(bounds, refCon)
xp.getMouseLocationGlobal()
xp.getWindowGeometry(windowID)
xp.setWindowGeometry(windowID, 0, 0, 10, 10)
xp.getWindowGeometryOS(windowID)
xp.setWindowGeometryOS(windowID, 0, 0, 10, 10)
xp.getWindowGeometryVR(windowID)
xp.setWindowGeometryVR(windowID, 10, 20)
xp.getWindowIsVisible(windowID)
xp.setWindowIsVisible(windowID, 1)
xp.windowIsPoppedOut(windowID)
xp.windowIsInVR(windowID)
xp.setWindowGravity(windowID, 0.0, 0.0, 1.0, 1.0)
xp.setWindowResizingLimits(windowID, 0, 0, 100, 100)
xp.setWindowPositioningMode(windowID, 1, -1)
xp.setWindowTitle(windowID, "foobar")
xp.getWindowRefCon(windowID)
xp.setWindowRefCon(windowID, refCon)
xp.takeKeyboardFocus(windowID)
xp.hasKeyboardFocus(windowID)
xp.bringWindowToFront(windowID)
xp.isWindowInFront(windowID)


def sniffer(key: int, flags: int, vKey: int, refCon: Any) -> int:
    return int()


xp.registerKeySniffer(sniffer, 0, refCon)
xp.unregisterKeySniffer(sniffer, 0, refCon)


def hotKey(refCon: Any) -> None:
    return


xp.registerHotKey(1, 0, "test", hotKey, refCon)
xp.unregisterHotKey(1)
xp.countHotKeys()
xp.getNthHotKey(1)


xp.getHotKeyInfo(1)
xp.setHotKeyCombination(1, 2, 0)

# XPLMGraphics
x, y, z = 0.0, 1.0, 2.0
color = (1.0, 1.0, 1.0)
xp.setGraphicsState(0, 0, 0, 0, 0, 0)
xp.bindTexture2d(0, 0)
xp.generateTextureNumbers(0)
xp.worldToLocal(x, y, z)
xp.localToWorld(x, y, z)
xp.drawTranslucentDarkBox(left, top, right, bottom)
xp.drawString(color, int(x), int(y), title, 1, 1)
xp.drawNumber(color, int(x), int(y), 1.0, 1, 2, 1, 18)
xp.getFontDimensions(18)
xp.measureString(18, descriptor)

#XPLMPlugins
feature = "MY FEATURE"
xp.getMyID()
xp.countPlugins()
xp.getNthPlugin(1)
xp.findPluginByPath(path)
xp.findPluginBySignature(signature)
xp.getPluginInfo(1)
xp.isPluginEnabled(1)
xp.enablePlugin(1)
xp.disablePlugin(1)
xp.reloadPlugins()
xp.sendMessageToPlugin(1, 2, param1)
xp.hasFeature(feature)
xp.isFeatureEnabled(feature)
xp.enableFeature(feature, 1)


def XPLMFeatureEnumerator_f(name: str, refCon: Any) -> None:
    return


xp.enumerateFeatures(XPLMFeatureEnumerator_f, refCon)

# XPLMUtilities
xp.speakString(descriptor)
xp.getVirtualKeyDescription(1)
xp.reloadScenery()
xp.getSystemPath()
xp.getPrefsPath()
xp.getDirectorySeparator()
xp.extractFileAndPath(path)
xp.getDirectoryContents(path, 0, 100, 100)
xp.getVersions()
xp.getLanguage()
xp.debugString(descriptor)


def errorCallback(message: str) -> None:
    return


xp.setErrorCallback(errorCallback)
xp.findSymbol(title)
xp.loadDataFile(1, path)
xp.saveDataFile(1, path)
xp.findCommand(path)

commandRef = XPLMCommandRef(0)

xp.commandBegin(commandRef)
xp.commandEnd(commandRef)
xp.commandOnce(commandRef)
xp.createCommand(name, descriptor)


def commandCallback(commandRef: XPLMCommandRef, phase: int, refCon: Any) -> int:
    return int()


xp.registerCommandHandler(commandRef, commandCallback, 1, refCon)
xp.unregisterCommandHandler(commandRef, commandCallback, 1, refCon)

#XPPython
xp.pythonGetDicts()
xp.pythonGetCapsules()
xp.getPluginStats()
xp.systemLog(descriptor)
xp.pythonLog(descriptor)
xp.log(descriptor)
xp.log()

# XPLMMenu
menuID = XPLMMenuID(0)
xp.findPluginsMenu()
xp.findAircraftMenu()


def menuHandler(menuRefCon: Any, itemRefCon: Any) -> None:
    return


xp.createMenu(name, menuID, 0, menuHandler, refCon)
xp.destroyMenu(menuID)
xp.clearAllMenuItems(menuID)
xp.appendMenuItem(menuID, name, refCon)
xp.appendMenuItemWithCommand(menuID, name, commandRef)
xp.appendMenuSeparator(menuID)
xp.setMenuItemName(menuID, index, name)
xp.checkMenuItem(menuID, index, 2)
xp.checkMenuItemState(menuID, index)
xp.enableMenuItem(menuID, index, enabled)
xp.removeMenuItem(menuID, index)

# XPLMScenery
probeRef = XPLMProbeRef(0)
xp.createProbe(0)
xp.destroyProbe(probeRef)
xp.probeTerrainXYZ(probeRef, x, y, z)
xp.getMagneticVariation(0.0, 0.0)
xp.degTrueToDegMagnetic(0.0)
xp.degMagneticToDegTrue(0.0)
xp.loadObject(path)


def loadedCallback(objectRef: XPLMObjectRef, refCon: Any) -> None:
    return

objectRef = XPLMObjectRef(0)
xp.loadObjectAsync(path, loadedCallback, refCon)
xp.unloadObject(objectRef)


def objectEnumerator(path: str, refCon: Any) -> None:
    return


xp.lookupObjects(path, 0.0, 0.0, objectEnumerator, refCon)

# XPLMNavigation
lat, lon, freq = 0.0, 0.0, 0
xp.getFirstNavAid()
navRef = XPLMNavRef(0)
xp.getNextNavAid(navRef)
xp.findFirstNavAidOfType(0)
xp.findLastNavAidOfType(0)
xp.findNavAid(name, "type", lat, lon, freq, 1)
xp.getNavAidInfo(navRef)
xp.countFMSEntries()
xp.getDisplayedFMSEntry()
xp.getDestinationFMSEntry()
xp.setDisplayedFMSEntry(index)
xp.setDestinationFMSEntry(index)
xp.getFMSEntryInfo(index)
xp.setFMSEntryInfo(index, navRef, 0)
xp.setFMSEntryLatLon(index, lat, lon, 0)
xp.clearFMSEntry(index)
xp.getGPSDestinationType()
xp.getGPSDestination()

# XPLMMap
MAP_USER_INTERFACE = "XPLM_MAP_USER_INTERFACE"
MapLayer_Markings = 1


def deleteLayer(layerID: XPLMMapLayerID, refCon: Any) -> None:
    return


def prepLayer(layerID: XPLMMapLayerID, bounds: Tuple[float, float, float, float], projection: XPLMMapProjectionID,
              refCon: Any) -> None:
    return


def drawLayer(layerID: XPLMMapLayerID, bounds: Tuple[float, float, float, float], zoom: float, mapUnits: float,
              mapStyle: int, projection: XPLMMapProjectionID, refCon: Any) -> None:
    return


def iconLayer(layerID: XPLMMapLayerID, bounds: Tuple[float, float, float, float], zoom: float, mapUnits: float,
              mapStyle: int, projection: XPLMMapProjectionID, refCon: Any) -> None:
    return


def labelLayer(layerID: XPLMMapLayerID, bounds: Tuple[float, float, float, float], zoom: float, mapUnits: float,
               mapStyle: int, projection: XPLMMapProjectionID, refCon: Any) -> None:
    return


layerID = XPLMMapLayerID(0)
projectionID = XPLMMapProjectionID(0)
xp.createMapLayer(MAP_USER_INTERFACE, MapLayer_Markings,
                  deleteLayer, prepLayer, drawLayer, iconLayer, labelLayer,
                  1, name, refCon)
xp.destroyMapLayer(layerID)


def mapCreated(name: str, refCon: Any) -> None:
    return


xp.registerMapCreationHook(mapCreated, refCon)
xp.mapExists(MAP_USER_INTERFACE)
xp.drawMapIconFromSheet(layerID, path, 1, 2, 3, 4, x, y, 1, 1.0, 1.0)
xp.drawMapLabel(layerID, title, x, y, 1, 1.0)
xp.mapProject(projectionID, lat, lon)
xp.mapUnproject(projectionID, x, y)
xp.mapScaleMeter(projectionID, x, y)
xp.mapGetNorthHeading(projectionID, x, y)

# XPUIGraphics
xp.drawWindow(left, bottom, right, top, 1)
xp.getWindowDefaultDimensions(1)
xp.drawElement(left, bottom, right, top, 1, 0)
xp.getElementDefaultDimensions(1)
xp.drawTrack(left, bottom, right, top, 0, 100, 59, 1, 0)
xp.getTrackDefaultDimensions(1)
xp.getTrackMetrics(left, bottom, right, top, 0, 100, 50, 1)

# XPWidgetUtils
xp.moveWidgetBy(widgetID, 0, 0)
xp.fixedLayout(0, widgetID, param1, param2)
xp.selectIfNeeded(0, widgetID, param1, param2, 1)
xp.defocusKeyboard(0, widgetID, param1, param2, 1)
xp.dragWidget(0, widgetID, param1, param2, left, top, right, bottom)

# XPLMProcessing
xp.getElapsedTime()
xp.getCycleNumber()


def flCallback(since: float, elapsed: float, counter: int, refCon: Any) -> float:
    return float()


xp.registerFlightLoopCallback(flCallback, 0.0, refCon)
xp.unregisterFlightLoopCallback(flCallback, refCon)
xp.setFlightLoopCallbackInterval(flCallback, 0.0, 1, refCon)
flightLoopID = xp.createFlightLoop(flCallback, 1, refCon)
xp.destroyFlightLoop(flightLoopID)
xp.scheduleFlightLoop(flightLoopID, 0.0, 1)

# XPLMPlanes
xp.setUsersAircraft(path)
xp.placeUserAtAirport(name)
xp.placeUserAtLocation(lat, lon, 0.0, 0.0)
xp.countAircraft()
xp.getNthAircraftModel(1)


def getAircraft(refCon: Any) -> None:
    return


xp.acquirePlanes(callback=getAircraft, refCon=refCon)
xp.releasePlanes()
xp.setActiveAircraftCount(count)
xp.setAircraftModel(index, path)
xp.disableAIForPlane(index)

# XPLMInstance
instance = xp.createInstance(objectRef, ['dataRef1', 'dataRef2'])
xp.destroyInstance(instance)
xp.instanceSetPosition(instance, (0.0, 0.0, 0.0, 0.0, 0.0, 0.0), [0.0, 0.0])
