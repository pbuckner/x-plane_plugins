Common Installation Errors
--------------------------

On Windows
==========

.. code-block:: none

   <XP>/Resources/plugins/XPPython3/win_x64/XPPython3.xpl: Error Code = 126 :
   The specified module could not be found.
     
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

.. code-block:: none

   Failed with urllib: <urlopen error [SSL: CERTIFICATE_VERIFY_FAILED] certificate verify
   failed: self signed certificate in certificate chain (_ssl.c:1122)>

-or-

.. code-block:: none

   !!!! Installation Incomplete: Run /Applications/Python<version>/Install Certificates,
   and restart X-Plane.

**Cause**:
  .. image:: /images/install_python_mac.png
     :width: 50%
     :align: right        

  When installing python3 on Mac, there is an additional step you must do. It is
  documented by the python installation tool, but you (and I) didn't actually read everything in the python installer.

  See the highlighted section from the Mac python installer:


**Solution**:
 Browse ``/Applications/Python<version>`` and read the ReadMe.rtf file you find there. This is the "Certificate verification and OpenSSL" issue
 mention in the ReadMe. Just double click on the ``Install Certificates`` command file and that will fix it.

 .. image:: /images/installer_command_execution.png
    :width: 45%
    :align: right

 .. image:: /images/mac_certificate_installer.png
    :width: 50%

         
On All Platforms
================

.. code-block:: none

   [XPPython3] Cryptography package not installed, XPPython3.xpyce will not be supported.
   See Documentation.

**Cause**:
    Cryptography is an option package. It is required by XPPython3's xpyce module, but that module is
    only required if you are using encrypted python.

**Solution**:
    You can either ignore this error (if you're not using encrypted python) or you can safely install
    this packages using XPPython3 Pip Package Installer, and install ``cryptography``.

    See also :ref:`pip-package-installer`.
        
.. code-block:: none

   Can't open <folder> to scan for plugins.

**Cause**:
    XPPython3 looks for its main plugin folder PythonPlugins (See :doc:`installation_plugin`), and if not
    found, it will complain. Additionally, XPPython3 will look for aircraft-specific plugins specific
    to the loaded user aircraft (:doc:`/development/plugins`).

**Solution**:
    If you were expecting python plugin to be loaded, check to make sure the file(s) are place in the
    correct location. If you're not anticipating a plugin to be loaded, ignore the message.
  
