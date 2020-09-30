"""
Python Plugin updater, modelled after PI_ScriptUpdater.py by
Joan Perez i Cauhe
"""
import os
import os.path
from zipfile import ZipFile
import hashlib
import json
try:
    from urllib.request import urlopen, urlretrieve  # py3
except ImportError:
    from urllib import urlopen, urlretrieve  # py2

import time
from XPPython3 import scriptconfig
log = scriptconfig.log
system_log = scriptconfig.system_log


class Updater(scriptconfig.Config):
    VersionCheckURL = "http://example.com/foo.json"

    def __init__(self):
        super(Updater, self).__init__()
        self.new_version = None
        if self.config.get('beta', False):
            self.Sig = self.Sig + 'b'
        self.check()
        self.json_info = {}

    def check(self, forceUpgrade=False):
        log("Calling Check with version check url: {}".format(self.VersionCheckURL))
        if self.VersionCheckURL:
            try:
                ret = urlopen(self.VersionCheckURL)
                if ret.getcode() != 200:
                    system_log("Failed to get {}, returned code: {}".format(self.VersionCheckURL, ret.getcode()))
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
                if self.json_info[self.Sig]['version'] != self.Version or forceUpgrade:
                    self.new_version = self.json_info[self.Sig]['version']
                    system_log(">>>>> A new version is available: v.{} -> v.{}.".format(self.Version, self.new_version))
                    if forceUpgrade or (self.json_info[self.Sig]['autoUpgrade'] and self.config and self.config.get('autoUpgrade', False)):
                        system_log(">>>>> Automatically upgrading")
                        self.update(self.json_info[self.Sig]['download'])
                    else:
                        system_log(">>>>> To upgrade: {}".format(self.json_info[self.Sig]['upgrade']))
                else:
                    log("Version is up to date")
            except KeyError as e:
                log("Failed to find key: {}".format(e))

    def update(self, download_url):
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
        if not self.verify(zipfile):
            system_log("Not upgraded: File does not match checksum: incomplete download?")
            return
        with ZipFile(zipfile, 'r') as zipfp:
            if not zipfp.testzip():
                success = True
                for i in zipfp.infolist():
                    try:
                        if os.path.exists(os.path.join(install_path, i.filename)):
                            system_log("already exists: {}".format(i.filename))
                            os.replace(os.path.join(install_path, i.filename),
                                       os.path.join(install_path, i.filename + '.bak'))
                            system_log('{} moved to {}'.format(i.filename, i.filename + '.bak'))
                        zipfp.extract(i, path=install_path)
                    except PermissionError as e:
                        success = False
                        system_log(">>>> Failed to extract {}, upgrade failed: {}".format(i.filename, e))
                        break
            else:
                success = False
                system_log("failed testzip()")
        if success:
            os.remove(zipfile)
            system_log("Upgraded: restart X-Plane to load new version")
            # log("this will reload plugins -- commented out")
            # log("This works, but X-Plane then prompts 'Please place new plugin in directory'.. 'Understood'")
            # log("which should not be necessary -- any way to stop it?")
            # plugins.reloadPlugins()

    def verify(self, filename):
        try:
            cksum = None
            hash_md5 = hashlib.md5()
            with open(filename, 'rb') as f:
                for chunk in iter(lambda: f.read(4086), b''):
                    hash_md5.update(chunk)
                cksum = hash_md5.hexdigest()
            return 'cksum' in self.json_info[self.Sig] and cksum == self.json_info[self.Sig]['cksum']
        except Exception as e:
            system_log("Failed with file checksum: {}, json has: {}. {}, ".format(self.json_info[self.Sig], cksum, e))
