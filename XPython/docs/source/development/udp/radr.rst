RADR - Weather Radar
++++++++++++++++++++

:Send:

   Single packet with ``RADR`` command and number of radar points per frame
   you'd like X-Plane to send you.::

     msg = struct.pack('<4sx10s', b'RADR', b'10')  # requesting 10 radar points per frame
     sock.sendto(msg, (beacon['ip'], beacon['port']))

   To stop, set request to b'0' times per frame.::

     msg = struct.pack('<4sx10s', b'RADR', b'0')  # requesting 0 radar points per frame
     sock.sendto(msg, (beacon['ip'], beacon['port']))

----

Received data differs for XP11 and XP12:

X-Plane 11
----------

:Receive:

   Single packet with radar data::

     (header,       # == 'RADR'
      lon,          # float longitude of radar point
      lat,          # float latitude
      storm_level,  # precipitation level, 0 to 100
      storm_height  # storm tops in meters MSL
      ) = struct.unpack("<4xffBf", packet)

.. Note::

    X-Plane 11.55 (at least) documentation indicates 4-byte ``float storm_level_0_100`` whereas it is
    actually a single unsigned char.


X-Plane 12
----------

:Receive:

   Single packet with radar data::

     (header,       # == 'RADR'
      lon,          # float longitude of radar point
      lat,          # float latitude
      bases_meters, # float cloud bases in meters MSL
      tops_meters,  # float cloud tops in meters MSL
      clouds ratio, # float ratio, clouds present in the lat and lon
      precip_ratio  # float ratio, precipitation present at this lat and lon
      ) = struct.unpack("<4xffffff", packet)
    
.. toctree::
   :maxdepth: 1
