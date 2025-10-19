from __future__ import annotations
from typing import Any, Callable, Union, List, Iterable, SupportsIndex, overload, Optional, Self, Literal, Sequence, Dict
from itertools import takewhile
from XPPython3 import xp
import re

# (getSelfModuleName was added to XPPython 4.6 & allows this code to distinguish between "my" datarefs and "other" python
# plugin datarefs... without it, all python datarefs appear to be "mine" & therefore read/writeable)
try:
    getSelfModuleName = xp.getSelfModueName
except AttributeError:
    def getSelfModuleName() -> Optional[str]:
        return None


_DataRefs: Dict[str, 'DataRef'] = {}


class DataRef:
    # ----------------------
    # DataRef holds information about a dataref
    # (and, if it's "ours", its current value).
    # If the dataref is not ours, we gather information
    # about it from X-Plane.
    # ----------------------
    def __init__(self: Self, name: str, make: bool = False, callback: Union[Callable[['DataRef'], None], Literal[False]] = None,
                 dataType: int = 0, dim: int = 512, dataRefType: Optional[str] = None) -> None:
        self.name: str = name
        self._ours: bool = False
        self._our_value: Any = None
        self._isarray: bool = self.name.endswith(']')
        self._dref_name: str = self.name.split('[')[0] if self._isarray else name  # provided "name", but without [] if present
        self._count: int = 1
        self._notify: Optional[Callable[[Optional[DataRef]], None]] = None
        self._dim: int = dim
        self._dataRefType: Optional[str] = None  # 'number', 'array[3]', 'string'. These are "lua" types provided on creation
        self._readonly: bool = callback is False  # (only applies if this is 'ours')
        self._owning_plugin: Optional[str] = None  # this allows us to set read-only datarefs, if we create them.
        self._index: int = 0
        try:
            if self._isarray:
                m = re.search(r'\[([0-9]+)\]$', self.name)
                if m:
                    self._index = int(m.group(1))
                else:
                    raise AttributeError
        except AttributeError as e:
            raise ValueError("Array dataref missing number within []") from e

        self.dref: Any = xp.findDataRef(self._dref_name)  # None if not found
        self.types: List[str] = []  # string version of X-Plane datarefs types ('int', 'float', 'double', 'float_array', ...)

        # we didn't find an existing dataref, so we try to create one
        if make and self.dref is None:
            self._owning_plugin = getSelfModuleName()
            if callable(callback):
                self._notify = callback
            self._ours = True
            self._dataRefType = dataRefType
            if dataType & (xp.Type_FloatArray | xp.Type_IntArray | xp.Type_Data):
                self._our_value = [0, ] * dim
            elif dataType & (xp.Type_Int | xp.Type_Float | xp.Type_Double):
                self._our_value = 0
            else:
                raise RuntimeError(f"bad dataType/unknown {dataType=}")
            self.dref = xp.registerDataAccessor(name, dataType,
                                                writable=not self._readonly,
                                                readInt=self.get_int,
                                                readFloat=self.get_float,
                                                readDouble=self.get_float,
                                                readIntArray=self.get_int_array,
                                                readFloatArray=self.get_float_array,
                                                readData=self.get_array,
                                                writeInt=None if self._readonly else self.set_int,
                                                writeFloat=None if self._readonly else self.set_float,
                                                writeDouble=None if self._readonly else self.set_float,
                                                writeIntArray=None if self._readonly else self.set_array,
                                                writeFloatArray=None if self._readonly else self.set_array,
                                                writeData=None if self._readonly else self.set_array)

            for sig in ('com.leecbaker.datareftool', 'xplanesdk.examples.DataRefEditor'):
                dre = xp.findPluginBySignature(sig)
                if dre != xp.NO_PLUGIN_ID:
                    xp.sendMessageToPlugin(dre, 0x01000000, self._dref_name)

        if self.dref is None:
            raise ValueError(f"Unknown dataref {name}")

        # We either found an existing, or we successfully created it (e.g., "self._ours")
        _types: int = xp.getDataRefTypes(self.dref)
        if _types & xp.Type_Int:
            self.types.append('int')
        if _types & xp.Type_Float:
            self.types.append('float')
        if _types & xp.Type_Double:
            self.types.append('double')
        if _types & xp.Type_FloatArray:
            self._isarray = True
            if self._index == 0:
                self._count = -1
            self.types.append('float_array')
            if not self._ours:
                self._dim = xp.getDatavf(self.dref)
        if _types & xp.Type_IntArray:
            self._isarray = True
            if self._index == 0:
                self._count = -1
            self.types.append('int_array')
            if not self._ours:
                self._dim = xp.getDatavi(self.dref)
        if _types & xp.Type_Data:
            self._isarray = True
            if self._index == 0:
                self._count = -1
            self.types.append('data')
            if not self._ours:
                self._dim = xp.getDatab(self.dref)

        _DataRefs[self.name] = self

    def __len__(self: Self) -> int:
        if self._isarray:
            return self._dim
        raise TypeError(f"dataref of of type '{self.types}' has no len()")

    def __str__(self: Self) -> str:
        return f"DataRef: '{self.name} [{', '.join(self.types)}]'"

    # ----------------------------------
    # value @property & @value.setter
    # do the main work -- accessing the dataref
    # in X-Plane (or locally).
    # ----------------------------------
    @property
    def bytes(self: Self) -> Union[int, 'DList']:
        """
        "getting" value of dataref:
        If it's 'our' dataref, simply return the value

        If it's X-Plane dataref, do the appropriate query and return value
        """
        if self._ours:
            if 'data' in self.types:
                return DList(self, list(self._our_value), as_bytes=True)
            return self._our_value
        if not self.types:
            raise ValueError(f"Unknown type for {self.name}")
        if 'data' in self.types and self._isarray:
            # e.g., sim/cockpit2/radios/indicators/fms_cdu1_style_line0 - line15
            # ... these are 8-bit data fields rather than strings: data = ref.value(True)
            byte_values: List[int] = []
            xp.getDatab(self.dref, byte_values, offset=self._index, count=self._count)
            return byte_values[0] if self._count == 1 else DList(self, byte_values, as_bytes=True)
        raise ValueError(f".bytes property not supported for this type {self.name}")

    @bytes.setter
    def bytes(self, value: Union[List[int], bytes, bytearray]) -> None:
        if 'data' not in self.types:
            raise ValueError(f".bytes property not supported for this type {self.name}")
        if self._ours:
            if self._readonly and self._owning_plugin != getSelfModuleName():
                return  # mimic X-Plane behavior... no error when trying to set a read-only dataref
            self._our_value[0:min(self._dim, len(value))] = value[0:self._dim]
            if self._notify:
                self._notify(self)
        else:
            xp.setDatab(self.dref, value, offset=self._index, count=self._count)

    @property
    def value(self: Self) -> Union[int, float, str, List[Union[int, float]], 'DList']:
        """
        "getting" value of dataref:
        If it's 'our' dataref, simply return the value

        If it's X-Plane dataref, do the appropriate query and return value
        """
        if self._ours:
            if 'data' in self.types:
                return bytearray([x for x in takewhile(lambda x: x != 0, self._our_value)]).decode('utf-8')
            else:
                return DList(self, self._our_value) if self._isarray else self._our_value
        if not self.types:
            raise ValueError(f"Unknown type for {self.name}")
        # we'll "prefer" double value over float value over integer value
        # where all are supported
        if 'double' in self.types:
            return xp.getDatad(self.dref)
        if 'float' in self.types:
            return xp.getDataf(self.dref)
        if 'int' in self.types:
            return xp.getDatai(self.dref)
        if 'float_array' in self.types and self._isarray:
            float_values: List[float] = []
            xp.getDatavf(self.dref, float_values, offset=self._index, count=self._count)
            return float_values[0] if self._count == 1 else DList(self, float_values)
        if 'int_array' in self.types and self._isarray:
            int_values: List[int] = []
            xp.getDatavi(self.dref, int_values, offset=self._index, count=self._count)
            return int_values[0] if self._count == 1 else DList(self, int_values)
        if 'data' in self.types and self._isarray:
            return xp.getDatas(self.dref, offset=self._index, count=self._count)
            # return values[0] if self._count == 1 else DList(self, values)
        raise ValueError(f"Unable to get value for {self.name}")

    @value.setter
    def value(self, value: Any) -> None:
        if self._ours:
            if self._readonly and self._owning_plugin != getSelfModuleName():
                return  # mimic X-Plane behavior... no error when trying to set a read-only dataref
            if 'data' in self.types:
                # because we allow strings to NOT match length of 'array', we need to zero out
                # values longer than string value (e.g., foo.value = 'ab' results in [98, 99, 0, 0, 0...]
                # print(f"value setter Before {self._our_value} [0:{min(self._dim, len(value))}] {self._dim=}, {len(value)=}")
                self._our_value = bytearray(self._dim)
                value_l = min(self._dim, len(value))
                self._our_value[0:value_l] = bytearray(value.encode('utf-8'))[0:self._dim]
                # print(f"value setter after {self._our_value}")
            elif 'float_array' in self.types or 'int_array' in self.types:
                self._our_value[0:min(self._dim, len(value))] = value[0:self._dim]
            elif isinstance(value, (int, float)):
                self._our_value = value
            else:
                raise TypeError(f"Cannot assign {value} to this datatype {self._dataRefType}")
            if self._notify:
                self._notify(self)
            return

        if not self.types:
            raise ValueError(f"Unknown type for {self.name}")
        # we'll "prefer" double value over float value over integer value
        # where all are supported
        if 'double' in self.types:
            xp.setDatad(self.dref, float(value))
            return
        if 'float' in self.types:
            xp.setDataf(self.dref, float(value))
            return
        if 'int' in self.types:
            xp.setDatai(self.dref, int(value))
            return
        if 'float_array' in self.types and self._isarray:
            if self._count == 1:
                value = [value,]
            xp.setDatavf(self.dref, value, self._index, len(value))
            return
        if 'int_array' in self.types and self._isarray:
            if self._count == 1:
                value = [value,]
            xp.setDatavi(self.dref, value, self._index, len(value))
            return
        if 'data' in self.types and self._isarray:
            xp.setDatas(self.dref, value, offset=self._index, count=-1)
            return
        raise ValueError(f"Unable to set value for {self.name}")

    # ----------------------------
    # get(), set(), set_array() and get_array()
    # are callbacks we provide so _other_ plugins
    # can access "our" dataref
    # ----------------------------
    def set_array(self: Self, _refCon: Any, values: List[Any], offset: int, count: int) -> None:
        # print(f"set_array {self.name}, {values=} {offset=} {count=}")
        if not self._ours:
            raise ValueError("This is not ours")
        if count < 0:
            raise ValueError("count must be non-negative")
        if offset < 0:
            raise ValueError("offset must be non-negative")
        end = min(offset + count, self._dim)
        # print(f"Before {self._our_value}")
        self._our_value[offset:end] = values[0:end - offset]
        # print(f"After {self._our_value}")
        if self._notify:
            self._notify(self)

    def get_array(self: Self, _refCon: Any, values: Optional[List[Any]], offset: int, count: int) -> int:
        if not self._ours:
            raise ValueError("This is not ours")
        if offset < 0:
            raise ValueError("offset must be non-negative")
        if values is None:
            return len(self._our_value)
        if count == -1:
            values.extend([x for x in self._our_value])
            return len(self._our_value)
        if count < 0:
            raise ValueError("count must be non-negative")
        ret = [x for x in self._our_value[offset:min(self._dim, offset + count)]]
        count = len(ret)
        values.extend(ret)
        return count

    def get_int_array(self: Self, _refCon: Any, values: Optional[List[Any]], offset: int, count: int) -> int:
        if not self._ours:
            raise ValueError("This is not ours")
        if values is None:
            return len(self._our_value)
        if count == -1:
            values.extend([int(x) for x in self._our_value])
            return len(self._our_value)
        ret = [int(x) for x in self._our_value[offset:min(self._dim, offset + count)]]
        count = len(ret)
        values.extend(ret)
        return count

    def get_float_array(self: Self, _refCon: Any, values: Optional[List[Any]], offset: int, count: int) -> int:
        if not self._ours:
            raise ValueError("This is not ours")
        if values is None:
            return len(self._our_value)
        if count == -1:
            values.extend([float(x) for x in self._our_value])
            return len(self._our_value)
        ret = [float(x) for x in self._our_value[offset:min(self._dim, offset + count)]]
        count = len(ret)
        values.extend(ret)
        return count

    # def get(self: Self, *_unused) -> Any:
    #     return self.value

    def get_int(self: Self, *_unused: Any) -> int:
        return int(self.value)

    def get_float(self: Self, *_unused: Any) -> float:
        return float(self.value)

    def set_int(self: Self, _refCon: Any, value: int) -> None:
        self.set(value)

    def set_float(self: Self, _refCon: Any, value: float) -> None:
        self.set(value)

    def set(self: Self, value: Union[int, float, str]) -> None:
        self.value = value

    # --------------------------
    # __getitem__() and __setitem__() allow
    # you to use foo.value[n] for array types
    # --------------------------
    @overload
    def __getitem__(self: Self, idx: SupportsIndex) -> Union[int, float]:
        ...

    @overload
    def __getitem__(self: Self, idx: slice) -> Sequence[Union[int, float]]:  # type: ignore
        ...

    def __getitem__(self, idx: Union[slice, SupportsIndex]) -> Union[Sequence[Union[int, float]], int, float, str]:
        if not self._isarray:
            raise TypeError(f"'{self._dataRefType or self.types}' Dataref object is  not scriptable")
        if isinstance(idx, slice):
            if idx.start < 0:
                raise IndexError("negative indices not supported")
            start, stop, step = idx.indices(self._dim)
            if step != 1:
                raise IndexError("Slice only supports step of 1")
            count = stop - start
        else:
            start = int(idx)
            if start < 0:
                raise IndexError("negative indices not supported")
            stop = start + 1
            count = 1

        if self._ours:
            ret = self._our_value[start:start + count]
            if 'data' in self.types:
                ret = [chr(x) for x in ret]
            if count == 1:
                ret = ret[0]
            return ret

        if 'data' in self.types:
            # print(f"DRef getting {self.dref} [{start} #{count}]")
            return xp.getDatas(self.dref, offset=start, count=count)

        value: List[Any] = []
        if 'float_array' in self.types:
            # print(f"DRef getting {self.dref} [{start} #{count}]")
            xp.getDatavf(self.dref, value, offset=start, count=count)
        elif 'int_array' in self.types:
            # print(f"DRef getting {self.dref} [{start} #{count}]")
            xp.getDatavi(self.dref, value, offset=start, count=count)
        return value if count > 1 else value[0]

    @overload
    def __setitem__(self: Self, idx: SupportsIndex, value: Any) -> None:
        ...

    @overload
    def __setitem__(self: Self, idx: slice, value: Any) -> None:  # type: ignore
        ...

    def __setitem__(self, idx: Union[slice, SupportsIndex], value: Any) -> None:
        if not self._isarray:
            raise TypeError(f"'{self._dataRefType or self.types}' Dataref object is  not scriptable")

        if isinstance(idx, slice):
            if idx.start < 0:
                raise IndexError("negative indices not supported")
            start, stop, step = idx.indices(self._dim)
            if step != 1:
                raise IndexError("Slice only supports step of 1")
            count = stop - start
            if not isinstance(value, str) and count > len(value):
                raise RuntimeError(f"Not enough elements ({len(value)}) in value to set array dataref slice ({count})")
        else:
            start = int(idx)
            if start < 0:
                raise IndexError('negative indices not supported')
            stop = start + 1
            count = 1
            value = [value, ] if 'data' not in self.types else value

        if self._ours:
            if self._readonly and self._owning_plugin != getSelfModuleName():
                return  # mimic X-Plane behavior... no error when trying to set a read-only dataref

            if 'data' in self.types:
                self._our_value[start:start + count] = bytearray(count)  # zero it out
                l_value = min(count, len(value), self._dim - start)
                self._our_value[start:start + l_value] = bytearray(value[0:l_value].encode('utf-8'))
            else:
                self._our_value[start:start + count] = value[0:count]
            if self._notify:
                self._notify(self)
            return

        if 'float_array' in self.types:
            # print("DRef setting {} [{}] to '{}'".format(self.dataref.dref, idx, value))
            xp.setDatavf(self.dref, value[0:count], offset=start, count=count)
        elif 'int_array' in self.types:
            # print("DRef setting {} [{}] to '{}'".format(self.dataref.dref, idx, value))
            xp.setDatavi(self.dref, value[0:count], offset=start, count=count)
        elif 'data' in self.types:
            # print(f"DRef setting {self.dref} [{start=}, {count=}] to '{value}'")
            xp.setDatas(self.dref, value[0:count], offset=start, count=count)
        return


def create_dataref(name: str, dataRefType: str = "number", callback: Union[Callable[['DataRef'], None], Literal[False]] = None) -> DataRef:
    """
    create_dataref(name, type="number|array|string", callback=False)

    Creates a dataref.

    dataRefType:
    -----------
      "number" created int/float/double type (all three types).
               That means you can 'store' as python int or float, accessing a 'number' dataref
               we created this way will _always_ retrieve a float as the '.value'.
               You can access an 'int' version (we'll cast floats to int), but you'd have to use
                 xp_df = xp.findDataref('...')
                 value = xp.getDatai(xp_df)

      "array[x]" creates int_array & float_array of dimension x.
      "array[]" create int_array & float_array of dimension 512.
      "string" creates string with maximum 512 characters

    callback:
    --------
      * If None dataref is writable, but no callback
      * If False dataref is writable _only_ by python, other plugins see it as readonly.
      * If callable(), that callback will be called when dataref is written to.
        callback takes no parameters.

    """
    if name in _DataRefs:
        return _DataRefs[name]

    if callable(dataRefType) and not callback:
        callback = dataRefType
        dataRefType = 'number'
    dataType = ((xp.Type_Int | xp.Type_Float | xp.Type_Double) if dataRefType == "number"
                else (xp.Type_IntArray | xp.Type_FloatArray) if dataRefType.startswith("array[")
                else xp.Type_Data if dataRefType in ('string', 'data')
                else xp.Type_Unknown)
    assert dataType != xp.Type_Unknown, 'dataRefType must be one of "number" | "array[]" | "string" | "data"'

    dim: int = 512  # ignored except for array types
    # if name looks like an array, e.g. sim/foo[12], we'll use THAT value as
    try:
        dim = int(re.match(r'array\[ *([0-9]+) *\]$', dataRefType).group(1))
        assert dim > 0, "For array type, dimension must be > 0"
    except AttributeError:
        pass

    return DataRef(name, make=True, callback=callback, dataType=dataType, dim=dim, dataRefType=dataRefType)


def find_dataref(name: str, _dr_type: Optional[Any] = None) -> DataRef:
    # (Note, xlua allows specifying dr_type, be we don't care, so I've added a parameter to ease porting)
    try:
        return _DataRefs[name]
    except KeyError:
        return DataRef(name)


class DList(list):
    def __init__(self: Self, dataref: DataRef, values: Any, as_bytes: bool = False) -> None:
        self.dataref = dataref
        self.as_bytes = as_bytes
        super(DList, self).__init__(list(values))

    @overload
    def __setitem__(self: Self, idx: SupportsIndex, value: Any) -> None:
        ...

    @overload
    def __setitem__(self: Self, idx: slice, value: Iterable[Any]) -> None:  # type: ignore
        ...

    def __setitem__(self: Self, idx: Union[slice, SupportsIndex], value: Any, /) -> None:
        if isinstance(idx, slice):
            start, stop, step = idx.indices(self.dataref._dim)

            if step != 1:
                raise IndexError("Slice only support step of 1")
            count = stop - start
            if count > len(value):
                raise RuntimeError(f"Not enough elements ({len(value)}) in value to set dataref slice ({count})")
        else:
            start = int(idx)
            if start < 0:
                raise IndexError("Negative indices not supported")
            stop = start + 1
            count = 1
            value = [value, ]

        if self.dataref._ours:
            if self.dataref._readonly and self.dataref._owning_plugin != getSelfModuleName():
                return  # mimic X-Plane behavior... no error when trying to set a read-only dataref

            if 'data' in self.dataref.types:
                self.dataref._our_value[start:start + count] = [ord(x) for x in value[0:count]]
            else:
                self.dataref._our_value[start:start + count] = value[0:count]
            if self.dataref._notify:
                self.dataref._notify(self.dataref)
            return

        if 'float_array' in self.dataref.types:
            # print("DList setting {} [{}] to '{}'".format(self.dataref.dref, idx, value))
            xp.setDatavf(self.dataref.dref, value[0:count], offset=start, count=count)
            return
        if 'int_array' in self.dataref.types:
            # print("DList setting {} [{}] to '{}'".format(self.dataref.dref, idx, value))
            xp.setDatavi(self.dataref.dref, value[0:count], offset=start, count=count)
            return
        if 'data' in self.dataref.types:
            # print("DList setting {} [{}] to '{}'".format(self.dataref.dref, idx, value))
            if self.as_bytes:
                xp.setDatab(self.dataref.dref, value[0:count], offset=start, count=count)
            else:
                xp.setDatas(self.dataref.dref, value[0:count], offset=start, count=count)
            return
        raise TypeError(f"'{self.dataref._dataRefType or self.dataref.types}' Dataref object is  not scriptable")
