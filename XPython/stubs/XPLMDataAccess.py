from xp_hinting import *
from typing import Callable, NewType, Any, Union, List
# XPLMDataTypeID bitfield is used to identify the type of data:
Type_Unknown = 0  #: Data of a type the current XPLM doesn't do.
Type_Int = 1  #: A single 4-byte integer, native endian.
Type_Float = 2  #: A single 4-byte float, native endian.
Type_Double = 4  #: A single 8-byte double, native endian.
Type_FloatArray = 8  #: An array of 4-byte floats, native endian.
Type_IntArray = 16  #: An array of 4-byte integers, native endian.
Type_Data = 32  # A: variable block of data.


def findDataRef(name: str) -> Union[int, None]:
    return int()


def getDataRefTypes(dataRef: int) -> int:
    return int()


def getDatai(dataRef: int) -> int:
    return int()


def setDatai(dataRef: int, value: int = 0) -> None:
    return


def getDataf(dataRef: int) -> float:
    return float()


def setDataf(dataRef: int, value: float = 0.0) -> None:
    return


def getDatas(dataRef: int, offset: int = 0, count: int = -1) -> str:
    return str()


def setDatas(dataRef: int, value: str, offset: int = 0, count: int = -1) -> None:
    return


def getDatad(dataRef: int) -> float:
    return float()


def setDatad(dataRef: int, value: float = 0.0) -> None:
    return


def getDatavi(dataRef: int, values: Union[list, None] = None, offset: int = 0, count: int = -1) -> int:
    return int()  # of items


def setDatavi(dataRef: int, values: list, offset: int = 0, count: int = -1) -> None:
    return


def getDatavf(dataRef: int, values: Union[list, None] = None, offset: int = 0, count: int = -1) -> int:
    return int()  # of items


def setDatavf(dataRef: int, values: list, offset: int = 0, count: int = -1) -> None:
    return


def getDatab(dataRef: int, values: Union[list, None], offset: int = 0, count: int = -1) -> int:
    return int()  # of items


def setDatab(dataRef: int, values: list, offset: int = 0, count: int = -1) -> None:
    return


def getDatai_f(refCon: Any) -> int:
    return int()


def setDatai_f(refCon: Any, value: int) -> None:
    return


def getDataf_f(refCon: Any) -> float:
    return float()


def setDataf_f(refcon: Any, value: float) -> None:
    return


def getDatad_f(refcon: Any) -> float:
    return float()


def setDatad_f(refCon: Any, value: float) -> None:
    return


def getDatavi_f(refcon: Any, values: Union[list, None], offset: int, count: int) -> int:
    return int()  # of items


def setDatavi_f(refcon: Any, values: list, offset: int, count: int) -> None:
    return


def getDatavf_f(refcon: Any, values: Union[list, None], offset: int, count: int) -> int:
    return int()  # of items


def setDatavf_f(refcon: Any, values: list, offset: int, count: int) -> None:
    return


def getDatab_f(refCon: Any, values: Union[list, None], offset: int, count: int) -> int:
    return int()  # of items


def setDatab_f(refCon: Any, inValue: list, offset: int, count: int) -> None:
    return


def registerDataAccessor(name: str, dataType: int = 0, writable: int = -1,
                         readInt: Union[None, Callable[[Any], int]] = None,
                         writeInt: Union[None, Callable[[Any, int], None]] = None,
                         readFloat: Union[None, Callable[[Any], float]] = None,
                         writeFloat: Union[None, Callable[[Any, float], None]] = None,
                         readDouble: Union[None, Callable[[Any], float]] = None,
                         writeDouble: Union[None, Callable[[Any, float], None]] = None,
                         readIntArray: Union[None, Callable[[Any, Union[List[int, ], None], int, int], int]] = None,
                         writeIntArray: Union[None, Callable[[Any, list, int, int], None]] = None,
                         readFloatArray: Union[None, Callable[[Any, Union[list, None], int, int], int]] = None,
                         writeFloatArray: Union[None, Callable[[Any, list, int, int], None]] = None,
                         readData: Union[None, Callable[[Any, Union[list, None], int, int], int]] = None,
                         writeData: Union[None, Callable[[Any, list, int, int], None]] = None,
                         readRefcon: Any = None,
                         writeRefcon: Any = None) -> int:
    return int()  # XPLMDataRef


def unregisterDataAccessor(accessor: int) -> None:
    return


def dataChanged_f(refCon: Any) -> None:
    return


def shareData(name: str, dataType: int,
              dataChanged: Union[None, Callable[[Any], None]] = None,
              refCon: Any = None) -> int:
    return int()  # 1 on success, 0 if data already exists but is of the wrong type


def unshareData(name: str, dataType: int,
                dataChanged: Union[None, Callable[[Any], None]] = None,
                refCon: Any = None) -> int:
    return int()  # 0 if cannot find dataref
