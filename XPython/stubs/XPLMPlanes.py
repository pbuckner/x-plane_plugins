from typing import Tuple, Callable, Union, Any, Sequence

USER_AIRCRAFT = 0


def setUsersAircraft(path: str) -> None:
    return


def placeUserAtAirport(code: str) -> None:
    return


def placeUserAtLocation(latitude: float, longitude: float, elevation: float, speed: float) -> None:
    return


def countAircraft() -> Tuple[int, int, int]:
    return (int(), int(), int())  #: (total, active, controlling Plugin)


def getNthAircraftModel(index: int) -> Tuple[str, str]:
    return str(), str()  # (model filename, path to model)


def acquirePlanes(aircraft: Union[None, Sequence[str]] = None,
                  callback: Callable[[Any], None] = None,
                  refCon: Any = None) -> int:
    return int()  # 1=you gained access; 0=otherwise


def releasePlanes() -> None:
    return


def setActiveAircraftCount(count: int) -> None:
    return


def setAircraftModel(index: int, path: str) -> None:
    return


def disableAIForPlane(index: int) -> None:
    return
