ACFN - Load an Aircraft
+++++++++++++++++++++++

Load an aircraft. Set index to 0, to load the user aircraft. Otherwise, use 1-19 for AI aircraft.

:Send:
   Index of plane to update with the *relative* path of the aircraft's ACF file
   (e.g, ``Aircraft/Laminar Research/Cessna 172 SP/Cessna_172SP.acf``).::

     msg = struct.pack('<4sxi150s2xi', b'ACFN',
                       plane_index,           # 0 -> User aircraft, otherwise 1-19
                       path.encode('utf-8'),  # remember to encode string as bytes
                       livery)                # livery index for aircraft
     sock.sendto(msg, (beacon['ip'], beacon['port']))

.. toctree::
   :maxdepth: 1
