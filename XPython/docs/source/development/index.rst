Plugin Development
==================

What is XPPython3
-----------------
XPPython3 is a standard X-Plane compiled plugin, which loads and executes python plugins. Messages sent by X-Plane to
plugins are received by XPPython3 which then forwards messages to each of the python plugins, translating native "C" language
data types into relevant python datatypes.

Function calls made from python plugins are handled by XPPython3, which translates python data types to C and then makes
the equivalent call into X-Plane using the X-Plane SDK. From X-Plane's perspective, XPPython3 is a single plugin: Only
XPPython3 knows about the individual python plugins.

    .. image:: /images/xppython3-overview.png

The XPPython3 programming interface matches the X-Plane C SDK interface, using the same concepts. So if you're
familiar with the SDK you'll find the python interface pretty easy. Similarly, you should be able to
translate examples from C into python (and vice-versa) without much effort. [#effort]_

Why use XPPython3?

* Python language
* Simplified API through the addition of keyword and default parameters
* Integrated python interpreter allowing you to literally cut-and-paste examples from this documentation into a
  running X-Plane.

Hello World
-----------

So, following the time-honored tradition of first program: here's an audio Hello World.

Put the following in a file called "PI_HelloWorld.py" (being mindful of leading spaces!) and place
that file in ``Resources/plugins/PythonPlugins``::

  import xp

  class PythonInterface:
      def XPluginStart(self):
          return "Hello World", "xppython3.hello", "Simple Hello World"

      def XPluginEnable(self):
          xp.speakString("Hello World")
          return 1

Make sure your audio is on and start X-Plane. While the XP is initializing, before the plane or location is loaded,
you'll hear "Hello World".

Your first plugin!

Now, while X-Plane continues to run, navigate to the Plugins menu, and select XPPython3->Performance. Here
you'll see (at least) three python plugins running::

  PythonPlugins.PI_HelloWorld
  XPPython3.I_PI_FirstTime
  XPPython3.I_PI_Updater

That's it. Your plugin is running. Exit X-Plane and take a look at ``<XP>/XPPython3Log.txt``, located
in the same folder as ``Log.txt``. It will have lines like::

  [XPPython3] Version 4.0.0 - for Python 3.11 Started -- Thu Dec  8 09:23:34 2022
  ...
  [XPPython3] PythonPlugins.PI_HelloWorld initialized.
  [XPPython3]  Name: Hello World
  [XPPython3]  Sig:  xppython3.hello
  [XPPython3]  Desc: Simple Hello World

Want to do something more interesting? Work through the next :doc:`quickstart` examples.

|
|

.. toctree::
   :maxdepth: 1
   :hidden:
   :caption: Development Resources:

   quickstart
   Python SDK Modules <modules/index>
   Graphics options <graphics>
   tools
   deployment
   implementation_details
   xlua_transition

----

.. [#effort]
   Except, of course, where python has vastly more programmer-friendly control and data structures, and
   built-in libraries which you'll have to deal with yourself in the cold C world. Python:
   `"Batteries included" <https://www.computer.org/csdl/magazine/cs/2007/03/c3007/13rRUzpQPH7>`_
