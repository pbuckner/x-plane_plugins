NFAL/NREC - Fail/Recover NavAid
+++++++++++++++++++++++++++++++

:Send:
   Fail a navaid by sending ``NFAL`` with the navaid id.::

     navaid = 'NOLLA (SEATTLE) NDB'
     msg = struct.pack('<4sx150s', b'NFAL',
                       navaid.encode('utf-8')   # remember to encode string as bytes
     sock.sendto(msg, (beacon['ip'], beacon['port']))

.. Note::
   With **XP11**, the naviad "name" is used, which matches a value in the file ``Resources/default data/earth_nav.dat``. This
   is not a traditional naviad ID, so for now, you'll need to see how the navaid is listed in that file.

   With **XP12**, the navaid "id" is used, which matches the value in parentheses as listed in the Failures section
   of the Flight Configuration page. For example, near Seattle it lists:

    | SEATTLE VORTAC (SEA)
    | SEATTLE VORTAC DME (SEA)
    | SEATTLE-TACOMA INTL DME-ILS (IBEJ)

   You can fail 'SEA' which will fail both the VORTAC and the DME functionality. You cannot fail only one (via UDP).
   You can only fail navaids within the current region (about 4 degrees longitude, and 3 degrees latitude).
   
:Send:
   To recover a navaid by sending the ``NREC`` command with the same navaid id.::
   
     navaid = 'NOLLA (SEATTLE) NDB'
     msg = struct.pack('<4sx150s', b'NREC',
                       navaid.encode('utf-8')   # remember to encode string as bytes
     sock.sendto(msg, (beacon['ip'], beacon['port']))

   
.. toctree::
   :maxdepth: 1
