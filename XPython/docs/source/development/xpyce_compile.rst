XPYCE_COMPILE(1) User Commands
==============================

NAME
----

xpyce_compile - Compile and encrypt Python files for XPPython3 plugin

SYNOPSIS
-------- 

::

  xpyce_compile.py [-h] [--encrypt-by-directory] [--manifest FILE]
                   [--key PATH:KEY] ... path [path ...]``

DESCRIPTION
-----------

xpyce_compile compiles Python source files (\*.py) to bytecode (\*.pyc) and encrypts them
to protected format (\*.xpyce) for use with the X-Plane XPPython3 plugin. The encryption
uses AES-256-CTR with HMAC-SHA-512 authentication.

Files can be encrypted with content-based keys (default), custom per-file keys, or
directory-wide keys. Custom keys are useful when updating files over time while
maintaining decryption compatibility with fixed key values.

OPTIONS
-------

Positional Arguments
********************

*path*
    One or more Python files (\*.py) or directories to be compiled and encrypted.
    For directories, all .py files are recursively compiled and encrypted. (__pycache__
    directory is excluded.)


Optional Arguments
******************

``-h``, ``--help``
    Show help message and exit.

``--encrypt-by-directory``
    When a directory is provided, encrypt all matching files in that directory
    with the same key. The manifest returns the module name for the directory path and single key
    instead of per-file paths. This is useful for grouping related files
    that should be encrypted together.

``--manifest`` *FILE*
    Path to JSON file containing a mapping of file/directory paths to encryption
    keys. The JSON format is::

      {
        "module1.file1": "hexadecimal_key_string",
        "module2": "hexadecimal_key_string"
      }

    Keys must be 64-character hexadecimal strings (256 bits). If a manifest file
    is provided and a file is not listed, it will be encrypted with a content-based
    key instead, and the new key will be returned.

``--key`` *PATH:KEY*
    Specify a custom encryption key for a specific file or directory in the format
    PATH:KEY where KEY is a 64-character hexadecimal string. This option can be
    used multiple times to specify multiple keys. Command-line keys override keys
    from the manifest file.

ENVIRONMENT
-----------

The script attempts run using ``env python3.12``. It can also be executed by
passing it directly to a python3.12 interpreter. The script needs to import
from the XPPython3.xpyce module. If the import fails, it
automatically adds '../XPPython3' (which presumes the script is running from PythonPlugins
directory) to the Python path and retries.

FILES
-----

Output Files
************

Compiled and encrypted files are created with the .xpyce extension in the same
directory as their source .py files. The original .pyc file is renamed to .xpyce.

Example::

   mymodule.py → mymodule.xpyce

Resulting set of modules and keys is printed to standard out::

  mymodule: a1b2c3d4e5f6789012345678901234567890123456789012345678901234
  ...
  

Manifest Input File
*******************

JSON file specified with --manifest containing path-to-key mappings.

EXIT STATUS
-----------

::

   0    Successful completion
   1    Error occurred (file not found, invalid JSON, invalid key format, etc.)


EXAMPLES
--------

Basic Usage: Single File with Content-Based Key
***********************************************

::

  $ python3 xpyce_compile.py mymodule.py
  mymodule: a1b2c3d4e5f6789012345678901234567890123456789012345678901234


Multiple Files with Individual Content-Based Keys
*************************************************

::

  $ python3 xpyce_compile.py module1.py module2.py subdir/module3.py
  module1: a1b2c3d4e5f6789012345678901234567890123456789012345678901234
  module2: f6e5d4c3b2a1987654321098765432109876543210987654321098765432
  subdir.module3: 1234567890abcdef1234567890abcdef1234567890abcdef1234567890abcdef


Encrypt Directory with Directory-Wide Key
*****************************************

Encrypt all Python files in a directory with a single key::

  $ python3 xpyce_compile.py --encrypt-by-directory path/to/modules
  path.to.modules: a1b2c3d4e5f6789012345678901234567890123456789012345678901234

All .py files in ``path/to/modules`` are compiled and encrypted with the same key.


Using Command-Line Keys
***********************

Encrypt files with specific custom keys provided on the command line::

  $ python3 xpyce_compile.py \
      --key mymodule:a1b2c3d4e5f6789012345678901234567890123456789012345678901234 \
      --key subdir.other:f6e5d4c3b2a1987654321098765432109876543210987654321098765432 \
      mymodule.py subdir/other.py

  mymodule: a1b2c3d4e5f6789012345678901234567890123456789012345678901234
  subdir.other: f6e5d4c3b2a1987654321098765432109876543210987654321098765432


Using a Manifest File
*********************

Create a JSON manifest file (keys.json)::

  {
    "mymodule": "a1b2c3d4e5f6789012345678901234567890123456789012345678901234",
    "subdir.module1": "b2c3d4e5f6789012345678901234567890123456789012345678901234a1",
    "path.to.directory": "c3d4e5f6789012345678901234567890123456789012345678901234a1b2"
  }


Then encrypt using the manifest::

  $ python3 xpyce_compile.py --manifest keys.json mymodule.py subdir/module1.py
  mymodule: a1b2c3d4e5f6789012345678901234567890123456789012345678901234
  subdir.module1: b2c3d4e5f6789012345678901234567890123456789012345678901234a1


Combining Manifest and Command-Line Keys
****************************************

Use a manifest file as a base, override specific files on the command line::

  $ python3 xpyce_compile.py --manifest keys.json \
      --key mymodule:newkey1234567890abcdef1234567890abcdef1234567890abcdef123456 \
      mymodule.py subdir/module1.py
  mymodule: newkey1234567890abcdef1234567890abcdef1234567890abcdef123456
  subdir.module1: b2c3d4e5f6789012345678901234567890123456789012345678901234a1


Directory with Directory-Wide Key from Manifest
***********************************************

Encrypt an entire directory using a key specified in the manifest::

  $ python3 xpyce_compile.py --manifest keys.json --encrypt-by-directory /path/to/directory
  path.to.directory: c3d4e5f6789012345678901234567890123456789012345678901234a1b2


Mixed: Files and Directories
****************************

Process both individual files and directories in one command::

  $ python3 xpyce_compile.py --manifest keys.json --encrypt-by-directory \
      script.py \
      /path/to/modules/ \
      another_file.py

  script: d4e5f6789012345678901234567890123456789012345678901234a1b2c3
  path.to.modules: c3d4e5f6789012345678901234567890123456789012345678901234a1b2
  another_file: e5f6789012345678901234567890123456789012345678901234a1b2c3d4


Updating an Encrypted File
**************************

Re-encrypt a file with the same key, allowing you to update its contents
without needed to re-distribute keys::

  # Original encryption
  $ python3 xpyce_compile.py --key mymodule.py:fixed_key_value_here mymodule.py
  mymodule: a1b2c3d4e5f6789012345678901234567890123456789012345678901234

  # ... later, modify mymodule.py ...

  # Re-encrypt with the same key
  $ python3 xpyce_compile.py --key mymodule:a1b2c3d4e5f6789012345678901234567890123456789012345678901234 mymodule.py

  # Both versions decrypt with the same key (or use previously created manifest)

NOTES
-----

Key Format
**********

Encryption keys must be valid hexadecimal strings. For custom keys:

- Length: 64 characters (256 bits)

- Format: [0-9a-fA-F]{64}

- Example: a1b2c3d4e5f6789012345678901234567890123456789012345678901234

Invalid key formats will result in an error and non-zero exit status.

File Matching
*************

When processing directories, only files with .py extension are compiled.
Subdirectories are processed recursively. File exclusion is handled via the
underlying encrypt_path() function.

Content-Based Keys
******************

When no custom key is provided, encryption uses a key derived from the file
contents (SHA-256 hash). This means identical file contents will always encrypt
to the same ciphertext, enabling deduplication but making the encryption
deterministic.

When using ``--encrypt-by-directory`` we first generate a long random string
and derived the key from that value. This means directories *will not* always
encrypt to the same ciphertext. Use a ``--manifest`` or ``--key`` to generate
stable keys.

Manifest Precedence
*******************

Custom keys are applied in this order (first match wins):

1. Directory-wide key (if ``--encrypt-by-directory`` is used)

2. Command-line keys (``--key`` arguments)

3. Manifest file keys (``--manifest`` argument)

4. Content-based key (default)

AUTHOR
------

Peter Buckner <pbuck@xppython3.org> for use with X-Plane XPPython3 Plugin
