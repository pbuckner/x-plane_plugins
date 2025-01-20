from typing import Any, Tuple
import OpenGL.GL as GL
import OpenGL.GLU as GLU
try:
    import numpy  # noqa # pylint: disable=unused-import
except ModuleNotFoundError:
    # PIL uses numpy, so if we don't have numpy, we'll need to install both
    print("Missing python numpy module. Use pip to install 'numpy' and 'Pillow'")
    raise
try:
    from PIL import Image
except ModuleNotFoundError:
    print("Missing python Pillow module. Use pip to install 'Pillow'")
    raise
try:
    xp: Any  # this quiets mypy type checking
    from XPPython3 import xp
except ModuleNotFoundError:
    from .mock_xp import xp
from .colors import RGBColor
from .transformations import graphicsContext, translateContext


Textures = {}
XPGL_Texture_ID = 0


def loadImage(file_name: str, x: float = 0, y: float = 0, width: float = 0, height: float = 0, ) -> int:
    global XPGL_Texture_ID  # pylint: disable=global-statement
    XPGL_Texture_ID += 1
    try:
        info = [(k, v) for (k, v) in Textures.items() if v['file_name'] == file_name][0]
        texture_id = info[1]['texture_id']
        size = info[1]['size']
    except IndexError:
        texture_id, size = loadFileImage(file_name)

    Textures[XPGL_Texture_ID] = {'texture_id': texture_id,
                                 'file_name': file_name,
                                 'tex_left': x / size[0],
                                 'tex_right': (x + (width or size[0])) / size[0],
                                 'tex_top': (y + (height or size[1])) / size[1],
                                 'tex_bottom': y / size[1],
                                 'x': x or 0,
                                 'y': y or 0,
                                 'size': size,
                                 'width': width or size[0],
                                 'height': height or size[1]}
    return XPGL_Texture_ID


def loadFileImage(file_name: str) -> Tuple[int, Tuple[int, int]]:
    # (SASL uses a search path for images)
    xp.bindTexture2d(0, 0)
    texture_id: int = xp.generateTextureNumbers(1)[0]
    xp.bindTexture2d(texture_id, 0)
    if not GL.glIsTexture(texture_id):
        raise ValueError(f"TextureID '{texture_id}' is not a valid texture")

    im = Image.open(file_name)
    ix, iy, image = im.size[0], im.size[1], im.tobytes('raw', 'RGBA', 0, -1)
    # ix, iy, image = im.size[0], im.size[1], im.convert('RGBA').tobytes()

    xp.bindTexture2d(texture_id, 0)
    GL.glPixelStorei(GL.GL_UNPACK_ALIGNMENT, 1)
    GLU.gluBuild2DMipmaps(GL.GL_TEXTURE_2D, GL.GL_RGBA, ix, iy, GL.GL_RGBA, GL.GL_UNSIGNED_BYTE, image)
    GL.glTexParameterf(GL.GL_TEXTURE_2D, GL.GL_TEXTURE_WRAP_S, GL.GL_REPEAT)
    GL.glTexParameterf(GL.GL_TEXTURE_2D, GL.GL_TEXTURE_WRAP_T, GL.GL_REPEAT)
    GL.glTexParameterf(GL.GL_TEXTURE_2D, GL.GL_TEXTURE_MIN_FILTER, GL.GL_LINEAR)
    GL.glTexParameterf(GL.GL_TEXTURE_2D, GL.GL_TEXTURE_MAG_FILTER, GL.GL_LINEAR)
    GL.glTexEnvi(GL.GL_TEXTURE_ENV, GL.GL_TEXTURE_ENV_MODE, GL.GL_MODULATE)
    xp.bindTexture2d(0, 0)
    return texture_id, (ix, iy)


def drawRotatedTexture(texture_id: int, angle: float, x: int, y: int,
                       width: int, height: int, color: RGBColor = None) -> None:
    with graphicsContext():
        with translateContext(x, y):
            GL.glRotate(angle, 0, 0, 1)
        drawTexture(texture_id, x, y, width, height, color)


def drawRotatedTextureCenter(texture_id: int, angle: float, x: int, y: int,
                             width: int, height: int, color: RGBColor = None) -> None:
    with graphicsContext():
        with translateContext(x + width / 2, y + height / 2):
            GL.glRotate(angle, 0, 0, 1)
        drawTexture(texture_id, x, y, width, height, color)


def drawTexture(texture_id: int, x: float, y: float, width: float, height: float, color: RGBColor = None) -> None:
    # color, if provided, will replace WHITE areas of the image
    # otherwise... we'll use white

    # "texture_id" is xpgl_texture_id, NOT opgl_texture_id

    info = Textures[texture_id]

    opgl_texture_id = info['texture_id']

    xp.setGraphicsState(numberTexUnits=1, alphaBlending=1, depthWriting=1)
    GL.glBlendFunc(GL.GL_SRC_ALPHA, GL.GL_ONE_MINUS_SRC_ALPHA)
    xp.bindTexture2d(opgl_texture_id, 0)
    with graphicsContext():
        if color is not None:
            GL.glColor(*color)
        else:
            GL.glColor(1, 1, 1, 1)
        GL.glBegin(GL.GL_QUADS)

        # flipped
        tex_left = info['tex_left']
        tex_right = info['tex_right']
        tex_top = info['tex_top']
        tex_bottom = info['tex_bottom']

        GL.glTexCoord(tex_right, tex_bottom)
        GL.glVertex(x + width, y)           # right, bottom

        GL.glTexCoord(tex_left, tex_bottom)
        GL.glVertex(x, y)                   # left, bottom

        GL.glTexCoord(tex_left, tex_top)
        GL.glVertex(x, y + height)          # left, top

        GL.glTexCoord(tex_right, tex_top)
        GL.glVertex(x + width, y + height)  # right, top

        GL.glEnd()
    xp.setGraphicsState(numberTexUnits=0)
    xp.bindTexture2d(0, 0)
