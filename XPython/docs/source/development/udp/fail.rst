FAIL/RECO - Fail a System
+++++++++++++++++++++++++

:Send:

   Fail a particular system. Select system to fail by index::

     msg = struct.pack('<4sxi', b'FAIL', index)
     sock.sendto(msg, (beacon['ip'], beacon['port']))

----

:Send:
   To recover a particular system. Select system to by index::

     msg = struct.pack('<4sxi', b'RECO', index)
     sock.sendto(msg, (beacon['ip'], beacon['port']))

To recover all systems, use :doc:`RESE <rese>` command.

.. Note:: ``index`` doesn't appear to be valid anymore (X-Plane 11.55). Perhaps failures used to
          have an index number, but there is no current mapping of values -> failures.

.. Note::
   Bug report filed with Laminar Research Sep-14-2021.


.. toctree::
   :maxdepth: 1
