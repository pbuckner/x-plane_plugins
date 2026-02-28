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

bool loadSDKFunctions(void);

#endif
