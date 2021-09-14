LSND/SSND - Loop a Sound
++++++++++++++++++++++++

Loop a WAV file, playing until you send the equivalent ``SSND``.
X-Plane includes many sound files under ``<XP>/Resources/sounds``.

:Send:
   Use the *relative* path to the sound file (e.g., ``Resources/sounds/alert/fire_bell.wav``)::

     path = 'Resources/sounds/alert/fire_bell.wav'
     msg = struct.pack('<4sxiff500s', b'LSND',
                       index,                 # 0-4
                       freq,                  # frequency, 1.0 == 100%
                       vol,                   # volume, 1.0 == 100%
                       path.encode('utf-8'))  # remember to encode string as bytes
     sock.sendto(msg, (beacon['ip'], beacon['port']))

This loops the given sound. See :doc:`SOUN <soun>` to play a sound once.
``freq`` and ``vol`` play the sound relative to how it was recorded: 1.0 means play at
100% of original speed, and original volume.

You can enable up to 5 loops to play at the same time, by assigning different values
for ``index``, and sending five UDP commands.

To stop looping, send the same command with ``SSND``, and a matching index number.
   
.. toctree::
   :maxdepth: 1
