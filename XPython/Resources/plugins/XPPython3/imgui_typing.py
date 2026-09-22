# pylint: disable=unused-argument
# flake8: noqa
from dataclasses import dataclass
from typing import List, NamedTuple, NewType, Self, Any


@dataclass
class Vector2Float:
    x: List[float]
    y: List[float]


class Vec2(NamedTuple):
    """imgui.core.Vec2 -- a tuple subclass, so it unpacks AND has .x / .y"""
    x: float
    y: float


@dataclass
class IMGUIDrawList:
    def add_line(self: Self, start_x: float, start_y: float, end_x: float, end_y: float,
                 col: int, thickness: float = 1.0) -> None: ...
    def add_rect(self: Self, upper_left_x: float, upper_left_y: float,
                 lower_right_x: float, lower_right_y: float, col: int,
                 rounding: float = 0.0, flags: int = 0, thickness: float = 1.0) -> None: ...
    def add_rect_filled(self: Self, upper_left_x: float, upper_left_y: float,
                        lower_right_x: float, lower_right_y: float, col: int,
                        rounding: float = 0.0, flags: int = 0) -> None: ...
    def add_circle(self: Self, centre_x: float, centre_y: float, radius: float,
                   col: int, num_segments: int = 0, thickness: float = 1.0) -> None: ...
    def add_circle_filled(self: Self, centre_x: float, centre_y: float, radius: float,
                          col: int, num_segments: int = 0) -> None: ...
    def add_text(self: Self, pos_x: float, pos_y: float, col: int, text: str) -> None: ...

IMGUIContext = NewType('IMGUIContext', int)


@dataclass
class IMGUIStyle:
    window_rounding: int


@dataclass
class IMGUIFontAtlas:
    texture_id: int
    texture_width: int
    texture_height: int
    def get_tex_data_as_rgba32(self: Self) -> tuple[int, int, Any]: ...
    def add_font_from_file_ttf(self: Self, filename: str, size_pixels: float,
                               font_config: Any = None, glyph_ranges: Any = None) -> Any: ...
    def clear_tex_data(self: Self) -> None: ...
    def clear(self: Self) -> None: ...


@dataclass
class IMGUIo:
    config_mac_osx_behaviors: bool
    config_windows_resize_from_edges: bool
    config_resize_windows_from_edges: bool
    want_text_input: bool
    config_flags: int
    key_map: list[int]
    mouse_down: list[bool]
    keys_down: list[bool]
    key_shift: bool
    key_alt: bool
    key_ctrl: bool
    delta_time: float
    mouse_wheel: float
    mouse_wheel_horizontal: float
    fonts: IMGUIFontAtlas
    def add_input_character(self, key: int) -> None: ...

    # imgui READS these back as Vec2 (so .x / .y work) but ACCEPTS any 2-sequence
    # when setting.  Expressing that needs asymmetric property types, which mypy
    # only supports from 1.16; we are on 1.14, so use Any to allow both.  Revisit
    # as: mouse_pos/display_size/display_fb_scale -> property returning Vec2.
    mouse_pos: Any
    display_size: Any
    display_fb_scale: Any

@dataclass
class IMGUICommand:
    clip_rect: tuple[float, float, float, float]
    elem_count: int
    texture_id: int

@dataclass
class IMGUICommands:
    idx_buffer_data: Any
    idx_buffer_size: int
    vtx_buffer_data: Any
    vtx_buffer_size: int
    commands: tuple[IMGUICommand]

@dataclass
class IMGUIDrawData:
    total_idx_count: int
    total_vtx_count: int
    display_pos: Vec2
    display_size: Vec2
    def scale_clip_rects(self: Self, fb_scale: Vector2Float) -> None: ...
    commands_lists: tuple[IMGUICommands]
