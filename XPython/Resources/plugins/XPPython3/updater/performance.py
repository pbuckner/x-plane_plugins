from XPPython3 import xp
from XPPython3.xp_typing import PythonInterfaceType
from .my_widget_window import MyWidgetWindow

PLUGIN_MODULE_NAME = 4


class Performance:
    def __init__(self, interface: PythonInterfaceType):
        self.interface = interface
        self.window = MyWidgetWindow()
        self.status_idx = 0
        self.stats: dict = {}
        self.frame_rate_period_dref = xp.findDataRef('sim/time/framerate_period')

    def toggleCommand(self, _inCommand, inPhase, _inRefcon):
        if inPhase == xp.CommandBegin:
            if not (self.window and self.window.widgetID):
                self.createWindow()
                xp.setFlightLoopCallbackInterval(self.fLCallback, -1, 1, None)
            else:
                xp.setFlightLoopCallbackInterval(self.fLCallback, 0, 1, None)
                xp.destroyWidget(self.window.widgetID, 1)
                self.window.widgetID = None
        return 0

    def fLCallback(self, *_args, **_kwargs):
        def sum_merge(a, b):
            res = {}
            for plugin in set(a) | set(b):
                fields = set(a.get(plugin, {})) | set(b.get(plugin, {}))
                res[plugin] = {x: (a.get(plugin, {}).get(x, 0) + b.get(plugin, {}).get(x, 0)) for x in fields}
            return res

        maximum = 10
        data = xp.getPluginStats()

        if self.status_idx == 0:
            self.stats = data
        else:
            self.stats = sum_merge(self.stats, data)
        self.status_idx += 1

        if self.window and self.window.widgetID:
            if self.status_idx == maximum:
                total = self.stats[None]['fl'] + self.stats[None]['draw'] + self.stats[None]['customw']
                fontID = xp.Font_Proportional
                w = self.window.widgets
                for k, v in self.stats.items():
                    k = str(k) if k is not None else 'All'
                    if k + 'fl' in w:
                        (_left, top, right, bottom) = xp.getWidgetGeometry(w[k + 'customw'])
                        value = str(int(v['customw'] / maximum))
                        newleft = int(right - xp.measureString(fontID, value))
                        xp.setWidgetGeometry(w[k + 'customw'], newleft, top, right, bottom)
                        xp.setWidgetDescriptor(w[k + 'customw'], value)

                        (_left, top, right, bottom) = xp.getWidgetGeometry(w[k + 'draw'])
                        value = str(int(v['draw'] / maximum))
                        newleft = int(right - xp.measureString(fontID, value))
                        xp.setWidgetGeometry(w[k + 'draw'], newleft, top, right, bottom)
                        xp.setWidgetDescriptor(w[k + 'draw'], value)

                        (_left, top, right, bottom) = xp.getWidgetGeometry(w[k + 'fl'])
                        value = str(int(v['fl'] / maximum))
                        newleft = int(right - xp.measureString(fontID, value))
                        xp.setWidgetGeometry(w[k + 'fl'], newleft, top, right, bottom)
                        xp.setWidgetDescriptor(w[k + 'fl'], value)

                        (_left, top, right, bottom) = xp.getWidgetGeometry(w[k + 'pct'])
                        value = f"{100.0 * (v['customw'] + v['fl'] + v['draw']) / total:.1f}%"
                        newleft = int(right - xp.measureString(fontID, value))
                        xp.setWidgetGeometry(w[k + 'pct'], newleft, top, right, bottom)
                        xp.setWidgetDescriptor(w[k + 'pct'], value)
                frp = xp.getDataf(self.frame_rate_period_dref)
                xp.setWidgetDescriptor(w['frvalue'], f'{frp * 1000000:.0f} / {1.0 / frp:4.1f} fps')
                if self.stats[None]['fl'] > 0:
                    sq_frp = frp * frp
                    t = ((self.stats[None]['fl']
                          + self.stats[None]['draw']
                          + self.stats[None]['customw']) / (maximum * 1000000.0)) / sq_frp
                    xp.setWidgetDescriptor(w['fl_fps'],
                                           f"Cost: {(self.stats[None]['customw'] / (maximum * 1000000.0)) / sq_frp:4.1f} "
                                           f"+ {(self.stats[None]['draw'] / (maximum * 1000000.0)) / sq_frp:4.1f} "
                                           f"+ {(self.stats[None]['fl'] / (maximum * 1000000.0)) / sq_frp:4.1f} "
                                           f"= {t:5.2f} fps")
                self.status_idx = 0
            return -1
        else:
            return 0

    def performanceWindowCallback(self, inMessage, _inWidget, _inParam1, _inParam2):
        if inMessage == xp.Message_CloseButtonPushed:
            xp.commandOnce(self.interface.cmds[3]['commandRef'])
            return 1

        return 0

    def createWindow(self):
        fontID = xp.Font_Proportional
        _w, strHeight, _ignore = xp.getFontDimensions(fontID)
        # Only get enabled plugins
        data = sorted([x[PLUGIN_MODULE_NAME] for x in xp.pythonGetDicts()['plugins'].values() if not x[-1]])
        data.append('All')

        left = 100
        top = 300
        width = 525
        height = int((3 + len(data)) * (strHeight + 5) + 40)

        self.window.widgetID = xp.createWidget(left, top, left + width, top - height, 1, "Python Plugins Performance",
                                               1, 0, xp.WidgetClass_MainWindow)
        xp.setWidgetProperty(self.window.widgetID, xp.Property_MainWindowHasCloseBoxes, 1)
        xp.addWidgetCallback(self.window.widgetID, self.performanceWindowCallback)
        top -= 30
        right = left + width
        bottom = int(top - strHeight)

        # Top line -- header
        colRight = [240, 160, 80, 20]
        label = 'Plugin (times in μsec)'
        self.window.widgets['title' + 'label'] = xp.createWidget(
            left + 10, top, left + int(xp.measureString(fontID, label)),
            bottom, 1, label, 0, self.window.widgetID, xp.WidgetClass_Caption)

        for k in (('Custom Widgets', 'customw', colRight[0]),
                  ('Drawing Misc', 'draw', colRight[1]),
                  ('Flight Loop', 'fl', colRight[2]),
                  ('%', 'pct', colRight[3])):
            label, code, col = k
            strWidth = xp.measureString(fontID, label)
            self.window.widgets['title' + code] = xp.createWidget(
                right - col - int(strWidth), top, right - col, bottom,
                1, label, 0, self.window.widgetID, xp.WidgetClass_Caption)

        # widgets for each plugin line
        top -= int(strHeight * 1.5)
        bottom = int(top - 1.5 * strHeight)

        for k in data:
            if k == data[-1]:
                top -= 5
                bottom -= 5

            # shorten the displayed label, if it's too wide
            k_label = k
            strWidth = int(xp.measureString(fontID, k_label))
            if strWidth > right - 300:
                k_label = k_label.replace('Laminar Research', '.').replace('.plugins.PythonPlugins.', '...')
                strWidth = int(xp.measureString(fontID, k_label))
                if strWidth > right - 300:
                    k_label = '...' + k_label[-35:]
                    strWidth = int(xp.measureString(fontID, k_label))
            self.window.widgets[k + 'label'] = xp.createWidget(left + 10, top, left + strWidth, bottom,
                                                               1, k_label, 0,
                                                               self.window.widgetID, xp.WidgetClass_Caption)
            self.window.widgets[k + 'customw'] = xp.createWidget(right - 300, top, right - colRight[0], bottom,
                                                                 1, '', 0,
                                                                 self.window.widgetID, xp.WidgetClass_Caption)
            self.window.widgets[k + 'draw'] = xp.createWidget(right - 300, top, right - colRight[1], bottom,
                                                              1, '', 0,
                                                              self.window.widgetID, xp.WidgetClass_Caption)
            self.window.widgets[k + 'fl'] = xp.createWidget(right - 200, top, right - colRight[2], bottom,
                                                            1, '', 0,
                                                            self.window.widgetID, xp.WidgetClass_Caption)
            self.window.widgets[k + 'pct'] = xp.createWidget(right - 80, top, right - colRight[3], bottom,
                                                             1, '', 0,
                                                             self.window.widgetID, xp.WidgetClass_Caption)
            top -= strHeight + 5
            bottom = top - strHeight

        # skip a line for footer
        top -= int(strHeight) + 5
        bottom = int(top - strHeight)

        # Footer
        label = 'Current frame rate (in μsec):'
        strWidth = xp.measureString(fontID, label)
        self.window.widgets['frlabel'] = xp.createWidget(left + 10, top, left + int(strWidth), bottom,
                                                         1, label, 0, self.window.widgetID, xp.WidgetClass_Caption)
        label = "0"
        self.window.widgets['frvalue'] = xp.createWidget(left + 10 + int(strWidth) + 10, top, right - 300, bottom,
                                                         1, label, 0, self.window.widgetID, xp.WidgetClass_Caption)
        self.window.widgets['fl_fps'] = xp.createWidget(right - 225, top, right - 100, bottom,
                                                        1, 'fl_fps', 0, self.window.widgetID, xp.WidgetClass_Caption)
