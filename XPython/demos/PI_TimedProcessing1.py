"""
TimedProcessing.c

Ported to Python by Sandy Barbour - 28/04/2005
Ported to XPPython3 by Peter Buckner - 2-Aug-2020

This example plugin demonstrates how to use the timed processing callbacks
to continuously record sim data to disk.

This technique can be used to record data to disk or to the network.  Unlike
UDP data output, we can increase our frequency to capture data every single
sim frame.  (This example records once per second.)

Use the timed processing APIs to do any periodic or asynchronous action in
your plugin.
"""

import os
import xp


class PythonInterface:
    def XPluginStart(self):
        self.Name = "TimedProcessing"
        self.Sig = "timedProcessing.demos.xppython3"
        self.Desc = "A plugin that records sim data."

        # Open a file to write to.  We locate the X-System directory
        # and then concatenate our file name.  This makes us save in
        # the X-System directory.  Open the file.

        print("system path is {}".format(xp.getSystemPath()))
        self.outputPath = os.path.join(xp.getSystemPath(), "timedprocessing1.txt")
        self.OutputFile = open(self.outputPath, 'w')

        # Find the data refs we want to record.
        self.PlaneLat = xp.findDataRef("sim/flightmodel/position/latitude")
        self.PlaneLon = xp.findDataRef("sim/flightmodel/position/longitude")
        self.PlaneEl = xp.findDataRef("sim/flightmodel/position/elevation")

        # Register our callback for once a second.  Positive intervals
        # are in seconds, negative are the negative of sim frames.  Zero
        # registers but does not schedule a callback for time.
        xp.registerFlightLoopCallback(self.FlightLoopCallback, 1.0, 0)
        return self.Name, self.Sig, self.Desc

    def XPluginStop(self):
        # Unregister the callback
        xp.unregisterFlightLoopCallback(self.FlightLoopCallback, 0)

        # Close the file
        self.OutputFile.close()

    def XPluginEnable(self):
        return 1

    def XPluginDisable(self):
        pass

    def XPluginReceiveMessage(self, inFromWho, inMessage, inParam):
        pass

    def FlightLoopCallback(self, elapsedMe, elapsedSim, counter, refcon):
        # The actual callback.  First we read the sim's time and the data.
        elapsed = xp.getElapsedTime()
        lat = xp.getDataf(self.PlaneLat)
        lon = xp.getDataf(self.PlaneLon)
        el = xp.getDataf(self.PlaneEl)

        # Write the data to a file.
        buf = "Time=%f, lat=%f,lon=%f,el=%f.\n" % (elapsed, lat, lon, el)
        self.OutputFile.write(buf)

        # Return 1.0 to indicate that we want to be called again in 1 second.
        return 1.0
