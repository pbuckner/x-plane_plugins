from __future__ import print_function
import ssl
import sys
import pickle

import XPLMUtilities
import XPLMProcessing

Name = ''


def log(s):
    elapsedTime = XPLMProcessing.XPLMGetElapsedTime()
    hours = int(elapsedTime / 3600)
    minutes = int(elapsedTime / 60) % 60
    seconds = elapsedTime % 60
    print('{:d}:{:02d}:{:06.3f} [{:s}] {:s}'.format(hours, minutes, seconds, Name, s))


def system_log(s):
    elapsedTime = XPLMProcessing.XPLMGetElapsedTime()
    hours = int(elapsedTime / 3600)
    minutes = int(elapsedTime / 60) % 60
    seconds = elapsedTime % 60
    XPLMUtilities.XPLMDebugString('{:d}:{:02d}:{:06.3f} [{:s}] {:s}\n'.format(hours, minutes, seconds, Name, s))
    log(s)


RELPATH = 'Resources/plugins/PythonScripts/' if sys.version_info.major == 2 else 'Resources/plugins/PythonPlugins/'


class Config(object):
    ConfigFilename = 'example.pkl'
    defaults = {}
    Name = "Plugin Name"
    Sig = "Unique Plugin Signature"
    Desc = "Short description"
    Version = "0.00"

    def __init__(self, *args, **kwargs):
        global Name
        Name = self.Name
        self.new_version = None

        system_log('+++++ {} v{} +++++'.format(self.Name, self.Version))
        system_log("Python version is {}.{}.{} / OpenSSL is {}".format(sys.version_info.major,
                                                                       sys.version_info.minor,
                                                                       sys.version_info.micro,
                                                                       ssl.OPENSSL_VERSION))

        self.config = dict(self.defaults)

        self.sys_path = ""
        self.sys_path = XPLMUtilities.XPLMGetSystemPath()
        if self.ConfigFilename and self.ConfigFilename != 'example.pkl':
            self.filename = self.sys_path + RELPATH + self.ConfigFilename
            try:
                with open(self.filename, 'rb') as fp:
                    self.config.update(pickle.load(fp))
            except IOError:
                system_log("Cannot load initialization file {}, defaulting to internal values.".format(self.ConfigFilename))
        super(Config, self).__init__(*args, **kwargs)

    def save(self):
        try:
            with open(self.filename, 'wb') as fp:
                pickle.dump(self.config, fp)
        except IOError:
            system_log("Cannot create initialization file {}, not saving configuration.".format(self.ConfigFilename))
