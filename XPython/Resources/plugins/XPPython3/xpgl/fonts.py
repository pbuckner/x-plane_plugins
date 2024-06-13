from typing import Any, List, Tuple
from array import array
from OpenGL.GL import shaders
import OpenGL.GLUT as GLUT
from OpenGL import GL, error
import math
import ctypes
from .colors import RGBColor, Colors

try:
    xp: Any  # this quiets mypy type checking
    from XPPython3 import xp

    def enableBlend():
        xp.setGraphicsState(numberTexUnits=1, alphaBlending=1)
except ModuleNotFoundError:
    from .mock_xp import xp

    def enableBlend():
        GL.glEnable(GL.GL_BLEND)

try:
    from freetype import Face, FT_LOAD_RENDER  # type: ignore
except ModuleNotFoundError:
    xp.log("Failed to import freetype, use pip and install 'freetype-py'")


class Faces:
    Face_Info = {}
    Face_ID_Cntr = -1
    FontShaderProgram = None
    Uniform_Color = None
    Uniform_Tex = None
    Attribute_Coord = None

    @classmethod
    def init(cls):
        xp.log("initing...")
        if Faces.FontShaderProgram is not None:
            return
        # SET UP SIMPLE SHADER
        # since text is basically two-dimensional, we could use an attribute vec2 for vertices,
        # and another attribute vec2 for texture coordinates. But it is also possible to combine
        # the vertex and texture coordinates into a singe four-dimensional vector ('coord') and
        # have the vertex shader split it into two -- the first two elements are Position,
        # the second two are the texture coordinates ('texcoord').
        VERTEX_SHADER = """#version 120
        attribute vec4 coord;
        varying vec2 texcoord;

        void main(void)
        {
           gl_Position = gl_ModelViewProjectionMatrix * vec4(coord.xy, 0, 1);
           //gl_Position = coord;
            texcoord = coord.zw;
        }
        """
        # The best way to draw text is to use a texture that contains only alpha values.
        # The RGB color itself is set to the same value for all pixels. Where the alpha
        # color is 1 (opaque) the font color is drawn. Where it is 0 (transparent), the
        # background color is drawn. Where the alph value is between 0 and 1, the
        # background color is allowed to mix with the font color.
        # texture2D() samples the given texture, at the particular coordinate and returns
        #   a vec4 value -- we then use just the 'red' value as the alpha, to determine
        #   intensity of the color.
        FRAGMENT_SHADER = """#version 120
        varying vec2 texcoord;
        uniform sampler2D tex;
        uniform vec4 color;

        void main(void)
        {
            gl_FragColor = vec4(1, 1, 1, texture2D(tex, texcoord).r) * color;
            //gl_FragColor = vec4(.5, .5, .5, 1);
            //gl_FragColor = vec4(.5, .5, .5, 1) * color;
        }
        """

        vertexshader = shaders.compileShader(VERTEX_SHADER, GL.GL_VERTEX_SHADER)
        fragmentshader = shaders.compileShader(FRAGMENT_SHADER, GL.GL_FRAGMENT_SHADER)
        Faces.FontShaderProgram = shaders.compileProgram(vertexshader, fragmentshader, validate=False)
        Faces.Uniform_Color = GL.glGetUniformLocation(Faces.FontShaderProgram, 'color')
        Faces.Uniform_Tex = GL.glGetUniformLocation(Faces.FontShaderProgram, 'tex')
        Faces.Attribute_Coord = GL.glGetAttribLocation(Faces.FontShaderProgram, 'coord')

    @classmethod
    def load_font(cls, filename="/System/Library/Fonts/MarkerFelt.ttc", size=48, angle=0.0):
        # size in pixels
        # angle in degrees, 0 is "3 o'clock", counter clockwise.

        xp.log(f"Loading font: {filename}")
        if filename in (xp.Font_Proportional, xp.Font_Basic):
            return FontXP(filename)

        if filename.upper() == "HELVETICA":
            if size == 10:
                bitmapFont = GLUT.GLUT_BITMAP_HELVETICA_10
            elif size == 12:
                bitmapFont = GLUT.GLUT_BITMAP_HELVETICA_12
            elif size == 18:
                bitmapFont = GLUT.GLUT_BITMAP_HELVETICA_18
            else:
                raise ValueError(f"Unknown font filename {filename} [{size}]")
            return FontGLUT(size, bitmapFont)

        if filename.upper() == "TIMES_ROMAN":
            if size == 10:
                bitmapFont = GLUT.GLUT_BITMAP_TIMES_ROMAN_10
            elif size == 24:
                bitmapFont = GLUT.GLUT_BITMAP_TIMES_ROMAN_24
            else:
                raise ValueError(f"Unknown font filename {filename} [{size}]")
            return FontGLUT(size, bitmapFont)

        if Faces.FontShaderProgram is None:
            Faces.init()

        face = Face(filename)
        # face.set_pixel_sizes(0, 48)
        face.set_char_size(size * 64)  # 1/64th of a pixel
        Faces.Face_ID_Cntr += 1
        Faces.Face_Info[Faces.Face_ID_Cntr] = {'face': face,
                                               'font_id': Faces.Face_ID_Cntr,
                                               'filename': filename,
                                               'angle': angle,  # degrees
                                               'size': size,  # pixels
                                               'characters': {},
                                               'cache': {}}
        if angle != 0:
            print(f"Angle {angle} now {angle * math.pi / 180}")
            angle = angle * math.pi / 180
        for p in range(128):
            tex = xp.generateTextureNumbers(1)[0]
            xp.bindTexture2d(tex, 0)
            # Clamp texture an edges and enable linear interpolation
            GL.glTexParameteri(GL.GL_TEXTURE_2D, GL.GL_TEXTURE_WRAP_S, GL.GL_CLAMP_TO_EDGE)
            GL.glTexParameteri(GL.GL_TEXTURE_2D, GL.GL_TEXTURE_WRAP_T, GL.GL_CLAMP_TO_EDGE)

            GL.glTexParameteri(GL.GL_TEXTURE_2D, GL.GL_TEXTURE_MIN_FILTER, GL.GL_LINEAR)
            GL.glTexParameteri(GL.GL_TEXTURE_2D, GL.GL_TEXTURE_MAG_FILTER, GL.GL_LINEAR)

            # disable default 4-byte alignment (though most textures are aligned, glyphs are not)
            GL.glPixelStorei(GL.GL_UNPACK_ALIGNMENT, 1)

            if angle != 0:
                # ###################vvvvvvvvvvvv
                GL.glMatrixMode(GL.GL_MODELVIEW)
                GL.glLoadIdentity()
                GL.glPushMatrix()
                matrix = (ctypes.c_long * 4)()
                matrix[0] = int(math.cos(angle) * 0x10000)
                matrix[1] = int(-math.sin(angle) * 0x10000)
                matrix[2] = int(math.sin(angle) * 0x10000)
                matrix[3] = int(math.cos(angle) * 0x10000)
                pen = (ctypes.c_long * 2)()
                pen[0] = int(0 * 64)  # (I don't use pen... not sure how / if I should)
                pen[1] = int(0 * 64)

                face.set_transform(matrix, pen)
                # ####################^^^^^^^^^^^^^^^^^

            face.load_char(chr(p), FT_LOAD_RENDER)

            GL.glTexImage2D(GL.GL_TEXTURE_2D,
                            0,
                            GL.GL_RED,  # internal format: pixel consists of single component R
                            face.glyph.bitmap.width,
                            face.glyph.bitmap.rows,
                            0,  # no border
                            GL.GL_RED,  # format: pixel consists of single component R
                            GL.GL_UNSIGNED_BYTE,
                            face.glyph.bitmap.buffer)
            Faces.Face_Info[Faces.Face_ID_Cntr]['characters'][chr(p)] = {
                'font_id': Faces.Face_ID_Cntr,
                'texture': tex,
                'glyph': face.load_glyph(face.get_char_index(chr(p))),
                'bitmap_left': face.glyph.bitmap_left,
                'bitmap_top': face.glyph.bitmap_top,
                'width': face.glyph.bitmap.width,
                'rows': face.glyph.bitmap.rows,
                'advance_x': face.glyph.advance.x / 64,  # advance.x, advance.y are pixel, need to convert to 26.6 format
                'advance_y': face.glyph.advance.y / 64,
            }
            if angle != 0:
                GL.glPopMatrix()
                GL.glFlush()

        return FontTrueType(Faces.Face_ID_Cntr)

    @classmethod
    def unload_font(cls, font):
        font.unload()


class Font:

    def measure_text(self, text):
        raise RuntimeError("measure_text() not subclassed")

    def align(self, x, y, text, alignment='L'):
        if alignment.upper()[0] in ('R', 'C', 'M'):
            length = self.measure_text(text)
            if alignment.upper()[0] == 'R':
                x = x - length
            else:
                x = x - (length / 2)
        return x, y

    def unload(self):
        pass


class FontXP(Font):
    def __init__(self, bitmapFont):
        self.bitmapFont = bitmapFont

    def measure_text(self, text):
        return xp.measureString(self.bitmapFont, text)

    def draw_text(self, x, y, text, alignment='L', color: RGBColor = Colors['white']):
        GL.glColor(*color)
        x, y = self.align(x, y, text, alignment)
        xp.drawString(color, int(x), int(y), text, fontID=self.bitmapFont)
        return x + self.measure_text(text)


class FontGLUT(Font):
    def __init__(self, size, bitmapFont):
        self.size = size
        self.bitmapFont = bitmapFont

    def measure_text(self, text):
        x = 0.
        try:
            for p in text:
                x += GLUT.glutBitmapWidth(self.bitmapFont, ord(p))
            return x
        except error.NullFunctionError:
            xp.log("GLUT not properly installed")

    def draw_text(self, x, y, text, alignment='L', color: RGBColor = Colors['white']):
        # if any part of result would be rendered out-of-bounds (< 0), NOTHING is displayed!
        # this is not an issue for +x or +y
        x, y = self.align(x, y, text, alignment)
        GL.glColor(*color, 1)
        GL.glRasterPos(x, y)
        try:
            for c in text:
                GLUT.glutBitmapCharacter(self.bitmapFont, ord(c))
            return x + self.measure_text(text)
        except error.NullFunctionError:
            xp.log("GLUT not propery installed")


class FontTrueType(Font):
    def __init__(self, font_id):
        try:
            self.font_info = Faces.Face_Info[font_id]
        except KeyError as e:
            raise ValueError(f"Unknown Face for font_id {font_id}") from e

    def unload(self):
        try:
            for p in range(128):
                GL.glDeleteTextures(1, [self.font_info['characters'][chr(p)]['texture'], ])
            del Faces.Face_Info[self.font_info['font_id']]
        except KeyError as e:
            raise ValueError("unload_font() Error: Cannot find font") from e

    def measure_text(self, text):
        x = 0
        for p in text:
            x += self.font_info['characters'][p]['advance_x']
        return x

    def draw_text(self, x, y, text, alignment='L', color: RGBColor | Tuple | List = None):
        # Unlike GLUT, TrueType has no problem drawing < 0
        x, y = self.align(x, y, text, alignment)
        GL.glUseProgram(Faces.FontShaderProgram)
        # Fragment shader allows us to render transparent text, and should be used in combination with blending;
        enableBlend()
        GL.glBlendFunc(GL.GL_SRC_ALPHA, GL.GL_ONE_MINUS_SRC_ALPHA)

        # Intialization: Use a single texture object to render all the glyphs
        # Set the active texture unit to 0 (GL_TEXTURE0)
        # generate a texture id and bind current unit's 2D_texture to that id ('tex')
        # Finally, set 'tex' in the shader to 0 (
        GL.glActiveTexture(GL.GL_TEXTURE0)
        # set up a VBO for our combined vertex and texture coordinates
        vbo = GL.glGenBuffers(1)
        GL.glEnableVertexAttribArray(Faces.Attribute_Coord)
        GL.glBindBuffer(GL.GL_ARRAY_BUFFER, vbo)
        GL.glVertexAttribPointer(Faces.Attribute_Coord, 4, GL.GL_FLOAT, GL.GL_FALSE, 0, None)
        # Vertex need to be normalized going -1.0 ... 1.0

        if color and len(color) == 3:
            color = *color, 1.
        elif not color:
            color = (1., 1., 1., 1.)

        GL.glUniform4fv(Faces.Uniform_Color, 1, color)

        for p in text:
            info = self.font_info['characters'][p]
            xp.bindTexture2d(info['texture'], 0)
            x2 = x + info['bitmap_left']
            y2 = -y - info['bitmap_top']

            w = info['width']
            h = info['rows']

            box = [x2, -y2, 0, 0,
                   x2 + w, -y2, 1, 0,
                   x2, -y2 - h, 0, 1,
                   x2 + w, -y2 - h, 1, 1]

            x += info['advance_x']
            y += info['advance_y']
            vertices = array("f", box).tobytes()

            GL.glBufferData(GL.GL_ARRAY_BUFFER, vertices, GL.GL_DYNAMIC_DRAW)
            GL.glDrawArrays(GL.GL_TRIANGLE_STRIP, 0, 4)

        GL.glDeleteBuffers(1, [vbo])
        GL.glDisableVertexAttribArray(Faces.Attribute_Coord)
        GL.glUseProgram(0)
        xp.bindTexture2d(0, 0)
        return x
