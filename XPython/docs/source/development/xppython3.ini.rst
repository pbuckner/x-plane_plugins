Preferences File
----------------

We support an optional file which can be used to alter some internal behavior
of XPPython3.

You may create the file ``<XP>/Output/preferences/xppython3.ini``. We use the ``Main``
section indicator and support a few flags. Missing values are evaluated as being set to zero where
appropriate. For example::

  [Main]
  debug = 0
  flush_log = 1
  py_verbose = 0
  log_file_name = MyLogFile.txt
  log_file_preserve = 0

Flags are:

* **debug**: when "1", print loading and initialization progress for all XPPython3 plugins,
  and provide more detailed warning messages which may help pin-point plugin coding
  errors. "0" is do not print.

  Recommend keeping this off unless you either don't appear to be able to load your python plugin, or
  your plugin crashes yet the printed exception isn't detailed enough. Otherwise, this options puts
  a lot of extra noise in the XPPython3Log.txt file, which may make it harder to debug your code.

* **flush_log**: when "1", immediately call ``flush`` on all writes to XPPython3Log.txt. Normally,
  calls using ``xp.log`` are buffered, for performance reasons. If you're debugging by following
  the XPPython3Log.txt file in another window, you'll not see output until the buffer is flushed.
  Using the flag will cause your updates to be immediately visible. "0" is do not flush immediately.

  Recommend setting this on during debug. The performance penalty is pretty small, and the immediate
  flush is very useful.

* **py_verbose**: This sets the value for ``PYTHONVERBOSE`` which is equivalent to the ``-v`` python flag.
  Set to "1": print a message each time a module (*any python module*) is initialized, showing the
  place from which it is loaded. Set  to "2": print a message for each file that is checked for
  when searching for a module. Also provides information on module cleanup at exit. "0" is do not print.

  Recommend keeping this off unless you just cannot get your plugin loaded, or it appears to be
  loading the "wrong" file. This generates a *lot* of log entries.

* **log_file_name**: This sets the value for the *python* log file. The default value is ``XPPython3Log.txt``,
  which will be located in the X-Plane root directory, next to ``Log.txt``. If you set this to
  a bad value (illegal file, file with incorrect permissions, etc.), python information will be sent to standard output.
  If you set this value to exactly ``Log.txt``, python information will be added directly to X-Plane's
  ``Log.txt`` file. This has the advantage of adding python output to the built-in X-Plane Dev Console (with
  the disadvantage of interspersing python information with all the other X-Plane logging messages.)
  This parameter is identical to the ``XPPYTHON3_LOG`` environment variable. The environment variable, if
  defined, has precedence.

* **log_file_preserve**: This controls if the python log file is preserved between invocations. Set to "1"
  and the file is preserved. Set to "0", or not set, and the python log file is truncated on startup.
  This parameter is identical to the ``XPPYTHON3_PRESERVE`` environment variable. The environment variable, if
  defined, has precedence. Note that the environment variable is "true" if it is set to *any* value, including 0.
  If you do not which this behavior be sure to unset the environment variable.
