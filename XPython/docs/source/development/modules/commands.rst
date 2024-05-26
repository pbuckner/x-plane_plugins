Easy Commands
=============

.. warning:: This is proposed documentation, for review only.

..
   py:module:: commands
  
X-Plane already defines a larger number of commands. To use or modify an existing
command:

* :py:func:`find_command`: return command object for existing command

* :py:func:`replace_command`: overwrite existing command to execute your code

* :py:func:`wrap_command`: modify existing command by adding code to execute before and/or after existing functionality.

* :py:func:`filter_command`: conditionally execute the command when triggered.

You can also create your own commands and these can be further modified using the previous functions:

* :py:func:`create_command`: create named command
  
Command are implemented using the SDK Command interface, as further described in :doc:`utilities`.

Each of these functions return a ``Command`` object, which allows you to execute the command once or multiple times.

.. py:class:: Command

  Result of :py:func:`find_command`, :py:func:`replace_command`, :py:func:`wrap_command` and :py:func:`create_command`. You
  should not create an instance of this class yourself.

.. py:function:: Command.once()
                 
  Execute command once::
  
    >>> my_cmd = find_command('sim/operation/throttle_up')
    >>> print(my_cmd)
    <Command: 'sim/operation/throttle_up'>
    >>> my_cmd.once()

.. py:function:: Command.start()

  Start execution of this command. Command will continue to execute until stopped with :py:func:`Command.stop`::
  
    >>> my_cmd = find_command('sim/operation/throttle_up')
    >>> my_cmd.start()
    >>> my_cmd.stop()

.. py:function:: Command.stop()

  Stops execution of this command. No error if command is not executing.

Functions
---------

.. py:function:: create_command(name, description, callback)

  Return :py:class:`Command` instance for newly created command with string ``name``.
  The ``description`` will be user-visible in sim, and the provided function ``callback`` will
  be executed when the command is triggered.

  Callback function takes two parameters, ``phase`` and ``duration``. `Phase` is the value 0, 1, or 2
  corresponding to:

  * *CommandBegin* = `0`: the command is being started

  * *CommandContinue* = `1`: the command is continuing to be executed.

  * *CommandEnd* = `2`: the command has ended.

  `Duration` is number of seconds since the start of execution of this command. It is always 0.0
  when the command is started::

    >>> def say_hello(phase, duration):
    ...    if phase == 0:
    ...        xp.speakString(f"Hello, duration is now {int(duration)}")
    ...
    >>> my_cmd = create_command('my/speak', "Says 'Hello duration now...'", say_hello)
    >>> my_cmd.once()

  You can retrieve this command later using :py:func:`find_command` or further modify it using
  the other command functions.

.. py:function:: find_command(name)

  Return :py:class:`Command` instance for command with string ``name``. Raises ``ValueError`` exception
  if command does not exist::

    >>> pause = find_command('sim/operation/pause_toggle')
    >>> print(pause)
    <Command: 'sim/operation/pause_toggle'>
    >>> pause.once()  # sim is paused
    >>> pause.once()  # sim is not paused
    >>> fail_cmd = find_command('sim/operation/double_my_FPS!!!!')
    ValueError: Command not found.

.. py:function:: replace_command(name, callback)

  Call ``callback`` instead of whatever may be defined for existing command.
  Building on the :py:func:`create_command` example above, say we want to change the callback::

    >>> def say_goodbye(phase, duration):
    ...    if phase == 0:
    ...        xp.speakString("Goodbye..")
    ...
    >>> my_replaced_cmd = replace_command('my/speak', say_goodbye)

  ... will now "say goodbye" instead of "hello" on execution. Note that
  the :py:class:`Command` is the same: there is no way to call the "original"
  callback.

    >>> id(my_cmd) == id(my_replaced_cmd)
    True

.. py:function:: wrap_command(name, before, after)

  Similar to :py:func:`replace_command`, this function returns a :py:class:`Command` object
  for existing command with string ``name``. Rather than `replace` the original callback, it `adds` callbacks
  to be executed `before` and `after` the original callback. Either may be ``None``.

  The callbacks have the same signature, taking ``phase`` and ``duration`` and do not
  return a value.

  Because callbacks are computationally expensive, if you don't need a callback, set it
  to ``None``.

.. py:function:: filter_command(name, filter)

  This returns a :py:class:`Command` object for existing command with string ``name``.
  Provided `filter` callback is executed whenever the command is triggered. If the
  filter callback return True, the command is executed. Otherwise the command
  is not executed.

  Callback takes no parameters and must return True/False.

  If the filter is executed on a newly created command and it returns False,
  we'll fabricate a call to the command with an `end` phase.
  If the filter later returns True, we'll fabricate a call with a `begin` phase.
  For existing Laminar commands we cannot fabricate these extra `begin` and `end`
  phases::

    >>> local_hours = find_dataref('sim/cockpit2/clock_timer/local_time_hours')
    >>> def execute_if_daytime(phase, duration):
    ...    return (9 < local_hours < 18)
    ...
    >>> filter_command('my/speak', execute_if_daytime)
