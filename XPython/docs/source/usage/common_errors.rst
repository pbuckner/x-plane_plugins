Common Installation Errors
--------------------------

On Windows
==========

:code:`<XP>/Resources/plugins/XPPython3/win_x64/XPPython3.xpl: Error Code = 126 : The specified module could not be found.`
     
  **Cause**:
    X-Plane cannot load all DLLs required by plugin. In this case, the python plugin is looking for python itself.
    Usually, python is installed in `C:\\Program Files\\Python3X folder`, where you'll find a file `python3.dll`.
   
  **Solution**:
    1. Python needs to be installed "for all users" -- that places the folder under \Program Files, if not for all
       users, it's stored somewhere else & X-Plane may not be able to find it. And,
    2. Add Python to environment variables.

  Both of these options can be set by the installer downloaded from python.org (this *Advanced Options* window is
  the next window after *Options* in the installation wizard).

     .. image:: /images/pythonwindows.png

On Mac
======

:code:`Failed with urillib: <urlopen error [SSL: CERTIFICATE_VERIFY_FAILED] certificate verify failed: self signed certificate in certificate chain (_ssl.c:1122)>`

    -or-

:code:`!!!! Installation Incomplete: Run /Applications/Python<version>/Install Certificates, and restart X-Plane.`

  **Cause**:
    When installing python3 on Mac, there is an additional step you must do. It is
    documented by the python installation tool, but you (and I) didn't actually read everything in the python installer.

  **Solution**:
   Browse ``/Applications/Python<version>`` and read the ReadMe.rtf file you find there. This is the "Cerificate verification and OpenSSL" issue
   mention in the ReadMe. Just double click on the ``Install Certificates`` command file and that will fix it.

         
On All Platforms
================

:code:`[XPPython3] Cryptography package not installed, XPPython3.xpyce will not be supported. See Documentation.`

  **Cause**:
    Cryptography is an option package. It is required by XPPython3's xpyce module, but that module is
    only required if you are using encrypted python.

  **Solution**:
    You can either ignore this error (if you're not using encrypted python) or you can safely install
    this packages using XPPython3 Pip Package Installer, and install ``cryptography``.
        
:code:`Can't open *<folder>* to scan for plugins.`

  **Cause**
    XPPython3 looks for its main plugin folder PythonPlugins (See :doc:`installation_plugin`), and if not
    found, it will complain. Additionally, XPPython3 will look for aircraft-specific plugins specific
    to the loaded user aircraft (:doc:`/development/plugins`).

  **Solution**:
    If you were expecting python plugin to be loaded, check to make sure the file(s) are place in the
    correct location. If you're not anticipating a plugin to be loaded, ignore the message.
  
