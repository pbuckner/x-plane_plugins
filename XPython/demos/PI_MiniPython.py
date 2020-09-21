import traceback
import sys
import io
import webbrowser
import contextlib
import xp
from XPListBox import XPCreateListBox


class PythonInterface:
    def __init__(self):
        self.prevCommands = []
        self.historyIdx = -1
        self.locals = {'self': self}
        self.prevCommands = ['print("hello world")', ]
        self.historyIdx = -1
        self.widget1 = None
        self.textWidget = None
        self.button = None
        self.listboxWidget = None
        self.toggleCommandRef = None
        self.menuIdx = None

    def XPluginStart(self):
        width, height, _ignored = xp.getFontDimensions(xp.Font_Basic)
        self.widget1 = xp.createWidget(100, 500, 700, 110, 0, "Mini Python Interpreter", 1, 0, xp.WidgetClass_MainWindow)
        self.textWidget = xp.createWidget(110, 480, 680, 460, 1, self.prevCommands[-1], 0, self.widget1, xp.WidgetClass_TextField)
        self.button = xp.createWidget(110, 450, 150, 430, 1, "Do", 0, self.widget1, xp.WidgetClass_Button)
        self.docButton = xp.createWidget(610, 450, 650, 430, 1, "Help", 0, self.widget1, xp.WidgetClass_Button)
        col1 = 160
        col2 = 350
        row = 463
        xp.createWidget(col1, row, 480, row - height, 1,
                        '^A ^E: Beginning / End of line',
                        0, self.widget1, xp.WidgetClass_Caption)
        xp.createWidget(col2, row, 480, row - height, 1,
                        '^D: Delete char',
                        0, self.widget1, xp.WidgetClass_Caption)
        row -= height + 4
        xp.createWidget(col1, row, 480, row - height, 1,
                        '^N ^P: Next / Previous line',
                        0, self.widget1, xp.WidgetClass_Caption)
        xp.createWidget(col2, row, 480, row - height, 1,
                        '^K: Kill to EOL',
                        0, self.widget1, xp.WidgetClass_Caption)
        row -= height + 4
        xp.createWidget(col1, row, 480, row - height, 1,
                        '^F ^B: Move Forward / Back',
                        0, self.widget1, xp.WidgetClass_Caption)
        xp.createWidget(col2, row, 480, row - height, 1,
                        '<Return>: Execute line',
                        0, self.widget1, xp.WidgetClass_Caption)

        xp.setWidgetProperty(self.button, xp.Property_ButtonType, xp.PushButton)
        xp.setWidgetProperty(self.button, xp.Property_ButtonBehavior, xp.ButtonBehaviorPushButton)
        xp.setWidgetProperty(self.docButton, xp.Property_ButtonType, xp.PushButton)
        xp.setWidgetProperty(self.docButton, xp.Property_ButtonBehavior, xp.ButtonBehaviorPushButton)

        xp.setWidgetProperty(self.widget1, xp.Property_MainWindowHasCloseBoxes, 1)
        xp.addWidgetCallback(self.widget1, self.widgetMsgs)
        xp.addWidgetCallback(self.textWidget, self.textEdit)
        self.listboxWidget = XPCreateListBox(110, row - height - 8, 700, row - height - 300, 1, self.widget1)

        self.toggleCommandRef = xp.createCommand('xppython3/mini-python/toggle', 'Toggle Mini-Python window')
        xp.registerCommandHandler(self.toggleCommandRef, self.toggleCommand, 1, None)
        self.menuIdx = xp.appendMenuItemWithCommand(xp.findPluginsMenu(), 'Mini Python', self.toggleCommandRef)
        return 'Mini Python Interpreter', 'xppython3.minipython', 'For debugging / testing, the provides a mini python interpreter'

    def textEdit(self, message, widgetID, param1, param2):
        # Normally, most key presses (of printable keys) is handled by the TextField widget
        # Here, we want to intercept the keypress _first_
        if message == xp.Msg_KeyPress and not (param1[1] & xp.UpFlag):
            if param1[1] & xp.ControlFlag:
                if param1[2] == xp.VK_K:
                    start = xp.getWidgetProperty(widgetID, xp.Property_EditFieldSelStart, None)
                    xp.setWidgetDescriptor(widgetID, xp.getWidgetDescriptor(widgetID)[0:start])
                    return 1
                elif param1[2] == xp.VK_K:
                    start = xp.getWidgetProperty(widgetID, xp.Property_EditFieldSelStart, None)
                    xp.setWidgetDescriptor(widgetID, xp.getWidgetDescriptor(widgetID)[0:start])
                    return 1
                elif param1[2] == xp.VK_F:
                    numChars = len(xp.getWidgetDescriptor(widgetID))
                    start = min(numChars, xp.getWidgetProperty(widgetID, xp.Property_EditFieldSelStart, None) + 1)
                    xp.setWidgetProperty(widgetID, xp.Property_EditFieldSelStart, start)
                    xp.setWidgetProperty(widgetID, xp.Property_EditFieldSelEnd, start)
                    return 1
                elif param1[2] == xp.VK_B:
                    start = max(0, xp.getWidgetProperty(widgetID, xp.Property_EditFieldSelStart, None) - 1)
                    xp.setWidgetProperty(widgetID, xp.Property_EditFieldSelStart, start)
                    xp.setWidgetProperty(widgetID, xp.Property_EditFieldSelEnd, start)
                    return 1
                elif param1[2] == xp.VK_A:
                    xp.setWidgetProperty(widgetID, xp.Property_EditFieldSelStart, 0)
                    xp.setWidgetProperty(widgetID, xp.Property_EditFieldSelEnd, 0)
                    return 1
                elif param1[2] == xp.VK_E:
                    numChars = len(xp.getWidgetDescriptor(widgetID))
                    xp.setWidgetProperty(widgetID, xp.Property_EditFieldSelStart, numChars)
                    xp.setWidgetProperty(widgetID, xp.Property_EditFieldSelEnd, numChars)
                    return 1
                elif param1[2] == xp.VK_D:
                    start = xp.getWidgetProperty(widgetID, xp.Property_EditFieldSelStart, None)
                    text = xp.getWidgetDescriptor(widgetID)
                    xp.setWidgetDescriptor(widgetID, text[0:start] + text[start + 1:])
                    return 1

            if param1[2] == xp.VK_DOWN or (param1[2] == xp.VK_N and param1[1] & xp.ControlFlag):
                self.historyIdx = (self.historyIdx + 1) % len(self.prevCommands)
                xp.setWidgetDescriptor(widgetID, self.prevCommands[self.historyIdx])
                return 1
            if param1[2] == xp.VK_UP or (param1[2] == xp.VK_P and param1[1] & xp.ControlFlag):
                xp.setWidgetDescriptor(widgetID, self.prevCommands[self.historyIdx])
                self.historyIdx = (self.historyIdx - 1) % len(self.prevCommands)
                return 1
        return 0

    def widgetMsgs(self, message, widgetID, param1, param2):
        if message == xp.Message_CloseButtonPushed:
            xp.hideWidget(self.widget1)
            return 1

        if message == xp.Msg_PushButtonPressed and param1 == self.docButton:
            webbrowser.open('https://xppython3.rtfd.io/en/latest/development/index.html')
            return 1

        execute = False
        if message == xp.Msg_PushButtonPressed and param1 == self.button:
            execute = True
        if not execute:
            if message == xp.Msg_KeyPress:
                if param1[2] == xp.VK_RETURN and param1[1] & xp.DownFlag:
                    execute = True
        if execute:
            self.prevCommands.append(xp.getWidgetDescriptor(self.textWidget))
            self.historyIdx = -1
            xp.setWidgetDescriptor(self.textWidget, '')
            self.do(self.prevCommands[self.historyIdx])
            return 1
        return 0

    def toggleCommand(self, *args, **kwargs):
        if not self.widget1:
            return 0
        if xp.isWidgetVisible(self.widget1):
            xp.hideWidget(self.widget1)
        else:
            xp.showWidget(self.widget1)
        return 1

    def XPluginStop(self):
        if self.toggleCommandRef:
            xp.unregisterCommandHandler(self.toggleCommandRef,
                                        self.toggleCommand,
                                        1, None)
            self.toggleCommandRef = None
        if self.widget1:
            xp.destroyWidget(self.widget1, 1)
            self.widget1 = None
        if self.menuIdx:
            xp.removeMenuItem(xp.findPluginsMenu(), self.menuIdx)
            self.menuIdx = None

    def XPluginEnable(self):
        return 1

    def XPluginDisable(self):
        pass

    def XPluginReceiveMessage(self, inFromWho, inMessage, inParam):
        pass

    def do(self, s):
        """
        Generally, we properly execute one-line python code, either evals:
            >>> sys.version_info.major
            3
        or execs:
            >>> print(system.version_info.major)
            3
        We don't attempt to identify and execute multi-line statements:
            >>> for i in ['a', 'b']:
                                   ^
            SyntaxError: unexpected EOF while parseing

        """
        f = io.StringIO()
        e = io.StringIO()
        with contextlib.redirect_stderr(e):
            with contextlib.redirect_stdout(f):
                print(">>> {}".format(s))
                try:
                    exec(compile(s, '<string>', 'single'), globals(), self.locals)
                except:
                    type, value, tb = sys.exc_info()
                    traceback.print_exception(type, value, tb)

        s = f.getvalue().strip()
        if s:
            for line in s.split('\n'):
                self.listboxWidget.add(line)
        s = e.getvalue().strip()
        if s:
            for line in s.split('\n'):
                self.listboxWidget.add(line)
