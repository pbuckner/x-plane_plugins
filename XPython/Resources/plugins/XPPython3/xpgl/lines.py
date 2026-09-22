from typing import List, Tuple, Any
from .colors import Colors, RGBColor
from OpenGL import GL
try:
    xp: Any  # this quiets mypy type checking
    from XPPython3 import xp
except ModuleNotFoundError:
    from .mock_xp import xp

Smooth_Lines = True
Pattern = 0xffff  # i.e., off


def drawLine(x1: float, y1: float, x2: float, y2: float, thickness: float = 1.,
             color: RGBColor = Colors['white']) -> None:
    drawPolyLine([(x1, y1), (x2, y2)], thickness, color)


def drawPolyLine(points: List | Tuple, thickness: float = 1., color: RGBColor = Colors['white']) -> None:
    # Declare our state unconditionally: it is not an anti-aliasing detail. Without
    # it we inherit whatever X-Plane left current, and in (e.g.) a widget draw
    # callback that includes an enabled texture unit -- GL_MODULATE then multiplies
    # 'color' by a texel of the UI atlas and the line draws black. numberTexUnits=0
    # (the default) turns texturing off. alphaBlending=1 enables GL_BLEND, so the
    # blend func has to be set here too, not only on the smooth path.
    xp.setGraphicsState(alphaTesting=1, alphaBlending=1)
    GL.glBlendFunc(GL.GL_SRC_ALPHA, GL.GL_ONE_MINUS_SRC_ALPHA)
    if Smooth_Lines:
        GL.glEnable(GL.GL_LINE_SMOOTH)  # for anti-aliasing
        GL.glHint(GL.GL_LINE_SMOOTH_HINT, GL.GL_DONT_CARE)
    else:
        # GL_LINE_SMOOTH is not reset below, so a previous smooth call would
        # otherwise leave it enabled for everything that follows.
        GL.glDisable(GL.GL_LINE_SMOOTH)

    if Pattern != 0xffff:
        GL.glEnable(GL.GL_LINE_STIPPLE)
        GL.glLineStipple(int(thickness), Pattern)
    width = GL.glGetFloat(GL.GL_LINE_WIDTH)
    GL.glLineWidth(thickness)
    if len(color) == 3:
        GL.glColor(*color, 1.)
    else:
        GL.glColor(*color)
    GL.glBegin(GL.GL_LINE_STRIP)
    for i in points:
        GL.glVertex(i[0], i[1])
    GL.glEnd()
    GL.glLineWidth(width)
    if Pattern != 0xffff:
        GL.glDisable(GL.GL_LINE_STIPPLE)


def setLinePattern(pattern: int = 0xffff) -> None:
    global Pattern  # pylint: disable=global-statement
    # 0xffff  off
    # 0x5555  evenly spaced dots, same width as line 0101-0101-0101-0101
    # 0x3333
    # 0x00ff  dash
    Pattern = pattern
