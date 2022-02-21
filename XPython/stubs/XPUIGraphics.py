from typing import Tuple

Window_Help = 0
Window_MainWindow = 1
Window_SubWindow = 2
Window_Screen = 4
Window_ListView = 5
Element_TextField = 6
Element_CheckBox = 9
Element_CheckBoxLit = 10
Element_WindowCloseBox = 14
Element_WindowCloseBoxPressed = 15
Element_PushButton = 16
Element_PushButtonLit = 17
Element_OilPlatform = 24
Element_OilPlatformSmall = 25
Element_Ship = 26
Element_ILSGlideScope = 27
Element_MarkerLeft = 28
Element_Airport = 29
Element_Waypoint = 30
Element_NDB = 31
Element_VOR = 32
Element_RadioTower = 33
Element_AircraftCarrier = 34
Element_Fire = 35
Element_MarkerRight = 36
Element_CustomObject = 37
Element_CoolingTower = 38
Element_SmokeStack = 39
Element_Building = 40
Element_PowerLine = 41
Element_CopyButtons = 45
Element_CopyButtonsWithEditingGrid = 46
Element_EditingGrid = 47
Element_ScrollBar = 48
Element_VORWithCompassRose = 49
Element_Zoomer = 51
Element_TextFieldMiddle = 52
Element_LittleDownArrow = 53
Element_LittleUpArrow = 54
Element_WindowDragBar = 61
Element_WindowDragBarSmooth = 62
Track_ScrollBar = 0
Track_Slider = 1
Track_Progress = 2


def drawWindow(left: int, bottom: int, right: int, top: int, style: int = 1) -> None:
    return


def getWindowDefaultDimensions(style: int) -> Tuple[int, int]:
    return int(), int()  # width, height


def drawElement(left: int, bottom: int, right: int, top: int, style: int, lit: int = 0) -> None:
    return


def getElementDefaultDimensions(style: int) -> Tuple[int, int, int]:
    return int(), int(), int()  # width, height, canBeLit


def drawTrack(left: int, bottom: int, right: int, top: int,
              minValue: int, maxValue: int, value: int,
              style: int, lit: int = 0) -> None:
    return


def getTrackDefaultDimensions(style: int) -> Tuple[int, int]:
    return int(), int()  # width, canBeLit


class TrackMetrics:
    isVertical = 0
    downBtnSize = 0
    downPageSize = 0
    thumbSize = 0
    upPageSize = 0
    upBtnSize = 0


def getTrackMetrics(left: int, bottom: int, right: int, top: int,
                    minValue: int, maxValue: int, value: int,
                    style: int) -> TrackMetrics:
    return TrackMetrics()
