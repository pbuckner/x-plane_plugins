from dataclasses import dataclass
from typing import Any, Callable, Generic, Optional, Type, TypeVar, NewType, Literal, Sequence
from XPPython3.xp_typing import *    
def createInstance(obj:XPLMObjectRef, dataRefs:Optional[tuple[str]]=None) -> XPLMInstanceRef:
    """
    Create Instance for object retrieved by loadObject() or loadObjectAsync().
    
    Provide list of string dataRefs to be registered for this object.
    Returns object instance, to be used with instanceSetPosition().
    """
    ...

def destroyInstance(instance:XPLMObjectRef) -> None:
    """
    Destroys instance created by createInstance().
    """
    ...

def instanceSetPosition(instance:XPLMObjectRef, position:XPLMDrawInfo_t | tuple[float, float, float, float, float, float], 
          data:Optional[tuple[float]]=None) -> None:
    """
    Update position (x, y, z, pitch, heading, roll), 
    and all datarefs (<float>, <float>, ...)
    
    You should provide values for all registered dataref, otherwise they'll
    be set to 0.0.
    
    """
    ...

