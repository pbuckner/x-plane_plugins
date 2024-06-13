from typing import List, Tuple
import math
import OpenGL.GL as GL
from .colors import RGBColor, Colors
from . import triangulate
from .lines import Smooth_Lines, Pattern, drawPolyLine


def drawTriangle(x1: float, y1: float, x2: float, y2: float, x3: float, y3: float,
                 color: RGBColor = Colors['white']) -> None:
    """ draw filled triangle """
    drawPolygon(points=[(x1, y1), (x2, y2), (x3, y3)], isFilled=True, color=color)


def drawRectangle(x: float, y: float, width: float, height: float, color: RGBColor = Colors['white']) -> None:
    """ draw filled rectangle """
    drawPolygon(points=[(x, y), (x + width, y), (x + width, y + height), (x, y + height)], isFilled=True, color=color)


def drawFrame(x: float, y: float, width: float, height: float, thickness: float = 1.,
              color: RGBColor = Colors['white']) -> None:
    """ draw frame (rectangle outline) """
    drawPolygon(points=[(x, y), (x + width, y), (x + width, y + height), (x, y + height)],
                isFilled=False, thickness=thickness, color=color)


def drawPolygon(points: List | Tuple, isFilled: bool = True, thickness: float = 1.,
                color: RGBColor = Colors['white']) -> None:
    # https://github.com/yaugenst/triangulation/blob/master/sources/main.py
    #  and code in triangulation

    width = GL.glGetFloatv(GL.GL_LINE_WIDTH)
    GL.glColor(*color, 1)
    if isFilled:
        GL.glLineWidth(1)
        tri = []
        plist = points[::-1] if triangulate.IsClockwise(points) else points[:]
        while len(plist) >= 3:
            a = triangulate.GetEar(plist)
            if a == []:
                break
            tri.append(a)

        GL.glBegin(GL.GL_TRIANGLES)
        for x in tri:
            for i in x[::-1]:
                GL.glVertex(i[0], i[1])
        GL.glEnd()
        GL.glLineWidth(width)
        return

    if Smooth_Lines:
        GL.glEnable(GL.GL_LINE_SMOOTH)  # for anti-aliasing
        GL.glHint(GL.GL_LINE_SMOOTH_HINT, GL.GL_DONT_CARE)
    if Pattern != 0xffff:
        GL.glEnable(GL.GL_LINE_STIPPLE)
        GL.glLineStipple(int(thickness), Pattern)
    GL.glLineWidth(thickness)
    GL.glBegin(GL.GL_LINE_LOOP)
    for i in points:
        GL.glVertex(i[0], i[1])
    if points[0] != points[-1]:
        GL.glVertex(points[0][0], points[0][1])
    GL.glEnd()
    GL.glLineWidth(width)
    if Pattern != 0xffff:
        GL.glDisable(GL.GL_LINE_STIPPLE)


def drawCircle(x: float, y: float, radius: float,
               isFilled: bool = False, thickness: float = 1.,
               num_vertices: int = 36,
               color: RGBColor = Colors['white']) -> None:
    # "radius" assumes same pixel size... that's the best we can do. Circles will be squished if not the same
    rads = 2 * math.pi / num_vertices
    alpha = 0.0
    vertices = []
    for _i in range(num_vertices):
        # need to go counter-clockwise
        px = x + radius * math.cos(alpha)
        py = y + radius * math.sin(alpha)
        vertices.append((px, py))
        alpha += rads
    drawPolygon(points=vertices, thickness=thickness, isFilled=isFilled, color=color)


def drawArcLine(x: float, y: float, radius: float, start_angle: float, arc_angle: float,
                thickness: float = 1., num_vertices: float = 36,
                color: RGBColor = Colors['white']) -> None:
    def deg2rad(degrees):
        return math.pi * degrees / 180.0

    rads = 2 * math.pi / num_vertices  # #radians per line segment
    alpha = deg2rad(start_angle % 360.)
    omega = deg2rad(start_angle + arc_angle)
    if omega < alpha:
        alpha, omega = omega, alpha

    vertices = []
    while alpha < omega:
        px = x + radius * math.cos(alpha)
        py = y + radius * math.sin(alpha)
        vertices.append((px, py))
        alpha += rads
    vertices.append((x + radius * math.cos(omega), y + radius * math.sin(omega)))

    drawPolyLine(vertices, thickness, color)


def drawArc(x: float, y: float, radius_inner: float, radius_outer: float,
            start_angle: float, arc_angle: float,
            num_vertices: int = 36, color: RGBColor = Colors['white']) -> None:

    def deg2rad(degrees):
        return math.pi * degrees / 180.0

    if radius_inner > radius_outer:
        radius_inner, radius_outer = radius_outer, radius_inner

    rads = 2 * math.pi / num_vertices  # (for circle)
    alpha = deg2rad(start_angle % 360)
    omega = deg2rad(start_angle + arc_angle)

    if omega < alpha:
        alpha, omega = omega, alpha

    vertices = []
    angle = alpha
    while angle < omega:
        px = x + radius_outer * math.cos(angle)
        py = y + radius_outer * math.sin(angle)
        vertices.append((px, py))
        angle += rads

    vertices.append((x + radius_outer * math.cos(omega), y + radius_outer * math.sin(omega)))
    if radius_inner > 0:
        angle = omega
        while angle > alpha:
            px = x + radius_inner * math.cos(angle)
            py = y + radius_inner * math.sin(angle)
            vertices.append((px, py))
            angle -= rads
        vertices.append((x + radius_inner * math.cos(alpha), y + radius_inner * math.sin(alpha)))
    else:
        vertices.append((x, y))
    drawPolygon(points=vertices, isFilled=True, color=color)
    
