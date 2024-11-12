"""
Python Plugin updater, modelled after PI_ScriptUpdater.py by
Joan Perez i Cauhe
"""
import json
import os
import os.path
import platform
import certifi
from urllib.request import urlopen
from urllib.parse import urlencode
from urllib.error import URLError
try:
    from ssl import SSLCertVerificationError  # py 3.7+
except ImportError:
    from ssl import CertificateError as SSLCertVerificationError  # py < 3.7, py2

from XPPython3 import xp
from XPPython3.scriptconfig import Config
from XPPython3.zip_download import ZipDownload
from XPPython3.updater.version import calc_update, Version


class Updater(Config):
    VersionCheckURL = "http://example.com/foo.json"
    VersionCheckData = None

    """
    To Use
    ------
    Set this constant:

      VersionCheckURL: Should be set by inherited class to full URL of the version json file.
              e.g., "https://github.com/plugindeveloper1/version.json"

      Format of json file is:
        ┌──────────────────────────────
        │{
        │  "<plugin1 signature>": {
        │         "upgrade":      "<optional instructions written to log. "See documentation" if not provided>",
        │         "autoUpgrade":  "<optional boolean: Can this be auto upgraded, or is user confirmation required?"
        |                         "(false if not provided)>",
        │         "version":  "<required stable version string>",
        │         "download": "<required full URL to file to download for stable version>",
        │         "cksum":    "<required md5sum cksum of file to be downloaded>",
        │         "beta_version":  "<optional beta or pre-release version string>",
        │         "beta_download": "<optional full URL to file to download for beta version. "
        |                          "Required if beta_version is provided>",
        │         "beta_cksum":    "<optional md5sum cksum of beta file. Required if beta_version is provided>",
        │  },
        │  "<plugin2.signature>": {
        │         ...
        │  }
        │}
        └──────────────────────────────

    External Functions:

      check(forceUpgrade:bool)
          Check to see if a more receive version is available and if so, or if forceUpgrade, attempt to upgrade.
          check() is _always_ called at startup. You may _also_ choose to call check() based on
          buttons / menu items in your plugin code.

      calc_update(try_beta:bool, current_version:str, stable_version:str, beta_version:str)
          returns (up_to_date: bool, new_version: str)

          Based on inputs, calculate if provided "current_version" is the latest.
          "stable_version" and "beta_version" are provided and compared with "current_version"
          "try_beta" indicates the caller has preference to use a more recent beta (or "pre-release")
          if available. Otherwise, betas are ignored. (See documentation of Version for format of numbers)
    """
    def __init__(self):
        self.initial_progress_msg = f"Updating {self.Name} Plugin"
        self.final_progress_msg = "Upgrade complete.\nRestart X-Plane to load new version."
        super(Updater, self).__init__()
        self.download_path = os.path.join(xp.getSystemPath(), 'Resources', 'Downloads', self.Sig)
        self.install_path = os.path.join(xp.getSystemPath(), self.plugin_path)
        self.new_version = '<Error>'
        self.beta_version = '<Error>'
        self.json_info = {}
        try:
            if any([getattr(self, 'do_not_check_for_updates', True),
                    not getattr(getattr(self, 'preferences'), 'preferences').get('check_for_update', False),
                    getattr(getattr(self, 'preferences'), 'preferences').get('debug', True)]):
                self.new_version = "<not checked>"
                self.beta_version = "<not checked>"
                return
        except AttributeError:
            pass

        if xp.getCycleNumber() == 0:
            xp.log(">>>>>>>>>> checking <<<<<<<<<<<<<")
            self.check()
        else:
            xp.log(f"*** Cyclenumber is {xp.getCycleNumber()=}, skipping update checking")

    def check(self, forceUpgrade=False):
        xp.log(f"Calling Check with version check url: {self.VersionCheckURL}")
        if self.VersionCheckURL:
            if self.VersionCheckData is not None:
                if hasattr(self, 'uuid'):
                    self.VersionCheckData['uuid'] = self.uuid
                data = urlencode(self.VersionCheckData).encode('utf-8')
            else:
                data = None
            try:
                ret = urlopen(self.VersionCheckURL + ('&beta=y' if self.config.get('beta', False) else ''), data=data,
                              cafile=certifi.where())
                if ret.getcode() != 200:
                    xp.systemLog(f"Failed to get {self.VersionCheckURL}, returned code: {ret.getcode()}")
                    return
            except URLError as e:
                xp.log(f"URLError is {e}")
                try:
                    if ((isinstance(e.reason, SSLCertVerificationError)
                         and e.reason.reason == 'CERTIFICATE_VERIFY_FAILED'
                         and platform.system() == 'Darwin')):
                        msg = ("\nError: !!! Python Installation Incomplete:\n"
                               "    Run /Applications/Python<version>/Install Certificates, and restart X-Plane.\n"
                               "    See https://xppython3.readthedocs.io/en/latest/usage/common_errors.html\n")
                        xp.systemLog(msg)
                        xp.log(msg)
                        return
                except Exception:
                    pass
                xp.log("Internet connection error, cannot check version. Will check next time.")
                return
            except Exception as e:
                xp.log(f"Failed with urllib: {e}")
                return
            try:
                data = ret.read()
                ret.close()
                del ret
            except Exception as e:
                xp.log(f"Failed reading result: {e}")
                return
            try:
                self.json_info = json.loads(data)
            except Exception as e:
                xp.log(f"Failed converting to json: {e}")
                return
            try:
                info = self.json_info[self.Sig]
            except KeyError:
                try:
                    if self.Sig.startswith("xppython3"):
                        info = self.json_info['com.avnwx.' + self.Sig]
                    else:
                        raise
                except KeyError as e:
                    xp.log(f"Failed to find key: {e}")
                    return

            try:
                self.new_version = info.get('version', {})
            except KeyError:
                xp.log(f"scriptupdater cannot determine update version information for plugin {self.Sig}: {info}")
                return
            self.beta_version = info.get('beta_version', '')
            uptodate, version = calc_update(try_beta=self.config.get('beta', False),
                                            current_version=self.Version,
                                            stable_version=self.new_version,
                                            beta_version=self.beta_version)
            uptodate = uptodate and not forceUpgrade
            update_which = None if uptodate else ('beta' if version == self.beta_version else 'release')

            if update_which:
                xp.systemLog(f">>>>> A new version is available: v.{self.Version} -> v.{version}.")
                if forceUpgrade or (info.get('autoUpgrade', False) and self.config and self.config.get('autoUpgrade', False)):
                    xp.systemLog(">>>>> Automatically upgrading")
                    z = ZipDownload()
                    z.install_path = self.install_path
                    z.initial_progress_msg = self.initial_progress_msg
                    z.final_progress_msg = self.final_progress_msg
                    z.backup = False
                    z.remove_top_dir = False
                    if update_which == 'release':
                        z.get_zipfile(info['download'], info.get('cksum', None))
                    else:
                        z.get_zipfile(info['beta_download'], info.get('beta_cksum', None))
                else:
                    xp.systemLog(f">>>>> To upgrade: {info.get('upgrade', 'See documentation')}")
            else:
                xp.log("Version is up to date")
                xp.systemLog("Version is up to date")
