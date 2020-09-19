"""
Camera.py

Ported to Python by Sandy Barbour - 28/04/2005
Ported to XPPython3 by Peter Buckner - 16-Jul-2020

This plugin registers a new view with the sim that orbits the aircraft.  We do this by:

1. Registering a hotkey to engage the view.
2. Setting the view to external when we are engaged.
3. Registering a new camera control funcioin that ends when a new view is picked.
"""

import math
import xp
try:
    from XPListBox import XPCreateListBox
except ImportError:
    print("XPListBox is a custom python file provided with XPPython3, and required by this example you could copy it into PythonPlugins folder")
    raise


class PythonInterface:
    def __init__(self):
        self.Name = "Camera1"
        self.Sig = "camera1.demos.xppython3"
        self.Desc = "An example using the camera module."
        self.PlaneX = None
        self.PlaneY = None
        self.PlaneZ = None
        self.HotKey = None

        self.windowWidgetID = None
        self.listboxWidget = None

    def XPluginStart(self):
        # Prefetch the sim variables we will use.
        self.PlaneX = xp.findDataRef("sim/flightmodel/position/local_x")
        self.PlaneY = xp.findDataRef("sim/flightmodel/position/local_y")
        self.PlaneZ = xp.findDataRef("sim/flightmodel/position/local_z")

        # Register our hot key for the new view.
        self.HotKey = xp.registerHotKey(xp.VK_F8, xp.DownFlag, "Circling External View 1", self.MyHotKeyCallback, 0)
        return self.Name, self.Sig, self.Desc

    def XPluginStop(self):
        xp.unregisterHotKey(self.HotKey)

    def XPluginEnable(self):
        self.createLoggingWindow()
        self.clearDisplay()
        self.display("Python CameraCallback 1 - F8 to start")
        return 1

    def XPluginDisable(self):
        pass

    def XPluginReceiveMessage(self, inFromWho, inMessage, inParam):
        pass

    def MyHotKeyCallback(self, inRefcon):
        """
        This is the hotkey callback.  First we execute default_view command.
        This guarantees that no panels are showing and we are an external view.

        findCommand is relatively expensive, so ideally, we'd do that on XPluginStart and
        re-use the result.
        """
        xp.commandOnce(xp.findCommand("sim/view/default_view"))

        # Now we control the camera until the view changes.
        xp.controlCamera(xp.ControlCameraUntilViewChanges, self.MyOrbitPlaneFunc, 0)

    def MyOrbitPlaneFunc(self, outCameraPosition, inIsLosingControl, inRefcon):
        """
        MyOrbitPlaneFunc

        This is the actual camera control function, the real worker of the plugin.  It is
        called each time X-Plane needs to draw a frame.
        """
        if (inIsLosingControl):
            xp.dontControlCamera()

        if (outCameraPosition and not inIsLosingControl):
            self.clearDisplay()
            self.display("Python CameraCallback 1 Start")
            self.display("   Select any other view to Exit")
            self.display("outCameraPosition :- " + str(outCameraPosition))
            self.display("inIsLosingControl :- " + str(inIsLosingControl))
            self.display("inRefcon :- " + str(inRefcon))
            self.display("x       :- " + str(outCameraPosition[0]))
            self.display("y       :- " + str(outCameraPosition[1]))
            self.display("z       :- " + str(outCameraPosition[2]))
            self.display("pitch   :- " + str(outCameraPosition[3]))
            self.display("heading :- " + str(outCameraPosition[4]))
            self.display("roll    :- " + str(outCameraPosition[5]))
            self.display("zoom    :- " + str(outCameraPosition[6]))

            # First get the screen size and mouse location.  We will use this to decide
            # what part of the orbit we are in.  The mouse will move us up-down and around.
            w, h = xp.getScreenSize()
            x, y = xp.getMouseLocationGlobal()
            heading = 360.0 * float(x) / float(w)
            pitch = 20.0 * ((float(y) / float(h) * 2.0) - 1.0)

            # Now calculate where the camera should be positioned to be 200
            # meters from the plane and pointing at the plane at the pitch and
            # heading we wanted above.
            dx = -200.0 * math.sin(heading * 3.1415 / 180.0)
            dz = 200.0 * math.cos(heading * 3.1415 / 180.0)
            dy = -200 * math.tan(pitch * 3.1415 / 180.0)
            self.display("dx    :- {}".format(dx))
            self.display("dy    :- {}".format(dy))
            self.display("dz    :- {}".format(dz))

            # Fill out the camera position info.
            outCameraPosition[0] = xp.getDataf(self.PlaneX) + dx
            outCameraPosition[1] = xp.getDataf(self.PlaneY) + dy
            outCameraPosition[2] = xp.getDataf(self.PlaneZ) + dz
            outCameraPosition[3] = pitch
            outCameraPosition[4] = heading
            outCameraPosition[5] = 0
        return 1

    def createLoggingWindow(self, num_rows=16, num_characters=75):
        # Rather than hard-code a fixed size of scrolling area of list box,
        # we'll set size of box based on input parameters
        FontWidth, FontHeight, _other = xp.getFontDimensions(xp.Font_Basic)
        listbox_item_height = int(FontHeight * 1.2)
        left = 100
        bottom = 50

        top = bottom + (listbox_item_height) * num_rows
        right = left + int(num_characters * FontWidth)

        self.windowWidgetID = xp.createWidget(left - 5, top + 20, right + 5, bottom - 5, 1, "XPPython3", 1, 0, xp.WidgetClass_MainWindow)
        self.listboxWidget = XPCreateListBox(left, top, right, bottom, 1, self.windowWidgetID)

    def clearDisplay(self):
        self.listboxWidget.clear()

    def display(self, s):
        self.listboxWidget.add(s)
