from typing import Sequence, Tuple, Union
from xp_hinting import XPLMObjectRef, XPLMInstanceRef


def createInstance(obj: XPLMObjectRef, datarefs: Sequence[str]) -> XPLMInstanceRef:
    return XPLMInstanceRef(0)


def destroyInstance(instance: XPLMInstanceRef) -> None:
    return


def instanceSetPosition(instance: XPLMInstanceRef,
                        position: Tuple[float, float, float, float, float, float],
                        data: Union[None, Sequence[float]] = None):
    return
