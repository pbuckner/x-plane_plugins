from typing import Self, Optional
from XPPython3 import xp_typing


class MyWidgetWindow:
    def __init__(self: Self) -> None:
        self.widgetID: Optional[xp_typing.XPWidgetID] = None
        self.widgets: dict = {}
