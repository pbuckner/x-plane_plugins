"""
ShareData.py

Ported to Python by Sandy Barbour - 03/05/2005
Ported to XPPython3 by Peter Buckner - 2-Aug-2020

This is an example plugin that demonstrates how to share data, both owned
by a plugin and shared.

Data can be published in two ways: a plugin can publish data it owns.
In this case, it provides callbacks to read (and optionally write) the
data.  As other plugins access the data ref, the SDK calls back the
accessors.

Data can also be shared.  In this case, the SDK allocates the memory
for the data.  Each plugin that shares it registers a callback that is
called by the SDK when any plugin writes the data.

We use the xppython3 namespace to allocate unique data refs.  When creating
your own datarefs, make sure to prefix the data ref with a domain unique to
your organization.  'sim' is the domain for the main simulator.

This plugin "owns" dataref 'xppython3/demos/sharedata/number1'
a) we register it, indicating it's of type float and double
b) we provide Callbacks for it: My(Get|Set)Data(d|f)Callback

This plugin also "declares" the existence of a second dataref
'xppython3/demos/sharedata/sharedint1'. By calling xp.shareData()
we indicate this dataref exists, and the SDK will set aside space
for it, to live beyond the existence of this plugin (that is, if
this plugin become disabled, the dataref is still available to
other plugins.)

This is in contrast with the 'owned' dataref: when this plugin
becomes disabled, the dataref is no longer available to others.

Execute this plugin *with* PI_SharedData2.py and you'll see they're
sharing data.

Clicking on the window of this plugin, we'll get and print (to XPPython3.log)
the current value of the shared data.
"""

import xp


class PythonInterface:
    def XPluginStart(self):
        self.Name = "SharedData1"
        self.Sig = "sharedData1.demos.xppython3"
        self.Desc = "A plugin that shares a data ref."
        windowInfo = (50, 600, 300, 400, 1,
                      self.DrawWindowCallback,
                      self.MouseClickCallback,
                      self.KeyCallback,
                      self.CursorCallback,
                      self.MouseWheelCallback,
                      0,
                      xp.WindowDecorationRoundRectangle,
                      xp.WindowLayerFloatingWindows,
                      None)
        self.Window = xp.createWindowEx(windowInfo)

        self.OwnedFloatData = 1.5
        self.OwnedDoubleData = 2.5
        # Register our owned data.  Note that we pass two sets of
        # function callbacks for two data types and leave the rest blank.
        self.OwnedDataRef = xp.registerDataAccessor(
            "xppython3/demos/sharedata/number1",
            xp.Type_Float + xp.Type_Double,         # The types we support
            1,                                      # Writable
            0, 0,                                   # No accessors for ints
            self.MyGetDatafCallback, self.MySetDatafCallback,   # Accessors for floats
            self.MyGetDatadCallback, self.MySetDatadCallback,   # Accessors for doubles
            0, 0,                                   # No accessors for int arrays
            0, 0,                                   # No accessors for float arrays
            0, 0,                                   # No accessors for raw data
            0, 0)                                   # Refcons not used

        # Subscribe to shared data.  If no one else has made it, this will
        # cause the SDK to allocate the data.
        xp.shareData("xppython3/demos/sharedata/sharedint1", xp.Type_Int, self.MyDataChangedCallback, 0)
        self.SharedDataRef = xp.findDataRef("xppython3/demos/sharedata/sharedint1")
        self.Buffer = "ShareData 1 - SharedDataRef := {}\n".format(self.SharedDataRef)
        print(self.Buffer)

        return self.Name, self.Sig, self.Desc

    def XPluginStop(self):
        xp.destroyWindow(self.Window)
        if (self.OwnedDataRef):
            xp.unregisterDataAccessor(self.OwnedDataRef)
        xp.unshareData("xppython3/demos/sharedata/sharedint1", xp.Type_Int, self.MyDataChangedCallback, 0)

    def XPluginEnable(self):
        return 1

    def XPluginDisable(self):
        pass

    def XPluginReceiveMessage(self, inFromWho, inMessage, inParam):
        pass

    def DrawWindowCallback(self, inWindowID, inRefcon):
        left, top, right, bottom = xp.getWindowGeometry(inWindowID)
        xp.drawTranslucentDarkBox(int(left), int(top), int(right), int(bottom))
        color = 1.0, 1.0, 1.0
        xp.drawString(color, left + 5, top - 20, "Click Here", 0, xp.Font_Basic)

    def KeyCallback(self, inWindowID, inKey, inFlags, inVirtualKey, inRefcon, losingFocus):
        pass

    def MouseClickCallback(self, inWindowID, x, y, inMouse, inRefcon):
        if (inMouse == xp.MouseDown):
            AccessorDataRef = xp.findDataRef("xppython3/demos/sharedata/number1")
            xp.setDataf(AccessorDataRef, 1.2345)
            DataRefFloat = xp.getDataf(AccessorDataRef)
            print("PI_SharedData1: Shared DataRefFloat 1 = {}".format(DataRefFloat))
            xp.setDatad(AccessorDataRef, 9.87654321234)
            DataRefDouble = xp.getDatad(AccessorDataRef)
            print("PI_SharedData1: Shared DataRefDouble 1 = {}".format(DataRefDouble))
        return 1

    def CursorCallback(self, inWindowID, x, y, inRefcon):
        return xp.CursorDefault

    def MouseWheelCallback(self, inWindowID, x, y, wheel, clicks, inRefcon):
        return 1

    def MyDataChangedCallback(self, inRefcon):
        """
        This is the callback for our shared data.  Right now we do not react
        to our shared data being chagned. (For "owned" data, we don't
        get a callback like this -- instead, our Accessors are called: MySetData(f|d)Callback.
        """
        pass

    # These callbacks are called by the SDK to read and write the sim.
    # We provide two sets of callbacks allowing our data to appear as
    # float and double.  This is done for didactic purposes; multityped
    # data is provided as a backward compatibility solution and probably
    # should not be used in initial designs as a convenience to client
    # code.

    def MyGetDatafCallback(self, inRefcon):
        return self.OwnedFloatData

    def MySetDatafCallback(self, inRefcon, inValue):
        self.OwnedFloatData = inValue
        pass

    def MyGetDatadCallback(self, inRefcon):
        return self.OwnedDoubleData

    def MySetDatadCallback(self, inRefcon, inValue):
        self.OwnedDoubleData = inValue
