DREF - Set Datarefs
+++++++++++++++++++

:Send:
   
   Setting a dataref is trivial::

     msg = struct.pack('<4sxf500s', b'DREF',
                       value,
                       dataref.encode('utf-8'))
     sock.sendto(msg, (beacon['ip'], beacon['port']))

Other than the count of bytes sent (==509) you'll not get any acknowledgement or return from X-Plane.

Note that ``value`` is **always** a float: You'll need to cast integer values to float before packing. You'll
also need to convert string characters to floats (e.g., ``float(ord('C'))``) if that's what you're trying to
set.

Similar to Get Datarefs (see :doc:`RREF <rref>`), arrays are sent *one element at a time*, ``sim/aircraft/view/acf_ICAO[0]``, ``sim/aircraft/view/acf_ICAO[1]``, etc.

.. Note::
    X-Plane 11.55 (at least) does not support updating ``byte[x]`` type of datarefs. You can
    send the UDP, and X-Plane will receive it correctly but does not actually make a change.
    A bug has been filed with Laminar Research: as XPD-11353.
  

.. toctree::
   :maxdepth: 1
