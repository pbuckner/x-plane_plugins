RREF - Get Datarefs
+++++++++++++++++++

To get a dataref, you'll create a simple structure consisting of the dataref string, the frequency you want
to receive it and an index number. This structure is your request, sent to X-Plane. In a way, you're *subscribing*
to the dataref, and you'll continuously receive its value, at the frequency you requested,
until you *unsubscribe* from it.

:Send:

   The dataref is a bytestring.::

     dataref = 'sim/aircraft/engines/acf_num_engines'
     msg = struct.pack("<4sxii400s", b'RREF',
                       freq,                     # Send data # times/second
                       index,                    # include this index number with results
                       dataref.encode('utf-8'))  # remember to encode as bytestring
     sock.sendto(msg, (beacon['ip'], beacon['port']))

   To stop subscription, request with ``freq=0``

----

:Receive:
   UDP packets will have header ``RREF\x00`` and include the index you passed during
   the request (that's how you'll know which dataref this is) and a single
   floating point value *regardless of which dataref you're requesting*.::

     (header,         # 'RREF'
      idx,            # integer, matching what you sent
      value           # single floating point value
      ) = struct.unpack('<4sxif', data)

   Note the received packet may contain multiple <index><value> pairs, see example below.
   
Assuming your request is well-formed, X-Plane will respond (eventually) with a structure consisting of the
index (matching the one you provided) and the current dataref value. "Eventually" because this
is all asynchronous.

To stop receiving the dataref, you'll need to form another request, identical to the first with the exception
of setting frequency to zero.

Example
-------

For example to retrieve the integer number of engines for the user aircraft, we'll use the following dataref.
(X-Plane datarefs are listed in ``<XP>/Resources/plugins/DataRefs.txt``. Other plugins and third-party
aircraft & scenery may define additional datarefs.)

    +-------------------------------------+------+----------+
    | Dataref                             | Type | Writable |
    +=====================================+======+==========+
    | sim/aircraft/engine/acf_num_engines | int  | y        |
    +-------------------------------------+------+----------+

The code might look like::

  # 1) Subscribe to receive once per second
  cmd = b'RREF'  # "Request DataRef(s)"
  freq = 1       # number of times per second (integer)
  index = 0      # "my" number, so I can match responsed with my request
  msg = struct.pack("<4sxii400s", cmd, freq, index, b'sim/aircraft/engines/acf_num_engines')
  sock.sendto(msg, (beacon['ip'], beacon['port']))
  
  # 2) Block, waiting to receive a packet
  data, addr = sock.recvfrom(2048)
  header = data[0:5]
  if header != b'RREF\x00':
      raise ValueError("Unknown packet")

  # 3) Unpack the data:
  idx, value = struct.unpack("<if", data[5:13])
  assert idx == index
  print("Number of engines is {}".format(int(value)))

  # 4) Unsubscribe -- as otherwise we'll continue to get this data, once every second!
  freq = 0
  msg = struct.pack("<4sxii400s", cmd, freq, index, b'sim/aircraft/engines/acf_num_engines')
  sock.sendto(msg, (beacon['ip'], beacon['port']))

You'll always get a float
-------------------------
Unpacking the data is always this same. The returned data is always 8 bytes: a 4-byte integer (your index)
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

Arrays are handled one-element-at-a-time!
-----------------------------------------
Second, and this is true for *all* array types (i.e., ``int[]``, ``byte[]``, ``float[]``), you'll
need to actually subscribe to *each* element in the array. Yes, that's crazy, but that's how
the X-Plane UDP interface works.

So, for ``acf_ICAO``, you'll subscribe to 40 datarefs, using the following dataref names::

   sim/aircraft/view/acf_ICAO[0]
   sim/aircraft/view/acf_ICAO[1]
   sim/aircraft/view/acf_ICAO[2]
   ...
   sim/aircraft/view/acf_ICAO[39]

You'll have a different index for each, and you'll receive UDP packets with **a single float value for each array element**.

Multiple results in same packet
-------------------------------

For performance reasons, X-Plane may send multiple dataref results in a single UDP packet, so your code needs
to be prepared for this::

  data, addr = sock.recvfrom(2048)
  values = data[5:]                  # skipping over 'RREF\x00' header, get _all_ values
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

Using threads
-------------

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


.. toctree::
   :maxdepth: 1
