Transitioning from Xlua to XPPython3
====================================

.. warning:: This is proposed documentation, for review only.

Laminar provides a Xlua plugin which is commonly used with Aircraft plugins. It is simple, but
limited in functionality. If you're familiar with Xlua and want to transtion to python,
here are some tools to make it easier for you.

Overview
--------

Because nearly all Xlua plugins are for aircraft, we'll describe XPPython3 in the context
of using it as an aircraft plugin.

Installation
++++++++++++

As with any XPPython3 plugin, the user must install XPPython3, once only, under Resources/plugins as
described in :doc:`/usage/installation_plugin`.
That plugin will be responsible for loading all python-based plugins whether they be global, aircraft,
or scenery plugins. (See also :doc:`/development/plugins`)

An aircraft-specific plugin will be installed under the Aircraft's folder, under a new directory called
``plugins/PythonPlugins``. This is analogous to xlua plugins being placed under ``plugins/xlua``. The
actual name of the main python plugin file must start with ``PI_``, but otherwise does not matter::

    <X-Plane>/
    └─── Resources/
    |    └─── plugins/
    |         └─── XPPython3/     <--- standard XPPython3 installation
    |         └─── PythonPlugins/ <--- folder for global python plugins
    └─── Aircraft/
         └─── Cessna 172SP/
              └─── plugins/
                   ├─── xlua/            <--- standard xlua plugins
                   |    └─── ...
                   └─── PythonPlugins/   <--- folder for python aircraft plugins
                        ├─── PI_MyPlugin.py
                        └─── PI_MyOtherPlugin.py
           
Placing your python plugin under your Aircraft folder means it will be loaded only when
your aircraft is loaded, and unloaded when the user changes aircraft. Placing your
plugin under the ``Resources/plugins`` global location would load your plugin for
every flight.

How it works
++++++++++++

Fundamentally, an aircraft plugin is nearly identical to any other plugin. It will
likely read / write datarefs, set timers for delayed operation and respond to user
and sim events.

The X-Plane SDK supports all of these actions of course, but as it's designed for
a C/C++ programmmer it has not been simplified for typical use of an aircraft.

With XPPython3, we include a module (``easy_python``) that, together with
some addition tools, mimics the Xlua interface. It provides the same
functionality, and does the heavy lifting to convert this simplified interface back
into native X-Plane API.

This means you can do simple-things simply, yet still have the full power of the
SDK API (and XPPython3) if/when you want it.

The easiest way to see this is by examining a :doc:`xlua_typical`.


.. toctree::
   :titlesonly:
   :hidden:

   xlua_typical
   /development/modules/datarefs
   /development/modules/commands
   /development/modules/timers
   /development/modules/easy_python
   /development/modules/xlua_variables

