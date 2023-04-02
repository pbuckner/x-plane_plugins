:orphan:

Upgrading XPPython3
===================

Normally, once you have installed XPPython3, it will automatically inform you if/when
a new version is available by adding a white dot next to the XPPython3 Menu. Then,
select the "Update to..." menu item to start the update.

   .. image:: /images/xppython3menu.png

Upgrading to XPPython3 version 4 *automatically* is possible only if you've already
installed v3.1.5 on X-Plane 12 *and* you're using Python v3.10.x.

If:

+-----------------------------------------------+----------------------------------+
|You're using X-Plane 11                        |Don't upgrade. XPPython3 v4 does  |
|                                               |not support X-Plane 11.           |
+-----------------------------------------------+----------------------------------+
|You're using X-Plane 12, but have not installed|Just follow version 4             |
|XPPython3                                      |:doc:`installation_plugin`.       |
|                                               |                                  |
+-----------------------------------------------+----------------------------------+
|You're using X-Plane 12 with XPPython3 v3.1.5, |You need to *delete* your current |
|with Python versions less that v3.10           |XPPython3 folder (not your        |
|                                               |PythonPlugins!) and follow        |
|                                               |:doc:`installation_plugin`. In the|
|                                               |process, you'll need to download  |
|                                               |and install a newer version of    |
|                                               |python. (This version only        |
|                                               |supports Python 3.10+.)           |
|                                               |                                  |
+-----------------------------------------------+----------------------------------+
|You're using X-Plane 12 with XPPython3 v3.1.5, |Congratulations, XPPython3 will   |
|with Python version 3.10.x.                    |inform you a new version is       |
|                                               |ready. You can upgrade by         |
|                                               |selecting the XPPython3 menu item |
|                                               |to upgrade.                       |
+-----------------------------------------------+----------------------------------+
  
