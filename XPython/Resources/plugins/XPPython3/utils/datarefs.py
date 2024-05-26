from __future__ import annotations
from typing import Any, Callable, Union, List, Iterable, SupportsIndex, overload
from XPPython3 import xp
import re


"""

find_dataref()

>>> foo = find_dataref("foo")
>>> foo
<DataRef obect at 0x87646500>
>>> print(foo)
DataRef: 'foo' [int]
>>> foo.value = 2
>>> foo.value
2
>>> bar_one = find_dataref("bar[1]")
>>> print(bar_one)
DataRef: 'bar[1]' [int_array]
>>> bar_one.value = 3
>>> bar_one.value
3
>>> bar_list = find_dataref("bar")
>>> bar_list.value
[1, 3, 0]
>>> bar_list[1]
3
>>> bar_list[1] = 4
>>> bar_list
<DataRef: 'bar'>
>>> bar_list.value
[1, 4, 0]
>>> bar_list.value[2]
0
>>> bar_list.value[2] = 4
>>> bar_list.value
[1, 4, 4]

To use:
  1. identify which datarefs you want to use, and store the accessor:

       num_batteries = find_dataref("sim/aircraft/electrical/num_batteries")
       inverter_on = find_dataref("sim/cockpit2/electrical/inverter_on")
       fuel_pump_on_zero = find_dataref("sim/cockpit/engine/fuel_pump_on[0]")

     Don't bother with datatype, we'll figure it out.
     Array / bytes data ref types return the full array (e.g., inverter_on above),
     but you can include [<digit>] to retrieve single value from an array or
     bytes data ref (e.g., fuel_pump_on_zero above).

     Data has not been retrieved yet.

  2. To get current value, simply access the '.value' attribute:

     >>> if num_batteries.value == 1:
     ...   do_something()
     ...

     If it's an array type, you'll (probably) want to index it:

     >>> if inverter_on.value[0] and inverter_on.value[1]:
     ...    do_something()
     ... elif inverter_on.value[0]:
     ...    do_something_else()
     ...

     Values are not cached, are are looked up immediately.
     While it is fast, if you're going to access the same data ref
     many times in the same cycle, it may be preferable to assign
     the retrieved value to a local variable.

     Note: For array types, you can drop '.value' for accessing
           indexed data: e.g., inverter_on[0] is similar to inverter_on.value[0]:
           * inverter_on[0] queries X-Plane for the single valued item
           * inverter_on.value[0] retrieves from X-Plane the list of values, and
             then python returns the [0] list element.

  3. To set value, simply assign a new value:

     >>> fuel_pump_on_zero.value
     0
     >>> fuel_pump_on_zero.value = 1
     >>> fuel_pump_on_zero.value
     1

     Data is immediately updated in X-Plane.

     If the data ref is an array type, and you specified it
     with an index (e.g., fuel_pump_on_zero), we'll correctly
     set the single value.

     >>> inverter_on.value
     [0, 0]
     >>> inverter_on.value[1] = 1
     >>> inverter_on.value
     [0, 1]

     Again, you can forgo the .value attribute for array types:

     >>> inverter_on.value
     [0, 0]
     >>> inverter_on[1] = 1
     >>> inverter_on.value
     [0, 1]

"""


_DataRefs: dict[str, DataRef] = {}


class DataRef:
    # ----------------------
    # DataRef holds information about a dataref
    # (and, if it's "ours", its current value).
    # If the dataref is not ours, we gather information
    # about it from X-Plane.
    # ----------------------
    def __init__(self, name, make=False, callback=False, dataType=0, dim=512):
        self.name: str = name
        self._ours: bool = False
        self._our_value: Any = None
        self._dref_name: str = name
        self._isarray: bool = self.name.endswith(']')
        self._count: int = 1
        self._notify: Callable = None
        self._dim: int = dim
        try:
            self._index: int = None if not self._isarray else int(re.search(r'\[([0-9]+)\]$', self.name).group(1))
        except AttributeError as e:
            raise ValueError("Array dataref missing number within []") from e

        if self._isarray:
            self._dref_name = self.name.split('[')[0]

        self._dref = xp.findDataRef(self._dref_name)  # None if not found
        self.types = []
        if make and self._dref is None:
            if callable(callback):
                self._notify = callback
            self._ours = True
            if dataType & (xp.Type_FloatArray | xp.Type_IntArray | xp.Type_Data):
                self._our_value = [0, ] * dim
            elif dataType & (xp.Type_Int | xp.Type_Float | xp.Type_Double):
                self._our_value = 0
            self._dref = xp.registerDataAccessor(name, dataType,
                                                 writable=callback is not False,
                                                 readInt=self.get,
                                                 readFloat=self.get,
                                                 readDouble=self.get,
                                                 readIntArray=self.get_array,
                                                 readFloatArray=self.get_array,
                                                 readData=self.get_array,
                                                 writeInt=self.set_int,
                                                 writeFloat=self.set_float,
                                                 writeDouble=self.set_float,
                                                 writeIntArray=self.set_array,
                                                 writeFloatArray=self.set_array,
                                                 writeData=self.set_array)

            for sig in ('com.leecbaker.datareftool', 'xplanesdk.examples.DataRefEditor'):
                dre = xp.findPluginBySignature(sig)
                if dre != xp.NO_PLUGIN_ID:
                    xp.sendMessageToPlugin(dre, 0x01000000, self._dref_name)

        if self._dref is not None:
            _types = xp.getDataRefTypes(self._dref)
            if _types & xp.Type_Int:
                self.types.append('int')
            if _types & xp.Type_Float:
                self.types.append('float')
            if _types & xp.Type_Double:
                self.types.append('double')
            if _types & xp.Type_FloatArray:
                self._isarray = True
                if self._index is None:
                    self._index = 0
                    self._count = -1
                self.types.append('float_array')
            if _types & xp.Type_IntArray:
                self._isarray = True
                if self._index is None:
                    self._index = 0
                    self._count = -1
                self.types.append('int_array')
            if _types & xp.Type_Data:
                self._isarray = True
                if self._index is None:
                    self._index = 0
                    self._count = -1
                self.types.append('data')
        if self._dref is None:
            raise ValueError(f"Unknown dataref {name}")
        _DataRefs[self.name] = self

    def __str__(self):
        return f"DataRef: '{self.name} [{', '.join(self.types)}]'"

    # ----------------------------------
    # value @property & @value.setter
    # do the main work -- accessing the dataref
    # in X-Plane (or locally).
    # ----------------------------------
    @property
    def value(self) -> Any:
        """
        "getting" value of dataref:
        If it's 'our' dataref, simply return the value

        If it's X-Plane dataref, do the appropriate query and return value
        """
        if self._ours:
            if 'data' in self.types:
                return bytearray([x for x in self._our_value if x]).decode('utf-8')
            else:
                return self._our_value
        if not self.types:
            raise ValueError(f"Unknown type for {self.name}")
        # we'll "prefer" double value over float value over integer value
        # where all are supported
        if 'double' in self.types:
            return xp.getDatad(self._dref)
        if 'float' in self.types:
            return xp.getDataf(self._dref)
        if 'int' in self.types:
            return xp.getDatai(self._dref)
        if 'float_array' in self.types and self._isarray:
            float_values: List[float] = []
            xp.getDatavf(self._dref, float_values, offset=self._index, count=self._count)
            return float_values[0] if self._count == 1 else DList(self, float_values)
        if 'int_array' in self.types and self._isarray:
            int_values: List[int] = []
            xp.getDatavi(self._dref, int_values, offset=self._index, count=self._count)
            return int_values[0] if self._count == 1 else DList(self, int_values)
        if 'data' in self.types and self._isarray:
            values = xp.getDatas(self._dref, offset=self._index, count=self._count)
            return values
            # return values[0] if self._count == 1 else DList(self, values)
        raise ValueError(f"Unable to get value for {self.name}")

    @value.setter
    def value(self, value: Any) -> None:
        if self._ours:
            if 'data' in self.types:
                self._our_value = bytearray(value.encode('utf-8'))
            else:
                self._our_value = value
            if self._notify:
                self._notify()
            return
        if not self.types:
            raise ValueError(f"Unknown type for {self.name}")
        # we'll "prefer" double value over float value over integer value
        # where all are supported
        if 'double' in self.types:
            xp.setDatad(self._dref, value)
            return
        if 'float' in self.types:
            xp.setDataf(self._dref, float(value))
            return
        if 'int' in self.types:
            xp.setDatai(self._dref, int(value))
            return
        if 'float_array' in self.types and self._isarray:
            xp.setDatavf(self._dref, [float(value), ], self._index, 1)
            return
        if 'int_array' in self.types and self._isarray:
            xp.setDatavi(self._dref, [int(value), ], self._index, 1)
            return
        if 'data' in self.types and self._isarray:
            xp.setDatas(self._dref, value, offset=self._index, count=1)
            return
        raise ValueError(f"Unable to set value for {self.name}")

    # ----------------------------
    # get(), set(), set_array() and get_array()
    # are callbacks we provide so _other_ plugins
    # can access "our" dataref
    # ----------------------------
    def set_array(self, _refCon: Any, values: list[Any], offset: int, count: int) -> None:
        if not self._ours:
            raise ValueError("This is not ours")
        self._our_value[offset:offset + count] = values[0:count]

    def get_array(self, _refCon, values: list[Any], offset: int, count: int) -> int:
        if not self._ours:
            raise ValueError("This is not ours")
        if count == -1:
            values.extend([x for x in self._our_value])
            return len(self._our_value)
        ret = [x for x in self._our_value[offset:min(self._dim, offset + count)]]
        count = len(ret)
        values.extend(ret)
        return count

    def get(self, *_unused) -> Any:
        return self.value

    def set_int(self, _dataref: Any, value: int) -> None:
        self.set(value)

    def set_float(self, _dataref: Any, value: float) -> None:
        self.set(value)

    def set(self, value: Union[int, float, str]) -> None:
        self.value = value

    # --------------------------
    # __getitem__() and __setitem__() allow
    # you to use foo.value[n] for array types
    # --------------------------
    def __getitem__(self, idx: int, *args) -> Any:
        if not self._isarray:
            raise IndexError
        if self._ours:
            if 'data' in self.types:
                return chr(self._our_value[idx])
            return self._our_value[idx]
        return self.value[idx]

    def __setitem__(self, idx: int, value: Any) -> None:
        if not self._isarray:
            raise IndexError

        if self._ours:
            if 'data' in self.types:
                self._our_value[idx] = ord(value)
            else:
                self._our_value[idx] = value
            if self._notify:
                self._notify()
            return

        if 'float_array' in self.types:
            # print("DRef setting {} [{}] to '{}'".format(self.dataref._dref, idx, value))
            xp.setDatavf(self._dref, [value, ], offset=idx, count=1)
            return
        if 'int_array' in self.types:
            # print("DRef setting {} [{}] to '{}'".format(self.dataref._dref, idx, value))
            xp.setDatavi(self._dref, [value, ], offset=idx, count=1)
            return
        if 'data' in self.types:
            # print("DRef setting {} [{}] to '{}'".format(self.dataref._dref, idx, value))
            xp.setDatab(self._dref, [value, ], offset=idx, count=1)
            return


def create_dataref(name: str, dataRefType: str = "number", callback: Callable[[None], None] = None) -> DataRef:
    """
    create_dataref(name, type="number|array|string", callback=False)

    Creates a dataref. If callback is False, dataref is not
      "number" created int/float/double type
      "array[x]" creates int_array/float_array of dimension x
      "string" creates data

    Callback:
      * If None dataref is writable, but no callback
      * If callable(), that callback will be called when callback is written to

    Create works for number, array (Setting Callback=None allows it to be settable, Callback=False is not writable
      get /set work

    Have not tried setting notification callback

    Get/Set all data types seems good.
    """
    if callable(dataRefType) and not callback:
        callback = dataRefType
        dataRefType = 'number'
    dataType = ((xp.Type_Int | xp.Type_Float | xp.Type_Double) if dataRefType == "number"
                else (xp.Type_IntArray | xp.Type_FloatArray) if dataRefType.startswith("array[")
                else xp.Type_Data if dataRefType == "string"
                else xp.Type_Unknown)
    assert dataType != xp.Type_Unknown, 'dataRefType must be one of "number" | "array[]" | "string"'

    try:
        # if name looks like an array, e.g. sim/foo[12], we'll use THAT value as
        m = re.match(r'array\[ *([0-9]+) *\]$', dataRefType)
        dim = int(m.group(1))
        assert dim > 0, "For array type, dimension must be > 0"
    except AttributeError:
        dim = 512

    try:
        return _DataRefs[name]
    except KeyError:
        return DataRef(name, make=True, callback=callback, dataType=dataType, dim=dim)


def find_dataref(name: str) -> DataRef:
    try:
        return _DataRefs[name]
    except KeyError:
        return DataRef(name)


class DList(list):
    def __init__(self, dataref: DataRef, values: Any):
        self.dataref = dataref
        super(DList, self).__init__(list(values))

    @overload
    def __setitem__(self, idx: SupportsIndex, value: Any) -> None:
        #  self.dataref
        #
        if 'float_array' in self.dataref.types:
            # print("DList setting {} [{}] to '{}'".format(self.dataref._dref, idx, value))
            xp.setDatavf(self.dataref._dref, [value, ], offset=int(idx), count=1)
            return
        if 'int_array' in self.dataref.types:
            # print("DList setting {} [{}] to '{}'".format(self.dataref._dref, idx, value))
            xp.setDatavi(self.dataref._dref, [value, ], offset=int(idx), count=1)
            return
        if 'data' in self.dataref.types:
            # print("DList setting {} [{}] to '{}'".format(self.dataref._dref, idx, value))
            xp.setDatas(self.dataref._dref, value, offset=int(idx), count=1)
            return
        raise ValueError("dataref does not support indexing")

    @overload
    def __setitem__(self, idx: slice, value: Iterable[Any]) -> None:
        raise ValueError("slice not supported")

    def __setitem__(self, idx: slice | SupportsIndex, value: Any, /) -> None:
        ...


def test():
    num_batteries = find_dataref("sim/aircraft/electrical/num_batteries")
    fuel_pump_on_zero = find_dataref("sim/cockpit/engine/fuel_pump_on[0]")
    inverter_on = find_dataref("sim/cockpit2/electrical/inverter_on")
    print(f"num_batteries: {num_batteries.value}")
    print(f"fuel_pump_on_zero: {fuel_pump_on_zero.value}")
    print(f"inverter_on: {inverter_on.value}")
    print(f"inverter_on.value[0]: {inverter_on.value[0]}")
    print(f"inverter_on[0]: {inverter_on[0]}")
    print(f"inverter_on[1]: {inverter_on[1]}")
    try:
        print(f"inverter_on[2]: {inverter_on[2]}")
    except IndexError:
        print("inverter_on[2]: IndexError")

    fuel_pump_on_zero.value = 1
    assert fuel_pump_on_zero.value == 1
    print(f"NOW fuel_pump_on_zero: {fuel_pump_on_zero.value}")

    inverter_on[0] = 0
    assert inverter_on[0] == 0
    inverter_on[1] = 0
    assert inverter_on[1] == 0
    assert inverter_on.value == [0, 0]

    inverter_on[0] = 1
    inverter_on[1] = 1
    assert inverter_on.value == [1, 1]

    assert inverter_on.value[0] == 1
    assert inverter_on.value[1] == 1

    inverter_on.value[0] = 0
    inverter_on.value[1] = 0
    assert inverter_on.value == [0, 0]


def test2():
    tail = find_dataref('sim/aircraft/view/acf_tailnum')
    print(tail.value)
    df_foo = create_dataref('sim/foo', 'string')
    df_foo.value = "ABCD"
