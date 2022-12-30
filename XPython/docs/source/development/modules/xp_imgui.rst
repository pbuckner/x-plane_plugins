xp_imgui
========

.. module:: xp_imgui

Note this is not automatically included with the ``xp`` module, *and* since you'll
be making imgui calls, make sure you import that module as well. To use::

  from XPPython3 import xp_imgui
  import imgui

`Dear ImGui <https://github.com/ocornut/imgui>`_ is a rich third-party widget library
which allows you to add a wide-range of user-interactions very simply. You can
use it as a replacement for Laminar Widget user interface.

This module provides a bridge between X-Plane and Imgui library by supporting
your creation of an ImGui window. Within the window, you can make any ImGui calls.
We handle the mapping between X-Plane (graphics, keyboard, mouse-clicks) and ImGui.


Class
-----

.. py:class:: Window(...)

 Return instance of `xp_imgui.Window`. Parameters are similar to those used with
 Laminar's window creator :func:`xp.createWindowEx`, with the except that *only*
 a ``draw()`` callback is available.

  | **Parameters and defaults:**
  | *left=100, top=200, right=200, bottom=100,*
  | *visible=0,*
  | *draw=None,* 
  | *refCon=None*
  | *decoration=WindowDecorationRoundRectangle,*
  | *layer=WindowLayerFloatingWindows,*

 Parameter values have the same meaning as the Laminar function. The result is a creation
 of a "modern" Laminar window, with drawing set for ImGui. Additional instance members and methods
 are exposed.

 The key callback is for specifying and drawing the ImGui elements. Your callback function should provide:

 .. py:function:: myDrawCallback(windowID, refCon)

    ``windowID`` is the standard X-Plane windowID.

    ``refCon`` is the reference constant provide with window creation.
       
    We'll set the ImGui context so all your draw callback needs to do is make calls
    to imgui::

      >>> def drawWindow(windowID, refCon):
      ...    imgui.button("Click me")
      ...

See :doc:`/development/imgui` for additional details.

Window Objects
--------------

The following attributes and methods describe :py:class:`Window`'s public interface.
 
  .. py:attribute:: Window.windowID
     :type: XPLMWindowID

     The windowID created for the X-Plane window (internally created using :func:`createWindowEx`).
     This attribute allows you to call most standard XP SDK window functions using *this* value::

       >>> myWindow = xp_imgui.Window(...)
       >>> xp.setWindowTitle(myWindow.windowID, "New Window Title")
       >>>

  .. py:attribute:: Window.imgui_context
     :type: ImGuiContext instance

     The ImGui context created when the window is created, result of ``imgui.create_context()``

  .. py:attribute:: Window.io
     :type: ImGuiIO instance              

     The ImGui io created when the window is created, result of ``imgui.get_io()``

  .. py:method:: Window.delete() -> None

     Cleans up context and ultimately calls :func:`destroyWindow` with this window's windowID.
     Use this *instead of* calling ``xp.destroyWindow(myWindow.windowID)``.

Convenience methods. You can use these, or their equivalent X-Plane SDK alternative:

  .. py:method:: Window.setTitle(title: str) -> None

     Identical to ``xp.setWindowTitle(self.windowID, title)``

  .. py:method:: Window.hasInputFocus() -> boolean

     Identical to ``xp.hasKeyboardFocus(self.windowID)``

  .. py:method:: Window.requestInputFocus(req: boolean)

     Identical to ``xp.takeKeyboardFocus(self.windowID if req else None)``

Internal methods. These provide the translation to/from X-Plane and ImGui. You should not need
to call these directly. However, if you see incorrect behavior, view the source and suggest
and update to me!

  .. py:method:: Window.drawWindow(windowID, inRefCon) -> None


See :doc:`/development/imgui` for examples and details.
                 
