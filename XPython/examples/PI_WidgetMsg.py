from XPLMDefs import xplm_ShiftFlag, xplm_OptionAltFlag, xplm_ControlFlag, xplm_DownFlag, xplm_UpFlag
from XPLMDisplay import xplm_CursorDefault, xplm_CursorHidden, xplm_CursorArrow, xplm_CursorCustom
from XPLMGraphics import xplmFont_Proportional, XPLMGetFontDimensions
from XPWidgets import XPCreateWidget, XPAddWidgetCallback, XPSetWidgetProperty, XPSetWidgetDescriptor, XPPlaceWidgetWithin, XPShowWidget, XPHideWidget, XPDestroyWidget
from XPWidgetDefs import xpMsg_None, xpMsg_Create, xpMsg_Destroy, xpMsg_Paint, xpMsg_Draw, xpMsg_KeyPress
from XPWidgetDefs import xpMsg_KeyTakeFocus, xpMsg_KeyLoseFocus, xpMsg_MouseDown, xpMsg_MouseDrag, xpMsg_MouseUp
from XPWidgetDefs import xpMsg_Reshape, xpMsg_ExposedChanged, xpMsg_AcceptChild, xpMsg_LoseChild, xpMsg_AcceptParent
from XPWidgetDefs import xpMsg_Shown, xpMsg_Hidden, xpMsg_DescriptorChanged, xpMsg_PropertyChanged, xpMsg_MouseWheel
from XPWidgetDefs import xpMsg_CursorAdjust, xpMsg_UserStart
from XPWidgetDefs import xpProperty_Refcon, xpProperty_Dragging, xpProperty_DragXOff, xpProperty_DragYOff
from XPWidgetDefs import xpProperty_Hilited, xpProperty_Object, xpProperty_Clip, xpProperty_Enabled, xpProperty_UserStart
from XPStandardWidgets import xpWidgetClass_MainWindow, xpWidgetClass_Caption, xpWidgetClass_TextField, xpWidgetClass_Button
from XPStandardWidgets import xpWidgetClass_Progress, xpWidgetClass_ScrollBar
from XPStandardWidgets import xpMsg_PushButtonPressed, xpMsg_ButtonStateChanged
from XPStandardWidgets import xpMsg_TextFieldChanged, xpMsg_ScrollBarSliderPositionChanged
from XPStandardWidgets import xpProperty_MainWindowType, xpProperty_MainWindowHasCloseBoxes, xpProperty_SubWindowType
from XPStandardWidgets import xpProperty_ButtonType, xpProperty_ButtonBehavior, xpProperty_ButtonState
from XPStandardWidgets import xpProperty_EditFieldSelStart, xpProperty_EditFieldSelEnd, xpProperty_EditFieldSelDragStart
from XPStandardWidgets import xpProperty_TextFieldType, xpProperty_PasswordMode, xpProperty_MaxCharacters
from XPStandardWidgets import xpProperty_ScrollPosition, xpProperty_Font, xpProperty_ActiveEditSide
from XPStandardWidgets import xpProperty_ScrollBarSliderPosition, xpProperty_ScrollBarMin, xpProperty_ScrollBarMax
from XPStandardWidgets import xpProperty_ScrollBarPageAmount, xpProperty_ScrollBarType, xpProperty_ScrollBarSlop
from XPStandardWidgets import xpProperty_CaptionLit, xpProperty_GeneralGraphicsType, xpProperty_ProgressPosition
from XPStandardWidgets import xpProperty_ProgressMin, xpProperty_ProgressMax
from XPStandardWidgets import xpTextTranslucent
from XPStandardWidgets import xpButtonBehaviorRadioButton, xpRadioButton


msgs = {
    xpMsg_None: {'name': 'None',
                 'param1': lambda x: '<should never be called>',
                 'param2': lambda x: '<>',
    },
    xpMsg_Create: {'name': 'Create',  # done (for subclass only)
                   'param1': lambda x: 'Subclass' if x == 1 else 'Not subclass',
                   'param2': lambda x: '<>',
    },
    xpMsg_Destroy: {'name': 'Destroy',  # done
                    'param1': lambda x: 'Explicit deletion' if x == 0 else 'Recursive deletion',
                    'param2': lambda x: '<>',
    },
    xpMsg_Paint: {'name': 'Paint',  # done
                  'param1': lambda x: '<>',
                  'param2': lambda x: '<>',
    },
    xpMsg_Draw: {'name': 'Draw',  # done
                 'param1': lambda x: '<>',
                 'param2': lambda x: '<>',
    },
    xpMsg_KeyPress: {'name': 'KeyPress',  # done
                     'param1': lambda x: keyState(x),
                     'param2': lambda x: '<>',
    },
    xpMsg_KeyTakeFocus: {'name': 'KeyTakeFocus',  # done (someone else gave up focus???)
                         'param1': lambda x: 'Child gave up focus' if x == 1 else 'Someone else gave up focus',
                         'param2': lambda x: '<>',
    },
    xpMsg_KeyLoseFocus: {'name': 'KeyLoseFocus',  # done (both)
                         'param1': lambda x: 'Another widget is taking' if x == 1 else 'Someone called API to request remove focus',
                         'param2': lambda x: '<>',
    },
    xpMsg_MouseDown: {'name': 'MouseDown',  # done
                      'param1': lambda x: mouseState(x),
                      'param2': lambda x: '<>',
    },
    xpMsg_MouseDrag: {'name': 'MouseDrag',  # done
                      'param1': lambda x: mouseState(x),
                      'param2': lambda x: '<>',
    },
    xpMsg_MouseUp: {'name': 'MouseUp',  # done
                    'param1': lambda x: mouseState(x),
                    'param2': lambda x: '<>',
    },
    xpMsg_Reshape: {'name': 'Reshape',  # done (drag the window to generate a "reshape")
                    'param1': lambda x: 'Widget: {}'.format(x),
                    'param2': lambda x: widgetGeometry(x),
    },
    xpMsg_ExposedChanged: {'name': 'ExposedChanged',  # done
                           'param1': lambda x: '<>',
                           'param2': lambda x: '<>',
    },
    xpMsg_AcceptChild: {'name': 'AcceptChild',  # done
                        'param1': lambda x: 'Child widget: {}'.format(x),
                        'param2': lambda x: '<>',
    },
    xpMsg_LoseChild: {'name': 'LoseChild',  # done
                      'param1': lambda x: 'Child widget: {}'.format(x),
                      'param2': lambda x: '<>',
    },
    xpMsg_AcceptParent: {'name': 'AcceptParent',  # done
                         'param1': lambda x: 'Parent widget: {}'.format(x) if x else 'No Parent',
                         'param2': lambda x: '<>',
    },
    xpMsg_Shown: {'name': 'Shown',  # done
                  'param1': lambda x: 'Shown widget: {}'.format(x),
                  'param2': lambda x: '<>',
    },
    xpMsg_Hidden: {'name': 'Hidden',  # done
                   'param1': lambda x: 'Shown widget: {}'.format(x),
                   'param2': lambda x: '<>',
    },
    xpMsg_DescriptorChanged: {'name': 'DescriptorChanged',  # done
                              'param1': lambda x: '<>',
                              'param2': lambda x: '<>',
    },
    xpMsg_PropertyChanged: {'name': 'PropertyChanged',  # done
                            'param1': lambda x: propertyID(x),
                            'param2': lambda x: x,
    },
    xpMsg_MouseWheel: {'name': 'MouseWheel',
                       'param1': lambda x: mouseState(x),
                       'param2': lambda x: '<>',
    },
    xpMsg_CursorAdjust: {'name': 'CursorAdjust',  # done
                         'param1': lambda x: mouseState(x),
                         'param2': lambda x: '<pointer 0x{:x}>'.format(x),
    },
    xpMsg_UserStart: {'name': 'UserStart',
                      'param1': lambda x: '<>',
                      'param2': lambda x: '<>',
    },
    xpMsg_PushButtonPressed: {'name': 'PushButtonPressed',  # done
                              'param1': lambda x: 'Widget: {}'.format(x),
                              'param2': lambda x: '<>',
    },
    xpMsg_ButtonStateChanged: {'name': 'ButtonStateChanged',  # done
                               'param1': lambda x: 'Widget: {}'.format(x),
                               'param2': lambda x: 'New Value: {}'.format(x),
    },
    xpMsg_TextFieldChanged: {'name': 'TextFieldChanged',  # In 2012 this was reported broken. Still (2020) I can't generate it.
                             'param1': lambda x: 'Widget: {}'.format(x),
                             'param2': lambda x: '<>',
    },
    xpMsg_ScrollBarSliderPositionChanged: {'name': 'ScrollBarSliderPositionChanged',  # done
                                           'param1': lambda x: 'Widget: {}'.format(x),
                                           'param2': lambda x: '<>',
    },
}


class PythonInterface(object):
    def __init__(self):
        self.Sig = "xppython.widget.msgs"
        self.Name = "Regression Test {}".format(self.Sig)
        self.Desc = "Regression test {} example".format(self.Sig)

        fontID = xplmFont_Proportional
        charWidth = []
        charHeight = []
        XPLMGetFontDimensions(fontID, charWidth, charHeight, None)
        self.strHeight = int(charHeight[0])
        self.widgets = {}

    def XPluginStart(self):
        return self.Name, self.Sig, self.Desc

    def XPluginStop(self):
        if 'mainWindow' in self.widgets:
            XPDestroyWidget(self.widgets['mainWindow'], 1)

    def XPluginEnable(self):
        self.createWidgets()
        return 1

    def XPluginDisable(self):
        pass

    def XPluginReceiveMessage(self, inFromWho, inMessage, inParam):
        pass

    def createWidgets(self):
        # Some messages are sent over-and-over, filling up the log
        # in order to test more easily, limit the number of times
        # we'll print message _per_widget_
        for key, msg in msgs.items():
            msg['widgets'] = {}
            msg['max'] = 1000
            if msg['name'] in ("Draw", "Paint", "CursorAdjust", "MouseDrag", "Reshape"):
                msg['max'] = 10

        left = 100
        top = 400
        right = 600
        bottom = 50
        self.widgets['mainWindow'] = XPCreateWidget(left, top, right, bottom, 1, "Sample", 1, 0, xpWidgetClass_MainWindow)

        XPAddWidgetCallback(self.widgets['mainWindow'], self.mainWindowCallback)

        left += 10
        top -= 20
        margin = self.strHeight + 10
        self.widgets['caption'] = XPCreateWidget(left, top, left + 30, top - self.strHeight,
                                                 1, 'caption', 0, self.widgets['mainWindow'], xpWidgetClass_Caption)

        top -= margin
        self.widgets['textfield'] = XPCreateWidget(left, top, left + 30, top - self.strHeight,
                                                   1, 'textfield', 0, self.widgets['mainWindow'], xpWidgetClass_TextField)

        XPAddWidgetCallback(self.widgets['textfield'], self.widgetCallback)
        XPSetWidgetProperty(self.widgets['textfield'], xpProperty_TextFieldType, xpTextTranslucent)
        XPSetWidgetProperty(self.widgets['textfield'], xpProperty_TextFieldType, xpTextTranslucent)
        XPSetWidgetDescriptor(self.widgets['textfield'], '!!!')

        top -= margin
        self.widgets['button'] = XPCreateWidget(left, top, left + 10 + 30, top - self.strHeight,
                                                1, 'button', 0, self.widgets['mainWindow'], xpWidgetClass_Button)

        top -= margin
        self.widgets['radiobutton'] = XPCreateWidget(left, top, left + 10 + 30, top - self.strHeight,
                                                     1, 'radiobutton', 0, self.widgets['mainWindow'], xpWidgetClass_Button)
        XPSetWidgetProperty(self.widgets['radiobutton'], xpProperty_ButtonType, xpRadioButton)
        XPSetWidgetProperty(self.widgets['radiobutton'], xpProperty_ButtonBehavior, xpButtonBehaviorRadioButton)

        top -= margin
        self.widgets['scrollbar'] = XPCreateWidget(left, top, left + 10 + 100, top - self.strHeight,
                                                   1, 'scrollbar', 0, self.widgets['mainWindow'], xpWidgetClass_ScrollBar)

        XPSetWidgetProperty(self.widgets['scrollbar'], xpProperty_ScrollBarMin, 0)
        XPSetWidgetProperty(self.widgets['scrollbar'], xpProperty_ScrollBarMax, 100)
        XPSetWidgetProperty(self.widgets['scrollbar'], xpProperty_ScrollBarSliderPosition, 33)

        top -= margin
        self.widgets['progress'] = XPCreateWidget(left, top, left + 10 + 100, top - self.strHeight,
                                                  1, 'progress', 0, self.widgets['mainWindow'], xpWidgetClass_Progress)
        XPAddWidgetCallback(self.widgets['progress'], self.widgetCallback)
        XPSetWidgetProperty(self.widgets['progress'], xpProperty_ProgressMin, 0)
        XPSetWidgetProperty(self.widgets['progress'], xpProperty_ProgressMax, 100)
        XPSetWidgetProperty(self.widgets['progress'], xpProperty_ProgressPosition, 66)
        XPPlaceWidgetWithin(self.widgets['progress'], 0)
        XPPlaceWidgetWithin(self.widgets['progress'], self.widgets['mainWindow'])

        XPHideWidget(self.widgets['progress'])
        XPShowWidget(self.widgets['progress'])

    def widgetCallback(self, inMessage, inWidget, inParam1, inParam2):
        return self.mainWindowCallback(inMessage, inWidget, inParam1, inParam2)

    def mainWindowCallback(self, inMessage, inWidget, inParam1, inParam2):
        try:
            try:
                msgs[inMessage]['widgets'][str(inWidget)] -= 1
            except KeyError:
                msgs[inMessage]['widgets'][str(inWidget)] = msgs[inMessage]['max']
            if msgs[inMessage]['widgets'][str(inWidget)] > 0:
                print('{} received: {}, ({}, {}) [#{}]'.format(inWidget,
                                                               msgs[inMessage]['name'],
                                                               msgs[inMessage]['param1'](inParam1),
                                                               msgs[inMessage]['param2'](inParam2),
                                                               msgs[inMessage]['widgets'][str(inWidget)]))
        except:
            print("Unable to translate message {}".format(inMessage))
            raise

        if inMessage == xpMsg_Paint:
            return 0  # so 'draw' is called
        if inMessage == xpMsg_CursorAdjust:
            inParam2 = xplm_CursorArrow
            return 1
        return 0  # forward message to "next"


def mouseState(x):
    return '({}, {}) Btn: #{} delta:{}'.format(x[0], x[1], 'left' if x[2] == 0 else 'unknown', x[3])


def keyState(x):
    modifiers = []
    inFlags = x[1]
    if inFlags & xplm_ShiftFlag:
        modifiers.append('Shift')
    if inFlags & xplm_OptionAltFlag:
        modifiers.append('Alt')
    if inFlags & xplm_ControlFlag:
        modifiers.append('Ctl')
    if inFlags & xplm_DownFlag:
        modifiers.append('Key Down')
    if inFlags & xplm_UpFlag:
        modifiers.append('Key Up')
    return '{} [{}], #{}'.format(x[0], ' '.join(modifiers), x[2])


def widgetGeometry(x):
    return "dx, dy: ({}, {}), dwidth, dheight: ({}, {})".format(x[0], x[1], x[2], x[3])


def cursorStatus(x):
    values = {xplm_CursorDefault: 'Default',
              xplm_CursorHidden: 'Hidden',
              xplm_CursorArrow: 'Arrow',
              xplm_CursorCustom: 'Custom'}
    try:
        return values[x]
    except KeyError:
        print("Bad value for cursorStatus: {}".format(x))
        raise


def propertyID(x):
    values = {
        xpProperty_MainWindowType: 'MainWindowType',
        xpProperty_MainWindowHasCloseBoxes: 'MainWindowHasCloseBoxes',
        xpProperty_SubWindowType: 'SubWindowType',
        xpProperty_ButtonType: 'ButtonType',
        xpProperty_ButtonBehavior: 'ButtonBehavior',
        xpProperty_ButtonState: 'ButtonState',
        xpProperty_EditFieldSelStart: 'EditFieldSelStart',  # done
        xpProperty_EditFieldSelEnd: 'EditFieldSelEnd',  # done
        xpProperty_EditFieldSelDragStart: 'EditFieldSelDragStart',  # done
        xpProperty_TextFieldType: 'TextFieldType',
        xpProperty_PasswordMode: 'PasswordMode',
        xpProperty_MaxCharacters: 'MaxCharacters',
        xpProperty_ScrollPosition: 'ScrollPosition',  # done
        xpProperty_Font: 'Font',
        xpProperty_ActiveEditSide: 'ActiveEditSide',  # done
        xpProperty_ScrollBarSliderPosition: 'ScrollBarSliderPosition',
        xpProperty_ScrollBarMin: 'ScrollBarMin',
        xpProperty_ScrollBarMax: 'ScrollBarMax',
        xpProperty_ScrollBarPageAmount: 'ScrollBarPageAmount',
        xpProperty_ScrollBarType: 'ScrollBarType',
        xpProperty_ScrollBarSlop: 'ScrollBarSlop',
        xpProperty_CaptionLit: 'CaptionLit',
        xpProperty_GeneralGraphicsType: 'GeneralGraphicsType',
        xpProperty_ProgressPosition: 'ProgressPosition',
        xpProperty_ProgressMin: 'ProgressMin',
        xpProperty_ProgressMax: 'ProgressMax',
        xpProperty_Refcon: 'Refcon',
        xpProperty_Dragging: 'Dragging',  # done
        xpProperty_DragXOff: 'DragXOff',  # done
        xpProperty_DragYOff: 'DragYOff',  # done
        xpProperty_Hilited: 'Hilited',
        xpProperty_Object: 'Object',
        xpProperty_Clip: 'Clip',
        xpProperty_Enabled: 'Enabled',
        xpProperty_UserStart: 'UserStart',
    }
    return values[x]
