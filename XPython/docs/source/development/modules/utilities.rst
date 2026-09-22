XPLMUtilities
=============
.. py:module:: XPLMUtilities
.. py:currentmodule:: xp

To use::

  import xp

Functions
---------

.. py:function:: speakString(string) -> None

    Display *string* in a translucent overlay over the current
    display and also speak the string if text-to-speech is enabled.

    :param string: Text to display and speak
    :type string: str
    :return: None

    The string is spoken asynchronously, this function returns immediately.

    >>> xp.speakString("Hello world")

    .. image:: /images/speak.png

    For XP12, this is viewable if 'Non-ATC' messages button is selected
    
    .. image:: /images/speakstring-xp12.png

    `Official SDK <https://developer.x-plane.com/sdk/XPLMUtilities/#XPLMSpeakString>`__ :index:`XPLMSpeakString`               
    
.. py:function:: getVirtualKeyDescription(vKey) -> str

    Given a virtual key code *vKey* (as defined in :py:mod:`XPLMDefs`) this routine returns a
    human-readable string describing the character.

    :param vKey: Virtual key code
    :type vKey: int
    :return: Human-readable string describing the key

    This routine is provided for showing users what keyboard mappings they have set up. The string may
    read 'unknown' or be a blank or none string if the virtual key is unknown.

    You'll get virtual keys in a ``key()`` callback associated with windows (see :py:func:`createWindowEx`),
    Key Sniffing (:py:func:`registerKeySniffer`), and Hot Keys (:py:func:`registerHotKey`).

    >>> xp.VK_SEMICOLON
    181
    >>> xp.getVirtualKeyDescription(xp.VK_SEMICOLON)
    ';'

    .. note:: With XP 12.3 some key codes (VK_SEMICOLON included) are no longer translated properly. Fixed in XP 12.4.

    `Official SDK <https://developer.x-plane.com/sdk/XPLMUtilities/#XPLMGetVirtualKeyDescription>`__ :index:`XPLMGetVirtualKeyDescription`

.. py:function::  reloadScenery() -> None

    Reload the current set of scenery.

    You can use this function in two typical ways: simply call it to reload the scenery, picking
    up any new installed scenery, .env files, etc. from disk. Or, change the
    ``sim/flightmodel/position/lat_ref`` and ``sim/flightmodel/position/lon_ref``
    dataRefs and then call this function to shift the scenery environment.

    Note, as loading the scenery can take a significant amount of time, this
    function will block the sim until it is complete.

    >>> xp.reloadScenery()

    `Official SDK <https://developer.x-plane.com/sdk/XPLMUtilities/#XPLMReloadScenery>`__ :index:`XPLMReloadScenery`

.. py:function:: getSystemPath() -> str

    :return: Full path to X-Plane folder (ends with /)

    Note that this is a directory path, so it ends in a trailing  /.

    >>> xp.getSystemPath()
    '/Volumes/Red1/X-Plane 11/'

    `Official SDK <https://developer.x-plane.com/sdk/XPLMUtilities/#XPLMGetSystemPath>`__ :index:`XPLMGetSystemPath`


.. py:function:: getPrefsPath() -> str

    :return: Full path to a file in preferences directory

    This routine returns a full path to **a file** that is within X-Plane's
    preferences directory.

    (You should remove the file name back to the last
    directory separator to get the preferences directory.)

    >>> xp.getPrefsPath()
    '/Volumes/Red1/X-Plane 11/Output/preferences/Set X-Plane.prf'
    >>> os.path.dirname(xp.getPrefsPath())
    '/Volumes/Red1/X-Plane 11/Output/preferences'

    `Official SDK <https://developer.x-plane.com/sdk/XPLMUtilities/#XPLMGetPrefsPath>`__ :index:`XPLMGetPrefsPath`

.. py:function:: getDirectorySeparator() -> str

    :return: Directory separator character

    This routine returns a string the directory separator for the current platform.

    This allows you to write code that concatenates directory paths without having
    to check for the platform.

    But seriously, you're writing in python, so use ``os.path.join()`` instead.

    >>> xp.getDirectorySeparator()
    '/'

    `Official SDK <https://developer.x-plane.com/sdk/XPLMUtilities/#XPLMGetDirectorySeparator>`__ :index:`XPLMGetDirectorySeparator`

.. py:function:: extractFileAndPath(fullPath) -> Tuple[str, str]

    :param fullPath: Full path to a file
    :type fullPath: str
    :return: Tuple of (filename, path)

    Given a full path to a file, this routine separates the path from the file.

    If the path is a partial directory (e.g. ends in : or \) the trailing
    directory separator is removed. This routine works in-place; a pointer to
    the file part of the buffer is returned; the original buffer still starts
    with the path.

    >>> xp.extractFileAndPath('/etc/home/foo')
    ('foo', '/etc/home')
    >>> xp.extractFileAndPath('/etc/home/')
    ('home', '/etc')
    >>> xp.extractFileAndPath('etc/home/')
    ('home', 'etc')

    Don't use this: use ``os.path`` instead.

    `Official SDK <https://developer.x-plane.com/sdk/XPLMUtilities/#XPLMExtractFileAndPath>`__ :index:`XPLMExtractFileAndPath`

.. py:function:: getDirectoryContents(dir, firstReturn=0, bufSize=2048, maxFiles=100) -> Tuple [int, List[str], int]

    Return a list of files and directories in a directory *dir*.

    :param dir: Directory path to list
    :type dir: str
    :param firstReturn: Index of first file to return (default: 0)
    :type firstReturn: int
    :param bufSize: Maximum buffer size in bytes (default: 2048)
    :type bufSize: int
    :param maxFiles: Maximum number of files to return (default: 100)
    :type maxFiles: int
    :return: Tuple of (result, names, totalFiles)

    The output is returned as a three-element tuple (res, names, totalFiles):

    ============ =====================================================
    result       =1 if all information could be returned, 0 otherwise.
    names        a list of file and subdirectory strings
    totalFiles   number of files in the directory
    ============ =====================================================

    >>> xp.getDirectoryContents(xp.getSystemPath())
    (1, ['.DS_Store', 'Aircraft', 'Airfoil Maker.app', 'Airfoils', 'canWriteDataRef', 'Custom Data', 'Custom Scenery',
    'Cycle Dump.txt', 'Data.txt', 'Global Scenery', 'global_turbulence.grib', 'global_winds.grib', 'imgui.ini',
    'Instructions', 'Log.txt', 'METAR.rwx', 'Output', 'Plane Maker.app', 'Resources', 'stations.txt', 'Weapons',
    'X-Plane 11 Installer.app', 'X-Plane.app', 'XPPython3Log.txt'], 24)

    You can set the *maxFiles* returned, and skip over the initial files using *firstReturn*.

    >>> xp.getDirectoryContents(xp.getSystemPath(), firstReturn=20, maxFiles=2)
    (0, ['Weapons', 'X-Plane 11 Installer.app'], 24)

    Also, since the SDK is C language, you can set the maximum buffer size of the set of strings
    to be pre-allocated. If the buffer is too small, fewer names will be returned (and *result* will be
    set to 0.

    \.\.\. and then show pity on poor C programmers who have to deal with such things.
    
    You should use standard python ``os.walk()`` or ``glob.glob()`` instead.

    `Official SDK <https://developer.x-plane.com/sdk/XPLMUtilities/#XPLMGetDirectoryContents>`__ :index:`XPLMGetDirectoryContents`
    

.. py:function:: getVersions() -> Tuple[int, int, int]

    :return: Tuple of (xplaneVersion, xplmVersion, hostID)

    Return the revision of X-Plane, XPLM SDK, and the host ID of the app running us.

    The most common use of this routine is to special-case around X-Plane
    version-specific behavior.

    HostID:
      .. py:data:: Host_Unknown
           :value: 0        
      .. py:data:: Host_XPlane
           :value: 1        
     
    The plug-in system is based on Austin's cross-platform OpenGL framework and
    could theoretically be adapted to run in other apps like WorldMaker. The
    plug-in system also runs against a test harness for internal development
    and could be adapted to another flight sim (in theory at least). So the ``Host_*``
    enum is provided to allow plug-ins to identify what app they are running
    under.

    >>> xp.getVersions()
    (11550, 303, 1)

    `Official SDK <https://developer.x-plane.com/sdk/XPLMUtilities/#XPLMGetVersions>`__ :index:`XPLMGetVersions`

.. py:function::  getLanguage() -> int

 :return: Language code constant

 Returns the language code the sim is running in.

  .. table::
   :align: left
   
   =============================== ===
   .. py:data:: Language_Unknown   =0
   .. py:data:: Language_English   =1
   .. py:data:: Language_French    =2
   .. py:data:: Language_German    =3
   .. py:data:: Language_Italian   =4
   .. py:data:: Language_Spanish   =5
   .. py:data:: Language_Korean    =6
   .. py:data:: Language_Russian   =7
   .. py:data:: Language_Greek     =8
   .. py:data:: Language_Japanese  =9
   .. py:data:: Language_Chinese   =10
   .. py:data:: Language_Ukrainian =11
   =============================== ===


 The Language enum defines what language the sim is running in. This enumeration
 do not imply that the sim can or does run in all of these languages; they
 simply provide a known encoding in the event that a given sim version is
 localized to a certain language.

 >>> xp.getLanguage()
 1

 `Official SDK <https://developer.x-plane.com/sdk/XPLMUtilities/#XPLMGetLanguage>`__ :index:`XPLMGetLanguage`

.. py:function:: debugString(string) -> None

    :param string: String to write to Log.txt
    :type string: str

    This routine outputs a string to the Log.txt file.

    You'll likely want to append a newline to the string. The buffer is
    immediately flushed so you will not lose data. (This does cause a
    performance penalty.)

    >>> xp.debugString(f"Current version is {xp.getVersions()}\n")

    Alternatively, use :py:func:`log` to write to XPPython3Log.txt, or :py:func:`systemLog` to write
    to Log.txt. Both of these routines add a trailing newline. Writes to XPPython3Log.txt are
    buffered (and therefore faster), writes to Log.txt are immediately flushed.

    `Official SDK <https://developer.x-plane.com/sdk/XPLMUtilities/#XPLMDebugString>`__ :index:`XPLMDebugString`

.. py:function:: setErrorCallback(callback) -> None

    :param callback: Callback function to receive error messages
    :type callback: Callable[[str], None]

    Install an error-reporting callback for your plugin.

    Normally the plugin system performs minimum diagnostics to maximize
    performance. When you install an error callback, you will receive calls due
    to certain plugin errors, such as passing bad parameters or incorrect data.

    The error callback reports *programming* errors, e.g., bad API parameters.
    Every error that is returned by the error callback represents a mistake
    in your plugin that you should fix. A few APIs also use it to explain
    why a call that can legitimately fail did fail -- for example,
    :func:`fontAddFace` reports the reason it could not load a font file. But the
    callback is not a general channel for run-time conditions your plugin is expected to handle.     

    .. py:function:: MyCallback(message) -> None
    
        :param str message:

        Your error callback takes a single string and should not return any value.
        NOTE: for the sake of debugging, your error callback will be
        called even if your plugin is not enabled, allowing you to receive debug
        info in your XPluginStart and XPluginStop callbacks. To avoid causing logic
        errors in the management code, do not call any other plugin routines from
        your error callback - it is only meant for logging!

    The intention is for you to install the error callback during debug
    sections and put a break-point inside your callback. This will cause you to
    break into the debugger from within the SDK at the point in your plugin
    where you made an illegal call.

    Installing an error callback may activate error checking code that would
    not normally run, and this may adversely affect performance, so do not
    leave error callbacks installed in shipping plugins.

    Error callbacks can be defined for *each "real" plugin*, but as all XPPython3 python plugins
    appear as a single plugin to X-Plane, there can be *only one error callback* for all
    XPPython3 plugins. Additional calls to :py:func:`setErrorCallback` will replace the previous
    callback with the new callback (for all XPPython3 plugins).
    
    >>> def MyCallback(msg):
    ...   xp.log(msg)
    ...
    >>> xp.setErrorCallback(MyCallback)
    
    .. note:: We *automatically* enable a standard error callback on BETA builds
              of XPPython3, and will enable this same callback on released builds
              if you enable ``debug`` in the preferences file. See :doc:`/development/xppython3.ini`.

              This standard callback prints the error to the python log. You do
              not need to call ``setErrorCallback`` for this feature.

    `Official SDK <https://developer.x-plane.com/sdk/XPLMUtilities/#XPLMSetErrorCallback>`__ :index:`XPLMSetErrorCallback`

.. py:function:: findSymbol(symbol) -> int

    This routine will attempt to find the C-API *symbol*.

    :param symbol: C-API symbol name to find
    :type symbol: str
    :return: Integer pointer to the symbol, or None if not found

    If the symbol is found a pointer the function is returned,
    in the form of a integer constructed from the pointer.

    The idea is that this allows your code to access something within X-Plane which
    is not supported directly by the SDK interface.

    For example, assume the SDK did not support the command XPLMGetVersion(). The
    symbol ``XPLMGetVersion`` still exists within X-Plane, so you could access and use it like this:

    >>> import ctypes
    >>> var = xp.findSymbol('XPLMGetVersions')
    >>> MyGetVersionsPrototype = ctypes.CFUNCTYPE(ctypes.POINTER(ctypes.c_int),
    ...                                           ctypes.POINTER(ctypes.c_int),
    ...                                           ctypes.POINTER(ctypes.c_int))
    ...
    >>> MyGetVersionsTemp = MyGetVersionsPrototype(var)
    >>> xp_version = ctypes.c_int()
    >>> sdk_version = ctypes.c_int()
    >>> hostID = ctypes.c_int()
    >>> MyGetVersionsTemp(ctypes.byref(xp_version), ctypes.byref(sdk_version), ctypes.byref(hostID))
    <ctypes.wintypes.LP_c_int object at 0xfb7fa3ece40>
    >>> print(f"XPlaneVersion: {xp_version.value}, SDK version: {sdk_version.value}, hostID: {hostID.value}")
    XPlaneVersion: 11550, SDK version: 303, hostID: 1

    Note you need to know the C-Language signature of the symbol and understand how to use python ``ctypes``.

    `Official SDK <https://developer.x-plane.com/sdk/XPLMUtilities/#XPLMFindSymbol>`__ :index:`XPLMFindSymbol`

.. py:function:: loadDataFile(fileType, path) -> int

   Loads a data file of a given type.

   :param fileType: Type of data file (DataFile_Situation or DataFile_ReplayMovie)
   :type fileType: int
   :param path: Path to file (absolute or relative to getSystemPath), or None to clear replay
   :type path: Optional[str]
   :return: 1 on success (file found), 0 otherwise

   *path* may be absolute, or relative :py:func:`getSystemPath`
   To clear the replay, pass a None *path* (this is only valid with
   replay movies, not sit files).

   *fileType*:
    .. py:data:: DataFile_Situation
          :value: 1        
    .. py:data:: DataFile_ReplayMovie
          :value: 2        

   DataFile_* enums define types of data files you can load or unload using the SDK.

   Returns 1 on success (file found), 0 otherwise.

   >>> xp.loadDataFile(xp.DataFile_ReplayMovie, 'Output/replays/Cessna Skyhawk Replay.rep')
   1

   `Official SDK <https://developer.x-plane.com/sdk/XPLMUtilities/#XPLMLoadDataFile>`__ :index:`XPLMLoadDataFile`

.. py:function::  saveDataFile(fileType, path) -> int

    Saves the current situation or replay.

    :param fileType: Type of data file (DataFile_Situation or DataFile_ReplayMovie)
    :type fileType: int
    :param path: Path to file (absolute or relative to getSystemPath)
    :type path: str
    :return: 1 on success, 0 otherwise

    *path* is absolute, or relative to :py:func:`getSystemPath`.
    See File Types in :py:func:`loadDataFile`.

    >>> xp.saveDataFile(xp.DataFile_Situation, '/tmp/test.sit')
    1

    `Official SDK <https://developer.x-plane.com/sdk/XPLMUtilities/#XPLMSaveDataFile>`__ :index:`XPLMSaveDataFile`

.. py:function:: returnString(string) -> str

    :param str string: String to hand back to X-Plane
    :return: the same string (from the host-managed slot)

    Copy *string* into the host-managed return slot for the current callback. New
    with X-Plane 12.4.4 (SDK 440).

    This is a low-level helper: it is the only sanctioned way for a callback that
    must return a ``const char *`` to hand a string back to X-Plane. In practice
    you rarely call it yourself. XPPython3 uses it internally to return the
    values of browser-function callbacks (see :func:`windowAddBrowserFunction` and
    :func:`avionicsAddBrowserFunction`), where you simply ``return`` a Python
    string and XPPython3 marshals it for you.

    `Official SDK <https://developer.x-plane.com/sdk/XPLMUtilities/#XPLMReturnString>`__ :index:`XPLMReturnString`

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

.. py:function:: findCommand(name) -> XPLMCommandRef

    :param name: Command name to find
    :type name: str
    :return: :class:`XPLMCommandRef` if found, None otherwise

    Look up a command by *name*, and return its commandRef
    or None if the command does not exist.

    >>> xp.findCommand('xppython3/about')
    <capsule object "XPLMCommandRef" at 0x7fab99874536>

    `Official SDK <https://developer.x-plane.com/sdk/XPLMUtilities/#XPLMFindCommand>`__ :index:`XPLMFindCommand`

.. py:function:: commandBegin(commandRef) -> None

    :param commandRef: Command to begin executing
    :type commandRef: :class:`XPLMCommandRef`

    Start the execution of a command, specified by its *commandRef*.

    The command is "held down" until you call :py:func:`commandEnd`.

    (Some commands act as 'toggles', whereby repeated invocations of :py:func:`commandBegin`
    enable / disable the action. For these types of commands using :py:func:`commandOnce` is preferred.)

    >>> commandRef = xp.findCommand('xppython3/about')
    >>> xp.commandBegin(commandRef)

    `Official SDK <https://developer.x-plane.com/sdk/XPLMUtilities/#XPLMCommandBegin>`__ :index:`XPLMCommandBegin`

.. py:function:: commandEnd(commandRef) -> None

    :param commandRef: Command to end
    :type commandRef: :class:`XPLMCommandRef`

    End the execution of a given command, specified by its *commandRef*
    that was started with :py:func:`commandBegin`.

    >>> commandRef = xp.findCommand('sim/flight_controls/brakes_max')
    >>> xp.commandBegin(commandRef)
    >>> xp.commandEnd(commandRef)

    `Official SDK <https://developer.x-plane.com/sdk/XPLMUtilities/#XPLMCommandEnd>`__ :index:`XPLMCommandEnd`
    
.. py:function:: commandOnce(commandRef) -> None

    :param commandRef: Command to execute
    :type commandRef: :class:`XPLMCommandRef`

    This executes a given *commandRef*, doing both CommandBegin and CommandEnd.

    You can use this as a replacement for the deprecated
    ``XPLMCommandKeyStroke``.

    >>> xp.commandOnce(xp.findCommand('xppython3/about'))

    `Official SDK <https://developer.x-plane.com/sdk/XPLMUtilities/#XPLMCommandOnce>`__ :index:`XPLMCommandOnce`

.. py:function:: createCommand(name, description=None) -> XPLMCommandRef

    :param name: Command name
    :type name: str
    :param description: Human-readable description (default: uses name)
    :type description: Optional[str]
    :return: XPLMCommandRef for the created or existing command

    Create a new command for a given string.

    If the command already exists, the existing commandRef is returned. The description
    may appear in user interface contexts, such as the joystick configuration
    screen. If description is not provided, it will be set to *name*.

    You'll still need to register a command handler (:py:func:`registerCommandHandler`)
    in order to have your new command actually do anything.

    >>> xp.createCommand('my/new/command', 'Do something!')

    `Official SDK <https://developer.x-plane.com/sdk/XPLMUtilities/#XPLMCreateCommand>`__ :index:`XPLMCreateCommand`

.. py:function:: registerCommandHandler(commandRef, callback, before=1, refCon=None) -> None

    :param commandRef: Command to monitor
    :type commandRef: :class:`XPLMCommandRef`
    :param callback: Callback function to execute
    :type callback: Callable[[:class:`XPLMCommandRef`, int, Any], int]
    :param before: 1 to run before X-Plane, 0 to run after (default: 1)
    :type before: int
    :param refCon: Reference constant passed to callback (default: None)
    :type refCon: Any

    Register a *callback* for the given *commandRef*.

    You provide a callback with a reference constant.

    If *before* is 1, your callback will be executed before
    X-Plane executes the command, and your returning 0 from your callback will
    disable X-Plane's processing of the command. If *before* is 0, your
    callback will run after X-Plane. (You can register a single callback both
    before and after a command.)

    Your callback should look like:

    .. py:function:: commandCallback(commandRef, phase, refCon) -> int
    
        Your callback receives the *commandRef* for the
        particular command, the *phase* of the command that is executing, and the
        *refCon* that you provided when registering the callback.
    
        Your command handler should return 1 to let processing of the command
        continue to other plugins and X-Plane, or 0 to halt processing, potentially
        bypassing X-Plane code.
    
        *phase* is:
         .. py:data:: CommandBegin
           :value: 0                  
    
           The command is being started.
    
         .. py:data:: CommandContinue
            :value: 1
    
            The command is continuing to execute.
    
         .. py:data:: CommandEnd
            :value: 2
    
            The command has ended.
    
    >>> def MyHandler(commandRef, phase, refCon):
    ...    print(f"Command got phase: {phase}")
    ...    return 1
    ...
    >>> commandRef = xp.findCommand('sim/flight_controls/brakes_max')
    >>> xp.registerCommandHandler(commandRef, MyHandler)
    >>> xp.commandBegin(commandRef)
    Command got phase: 0
    Command got phase: 1
    >>> xp.commandEnd(commandRef)
    Command got phase: 2

    Unlike some callbacks, you *must* define an actual function: you must not define the callback
    in the call using a lambda expression. This is because
    to unregister the command, you must provide the `exact same callback function`, which is not possible::

      # Works
      >>> a = lambda ref, phase, refcon: print("hello world")
      >>> xp.registerCommandHandler(commandRef, a)
      >>> xp.unregisterCommandHandler(commandRef, a)
      # Does not work
      >>> xp.registerCommandHandler(commandRef, lambda ref, phase, refcon: print("hello world")
      >>> xp.unregisterCommandHandler(commandRef, lambda ref, phase, refcon: print("hello world")

    `Official SDK <https://developer.x-plane.com/sdk/XPLMUtilities/#XPLMRegisterCommandHandler>`__ :index:`XPLMRegisterCommandHandler`


.. py:function:: unregisterCommandHandler(commandRef, callback, before, refCon) -> None

    :param commandRef: Command that was monitored
    :type commandRef: :class:`XPLMCommandRef`
    :param callback: Callback function that was registered
    :type callback: Callable[[:class:`XPLMCommandRef`, int, Any], int]
    :param before: 1 if registered before X-Plane, 0 if after
    :type before: int
    :param refCon: Reference constant used during registration
    :type refCon: Any

    Remove a command callback registered with :py:func:`registerCommandHandler`.

    Parameters much match those used with registration.

    >>> xp.unregisterCommandHandler(commandRef, MyHandler)

    `Official SDK <https://developer.x-plane.com/sdk/XPLMUtilities/#XPLMUnregisterCommandHandler>`__ :index:`XPLMUnregisterCommandHandler`

.. _XPLMCommandRef:

Types
*****

.. py:class:: XPLMCommandRef

    A command ref is an opaque identifier for an X-Plane command. Command
    references stay the same for the life of your plugin but not between
    executions of X-Plane. Command refs are used to execute commands, create
    commands, and create callbacks for particular commands. A command ref
    is returned by :py:func:`findCommand` or created by :py:func:`createCommand`.

