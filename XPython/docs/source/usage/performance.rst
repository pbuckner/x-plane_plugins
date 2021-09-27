Plugin Performance
==================

The **Performance** popup displays all loaded python plugins, with the first line showing the
performance for all of XPPython3 plugin itself.
The columns show time spent (in microseconds μ) per frame, divided into categories:

  .. image:: /images/perf.png
             
You can compare these values with those provided by X-Plane's built-in Plugin Admin Performance tab:

  .. image:: /images/lr_performance.png

The *Flight Loop* column in the popup will very closely match the "Flight Loop" line in the X-Plane performance tab. (Though
it doesn't happen to in the images here.) This is the time spent executing a flight loop callback. For XPPython3 performance,
we've been able to determine *which* python plugin is using the most time. (Note the performance presentation itself is
done within the ``I_PI_Updater`` plugin, which is why it's consuming 65 μsec per frame: if you're not displaying the
performance window, the flight loop is canceled.)

The *Drawing Misc* column also closely matches the "Drawing Misc" line in the performance tab. This
covers work done within drawing callback, usually associated with windows you've created.

*Custom Widgets* reflect time spent working with custom widgets. Usually the largest amount of time
is spent drawing custom widgets, so if none are displayed, this number will be very small. This figure
*does not* include time drawing non-custom widgets. I don't have access to that time. X-Plane's "Drawing Window" value
appears to include all widget work, including drawing non-custom widgets (which we don't include).

Finally, the *Percentage* column shows how much of XPPython3's time is allocated to a particular plugin.
In the above example, you can see the PI_SeeAndAvoid plugin is responsible for about 82% of XPPython3's time,
a majority of that is due to "Drawing Misc". Indeed, SeeAndAvoid has an on-screen "Flight Following" window which
constantly updates based on the location of nearly aircraft: If you choose to not display Flight Following,
the plugin's "Drawing Misc" value drops to near zero.

In any case, I hope this is helpful: If you're developing python plugins, this can give you some
insight into how they're performing. If you're wondering what's slowing down your system, this might
help you determine the bottleneck. XPPython3, by itself, consumes very little time.

