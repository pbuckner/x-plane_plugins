from typing import Any
from . import datarefs
import time

class xlua:
    def __init__(self):
        self.is_in_replay = datarefs.find_dataref('sim/time/is_in_replay')
        self.frame_rate_period = datarefs.find_dataref('sim/operation/misc/frame_rate_period')
        self.paused = datarefs.find_dataref('sim/time/paused')
        self.running_time = datarefs.find_dataref('sim/time/total_running_time_sec')
        self.flight_time = datarefs.find_dataref('sim/time/total_flight_time_sec')

    @property
    def IN_REPLAY(self) -> Any:
        return self.is_in_replay.value

    @property
    def SIM_PERIOD(self) -> Any:
        return self.frame_rate_period.value

    @property
    def PAUSED(self) -> Any:
        return self.paused.value

    @property
    def RUNNING_TIME(self) -> Any:
        return self.running_time.value

    @property
    def FLIGHT_TIME(self) -> Any:
        return self.flight_time.value

_x = xlua()


def __getattr__(name: str) -> Any:
    if name in ('IN_REPLAY', 'SIM_PERIOD', 'PAUSED', 'RUNNING_TIME', 'FLIGHT_TIME'):
        return _x.__getattribute__(name)
    raise AttributeError(f"module '{__name__}' has no attribute '{name}'")


import copy


class Table(dict):

    def __init__(self, data: dict | list):
        if isinstance(data, dict):
            for i, v in data.items():
                self[i] = copy.copy(v)
        else:
            self[0] = None
            for i, v in enumerate(data):
                self[i + 1] = copy.copy(v)
        

    # def __getattribute__(self, key):
    #     return self['d'][key]

    def __getattr__(self, key):
        try:
            return self[key]
        except KeyError as k:
            return None

    def __len__(self):
        return len(self.keys())

    def __setattr__(self, key, v):
        self[key] = v

    def __pairs__(self):
        for i, v in self.items():
            if i == 0:
                continue
            yield i, v

    def __ipairs__(self):
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


def pairs(table: Table):
    return table.__pairs__()


def ipairs(table: Table):
    return table.__ipairs__()


def tonumber(s: str):
    try:
        return float(s)
    except (TypeError, ValueError):
        return None


def string_find(fullString, searchString):
    try:
        return fullString.index(searchString) + 1
    except ValueError:
        return None
    

def string_sub(originalString, startIndex, endIndex=None):
    if endIndex is None:
        return originalString[startIndex - 1:]
    return originalString[startIndex - 1:endIndex]


def string_byte(s, offset=1):
    if isinstance(s, str):
        return ord(s[offset - 1])
    return ord(str(s)[offset - 1])


program_start = time.time()


def os_clock():
    # returns "CPU elapsed time", these returns something similar.
    return time.time() - program_start
    
# print(f"{theList.__pairs__()=}")
# for k, v in pairs(theList):
#     print(f"{k=}, {v=}")

# lines = Table(['a', 'b', 'c', 'd'])
# for i, v in ipairs(lines):
#     print(f"{i=}, {v=}")
    
# print(f"{len(lines)=}")

