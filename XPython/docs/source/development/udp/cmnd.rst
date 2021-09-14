CMND - Execute Command
++++++++++++++++++++++
X-Plane Commands are simply those listed in ``<XP>/Resources/plugins/Commands.txt`` (and any defined by plugins or
aircraft.)

:Send:
   Send the command as a string::

     dref = "sim/map/show_current"
     msg = struct.pack('<4sx500s', b'CMND',
                       dref.encode('utf-8'))
     sock.sendto(msg, (beacon['ip'], beacon['port']))
   
For example, there is a command ``sim/map/show_current`` which will toggle the display of the map window.

To execute this command, you're going to use a socket::

  sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
  sock.settimeout(3.0)
  
  msg = 'CMND\x00'                                  # Sending a command (with terminating null byte)
  msg += 'sim/map/show_current'                     # the command to send
  msg = msg.encode('utf-8')                         # convert from unicode to utf-8 encoded string
  sock.sendto(msg, (beacon['ip'], beacon['port']))  # send to (addr, port) as provided by the beacon

Note you don't get a return value from X-Plane: there is no "success" or "failure". You will get a return
count of the number of bytes sent by ``sock.sendto()``, but you'll get that even if you completely
mess up the command.

Because the result of encoding a (unicode) string is a bytes object, we could just have easily done::

  msg = b'CMND\x00sim/map/show_current'
  sock.sendto(msg, (beacon['ip'], beacon['port']))

or::

  cmd = b'CMND'
  dref = b'sim/map/show_current'
  msg = struct.pack('<4sx500s', cmd, dref)
  sock.sendto(msg, (beacon['ip'], beacon['port']))

.. toctree::
   :maxdepth: 1
