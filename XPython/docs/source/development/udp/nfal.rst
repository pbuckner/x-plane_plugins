NFAL/NREC - Fail/Recover NavAid
+++++++++++++++++++++++++++++++

:Send:
   Fail a navaid by sending ``NFAL`` with the navaid id.::

     navaid = 'NOLLA (SEATTLE) NDB'
     msg = struct.pack('<4sx150s', b'NFAL',
                       navaid.encode('utf-8')   # remember to encode string as bytes
     sock.sendto(msg, (beacon['ip'], beacon['port']))

.. Note::
   The naviad "name" is used, which matches a value in the file ``Resources/default data/earth_nav.dat``. This
   is not a traditional naviad ID, so for now, you'll need to see how the navaid is listed in that file. This
   may change in the future.

   
:Send:
   To recover a navaid by sending the ``NREC`` command with the same navaid id.::
   
     navaid = 'NOLLA (SEATTLE) NDB'
     msg = struct.pack('<4sx150s', b'NREC',
                       navaid.encode('utf-8')   # remember to encode string as bytes
     sock.sendto(msg, (beacon['ip'], beacon['port']))

   
.. toctree::
   :maxdepth: 1
