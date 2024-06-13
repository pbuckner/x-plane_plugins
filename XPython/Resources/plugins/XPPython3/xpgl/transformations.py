from contextlib import contextmanager
import OpenGL.GL as GL


@contextmanager
def graphicsContext(*_args, **_kwargs):
    saveGraphicsContext()
    try:
        yield
    finally:
        restoreGraphicsContext()


@contextmanager
def translateContext(dx: float = 0, dy: float = 0):
    try:
        if dx or dy:
            setTranslateTransform(dx, dy)
        yield
    finally:
        if dx or dy:
            setTranslateTransform(-dx, -dy)


def saveGraphicsContext() -> None:
    GL.glPushMatrix()


def restoreGraphicsContext() -> None:
    GL.glPopMatrix()


def setTranslateTransform(dx: float = 0, dy: float = 0) -> None:
    GL.glTranslate(dx, dy, 0)


def setRotateTransform(angle: float, x: float = 0, y: float = 0) -> None:
    with translateContext(x, y):
        GL.glRotate(angle, 0, 0, 1)


def setScaleTransform(dx: float = 1, dy: float = 1,
                      x: float = 0, y: float = 0) -> None:
    with translateContext(x, y):
        GL.glScale(dx, dy, 1)
