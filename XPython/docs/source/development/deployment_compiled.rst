Compiled Python
===============

Understandably, you may want to hide certain details for commercial purposes or
to simplify maintenace by making user-modifications difficult.

As you may know, the python interpreter compiles .py files into .pyc files. These
compiled files work cross-platform, but *only* with the same major.minor version of python.

How
---

If you move the compiled .pyc to the same directory as the original .py, python will attempt
to load the .pyc file.

XPPython3 supports this as well. This means you can:

#. Create your plugin ``PI_my_fine_plugin.py`` in, for example, Resources/plugins/PythonPlugins.

#. Run and test it. Python will automatically create ``__pycache__/PI_my_fine_plugin.cpython-312.pyc``.

#. Rename the compiled file to ``PI_my_fine_plugin.pyc``.

#. Move it from the ``__pycache__`` sub-directory to the original location of your plugin.

#. Delete (or at least, don't deploy) your ``PI_my_fine_plugin.py`` file.

Result
------

XPPython will notice your ``PI_my_fine_plugin.pyc`` file on startup in the standard ``PythonPlugins`` directory,
and will use it to enable your plugin. You can have some or all of your python files compiled, but
they will need to be located in the same place where your non-compiled python files would be found::

      --Original files--                 --Compiled files--
     └─── PythonPlugins/                └─── PythonPlugins/                    
          ├─── PI_my_fine_plugin.py          ├─── PI_my_fine_plugin.pyc  
          └─── my_fine_plugin/               └─── my_fine_plugin/       
               ├─── module.py                     ├─── module.pyc        
               ├─── utils.py                      ├─── utils.pyc         
               ├─── image.png                     ├─── image.png        
               └─── doc/                          └─── doc/             

This is easy and works cross-platform. Your code isn't *secure*, but it's in a form
which most users would find difficult to modify.

Note this will **fail** if/when XPPython3 changes it's underlying version of python (currently 3.12).
There are no plans to do this for the lifetime of X-Plane 12. I anticipate that with the next
major version of X-Plane (v.13?) XPPython3 will upgrade it's version of python.

Manual Compilation
------------------

You can manually compile your files, placing the resulting \*.pyc files in the proper directories using::

  $ cd plugins/PythonPlugins
  $ python3.12 -m compileall PI_my_fine_plugin.py my_fine_plugin -b -o 2

The ``-b`` option causes the compiled files to *not* be placed in the ``__pycache__`` directory, and the ``-o 2``
option causes all comments to be stripped from the resulting files.

Be sure to compile using Python 3.12, as this will create \*.pyc files compatible with XPPython3 for X-Plane 12.

.. toctree::
   :hidden:
   :maxdepth: 1

   xpyce_compile
