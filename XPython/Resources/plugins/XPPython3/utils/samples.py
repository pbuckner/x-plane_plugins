import os
import xp
from XPPython3.zip_download import ZipDownload


def download():
    # 2) Copy sample python plugins into PythonPlugins/samples directory
    pluginsPath = os.path.normpath(os.path.join(xp.getSystemPath(), xp.PLUGINSPATH))
    samplesPath = os.path.join(pluginsPath, 'samples')
    if not os.path.exists(samplesPath):
        os.makedirs(samplesPath)

    xp.log("Copying sample files from github -> PythonPlugins/samples")
    z = ZipDownload()
    z.install_path = samplesPath
    z.initial_progress_msg = 'Updating XPPython3 Sample plugins -> PythonPlugins/samples'
    z.final_progress_msg = ("XPPython3 Successfully Installed.\n\n"
                            " • You should install 3rd party plugins in: \n"
                            "    <X-Plane>/Resources/plugins/PythonPlugins/\n"
                            " • We've also copied lots of plugin examples into \n"
                            "    <X-Plane>/Resources/plugins/PythonPlugins/samples/\n"
                            "       ◦ Move a file into PythonPlugins to execute, or \n"
                            "       ◦ Look at the code examples and write your own! \n"
                            "\n"
                            "Documentation at xppython3.readthedocs.io")
    z.backup = False
    z.remove_top_dir = True
    z.get_zipfile('https://github.com/pbuckner/xppython3-demos/zipball/main', None)
