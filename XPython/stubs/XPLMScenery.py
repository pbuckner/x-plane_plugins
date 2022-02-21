from typing import Any, Union, Callable
from xp_hinting import XPLMProbeRef, XPLMObjectRef

ProbeY = 0
ProbeHitTerrain = 0
ProbeError = 1
ProbeMissed = 2


def createProbe(probeType: int = ProbeY) -> XPLMProbeRef:
    return XPLMProbeRef(0)


def destroyProbe(probeRef: XPLMProbeRef) -> None:
    return


class PyProbeInfo:
    result = 0  # XPLMProbeResult
    locationX = 0
    locationY = 0
    locationZ = 0
    normalX = 0
    normalY = 0
    normalZ = 0
    velocityX = 0
    velocityY = 0
    velocityZ = 0
    is_wet = 0


def probeTerrainXYZ(probeRef: XPLMProbeRef, x: float, y: float, z: float) -> PyProbeInfo:
    return PyProbeInfo()


def getMagneticVariation(latitude: float, longitude: float) -> float:
    return float()


def degTrueToDegMagnetic(degreesTrue: float = 0.0) -> float:
    return float()


def degMagneticToDegTrue(degreesMagnetic: float = 0.0) -> float:
    return float()


def loadObject(path: str) -> XPLMObjectRef:
    return XPLMObjectRef(0)


def loadObjectAsync(path: str, loaded: Callable[[XPLMObjectRef, Any], None], refCon: Union[None, Any] = None):
    return


def unloadObject(XPLMObjectRef):
    return


def lookupObjects(path: str, latitude: float, longitude: float,
                  enumerator: Callable[[str, Any], None], refCon: Union[None, Any] = None) -> int:
    return int()  # number of objects found
