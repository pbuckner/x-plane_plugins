Global, Aircraft, and Scenery Plugins
=====================================

From an X-Plane perspective, there are three types of plugins:
Global, Aircraft, and Scenery. Internally, these plugins are identical.
They have access to the same SDK calls, receive the same messages,
and must follow the same set of rules.

They differ in where they are place in the filesystem, and when they are
intialized:

 ========= ================================ =============================
 Type      Located                          Lifetime
 ========= ================================ =============================
 Global    Resources/plugins                Loaded first at startup
                                            Unloaded at program exit.
 Aircraft  Aircraft/<aircraft>/plugins      Loaded when User Aircraft
                                            is initialized or changed.
                                            Unloaded when User Aircraft
                                            is changed or program exit.
 Scenery   Custom Scenery/<scenery>/plugins Loaded (after Global) at
                                            startup. Loaded (before
                                            Global) at program exit.
 ========= ================================ =============================


You can create XPPython3 plugins of any type by placing your PI_<plugin>.py
file(s) in the appropriate ``PythonPlugins`` directory, *under* the
relevant ``plugins`` directory. (The ``plugins`` directories contain the C/C++,
XLua, and other plugin types: we look for ``PythonPlugins`` directory there
to find any XPPython3 plugins.)

For **Aircraft** the ``plugins`` directory is contained in the same folder
as the aircrafts ``.acf`` file: That's how we located it, by getting the
``.acf`` file and then looking for ``plugins/PythonPlugins``.

 To have a plugin activated only for Laminar Cessna 172, place it in
 a new folder::

  <XP>/Aircraft/Laminar Resource/Cessna 172SP/plugins/PythonPlugins/

 You can have any number of XPPython3 plugins in that directory.  

For **Scenery** the ``plugins`` directory is in the same location
as the scenery's ``Earth nav data`` and ``objects`` folder.

 To have a plugin for KSEA Demo Area, place it in a new folder::

  <XP>/Custom Scenery/KSEA Demo Area/plugins/PythonPlugins/

 You can have any number of XPPython3 plugins in that directory.  

.. warning:: Scenery plugins are **always** loaded and enabled. This means your KSEA plugin
          will be running even when you're flying in South Africa. For this reason, make
          sure your plugin does very little when the User Aircraft is no where near it.

Access to Python Modules
------------------------

Note that XPPython plugins of one type may access python modules of another
type: We load each as a package.

Internal plugins are loaded using the ``XPPython3`` package, so you can always
access them from any plugin as, for example, ``import XPPython3.xp``.

Global plugins are loaded using the ``PythonPlugins`` package, so you can
always access them from any plugin as, for example, ``import PythonPlugins.PI_<plugin>``,
or perhaps more likely, ``import PythonPlugins.myplugin.utils``

Aircraft plugins are loaded using a long package name (it's unlikely you'll be
cross-referencing these.) But they are available, for example::

  import importlib
  importlib.import_module("utils.py", "Laminar Resource.Baron B58.plugins.PythonPlugins")

Similarly, Scenery plugins are loaded using a long package name, for example::

  import importlib
  importlib.import_module("utils.py", "KSEA Demo Area.plugins.PythonPlugins")

A more useful technique would be to place your shared utilities under the Global directory and
then import them into (each) of your Aircraft or Scenery plugins::

  # PI_MyAircraft.py
  import PythonPlugins.abc_aircraft.aircraft_utils as aircraft_utils

Python modules which don't have to be shared can be (should be) stored with the aircraft plugin
and can be loaded directly using a relative import::

  # PI_MyAircraft.py
  from . import cessna_utils


Startup Sequence
----------------

Plugins are started in a particular sequence. In general, you should code as much as
possible to avoid relying on a particular sequence. Laminar suggests you register your
data references in you XPluginStart() routine, but look for other data references in your
XPluginEnable() routine, as this most plugins will be Started prior to any plugin becoming
Enabled.

.. note:: With a particular grouping, order **is not guaranteed**. While it may appear that plugins
    are loaded alphabetically, this may not always work.

On program startup, the sequence is::

  X-Plane Started

    Global Plugins Started
    -----------------------
      START Resources/plugins/abc.xpl
      START Resources/plugins/XPPython3.xpl
         START XPPython3/I_PI_<plugin1>.py
               XPPython3/I_PI_<plugin2>.py
         START PythonPlugins/PI_<plugin1>.py
               PythonPlugins/PI_<plugin2>.py
         START Custom Scenery/.../PI_<plugin1>.py
               Custom Scenery/.../PI_<plugin2>.py
      START Resources/plugins/ZYX.xpl

    Scenery Plugins Started
    -----------------------
      START Custom Scenery/.../plugins/abc.xpl

    Global Plugins Enabled
    -----------------------
      ENABLE Resources/plugins/abc.xpl
      ENABLE Resources/plugins/XPPython3.xpl
         ENABLE XPPython3/I_PI_<plugin1>.py
                XPPython3/I_PI_<plugin2>.py
         ENABLE PythonPlugins/PI_<plugin1>.py
                PythonPlugins/PI_<plugin2>.py
         ENABLE Custom Scenery/.../PI_<plugin1>.py
                Custom Scenery/.../PI_<plugin2>.py
      ENABLE Resources/plugins/ZYX.xpl

    Scenery Plugins Enabled
    -----------------------
      ENABLE Custom Scenery/.../plugins/abc.xpl

This is followed by the loading of the selected User Aircraft::

  Aircraft Loaded

    Aircraft Plugins Started & Enabled
    ------------------------
      START  Aircraft/.../plugins/abc.xpl
      ENABLE Aircraft/.../plugins/abc.xpl
      START  Aircraft/.../plugins/PythonPlugins/PI_<plugin1>.py
      ENABLE Aircraft/.../plugins/PythonPlugins/PI_<plugin1>.py
      START  Aircraft/.../plugins/PythonPlugins/PI_<plugin2>.py
      ENABLE Aircraft/.../plugins/PythonPlugins/PI_<plugin2>.py

When the user changes the selected aircraft::

  Aircraft Changed

    Aircraft1 Plugins Disabled & Stopped
    ------------------------
      DISABLE Aircraft/.../plugins/abc.xpl
      STOP    Aircraft/.../plugins/abc.xpl
      DISABLE Aircraft/.../plugins/PythonPlugins/PI_<plugin1>.py
      STOP    Aircraft/.../plugins/PythonPlugins/PI_<plugin1>.py
      DISABLE Aircraft/.../plugins/PythonPlugins/PI_<plugin2>.py
      STOP    Aircraft/.../plugins/PythonPlugins/PI_<plugin2>.py

    Aircraft2 Plugins Started & Enabled
    ------------------------
      START  Aircraft/.../plugins/abc.xpl
      ENABLE Aircraft/.../plugins/abc.xpl
      START  Aircraft/.../plugins/PythonPlugins/PI_<plugin1>.py
      ENABLE Aircraft/.../plugins/PythonPlugins/PI_<plugin1>.py
      START  Aircraft/.../plugins/PythonPlugins/PI_<plugin2>.py
      ENABLE Aircraft/.../plugins/PythonPlugins/PI_<plugin2>.py

On program exit::

  X-Plane Started

    Global and Scenery Plugins Disabled
    -----------------------------------
      DISABLE Resources/plugins/abc.xpl
      DISABLE Resources/plugins/XPPython3.xpl
        DISABLE PythonPlugins/PI_<plugin1>.py
        DISABLE PythonPlugins/PI_<plugin2>.py
        DISABLE XPPython/I_PI_<plugins>.py
        DISABLE Custom Scenery/.../PI_<plugins>.py
      DISABLE Custom Scenery/plugins/abc.xpl

    Aircraft Plugins Disabled
    ------------------------
      DISABLE Aircraft/.../plugins/abc.xpl
      DISABLE Aircraft/.../plugins/PythonPlugins/PI_<plugin1>.py
      DISABLE Aircraft/.../plugins/PythonPlugins/PI_<plugin2>.py

    Global and Scenery Plugins Stopped
    ----------------------------------
      STOP Resources/plugins/abc.xpl
      STOP Resources/plugins/XPPython3.xpl
        STOP PythonPlugins/PI_<plugin1>.py
        STOP PythonPlugins/PI_<plugin2>.py
        STOP XPPython/I_PI_<plugins>.py
        STOP Custom Scenery/.../PI_<plugins>.py
      STOP Custom Scenery/plugins/abc.xpl

    Aircraft Plugins Stopped
    ------------------------
      STOP    Aircraft/.../plugins/abc.xpl
      STOP    Aircraft/.../plugins/PythonPlugins/PI_<plugin1>.py
      STOP    Aircraft/.../plugins/PythonPlugins/PI_<plugin2>.py

Note that XPPython3 Custom Scenery plugins are loaded with Global plugins, before the loading
of non-python Custom Scenery plugins.
