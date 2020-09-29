XPLMUtilities
=============
.. py:module:: XPLMUtilities

To use::

  import XPLMUtilities

Functions
---------

.. py:function:: XPLMError_f(message: str) -> None:

    An XPLM error callback is a function that you provide to receive debugging
    information from the plugin SDK. See :py:func:`XPLMSetErrorCallback` for more
    information. NOTE: for the sake of debugging, your error callback will be
    called even if your plugin is not enabled, allowing you to receive debug
    info in your XPluginStart and XPluginStop callbacks. To avoid causing logic
    errors in the management code, do not call any other plugin routines from
    your error callback - it is only meant for logging!



.. py:function:: XPLMSpeakString(string: str) -> None:

    This function displays the string in a translucent overlay over the current
    display and also speaks the string if text-to-speech is enabled. The string
    is spoken asynchronously, this function returns immediately.



.. py:function:: XPLMGetVirtualKeyDescription(virtualKey) -> str:

    Given a virtual key code (as defined in :py:mod:`XPLMDefs`) this routine returns a
    human-readable string describing the character. This routine is provided
    for showing users what keyboard mappings they have set up. The string may
    read 'unknown' or be a blank or None string if the virtual key is unknown.

.. py:function::  XPLMReloadScenery(None) -> None:

    XPLMReloadScenery reloads the current set of scenery. You can use this
    function in two typical ways: simply call it to reload the scenery, picking
    up any new installed scenery, .env files, etc. from disk. Or, change the
    lat/ref and lon/ref data refs and then call this function to shift the
    scenery environment.



.. py:function:: XPLMGetSystemPath(None) -> str:

    This function returns the full path to the X-System folder. Note that this
    is a directory path, so it ends in a trailing  /.


.. py:function:: XPLMGetPrefsPath(None) -> str:

    This routine returns a full path to **a file** that is within X-Plane's
    preferences directory. (You should remove the file name back to the last
    directory separator to get the preferences directory.


.. py:function:: XPLMGetDirectorySeparator(None) -> str:

    This routine returns a string the directory separator for the current platform.
    This allows you to write code that concatinates directory paths without having
    to #ifdef for platform.

    But seriously, you're writing in python, so use ``os.path.join()`` instead.


.. py:function:: XPLMExtractFileAndPath(inFullPath) -> file, path:

    Given a full path to a file, this routine separates the path from the file.
    If the path is a partial directory (e.g. ends in : or \) the trailing
    directory separator is removed. This routine works in-place; a pointer to
    the file part of the buffer is returned; the original buffer still starts
    with the path::

      >>> XPLMExtractFileAndPath('/etc/home/foo')
      ('foo', '/etc/home')
      >>> XPLMExtractFileAndPath('/etc/home/')
      ('home', '/etc')
      >>> XPLMExtractFileAndPath('etc/home/')
      ('home', 'etc')

    Don't use this: use ``os.path`` instead

.. py:function:: XPLMGetDirectoryContents(dir, firstReturn, bufSize, indexCount) -> (res, names, totalFiles):

    This routine returns a list of files in a directory (specified by a full
    path, no trailing /). The output is returned as a list of strings.
    An index array (if specified) is filled with pointers
    into the strings. This routine will return 1 if you had
    capacity for all files or 0 if you did not. You can also skip a given
    number of files.

    :param str dir: a string containing the full path to the directory with no trailing directory char
    :param int firstReturn:  the zero-based index of the first file in the directory to return. (Usually zero to fetch all in one pass.)
    :param int bufSize: the size of the file name buffer in bytes.
    :param int indexCount: the max size of the index in entries.
    :return:

       * res: int =1 if all info could be returned, 0 otherwise
       * names: list of files in the directory
       * totalFiles: int number of files in the directory

    You should use standard python ``os.walk()`` or ``glob.glob()`` instead.

.. py:function:: XPLMGetVersions(None) -> (x-plane, xplm, hostID):

    This routine returns the revision of both X-Plane and the XPLM SDK. All
    versions are three-digit decimal numbers (e.g. 606 for version 6.06 of
    X-Plane); the current revision of the XPLM is 303. This routine also
    returns the host ID of the app running us.

    The most common use of this routine is to special-case around X-Plane
    version-specific behavior.

    HostID:
      .. py:data:: xplm_Host_Unknown
           :value: 0        
      .. py:data:: xplm_Host_XPlane
           :value: 1        
     
    The plug-in system is based on Austin's cross-platform OpenGL framework and
    could theoretically be adapted to run in other apps like WorldMaker. The
    plug-in system also runs against a test harness for internal development
    and could be adapted to another flight sim (in theory at least). So the xplm_Host_*
    enum is provided to allow plug-ins to indentify what app they are running
    under.
     

.. py:function::  XPLMGetLanguage(None) -> languageCode:

 This routine returns the langauge the sim is running in:

  .. table::
   :align: left
   
   ==================================== ===
   .. py:data:: xplm_Language_Unknown   =0
   .. py:data:: xplm_Language_English   =1
   .. py:data:: xplm_Language_French    =2
   .. py:data:: xplm_Language_German    =3
   .. py:data:: xplm_Language_Italian   =4
   .. py:data:: xplm_Language_Spanish   =5
   .. py:data:: xplm_Language_Korean    =6
   .. py:data:: xplm_Language_Russian   =7
   .. py:data:: xplm_Language_Greek     =8
   .. py:data:: xplm_Language_Japanese  =9
   .. py:data:: xplm_Language_Chinese   =10
   ==================================== ===


 xplm_Language_* enums define what language the sim is running in. These enumerations
 do not imply that the sim can or does run in all of these languages; they
 simply provide a known encoding in the event that a given sim version is
 localized to a certain language.


.. py:function:: XPLMDebugString(string: str) -> None:

    This routine outputs a string to the Log.txt file. The file is
    immediately flushed so you will not lose data. (This does cause a
    performance penalty.)


.. py:function:: XPLMSetErrorCallback(callback) -> None:

    :param callback: :py:func:`XPLMError_f` callback                 

    XPLMSetErrorCallback installs an error-reporting callback for your plugin.
    Normally the plugin system performs minimum diagnostics to maximize
    performance. When you install an error callback, you will receive calls due
    to certain plugin errors, such as passing bad parameters or incorrect data.

    The intention is for you to install the error callback during debug
    sections and put a break-point inside your callback. This will cause you to
    break into the debugger from within the SDK at the point in your plugin
    where you made an illegal call.

    Installing an error callback may activate error checking code that would
    not normally run, and this may adversely affect performance, so do not
    leave error callbacks installed in shipping plugins.


.. py:function:: XPLMFindSymbol(symbol: str) -> int:

    This routine will attempt to find the C-APIl symbol passed in the symbol
    parameter. If the symbol is found a pointer the function is returned,
    in the form of a integer constructed from the pointer.

    The idea is that this allows your code to access something within X-Plane which
    is not supported directly by the SDK interface.

    For example, assume the SDK did not support the command XPLMGetVersion(). The
    symbol ``XPLMGetVersion`` still exists within X-Plane, so you could access and use it like this::

      var = XPLMFindSymbol('XPLMGetVersions')
      XPLMGetVersionsPrototype = ctypes.CFUNCTYPE(ctypes.POINTER(ctypes.c_int),
                                                  ctypes.POINTER(ctypes.c_int),
                                                  ctypes.POINTER(ctypes.c_int))
      XPLMGetVersionsTemp = XPLMGetVersionsPrototype(var)
      xp_version = ctypes.c_int()
      sdk_version = ctypes.c_int()
      hostID = ctypes.c_int()
      XPLMGetVersionsTemp(ctypes.byref(xp_version), ctypes.byref(sdk_version), ctypes.byref(hostID))
      print("XPlaneVersion: {}, SDK version: {}, hostID: {}".format(xp_version, version, hostID))

    Note you need to know the C-Language signature of the symbol and understand how to use python ``ctypes``.


.. py:function:: XPLMLoadDataFile(fileType, path) -> None:

   :param fileType: int
   :param str path: Relative to the X-System folder

   File Types:
    .. py:data:: xplm_DataFile_Situation
          :value: 1        
    .. py:data:: xplm_DataFile_ReplayMovie
          :value: 2        

    xplm_DataFile_* enums define types of data files you can load or unload using the SDK.

   Loads a data file of a given type. Paths must be relative to the X-System
   folder. To clear the replay, pass a None file name (this is only valid with
   replay movies, not sit files).


.. py:function::  XPLMSaveDataFile(fileType, path) -> None:

    Saves the current situation or replay; paths are relative to the X-System
    folder. See File Types in :py:func:`XPLMLoadDataFile`.

X-Plane Command Management
--------------------------

The command management APIs let plugins interact with the command-system in
X-Plane, the abstraction behind keyboard presses and joystick buttons. This
API lets you create new commands and modify the behavior (or get
notification) of existing ones.

An X-Plane command consists of three phases: a beginning, continuous
repetition, and an ending. The command may be repeated zero times in the
event that the user presses a button only momentarily.

Note that a command is not "owned" by a particular plugin. Since many
plugins may participate in a command's execution, the command does not go
away if the plugin that created it is unloaded.

.. _XPLMCommandRef:

XPLMCommandRef
**************

A command ref is an opaque identifier for an X-Plane command. Command
references stay the same for the life of your plugin but not between
executions of X-Plane. Command refs are used to execute commands, create
commands, and create callbacks for particular commands. A command ref
is returned by :py:func:`XPLMFindCommand` or created by :py:func:`XPLMCreateCommand`.

.. py:function:: XPLMCommandCallback_f(commandRef, phase, refCon) -> int:

    :param commandRef: :ref:`XPLMCommandRef`
    :param phase: integer indicating which phase of the command is executing (see below)
    :param refCon: Reference constant you provided upon registering your callback with :py:func:`XPLMRegisterCommandHandler`

    A command callback is your function in your plugin that is called when a
    command is pressed. Your callback receives the command reference for the
    particular command, the phase of the command that is executing, and a
    reference pointer that you specify when registering the callback.

    Your command handler should return 1 to let processing of the command
    continue to other plugins and X-Plane, or 0 to halt processing, potentially
    bypassing X-Plane code.

    phase is:
     .. py:data:: xplm_CommandBegin
       :value: 0                  

       The command is being started.

     .. py:data:: xplm_CommandContinue
        :value: 1

        The command is continuing to execute.

     .. py:data:: xplm_CommandEnd
        :value: 2

        The command has ended.


.. py:function:: XPLMFindCommand(name: str) -> commandRef:

    :return: :ref:`XPLMCommandRef`

    XPLMFindCommand looks up a command by name, and returns its command
    reference or None if the command does not exist.


.. py:function:: XPLMCommandBegin(commandRef) -> None:

    :param commandRef: :ref:`XPLMCommandRef`

    XPLMCommandBegin starts the execution of a command, specified by its
    command reference.
    The command is "held down" until you call :py:func:`XPLMCommandEnd`.


.. py:function:: XPLMCommandEnd(commandRef) -> None:

    :param commandRef: :ref:`XPLMCommandRef`

    XPLMCommandEnd ends the execution of a given command that was started with
    :py:func:`XPLMCommandBegin`.


.. py:function:: XPLMCommandOnce(commandRef) -> None:

    :param commandRef: :ref:`XPLMCommandRef`

    This executes a given commandRef, that is, the command begins and
    ends immediately. You can use this as a replacement for the deprecated
    ``XPLMCommandKeyStroke``::

      xp.commandOnce(xp.findCommand('sim/operation/toggle_pause'))


.. py:function:: XPLMCreateCommand(inName:str, inDescription:str) -> commandRef:

    :return: :ref:`XPLMCommandRef`

    XPLMCreateCommand creates a new command for a given string. If the command
    already exists, the existing command reference is returned. The description
    may appear in user interface contexts, such as the joystick configuration
    screen.

.. py:function:: XPLMRegisterCommandHandler(commandRef, callback, before, refCon) -> int:

    :param commandRef: :ref:`XPLMCommandRef`
    :param callback: :py:func:`XPLMRegisterCommandHandler`
    :param int before: 1= your command handler callback will be executed before X-Plane executes the command.                 
    :param object refCon: Reference constant to be passed to you callback.

    XPLMRegisterCommandHandler registers a callback to be called when a command
    is executed. You provide a callback with a reference pointer.

    If ``before`` is true, your command handler callback will be executed before
    X-Plane executes the command, and returning 0 from your callback will
    disable X-Plane's processing of the command. If ``before`` is false, your
    callback will run after X-Plane. (You can register a single callback both
    before and after a command.)


.. py:function:: XPLMUnregisterCommandHandler(commandRef, callback, before, refCon):

    XPLMUnregisterCommandHandler removes a command callback registered with
    XPLMRegisterCommandHandler. Parameters much match those used with :py:func:`XPLMRegisterCommandHandler`
