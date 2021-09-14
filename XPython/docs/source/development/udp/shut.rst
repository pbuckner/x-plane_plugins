SHUT/QUIT - Quit X-Plane
++++++++++++++++++++++++

:Send:

   Shut down X-Plane. ``SHUT`` and ``QUIT`` are identical commands::

     msg = struct.pack('<4sx', b'SHUT')
     sock.sendto(msg, (beacon['ip'], beacon['port']))

.. toctree::
   :maxdepth: 1
