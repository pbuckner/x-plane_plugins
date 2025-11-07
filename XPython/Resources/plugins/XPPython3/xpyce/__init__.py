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
#   Purpose: This module enables the loading of encrypted Python stored in the
#            Soroco format (.xpyce).

# Modified by Peter Buckner <xppython@avnwx.com> <pbuck@xppython3.org>
# for use with X-Plane XPPython3 Plugin

"""
This defines the Soroco import machinery to import .xpye formatted Python
files.  Such files are defined as an AES-256 convergently encrypted Python
files with a SHA-512 HMAC.

NOTE: THIS IS AUTOMATICALLY LOADED INTO XPPython3 as of XXXXX

You can enable the Python format for your interpreter by doing the following:

>>> import sys
>>> from xpyce import XPYCEPathFinder
>>> sys.meta_path.insert(0, XPYCEPathFinder)

If you had encrypted Python code, you'd want to then set the `KEYS` variable
to an appropriate value (XPYECPathFinder.KEYS).

"""

from ._crypto import encrypt_path, HMACFailureException
from ._imports import XPYCEPathFinder, update_keys


__all__ = ['encrypt_path', 'HMACFailureException', 'XPYCEPathFinder', 'update_keys']
__version__ = '2.0.0'
