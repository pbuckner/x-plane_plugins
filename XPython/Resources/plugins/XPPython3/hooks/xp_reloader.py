# XP_RELOADER
#
# Called by XPPython3.xpl during module reload
#
# Go through all modules (it appears that sys.modules provides values _in_order_
# though I don't know that's guaranteed.
#
# Once we find _a_ module which should be reloaded, we'll reload _all_
# subsequent modules _under_ "Resources/plugins"

from XPPython3 import xp
import os
import sys
import importlib
import time

# OPTIONAL INPUTS:
my_reload_unknown = locals().get('reload_unknown', False)
my_mtimes = locals().get('mtimes', {})
my_sym_start = locals().get('sym_start', time.time())

# KEY OUTPUT:
result = ''  # list of update modules (or errors)

VERBOSE = False
VVERBOSE = False


def hook(reload_unknown: bool, mtimes: dict, sym_start: float) -> str:
    reload_flag = False
    system_path = xp.getSystemPath()
    updated_mods = []

    for mod in list(sys.modules.values()):
        if mod and hasattr(mod, '__file__') and mod.__file__:  # some don't have __file__, some __file__ are None
            filename = mod.__file__
            try:
                mtime = os.stat(filename).st_mtime  # if it's simple foo.py
                if VERBOSE:
                    xp.log(f'mtime for {mod} is {mtime}')
            except (OSError, IOError) as e:
                if filename.endswith('.py') and os.path.exists(filename + 'c'):  # missing .py, but there is a pyc
                    mtime = os.stat(filename + 'c').st_mtime
                    if VERBOSE:
                        xp.log(f'mtime for {mod} is {mtime}')
                else:
                    updated_mods.append(f'Cannot find mtime for {filename}: {e}')
                    continue
            if filename.endswith('.pyc') and os.path.exists(filename[:-1]):  # if there is (also) a pyc
                mtime = max(mtime, os.stat(filename[:-1]).st_mtime)
                if VERBOSE:
                    xp.log(f'mtime now {mtime} after looking at pyc')
            if (not reload_unknown and mod not in mtimes) or (mtime <= sym_start and mod not in mtimes):
                mtimes[mod] = mtime
                if VERBOSE:
                    xp.log(f'Setting mtimes[{mod}] to {mtime} (1)')
            elif (reload_unknown and mod not in mtimes and mtime > sym_start) or mtimes[mod] < mtime:
                mtimes[mod] = mtime
                if VERBOSE:
                    xp.log(f'Setting mtimes[{mod}] to {mtime} (2)')
                if hasattr(mod, '__spec__') and hasattr(mod.__spec__, 'origin') and mod.__spec__.origin in ('frozen', 'builtin'):  # pylint: disable=no-member
                    if VERBOSE:
                        xp.log(f'skipping due to origin {mod.__spec__.origin}')
                    continue
                reload_flag = True
                updated_mods.append(filename)

            if reload_flag and filename.startswith(system_path):
                try:
                    if updated_mods[-1] == filename:
                        updated_mods[-1] = updated_mods[-1] + " [Changed]"
                    else:
                        updated_mods.append(filename)  # we didn't see a change to _this_ mod, but will reload anyway
                    mtimes[mod] = mtime
                    importlib.reload(mod)
                except ImportError:
                    updated_mods.append(f'>> Failed to reload {filename}')
                except Exception as e:  # pylint: disable=broad-except
                    updated_mods.append(f'>> Failed reloading {filename}: {e}')
    return '\n  > '.join(updated_mods)


result = hook(my_reload_unknown, my_mtimes, my_sym_start)
