from typing import Tuple, TypeVar


def XPLMCameraControl_f(outCameraPosition: list, inIsLosingControl: bool, inRefcon: object) -> int:
    """
    You use an XPLMCameraControl function to provide continuous control over
    the camera. You are passed outCameraPosition list in which to put the new camera
    position; modify it and return 1 to reposition the camera. Return 0 to
    surrender control of the camera; camera control will be handled by X-Plane
    on this draw loop. The contents of the outCameraPosition structure as you are called are
    undefined.

    Note: returning 0 means you are not changing camera position, but you will still get another
    callback during the name frame. To release control completely, call :py:func:`XPLMDontControlCamera`.

    If X-Plane is taking camera control away from you, this function will be
    called with inIsLosingControl set to 1 and outCameraPosition None.

    :param outCameraPosition: list you update with new value
    :type outCameraPosition: XPLMCameraPosition_t or None
    :param inIsLosingControl: 1 if you are losing control
    :type inIsLosingControl: int
    :param inRefcon: reference constant provided with XPLMControlCamera
    :type inRefcon: object
    :rtype: int, 1 to reposition, 0 to surrender control

    """
    return int


XPLMCameraControlDuration = int
#: Control the camera until the user picks a new view.
xplm_ControlCameraUntilViewChanges = 1
#: Control the camera until your plugin is disabled or another plugin forcably
#: takes control.
xplm_ControlCameraForever = 2


def XPLMControlCamera(inHowLong: XPLMCameraControlDuration, inControlFunc: XPLMCameraControl_f, inRefcon: object) -> None:
    """
    This function repositions the camera on the next drawing cycle. You must
    pass a non-null control function. Specify in `XPLMCameraControlDuration` inHowLong how long you'd like
    control (indefinitely or until a key is pressed).

    :param inHowLong: int enumuration, how long you'd like control
    :type inHowLong: XPLMCameraControlDuration
    :param inControlFunc: your callback
    :type inControlFunc: XPLMCameraControl_f
    :param inRefcon: any python object
    """


def XPLMDontControlCamera() -> None:
    """
    This function stops you from controlling the camera. If you have a camera
    control function, it will not be called with an inIsLosingControl flag.
    X-Plane will control the camera on the next cycle.

    For maximum compatibility you should not use this routine unless you are in
    posession of the camera.
    """


def XPLMIsCameraBeingControlled() -> Tuple[int, int]:
    """
    Return a tuple
    The first tuple element is 1 if the camera is being controlled, 0 otherwise.
    If the first element is nonzero, the second one contains `XPLMCameraControlDuration`
    of the current camera control.

    :rtype: [isCameraControlled, :any:`XPLMCameraControlDuration`]
    """
    return Tuple[int, int]


#: Camera position is described using a list of seven floats:
#: `[x, y, z, pitch, heading, roll, roll, zoom]`
#:
#: X, Y, and Z are the camera's position in OpenGL coordinates; pitch, roll, and yaw are
#: rotations from a camera facing flat north in degrees. Positive pitch means
#: nose up, positive roll means roll right, and positive yaw means yaw right,
#: all in degrees. Zoom is a zoom factor, with 1.0 meaning normal zoom and 2.0
#: magnifying by 2x (objects appear larger).
XPLMCameraPosition_t = TypeVar('XPLMCameraPosition_t', float, float, float, float, float, float)


def XPLMReadCameraPosition() -> XPLMCameraPosition_t:
    """Read current camera position.
    Returns XPLMCameraPosition_t list [x, y, z, pitch, heading, roll, zoom].

    :rtype: :data:`XPLMCameraPosition_t`
    """
    return XPLMCameraPosition_t
