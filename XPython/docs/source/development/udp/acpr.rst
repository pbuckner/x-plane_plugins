ACPR - Load and Init the Airplane at Location
+++++++++++++++++++++++++++++++++++++++++++++

:Send:
   Single command which combines both the :doc:`ACFN <acfn>` and :doc:`PREL <prel>` datastructures.
   (Yes, repeating the "plane_index" user aircraft)::

     msg = struct.pack('<4sxi150s2xiii8siiddddd', b'ACPR',
                        plane_index,             # 0 -> User aircraft, otherwise 1-19
                        path.encode('utf-8'),    # remember to encode string as bytes
                        livery,                  # livery index for aircraft
                        type_start,              # See enumeration with PREL
                        plane_index,             # 0 -> User aircraft, otherwise 1-19
                        apt_id.encode('utf-8'),  # remember to encode string to bytes
                        apt_rwy_idx,             # it's an index, not the runway heading
                        apt_rwy_dir,             # again, an index
                        lat_deg, lon_deg,        # Not needed, if you use apt_id
                        ele_mtr,                 # elevation meters
                        psi_tru,                 # aircraft heading true
                        spd_msc)                 # speed meters per second
     sock.sendto(msg, (beacon['ip'], beacon['port']))
     

.. toctree::
   :maxdepth: 1
