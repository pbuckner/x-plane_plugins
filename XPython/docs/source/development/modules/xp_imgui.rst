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
  | *contentType=WindowContentTypeOpenGL,*

 Parameter values have the same meaning as the Laminar function. The result is a creation
 of a "modern" Laminar window, with drawing set for ImGui.

 .. note:: By default, you can use OpenGL in the ``draw()`` function in addition to ImGui, to
           draw shapes and textures.

           Set contentType to :data:`xp.WindowContentTypePanelGraphics` for greater performance
           and the ability to use :doc:`panelgraphics` instead of OpenGL.

 Additional instance members and methods
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

     The windowID created for the X-Plane window (internally created using :func:`xp.createWindowEx`).
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

     Cleans up context and ultimately calls :func:`xp.destroyWindow` with this window's windowID.
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
an update to me!

  .. py:method:: Window.drawWindow(windowID, inRefCon) -> None


See :doc:`/development/imgui` for examples and details.

AvionicsDevice
--------------

.. py:class:: AvionicsDevice(...)

 Return instance of `xp_imgui.AvionicsDevice`: ImGui drawn onto the screen of a
 cockpit device, rather than into a floating window. Parameters are similar to
 those used with :func:`xp.createAvionicsEx`.

  | **Parameters and defaults:**
  | *screenWidth=400, screenHeight=300,*
  | *bezelWidth=None, bezelHeight=None,*
  | *screenOffsetX=None, screenOffsetY=None,*
  | *draw=None,*
  | *refCon=None,*
  | *bezelDraw=None,*
  | *brightness=None,*
  | *deviceID='xppython3.imgui.device',*
  | *deviceName='imgui device',*
  | *bezelPad=20,*
  | *contentType=WindowContentTypePanelGraphics*

 The bezel defaults to the screen plus *bezelPad* on each side, with the screen
 centered in it; set *bezelWidth* / *bezelHeight* / *screenOffsetX* /
 *screenOffsetY* to place it yourself.

 .. note:: *contentType* must be :data:`xp.WindowContentTypePanelGraphics`, and any
           other value raises ``ValueError``. The OpenGL renderer requires a
           windowID and window geometry, which a device does not have.

 Your ``draw()`` callback provides the ImGui elements for the device **screen**:

 .. py:function:: myDrawCallback(avionicsID, refCon)

    ``avionicsID`` is the standard X-Plane :class:`xp.XPLMAvionicsID`.

    ``refCon`` is the reference constant provided at device creation::

      >>> def drawScreen(avionicsID, refCon):
      ...    imgui.button("Click me")
      ...

 The *bezel* is not ImGui. Your optional ``bezelDraw(r, g, b, refCon)`` callback
 draws it with plain :doc:`panelgraphics` calls, and is called only while the
 pop-up is visible; *r*, *g*, *b* are the ambient tint X-Plane wants it modulated
 by. The optional ``brightness(rheoValue, ambientBrightness, busVoltsRatio,
 refCon)`` callback returns the screen brightness, defaulting to full --- note a
 device bound to a 3-D object reports ``busVoltsRatio = -1``, as it is not on the
 aircraft bus.

AvionicsDevice Objects
----------------------

The following attributes and methods describe :py:class:`AvionicsDevice`'s public
interface. ``imgui_context`` and ``io`` are as described for :py:class:`Window`.

  .. py:attribute:: AvionicsDevice.avionicsID
     :type: XPLMAvionicsID

     The device created for you (internally using :func:`xp.createAvionicsEx`). Use
     it with the standard SDK avionics functions::

       >>> device = xp_imgui.AvionicsDevice(...)
       >>> xp.setAvionicsPopupVisible(device.avionicsID)
       >>>

  .. py:method:: AvionicsDevice.delete() -> None

     Cleans up the ImGui context and renderer and ultimately calls
     :func:`xp.destroyAvionics`. Use this *instead of*
     ``xp.destroyAvionics(myDevice.avionicsID)``.

  .. py:method:: AvionicsDevice.popup(visible: bool = True) -> None

     Show or hide the 2-D pop-up window. Identical to
     ``xp.setAvionicsPopupVisible(self.avionicsID, 1 if visible else 0)``.

  .. py:method:: AvionicsDevice.isPopupVisible() -> bool

     Identical to ``xp.isAvionicsPopupVisible(self.avionicsID)``.

  .. py:method:: AvionicsDevice.hasInputFocus() -> bool

     Identical to ``xp.hasAvionicsKeyboardFocus(self.avionicsID)``.

  .. py:method:: AvionicsDevice.requestInputFocus(req: bool) -> None

     ``req=True`` calls :func:`xp.takeAvionicsKeyboardFocus`. Unlike the window
     case, ``req=False`` does nothing: the SDK has no avionics equivalent of
     ``takeKeyboardFocus(0)``, so X-Plane drops focus on its own. Focus applies
     only to the pop-up --- typing is not routed to a device drawn on a 3-D
     object or in the panel.

See :doc:`/development/imgui_avionics` for examples and details.
                 
