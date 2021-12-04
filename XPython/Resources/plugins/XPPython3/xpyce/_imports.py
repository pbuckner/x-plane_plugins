#   Copyright 2016-18 Soroco Americas Private Limited
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
#
#   Primary Author: Wolfgang Richter <wolf@soroco.com>
#
#   Purpose: Implement the import machinery necessary for loading .xpyce files.

# Modified by Peter Buckner <xppython@avwx.com>
# for use with X-Plane XPPython3 Plugin

"""
This provides the implementation of the import machinery necessary to load
XPYCE-formatted, encrypted files.
"""


import sys
from importlib._bootstrap_external import _compile_bytecode
from importlib.machinery import (FileFinder, ModuleSpec, PathFinder,
                                 SourcelessFileLoader)
from os.path import relpath
from typing import Any, Dict, List, Optional, Tuple

from ._crypto import decrypt

# Globals
EXTENSIONS = ['.cpython-{}{}.xpyce'.format(sys.version_info.major, sys.version_info.minor)]
try:
    from XPPython3 import xp
    PREFIXES = [None, xp.PLUGINSPATH + '/..', xp.INTERNALPLUGINSPATH + '/..', 'Aircraft', 'Custom Scenery']
except ImportError:
    PREFIXES = [None, ]


class XPYCEFileLoader(SourcelessFileLoader):
    """
    This class is responsible for decrypting and loading XPYCE-formatted files.
    """
    # Simplified from original Python SourcelessFileLoader source
    def get_code(self, fullname: str) -> Any:
        """
        Decrypt, and interpret as Python bytecode into a module return.

        Args:
            fullname: The name of the module to decrypt and compile

        Returns:
            Compiled bytecode
        """
        path = self.get_filename(fullname)
        data = self.get_data(path)

        # print("XPYCE FileLoader Fullname: {}, path from get_filename is {}".format(fullname, path))
        # It is important to normalize path case for platforms like Windows
        decryption_key = None
        for prefix in PREFIXES:
            if decryption_key:
                break
            lookup_module = relpath(path, start=prefix).replace('/', '.').replace('\\', '.')
            for module in XPYCEPathFinder.KEYS:
                if module == lookup_module:
                    decryption_key = XPYCEPathFinder.KEYS[module]
                    break
        if not decryption_key:
            raise KeyError("Cannot find decryption_key for module '{}'".format(fullname))

        try:
            data = decrypt(data, decryption_key)
        except Exception as e:
            print("Could not decrypt module '{}' with provided decryption_key".format(fullname))
            raise e

        # .pyc changed from 3 32-bit words to 4 32-bit words with Python3.7
        # Skip over the header to get to the raw data

        if sys.version_info.minor < 7:
            bytes_data = data[12:]
        else:
            bytes_data = data[16:]

        return _compile_bytecode(bytes_data, name=fullname, bytecode_path=path)


class XPYCEFileFinder(FileFinder):
    """
    This is responsible for finding XPYCE-formatted files as matches to
    import statements.
    """

    def __init__(self, path: str, *loader_details: Tuple[str]) -> None:
        """
        Add the XPYCEFileLoader to the list of loaders so that .xpyce files
        can be loaded

        Args:
            path: The path for the loader to search on
            *loader_details: Tuple where each element is a mapping of
                             a loader to extensions it can load

        Returns:
            None
        """
        loader_details += ([XPYCEFileLoader, EXTENSIONS],)
        super().__init__(path, *loader_details)


def update_keys(h):
    XPYCEPathFinder.KEYS.update(h)


class XPYCEPathFinder(PathFinder):
    """
    This class goes through the paths Python knows about and tries to
    import XPYCE-formatted files.
    """

    KEYS: Dict[str, str] = {}

    def __init__(self) -> None:
        """Instantiate a XPYCEPathFinder

        Returns:
            None
        """
        super().__init__()

    @classmethod
    def find_spec(cls, fullname: str, path: Optional[List[str]] = None,
                  target: Optional[str] = None) -> Optional[ModuleSpec]:
        """
        This finds and returns a Python module based on the XPYCE
        encrypted format.

        Args:
            fullname: The full import (i.e. os.path)
            path: A list of paths to search for the module
            target: Eventually unused argument passed to PathFinder.find_spec

        Returns:
            A Python module based on the XPYCE format
        """
        if path is None:
            path = sys.path

        sorocospec = None

        for p in path:
            sorocospec = XPYCEFileFinder(p).find_spec(fullname, target)

            if sorocospec is None:
                continue
            if sorocospec.origin is None:
                sorocospec = None
                break

            # This line is important for Python's internal libraries (like
            # warnings) to work.  Setting has_location to True can break
            # introspection because Python will assume the entire source code
            # is there, but it is encrypted
            sorocospec.has_location = False

            if sorocospec is not None:
                break
        return sorocospec
