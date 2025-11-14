Deploying your Plugin
=====================

There are a few different ways you can package your plugin to make it available
to other users. Actual distribution of your plugin (e.g., posting a copy at ``forums.x-plane.org``)
is not covered here.

You should *never* distribute a copy of XPPython3. Please refer users to
the `Plugin Installation <https://xppython3.readthedocs.io/en/latest/usage/installation_plugin.html>`_ page on this
website so they
can get the latest version of software with full installation instructions. If they have problems
installing XPPython3, refer them to me (``pbuck@xppython3.org``) or tell me what the issues are
so I can improve the experience for everyone.

* Full source (described below)

* :doc:`deployment_compiled`: Deploy \*.pyc files instead of python source.

* :doc:`deployment_encrypted`: Deploy \*.xpyce files which require a key to decrypt.

* :doc:`deployment_sharedlib`: Deploy platform-specific shared library (i.e., compiled C code).
  
Full Source
-----------

Providing your plugin as python source is the simplest and most common way of distributing
an XPPython3 plugin.

This should include a *single* file of the form ``PI_<my_fine_plugin>.py``, where *my_fine_plugin* is a
unique name for your plugin: there is no registry for plugin names. If you need to distribute
additional files such as images, configuration, or extra python code, these *should* all
be located under a single directory with the same name as your plugin, e.g. ``my_fine_plugin/``.

This makes future updates easier as the user would only need to replace the PI*.py file and the single
sub-directory. Similarly, by keeping all your extra files under the same directory, you'll reduce
the chance of naming conflicts.

.. toctree::
   :maxdepth: 1
   :hidden:
   :caption: Deployment Methods:

   deployment_compiled
   deployment_encrypted
   deployment_sharedlib
