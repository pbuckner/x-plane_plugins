FLIR - Forward Looking Infrared Images
++++++++++++++++++++++++++++++++++++++
You can get image capture from X-Plane over UDP. Only black-and-white images are sent to save bandwidth and simulate
FLIR.

:Send:
   Single packet with ``FLIR`` command and number of frames per second to receive::

     msg = struct.pack('<4sx10s', b'FLIR', b'10')  # requesting 10 images per second
     sock.sendto(msg, (beacon['ip'], beacon['port']))

   To stop, set request to b'0' times per second.::

     msg = struct.pack('<4sx10s', b'FLIR', b'0')  # requesting 0 frames per second
     sock.sendto(msg, (beacon['ip'], beacon['port']))

----

:Receive:

  (See "Getting X-Plane real-time FLIR.rtf" for how to extract the FLIR images you'll receive.)

  .. Note::

     X-Plane 11.55 (at least) **does not send FLIR data**. This appears to be completely broken.

     A bug has been filed with Laminar Research: 2021-Sep-3.

.. toctree::
   :maxdepth: 1
