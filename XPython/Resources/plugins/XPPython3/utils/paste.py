"""
Significantly edited down version of pyperclip, by Al Sweigart al@inventwithpython.com
His module is much more flexible and robust (and includes copything to the Clipboard).

As I didn't need all that, I simplified to bare minimum.

BSD License
"""
import platform
import subprocess
import time
import contextlib

import ctypes
from ctypes import c_wchar_p, c_size_t, c_wchar, sizeof
from ctypes.wintypes import (HWND, BOOL, HANDLE, UINT, HGLOBAL, LPVOID)


def darwin_paste() -> list[str]:
    p = subprocess.run(['pbpaste', 'r'], check=False, capture_output=True)
    return p.stdout.decode('utf-8').split('\n')


def darwin_copy(text) -> None:
    p = subprocess.Popen(['pbcopy', 'w'],
                         stdin=subprocess.PIPE, close_fds=True)
    p.communicate(input=text.encode('UTF-8'))


def win_copy(text) -> None:
    @contextlib.contextmanager
    def clipboard(hwnd):
        t = time.time() + 0.5
        success = False
        OpenClipboard = ctypes.windll.user32.OpenClipboard  # type: ignore
        OpenClipboard.argtypes = [HWND]
        OpenClipboard.restype = BOOL
        CloseClipboard = ctypes.windll.user32.CloseClipboard  # type: ignore
        CloseClipboard.argtypes = []
        CloseClipboard.restype = BOOL

        while time.time() < t:
            success = OpenClipboard(hwnd)
            if success:
                break
            time.sleep(0.01)
        # if not success:
        #     print("not successful")
        # else:
        #     print("Success opening clipboard")
        try:
            yield
        finally:
            CloseClipboard()

    CF_UNICODE_TEXT = 13
    SetClipboardData = ctypes.windll.user32.SetClipboardData  # type: ignore
    SetClipboardData.argtypes = [UINT, HANDLE]
    SetClipboardData.restype = HANDLE

    EmptyClipboard = ctypes.windll.user32.EmptyClipboard  # type: ignore
    EmptyClipboard.argtypes = []
    EmptyClipboard.restype = BOOL

    GlobalAlloc = ctypes.windll.kernel32.GlobalAlloc  # type: ignore
    GlobalAlloc.argtypes = [UINT, c_size_t]
    GlobalAlloc.restype = HGLOBAL

    GlobalLock = ctypes.windll.kernel32.GlobalLock  # type: ignore
    GlobalLock.argtypes = [HGLOBAL]
    GlobalLock.restype = LPVOID

    GlobalUnlock = ctypes.windll.kernel32.GlobalUnlock  # type: ignore
    GlobalUnlock.argtypes = [HGLOBAL]
    GlobalUnlock.restype = BOOL

    wcslen = ctypes.windll.msvcrt.wcslen  # type: ignore
    wcslen.argtypes = [c_wchar_p]
    wcslen.restype = UINT
    GMEM_MOVEABLE = 0x0002

    with clipboard(None):
        EmptyClipboard()

        count = wcslen(text) + 1
        handle = GlobalAlloc(GMEM_MOVEABLE, count * sizeof(c_wchar))
        locked_handle = GlobalLock(handle)

        ctypes.memmove(c_wchar_p(locked_handle), c_wchar_p(text), count * sizeof(c_wchar))

        GlobalUnlock(handle)
        SetClipboardData(CF_UNICODE_TEXT, handle)


def win_paste() -> list[str]:
    @contextlib.contextmanager
    def clipboard(hwnd):
        t = time.time() + 0.5
        success = False
        OpenClipboard = ctypes.windll.user32.OpenClipboard  # type: ignore
        OpenClipboard.argtypes = [HWND]
        OpenClipboard.restype = BOOL
        CloseClipboard = ctypes.windll.user32.CloseClipboard  # type: ignore
        CloseClipboard.argtypes = []
        CloseClipboard.restype = BOOL

        while time.time() < t:
            success = OpenClipboard(hwnd)
            if success:
                break
            time.sleep(0.01)
        # if not success:
        #     print("not successful")
        # else:
        #     print("Success opening clipboard")
        try:
            yield
        finally:
            CloseClipboard()

    CF_UNICODE_TEXT = 13
    GetClipboardData = ctypes.windll.user32.GetClipboardData  # type: ignore
    GetClipboardData.argtypes = [UINT]
    GetClipboardData.restype = HANDLE

    with clipboard(None):
        handle = GetClipboardData(CF_UNICODE_TEXT)
        if not handle:
            return ["", ]
        res = str(c_wchar_p(handle).value)
        return res.split('\r\n')


def lin_paste() -> list[str]:
    try:
        p = subprocess.run(['xclip', '-o'], capture_output=True, check=False)
    except Exception as e:  # pylint: disable=broad-except
        print(f"Failed to run xclip successfully {e}")
        return ["", ]
    return p.stdout.decode('utf-8').split('\n')


def lin_copy(text) -> None:
    p = subprocess.Popen(['xsel', '-b', '-i'],
                         stdin=subprocess.PIPE, close_fds=True)
    p.communicate(input=text.encode('UTF-8'))


def putClipboard(text):
    if platform.system() == 'Darwin':
        return darwin_copy(text)
    elif platform.system() == 'Windows':
        return win_copy(text)
    elif platform.system() == 'Linux':
        return lin_copy(text)


def getClipboard() -> list[str]:
    if platform.system() == 'Darwin':
        return darwin_paste()
    elif platform.system() == 'Windows':
        return win_paste()
    elif platform.system() == 'Linux':
        return lin_paste()

    print("paste not supported on this platform")
    return ["", ]


if __name__ == '__main__':
    test_res = getClipboard()
    print(f"Got {len(test_res)} lines, {test_res}")
