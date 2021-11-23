#define _GNU_SOURCE 1
#include <Python.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdbool.h>

#include <XPLM/XPLMDefs.h>
#include <Widgets/XPWidgetDefs.h>
#include <Widgets/XPStandardWidgets.h>

static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  Py_RETURN_NONE;
}


static PyMethodDef XPStandardWidgetsMethods[] = {
  {"_cleanup", cleanup, METH_VARARGS, ""},
  {NULL, NULL, 0, NULL}
};


static struct PyModuleDef XPStandardWidgetsModule = {
  PyModuleDef_HEAD_INIT,
  "XPStandardWidgets",
  "Laminar documentation: \n"
  "   https://developer.x-plane.com/sdk/XPStandardWidgets/\n"
  "XPPython3 documentation: \n"
  "   https://xppython3.rtfd.io/en/stable/development/modules/standardwidgets.html",
  -1,
  XPStandardWidgetsMethods,
  NULL,
  NULL,
  NULL,
  NULL
};

PyMODINIT_FUNC
PyInit_XPStandardWidgets(void)
{
  PyObject *mod = PyModule_Create(&XPStandardWidgetsModule);
  if(mod){
    PyModule_AddStringConstant(mod, "__author__", "Peter Buckner (xppython3@avnwx.com)");
    PyModule_AddIntConstant(mod, "xpWidgetClass_MainWindow", xpWidgetClass_MainWindow);
    PyModule_AddIntConstant(mod, "xpMainWindowStyle_MainWindow", xpMainWindowStyle_MainWindow);
    PyModule_AddIntConstant(mod, "xpMainWindowStyle_Translucent", xpMainWindowStyle_Translucent);
    PyModule_AddIntConstant(mod, "xpProperty_MainWindowType", xpProperty_MainWindowType);
    PyModule_AddIntConstant(mod, "xpProperty_MainWindowHasCloseBoxes", xpProperty_MainWindowHasCloseBoxes);
    PyModule_AddIntConstant(mod, "xpMessage_CloseButtonPushed", xpMessage_CloseButtonPushed);
    PyModule_AddIntConstant(mod, "xpWidgetClass_SubWindow", xpWidgetClass_SubWindow);
    PyModule_AddIntConstant(mod, "xpSubWindowStyle_SubWindow", xpSubWindowStyle_SubWindow);
    PyModule_AddIntConstant(mod, "xpSubWindowStyle_Screen", xpSubWindowStyle_Screen);
    PyModule_AddIntConstant(mod, "xpSubWindowStyle_ListView", xpSubWindowStyle_ListView);
    PyModule_AddIntConstant(mod, "xpProperty_SubWindowType", xpProperty_SubWindowType);
    PyModule_AddIntConstant(mod, "xpWidgetClass_Button", xpWidgetClass_Button);
    PyModule_AddIntConstant(mod, "xpPushButton", xpPushButton);
    PyModule_AddIntConstant(mod, "xpRadioButton", xpRadioButton);
    PyModule_AddIntConstant(mod, "xpWindowCloseBox", xpWindowCloseBox);
    PyModule_AddIntConstant(mod, "xpLittleDownArrow", xpLittleDownArrow);
    PyModule_AddIntConstant(mod, "xpLittleUpArrow", xpLittleUpArrow);
    PyModule_AddIntConstant(mod, "xpButtonBehaviorPushButton", xpButtonBehaviorPushButton);
    PyModule_AddIntConstant(mod, "xpButtonBehaviorCheckBox", xpButtonBehaviorCheckBox);
    PyModule_AddIntConstant(mod, "xpButtonBehaviorRadioButton", xpButtonBehaviorRadioButton);
    PyModule_AddIntConstant(mod, "xpProperty_ButtonType", xpProperty_ButtonType);
    PyModule_AddIntConstant(mod, "xpProperty_ButtonBehavior", xpProperty_ButtonBehavior);
    PyModule_AddIntConstant(mod, "xpProperty_ButtonState", xpProperty_ButtonState);
    PyModule_AddIntConstant(mod, "xpMsg_PushButtonPressed", xpMsg_PushButtonPressed);
    PyModule_AddIntConstant(mod, "xpMsg_ButtonStateChanged", xpMsg_ButtonStateChanged);
    PyModule_AddIntConstant(mod, "xpWidgetClass_TextField", xpWidgetClass_TextField);
    PyModule_AddIntConstant(mod, "xpTextEntryField", xpTextEntryField);
    PyModule_AddIntConstant(mod, "xpTextTransparent", xpTextTransparent);
    PyModule_AddIntConstant(mod, "xpTextTranslucent", xpTextTranslucent);
    PyModule_AddIntConstant(mod, "xpProperty_EditFieldSelStart", xpProperty_EditFieldSelStart);
    PyModule_AddIntConstant(mod, "xpProperty_EditFieldSelEnd", xpProperty_EditFieldSelEnd);
    PyModule_AddIntConstant(mod, "xpProperty_EditFieldSelDragStart", xpProperty_EditFieldSelDragStart);
    PyModule_AddIntConstant(mod, "xpProperty_TextFieldType", xpProperty_TextFieldType);
    PyModule_AddIntConstant(mod, "xpProperty_PasswordMode", xpProperty_PasswordMode);
    PyModule_AddIntConstant(mod, "xpProperty_MaxCharacters", xpProperty_MaxCharacters);
    PyModule_AddIntConstant(mod, "xpProperty_ScrollPosition", xpProperty_ScrollPosition);
    PyModule_AddIntConstant(mod, "xpProperty_Font", xpProperty_Font);
    PyModule_AddIntConstant(mod, "xpProperty_ActiveEditSide", xpProperty_ActiveEditSide);
    PyModule_AddIntConstant(mod, "xpMsg_TextFieldChanged", xpMsg_TextFieldChanged);
    PyModule_AddIntConstant(mod, "xpWidgetClass_ScrollBar", xpWidgetClass_ScrollBar);
    PyModule_AddIntConstant(mod, "xpScrollBarTypeScrollBar", xpScrollBarTypeScrollBar);
    PyModule_AddIntConstant(mod, "xpScrollBarTypeSlider", xpScrollBarTypeSlider);
    PyModule_AddIntConstant(mod, "xpProperty_ScrollBarSliderPosition", xpProperty_ScrollBarSliderPosition);
    PyModule_AddIntConstant(mod, "xpProperty_ScrollBarMin", xpProperty_ScrollBarMin);
    PyModule_AddIntConstant(mod, "xpProperty_ScrollBarMax", xpProperty_ScrollBarMax);
    PyModule_AddIntConstant(mod, "xpProperty_ScrollBarPageAmount", xpProperty_ScrollBarPageAmount);
    PyModule_AddIntConstant(mod, "xpProperty_ScrollBarType", xpProperty_ScrollBarType);
    PyModule_AddIntConstant(mod, "xpProperty_ScrollBarSlop", xpProperty_ScrollBarSlop);
    PyModule_AddIntConstant(mod, "xpMsg_ScrollBarSliderPositionChanged", xpMsg_ScrollBarSliderPositionChanged);
    PyModule_AddIntConstant(mod, "xpWidgetClass_Caption", xpWidgetClass_Caption);
    PyModule_AddIntConstant(mod, "xpProperty_CaptionLit", xpProperty_CaptionLit);
    PyModule_AddIntConstant(mod, "xpWidgetClass_GeneralGraphics", xpWidgetClass_GeneralGraphics);
    PyModule_AddIntConstant(mod, "xpShip", xpShip);
    PyModule_AddIntConstant(mod, "xpILSGlideScope", xpILSGlideScope);
    PyModule_AddIntConstant(mod, "xpMarkerLeft", xpMarkerLeft);
    PyModule_AddIntConstant(mod, "xp_Airport", xp_Airport);
    PyModule_AddIntConstant(mod, "xpNDB", xpNDB);
    PyModule_AddIntConstant(mod, "xpVOR", xpVOR);
    PyModule_AddIntConstant(mod, "xpRadioTower", xpRadioTower);
    PyModule_AddIntConstant(mod, "xpAircraftCarrier", xpAircraftCarrier);
    PyModule_AddIntConstant(mod, "xpFire", xpFire);
    PyModule_AddIntConstant(mod, "xpMarkerRight", xpMarkerRight);
    PyModule_AddIntConstant(mod, "xpCustomObject", xpCustomObject);
    PyModule_AddIntConstant(mod, "xpCoolingTower", xpCoolingTower);
    PyModule_AddIntConstant(mod, "xpSmokeStack", xpSmokeStack);
    PyModule_AddIntConstant(mod, "xpBuilding", xpBuilding);
    PyModule_AddIntConstant(mod, "xpPowerLine", xpPowerLine);
    PyModule_AddIntConstant(mod, "xpVORWithCompassRose", xpVORWithCompassRose);
    PyModule_AddIntConstant(mod, "xpOilPlatform", xpOilPlatform);
    PyModule_AddIntConstant(mod, "xpOilPlatformSmall", xpOilPlatformSmall);
    PyModule_AddIntConstant(mod, "xpWayPoint", xpWayPoint);
    PyModule_AddIntConstant(mod, "xpProperty_GeneralGraphicsType", xpProperty_GeneralGraphicsType);
    PyModule_AddIntConstant(mod, "xpWidgetClass_Progress", xpWidgetClass_Progress);
    PyModule_AddIntConstant(mod, "xpProperty_ProgressPosition", xpProperty_ProgressPosition);
    PyModule_AddIntConstant(mod, "xpProperty_ProgressMin", xpProperty_ProgressMin);
    PyModule_AddIntConstant(mod, "xpProperty_ProgressMax", xpProperty_ProgressMax);

    PyModule_AddIntConstant(mod, "WidgetClass_MainWindow", xpWidgetClass_MainWindow);
    PyModule_AddIntConstant(mod, "MainWindowStyle_MainWindow", xpMainWindowStyle_MainWindow);
    PyModule_AddIntConstant(mod, "MainWindowStyle_Translucent", xpMainWindowStyle_Translucent);
    PyModule_AddIntConstant(mod, "Property_MainWindowType", xpProperty_MainWindowType);
    PyModule_AddIntConstant(mod, "Property_MainWindowHasCloseBoxes", xpProperty_MainWindowHasCloseBoxes);
    PyModule_AddIntConstant(mod, "Message_CloseButtonPushed", xpMessage_CloseButtonPushed);
    PyModule_AddIntConstant(mod, "WidgetClass_SubWindow", xpWidgetClass_SubWindow);
    PyModule_AddIntConstant(mod, "SubWindowStyle_SubWindow", xpSubWindowStyle_SubWindow);
    PyModule_AddIntConstant(mod, "SubWindowStyle_Screen", xpSubWindowStyle_Screen);
    PyModule_AddIntConstant(mod, "SubWindowStyle_ListView", xpSubWindowStyle_ListView);
    PyModule_AddIntConstant(mod, "Property_SubWindowType", xpProperty_SubWindowType);
    PyModule_AddIntConstant(mod, "WidgetClass_Button", xpWidgetClass_Button);
    PyModule_AddIntConstant(mod, "PushButton", xpPushButton);
    PyModule_AddIntConstant(mod, "RadioButton", xpRadioButton);
    PyModule_AddIntConstant(mod, "WindowCloseBox", xpWindowCloseBox);
    PyModule_AddIntConstant(mod, "LittleDownArrow", xpLittleDownArrow);
    PyModule_AddIntConstant(mod, "LittleUpArrow", xpLittleUpArrow);
    PyModule_AddIntConstant(mod, "ButtonBehaviorPushButton", xpButtonBehaviorPushButton);
    PyModule_AddIntConstant(mod, "ButtonBehaviorCheckBox", xpButtonBehaviorCheckBox);
    PyModule_AddIntConstant(mod, "ButtonBehaviorRadioButton", xpButtonBehaviorRadioButton);
    PyModule_AddIntConstant(mod, "Property_ButtonType", xpProperty_ButtonType);
    PyModule_AddIntConstant(mod, "Property_ButtonBehavior", xpProperty_ButtonBehavior);
    PyModule_AddIntConstant(mod, "Property_ButtonState", xpProperty_ButtonState);
    PyModule_AddIntConstant(mod, "Msg_PushButtonPressed", xpMsg_PushButtonPressed);
    PyModule_AddIntConstant(mod, "Msg_ButtonStateChanged", xpMsg_ButtonStateChanged);
    PyModule_AddIntConstant(mod, "WidgetClass_TextField", xpWidgetClass_TextField);
    PyModule_AddIntConstant(mod, "TextEntryField", xpTextEntryField);
    PyModule_AddIntConstant(mod, "TextTransparent", xpTextTransparent);
    PyModule_AddIntConstant(mod, "TextTranslucent", xpTextTranslucent);
    PyModule_AddIntConstant(mod, "Property_EditFieldSelStart", xpProperty_EditFieldSelStart);
    PyModule_AddIntConstant(mod, "Property_EditFieldSelEnd", xpProperty_EditFieldSelEnd);
    PyModule_AddIntConstant(mod, "Property_EditFieldSelDragStart", xpProperty_EditFieldSelDragStart);
    PyModule_AddIntConstant(mod, "Property_TextFieldType", xpProperty_TextFieldType);
    PyModule_AddIntConstant(mod, "Property_PasswordMode", xpProperty_PasswordMode);
    PyModule_AddIntConstant(mod, "Property_MaxCharacters", xpProperty_MaxCharacters);
    PyModule_AddIntConstant(mod, "Property_ScrollPosition", xpProperty_ScrollPosition);
    PyModule_AddIntConstant(mod, "Property_Font", xpProperty_Font);
    PyModule_AddIntConstant(mod, "Property_ActiveEditSide", xpProperty_ActiveEditSide);
    PyModule_AddIntConstant(mod, "Msg_TextFieldChanged", xpMsg_TextFieldChanged);
    PyModule_AddIntConstant(mod, "WidgetClass_ScrollBar", xpWidgetClass_ScrollBar);
    PyModule_AddIntConstant(mod, "ScrollBarTypeScrollBar", xpScrollBarTypeScrollBar);
    PyModule_AddIntConstant(mod, "ScrollBarTypeSlider", xpScrollBarTypeSlider);
    PyModule_AddIntConstant(mod, "Property_ScrollBarSliderPosition", xpProperty_ScrollBarSliderPosition);
    PyModule_AddIntConstant(mod, "Property_ScrollBarMin", xpProperty_ScrollBarMin);
    PyModule_AddIntConstant(mod, "Property_ScrollBarMax", xpProperty_ScrollBarMax);
    PyModule_AddIntConstant(mod, "Property_ScrollBarPageAmount", xpProperty_ScrollBarPageAmount);
    PyModule_AddIntConstant(mod, "Property_ScrollBarType", xpProperty_ScrollBarType);
    PyModule_AddIntConstant(mod, "Property_ScrollBarSlop", xpProperty_ScrollBarSlop);
    PyModule_AddIntConstant(mod, "Msg_ScrollBarSliderPositionChanged", xpMsg_ScrollBarSliderPositionChanged);
    PyModule_AddIntConstant(mod, "WidgetClass_Caption", xpWidgetClass_Caption);
    PyModule_AddIntConstant(mod, "Property_CaptionLit", xpProperty_CaptionLit);
    PyModule_AddIntConstant(mod, "WidgetClass_GeneralGraphics", xpWidgetClass_GeneralGraphics);
    PyModule_AddIntConstant(mod, "Ship", xpShip);
    PyModule_AddIntConstant(mod, "ILSGlideScope", xpILSGlideScope);
    PyModule_AddIntConstant(mod, "MarkerLeft", xpMarkerLeft);
    PyModule_AddIntConstant(mod, "_Airport", xp_Airport);
    PyModule_AddIntConstant(mod, "NDB", xpNDB);
    PyModule_AddIntConstant(mod, "VOR", xpVOR);
    PyModule_AddIntConstant(mod, "RadioTower", xpRadioTower);
    PyModule_AddIntConstant(mod, "AircraftCarrier", xpAircraftCarrier);
    PyModule_AddIntConstant(mod, "Fire", xpFire);
    PyModule_AddIntConstant(mod, "MarkerRight", xpMarkerRight);
    PyModule_AddIntConstant(mod, "CustomObject", xpCustomObject);
    PyModule_AddIntConstant(mod, "CoolingTower", xpCoolingTower);
    PyModule_AddIntConstant(mod, "SmokeStack", xpSmokeStack);
    PyModule_AddIntConstant(mod, "Building", xpBuilding);
    PyModule_AddIntConstant(mod, "PowerLine", xpPowerLine);
    PyModule_AddIntConstant(mod, "VORWithCompassRose", xpVORWithCompassRose);
    PyModule_AddIntConstant(mod, "OilPlatform", xpOilPlatform);
    PyModule_AddIntConstant(mod, "OilPlatformSmall", xpOilPlatformSmall);
    PyModule_AddIntConstant(mod, "WayPoint", xpWayPoint);
    PyModule_AddIntConstant(mod, "Property_GeneralGraphicsType", xpProperty_GeneralGraphicsType);
    PyModule_AddIntConstant(mod, "WidgetClass_Progress", xpWidgetClass_Progress);
    PyModule_AddIntConstant(mod, "Property_ProgressPosition", xpProperty_ProgressPosition);
    PyModule_AddIntConstant(mod, "Property_ProgressMin", xpProperty_ProgressMin);
    PyModule_AddIntConstant(mod, "Property_ProgressMax", xpProperty_ProgressMax);
    
  }

  return mod;
}



