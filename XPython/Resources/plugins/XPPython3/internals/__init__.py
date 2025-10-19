from typing import List, Any
from XPPython3 import xp
from XPPython3.xp_typing import XPLMDataRef, XPLMFlightLoopID


def getDataRefs() -> List[XPLMDataRef]:
    moduleName = xp.getSelfModuleName()
    return [x[18] for x in xp.getDataRefCallbackDict().values() if x[0] == moduleName]


def getFlightLoops() -> List[XPLMFlightLoopID]:
    moduleName = xp.getSelfModuleName()
    return [x[4] for x in xp.getFlightLoopCallbackDict().values() if x[0] == moduleName]


def getCommands() -> List[tuple[Any, Any, Any, Any]]:
    moduleName = xp.getSelfModuleName()
    return [[x[1], x[2], x[3], x[4]] for x in xp.getCommandCallbackDict().values() if x[0] == moduleName]
