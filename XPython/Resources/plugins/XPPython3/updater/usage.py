from typing import List
from XPPython3 import xp
import requests


class Usage:
    UsagePostURL = 'https://maps.avnwx.com/x-plane/usage'

    def __init__(self, interface):
        pref = interface.preferences.preferences
        if pref['debug'] or not pref['collect_python_plugin_stats']:
            # if debug is set, or 'collect_python_plugin_stats is set to False, skip data collection
            return

        plugins: List = []
        for (interfaceObject, info) in xp.pythonGetDicts()['plugins'].items():
            _name, sig, description, _module, module_name, disabled = info[0:6]  # [0:6] in case we add more variables later...
            if not (module_name.startswith('XPPython3.') or disabled):
                try:
                    if getattr(interfaceObject, 'xp3_disable_usage_stats'):
                        continue
                except AttributeError:
                    pass
                plugins.append((module_name, sig, description))
        if plugins:
            try:
                # blast it & not wait for response
                requests.post(self.UsagePostURL, json={'uuid': interface.uuid, 'plugins': plugins}, timeout=(5, 0.0001))
            except (requests.ReadTimeout, requests.ConnectTimeout, requests.ConnectionError, requests.HTTPError):
                pass
