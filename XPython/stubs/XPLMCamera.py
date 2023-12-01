from typing import Tuple, TypeVar, Any, Union, Callable, List


ControlCameraUntilViewChanges = 1
ControlCameraForever = 2


def controlCamera(howLong: int = ControlCameraUntilViewChanges,
                  controlFunc: Union[None, Callable[[List[int, ], bool, Any], int]] = None,
                  refCon: Any = None) -> None: ...

def dontControlCamera() -> None: ...

def isCameraBeingControlled() -> Tuple[int, int]: ...

def readCameraPosition() -> Tuple[float, float, float, float, float, float, float]: ...
