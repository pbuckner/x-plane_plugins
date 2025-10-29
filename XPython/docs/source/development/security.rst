Security
========

A couple notes on security.

Your Responsibility
-------------------

Your plugin executes as part of X-Plane but with almost no other restrictions. Your code
and easily cause harm to your users' computing platforms. Some rules:

* Your code should not attempt to access *any* location outside of the X-Plane
  directory root.

* Your code should not "call home" without telling the user and, ideally, give them
  the option to disable. At least log the request so they can know it is being
  done (and why).

* Your code should not continue executing as a daemon or
  hidden process after X-Plane has exited. If your code lives outside of X-Plane, make
  sure it's visible to the user.

Your Code Security
------------------

Python programs tend to be released as source code. This convention promotes trust
and allows users to understand details of program execution.

Understandably, you may want to hide certain details for commercial purposes or
to simplify maintenace by making user-modifications difficult. Here are some options.

Deploy Compiled Python
**********************

As you may know, the python interpreter compiles .py files into .pyc files. These
compiled files work cross-platform, but *only* with the same major.minor version of python.

If you move the compiled .pyc to the same directory as the original .py, python will attempt
to load the .pyc file.

XPPython3 supports this as well. This means you can:

#. Create your plugin ``PI_foo.py`` in, for example, Resources/plugins/PythonPlugins.

#. Run and test it. Python will automatically create ``__pycache__/PI_foo.cpython-312.pyc``.

#. Rename the compiled file to ``PI_foo.pyc``.

#. Move it from the ``__pycache__`` sub-directory to the original location of your plugin.

#. Delete (or at least, don't deploy) your ``PI_foo.py`` file.

XPPython will notice your ``PI_foo.pyc`` file on startup in the standard ``PythonPlugins`` directory,
and will use it to enable your plugin.

This is easy and works cross-platform. Your code isn't secure, but it's in a form
which most users would find difficult to modify.

Note this will **fail** if/when XPPython3 changes it's underlying version of python (currently 3.12).
There are no plans to do this for the lifetime of X-Plane 12. I anticipate that with the next
major version of X-Plane (v.13?) XPPython3 will upgrade it's version of python.
