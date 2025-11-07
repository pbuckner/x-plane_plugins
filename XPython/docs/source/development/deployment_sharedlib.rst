Shared Library
==============

Just as you can embed Python in C, you can embed C/C++ in Python. Essentially you
just build a shared library and then use a bit of bridging python code. Reasons to do this:

* **Speed**: C is nearly always going to be faster to execute.

* **Security**: Compiled C-code is less readable than compiled Python code.

* **Re-use**: Perhaps your fancy algorithm has already been written in C.

The main downside is you need to build your shared library three times, once for
each X-Plane platform. That means cross-compilers or multiple build machines.

There are a few technologies for calling C code from python. For this example
we'll be using ``ctypes``. It's the fastest interface and, if you only have a
few interfaces, is very easy to use. You may want to look at ``SWIG`` or ``cython``
as alternatives.

How
---

Plan and organize your C functions into modules so your python calls make sense.
(You're compiling one or more *modules* not 100% of your plugin.)

#. Compile a shared object, similar to::

    $ cc -fPIC -shared -o module.so module.c

   Compile a version for each platform, using different file names.
  
#. Place the shared object(s) in your module folder under the ``plugins/PythonPlugins`` directory.

#. Add some loader python code to the module's ``__init__.py`` file, to find and load
   the correct version of the shared library

#. Specify, in the ``__init__.py`` file, the function parameters for proper marshaling.

#. Import and call your function(s) just has you normally would using python

Result
------

Your shared object is unchanged by python. Python's ``ctypes`` module knows how
to load and access functions defined within the share object. You add a few
definitions and convenience functions to further "hide" the C-language details, keeping
your python code clean and "pythonic".

Example
-------

We'll repeat the example from :doc:`deployment_encrypted`: you have an algorithm "compute.munge" you
want to protect::

  #include <stdio.h>
  #include <string.h>

  char *munge(char *string)
  {
    /* reverse string */
    int len = strlen(string);
    int start = 0;
    int end = len - 1;
    char tmp;
    while (start < end) {
      tmp = string[start];
      string[start] = string[end];
      string[end] = tmp;
      start++;
      end--;
    }
    return string;
  }

Copy and paste the above into a file ``PythonPlugins/myplugin/compute.c``. This is analogous to our
creating ``compute.py`` file (python module), which contains the function ``munge()``.

Compile into a shared object, placing the shared object(s) in under the ``myplugin`` directory. For example::

  # MacOS
  cc -fPIC -shared -o compute.dylib compute.c

  # Windows
  cc -fPIC -shared -o compute.dll   compute.c

  # Linux
  cc -fPIC -shared -o compute.so    compute.c

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
          output = compute.munge(refCon.encode()).decode()
          xp.speakString(f"Munge of f{refCon} is f{output}")
          return 0
          
Add loader instructions into the ``myplugin/__init__.py`` file, this is pretty generic::

  import ctypes
  from platform import system
  ext = {'Linux': 'so', 'Darwin': 'dylib', 'Windows': 'dll'}[system()]

  compute = ctypes.CDLL(f"{__package__}/compute.{ext}")

Repeat the final line for each module/shared object you have under this ``myplugin`` directory.

**AND**, add to ``__init__.py`` function definitions (argument types and result type),
for each function you're going to load from the shared object. In our case, we have
a single function "munge" which takes a single ``char *`` as an argument with a ``char *``
as the result::

  compute.munge.argtypes = (ctypes.c_char_p, )
  compute.munge.restype = ctypes.c_char_p
  
Done.

A couple things to note: In the ``__init__.py`` file we use ``__package__`` which (like ``__file__``)
computes to the location of the imported package. The allows ``ctypes.CDLL()`` to have a full absolute
path to the shared object. Otherwise python will use the platform-specific loader mechanism such
as searching along the ``LD_LIBRARY_PATH``.

Second, note in this example the ``PI_MyPlugin.py`` file, differs slightly from the same file
used in :doc:`deployment_encrypted`. Specifically the call to "munge()" is::

  output = compute.munge(refCon.encode()).decode()
  
This is simply because python strings need to be encoded before passing to C-language functions and ``char *`` need
to be encoded back to python strings: ``ctypes`` won't do this for you.

Your resulting plugin will consist of::
   
  <X-Plane>/
  └─── Resources/
       └─── plugins/
            └─── PythonPlugins/
                 ├─── PI_MyPlugin.py
                 └─── myplugin/
                      ├─── __init__.py
                      ├─── compute.dynlib
                      ├─── compute.dll
                      └─── compute.so

