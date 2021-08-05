xp_imgui
========

To use::

  import xp_imgui

.. module:: xp_imgui

Class
-----

.. py:class:: Window(createWindowTuple):

 Return instance of xp_imgui.Window. Required parameter is a 14-element tuple, identical
 to that used with :func:`XPLMDisplay.XPLMCreateWindowEx`.

 :members: stop, imgui_context, windowID, imgui_context, io

 .. py:method:: delete() -> None
 .. py:method:: handleMouseClick() -> int
 .. py:method:: drawWindow()
 .. py:method:: hasInputFocus()
 .. py:method:: requestInputFocus()
 .. py:method:: handleRightClick()
 .. py:method:: handleCursor()
 .. py:method:: handleMouseWheel()
 .. py:method:: handleKey()
                
   
 .. py:method:: setTitle(title: str) -> None

    Set window title
