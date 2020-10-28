Plugin Development
==================

To develop python3 plugins, you'll need to have the XPPython3 plugin installed first (and python3, of course).

Output from python :code:`print()` statements, python coding errors and exceptions will appear
in :code:`XPPython3.log`.

.. Note::
 Errors in some calls to XP interface don't cause exceptions but
 rather just terminate the plugin's method (e.g., terminate the flightloop) with no further
 message. These are hard to debug, but adding print statements to you plugin to verify statement
 execution will help.

If you write bad python code, or use Python2 rather than Python3 syntax, you'll see the exception
in XPPython3.log.

Documentation
-------------

You will find these resources helpful:

* XPPython3 Module Documentation

  This site, under :doc:`modules/index`.

  Also, check out the `Simplified Python Interface in module xp <modules/xp.html>`_.  You'll find it much faster and easier to write new
  code using this interface and won't wear out your XPLM keys.

  We've attempted to fully document the API here, noting changes from the official C-Language SDK. (Don't forget to use the `Search`
  field in the header of each page to find what you're looking for!)
  
  * `X-Plane Developer Documentation @ developer.x-plane.com <https://developer.x-plane.com/sdk/plugin-sdk-documents/>`_

    X-Plane's documentation is for C-language, and most of that has been translated to python and documented
    with XPPython3. However, when in doubt, check the Laminar docs.

* Additional XPPython3 Documentation:

  * :doc:`/changelog`. Lists recent changes to this SDK.

  * :doc:`import`. Information about python packages and proper way to import.
    
  * :doc:`plugins`: Though most plugins are Global Plugins, you can code Aircraft and Scenery plugins.

Tools
-----

* Python Stubs

     Download from github: `stubs.zip <https://github.com/pbuckner/x-plane_plugins/raw/master/XPython/stubs.zip>`_.
   
     Because the XPLM* modules are contained within a shared library, they are not useful for support tools such as
     pylint. For this reason, we've included a set of stubs.
   
     Include the stub directory as part of your python path (during development) and pylint will be useful. If you
     use ``xp.py`` be sure to include your ``Resources/plugins/XPPython3`` directory.
   
     For example, include this in :code:`~/.pylintrc`:
   
     .. code-block:: text
   
         [MASTER]                                                                                    
         init-hook="import sys;sys.path.extend(['.', '/path_to_stubs/stubs', 'XP/Resources/plugins/XPPython3'])"
   
     .. Note:: Do not place stubs in XPPython3 or PythonPlugins folder where they will be found by
               X-Plane! The stubs do not actually execute code.
               
* Mini Debugger
    See :doc:`debugger`. The ``PI_MiniPython.py`` plugin, available under ``demos``, allows you to
    type in python expressions in a running application and see the results.

Code Examples
-------------

* :doc:`quickstart`.  Bare-bones skeleton and Hello World to get you started.

* `XPython/demos/ <https://github.com/pbuckner/x-plane_plugins/raw/master/XPython/demos/>`_.   Ported versions of C++ and Python2 demos.

* `XPython/examples/ <https://github.com/pbuckner/x-plane_plugins/raw/master/XPython/examples/>`_ Python test code exercising each interface
  is in this directory, organized primarily one example file per module.
  You can copy them into your :code:`Resources/plugins/PythonPlugins` directory to have them executed by X-Plane with
  few modifications.

  Most of these examples where used to exercise specific aspects of the API rather than provide useful, instructive examples.
  See `XPython/demos/ <https://github.com/pbuckner/x-plane_plugins/raw/master/XPython/demos/>`_
  if you're looking for something more helpful.

Advanced Topics
---------------

* Porting from older plugin?

  Note that XPPython3 is backward compatible to the API, but you will need
  to make changes to support python3 vs. python2. That being said XPPython3 has a new simplified API which (though not
  backward compatible) will make new code easier to write.

  * :doc:`changesfromp2`
  * :doc:`portingNotes`
  * `NOAA Weather Plugin <https://github.com/pbuckner/XplaneNoaaWeather/>`_

    **Unofficial** port of Joan's NOAWeather plugin.
    Use git-compare to see what types of things change from python2 to python3. (Joan is familiar with the changes
    and will be responsible for official support in the future -- this is merely a porting example.)



* Need encryption? See :doc:`xpyce`. For security / privacy reasons, you can now distribute Plugins which contain encrypted modules.



.. toctree::
   :maxdepth: 1
   :caption: Contents:

   import
   callbacks
   quickstart
   changesfromp2
   debugger
   plugins
   menus
   xpyce
   portingNotes
   modules/index
   xppythondicts
