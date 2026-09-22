# pylint: disable=unused-argument
# (a stub's parameters are never used -- there is no body)
# Hand-maintained stub fragment for names defined in xp.py itself.
#
# generate_xp_pyi.py builds xp.pyi from the XP*.pyi C-module stubs, which know
# nothing about the helpers xp.py adds on top of them.  This file is appended
# verbatim to the generated xp.pyi, so anything declared here survives a
# regeneration.  Add a declaration here whenever you add a module-level name to
# xp.py; the generator warns about any xp.py name that neither source covers.

from contextlib import AbstractContextManager
from typing import Any, Callable, Optional
# "X as X" is the stub re-export form -- a plain import would not make these
# visible as xp.<name> to mypy.
from XPPython3.xp_typing import (FMOD_CHANNEL as FMOD_CHANNEL,  # noqa: F401 (re-exports)
                                 XPLMAudioBus as XPLMAudioBus,
                                 XPLMRetainedDrawing as XPLMRetainedDrawing,
                                 XPLMWeatherInfoClouds_t as XPLMWeatherInfoClouds_t,
                                 XPLMWeatherInfoWinds_t as XPLMWeatherInfoWinds_t)


MsgEnteredVR: int
MsgExitingVR: int
Airport: int

pythonLog: Callable[..., None]
sys_log: Callable[..., None]


def playWaveOnBus(wav: Any, loop: int, audioType: XPLMAudioBus,
                  callback: Optional[Callable[[Any, int], None]] = None,
                  refCon: Any = None) -> None | FMOD_CHANNEL:
    """
    Play an open wave file (wave.Wave_read) on the given audio bus.
    """
    ...


class PluginItem:
    """Attributes of one loaded plugin, looked up by instance or by name."""
    instance: Any
    module_name: str
    name: str
    signature: str
    description: str
    disabled: int
    def __init__(self, key: Any) -> None: ...


def getPluginInstance(signature: str = None) -> Any:
    """
    Return the PythonInterface instance for signature, or for the calling
    plugin when signature is None.
    """
    ...


class Retained:
    """Holds a retained-drawing handle; empty until the with-block exits."""
    handle: Optional[XPLMRetainedDrawing]
    def __init__(self) -> None: ...
    def draw(self) -> None: ...
    def destroy(self) -> None: ...


def weatherUpdateContext(isIncremental: int = 1,
                         updateImmediately: int = 0) -> AbstractContextManager[None]:
    """
    Context manager wrapping beginWeatherUpdate() / endWeatherUpdate().
    """
    ...


def transformContext() -> AbstractContextManager[None]:
    """
    Context manager wrapping transformPush() / transformPop().
    """
    ...


def scissorContext() -> AbstractContextManager[None]:
    """
    Context manager wrapping scissorPush() / scissorPop().
    """
    ...


def setupStencilMask(bits: int, mask: int) -> AbstractContextManager[None]:
    """
    Context manager wrapping beginSetupStencilMask() / endSetupStencilMask().
    """
    ...


def retainedDrawing() -> AbstractContextManager[Retained]:
    """
    Context manager wrapping beginRetainedDrawing() / endRetainedDrawing().
    Yields a Retained instance whose handle is set when the block exits.
    """
    ...
