from typing import Self, Any
from XPPython3 import xp
from XPPython3.xp_typing import XPWidgetMessage


class WidgetMsgHelper:
    """
    Decodes Widget Message, based on type of message, and params

    example:
        print(WidgetMsgHelper(inMessage, inParam1, inParam2))
    """
    msgs = {
        xp.Msg_None: {'name': 'None',
                      'param1': lambda x: '<should never be called>',
                      'param2': lambda x: '<>', },
        xp.Msg_Create: {'name': 'Create',
                        'param1': lambda x: 'Subclass' if x == 1 else 'Not subclass',
                        'param2': lambda x: '<>', },
        xp.Msg_Destroy: {'name': 'Destroy',
                         'param1': lambda x: 'Explicit deletion' if x == 0 else 'Recursive deletion',
                         'param2': lambda x: '<>', },
        xp.Msg_Paint: {'name': 'Paint',
                       'param1': lambda x: '<>',
                       'param2': lambda x: '<>', },
        xp.Msg_Draw: {'name': 'Draw',
                      'param1': lambda x: '<>',
                      'param2': lambda x: '<>', },
        xp.Msg_KeyPress: {'name': 'KeyPress',
                          'param1': lambda x: WidgetMsgHelper.keyState(x),  # pylint: disable=unnecessary-lambda
                          'param2': lambda x: '<>', },
        xp.Msg_KeyTakeFocus: {'name': 'KeyTakeFocus',  # (someone else gave up focus???)
                              'param1': lambda x: 'Child gave up focus' if x == 1 else 'Someone else gave up focus',
                              'param2': lambda x: '<>', },
        xp.Msg_KeyLoseFocus: {'name': 'KeyLoseFocus',
                              'param1': lambda x: 'Another widget is taking' if x == 1 else 'Someone called API to request remove focus',
                              'param2': lambda x: '<>', },
        xp.Msg_MouseDown: {'name': 'MouseDown',
                           'param1': lambda x: WidgetMsgHelper.mouseState(x),  # pylint: disable=unnecessary-lambda
                           'param2': lambda x: '<>', },
        xp.Msg_MouseDrag: {'name': 'MouseDrag',
                           'param1': lambda x: WidgetMsgHelper.mouseState(x),  # pylint: disable=unnecessary-lambda
                           'param2': lambda x: '<>', },
        xp.Msg_MouseUp: {'name': 'MouseUp',
                         'param1': lambda x: WidgetMsgHelper.mouseState(x),  # pylint: disable=unnecessary-lambda
                         'param2': lambda x: '<>', },
        xp.Msg_Reshape: {'name': 'Reshape',  # (drag the window to generate a "reshape")
                         'param1': lambda x: f'Widget: {x}',
                         'param2': lambda x: WidgetMsgHelper.widgetGeometry(x), },  # pylint: disable=unnecessary-lambda
        xp.Msg_ExposedChanged: {'name': 'ExposedChanged',
                                'param1': lambda x: '<>',
                                'param2': lambda x: '<>', },
        xp.Msg_AcceptChild: {'name': 'AcceptChild',
                             'param1': lambda x: f'Child widget: {x}',
                             'param2': lambda x: '<>', },
        xp.Msg_LoseChild: {'name': 'LoseChild',
                           'param1': lambda x: f'Child widget: {x}',
                           'param2': lambda x: '<>', },
        xp.Msg_AcceptParent: {'name': 'AcceptParent',
                              'param1': lambda x: f'Parent widget: {x or "No Parent"}',
                              'param2': lambda x: '<>', },
        xp.Msg_Shown: {'name': 'Shown',
                       'param1': lambda x: f'Shown widget: {x}',
                       'param2': lambda x: '<>', },
        xp.Msg_Hidden: {'name': 'Hidden',
                        'param1': lambda x: f'Shown widget: {x}',
                        'param2': lambda x: '<>', },
        xp.Msg_DescriptorChanged: {'name': 'DescriptorChanged',
                                   'param1': lambda x: '<>',
                                   'param2': lambda x: '<>', },
        xp.Msg_PropertyChanged: {'name': 'PropertyChanged',
                                 'param1': lambda x: WidgetMsgHelper.propertyID(x),  # pylint: disable=unnecessary-lambda
                                 'param2': lambda x: x, },
        xp.Msg_MouseWheel: {'name': 'MouseWheel',
                            'param1': lambda x: WidgetMsgHelper.mouseState(x),  # pylint: disable=unnecessary-lambda
                            'param2': lambda x: '<>', },
        xp.Msg_CursorAdjust: {'name': 'CursorAdjust',
                              'param1': lambda x: WidgetMsgHelper.mouseState(x),  # pylint: disable=unnecessary-lambda
                              'param2': lambda x: f'<pointer 0x{x[0]:x}>', },
        xp.Msg_UserStart: {'name': 'UserStart',
                           'param1': lambda x: '<>',
                           'param2': lambda x: '<>', },
        xp.Msg_PushButtonPressed: {'name': 'PushButtonPressed',
                                   'param1': lambda x: f'Widget: {x}',
                                   'param2': lambda x: '<>', },
        xp.Msg_ButtonStateChanged: {'name': 'ButtonStateChanged',
                                    'param1': lambda x: f'Widget: {x}',
                                    'param2': lambda x: f'New Value: {x}', },
        xp.Msg_TextFieldChanged: {'name': 'TextFieldChanged',  # In 2012 this was reported broken. Still (2020) I can't generate it.
                                  'param1': lambda x: f'Widget: {x}',
                                  'param2': lambda x: '<>', },
        xp.Msg_ScrollBarSliderPositionChanged: {'name': 'ScrollBarSliderPositionChanged',
                                                'param1': lambda x: f'Widget: {x}',
                                                'param2': lambda x: '<>', },
        xp.Message_CloseButtonPushed: {'name': 'CloseButtonPushed',
                                       'param1': lambda x: '<>',
                                       'param2': lambda x: '<>', },
    }
    prop_values = {
        xp.Property_MainWindowType: 'MainWindowType',
        xp.Property_MainWindowHasCloseBoxes: 'MainWindowHasCloseBoxes',
        xp.Property_SubWindowType: 'SubWindowType',
        xp.Property_ButtonType: 'ButtonType',
        xp.Property_ButtonBehavior: 'ButtonBehavior',
        xp.Property_ButtonState: 'ButtonState',
        xp.Property_EditFieldSelStart: 'EditFieldSelStart',
        xp.Property_EditFieldSelEnd: 'EditFieldSelEnd',
        xp.Property_EditFieldSelDragStart: 'EditFieldSelDragStart',
        xp.Property_TextFieldType: 'TextFieldType',
        xp.Property_PasswordMode: 'PasswordMode',
        xp.Property_MaxCharacters: 'MaxCharacters',
        xp.Property_ScrollPosition: 'ScrollPosition',
        xp.Property_Font: 'Font',
        xp.Property_ActiveEditSide: 'ActiveEditSide',
        xp.Property_ScrollBarSliderPosition: 'ScrollBarSliderPosition',
        xp.Property_ScrollBarMin: 'ScrollBarMin',
        xp.Property_ScrollBarMax: 'ScrollBarMax',
        xp.Property_ScrollBarPageAmount: 'ScrollBarPageAmount',
        xp.Property_ScrollBarType: 'ScrollBarType',
        xp.Property_ScrollBarSlop: 'ScrollBarSlop',
        xp.Property_CaptionLit: 'CaptionLit',
        xp.Property_GeneralGraphicsType: 'GeneralGraphicsType',
        xp.Property_ProgressPosition: 'ProgressPosition',
        xp.Property_ProgressMin: 'ProgressMin',
        xp.Property_ProgressMax: 'ProgressMax',
        xp.Property_Refcon: 'Refcon',
        xp.Property_Dragging: 'Dragging',
        xp.Property_DragXOff: 'DragXOff',
        xp.Property_DragYOff: 'DragYOff',
        xp.Property_Hilited: 'Hilited',
        xp.Property_Object: 'Object',
        xp.Property_Clip: 'Clip',
        xp.Property_Enabled: 'Enabled',
        xp.Property_UserStart: 'UserStart',
    }

    def __init__(self: Self, inMessage: XPWidgetMessage, inParam1: Any, inParam2: Any) -> None:
        self.inMessage = inMessage
        self.inParam1 = inParam1
        self.inParam2 = inParam2

    @staticmethod
    def mouseState(x: tuple[int, int, int, int]) -> str:
        return f'({x[0]}, {x[1]}) Btn: #{"left" if x[2] == 0 else "unknown"} delta:{x[3]}'

    @staticmethod
    def widgetGeometry(x: tuple[int, int, int, int]) -> str:
        return f"dx, dy: ({x[0]}, {x[1]}), dwidth, dheight: ({x[2]}, {x[3]})"

    @staticmethod
    def propertyID(x: int) -> str:
        return WidgetMsgHelper.prop_values[x]

    @staticmethod
    def keyState(x: tuple[int, int, int]) -> str:
        modifiers = []
        inFlags = x[1]
        if inFlags & xp.ShiftFlag:
            modifiers.append('Shift')
        if inFlags & xp.OptionAltFlag:
            modifiers.append('Alt')
        if inFlags & xp.ControlFlag:
            modifiers.append('Ctl')
        if inFlags & xp.DownFlag:
            modifiers.append('Key Down')
        if inFlags & xp.UpFlag:
            modifiers.append('Key Up')
        return f'{x[0]} [{" ".join(modifiers)}], #{x[2]}'

    def __str__(self: Self) -> str:
        m = WidgetMsgHelper.msgs[self.inMessage]
        param1 = m['param1'](self.inParam1) if callable(m['param1']) else self.inParam1
        param2 = m['param2'](self.inParam2) if callable(m['param2']) else self.inParam2
        return f"Received: {m['name']}, ({param1}, {param2})"
