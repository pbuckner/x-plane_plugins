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

