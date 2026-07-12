#include <Python.h>
#include <stdint.h>
#include <dlfcn.h>
#include <sys/time.h>
#include <fmod.h>
#include <fmod_studio.h>
#include <XPLM/XPLMDataAccess.h>
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMProcessing.h>
#include <XPLM/XPLMScenery.h>
#include <XPLM/XPLMMenus.h>

#include <XPLM/XPLMPlugin.h>
#include <XPLM/XPLMInstance.h>
#include <XPLM/XPLMMap.h>
#include <XPLM/XPLMDisplay.h>
#include <XPLM/XPLMPanelGraphics.h>
#include <XPLM/XPLMPlanes.h>
#include <XPLM/XPLMWeather.h>
#include <XPLM/XPLMSound.h>
#include <XPLM/XPLMNavigation.h>
#include <Widgets/XPWidgets.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "utils.h"

typedef struct{
  const char *name;
  void **fun_ptr;
} t_fcn_info;

// XPLM210 functions
typeof(XPLMCreateFlightLoop) *XPLMCreateFlightLoop_ptr = nullptr;
typeof(XPLMDestroyFlightLoop) *XPLMDestroyFlightLoop_ptr = nullptr;
typeof(XPLMLoadObjectAsync) *XPLMLoadObjectAsync_ptr = nullptr;
typeof(XPLMRemoveMenuItem) *XPLMRemoveMenuItem_ptr = nullptr;
typeof(XPLMScheduleFlightLoop) *XPLMScheduleFlightLoop_ptr = nullptr;

t_fcn_info funcs210[] = {
  {"XPLMCreateFlightLoop", (void **)&XPLMCreateFlightLoop_ptr},
  {"XPLMDestroyFlightLoop", (void **)&XPLMDestroyFlightLoop_ptr},
  {"XPLMLoadObjectAsync", (void **)&XPLMLoadObjectAsync_ptr},
  {"XPLMRemoveMenuItem", (void **)&XPLMRemoveMenuItem_ptr},
  {"XPLMScheduleFlightLoop", (void **)&XPLMScheduleFlightLoop_ptr},
  {nullptr, nullptr}
};

// XPLM300 functions
typeof(XPLMAppendMenuItemWithCommand) *XPLMAppendMenuItemWithCommand_ptr = nullptr;
typeof(XPLMCreateMapLayer) *XPLMCreateMapLayer_ptr = nullptr;
typeof(XPLMDegMagneticToDegTrue) *XPLMDegMagneticToDegTrue_ptr = nullptr;
typeof(XPLMDegTrueToDegMagnetic) *XPLMDegTrueToDegMagnetic_ptr = nullptr;
typeof(XPLMDestroyMapLayer) *XPLMDestroyMapLayer_ptr = nullptr;
typeof(XPLMDrawMapIconFromSheet) *XPLMDrawMapIconFromSheet_ptr = nullptr;
typeof(XPLMDrawMapLabel) *XPLMDrawMapLabel_ptr = nullptr;
typeof(XPLMFindAircraftMenu) *XPLMFindAircraftMenu_ptr = nullptr;
typeof(XPLMGetAllMonitorBoundsGlobal) *XPLMGetAllMonitorBoundsGlobal_ptr = nullptr;
typeof(XPLMGetAllMonitorBoundsOS) *XPLMGetAllMonitorBoundsOS_ptr = nullptr;
typeof(XPLMGetMagneticVariation) *XPLMGetMagneticVariation_ptr = nullptr;
typeof(XPLMGetMouseLocationGlobal) *XPLMGetMouseLocationGlobal_ptr = nullptr;
typeof(XPLMGetScreenBoundsGlobal) *XPLMGetScreenBoundsGlobal_ptr = nullptr;
typeof(XPLMGetWindowGeometryOS) *XPLMGetWindowGeometryOS_ptr = nullptr;
typeof(XPLMMapExists) *XPLMMapExists_ptr = nullptr;
typeof(XPLMMapGetNorthHeading) *XPLMMapGetNorthHeading_ptr = nullptr;
typeof(XPLMMapProject) *XPLMMapProject_ptr = nullptr;
typeof(XPLMMapScaleMeter) *XPLMMapScaleMeter_ptr = nullptr;
typeof(XPLMMapUnproject) *XPLMMapUnproject_ptr = nullptr;
typeof(XPLMPlaceUserAtLocation) *XPLMPlaceUserAtLocation_ptr = nullptr;
typeof(XPLMRegisterMapCreationHook) *XPLMRegisterMapCreationHook_ptr = nullptr;
typeof(XPLMSetWindowGeometryOS) *XPLMSetWindowGeometryOS_ptr = nullptr;
typeof(XPLMSetWindowGravity) *XPLMSetWindowGravity_ptr = nullptr;
typeof(XPLMSetWindowPositioningMode) *XPLMSetWindowPositioningMode_ptr = nullptr;
typeof(XPLMSetWindowResizingLimits) *XPLMSetWindowResizingLimits_ptr = nullptr;
typeof(XPLMSetWindowTitle) *XPLMSetWindowTitle_ptr = nullptr;
typeof(XPLMWindowIsPoppedOut) *XPLMWindowIsPoppedOut_ptr = nullptr;
typeof(XPLMHasKeyboardFocus) *XPLMHasKeyboardFocus_ptr = nullptr;
typeof(XPLMCreateInstance) *XPLMCreateInstance_ptr = nullptr;
typeof(XPLMDestroyInstance) *XPLMDestroyInstance_ptr = nullptr;
typeof(XPLMInstanceSetPosition) *XPLMInstanceSetPosition_ptr = nullptr;


t_fcn_info funcs300[] = {
  {"XPLMAppendMenuItemWithCommand", (void **)&XPLMAppendMenuItemWithCommand_ptr},
  {"XPLMCreateMapLayer", (void **)&XPLMCreateMapLayer_ptr},
  {"XPLMDegMagneticToDegTrue", (void **)&XPLMDegMagneticToDegTrue_ptr},
  {"XPLMDegTrueToDegMagnetic", (void **)&XPLMDegTrueToDegMagnetic_ptr},
  {"XPLMDestroyMapLayer", (void **)&XPLMDestroyMapLayer_ptr},
  {"XPLMDrawMapIconFromSheet", (void **)&XPLMDrawMapIconFromSheet_ptr},
  {"XPLMDrawMapLabel", (void **)&XPLMDrawMapLabel_ptr},
  {"XPLMFindAircraftMenu", (void **)&XPLMFindAircraftMenu_ptr},
  {"XPLMGetAllMonitorBoundsGlobal", (void **)&XPLMGetAllMonitorBoundsGlobal_ptr},
  {"XPLMGetAllMonitorBoundsOS", (void **)&XPLMGetAllMonitorBoundsOS_ptr},
  {"XPLMGetMagneticVariation", (void **)&XPLMGetMagneticVariation_ptr},
  {"XPLMGetMouseLocationGlobal", (void **)&XPLMGetMouseLocationGlobal_ptr},
  {"XPLMGetScreenBoundsGlobal", (void **)&XPLMGetScreenBoundsGlobal_ptr},
  {"XPLMGetWindowGeometryOS", (void **)&XPLMGetWindowGeometryOS_ptr},
  {"XPLMMapExists", (void **)&XPLMMapExists_ptr},
  {"XPLMMapGetNorthHeading", (void **)&XPLMMapGetNorthHeading_ptr},
  {"XPLMMapProject", (void **)&XPLMMapProject_ptr},
  {"XPLMMapScaleMeter", (void **)&XPLMMapScaleMeter_ptr},
  {"XPLMMapUnproject", (void **)&XPLMMapUnproject_ptr},
  {"XPLMPlaceUserAtLocation", (void **)&XPLMPlaceUserAtLocation_ptr},
  {"XPLMRegisterMapCreationHook", (void **)&XPLMRegisterMapCreationHook_ptr},
  {"XPLMSetWindowGeometryOS", (void **)&XPLMSetWindowGeometryOS_ptr},
  {"XPLMSetWindowGravity", (void **)&XPLMSetWindowGravity_ptr},
  {"XPLMSetWindowPositioningMode", (void **)&XPLMSetWindowPositioningMode_ptr},
  {"XPLMSetWindowResizingLimits", (void **)&XPLMSetWindowResizingLimits_ptr},
  {"XPLMSetWindowTitle", (void **)&XPLMSetWindowTitle_ptr},
  {"XPLMWindowIsPoppedOut", (void **)&XPLMWindowIsPoppedOut_ptr},
  {"XPLMHasKeyboardFocus", (void **)&XPLMHasKeyboardFocus_ptr},
  {"XPLMCreateInstance", (void **)&XPLMCreateInstance_ptr},
  {"XPLMDestroyInstance", (void **)&XPLMDestroyInstance_ptr},
  {"XPLMInstanceSetPosition", (void **)&XPLMInstanceSetPosition_ptr},
  {nullptr, nullptr}
};

//XPLM301 functions
typeof(XPGetWidgetUnderlyingWindow) *XPGetWidgetUnderlyingWindow_ptr = nullptr;
typeof(XPLMGetWindowGeometryVR) *XPLMGetWindowGeometryVR_ptr = nullptr;
typeof(XPLMSetWindowGeometryVR) *XPLMSetWindowGeometryVR_ptr = nullptr;
typeof(XPLMWindowIsInVR) *XPLMWindowIsInVR_ptr = nullptr;

t_fcn_info funcs301[] = {
  {"XPGetWidgetUnderlyingWindow", (void **)&XPGetWidgetUnderlyingWindow_ptr},
  {"XPLMGetWindowGeometryVR", (void **)&XPLMGetWindowGeometryVR_ptr},
  {"XPLMSetWindowGeometryVR", (void **)&XPLMSetWindowGeometryVR_ptr},
  {"XPLMWindowIsInVR", (void **)&XPLMWindowIsInVR_ptr},
  {nullptr, nullptr}
};

//XPLM400 functions
typeof(XPLMRegisterAvionicsCallbacksEx) *XPLMRegisterAvionicsCallbacksEx_ptr = nullptr;
typeof(XPLMUnregisterAvionicsCallbacks) *XPLMUnregisterAvionicsCallbacks_ptr = nullptr;
typeof(XPLMCountDataRefs) *XPLMCountDataRefs_ptr = nullptr;
typeof(XPLMGetDataRefsByIndex) *XPLMGetDataRefsByIndex_ptr = nullptr;
typeof(XPLMGetDataRefInfo) *XPLMGetDataRefInfo_ptr = nullptr;
typeof(XPLMGetMETARForAirport) *XPLMGetMETARForAirport_ptr = nullptr;
typeof(XPLMGetWeatherAtLocation) *XPLMGetWeatherAtLocation_ptr = nullptr;

#if defined(_FMOD_COMMON_H)
typeof(XPLMGetFMODStudio) *XPLMGetFMODStudio_ptr = nullptr;
typeof(XPLMGetFMODChannelGroup) *XPLMGetFMODChannelGroup_ptr = nullptr;
#endif
typeof(XPLMPlayPCMOnBus) *XPLMPlayPCMOnBus_ptr = nullptr;
typeof(XPLMStopAudio) *XPLMStopAudio_ptr = nullptr;
typeof(XPLMSetAudioPosition) *XPLMSetAudioPosition_ptr = nullptr;
typeof(XPLMSetAudioFadeDistance) *XPLMSetAudioFadeDistance_ptr = nullptr;
typeof(XPLMSetAudioPitch) *XPLMSetAudioPitch_ptr = nullptr;
typeof(XPLMSetAudioVolume) *XPLMSetAudioVolume_ptr = nullptr;
typeof(XPLMSetAudioCone) *XPLMSetAudioCone_ptr = nullptr;

t_fcn_info funcs400[] = {
  {"XPLMRegisterAvionicsCallbacksEx", (void **) &XPLMRegisterAvionicsCallbacksEx_ptr},
  {"XPLMUnregisterAvionicsCallbacks", (void **) &XPLMUnregisterAvionicsCallbacks_ptr},
  {"XPLMCountDataRefs", (void **) &XPLMCountDataRefs_ptr},
  {"XPLMGetDataRefsByIndex", (void **) &XPLMGetDataRefsByIndex_ptr},
  {"XPLMGetDataRefInfo", (void **) &XPLMGetDataRefInfo_ptr},
  {"XPLMGetMETARForAirport", (void **) &XPLMGetMETARForAirport_ptr},
  {"XPLMGetWeatherAtLocation", (void **) &XPLMGetWeatherAtLocation_ptr},
#if defined(_FMOD_COMMON_H)
  {"XPLMGetFMODStudio", (void **) &XPLMGetFMODStudio_ptr},
  {"XPLMGetFMODChannelGroup", (void **) &XPLMGetFMODChannelGroup_ptr},
#endif
  {"XPLMPlayPCMOnBus", (void **) &XPLMPlayPCMOnBus_ptr},
  {"XPLMStopAudio", (void **) &XPLMStopAudio_ptr},
  {"XPLMSetAudioPosition", (void **) &XPLMSetAudioPosition_ptr},
  {"XPLMSetAudioFadeDistance", (void **) &XPLMSetAudioFadeDistance_ptr},
  {"XPLMSetAudioPitch", (void **) &XPLMSetAudioPitch_ptr},
  {"XPLMSetAudioVolume", (void **) &XPLMSetAudioVolume_ptr},
  {"XPLMSetAudioCone", (void **) &XPLMSetAudioCone_ptr},
  {nullptr, nullptr}
};

//XPLM410 functions
typeof(XPLMGetAvionicsHandle) *XPLMGetAvionicsHandle_ptr = nullptr;
typeof(XPLMIsAvionicsBound) *XPLMIsAvionicsBound_ptr = nullptr;
typeof(XPLMIsCursorOverAvionics) *XPLMIsCursorOverAvionics_ptr = nullptr;
typeof(XPLMIsAvionicsPopupVisible) *XPLMIsAvionicsPopupVisible_ptr = nullptr;
typeof(XPLMIsAvionicsPoppedOut) *XPLMIsAvionicsPoppedOut_ptr = nullptr;
typeof(XPLMHasAvionicsKeyboardFocus) *XPLMHasAvionicsKeyboardFocus_ptr = nullptr;
typeof(XPLMAvionicsNeedsDrawing) *XPLMAvionicsNeedsDrawing_ptr = nullptr;
typeof(XPLMSetAvionicsPopupVisible) *XPLMSetAvionicsPopupVisible_ptr = nullptr;
typeof(XPLMPopOutAvionics) *XPLMPopOutAvionics_ptr = nullptr;
typeof(XPLMTakeAvionicsKeyboardFocus) *XPLMTakeAvionicsKeyboardFocus_ptr = nullptr;
typeof(XPLMDestroyAvionics) *XPLMDestroyAvionics_ptr = nullptr;
typeof(XPLMGetAvionicsGeometry) *XPLMGetAvionicsGeometry_ptr = nullptr;
typeof(XPLMSetAvionicsGeometry) *XPLMSetAvionicsGeometry_ptr = nullptr;
typeof(XPLMGetAvionicsGeometryOS) *XPLMGetAvionicsGeometryOS_ptr = nullptr;
typeof(XPLMSetAvionicsGeometryOS) *XPLMSetAvionicsGeometryOS_ptr = nullptr;
typeof(XPLMGetAvionicsBrightnessRheo) *XPLMGetAvionicsBrightnessRheo_ptr = nullptr;
typeof(XPLMSetAvionicsBrightnessRheo) *XPLMSetAvionicsBrightnessRheo_ptr = nullptr;
typeof(XPLMGetAvionicsBusVoltsRatio) *XPLMGetAvionicsBusVoltsRatio_ptr = nullptr;
typeof(XPLMCreateAvionicsEx) *XPLMCreateAvionicsEx_ptr = nullptr;
typeof(XPLMCountFMSFlightPlanEntries) *XPLMCountFMSFlightPlanEntries_ptr = nullptr;
typeof(XPLMGetDisplayedFMSFlightPlanEntry) *XPLMGetDisplayedFMSFlightPlanEntry_ptr = nullptr;
typeof(XPLMGetDestinationFMSFlightPlanEntry) *XPLMGetDestinationFMSFlightPlanEntry_ptr = nullptr;
typeof(XPLMSetDisplayedFMSFlightPlanEntry) *XPLMSetDisplayedFMSFlightPlanEntry_ptr = nullptr;
typeof(XPLMSetDestinationFMSFlightPlanEntry) *XPLMSetDestinationFMSFlightPlanEntry_ptr = nullptr;
typeof(XPLMSetDirectToFMSFlightPlanEntry) *XPLMSetDirectToFMSFlightPlanEntry_ptr = nullptr;
typeof(XPLMGetFMSFlightPlanEntryInfo) *XPLMGetFMSFlightPlanEntryInfo_ptr = nullptr;
typeof(XPLMSetFMSFlightPlanEntryInfo) *XPLMSetFMSFlightPlanEntryInfo_ptr = nullptr;
typeof(XPLMSetFMSFlightPlanEntryLatLon) *XPLMSetFMSFlightPlanEntryLatLon_ptr = nullptr;
typeof(XPLMSetFMSFlightPlanEntryLatLonWithId) *XPLMSetFMSFlightPlanEntryLatLonWithId_ptr = nullptr;
typeof(XPLMClearFMSFlightPlanEntry) *XPLMClearFMSFlightPlanEntry_ptr = nullptr;
typeof(XPLMLoadFMSFlightPlan) *XPLMLoadFMSFlightPlan_ptr = nullptr;
typeof(XPLMSaveFMSFlightPlan) *XPLMSaveFMSFlightPlan_ptr = nullptr;

t_fcn_info funcs410[] = {
  {"XPLMGetAvionicsHandle", (void **) &XPLMGetAvionicsHandle_ptr},
  {"XPLMIsAvionicsBound", (void **) &XPLMIsAvionicsBound_ptr},
  {"XPLMIsCursorOverAvionics", (void **) &XPLMIsCursorOverAvionics_ptr},
  {"XPLMIsAvionicsPopupVisible", (void **) &XPLMIsAvionicsPopupVisible_ptr},
  {"XPLMIsAvionicsPoppedOut", (void **) &XPLMIsAvionicsPoppedOut_ptr},
  {"XPLMHasAvionicsKeyboardFocus", (void **) &XPLMHasAvionicsKeyboardFocus_ptr},
  {"XPLMAvionicsNeedsDrawing", (void **) &XPLMAvionicsNeedsDrawing_ptr},
  {"XPLMSetAvionicsPopupVisible", (void **) &XPLMSetAvionicsPopupVisible_ptr},
  {"XPLMPopOutAvionics", (void **) &XPLMPopOutAvionics_ptr},
  {"XPLMTakeAvionicsKeyboardFocus", (void **) &XPLMTakeAvionicsKeyboardFocus_ptr},
  {"XPLMDestroyAvionics", (void **) &XPLMDestroyAvionics_ptr},
  {"XPLMGetAvionicsGeometry", (void **) &XPLMGetAvionicsGeometry_ptr},
  {"XPLMSetAvionicsGeometry", (void **) &XPLMSetAvionicsGeometry_ptr},
  {"XPLMGetAvionicsGeometryOS", (void **) &XPLMGetAvionicsGeometryOS_ptr},
  {"XPLMSetAvionicsGeometryOS", (void **) &XPLMSetAvionicsGeometryOS_ptr},
  {"XPLMGetAvionicsBrightnessRheo", (void **) &XPLMGetAvionicsBrightnessRheo_ptr},
  {"XPLMSetAvionicsBrightnessRheo", (void **) &XPLMSetAvionicsBrightnessRheo_ptr},
  {"XPLMGetAvionicsBusVoltsRatio", (void **) &XPLMGetAvionicsBusVoltsRatio_ptr},
  {"XPLMCreateAvionicsEx", (void **) &XPLMCreateAvionicsEx_ptr},
  {"XPLMCountFMSFlightPlanEntries", (void **) &XPLMCountFMSFlightPlanEntries_ptr},
  {"XPLMGetDisplayedFMSFlightPlanEntry", (void **) &XPLMGetDisplayedFMSFlightPlanEntry_ptr},
  {"XPLMGetDestinationFMSFlightPlanEntry", (void **) &XPLMGetDestinationFMSFlightPlanEntry_ptr},
  {"XPLMSetDisplayedFMSFlightPlanEntry", (void **) &XPLMSetDisplayedFMSFlightPlanEntry_ptr},
  {"XPLMSetDestinationFMSFlightPlanEntry", (void **) &XPLMSetDestinationFMSFlightPlanEntry_ptr},
  {"XPLMSetDirectToFMSFlightPlanEntry", (void **) &XPLMSetDirectToFMSFlightPlanEntry_ptr},
  {"XPLMGetFMSFlightPlanEntryInfo", (void **) &XPLMGetFMSFlightPlanEntryInfo_ptr},
  {"XPLMSetFMSFlightPlanEntryInfo", (void **) &XPLMSetFMSFlightPlanEntryInfo_ptr},
  {"XPLMSetFMSFlightPlanEntryLatLon", (void **) &XPLMSetFMSFlightPlanEntryLatLon_ptr},
  {"XPLMSetFMSFlightPlanEntryLatLonWithId", (void **) &XPLMSetFMSFlightPlanEntryLatLonWithId_ptr},
  {"XPLMClearFMSFlightPlanEntry", (void **) &XPLMClearFMSFlightPlanEntry_ptr},
  {"XPLMLoadFMSFlightPlan", (void **) &XPLMLoadFMSFlightPlan_ptr},
  {"XPLMSaveFMSFlightPlan", (void **) &XPLMSaveFMSFlightPlan_ptr},
  {nullptr, nullptr}
};

typeof(XPLMInstanceSetPositionDouble) *XPLMInstanceSetPositionDouble_ptr = nullptr;
typeof(XPLMInstanceSetAutoShift) *XPLMInstanceSetAutoShift_ptr = nullptr;
typeof(XPLMBeginWeatherUpdate) *XPLMBeginWeatherUpdate_ptr = nullptr;
typeof(XPLMEndWeatherUpdate) *XPLMEndWeatherUpdate_ptr = nullptr;
typeof(XPLMSetWeatherAtLocation) *XPLMSetWeatherAtLocation_ptr = nullptr;
typeof(XPLMSetWeatherAtAirport) *XPLMSetWeatherAtAirport_ptr = nullptr;
typeof(XPLMEraseWeatherAtLocation) *XPLMEraseWeatherAtLocation_ptr = nullptr;
typeof(XPLMEraseWeatherAtAirport) *XPLMEraseWeatherAtAirport_ptr = nullptr;
t_fcn_info funcs420[] = {
  {"XPLMInstanceSetPositionDouble", (void **) &XPLMInstanceSetPositionDouble_ptr},
  {"XPLMInstanceSetAutoShift", (void **) &XPLMInstanceSetAutoShift_ptr},
  {"XPLMBeginWeatherUpdate", (void **) &XPLMBeginWeatherUpdate_ptr},
  {"XPLMEndWeatherUpdate", (void **) &XPLMEndWeatherUpdate_ptr},
  {"XPLMSetWeatherAtLocation", (void **) &XPLMSetWeatherAtLocation_ptr},
  {"XPLMGetWeatherAtLocation", (void **) &XPLMGetWeatherAtLocation_ptr},
  {"XPLMSetWeatherAtAirport", (void **) &XPLMSetWeatherAtAirport_ptr},
  {"XPLMEraseWeatherAtLocation", (void **) &XPLMEraseWeatherAtLocation_ptr},
  {"XPLMEraseWeatherAtAirport", (void **) &XPLMEraseWeatherAtAirport_ptr},
  {nullptr, nullptr}
};

typeof(XPLMInitFlight) *XPLMInitFlight_ptr = nullptr;
typeof(XPLMUpdateFlight) *XPLMUpdateFlight_ptr = nullptr;
t_fcn_info funcs430[] = {
  {"XPLMInitFlight", (void **) &XPLMInitFlight_ptr},
  {"XPLMUpdateFlight", (void **) &XPLMUpdateFlight_ptr},
  {nullptr, nullptr}
};

typeof(XPLMReturnString) *XPLMReturnString_ptr = nullptr;
typeof(XPLMWindowSetURL) *XPLMWindowSetURL_ptr = nullptr;
typeof(XPLMWindowRefresh) *XPLMWindowRefresh_ptr = nullptr;
typeof(XPLMWindowInjectScript) *XPLMWindowInjectScript_ptr = nullptr;
typeof(XPLMWindowAddBrowserFunction) *XPLMWindowAddBrowserFunction_ptr = nullptr;
typeof(XPLMIsAvionicsMappedToVR) *XPLMIsAvionicsMappedToVR_ptr = nullptr;
typeof(XPLMSetAvionicsMappedToVR) *XPLMSetAvionicsMappedToVR_ptr = nullptr;
typeof(XPLMMakeColor) *XPLMMakeColor_ptr = nullptr;
typeof(XPLMLines) *XPLMLines_ptr = nullptr;
typeof(XPLMLinesWithWidth) *XPLMLinesWithWidth_ptr = nullptr;
typeof(XPLMLinesStipple) *XPLMLinesStipple_ptr = nullptr;
typeof(XPLMLinesc) *XPLMLinesc_ptr = nullptr;
typeof(XPLMLinescWithWidth) *XPLMLinescWithWidth_ptr = nullptr;
typeof(XPLMLineStrip) *XPLMLineStrip_ptr = nullptr;
typeof(XPLMLineStripWithWidth) *XPLMLineStripWithWidth_ptr = nullptr;
typeof(XPLMLineStripc) *XPLMLineStripc_ptr = nullptr;
typeof(XPLMLineStripcWithWidth) *XPLMLineStripcWithWidth_ptr = nullptr;
typeof(XPLMLineStripStipple) *XPLMLineStripStipple_ptr = nullptr;
typeof(XPLMLineLoop) *XPLMLineLoop_ptr = nullptr;
typeof(XPLMLineLoopWithWidth) *XPLMLineLoopWithWidth_ptr = nullptr;
typeof(XPLMLineLoopc) *XPLMLineLoopc_ptr = nullptr;
typeof(XPLMLineLoopcWithWidth) *XPLMLineLoopcWithWidth_ptr = nullptr;
typeof(XPLMLineLoopStipple) *XPLMLineLoopStipple_ptr = nullptr;
typeof(XPLMPolygon) *XPLMPolygon_ptr = nullptr;
typeof(XPLMPolygonWithWidth) *XPLMPolygonWithWidth_ptr = nullptr;
typeof(XPLMPolygonc) *XPLMPolygonc_ptr = nullptr;
typeof(XPLMPolygoncWithWidth) *XPLMPolygoncWithWidth_ptr = nullptr;
typeof(XPLMQuadstrip) *XPLMQuadstrip_ptr = nullptr;
typeof(XPLMQuadstripWithWidth) *XPLMQuadstripWithWidth_ptr = nullptr;
typeof(XPLMQuadstripc) *XPLMQuadstripc_ptr = nullptr;
typeof(XPLMQuadstripcWithWidth) *XPLMQuadstripcWithWidth_ptr = nullptr;
typeof(XPLMTransformPush) *XPLMTransformPush_ptr = nullptr;
typeof(XPLMTransformPop) *XPLMTransformPop_ptr = nullptr;
typeof(XPLMTransformTranslate) *XPLMTransformTranslate_ptr = nullptr;
typeof(XPLMTransformRotate) *XPLMTransformRotate_ptr = nullptr;
typeof(XPLMTransformScale) *XPLMTransformScale_ptr = nullptr;
typeof(XPLMScissorPush) *XPLMScissorPush_ptr = nullptr;
typeof(XPLMScissorPop) *XPLMScissorPop_ptr = nullptr;
typeof(XPLMScissorSet) *XPLMScissorSet_ptr = nullptr;
typeof(XPLMScissorShrink) *XPLMScissorShrink_ptr = nullptr;
typeof(XPLMBeginSetupStencilMask) *XPLMBeginSetupStencilMask_ptr = nullptr;
typeof(XPLMEndSetupStencilMask) *XPLMEndSetupStencilMask_ptr = nullptr;
typeof(XPLMUseStencilMask) *XPLMUseStencilMask_ptr = nullptr;
typeof(XPLMClearStencilMask) *XPLMClearStencilMask_ptr = nullptr;
typeof(XPLMCreateFont) *XPLMCreateFont_ptr = nullptr;
typeof(XPLMDestroyFont) *XPLMDestroyFont_ptr = nullptr;
typeof(XPLMFontAddFace) *XPLMFontAddFace_ptr = nullptr;
typeof(XPLMFontGetMetrics) *XPLMFontGetMetrics_ptr = nullptr;
typeof(XPLMFontMeasureString) *XPLMFontMeasureString_ptr = nullptr;
typeof(XPLMFontGetLineCount) *XPLMFontGetLineCount_ptr = nullptr;
typeof(XPLMFontFitForward) *XPLMFontFitForward_ptr = nullptr;
typeof(XPLMFontFitReverse) *XPLMFontFitReverse_ptr = nullptr;
typeof(XPLMFontDrawString) *XPLMFontDrawString_ptr = nullptr;
typeof(XPLMFontDrawStringFixedSpacing) *XPLMFontDrawStringFixedSpacing_ptr = nullptr;
typeof(XPLMFontDrawStringWordWrapped) *XPLMFontDrawStringWordWrapped_ptr = nullptr;
typeof(XPLMFontDrawStringRotated) *XPLMFontDrawStringRotated_ptr = nullptr;
typeof(XPLMCreateTextureAtlas) *XPLMCreateTextureAtlas_ptr = nullptr;
typeof(XPLMDestroyTextureAtlas) *XPLMDestroyTextureAtlas_ptr = nullptr;
typeof(XPLMTextureAtlasAddImageFile) *XPLMTextureAtlasAddImageFile_ptr = nullptr;
typeof(XPLMTextureAtlasAddImageFileSet) *XPLMTextureAtlasAddImageFileSet_ptr = nullptr;
typeof(XPLMTextureAtlasAddImage) *XPLMTextureAtlasAddImage_ptr = nullptr;
typeof(XPLMTextureAtlasAddImageSet) *XPLMTextureAtlasAddImageSet_ptr = nullptr;
typeof(XPLMTextureAtlasBake) *XPLMTextureAtlasBake_ptr = nullptr;
typeof(XPLMTextureAtlasGetImageWidth) *XPLMTextureAtlasGetImageWidth_ptr = nullptr;
typeof(XPLMTextureAtlasGetImageHeight) *XPLMTextureAtlasGetImageHeight_ptr = nullptr;
typeof(XPLMTextureAtlasGetImageUVMap) *XPLMTextureAtlasGetImageUVMap_ptr = nullptr;
typeof(XPLMTextureAtlasDrawAt) *XPLMTextureAtlasDrawAt_ptr = nullptr;
typeof(XPLMTextureAtlasDrawIn) *XPLMTextureAtlasDrawIn_ptr = nullptr;
typeof(XPLMTextureAtlasDrawStretched) *XPLMTextureAtlasDrawStretched_ptr = nullptr;
typeof(XPLMTextureAtlasDrawScaled) *XPLMTextureAtlasDrawScaled_ptr = nullptr;
typeof(XPLMTextureAtlasDrawMesh) *XPLMTextureAtlasDrawMesh_ptr = nullptr;
typeof(XPLMTextureSourceDrawIn) *XPLMTextureSourceDrawIn_ptr = nullptr;
typeof(XPLMTextureSourceDrawMesh) *XPLMTextureSourceDrawMesh_ptr = nullptr;
typeof(XPLMBeginRetainedDrawing) *XPLMBeginRetainedDrawing_ptr = nullptr;
typeof(XPLMEndRetainedDrawing) *XPLMEndRetainedDrawing_ptr = nullptr;
typeof(XPLMDrawRetained) *XPLMDrawRetained_ptr = nullptr;
typeof(XPLMDestroyRetainedDrawing) *XPLMDestroyRetainedDrawing_ptr = nullptr;
typeof(XPLMCreateSVTDisplay) *XPLMCreateSVTDisplay_ptr = nullptr;
typeof(XPLMDestroySVTDisplay) *XPLMDestroySVTDisplay_ptr = nullptr;
typeof(XPLMSVTDisplayDrawIn) *XPLMSVTDisplayDrawIn_ptr = nullptr;
typeof(XPLMCreateMapDisplay) *XPLMCreateMapDisplay_ptr = nullptr;
typeof(XPLMDestroyMapDisplay) *XPLMDestroyMapDisplay_ptr = nullptr;
typeof(XPLMMapDisplayDrawIn) *XPLMMapDisplayDrawIn_ptr = nullptr;
typeof(XPLMCreateTexture) *XPLMCreateTexture_ptr = nullptr;
typeof(XPLMDestroyTexture) *XPLMDestroyTexture_ptr = nullptr;
typeof(XPLMDrawCalls) *XPLMDrawCalls_ptr = nullptr;
typeof(XPLMAccumulateTouchZone) *XPLMAccumulateTouchZone_ptr = nullptr;
typeof(XPLMAvionicsSetTouchEventHandler) *XPLMAvionicsSetTouchEventHandler_ptr = nullptr;
typeof(XPLMWindowSetTouchEventHandler) *XPLMWindowSetTouchEventHandler_ptr = nullptr;
typeof(XPLMReloadThisPlugin) *XPLMReloadThisPlugin_ptr = nullptr;
t_fcn_info funcs440[] = {
  {"XPLMReturnString", (void **) &XPLMReturnString_ptr},
  {"XPLMWindowSetURL", (void **) &XPLMWindowSetURL_ptr},
  {"XPLMWindowRefresh", (void **) &XPLMWindowRefresh_ptr},
  {"XPLMWindowInjectScript", (void **) &XPLMWindowInjectScript_ptr},
  {"XPLMWindowAddBrowserFunction", (void **) &XPLMWindowAddBrowserFunction_ptr},
  {"XPLMIsAvionicsMappedToVR", (void **) &XPLMIsAvionicsMappedToVR_ptr},
  {"XPLMSetAvionicsMappedToVR", (void **) &XPLMSetAvionicsMappedToVR_ptr},
  {"XPLMMakeColor", (void **) &XPLMMakeColor_ptr},
  {"XPLMLines", (void **) &XPLMLines_ptr},
  {"XPLMLinesWithWidth", (void **) &XPLMLinesWithWidth_ptr},
  {"XPLMLinesStipple", (void **) &XPLMLinesStipple_ptr},
  {"XPLMLinesc", (void **) &XPLMLinesc_ptr},
  {"XPLMLinescWithWidth", (void **) &XPLMLinescWithWidth_ptr},
  {"XPLMLineStrip", (void **) &XPLMLineStrip_ptr},
  {"XPLMLineStripWithWidth", (void **) &XPLMLineStripWithWidth_ptr},
  {"XPLMLineStripc", (void **) &XPLMLineStripc_ptr},
  {"XPLMLineStripcWithWidth", (void **) &XPLMLineStripcWithWidth_ptr},
  {"XPLMLineStripStipple", (void **) &XPLMLineStripStipple_ptr},
  {"XPLMLineLoop", (void **) &XPLMLineLoop_ptr},
  {"XPLMLineLoopWithWidth", (void **) &XPLMLineLoopWithWidth_ptr},
  {"XPLMLineLoopc", (void **) &XPLMLineLoopc_ptr},
  {"XPLMLineLoopcWithWidth", (void **) &XPLMLineLoopcWithWidth_ptr},
  {"XPLMLineLoopStipple", (void **) &XPLMLineLoopStipple_ptr},
  {"XPLMPolygon", (void **) &XPLMPolygon_ptr},
  {"XPLMPolygonWithWidth", (void **) &XPLMPolygonWithWidth_ptr},
  {"XPLMPolygonc", (void **) &XPLMPolygonc_ptr},
  {"XPLMPolygoncWithWidth", (void **) &XPLMPolygoncWithWidth_ptr},
  {"XPLMQuadstrip", (void **) &XPLMQuadstrip_ptr},
  {"XPLMQuadstripWithWidth", (void **) &XPLMQuadstripWithWidth_ptr},
  {"XPLMQuadstripc", (void **) &XPLMQuadstripc_ptr},
  {"XPLMQuadstripcWithWidth", (void **) &XPLMQuadstripcWithWidth_ptr},
  {"XPLMTransformPush", (void **) &XPLMTransformPush_ptr},
  {"XPLMTransformPop", (void **) &XPLMTransformPop_ptr},
  {"XPLMTransformTranslate", (void **) &XPLMTransformTranslate_ptr},
  {"XPLMTransformRotate", (void **) &XPLMTransformRotate_ptr},
  {"XPLMTransformScale", (void **) &XPLMTransformScale_ptr},
  {"XPLMScissorPush", (void **) &XPLMScissorPush_ptr},
  {"XPLMScissorPop", (void **) &XPLMScissorPop_ptr},
  {"XPLMScissorSet", (void **) &XPLMScissorSet_ptr},
  {"XPLMScissorShrink", (void **) &XPLMScissorShrink_ptr},
  {"XPLMBeginSetupStencilMask", (void **) &XPLMBeginSetupStencilMask_ptr},
  {"XPLMEndSetupStencilMask", (void **) &XPLMEndSetupStencilMask_ptr},
  {"XPLMUseStencilMask", (void **) &XPLMUseStencilMask_ptr},
  {"XPLMClearStencilMask", (void **) &XPLMClearStencilMask_ptr},
  {"XPLMCreateFont", (void **) &XPLMCreateFont_ptr},
  {"XPLMDestroyFont", (void **) &XPLMDestroyFont_ptr},
  {"XPLMFontAddFace", (void **) &XPLMFontAddFace_ptr},
  {"XPLMFontGetMetrics", (void **) &XPLMFontGetMetrics_ptr},
  {"XPLMFontMeasureString", (void **) &XPLMFontMeasureString_ptr},
  {"XPLMFontGetLineCount", (void **) &XPLMFontGetLineCount_ptr},
  {"XPLMFontFitForward", (void **) &XPLMFontFitForward_ptr},
  {"XPLMFontFitReverse", (void **) &XPLMFontFitReverse_ptr},
  {"XPLMFontDrawString", (void **) &XPLMFontDrawString_ptr},
  {"XPLMFontDrawStringFixedSpacing", (void **) &XPLMFontDrawStringFixedSpacing_ptr},
  {"XPLMFontDrawStringWordWrapped", (void **) &XPLMFontDrawStringWordWrapped_ptr},
  {"XPLMFontDrawStringRotated", (void **) &XPLMFontDrawStringRotated_ptr},
  {"XPLMCreateTextureAtlas", (void **) &XPLMCreateTextureAtlas_ptr},
  {"XPLMDestroyTextureAtlas", (void **) &XPLMDestroyTextureAtlas_ptr},
  {"XPLMTextureAtlasAddImageFile", (void **) &XPLMTextureAtlasAddImageFile_ptr},
  {"XPLMTextureAtlasAddImageFileSet", (void **) &XPLMTextureAtlasAddImageFileSet_ptr},
  {"XPLMTextureAtlasAddImage", (void **) &XPLMTextureAtlasAddImage_ptr},
  {"XPLMTextureAtlasAddImageSet", (void **) &XPLMTextureAtlasAddImageSet_ptr},
  {"XPLMTextureAtlasBake", (void **) &XPLMTextureAtlasBake_ptr},
  {"XPLMTextureAtlasGetImageWidth", (void **) &XPLMTextureAtlasGetImageWidth_ptr},
  {"XPLMTextureAtlasGetImageHeight", (void **) &XPLMTextureAtlasGetImageHeight_ptr},
  {"XPLMTextureAtlasGetImageUVMap", (void **) &XPLMTextureAtlasGetImageUVMap_ptr},
  {"XPLMTextureAtlasDrawAt", (void **) &XPLMTextureAtlasDrawAt_ptr},
  {"XPLMTextureAtlasDrawIn", (void **) &XPLMTextureAtlasDrawIn_ptr},
  {"XPLMTextureAtlasDrawStretched", (void **) &XPLMTextureAtlasDrawStretched_ptr},
  {"XPLMTextureAtlasDrawScaled", (void **) &XPLMTextureAtlasDrawScaled_ptr},
  {"XPLMTextureAtlasDrawMesh", (void **) &XPLMTextureAtlasDrawMesh_ptr},
  {"XPLMTextureSourceDrawIn", (void **) &XPLMTextureSourceDrawIn_ptr},
  {"XPLMTextureSourceDrawMesh", (void **) &XPLMTextureSourceDrawMesh_ptr},
  {"XPLMBeginRetainedDrawing", (void **) &XPLMBeginRetainedDrawing_ptr},
  {"XPLMEndRetainedDrawing", (void **) &XPLMEndRetainedDrawing_ptr},
  {"XPLMDrawRetained", (void **) &XPLMDrawRetained_ptr},
  {"XPLMDestroyRetainedDrawing", (void **) &XPLMDestroyRetainedDrawing_ptr},
  {"XPLMCreateSVTDisplay", (void **) &XPLMCreateSVTDisplay_ptr},
  {"XPLMDestroySVTDisplay", (void **) &XPLMDestroySVTDisplay_ptr},
  {"XPLMSVTDisplayDrawIn", (void **) &XPLMSVTDisplayDrawIn_ptr},
  {"XPLMCreateMapDisplay", (void **) &XPLMCreateMapDisplay_ptr},
  {"XPLMDestroyMapDisplay", (void **) &XPLMDestroyMapDisplay_ptr},
  {"XPLMMapDisplayDrawIn", (void **) &XPLMMapDisplayDrawIn_ptr},
  {"XPLMCreateTexture", (void **) &XPLMCreateTexture_ptr},
  {"XPLMDestroyTexture", (void **) &XPLMDestroyTexture_ptr},
  {"XPLMDrawCalls", (void **) &XPLMDrawCalls_ptr},
  {"XPLMAccumulateTouchZone", (void **) &XPLMAccumulateTouchZone_ptr},
  {"XPLMAvionicsSetTouchEventHandler", (void **) &XPLMAvionicsSetTouchEventHandler_ptr},
  {"XPLMWindowSetTouchEventHandler", (void **) &XPLMWindowSetTouchEventHandler_ptr},
  {"XPLMReloadThisPlugin", (void **) &XPLMReloadThisPlugin_ptr},
  {nullptr, nullptr}
};


bool loadFunctions(t_fcn_info *ptr, void *handle)
{
  void *fun_ptr;
  bool res = true;

  while(ptr->name != nullptr){
    fun_ptr = dlsym(handle, ptr->name);
    if(fun_ptr != nullptr){
      *(ptr->fun_ptr) = fun_ptr;
    }else{
      fprintf(stderr, "Couldn't get address of function '%s'.\n", ptr->name);
      res = false;
    }
    ++ptr;
  }
  return res;
}

bool loadSDKFunctions(void)
{
  pythonDebug("loadSDKFunctions Loading...");
  int xp_ver, xplm_ver;
  XPLMHostApplicationID app;
  XPLMGetVersions(&xp_ver, &xplm_ver, &app);

  void *handle = dlopen(nullptr, RTLD_NOW);
  if(handle == nullptr){
    fprintf(stderr, "Problem dlopening executable.\n");
    return false;
  }

  bool res = true;
  if(xplm_ver >= 440 || xp_ver >= 12440) res &= loadFunctions(funcs440, handle);
  if(xplm_ver >= 430 || xp_ver >= 12400) res &= loadFunctions(funcs430, handle);
  if(xplm_ver >= 420) res &= loadFunctions(funcs420, handle);
  if(xplm_ver >= 410) res &= loadFunctions(funcs410, handle);
  if(xplm_ver >= 400) res &= loadFunctions(funcs400, handle);
  if(xplm_ver >= 301) res &= loadFunctions(funcs301, handle);
  if(xplm_ver >= 300) res &= loadFunctions(funcs300, handle);
  if(xplm_ver >= 210) res &= loadFunctions(funcs210, handle);

  dlclose(handle);
  pythonDebug("  loadSDKFunctions Loaded.");
  return res;
}

