:orphan:
   
Other Versions of Python
========================

Want to use something other than the latest version of python? *Please don't.*

`XPPython3 Version 3.1.5 <https://xppython3.rtfd.io/en/3.1.5/>`_ supports older versions. It works
with X-Plane 12, but will not add XP12-specific features.

XPPython3 Version 4 has been tested with:

  + Windows: python310, python311
  + Mac: python310, python311
  + Ubuntu 22: python310

Regardless of the python version, you *must* install a compatible version of XPPython3 plugin (so why not use the most recent?)

The XPPython3 plugin version **must match** the version of python (3.10, 3.11) on your computer:
the plugin will not load if a compatible version of python is not correctly installed.
If you change python versions on you computer, you must change plugin version
to match. (You can have multiple versions of python on you computer: XPPython3 will look to match the correct version
of python.)

Any micro-release may be used for a particular major.minor release: For example, python 3.10.0 and 3.10.4 are both "3.10".
If you must use something other than the latest python, use a compatible version of the plugin:

  + For Python3.10: `xppython310.zip <https://github.com/pbuckner/x-plane_plugins/raw/master/XPython/Resources/plugins/xp310.zip>`_.
  + For Python3.11: `xppython311.zip <https://github.com/pbuckner/x-plane_plugins/raw/master/XPython/Resources/plugins/xp311.zip>`_.

.. Note::
   All operating systems will allow you to have multiple versions of python installed (a bold statement, I know).
   The hard part becomes setting `which` python is used when you merely invoke :code:`python`. XPPython3 does not
   care which is the `default` version of python on you system: It looks for, and loads the first version of
   python3 it can find in the standard locations, so if you've installed it from https://python.org it will be loaded.
