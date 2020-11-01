.. automodule:: xp
   :members:             

xp
============

To use::

  from XPPython3 import xp

This module provides simplified mappings to the original C / C++ named functions and constants.
The resulting API is the same -- same parameters, same return values, but
you won't be typing XPLM over-and-over.

To use, add :code:`from XPPython3 import xp` at the top of you file, and all of the SDK API, from all modules
will be available to you. To further reduce typing, function names and constants have been
renamed (shortened) in a consistent manner:


Rules:

* Functions are camelCase. Leading XPLM, XP, XPUI are dropped

.. table::

 ================================== =======================
 Laminar SDK                        import xp
 ================================== =======================
 XPLMCamera.XPLMReadCameraPostion() xp.readCameraPosition()
 XPUIGraphics.XPDrawWindow()        xp.drawWindow()
 ================================== =======================

* Constants start with Capital letter. Leading xp\_, xplm\_, etc. are dropped

.. table::

 ======================================== =======================
 Laminar SDK                              import xp
 ======================================== =======================
 XPLMCamera.xplm_ControlCameraForever     xp.ControlCameraForever
 XPLMDisplay.xplm_MouseUp                 xp.MouseUp
 XPStandardWidgets.xpProperty_ButtonState xp.Property_ButtonState
 XPWidgetDefs.xpMsg_Paint                 xp.Msg_Paint
 XPStandardWidgets.xpMsg_TextFieldChanged xp.Msg_TextFieldChanged
 ======================================== =======================

Compare:

::

 import XPLMDataAccess
 import XPLMInstance

 x = XPLMDataAccess.XPLMGetDatad(XPLMDataAccess.XPLMFindDataRef('sim/flightmodel/position/local_x'))
 y = XPLMDataAccess.XPLMGetDatad(XPLMDataAccess.XPLMFindDataRef('sim/flightmodel/position/local_y'))
 z = XPLMDataAccess.XPLMGetDatad(XPLMDataAccess.XPLMFindDataRef('sim/flightmodel/position/local_z'))
 pitch, heading, roll = (1, 2, 3)
 position = (x, y, z, pitch, heading, roll)
 XPLMInstance.XPLMInstanceSetPosition(self.g_instance,
                                     position,
                                     [self.g_tire, 0.0])

with:

::

 from XPPython3 import xp
 x = xp.getDatad(xp.findDataRef('sim/flightmodel/position/local_x'))
 y = xp.getDatad(xp.findDataRef('sim/flightmodel/position/local_y'))
 z = xp.getDatad(xp.findDataRef('sim/flightmodel/position/local_z'))
 pitch, heading, roll = (1, 2, 3)
 position = (x, y, z, pitch, heading, roll)
 xp.instanceSetPosition(self.g_instance, position, [self.g_tire, 0.0])

Because we're not introducing a new API, you can intermix the two calling styles. :code:`xp.py` is located
in :code:`Resources/plugins/XPPython3/` directory, which will be in your python path for install plugins.

  
.. note:: The remainder of this page lists all of the ``xp`` values and their equivalent ``XPLM`` or ``XP`` function or constant. I don't
          expect you to review this, but it does allow this documentation to provide an index mapping from one to the other. Use the
          *Quick search* field at the top of this page if you're looking for something.

Constants
---------
.. py:data:: AircraftCarrier

   See :py:data:`XPStandardWidgets.xpAircraftCarrier`

.. py:data:: Airport

   See :py:data:`XPStandardWidgets.xp_Airport`

.. py:data:: Building

   See :py:data:`XPStandardWidgets.xpBuilding`

.. py:data:: ButtonBehaviorCheckBox

   See :py:data:`XPStandardWidgets.xpButtonBehaviorCheckBox`

.. py:data:: ButtonBehaviorPushButton

   See :py:data:`XPStandardWidgets.xpButtonBehaviorPushButton`

.. py:data:: ButtonBehaviorRadioButton

   See :py:data:`XPStandardWidgets.xpButtonBehaviorRadioButton`

.. py:data:: CommandBegin

   See :py:data:`XPLMUtilities.xplm_CommandBegin`

.. py:data:: CommandContinue

   See :py:data:`XPLMUtilities.xplm_CommandContinue`

.. py:data:: CommandEnd

   See :py:data:`XPLMUtilities.xplm_CommandEnd`

.. py:data:: ControlCameraForever

   See :py:data:`XPLMCamera.xplm_ControlCameraForever`

.. py:data:: ControlCameraUntilViewChanges

   See :py:data:`XPLMCamera.xplm_ControlCameraUntilViewChanges`

.. py:data:: ControlFlag

   See :py:data:`XPLMDefs.xplm_ControlFlag`

.. py:data:: CoolingTower

   See :py:data:`XPStandardWidgets.xpCoolingTower`

.. py:data:: CursorArrow

   See :py:data:`XPLMDisplay.xplm_CursorArrow`

.. py:data:: CursorCustom

   See :py:data:`XPLMDisplay.xplm_CursorCustom`

.. py:data:: CursorDefault

   See :py:data:`XPLMDisplay.xplm_CursorDefault`

.. py:data:: CursorHidden

   See :py:data:`XPLMDisplay.xplm_CursorHidden`

.. py:data:: CustomObject

   See :py:data:`XPStandardWidgets.xpCustomObject`

.. py:data:: DataFile_ReplayMovie

   See :py:data:`XPLMUtilities.xplm_DataFile_ReplayMovie`

.. py:data:: DataFile_Situation

   See :py:data:`XPLMUtilities.xplm_DataFile_Situation`

.. py:data:: DownFlag

   See :py:data:`XPLMDefs.xplm_DownFlag`

.. py:data:: Element_AircraftCarrier

   See :py:data:`XPUIGraphics.xpElement_AircraftCarrier`

.. py:data:: Element_Airport

   See :py:data:`XPUIGraphics.xpElement_Airport`

.. py:data:: Element_Building

   See :py:data:`XPUIGraphics.xpElement_Building`

.. py:data:: Element_CheckBox

   See :py:data:`XPUIGraphics.xpElement_CheckBox`

.. py:data:: Element_CheckBoxLit

   See :py:data:`XPUIGraphics.xpElement_CheckBoxLit`

.. py:data:: Element_CoolingTower

   See :py:data:`XPUIGraphics.xpElement_CoolingTower`

.. py:data:: Element_CopyButtons

   See :py:data:`XPUIGraphics.xpElement_CopyButtons`

.. py:data:: Element_CopyButtonsWithEditingGrid

   See :py:data:`XPUIGraphics.xpElement_CopyButtonsWithEditingGrid`

.. py:data:: Element_CustomObject

   See :py:data:`XPUIGraphics.xpElement_CustomObject`

.. py:data:: Element_EditingGrid

   See :py:data:`XPUIGraphics.xpElement_EditingGrid`

.. py:data:: Element_Fire

   See :py:data:`XPUIGraphics.xpElement_Fire`

.. py:data:: Element_ILSGlideScope

   See :py:data:`XPUIGraphics.xpElement_ILSGlideScope`

.. py:data:: Element_LittleDownArrow

   See :py:data:`XPUIGraphics.xpElement_LittleDownArrow`

.. py:data:: Element_LittleUpArrow

   See :py:data:`XPUIGraphics.xpElement_LittleUpArrow`

.. py:data:: Element_MarkerLeft

   See :py:data:`XPUIGraphics.xpElement_MarkerLeft`

.. py:data:: Element_MarkerRight

   See :py:data:`XPUIGraphics.xpElement_MarkerRight`

.. py:data:: Element_NDB

   See :py:data:`XPUIGraphics.xpElement_NDB`

.. py:data:: Element_OilPlatform

   See :py:data:`XPUIGraphics.xpElement_OilPlatform`

.. py:data:: Element_OilPlatformSmall

   See :py:data:`XPUIGraphics.xpElement_OilPlatformSmall`

.. py:data:: Element_PowerLine

   See :py:data:`XPUIGraphics.xpElement_PowerLine`

.. py:data:: Element_PushButton

   See :py:data:`XPUIGraphics.xpElement_PushButton`

.. py:data:: Element_PushButtonLit

   See :py:data:`XPUIGraphics.xpElement_PushButtonLit`

.. py:data:: Element_RadioTower

   See :py:data:`XPUIGraphics.xpElement_RadioTower`

.. py:data:: Element_ScrollBar

   See :py:data:`XPUIGraphics.xpElement_ScrollBar`

.. py:data:: Element_Ship

   See :py:data:`XPUIGraphics.xpElement_Ship`

.. py:data:: Element_SmokeStack

   See :py:data:`XPUIGraphics.xpElement_SmokeStack`

.. py:data:: Element_TextField

   See :py:data:`XPUIGraphics.xpElement_TextField`

.. py:data:: Element_TextFieldMiddle

   See :py:data:`XPUIGraphics.xpElement_TextFieldMiddle`

.. py:data:: Element_VOR

   See :py:data:`XPUIGraphics.xpElement_VOR`

.. py:data:: Element_VORWithCompassRose

   See :py:data:`XPUIGraphics.xpElement_VORWithCompassRose`

.. py:data:: Element_Waypoint

   See :py:data:`XPUIGraphics.xpElement_Waypoint`

.. py:data:: Element_WindowCloseBox

   See :py:data:`XPUIGraphics.xpElement_WindowCloseBox`

.. py:data:: Element_WindowCloseBoxPressed

   See :py:data:`XPUIGraphics.xpElement_WindowCloseBoxPressed`

.. py:data:: Element_WindowDragBar

   See :py:data:`XPUIGraphics.xpElement_WindowDragBar`

.. py:data:: Element_WindowDragBarSmooth

   See :py:data:`XPUIGraphics.xpElement_WindowDragBarSmooth`

.. py:data:: Element_Zoomer

   See :py:data:`XPUIGraphics.xpElement_Zoomer`

.. py:data:: Fire

   See :py:data:`XPStandardWidgets.xpFire`

.. py:data:: FlightLoop_Phase_AfterFlightModel

   See :py:data:`XPLMProcessing.xplm_FlightLoop_Phase_AfterFlightModel`

.. py:data:: FlightLoop_Phase_BeforeFlightModel

   See :py:data:`XPLMProcessing.xplm_FlightLoop_Phase_BeforeFlightModel`

.. py:data:: Font_Basic

   See :py:data:`XPLMGraphics.xplmFont_Basic`

.. py:data:: Font_Proportional

   See :py:data:`XPLMGraphics.xplmFont_Proportional`

.. py:data:: Host_Unknown

   See :py:data:`XPLMUtilities.xplm_Host_Unknown`

.. py:data:: Host_XPlane

   See :py:data:`XPLMUtilities.xplm_Host_XPlane`

.. py:data:: ILSGlideScope

   See :py:data:`XPStandardWidgets.xpILSGlideScope`

.. py:data:: KEY_0

   See :py:data:`XPLMDefs.XPLM_KEY_0`

.. py:data:: KEY_1

   See :py:data:`XPLMDefs.XPLM_KEY_1`

.. py:data:: KEY_2

   See :py:data:`XPLMDefs.XPLM_KEY_2`

.. py:data:: KEY_3

   See :py:data:`XPLMDefs.XPLM_KEY_3`

.. py:data:: KEY_4

   See :py:data:`XPLMDefs.XPLM_KEY_4`

.. py:data:: KEY_5

   See :py:data:`XPLMDefs.XPLM_KEY_5`

.. py:data:: KEY_6

   See :py:data:`XPLMDefs.XPLM_KEY_6`

.. py:data:: KEY_7

   See :py:data:`XPLMDefs.XPLM_KEY_7`

.. py:data:: KEY_8

   See :py:data:`XPLMDefs.XPLM_KEY_8`

.. py:data:: KEY_9

   See :py:data:`XPLMDefs.XPLM_KEY_9`

.. py:data:: KEY_DECIMAL

   See :py:data:`XPLMDefs.XPLM_KEY_DECIMAL`

.. py:data:: KEY_DELETE

   See :py:data:`XPLMDefs.XPLM_KEY_DELETE`

.. py:data:: KEY_DOWN

   See :py:data:`XPLMDefs.XPLM_KEY_DOWN`

.. py:data:: KEY_ESCAPE

   See :py:data:`XPLMDefs.XPLM_KEY_ESCAPE`

.. py:data:: KEY_LEFT

   See :py:data:`XPLMDefs.XPLM_KEY_LEFT`

.. py:data:: KEY_RETURN

   See :py:data:`XPLMDefs.XPLM_KEY_RETURN`

.. py:data:: KEY_RIGHT

   See :py:data:`XPLMDefs.XPLM_KEY_RIGHT`

.. py:data:: KEY_TAB

   See :py:data:`XPLMDefs.XPLM_KEY_TAB`

.. py:data:: KEY_UP

   See :py:data:`XPLMDefs.XPLM_KEY_UP`

.. py:data:: Language_Chinese

   See :py:data:`XPLMUtilities.xplm_Language_Chinese`

.. py:data:: Language_English

   See :py:data:`XPLMUtilities.xplm_Language_English`

.. py:data:: Language_French

   See :py:data:`XPLMUtilities.xplm_Language_French`

.. py:data:: Language_German

   See :py:data:`XPLMUtilities.xplm_Language_German`

.. py:data:: Language_Greek

   See :py:data:`XPLMUtilities.xplm_Language_Greek`

.. py:data:: Language_Italian

   See :py:data:`XPLMUtilities.xplm_Language_Italian`

.. py:data:: Language_Japanese

   See :py:data:`XPLMUtilities.xplm_Language_Japanese`

.. py:data:: Language_Korean

   See :py:data:`XPLMUtilities.xplm_Language_Korean`

.. py:data:: Language_Russian

   See :py:data:`XPLMUtilities.xplm_Language_Russian`

.. py:data:: Language_Spanish

   See :py:data:`XPLMUtilities.xplm_Language_Spanish`

.. py:data:: Language_Unknown

   See :py:data:`XPLMUtilities.xplm_Language_Unknown`

.. py:data:: LittleDownArrow

   See :py:data:`XPStandardWidgets.xpLittleDownArrow`

.. py:data:: LittleUpArrow

   See :py:data:`XPStandardWidgets.xpLittleUpArrow`

.. py:data:: MAP_IOS

   See :py:data:`XPLMMap.XPLM_MAP_IOS`

.. py:data:: MAP_USER_INTERFACE

   See :py:data:`XPLMMap.XPLM_MAP_USER_INTERFACE`

.. py:data:: MSG_AIRPLANE_COUNT_CHANGED

   See :py:data:`XPLMPlugin.XPLM_MSG_AIRPLANE_COUNT_CHANGED`

.. py:data:: MSG_AIRPORT_LOADED

   See :py:data:`XPLMPlugin.XPLM_MSG_AIRPORT_LOADED`

.. py:data:: MSG_ENTERED_VR

   See :py:data:`XPLMPlugin.XPLM_MSG_ENTERED_VR`

.. py:data:: MSG_EXITING_VR

   See :py:data:`XPLMPlugin.XPLM_MSG_EXITING_VR`

.. py:data:: MSG_LIVERY_LOADED

   See :py:data:`XPLMPlugin.XPLM_MSG_LIVERY_LOADED`

.. py:data:: MSG_PLANE_CRASHED

   See :py:data:`XPLMPlugin.XPLM_MSG_PLANE_CRASHED`

.. py:data:: MSG_PLANE_LOADED

   See :py:data:`XPLMPlugin.XPLM_MSG_PLANE_LOADED`

.. py:data:: MSG_PLANE_UNLOADED

   See :py:data:`XPLMPlugin.XPLM_MSG_PLANE_UNLOADED`

.. py:data:: MSG_SCENERY_LOADED

   See :py:data:`XPLMPlugin.XPLM_MSG_SCENERY_LOADED`

.. py:data:: MSG_WILL_WRITE_PREFS

   See :py:data:`XPLMPlugin.XPLM_MSG_WILL_WRITE_PREFS`

.. py:data:: MainWindowStyle_MainWindow

   See :py:data:`XPStandardWidgets.xpMainWindowStyle_MainWindow`

.. py:data:: MainWindowStyle_Translucent

   See :py:data:`XPStandardWidgets.xpMainWindowStyle_Translucent`

.. py:data:: MapLayer_Fill

   See :py:data:`XPLMMap.xplm_MapLayer_Fill`

.. py:data:: MapLayer_Markings

   See :py:data:`XPLMMap.xplm_MapLayer_Markings`

.. py:data:: MapOrientation_Map

   See :py:data:`XPLMMap.xplm_MapOrientation_Map`

.. py:data:: MapOrientation_UI

   See :py:data:`XPLMMap.xplm_MapOrientation_UI`

.. py:data:: MapStyle_IFR_HighEnroute

   See :py:data:`XPLMMap.xplm_MapStyle_IFR_HighEnroute`

.. py:data:: MapStyle_IFR_LowEnroute

   See :py:data:`XPLMMap.xplm_MapStyle_IFR_LowEnroute`

.. py:data:: MapStyle_VFR_Sectional

   See :py:data:`XPLMMap.xplm_MapStyle_VFR_Sectional`

.. py:data:: MarkerLeft

   See :py:data:`XPStandardWidgets.xpMarkerLeft`

.. py:data:: MarkerRight

   See :py:data:`XPStandardWidgets.xpMarkerRight`

.. py:data:: Menu_Checked

   See :py:data:`XPLMMenus.xplm_Menu_Checked`

.. py:data:: Menu_NoCheck

   See :py:data:`XPLMMenus.xplm_Menu_NoCheck`

.. py:data:: Menu_Unchecked

   See :py:data:`XPLMMenus.xplm_Menu_Unchecked`

.. py:data:: Message_CloseButtonPushed

   See :py:data:`XPStandardWidgets.xpMessage_CloseButtonPushed`

.. py:data:: Mode_Direct

   See :py:data:`XPWidgetDefs.xpMode_Direct`

.. py:data:: Mode_DirectAllCallbacks

   See :py:data:`XPWidgetDefs.xpMode_DirectAllCallbacks`

.. py:data:: Mode_Once

   See :py:data:`XPWidgetDefs.xpMode_Once`

.. py:data:: Mode_Recursive

   See :py:data:`XPWidgetDefs.xpMode_Recursive`

.. py:data:: Mode_UpChain

   See :py:data:`XPWidgetDefs.xpMode_UpChain`

.. py:data:: MouseDown

   See :py:data:`XPLMDisplay.xplm_MouseDown`

.. py:data:: MouseDrag

   See :py:data:`XPLMDisplay.xplm_MouseDrag`

.. py:data:: MouseUp

   See :py:data:`XPLMDisplay.xplm_MouseUp`

.. py:data:: MsgAirplaneCountChanged

   See :py:data:`XPLMPlugin.XPLM_MSG_AIRPLANE_COUNT_CHANGED`

.. py:data:: MsgAirportLoaded

   See :py:data:`XPLMPlugin.XPLM_MSG_AIRPORT_LOADED`

.. py:data:: MsgEnteredVR

   See :py:data:`XPLMPlugin.XPLM_MSG_ENTERED_VR`

.. py:data:: MsgExitingVR

   See :py:data:`XPLMPlugin.XPLM_MSG_EXITING_VR`

.. py:data:: MsgLiveryLoaded

   See :py:data:`XPLMPlugin.XPLM_MSG_LIVERY_LOADED`

.. py:data:: MsgPlaneCrashed

   See :py:data:`XPLMPlugin.XPLM_MSG_PLANE_CRASHED`

.. py:data:: MsgPlaneLoaded

   See :py:data:`XPLMPlugin.XPLM_MSG_PLANE_LOADED`

.. py:data:: MsgPlaneUnloaded

   See :py:data:`XPLMPlugin.XPLM_MSG_PLANE_UNLOADED`

.. py:data:: MsgSceneryLoaded

   See :py:data:`XPLMPlugin.XPLM_MSG_SCENERY_LOADED`

.. py:data:: MsgWillWritePrefs

   See :py:data:`XPLMPlugin.XPLM_MSG_WILL_WRITE_PREFS`

.. py:data:: Msg_AcceptChild

   See :py:data:`XPWidgetDefs.xpMsg_AcceptChild`

.. py:data:: Msg_AcceptParent

   See :py:data:`XPWidgetDefs.xpMsg_AcceptParent`

.. py:data:: Msg_ButtonStateChanged

   See :py:data:`XPStandardWidgets.xpMsg_ButtonStateChanged`

.. py:data:: Msg_Create

   See :py:data:`XPWidgetDefs.xpMsg_Create`

.. py:data:: Msg_CursorAdjust

   See :py:data:`XPWidgetDefs.xpMsg_CursorAdjust`

.. py:data:: Msg_DescriptorChanged

   See :py:data:`XPWidgetDefs.xpMsg_DescriptorChanged`

.. py:data:: Msg_Destroy

   See :py:data:`XPWidgetDefs.xpMsg_Destroy`

.. py:data:: Msg_Draw

   See :py:data:`XPWidgetDefs.xpMsg_Draw`

.. py:data:: Msg_ExposedChanged

   See :py:data:`XPWidgetDefs.xpMsg_ExposedChanged`

.. py:data:: Msg_Hidden

   See :py:data:`XPWidgetDefs.xpMsg_Hidden`

.. py:data:: Msg_KeyLoseFocus

   See :py:data:`XPWidgetDefs.xpMsg_KeyLoseFocus`

.. py:data:: Msg_KeyPress

   See :py:data:`XPWidgetDefs.xpMsg_KeyPress`

.. py:data:: Msg_KeyTakeFocus

   See :py:data:`XPWidgetDefs.xpMsg_KeyTakeFocus`

.. py:data:: Msg_LoseChild

   See :py:data:`XPWidgetDefs.xpMsg_LoseChild`

.. py:data:: Msg_MouseDown

   See :py:data:`XPWidgetDefs.xpMsg_MouseDown`

.. py:data:: Msg_MouseDrag

   See :py:data:`XPWidgetDefs.xpMsg_MouseDrag`

.. py:data:: Msg_MouseUp

   See :py:data:`XPWidgetDefs.xpMsg_MouseUp`

.. py:data:: Msg_MouseWheel

   See :py:data:`XPWidgetDefs.xpMsg_MouseWheel`

.. py:data:: Msg_None

   See :py:data:`XPWidgetDefs.xpMsg_None`

.. py:data:: Msg_Paint

   See :py:data:`XPWidgetDefs.xpMsg_Paint`

.. py:data:: Msg_PropertyChanged

   See :py:data:`XPWidgetDefs.xpMsg_PropertyChanged`

.. py:data:: Msg_PushButtonPressed

   See :py:data:`XPStandardWidgets.xpMsg_PushButtonPressed`

.. py:data:: Msg_Reshape

   See :py:data:`XPWidgetDefs.xpMsg_Reshape`

.. py:data:: Msg_ScrollBarSliderPositionChanged

   See :py:data:`XPStandardWidgets.xpMsg_ScrollBarSliderPositionChanged`

.. py:data:: Msg_Shown

   See :py:data:`XPWidgetDefs.xpMsg_Shown`

.. py:data:: Msg_TextFieldChanged

   See :py:data:`XPStandardWidgets.xpMsg_TextFieldChanged`

.. py:data:: Msg_UserStart

   See :py:data:`XPWidgetDefs.xpMsg_UserStart`

.. py:data:: NAV_NOT_FOUND

   See :py:data:`XPLMNavigation.XPLM_NAV_NOT_FOUND`

.. py:data:: NDB

   See :py:data:`XPStandardWidgets.xpNDB`

.. py:data:: NO_PLUGIN_ID

   See :py:data:`XPLMDefs.XPLM_NO_PLUGIN_ID`

.. py:data:: Nav_Airport

   See :py:data:`XPLMNavigation.xplm_Nav_Airport`

.. py:data:: Nav_DME

   See :py:data:`XPLMNavigation.xplm_Nav_DME`

.. py:data:: Nav_Fix

   See :py:data:`XPLMNavigation.xplm_Nav_Fix`

.. py:data:: Nav_GlideSlope

   See :py:data:`XPLMNavigation.xplm_Nav_GlideSlope`

.. py:data:: Nav_ILS

   See :py:data:`XPLMNavigation.xplm_Nav_ILS`

.. py:data:: Nav_InnerMarker

   See :py:data:`XPLMNavigation.xplm_Nav_InnerMarker`

.. py:data:: Nav_LatLon

   See :py:data:`XPLMNavigation.xplm_Nav_LatLon`

.. py:data:: Nav_Localizer

   See :py:data:`XPLMNavigation.xplm_Nav_Localizer`

.. py:data:: Nav_MiddleMarker

   See :py:data:`XPLMNavigation.xplm_Nav_MiddleMarker`

.. py:data:: Nav_NDB

   See :py:data:`XPLMNavigation.xplm_Nav_NDB`

.. py:data:: Nav_OuterMarker

   See :py:data:`XPLMNavigation.xplm_Nav_OuterMarker`

.. py:data:: Nav_Unknown

   See :py:data:`XPLMNavigation.xplm_Nav_Unknown`

.. py:data:: Nav_VOR

   See :py:data:`XPLMNavigation.xplm_Nav_VOR`

.. py:data:: OilPlatform

   See :py:data:`XPStandardWidgets.xpOilPlatform`

.. py:data:: OilPlatformSmall

   See :py:data:`XPStandardWidgets.xpOilPlatformSmall`

.. py:data:: OptionAltFlag

   See :py:data:`XPLMDefs.xplm_OptionAltFlag`

.. py:data:: PLUGIN_XPLANE

   See :py:data:`XPLMDefs.XPLM_PLUGIN_XPLANE`

.. py:data:: Phase_FirstCockpit

   See :py:data:`XPLMDisplay.xplm_Phase_FirstCockpit`

.. py:data:: Phase_Gauges

   See :py:data:`XPLMDisplay.xplm_Phase_Gauges`

.. py:data:: Phase_LastCockpit

   See :py:data:`XPLMDisplay.xplm_Phase_LastCockpit`

.. py:data:: Phase_Modern3D

   See :py:data:`XPLMDisplay.xplm_Phase_Modern3D`

.. py:data:: Phase_Panel

   See :py:data:`XPLMDisplay.xplm_Phase_Panel`

.. py:data:: Phase_Window

   See :py:data:`XPLMDisplay.xplm_Phase_Window`

.. py:data:: PowerLine

   See :py:data:`XPStandardWidgets.xpPowerLine`

.. py:data:: ProbeError

   See :py:data:`XPLMScenery.xplm_ProbeError`

.. py:data:: ProbeHitTerrain

   See :py:data:`XPLMScenery.xplm_ProbeHitTerrain`

.. py:data:: ProbeMissed

   See :py:data:`XPLMScenery.xplm_ProbeMissed`

.. py:data:: ProbeY

   See :py:data:`XPLMScenery.xplm_ProbeY`

.. py:data:: Property_ActiveEditSide

   See :py:data:`XPStandardWidgets.xpProperty_ActiveEditSide`

.. py:data:: Property_ButtonBehavior

   See :py:data:`XPStandardWidgets.xpProperty_ButtonBehavior`

.. py:data:: Property_ButtonState

   See :py:data:`XPStandardWidgets.xpProperty_ButtonState`

.. py:data:: Property_ButtonType

   See :py:data:`XPStandardWidgets.xpProperty_ButtonType`

.. py:data:: Property_CaptionLit

   See :py:data:`XPStandardWidgets.xpProperty_CaptionLit`

.. py:data:: Property_Clip

   See :py:data:`XPWidgetDefs.xpProperty_Clip`

.. py:data:: Property_DragXOff

   See :py:data:`XPWidgetDefs.xpProperty_DragXOff`

.. py:data:: Property_DragYOff

   See :py:data:`XPWidgetDefs.xpProperty_DragYOff`

.. py:data:: Property_Dragging

   See :py:data:`XPWidgetDefs.xpProperty_Dragging`

.. py:data:: Property_EditFieldSelDragStart

   See :py:data:`XPStandardWidgets.xpProperty_EditFieldSelDragStart`

.. py:data:: Property_EditFieldSelEnd

   See :py:data:`XPStandardWidgets.xpProperty_EditFieldSelEnd`

.. py:data:: Property_EditFieldSelStart

   See :py:data:`XPStandardWidgets.xpProperty_EditFieldSelStart`

.. py:data:: Property_Enabled

   See :py:data:`XPWidgetDefs.xpProperty_Enabled`

.. py:data:: Property_Font

   See :py:data:`XPStandardWidgets.xpProperty_Font`

.. py:data:: Property_GeneralGraphicsType

   See :py:data:`XPStandardWidgets.xpProperty_GeneralGraphicsType`

.. py:data:: Property_Hilited

   See :py:data:`XPWidgetDefs.xpProperty_Hilited`

.. py:data:: Property_MainWindowHasCloseBoxes

   See :py:data:`XPStandardWidgets.xpProperty_MainWindowHasCloseBoxes`

.. py:data:: Property_MainWindowType

   See :py:data:`XPStandardWidgets.xpProperty_MainWindowType`

.. py:data:: Property_MaxCharacters

   See :py:data:`XPStandardWidgets.xpProperty_MaxCharacters`

.. py:data:: Property_Object

   See :py:data:`XPWidgetDefs.xpProperty_Object`

.. py:data:: Property_PasswordMode

   See :py:data:`XPStandardWidgets.xpProperty_PasswordMode`

.. py:data:: Property_ProgressMax

   See :py:data:`XPStandardWidgets.xpProperty_ProgressMax`

.. py:data:: Property_ProgressMin

   See :py:data:`XPStandardWidgets.xpProperty_ProgressMin`

.. py:data:: Property_ProgressPosition

   See :py:data:`XPStandardWidgets.xpProperty_ProgressPosition`

.. py:data:: Property_Refcon

   See :py:data:`XPWidgetDefs.xpProperty_Refcon`

.. py:data:: Property_ScrollBarMax

   See :py:data:`XPStandardWidgets.xpProperty_ScrollBarMax`

.. py:data:: Property_ScrollBarMin

   See :py:data:`XPStandardWidgets.xpProperty_ScrollBarMin`

.. py:data:: Property_ScrollBarPageAmount

   See :py:data:`XPStandardWidgets.xpProperty_ScrollBarPageAmount`

.. py:data:: Property_ScrollBarSliderPosition

   See :py:data:`XPStandardWidgets.xpProperty_ScrollBarSliderPosition`

.. py:data:: Property_ScrollBarSlop

   See :py:data:`XPStandardWidgets.xpProperty_ScrollBarSlop`

.. py:data:: Property_ScrollBarType

   See :py:data:`XPStandardWidgets.xpProperty_ScrollBarType`

.. py:data:: Property_ScrollPosition

   See :py:data:`XPStandardWidgets.xpProperty_ScrollPosition`

.. py:data:: Property_SubWindowType

   See :py:data:`XPStandardWidgets.xpProperty_SubWindowType`

.. py:data:: Property_TextFieldType

   See :py:data:`XPStandardWidgets.xpProperty_TextFieldType`

.. py:data:: Property_UserStart

   See :py:data:`XPWidgetDefs.xpProperty_UserStart`

.. py:data:: PushButton

   See :py:data:`XPStandardWidgets.xpPushButton`

.. py:data:: pythonExecutable
                 
  See :py:data:`XPPython.pythonExecutable`
  
.. py:data:: RadioButton

   See :py:data:`XPStandardWidgets.xpRadioButton`

.. py:data:: RadioTower

   See :py:data:`XPStandardWidgets.xpRadioTower`

.. py:data:: ScrollBarTypeScrollBar

   See :py:data:`XPStandardWidgets.xpScrollBarTypeScrollBar`

.. py:data:: ScrollBarTypeSlider

   See :py:data:`XPStandardWidgets.xpScrollBarTypeSlider`

.. py:data:: ShiftFlag

   See :py:data:`XPLMDefs.xplm_ShiftFlag`

.. py:data:: Ship

   See :py:data:`XPStandardWidgets.xpShip`

.. py:data:: SmokeStack

   See :py:data:`XPStandardWidgets.xpSmokeStack`

.. py:data:: SubWindowStyle_ListView

   See :py:data:`XPStandardWidgets.xpSubWindowStyle_ListView`

.. py:data:: SubWindowStyle_Screen

   See :py:data:`XPStandardWidgets.xpSubWindowStyle_Screen`

.. py:data:: SubWindowStyle_SubWindow

   See :py:data:`XPStandardWidgets.xpSubWindowStyle_SubWindow`

.. py:data:: TextEntryField

   See :py:data:`XPStandardWidgets.xpTextEntryField`

.. py:data:: TextTranslucent

   See :py:data:`XPStandardWidgets.xpTextTranslucent`

.. py:data:: TextTransparent

   See :py:data:`XPStandardWidgets.xpTextTransparent`

.. py:data:: Track_Progress

   See :py:data:`XPUIGraphics.xpTrack_Progress`

.. py:data:: Track_ScrollBar

   See :py:data:`XPUIGraphics.xpTrack_ScrollBar`

.. py:data:: Track_Slider

   See :py:data:`XPUIGraphics.xpTrack_Slider`

.. py:data:: Type_Data

   See :py:data:`XPLMDataAccess.xplmType_Data`

.. py:data:: Type_Double

   See :py:data:`XPLMDataAccess.xplmType_Double`

.. py:data:: Type_Float

   See :py:data:`XPLMDataAccess.xplmType_Float`

.. py:data:: Type_FloatArray

   See :py:data:`XPLMDataAccess.xplmType_FloatArray`

.. py:data:: Type_Int

   See :py:data:`XPLMDataAccess.xplmType_Int`

.. py:data:: Type_IntArray

   See :py:data:`XPLMDataAccess.xplmType_IntArray`

.. py:data:: Type_Unknown

   See :py:data:`XPLMDataAccess.xplmType_Unknown`

.. py:data:: USER_AIRCRAFT

   See :py:data:`XPLMPlanes.XPLM_USER_AIRCRAFT`

.. py:data:: UpFlag

   See :py:data:`XPLMDefs.xplm_UpFlag`

.. py:data:: VK_0

   See :py:data:`XPLMDefs.XPLM_VK_0`

.. py:data:: VK_1

   See :py:data:`XPLMDefs.XPLM_VK_1`

.. py:data:: VK_2

   See :py:data:`XPLMDefs.XPLM_VK_2`

.. py:data:: VK_3

   See :py:data:`XPLMDefs.XPLM_VK_3`

.. py:data:: VK_4

   See :py:data:`XPLMDefs.XPLM_VK_4`

.. py:data:: VK_5

   See :py:data:`XPLMDefs.XPLM_VK_5`

.. py:data:: VK_6

   See :py:data:`XPLMDefs.XPLM_VK_6`

.. py:data:: VK_7

   See :py:data:`XPLMDefs.XPLM_VK_7`

.. py:data:: VK_8

   See :py:data:`XPLMDefs.XPLM_VK_8`

.. py:data:: VK_9

   See :py:data:`XPLMDefs.XPLM_VK_9`

.. py:data:: VK_A

   See :py:data:`XPLMDefs.XPLM_VK_A`

.. py:data:: VK_ADD

   See :py:data:`XPLMDefs.XPLM_VK_ADD`

.. py:data:: VK_B

   See :py:data:`XPLMDefs.XPLM_VK_B`

.. py:data:: VK_BACK

   See :py:data:`XPLMDefs.XPLM_VK_BACK`

.. py:data:: VK_BACKQUOTE

   See :py:data:`XPLMDefs.XPLM_VK_BACKQUOTE`

.. py:data:: VK_BACKSLASH

   See :py:data:`XPLMDefs.XPLM_VK_BACKSLASH`

.. py:data:: VK_C

   See :py:data:`XPLMDefs.XPLM_VK_C`

.. py:data:: VK_CLEAR

   See :py:data:`XPLMDefs.XPLM_VK_CLEAR`

.. py:data:: VK_COMMA

   See :py:data:`XPLMDefs.XPLM_VK_COMMA`

.. py:data:: VK_D

   See :py:data:`XPLMDefs.XPLM_VK_D`

.. py:data:: VK_DECIMAL

   See :py:data:`XPLMDefs.XPLM_VK_DECIMAL`

.. py:data:: VK_DELETE

   See :py:data:`XPLMDefs.XPLM_VK_DELETE`

.. py:data:: VK_DIVIDE

   See :py:data:`XPLMDefs.XPLM_VK_DIVIDE`

.. py:data:: VK_DOWN

   See :py:data:`XPLMDefs.XPLM_VK_DOWN`

.. py:data:: VK_E

   See :py:data:`XPLMDefs.XPLM_VK_E`

.. py:data:: VK_END

   See :py:data:`XPLMDefs.XPLM_VK_END`

.. py:data:: VK_ENTER

   See :py:data:`XPLMDefs.XPLM_VK_ENTER`

.. py:data:: VK_EQUAL

   See :py:data:`XPLMDefs.XPLM_VK_EQUAL`

.. py:data:: VK_ESCAPE

   See :py:data:`XPLMDefs.XPLM_VK_ESCAPE`

.. py:data:: VK_EXECUTE

   See :py:data:`XPLMDefs.XPLM_VK_EXECUTE`

.. py:data:: VK_F

   See :py:data:`XPLMDefs.XPLM_VK_F`

.. py:data:: VK_F1

   See :py:data:`XPLMDefs.XPLM_VK_F1`

.. py:data:: VK_F10

   See :py:data:`XPLMDefs.XPLM_VK_F10`

.. py:data:: VK_F11

   See :py:data:`XPLMDefs.XPLM_VK_F11`

.. py:data:: VK_F12

   See :py:data:`XPLMDefs.XPLM_VK_F12`

.. py:data:: VK_F13

   See :py:data:`XPLMDefs.XPLM_VK_F13`

.. py:data:: VK_F14

   See :py:data:`XPLMDefs.XPLM_VK_F14`

.. py:data:: VK_F15

   See :py:data:`XPLMDefs.XPLM_VK_F15`

.. py:data:: VK_F16

   See :py:data:`XPLMDefs.XPLM_VK_F16`

.. py:data:: VK_F17

   See :py:data:`XPLMDefs.XPLM_VK_F17`

.. py:data:: VK_F18

   See :py:data:`XPLMDefs.XPLM_VK_F18`

.. py:data:: VK_F19

   See :py:data:`XPLMDefs.XPLM_VK_F19`

.. py:data:: VK_F2

   See :py:data:`XPLMDefs.XPLM_VK_F2`

.. py:data:: VK_F20

   See :py:data:`XPLMDefs.XPLM_VK_F20`

.. py:data:: VK_F21

   See :py:data:`XPLMDefs.XPLM_VK_F21`

.. py:data:: VK_F22

   See :py:data:`XPLMDefs.XPLM_VK_F22`

.. py:data:: VK_F23

   See :py:data:`XPLMDefs.XPLM_VK_F23`

.. py:data:: VK_F24

   See :py:data:`XPLMDefs.XPLM_VK_F24`

.. py:data:: VK_F3

   See :py:data:`XPLMDefs.XPLM_VK_F3`

.. py:data:: VK_F4

   See :py:data:`XPLMDefs.XPLM_VK_F4`

.. py:data:: VK_F5

   See :py:data:`XPLMDefs.XPLM_VK_F5`

.. py:data:: VK_F6

   See :py:data:`XPLMDefs.XPLM_VK_F6`

.. py:data:: VK_F7

   See :py:data:`XPLMDefs.XPLM_VK_F7`

.. py:data:: VK_F8

   See :py:data:`XPLMDefs.XPLM_VK_F8`

.. py:data:: VK_F9

   See :py:data:`XPLMDefs.XPLM_VK_F9`

.. py:data:: VK_G

   See :py:data:`XPLMDefs.XPLM_VK_G`

.. py:data:: VK_H

   See :py:data:`XPLMDefs.XPLM_VK_H`

.. py:data:: VK_HELP

   See :py:data:`XPLMDefs.XPLM_VK_HELP`

.. py:data:: VK_HOME

   See :py:data:`XPLMDefs.XPLM_VK_HOME`

.. py:data:: VK_I

   See :py:data:`XPLMDefs.XPLM_VK_I`

.. py:data:: VK_INSERT

   See :py:data:`XPLMDefs.XPLM_VK_INSERT`

.. py:data:: VK_J

   See :py:data:`XPLMDefs.XPLM_VK_J`

.. py:data:: VK_K

   See :py:data:`XPLMDefs.XPLM_VK_K`

.. py:data:: VK_L

   See :py:data:`XPLMDefs.XPLM_VK_L`

.. py:data:: VK_LBRACE

   See :py:data:`XPLMDefs.XPLM_VK_LBRACE`

.. py:data:: VK_LEFT

   See :py:data:`XPLMDefs.XPLM_VK_LEFT`

.. py:data:: VK_M

   See :py:data:`XPLMDefs.XPLM_VK_M`

.. py:data:: VK_MINUS

   See :py:data:`XPLMDefs.XPLM_VK_MINUS`

.. py:data:: VK_MULTIPLY

   See :py:data:`XPLMDefs.XPLM_VK_MULTIPLY`

.. py:data:: VK_N

   See :py:data:`XPLMDefs.XPLM_VK_N`

.. py:data:: VK_NEXT

   See :py:data:`XPLMDefs.XPLM_VK_NEXT`

.. py:data:: VK_NUMPAD0

   See :py:data:`XPLMDefs.XPLM_VK_NUMPAD0`

.. py:data:: VK_NUMPAD1

   See :py:data:`XPLMDefs.XPLM_VK_NUMPAD1`

.. py:data:: VK_NUMPAD2

   See :py:data:`XPLMDefs.XPLM_VK_NUMPAD2`

.. py:data:: VK_NUMPAD3

   See :py:data:`XPLMDefs.XPLM_VK_NUMPAD3`

.. py:data:: VK_NUMPAD4

   See :py:data:`XPLMDefs.XPLM_VK_NUMPAD4`

.. py:data:: VK_NUMPAD5

   See :py:data:`XPLMDefs.XPLM_VK_NUMPAD5`

.. py:data:: VK_NUMPAD6

   See :py:data:`XPLMDefs.XPLM_VK_NUMPAD6`

.. py:data:: VK_NUMPAD7

   See :py:data:`XPLMDefs.XPLM_VK_NUMPAD7`

.. py:data:: VK_NUMPAD8

   See :py:data:`XPLMDefs.XPLM_VK_NUMPAD8`

.. py:data:: VK_NUMPAD9

   See :py:data:`XPLMDefs.XPLM_VK_NUMPAD9`

.. py:data:: VK_NUMPAD_ENT

   See :py:data:`XPLMDefs.XPLM_VK_NUMPAD_ENT`

.. py:data:: VK_NUMPAD_EQ

   See :py:data:`XPLMDefs.XPLM_VK_NUMPAD_EQ`

.. py:data:: VK_O

   See :py:data:`XPLMDefs.XPLM_VK_O`

.. py:data:: VK_P

   See :py:data:`XPLMDefs.XPLM_VK_P`

.. py:data:: VK_PERIOD

   See :py:data:`XPLMDefs.XPLM_VK_PERIOD`

.. py:data:: VK_PRINT

   See :py:data:`XPLMDefs.XPLM_VK_PRINT`

.. py:data:: VK_PRIOR

   See :py:data:`XPLMDefs.XPLM_VK_PRIOR`

.. py:data:: VK_Q

   See :py:data:`XPLMDefs.XPLM_VK_Q`

.. py:data:: VK_QUOTE

   See :py:data:`XPLMDefs.XPLM_VK_QUOTE`

.. py:data:: VK_R

   See :py:data:`XPLMDefs.XPLM_VK_R`

.. py:data:: VK_RBRACE

   See :py:data:`XPLMDefs.XPLM_VK_RBRACE`

.. py:data:: VK_RETURN

   See :py:data:`XPLMDefs.XPLM_VK_RETURN`

.. py:data:: VK_RIGHT

   See :py:data:`XPLMDefs.XPLM_VK_RIGHT`

.. py:data:: VK_S

   See :py:data:`XPLMDefs.XPLM_VK_S`

.. py:data:: VK_SELECT

   See :py:data:`XPLMDefs.XPLM_VK_SELECT`

.. py:data:: VK_SEMICOLON

   See :py:data:`XPLMDefs.XPLM_VK_SEMICOLON`

.. py:data:: VK_SEPARATOR

   See :py:data:`XPLMDefs.XPLM_VK_SEPARATOR`

.. py:data:: VK_SLASH

   See :py:data:`XPLMDefs.XPLM_VK_SLASH`

.. py:data:: VK_SNAPSHOT

   See :py:data:`XPLMDefs.XPLM_VK_SNAPSHOT`

.. py:data:: VK_SPACE

   See :py:data:`XPLMDefs.XPLM_VK_SPACE`

.. py:data:: VK_SUBTRACT

   See :py:data:`XPLMDefs.XPLM_VK_SUBTRACT`

.. py:data:: VK_T

   See :py:data:`XPLMDefs.XPLM_VK_T`

.. py:data:: VK_TAB

   See :py:data:`XPLMDefs.XPLM_VK_TAB`

.. py:data:: VK_U

   See :py:data:`XPLMDefs.XPLM_VK_U`

.. py:data:: VK_UP

   See :py:data:`XPLMDefs.XPLM_VK_UP`

.. py:data:: VK_V

   See :py:data:`XPLMDefs.XPLM_VK_V`

.. py:data:: VK_W

   See :py:data:`XPLMDefs.XPLM_VK_W`

.. py:data:: VK_X

   See :py:data:`XPLMDefs.XPLM_VK_X`

.. py:data:: VK_Y

   See :py:data:`XPLMDefs.XPLM_VK_Y`

.. py:data:: VK_Z

   See :py:data:`XPLMDefs.XPLM_VK_Z`

.. py:data:: VOR

   See :py:data:`XPStandardWidgets.xpVOR`

.. py:data:: VORWithCompassRose

   See :py:data:`XPStandardWidgets.xpVORWithCompassRose`

.. py:data:: WayPoint

   See :py:data:`XPStandardWidgets.xpWayPoint`

.. py:data:: WidgetClass_Button

   See :py:data:`XPStandardWidgets.xpWidgetClass_Button`

.. py:data:: WidgetClass_Caption

   See :py:data:`XPStandardWidgets.xpWidgetClass_Caption`

.. py:data:: WidgetClass_GeneralGraphics

   See :py:data:`XPStandardWidgets.xpWidgetClass_GeneralGraphics`

.. py:data:: WidgetClass_MainWindow

   See :py:data:`XPStandardWidgets.xpWidgetClass_MainWindow`

.. py:data:: WidgetClass_None

   See :py:data:`XPWidgetDefs.xpWidgetClass_None`

.. py:data:: WidgetClass_Progress

   See :py:data:`XPStandardWidgets.xpWidgetClass_Progress`

.. py:data:: WidgetClass_ScrollBar

   See :py:data:`XPStandardWidgets.xpWidgetClass_ScrollBar`

.. py:data:: WidgetClass_SubWindow

   See :py:data:`XPStandardWidgets.xpWidgetClass_SubWindow`

.. py:data:: WidgetClass_TextField

   See :py:data:`XPStandardWidgets.xpWidgetClass_TextField`

.. py:data:: WindowCenterOnMonitor

   See :py:data:`XPLMDisplay.xplm_WindowCenterOnMonitor`

.. py:data:: WindowCloseBox

   See :py:data:`XPStandardWidgets.xpWindowCloseBox`

.. py:data:: WindowDecorationNone

   See :py:data:`XPLMDisplay.xplm_WindowDecorationNone`

.. py:data:: WindowDecorationRoundRectangle

   See :py:data:`XPLMDisplay.xplm_WindowDecorationRoundRectangle`

.. py:data:: WindowDecorationSelfDecorated

   See :py:data:`XPLMDisplay.xplm_WindowDecorationSelfDecorated`

.. py:data:: WindowDecorationSelfDecoratedResizable

   See :py:data:`XPLMDisplay.xplm_WindowDecorationSelfDecoratedResizable`

.. py:data:: WindowFullScreenOnAllMonitors

   See :py:data:`XPLMDisplay.xplm_WindowFullScreenOnAllMonitors`

.. py:data:: WindowFullScreenOnMonitor

   See :py:data:`XPLMDisplay.xplm_WindowFullScreenOnMonitor`

.. py:data:: WindowLayerFlightOverlay

   See :py:data:`XPLMDisplay.xplm_WindowLayerFlightOverlay`

.. py:data:: WindowLayerFloatingWindows

   See :py:data:`XPLMDisplay.xplm_WindowLayerFloatingWindows`

.. py:data:: WindowLayerGrowlNotifications

   See :py:data:`XPLMDisplay.xplm_WindowLayerGrowlNotifications`

.. py:data:: WindowLayerModal

   See :py:data:`XPLMDisplay.xplm_WindowLayerModal`

.. py:data:: WindowPopOut

   See :py:data:`XPLMDisplay.xplm_WindowPopOut`

.. py:data:: WindowPositionFree

   See :py:data:`XPLMDisplay.xplm_WindowPositionFree`

.. py:data:: WindowVR

   See :py:data:`XPLMDisplay.xplm_WindowVR`

.. py:data:: Window_Help

   See :py:data:`XPUIGraphics.xpWindow_Help`

.. py:data:: Window_ListView

   See :py:data:`XPUIGraphics.xpWindow_ListView`

.. py:data:: Window_MainWindow

   See :py:data:`XPUIGraphics.xpWindow_MainWindow`

.. py:data:: Window_Screen

   See :py:data:`XPUIGraphics.xpWindow_Screen`

.. py:data:: Window_SubWindow

   See :py:data:`XPUIGraphics.xpWindow_SubWindow`


Functions
---------

.. py:function:: acquirePlanes()

  See :func:`XPLMPlanes.XPLMAcquirePlanes`
  
.. py:function:: addWidgetCallback()

  See :func:`XPWidgets.XPAddWidgetCallback`
  
.. py:function:: appendMenuItem()

  See :func:`XPLMMenus.XPLMAppendMenuItem`
  
.. py:function:: appendMenuItemWithCommand()

  See :func:`XPLMMenus.XPLMAppendMenuItemWithCommand`
  
.. py:function:: appendMenuSeparator()

  See :func:`XPLMMenus.XPLMAppendMenuSeparator`
  
.. py:function:: bindTexture2d()

  See :func:`XPLMGraphics.XPLMBindTexture2d`
  
.. py:function:: bringRootWidgetToFront()

  See :func:`XPWidgets.XPBringRootWidgetToFront`
  
.. py:function:: bringWindowToFront()

  See :func:`XPLMDisplay.XPLMBringWindowToFront`
  
.. py:function:: canWriteDataRef()

  See :func:`XPLMDataAccess.XPLMCanWriteDataRef`
  
.. py:function:: checkMenuItem()

  See :func:`XPLMMenus.XPLMCheckMenuItem`
  
.. py:function:: checkMenuItemState()

  See :func:`XPLMMenus.XPLMCheckMenuItemState`
  
.. py:function:: clearAllMenuItems()

  See :func:`XPLMMenus.XPLMClearAllMenuItems`
  
.. py:function:: clearFMSEntry()

  See :func:`XPLMNavigation.XPLMClearFMSEntry`
  
.. py:function:: commandBegin()

  See :func:`XPLMUtilities.XPLMCommandBegin`
  
.. py:function:: commandEnd()

  See :func:`XPLMUtilities.XPLMCommandEnd`
  
.. py:function:: commandOnce()

  See :func:`XPLMUtilities.XPLMCommandOnce`
  
.. py:function:: controlCamera()

  See :func:`XPLMCamera.XPLMControlCamera`
  
.. py:function:: countAircraft()

  See :func:`XPLMPlanes.XPLMCountAircraft`
  
.. py:function:: countChildWidgets()

  See :func:`XPWidgets.XPCountChildWidgets`
  
.. py:function:: countFMSEntries()

  See :func:`XPLMNavigation.XPLMCountFMSEntries`
  
.. py:function:: countHotKeys()

  See :func:`XPLMDisplay.XPLMCountHotKeys`
  
.. py:function:: countPlugins()

  See :func:`XPLMPlugin.XPLMCountPlugins`
  
.. py:function:: createCommand()

  See :func:`XPLMUtilities.XPLMCreateCommand`
  
.. py:function:: createCustomWidget()

  See :func:`XPWidgets.XPCreateCustomWidget`
  
.. py:function:: createFlightLoop()

  See :func:`XPLMProcessing.XPLMCreateFlightLoop`
  
.. py:function:: createInstance()

  See :func:`XPLMInstance.XPLMCreateInstance`
  
.. py:function:: createMapLayer()

  See :func:`XPLMMap.XPLMCreateMapLayer`
  
.. py:function:: createMenu()

  See :func:`XPLMMenus.XPLMCreateMenu`
  
.. py:function:: createProbe()

  See :func:`XPLMScenery.XPLMCreateProbe`
  
.. py:function:: createWidget()

  See :func:`XPWidgets.XPCreateWidget`
  
.. py:function:: createWidgets()

  See :func:`XPWidgetUtils.XPUCreateWidgets`
  
.. py:function:: createWindowEx()

  See :func:`XPLMDisplay.XPLMCreateWindowEx`
  
.. py:function:: debugString()

  See :func:`XPLMUtilities.XPLMDebugString`
  
.. py:function:: defocusKeyboard()

  See :func:`XPWidgetUtils.XPUDefocusKeyboard`
  
.. py:function:: degMagneticToDegTrue()

  See :func:`XPLMScenery.XPLMDegMagneticToDegTrue`
  
.. py:function:: degTrueToDegMagnetic()

  See :func:`XPLMScenery.XPLMDegTrueToDegMagnetic`
  
.. py:function:: destroyFlightLoop()

  See :func:`XPLMProcessing.XPLMDestroyFlightLoop`
  
.. py:function:: destroyInstance()

  See :func:`XPLMInstance.XPLMDestroyInstance`
  
.. py:function:: destroyMapLayer()

  See :func:`XPLMMap.XPLMDestroyMapLayer`
  
.. py:function:: destroyMenu()

  See :func:`XPLMMenus.XPLMDestroyMenu`
  
.. py:function:: destroyProbe()

  See :func:`XPLMScenery.XPLMDestroyProbe`
  
.. py:function:: destroyWidget()

  See :func:`XPWidgets.XPDestroyWidget`
  
.. py:function:: destroyWindow()

  See :func:`XPLMDisplay.XPLMDestroyWindow`
  
.. py:function:: disableAIForPlane()

  See :func:`XPLMPlanes.XPLMDisableAIForPlane`
  
.. py:function:: disablePlugin()

  See :func:`XPLMPlugin.XPLMDisablePlugin`
  
.. py:function:: dontControlCamera()

  See :func:`XPLMCamera.XPLMDontControlCamera`
  
.. py:function:: dragWidget()

  See :func:`XPWidgetUtils.XPUDragWidget`
  
.. py:function:: drawElement()

  See :func:`XPUIGraphics.XPDrawElement`
  
.. py:function:: drawMapIconFromSheet()

  See :func:`XPLMMap.XPLMDrawMapIconFromSheet`
  
.. py:function:: drawMapLabel()

  See :func:`XPLMMap.XPLMDrawMapLabel`
  
.. py:function:: drawNumber()

  See :func:`XPLMGraphics.XPLMDrawNumber`
  
.. py:function:: drawString()

  See :func:`XPLMGraphics.XPLMDrawString`
  
.. py:function:: drawTrack()

  See :func:`XPUIGraphics.XPDrawTrack`
  
.. py:function:: drawTranslucentDarkBox()

  See :func:`XPLMGraphics.XPLMDrawTranslucentDarkBox`
  
.. py:function:: drawWindow()

  See :func:`XPUIGraphics.XPDrawWindow`
  
.. py:function:: enableFeature()

  See :func:`XPLMPlugin.XPLMEnableFeature`
  
.. py:function:: enableMenuItem()

  See :func:`XPLMMenus.XPLMEnableMenuItem`
  
.. py:function:: enablePlugin()

  See :func:`XPLMPlugin.XPLMEnablePlugin`
  
.. py:function:: enumerateFeatures()

  See :func:`XPLMPlugin.XPLMEnumerateFeatures`
  
.. py:function:: extractFileAndPath()

  See :func:`XPLMUtilities.XPLMExtractFileAndPath`
  
.. py:function:: findAircraftMenu()

  See :func:`XPLMMenus.XPLMFindAircraftMenu`
  
.. py:function:: findCommand()

  See :func:`XPLMUtilities.XPLMFindCommand`
  
.. py:function:: findDataRef()

  See :func:`XPLMDataAccess.XPLMFindDataRef`
  
.. py:function:: findFirstNavAidOfType()

  See :func:`XPLMNavigation.XPLMFindFirstNavAidOfType`
  
.. py:function:: findLastNavAidOfType()

  See :func:`XPLMNavigation.XPLMFindLastNavAidOfType`
  
.. py:function:: findNavAid()

  See :func:`XPLMNavigation.XPLMFindNavAid`
  
.. py:function:: findPluginByPath()

  See :func:`XPLMPlugin.XPLMFindPluginByPath`
  
.. py:function:: findPluginBySignature()

  See :func:`XPLMPlugin.XPLMFindPluginBySignature`
  
.. py:function:: findPluginsMenu()

  See :func:`XPLMMenus.XPLMFindPluginsMenu`
  
.. py:function:: findRootWidget()

  See :func:`XPWidgets.XPFindRootWidget`
  
.. py:function:: findSymbol()

  See :func:`XPLMUtilities.XPLMFindSymbol`
  
.. py:function:: fixedLayout()

  See :func:`XPWidgetUtils.XPUFixedLayout`
  
.. py:function:: generateTextureNumbers()

  See :func:`XPLMGraphics.XPLMGenerateTextureNumbers`
  
.. py:function:: getAllMonitorBoundsGlobal()

  See :func:`XPLMDisplay.XPLMGetAllMonitorBoundsGlobal`
  
.. py:function:: getAllMonitorBoundsOS()

  See :func:`XPLMDisplay.XPLMGetAllMonitorBoundsOS`
  
.. py:function:: getCycleNumber()

  See :func:`XPLMProcessing.XPLMGetCycleNumber`
  
.. py:function:: getDataRefTypes()

  See :func:`XPLMDataAccess.XPLMGetDataRefTypes`
  
.. py:function:: getDatab()

  See :func:`XPLMDataAccess.XPLMGetDatab`
  
.. py:function:: getDatad()

  See :func:`XPLMDataAccess.XPLMGetDatad`
  
.. py:function:: getDataf()

  See :func:`XPLMDataAccess.XPLMGetDataf`
  
.. py:function:: getDatai()

  See :func:`XPLMDataAccess.XPLMGetDatai`
  
.. py:function:: getDatavf()

  See :func:`XPLMDataAccess.XPLMGetDatavf`
  
.. py:function:: getDatavi()

  See :func:`XPLMDataAccess.XPLMGetDatavi`
  
.. py:function:: getDestinationFMSEntry()

  See :func:`XPLMNavigation.XPLMGetDestinationFMSEntry`
  
.. py:function:: getDirectoryContents()

  See :func:`XPLMUtilities.XPLMGetDirectoryContents`
  
.. py:function:: getDirectorySeparator()

  See :func:`XPLMUtilities.XPLMGetDirectorySeparator`
  
.. py:function:: getDisplayedFMSEntry()

  See :func:`XPLMNavigation.XPLMGetDisplayedFMSEntry`
  
.. py:function:: getElapsedTime()

  See :func:`XPLMProcessing.XPLMGetElapsedTime`
  
.. py:function:: getElementDefaultDimensions()

  See :func:`XPUIGraphics.XPGetElementDefaultDimensions`
  
.. py:function:: getFMSEntryInfo()

  See :func:`XPLMNavigation.XPLMGetFMSEntryInfo`
  
.. py:function:: getFirstNavAid()

  See :func:`XPLMNavigation.XPLMGetFirstNavAid`
  
.. py:function:: getFontDimensions()

  See :func:`XPLMGraphics.XPLMGetFontDimensions`
  
.. py:function:: getGPSDestination()

  See :func:`XPLMNavigation.XPLMGetGPSDestination`
  
.. py:function:: getGPSDestinationType()

  See :func:`XPLMNavigation.XPLMGetGPSDestinationType`
  
.. py:function:: getHotKeyInfo()

  See :func:`XPLMDisplay.XPLMGetHotKeyInfo`
  
.. py:function:: getLanguage()

  See :func:`XPLMUtilities.XPLMGetLanguage`
  
.. py:function:: getMagneticVariation()

  See :func:`XPLMScenery.XPLMGetMagneticVariation`
  
.. py:function:: getMouseLocationGlobal()

  See :func:`XPLMDisplay.XPLMGetMouseLocationGlobal`
  
.. py:function:: getMyID()

  See :func:`XPLMPlugin.XPLMGetMyID`
  
.. py:function:: getNavAidInfo()

  See :func:`XPLMNavigation.XPLMGetNavAidInfo`
  
.. py:function:: getNextNavAid()

  See :func:`XPLMNavigation.XPLMGetNextNavAid`
  
.. py:function:: getNthAircraftModel()

  See :func:`XPLMPlanes.XPLMGetNthAircraftModel`
  
.. py:function:: getNthChildWidget()

  See :func:`XPWidgets.XPGetNthChildWidget`
  
.. py:function:: getNthHotKey()

  See :func:`XPLMDisplay.XPLMGetNthHotKey`
  
.. py:function:: getNthPlugin()

  See :func:`XPLMPlugin.XPLMGetNthPlugin`
  
.. py:function:: getParentWidget()

  See :func:`XPWidgets.XPGetParentWidget`
  
.. py:function:: getPluginInfo()

  See :func:`XPLMPlugin.XPLMGetPluginInfo`
  
.. py:function:: getPrefsPath()

  See :func:`XPLMUtilities.XPLMGetPrefsPath`
  
.. py:function:: getScreenBoundsGlobal()

  See :func:`XPLMDisplay.XPLMGetScreenBoundsGlobal`
  
.. py:function:: getScreenSize()

  See :func:`XPLMDisplay.XPLMGetScreenSize`
  
.. py:function:: getSystemPath()

  See :func:`XPLMUtilities.XPLMGetSystemPath`
  
.. py:function:: getTrackDefaultDimensions()

  See :func:`XPUIGraphics.XPGetTrackDefaultDimensions`
  
.. py:function:: getTrackMetrics()

  See :func:`XPUIGraphics.XPGetTrackMetrics`
  
.. py:function:: getVersions()

  See :func:`XPLMUtilities.XPLMGetVersions`
  
.. py:function:: getVirtualKeyDescription()

  See :func:`XPLMUtilities.XPLMGetVirtualKeyDescription`
  
.. py:function:: getWidgetClassFunc()

  See :func:`XPWidgets.XPGetWidgetClassFunc`
  
.. py:function:: getWidgetDescriptor()

  See :func:`XPWidgets.XPGetWidgetDescriptor`
  
.. py:function:: getWidgetExposedGeometry()

  See :func:`XPWidgets.XPGetWidgetExposedGeometry`
  
.. py:function:: getWidgetForLocation()

  See :func:`XPWidgets.XPGetWidgetForLocation`
  
.. py:function:: getWidgetGeometry()

  See :func:`XPWidgets.XPGetWidgetGeometry`
  
.. py:function:: getWidgetProperty()

  See :func:`XPWidgets.XPGetWidgetProperty`
  
.. py:function:: getWidgetUnderlyingWindow()

  See :func:`XPWidgets.XPGetWidgetUnderlyingWindow`
  
.. py:function:: getWidgetWithFocus()

  See :func:`XPWidgets.XPGetWidgetWithFocus`
  
.. py:function:: getWindowDefaultDimensions()

  See :func:`XPUIGraphics.XPGetWindowDefaultDimensions`
  
.. py:function:: getWindowGeometry()

  See :func:`XPLMDisplay.XPLMGetWindowGeometry`
  
.. py:function:: getWindowGeometryOS()

  See :func:`XPLMDisplay.XPLMGetWindowGeometryOS`
  
.. py:function:: getWindowGeometryVR()

  See :func:`XPLMDisplay.XPLMGetWindowGeometryVR`
  
.. py:function:: getWindowIsVisible()

  See :func:`XPLMDisplay.XPLMGetWindowIsVisible`
  
.. py:function:: getWindowRefCon()

  See :func:`XPLMDisplay.XPLMGetWindowRefCon`
  
.. py:function:: hasFeature()

  See :func:`XPLMPlugin.XPLMHasFeature`
  
.. py:function:: hasKeyboardFocus()

  See :func:`XPLMDisplay.XPLMHasKeyboardFocus`
  
.. py:function:: hideWidget()

  See :func:`XPWidgets.XPHideWidget`
  
.. py:function:: instanceSetPosition()

  See :func:`XPLMInstance.XPLMInstanceSetPosition`
  
.. py:function:: isCameraBeingControlled()

  See :func:`XPLMCamera.XPLMIsCameraBeingControlled`
  
.. py:function:: isDataRefGood()

  See :func:`XPLMDataAccess.XPLMIsDataRefGood`
  
.. py:function:: isFeatureEnabled()

  See :func:`XPLMPlugin.XPLMIsFeatureEnabled`
  
.. py:function:: isPluginEnabled()

  See :func:`XPLMPlugin.XPLMIsPluginEnabled`
  
.. py:function:: isWidgetInFront()

  See :func:`XPWidgets.XPIsWidgetInFront`
  
.. py:function:: isWidgetVisible()

  See :func:`XPWidgets.XPIsWidgetVisible`
  
.. py:function:: isWindowInFront()

  See :func:`XPLMDisplay.XPLMIsWindowInFront`
  
.. py:function:: loadDataFile()

  See :func:`XPLMUtilities.XPLMLoadDataFile`
  
.. py:function:: loadObject()

  See :func:`XPLMScenery.XPLMLoadObject`
  
.. py:function:: loadObjectAsync()

  See :func:`XPLMScenery.XPLMLoadObjectAsync`
  
.. py:function:: localToWorld()

  See :func:`XPLMGraphics.XPLMLocalToWorld`
  
.. py:function:: log()

  See :func:`XPPython.XPPythonLog`                 
  
.. py:function:: lookupObjects()

  See :func:`XPLMScenery.XPLMLookupObjects`
  
.. py:function:: loseKeyboardFocus()

  See :func:`XPWidgets.XPLoseKeyboardFocus`
  
.. py:function:: mapExists()

  See :func:`XPLMMap.XPLMMapExists`
  
.. py:function:: mapGetNorthHeading()

  See :func:`XPLMMap.XPLMMapGetNorthHeading`
  
.. py:function:: mapProject()

  See :func:`XPLMMap.XPLMMapProject`
  
.. py:function:: mapScaleMeter()

  See :func:`XPLMMap.XPLMMapScaleMeter`
  
.. py:function:: mapUnproject()

  See :func:`XPLMMap.XPLMMapUnproject`
  
.. py:function:: measureString()

  See :func:`XPLMGraphics.XPLMMeasureString`
  
.. py:function:: moveWidgetBy()

  See :func:`XPWidgetUtils.XPUMoveWidgetBy`
  
.. py:function:: placeUserAtAirport()

  See :func:`XPLMPlanes.XPLMPlaceUserAtAirport`
  
.. py:function:: placeUserAtLocation()

  See :func:`XPLMPlanes.XPLMPlaceUserAtLocation`
  
.. py:function:: placeWidgetWithin()

  See :func:`XPWidgets.XPPlaceWidgetWithin`
  
.. py:function:: probeTerrainXYZ()

  See :func:`XPLMScenery.XPLMProbeTerrainXYZ`
  
.. py:function:: pythonGetCapsules()

  See :func:`XPPython.XPPythonGetCapsules`
  
.. py:function:: pythonGetDicts()

  See :func:`XPPython.XPPythonGetDicts`

.. py:function:: pythonLog()

  See :func:`XPPython.XPPythonLog`                 
  
.. py:function:: readCameraPosition()

  See :func:`XPLMCamera.XPLMReadCameraPosition`
  
.. py:function:: registerCommandHandler()

  See :func:`XPLMUtilities.XPLMRegisterCommandHandler`
  
.. py:function:: registerDataAccessor()

  See :func:`XPLMDataAccess.XPLMRegisterDataAccessor`
  
.. py:function:: registerDrawCallback()

  See :func:`XPLMDisplay.XPLMRegisterDrawCallback`
  
.. py:function:: registerFlightLoopCallback()

  See :func:`XPLMProcessing.XPLMRegisterFlightLoopCallback`
  
.. py:function:: registerHotKey()

  See :func:`XPLMDisplay.XPLMRegisterHotKey`
  
.. py:function:: registerKeySniffer()

  See :func:`XPLMDisplay.XPLMRegisterKeySniffer`
  
.. py:function:: registerMapCreationHook()

  See :func:`XPLMMap.XPLMRegisterMapCreationHook`
  
.. py:function:: releasePlanes()

  See :func:`XPLMPlanes.XPLMReleasePlanes`
  
.. py:function:: reloadPlugins()

  See :func:`XPLMPlugin.XPLMReloadPlugins`
  
.. py:function:: reloadScenery()

  See :func:`XPLMUtilities.XPLMReloadScenery`
  
.. py:function:: removeMenuItem()

  See :func:`XPLMMenus.XPLMRemoveMenuItem`
  
.. py:function:: saveDataFile()

  See :func:`XPLMUtilities.XPLMSaveDataFile`
  
.. py:function:: scheduleFlightLoop()

  See :func:`XPLMProcessing.XPLMScheduleFlightLoop`
  
.. py:function:: selectIfNeeded()

  See :func:`XPWidgetUtils.XPUSelectIfNeeded`
  
.. py:function:: sendMessageToPlugin()

  See :func:`XPLMPlugin.XPLMSendMessageToPlugin`
  
.. py:function:: sendMessageToWidget()

  See :func:`XPWidgets.XPSendMessageToWidget`
  
.. py:function:: setActiveAircraftCount()

  See :func:`XPLMPlanes.XPLMSetActiveAircraftCount`
  
.. py:function:: setAircraftModel()

  See :func:`XPLMPlanes.XPLMSetAircraftModel`
  
.. py:function:: setDatab()

  See :func:`XPLMDataAccess.XPLMSetDatab`
  
.. py:function:: setDatad()

  See :func:`XPLMDataAccess.XPLMSetDatad`
  
.. py:function:: setDataf()

  See :func:`XPLMDataAccess.XPLMSetDataf`
  
.. py:function:: setDatai()

  See :func:`XPLMDataAccess.XPLMSetDatai`
  
.. py:function:: setDatavf()

  See :func:`XPLMDataAccess.XPLMSetDatavf`
  
.. py:function:: setDatavi()

  See :func:`XPLMDataAccess.XPLMSetDatavi`
  
.. py:function:: setDestinationFMSEntry()

  See :func:`XPLMNavigation.XPLMSetDestinationFMSEntry`
  
.. py:function:: setDisplayedFMSEntry()

  See :func:`XPLMNavigation.XPLMSetDisplayedFMSEntry`
  
.. py:function:: setErrorCallback()

  See :func:`XPLMUtilities.XPLMSetErrorCallback`
  
.. py:function:: setFMSEntryInfo()

  See :func:`XPLMNavigation.XPLMSetFMSEntryInfo`
  
.. py:function:: setFMSEntryLatLon()

  See :func:`XPLMNavigation.XPLMSetFMSEntryLatLon`
  
.. py:function:: setFlightLoopCallbackInterval()

  See :func:`XPLMProcessing.XPLMSetFlightLoopCallbackInterval`
  
.. py:function:: setGraphicsState()

  See :func:`XPLMGraphics.XPLMSetGraphicsState`
  
.. py:function:: setHotKeyCombination()

  See :func:`XPLMDisplay.XPLMSetHotKeyCombination`
  
.. py:function:: setKeyboardFocus()

  See :func:`XPWidgets.XPSetKeyboardFocus`
  
.. py:function:: setMenuItemName()

  See :func:`XPLMMenus.XPLMSetMenuItemName`
  
.. py:function:: setUsersAircraft()

  See :func:`XPLMPlanes.XPLMSetUsersAircraft`
  
.. py:function:: setWidgetDescriptor()

  See :func:`XPWidgets.XPSetWidgetDescriptor`
  
.. py:function:: setWidgetGeometry()

  See :func:`XPWidgets.XPSetWidgetGeometry`
  
.. py:function:: setWidgetProperty()

  See :func:`XPWidgets.XPSetWidgetProperty`
  
.. py:function:: setWindowGeometry()

  See :func:`XPLMDisplay.XPLMSetWindowGeometry`
  
.. py:function:: setWindowGeometryOS()

  See :func:`XPLMDisplay.XPLMSetWindowGeometryOS`
  
.. py:function:: setWindowGeometryVR()

  See :func:`XPLMDisplay.XPLMSetWindowGeometryVR`
  
.. py:function:: setWindowGravity()

  See :func:`XPLMDisplay.XPLMSetWindowGravity`
  
.. py:function:: setWindowIsVisible()

  See :func:`XPLMDisplay.XPLMSetWindowIsVisible`
  
.. py:function:: setWindowPositioningMode()

  See :func:`XPLMDisplay.XPLMSetWindowPositioningMode`
  
.. py:function:: setWindowRefCon()

  See :func:`XPLMDisplay.XPLMSetWindowRefCon`
  
.. py:function:: setWindowResizingLimits()

  See :func:`XPLMDisplay.XPLMSetWindowResizingLimits`
  
.. py:function:: setWindowTitle()

  See :func:`XPLMDisplay.XPLMSetWindowTitle`
  
.. py:function:: shareData()

  See :func:`XPLMDataAccess.XPLMShareData`
  
.. py:function:: showWidget()

  See :func:`XPWidgets.XPShowWidget`
  
.. py:function:: speakString()

  See :func:`XPLMUtilities.XPLMSpeakString`
  
.. py:function:: sys_log()
                 
  See :func:`XPPython.XPSystemLog`                 
  
.. py:function:: systemLog()
                 
  See :func:`XPPython.XPSystemLog`                 
  
.. py:function:: takeKeyboardFocus()

  See :func:`XPLMDisplay.XPLMTakeKeyboardFocus`
  
.. py:function:: unloadObject()

  See :func:`XPLMScenery.XPLMUnloadObject`
  
.. py:function:: unregisterCommandHandler()

  See :func:`XPLMUtilities.XPLMUnregisterCommandHandler`
  
.. py:function:: unregisterDataAccessor()

  See :func:`XPLMDataAccess.XPLMUnregisterDataAccessor`
  
.. py:function:: unregisterDrawCallback()

  See :func:`XPLMDisplay.XPLMUnregisterDrawCallback`
  
.. py:function:: unregisterFlightLoopCallback()

  See :func:`XPLMProcessing.XPLMUnregisterFlightLoopCallback`
  
.. py:function:: unregisterHotKey()

  See :func:`XPLMDisplay.XPLMUnregisterHotKey`
  
.. py:function:: unregisterKeySniffer()

  See :func:`XPLMDisplay.XPLMUnregisterKeySniffer`
  
.. py:function:: unshareData()

  See :func:`XPLMDataAccess.XPLMUnshareData`
  
.. py:function:: windowIsInVR()

  See :func:`XPLMDisplay.XPLMWindowIsInVR`
  
.. py:function:: windowIsPoppedOut()

  See :func:`XPLMDisplay.XPLMWindowIsPoppedOut`
  
.. py:function:: worldToLocal()

  See :func:`XPLMGraphics.XPLMWorldToLocal`
  
