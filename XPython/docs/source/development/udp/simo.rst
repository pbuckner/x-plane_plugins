SIMO - Load or Save a Situation or Movie
++++++++++++++++++++++++++++++++++++++++

  
:Send:
  The SIMO command takes an enumerated command and a path::

    msg = struct.pack('<4sxi150s2x', b'SIMO',
                      cmd,                   # 0=Save sit, 1=Load sit, 2=Save Movie, 3=Load Movie
                      path.encode('utf-8'))  # Relative path, e.g., 'Output/my_movie'
    sock.sendto(msg, (beacon['ip'], beacon['port']))

Note that "movie" is different from the avi file created as part of "Toggle Video Recording". The movie file
is X-Plane proprietary format. When you load a movie, you'll enter "Toggle Replay Mode" with the movie paused.

.. toctree::
   :maxdepth: 1
