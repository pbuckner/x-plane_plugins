VEHS - Drive X-Planes Visuals Single
++++++++++++++++++++++++++++++++++++

This is just like the :doc:`vehx`, but it does not stop the X-Plane flight model: it
allows you to move the place one time... Then the flight model continues to run
from the new location/orientation, and the physics is not disabled.

:Send:
   Send single packet with ``VEHS`` command::

     msg = struct.pack('<4sxidddfff', b'VEHS',
                       p,              # The index of the airplane you want to control.
                       dat_lat,        # latitude, in degrees
                       dat_lon,        # longitude, in degrees
                       dat_ele,        # elevation above sea level, in meters
                       veh_psi_true,   # heading, degrees true
                       veh_the,        # pitch, degrees
                       veh_phi)        # roll, degrees
     sock.sendto(msg, (beacon['ip'], beacon['port']))

   Use 0 for the main airplane that you fly to drive the visuals.

.. toctree::
   :maxdepth: 1
