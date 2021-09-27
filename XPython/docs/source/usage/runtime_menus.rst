Runtime XPPython3 Menus
=======================

XPPython3 installs a single menu item, "XPPython3", on the plugin menu.

   .. image:: /images/xppython3menu.png

* **About** menu item further describes this plugin, including links to documentation
  and current version number. It will indicate when a new version is available (we *never* automatically
  update your version.)

  .. image:: /images/about.png

  * **User Documentation** opens a browser to :doc:`/usage/installation_plugin`.

  * **Plugin Development** also points here, but to the initial :doc:`/development/index` page.

  * **Support** takes you to the main XPPython3 page on https://forums.x-plane.org, where you can ask for help.

  * **Donate** allows you to gain karma points.

  * **Changelog** points to the current :doc:`/changelog`.

  .. Note:: If you're adventurous you can select "Include Betas" which will cause the plugin updater to look for
     the most recent beta software. Honestly, don't do this unless you're working directly with me, as
     god-only-knows what problems you may discover: *Beta* allows me to easily distribute a release to active
     developers around the world, but I don't always update for all platforms and versions.


* **Performance** menu item displays a list of all loaded XPPython3 plugins with some live performance statistics for each.

  .. image:: /images/perf.png
             
  You can use this to see how a particular plugin is impacting your overall performance. For details, see :doc:`performance`.


The next three items are:

* **Version** -- with an indication if you're using the most current version. If you're not,
  selecting this item will attempt to update your XPPython3 version to the latest.
  (It will not change Python versions, just XPPython3 plugin versions.) If successful, the new version
  is downloaded, but you'll need to restart X-Plane to have the new plugin take effect.

  Selecting this items will *always* attempt to update XPPython3, so even if the system thinks you have
  the most current version, selecting this menu item will cause the Updater to download XPPython3 again.
  This may be useful if you've messed up an installation: It *does not* change anything in your PythonPlugins
  folder.

  If all else fails, you can always re-download the zip file (mentioned at the top of this page) and replace
  your current XPPython3 folder with the contents of the zip file.

  Once a new version is downloaded and verified, you'll need to *restart* X-Plane to load the new plugin.
  
* **Download Samples** -- Refreshes python sample plugins by downloading the latest set from github and
  putting them in the ``PythonPlugins/samples`` subdirectory. Moving a sample plugin from there up into
  the ``PythonPlugins`` will enable it to be loaded by X-Plane. See :doc:`/development/samples`.
  
* **Pip Package Installer** -- *pip* is a python-based installer that is commonly used to install
  python packages (i.e., modules or script libraries). While Python comes with lots of built-in libraries,
  you may need to install some 3rd party libraries. Using this installer will make sure the
  libraries are installed into the proper version of python on your computer. This is *not* to be used
  to install X-Plane plugins. If your python plugin requires additional Python packages, your plugin should
  tell you which packages you'll need to install.

  You will most likely need OpenGL (which is a drawing package). For python, it is called ``pyopengl``. That's
  why we suggest you install it at your first opportunity. It's free.

Finally:

* **Reload** - Stops and restarts python, reloading all scripts.

  .. Caution:: While this works, note that some
    plugins do not clean up after themselves, so loading all python plugins may result in duplications. Note
    that "reloading" is really a developer tool and not particularly reliable, so use it if you can while
    you're developing new plugins, but otherwise restart X-Plane if you want to run a clean version. For example,
    it is a known problem that reloading XPPython3 after using the imgui module will crash the system.


.. toctree::
  :caption: Details
  :maxdepth: 1
             

  performance             
