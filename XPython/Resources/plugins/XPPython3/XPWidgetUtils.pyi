# pylint: disable=unused-argument
# (a stub's parameters are never used -- there is no body)
from typing import Any, Optional, Sequence
from XPPython3.xp_typing import XPWidgetID, XPWidgetMessage
NO_PARENT: int
PARAM_PARENT: int


def createWidgets(widgetDefs: Sequence[Sequence[Any]], parentID: Optional[XPWidgetID]) -> None | list[XPWidgetID]:
    """
    This does not work in X-Plane.
    """
    ...


def moveWidgetBy(widgetID: XPWidgetID, dx: int = 0, dy: int = 0) -> None:
    """
    Move widget by amount. +x = right, +y = up
    """
    ...


def fixedLayout(message: XPWidgetMessage, widgetID: XPWidgetID, param1: int, param2: int) -> int:
    """
    Use this as a widgetCallback to have child widgets maintain relative positions

    Seems to be completely useless with X-Plane 11.55+
    """
    ...


def selectIfNeeded(message: XPWidgetMessage, widgetID: XPWidgetID, param1: int, param2: int, eatClick: int = 1) -> int:
    """
    Call within widget Callback to raise widget, if not already

    Seems completely useless with X-Plane 11.55+
    """
    ...


def defocusKeyboard(message: XPWidgetMessage, widgetID: XPWidgetID, param1: int, param2: int, eatClick: int = 1) -> int:
    """
    Send keyboard focus back to X-Plane

    Seems completely useless in X-Plane 11.55
    """
    ...


def dragWidget(message: XPWidgetMessage, widgetID: XPWidgetID, param1: int, param2: int, left: int, top: int, right: int, bottom: int) -> int:
    """
    Add to your widget callback to support drag areas

    (left, top, right, bottom) define area within your widget where, if drag
    is initiated, this callback will cause the widget to move.
    """
    ...
