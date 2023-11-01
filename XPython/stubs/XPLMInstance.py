from typing import Sequence, Tuple, Union
from XPPython3.xp import XPLMInstanceRef, XPLMObjectRef


def createInstance(obj: XPLMObjectRef, datarefs: Union[None, Sequence[str]] = None) -> XPLMInstanceRef:
    return XPLMInstanceRef(0)


def destroyInstance(instance: XPLMInstanceRef) -> None:
    return


def instanceSetPosition(instance: XPLMInstanceRef,
                        position: Tuple[float, float, float, float, float, float],
                        data: Union[None, Sequence[float]] = None):
    return
