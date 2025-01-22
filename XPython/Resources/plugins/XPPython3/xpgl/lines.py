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
    if Smooth_Lines:
        xp.setGraphicsState(alphaTesting=1, alphaBlending=1)
        GL.glEnable(GL.GL_LINE_SMOOTH)  # for anti-aliasing
        GL.glBlendFunc(GL.GL_SRC_ALPHA, GL.GL_ONE_MINUS_SRC_ALPHA)
        GL.glHint(GL.GL_LINE_SMOOTH_HINT, GL.GL_DONT_CARE)

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
