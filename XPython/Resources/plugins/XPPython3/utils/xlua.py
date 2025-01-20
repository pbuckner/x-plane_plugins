from typing import Any, Self, Generator, Optional
from . import datarefs
import time
import copy


class xlua:
    def __init__(self: Self) -> None:
        self.is_in_replay = datarefs.find_dataref('sim/time/is_in_replay')
        self.frame_rate_period = datarefs.find_dataref('sim/operation/misc/frame_rate_period')
        self.paused = datarefs.find_dataref('sim/time/paused')
        self.running_time = datarefs.find_dataref('sim/time/total_running_time_sec')
        self.flight_time = datarefs.find_dataref('sim/time/total_flight_time_sec')

    @property
    def IN_REPLAY(self: Self) -> int:
        return self.is_in_replay.value

    @property
    def SIM_PERIOD(self: Self) -> float:
        return self.frame_rate_period.value

    @property
    def PAUSED(self: Self) -> int:
        return self.paused.value

    @property
    def RUNNING_TIME(self) -> float:
        return self.running_time.value

    @property
    def FLIGHT_TIME(self) -> float:
        return self.flight_time.value


_x = xlua()


def __getattr__(name: str) -> int | float:
    if name in ('IN_REPLAY', 'SIM_PERIOD', 'PAUSED', 'RUNNING_TIME', 'FLIGHT_TIME'):
        return _x.__getattribute__(name)
    raise AttributeError(f"module '{__name__}' has no attribute '{name}'")


class Table(dict):

    def __init__(self: Self, data: dict | list) -> None:
        if isinstance(data, dict):
            for i, v in data.items():
                self[i] = copy.copy(v)
        else:
            self[0] = None
            for i, v in enumerate(data):
                self[i + 1] = copy.copy(v)

    # def __getattribute__(self, key):
    #     return self['d'][key]

    def __getattr__(self, key: str) -> None | Any:
        try:
            return self[key]
        except KeyError:
            return None

    def __len__(self: Self) -> int:
        return len(self.keys())

    def __setattr__(self: Self, key: str, v: Any) -> None:
        self[key] = v

    def __pairs__(self: Self) -> Generator[tuple[int, Any], None, None]:
        for i, v in self.items():
            if i == 0:
                continue
            yield i, v

    def __ipairs__(self: Self) -> Generator[tuple[int, Any], None, None]:
        i = 1
        if 1 not in self.keys():
            return
        value = self[i]
        while value:
            yield i, value
            i += 1
            try:
                value = self[i]
            except KeyError:
                value = None

    # @staticmethod
    # def pairs(table):
    #     for i, v in
    #     yield(table

# a = Table()
# lines = Table(['a', 'b', 'c', 'd'])
# theList = None
# for i, line in lines.items():
#     print(i, line)
#     theList = Table({'n': theList, 'value': line})

# print(theList)
# # print(theList.items())

# print(theList.value)
# print(theList.n.value)
# print(theList.n.n.value)


def pairs(table: Table) -> Generator[tuple[int, Any], None, None]:
    return table.__pairs__()


def ipairs(table: Table) -> Generator[tuple[int, Any], None, None]:
    return table.__ipairs__()


def tonumber(s: str) -> None | float:
    try:
        return float(s)
    except (TypeError, ValueError):
        return None


def string_find(fullString: str, searchString: str) -> None | int:
    try:
        return fullString.index(searchString) + 1
    except ValueError:
        return None


def string_sub(originalString: str, startIndex: int, endIndex: Optional[int] = None) -> str:
    if endIndex is None:
        return originalString[startIndex - 1:]
    return originalString[startIndex - 1:endIndex]


def string_byte(s: str, offset: int = 1) -> int:
    if isinstance(s, str):
        return ord(s[offset - 1])
    return ord(str(s)[offset - 1])


program_start = time.time()


def os_clock() -> float:
    # returns "CPU elapsed time", these returns something similar.
    return time.time() - program_start

# print(f"{theList.__pairs__()=}")
# for k, v in pairs(theList):
#     print(f"{k=}, {v=}")

# lines = Table(['a', 'b', 'c', 'd'])
# for i, v in ipairs(lines):
#     print(f"{i=}, {v=}")

# print(f"{len(lines)=}")
