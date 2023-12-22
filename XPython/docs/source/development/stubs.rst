XPPython3 Stubs
---------------

Because core XPPython3 code is embedded in a compiled library and cannot
be executed outside of a running X-Plane instance, it is more difficult
to code and error check python plugins than coding standalone python applications.

By using stubs, we can "fake" out editors and `linters <https://en.wikipedia.org/wiki/Lint_(software)>`_, by
using stub files: python code which provides function signatures but which are
not suitable for execution. This is commonly done using ``*.pyi`` files.

XPPython3 has a fully annotated set of stub files included with the basic installation. (Prior to XPPython3 v4.1.2
these needed to be separately downloaded and installed.)

The result depends on your tools, but you may get hinting as well as static type-checking. For example with
Microsoft Visual Studio Code:

 .. image:: /images/hinting.png

To use:

* Include your:
    | ``Resources/plugins``,
    | ``Resources/plugins/XPPython3``, and possibly your
    | ``Resources/plugins/PythonPlugins``

  directories, as part of your python path during development.
  This will to pick up ``xp.py``, and all of the ``*.pyi`` files.

* Configuring your development environment will, of course, depend on which development environment / tools you
  use. Here are some examples. Though, *please*, check the official documentation of the correct
  version of your development environment!

Assume my installation is:

  Plugin in: ``/Volumes/Monster/X-Plane/Resources/plugins/XPPython3``

For

  +----------------+---------------------------------------------------------------------------------+
  | pylint         |Update ``~/.pylintrc`` file, adding something like::                             |
  |                |                                                                                 |
  |                |  [MASTER]                                                                       |
  |                |  init-hook="import sys;sys.path.extend(['.',                                    |
  |                |    '/Volumes/Monster/X-Plane/Resources/plugins',                                |
  |                |    '/Volumes/Monster/X-Plane/Resources/plugins/XPPython3'])"                    |
  |                |                                                                                 |
  |                |Note: ``init-hook`` must be on a single line, I've added newlines for            |
  |                |readability.                                                                     |
  +----------------+---------------------------------------------------------------------------------+
  | mypy           |Update ``~/.mypy.ini`` adding the path, like::                                   |
  |                |                                                                                 |
  |                |  [mypy]                                                                         |
  |                |  mypy_path = .:                                                                 |
  |                |    /Volumes/Monster/X-Plane/Resources/plugins:                                  |
  |                |    /Volumes/Monster/X-Plane/Resources/plugins/XPPython3                         |
  |                |  check_untyped_defs = True                                                      |
  |                |                                                                                 |
  |                |  [mypy-OpenGL.*]                                                                |
  |                |  ignore_missing_imports = True                                                  |
  |                |                                                                                 |
  |                |Note: ``mypy_path`` must be on a single line, I've added newlines for            |
  |                |readability.                                                                     |
  +----------------+---------------------------------------------------------------------------------+
  | MS Visual      |Add paths to ``python.analysis.extraPaths``. For example, your ``settings.json`` |
  | Studio Code    |might look like::                                                                |
  |                |                                                                                 |
  |                | {                                                                               |
  |                |    "python.defaultInterpreterPath": "/usr/local/bin/python3",                   |
  |                |    "python.analysis.extraPaths": [                                              |
  |                |      ".",                                                                       |
  |                |      "/Volumes/Monster/X-Plane/Resources/plugins",                              |
  |                |      "/Volumes/Monster/X-Plane/Resources/plugins/XPPython3"                     |
  |                |     ]                                                                           |
  |                | }                                                                               |
  |                |                                                                                 |
  +----------------+---------------------------------------------------------------------------------+
  | PyCharm        |Add paths to your "Interpreter path". From Preferences select "Python            |
  |                |Interpreter".                                                                    |
  |                |                                                                                 |
  |                |1) Select the Cog item in the upper right, next to your selected python          |
  |                |   version, a select "Show All...".                                              |
  |                |                                                                                 |
  |                |2) In the popup, select your python version, which will cause the five icons at  |
  |                |   the top of the popup to become enabled. Select the right-most "Folders"       |
  |                |   icon.                                                                         |
  |                |                                                                                 |
  |                |3) Another popup is displayed. Use the '+' to add the necessary paths.           |
  |                |                                                                                 |
  |                |.. image:: /images/pycharm.png                                                   |
  |                |                                                                                 |
  |                |It appears you may only need to add the XPPython3 directory as a Content Root    |
  |                |associated with your Project's Structure. I don't use PyCharm so I cannot speak  |
  |                |to the best way of configurating it.                                             |
  +----------------+---------------------------------------------------------------------------------+

  
