ALRT - Display Alert Message
++++++++++++++++++++++++++++

:Send:

   You'll post four lines of text which will result in a popup. The title ("Be Advised") and button ("Understood") cannot
   be changed.::

     line1 = "Hello!"
     line2 = "Something really surprising happened, and I wanted to send it \
              to you via UDP so you can see it. Really long lines wrap \
              automatically."
     line3 = "Note:\nAn embedded newline results in a line break in the output."
     line4 = "There is a paragraph break between lines."
     msg = struct.pack('<4sx240s240s240s240s', b'ALRT',
                       line1.encode('utf-8'),
                       line2.encode('utf-8'),
                       line3.encode('utf-8'),
                       line4.encode('utf-8'))
     sock.sendto(msg, (beacon['ip'], beacon['port']))

.. image:: /images/alert.png

The popup is a fixed width, long lines are wrapped automatically. If the height of the text exceeds the height of
the popup, the popup will automatically include a scrollbar.


.. toctree::
   :maxdepth: 1
