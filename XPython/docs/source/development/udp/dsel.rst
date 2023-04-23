DSEL/USEL - Stream Data Output
++++++++++++++++++++++++++++++

DSEL Request
------------

X-Plane Settings "Data Output" screen lists a hundred or so data sets which can be output to the Cockpit, saved to file, or sent via UDP.
The DSEL command allows you to effectively request one (or more) of these data set *to the configured port*.

This might save you from manually navigating to Settings in-game and selecting the dataset and setting the networking details.

:Send:

   The "index" requested matches the index lists on the Data Output window, so::

     index = 1
     msg = struct.pack('<4sxi', b'DSEL',
                       index)               # requesting index==1, first item in the list
     sock.sendto(msg, (beacon['ip'], beacon['port']))

  
===== ====================================
Index Dataset
===== ====================================
0     Frame rate
1     Times
...    
138   Servo aileron/elevator/rudders
===== ====================================

You can request more than one index at a time within the same command, by packing more integers, e.g.,::

  items = [0, 1, 138]
  msg = struct.pack('<4sx{COUNT}i'.format(COUNT=len(items)), b'DSEL', *items)

To stop receiving data, use send ``USEL`` command with the same list of integers.

.. Note::
   DSEL merely indicates *what* data to send, not *where* to send it. It *will not* automatically send the requested
   UDP data to the same port making the request (unlike other commands listed here.)

   Therefore, you'll need to *also* set the Internet options using :doc:`ISE4 <ise4>` or :doc:`ISE6 <ise4>`.
   
DATA Response
-------------

The result of the ``DSEL`` command will be a simple data structure sent to the IP / Port set by :doc:`ISE4 <ise4>` or :doc:`ISE6 <ise4>`.
(Let me say that again -- the data is sent to the **set** [#how]_ IP / Port, *not* returned to the same port which made the ``DSEL`` request.)

:Receive:

   The received packet will start with ``DATA``, followed by 9 floats.::

     header, index, *vals = struct.unpack('<4xf8f', data)
     assert header == 'DATA'
  
   ``index`` is a float, but represents the (integer) index you requested with ``DSEL``. That way you know *which dataset* the
   data values correspond to.

The values will always be 8 floats (in the above example, we've unpacked them into a list ``vals``.
What are they? There is no way of determining programmatically. But, they
will match the 8 values (8 slots, really) displayed if you were to select "Show in Cockpit" for that dataset.

For example, index 0 is listed as "Frame rate". If you were to Show in Cockpit, you'd see

 .. image:: /images/data_output.png

 ======= ======= ======= ======= ======= ======= ======= =======
 f-ac    f-sim           frame   cpu     gpu     grnd    flit      
 vals[0] vals[1] vals[2] vals[3] vals[4] vals[5] vals[6] vals[7]
 /sec    /sec            time    time    time    ratio   ratio
 ======= ======= ======= ======= ======= ======= ======= =======


The eight values represent the above 8 slots (you'll note the third slot is blank, and the third data
item is '-999.0')
 
----

.. [#how]
   Of course, the set port might be the same port as your socket, but you'd have to configure it that way, either using
   :doc:`ISE4 <ise4>` or :doc:`ISE6 <ise4>` or simply setting the IP/Port on the X-Plane Settings->Data Output page:
   Select "Send network data output" and set the values for IP Address and Port to match your socket's address and port.

   >>> cmd = 64
   >>> ip = sock.getsockname()[0]
   >>> port = str(sock.getsockname()[1])
   >>> msg = struct.pack('<ISE4xi16s8si', cmd, ip.encode('utf-8'), port.encode('utf-8'), 1)
   >>> sock.sendto(msg, (beacon['ip'], beacon['port']))
   >>>

   
.. toctree::
   :maxdepth: 1
