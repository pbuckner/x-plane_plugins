xpgl Debugging Script
=====================

To simplify experimentation and aid with debugging, you can use any of these functions in a stand-alone
python program. We've provided a pygame-based shell: You can edit your ``draw()`` function within this
shell and then execute it directly from the command line, independent from X-Plane.

Note, however, you'll need your own instance of Python (with OpenGL pygame and other support modules).
While it is possible to use the XPPython3-installed version of python, you would need to manipulate
python path to get it to locate everything you need.

Use the file ``xpgl_test.py`` located under ``samples``, and move it into your ``PythonPlugins`` folder.
From there, directly execute it. By default, it will open a window 500x500 and if you press the 'q' key
within the window, it will exit::

  $ python3.12 xpgl_test.py

The drawing canvas is in textuals, with (0,0) in the lower left corner, increasing up in the Y axis, and
to the right on the X axis. Upper right corner is *always* the screen's (width, height). If the canvas is
shrunk or enlarged, the number of textuals *do not change*, so a single textual may map to more or less than
a single physical display pixel.

There are a few things to customize within the file:

* **load()**: Function called once, before any drawing. This is ideally where you would
  load images or fonts. Within X-Plane, this tasks would likely be done during XPluginEnable().

* **draw()**: Function called repeatedly. This is whatever you want to draw. We've set up the
  OpenGL context to match how X-Plane sets it up before calling your callback (as best we
  I determine.)

Any data you need for your draw function you'll have to mock, perhaps by setting the global ``Data``
data structure. You cannot call dataref routines from this stub. We have mocked some :py:mod:`xp`
functions, so you can use them::

  xp.getCycleNumber() -> int
  xp.generateTextureNumbers(count: int) -> List[int]
  xp.bindTexture2d(textureID: int, textureUnit: int) -> None
  xp.setGraphicsState(...) -> None
  xp.log(str) -> None

We've also changed the working directory while in the script to match X-Plane root directory.
Therefore any calls you make to read files can use the same *relative* path as you'd use
in a real X-Plane installation.

As an enhanced demo, the provided example can toggle between displaying as a mask vs displaying as a stencil.
Press the 's' key while over the window to see the difference. The image on the left shows the mask, on the right
is the stencil.

.. image:: /images/xpgl_debugger1.png
           :width: 40%

.. image:: /images/xpgl_debugger2.png
           :width: 40%
                   
Also provided are two 'draw' functions and you can flip between them
by pressing the 'f' key while over the window. The first function is the triangle and circles (above). The
second drawing demonstrates rotations, fonts and textures (below). It is also influenced by the 's' stencil
toggle.

.. image:: /images/xpgl_debugger3.gif
           :width: 40%

.. image:: /images/xpgl_debugger4.gif
           :width: 40%

Extensive documentation is included in the file.
