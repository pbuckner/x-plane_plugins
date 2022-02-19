Preferences File
----------------

We support an optional file which can be used to alter some internal behavior
of XPPython3.

You may create the file ``<XP>/Output/preferences/xppython3.ini``. We use the ``Main``
section indicator and support a few flags. Missing values are set to zero. For example::

  [Main]
  debug = 0
  flush_log = 1
  py_verbose = 0

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

