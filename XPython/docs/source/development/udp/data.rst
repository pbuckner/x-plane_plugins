DATA - Input to Data Output
+++++++++++++++++++++++++++

DATA Request
------------

X-Plane Settings "Data Output" screen lists a hundred or so data sets which can be output to the Cockpit, saved to file, or sent via UDP.
The :doc:`dsel` command allows you to effectively request one (or more) of these data sets sent *to the configured port*. The response
to ``DSEL`` is ``DATA``. (You should be familiar with the ``DSEL`` command before trying to use this ``DATA`` command.)

The ``DATA`` command *can also be sent* to X-Plane.

:Send:

  The command looks just like the ``DATA`` response to ``DSEL``. It consists of the index, followed by eight
  data values (all floats)::
  
   msg = struct.pack('<4sxfffffffff'.format(b'DATA', index,
                                            col0, col1, col2, col3,
                                            col4, col5, col6, col7)

The index is a float, but represents the (integer) index of the "data output" you want to change. For example,
index #8 is for "Joystick aileron/elevator/rudder". Select that item in settings, and you'll see the following
on the screen.

.. image:: /images/udp_joy.png

The ``DATA`` command *sent* might look like::

   msg = struct.pack('<4sxfffffffff'.format(b'DATA', float(8),
                                            -.1115, 0.0, -999.0, -999.0
                                            -999.0, -999.0, -999.0, -999.0)

Which would (attempt to) set the elevator to -.1115, and the aileron to 0.0. The -999.0 values
indicate no input (that is, don't attempt to change the X-Plane value). You can stream these
UDP commands to drive X-Plane using external devices.

While there is no direct response to this command, if you have set ``DSEL`` you will receive
the ``DATA`` response (whereever you have set up the ``DSEL`` response.

Note that *many* values on the Data Output page cannot be set directly, as they're derived from
internal data. Attempts to set them will result in a no-op.
You cannot, for example, set the GPU busy time (gpu) to 0.00001 or frame rate (f-act) to 120.0.
If only...
   
.. toctree::
   :maxdepth: 1
