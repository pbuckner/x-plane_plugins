ImGui Coding
------------
**ImGui** is an open-source C++ library. As of December 2022, imgui is on version 1.89.1.

**pyimgui** is an open source python wrapper to *some* of the features of the C++ library. It
wraps an older version of imgui, version 1.65. (pyimgui is version 1.4.1, which generally supports imgui 1.65).

XPPython3 has taken a development copy of pyimgui v2.0.0 and is providing it for use with X-Plane, this adds a lot
of new features (moving imgui from 1.65 to 1.82), though doesn't quite match the latest version of imgui (v1.89.1).

Even so, this is more than adequate for implementing
widgets to interact with the user: more complicated drawing is probably not going to work. Some of the
features in the C++ library are not (yet) supported by pyimgui.

There are two ways to see what this version of imgui can do:

#. Modify the provided sample in PI_imgui.py, and run that within X-Plane

#. Run standalone `pyglet <https://pyglet.readthedocs.io/en/latest/>`_ based GUI, where
   you can try out the widgets without running all of X-Plane.

ShowDemoWindow
..............

.. image:: /images/pyglet.png     
           :align: right
           :scale: 50%

Either way, imgui comes with a great demonstration. In C++ it is ``ImGui::ShowDemoWindow()``.
You can see this by calling the pyimgui method ``show_demo_window()``. Helpful to see what's
available, but not very helpful from a programming standpoint
because ``show_demo_window()`` merely calls the C++ method.

You can copy & paste this directly into the :doc:`/development/debugger`::

 >>> from XPPython3 import xp_imgui
 >>> from XPPython3 import imgui
 >>> def drawWindow(windowID, refCon):
 ...     imgui.show_demo_window()
 ...
 >>> window = xp_imgui.Window(draw=drawWindow, visible=1)


However, we've also included the file ``testwindow.py`` under ``XPPython3/imgui`` which
is work-in-progress rewriting the C++ method fully in python [#F1]_. Look at the python file to see
the actual set of pyimgui calls you can make (e.g., ``imgui.radio_button``, ``imgui.slider_float``).
refer to `pyimgui's documentation <https://pyimgui.readthedocs.io/en/latest/index.html>`_, especially
`pyimgui.core <https://pyimgui.readthedocs.io/en/latest/reference/imgui.core.html>`_.

 >>> from XPPython3 import xp_imgui
 >>> from XPPython3 import imgui
 >>> from XPPython3.imgui import testwindow
 >>> def drawWindow(windowID, refCon):
 ...     testwindow.show_test_window()
 ...
 >>> window = xp_imgui.Window(draw=drawWindow, visible=1)


ImGui Standalone
................

To "play" with imgui in a python environment, without having to fire up X-Plane,
You can use a different application environment to drive ImGui.

To run standalone, do this:

#. Get python3 running
#. Install python modules ``pyopengl``, ``pyglet`` ::

     $ python3 -m pip install pyopengl pyglet

#. Run sample ``imgui_pyglet.py`` from ``Resources/plugins/PythonPlugins/samples``,
   picking up pyimgui from ``Resources/plugins/XPPython3``::

     $ cd Resources/plugins/PythonPlugins/samples
     $ PYTHONPATH=<XP>/Resources/plugins/XPPython3
     $ export PYTHONPATH
     $ python3 imgui_pyglet.py

You can make changes within imgui_pyglet, updating the ``update()`` function to experiment
with different ImGui functionaliy.

.. image:: /images/imgui_plot.gif

----

.. [#F1] Normally, this test file is shipped as part of the imgui python distribution. I've included
         it here for convenience, and corrected a few minor bugs.
