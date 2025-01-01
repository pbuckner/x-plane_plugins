from XPPython3 import xp
from typing import Optional
import re
import tempfile
import os
import queue
import threading
import stat
import subprocess
from XPPython3.ui.popups import ScrollingPopup
from pip._vendor.packaging.version import Version
from pip._vendor.packaging.requirements import Requirement


# this is no longer used. By specifying 'target' we don't properly get pip list to update.:
# # location where we want PIP to install packages. Relative to X-Plane directory
# PKG_DIR = {
#     'linux': 'Resources/plugins/XPPython3/lin_x64/python3.12/lib/python3.12/dist-packages',
#     'win32': 'Resources/plugins/XPPython3/win_x64/Lib/site-packages',
#     'darwin': 'Resources/plugins/XPPython3/mac_x64/python3.12/lib/python3.12/site-packages',
# }


def load_requirements(requirements: Optional[list | str] = None, force: bool = False,
                      start_message: Optional[str] = None, end_message: Optional[str] = None) -> bool:
    if not requirements:
        return False
    requirements = [requirements,] if isinstance(requirements, str) else requirements
    if start_message is None:
        start_message = "Checking and updating required python packages.\n"
    if end_message is None:
        end_message = "\nThis information has been added to XPPython3 log file."

    # target = PKG_DIR[sys.platform]
    mode = os.stat(xp.pythonExecutable).st_mode
    if not mode & stat.S_IXUSR:
        os.chmod(xp.pythonExecutable, mode | stat.S_IXUSR | stat.S_IXGRP)

    if not force:
        cmd = [xp.pythonExecutable,
               '-m', 'pip', 'list']
        installed: dict = {}
        with subprocess.Popen(cmd,
                              stdout=subprocess.PIPE,
                              stderr=subprocess.STDOUT,
                              bufsize=-1,
                              encoding='utf-8',
                              universal_newlines=True) as sub:
            if sub.stdout is not None:
                for line in sub.stdout:
                    try:
                        module, version_str = re.split(' +', line)
                        if module == 'Package' or module.startswith('---'):
                            continue
                        installed[module] = Version(version_str)
                    except ValueError:
                        pass
        needed: list[str] = []
        for req in requirements:
            require = Requirement(req)
            if require.name not in installed:
                needed.append(req)
                xp.log(f"xp_pip needs {require.name} which is not yet installed.")
                force = True
            elif not list(require.specifier.filter([installed[require.name], ])):
                needed.append(req)
                xp.log(f"xp_pip found {require.name} version {str(installed[require.name])} needs to install version {str(require.specifier)}")
                force = True

    if force:
        with tempfile.NamedTemporaryFile(delete=False) as fp:
            fp.write("\n".join(needed).encode('utf-8'))
            tmp_file = fp.name
        cmd = [xp.pythonExecutable,
               '-s', '-m', 'pip', 'install', '--no-warn-script-location',
               # '--target', target,
               '--upgrade',
               '-r', tmp_file,
               ]
        startPipCall(cmd, {'start': start_message, 'end': end_message}, temp_file=tmp_file)
        return True
    else:
        return False


def load_packages(packages: Optional[list | str] = None, start_message: Optional[str] = None, end_message: Optional[str] = None) -> bool:
    if not packages:
        return False
    packages = [packages,] if isinstance(packages, str) else packages
    if start_message is None:
        start_message = ""
    if end_message is None:
        end_message = "This information has been added to XPPython3 log file."

    # target = PKG_DIR[sys.platform]
    mode = os.stat(xp.pythonExecutable).st_mode
    if not mode & stat.S_IXUSR:
        os.chmod(xp.pythonExecutable, mode | stat.S_IXUSR | stat.S_IXGRP)
    cmd = [xp.pythonExecutable,
           '-s', '-m', 'pip', 'install', '--no-warn-script-location',
           '--upgrade',
           # '--target', target,
           ] + packages
    startPipCall(cmd, {'start': start_message, 'end': end_message}, temp_file=None)
    return True


def startPipCall(cmd: Optional[list] = None, msgs: Optional[dict] = None, temp_file: Optional[str] = None):
    all_msgs = {'start': None, 'end': None}
    all_msgs.update(msgs or {})

    pip_output_popup = ScrollingPopup("PIP Output", 100, 400, 600, 100)
    q: queue.Queue = queue.Queue()

    if all_msgs['start']:
        for msg_line in all_msgs['start'].split('\n'):
            q.put(msg_line)

    t = threading.Thread(name="pip", target=pip, args=[cmd, q])
    t.start()
    pipWindowLoopID = xp.createFlightLoop(pipWindowLoop,
                                          refCon={'q': q,
                                                  'popup': pip_output_popup,
                                                  'msgs': all_msgs,
                                                  'tempfile': temp_file})
    xp.scheduleFlightLoop(pipWindowLoopID, -1)


def pipWindowLoop(_since, _elapsed, _counter, refCon):
    q = refCon['q']
    pip_output_popup = refCon['popup']
    msgs = refCon['msgs']
    try:
        line = q.get_nowait()
        if line == '[DONE]':
            pip_output_popup.add(" -- [Completed] --")
            for i in pip_output_popup.get_data():
                xp.log(f"PIP>> {i}")
            if msgs['end']:
                for msg_line in msgs['end'].split('\n'):
                    pip_output_popup.add(msg_line)
            if refCon['tempfile']:
                try:
                    os.unlink(refCon['tempfile'])
                except (PermissionError, FileNotFoundError):
                    pass
            return 0
        pip_output_popup.add(line)
    except queue.Empty:
        pass
    return -10  # Check again in 10 frames


def pip(cmd, q):
    xp.log(f"Doing {' '.join(cmd)}")
    q.put(f"$ {' '.join(cmd)}")
    try:
        with subprocess.Popen(cmd,
                              stdout=subprocess.PIPE,
                              stderr=subprocess.STDOUT,
                              bufsize=1,
                              encoding='utf-8',
                              universal_newlines=True) as sub:
            for line in sub.stdout:
                if line[-1] == '\n':
                    line = line[:-1]
                q.put(line)
        xp.log("All done")
    except PermissionError:
        xp.log(f"Error: {cmd[0]} does not have executable permission.")
    except FileNotFoundError:
        xp.log(f"Error: {cmd[0]} not found.")
    q.put('[DONE]')
