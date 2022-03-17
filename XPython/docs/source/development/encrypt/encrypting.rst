Generating Keys and Encrypting Code
-----------------------------------

* You *must* do this for each version of python.

  * \*.pyc files potentially differ for each version of python.
  * \*.pyc files are the same across platforms, so you only need to generate keys on a single computer (Mac, Windows, or Linux).
  * \*.pyc files are the same between minor releases, so 3.7.1 versus 3.7.3 doesn't matter.

* Use ``xpyce_compile.py`` to both compile and encrypt your \*.py file. It is located in XPPython3 folder.
  (You'll have to have already installed the ``cryptography`` module for each version of python, e.g.::

    $ python3.6  -m pip install cryptography
    $ python3.7  -m pip install cryptography
    $ python3.8  -m pip install cryptography
    $ python3.9  -m pip install cryptography
    $ python3.10 -m pip install cryptography
  
* Call the script *for each version of python* and either provide a valid key, or allow the script to generate one:

  .. code-block:: console

     $ cd Resources/plugins/PythonPlugins
     $ cd myplugin
     $ python3.6 ../../XPPython3/xpyce_compile.py compute.py 
     compute.cpython-36.xpyce: 3WrDPhFzJ_XCddKBi_Omzld9IfXrIFkMo7beVBP5L0o=

  By not providing a key, the script created a new key. On subsequent calls, provide that
  same key with the ``-k <key>`` option, so all files are encrypted with the same value (below, I've truncated the keys
  to make the example more readable):
  
  .. code-block:: console

     $ KEY=3WrDPhFzJ_XCddKBi_Omzld9IfXrIFkMo7beVBP5L0o=
     $ python3.7  ../../XPPython3/xpyce_compile.py compute.py -k $KEY
     $ python3.8  ../../XPPython3/xpyce_compile.py compute.py -k $KEY
     $ python3.9  ../../XPPython3/xpyce_compile.py compute.py -k $KEY
     $ python3.10 ../../XPPython3/xpyce_compile.py compute.py -k $KEY
     $ ls compute.*
     compute.py
     compute.cpython-36.xpyce
     compute.cpython-37.xpyce
     compute.cpython-38.xpyce
     compute.cpython-39.xpyce
     compute.cpython-310.xpyce

The ``xpyce_compile.py`` script will compile and encrypt any number of files at the same time, just
include them in the command line. Also, if you have a (valid) key, you can specify that key to be
used for all of the files. Otherwise, a new key is generated for each file.

What's a *valid* key?
Valid keys have a particular form: you cannot just make up a string. The easiest way
to get a valid key is to have ``xpyce_compile.py`` run without specifying a key: it will generate a good
result and then re-use *that* key for all subsequent encryptions.

Because we use `Fernet symmetric encryption <https://cryptography.io/en/latest/fernet/>`_
keys must be a URL-safe base64-encoded 32-byte value. If you do not specify a key with ``xpyce_compile``, one will
be created for you.

Write down your keys!

.. Note:: We recommend you use the *same* key for all files, and for all python versions for
         your plugin. This will make key management vastly easier for you.

         Additionally, using the same key, you'll have the option to *continue* to use that key
         for future updates / bug-fixes for your plugin, which will avoid additional
         key-management issues for you users.


You **must** provide *all* versions of \*.xpyce files with your solution. Otherwise, your users will be
restricted to only the python versions you provide.
