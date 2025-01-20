# pylint: disable=unused-argument
# flake8: noqa
from dataclasses import dataclass
from typing import List, NewType, Self, Any


@dataclass
class Vector2Float:
    x: List[float]
    y: List[float]

IMGUIContext = NewType('IMGUIContext', int)


@dataclass
class IMGUIStyle:
    window_rounding: int


@dataclass
class IMGUIo:
    config_mac_osx_behaviors: bool
    config_windows_resize_from_edges: bool
    config_resize_windows_from_edges: bool
    want_text_input: bool
    config_flags: int
    key_map: list[int]
    mouse_pos: tuple[float, float]
    mouse_down: list[bool]
    keys_down: list[bool]
    display_size: tuple[float, float]
    display_fb_scale: tuple[float, float]
    key_shift: bool
    key_alt: bool
    key_ctrl: bool
    def add_input_character(self, key: int) -> None: ...

@dataclass
class IMGUICommand:
    clip_rect: tuple[float, float, float, float]
    elem_count: int
    texture_id: int

@dataclass
class IMGUICommands:
    idx_buffer_data: Any
    vtx_buffer_data: Any
    commands: tuple[IMGUICommand]

@dataclass
class IMGUIDrawData:
    total_idx_count: int
    total_vtx_count: int
    display_pos: tuple[float, float]
    display_size: tuple[float, float]
    def scale_clip_rects(self: Self, fb_scale: Vector2Float) -> None: ...
    commands_lists: tuple[IMGUICommands]
