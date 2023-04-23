ISE4/ISE6 - Set Network Parameters
++++++++++++++++++++++++++++++++++++

This sets "Network Configuration" found on the Data Output page of X-Plane, and informs X-Plane
the destination IP and port for bulk data. (See :doc:`dsel` for bulk data info.)

:Send:

   You set IP and port, which may be different from where you're invoking the command. For
   UDP data, set the cmd=64.

   To stop sending data, set the final parameter to 0.::

     cmd = 64
     msg = struct.pack('<4sxi16s8si', b'ISE4',
                       cmd,                  # =64 for UDP data
                       ip.encode('utf-8'),   # e.g., b'192.168.1.5'
                       port.encode('utf-8'), # e.g., b'50987'... characters, not an integer!
                       use_ip)               # 0=disable, 1=enable
     sock.sendto(msg, (beacon['ip'], beacon['port']))

======== ===========================================
cmd      Purpose
======== ===========================================
 0 - 18  Multiplayer
19 - 38  External visuals
39       Master machine, this is an external
42       Master machine, this is an IOS
62       IOS, this is master machine
64       data output target
71       Xavion 1
72       Xavion 2
73       Xavion 3
74       Xavion 4
75       Foreflight, one IP address
76       Foreflight, broadcast
77       X-Plane control pad for IOS
======== ===========================================

:Send:

   ISE6 is similar::

     cmd = 64
     msg = struct.pack('<4sxi65s6sxi', b'ISE6',
                       cmd,                  # =64 for UDP data
                       ip.encode('utf-8'),   # e.g., b'fe80::8f6:826:b4e1:76e3'
                       port.encode('utf-8'), # e.g., b'50987'... characters, not an integer!
                       use_ip)               # 0=disable, 1=enable
     sock.sendto(msg, (beacon['ip'], beacon['port']))

Note that the port# of your socket can be found as ``sock.getsockname()[1]``
   
.. toctree::
   :maxdepth: 1
