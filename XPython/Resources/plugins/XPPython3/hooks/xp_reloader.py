import sys
import os
from XPPython3 import xp
import importlib
VERBOSE=False
VVERBOSE=False

reload_unknown = locals()['reload_unknown']
mtimes = locals()['mtimes']
sym_start = locals()['sym_start']

result = []
for mod in list(sys.modules.values()):
    if mod and hasattr(mod, '__file__') and mod.__file__: # some don't have __file__, some __file__ are None
        try:
            mtime = os.stat(mod.__file__).st_mtime  # if it's simple foo.py
            if VERBOSE: xp.log(f'mtime for {mod} is {mtime}')
        except (OSError, IOError) as e:
            result.append(f'Opps for {mod.__file__}: {e}')
            continue
        if mod.__file__.endswith('.pyc') and os.path.exists(mod.__file__[:-1]):  # if there is (also) a pyc
            mtime = max(mtime, os.stat(mod.__file__[:-1]).st_mtime)
            if VERBOSE: xp.log(f'mtime now {mtime} after looking at pyc')
        if (not reload_unknown and mod not in mtimes) or (mtime <= sym_start and mod not in mtimes):
            mtimes [mod] = mtime
            if VERBOSE: xp.log(f'Setting mtimes[{mod}] to {mtime} (1)')
        elif (reload_unknown and not mod in mtimes and mtime > sym_start)  or mtimes[mod] < mtime:
            mtimes[mod] = mtime
            if VERBOSE: xp.log(f'Setting mtimes[{mod}] to {mtime} (2)')
            if hasattr(mod, '__spec__') and hasattr(mod.__spec__, 'origin') and mod.__spec__.origin in ('frozen', 'builtin'):
                if VERBOSE: xp.log(f'skipping due to origin {mod.__spec__.origin}')
                continue
            try:
                result.append(mod.__file__)
                importlib.reload(mod)
                mtimes[mod] = mtime
            except ImportError:
                result.append(f'>> Failed to reload {mod.__file__}')
                pass
            except Exception as e:
                result.append(f'>> Failed reloading {mod.__file__}: {e}')
    else:
        if VERBOSE and VVERBOSE: xp.log(f'no file for {mod}')
result = '\n  > '.join(result)
