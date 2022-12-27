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

OpenGL is a common package used by plugins, but not initially included. This
makes it a great "first example".

1) XPPython3 Menu, select "Pip Package Installer"
2) Install package ``pyopengl``. Type in the package name and press Install. It will take a few seconds (depends on
   your Internet speed.) Though this package is not required by XPPython3, it is commonly used by plugins, so you might
   as well install it now.

If Pip fails because it cannot find pip (most commonly on linux) make sure you've manually added the python3-pip package.

.. image:: /images/pip_installer.png

Installing a package already installed is not a problem. You can install multiple packages at the same time.

----

.. [#F1] You'll install python libraries using PIP: you do not install X-Plane plugins using PIP.
