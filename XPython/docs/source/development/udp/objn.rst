OBJN/OBJL - Load and Place Object
+++++++++++++++++++++++++++++++++

:Send:
   Load an object into an object index -- you provide the integer index number and
   a relative path to an OBJ file. For example, Lady Liberty from
   ``Custom Scenery/X-Plane Landmarks - New York/objects/Lady_Liberty.obj``::

     path = 'Custom Scenery/X-Plane Landmarks - New York/objects/Lady_Liberty.obj')
     index = 1
     msg = struct.pack('<4sxi500s', b'OBJN',
                       index,
                       path.encode('utf-8'))  # remember to encode string as bytes
     self.sock.sendto(msg, (beacon['ip'], beacon['port']))
   
   Nothing will be displayed until you issue ``OBJL``.

To display a loaded object, use ``OBJL`` and provide location information.
   
:Send:
   ::

      msg = struct.pack('<4sxi4xdddfffif4x', b'OBJL',
                        index,            # as provided with OBJN above
                        lat, lon, ele,    # floats, elevation is meters (ignored if on_ground = 1)
                        psi, theta, phi,  # floats
                        on_ground,        # 1= set on ground, 0= use provided elevation
                        smoke_size)       # 0.0 is no smoke
      self.sock.sendto(msg, (beacon['ip'], beacon['port']))

.. Note::
      Placing on ground at latitude, longitude works, but setting elevation, or rotational values
      results in unexpected movement of the object. It's broken in 11.55: Bug filed with Laminar
      Research Sept-17-2021.
      
.. toctree::
   :maxdepth: 1
