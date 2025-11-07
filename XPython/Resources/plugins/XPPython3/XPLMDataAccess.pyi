from dataclasses import dataclass
from typing import Any, Callable, Generic, Optional, Type, TypeVar, NewType, Literal, Sequence
from XPPython3.xp_typing import *    
Type_Unknown: XPLMDataTypeID
Type_Int: XPLMDataTypeID
Type_Float: XPLMDataTypeID
Type_Double: XPLMDataTypeID
Type_FloatArray: XPLMDataTypeID
Type_IntArray: XPLMDataTypeID
Type_Data: XPLMDataTypeID
def findDataRef(name:str) -> XPLMDataRef:
    """
    Looks up string name of data ref and returns dataRef code
    to be used with get and set data ref functions,
    or None, if name cannot be found.
    """
    ...

def canWriteDataRef(dataRef:XPLMDataRef) -> bool:
    """
    Returns True if dataRef is writable, False otherwise. Also
    returns False if provided dataRef is None.
    """
    ...

def isDataRefGood(dataRef:XPLMDataRef) -> bool:
    """
    (Deprecated, do not use.)
    """
    ...

def getDataRefTypes(dataRef:XPLMDataRef) -> int:
    """
    Returns or'd values of data type(s) supported by dataRef.
       1 Type_Int
       2 Type_Float
       4 Type_Double
       8 Type_FloatArray
      16 Type_IntArray
      32 Type_Data
    """
    ...

def getDatai(dataRef:XPLMDataRef) -> int:
    """
    Returns integer value for dataRef.
    """
    ...

def setDatai(dataRef:XPLMDataRef, value:int=0) -> None:
    """
    Sets integer value for dataRef.
    """
    ...

def getDataf(dataRef:XPLMDataRef) -> float:
    """
    Returns float value for dataRef.
    """
    ...

def setDataf(dataRef:XPLMDataRef, value:float=0.0) -> None:
    """
    Sets float value for dataRef.
    """
    ...

def getDatad(dataRef:XPLMDataRef) -> float:
    """
    Returns double value for dataRef (as a python float)
    """
    ...

def setDatad(dataRef:XPLMDataRef, value:float=0.0) -> None:
    """
    Sets double value for dataRef.
    """
    ...

def getDatavi(dataRef:XPLMDataRef, values:Optional[list[int]]=None, offset:int=0, count:int=-1) -> int:
    """
    Get integer array value for dataRef.
    
    If values is None, return number of elements in the array (only).
    Otherwise, values should be a list into which will be copied elements
    from the dataRef, starting at offset, and continuing for count # of elements.
    If count is negative, or unspecified, all elements (relative offset) are copied.
    
    Returns the number of elements copied.
    """
    ...

def setDatavi(dataRef:XPLMDataRef, values:list[int], offset:int=0, count:int=-1) -> None:
    """
    Set integer array value for dataRef.
    
    values is a list of integers, to be written into dataRef starting
    at offset. Up to count values are written.
    
    If count is negative (or not provided), all values in the list are copied.
    It is an error for count to be greater than the length of the list.
    
    No return value.
    """
    ...

def getDatavf(dataRef:XPLMDataRef, values:Optional[list[float]]=None, offset:int=0, count:int=-1) -> int:
    """
    Get float array value for dataRef.
    
    If values is None, return number of elements in the array (only).
    Otherwise, values should be a list into which will be copied elements
    from the dataRef, starting at offset, and continuing for count # of elements.
    If count is negative, or unspecified, all elements (relative offset) are copied.
    
    Returns the number of elements copied.
    """
    ...

def setDatavf(dataRef:XPLMDataRef, values:list[float], offset:int=0, count:int=-1) -> None:
    """
    Set float array value for dataRef.
    
    values is a list of floats, to be written into dataRef starting
    at offset. Up to count values are written.
    
    If count is negative (or not provided), all values in the list are copied.
    It is an error for count to be greater than the length of the list.
    
    No return value.
    """
    ...

def getDatab(dataRef:XPLMDataRef, values:Optional[list[int]]=None, offset:int=0, count:int=-1) -> int:
    """
    Get byte array value for dataRef.
    
    If values is None, return number of elements in the array (only).
    Otherwise, values should be a list into which will be copied elements
    from the dataRef, starting at offset, and continuing for count # of elements.
    If count is negative, or unspecified, all elements (relative offset) are copied.
    
    See also getDatas().
    
    Returns the number of elements copied.
    """
    ...

def setDatab(dataRef:XPLMDataRef, values:list[int] | bytes, offset:int=0, count:int=-1) -> None:
    """
    Set byte array value for dataRef.
    
    values is a list of bytes, to be written into dataRef starting
    at offset. Up to count values are written.
    
    If count is negative (or not provided), all values in the list are copied.
    It is an error for count to be greater than the length of the list.
    
    See also setDatas().
    
    No return value.
    """
    ...

def getDatas(dataRef:XPLMDataRef, offset:int=0, count:int=-1) -> str | None:
    """
    Returns string value for dataRef.
    
    String is the first null-terminated sequence found in the byte-array
    dataRef, starting at offset. If count is given, string returned is
    restricted to count length (not including a null byte).
    
    Note not all byte-array dataRefs are strings: be sure the requested
    dataRef is storing character information. Otherwise use getDatab().
    """
    ...

def setDatas(dataRef:XPLMDataRef, value:str, offset:int=0, count:int=-1) -> None:
    """
    Set byte array to string value for dataRef.
    
    value is a python unicode string (capable of being encoded as 'UTF-8').
    String is written into the dataRef starting at offset. Up to count
    characters are written. If count is more than len(value), the written
    values are padded with zeros ('\\x00') up to count.
    
    If count is negative (or not provided), value is copied AND the
    remaining length of the dataRef is zero-filled. Use count to limit
    the amount of padding.
    
    If len(value) is greater than existing dataRef value, and count is not
    specified, the underlying dataRef is NOT extended to accommodate the
    full string. Instead the string is copied upto the end of the existing
    data. To extend the underlying dataRef, provide a larger value for count.
    
    Caution: extend dataRef only if the underlying dataRef is implemented in 
    python. Attempting to extend non-python dataRefs will cause the sim to
    crash.
    
    No return value.
    """
    ...

def registerDataAccessor(name:str, dataType:int=Type_Unknown, writable:int=-1, 
          readInt:Optional[Callable[[Any], int]]=None, writeInt:Optional[Callable[[Any, int], None]]=None, 
          readFloat:Optional[Callable[[Any], float]]=None, writeFloat:Optional[Callable[[Any, float], None]]=None, 
          readDouble:Optional[Callable[[Any], float]]=None, writeDouble:Optional[Callable[[Any, float], None]]=None, 
          readIntArray:Optional[Callable[[Any, list[int], int, int], int]]=None, writeIntArray:Optional[Callable[[Any, list[int], int, int], None]]=None, 
          readFloatArray:Optional[Callable[[Any, list[float], int, int], int]]=None, writeFloatArray:Optional[Callable[[Any, list[float], int, int], None]]=None, 
          readData:Optional[Callable[[Any, list[int], int, int], int]]=None, writeData:Optional[Callable[[Any, list[int], int, int], None]]=None, 
          readRefCon:Any=None, writeRefCon:Any=None) -> XPLMDataRef:
    """
    Register data accessors for provided string name.
    
    Provide one or more read/write callback functions which implement
    get/set access. If dataType is Type_Unknown, or writable is -1, we'll
    calculate their value to match provided callbacks.
    
    Two optional refCon are available, to be passed to your get/set functions.
    
    * Scalar get callback functions take single (refCon) parameter 
      and return the value.
    * Scalar set callback functions take (refCon, value) parameters 
      with no return.
    * Vector gets take (refCon, values, offset, count), return # elements
      copied into values.
    * Vector sets take (refCon, values, offset, count), with no return
    """
    ...

def unregisterDataAccessor(accessor:XPLMDataRef) -> None:
    """
    Unregisters data accessor.
    """
    ...

def shareData(name:str, dataType:int, 
          dataChanged:Optional[Callable[[Any], None]]=None, refCon:Any=None) -> int:
    """
    Create shared data ref with provided name and dataType.
    
    Optionally provide a callback function which will be called whenever
    this data ref has been changed.
    
    Callback takes single (refCon) parameter
    
    Returns 1 on success 0 otherwise.
    """
    ...

def countDataRefs() -> int:
    """
    Returns the total number of datarefs that have been registered in X-Plane.
    """
    ...

def getDataRefsByIndex(offset:int=0, count:int=1) -> None | list[XPLMDataRef]:
    """
    Returns list of dataRefs, each similar to return from xp.findDataRef().
    Use xp.getDataRefInfo() to access information about the dataref.
    As a special case, count=-1 returns all datarefs starting from offset to the end.
    
    CAUTION: requesting datarefs greater than countDataRefs() returns garbage. If you
    try to use these, you may crash the sim.
    """
    ...

def getDataRefInfo(dataRef:XPLMDataRef) -> XPLMDataRefInfo_t:
    """
    Return DataRefInfo object for provided dataRef.
    
    XPLMDataRefInfo_t object is .name, .type, .writable, .owner
      recall type is a bitfield, see xp.getDataRefTypes()
    """
    ...

def unshareData(name:str, dataType:int, dataChanged:Optional[Callable[[Any], None]]=None, refCon:Any=None) -> int:
    """
    Unshare data. If dataChanged function was provided with initial shareData()
    the callback will no longer be called on data changes.
    All parameter values must match those provided with shareData()
    in order to be successful.

    Returns 1 on success, 0 otherwise
    """
    ...

def getDataRefCallbackDict() -> dict[int, tuple[str, str, int, int, Any, Any, Any, Any, Any, Any, Any, Any, Any, Any, Any, Any]]:
    """
    Copy of internal DataRefInfo
    """
    ...

def getSharedDataRefCallbackDict() -> dict[int, tuple[str, str, int, Any, Any]]:
    """
    Copy of internal Shared DataRefs Dict
    """
    ...

