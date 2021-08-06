##############################################
# To use:
#      def drawCallback(windowID, refCon):
#          imgui.text('label')
#          if imgui.button("Press Me"):
#             do_something()

#      imgWindow = Window([..., drawCallback, ...])
#      imgWindow.setTitle('<string>')
#      ...
#      imgWindow.destroy()
#
# Create an IMGUI window by creating instance of Window() class
#   requires single parameter, which is 14 element list identical to
#   list you would otherwise provide to XPLMCreateWindowEx (that is:
#   position, reference constant, and callback functions)
# Once created, you will do your imgui drawing in your drawWindow callback
#   When you're called, we'll have already set up the environment:
#      set_current_context()
#      new_frame()
#      begin()
#        ... your callback here
#      end()
#      render()
# You callback will receive the XP windowID, and whatever you provided as
#   the reference constant
# You can set the window title using .setTitle(title=<string>)
# At program end, you should destroy the IMGUI window calling .destroy()
#
# YOU SHOULD NOT NEED TO MAKE ANY CHANGES TO THIS FILE -- if you do, it's most
# likely you're fixing a bug I should know about, so tell me about it -- pbuck@avnwx.com

import traceback
from collections import namedtuple
import sys
try:
    import OpenGL.GL as GL
except ImportError:
    print("[XPPython3] OpenGL not found. Use XPPython3 Pip Package Installer to install 'PyOpenGL' package and restart.")
    import platform
    if platform.system() == 'Darwin' and platform.mac_ver()[0] == '10.16':
        print("[XPPython3] For Mac 10.16 \"Big Sur\" you need to also edit OpenGL/platform/ctypesloader.py.")
    raise

from XPPython3 import imgui
from XPPython3 import xp
from .xprenderer import XPRenderer

CreateWindow_t = namedtuple('CreateWindow_t', ['left', 'top', 'right', 'bottom',
                                               'visible',
                                               'drawWindowFunc',
                                               'handleMouseFunc',
                                               'handleKeyFunc',
                                               'handleCursorFunc',
                                               'handleMouseWheelFunc',
                                               'refcon',
                                               'decorateAsFloatingWindow',
                                               'layer',
                                               'handleRightClickFunc'])


def loge(s):
    """ log with exception traceback """
    xp.log('{}\n{}'.format(s, traceback.format_exc()))


class Window:
    def __init__(self, pok):
        self.stop = False
        self.imgui_context = None
        self.fontTextureId = None
        self.windowID = None
        self.modelviewMatrixRef = xp.findDataRef("sim/graphics/view/modelview_matrix")
        self.viewportRef = xp.findDataRef("sim/graphics/view/viewport")
        self.projectionMatrixRef = xp.findDataRef("sim/graphics/view/projection_matrix")
        self.modelview = []
        self.projection = []
        self.viewport = []

        if len(pok) == 13:
            # Because handleRightClickFunc may not be specified
            pok.append(None)
        createWindow_t = CreateWindow_t(*pok)

        # Initialize
        self.imgui_context = imgui.create_context()
        imgui.set_current_context(self.imgui_context)
        imgui.get_style().window_rounding = 0

        self.io = imgui.get_io()
        # self.io.ini_file_name = '/dev/null'
        self.io.config_mac_osx_behaviors = False
        self.io.config_resize_windows_from_edges = False

        # self.io.ini_file_name = None
        self.io.config_flags = imgui.CONFIG_NAV_NO_CAPTURE_KEYBOARD

        # self.io.KeyMap here
        self.io.key_map[imgui.KEY_TAB] = xp.VK_TAB
        self.io.key_map[imgui.KEY_LEFT_ARROW] = xp.VK_LEFT
        self.io.key_map[imgui.KEY_RIGHT_ARROW] = xp.VK_RIGHT
        self.io.key_map[imgui.KEY_UP_ARROW] = xp.VK_UP
        self.io.key_map[imgui.KEY_DOWN_ARROW] = xp.VK_DOWN
        self.io.key_map[imgui.KEY_PAGE_UP] = xp.VK_PRIOR
        self.io.key_map[imgui.KEY_PAGE_DOWN] = xp.VK_NEXT
        self.io.key_map[imgui.KEY_HOME] = xp.VK_HOME
        self.io.key_map[imgui.KEY_END] = xp.VK_END
        self.io.key_map[imgui.KEY_INSERT] = xp.VK_INSERT
        self.io.key_map[imgui.KEY_DELETE] = xp.VK_DELETE
        self.io.key_map[imgui.KEY_BACKSPACE] = xp.VK_BACK
        self.io.key_map[imgui.KEY_SPACE] = xp.VK_SPACE
        self.io.key_map[imgui.KEY_ENTER] = xp.VK_ENTER
        self.io.key_map[imgui.KEY_ESCAPE] = xp.VK_ESCAPE
        self.io.key_map[imgui.KEY_A] = xp.VK_A
        self.io.key_map[imgui.KEY_C] = xp.VK_C
        self.io.key_map[imgui.KEY_V] = xp.VK_V
        self.io.key_map[imgui.KEY_X] = xp.VK_X
        self.io.key_map[imgui.KEY_Y] = xp.VK_Y
        self.io.key_map[imgui.KEY_Z] = xp.VK_Z

        self.renderer = XPRenderer()
        self.onClickCB = createWindow_t.handleMouseFunc
        self.buildWindow = createWindow_t.drawWindowFunc

        imguiCreateWindow_t = [createWindow_t.left, createWindow_t.top, createWindow_t.right, createWindow_t.bottom,
                               createWindow_t.visible,
                               self.drawWindow, self.handleMouseClick, self.handleKey,
                               self.handleCursor, self.handleMouseWheel, createWindow_t.refcon,
                               createWindow_t.decorateAsFloatingWindow, createWindow_t.layer,
                               self.handleRightClick]
        self.windowID = xp.createWindowEx(imguiCreateWindow_t)
        xp.bringWindowToFront(self.windowID)

    def delete(self):
        self.stop = True
        try:
            if self.imgui_context is not None:
                # (Bug in older version of imgui.cpp destroys 'current context' rather than the one passed it
                #  so we'll set_current just to be sure.)
                imgui.set_current_context(self.imgui_context)
                imgui.destroy_context(self.imgui_context)
                self.imgui_context = None
            if self.fontTextureId is not None:
                GL.glDeleteTextures([self.fontTextureId])
                self.fontTextureId = None
            if self.windowID is not None:
                xp.destroyWindow(self.windowID)
                self.windowID = None
        except Exception:
            loge("Exception while trying to delete window")

    def handleMouseClick(self, inWindowID, x, y, inMouse, inRefCon):
        # if not xp.hasKeyboardFocus(inWindowID):
        #     xp.takeKeyboardFocus(inWindowID)
        # First, handle imgui mouse click
        imgui.set_current_context(self.imgui_context)
        imguiX, imguiY = self.translateToImguiSpace(x, y)
        self.io.mouse_pos = imguiX, imguiY
        if inMouse in (xp.MouseDown, xp.MouseDrag):
            self.io.mouse_down[0] = True
        else:
            self.io.mouse_down[0] = False

        # Second, if user provided traditional XP callback, call that also
        if (self.onClickCB):
            self.onClickCB(inWindowID, x, y, inMouse, inRefCon)

        # Finally, return 1 to indicate we've handled the mouse click
        return 1

    def setTitle(self, title):
        xp.setWindowTitle(self.windowID, title)

    def drawWindow(self, windowID, inRefCon):
        self.updateMatrices()
        imgui.set_current_context(self.imgui_context)
        if self.stop:
            return
        try:
            try:
                # Set site of the window into which we'll be writing imgui
                (self.mLeft, self.mTop, self.mRight, self.mBottom) = xp.getWindowGeometry(windowID)
                io = imgui.get_io()

                hasKeyboardFocus = self.hasInputFocus()
                if io.want_text_input and not hasKeyboardFocus and xp.isWindowInFront(windowID):
                    self.requestInputFocus(True)
                elif not io.want_text_input and hasKeyboardFocus:
                    self.requestInputFocus(False)
                    # reset keysdown otherwise we'll think any keys used to defocus the keyboard are still down!
                    for x in range(len(io.keys_down)):
                        io.keys_down[x] = False

                io.display_size = self.mRight - self.mLeft, self.mTop - self.mBottom
                io.display_fb_scale = 1.0, 1.0
                try:
                    imgui.new_frame()
                    imgui.set_next_window_position(0.0, 0.0, imgui.ALWAYS)
                    imgui.set_next_window_size(self.mRight - self.mLeft, self.mTop - self.mBottom, imgui.ALWAYS)
                except Exception as e:
                    xp.log("Failure with window pos / size: {}".format(e))
                    raise

                imgui.begin("",
                            flags=imgui.WINDOW_NO_TITLE_BAR | imgui.WINDOW_NO_RESIZE | imgui.WINDOW_NO_COLLAPSE)
                if self.buildWindow:
                    self.buildWindow(windowID, inRefCon)
                imgui.end()
                imgui.render()

                imgui.set_current_context(self.imgui_context)
                self.renderer.render(self, imgui.get_draw_data())
            except Exception:
                loge('oops')
                self.stop = True
                return
        except Exception:
            loge("Exception in drawwindow")
            raise


    def hasInputFocus(self):
        ret = xp.hasKeyboardFocus(self.windowID)
        return ret

    def requestInputFocus(self, req):
        xp.takeKeyboardFocus(self.windowID if req else None)

    def handleRightClick(self, inWindowID, x, y, inMouse, inRefCon):
        return 1

    def handleCursor(self, inWindowID, x, y, inRefCon):
        imgui.set_current_context(self.imgui_context)
        io = imgui.get_io()
        outX, outY = self.translateToImguiSpace(x, y)
        io.mouse_pos = outX, outY

        return xp.CursorDefault

    def handleMouseWheel(self, inWindowID, x, y, wheel, clicks, inRefCon):
        imgui.set_current_context(self.imgui_context)
        io = imgui.get_io()
        outX, outY = self.translateToImguiSpace(x, y)
        io.mouse_pos = outX, outY
        if wheel == 0:
            io.mouse_wheel = clicks
        elif wheel == 1:
            io.mouse_wheel_horizontal = clicks
        return 1

    def handleKey(self, inWindowID, inKey, inFlags, inVirtualKey, inRefCon, losingFocus):
        # xp.log("Handle Key: winid: {}, key: {}, flags: {}, v_key: {}, losing: {}".format(
        #     inWindowID, inKey, inFlags, inVirtualKey, losingFocus))
        if losingFocus:
            self.requestInputFocus(False)
            return
        imgui.set_current_context(self.imgui_context)
        io = imgui.get_io()
        if io.want_text_input:
            # if you press and hold a key, the flags wiil actually be down, 0, 0, ..., up
            # so the key always has to be considered as pressed unless the up flag is set
            try:
                # print("inKey: {}, virtualKey: {}, Description: {}".format(inKey, inVirtualKey, xp.getVirtualKeyDescription(inVirtualKey & 0xff)))
                io.keys_down[inVirtualKey & 0xff] = (inFlags & xp.UpFlag) != xp.UpFlag
            except IndexError:
                loge('IndexError for inKey: {}, inVirtualKey: {} / {}'.format(inKey, inVirtualKey, inVirtualKey & 0xff))
                return
            io.key_shift = (inFlags & xp.ShiftFlag) == xp.ShiftFlag
            io.key_alt = (inFlags & xp.OptionAltFlag) == xp.OptionAltFlag
            io.key_ctrl = (inFlags & xp.ControlFlag) == xp.ControlFlag
            try:
                if (inFlags & xp.UpFlag) != xp.UpFlag and not io.key_ctrl and not io.key_alt and chr(inKey).isprintable():
                    io.add_input_character(inKey)
            except Exception:
                loge('failed to add key: {}, vk: {}'.format(inKey, inVirtualKey))
        return

    def translateToImguiSpace(self, x, y):
        imguiX = x - self.mLeft
        if imguiX < 0 or imguiX > (self.mRight - self.mLeft):
            return -sys.float_info.max, -sys.float_info.max
        imguiY = self.mTop - y
        if imguiY < 0 or imguiY > (self.mTop - self.mBottom):
            return -sys.float_info.max, -sys.float_info.max
        return imguiX, imguiY

    def translateImguiToBoxel(self, x, y):
        return int(self.mLeft + x), int(self.mTop - y)

    def boxelsToNative(self, x, y):
        # vrEnabledRef = XPLMFindDataRef("sim/graphics/VR/enabled");
        try:
            boxelPos = [float(x), float(y), 0, 1]

            eye = self.multMatrixVec4f(self.modelview, boxelPos)
            ndc = self.multMatrixVec4f(self.projection, eye)
            outX = int((ndc[0] * 0.5 + 0.5) * self.viewport[2] + self.viewport[0])
            outY = int((ndc[1] * 0.5 + 0.5) * self.viewport[3] + self.viewport[1])
        except Exception as e:
            xp.log("Exception in Boxesl to Native: {}".format(e))
            raise
        return outX, outY

    @staticmethod
    def multMatrixVec4f(m, v):
        return [v[0] * m[0] + v[1] * m[4] + v[2] * m[8] + v[3] * m[12],
                v[0] * m[1] + v[1] * m[5] + v[2] * m[9] + v[3] * m[13],
                v[0] * m[2] + v[1] * m[6] + v[2] * m[10] + v[3] * m[14],
                v[0] * m[3] + v[1] * m[7] + v[2] * m[11] + v[3] * m[15]]

    def updateMatrices(self):
        xp.getDatavf(self.modelviewMatrixRef, self.modelview, 0, 16)
        xp.getDatavf(self.projectionMatrixRef, self.projection, 0, 16)
        xp.getDatavi(self.viewportRef, self.viewport, 0, 4)
