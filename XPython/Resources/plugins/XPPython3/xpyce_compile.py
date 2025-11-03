#!env python3.12

# Because the resulting code needs to execute within the X-Plane-embedded python interpreter,
# and that is running python 3.12. We _must_ compile this code with the same Major.Minor version
# of python.
#
# It is possible to use the same python binary as included with X-Plane XPPython3, though
# "how" differs for the different platforms:
#
# MacOS
# -----
# $ <X-Plane>/Resources/plugins/XPPython3/mac_x64/python3.12/Resources/Python.app/Contents/MacOS/Python \
#        <X-Plane>/Resources/plugins/XPPython3/xpyce_compile.py [args]
#
# Linux
# -----
# # <X-Plane>/Resources/plugins/XPPython3/lin_x64/python3.12/bin/python3.12 \
#        <X-Plane>/Resources/plugins/XPPython3/xpyce_compile.py [args]
#
# Windows
# -------
# # <X-Plane>/Resources/plugins/XPPython3/win_x64/pythonw.exe \
#        <X-Plane>/Resources/plugins/XPPython3/xpyce_compile.py [args]


import json
import os
import os.path
import re
import sys
import py_compile
try:
    from xpyce import encrypt_path
except ImportError:
    # Assume we're executing this from PythonPlugin directory, so add possible path and retry
    sys.path.append('../XPPython3')
    from xpyce import encrypt_path


def compile_file(filename):
    """
    Compile a single .py file to .pyc
    Returns the .pyc filename
    """
    pyc_filename = re.sub('.py$', '.pyc', filename)
    py_compile.compile(filename, cfile=pyc_filename, optimize=2)
    return pyc_filename


def compile_directory(directory):
    """
    Compile all .py files in a directory to .pyc
    Returns list of .pyc filenames
    """
    pyc_files = []
    for root, _dirs, files in os.walk(directory):
        for filename in files:
            if filename.endswith('.py'):
                filepath = os.path.join(root, filename)
                pyc_file = compile_file(filepath)
                pyc_files.append(pyc_file)
    return pyc_files


def load_custom_keys(manifest_file, key_args):
    """
    Load custom keys from manifest file and/or command-line arguments.
    Command-line keys override manifest file keys.

    Args:
        manifest_file: path to JSON file with key mappings (optional)
        key_args: list of 'path:key' strings from --key arguments

    Returns:
        dict mapping paths to hex key strings
    """
    custom_keys = {}

    # Load from manifest file if provided
    if manifest_file:
        try:
            with open(manifest_file, 'r', encoding='UTF-8') as f:
                custom_keys = json.load(f)
        except FileNotFoundError:
            print(f"Error: Manifest file '{manifest_file}' not found", file=sys.stderr)
            sys.exit(1)
        except json.JSONDecodeError as e:
            print(f"Error: Invalid JSON in manifest file: {e}", file=sys.stderr)
            sys.exit(1)

    # Override with command-line keys
    if key_args:
        for key_arg in key_args:
            try:
                path, key = key_arg.split(':', 1)
                custom_keys[path] = key.strip()
            except ValueError:
                print(f"Error: Invalid key argument format '{key_arg}'. Expected 'path:key'", file=sys.stderr)
                sys.exit(1)

    return custom_keys


if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser('python3 xpyce_compile.py')
    parser.add_argument('path', type=str, nargs='+', help="*.py file(s) or directory(ies) to be compiled and encrypted")
    parser.add_argument('--encrypt-by-directory', action='store_true',
                        help="If path is a directory, encrypt all matching files with the same key")
    parser.add_argument('--manifest', type=str, default=None,
                        help="JSON file mapping file/directory paths to encryption keys")
    parser.add_argument('--key', type=str, action='append', dest='keys', default=[],
                        help="Encryption key in format 'path:key' (can be used multiple times)")
    args = parser.parse_args()

    # Load custom keys from manifest and command-line arguments
    my_custom_keys = load_custom_keys(args.manifest, args.keys)
    results = {}

    for my_path in args.path:
        if os.path.isfile(my_path):
            if my_path.endswith('.py'):
                my_pyc_file = compile_file(my_path)
                manifest = encrypt_path(my_pyc_file, custom_keys=my_custom_keys,
                                        encrypt_by_directory=args.encrypt_by_directory)
                for file_path, my_key in manifest:
                    results.update({file_path: my_key})
        elif os.path.isdir(my_path):
            # Compile all .py files in the directory
            compile_directory(my_path)
            # Encrypt with directory-wide key if flag is set
            manifest = encrypt_path(my_path, custom_keys=my_custom_keys,
                                    encrypt_by_directory=args.encrypt_by_directory)
            for file_path, my_key in manifest:
                results.update({file_path: my_key})

    for k, v in results.items():
        print('{}: {}'.format(k, v))
