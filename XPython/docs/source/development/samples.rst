Samples and Demos
-----------------

When you first loaded XPPython3, we automatically downloaded the current set of sample programs
to the ``samples`` directory under ``PythonPlugins``. The full set is located on github
`xppython3-demos/ <https://github.com/pbuckner/xppython3-demos/>`_.

To use any sample, copy or move it from ``PythonPlugins/samples`` to ``PythonPlugins/`` (because
XPPython3 doesn't look for plugins in subdirectories!) You may have to copy one or two other
files from ``samples`` to get the plugin working.

   +----------------------------+--------------------------------------------------------+
   |PI_Bounds.py                | Displays monitor number, size and if you're running VR |
   |                            | using GetWindowGeometry, GetMonitorBounds and related. |
   |                            | See also :doc:`/development/window_position`.          |
   +----------------------------+--------------------------------------------------------+
   |PI_Camera1.py               | Ported from python2 example. Registers a new view and  |
   |                            | orbits the aircraft, using ControlCamera.              |
   +----------------------------+--------------------------------------------------------+
   |PI_CommandSim1.py           | Ported from python2 example. Simple sending of XP      |
   |                            | commmands using menu items. AppendMenuItemWithCommand  |
   |                            | and FindCommand.                                       |
   +----------------------------+--------------------------------------------------------+
   |PI_HelloWidget.py           | Displays simple widget-based window, and logs          |
   |                            | messages. Described more fully in                      |
   |                            | :doc:`/development/hellowidget`. Uses CreateWidget,    |
   |                            | AddWidgetCallback.                                     |
   +----------------------------+--------------------------------------------------------+
   |PI_HelloWorld1.py           | Ported from python2 example. Described more fully in   |
   |                            | :doc:`/development/helloworld`. Uses CreateWindowEx    |
   |                            | and window callbacks.                                  |
   +----------------------------+--------------------------------------------------------+
   |PI_HotKey1.py               | Ported from python2 example. Maps a hotkey to a        |
   |                            | command. Uses RegisterHotKey and SpeakString.          |
   +----------------------------+--------------------------------------------------------+
   |PI_KeySniffer1.py           | Ported from python2 example. Intercepts key strokes    |
   |                            | and displays result. Uses RegisterKeySniffer.          |
   +----------------------------+--------------------------------------------------------+
   |PI_imgui.py                 | Simple popup window with imgui-based widgets.          |
   |                            | See :doc:`/development/imgui`.                         |
   +----------------------------+--------------------------------------------------------+
   |PI_ListBox.py               | Demonstrates the use of a listbox widget (requires     |
   |                            | XPListBox.py.) XPListBox was modelled after            |
   |                            | a version on                                           |
   |                            | `xsquawkbox.net                                        |
   |                            | <https://www.xsquawkbox.net/xpsdk/mediawiki/           |
   |                            | TestWidgets>`_.                                        |
   +----------------------------+--------------------------------------------------------+
   |PI_ManagePlugins1.py        | Ported from python2 example. Demonstrates finding,     |
   |                            | enabling and disabling plugins. Uses CountPlugins,     |
   |                            | GetNthPlugin, GetMyID, DisablePlugin                   |
   +----------------------------+--------------------------------------------------------+
   |PI_Map.py                   | Ported from C version                                  |
   |                            | `on developer.x-plane.com                              |
   |                            | <https://developer.x-plane.com/code-sample-type/       |
   |                            | xplm300-sdk/map/>`_.                                   |
   |                            | Draws on the X-Plane map usingRegisterMapCreationHook, |
   |                            | OpenGL, DrawMapLabel, MapProject, MapUnProject,        |
   |                            | MapScaleMeter.                                         |
   +----------------------------+--------------------------------------------------------+
   |PI_MiniPython.py            | In-game python interpreter. Allows you to execute      |
   |                            | most python within context of running X-Plane. Great   |
   |                            | way to test SDK APIs and see what happens. Requires    |
   |                            | XPListBox.py to be copied also.                        |
   +----------------------------+--------------------------------------------------------+
   |PI_MultiProcess.py          | Demostrates use of python multiprocessing module.      |
   |                            | Uses xp.pythonExecutable.                              |
   +----------------------------+--------------------------------------------------------+
   |PI_Navigation1.py           | Ported from python2 example. Demonstrates use of FMC   |
   |                            | and the navigation databases. Uses FindNavAid,         |
   |                            | GetNavAidInfo, SetFMSEntry, ClearFMSEntry.             |
   +----------------------------+--------------------------------------------------------+
   |PI_OpenGL.py                | Demonstrates use of OpenGL, *and* tests the            |
   |                            | installation for correct set up of OpenGL. **Note**    |
   |                            | Mac Big Sur will not run OpenGL without a minor        |
   |                            | one-time change. This plugin may be useful to          |
   |                            | plugin developers who require clients to install OpenGL|
   |                            | (or imgui).                                            |
   +----------------------------+--------------------------------------------------------+
   |PI_SharedData1.py,          | Ported from python2. Demonstrates sharing data         |
   |PI_SharedData2.py           | between to different plugins. Using FindDataRef,       |
   |                            | ShareData, RegisterDataAccessor.                       |
   +----------------------------+--------------------------------------------------------+
   |PI_SimData1.py              | Ported from python2. Demonstrates how to interact with |
   |                            | X-Plane by reading and writing data. Uses CreateMenu,  |
   |                            | AppendMenuItem, FindDataRef, GetDatai and SetDatai.    |
   +----------------------------+--------------------------------------------------------+
   |PI_TimedProcessing.py       | Ported from python2. Demonstrates how to use timed     |
   |                            | processing callback to continuously record data to     |
   |                            | disk. Uses GetSystemPath, FindDataRef, GetDataf        |
   |                            | RegisterFlightLoopCallback, GetElapsedTime.            |
   +----------------------------+--------------------------------------------------------+



 
