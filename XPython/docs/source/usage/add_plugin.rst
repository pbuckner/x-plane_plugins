Adding a Python Plugin
======================

By itself, this XPPython3 plugin does very little. The power is that it allows you to
create, install, and execute X-Plane plugins which are written in Python.

We provide lots of simple :doc:`/development/samples`, you can find more XPPython3 plugins on
the internet and at the `X-Plane.org forum <https://forums.x-plane.org>`_, and you can write some
for yourself!

So, once you've installed XPPython3, where do these python plugins go?



File System Layout
------------------

XPPython3 is an X-Plane plugin, so it needs to be in the ``Resources/plugins`` folder.
In turn, XPPython3 loads PI\_\*.py plugin files, so *they* need to be in ``Resources/plugins/PythonPlugins`` folder.
Python plugins automatically downloaded to the ``PythonPlugins/samples`` folder won't get loaded on X-Plane startup.

The structure is:

.. code-block:: none

  <X-Plane>/
  └─── Resources/
       └─── plugins/
            ├─── XPPython3/
            │    ├─── mac_x64/
            │    |    └─── XPPython3.xpl
            │    ├─── lin_x64/
            │    |    └─── XPPython3.xpl
            │    └─── win_x64/
            │         └─── XPPython3.xpl
            └─── PythonPlugins/
                 ├─── PI_<plugin1>.py
                 ├─── PI_<plugin2>.py
                 ├─── ....
                 └─── samples/
                      ├─── PI_<sample1>.py
                      ├─── PI_<sample2>.py
                      └─── ....

    
Add a Python Plugin
-------------------

Third-party Python plugins (and those *you* create) themselves go to:

* **Resources/plugins/PythonPlugins/** folder

  + Single file named :code:`PI_<anything>.py` for each separate python plugin. This is the starting point for each python plugin [#F2]_

    + A third-party plugin may *also* have other files or a folder to go into this PythonPlugins folder. That's okay.

  + Plugins are loaded in order as returned by the OS: that is, do not assume alphabetically!
  + Python files can then import other python files.

(There are exceptions, but this will cover 99% of your plugins. [#F1]_) 

So as a quick test, copy the file ``PI_HellowWorld1.py`` from ``PythonPlugins/samples`` up one folder to ``PythonPlugins``.

.. code-block:: none

  <X-Plane>/
  └─── Resources/
       └─── plugins/
            ├─── XPPython3/
            └─── PythonPlugins/
                 ├─── PI_HelloWorld1.py  ❮══════╗
                 └─── samples/                  ║
                      └─── PI_HelloWorld1.py ❯══╝

Restart X-Plane and you'll get a popup window:

.. image:: /images/hello_world_demo.png

|
|

Internal Python Files
---------------------

Internally, we also use:

* **Resources/plugins/XPPython3/** folder

  + *Do Not* place your python files in this directory. They may be deleted on reload of the XPPython3 plugin.
  + These are "internal" plugins. This is intended for internal use, and are additional python plugins loaded
    prior to the user directory "PythonPlugins". This is the same folder as holding
    the binary :code:`*.xpl` plugin files. To be loaded on startup from this folder,
    files need to be named :code:`I_PI<anything>.py`.
  + Python files in this directory will also be in your PYTHONPATH and therefore accessible to your
    scripts. (Feel free to look at these source files for examples / inspiration).


-----

.. [#F1] *Most* python plugins are "global" and are installed as described above.
         It is possible to write python plugins for specific Aircraft, or related to Scenery.
         Such plugins are install slightly differently. See :doc:`/development/plugins`.
.. [#F2] For Python2, we used "PythonScripts" folder -- same idea, but we need a different folder.
