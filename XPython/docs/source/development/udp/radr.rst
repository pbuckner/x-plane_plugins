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

:Receive:

   Single packet with radar data::

     (header,       # == 'RADR5'
      lon,          # float longitude of radar point
      lat,          # float latitude
      storm_level,  # precipitation level, 0 to 100
      storm_height  # storm tops in meters MSL
      ) = struct.unpack("<5sffBf", packet)

.. Note::

    X-Plane 11.55 (at least) documentation indicates the first five bytes received are ``RADR\x00``.
    In reality, ``RADR5`` is sent (without a null).
    Additionally, documentation indicates 4-byte ``float storm_level_0_100`` whereas it is
    actually a single unsigned char.
    
    A bug has been filed with Laminar Research: 2021-Sep-2.

.. toctree::
   :maxdepth: 1
