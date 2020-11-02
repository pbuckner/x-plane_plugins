"""
Python Plugin updater, modelled after PI_ScriptUpdater.py by
Joan Perez i Cauhe
"""
import hashlib
from distutils.version import StrictVersion
import json
import os
import os.path
import platform
import re
import threading
from zipfile import ZipFile
from urllib.request import urlopen, urlretrieve, URLError
try:
    from ssl import SSLCertVerificationError  # py 3.7+
except ImportError:
    from ssl import CertificateError as SSLCertVerificationError  # py < 3.7, py2

import XPPython3.xp as xp
from XPPython3.scriptconfig import Config
from XPPython3.XPProgressWindow import XPProgressWindow


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
        super(Updater, self).__init__()
        self.update_thread = None
        self.new_version = '<Error>'
        self.beta_version = '<Error>'
        self.progressWindow = XPProgressWindow("Updating {} Plugin".format(self.Name))
        self.check()
        self.json_info = {}

    def __del__(self):
        self.progressWindow.destroy()

    def check(self, forceUpgrade=False):
        xp.log("Calling Check with version check url: {}".format(self.VersionCheckURL))
        if self.VersionCheckURL:
            try:
                ret = urlopen(self.VersionCheckURL)
                if ret.getcode() != 200:
                    xp.sys_log("Failed to get {}, returned code: {}".format(self.VersionCheckURL, ret.getcode()))
                    return
            except URLError as e:
                if all([isinstance(e.reason, SSLCertVerificationError),
                        e.reason.reason == 'CERTIFICATE_VERIFY_FAILED',
                        platform.system() == 'Darwin']):
                    xp.sys_log("!!! Installation Incomplete: Run /Applications/Python<version>/Install Certificates, and restart X-Plane.")
                    return
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
                    if update_which == 'release':
                        self._update(info['download'], info.get('cksum', None))
                    else:
                        self._update(info['beta_download'], info.get('beta_cksum', None))
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

    def _update(self, download_url, cksum):
        if self.update_thread and self.update_thread.is_alive():
            self.update_thread.join()

        self.update_thread = threading.Thread(target=lambda: self._download(download_url, cksum))
        self.progressWindow.setCaption("Starting Download...")
        self.progressWindow.setProgress(0)
        self.progressWindow.show()
        self.update_thread.start()

    def _download(self, download_url, cksum):
        def hook(chunk, maxChunk, total):
            if total > 0:
                p = (chunk * maxChunk) / total
                self.progressWindow.setProgress(p)
                self.progressWindow.setCaption("Downloading [{:2.0%}]".format(p))

        self.progressWindow.setCaption("Downloading")
        success = None
        download_path = os.path.join(self.sys_path, 'Resources/Downloads/', self.Sig)
        install_path = os.path.join(self.sys_path, self.plugin_path)

        if not os.path.exists(download_path):
            xp.log("Making download directory: {}".format(download_path))
            os.makedirs(download_path)

        zipfile = download_path + '/._UPDATE.zip'
        urlretrieve(download_url, zipfile, reporthook=hook)
        self.progressWindow.setCaption("Download complete.")
        xp.log("Downloaded file: {}".format(zipfile))
        try:
            if not self._verify(zipfile, cksum):
                self.progressWindow.setCaption("Not upgraded: Field does not match checksum: incomplete download?")
                xp.sys_log("Not upgraded: File does not match checksum: incomplete download?")
                return
        except Exception as e:
            self.progressWindow.setCaption("Failed to verify download checksum.")
            xp.sys_log("Failed to verify download file checksum: {}, json has: {}. {}, ".format(self.Sig, cksum, e))

        with ZipFile(zipfile, 'r') as zipfp:
            self.progressWindow.setCaption("Testing the downloaded zip file...")
            if not zipfp.testzip():
                self.progressWindow.setCaption("Testing complete. Preparing to extract.")
                success = True
                files = zipfp.infolist()
                numfiles = len(files)
                for idx, i in enumerate(files):
                    self.progressWindow.setCaption("Extracting [{}/{}] {}".format(idx + 1, numfiles, os.path.basename(i.filename)))
                    try:
                        if os.path.exists(os.path.join(install_path, i.filename)):
                            xp.sys_log("already exists: {}".format(i.filename))
                            os.replace(os.path.join(install_path, i.filename),
                                       os.path.join(install_path, i.filename + '.bak'))
                            xp.sys_log('{} moved to {}'.format(i.filename, i.filename + '.bak'))
                        zipfp.extract(i, path=install_path)
                    except PermissionError as e:
                        success = False
                        self.progressWindow.setCaption("Extraction failed for {}.".format(i.filename))
                        xp.sys_log(">>>> Failed to extract {}, upgrade failed: {}".format(i.filename, e))
                        break
            else:
                self.progressWindow.setCaption("Test failed.")
                success = False
                xp.sys_log("failed testzip()")
        if success:
            os.remove(zipfile)
            self.progressWindow.setProgress(1)
            self.progressWindow.setCaption("Upgrade complete.\nRestart X-Plane to load new version.")
            xp.sys_log("Upgraded: restart X-Plane to load new version")
            # xp.log("this will reload plugins -- commented out")
            # xp.log("This works, but X-Plane then prompts 'Please place new plugin in directory'.. 'Understood'")
            # xp.log("which should not be necessary -- any way to stop it?")
            # plugins.reloadPlugins()

    def _verify(self, filename, file_cksum=None):
        cksum = None
        hash_md5 = hashlib.md5()
        self.progressWindow.setCaption("Verifying...")
        self.progressWindow.setProgress()
        filesize = os.path.getsize(filename)
        fileread = 0
        with open(filename, 'rb') as f:
            for chunk in iter(lambda: f.read(4086), b''):
                fileread += 4086
                p = fileread / filesize
                self.progressWindow.setProgress(p)
                self.progressWindow.setCaption("Verifying [{:2.0%}]".format(p))
                hash_md5.update(chunk)
            cksum = hash_md5.hexdigest()
        self.progressWindow.setCaption("Verification complete.")
        return file_cksum == cksum
