Common Installation Errors
--------------------------

On All Platforms
================

.. code-block:: none

   Can't open <folder> to scan for plugins.

**Cause**:
    XPPython3 looks for its main plugin folder PythonPlugins (See :doc:`installation_plugin`), and if not
    found, it will complain. Additionally, XPPython3 will look for aircraft-specific plugins specific
    to the loaded user aircraft (:doc:`/development/plugins`).

**Solution**:
    If you were expecting python plugin to be loaded, check to make sure the file(s) are place in the
    correct location. If you're not anticipating a plugin to be loaded, ignore the message.
  
----

On Mac
======


You get popup on initial execution:

 .. image:: /images/quarantine.png

**Cause**
   The XPPython3 plugin developer (me) cannot be verified, because I've not paid Apple $100 / year to get a signature [#F1]_. So
   Apple decides that since it doesn't know who I am, it cannot check the contents of the plugin, and will therefore
   not allow it to run without additional action on your part:

**Solution**
   1. Press **Cancel**. If you move it to the trash, you'll have to download it again...
      

   2. Follow instructions to remove the quarantine as described in :doc:`mac_quarantine`.

----        


On Linux
========

    Some distros, apparently, don't contain one or more of the "other" libraries we require. For example Arch distro (for
    one user) didn't include ``libbsd`` shared library. If you *don't see anything* in your Log.txt file post installation
    of XPPython3, check the linked shared objects and install any that are missing::

      $ ldd XPPython3.xpl
          linux-vdso.so.1 (0x00007ffe8d3d)
          libbsd.so.0 => /lib/x86_64-linux-gnu/libbsd.so.0 (0x00007ffed38d)
          libm.so.6 => /lib/x86_64-linux-gnu/libm.so.6 (0x00007fed3f8d)
          libpython3.12.so.1.0 => /lib/x86_64-linux-gnu/libpython3.12.so.1.0 (0x00007fed3f8d)
          libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007fed3f8d)
          libmd.so.0 => /lib/x86_64-linux-gnu/libmd.so.0 (0x00007fed3f8d)
          /lib64/ld-linux-x86-64.so.2 (0x00007fed3f8d)
          libz.so.1 => /lib/x86_64-linux-gnu/libz.so.1 (0x00007fed3f8d)
          libexpat.so.1 => /lib/x86_64-linux-gnu/libexpat.so.1 (0x00007fed3f8d)
      


----

.. [#F1] "*Apple signature costs $100/year*". I'm in favor of the quarantine system, but not
         crazy about having to pay to get a signature as I make no money on XPPython3.
         If you'd like to donate, I promise to
         use the first $100 of XPPython3 donations each year to purchase a signature so
         that others may avoid this hassle. You can donate through
         `Paypal <https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=TA3EJ9VWFCH3N&source=url>`_.
         

