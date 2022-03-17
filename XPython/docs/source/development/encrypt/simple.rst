Basic Encryption Example
------------------------

For example, assume you have an algorithm you want to protect::

  from collections import deque
  def munge(input):
      d = deque(input)
      d.reverse()
      return ''.join(d)

Copy and paste the above into a file ``PythonPlugins/myplugin/compute.py``.

Create the rest of your plugin in the usual way, copy and paste this into ``PythonPlugins/PI_MyPlugin.py``::

  from XPPython3 import xp
  from .myplugin import compute
  class PythonInterface:
      def XPluginStart(self):
          return "MyPlugin", "MyPlugin", "MyPlugin"

      def XPluginEnable(self):
          self.flID = xp.createFlightLoop(self.callback, refCon='maps')
          xp.scheduleFlightLoop(self.flID, 1)
          return 1

      def XPluginDisable(self):
          xp.destroyFlightLoop(self.flID)

      @staticmethod
      def callback(lastCall, elapsedTime, counter, refCon):
          output = compute.munge(refCon)
          xp.speakString(f"Munge of f{refCon} is f{output}")
          return 0
          

Before continuing, start X-Plane and make sure your plugin works.

.. image:: /images/xpyce-maps.png

You have a Plugin, ``PI_MyPlugin.py``, which will import a local file ``.myplugin.compute``.
Your file structure would look like:

::

  <X-Plane>/
  └─── Resources/
       └─── plugins/
            ├─── XPPython3/
            │    ├─── xpyce_compile.py
            │    └─── ....
            └─── PythonPlugins/
                 ├─── PI_MyPlugin.py
                 └─── myplugin/
                      └─── compute.py

Step 1. Encrypt your code
*************************

Use the provided script to encrypt the file you want to protect, in this case ``compute.py``. You'll
need to use the same version of python as will be executed by XPPython3.

.. code-block:: console

     $ python3.10 ../../XPPython3/xpyce_compile.py compute.py 
     compute.cpython-310.xpyce: 3WrDPhFzJ_XCddKBi_Omzld9IfXrIFkMo7beVBP5L0o=

The script returns the name of the compiled file and an encryption key.

And now your file structure looks like::

  <X-Plane>/
  └─── Resources/
       └─── plugins/
            ├─── XPPython3/
            │    ├─── xpyce_compile.py
            │    └─── ....
            └─── PythonPlugins/
                 ├─── PI_MyPlugin.py
                 └─── myplugin/
                      ├─── compute.py
                      └─── compute.cpython-310.xpyce

... with both the .py source file and the .xpyce compiled & encrypted file under ``myplugin``.

Step 2. Add Keys to your PI\_\*.py file
***************************************

XPPython3 can't read the \*.xpyce file without a key, so you'll need to provide that in
PI_MyPlugin.py file. It can be simple as adding the following to the top (**Note**: your key will be
different from those listed here -- use value you just computed!)::

   from XPPython3.xpyce import update_keys

   my_keys = {'myplugin.compute': '3WrDPhFzJ_XCddKBi_Omzld9IfXrIFkMo7beVBP5L0o='}
   update_keys(my_keys)

   from .myplugin import compute

   class PythonInterface:
      ...

The key to your dict ``my_keys`` is the absolute module name: ``<package>.<module>``. If you have
more than one file you're encrypting (with the same key), you can simply use ``<package>``, e.g.::

   my_keys = {'myplugin': '3WrDPhFzJ_XCddKBi_Omzld9IfXrIFkMo7beVBP5L0o='}
   update_keys(my_keys)

.. note:: Make sure you call ``update_keys()`` *before* attempting to ``import compute``!

With the key, the user could manually *decrypt* the \*.xpyce file but that merely gets them
a compiled \*.pyc file. This is a great way to protect your algorithm without any digital rights management.

.. note:: There are tools which will allow an enterprising individual to read and manipulate python byte-code, but
          the same can be said for manipulating compiled binary shared objects. **Nothing** is absolutely secure.
          
An alternative to directly adding keys to your PI\_\*.py file, you might require a user to login to your server and download
keys, or read them from a configuration file. In any case, you have to call ``update_keys()`` *prior* to importing
the encrypted module(s).

Step 3. Remove your sensitive python file
*****************************************

Remove (or really, just don't package and deploy) your sensitive python file. Of course you'll
want to keep and test with an unencrypted version. For this example, simply rename ``compute.py`` to ``compute.pydisable``
so python can't find it::

  $ mv compute.py compute.pydisable

Now, restart X-Plane and notice the plugin still works!

For your testing purposes, XPPython3 will *always* load the \*.py version of the file if it exists, so you can test
without needing to remove the encryption routines, \*.xpyce files & keys until you're ready to deploy.

Next
****

There are lots of options to tailor this process, see :doc:`details` when you're ready to dive-in.

