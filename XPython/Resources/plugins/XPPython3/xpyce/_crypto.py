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
#   Purpose: Provide convergent encryption primitives for use in a larger
#            trusted computing system.

# Modified by Peter Buckner <xppython@avnwx.com> <pbuck@xppython3.org>
# for use with X-Plane XPPython3 Plugin

"""
This file implements convergent encryption logic for use in a deduplicating
storage system.  It is designed to be used in tandem with an import loader in
Python that can load encrypted files.

Example use: See xpyce_compile.py

"""


from string import ascii_letters, digits
from random import choice
from hashlib import sha256 as srchash
from hmac import compare_digest
from hmac import new as hmac
from os import rename, walk
from os.path import dirname, isfile, join, splitext, basename
from typing import List, Optional, Set, Tuple

from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives.ciphers import Cipher
from cryptography.hazmat.primitives.ciphers.algorithms import AES as CIPHER
from cryptography.hazmat.primitives.ciphers.modes import CTR as MODE

BACKEND = default_backend()
ENC_EXT = {'.pyc'}
HMAC_HS = 'sha512'


class HMACFailureException(Exception):
    """This class represents a failure in verifying the HMAC of a file."""

    def __init__(self, message: str) -> None:
        """
        Instantiate an HMACFailureException

        Args:
            message: Error message

        Returns:
            None
        """
        self.message = message
        super().__init__(message)


def encrypt_path(path: str, extensions: Optional[Set[str]] = None, exclusions:
                 Optional[Set[str]] = None, custom_keys: Optional[dict] = None,
                 encrypt_by_directory: bool = False) -> List[Tuple[str, str]]:
    """
    This function convergently encrypts the file at path.  If path is a
    directory, it recursively encrypts all files in the directory which have
    matching extensions.  After encryption, it appends an HMAC, and renames
    the file to have a '.xpyce' extension.

    It is close to: AES_256_CTR_HMAC_SHA_512

    By default, exclusions is set to '__pycache__' to skip that directory. If
    you provide exclusions you should (most likely) include that directory
    as well.

    Args:
        path: path to convergently encrypt (file or directory)
        extensions: the file extensions that should be encrypted
        exclusions: files that should not be encrypted
        custom_keys: optional dict mapping file paths to hex key strings
        encrypt_by_directory: if True and path is a directory, all files in that
                            directory use the same key and manifest returns the
                            directory path instead of individual file paths
                            (if encrypt_by_directory, we use random key, rather
                            than convergent encryption.)

    Returns:
        List of all paths and their encryption key
    """
    if extensions is None:
        extensions = ENC_EXT

    if exclusions is None:
        exclusions = set(['__pycache__'])

    if custom_keys is None:
        custom_keys = {}

    manifest = []

    is_directory = not isfile(path)

    # Determine if we're using directory-wide key
    use_dir_key = encrypt_by_directory and is_directory

    if use_dir_key:
        # Generate key for the directory
        if path in custom_keys:
            module = path.replace('/', '.')
            dir_key_hex = custom_keys[module]
            dir_key = int(dir_key_hex, 16).to_bytes(32, 'big')
        else:
            # hash of path is too short and too repeatable (unlike hash of source file _contents_)
            long_random_string = generate_long_random_string(100)
            hashv = srchash(long_random_string.encode())
            dir_key_hex = hashv.hexdigest()
            dir_key = hashv.digest()

    walker = walk(path) if is_directory else [('', None, [path])]

    for root, _, files in walker:
        if basename(root) in exclusions:
            print(f"Skipping {root} due to exclusion")
            continue
        for fname in files:
            _, ext = splitext(fname)

            if ext not in extensions:
                continue

            absolute_path = join(root, fname)

            # Skip absolute paths
            if absolute_path in exclusions or root in exclusions:
                continue

            # Skip requested folders
            dirn = dirname(root)
            skip = False
            while dirn and dirn != '/' and not dirn.endswith(':\\'):
                if dirn in exclusions:
                    skip = True
                    break
                dirn = dirname(dirn)

            if skip:
                continue

            module, ext = splitext(absolute_path)
            module = module.replace('/', '.')

            with open(absolute_path, 'rb+') as openf:
                # read
                data = openf.read()

                # Determine key: directory key takes precedence, then custom key, then content hash
                if use_dir_key:
                    key = dir_key
                    key_hex = dir_key_hex
                elif module in custom_keys:
                    print(f"Using custom key for '{module}'")
                    key = int(custom_keys[module], 16).to_bytes(32, 'big')
                    key_hex = custom_keys[module]
                else:
                    if custom_keys:
                        print(f"couldn't find {absolute_path} in keys {list(custom_keys.keys())}")
                    hashv = srchash(data)
                    key = hashv.digest()
                    key_hex = hashv.hexdigest()

                # encrypt
                cipher = Cipher(CIPHER(key), MODE(key[0:16]), backend=BACKEND)
                encryptor = cipher.encryptor()
                ciphertext = encryptor.update(data)

                # write out
                openf.seek(0)
                openf.write(ciphertext)

                # append HMAC
                openf.write(hmac(key, ciphertext, HMAC_HS).digest())

            new_absolute_path, ext = splitext(absolute_path)
            new_absolute_path += '.xpyce'
            rename(absolute_path, new_absolute_path)

            # Add to manifest: directory path if using dir key, file path otherwise
            if not use_dir_key:
                manifest.append((module, key_hex))

    # Add directory entry if using directory key
    if use_dir_key:
        module, ext = splitext(path)
        module = module.replace('/', '.')
        manifest.append((module, dir_key_hex))

    return manifest


def __verify_hmac(data: bytes, ohmac: bytes, key: bytes) -> bool:
    """
    This function verifies that a provided HMAC matches a computed HMAC for
    the data given a key.

    Args:
        data: the data to HMAC and verify
        ohmac: the original HMAC, normally appended to the data
        key: the key to HMAC with for verification

    Returns:
        a boolean value denoting whether or not the HMAC's match
    """
    return compare_digest(ohmac, hmac(key, data, HMAC_HS).digest())


def decrypt(data: bytes, key: str) -> bytes:
    """
    This function takes in a list of ciphertext bytes, verifies their appended
    HMAC, and returns a corresponding list of plaintext bytes.  If there is a
    problem verifying HMAC, this function raises HMACFailureException.

    Args:
        data: the ciphertext to verify and decrypt
        key: the key to verify the HMAC with and decrypt

    Returns:
        The decrypted plaintext bytes for the file

    Raises:
        HMACFailureException if HMAC verification fails
    """
    data, ohmac = data[:-64], data[-64:]
    bkey = int(key, 16).to_bytes(32, 'big')

    # verify
    if not __verify_hmac(data, ohmac, bkey):
        raise HMACFailureException('HMAC verification has failed.')

    # decrypt into plaintext
    decryptor = Cipher(CIPHER(bkey), mode=MODE(bkey[0:16]),
                       backend=BACKEND).decryptor()
    data = decryptor.update(data) + decryptor.finalize()

    return data


def generate_long_random_string(length: int = 50) -> str:
    chars = ascii_letters + digits
    return ''.join(choice(chars) for _ in range(length))
