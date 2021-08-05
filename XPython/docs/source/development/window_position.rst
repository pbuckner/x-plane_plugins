More about Window Positioning
=============================

When you create a new window using :py:func:`XPLMDisplay.XPLMCreateWindowEx`, you're required
to specify coordinate positions: left, top, right, and bottom. What do these coordinates represent, especially
as they relate to multiple-monitor setups, or virtual reality?

.. image:: /images/single-monitor.png
           :align: right

In overly simplistic terms the coordinated represent points on a coordinate system where (0, 0) is the lower left of your
screen and positive number move up and to the right, such as seen in the image on the right.

With a single monitor you can use :py:func:`XPLMDisplay.XPLMGetScreenBoundsGlobal` to get the size of the monitor, with (0, 0)
being in the lower left corner.

However, with multiple monitors *even if X-Plane isn't using more than one monitor*,
XPLMGetScreenBoundsGlobal() return the usable area of the screen, but using Global coordinates. Which means (0,0) is not
guaranteed to be the lower left of your windows.

Consider a three-monitor PC, with the middle monitor configured as the "main" monitor. For fun, also assume the monitors are
differently sized.

.. image:: /images/three-monitor.png
           :align: center

In the above image, you can see the pixel resolution of each monitor. Note that X-Plane uses "boxels" rather than "pixels" and will
automatically scale high-resolution displays. For other displays, boxels == pixels

So, where is point (0, 0)? It's the bottom left of the main window You can determine this using :py:func:`XPLMDisplay.XPLMGetAllMonitorBoundsOS` (``getAllMonitorBoundsOS`` returns immediately, so though it uses a callback, we don't need to do any fancy programming.):

.. code::

   def get_bounds(idx, left, top, right, bottom, data):
       data[idx] = (left, top, right, bottom)

   bounds = {}
   xp.getAllMonitorBoundsOS(get_bounds, bounds)
   for idx, values in bounds.items():
       print("[{}] {}".format(idx, values)

Which would return:

.. code::

   [0] (0, 1440, 2560, 0)
   [1] (2560, 1840, 3760, -80)
   [2] (-1280, 1368, 0, 344)

Look at those results more closely: (0, 0) is indeed lower left of the main monitor. Monitor index numbers are established by the
OS, not X-Plane, so the index numbers don't necessarily go from left-to-right. Because the Right monitor (#1) extends lower than the
main monitor, it's vertical extent runs from -80 to 1840. Similarly for the left monitor which starts a bit higher than the main monitor, so
it's vertical extent is 344 to 1368.

Where is X-Plane? We don't know yet -- we've only looked at what the OS is telling us.

Let's look at a few different ways of running X-Plane:

 * Single window on one of the monitors ("Windowed Simulator")
 * Full Screen on one of the monitors
 * Full Screen on two of the monitors

Windowed Simulator
------------------
When running as a window (that is, not full screen) the results are simple:
The lower left of the window is (0, 0). The vertical and horizontal
and vertical extents match the size of the screen (not the monitor). This is true regardless of
which monitor the window may appear on. It is trival to display something in the center of the screen as
it's simply half the vertical and half the horizontal size.

Full Screen on One Monitor
--------------------------

For Full Screen on one of the monitors, :py:func:`XPLMDisplay.XPLMGetScreenBoundsGlobal` returns the coordinates for the
screen being used.

For example, if we're full screen on the main monitor (#0), we'll get:

.. code::

     (0, 1440, 2560, 0) == xp.getScreenBoundsGlobal()

If we're full screen on the left monitor (#2), we'll get:

.. code::

     (-1280, 1368, 0, 344) == xp.getScreenBoundsGlobal()

This means you can't simply create a popup window at (100, 500) and assume it will be displayed with it's upper-left corner
100 pixels from the left side and 500 pixels up from the bottom. You'd need to get the BoundsGlobal and then add
+100 and +500 to make the proper placement. So, if you're running on monitor #2, place the upper-left corner of your window at (-1180, 844).

Want to place a 100x100 window in the lower right of the screen? Place it at:

.. image:: /images/xp2.png
           :align: center

.. code::

     (left, top, right, bottom) = xp.getScreenBoundsGlobal()
     xp.createWindowEX([right-100, bottom+100, right, bottom, ...])

Full Screen on Two Monitors
---------------------------

More than one monitor means ``getScreenBoundsGlobal()`` returns virtual space: a single rectangle
which may or may not be fully displayable on your monitors.

For example, if we're using Monitors #0 and #2, ``getScreenBoundsGlobal()`` will return *the combined extent* of
monitors 0 and 2 or:

.. code::

   (-1280, 1440, 2560, 0)
   -or-
   (-1280, 1440)----------(2560, 1440)
         |                     |
         | ScreenBoundsGlobal  |
         |     3840 x 1440     |
         |                     |
   (-1280, 0)-------------(2560, 0)
   

But, the upper-right corner *is not visible*. The point (-1280, 1440) is above the top of the left-most monitor, as it goes only up to 1328. The
lower-right corder (-1280, 0) is similarly not visible as the left monitor does not extend down as low as the main monitor.
MonitorBoundsOS would tell us:

.. code::

      (0, 1440)----------(2560, 1440)
        |                     |
        |  AllGlobalOS: [0]   |
        |     2560 x 1440     |
        |                     |
      (0, 0)-------------(2560, 0)

   (2560, 1840)----------(3760, 1840)
        |                     |
        |  AllGlobalOS: [1]   |
        |     1200 x 1920     |
        |                     |
   (2560, -80)-----------(3760, -80)

  (-1280, 1368)----------(0, 1368)
        |                  |
        | AllGlobalOS: [2] |
        |   1280 x 1024    |
        |                  |
  (-1280, 344)-----------(0, 344)

What's the lesson? If you're going to programmatically place a window, you need to fully understand the coordinate systems.

Positioning Windows
-------------------

So how does one position windows? You can use a combination of :py:func:`XPLMDisplay.XPLMSetWindowPositioningMode` and the bounds information
described above.

 * :data:`XPLMDisplay.xplm_WindowPositionFree`: WindowPositionFree is the default and will put the window at whatever coordinates
   defined using CreateWindowEx (or subsequently changed using :py:func:`XPLMDisplay.XPLMSetWindowGeometry`).

 * :data:`XPLMDisplay.xplm_WindowCenterOnMonitor`: WindowCenterOnMonitor will center the window on the specified monitor index, with -1
   setting it centered on the main X-Plane monitor, whichever that might be. (Note: the Main X-Plane Monitor is the one with the menu,
   which may not be the Main OS Monitor!)
   In this case, values passed using CreateWindowEx are ignored except
   to determine the length and width of the window.

 * :data:`XPLMDisplay.xplm_WindowPopOut`: WindowPopup will display the window outside of X-Plane, as an OS-hosted window. This window
   can be on any monitor, even monitors not being used by X-Plane. The coordinates of the window still match the overall coordinates.

 * :data:`XPLMDisplay.xplm_WindowVR`: The window is centered in the VR display. This is similar to ``WindowCenterOnMonitor`` in that
   the length and width of the window are obtained from the window geometry. The window position is simply centered and remains centered
   in the user's view as the user moves around.

(The other to positioning modes ``WindowFullScreenOnMonitor``, and ``WindowFullScreenOnAllMonitors`` are rarely used but should be
self-explanatory.)

Note that the positioning mode is set *after the window is created* using :py:func:`XPLMDisplay.XPLMSetWindowPositioningMode`. There
is no way to set it first. So if you want to display a window in the center of the main screen *and let the user move it*, you can:

.. code::

   [1]  windowID = xp.createWindowEx([0, height, width, 0,...])
   [2]  xp.setWindowPositioningMode(windowID, xp.WindowCenterOnMonitor, -1)
   [3]  (left, right, top, bottom) = xp.getWindowGeometry(windowID)
   [4]  xp.setWindowGeometry(windoID, left, right, top, bottom)
   [5]  xp.setWindowPositioningMode(windowID, xp.WindowPositionFree)

[1]: Since you'll start with window positioning mode as CenterOnMonitor, need to specific the height and
width of the window, not the actual coordinates.

[2]: Immediately set positioning mode after the window is created. ``-1`` puts it on the main X-Plane monitor. ``2`` would put it
on monitor index #2, and an invalid index, say ``20``, would also place it on the main X-Plane monitor.
``0`` places it on the main OS monitor (since the main OS monitor is always #0).

[3]: Get the *actual* window geometry, which will be different than the four values provided with CreateWindowEx.

[4]: Immediately *update* the stored geometry for the window. If you don't do this, then when you set positioning to Free,
it will move the window to the original specification: (0, height, width, 0).

[5]: By setting window position to Free, the user can now move the window.

Example
-------

See demo code in plugin :doc:`PI_Bounds.py </development/samples>`
