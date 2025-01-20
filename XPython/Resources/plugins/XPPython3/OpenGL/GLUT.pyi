# pylint: disable=unused-argument
# flake8: noqa

from typing import Any, TypeAlias

GLUTBitmapFont: TypeAlias = Any

GLUT_BITMAP_HELVETICA_10: GLUTBitmapFont
GLUT_BITMAP_HELVETICA_12: GLUTBitmapFont
GLUT_BITMAP_HELVETICA_18: GLUTBitmapFont
GLUT_BITMAP_TIMES_ROMAN_10: GLUTBitmapFont
GLUT_BITMAP_TIMES_ROMAN_24: GLUTBitmapFont

def glutBitmapWidth(font: GLUTBitmapFont, c: int) -> float: ...
def glutBitmapCharacter(font: GLUTBitmapFont, c: int) -> None: ...
