RESE - Reset Failures
+++++++++++++++++++++

:Send:

   Reset (recover) all failed systems::

     msg = struct.pack('<4sx', b'RESE')
     sock.sendto(msg, (beacon['ip'], beacon['port']))

To fail or recover a single systems, use :doc:`FAIL <fail>` or :doc:`RECO <fail>` command.

.. toctree::
   :maxdepth: 1
