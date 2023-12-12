import os
import ssl
import sys
import pickle

from XPPython3 import xp

Name = ''


def log(s):
    elapsedTime = xp.getElapsedTime()
    hours = int(elapsedTime / 3600)
    minutes = int(elapsedTime / 60) % 60
    seconds = elapsedTime % 60
    xp.log('{:d}:{:02d}:{:06.3f} [{:s}] {:s}'.format(hours, minutes, seconds, Name, s))


def system_log(s):
    elapsedTime = xp.getElapsedTime()
    hours = int(elapsedTime / 3600)
    minutes = int(elapsedTime / 60) % 60
    seconds = elapsedTime % 60
    xp.debugString('{:d}:{:02d}:{:06.3f} [{:s}] {:s}\n'.format(hours, minutes, seconds, Name, s))
    log(s)


class Config(object):
    ConfigFilename = 'example.pkl'
    defaults = {}
    Name = "Plugin Name"
    Sig = "Unique Plugin Signature"
    Desc = "Short description"
    Version = "0.00"
    internal = False  # Set to True, if your plugin is an internal plugin

    def __init__(self, *args, **kwargs):
        global Name
        Name = self.Name
        self.new_version = None

        self.plugin_path = os.path.join(xp.INTERNALPLUGINSPATH, '..') if self.internal else xp.PLUGINSPATH

        system_log('+++++ {} v{} +++++'.format(self.Name, self.Version))
        system_log("Python version is {}.{}.{} / OpenSSL is {}".format(sys.version_info.major,
                                                                       sys.version_info.minor,
                                                                       sys.version_info.micro,
                                                                       ssl.OPENSSL_VERSION))

        self.config = dict(self.defaults)

        if self.ConfigFilename and self.ConfigFilename != 'example.pkl':
            self.filename = os.path.join(xp.INTERNALPLUGINSPATH if self.internal else xp.PLUGINSPATH, self.ConfigFilename)
            try:
                with open(self.filename, 'rb') as fp:
                    self.config.update(pickle.load(fp))
            except IOError:
                system_log(f"Cannot load initialization file {self.ConfigFilename}, defaulting to internal values.")
            except EOFError:
                system_log(f"Bad initialization file {self.ConfigFilename}, defaulting to internal values.")
        super(Config, self).__init__(*args, **kwargs)

    def save(self):
        try:
            with open(self.filename, 'wb') as fp:
                pickle.dump(self.config, fp)
        except IOError:
            system_log("Cannot create initialization file {}, not saving configuration.".format(self.ConfigFilename))
