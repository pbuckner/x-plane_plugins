from XPPython3.xp_typing import XPLMFontID
from .fonts import Faces, Font
from .colors import RGBColor, Colors


def loadFont(fontName: str | XPLMFontID, size: int = 0) -> Font:
    return Faces.load_font(fontName, size)


def measureText(fontID: Font, text: str) -> float:
    return fontID.measure_text(text)


def drawText(fontID: Font, x: float, y: float, text: str = None,
             alignment: str = 'L', color: RGBColor = Colors['white']) -> float:
    return fontID.draw_text(x, y, text, alignment, color)  # type: ignore
