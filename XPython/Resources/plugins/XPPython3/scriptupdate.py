"""
Python Plugin updater, modelled after PI_ScriptUpdater.py by
Joan Perez i Cauhe
"""
from distutils.version import StrictVersion
import json
import os
import os.path
import platform
import re
from urllib.request import urlopen
from urllib.parse import urlencode
from urllib.error import URLError
try:
    from ssl import SSLCertVerificationError  # py 3.7+
except ImportError:
    from ssl import CertificateError as SSLCertVerificationError  # py < 3.7, py2

import XPPython3.xp as xp
from XPPython3.scriptconfig import Config
from XPPython3.zip_download import ZipDownload


class Version(StrictVersion):
    """
    Extract first thing which looks like a version number from
    provided string and convert using StrictVersion.

     1) Version number is first extracted, as the first "word" with leading number, through to first space'
         '3.0.1 Release'         -> '3.0.1'
         '3.0.1a1 Beta'          -> '3.0.1a1'
         'Release 3.0.1a1 Beta'  -> '3.0.1a1'
         'Version-3 Beta 2'      -> '3'

     2) If version number ends with letter, we append '0' to it. This permits us to use step 3
         '3.0.1'   -> '3.0.1'
         '3.0.1a'  -> '3.0.1a0'
         '3.0.1a1' -> '3.0.1a1'

     3) Comparison uses distutils.version.StrictVersion (note that pre-release letter is _only_ a or b):
       * dot separated Major, Minor, Patch values:
            3.0
            3.1
            3.1.10
       * number are compared numerically, missing patch value is 'zero' (Major and Minor are required)
            3.0.0
            3.1.0
            3.1.10
       * optional 'pre-release' after version number consists of letter 'a' or 'b' _with_ a number
            3.0a1
            3.1a2
            3.1.0a11
            3.1.10a1
            3.1.10a2
            3.1.10a10
       * 'pre-release' are less than release of same number
            3.1.9
            3.1.10a1
            3.1.10
    """
    def __init__(self, vstring=None):
        try:
            vstring = re.search(r'\d[\.\dab]*', vstring)[0]
        except TypeError:
            vstring = '0.0'
        if vstring[-1].isalpha():
            vstring += '0'
        super(Version, self).__init__(vstring)


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
        │         "autoUpgrade":  "<optional boolean: Can this be auto upgraded, or is user confirmation required? (false if not provided)>",
        │         "version":  "<required stable version string>",
        │         "download": "<required full URL to file to download for stable version>",
        │         "cksum":    "<required md5sum cksum of file to be downloaded>",
        │         "beta_version":  "<optional beta or pre-release version string>",
        │         "beta_download": "<optional full URL to file to download for beta version. Required if beta_version is provided>",
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
        self.initial_progress_msg = "Updating {} Plugin".format(self.Name)
        self.final_progress_msg = "Upgrade complete.\nRestart X-Plane to load new version."
        super(Updater, self).__init__()
        self.download_path = os.path.join(xp.getSystemPath(), 'Resources', 'Downloads', self.Sig)
        self.install_path = os.path.join(xp.getSystemPath(), self.plugin_path)
        self.new_version = '<Error>'
        self.beta_version = '<Error>'
        self.check()
        self.json_info = {}

    def check(self, forceUpgrade=False):
        xp.log("Calling Check with version check url: {}".format(self.VersionCheckURL))
        if self.VersionCheckURL:
            if self.VersionCheckData is not None:
                data = urlencode(self.VersionCheckData).encode('utf-8')
            else:
                data = None
            try:
                ret = urlopen(self.VersionCheckURL + ('&beta=y' if self.config.get('beta', False) else ''), data=data)
                if ret.getcode() != 200:
                    xp.sys_log("Failed to get {}, returned code: {}".format(self.VersionCheckURL, ret.getcode()))
                    return
            except URLError as e:
                xp.log("URLError is {}".format(e))
                try:
                    if ((isinstance(e.reason, SSLCertVerificationError)
                         and e.reason.reason == 'CERTIFICATE_VERIFY_FAILED'
                         and platform.system() == 'Darwin')):
                        msg = ("\nError: !!! Python Installation Incomplete:\n"
                               "    Run /Applications/Python<version>/Install Certificates, and restart X-Plane.\n"
                               "    See https://xppython3.readthedocs.io/en/latest/usage/common_errors.html\n")
                        xp.sys_log(msg)
                        xp.log(msg)
                        return
                except Exception:
                    pass
                xp.log("Internet connection error, cannot check version. Will check next time.")
                return
            except Exception as e:
                xp.log("Failed with urllib: {}".format(e))
                return
            try:
                data = ret.read()
            except Exception as e:
                xp.log("Failed reading result: {}".format(e))
                return
            try:
                self.json_info = json.loads(data)
            except Exception as e:
                xp.log("Failed converting to json: {}".format(e))
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
                    xp.log("Failed to find key: {}".format(e))
                    return

            self.new_version = info['version']
            self.beta_version = info.get('beta_version', '')
            uptodate, version = self.calc_update(try_beta=self.config.get('beta', False),
                                                 current_version=self.Version,
                                                 stable_version=self.new_version,
                                                 beta_version=self.beta_version)
            uptodate = uptodate and not forceUpgrade
            update_which = None if uptodate else ('beta' if version == self.beta_version else 'release')

            if update_which:
                xp.sys_log(">>>>> A new version is available: v.{} -> v.{}.".format(self.Version, version))
                if forceUpgrade or (info.get('autoUpgrade', False) and self.config and self.config.get('autoUpgrade', False)):
                    xp.sys_log(">>>>> Automatically upgrading")
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
                    xp.sys_log(">>>>> To upgrade: {}".format(info.get('upgrade', 'See documentation')))
            else:
                xp.log("Version is up to date")

    @staticmethod
    def calc_update(try_beta, current_version, stable_version, beta_version):
        """
        returns tuple (up-to-date: T/F, change to: version)
        """
        current = Version(current_version)
        stable = Version(stable_version)
        beta = Version(beta_version)
        if try_beta:
            if beta > current:
                if stable > beta:
                    return (False, stable_version)
                return (False, beta_version)
            if beta == current and stable < beta:
                return (True, beta_version)
            if stable > current:
                return (False, stable_version)
            return (True, current_version)
        if stable > current:
            return (False, stable_version)
        if stable == current:
            return (True, current_version)
        # If i have a beta (any beta), go with stable_version
        # otherwise, stick with what I have.
        if current.prerelease:
            # you have beta
            return (False, stable_version)
        return (True, current_version)
