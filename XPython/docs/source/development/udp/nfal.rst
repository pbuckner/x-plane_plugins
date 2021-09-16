NFAL/NREC - Fail/Recover NavAid
+++++++++++++++++++++++++++++++

:Send:
   Fail a navaid by sending ``NFAL`` with the navaid id.::

     navaid = 'BF'
     msg = struct.pack('<4sx150s', b'NFAL',
                       navaid.encode('utf-8')   # remember to encode string as bytes
     sock.sendto(msg, (beacon['ip'], beacon['port']))

.. Note::
   This command does not appear to work (X-Plane 11.55). Bug filed with Laminar Research Sep-16-2021.
   Consider the NDB NOLLA near Seattle. The navaid name is 'NOLLA', the ID is 'BF', and the X-Plane internal id
   (as returned by ``XPLMFindNavAid('NOLLA', ..., xplm_Nav_NDB)`` is 341.
   *None* of these values for ``navaid`` works.

   
:Send:
   To recover a navaid by sending the ``NREC`` command with the same navaid id.::
   
     navaid = 'BF'
     msg = struct.pack('<4sx150s', b'NREC',
                       navaid.encode('utf-8')   # remember to encode string as bytes
     sock.sendto(msg, (beacon['ip'], beacon['port']))

.. Note:: This command does not appear to work (X-Plane 11.55). Bug filed with Laminar Research Sep-16-2021.
   
.. toctree::
   :maxdepth: 1
