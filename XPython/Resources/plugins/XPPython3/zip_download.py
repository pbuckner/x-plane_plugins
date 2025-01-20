from typing import Self, Optional
import hashlib
import os
import threading
from zipfile import ZipFile, ZipInfo
from urllib.request import urlopen
from urllib.error import URLError
import certifi
import platform
import stat
try:
    from ssl import SSLCertVerificationError  # py 3.7+
except ImportError:
    from ssl import CertificateError as SSLCertVerificationError  # py < 3.7, py2
from XPPython3 import xp
from XPPython3.XPProgressWindow import XPProgressWindow
from XPPython3.xp_typing import XPLMFlightLoopID

log = xp.systemLog


ZIP_UNIX_SYSTEM = 3


class MyZipFile(ZipFile):
    # preserves file permissions
    def _extract_member(self: Self, member: str | ZipInfo, targetpath: str, pwd: str) -> str:
        if not isinstance(member, ZipInfo):
            member = self.getinfo(member)
        targetpath = super()._extract_member(member, targetpath, pwd)  # type: ignore
        if member.create_system == ZIP_UNIX_SYSTEM and os.path.isfile(targetpath):
            # only restore 'user-execute' permission -- that's safe
            unix_attr = member.external_attr >> 16
            if unix_attr & stat.S_IXUSR:
                os.chmod(targetpath, os.stat(targetpath).st_mode | stat.S_IXUSR)
        return targetpath


class ZipDownload:
    initial_progress_msg = "Updating file"
    final_progress_msg = "Update complete"
    download_path = os.path.join(xp.getSystemPath(), 'Resources', 'Downloads')
    install_path: Optional[str] = None
    backup = True
    progressWindow = None
    remove_top_dir = False

    def __init__(self: Self):
        self.update_thread: Optional[threading.Thread] = None
        self.counter = 0
        self.progressWindow = None
        self.caption = ''
        self.progress = 0.0
        self.flightLoopID: Optional[XPLMFlightLoopID] = None

    def get_zipfile(self: Self, download_url: str, cksum: Optional[str] = None) -> None:
        num_captions = max(2, len(self.initial_progress_msg.split('\n')), len(self.final_progress_msg.split('\n')))
        self.progressWindow = XPProgressWindow(self.initial_progress_msg, num_captions)
        self.flightLoopID = xp.createFlightLoop(self.progressFLCallback)
        xp.scheduleFlightLoop(self.flightLoopID, -1)
        log(f"ZipDownload.get_zipfile called with: {download_url}")
        if self.update_thread and self.update_thread.is_alive():
            self.update_thread.join()

        self.update_thread = threading.Thread(target=lambda: self._download_wrapper(download_url, cksum))
        self.setCaption("Starting Download...")
        self.setProgress(0)
        self.progressWindow.show()
        self.update_thread.start()

    def progressFLCallback(self: Self, *_args, **_kwargs) -> int:
        if self.progressWindow:
            self.progressWindow.setCaption(self.caption)
            self.progressWindow.setProgress(self.progress)
            if xp.isWidgetVisible(self.progressWindow.progressWindow):
                return -1
        return 0

    def setCaption(self: Self, caption: str) -> None:
        self.caption = caption

    def setProgress(self: Self, progress: float = 0) -> None:
        self.progress = progress

    def _download_wrapper(self: Self, download_url: str, cksum: Optional[str] = None) -> None:
        log('Download started')
        try:
            msg = self._download(download_url, cksum)
        except Exception as e:  # pylint: disable=broad-except
            msg = f'Download failed with exception {e}'
        if msg:
            log(msg)
            self.setCaption(msg)

    def _download(self: Self, download_url: str, cksum: Optional[str] = None) -> str:
        # returned 'str' is the last message (success or failure message) which
        # should be displayed to user

        def hook(chunk: int, maxChunk: int, total: int) -> None:
            if total > 0:
                p = (chunk * maxChunk) / total
                self.setProgress(p)
                self.setCaption(f"Downloading [{p:2.0%}]")
            else:
                self.setCaption(f"Downloading [{self.counter}]...")
            self.counter += 1

        if self.install_path is None:
            return "Error: install_path not set"

        self.setCaption("Downloading")

        if not os.path.exists(self.download_path):
            log(f"Making download directory: {self.download_path}")
            os.makedirs(self.download_path)

        zipfile = os.path.normpath(os.path.join(self.download_path, '._UPDATE.zip'))
        if os.path.exists(zipfile):
            try:
                os.remove(zipfile)
            except Exception as e:  # pylint: disable=broad-except
                xp.log(f"Failed to remove previous download {zipfile}: {e}")
                return f"Failed to removed previous download {zipfile}"

        try:
            with urlopen(download_url, cafile=certifi.where()) as fp:
                total = int(fp.headers['Content-Length'] or '-1')
                chunk_count = 0
                with open(zipfile, 'wb') as zp:
                    # [4096 ... 1024*1024], optimized to total // 100
                    chunksize = min(max(4096, total // 100), 1024 * 1024)
                    data = fp.read(chunksize)
                    while data:
                        chunk_count += 1
                        zp.write(data)
                        hook(chunk_count, chunksize, total)
                        data = fp.read(chunksize)
                xp.log(f"Downloaded {chunk_count * chunksize} to {zipfile}")
        except URLError as e:
            try:
                if ((isinstance(e.reason, SSLCertVerificationError)
                     and e.reason.reason == 'CERTIFICATE_VERIFY_FAILED'
                     and platform.system() == 'Darwin')):
                    msg = ("\nError: !!! Python Installation Incomplete:\n"
                           "    Run /Applications/Python<version>/Install Certificates, and restart X-Plane.\n"
                           "    See https://xppython3.readthedocs.io/en/latest/usage/common_errors.html\n")
                    log(msg)
                    xp.log(msg)
                    return "Python Installation incomplete, See XPPython3Log.txt for details"
            except Exception:  # pylint: disable=broad-except
                pass
            xp.log(f"Internet connection error {e}")
            return "Internet connection error, cannot check version. Try again later."
        except Exception as e:  # pylint: disable=broad-except
            xp.log(f'Error while retrieving: {download_url} ({zipfile}): {e}')
            return "Error while attempting to retrieve file. See XPPython3Log.txt."

        self.setCaption("Download complete.")
        log(f"Downloaded file: {zipfile}")
        try:
            if cksum:
                if not self._verify(zipfile, cksum):
                    return "Error: Downloaded file does not match checksum: incomplete download?"
                xp.log("File cksum verified")
        except Exception as e:  # pylint: disable=broad-except
            xp.log(f"Failed to verify download file checksum: {download_url}, json has: {cksum}. {e}")
            return "Failed to verify download. Checksum error"

        with MyZipFile(zipfile, 'r') as zipfp:
            if not cksum:
                # If cksum, we've already checked file integrity, no need to further testzip()
                self.setCaption("Testing the downloaded zip file...")
                try:
                    zipfp.testzip()
                    xp.log("TextZip complete")
                except Exception as e:  # pylint: disable=broad-except
                    xp.log(f"Downloaded file {zipfile} failed integrity check: {e}")
                    return "Downloaded file failed integrity check"

            self.setCaption("Preparing to extract.")
            files = zipfp.infolist()
            numfiles = len(files)
            for idx, i in enumerate(files):
                self.setProgress(idx / numfiles)
                self.setCaption(f"Extracting [{idx + 1}/{numfiles}]\n   {os.path.basename(i.filename)}")

                if self.remove_top_dir:
                    # change ZipInfo value of filename
                    i.filename = '/'.join(i.filename.split('/')[1:])
                    if not i.filename:
                        continue
                    xp.log(f"renamed to {i.filename}")

                # ALWAYS rename existing file to *.bak. Then GENTLY try to remove it if we don't want to keep
                # it around. This allows us to download shared libraries which are currently in use
                # -- we rename them, download a new version & then on restart, a new one is loaded.
                try:
                    path = os.path.normpath(os.path.join(self.install_path, i.filename))
                    if os.path.exists(path):
                        if not os.path.isdir(path):
                            # remove old 'bak' if it exists, ignore if it doesn't
                            try:
                                os.remove(path + '.bak')
                            except FileNotFoundError:
                                pass
                            # in-place rename current -> .bak
                            try:
                                os.replace(path, path + '.bak')
                            except Exception as e:
                                xp.log(f'Failed to move {i.filename} to {i.filename}.bak, {e}')
                                raise

                            if self.backup:
                                xp.log(f'{i.filename} moved to {i.filename}.bak')
                            else:
                                # we didn't want .bak, so attemtpt to remove .bak, ignore if failure
                                try:
                                    os.remove(path + '.bak')
                                except PermissionError:
                                    pass

                except Exception as e:  # pylint: disable=broad-except
                    xp.log(f"Failed dealing with backup file {i.filename}, {e}")
                    return "Extraction failed. See log files."

                try:
                    zipfp.extract(i, path=self.install_path)
                except Exception as e:  # pylint: disable=broad-except
                    xp.log(f"Failed to extract {i.filename}, upgrade failed: {e}")
                    return f">>>> Failed to extract {i.filename}, download failed."
            self.setProgress(1)

        os.remove(zipfile)
        self.setProgress(1)
        log("Download successful")
        return self.final_progress_msg

    def _verify(self: Self, filename: str, file_cksum: Optional[str] = None) -> bool:
        cksum = None
        hash_md5 = hashlib.md5()
        self.setCaption("Verifying...")
        self.setProgress()
        filesize = os.path.getsize(filename)
        fileread = 0
        chunksize = 65536
        with open(filename, 'rb') as f:
            for chunk in iter(lambda: f.read(chunksize), b''):
                fileread += chunksize
                p = fileread / filesize
                self.setProgress(p)
                self.setCaption(f"Verifying [{p:2.0%}]")
                hash_md5.update(chunk)
            cksum = hash_md5.hexdigest()
        self.setCaption("Verification complete.")
        return file_cksum == cksum
