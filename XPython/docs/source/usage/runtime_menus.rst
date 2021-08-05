Runtime XPPython3 Menus
=======================

XPPython3 installs a single menu item, "XPPython3", on the plugin menu. From there, you can:

* **Disable** - disables all scripts.
* **Enable** - only useful if you've disabled the scripts.
* **Reload** - Stops and restarts python, reloading all scripts. **Caution:** While this works, note that some
  plugins do not clean up after themselves, so loading all python plugins may result in duplications. Note
  that "reloading" is really a developer tool and not particularly reliable, so use it if you can while
  you're developing new plugins, but otherwise restart X-Plane if you want to run a clean version. For example,
  it is a known problem that reloading XPPython3 after using the imgui module will crash the system.

There is also an **About** menu item which further describes this plugin, including links to documentation
and current version number.

The XPPython3 menu includes two final items:

* **Version** -- with an indication if you're using the most current version. If you're not using the
  most current version, selecting this item will attempt to update your XPPython3 version to the latest.
  (It will not change Python versions, just XPPython3 plugin versions.) If successful, the new version
  is downloaded, but you'll need to restart X-Plane to have the new plugin take effect.

  Selecting this items will *always* attempt to update XPPython3, so even if the system thinks you have
  the most current version, selecting this menu item will cause the Updater to download XPPython3 again.
  This may be useful if you've messed up an installation: It *does not* change anything in your PythonPlugins
  folder.

  If all else fails, you can always re-download the zip file (mentioned at the top of this page) and replace
  your current XPPython3 folder with the contents of the zip file.
  
* **Pip Package Installer** -- *pip* is a python-based installer that is commonly used to install
  python packages (i.e., modules or script libraries). While Python comes with lots of built-in libraries,
  you may need to install some 3rd party libraries. Using this installer will make sure the
  libraries are installed into the proper version of python on your computer. This is *not* to be used
  to install X-Plane plugins. If your python plugin requires additional Python packages, your plugin should
  tell you which packages you'll need to install.

  You will most likely need OpenGL (which is a drawing package). For python, it is called ``pyopengl``. That's
  why we suggest you install it at your first opportunity. It's free.
