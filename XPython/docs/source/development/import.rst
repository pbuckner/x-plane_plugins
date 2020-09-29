Python Packages and Importing
=============================

Python3 introduced more powerful module packaging. To support this, together
with reducing module conflict, XPPython3 has structured imports in a simple
consistent manner.

The Problem
-----------

First, consider the problem. Assume you have a utility module called
``gis.py`` which perhaps does some greographic calculations, say calculating
Great Circle distance between two points, and determining the bearing from
one point to a second point.

You want to use this module in your plugin, so you simply::

  import gis

Sadly, assume some other plugin developer also happened
to create a ``gis.py`` module. Perhaps it's in a global plugin (under
``Resources/plugins/PythonPlugins``), or perhaps it's with a Scenery Plugin
(under ``Custom Scenery/KSEA/plugins/PythonPlugins``).

The problem is, once you've imported ``gis`` module, **everyone** gets that
same module. The fix is to use packages, such that "my" module differs from "your" module.

Our Solution
------------

To support this, we import global plugins as part of the ``PythonPlugins`` package, so your
``PI_MyPlugin.py`` is imported as the module ``PythonPlugins.PI_MyPlugin``.

================= ======================================================== =====================================
Plugin Type       Example .py                                              Imported as Module
================= ======================================================== =====================================
Global (user)     Resources/plugins/PythonPlugins/PI_Abc.py                PythonPlugins.PI_Abc
Global (internal) Resources/plugsin/XPPython3/xp.py                        XPPython3.xp
Aircraft          Aircraft/Laminar/Baron/plugins/PythonPlugins/PI_Baron.py Baron.plugins.PythonPlugins.PI_Baron
Scenery           Custom Scenery/KSEA/plugins/PythonPlugins/PI_ground.py   KSEA.plugins.PythonPlugins.PI_ground
================= ======================================================== =====================================

Now, ``gis.py`` can be uniquely identified as PythonPlugins.gis or KSEA.plugins.PythonPlugins.gis, etc.

Resulting Namespace
-------------------

Even better, so you don't pollute the namespace, create a subpackage under Resources/plugins. Consider
the following set of plugins::

  <X-Plane>/
  ├─── Resources/
  │    └─── plugins/
  │         ├─── XPPython3/
  │         │    └─── xp.py
  │         |     
  │         └─── PythonPlugins/
  │              ├─── PI_abc.py
  │              ├─── abc/
  │              │    └─── gis.py
  │              │
  │              ├─── PI_xyz.py
  │              └─── xyz/
  │                   └─── gis.py
  │              
  ├─── Aircraft/Laminar/Baron/plugins/PythonPlugins/
  │              ├─── PI_abc.py
  │              └─── abc/
  │                   └─── gis.py
  │              
  └─── Custom Scenery/KSEA/plugins/PythonPlugins/
                 ├─── PI_abc.py
                 └─── abc/
                      └─── gis.py
                 
First, any plugin can get access to the SDK and ``xp.py`` interface by using::

  import XPLMGraphics
  from XPPython3 import xp

Each plugin can get access to "their" copy of ``gis.py`` by loading a relative package::

  from .abc import gis

A plugin could reference a different copy of ``gis.py`` by loading an absolute package::

  # these are equivalent, and get the copy under Resources/plugins/PythonPlugins/abc:
  import PythonPlugins.abc.gis as gis
  from PythonPlugins.abc import gis

  # this refers to a different 'gis'
  from KSEA.plugins.PythonPlugins.abc import gis

Commonly, you're not going to be referencing "other" plugin's python files, but know that you can. More importantly,
you won't be accidentally importing someone else's python files causing headaches for you.

If you have common files -- for example you want all your Scenery plugins to use the same ``gis.py`` module, you have two
choices:

1. Put the same file under each Scenery plugin and use relative imports  -- ``from . import gis`` -- (which may be a maintenance issue
   if you want to update your ``gis.py`` file.), or
2. Create a global package under ``Resources/plugins/PythonPlugins``, say ``abc`` and place your file there. Then
   in each Scenery package::

     from PythonPlugins.abc import gis

   ... and you'll all be using the exact same module.

To summarize
------------

* Import standard python modules as you always have::

    import re

* Import XPLM modules directly::

    import XPLMGraphics

* Import xp interaface (things under XPPython3) using packages::

    from XPPython3 import xp

* Use a sub-package for your utility scripts and import them using relative imports::

    from .abc import utils

  Within that sub-package (e.g., within utils.py) if you need to refer to another file
  in the same package use::

    from . import another_file
    another_file.my_func()

    # or:
    from .another_file import my_func
    my_func()

