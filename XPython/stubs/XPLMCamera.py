from typing import Tuple, TypeVar, Any, Union, Callable, List


ControlCameraUntilViewChanges = 1
ControlCameraForever = 2


def controlCamera(howLong: int = ControlCameraUntilViewChanges,
                  controlFunc: Union[None, Callable[[List[int, ], bool, Any], int]] = None,
                  refCon: Any = None) -> None:
    return


def dontControlCamera() -> None:
    return


def isCameraBeingControlled() -> Tuple[int, int]:
    return (int(), int())


def readCameraPosition() -> Tuple[float, float, float, float, float, float]:
    return (0.0, 0.0, 0.0, 0.0, 0.0, 0.0)
