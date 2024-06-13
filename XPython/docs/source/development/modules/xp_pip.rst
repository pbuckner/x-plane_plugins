xp_pip
======

.. module:: utils.xp_pip

To use::

  from XPPython3.utils import xp_pip

You may need to install additional python packages into the python instance included with XPPython3.

.. note:: You do not need to re-install packages with new versions of XPPython3 or new versions of
          your plugin. Packages are installed as 'site-packages'.

There are a few different approaches you can take:

*  **Tell the user to use pip**: Users can always use the menu item "Pip Package Installer" as
   described in :doc:`/usage/pip`. Your installation instructions can reference that URL and guide the user.
   The benefit is there is no additional work for you. The disadvantage is you're relying on
   the user's ability to follow instructions.

*  **Execute python during installation**: As part of *your* installation process, you can locate
   and execute the custom version of python installed with XPPython3. By using this version
   of python, pip will install into the correction location, as used by XPPython3. The benefit
   is there is no additional effort to the user. The disadvantage is you have to do (most of) the
   work. See :ref:`call_python_directly` below.
   
* **Execute pip in-game**: You can programmatically invoke pip within X-Plane, perhaps as
  part of your initial installation. This does the same thing as if the user selected the
  menu item, but you can control if / when it is executed. The advantage (or perhaps disadvantage) is that
  it shows the user something is being downloaded and installed on their computer, and the results
  are logged. The disadvantage is that it requires a minor bit of coding on your part. See :ref:`call_pip_async`.

* **Execute pip in-game synchronously**: You can programmatically invoke pip within X-Plane, stopping
  other execution while it completes. Essentially, you're just spawning a subprocess to do the work and
  *waiting* until it is finished. The advantage is that you can do it at any time and continue with execution
  of your other plugin code, with needing to restart X-Plane. The disadvantage is coding on your part and
  a potential few seconds of inactivity of X-Plane. Progress is hidden from the user. See :ref:`call_pip_sync`.

My recommendation? If at all possible, either tell the user to use pip (the first option), or "manually" execute
pip during your plugin's execution (the second option). These require the least amount of additional coding and
testing on your part.


.. _call_python_directly: 

Call Python Directly
--------------------

To call python (and pip) directly, explicitly call the custom version of python. This differs
for the different platforms, and its location is relative the X-Plane root (or installed version
of XPPython3.)

* Mac::

    ./Resources/plugins/XPPython3/mac_x64/python3.12/Resources/Python.app/Contents/MacOS/Python -s -m pip install <packages>

* PC::

    ./Resources/plugins/XPPython3/win_x64/python.exe -s -m pip install <packages>

  (Note ``python.exe`` *not* ``pythonw.exe``)
    
* Linux::

    ./Resources/plugins/XPPython3/lin_x64/python3.12/bin/python3.12 -s -m pip install <packages>
  
.. _call_pip_async:

Calling Pip with Popup
----------------------

Invoking pip in this manner will result in a popup window informing the user what is being done, and
logs the result in the python log file.

.. py:function:: load_packages(packages=None, start_message=None, end_message=None) -> bool

  :param list | str packages: *list* of packages names. You can pass a *single package* as a str and we'll convert
  :param str start_message: Optional message displayed as first line(s) of output
  :param str end_message: Optional message displayed as last line(s) of output
  :return: True *if there are packages to install*. Return does not indicate success/failure of installation

  The *start_message* defaults to nothing. It may be more friendly to tell the user
  what you're doing. E.g., "Installing additional packages required for <My App>".

  The *end_message*, if not provided, will default to the string, "This information has been added to XPPython3 log file."
  If you really don't want a message, set it to ``""``.

  For either *start_message* or *end_message*, if you provide strings with newlines ``\n``, they will
  properly display as multiple lines in the output.

  During installation, a scrolling popup will be displayed with installation progress (output from pip).
  After installation, the popup message box will *still be displayed* and the user should close it.

  .. image:: /images/pip_output.png
     :width: 500px

  This is *asynchronous*, which means there is no easy way for your (other) code to know if/when it
  has completed. Therefore, it is best suited for a "install-and-restart" type of installation routine.

  You might code it as::

    missing_modules = []
    try:
        import non_standard_module
    except ModuleNotFoundError:
        missing_modules.append('non-standard-module-name')
    try:
        import other_module
    except ModuleNotFoundError:
        missing_modules.append('py-other')
  
    class XPluginInterface:
        ...
  
        def XPluginEnable(self):
            if missing_modules:
                 xp_pip.load_packages(missing_modules,
                                      "Loading missing modules",
                                      "Modules loaded.\nCheck for errors, and RESTART X-Plane.")
                 return 0  # to disable the plugin
            ...
            return 1
  
  Depending on how the rest of your code is structured, you *may* be able to simply have the user
  select *Reload* from the XPPython3 menu to complete execution.
  
.. py:function:: load_requirements(requirements=None, force=False, start_message=None, end_message=None) -> bool

  :param list | str requirements: List of packages or requirements. You can pass *single package* as a str and we'll convert
  :param bool force: By default, install *only* if a package is missing. If True, always attempt install.
  :param str start_message: Optional message displayed as first line(s) of output
  :param str end_message: Optional message displayed as last line(s) of output
  :return: True *if there are packages to install*. Return does not indicate success/failure of installation

  The function works similarly to :py:func:`load_packages`.
  It can take a single string, or a list of strings, each being a *package name* (not module name), optionally with
  a version requirement. Some examples::

    requirements = 'PyOpenGL'
    requirements = ['PyOpenGL', 'requests']
    requirements = ['PyOpengl==3.17', 'numpy>=1.26', '#pygment']

  Note not all variants of pip's requirements.txt have been tested (such as embedded ``-r`` or ``-c`` references or
  explicit URLs). Packages starting with ``#`` are ignored.

  *First* we do a simple, synchronous check to see if the package is already installed (ignoring
  any specified version). If *all* packages are already installed, no further action is taken and False
  is returned. This permits you to check at startup and do nothing if packages are already installed.
    
  If *any* package does not appear to be installed, we call pip asynchronously (like :py:func:`load_packages`) with
  the full set of requirements and pop up a window.::

    requirements = ('Pillow', 'py-freetype')
    xp_pop.load_requirements(requirements)

  If *force=True*, we skip the initial check and simply start pip with the requirements.
    
  *start_message* defaults to "Checking required packages", and *end_message* defaults
  to "This information has been added to XPPython3 log file."
    
  .. image:: /images/xp_pip_requirements.png
               :width: 500px

.. _call_pip_sync:

Calling Pip Synchronously
-------------------------

Calling pip synchronously is done by directly using python module ``subprocess``. On success, you
can re-import the module into the global space. Example code follows for fictional module ``DWIM_magic``.

Similar to previous example, you might attempt to load the module at startup. Note that often the
*package name* (i.e., the thing you install using pip) differs from the resulting *module name* (the thing
you import with python)::

  missing_modules = []
  try:
      import DWIM_magic
  except ModuleNotFoundError:
      missing_modules.append(['DWIM-magic', 'DWIM_magic')

Then, during ``XPluginEnable()`` verify module installation, for example::

  def XPluginEnable(self):
      if missing_modules and tryLoadModules(missing_modules):
          xp.log(f"Missing {missing_modules}")
          return 0
      ...
      return 1

Your code to try and load the missing module might look something like::

  import importlib

  def tryLoadModules(modules):
    cmd = [xp.pythonExecutable, '-m', 'pip', '-s', install'] + [x[0] for x in modules]
    xp.log(f"Calling pip as: {' '.join(cmd)}")
    try:
        output = subprocess.check_output(cmd, stderr=subprocess.STDOUT)
        xp.log(output.decode('utf-8'))
    except subprocess.CalledProcessError as e:
        xp.log("Fail: Calling pip failed: [{}]: {}".format(e.returncode, e.output.decode('utf-8')))
        return False

    for module in [x[1] for x in modules]:
        try:
            importlib.import_module(module)
        except ModuleNotFoundError as e:
            xp.log(f"Fail: Cannot load package for {module}: {e}")
            return False
    importlib.invalidate_caches()
    return True

Errors may be a bit harder to diagnose.
