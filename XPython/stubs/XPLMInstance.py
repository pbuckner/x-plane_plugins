from typing import Sequence, Tuple, Union
from XPPython3.xp import XPLMInstanceRef, XPLMObjectRef

def createInstance(obj: XPLMObjectRef, dataRefs: Union[None, Sequence[str]] = None) -> XPLMInstanceRef: ...

def destroyInstance(instance: XPLMInstanceRef) -> None: ...

def instanceSetPosition(instance: XPLMInstanceRef,
                        position: Tuple[float, float, float, float, float, float],
                        data: Union[None, Sequence[float]] = None) -> None: ...
