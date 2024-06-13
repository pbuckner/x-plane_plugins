from typing import Any
from contextlib import contextmanager
import OpenGL.GL as GL
try:
    xp: Any  # this quiets mypy type checking
    from XPPython3 import xp
except ModuleNotFoundError:
    from .mock_xp import xp


@contextmanager
def maskContext():
    drawMaskStart()
    try:
        yield
    finally:
        drawMaskEnd()


def drawMaskStart() -> None:
    """
    call this to set up drawing a mask or stencil.
        drawMaskStart()
           <draw routines for mask/stencil>
        drawUnderMask(invert)
           <draw routines effected by mask/stencil>
        drawMaskEnd()
           <draw routines uneffected by mask /stencil>
    """
    GL.glClear(GL.GL_DEPTH_BUFFER_BIT)
    GL.glEnable(GL.GL_STENCIL_TEST)
    GL.glColorMask(0, 0, 0, 0)

    xp.setGraphicsState(depthWriting=0)

    GL.glStencilFunc(GL.GL_NEVER, 1, 0xff)
    GL.glStencilOp(GL.GL_REPLACE, GL.GL_KEEP, GL.GL_KEEP)

    xp.setGraphicsState(depthTesting=1)
    GL.glStencilMask(0xff)
    GL.glClear(GL.GL_STENCIL_BUFFER_BIT)


def drawUnderMask(stencil=False) -> None:
    """
    by default (stencil=False), excludes drawing under mask
    optionally (stencil=True),  ONLY draws where stencil is defined
    See drawMaskStart().
    """
    GL.glColorMask(1, 1, 1, 1)
    xp.setGraphicsState(depthTesting=1, depthWriting=1)
    GL.glStencilMask(0x00)
    GL.glStencilFunc(GL.GL_NOTEQUAL if stencil else GL.GL_EQUAL, 0, 0xff)


def drawMaskEnd() -> None:
    """
    Resets (removes) drawing mask.
    See drawMaskStart().
    """
    GL.glDisable(GL.GL_STENCIL_TEST)
    xp.setGraphicsState(depthTesting=0)

