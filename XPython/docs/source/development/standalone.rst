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

The more difficult part is working this ``sent_to()`` and ``recvfrom()`` into a working program
so you don't get confused while interleaving sends and receives.

UDP and X-Plane
===============
X-Plane understands a particular format, described in
**<XP>/Instructions/X-Plane SPECS from Austin/Exchanging Data with X-Plane.rtfd**.
However, *that document is not accurate*. I suppose because it's just not been updated in a while, but
it no longer matches with X-Plane 11.55+ does.

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

Basically what you'll do is "find" X-Plane -- the host IP address and port (See *Connection with X-Plane*, next),
and then you'll send one or more command packets to it & wait for data packets.

Connecting with X-Plane
+++++++++++++++++++++++

X-Plane automatically broadcasts a "beacon" on the network, allowing other programs
to find it. Ideally, you should listen for this beacon to tell your standalone program
a) X-Plane is running; and, b) where it is located.

Multicast beacons are pretty standard, the only "custom" aspects are which port the
multicast is on (49707 for X-Plane) and the data contents of the beacon itself.

See example code in `find_xp() <../_static/find_xp.py>`_ which will wait for X-Plane to startup and will then
return information about the version of X-Plane found.

You can use it like::

  beacon = find_xp()
  port = beacon['port']
  ip = beacon['ip']

By default, X-Plane is set for UDP networking. You do not have to enable anything under X-Plane Settings->Network.
This includes External Visuals, External Apps, or UDP Ports.

Note your socket related to the beacon is different from the socket you use to send and receive information
from X-Plane. One you've *found* X-Plane, you can close the beacon socket.

Execute Command
+++++++++++++++
X-Plane Commands are simply those listed in <XP>/Resources/plugins/Commands.txt (and any defined by plugins or
aircraft.)

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

Get Datarefs
++++++++++++
To get a dataref, you'll create a simple structure consisting of the dataref string, the frequency you want
to receive it and an index number. This structure is your request, sent to X-Plane. In a way, you're *subscribing*
to the dataref, and you'll continuously receive its value, at the frequency you requested,
until you *unsubscribe* from it.

Assuming your request is well-formed, X-Plane will respond (eventually) with a structure consisting of the
index (matching the one you provided) and the current dataref value. I say "eventually" because this
is all asynchronous.

To stop receiving the dataref, you'll need to form another request, identical to the first with the exception
of setting frequency to zero.

For example to retrieve the integer number of engines for the user aircraft, we'll use the following dataref.

    +-------------------------------------+------+----------+
    | Dataref                             | Type | Writable |
    +-------------------------------------+------+----------+
    | sim/aircraft/engine/acf_num_engines | int  | y        |
    +-------------------------------------+------+----------+

The code might look like::

  # Subscribe to receive once per second
  cmd = b'RREF'  # "Request DataRef(s)"
  freq = 1       # number of times per second (integer)
  index = 0      # "my" number, so I can match responsed with my request
  msg = struct.pack("<4sxii400s", cmd, freq, index, b'sim/aircraft/engines/acf_num_engines')
  sock.sendto(msg, (beacon['ip'], beacon['port']))
  
  # Block, waiting to receive a packet
  data, addr = sock.recvfrom(2048)
  header = data[0:5]
  if header != b'RREF,':
      raise ValueError("Unknown packet")
  # Unpack the data:
  idx, value = struct.unpack("<if", data[5:13])
  assert idx == index
  print("Number of engines is {}".format(int(value)))

  # Unsubscribe -- as otherwise we'll continue to get this data, once every second!
  freq = 0
  msg = struct.pack("<4sxii400s", cmd, freq, index, b'sim/aircraft/engines/acf_num_engines')
  sock.sendto(msg, (beacon['ip'], beacon['port']))

Unpacking the data is always this same. The returned data is always 8 bytes, a 4-byte integer (your index)
and a 4-byte float (the dataref value). Your code needs to evaluate the resulting float based on
the datatype. In the above example, 'acf_num_engines' is an 'int' type, so we cast the returned floating
point value to an integer.

If you're unpacking something which is string, unpacking is a bit more complicated.

    +-------------------------------------+----------+----------+--------------------------------+
    | Dataref                             | Type     | Writable | Comment                        |
    +-------------------------------------+----------+----------+--------------------------------+
    | sim/aircraft/view/acf_ICAO          | byte[40] | y        | string ICAO code for aircraft  |
    |                                     |          |          | (a string) entered by author   |
    +-------------------------------------+----------+----------+--------------------------------+

First, remember that you're getting a float in return, so the value you get will need to be cast to a character::

  # value == 65.0, result == 'A'
  result = chr(int(value))  # covert float to integer, and then convert it to a character.

Second, and this is true for *all* array types (i.e., int[], byte[], float[]), you'll
need to actually subscribe to *each* element in the array. Yes, that's crazy, but that's how
the X-Plane UDP interface works.

So, for ``acf_ICAO``, you'll subscribe to 40 datarefs, using the following dataref names::

   sim/aircraft/view/acf_ICAO[0]
   sim/aircraft/view/acf_ICAO[1]
   sim/aircraft/view/acf_ICAO[2]
   ...
   sim/aircraft/view/acf_ICAO[39]

You'll have a different index for each, and you'll receive UDP packets with **a single value for each array element**.

For performance reasons, X-Plane may send multiple dataref results in a single UDP packet, so your code needs
to be prepared for this::

  data, addr = sock.recvfrom(2048)
  values = data[5:]                  # skipping over 'RREF,' header, get _all_ values
  num_values = int(len(values) / 8)  # Each dataref is 8 bytes long (index + value)
  for i in range(num_values):
      dref_info = data[(5 + 8 * i):(5 + 8 * (i + 1))]  # extract the 8 byte segment
      (index, value) = struct.unpack("<if", dref_info)
      ...

For ``acf_ICAO`` example, you may get a single UDB packet which is analagous to  ::

    RREF,      # the 5-byte header      -- data[0:5]
    (0, 67.0)  # 8 bytes, index + value -- data[5:13] 
    (1, 49.0)  # 8 bytes, index + value -- data[13:21] 
    (2, 55.0)  # 8 bytes, index + value -- data[21:29] 
    (3, 50.0)  # 8 bytes, index + value -- data[29:37] 
    (4,  0.0)  # 8 bytes, index + value -- data[37:45] 
    (5,  0,0)  # 8 bytes, index + value -- data[45:53] 
    ...

Based on your subscription, you'll know the indices map to::

   0 -> sim/aircraft/view/acf_ICAO[0]
   1 -> sim/aircraft/view/acf_ICAO[1]
   2 -> sim/aircraft/view/acf_ICAO[2]
   3 -> sim/aircraft/view/acf_ICAO[3]
   4 -> sim/aircraft/view/acf_ICAO[4]
   5 -> sim/aircraft/view/acf_ICAO[5]
   ...

And you'll know it is supposed to be a string, so you'll convert the floating point values to characters to yield::

    (0,  'C')
    (1,  '1')
    (2,  '7')
    (3,  '2')
    (4, '\0')
    (5, '\0')
    ...

And of course, you're not guaranteed to get them in the order you'd like, so be prepared for that.

Because reception is asynchronous and order is not guaranteed, you'll likely need to create a separate thread
to receive on, and use an internal datastructure to hold index numbers with their most recently received values.
perhaps something like::

  t = treading.thread(target=get_drefs, args=(my_mapping, ))
  t.start()

  def get_drefs(my_mapping):
      while Not_Exit:
          data, arry = Sock.recvfrom(2048)
          values = data[5:]
          num_values = int(len(values) / 8)
          for i in range(num_values):
              dref_info = data[(5 + 8 * i):(5 + 8 * (i + 1))]
              (index, value) = struct.unpack("<if", dref_info)
              my_mapping[index]['value'] = value


Set Datarefs
++++++++++++
Setting a dataref is trivial::

  cmd = b'DREF'
  msg = struct.pack('<4sxf500s', cmd, value, dataref.encode('utf-8'))
  sock.sendto(msg, (beacon['ip'], beacon['port']))

Other than the count of bytes sent (==509) you'll not get any acknowledgement or return from X-Plane.

Note that ``value`` is **always** a float: You'll need to cast integer values to float before packing. You'll
also need to convert string characters to floats (e.g., ``float(ord('C'))``) if that's what you're trying to
set.

Similar to Get Datarefs, arrays are sent *one element at a time*, ``sim/aircraft/view/acf_ICAO[0]``, ``sim/aircraft/view/acf_ICAO[1]``, etc.

.. Note::
    X-Plane 11.55 (at least) does not support updating ``byte[x]`` type of datarefs. You can
    send the UDP, and X-Plane will receive it correctly but does not actually make a change.
    A bug has been filed with Laminar Research: as XPD-11353.
  

Stream Data Output
++++++++++++++++++

 (like in-cockpit display -- to be provided)


X-Plane UDP Reference
=====================
In addition to these standard X-Plane commands, there are additional UDP commands supported by this interface:


Receive Aircraft Position - RPOS
++++++++++++++++++++++++++++++++

:Send:

 Single packet with ``RPOS`` command and number *as a string*, representing
 number of times per second you'd like X-Plane to send you a packet with
 current aircraft position.::
 
   msg = struct.pack('<4sx10s', b'RPOS', b'60')  # requesting 60 times /second

 You can use this to drive displays, moving maps, etc.

 To stop, set request to b'0' times per second::

   msg = struct.pack('<4sx10s', b'RPOS', b'0')  # requesting 0 times /second

----

:Receive:    

 Single packet with position data::

     data = struct.unpack"<5sdddffffffffff", packet)

     cmd == 'RPOS4'
     double dat_lon		longitude of the aircraft in X-Plane of course, in degrees
     double dat_lat		latitude
     double dat_ele		elevation above sea level in meters
     float y_agl_mtr		elevation above the terrain in meters
     float veh_the_loc	pitch, degrees
     float veh_psi_loc	true heading, in degrees
     float veh_phi_loc	roll, in degrees
     float vx_wrl		speed in the x, EAST, direction, in meters per second
     float vy_wrl		speed in the y, UP, direction, in meters per second
     float vz_wrl		speed in the z, SOUTH, direction, in meters per second
     float Prad			roll rate in radians per second
     float Qrad			pitch rate in radians per second
     float Rrad			yah rate in radians per second
    
.. Note::

    X-Plane 11.55 (at least) documentation indicates the first five bytes are ``RPOS\x00``.
    In reality, ``RPOS4`` is sent (without a null).
    A bug has been filed with Laminar Research: 2021-Sep-1.

Weather Radar - RADR
++++++++++++++++++++

:Send:

   Single packet with ``RADR`` command and number of radar points per frame
   you'd like X-Plane to send you.::

     msg = struct.pack('<4sx10s', b'RADR', b'10')  # requesting 10 radar points per frame

   To stop, set request to b'0' times per frame.::

     msg = struct.pack('<4sx10s', b'RADR', b'0')  # requesting 0 radar points per frame

----

:Receive:

   Single packet with radar data::

     data = struct.unpack("<5sffBf", packet)

     cmd == 'RADR5'
     float lon					longitude of the radar point
     float lat					latitude of course
     unsigned char storm_level_0_100	precip level, 0 to 100
     float storm_height_meters	the storm tops in meters above sea level

.. Note::

    X-Plane 11.55 (at least) documentation indicates the first five bytes are ``RADR\x00``.
    In reality, ``RADR5`` is sent (without a null).
    Additionally, documentation indicates 4-byte ``float storm_level_0_100`` whereas it is
    actually a single unsigned char.
    
    A bug has been filed with Laminar Research: 2021-Sep-2.

Forward Looking Infrared Images - FLIR
++++++++++++++++++++++++++++++++++++++
You can get image capture from X-Plane over UDP. Only black-and-white images are sent to save bandwidth and simulate
FLIR.

:Send:
   Single packet with ``FLIR`` command and number of frames per second to receive::

     msg = struct.pack('<4sx10s', b'FLIR', b'10')  # requesting 10 images per second

   To stop, set request to b'0' times per second.::

     msg = struct.pack('<4sx10s', b'FLIR', b'0')  # requesting 0 frames per second

----

:Receive:

  (See "Getting X-Plane real-time FLIR.rtf" for how to extract the FLIR images you'll receive.)

.. Note::

   X-Plane 11.55 (at least) **does not send FLIR data**. This appears to be completely broken.

   A bug has been filed with Laminar Research: 2021-Sep-3.

======== other UDP messages to x-plane ========

WORK IN PROGRESS

Drive X-Planes Visuals - VEHX
+++++++++++++++++++++++++++++
::

 struct.pack('<4sxidddfff', b'VEHX', p, dat_lat, dat_lon, dat_ele, veh_psi_true, veh_the, veh_phi)
 int p					The index of the airplane you want to control. use 0 for the main airplane that you fly to drive the visuals.
 double dat_lat			latitude, in degrees
 double dat_lon			longitude, in degrees
 double dat_ele			elevation above sea level, in meters
 float veh_psi_true		heading, degrees true
 float veh_the			pitch, degrees
 float veh_phi			roll, degrees





