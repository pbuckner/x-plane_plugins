Using ImGui
===========

XPPython3 supports **ImGui** ("Immediate Mode GUI"), the `Dear IMGUI <https://github.com/ocornut/imgui>`_ library,
which is wrapped by the `pyimgui module <https://pyimgui.readthedocs.io/en/latest/guide/first-steps.html>`_ [#F1]_.

If you're not familiar with **ImGui**, read about the C++ library, and the python module using the links above.

.. note:: To support XPPython3 and X-Plane, we've provided a developer version of python imgui wrapper
          as part of standard XPPython3 installation in XPPython3/imgui folder. You will not need to install
          the pyimgui package: You *will* need to include that version of imgui in your code::

            from XPPython3 import imgui

Installation
------------
Our version of imgui requires OpenGL, so everyone (you and all users of your plugin) will need to install
OpenGL. See :doc:`/development/opengl` for details.

There is nothing else you need to install.

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

.. [#F1] Well, kind-of. The generally available pyimgui package is version 1.4.1 which supports imgui librarion v. 1.65.
         It has bugs making it incompatible with X-Plane. Fortunately, XPPython3 includes a copy of the developer
         version of pyimgui 2.0.0 which sucpports imgui library v. 1.82.

