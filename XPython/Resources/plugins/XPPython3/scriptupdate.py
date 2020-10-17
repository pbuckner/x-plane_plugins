"""
Python Plugin updater, modelled after PI_ScriptUpdater.py by
Joan Perez i Cauhe
"""
import os
import os.path
import platform
from zipfile import ZipFile
import hashlib
import json
try:
    from urllib.request import urlopen, urlretrieve, URLError  # py3
except ImportError:
    from urllib import urlopen, urlretrieve  # py2
    from urllib2 import URLError
try:
    from ssl import SSLCertVerificationError  # py 3.7+
except ImportError:
    from ssl import CertificateError as SSLCertVerificationError  # py < 3.7, py2

import time
from XPPython3 import scriptconfig
from xp import log, sys_log
system_log = sys_log  # some older python plugins use system_log

class Updater(scriptconfig.Config):
    VersionCheckURL = "http://example.com/foo.json"

    def __init__(self):
        super(Updater, self).__init__()
        self.new_version = '<Error>'
        self.beta_version = '<Error>'
        self.check()
        self.json_info = {}

    def check(self, forceUpgrade=False):
        log("Calling Check with version check url: {}".format(self.VersionCheckURL))
        if self.VersionCheckURL:
            try:
                ret = urlopen(self.VersionCheckURL)
                if ret.getcode() != 200:
                    sys_log("Failed to get {}, returned code: {}".format(self.VersionCheckURL, ret.getcode()))
                    return
            except URLError as e:
                if all([isinstance(e.reason, SSLCertVerificationError),
                        e.reason.reason == 'CERTIFICATE_VERIFY_FAILED',
                        platform.system() == 'Darwin']):
                    sys_log("!!! Installation Incomplete: Run /Applications/Python<version>/Install Certificates, and restart X-Plane.")
                    return
                log("Internet connection error, cannot check version. Will check next time.")
                return
            except Exception as e:
                log("Failed with urllib: {}".format(e))
                return
            try:
                data = ret.read()
            except Exception as e:
                log("Failed reading result: {}".format(e))
                return
            try:
                self.json_info = json.loads(data)
            except Exception as e:
                log("Failed converting to json: {}".format(e))
                return
            try:
                info = self.json_info[self.Sig]
            except KeyError as e:
                log("Failed to find key: {}".format(e))
                return

            self.new_version = info['version']
            self.beta_version = info.get('beta_version', '')
            uptodate, version = self.calc_update(try_beta=self.config.get('beta', False),
                                                 current=self.Version,
                                                 stable_version=self.new_version,
                                                 beta_version=self.beta_version)
            uptodate = uptodate and not forceUpgrade
            update_which = None if uptodate else ('beta' if version == self.beta_version else 'release')

            if update_which:
                sys_log(">>>>> A new version is available: v.{} -> v.{}.".format(self.Version, version))
                if forceUpgrade or (info.get('autoUpgrade', False) and self.config and self.config.get('autoUpgrade', False)):
                    sys_log(">>>>> Automatically upgrading")
                    if update_which == 'release':
                        self.update(info['download'], info.get('cksum', None))
                    else:
                        self.update(info['beta_download'], info.get('beta_cksum', None))
                else:
                    sys_log(">>>>> To upgrade: {}".format(info.get('upgrade', 'See documentation')))
            else:
                log("Version is up to date")

    def calc_update(self, try_beta, current, stable_version, beta_version):
        """
        returns tuple (up-to-date: T/F, change to: version)
        """
        if try_beta:
            if beta_version > current:
                if stable_version > beta_version:
                    return (False, stable_version)
                return (False, beta_version)
            if beta_version == current and stable_version < beta_version:
                return (True, beta_version)
            if stable_version > current:
                return (False, stable_version)
            return (True, current)
        if stable_version > current:
            return (False, stable_version)
        if stable_version == current:
            return (True, current)
        # If i have a beta (any beta), go with stable_version
        # otherwise, stick with what I have.
        if len(current) > len(stable_version):
            # you have beta
            return (False, stable_version)
        return (True, current)

    def update(self, download_url, cksum):
        success = None
        download_path = os.path.join(self.sys_path, 'Resources/Downloads/', self.Sig)
        install_path = os.path.join(self.sys_path, self.plugin_path)

        # Message to self to stop
        time.sleep(1)

        if not os.path.exists(download_path):
            log("Making download directory: {}".format(download_path))
            os.makedirs(download_path)

        zipfile = download_path + '/._UPDATE.zip'
        urlretrieve(download_url, zipfile)
        log("Downloaded file: {}".format(zipfile))
        try:
            if not self.verify(zipfile, cksum):
                sys_log("Not upgraded: File does not match checksum: incomplete download?")
                return
        except Exception as e:
            sys_log("Failed to verify download file checksum: {}, json has: {}. {}, ".format(self.Sig, cksum, e))

        with ZipFile(zipfile, 'r') as zipfp:
            if not zipfp.testzip():
                success = True
                for i in zipfp.infolist():
                    try:
                        if os.path.exists(os.path.join(install_path, i.filename)):
                            sys_log("already exists: {}".format(i.filename))
                            os.replace(os.path.join(install_path, i.filename),
                                       os.path.join(install_path, i.filename + '.bak'))
                            sys_log('{} moved to {}'.format(i.filename, i.filename + '.bak'))
                        zipfp.extract(i, path=install_path)
                    except PermissionError as e:
                        success = False
                        sys_log(">>>> Failed to extract {}, upgrade failed: {}".format(i.filename, e))
                        break
            else:
                success = False
                sys_log("failed testzip()")
        if success:
            os.remove(zipfile)
            sys_log("Upgraded: restart X-Plane to load new version")
            # log("this will reload plugins -- commented out")
            # log("This works, but X-Plane then prompts 'Please place new plugin in directory'.. 'Understood'")
            # log("which should not be necessary -- any way to stop it?")
            # plugins.reloadPlugins()

    def verify(self, filename, file_cksum=None):
        cksum = None
        hash_md5 = hashlib.md5()
        with open(filename, 'rb') as f:
            for chunk in iter(lambda: f.read(4086), b''):
                hash_md5.update(chunk)
            cksum = hash_md5.hexdigest()
        return file_cksum == cksum
