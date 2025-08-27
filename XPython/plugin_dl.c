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

#include <XPLM/XPLMInstance.h>
#include <XPLM/XPLMMap.h>
#include <XPLM/XPLMDisplay.h>
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
typeof(XPLMCreateFlightLoop) *XPLMCreateFlightLoop_ptr = NULL;
typeof(XPLMDestroyFlightLoop) *XPLMDestroyFlightLoop_ptr = NULL;
typeof(XPLMLoadObjectAsync) *XPLMLoadObjectAsync_ptr = NULL;
typeof(XPLMRemoveMenuItem) *XPLMRemoveMenuItem_ptr = NULL;
typeof(XPLMScheduleFlightLoop) *XPLMScheduleFlightLoop_ptr = NULL;

t_fcn_info funcs210[] = {
  {"XPLMCreateFlightLoop", (void *)&XPLMCreateFlightLoop_ptr},
  {"XPLMDestroyFlightLoop", (void *)&XPLMDestroyFlightLoop_ptr},
  {"XPLMLoadObjectAsync", (void *)&XPLMLoadObjectAsync_ptr},
  {"XPLMRemoveMenuItem", (void *)&XPLMRemoveMenuItem_ptr},
  {"XPLMScheduleFlightLoop", (void *)&XPLMScheduleFlightLoop_ptr},
  {NULL, NULL}
};

// XPLM300 functions
typeof(XPLMAppendMenuItemWithCommand) *XPLMAppendMenuItemWithCommand_ptr = NULL;
typeof(XPLMCreateMapLayer) *XPLMCreateMapLayer_ptr = NULL;
typeof(XPLMDegMagneticToDegTrue) *XPLMDegMagneticToDegTrue_ptr = NULL;
typeof(XPLMDegTrueToDegMagnetic) *XPLMDegTrueToDegMagnetic_ptr = NULL;
typeof(XPLMDestroyMapLayer) *XPLMDestroyMapLayer_ptr = NULL;
typeof(XPLMDrawMapIconFromSheet) *XPLMDrawMapIconFromSheet_ptr = NULL;
typeof(XPLMDrawMapLabel) *XPLMDrawMapLabel_ptr = NULL;
typeof(XPLMFindAircraftMenu) *XPLMFindAircraftMenu_ptr = NULL;
typeof(XPLMGetAllMonitorBoundsGlobal) *XPLMGetAllMonitorBoundsGlobal_ptr = NULL;
typeof(XPLMGetAllMonitorBoundsOS) *XPLMGetAllMonitorBoundsOS_ptr = NULL;
typeof(XPLMGetMagneticVariation) *XPLMGetMagneticVariation_ptr = NULL;
typeof(XPLMGetMouseLocationGlobal) *XPLMGetMouseLocationGlobal_ptr = NULL;
typeof(XPLMGetScreenBoundsGlobal) *XPLMGetScreenBoundsGlobal_ptr = NULL;
typeof(XPLMGetWindowGeometryOS) *XPLMGetWindowGeometryOS_ptr = NULL;
typeof(XPLMMapExists) *XPLMMapExists_ptr = NULL;
typeof(XPLMMapGetNorthHeading) *XPLMMapGetNorthHeading_ptr = NULL;
typeof(XPLMMapProject) *XPLMMapProject_ptr = NULL;
typeof(XPLMMapScaleMeter) *XPLMMapScaleMeter_ptr = NULL;
typeof(XPLMMapUnproject) *XPLMMapUnproject_ptr = NULL;
typeof(XPLMPlaceUserAtLocation) *XPLMPlaceUserAtLocation_ptr = NULL;
typeof(XPLMRegisterMapCreationHook) *XPLMRegisterMapCreationHook_ptr = NULL;
typeof(XPLMSetWindowGeometryOS) *XPLMSetWindowGeometryOS_ptr = NULL;
typeof(XPLMSetWindowGravity) *XPLMSetWindowGravity_ptr = NULL;
typeof(XPLMSetWindowPositioningMode) *XPLMSetWindowPositioningMode_ptr = NULL;
typeof(XPLMSetWindowResizingLimits) *XPLMSetWindowResizingLimits_ptr = NULL;
typeof(XPLMSetWindowTitle) *XPLMSetWindowTitle_ptr = NULL;
typeof(XPLMWindowIsPoppedOut) *XPLMWindowIsPoppedOut_ptr = NULL;
typeof(XPLMHasKeyboardFocus) *XPLMHasKeyboardFocus_ptr = NULL;
typeof(XPLMCreateInstance) *XPLMCreateInstance_ptr = NULL;
typeof(XPLMDestroyInstance) *XPLMDestroyInstance_ptr = NULL;
typeof(XPLMInstanceSetPosition) *XPLMInstanceSetPosition_ptr = NULL;


t_fcn_info funcs300[] = {
  {"XPLMAppendMenuItemWithCommand", (void *)&XPLMAppendMenuItemWithCommand_ptr},
  {"XPLMCreateMapLayer", (void *)&XPLMCreateMapLayer_ptr},
  {"XPLMDegMagneticToDegTrue", (void *)&XPLMDegMagneticToDegTrue_ptr},
  {"XPLMDegTrueToDegMagnetic", (void *)&XPLMDegTrueToDegMagnetic_ptr},
  {"XPLMDestroyMapLayer", (void *)&XPLMDestroyMapLayer_ptr},
  {"XPLMDrawMapIconFromSheet", (void *)&XPLMDrawMapIconFromSheet_ptr},
  {"XPLMDrawMapLabel", (void *)&XPLMDrawMapLabel_ptr},
  {"XPLMFindAircraftMenu", (void *)&XPLMFindAircraftMenu_ptr},
  {"XPLMGetAllMonitorBoundsGlobal", (void *)&XPLMGetAllMonitorBoundsGlobal_ptr},
  {"XPLMGetAllMonitorBoundsOS", (void *)&XPLMGetAllMonitorBoundsOS_ptr},
  {"XPLMGetMagneticVariation", (void *)&XPLMGetMagneticVariation_ptr},
  {"XPLMGetMouseLocationGlobal", (void *)&XPLMGetMouseLocationGlobal_ptr},
  {"XPLMGetScreenBoundsGlobal", (void *)&XPLMGetScreenBoundsGlobal_ptr},
  {"XPLMGetWindowGeometryOS", (void *)&XPLMGetWindowGeometryOS_ptr},
  {"XPLMMapExists", (void *)&XPLMMapExists_ptr},
  {"XPLMMapGetNorthHeading", (void *)&XPLMMapGetNorthHeading_ptr},
  {"XPLMMapProject", (void *)&XPLMMapProject_ptr},
  {"XPLMMapScaleMeter", (void *)&XPLMMapScaleMeter_ptr},
  {"XPLMMapUnproject", (void *)&XPLMMapUnproject_ptr},
  {"XPLMPlaceUserAtLocation", (void *)&XPLMPlaceUserAtLocation_ptr},
  {"XPLMRegisterMapCreationHook", (void *)&XPLMRegisterMapCreationHook_ptr},
  {"XPLMSetWindowGeometryOS", (void *)&XPLMSetWindowGeometryOS_ptr},
  {"XPLMSetWindowGravity", (void *)&XPLMSetWindowGravity_ptr},
  {"XPLMSetWindowPositioningMode", (void *)&XPLMSetWindowPositioningMode_ptr},
  {"XPLMSetWindowResizingLimits", (void *)&XPLMSetWindowResizingLimits_ptr},
  {"XPLMSetWindowTitle", (void *)&XPLMSetWindowTitle_ptr},
  {"XPLMWindowIsPoppedOut", (void *)&XPLMWindowIsPoppedOut_ptr},
  {"XPLMHasKeyboardFocus", (void *)&XPLMHasKeyboardFocus_ptr},
  {"XPLMCreateInstance", (void *)&XPLMCreateInstance_ptr},
  {"XPLMDestroyInstance", (void *)&XPLMDestroyInstance_ptr},
  {"XPLMInstanceSetPosition", (void *)&XPLMInstanceSetPosition_ptr},
  {NULL, NULL}
};

//XPLM301 functions
typeof(XPGetWidgetUnderlyingWindow) *XPGetWidgetUnderlyingWindow_ptr = NULL;
typeof(XPLMGetWindowGeometryVR) *XPLMGetWindowGeometryVR_ptr = NULL;
typeof(XPLMSetWindowGeometryVR) *XPLMSetWindowGeometryVR_ptr = NULL;
typeof(XPLMWindowIsInVR) *XPLMWindowIsInVR_ptr = NULL;

t_fcn_info funcs301[] = {
  {"XPGetWidgetUnderlyingWindow", (void *)&XPGetWidgetUnderlyingWindow_ptr},
  {"XPLMGetWindowGeometryVR", (void *)&XPLMGetWindowGeometryVR_ptr},
  {"XPLMSetWindowGeometryVR", (void *)&XPLMSetWindowGeometryVR_ptr},
  {"XPLMWindowIsInVR", (void *)&XPLMWindowIsInVR_ptr},
  {NULL, NULL}
};

//XPLM400 functions
typeof(XPLMRegisterAvionicsCallbacksEx) *XPLMRegisterAvionicsCallbacksEx_ptr = NULL;
typeof(XPLMUnregisterAvionicsCallbacks) *XPLMUnregisterAvionicsCallbacks_ptr = NULL;
typeof(XPLMCountDataRefs) *XPLMCountDataRefs_ptr = NULL;
typeof(XPLMGetDataRefsByIndex) *XPLMGetDataRefsByIndex_ptr = NULL;
typeof(XPLMGetDataRefInfo) *XPLMGetDataRefInfo_ptr = NULL;
typeof(XPLMGetMETARForAirport) *XPLMGetMETARForAirport_ptr = NULL;
typeof(XPLMGetWeatherAtLocation) *XPLMGetWeatherAtLocation_ptr = NULL;

#if defined(_FMOD_COMMON_H)
typeof(XPLMGetFMODStudio) *XPLMGetFMODStudio_ptr = NULL;
typeof(XPLMGetFMODChannelGroup) *XPLMGetFMODChannelGroup_ptr = NULL;
#endif
typeof(XPLMPlayPCMOnBus) *XPLMPlayPCMOnBus_ptr = NULL;
typeof(XPLMStopAudio) *XPLMStopAudio_ptr = NULL;
typeof(XPLMSetAudioPosition) *XPLMSetAudioPosition_ptr = NULL;
typeof(XPLMSetAudioFadeDistance) *XPLMSetAudioFadeDistance_ptr = NULL;
typeof(XPLMSetAudioPitch) *XPLMSetAudioPitch_ptr = NULL;
typeof(XPLMSetAudioVolume) *XPLMSetAudioVolume_ptr = NULL;
typeof(XPLMSetAudioCone) *XPLMSetAudioCone_ptr = NULL;

t_fcn_info funcs400[] = {
  {"XPLMRegisterAvionicsCallbacksEx", (void*) &XPLMRegisterAvionicsCallbacksEx_ptr},
  {"XPLMUnregisterAvionicsCallbacks", (void*) &XPLMUnregisterAvionicsCallbacks_ptr},
  {"XPLMCountDataRefs", (void*) &XPLMCountDataRefs_ptr},
  {"XPLMGetDataRefsByIndex", (void*) &XPLMGetDataRefsByIndex_ptr},
  {"XPLMGetDataRefInfo", (void*) &XPLMGetDataRefInfo_ptr},
  {"XPLMGetMETARForAirport", (void*) &XPLMGetMETARForAirport_ptr},
  {"XPLMGetWeatherAtLocation", (void*) &XPLMGetWeatherAtLocation_ptr},
#if defined(_FMOD_COMMON_H)
  {"XPLMGetFMODStudio", (void*) &XPLMGetFMODStudio_ptr},
  {"XPLMGetFMODChannelGroup", (void*) &XPLMGetFMODChannelGroup_ptr},
#endif
  {"XPLMPlayPCMOnBus", (void*) &XPLMPlayPCMOnBus_ptr},
  {"XPLMStopAudio", (void*) &XPLMStopAudio_ptr},
  {"XPLMSetAudioPosition", (void*) &XPLMSetAudioPosition_ptr},
  {"XPLMSetAudioFadeDistance", (void*) &XPLMSetAudioFadeDistance_ptr},
  {"XPLMSetAudioPitch", (void*) &XPLMSetAudioPitch_ptr},
  {"XPLMSetAudioVolume", (void*) &XPLMSetAudioVolume_ptr},
  {"XPLMSetAudioCone", (void*) &XPLMSetAudioCone_ptr},
  {NULL, NULL}
};

//XPLM410 functions
typeof(XPLMGetAvionicsHandle) *XPLMGetAvionicsHandle_ptr = NULL;
typeof(XPLMIsAvionicsBound) *XPLMIsAvionicsBound_ptr = NULL;
typeof(XPLMIsCursorOverAvionics) *XPLMIsCursorOverAvionics_ptr = NULL;
typeof(XPLMIsAvionicsPopupVisible) *XPLMIsAvionicsPopupVisible_ptr = NULL;
typeof(XPLMIsAvionicsPoppedOut) *XPLMIsAvionicsPoppedOut_ptr = NULL;
typeof(XPLMHasAvionicsKeyboardFocus) *XPLMHasAvionicsKeyboardFocus_ptr = NULL;
typeof(XPLMAvionicsNeedsDrawing) *XPLMAvionicsNeedsDrawing_ptr = NULL;
typeof(XPLMSetAvionicsPopupVisible) *XPLMSetAvionicsPopupVisible_ptr = NULL;
typeof(XPLMPopOutAvionics) *XPLMPopOutAvionics_ptr = NULL;
typeof(XPLMTakeAvionicsKeyboardFocus) *XPLMTakeAvionicsKeyboardFocus_ptr = NULL;
typeof(XPLMDestroyAvionics) *XPLMDestroyAvionics_ptr = NULL;
typeof(XPLMGetAvionicsGeometry) *XPLMGetAvionicsGeometry_ptr = NULL;
typeof(XPLMSetAvionicsGeometry) *XPLMSetAvionicsGeometry_ptr = NULL;
typeof(XPLMGetAvionicsGeometryOS) *XPLMGetAvionicsGeometryOS_ptr = NULL;
typeof(XPLMSetAvionicsGeometryOS) *XPLMSetAvionicsGeometryOS_ptr = NULL;
typeof(XPLMGetAvionicsBrightnessRheo) *XPLMGetAvionicsBrightnessRheo_ptr = NULL;
typeof(XPLMSetAvionicsBrightnessRheo) *XPLMSetAvionicsBrightnessRheo_ptr = NULL;
typeof(XPLMGetAvionicsBusVoltsRatio) *XPLMGetAvionicsBusVoltsRatio_ptr = NULL;
typeof(XPLMCreateAvionicsEx) *XPLMCreateAvionicsEx_ptr = NULL;
typeof(XPLMCountFMSFlightPlanEntries) *XPLMCountFMSFlightPlanEntries_ptr = NULL;
typeof(XPLMGetDisplayedFMSFlightPlanEntry) *XPLMGetDisplayedFMSFlightPlanEntry_ptr = NULL;
typeof(XPLMGetDestinationFMSFlightPlanEntry) *XPLMGetDestinationFMSFlightPlanEntry_ptr = NULL;
typeof(XPLMSetDisplayedFMSFlightPlanEntry) *XPLMSetDisplayedFMSFlightPlanEntry_ptr = NULL;
typeof(XPLMSetDestinationFMSFlightPlanEntry) *XPLMSetDestinationFMSFlightPlanEntry_ptr = NULL;
typeof(XPLMSetDirectToFMSFlightPlanEntry) *XPLMSetDirectToFMSFlightPlanEntry_ptr = NULL;
typeof(XPLMGetFMSFlightPlanEntryInfo) *XPLMGetFMSFlightPlanEntryInfo_ptr = NULL;
typeof(XPLMSetFMSFlightPlanEntryInfo) *XPLMSetFMSFlightPlanEntryInfo_ptr = NULL;
typeof(XPLMSetFMSFlightPlanEntryLatLon) *XPLMSetFMSFlightPlanEntryLatLon_ptr = NULL;
typeof(XPLMSetFMSFlightPlanEntryLatLonWithId) *XPLMSetFMSFlightPlanEntryLatLonWithId_ptr = NULL;
typeof(XPLMClearFMSFlightPlanEntry) *XPLMClearFMSFlightPlanEntry_ptr = NULL;
typeof(XPLMLoadFMSFlightPlan) *XPLMLoadFMSFlightPlan_ptr = NULL;
typeof(XPLMSaveFMSFlightPlan) *XPLMSaveFMSFlightPlan_ptr = NULL;

t_fcn_info funcs410[] = {
  {"XPLMGetAvionicsHandle", (void*) &XPLMGetAvionicsHandle_ptr},
  {"XPLMIsAvionicsBound", (void*) &XPLMIsAvionicsBound_ptr},
  {"XPLMIsCursorOverAvionics", (void*) &XPLMIsCursorOverAvionics_ptr},
  {"XPLMIsAvionicsPopupVisible", (void*) &XPLMIsAvionicsPopupVisible_ptr},
  {"XPLMIsAvionicsPoppedOut", (void*) &XPLMIsAvionicsPoppedOut_ptr},
  {"XPLMHasAvionicsKeyboardFocus", (void*) &XPLMHasAvionicsKeyboardFocus_ptr},
  {"XPLMAvionicsNeedsDrawing", (void*) &XPLMAvionicsNeedsDrawing_ptr},
  {"XPLMSetAvionicsPopupVisible", (void*) &XPLMSetAvionicsPopupVisible_ptr},
  {"XPLMPopOutAvionics", (void*) &XPLMPopOutAvionics_ptr},
  {"XPLMTakeAvionicsKeyboardFocus", (void*) &XPLMTakeAvionicsKeyboardFocus_ptr},
  {"XPLMDestroyAvionics", (void*) &XPLMDestroyAvionics_ptr},
  {"XPLMGetAvionicsGeometry", (void*) &XPLMGetAvionicsGeometry_ptr},
  {"XPLMSetAvionicsGeometry", (void*) &XPLMSetAvionicsGeometry_ptr},
  {"XPLMGetAvionicsGeometryOS", (void*) &XPLMGetAvionicsGeometryOS_ptr},
  {"XPLMSetAvionicsGeometryOS", (void*) &XPLMSetAvionicsGeometryOS_ptr},
  {"XPLMGetAvionicsBrightnessRheo", (void*) &XPLMGetAvionicsBrightnessRheo_ptr},
  {"XPLMSetAvionicsBrightnessRheo", (void*) &XPLMSetAvionicsBrightnessRheo_ptr},
  {"XPLMGetAvionicsBusVoltsRatio", (void*) &XPLMGetAvionicsBusVoltsRatio_ptr},
  {"XPLMCreateAvionicsEx", (void*) &XPLMCreateAvionicsEx_ptr},
  {"XPLMCountFMSFlightPlanEntries", (void*) &XPLMCountFMSFlightPlanEntries_ptr},
  {"XPLMGetDisplayedFMSFlightPlanEntry", (void*) &XPLMGetDisplayedFMSFlightPlanEntry_ptr},
  {"XPLMGetDestinationFMSFlightPlanEntry", (void*) &XPLMGetDestinationFMSFlightPlanEntry_ptr},
  {"XPLMSetDisplayedFMSFlightPlanEntry", (void*) &XPLMSetDisplayedFMSFlightPlanEntry_ptr},
  {"XPLMSetDestinationFMSFlightPlanEntry", (void*) &XPLMSetDestinationFMSFlightPlanEntry_ptr},
  {"XPLMSetDirectToFMSFlightPlanEntry", (void*) &XPLMSetDirectToFMSFlightPlanEntry_ptr},
  {"XPLMGetFMSFlightPlanEntryInfo", (void*) &XPLMGetFMSFlightPlanEntryInfo_ptr},
  {"XPLMSetFMSFlightPlanEntryInfo", (void*) &XPLMSetFMSFlightPlanEntryInfo_ptr},
  {"XPLMSetFMSFlightPlanEntryLatLon", (void*) &XPLMSetFMSFlightPlanEntryLatLon_ptr},
  {"XPLMSetFMSFlightPlanEntryLatLonWithId", (void*) &XPLMSetFMSFlightPlanEntryLatLonWithId_ptr},
  {"XPLMClearFMSFlightPlanEntry", (void*) &XPLMClearFMSFlightPlanEntry_ptr},
  {"XPLMLoadFMSFlightPlan", (void*) &XPLMLoadFMSFlightPlan_ptr},
  {"XPLMSaveFMSFlightPlan", (void*) &XPLMSaveFMSFlightPlan_ptr},
  {NULL, NULL}
};

typeof(XPLMInstanceSetPositionDouble) *XPLMInstanceSetPositionDouble_ptr = NULL;
typeof(XPLMInstanceSetAutoShift) *XPLMInstanceSetAutoShift_ptr = NULL;
typeof(XPLMBeginWeatherUpdate) *XPLMBeginWeatherUpdate_ptr = NULL;
typeof(XPLMEndWeatherUpdate) *XPLMEndWeatherUpdate_ptr = NULL;
typeof(XPLMSetWeatherAtLocation) *XPLMSetWeatherAtLocation_ptr = NULL;
typeof(XPLMSetWeatherAtAirport) *XPLMSetWeatherAtAirport_ptr = NULL;
typeof(XPLMEraseWeatherAtLocation) *XPLMEraseWeatherAtLocation_ptr = NULL;
typeof(XPLMEraseWeatherAtAirport) *XPLMEraseWeatherAtAirport_ptr = NULL;
t_fcn_info funcs420[] = {
  {"XPLMInstanceSetPositionDouble", (void*) &XPLMInstanceSetPositionDouble_ptr},
  {"XPLMInstanceSetAutoShift", (void*) &XPLMInstanceSetAutoShift_ptr},
  {"XPLMBeginWeatherUpdate", (void*) &XPLMBeginWeatherUpdate_ptr},
  {"XPLMEndWeatherUpdate", (void*) &XPLMEndWeatherUpdate_ptr},
  {"XPLMSetWeatherAtLocation", (void*) &XPLMSetWeatherAtLocation_ptr},
  {"XPLMGetWeatherAtLocation", (void*) &XPLMGetWeatherAtLocation_ptr},
  {"XPLMSetWeatherAtAirport", (void*) &XPLMSetWeatherAtAirport_ptr},
  {"XPLMEraseWeatherAtLocation", (void*) &XPLMEraseWeatherAtLocation_ptr},
  {"XPLMEraseWeatherAtAirport", (void*) &XPLMEraseWeatherAtAirport_ptr},
  {NULL, NULL}
};



bool loadFunctions(t_fcn_info *ptr)
{
  void *fun_ptr;
  void *handle;
  bool res = true;

  handle = dlopen(NULL, RTLD_NOW);
  if(handle == NULL){
    fprintf(stderr, "Problem dlopening executable.\n");
    return false;
  }
  while(ptr->name != NULL){
    fun_ptr = dlsym(handle, ptr->name);
    if(fun_ptr != NULL){
      *(ptr->fun_ptr) = fun_ptr;
    }else{
      fprintf(stderr, "Couldn't get address of function '%s'.", ptr->name);
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

  bool res = true;
  if(xplm_ver >= 420){
    res &= loadFunctions(funcs420);
  }
  if(xplm_ver >= 410){
    res &= loadFunctions(funcs410);
  }
  if(xplm_ver >= 400){
    res &= loadFunctions(funcs400);
  }
  if(xplm_ver >= 301){
    res &= loadFunctions(funcs301);
  }
  if(xplm_ver >= 300){
    res &= loadFunctions(funcs300);
  }
  if(xplm_ver >= 210){
    res &= loadFunctions(funcs210);
  }
  pythonDebug("  loadSDKFunctions Loaded.");
  return res;
}

