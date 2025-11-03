Encrypted Python
================

You may want to restrict execution of your plugin. This can be achived by using the XPPython3 encryption
loader: a custom module loader which can decrypt module on-the-fly, but only if the proper key is available.

Whenever XPPython3 attempts to locate a python module, it will first try to find python source (\*.py). Failing that
it will look for :doc:`compiled python <deployment_compiled>` (\*.pyc), and failing that, will look for encrypted python (\*.xpyce).
This final step is unique to XPPython3.

``xpyce`` was originally introduced by XPPython3 v3.0.5 in 2020, but removed due to security and logistical issues.
It was re-introduced in 2025 with XPPython3 v4.6.

 I am indebted to Soroco Americas Private Limited (https://blog.soroco.com) and https://github.com/soroco/pyce
 for the initial code and idea. I've modified it slightly to support more python versions and work better
 within X-Plane. You can read their code and blog for background.

How xpyce works
---------------

Instead of distributing \*.py files, you can distribute encrypted \*.xpyce files. For each
original \*.py file, you'll create a compiled \.pyc file and then encrypt it, resulting in a
private decryption key *and* a \*.xpyce file.

Distribute the \*.xpyce file(s) and get the decryption key to the user somehow [#F1]_, and with that information, XPPython3 will
load the module.

Your (non-encrypted) python will update XPPython3 with the decryption keys at runtime, and when XPPython3 attempts to load
a module it will do the normal search for the appropriate \*.py file & failing that, will look for a relevant \*.xpyce
file. If the \*.xpyce file is found, XPPython3 will see if it knows a decryption key for that file and then attempts to
decrypt and load the compiled byte-code.

If everything is done correctly, there is a very small performance impact on load and zero impact on run time.

Decryption Keys and Loading Complexity
--------------------------------------

Internally, you're updating a loading dictionary which looks similar to::

  {'module.foo.abc': '<key for module.foo.abc>',
   'module.foo.xyz': '<key for module.foo.xyz>',
   ...
   }

The key to the dict is a module name (not a file!). When you encrypt a file you get a decryption key.
When you update the loading dictionary, you need to provide **the module name**.

We're flexible enough to support a variety of specifications. Assume your plugin ``PI_MyPlugin.py`` uses a module ``myplugin/compute.py``.
Your code will likely include something like::

  from .myplugin import compute

You can list the key as belonging to any of (or any combination of)::

  PythonPlugins.myplugin.compute
  Aircraft.myplugin.compute
  Custom Scenery.myplugin.compute
  myplugin.compute

The final specification is the most general, allowing the key to be used regardless where the plugin is installed. The
first three examples work *only* when your plugin is installed under ``PythonPlugins``, under *any* ``Aircraft``, or under
*any* ``Custom Scenery``. (See :doc:`import`.)

Additionally, you can apply the key to the parent module, which will cause the key to
be used for *all* \*.xpyce files located within that module::

  PythonPlugins.myplugin
  Aircraft.myplugin
  Custom Scenery.myplugin
  myplugin
  
Doesn't matter how you actually import the (potentially) encrypted module, the key look-up algorithm will search for
all possible alternatives, prefering best-match.

Note that you don't import modules like ``from Aircraft.myplugin import compute`` or ``from Custom Scenery.myplugin import compute``:
you'll still (likely) use relative imports like ``from .myplugin import compute``.

Example 1: Protect an algorithm
-------------------------------

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
          return "MyPlugin", "xppython3.myplugin", "Test plugin for encryption"

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

.. image:: /images/myplugin-encryption.png

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

For whatever reason, you want to protect ``compute.py``, so encrypt it:

Step 1. Generate the keys
*************************

The easiest way to generate a key is using *your platform copy of python 3.12*. If you don't have python 3.12
installed on your computer you can either install it, or see the comments in ``XPPython3/xpyce_compile.py`` for
how you can use the embedded XPPython3 copy of python.

* Use ``xpyce_compile.py`` to both compile and encrypt your \*.py file. It is located in XPPython3 folder.
  (You'll have to have already installed the ``cryptography`` module for python 3.12. e.g., ::

    python3.12 -m pip install cryptography
  
* The result is one line for each encrypted module with its key:

.. code-block:: console

   $ cd Resources/plugins/PythonPlugins
   $ python3.12 ../XPPython3/xpyce_compile.py myplugin/compute.py 
   myplugin.compute: c2acb3e6b463c72ef8a1051e90990f10fc6a14ecf5103f7ce016addff2dc52c6

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
                      └─── compute.xpyce

Step 2. Add Keys to your PI\_\*.py file
***************************************

XPPython3 can't read the \*.xpyce file without a key, so you'll need to provide that in
PI_MyPlugin.py file. It can be as simple as adding this to the top (**Note**: your key will be
different from that listed here -- use values you just computed!)::

   from XPPython3.xpyce import update_keys

   keys = {
        'myplugin.compute': 'c2acb3e6b463c72ef8a1051e90990f10fc6a14ecf5103f7ce016addff2dc52c6',
   }
   update_keys(keys)

   from .myplugin import compute

   class PythonInterface:
      ...

Also, make sure you call ``update_keys()`` *before* attempting to ``import compute``!

In the above example, we provide the keys in readable python (PI_MyPlugin.py). With the keys, the user could decrypt the
\*.xpyce file but, that merely gets them a compiled \*.pyc file, not python source. True, this can be further de-compiled,
but that takes a good deal of effort and skill.

.. note:: There are tools which will allow an enterprising individual to read and manipulate python byte-code, but
          the same can be said for manipulating compiled binary shared objects. **Nothing** is absolutely secure.
          
An alternative to directly adding keys to your PI\_\*.py file, you might require a user to login to your server and download
keys, or read them from a configuration file. In any case, you have to call ``update_keys`` *prior* to importing
the encrypted module(s).

Given what we've said before, you could list the key's module as ``'myplugin'``, and it will work just as well.

Step 3. Remove your sensitive python file
*****************************************

Remove (or really, just don't package and deploy) your sensitive python file. Of course you'll
want to keep and test with an unencrypted version. For this example, simply rename ``compute.py`` to ``compute.pydisable``
so python can't find it::

  $ mv compute.py compute.pydisable

Now, restart X-Plane and notice the plugin still works!

For your testing purposes, XPPython3 will *always* load the \*.py version of the file if it exists, so you can test
without needing to remove the encryption routines, \*.xpyce files & keys until you're ready to deploy.


Example 2: Convert an Existing Plugin
-------------------------------------

Assume you have an existing (python3) plugin: ``PI_MySecret.py``, and you want to convert it.

1. Create subdirectory and place your original code there:

  .. code-block:: console

     $ pwd
     <XP>/Resources/plugins/PythonPlugins
     $ mkdir mysecret
     $ mv PI_MySecret.py mysecret

2. Create a new "shell" plugin PI_MySecret.py under PythonPlugins, which looks similar to::

    from .mysecret import PI_MySecret

    class PythonInterface(PI_MySecret.PythonInterface):
        pass

   Seriously, it's just four lines long (before adding the keys).
   
3. Test and convince yourself this new plugin is identical to your old plugin. (We've not done any encryption yet).

4. Generate keys
   Remember to generate keys for ``mysecret/PI_MySecret.py``, not for your new shell ``PI_MySecret.py``

  .. code-block:: console

   $ python3.12 ../XPPython3/xpyce_compile.py mysecret
 
5. Add the keys to your shell PI_MySecret.py file::

     from XPPython3.xpyce import update_keys
     
     keys = {
         'mysecret': '6fa0a80137281869ccbafa58c01354c74d3004de114309ade691d239e122cd68'
     }
     update_keys(keys)
     
     from .mysecret import PI_MySecret
     
     class PythonInterface(PI_MySecret.PythonInterface):
         pass

6. Test again.

   Note that *we still load the* ``mysecret/PI_MySecret.py`` file. This will always be true: if the \*.py file
   exists, we use it. If the \*.py file does not exist, we look for the \*.xpyce.

7. Move / Remove the \*.py and test again.

   .. code-block:: console

     $ mv mysecret/PI_MySecret.py mysecret/PI_MySecret.py-disabled

8. Done. Ship the code as::

                 ├─── PI_MySecret.py
                 └─── mysecret/
                      └─── PI_MySecret.xpyce


Additional Compiler Features
----------------------------

* **Encrypt Multiple Files**:

  The xpyce_compiler (:doc:`Manual page <xpyce_compile>`) is quite flexible. To encrypt multiple files, specify all of them as input,
  or specify the containing directory. Optionally, you can cause all files in a directory to be encrypted with the same key thereby
  simplifying your key-distribution task.

* **Reuse Keys**:

  To facilitate your ability to *change* a python file, yet re-encrypt it with the original key, you can specify what key(s) to use
  on the xpyce_compiler command line. Alternatively, you can set a set of keys in a JSON manifest file. See :doc:`Manual page <xpyce_compile>`.



Alternative Execution Startup
-----------------------------

* **Read keys from** ``__init__.py``

  If you've encrypted a directory, you can use a trick with ``__init__.py``. When loading a modules (e.g., ``myplugin``), python
  looks for and executes ``__init__.py`` before attempting to load a file within that modules. For example, create ``myplugin/__init__.py``::

    from XPPython3.xpyce import update_keys
    keys = {
      "myplugin": 'a1b2c3d4e5f6789012345678901234567890123456789012345678901234',
    }

    update_keys(keys)

  And, skip Step 2 in our example above where the keys where loaded by the main PI*.py file. Now, when your main file calls::

    from .myplugin import compute

  The __init__ file will be read and executed first, loading the keys! (Be sure NOT to encrypt this __init__.py file or you'll be stuck!)

  
* **Load keys from server**

  The above examples provide keys directly in the python source -- remember XPPython3 still needs a PI\_\*.py file
  to with a defined PythonInterface class in order to start the plugin.

  If you don't want to store the code in the python file, you could add code to your ``PythonInterface().XPluginStart()``
  function: It could read a local file with username / password information, and exchange that information with your server
  to retrieve the decryption keys (Note: the decryption keys will be the same for all users!). On success, you update
  the loader dictionary using ``XPPython3.xpyce.update_keys()``.

  Then, your XPluginStart or XPluginEnable can import the encrypted module, something like::

    class PythonInterface:
       def XPluginStart(self):
           try:
               user_credentials = readCredentialsFromFile()
               keys = getKeysFromServer(user_credentials)
               XPPython3.xpyce.update_keys(keys)
               mod = importlib.import_module('.myplugin.main')
           except:
               mod = None
           return 'Name', 'Signature', 'Description'
  
       def XPluginEnable(self):
           if not mod:
              promptUserForCredentials():
           else:
              mod.doStuff()
  
  Fortunately, you can test all this *without encryption* by simply not deleting your \*.py module (``myplugin/compute.py`` in this
  example). Once you have the credentials part working, delete the python file: then the ``importlib.import_module()`` code
  will look for the encrypted version.
  
----

.. [#F1] "*Get the decryption key to the user somehow*". If you're selling the product, send the key to them through email. Or,
         have the user enter their email in your plugin (a non-encrypted part), and contact your server to return the key
         if the email matches someone in your database. Lots of possibilities.

         If it's non-commercial, and the reason you're doing this is simply to obfuscate code, *include the key in your code*.
         We'll decrypt the file on the fly, but never re-create the source code: Your plugin will be usable, though unreadable,
         without needing to distribute the key out-of-band.
