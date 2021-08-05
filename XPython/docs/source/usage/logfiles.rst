Logging
=======

There are two main log files. (Any particular plugin may also create their own log file.)

* **Log.txt**: The standard X-Plane Log file
* **XPPython3Log.txt**: Standard output for python plugins

For common installation error messages, see :doc:`common_errors`.

`Log.txt`
*********

* Some messages go to Log.txt. Specifically, when python plugin itself is loaded:

  :code:`Loaded: <XP>/Resources/plugins/XPPython3/mac_x64/xppython3.xpl (XPPython3.0.0).`
   
  If XPPython3 cannot load, you'll see an error in this log file.

`XPPython3Log.txt`
******************

Python messages go to :code:`<XP>/XPPython3Log.txt` [#F1]_. You can change location of this logfile
by setting environment variable :code:`XPPYTHON3_LOG`. Log is re-written each time [#F2]_. If you want to preserve
the contents of the logfile, set environment variable :code:`XPPYTHON3_PRESERVE`.

* Log always contains:

  .. parsed-literal::

     XPPython3 Version *<x.x.x>* Started.

  Then the script folder(s) are scanned. If the folder cannot be found (not an error really, but just to
  let you know). This includes scanning for aircraft-specific plugins:

  .. parsed-literal::

     Can\'t open *<folder>* to scan for plugins.

  On *each* python plugin startup, we print:

  .. parsed-literal::

     PI\_\ *<plugin>* initialized.
          Name: *<plugin name>*
          Sig:  *<plugin signature>*
          Desc: *<plugin description>*

  Successful shutdown will included::

    XPPython Stopped.

.. rubric:: Footnotes

.. [#F1] For python2 it was a couple files in the :code:`<XP>/Resources/plugins/PythonScripts` folder.

.. [#F2] Python2 appended to the file rather than clearing it out.
