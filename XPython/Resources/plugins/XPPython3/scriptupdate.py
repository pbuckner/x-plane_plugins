"""
Python Plugin updater, modelled after PI_ScriptUpdater.py by
Joan Perez i Cauhe
"""
import threading
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
from XPPython3.XPProgressWindow import XPProgressWindow
from xp import log, sys_log
system_log = sys_log  # some older python plugins use system_log


class Updater(scriptconfig.Config):
    VersionCheckURL = "http://example.com/foo.json"

    def __del__(self):
        self.progressWindow.destroy()

    def __init__(self):
        super(Updater, self).__init__()
        self.update_thread = None
        self.new_version = '<Error>'
        self.beta_version = '<Error>'
        self.progressWindow = XPProgressWindow("Updating Plugin")
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
        if self.update_thread and self.update_thread.is_alive():
            self.update_thread.join()

        self.update_thread = threading.Thread(target=lambda: self.do_download(download_url, cksum))
        self.progressWindow.setCaption("Starting Download...")
        self.progressWindow.setProgress(0)
        self.progressWindow.show()
        self.update_thread.start()

    def do_download(self, download_url, cksum):
        def hook(chunk, maxChunk, total):
            if total > 0:
                p = (chunk * maxChunk) / total
                self.progressWindow.setProgress(p)
                self.progressWindow.setCaption("Downloading [{:2.0%}]".format(p))

        self.progressWindow.setCaption("Downloading")
        success = None
        download_path = os.path.join(self.sys_path, 'Resources/Downloads/', self.Sig)
        install_path = os.path.join(self.sys_path, self.plugin_path)

        # Message to self to stop
        time.sleep(1)

        if not os.path.exists(download_path):
            log("Making download directory: {}".format(download_path))
            os.makedirs(download_path)

        zipfile = download_path + '/._UPDATE.zip'
        urlretrieve(download_url, zipfile, reporthook=hook)
        self.progressWindow.setCaption("Download complete.")
        log("Downloaded file: {}".format(zipfile))
        try:
            if not self.verify(zipfile, cksum):
                self.progressWindow.setCaption("Not upgraded: Field does not match checksum: incomplete download?")
                sys_log("Not upgraded: File does not match checksum: incomplete download?")
                return
        except Exception as e:
            self.progressWindow.setCaption("Failed to verify download checksum.")
            sys_log("Failed to verify download file checksum: {}, json has: {}. {}, ".format(self.Sig, cksum, e))

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
                            sys_log("already exists: {}".format(i.filename))
                            os.replace(os.path.join(install_path, i.filename),
                                       os.path.join(install_path, i.filename + '.bak'))
                            sys_log('{} moved to {}'.format(i.filename, i.filename + '.bak'))
                        zipfp.extract(i, path=install_path)
                    except PermissionError as e:
                        success = False
                        self.progressWindow.setCaption("Extraction failed for {}.".format(i.filename))
                        sys_log(">>>> Failed to extract {}, upgrade failed: {}".format(i.filename, e))
                        break
            else:
                self.progressWindow.setCaption("Test failed.")
                success = False
                sys_log("failed testzip()")
        if success:
            os.remove(zipfile)
            self.progressWindow.setCaption("Upgrade complete. Restart X-Plane to load new version.")
            sys_log("Upgraded: restart X-Plane to load new version")
            # log("this will reload plugins -- commented out")
            # log("This works, but X-Plane then prompts 'Please place new plugin in directory'.. 'Understood'")
            # log("which should not be necessary -- any way to stop it?")
            # plugins.reloadPlugins()

    def verify(self, filename, file_cksum=None):
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
