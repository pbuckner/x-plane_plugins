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
   |PI_Arduino1.py              | Finds and connects with Arduino device and polls device|
   |                            | for information using a separate python Thread in order|
   |                            | to not slow down X-Plane.                              |
   |                            |                                                        |
   |                            | Includes example Arduino program (which you'll need to |
   |                            | download to your device).                              |
   +----------------------------+--------------------------------------------------------+
   |PI_Bounds.py                | Displays monitor number, size and if you're running VR |
   |                            | using getWindowGeometry, getMonitorBounds and related. |
   |                            | See also :doc:`/development/window_position`.          |
   +----------------------------+--------------------------------------------------------+
   |PI_Camera1.py               | Ported from python2 example. Registers a new view and  |
   |                            | orbits the aircraft, using controlCamera.              |
   +----------------------------+--------------------------------------------------------+
   |PI_CommandSim1.py           | Ported from python2 example. Simple sending of XP      |
   |                            | commands using menu items. appendMenuItemWithCommand   |
   |                            | and findCommand.                                       |
   +----------------------------+--------------------------------------------------------+
   |PI_Control1.py              | Ported from python2. Demonstrates how to move aircraft |
   |                            | control surfaces. Should be used with the Override1    |
   |                            | plugin. Original by Sandy Barbour include with         |
   |                            | PythonInterface.                                       |
   +----------------------------+--------------------------------------------------------+
   |PI_ExampleGauge.py          | Ported from Sandy Barbour's Example Gauge example.     |
   |                            |                                                        |
   |                            | Draws a floating panel, with gauge tied to ENGN\_N1\_  |
   |                            | dataref. Press F8 to toggle display, drag panel by     |
   |                            | selecting top ~20 pixels of panel.                     |
   |                            |                                                        |
   |                            | Copy ExampleGauge/\*.bmp bitmaps from sample directory |
   |                            | into Resource/plugins/PythonPlugins/ExampleGauge/ so   |
   |                            | this plugin can find them.                             |
   |                            |                                                        |
   |                            | Uses OpenGL, textures, registerHotKey.                 |
   +----------------------------+--------------------------------------------------------+
   |PI_FMOD_Advanced.py         | Modeled after Bill Good's xp12-fmod-sdk-demo.          |
   |                            |                                                        |
   |                            | Plays a sequence of sound files over-and-over.         |
   |                            |                                                        |
   |                            | Use python ctypes module, in combination with          |
   |                            | getFMODStudio to directly access additional FMOD       |
   |                            | routines in the FMOD shared library.                   |
   +----------------------------+--------------------------------------------------------+
   |PI_FMSUtility1.py           | Ported from python2 example. Demonstrates access to    |
   |                            | the FMS. getDisplayFMSEntry, setDisplayFMSEntry,       |
   |                            | getDestinationFMSEntry, setDestinationFMSEntry,        |
   |                            | getFMSEntryInfo, setFMSEntryInfo, findNavAid,          |
   |                            | setFMSEntryLatLon.                                     |
   +----------------------------+--------------------------------------------------------+
   |PI_HelloWidget.py           | Displays simple widget-based window, and logs          |
   |                            | messages. Described more fully in                      |
   |                            | :doc:`/development/hellowidget`. Uses createWidget,    |
   |                            | addWidgetCallback.                                     |
   +----------------------------+--------------------------------------------------------+
   |PI_HelloWorld1.py           | Ported from python2 example. Described more fully in   |
   |                            | :doc:`/development/helloworld`. Uses createWindowEx    |
   |                            | and window callbacks.                                  |
   +----------------------------+--------------------------------------------------------+
   |PI_HotKey1.py               | Ported from python2 example. Maps a hotkey to a        |
   |                            | command. Uses registerHotKey and speakString.          |
   +----------------------------+--------------------------------------------------------+
   |PI_imgui.py                 | Simple popup window with imgui-based widgets.          |
   |                            | See :doc:`/development/imgui`.                         |
   +----------------------------+--------------------------------------------------------+
   |PI_imguiBarometer.py        | Shows interaction between an ImGui widget and a        |
   |                            | dataref. A slider monitors current value of            |
   |                            | 'barometer_setting' dataref, updating the slider if the|
   |                            | value is changed in the cockpit. Also, if the value of |
   |                            | the slider is manually changed, it will update the     |
   |                            | value in the aircraft. Uses imgui, command handlers and|
   |                            | dataref APIs.                                          |
   +----------------------------+--------------------------------------------------------+
   | PI_InputOutput1.py         | Ported from python2 example. Demonstrates input /      |
   |                            | output of data from X-Plane, (changing engine throttle |
   |                            | and obtaining resulting N1). Should be used with the   |
   |                            | Override1 plugin                                       |
   |                            | Original by Sandy Barbour included with                |
   |                            | PythonInterface.                                       |
   +----------------------------+--------------------------------------------------------+
   |PI_KeySniffer1.py           | Ported from python2 example. Intercepts key strokes    |
   |                            | and displays result. Uses registerKeySniffer.          |
   +----------------------------+--------------------------------------------------------+
   |PI_ListBox.py               | Demonstrates the use of a listbox widget (requires     |
   |                            | XPListBox.py.) XPListBox was modeled after             |
   |                            | a version on                                           |
   |                            | `xsquawkbox.net                                        |
   |                            | <https://www.xsquawkbox.net/xpsdk/mediawiki/           |
   |                            | TestWidgets>`_.                                        |
   +----------------------------+--------------------------------------------------------+
   |PI_ManagePlugins1.py        | Ported from python2 example. Demonstrates finding,     |
   |                            | enabling and disabling plugins. Uses countPlugins,     |
   |                            | getNthPlugin, getMyID, disablePlugin                   |
   +----------------------------+--------------------------------------------------------+
   |PI_Map.py                   | Ported from C version                                  |
   |                            | `on developer.x-plane.com                              |
   |                            | <https://developer.x-plane.com/code-sample-type/       |
   |                            | xplm300-sdk/map/>`_.                                   |
   |                            | Draws on the X-Plane map using RegisterMapCreationHook,|
   |                            | OpenGL, drawMapLabel, mapProject, mapUnProject,        |
   |                            | mapScaleMeter.                                         |
   |                            |                                                        |        
   |                            | To use, you'll also need to copy map-sample-image.png  |
   |                            | from ``PythonPlugins/samples`` into                    |
   |                            | ``Resources/plugins`` folder.                          |
   +----------------------------+--------------------------------------------------------+
   |PI_MiniPython.py            | In-game python interpreter. Allows you to execute      |
   |                            | most python within context of running X-Plane. Great   |
   |                            | way to test SDK APIs and see what happens. Requires    |
   |                            | XPListBox.py to be copied also.                        |
   +----------------------------+--------------------------------------------------------+
   |PI_MultiProcess.py          | Demonstrates use of python multiprocessing module.     |
   |                            | Uses xp.pythonExecutable.                              |
   +----------------------------+--------------------------------------------------------+
   |PI_Navigation1.py           | Ported from python2 example. Demonstrates use of FMC   |
   |                            | and the navigation databases. Uses findNavAid,         |
   |                            | getNavAidInfo, setFMSEntry, clearFMSEntry.             |
   +----------------------------+--------------------------------------------------------+
   |PI_OpenGL.py                | Demonstrates use of OpenGL, *and* tests the            |
   |                            | installation for correct set up of OpenGL. **Note**    |
   |                            | Mac Big Sur will not run OpenGL without a minor        |
   |                            | one-time change. This plugin may be useful to          |
   |                            | plugin developers who require clients to install OpenGL|
   |                            | (or imgui).                                            |
   +----------------------------+--------------------------------------------------------+
   |PI_Override1.py             | Ported from python2. Demonstrates how to change        |
   |                            | various override datarefs. Original by Sandy Barbour   |
   |                            | included with PythonInterface.                         |
   +----------------------------+--------------------------------------------------------+
   |PI_Position1.py             | Ported from python2. Demonstrates how to change        |
   |                            | aircraft attitude. Original by Sandy Barbour included  |
   |                            | with PythonInterface. Uses localToWorld, worldToLocal, |
   |                            | getWidgetProperty, setWidgetProperty, setDataf.        |
   +----------------------------+--------------------------------------------------------+
   |PI_RightWingTip1.py         | Loads and displaces Instance objects. Display is       |
   |                            | relative to the user aircraft such that the objects    |
   |                            | appear to be "welded" to the airframe. Based on idea   |
   |                            | described at forums.x-plane.org. User createInstance,  |
   |                            | lookupObjects, loadObjectAsync, instanceSetPosition.   |
   +----------------------------+--------------------------------------------------------+
   |PI_SharedData1.py,          | Ported from python2. Demonstrates sharing data         |
   |PI_SharedData2.py           | between to different plugins. Using findDataRef,       |
   |                            | shareData, registerDataAccessor.                       |
   +----------------------------+--------------------------------------------------------+
   |PI_SimData1.py              | Ported from python2. Demonstrates how to interact with |
   |                            | X-Plane by reading and writing data. Uses createMenu,  |
   |                            | appendMenuItem, findDataRef, getDatai and setDatai.    |
   +----------------------------+--------------------------------------------------------+
   |PI_TabbedWidget.py          | Simple widget example, where we use two buttons        |
   |                            | ("tabs") to select between two panels of data          |
   |                            | inputs. Uses a hierarchy of widgets, fixedLayout widget|
   |                            | utility, widget messaging, and setting properties.     |
   +----------------------------+--------------------------------------------------------+
   |PI_TCASOverride.py          | This plugin creates four traffic targets that will fly |
   |                            | circles around the users' plane. These traffic targets |
   |                            | exist purely as TCAS targets, not as 3D objects, as    |
   |                            | such would usually be placed by XPLMInstance.          |
   |                            | Ported from C version on developer.x-plane.com         |
   |                            | `"Overriding TCAS" <https://developer.x-plane.com/code-|
   |                            | sample/overriding-tcas/>`_ and discussed in            |
   |                            | `"Overriding TCAS and providing traffic information"   |
   |                            | <https://developer.x-plane.com/article/overriding-tcas-|
   |                            | and-providing-traffic-information/>`_                  |
   |                            | Uses getDatad, getDataf, getDatavi,                    |
   |                            | setDatavf, setDatai,  setDatavi, setDatab, getDatab    |
   |                            | setActiveAircraftCount, acquirePlanes, releasePlanes,  |
   |                            | countAircraft, getPluginInfo.                          |
   +----------------------------+--------------------------------------------------------+
   |PI_TextureDraw.py           | Ported from C version on devevlper.x-plane.com         |
   |                            | `"TextureDraw" <https://developer.x-plane.com/code-samp|
   |                            | le/texturedraw/>`_ Uses bindTexture2d, setGraphicsState|
   |                            | , and generateTextureNumbers, with OpenGL              |
   |                            | glTexSubImage2D.                                       |
   +----------------------------+--------------------------------------------------------+
   |PI_TimedProcessing.py       | Ported from python2. Demonstrates how to use timed     |
   |                            | processing callback to continuously record data to     |
   |                            | disk. Uses getSystemPath, findDataRef, getDataf        |
   |                            | registerFlightLoopCallback, getElapsedTime.            |
   +----------------------------+--------------------------------------------------------+



 
