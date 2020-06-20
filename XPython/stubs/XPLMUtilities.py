"""
X-PLANE USER INTERACTION
************************
The user interaction APIs let you simulate commands the user can do with a
joystick, keyboard etc. Note that it is generally safer for future
compatibility to use one of these commands than to manipulate the
underlying sim data.

xplm_key_* enums represent all the keystrokes available within X-Plane. They can
be sent to X-Plane directly. For example, you can reverse thrust using
these enumerations.

xplm_joy_*  are enumerations for all of the things you can do with a joystick
button in X-Plane. They currently match the buttons menu in the equipment
setup dialog, but these enums will be stable even if they change in
X-Plane.

The plug-in system is based on Austin's cross-platform OpenGL framework and
could theoretically be adapted to run in other apps like WorldMaker. The
plug-in system also runs against a test harness for internal development
and could be adapted to another flight sim (in theory at least). So the xplm_Host_*
enum is provided to allow plug-ins to indentify what app they are running
under.

xplm_Language_* enums define what language the sim is running in. These enumerations
do not imply that the sim can or does run in all of these languages; they
simply provide a known encoding in the event that a given sim version is
localized to a certain language.

xplm_DataFile_* enums define types of data files you can load or unload using the
SDK.


X-PLANE COMMAND MANAGEMENT
**************************
The command management APIs let plugins interact with the command-system in
X-Plane, the abstraction behind keyboard presses and joystick buttons. This
API lets you create new commands and modify the behavior (or get
notification) of existing ones.

An X-Plane command consists of three phases: a beginning, continuous
repetition, and an ending. The command may be repeated zero times in the
event that the user presses a button only momentarily.

A command ref is an opaque identifier for an X-Plane command. Command
references stay the same for the life of your plugin but not between
executions of X-Plane. Command refs are used to execute commands, create
commands, and create callbacks for particular commands.

Note that a command is not "owned" by a particular plugin. Since many
plugins may participate in a command's execution, the command does not go
away if the plugin that created it is unloaded.

"""


# XPLMHostApplicationID
xplm_Host_Unknown = 0
xplm_Host_XPlane = 1


# XPLMLanguageCode
xplm_Language_Unknown = 0
xplm_Language_English = 1
xplm_Language_French = 2
xplm_Language_German = 3
xplm_Language_Italian = 4
xplm_Language_Spanish = 5
xplm_Language_Korean = 6
xplm_Language_Russian = 7
xplm_Language_Greek = 8
xplm_Language_Japanese = 9
xplm_Language_Chinese = 10


# XPLMDataFileType
xplm_DataFile_Situation = 1
xplm_DataFile_ReplayMovie = 2


def XPLMError_f(inMessage):
    """
    An XPLM error callback is a function that you provide to receive debugging
    information from the plugin SDK. See XPLMSetErrorCallback for more
    information. NOTE: for the sake of debugging, your error callback will be
    called even if your plugin is not enabled, allowing you to receive debug
    info in your XPluginStart and XPluginStop callbacks. To avoid causing logic
    errors in the management code, do not call any other plugin routines from
    your error callback - it is only meant for logging!
    """


def XPLMSpeakString(inString):
    """
    This function displays the string in a translucent overlay over the current
    display and also speaks the string if text-to-speech is enabled. The string
    is spoken asynchronously, this function returns immediately.
    """


def XPLMCommandKeyStroke(inKey):
    """
    This routine simulates a command-key stroke. However, the keys are done by
    function, not by actual letter, so this function works even if the user has
    remapped their keyboard. Examples of things you might do with this include
    pausingbutton the simulator.
    """
    pass


def XPLMGetVirtualKeyDescription(inVirtualKey):
    """
    Given a virtual key code (as defined in XPLMDefs.h) this routine returns a
    human-readable string describing the character. This routine is provided
    for showing users what keyboard mappings they have set up. The string may
    read 'unknown' or be a blank or None string if the virtual key is unknown.
    """
    return str


def XPLMReloadScenery():
    """
    XPLMReloadScenery reloads the current set of scenery. You can use this
    function in two typical ways: simply call it to reload the scenery, picking
    up any new installed scenery, .env files, etc. from disk. Or, change the
    lat/ref and lon/ref data refs and then call this function to shift the
    scenery environment.
    """


def XPLMGetSystemPath():
    """
    This function returns the full path to the X-System folder. Note that this
    is a directory path, so it ends in a trailing : or /.
    """
    return str


def XPLMGetPrefsPath():
    """
    This routine returns a full path to a file that is within X-Plane's
    preferences directory. (You should remove the file name back to the last
    directory separator to get the preferences directory.
    """
    return str


def XPLMGetDirectorySeparator():
    """
    This routine returns a string with one char and a null terminator that is
    the directory separator for the current platform. This allows you to write
    code that concatinates directory paths without having to #ifdef for

    platform.
    """
    return str


def XPLMExtractFileAndPath(inFullPath):
    """
    Given a full path to a file, this routine separates the path from the file.
    If the path is a partial directory (e.g. ends in : or \) the trailing
    directory separator is removed. This routine works in-place; a pointer to
    the file part of the buffer is returned; the original buffer still starts
    with the path.
    """
    return str, str


def XPLMGetDirectoryContents(inDirectoryPath, inFirstReturn, inFileNameBufSize, inIndexCount):
    """
    This routine returns a list of files in a directory (specified by a full
    path, no trailing : or \). The output is returned as a list of strings.
    terminated strings. An index array (if specified) is filled with pointers
    into the strings. This routine will return 1 if you had
    capacity for all files or 0 if you did not. You can also skip a given
    number of files.

    inDirectoryPath - a string containing the full path to
    the directory with no trailing directory char.

    inFirstReturn - the zero-based index of the first file in the directory to
    return. (Usually zero to fetch all in one pass.)

    inFileNameBufSize - the size of the file name buffer in bytes.

    inIndexCount - the max size of the index in entries.

    Returns a tuple:
      res = 1 if all info could be returned, 0 if there was a buffer overrun.
      namesList = list of files in the directory.
      outTotalFiles =  number of files in the directory.

    WARNING: Before X-Plane 7 this routine did not properly iterate through
    directories. If X-Plane 6 compatibility is needed, use your own code to
    iterate directories.
    """
    return (int, list, int)  # (1=all info returned, list of files, #files in directory)


def XPLMGetVersions():
    """
    This routine returns the revision of both X-Plane and the XPLM DLL. All
    versions are three-digit decimal numbers (e.g. 606 for version 6.06 of
    X-Plane); the current revision of the XPLM is 200 (2.00). This routine also
    returns the host ID of the app running us.

    The most common use of this routine is to special-case around X-Plane
    version-specific behavior.

    """
    return (int, int, int)  # (outXPlaneVersion, outXPLMVersion, outHostID)


def XPLMGetLanguage():
    """
    This routine returns the langauge the sim is running in.
    """
    return int  # XPLMLanguageCode


def XPLMDebugString(inString):
    """
    This routine outputs a string to the Log.txt file. The file is
    immediately flushed so you will not lose data. (This does cause a
    performance penalty.)
    """


def XPLMSetErrorCallback(inCallback):
    """
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
    """


def XPLMFindSymbol(inString):
    """
    This routine will attempt to find the symbol passed in the inString
    parameter. If the symbol is found a pointer the function is returned,
    in the form of a integer constructed from the pointer.
    """
    return int  # pointer to function on success


def XPLMLoadDataFile(inFileType, inFilePath):
    """
    Loads a data file of a given type. Paths must be relative to the X-System
    folder. To clear the replay, pass a None file name (this is only valid with
    replay movies, not sit files).
    """
    pass


def XPLMSaveDataFile(inFileType, inFilePath):
    """
    Saves the current situation or replay; paths are relative to the X-System
    folder.
    """


# XPLMCommandPhase
xplm_CommandBegin = 0  # The command is being started.
xplm_CommandContinue = 1  # The command is continuing to execute.
xplm_CommandEnd = 2  # The command has ended.


def XPLMCommandCallback_f(inCommand, inPhase, inRefcon):
    """
    A command callback is a function in your plugin that is called when a
    command is pressed. Your callback receives the command reference for the
    particular command, the phase of the command that is executing, and a
    reference pointer that you specify when registering the callback.

    Your command handler should return 1 to let processing of the command
    continue to other plugins and X-Plane, or 0 to halt processing, potentially
    bypassing X-Plane code.
    """
    return int  # 1=let processing continue to other plugsin; 0=do not forward to next plugin/x-plane


def XPLMFindCommand(inName):
    """
    XPLMFindCommand looks up a command by name, and returns its command
    reference or None if the command does not exist.
    """
    return int  # XPLMCommandRef


def XPLMCommandBegin(inCommand):
    """
    XPLMCommandBegin starts the execution of a command, specified by its
    command reference. The command is "held down" until XPLMCommandEnd is
    called.
    """


def XPLMCommandEnd(inCommand):
    """
    XPLMCommandEnd ends the execution of a given command that was started with
    XPLMCommandBegin.
    """


def XPLMCommandOnce(inCommand):
    """
    This executes a given command momentarily, that is, the command begins and
    ends immediately.
    """


def XPLMCreateCommand(inName, inDescription):
    """
    XPLMCreateCommand creates a new command for a given string. If the command
    already exists, the existing command reference is returned. The description
    may appear in user interface contexts, such as the joystick configuration
    screen.
    """
    return int  # XPLMCommandRef


def XPLMRegisterCommandHandler(inComand, inHandler, inBefore, inRefcon):
    """
    XPLMRegisterCommandHandler registers a callback to be called when a command
    is executed. You provide a callback with a reference pointer.

    If inBefore is true, your command handler callback will be executed before
    X-Plane executes the command, and returning 0 from your callback will
    disable X-Plane's processing of the command. If inBefore is false, your
    callback will run after X-Plane. (You can register a single callback both
    before and after a command.)
    """


def XPLMUnregisterCommandHandler(inComand, inHandler, inBefore, inRefcon):
    """
    XPLMUnregisterCommandHandler removes a command callback registered with
    XPLMRegisterCommandHandler.
    """

