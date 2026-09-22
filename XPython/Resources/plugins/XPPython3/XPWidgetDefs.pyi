# pylint: disable=unused-argument
# (a stub's parameters are never used -- there is no body)
from typing import Any
from XPPython3.xp_typing import XPDispatchMode, XPWidgetMessage, XPWidgetPropertyID
Property_Refcon: XPWidgetPropertyID
Property_Dragging: XPWidgetPropertyID
Property_DragXOff: XPWidgetPropertyID
Property_DragYOff: XPWidgetPropertyID
Property_Hilited: XPWidgetPropertyID
Property_Object: XPWidgetPropertyID
Property_Clip: XPWidgetPropertyID
Property_Enabled: XPWidgetPropertyID
Property_UserStart: XPWidgetPropertyID
Mode_Direct: XPDispatchMode
Mode_UpChain: XPDispatchMode = XPDispatchMode(1)
Mode_Recursive: XPDispatchMode
Mode_DirectAllCallbacks: XPDispatchMode
Mode_Once: XPDispatchMode
WidgetClass_None: int
Msg_None: XPWidgetMessage
Msg_Create: XPWidgetMessage
Msg_Destroy: XPWidgetMessage
Msg_Paint: XPWidgetMessage
Msg_Draw: XPWidgetMessage
Msg_KeyPress: XPWidgetMessage
Msg_KeyTakeFocus: XPWidgetMessage
Msg_KeyLoseFocus: XPWidgetMessage
Msg_MouseDown: XPWidgetMessage
Msg_MouseDrag: XPWidgetMessage
Msg_MouseUp: XPWidgetMessage
Msg_Reshape: XPWidgetMessage
Msg_ExposedChanged: XPWidgetMessage
Msg_AcceptChild: XPWidgetMessage
Msg_LoseChild: XPWidgetMessage
Msg_AcceptParent: XPWidgetMessage
Msg_Shown: XPWidgetMessage
Msg_Hidden: XPWidgetMessage
Msg_DescriptorChanged: XPWidgetMessage
Msg_PropertyChanged: XPWidgetMessage
Msg_MouseWheel: XPWidgetMessage
Msg_CursorAdjust: XPWidgetMessage
Msg_UserStart: XPWidgetMessage


def PI_GetMouseState(param: Any) -> Any:
    """
    Python2 backward compatibility only: returns its argument unchanged.

    Not required for XPPython3 -- widget callbacks already receive mouse state.
    """
    ...


def PI_GetKeyState(param: Any) -> Any:
    """
    Python2 backward compatibility only: returns its argument unchanged.

    Not required for XPPython3 -- widget callbacks already receive key state.
    """
    ...
