Using ImGui
===========

XPPython3 supports **ImGui** ("Immediate Mode GUI"), the `Dear IMGUI <https://github.com/ocornut/imgui>`_ library,
which is wrapped by the `pyimgui module <https://pyimgui.readthedocs.io/en/latest/guide/first-steps.html>`_ [#F1]_.

If you're not familiar with **ImGui**, read about the C++ library, and the python module using the links above.

.. note:: Earlier versions of XPPython3  provided a developer version of python imgui wrapper
          as part of standard XPPython3 installation in XPPython3/imgui folder. This is no longer
          required, now that pyimgui version 2.0.0 has been made generally available, we've included
          that version with the (included) version of python. If your plugin had previously
          imported this library from XPPython3 packages, you need to change::

            from XPPython3 import imgui

          change to::

            import imgui


Test Plugin
-----------
We include a sample plugin which uses imgui: ``PI_imgui.py``, which can be found under ``PythonPlugins/samples/``. (:doc:`samples`)

.. code-block:: none

  <X-Plane>/
  └─── Resources/
       └─── plugins/
            ├─── XPPython3/
            └─── PythonPlugins/
                 ├─── PI_imgui.py  ❮══════╗
                 └─── samples/            ║
                      └─── PI_imgui.py ❯══╝

Copy that file
into ``PythonPlugins/`` and restart X-Plane. You'll see a new menu item under Plugins, and each time you select it, we'll
display another copy of the popup window.

.. image:: /images/imgui.png

Next, :doc:`imgui_plugin`

|
|
|

.. toctree::
   :hidden:

   imgui_plugin
   imgui_coding
   imgui_advanced

----

.. [#F1] You'll need at least version 2.0.0 of pyimgui. Earlier versions of the python module had errors making them
         incompatible with X-Plane.

