from dataclasses import dataclass
from typing import Any, Callable, Generic, Optional, Type, TypeVar, NewType, Literal, Sequence
from XPPython3.xp_typing import *    
ControlCameraUntilViewChanges: XPLMCameraControlDuration
ControlCameraForever: XPLMCameraControlDuration
def controlCamera(howLong:int=ControlCameraUntilViewChanges,
          controlFunc:Optional[Callable[[list[float], int, Any], int]]=None,
          refCon:Any=None) -> None:
    """
    Reposition camera on next drawing cycle.
      howLong: 1 = until view changes
               2 = forever
      controlFunc(position, isLosingControl, refCon) -> int
          position: list of floats (or None) you'll update with new values
                    [x, y, z, pitch, heading, roll, zoom]
          isLosingControl: 1 -> you are losing control
          refCon: reference constant provided with 'controlCamera' call
          Return 1 to reposition, 0 to surrender control.
      refCon: reference constant passed to call back func().
    """
    ...

def dontControlCamera() -> None:
    """
    Release control of camera.
    """
    ...

def isCameraBeingControlled() -> tuple[int, int]:
    """
    Returns two integer tuple. (isBeingControlled, howLong).
    
    isBeingControlled is 1 when camera is being controlled, 0 otherwise.
    howLong is
      1: Until View Changes
      2: Forever
      value is undefined when isBeingControlled is 0.
    """
    ...

def readCameraPosition() -> tuple[float, float, float, float, float, float, float]:
    """
    Returns tuple of seven floats:
     Index   Value   Meaning
     0,1,2  x,y,z    Camera's position in OpenGL coordinates
     3      pitch    In degrees, 0.0 is flat, positive for nose up.
     4      heading  In degrees, 0.0 is true north.
     5      roll     In degrees, 0.0 is flat, positive for roll right.
     6      zoom     1.0 is normal, 2.0 is 2x zoom (objects appear larger)
    """
    ...

