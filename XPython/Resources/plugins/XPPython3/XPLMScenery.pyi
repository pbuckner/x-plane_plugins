# pylint: disable=unused-argument
# (a stub's parameters are never used -- there is no body)
from typing import Any, Callable, Optional
from XPPython3.xp_typing import (XPLMObjectRef, XPLMProbeInfo_t, XPLMProbeRef,
                                 XPLMProbeResult, XPLMProbeType)
ProbeY: XPLMProbeType = XPLMProbeType(0)
ProbeHitTerrain: XPLMProbeResult
ProbeError: XPLMProbeResult
ProbeMissed: XPLMProbeResult


def createProbe(probeType: XPLMProbeType = ProbeY) -> XPLMProbeRef:
    """
    Return a probeRef
    """
    ...


def destroyProbe(probe: XPLMProbeRef) -> None:
    """
    Destroy a probeRef
    """
    ...


def probeTerrainXYZ(probe: XPLMProbeRef, x: float, y: float, z: float) -> XPLMProbeInfo_t:
    """
    Probe terrain using probeRef at (x, y, z) location

    Object returned as attributes:
      .result:    0=Hit, 1=Error, 2=Missed
      .locationX,
      .locationY,
      .locationZ: OpenGL point hit by probe
      .normalX,
      .normalY,
      .normalZ:   Normal vector (e.g. slope),
      .velocityX,
      .velocityY,
      .velocityZ: Velocity vector (e.g., meter/sec) of movement
      .is_wet:    1=we hit water
    """
    ...


def getMagneticVariation(latitude: float, longitude: float) -> float:
    """
    Magnetic declination at point
    """
    ...


def degTrueToDegMagnetic(degrees: float = 0.0) -> float:
    """
    Convert degrees True to degrees Magnetic, at user's current location
    """
    ...


def degMagneticToDegTrue(degrees: float = 0.0) -> float:
    """
    Convert degrees Magnetic to degrees True, at user's current location
    """
    ...


def loadObject(path: str) -> XPLMObjectRef:
    """
    Load OBJ file from path, returning objectRef

    Path may be absolute, or relative X-Plane Root
    """
    ...


def loadObjectAsync(path: str, loaded: Callable[[XPLMObjectRef, Any], None], refCon: Any = None) -> None:
    """
    Loads OBJ asynchronously, calling callback on completion.

    Callback signature is loaded(objectRef, refCon)
    Object path is absolute or relative X-Plane root.
    """
    ...


def unloadObject(objectRef: XPLMObjectRef) -> None:
    """
    Unloads objectRef
    """
    ...


def lookupObjects(path: str, latitude: float = 0.0, longitude: float = 0.0,
                  enumerator: Optional[Callable[[str, Any], None]] = None, refCon: Any = None) -> int:
    """
    Converts virtual path to file paths, calling enumerator with info

    Path is virtual path, which may have zero or more matching file paths
    in library. Count of results is returned by lookupObjects().
    For each item found, enumerator(path, refCon) is called.
    """
    ...


def getObjects(path: str, latitude: float = 0.0, longitude: float = 0.0) -> tuple[XPLMObjectRef | None]:
    """
    Converts virtual path to tuple of XPLMObjectRef.

    Path is virtual path, which may have zero or more matching file paths
    in library. Each file path is converted to XPLMObjectRef, and resulting
    tuple returned. Note that objects which fail to load will be returned
    as None within the tuple rather than XPLMObjectRef. Remember to unload
    each object when finished to avoid leaks.
    """
    ...
