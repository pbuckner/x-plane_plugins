from typing import Any, Callable, Optional
from XPPython3.xp_typing import *

def loadCursor(path: str) -> XPLMCursorRef:
    """
    Load a cursor from a .png file and return cursor reference.

    Multiple calls with same filename will return same cursor.

    Cursor file must be in RGBA format (with alpha channel).

    Recommended cursor image size is 17x17 or 32x32 pixels to support both
    1x and 2x resolution cursors.
    """
    ...

def setCursor(cursor: XPLMCursorRef) -> None:
    """
    Set the cursor to the given cursor reference.
    """
    ...

def unloadCursor(cursor: XPLMCursorRef) -> None:
    """
    Unload cursor when no longer needed.
    """
    ...
