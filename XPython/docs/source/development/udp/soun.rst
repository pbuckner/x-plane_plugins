SOUN - Play a Sound
+++++++++++++++++++

Play a WAV file. X-Plane includes many sound files under ``<XP>/Resources/sounds``.

:Send:
   Use the *relative* path to the sound file (e.g., ``Resources/sounds/alert/fire_bell.wav``)::

     path = 'Resources/sounds/alert/fire_bell.wav'
     msg = struct.pack('<4sxff500s', b'SOUN',
                       freq,                  # frequency, 1.0 == 100%
                       vol,                   # volume, 1.0 == 100%
                       path.encode('utf-8'))  # remember to encode string as bytes
     sock.sendto(msg, (beacon['ip'], beacon['port']))

This plays the sound once. See :doc:`LSND <lsnd>` to loop a sound indefinitely.

``freq`` and ``vol`` play the sound relative to how it was recorded: 1.0 means play at
100% of original speed, and original volume.
   
.. toctree::
   :maxdepth: 1
