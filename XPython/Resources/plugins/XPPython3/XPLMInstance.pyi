# pylint: disable=unused-argument
# (a stub's parameters are never used -- there is no body)
from typing import Optional, Sequence
from XPPython3.xp_typing import (XPLMCoordinateSpace_t, XPLMDrawInfo_t, XPLMDrawInfoDouble_t,
                                 XPLMInstanceRef, XPLMObjectRef)
CoordSpace_World: XPLMCoordinateSpace_t = XPLMCoordinateSpace_t(0)
CoordSpace_AircraftInterior: XPLMCoordinateSpace_t
CoordSpace_AircraftExterior: XPLMCoordinateSpace_t
CoordSpace_Camera: XPLMCoordinateSpace_t


def createInstance(obj: XPLMObjectRef, dataRefs: Optional[Sequence[str]] = None) -> XPLMInstanceRef:
    """
    Create Instance for object retrieved by loadObject() or loadObjectAsync().

    Provide list of string dataRefs to be registered for this object.
    Returns object instance, to be used with instanceSetPosition().
    """
    ...


def destroyInstance(instance: XPLMInstanceRef) -> None:
    """
    Destroys instance created by createInstance().
    """
    ...


def instanceSetAutoShift(instance: XPLMInstanceRef) -> None:
    """
    Tells X-Plane to move instance location if/when Sim's local coordinate
    system changes.
    """
    ...


def instanceSetPosition(instance: XPLMInstanceRef, position: XPLMDrawInfo_t | XPLMDrawInfoDouble_t | Sequence[float],
                        data: Optional[Sequence[float]] = None) -> None:
    """
    Update position (x, y, z, pitch, heading, roll),
    and all datarefs (<float>, <float>, ...)

    You should provide values for all registered dataref, otherwise they'll
    be set to 0.0. This function is identical to instanceSetPositionDouble
    as Python treats all floating point as doubles.

    """
    ...


def instanceSetPositionDouble(instance: XPLMInstanceRef, position: XPLMDrawInfo_t | XPLMDrawInfoDouble_t | Sequence[float],
                              data: Optional[Sequence[float]] = None) -> None:
    """
    Update position (x, y, z, pitch, heading, roll),
    and all datarefs (<float>, <float>, ...)

    This function is identical to instanceSetPosition, as Python treats
    all floating point as doubles.

    """
    ...


def createInstanceEx(objects: Sequence[XPLMObjectRef | tuple[XPLMObjectRef, float, float, float, float, float, float]],
                     dataRefs: Optional[Sequence[str]] = None, coordinateSpace: XPLMCoordinateSpace_t = CoordSpace_World,
                     aircraftIndex: int = 0, autoShift: int = 0) -> XPLMInstanceRef:
    """
    Create a multi-object instance (superset of createInstance()).

    objects is a sequence, each element being either an XPLMObjectRef (placed
    at the instance origin) or a tuple (obj, x, y, z, pitch, heading, roll)
    giving a fixed local offset. All objects move rigidly together and share
    the single dataRefs list. coordinateSpace is one of the CoordSpace_*
    constants; aircraftIndex (0=user) applies only to the aircraft spaces;
    autoShift (world space only) is like instanceSetAutoShift().

    Returns object instance, to be used with instanceSetPosition().
    """
    ...


def instanceSetCoordinateSpace(instance: XPLMInstanceRef, space: XPLMCoordinateSpace_t, aircraftIndex: int = 0) -> None:
    """
    Change the coordinate space used to interpret positions passed to
    instanceSetPosition(). space is one of the CoordSpace_* constants;
    aircraftIndex (0=user) applies only to the aircraft spaces. X-Plane
    re-expresses the instance's current world location in the new space,
    so the object does not jump.
    """
    ...
