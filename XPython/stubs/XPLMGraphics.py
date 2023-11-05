from typing import Tuple, Sequence, Union
Font_Basic = 0
Font_Proportional = 18


def setGraphicsState(fog: int = 0, numberTexUnits: int = 0, lighting: int = 0, alphaTesting: int = 0,
                     alphaBlending: int = 0, depthTesting: int = 0, depthWriting: int = 0) -> None:
    return


def bindTexture2d(textureID: int, textureUnit: int) -> None:
    return


def generateTextureNumbers(count: int) -> Tuple[int, ]:
    return (int(), )  # list of ints


def worldToLocal(lat: float, lon: float, alt: float = 0.0) -> Tuple[float, float, float]:
    return (float(), float(), float())  # (outX, outY, outZ)


def localToWorld(x: float, y: float, z: float) -> Tuple[float, float, float]:
    return (float(), float(), float())  # (outLatitude, outLongitude, outAltitude)


def drawTranslucentDarkBox(left: int, top: int, right: int, bottom: int) -> None:
    return


def drawString(rgb: Sequence[float] = (1.0, 1.0, 1.0), x: int = 0, y: int = 0, value: str = '',
               wordWrapWidth: Union[None, int] = None, fontID: int = 18) -> None:
    return


def drawNumber(rgb: Sequence[float] = (1.0, 1.0, 1.0), x: int = 0, y: int = 0, value: float = 0.0,
               digits: int = -1, decimals: int = 0, showSign: int = 1, fontID: int = 18) -> None:
    return


def getFontDimensions(fontID: int) -> Tuple[int, int, int]:
    return int(), int(), int()  # (width, height, digitsOnly)


def measureString(fontID: int, string: str) -> float:
    return float()  # width of string in (fractional) pixels
