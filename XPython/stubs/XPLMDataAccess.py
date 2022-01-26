from typing import Callable, NewType, Any, Union
# XPLMDataTypeID bitfield is used to identify the type of data:
xplmType_Unknown = 0  #: Data of a type the current XPLM doesn't do.
xplmType_Int = 1  #: A single 4-byte integer, native endian.
xplmType_Float = 2  #: A single 4-byte float, native endian.
xplmType_Double = 4  #: A single 8-byte double, native endian.
xplmType_FloatArray = 8  #: An array of 4-byte floats, native endian.
xplmType_IntArray = 16  #: An array of 4-byte integers, native endian.
xplmType_Data = 32  # A: variable block of data.
"""


PUBLISHING YOUR PLUGINS DATA
****************************
SHARING DATA BETWEEN MULTIPLE PLUGINS
*************************************
"""


def XPLMFindDataRef(name: str) -> Union[int, None]:
    return int()


def XPLMGetDataRefTypes(dataRef: int) -> int:
    return int()


def XPLMGetDatai(dataRef: int) -> int:
    return int()


def XPLMSetDatai(dataRef: int, value: int = 0) -> None:
    pass


def XPLMGetDataf(dataRef: int) -> float:
    return float()


def XPLMSetDataf(dataRef: int, value: float = 0.0) -> None:
    pass


def XPLMGetDatad(dataRef: int) -> float:
    return float()


def XPLMSetDatad(dataRef: int, value: float = 0.0) -> None:
    pass


def XPLMGetDatavi(dataRef: int, values: Union[list, None] = None, offset: int = 0, count: int = -1) -> int:
    return int()  # of items


def XPLMSetDatavi(dataRef: int, values: list, offset: int = 0, count: int = -1) -> None:
    pass


def XPLMGetDatavf(dataRef: int, values: Union[list, None] = None, offset: int = 0, count: int = -1) -> int:
    return int()  # of items


def XPLMSetDatavf(dataRef: int, values: list, offset: int = 0, count: int = -1) -> None:
    pass


def XPLMGetDatab(dataRef: int, values: Union[list, None], offset: int = 0, count: int = -1) -> int:
    return int()  # of items


def XPLMSetDatab(dataRef: int, values: list, offset: int = 0, count: int = -1) -> None:
    pass


def XPLMGetDatai_f(refCon: Any) -> int:
    return int()


def XPLMSetDatai_f(refCon: Any, value: int) -> None:
    pass


def XPLMGetDataf_f(refCon: Any) -> float:
    return float()


def XPLMSetDataf_f(refcon: Any, value: float) -> None:
    pass


def XPLMGetDatad_f(refcon: Any) -> float:
    return float()


def XPLMSetDatad_f(refCon: Any, value: float) -> None:
    pass


def XPLMGetDatavi_f(refcon: Any, values: Union[list, None], offset: int, count: int) -> int:
    return int()  # of items


def XPLMSetDatavi_f(refcon: Any, values: list, offset: int, count: int) -> None:
    pass


def XPLMGetDatavf_f(refcon: Any, values: Union[list, None], offset: int, count: int) -> int:
    return int()  # of items


def XPLMSetDatavf_f(refcon: Any, values: list, offset: int, count: int) -> None:
    pass


def XPLMGetDatab_f(refCon: Any, values: Union[list, None], offset: int, count: int) -> int:
    return int()  # of items


def XPLMSetDatab_f(refCon: Any, inValue: list, offset: int, count: int) -> None:
    pass


def XPLMRegisterDataAccessor(name: str, dataType: int = 0, writable: int = -1,
                             readInt: Union[None, Callable[[Any], int]] = None,
                             writeInt: Union[None, Callable[[Any, int], None]] = None,
                             readFloat: Union[None, Callable[[Any], float]] = None,
                             inWriteFloat: Union[None, Callable[[Any, float], None]] = None,
                             readDouble: Union[None, Callable[[Any], float]] = None,
                             writeDouble: Union[None, Callable[[Any, float], None]] = None,
                             readIntArray: Union[None, Callable[[Any, Union[list, None], int, int], int]] = None,
                             writeIntArray: Union[None, Callable[[Any, list, int, int], None]] = None,
                             readFloatArray: Union[None, Callable[[Any, Union[list, None], int, int], int]] = None,
                             writeFloatArray: Union[None, Callable[[Any, list, int, int], None]] = None,
                             readData: Union[None, Callable[[Any, Union[list, None], int, int], int]] = None,
                             writeData: Union[None, Callable[[Any, list, int, int], None]] = None,
                             readRefcon: Any = None,
                             writeRefcon: Any = None) -> int:
    return int()  # XPLMDataRef


def XPLMUnregisterDataAccessor(accessor: int) -> None:
    pass


def XPLMDataChanged_f(refCon: Any) -> None:
    pass


def XPLMShareData(name: str, dataType: int,
                  dataChanged: Union[None, Callable[[Any], None]],
                  refCon: Any) -> int:
    return int()  # 1 on success, 0 if data already exists but is of the wrong type


def XPLMUnshareData(name: str, dataType: int,
                    dataChanged: Union[None, Callable[[Any], None]],
                    refCon: Any) -> int:
    return int()  # 0 if cannot find dataref
