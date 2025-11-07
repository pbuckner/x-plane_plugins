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

# Modified by Peter Buckner <pbuck@xppython3.org>
# for use with X-Plane XPPython3 Plugin

"""
This provides the implementation of the import machinery necessary to load
XPYCE-formatted, encrypted files.
"""


import pathlib
import re
import sys
import marshal
# from importlib._bootstrap_external import _compile_bytecode
from importlib.abc import InspectLoader, Loader
from importlib.machinery import (FileFinder, ModuleSpec, PathFinder,
                                 SourcelessFileLoader)
import os.path
from types import ModuleType
from typing import Any, Dict, Optional, Tuple, Sequence

from ._crypto import decrypt

# Globals
EXTENSIONS = ['.xpyce',]
try:
    from XPPython3 import xp
    PREFIXES = [None, xp.PLUGINSPATH + '/..', xp.INTERNALPLUGINSPATH + '/..', 'Aircraft', 'Custom Scenery']
except ImportError:
    PREFIXES = [None, ]


PYC = True


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

        decryption_key = self.get_key(pathlib.Path(path).as_posix())

        if not decryption_key:
            raise KeyError(f"Cannot find decryption_key for module '{fullname}'")

        try:
            data = decrypt(data, decryption_key)
        except Exception as e:
            print(f"[XPPython3.xpyce] Could not decrypt module '{fullname}' with provided key")
            raise e

        if PYC:
            # .pyc changed from 3 32-bit words to 4 32-bit words with Python3.7
            # Skip over the header to get to the raw data

            if sys.version_info.minor < 7:
                bytes_data = data[12:]
            else:
                bytes_data = data[16:]

            return marshal.loads(bytes_data)
            # return _compile_bytecode(bytes_data, name=fullname, bytecode_path=path)
        else:
            return InspectLoader.source_to_code(data=data, path=fullname)

    def get_key(self, path):
        relpath, _ext = os.path.splitext(path)
        m = re.search('(Resources|Aircraft|Custom Scenery)/?.*/plugins/((PythonPlugins|XPPython3)/.*)', relpath)
        if m is None:
            print(f"[XPPython3.xpyce]  No valid key found for {relpath}")
            return None
        elif m[1] in ('Aircraft', "Custom Scenery"):
            relpath = m[2].replace("PythonPlugins", m[1])
        else:
            relpath = m[2]

        # print(f"[XPPython3.xpyce] XPYCE FileLoader path from get_filename is {path} with relpath {relpath}")

        # It is important to normalize path case for platforms like Windows

        # convert from file to module
        relpath = relpath.replace('/', '.').replace('\\', '.')

        for prefix in ('', "XPPython3.", "PythonPlugins.", "Aircraft.", "Custom Scenery."):
            # Try "{PythonPlugins,XPPython3}.foo.bar", then "foo.bar"
            if not relpath.startswith(prefix):
                continue

            lookup_module = relpath.replace(prefix, '')

            # Try exact match first
            if lookup_module in XPYCEPathFinder.KEYS:
                print(f"[XPPython3.xpyce]  Found key for module: {lookup_module}")
                return XPYCEPathFinder.KEYS[lookup_module]

            # Try parent modules (hierarchical fallback)
            # e.g., for "PythonPlugins.myplugin.compute", try "PythonPlugins.myplugin", then "PythonPlugins"
            parts = lookup_module.split('.')
            while len(parts) > 1:
                parts.pop()
                parent_module = '.'.join(parts)
                if parent_module in XPYCEPathFinder.KEYS:
                    print(f"[XPPython3.xpyce]  Found key for parent module: {parent_module}")
                    return XPYCEPathFinder.KEYS[parent_module]

        print(f"[XPPython3.xpyce]  No key found for {relpath} or any parent")
        return None


class XPYCEFileFinder(FileFinder):
    """
    This is responsible for finding XPYCE-formatted files as matches to
    import statements.
    """

    def __init__(self, path: str, *loader_details: Tuple[type[Loader], list[str]]) -> None:
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

        loader_details += ((XPYCEFileLoader, EXTENSIONS),)
        super().__init__(path, *loader_details)


def update_keys(h):
    XPYCEPathFinder.KEYS.update(h)


class XPYCEPathFinder(PathFinder):
    """
    This class goes through the paths Python knows about and tries to
    import XPYCE-formatted files.
    """

    KEYS: Dict[str, str] = {}

    @classmethod
    def find_spec(cls, fullname: str, path: Optional[Sequence[str]] = None,
                  target: Optional[ModuleType] = None) -> Optional[ModuleSpec]:
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
