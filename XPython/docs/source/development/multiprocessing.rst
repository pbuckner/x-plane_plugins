Multiprocessing and XPPython3
=============================

If you're trying to use python's ``subprocess`` or ``multiprocessing`` modules, you'll
quickly find out that X-Plane gets spawned, perhaps multiple times, rather than the *python*
executable. The result is usually a crash.

Using ``subprocess``, you commonly will spawn a python process using ``sys.executable``::

  subprocess.Popen([sys.executable, 'mypython.py'])

-or-::

  subprocess.run([sys.executable, 'mypython.py'])

(You should have no problems if you are trying to launch a process *other than* python using ``subprocess``.)

Using ``multiprocessing``, you don't specify the executable but know that it (also) defaults
to ``sys.executable``.

The problem is that X-Plane **is** the python executable, for example on a Mac::

  >>> print(sys.executable)
  '/Volumes/MyDrive/X-Plane-11.5/X-Plane.app/Contents/MacOS/X-Plane'

On a PC::

  >>> print(sys.executable)
  'D:\\X-Plane 11.50\\X-Plane.exe'

So, what to do?

Finding Python
--------------

Fortunately, you can specify the correct value rather than use the provided ``sys.executable``. The trick is
how to find the correct value on all platforms.

Short answer
++++++++++++


Use :py:data:`XPPython.pythonExecutable`.

We'll calculate it for you at startup and make it available via this constant, for example::

  subprocess.run([xp.pythonExecutable, 'mypython.py])

For ``multiprocessing``, you need to explicitly set the value::

  multiprocessing.set_executable(xp.pythonExecutable)
  p = multiprocessing.Process(target=f, args=child_conn, ))
  p.start()

Long answer
+++++++++++

We'll find python that's compatible with the executing X-Plane, including the same set of libraries, but
you may want to use a different version of python or a virtual environment with different libraries. Rather
than using ``xp.pythonExecutable`` you can calculate your own.

**On Windows**, python is located at::

  os.path.join(sys.exec_prefix, 'pythonw.exe')

*python.exe* will popup a console window while executing, *pythonw.exe* will not, so we use the latter.

**On Mac**, python is located at::

  os.path.join(sys.exec_prefix, 'bin', 'python3')

Normally, there will be *python3* symlinked to the specific minor version (e.g., python37 or python38)
Because the Mac uses different directories for each version of Python, *python3* will always match the
version you're running with X-Plane.

**On Linux**, python is also located at::

  os.path.join(sys.exec_prefix, 'bin', 'python3')

But, because ``sys.exec_prefix`` is often something like ``/usr``, the resolution of *python3* is likely
to be a symlink to a version in the same directory (which itself may contain ``python37``, ``python38``, and ``python39``,
you're not guaranteed to get the correct version.

To make sure you get the same version use::

  os.path.join(sys.exec_prefix, 'bin', 'python3{}'.format(sys.version_info.minor))

Or, as mentioned above, just use :py:data:`xp.pythonExecutable`

Example - PI_Multi.py
---------------------

Copy the following into your PythonPlugins directory.

::

   import os
   try:
       import xp
   except ImportError:
       pass
   
   import multiprocessing
   
   
   class PythonInterface:
   
       def XPluginStart(self):
           return 'PI_Multi', 'xppython.demos.multi', 'Example plugin using multiprocessing'
   
       def XPluginEnable(self):
           xp.log("Calling from PID {}".format(os.getpid()))
           parent_conn, child_conn = multiprocessing.Pipe()
           multiprocessing.set_executable(xp.pythonExecutable)
           p = multiprocessing.Process(target=f, args=(child_conn, ))
           p.start()
           xp.log('{}'.format(parent_conn.recv()))
           p.join()
           return 1
   
       def XPluginDisable(self):
           pass
   
       def XPluginStop(self):
           pass
   
       def XPluginReceiveMessage(self, *args, **kwargs):
           pass
   
   
   def f(conn):
       conn.send([42, None, 'hello from PID: {}'.format(os.getpid())])
       conn.close()
   
This is modeled after the example in python documentation for ``multiprocessing`` with Pipes. All it does is
spawn a separate process to run function ``f()``, when this plugin is enabled.

Couple of things to note:

1. We call ``multiprocessing.set_executable()``. If you don't, it will spawn a new copy of X-Plane with
   disastrous results.

2. The spawned function, ``f()``, *does not call X-Plane*. It's running in a regular python process, and
   therefore does not have access to Laminar SDK or XPPython3 calls. In our example, we use Pipes to
   send data back to the parent process. You should be able to use other ``multiprocessing`` features
   to exchange data.

3. Because the function ``f()`` is in this file, we need to guard against the ``import xp``. Otherwise,
   when the child function starts, it will load this whole file and immediately get an ImportError. Because
   we'll catch that error, the child process is able to continue. Python ``multiprocessing`` documentation
   describes how to guard against a similar problem using ``__main__``. But the idea is the same:
   when the child process starts up, you want it to run *only* the code it needs to run.
