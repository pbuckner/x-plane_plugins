RPOS - Request Aircraft Position
++++++++++++++++++++++++++++++++

:Send:

 Single packet with ``RPOS`` command and number *as a string*, representing
 number of times per second you'd like X-Plane to send you a packet with
 current aircraft position.::
 
   msg = struct.pack('<4sx10s', b'RPOS', b'60')  # requesting 60 times /second
   sock.sendto(msg, (beacon['ip'], beacon['port']))

 You can use this to drive displays, moving maps, etc.

 To stop, set request to b'0' times per second::

   msg = struct.pack('<4sx10s', b'RPOS', b'0')  # requesting 0 times /second
   sock.sendto(msg, (beacon['ip'], beacon['port']))

----

:Receive:    

 Single packet with position data::

     (header,          # 'RPOS'
      dat_lon,	       # float longitude of the aircraft in X-Plane of course, in degrees
      dat_lat,	       # float latitude
      dat_ele,	       # float elevation above sea level in meters
      y_agl_mtr,       # float elevation above the terrain in meters
      veh_the_loc,     # float pitch, degrees
      veh_psi_loc,     # float true heading, in degrees
      veh_phi_loc,     # float roll, in degrees
      vx_wrl,	       # float speed in the x, EAST, direction, in meters per second
      vy_wrl,	       # float speed in the y, UP, direction, in meters per second
      vz_wrl,	       # float speed in the z, SOUTH, direction, in meters per second
      Prad,            # float roll rate in radians per second
      Qrad,            # float pitch rate in radians per second
      Rrad,            # float yaw rate in radians per second
      ) = struct.unpack("<4xdddffffffffff", packet)
    
.. toctree::
   :maxdepth: 1
