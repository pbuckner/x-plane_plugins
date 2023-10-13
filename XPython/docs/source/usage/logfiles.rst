Logging
-------

There are two main log files. (Any particular plugin may also create their own log file.)

* **Log.txt**: The standard X-Plane Log file
* **XPPython3Log.txt**: Standard output for python plugins

If you have errors running python plugins,

1. Check **Log.txt**. Make sure python and the python plugin are installed correctly. If not,
   there will be a message in Log.txt, and XPPython3Log.txt will not be created. Verify it's Python3, not Python2
   getting loaded.

2. Check **XPPython3Log.txt**. Most python coding errors / exceptions will be reported in this
   log.

For common installation error messages, see :doc:`common_errors`.

`Log.txt`
=========

Some messages go to Log.txt. Specifically, when python plugin itself is loaded, something like:

.. parsed-literal::

   Loaded: /Volumes/C/X-Plane/Resources/plugins/XPPython3/mac_x64/XPPython3.xpl (xppython3.main).

followed by:

.. parsed-literal::

  [XPPython3] Starting 4.0.0 - for Python 3.11 (compiled: 30z01f0)... Logging to XPPythonLog.txt
  [XPPython3] Python runtime initialized 3.11.1
   
If XPPython3 cannot load, you'll see an error in this log file.

`XPPython3Log.txt`
==================

Python messages go to :code:`<XP>/XPPython3Log.txt` [#F1]_. You can change location of this logfile
by setting environment variable :code:`XPPYTHON3_LOG`. Log is re-written each time [#F2]_. If you want to preserve
the contents of the logfile, set environment variable :code:`XPPYTHON3_PRESERVE`.  Optionally, these
values can be set in the initialization file, see :doc:`/development/xppython3.ini`. (The final location
of the logging file will be indicated in ``Log.txt`` which will indicated the name of the file on python startup.)

If you've changed the log file environment variable, the new log file will be indicated in the
message in :code:`Log.txt` file (above). If the python log file cannot be opened,
or :code:`XPPYTHON3_LOG` is set to an empty value, logging will be to standard out.

* Python Log always contains:

  .. parsed-literal::

     [XPPython3] Version 4.0.0 - for Python 3.11 Started -- Mon Oct 31 13:24:28 2022
     [XPPython3] Python shared library loaded: /Library/Frameworks/Python.framework/Versions/3.11/lib/libpython3.11.dylib

  Then the script folder(s) are scanned. If the folder cannot be found it's listed (not an error really, but just to
  let you know). This includes scanning for aircraft-specific plugins:

  .. parsed-literal::

     Can\'t open *<folder>* to scan for plugins.

  On *each* python plugin startup, we print:

  .. parsed-literal::

     [XPPython3] PI\_\ *<plugin>* initialized.
     [XPPython3]      Name: *<plugin name>*
     [XPPython3]      Sig:  *<plugin signature>*
     [XPPython3]      Desc: *<plugin description>*

  Successful shutdown will end with::

     [XPPython3] Stopped. Mon Oct 31 13:32:23 2022

Internally, we map both `<stdout>` and `<stderr>` for python to this python log file. This allows you
to use the python :code:`print()` function for output. You can redirect output to
a different file as specified above, including to :code:`Log.txt`, which will result in information being visible
in the X-Plane Developer Console (Developer->Toggle Dev Console).
If you do this, be sure to also set :code:`XPPYTHON3_PRESERVE` to a value so that
python merely appends information to :code:`Log.txt`.::

  shell% XPPYTHON3_LOG=Log.txt
  shell% XPPYTHON3_PRESERVE=1
  shell% export XPPYTHON3_LOG XPPYTHON3_PRESERVE

You can also using standard python **logging** module. By default it writes to <stderr>, which goes to the python
log file::

  import logging
  logging.basicConfig(format='%(message)s')
  log = logging.getLogger(__name__)
  log.warning("Hello World")

.. rubric:: Footnotes

.. [#F1] For python2 it was a couple files in the :code:`<XP>/Resources/plugins/PythonScripts` folder.

.. [#F2] Python2 appended to the file rather than clearing it out.
