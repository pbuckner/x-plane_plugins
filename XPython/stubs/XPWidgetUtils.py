from typing import Any
from xp_hinting import XPLMWidgetID

NO_PARENT    = -1
PARAM_PARENT = -2


def createWidgets(inWidgetDefs,
                  inCount,
                  inParamParent,
                  ioWidgets):
    pass


def moveWidgetBy(widgetID: XPLMWidgetID, deltaX: int = 0, deltaY: int = 0) -> None:
    return


def fixedLayout(message: int, widgetID: XPLMWidgetID, param1: Any, param2: Any) -> int:
    return int()


def selectIfNeeded(message: int, widgetID: XPLMWidgetID, param1: Any, param2: Any, eatClick: int = 1) -> int:
    return int()


def defocusKeyboard(message: int, widgetID: XPLMWidgetID, param1: Any, param2: Any, eatClick: int = 1) -> int:
    return int()


def dragWidget(message: int,
               widgetID: XPLMWidgetID, param1: Any, param2: Any, left: int, top: int, right: int, bottom: int) -> int:
    return int()

