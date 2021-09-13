VEHX - Drive X-Planes Visuals
+++++++++++++++++++++++++++++

Perhaps you have a great flight model, but a bad visual system, so now you want to drive
X-Plane as a visual system.

Using the VEHX command, you can set the location and attitude of every plane. Note that
once you use this, you've also disabled the physics of the aircraft, so you'll need to
continue to drive all movement.

:Send:
   Stream of packets with ``VEHX`` command. Each packet will be acted upon immediately
   by X-Plane.::

     msg = struct.pack('<4sxidddfff', b'VEHX',
                       p,              # The index of the airplane you want to control.
                       dat_lat,        # latitude, in degrees
                       dat_lon,        # longitude, in degrees
                       dat_ele,        # elevation above sea level, in meters
                       veh_psi_true,   # heading, degrees true
                       veh_the,        # pitch, degrees
                       veh_phi)        # roll, degrees

   Use 0 for the main airplane that you fly to drive the visuals.

.. toctree::
   :maxdepth: 1
