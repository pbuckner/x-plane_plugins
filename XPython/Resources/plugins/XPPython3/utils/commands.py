import time
from XPPython3 import xp_typing
from XPPython3 import xp
from typing import Callable
# cmd = find_command("")
# cmd.once() execute command once
# cmd.start() starts command
# cmd.end() ends command
#
# cmd = create_command(name, description, function)
#    function(phase:int{0,1,2}, duration:float)
#
# cmd = replace_command(name, function)
# cmd = wrap_command(name, before_function, after_function)
# cmd = filter_command(name, bool_function)
#     bool_function() returns True if command is to execute, false to block
#     ... note, if filter_command returns false for python command, we'll
#     fabricate an end(). If filter_command returns false for laminar command
#     we simply block that command continuing. may or may not be able to send and "end"


Commands: dict = {}


class Command:
    _registry = []

    def __new__(cls, name: str, commandRef: xp_typing.XPLMCommandRef, callback: None | Callable[[int, float], None]):
        if commandRef is None:
            raise ValueError("Command not found.")
        if commandRef in Commands:
            return Commands[commandRef]
        self = object.__new__(cls)
        return self

    def __init__(self, name: str, commandRef: xp_typing.XPLMCommandRef, callback: None | Callable[[int, float], None]):
        try:
            assert self.function
        except AttributeError:
            self._registry.append(self)
            self.name = name
            self.commandRef = commandRef
            self.callback: Callable[[int, float], None] = callback
            self.callback_before: Callable[[int, float], None] = None
            self.callback_after: Callable[[int, float], None] = None
            self.callback_filter: Callable[[], int] = None
            self.started = 0.0
            self.function = 'create'
            Commands[commandRef] = self

    def once(self):
        xp.commandOnce(self.commandRef)

    def start(self):
        xp.commandBegin(self.commandRef)

    def stop(self):
        xp.commandEnd(self.commandRef)

    def __str__(self):
        return f"<Command: '{self.name}'>"


def find_command(name: str) -> Command:
    return Command(name, xp.findCommand(name), None)


def genericBeforeCallback(commandRef, phase, cmd):
    # xp.log(f"{cmd.name} BEFORE {phase=} {time.time()=} {cmd.started=}")
    if phase == 0:
        cmd.started = time.time()
    duration = time.time() - cmd.started
    if not callable(cmd.callback_before):
        xp.log(f'before {cmd.callback_before} isn\'t callable for {cmd}')
        return 0
    cmd.callback_before(phase, 0 if phase == 0 else duration)
    return 1  # continue processing command


def genericAfterCallback(commandRef, phase, cmd):
    # xp.log(f"{cmd.name} AFTER {phase=} {time.time()=} {cmd.started=}")
    duration = time.time() - cmd.started
    if not callable(cmd.callback_after):
        # xp.log(f'after {cmd.callback_after} isn\'t callable for {cmd}')
        return 0
    cmd.callback_after(phase, duration)
    return 0  # (ignored for after)


def genericCallback(commandRef, phase, cmd):
    # xp.log(f"{cmd.name} {phase=} {time.time()=} {cmd.started=}")
    if phase == 0:
        cmd.started = time.time()
    duration = time.time() - cmd.started
    cmd.callback(phase, 0 if phase == 0 else duration)
    if cmd.function == 'replace':
        return 0
    return 1  # continue processing command


def create_command(name: str, description: str, callback: Callable[[int, float], None]) -> Command:
    # callback(phase, duration) -> None
    commandRef = xp.createCommand(name, description)
    cmd = Command(name, commandRef, callback)
    xp.registerCommandHandler(commandRef, genericCallback, 1, cmd)
    return cmd


def replace_command(name: str, callback: Callable[[int, float], None]) -> Command:
    cmd = find_command(name)
    cmd.callback = callback
    cmd.function = 'replace'
    xp.registerCommandHandler(cmd.commandRef, genericCallback, 1, cmd)
    return cmd


def wrap_command(name: str, before: Callable[[int, float], None], after: Callable[[int, float], None]) -> Command:
    cmd = find_command(name)
    cmd.function = 'wrap'
    cmd.callback_before = before
    cmd.callback_after = after
    if callable(before):
        xp.registerCommandHandler(cmd.commandRef, genericBeforeCallback, 1, cmd)
    else:
        xp.log(f"{name} before is not callable {before}")
    if callable(after):
        xp.registerCommandHandler(cmd.commandRef, genericAfterCallback, 0, cmd)
    else:
        xp.log(f"{name} after is not callable {before}")
    return cmd


def genericFilterCallback(commandRef, phase: int, cmd: Command) -> int:
    if phase == 2:  # for 2,we're ending already, no need to call filter
        cmd.started = 0
        return 1

    ret = cmd.callback_filter()  # 1 to continue, 0 to stop
    if cmd.callback is None:     # this is a sim command
        return 1 if ret else 0

    if ret:
        if cmd.started:
            return 1
        cmd.started = time.time()
        genericCallback(commandRef, 0, cmd)  # send phase 0 rather than phase 1
        return 0
    if cmd.started:
        genericCallback(commandRef, 2, cmd)
    cmd.started = 0
    return 0


def filter_command(name: str, callback: Callable[[], int]) -> Command:
    cmd = find_command(name)
    cmd.function = 'filter'
    cmd.callback_filter = callback
    xp.registerCommandHandler(cmd.commandRef, genericFilterCallback, 1, cmd)
    return cmd
