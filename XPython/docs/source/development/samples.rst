Samples and Demos
-----------------

When you first loaded XPPython3, we automatically downloaded the current set of sample programs
to the ``samples`` directory under ``PythonPlugins``. The full set is located on github
`xppython3-demos/ <https://github.com/pbuckner/xppython3-demos/>`_.

You can download a full set of sample plugins by selecting **Download Samples** from the XPPython3
menu. (The files will be copied into ``PythonPlugins/samples``).

To use any sample, copy or move it from ``PythonPlugins/samples`` to ``PythonPlugins/`` (because
XPPython3 doesn't look for plugins in subdirectories!) You may have to copy one or two other
files from ``samples`` to get the plugin working.

   +----------------------------+--------------------------------------------------------+
   |PI_3dLabel.py               | Demonstrates how to use a 2-d drawing callback to draw |
   |                            | to the screen in a way that matches the 3-d            |
   |                            | coordinate system. Add-ons that need to add 3-d labels,|
   |                            | coach marks, or other non-3d graphics that "match" the |
   |                            | real world can use this technique to draw on with Metal|
   |                            | and Vulkan. Ported from C version on                   |
   |                            | developer.x-plane.com                                  |
   |                            | `"Drawing 2-D That Matches the 3-D world" <https://deve|
   |                            | loper.x-plane.com/code-sample/coachmarks/>`_.          |
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
   |PI_Control1.py              | Ported from python2. Demonstrates how to move aircraft |
   |                            | control surfaces. Should be used with the Override1    |
   |                            | plugin. Originally by Sandy Barbour includes with      |
   |                            | PythonInterface.                                       |
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
   |PI_InputOutput1.py          | Ported from python2 example. Demonstrates input /      |
   |                            | output of data from X-Plane, (changing engine throttle |
   |                            | and obtaining resulting N1). Should be used with the   |
   |                            | Override1 plugin                                       |
   |                            | Originally by Sandy Barbour included with              |
   |                            | PythonInterface.                                       |
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
   |PI_Override1.py             | Ported from python2. Demonstrates how to change        |
   |                            | various override datarefs. Originally by Sandy Barbour |
   |                            | included with PythonInterface.                         |
   +----------------------------+--------------------------------------------------------+
   |PI_Position1.py             | Ported from python2. Demonstrates how to change        |
   |                            | aircraft attitude. Originally by Sandy Barbour included|
   |                            | with PythonInterface. Uses LocalToWorld, WorldToLocal, |
   |                            | GetWidgetProperty, SetWidgetPropoerty, SetDataf.       |        
   +----------------------------+--------------------------------------------------------+
   |PI_SharedData1.py,          | Ported from python2. Demonstrates sharing data         |
   |PI_SharedData2.py           | between to different plugins. Using FindDataRef,       |
   |                            | ShareData, RegisterDataAccessor.                       |
   +----------------------------+--------------------------------------------------------+
   |PI_SimData1.py              | Ported from python2. Demonstrates how to interact with |
   |                            | X-Plane by reading and writing data. Uses CreateMenu,  |
   |                            | AppendMenuItem, FindDataRef, GetDatai and SetDatai.    |
   +----------------------------+--------------------------------------------------------+
   |PI_TCASOverride.py          | This plugin creates four traffic targets that will fly |
   |                            | circles around the users' plane. These traffic targets |
   |                            | exist purely as TCAS targets, not as 3D objects, as    |
   |                            | such would usually be placed by XPLMInstance.          |
   |                            | Ported from C version on developer.x-plane.com         |
   |                            | `"Overriding TCAS" <https://developer.x-plane.com/code-|
   |                            | sample/overriding-tcas/>`_ and discussed in            |
   |                            | `"Overriding TCAS and providing traffice information"  |
   |                            | <https://developer.x-plane.com/article/overriding-tcas-|
   |                            | and-providing-traffic-information/>`_                  |
   |                            | Uses GetDatad, GetDataf, GetDatavi,                    |
   |                            | SetDatavf, SetDatai,  SetDatavi, SetDatab              |
   |                            | SetActiveAircraftCount, AcquirePlanes, ReleasePlanes,  |
   |                            | CountAircraft, GetPluginInfo.                          |
   +----------------------------+--------------------------------------------------------+
   |PI_TimedProcessing.py       | Ported from python2. Demonstrates how to use timed     |
   |                            | processing callback to continuously record data to     |
   |                            | disk. Uses GetSystemPath, FindDataRef, GetDataf        |
   |                            | RegisterFlightLoopCallback, GetElapsedTime.            |
   +----------------------------+--------------------------------------------------------+



 
