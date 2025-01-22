from typing import List, Self
from types import ModuleType
from XPPython3 import xp
try:
    # we do this instead of simple 'import requests' because
    # 'requests' imports charset_normalizer, which had a bug
    # we need to be able to continue in light of this bug, so we can
    # automatically correct it later, but re-installing new version of
    # that module
    requests: ModuleType | None
    import requests
except ImportError:
    xp.log("requests module import error")
    requests = None


class Usage:
    UsagePostURL = 'https://maps.avnwx.com/x-plane/usage'

    def __init__(self: Self, preferences: dict) -> None:
        if not preferences.get('collect_python_plugin_stats', False):
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
        if plugins and requests is not None:
            try:
                # blast it & not wait for response
                # ( note that interface.uuid may be blank, but pref['uuid'] has value read from preference file
                #   This occurs when user doesn't want to record xppython_stats, but does want to record pythonplugin stats
                #   ... though I'd expect this to be a rare occurance.)
                requests.post(self.UsagePostURL, json={'uuid': preferences.get('uuid', ''), 'plugins': plugins}, timeout=(5, 0.0001))
            except (requests.ReadTimeout, requests.ConnectTimeout, requests.ConnectionError, requests.HTTPError):
                pass
