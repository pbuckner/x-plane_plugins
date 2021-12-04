Other Versions of Python
========================

Want to use something other than the latest version of python? Please don't.

You can, if you know what you're doing, by why?

XPPython3 has been tested with:

  + python37, python38, python39 on windows,
  + python37, python38, python39, python310 on Mac,
  + ubuntu 18 (python36, python37, python38), ubuntu 20 (python38)

Regardless of the python version, you *must* install a compatible version of XPPython3 plugin (so why not use the most recent?)

The plugin version **must match** the version of python (3.6, 3.7, 3.8, 3.9, 3.10) you computer is
running: the plugin will not load if python is not correctly installed, or if the
version does not match. If you change python versions on you computer, you must change plugin version
to match. Any micro-release may be used for a particular major.minor release: For example, python 3.7.0 and 3.7.3 are both "3.7".
If you must use something other than the latest python, use a compatible version of the plugin:

  + For Python3.6: `xppython36.zip <https://github.com/pbuckner/x-plane_plugins/raw/master/XPython/Resources/plugins/xppython36.zip>`_.
  + For Python3.7: `xppython37.zip <https://github.com/pbuckner/x-plane_plugins/raw/master/XPython/Resources/plugins/xppython37.zip>`_.
  + For Python3.8: `xppython38.zip <https://github.com/pbuckner/x-plane_plugins/raw/master/XPython/Resources/plugins/xppython38.zip>`_.
  + For Python3.9: `xppython39.zip <https://github.com/pbuckner/x-plane_plugins/raw/master/XPython/Resources/plugins/xppython39.zip>`_.
  + For Python3.10: `xppython310.zip <https://github.com/pbuckner/x-plane_plugins/raw/master/XPython/Resources/plugins/xppython310.zip>`_.

.. Note::
   All operating systems will allow you to have multiple versions of python installed (a bold statement, I know).
   The hard part becomes setting `which` python is used when you merely invoke :code:`python`. XPPython3 does not
   care which is the `default` version of python on you system: It looks for, and loads the first version of
   python3 it can find in the standard locations, so if you've installed it from https://python.org it will be loaded.
