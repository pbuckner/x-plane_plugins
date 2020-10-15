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


def convert(filename):
    """
    For given *.py file,
    1) compile it to .pyc
    2) encrypt it to .xpyce
    3) return .xpyce filename and key
    """

    def get_pycfilename(filename):
        """
        Convert python filename ('foo.py', 'bar/foo.py')
        into desired pyc filename('foo.cpython-38.pyc', 'bar/foo.cpython-38.pyc')
        """
        return re.sub('.py$', '.cpython-{}{}.pyc'.format(sys.version_info.major, sys.version_info.minor), filename)

    pyc_filename = get_pycfilename(filename)
    py_compile.compile(filename, cfile=pyc_filename)
    return encrypt_path(pyc_filename)[0]


if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser('python xpyce.py')
    parser.add_argument('file', type=str, nargs='+', help="*.py file or files to be compiled and encrypted")
    args = parser.parse_args()
    results = {}
    for a in args.file:
        if os.path.isfile(a):
            new_file, key = convert(a)
            results.update({new_file: key})

    for k, v in results.items():
        print('{}: {}'.format(k, v))
