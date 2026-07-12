#ifndef PLUGIN_DL__H
#define PLUGIN_DL__H

#include <sys/time.h>
#include <stdbool.h>
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMDataAccess.h>
#include <XPLM/XPLMGraphics.h>
#include <XPLM/XPLMProcessing.h>
#include <XPLM/XPLMScenery.h>
#include <XPLM/XPLMMenus.h>
#include <XPLM/XPLMInstance.h>
#include <XPLM/XPLMMap.h>
#include <XPLM/XPLMDisplay.h>
#include <XPLM/XPLMPanelGraphics.h>
#include <XPLM/XPLMPlugin.h>
#include <XPLM/XPLMPlanes.h>
#include <XPLM/XPLMWeather.h>
#include <XPLM/XPLMNavigation.h>
#include <XPLM/XPLMSound.h>
#include <Widgets/XPWidgetDefs.h>
#include <Widgets/XPWidgets.h>

// Dynamicaly bound function pointers
// XPLM210
extern typeof(XPLMCreateFlightLoop) *XPLMCreateFlightLoop_ptr;
extern typeof(XPLMDestroyFlightLoop) *XPLMDestroyFlightLoop_ptr;
extern typeof(XPLMLoadObjectAsync) *XPLMLoadObjectAsync_ptr;
extern typeof(XPLMRemoveMenuItem) *XPLMRemoveMenuItem_ptr;
extern typeof(XPLMScheduleFlightLoop) *XPLMScheduleFlightLoop_ptr;

//XPLM300
extern typeof(XPLMAppendMenuItemWithCommand) *XPLMAppendMenuItemWithCommand_ptr;
extern typeof(XPLMCreateMapLayer) *XPLMCreateMapLayer_ptr;
extern typeof(XPLMDegMagneticToDegTrue) *XPLMDegMagneticToDegTrue_ptr;
extern typeof(XPLMDegTrueToDegMagnetic) *XPLMDegTrueToDegMagnetic_ptr;
extern typeof(XPLMDestroyMapLayer) *XPLMDestroyMapLayer_ptr;
extern typeof(XPLMDrawMapIconFromSheet) *XPLMDrawMapIconFromSheet_ptr;
extern typeof(XPLMDrawMapLabel) *XPLMDrawMapLabel_ptr;
extern typeof(XPLMFindAircraftMenu) *XPLMFindAircraftMenu_ptr;
extern typeof(XPLMGetAllMonitorBoundsGlobal) *XPLMGetAllMonitorBoundsGlobal_ptr;
extern typeof(XPLMGetAllMonitorBoundsOS) *XPLMGetAllMonitorBoundsOS_ptr;
extern typeof(XPLMGetMagneticVariation) *XPLMGetMagneticVariation_ptr;
extern typeof(XPLMGetMouseLocationGlobal) *XPLMGetMouseLocationGlobal_ptr;
extern typeof(XPLMGetScreenBoundsGlobal) *XPLMGetScreenBoundsGlobal_ptr;
extern typeof(XPLMGetWindowGeometryOS) *XPLMGetWindowGeometryOS_ptr;
extern typeof(XPLMMapExists) *XPLMMapExists_ptr;
extern typeof(XPLMMapGetNorthHeading) *XPLMMapGetNorthHeading_ptr;
extern typeof(XPLMMapProject) *XPLMMapProject_ptr;
extern typeof(XPLMMapScaleMeter) *XPLMMapScaleMeter_ptr;
extern typeof(XPLMMapUnproject) *XPLMMapUnproject_ptr;
extern typeof(XPLMPlaceUserAtLocation) *XPLMPlaceUserAtLocation_ptr;
extern typeof(XPLMRegisterMapCreationHook) *XPLMRegisterMapCreationHook_ptr;
extern typeof(XPLMSetWindowGeometryOS) *XPLMSetWindowGeometryOS_ptr;
extern typeof(XPLMSetWindowGravity) *XPLMSetWindowGravity_ptr;
extern typeof(XPLMSetWindowPositioningMode) *XPLMSetWindowPositioningMode_ptr;
extern typeof(XPLMSetWindowResizingLimits) *XPLMSetWindowResizingLimits_ptr;
extern typeof(XPLMSetWindowTitle) *XPLMSetWindowTitle_ptr;
extern typeof(XPLMWindowIsPoppedOut) *XPLMWindowIsPoppedOut_ptr;
extern typeof(XPLMHasKeyboardFocus) *XPLMHasKeyboardFocus_ptr;
extern typeof(XPLMCreateInstance) *XPLMCreateInstance_ptr;
extern typeof(XPLMDestroyInstance) *XPLMDestroyInstance_ptr;
extern typeof(XPLMInstanceSetPosition) *XPLMInstanceSetPosition_ptr;
//XPLM_301
extern typeof(XPGetWidgetUnderlyingWindow) *XPGetWidgetUnderlyingWindow_ptr;
extern typeof(XPLMGetWindowGeometryVR) *XPLMGetWindowGeometryVR_ptr;
extern typeof(XPLMSetWindowGeometryVR) *XPLMSetWindowGeometryVR_ptr;
extern typeof(XPLMWindowIsInVR) *XPLMWindowIsInVR_ptr;

//XPLM_400
extern typeof(XPLMRegisterAvionicsCallbacksEx) *XPLMRegisterAvionicsCallbacksEx_ptr;
extern typeof(XPLMUnregisterAvionicsCallbacks) *XPLMUnregisterAvionicsCallbacks_ptr;
extern typeof(XPLMCountDataRefs) *XPLMCountDataRefs_ptr;
extern typeof(XPLMGetDataRefsByIndex) *XPLMGetDataRefsByIndex_ptr;
extern typeof(XPLMGetDataRefInfo) *XPLMGetDataRefInfo_ptr;
extern typeof(XPLMGetMETARForAirport) *XPLMGetMETARForAirport_ptr;
extern typeof(XPLMGetWeatherAtLocation) *XPLMGetWeatherAtLocation_ptr;


#define PLUGIN_DLL(x) extern typeof(x) *x##_ptr
//XPLM_410
PLUGIN_DLL(XPLMGetAvionicsHandle);
PLUGIN_DLL(XPLMIsAvionicsBound);
PLUGIN_DLL(XPLMIsCursorOverAvionics);
PLUGIN_DLL(XPLMIsAvionicsPopupVisible);
PLUGIN_DLL(XPLMIsAvionicsPoppedOut);
PLUGIN_DLL(XPLMHasAvionicsKeyboardFocus);
PLUGIN_DLL(XPLMAvionicsNeedsDrawing);
PLUGIN_DLL(XPLMSetAvionicsPopupVisible);
PLUGIN_DLL(XPLMPopOutAvionics);
PLUGIN_DLL(XPLMTakeAvionicsKeyboardFocus);
PLUGIN_DLL(XPLMDestroyAvionics);
PLUGIN_DLL(XPLMGetAvionicsGeometry);
PLUGIN_DLL(XPLMSetAvionicsGeometry);
PLUGIN_DLL(XPLMGetAvionicsGeometryOS);
PLUGIN_DLL(XPLMSetAvionicsGeometryOS);
PLUGIN_DLL(XPLMGetAvionicsBrightnessRheo);
PLUGIN_DLL(XPLMSetAvionicsBrightnessRheo);
PLUGIN_DLL(XPLMGetAvionicsBusVoltsRatio);
PLUGIN_DLL(XPLMCreateAvionicsEx);

PLUGIN_DLL(XPLMCountFMSFlightPlanEntries);
PLUGIN_DLL(XPLMGetDisplayedFMSFlightPlanEntry);
PLUGIN_DLL(XPLMGetDestinationFMSFlightPlanEntry);
PLUGIN_DLL(XPLMSetDisplayedFMSFlightPlanEntry);
PLUGIN_DLL(XPLMSetDestinationFMSFlightPlanEntry);
PLUGIN_DLL(XPLMSetDirectToFMSFlightPlanEntry);
PLUGIN_DLL(XPLMGetFMSFlightPlanEntryInfo);
PLUGIN_DLL(XPLMSetFMSFlightPlanEntryInfo);
PLUGIN_DLL(XPLMSetFMSFlightPlanEntryLatLon);
PLUGIN_DLL(XPLMSetFMSFlightPlanEntryLatLonWithId);
PLUGIN_DLL(XPLMClearFMSFlightPlanEntry);
PLUGIN_DLL(XPLMLoadFMSFlightPlan);
PLUGIN_DLL(XPLMSaveFMSFlightPlan);

#if defined(_FMOD_COMMON_H)
PLUGIN_DLL(XPLMGetFMODStudio);
PLUGIN_DLL(XPLMGetFMODChannelGroup);
#endif
PLUGIN_DLL(XPLMPlayPCMOnBus);
PLUGIN_DLL(XPLMStopAudio);
PLUGIN_DLL(XPLMSetAudioPosition);
PLUGIN_DLL(XPLMSetAudioFadeDistance);
PLUGIN_DLL(XPLMSetAudioPitch);
PLUGIN_DLL(XPLMSetAudioVolume);
PLUGIN_DLL(XPLMSetAudioCone);

// XPLM_420
PLUGIN_DLL(XPLMInstanceSetPositionDouble);
PLUGIN_DLL(XPLMInstanceSetAutoShift);
PLUGIN_DLL(XPLMBeginWeatherUpdate);
PLUGIN_DLL(XPLMEndWeatherUpdate);
PLUGIN_DLL(XPLMSetWeatherAtLocation);
PLUGIN_DLL(XPLMSetWeatherAtAirport);
PLUGIN_DLL(XPLMEraseWeatherAtLocation);
PLUGIN_DLL(XPLMEraseWeatherAtAirport);

// XPLM_430
PLUGIN_DLL(XPLMInitFlight);
PLUGIN_DLL(XPLMUpdateFlight);

// XPLM_440
PLUGIN_DLL(XPLMReturnString);
PLUGIN_DLL(XPLMWindowSetURL);
PLUGIN_DLL(XPLMWindowRefresh);
PLUGIN_DLL(XPLMWindowInjectScript);
PLUGIN_DLL(XPLMWindowAddBrowserFunction);
PLUGIN_DLL(XPLMIsAvionicsMappedToVR);
PLUGIN_DLL(XPLMSetAvionicsMappedToVR);
PLUGIN_DLL(XPLMMakeColor);
PLUGIN_DLL(XPLMLines);
PLUGIN_DLL(XPLMLinesWithWidth);
PLUGIN_DLL(XPLMLinesStipple);
PLUGIN_DLL(XPLMLinesc);
PLUGIN_DLL(XPLMLinescWithWidth);
PLUGIN_DLL(XPLMLineStrip);
PLUGIN_DLL(XPLMLineStripWithWidth);
PLUGIN_DLL(XPLMLineStripc);
PLUGIN_DLL(XPLMLineStripcWithWidth);
PLUGIN_DLL(XPLMLineStripStipple);
PLUGIN_DLL(XPLMLineLoop);
PLUGIN_DLL(XPLMLineLoopWithWidth);
PLUGIN_DLL(XPLMLineLoopc);
PLUGIN_DLL(XPLMLineLoopcWithWidth);
PLUGIN_DLL(XPLMLineLoopStipple);
PLUGIN_DLL(XPLMPolygon);
PLUGIN_DLL(XPLMPolygonWithWidth);
PLUGIN_DLL(XPLMPolygonc);
PLUGIN_DLL(XPLMPolygoncWithWidth);
PLUGIN_DLL(XPLMQuadstrip);
PLUGIN_DLL(XPLMQuadstripWithWidth);
PLUGIN_DLL(XPLMQuadstripc);
PLUGIN_DLL(XPLMQuadstripcWithWidth);
PLUGIN_DLL(XPLMTransformPush);
PLUGIN_DLL(XPLMTransformPop);
PLUGIN_DLL(XPLMTransformTranslate);
PLUGIN_DLL(XPLMTransformRotate);
PLUGIN_DLL(XPLMTransformScale);
PLUGIN_DLL(XPLMScissorPush);
PLUGIN_DLL(XPLMScissorPop);
PLUGIN_DLL(XPLMScissorSet);
PLUGIN_DLL(XPLMScissorShrink);
PLUGIN_DLL(XPLMBeginSetupStencilMask);
PLUGIN_DLL(XPLMEndSetupStencilMask);
PLUGIN_DLL(XPLMUseStencilMask);
PLUGIN_DLL(XPLMClearStencilMask);
PLUGIN_DLL(XPLMCreateFont);
PLUGIN_DLL(XPLMDestroyFont);
PLUGIN_DLL(XPLMFontAddFace);
PLUGIN_DLL(XPLMFontGetMetrics);
PLUGIN_DLL(XPLMFontMeasureString);
PLUGIN_DLL(XPLMFontGetLineCount);
PLUGIN_DLL(XPLMFontFitForward);
PLUGIN_DLL(XPLMFontFitReverse);
PLUGIN_DLL(XPLMFontDrawString);
PLUGIN_DLL(XPLMFontDrawStringFixedSpacing);
PLUGIN_DLL(XPLMFontDrawStringWordWrapped);
PLUGIN_DLL(XPLMFontDrawStringRotated);
PLUGIN_DLL(XPLMCreateTextureAtlas);
PLUGIN_DLL(XPLMDestroyTextureAtlas);
PLUGIN_DLL(XPLMTextureAtlasAddImageFile);
PLUGIN_DLL(XPLMTextureAtlasAddImageFileSet);
PLUGIN_DLL(XPLMTextureAtlasAddImage);
PLUGIN_DLL(XPLMTextureAtlasAddImageSet);
PLUGIN_DLL(XPLMTextureAtlasBake);
PLUGIN_DLL(XPLMTextureAtlasGetImageWidth);
PLUGIN_DLL(XPLMTextureAtlasGetImageHeight);
PLUGIN_DLL(XPLMTextureAtlasGetImageUVMap);
PLUGIN_DLL(XPLMTextureAtlasDrawAt);
PLUGIN_DLL(XPLMTextureAtlasDrawIn);
PLUGIN_DLL(XPLMTextureAtlasDrawStretched);
PLUGIN_DLL(XPLMTextureAtlasDrawScaled);
PLUGIN_DLL(XPLMTextureAtlasDrawMesh);
PLUGIN_DLL(XPLMTextureSourceDrawIn);
PLUGIN_DLL(XPLMTextureSourceDrawMesh);
PLUGIN_DLL(XPLMBeginRetainedDrawing);
PLUGIN_DLL(XPLMEndRetainedDrawing);
PLUGIN_DLL(XPLMDrawRetained);
PLUGIN_DLL(XPLMDestroyRetainedDrawing);
PLUGIN_DLL(XPLMCreateSVTDisplay);
PLUGIN_DLL(XPLMDestroySVTDisplay);
PLUGIN_DLL(XPLMSVTDisplayDrawIn);
PLUGIN_DLL(XPLMCreateMapDisplay);
PLUGIN_DLL(XPLMDestroyMapDisplay);
PLUGIN_DLL(XPLMMapDisplayDrawIn);
PLUGIN_DLL(XPLMCreateTexture);
PLUGIN_DLL(XPLMDestroyTexture);
PLUGIN_DLL(XPLMDrawCalls);
PLUGIN_DLL(XPLMAccumulateTouchZone);
PLUGIN_DLL(XPLMAvionicsSetTouchEventHandler);
PLUGIN_DLL(XPLMWindowSetTouchEventHandler);
PLUGIN_DLL(XPLMReloadThisPlugin);

bool loadSDKFunctions(void);

#endif
