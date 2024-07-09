Using PIP
=========

PIP is a Python Package Installer. Python, from python.org, includes a large number
of libraries to support most activities.

When you run third-party python plugins, those plugins may require libraries
not included with the base python installation. If that's the case, you
can install them using PIP. [#F1]_

To simplify matters within X-Plane, XPPython3 provides an interface to PIP.

.. _pip-package-installer:

Package Installation Example
----------------------------

pytz is a simple package, not initially included. It handles easily handles timezones in Python. This
makes it a great "first example".

1) XPPython3 Menu, select "Pip Package Installer"
2) Install package ``pytz``. Type in the package name and press Install. It will take a few seconds (depends on
   your Internet speed.) Though this package is not required by XPPython3, but it's a harmless example if you're
   looking to try something.

If Pip fails because it cannot find an executable version of pip (most commonly on linux), send your log files to support
and we'll check it out.

.. image:: /images/pip_installer.png

Installing a package already installed is not a problem. You can install multiple packages at the same time.


If you're looking to distribute your plugin which requires a non-standard python package, you should take a look
at automating installation using the XPPython3 :py:mod:`utils.xp_pip` module.

----

.. [#F1] You'll install python libraries using PIP: you do not install X-Plane plugins using PIP.
