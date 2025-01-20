from typing import Self, Optional
import os
import ssl
import sys
import pickle

from XPPython3 import xp

Name = ''


def log(s: str) -> None:
    elapsedTime = xp.getElapsedTime()
    hours = int(elapsedTime / 3600)
    minutes = int(elapsedTime / 60) % 60
    seconds = elapsedTime % 60
    xp.log(f'{hours:d}:{minutes:02d}:{seconds:06.3f} [{Name:s}] {s:s}')


def system_log(s: str) -> None:
    elapsedTime = xp.getElapsedTime()
    hours = int(elapsedTime / 3600)
    minutes = int(elapsedTime / 60) % 60
    seconds = elapsedTime % 60
    xp.debugString(f'{hours:d}:{minutes:02d}:{seconds:06.3f} [{Name:s}] {s:s}\n')
    log(s)


class Config:
    ConfigFilename = 'example.pkl'
    defaults: dict = {}
    Name = "Plugin Name"
    Sig = "Unique Plugin Signature"
    Desc = "Short description"
    Version = "0.00"
    internal = False  # Set to True, if your plugin is an internal plugin

    def __init__(self: Self, *args, **kwargs) -> None:
        global Name  # pylint: disable=global-statement
        Name = self.Name
        self.new_version: Optional[str] = None

        self.plugin_path = os.path.join(xp.INTERNALPLUGINSPATH, '..') if self.internal else xp.PLUGINSPATH

        system_log(f'+++++ {self.Name} v{self.Version} +++++')
        system_log(f"Python version is {sys.version_info.major}.{sys.version_info.minor}.{sys.version_info.micro} / OpenSSL is {ssl.OPENSSL_VERSION}")

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

    def save(self: Self) -> None:
        try:
            with open(self.filename, 'wb') as fp:
                pickle.dump(self.config, fp)
        except IOError:
            system_log(f"Cannot create initialization file {self.ConfigFilename}, not saving configuration.")
