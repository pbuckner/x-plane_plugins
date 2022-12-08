:orphan:
   
Common Installation Errors
--------------------------

On Windows
==========

.. code-block:: none

   <XP>/Resources/plugins/XPPython3/win_x64/XPPython3.xpl: Error Code = 126 :
   The specified module could not be found.
     
**Cause**:
    X-Plane cannot load all DLLs required by plugin. In this case, the XPPython plugin is looking for Python DLL.
    The Python version of XPPython3 (3.10, 3.11, 3.12) looks for the same named DLL
    (python310.dll, python311.dll, ..., pthon312.dll).
   
**Solution**:
    Windows using your %PATH% environment variable to find DLLs, so when you install:

    * Add Python to environment variables.

    If you forgot on the initial install of python,
    you can go to Windows Settings -> Apps -> Python 3.x (64-bit), select *Modify*, then go through
    the options until you find *Add Python to environment variables*.

    You may want to restart your computer after python installation.
    
     .. image:: /images/add_python_to_path.jpg

----

On Mac
======

You get popup on initial execution:

 .. image:: /images/quarantine.png

**Cause**
   The XPPython3 plugin developer (me) cannot be verified, because I've not paid Apple $100 / year to get a signature [#F1]_. So
   Apple decides that since it doesn't know who I am, it cannot check the contents of the plugin, and will therefore
   not allow it to run without additional action on your part:

**Solution**
   1. Press **Cancel**. If you move it to the trash, you'll have to download it again...
      

   2. Follow instructions to remove the quarantine as described in :doc:`mac_quarantine`.

----        

You get an eror message in a Log file complaining about SSL CERTIFICATES:
   
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

   Can't open <folder> to scan for plugins.

**Cause**:
    XPPython3 looks for its main plugin folder PythonPlugins (See :doc:`installation_plugin`), and if not
    found, it will complain. Additionally, XPPython3 will look for aircraft-specific plugins specific
    to the loaded user aircraft (:doc:`/development/plugins`).

**Solution**:
    If you were expecting python plugin to be loaded, check to make sure the file(s) are place in the
    correct location. If you're not anticipating a plugin to be loaded, ignore the message.
  
----

.. [#F1] "*Apple signature costs $100/year*". I'm in favor of the quarantine system, but not
         crazy about having to pay to get a signature as I make no money on XPPython3.
         If you'd like to donate, I promise to
         use the first $100 of XPPython3 donations each year to purchase a signature so
         that others may avoid this hassle. You can donate through
         `Paypal <https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=TA3EJ9VWFCH3N&source=url>`_.
         

