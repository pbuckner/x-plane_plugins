Tools
=====

More than just an API, XPPython3 includes tools to make it easier to write and debug X-Plane
plugins:

* **Mini Debugger** (See :doc:`debugger`.)

  The ``PI_MiniPython.py`` plugin, available under ``PythonPlugins/samples/``, allows you to
  type in python expressions in a running application and see the results: You'll need to move it
  from ``samples`` up into the ``PythonPlugins`` directory in order for it to load.

  .. image:: /images/MiniPython.gif
    :width: 50%

  Most of the XPPython3 API can be directly cut-and-pasted into the Mini Debugger. For example,
  :py:func:`xp.registerDrawCallback` documentation includes the following code::

    >>> def MyDraw(phase, after, refCon):
    ...    xp.setGraphicsState(numberTexUnits=1)
    ...    xp.drawString([.9, 0, 0], 110, 175, "Hello there")
    ...
    >>> xp.registerDrawCallback(MyDraw)
    1

  Click on the ``>>>`` in the upper right corner of the code box to get remove the displayed
  prompts::

    def MyDraw(phase, after, refCon):
       xp.setGraphicsState(numberTexUnits=1)
       xp.drawString([.9, 0, 0], 110, 175, "Hello there")

    xp.registerDrawCallback(MyDraw)

  Copy the above code and then paste it directly at the prompt in the textbox, and you'll see the results!

* **Plugin Performance** (See :doc:`/usage/performance`.)

  XPPython3 monitors the performance of individual python plugins and displays this information
  when you select the `XPPython3` -> `Performance` menu item.

* **XPGL Open GL Debugging Script**

  Stand-alone OpenGL pygame python script that allows you to experiment with OpenGL and :py:mod:`xpgl` module
  drawing routines using a graphics context similar to  X-Plane. (See :doc:`/development/modules/xpgl_debugging`)

* **Debug Config Initialization** (See :doc:`xppython3.ini`.)

  For debugging, we've added some run-time flags which can be configured and stored. These
  increase the level of internal debugging output, and increase the frequency of buffer flushing.
  Will slow down execution time a small bit, but may make your debugging easier.
  
* XPPythonGetDicts :doc:`xppythondicts`

* **Python Stubs**
   
  Because the XPLM* modules are contained within a shared library, they are not useful for support tools such as
  pylint. For this reason, we've included a set of ".pyi" stubs. See :doc:`stubs` for details.
   
* **Porting** from older plugin?

  Note that XPPython3 is backward compatible to the API, but you will need
  to make changes to support python3 vs. python2. That being said XPPython3 has a new simplified API which (though not
  backward compatible) will make new code easier to write.

  * :doc:`changesfromp2`
  * :doc:`portingNotes`

.. toctree::
   :hidden:

   debugger
   /usage/performance
   xppython3.ini
   xppythondicts
   stubs
   changesfromp2
   portingNotes
