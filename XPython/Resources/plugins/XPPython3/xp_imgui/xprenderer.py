#####################################
# YOU SHOULD NOT NEED TO MODIFY THIS FILE
# Nor should you need to call it directly as it is normally called only
# by xp_imgui.Window(). You should be using xp_imgui.Window()
#
# This defines the XPRenderer which connects imgui OpenGL with X-Plane
# It is modeled after the FlyWithLua integration at
#   https://github.com/X-Friese/FlyWithLua/blob/master/src/FloatingWindows/
# and modified for pyimgui. The errors, alas, are all mine.
#
# PLEASE let me know if something is wrong so I can fix it for everyone: pbuck@avnwx.com

import ctypes
try:
    import OpenGL.GL as GL
except ImportError:
    print("[XPPython3] OpenGL not found. Use XPPython3 Pip Package Installer to install 'PyOpenGL' package and restart.")
    raise
import imgui
from imgui.integrations.opengl import FixedPipelineRenderer
    
from XPPython3 import xp


class XPRenderer(FixedPipelineRenderer):
    _font_texture: int

    def refresh_font_texture(self):
        # NOTE: as copied from integrations.opengl, but change glGenTextures() to xp.generateTextureNumbers()
        width, height, pixels = self.io.fonts.get_tex_data_as_alpha8()

        if self._font_texture is not None:
            GL.glDeleteTextures([self._font_texture])

        # self._font_texture = GL.glGenTextures(1)
        self._font_texture = xp.generateTextureNumbers(1)[0]

        # GL.glBindTexture(GL.GL_TEXTURE_2D, self._font_texture)
        xp.bindTexture2d(self._font_texture, 0)
        GL.glTexParameteri(GL.GL_TEXTURE_2D, GL.GL_TEXTURE_MIN_FILTER, GL.GL_LINEAR)
        GL.glTexParameteri(GL.GL_TEXTURE_2D, GL.GL_TEXTURE_MAG_FILTER, GL.GL_LINEAR)
        GL.glTexImage2D(GL.GL_TEXTURE_2D, 0, GL.GL_ALPHA, width, height, 0, GL.GL_ALPHA, GL.GL_UNSIGNED_BYTE, pixels)

        self.io.fonts.texture_id = self._font_texture
        self.io.fonts.clear_tex_data()

    def render(self, window, draw_data):
        (mLeft, mTop, mRight, mBottom) = xp.getWindowGeometry(window.windowID)

        io = self.io

        display_width, display_height = io.display_size
        fb_width = int(display_width * io.display_fb_scale[0])
        fb_height = int(display_height * io.display_fb_scale[1])

        if fb_width == 0 or fb_height == 0:
            return

        draw_data.scale_clip_rects(*io.display_fb_scale)

        xp.setGraphicsState(0, 1, 0, 1, 1, 0, 0)

        GL.glPushClientAttrib(GL.GL_CLIENT_ALL_ATTRIB_BITS)
        GL.glPushAttrib(GL.GL_ENABLE_BIT | GL.GL_COLOR_BUFFER_BIT | GL.GL_TRANSFORM_BIT)
        GL.glDisable(GL.GL_CULL_FACE)
        GL.glEnable(GL.GL_SCISSOR_TEST)
        GL.glEnableClientState(GL.GL_VERTEX_ARRAY)
        GL.glEnableClientState(GL.GL_TEXTURE_COORD_ARRAY)
        GL.glEnableClientState(GL.GL_COLOR_ARRAY)
        GL.glEnable(GL.GL_TEXTURE_2D)

        # (don't set viewport -- we'll use scale and translate to position correctly)
        GL.glMatrixMode(GL.GL_PROJECTION)
        GL.glPushMatrix()
        GL.glScalef(1.0, -1.0, 1.0)
        GL.glTranslatef(mLeft, -mTop, 0.0)

        for commands in draw_data.commands_lists:
            idx_buffer = commands.idx_buffer_data

            GL.glVertexPointer(2, GL.GL_FLOAT, imgui.VERTEX_SIZE, ctypes.c_void_p(commands.vtx_buffer_data + imgui.VERTEX_BUFFER_POS_OFFSET))
            GL.glTexCoordPointer(2, GL.GL_FLOAT, imgui.VERTEX_SIZE, ctypes.c_void_p(commands.vtx_buffer_data + imgui.VERTEX_BUFFER_UV_OFFSET))
            GL.glColorPointer(4, GL.GL_UNSIGNED_BYTE, imgui.VERTEX_SIZE, ctypes.c_void_p(commands.vtx_buffer_data + imgui.VERTEX_BUFFER_COL_OFFSET))

            for command in commands.commands:
                GL.glBindTexture(GL.GL_TEXTURE_2D, command.texture_id)
                x, y, z, w = command.clip_rect

                bLeft, bTop = window.translateImguiToBoxel(x, y)
                bRight, bBottom = window.translateImguiToBoxel(z, w)
                nLeft, nTop = window.boxelsToNative(bLeft, bTop)
                nRight, nBottom = window.boxelsToNative(bRight, bBottom)

                GL.glScissor(int(nLeft), int(nBottom), int(nRight - nLeft), int(nTop - nBottom))

                if imgui.INDEX_SIZE == 2:
                    gltype = GL.GL_UNSIGNED_SHORT
                else:
                    gltype = GL.GL_UNSIGNED_INT

                GL.glDrawElements(GL.GL_TRIANGLES, command.elem_count, gltype, ctypes.c_void_p(idx_buffer))
                idx_buffer += (command.elem_count * imgui.INDEX_SIZE)

        GL.glMatrixMode(GL.GL_PROJECTION)
        GL.glPopMatrix()
        GL.glDisableClientState(GL.GL_COLOR_ARRAY)
        GL.glDisableClientState(GL.GL_TEXTURE_COORD_ARRAY)
        GL.glDisableClientState(GL.GL_VERTEX_ARRAY)
        GL.glPopAttrib()
        GL.glPopClientAttrib()
