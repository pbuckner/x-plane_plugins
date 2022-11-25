Standalone Python with UDP
--------------------------

In addition to plugins, X-Plane supports external communications via UDP. This
allows any number of separate processes (potentially on separate computers) to
get and set data remotely and execute some commands.

This is *completely* unrelated to XPPython3, but as you're programming in python
I figured I might as well introduce you to this method of interaction as well.

Intro to UDP
============
UDP simply sends one packet of data -- like throwing a rock -- it might make it to the receiver,
it might not (as opposed to TCP, which is more like a pipe.) Packets are not guaranteed to
arrive in order, nor are they even guaranteed to arrive. But they're quick, with low overhead.

The size of the packet is limited by the protocol -- about 65k bytes. The content of
the packet can be anything, but the sender and receiver have to agree.

UDP and Python
==============
UDP is pretty simple: you'll create a UDP (i.e., DATAGRAM) socket, and then use that socket
to send information to a remote port::

  sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
  msg = makeMsg()
  sock.send_to(msg, (X_PLANE_IP, UDP_PORT))

Receiving information is similarly simple::

  data, addr = sock.recvfrom(2048)
  handleData(data)

The more difficult part is working this ``send_to()`` and ``recvfrom()`` into a working program
so you don't get confused while interleaving sends and receives. (You can put the receiver in a
python thread. See example in :doc:`rref`.)

UDP and X-Plane
===============
X-Plane understands a particular format, described in
**Exchanging Data with X-Plane.rtfd** under **<XP>/Instructions**.
However, *that document is not accurate*. I suppose because it's just not been updated in a while, but
it no longer matches with what X-Plane 11.55 or 12 does.

X-Plane's interface is (usually) described using C language structures and expects a particular size
and encoding of data. You'll have to format ('pack')
the python data into the proper structure before sending it to X-Plane, and unpack any received data in a similar manner.

The easiest way to pack and unpack is to use the python ``struct`` module. For example::

    cmd = b'RREF'
    freq = 1
    index = 0
    msg = struct.pack("<4sxii400s", cmd, freq, index, b'sim/aircraft/engines/acf_num_engines')

The initial string ``<4sxii400s`` describes how to pack the remaining arguments:

  +----------+-------------------------------------------------------------+
  | ``>``    | little endian (i.e., least significant byte in the lowest   |
  |          | memory position)                                            |
  +----------+-------------------------------------------------------------+
  | ``4s``   | a 4-byte object, commonly string, e.g., 'RREF', expressed   |
  |          | as a bytes: b'RREF'.                                        |
  +----------+-------------------------------------------------------------+
  |  ``x``   | a null byte, or 0x00. X-Plane is looking for a              |
  |          | null-terminated 4-character string & this encodes the       |
  |          | null value. (does not consume an argument). Yes, you could  |
  |          | use ``5s``, without the ``x`` for the same effect, but this |
  |          | method emphasizes the usable command is a 4-character value |
  +----------+-------------------------------------------------------------+
  |  ``i``   | a 4-byte integer                                            |
  +----------+-------------------------------------------------------------+
  |  ``i``   | another 4-byte integer (you could also combine these as     |
  |          | ``2i``, which consumes two integer arguments)               |
  +----------+-------------------------------------------------------------+
  | ``400s`` | a 400-byte object. Note that Python pads and zero-fills to  |
  |          | fit 400 bytes.                                              |
  +----------+-------------------------------------------------------------+

Basically what you'll do is "find" X-Plane -- the host IP address and port (See *Connecting with X-Plane*, next),
and then you'll send one or more command packets to it & wait for data packets.

Connecting with X-Plane
+++++++++++++++++++++++

X-Plane automatically broadcasts a "beacon" on the network, allowing other programs
to find it. Ideally, you should listen for this beacon to tell your standalone program
a) X-Plane is running; and, b) where it is located.

Multicast beacons are pretty standard, the only "custom" aspects are which port the
multicast is on (49707 for X-Plane) and the data contents of the beacon itself.

See example code in `find_xp() <https://xppython3.readthedocs.io/en/stable/_static/find_xp.py>`_
which will wait for X-Plane to startup and will then
return information about the version of X-Plane found.

You can use it like::

  beacon = find_xp()
  port = beacon['port']
  ip = beacon['ip']

By default, X-Plane 11 is set for UDP networking. You do not have to enable anything under X-Plane Settings->Network.
This includes External Visuals, External Apps, or UDP Ports.

.. Note::
   XP 12 appears to disable UDP networking initially. Check Settings->Network page, and make sure
   "Accept incoming connections" is enabled.

Note your socket related to the beacon is different from the socket you use to send and receive information
from X-Plane. One you've *found* X-Plane, you can close the beacon socket.

For initial debugging, it may be helpful to have X-Plane log networking data to Log.txt: that way you can
see verify it is receiving what you think you're sending. This can be enabled on Settings->General, select "Output network data to Log.txt"
option under the Data section. The problem with this option is that it also logs all Beacon posts, so there
will be lots of log entries.

X-Plane UDP Summary
===================

*SEND*

  +-----------------------------+--------------------------------------------+
  |           Cmd - Description | Structure                                  |
  +=============================+============================================+
  |           :doc:`acfn`       | ACFN <index><path><livery>                 |
  +-----------------------------+--------------------------------------------+
  |           :doc:`acpr`       | ACPR <acfn><prel>                          |
  +-----------------------------+--------------------------------------------+
  |           :doc:`alrt`       | ALRT <line1><line2<line3><line4>           |
  +-----------------------------+--------------------------------------------+
  |           :doc:`cmnd`       | CMND <command>                             |
  +-----------------------------+--------------------------------------------+
  |           :doc:`dref`       | DREF <value><dataref>                      |
  +-----------------------------+--------------------------------------------+
  |           :doc:`dsel`       | DSEL <index><index>...                     |
  |                             |                                            |
  |                             | USEL <index><index>...                     |
  +-----------------------------+--------------------------------------------+
  |           :doc:`fail`       | FAIL <index>                               |
  |                             |                                            |
  |                             | RECO <index>                               |
  +-----------------------------+--------------------------------------------+
  |           :doc:`flir`       | FLIR <frequency>                           |
  |                             |                                            |
  |                             | *(Deprecated since 11.41)*                 |
  +-----------------------------+--------------------------------------------+
  |           :doc:`ise4`       | ISE4 <cmd><ip><port><enable>               |
  |                             |                                            |
  |                             | ISE6 <cmd><ip><port><enable>               |
  +-----------------------------+--------------------------------------------+
  |           :doc:`lsnd`       | LSND<index><freq><vol><path>               |
  |                             |                                            |
  |                             | SSND<index><freq><vol><path>               |
  +-----------------------------+--------------------------------------------+
  |           :doc:`nfal`       | NFAL <navaid_id>                           |
  |                             |                                            |
  |                             | NREC <navaid_id>                           |
  +-----------------------------+--------------------------------------------+
  |           :doc:`objn`       | OBJN <index><path>                         |
  |                             |                                            |
  |                             | OBJL <index><lat><lon><ele><psi><theta>    |
  |                             |      <phi><on_ground><smoke_size>          |
  +-----------------------------+--------------------------------------------+
  |           :doc:`prel`       | PREL <type_start><index><apt_id>           |
  |                             |      <rwy_id><rwy_dir>                     |
  |                             |      <lat><lon><elev><psi><spd>            |
  +-----------------------------+--------------------------------------------+
  |           :doc:`radr`       | RADR <freq>                                |
  +-----------------------------+--------------------------------------------+
  |           :doc:`rese`       | RESE                                       |
  +-----------------------------+--------------------------------------------+
  |           :doc:`rpos`       | RPOS <freq>                                |
  +-----------------------------+--------------------------------------------+
  |           :doc:`rref`       | RREF <freq><index><dataref>                |
  +-----------------------------+--------------------------------------------+
  |           :doc:`simo`       | SIMO<type><path>                           |
  +-----------------------------+--------------------------------------------+
  |           :doc:`shut`       | SHUT                                       |
  |                             |                                            |
  |                             | QUIT                                       |
  +-----------------------------+--------------------------------------------+
  |           :doc:`soun`       | SOUN<freq><vol><path>                      |
  +-----------------------------+--------------------------------------------+
  |           :doc:`vehx`       | VEHX <plane><lat><log><elev>               |
  |                             |      <psi><theta><ph>                      |
  +-----------------------------+--------------------------------------------+

*RECEIVE*

  +-------+----------------------------+-----------------------------------------+
  | Cmd   | In response to             | Structure                               |
  +=======+============================+=========================================+
  | DATA* |                :doc:`dsel` | DATA*<index><val1><val2>...<val8>       |
  +-------+----------------------------+-----------------------------------------+
  | FLIR  |                :doc:`flir` | *(Deprecated since 11.41)*              |
  +-------+----------------------------+-----------------------------------------+
  | RADR5 |                :doc:`radr` | RADR5<lon><lat><level><height>          |
  +-------+----------------------------+-----------------------------------------+
  | RPOS4 |                :doc:`rpos` | RPOS4 <lon><lat><elev><agl><theta><psi> |
  |       |                            | <phi><vx><vy><vz><P><Q><R>              |
  +-------+----------------------------+-----------------------------------------+
  | RREF  |                :doc:`rref` | RREF <index><value><index><value>...    |
  +-------+----------------------------+-----------------------------------------+

.. toctree::
   :maxdepth: 1
   :caption: Details

   acfn
   acpr
   alrt
   cmnd
   dref
   dsel
   fail
   flir
   ise4
   lsnd
   nfal
   objn
   prel
   radr
   rese
   rpos
   rref
   simo
   shut
   soun
   vehx
