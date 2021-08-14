import hashlib
import os
import threading
from zipfile import ZipFile
from urllib.request import urlretrieve
from urllib.error import URLError
try:
    from ssl import SSLCertVerificationError  # py 3.7+
except ImportError:
    from ssl import CertificateError as SSLCertVerificationError  # py < 3.7, py2
import XPPython3.xp as xp
from XPPython3.XPProgressWindow import XPProgressWindow

log = xp.sys_log


class ZipDownload:
    initial_progress_msg = "Updating file"
    final_progress_msg = "Update complete"
    download_path = os.path.join(xp.getSystemPath(), 'Resources', 'Downloads')
    install_path = None
    backup = True
    progressWindow = None
    remove_top_dir = False

    # def __del__(self):
    #     if self.progressWindow:
    #         self.progressWindow.destroy()

    def __init__(self):
        self.update_thread = None
        self.counter = 0

    def get_zipfile(self, download_url, cksum=None):
        num_captions = max(2, len(self.initial_progress_msg.split('\n')), len(self.final_progress_msg.split('\n')))
        self.progressWindow = XPProgressWindow(self.initial_progress_msg, num_captions)
        log("ZipDownload.get_zipfile called with: {}".format(download_url))
        if self.update_thread and self.update_thread.is_alive():
            self.update_thread.join()

        self.update_thread = threading.Thread(target=lambda: self._download(download_url, cksum))
        self.progressWindow.setCaption("Starting Download...")
        self.progressWindow.setProgress(0)
        self.progressWindow.show()
        self.update_thread.start()

    def _download(self, download_url, cksum=None):
        log('_download started')

        def hook(chunk, maxChunk, total):
            if total > 0:
                p = (chunk * maxChunk) / total
                self.progressWindow.setProgress(p)
                self.progressWindow.setCaption("Downloading [{:2.0%}]".format(p))
            else:
                self.progressWindow.setCaption("Downloading [{}]...".format(self.counter))
            self.counter += 1

        if self.install_path is None:
            log("Error: install_path not set")
            return

        self.progressWindow.setCaption("Downloading")
        success = None

        if not os.path.exists(self.download_path):
            log("Making download directory: {}".format(self.download_path))
            os.makedirs(self.download_path)

        zipfile = os.path.join(self.download_path, '._UPDATE.zip')
        try:
            urlretrieve(download_url, zipfile, reporthook=hook)
        except URLError as e:
            try:
                if ((isinstance(e.reason, SSLCertVerificationError)
                     and e.reason.reason == 'CERTIFICATE_VERIFY_FAILED'
                     and platform.system() == 'Darwin')):
                    msg = ("\nError: !!! Python Installation Incomplete:\n"
                           "    Run /Applications/Python<version>/Install Certificates, and restart X-Plane.\n"
                           "    See https://xppython3.readthedocs.io/en/latest/usage/common_errors.html\n")
                    xp.sys_log(msg)
                    xp.log(msg)
                    self.progressWindow.setCaption("Python Installation incomplete, See XPPython3Log.txt for details")
                    return
            except Exception:
                pass
            xp.log("Internet connection error, cannot check version. Will check next time.")
            
        except Exception as e:
            log('Error while retrieving: {} ({}): {}'.format(download_url, zipfile, e))
            return
        self.progressWindow.setCaption("Download complete.")
        log("Downloaded file: {}".format(zipfile))
        try:
            if cksum is not None and not self._verify(zipfile, cksum):
                self.progressWindow.setCaption("Not upgraded: Field does not match checksum: incomplete download?")
                xp.sys_log("Not upgraded: File does not match checksum: incomplete download?")
                return
        except Exception as e:
            self.progressWindow.setCaption("Failed to verify download checksum.")
            xp.sys_log("Failed to verify download file checksum: {}, json has: {}. {}, ".format(download_url, cksum, e))

        with ZipFile(zipfile, 'r') as zipfp:
            self.progressWindow.setCaption("Testing the downloaded zip file...")
            if not zipfp.testzip():
                self.progressWindow.setCaption("Testing complete. Preparing to extract.")
                success = True
                files = zipfp.infolist()
                numfiles = len(files)
                for idx, i in enumerate(files):
                    self.progressWindow.setProgress(idx / numfiles)
                    self.progressWindow.setCaption("Extracting [{}/{}] {}".format(idx + 1, numfiles, os.path.basename(i.filename)))
                    try:
                        if self.remove_top_dir:
                            # change ZipInfo value of filename
                            i.filename = '/'.join(i.filename.split('/')[1:])
                            if not i.filename:
                                continue
                            xp.sys_log("renamed to {}".format(i.filename))
                        if os.path.exists(os.path.join(self.install_path, i.filename)):
                            if self.backup and not os.path.isdir(os.path.join(self.install_path, i.filename)):
                                os.replace(os.path.join(self.install_path, i.filename),
                                           os.path.join(self.install_path, i.filename + '.bak'))
                                xp.sys_log('{} moved to {}'.format(i.filename, i.filename + '.bak'))
                        zipfp.extract(i, path=self.install_path)
                    except PermissionError as e:
                        success = False
                        self.progressWindow.setCaption("Extraction failed for {}.".format(i.filename))
                        xp.sys_log(">>>> Failed to extract {}, upgrade failed: {}".format(i.filename, e))
                        break
                self.progressWindow.setProgress(1)
            else:
                self.progressWindow.setCaption("Test failed.")
                success = False
                xp.sys_log("failed testzip()")
        if success:
            os.remove(zipfile)
            self.progressWindow.setProgress(1)
            self.progressWindow.setCaption(self.final_progress_msg)
            xp.sys_log("Download successful")

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
