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

Debugger
========

Latest copy of the debugger is always available from the ``samples/`` directory.

   +----------------------------+--------------------------------------------------------+
   |PI_MiniPython.py            | In-game python interpreter. Allows you to execute most |
   |                            | python within context of running X-Plane. Great way to |
   |                            | test SDK APIs and see what happens.                    |
   +----------------------------+--------------------------------------------------------+

Demos
=====

   +----------------------------+--------------------------------------------------------+
   |PI_3dLabel.py               | Demonstrates how to use a 2-d drawing callback to draw |
   |                            | to the screen in a way that matches the 3-d coordinate |
   |                            | system. Add-ons that need to add 3-d labels, coach     |
   |                            | marks, or other non-3d graphics that "match" the real  |
   |                            | world can use this technique to draw on with Metal and |
   |                            | Vulkan. Ported from C version on developer.x-plane.com |
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
   |PI_Avionics1_Draw.py        | Creates a custom avionics device with a bouncing       |
   |                            | "screensaver" logo drawn on its screen, plus bezel     |
   |                            | drawing, click and cursor handling. Uses               |
   |                            | :func:`xp.createAvionicsEx`, :mod:`xpgl` and OpenGL.   |
   |                            | Has a black bezel and is *not* configured using        |
   |                            | :doc:`modules/easy_python`. See                        |
   |                            | :doc:`/development/modules/display_avionics`.          |
   +----------------------------+--------------------------------------------------------+
   |PI_Avionics2_Draw.py        | Same custom avionics device as PI_Avionics1_Draw.py,   |
   |                            | but configured using :doc:`modules/easy_python`, with a|
   |                            | grey bezel, and drawing through                        |
   |                            | :func:`xpgl.maskContext` and                           |
   |                            | :func:`xpgl.graphicsContext`.                          |
   |                            |                                                        |
   +----------------------------+--------------------------------------------------------+
   |PI_AvnWx.py                 | Posts current aircraft location to AvnWx.com, which    |
   |                            | enables live-tracking on https://maps.avnwx.com.       |
   |                            |                                                        |
   |                            | See :doc:`/development/avnwx-tracker` for details.     |
   |                            |                                                        |
   |                            |                                                        |
   +----------------------------+--------------------------------------------------------+
   |PI_Bounds.py                | Displays monitor number, size and if you're running VR |
   |                            | using :func:`xp.getWindowGeometry`,                    |
   |                            | :func:`xp.getAllMonitorBoundsGlobal` and related. See  |
   |                            | also :doc:`/development/window_position`.              |
   +----------------------------+--------------------------------------------------------+
   |PI_Camera1.py               | Ported from python2 example. Registers a new view and  |
   |                            | orbits the aircraft, using :func:`xp.controlCamera`.   |
   +----------------------------+--------------------------------------------------------+
   |PI_CommandSim1.py           | Ported from python2 example. Simple sending of XP      |
   |                            | commands using menu items.                             |
   |                            | :func:`xp.appendMenuItemWithCommand` and               |
   |                            | :func:`xp.findCommand`.                                |
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
   |                            | Uses OpenGL, textures, :func:`xp.registerHotKey`.      |
   +----------------------------+--------------------------------------------------------+
   |PI_FMOD_Advanced.py         | Modeled after Bill Good's xp12-fmod-sdk-demo.          |
   |                            |                                                        |
   |                            | Plays a sequence of sound files over-and-over.         |
   |                            |                                                        |
   |                            | Use python ctypes module, in combination with          |
   |                            | :func:`xp.getFMODStudio` to directly access additional |
   |                            | FMOD routines in the FMOD shared library.              |
   +----------------------------+--------------------------------------------------------+
   |PI_FMSUtility1.py           | Ported from python2 example. Demonstrates access to the|
   |                            | FMS. getDisplayFMSEntry, setDisplayFMSEntry,           |
   |                            | :func:`xp.getDestinationFMSEntry`,                     |
   |                            | :func:`xp.setDestinationFMSEntry`,                     |
   |                            | :func:`xp.getFMSEntryInfo`, :func:`xp.setFMSEntryInfo`,|
   |                            | :func:`xp.findNavAid`, :func:`xp.setFMSEntryLatLon`.   |
   +----------------------------+--------------------------------------------------------+
   |PI_HelloWidget.py           | Displays simple widget-based window, and logs messages.|
   |                            | Described more fully in                                |
   |                            | :doc:`/development/hellowidget`. Uses                  |
   |                            | :func:`xp.createWidget`, :func:`xp.addWidgetCallback`. |
   +----------------------------+--------------------------------------------------------+
   |PI_HelloWorld1.py           | Ported from python2 example. Described more fully in   |
   |                            | :doc:`/development/helloworld`. Uses                   |
   |                            | :func:`xp.createWindowEx` and window callbacks.        |
   +----------------------------+--------------------------------------------------------+
   |PI_HotKey1.py               | Ported from python2 example. Maps a hotkey to a        |
   |                            | command. Uses :func:`xp.registerHotKey` and            |
   |                            | :func:`xp.speakString`.                                |
   +----------------------------+--------------------------------------------------------+
   |PI_imgui.py                 | Simple popup window with imgui-based widgets. See      |
   |                            | :doc:`/development/imgui`.                             |
   +----------------------------+--------------------------------------------------------+
   |PI_ImguiAvionics.py         | ImGui drawn onto an avionics device screen rather than |
   |                            | into a window, using xp_imgui.AvionicsDevice. The same |
   |                            | widgets as PI_ImguiBoth.py, sharing the same ImGui host|
   |                            | -- only where the frame is displayed differs. Panel    |
   |                            | graphics only: the OpenGL renderer needs a window.     |
   |                            | Requires XPLM440 / X-Plane 12.4.4. See                 |
   |                            | :doc:`/development/imgui_avionics`.                    |
   +----------------------------+--------------------------------------------------------+
   |PI_imguiBarometer.py        | Shows interaction between an ImGui widget and a        |
   |                            | dataref. A slider monitors current value of            |
   |                            | 'barometer_setting' dataref, updating the slider if the|
   |                            | value is changed in the cockpit. Also, if the value of |
   |                            | the slider is manually changed, it will update the     |
   |                            | value in the aircraft. Uses imgui, command handlers and|
   |                            | dataref APIs.                                          |
   +----------------------------+--------------------------------------------------------+
   |PI_ImguiBoth.py             | Opens the same ImGui drawing twice side by side, one   |
   |                            | window per contentType (OpenGL and PanelGraphics), so  |
   |                            | the two render paths can be compared directly. Note    |
   |                            | translucent colors are expected to differ: panel       |
   |                            | graphics blends in linear space, so low-alpha fills    |
   |                            | read brighter. See :doc:`/development/imgui_plugin`.   |
   +----------------------------+--------------------------------------------------------+
   | PI_InputOutput1.py         | Ported from python2 example. Demonstrates input /      |
   |                            | output of data from X-Plane, (changing engine throttle |
   |                            | and obtaining resulting N1). Should be used with the   |
   |                            | Override1 plugin Original by Sandy Barbour included    |
   |                            | with PythonInterface.                                  |
   +----------------------------+--------------------------------------------------------+
   |PI_KeySniffer1.py           | Ported from python2 example. Intercepts key strokes and|
   |                            | displays result. Uses :func:`xp.registerKeySniffer`.   |
   +----------------------------+--------------------------------------------------------+
   |PI_Listbox.py               | Demonstrates the use of a listbox widget. XPListBox was|
   |                            | modeled after a version on `xsquawkbox.net             |
   |                            | <https://www.xsquawkbox.net/xpsdk/mediawiki/           |
   |                            | TestWidgets>`_.                                        |
   +----------------------------+--------------------------------------------------------+
   |PI_ManagePlugins1.py        | Ported from python2 example. Demonstrates finding,     |
   |                            | enabling and disabling plugins. Uses                   |
   |                            | :func:`xp.countPlugins`, :func:`xp.getNthPlugin`,      |
   |                            | :func:`xp.getMyID`, :func:`xp.disablePlugin`           |        
   +----------------------------+--------------------------------------------------------+
   |PI_Map.py                   | Ported from C version `on developer.x-plane.com        |
   |                            | <https://developer.x-plane.com/code-sample-type/       |
   |                            | xplm300-sdk/map/>`_. Draws on the X-Plane map using    |
   |                            | RegisterMapCreationHook, OpenGL,                       |
   |                            | :func:`xp.drawMapLabel`, :func:`xp.mapProject`,        |
   |                            | :func:`xp.mapUnproject`, :func:`xp.mapScaleMeter`.     |
   |                            |                                                        |
   |                            | To use, you'll also need to copy map-sample-image.png  |
   |                            | from ``PythonPlugins/samples`` into                    |
   |                            | ``Resources/plugins`` folder.                          |
   +----------------------------+--------------------------------------------------------+
   |PI_MultiProcess.py          | Demonstrates use of python multiprocessing module. Uses|
   |                            | :data:`xp.pythonExecutable`.                           |
   +----------------------------+--------------------------------------------------------+
   |PI_Navigation1.py           | Ported from python2 example. Demonstrates use of FMC   |
   |                            | and the navigation databases. Uses                     |
   |                            | :func:`xp.findNavAid`, :func:`xp.getNavAidInfo`,       |
   |                            | setFMSEntry, :func:`xp.clearFMSEntry`.                 |
   +----------------------------+--------------------------------------------------------+
   |PI_ObjAvionics.py           | Draws a device screen onto a 3-D object out in the     |
   |                            | world: binds a :func:`xp.createAvionicsEx` device to a |
   |                            | loaded object with :func:`xp.setObjectAvionics`,       |
   |                            | matched by deviceID via the object's                   |
   |                            | ATTR_cockpit_device. Self-contained --- it writes its  |
   |                            | own single-quad .obj and texture at enable, and pins   |
   |                            | the instance ahead of the aircraft so the screen rides |
   |                            | with you like a view-limiting hood. Requires XPLM440 / |
   |                            | X-Plane 12.4.4. See                                    |
   |                            | :doc:`/development/modules/display_avionics`.          |
   +----------------------------+--------------------------------------------------------+
   |PI_OpenGL.py                | Demonstrates use of OpenGL, *and* tests the            |
   |                            | installation for correct set up of OpenGL. **Note** Mac|
   |                            | Big Sur will not run OpenGL without a minor one-time   |
   |                            | change. This plugin may be useful to plugin developers |
   |                            | who require clients to install OpenGL (or imgui).      |
   +----------------------------+--------------------------------------------------------+
   |PI_Override1.py             | Ported from python2. Demonstrates how to change various|
   |                            | override datarefs. Original by Sandy Barbour included  |
   |                            | with PythonInterface.                                  |
   +----------------------------+--------------------------------------------------------+
   |PI_Position1.py             | Ported from python2. Demonstrates how to change        |
   |                            | aircraft attitude. Original by Sandy Barbour included  |
   |                            | with PythonInterface. Uses :func:`xp.localToWorld`,    |
   |                            | :func:`xp.worldToLocal`, :func:`xp.getWidgetProperty`, |
   |                            | :func:`xp.setWidgetProperty`, :func:`xp.setDataf`.     |
   +----------------------------+--------------------------------------------------------+
   |PI_Probe.py                 | Continuously reports terrain elevation under the       |
   |                            | aircraft, drawing the result on screen. Uses           |
   |                            | :func:`xp.createProbe`, :func:`xp.probeTerrainXYZ` and |
   |                            | :func:`xp.localToWorld`, with the                      |
   |                            | :doc:`modules/easy_python` module and datarefs         |
   |                            | utilities.                                             |
   +----------------------------+--------------------------------------------------------+
   |PI_RightWingTip.py          | Loads and displaces Instance objects. Display is       |
   |                            | relative to the user aircraft such that the objects    |
   |                            | appear to be "welded" to the airframe. Based on idea   |
   |                            | described at forums.x-plane.org. User                  |
   |                            | :func:`xp.createInstance`, :func:`xp.lookupObjects`,   |
   |                            | :func:`xp.loadObjectAsync`,                            |
   |                            | :func:`xp.instanceSetPosition`.                        |
   +----------------------------+--------------------------------------------------------+
   |PI_SharedData1.py,          | Ported from python2. Demonstrates sharing data between |
   |PI_SharedData2.py           | to different plugins. Using :func:`xp.findDataRef`,    |
   |                            | :func:`xp.shareData`, :func:`xp.registerDataAccessor`. |
   +----------------------------+--------------------------------------------------------+
   |PI_SimData1.py              | Ported from python2. Demonstrates how to interact with |
   |                            | X-Plane by reading and writing data. Uses              |
   |                            | :func:`xp.createMenu`, :func:`xp.appendMenuItem`,      |
   |                            | :func:`xp.findDataRef`, :func:`xp.getDatai` and        |
   |                            | :func:`xp.setDatai`.                                   |
   +----------------------------+--------------------------------------------------------+
   |PI_TabbedWidget.py          | Simple widget example, where we use two buttons        |
   |                            | ("tabs") to select between two panels of data inputs.  |
   |                            | Uses a hierarchy of widgets, :func:`xp.fixedLayout`    |
   |                            | widget utility, widget messaging, and setting          |
   |                            | properties.                                            |
   +----------------------------+--------------------------------------------------------+
   |PI_TCASOverride.py          | This plugin creates four traffic targets that will fly |
   |                            | circles around the users' plane. These traffic targets |
   |                            | exist purely as TCAS targets, not as 3D objects, as    |
   |                            | such would usually be placed by XPLMInstance. Ported   |
   |                            | from C version on developer.x-plane.com `"Overriding   |
   |                            | TCAS" <https://developer.x-plane.com/code-             |
   |                            | sample/overriding-tcas/>`_ and discussed in            |
   |                            | `"Overriding TCAS and providing traffic information"   |
   |                            | <https://developer.x-plane.com/article/overriding-tcas-|
   |                            | and-providing-traffic-information/>`_ Uses             |
   |                            | :func:`xp.getDatad`, :func:`xp.getDataf`,              |
   |                            | :func:`xp.getDatavi`, :func:`xp.setDatavf`,            |
   |                            | :func:`xp.setDatai`, :func:`xp.setDatavi`,             |
   |                            | :func:`xp.setDatab`, :func:`xp.getDatab`               |
   |                            | :func:`xp.setActiveAircraftCount`,                     |
   |                            | :func:`xp.acquirePlanes`, :func:`xp.releasePlanes`,    |
   |                            | :func:`xp.countAircraft`, :func:`xp.getPluginInfo`.    |
   +----------------------------+--------------------------------------------------------+
   |PI_TextureDraw.py           | Ported from C version on devevlper.x-plane.com         |
   |                            | `"TextureDraw" <https://developer.x-plane.com/code-samp|
   |                            | le/texturedraw/>`_ Uses :func:`xp.bindTexture2d`,      |
   |                            | :func:`xp.setGraphicsState` , and                      |
   |                            | :func:`xp.generateTextureNumbers`, with OpenGL         |
   |                            | glTexSubImage2D.                                       |
   +----------------------------+--------------------------------------------------------+
   |PI_TimedProcessing1.py      | Ported from python2. Demonstrates how to use timed     |
   |                            | processing callback to continuously record data to     |
   |                            | disk. Uses :func:`xp.getSystemPath`,                   |
   |                            | :func:`xp.findDataRef`, :func:`xp.getDataf`            |
   |                            | :func:`xp.registerFlightLoopCallback`,                 |
   |                            | :func:`xp.getElapsedTime`.                             |
   +----------------------------+--------------------------------------------------------+
   |PI_xlua_skeleton.py         | Commented skeleton for xlua-style plugins: a starting  |
   |                            | point showing where datarefs, commands, timers and the |
   |                            | xlua callbacks go. See                                 |
   |                            | :doc:`/development/xlua_typical`.                      |
   +----------------------------+--------------------------------------------------------+

Panel Graphics Demos
====================

   +----------------------------+--------------------------------------------------------+
   |PI_PGDrawCalls.py           | Exercises the ImGui-style mesh drawing API: builds a   |
   |                            | texture with PIL, uploads one mesh, then dispatches it |
   |                            | three ways in a labelled grid -- textured, untextured  |
   |                            | with per-vertex color, and clipped by a scissor rect.  |
   |                            | Uses :func:`xp.createTexture`, :func:`xp.drawCalls`,   |
   |                            | :func:`xp.destroyTexture`. Requires PIL and XPLM440 /  |
   |                            | X-Plane 12.4.4. See                                    |
   |                            | :doc:`/development/modules/panelgraphics_textures`.    |
   +----------------------------+--------------------------------------------------------+
   |PI_PGFonts.py               | Exercises all twelve panel-graphics font functions:    |
   |                            | :func:`xp.createFont`, :func:`xp.fontAddFace`, the     |
   |                            | measurement/fit family (:func:`xp.fontGetMetrics`,     |
   |                            | :func:`xp.fontMeasureString`,                          |
   |                            | :func:`xp.fontGetLineCount`, :func:`xp.fontFitForward`,|
   |                            | :func:`xp.fontFitReverse`) and the four fontDrawString*|
   |                            | variants. Logs measurement results, then draws a       |
   |                            | labelled grid. Requires XPLM440 / X-Plane 12.4.4 See   |
   |                            | :doc:`/development/modules/panelgraphics_fonts`.       |
   +----------------------------+--------------------------------------------------------+
   |PI_PGLineCaps.py            | Shows the three line-cap styles: a labelled grid with  |
   |                            | one cap per cell, drawn over thin endpoint ticks at the|
   |                            | exact coordinates passed in, so you can see butt stop  |
   |                            | flush while round and square extend half the line width|
   |                            | past. Includes a stipple band, where the cap applies to|
   |                            | each dash. Uses :func:`xp.setLineCap`. Requires XPLM440|
   |                            | / X-Plane 12.4.4. See                                  |
   |                            | :doc:`/development/modules/panelgraphics_primitives`.  |
   +----------------------------+--------------------------------------------------------+
   |PI_PGMapLayers.py           | Interactive explorer for the base-map display: one     |
   |                            | pop-out avionics device drawing a single map view.     |
   |                            | SPACE cycles the eight map layers (Nexrad, IR, Topo,   |
   |                            | Terrain, Water, EGPWS, raw_elev, safe_taxi), TAB steps |
   |                            | through the dataOverride fields. Uses                  |
   |                            | :func:`xp.createMapDisplay`,                           |
   |                            | :func:`xp.mapDisplayDrawIn`,                           |
   |                            | :func:`xp.mapDisplayScaleMeter` and friends. Requires  |
   |                            | XPLM440 / X-Plane 12.4.4 See                           |
   |                            | :doc:`/development/modules/panelgraphics_map`.         |
   +----------------------------+--------------------------------------------------------+
   |PI_PGPrimitives.py          | Draws a labelled grid with one cell per primitive:     |
   |                            | lines, line strips and loops, stippled and per-vertex- |
   |                            | colored variants, filled polygons and quad strips, plus|
   |                            | transform, scissor and stencil cells. Pairs of cells   |
   |                            | show the same shape wound both ways, so a blank cell   |
   |                            | means something is wrong. Requires XPLM440 / X-Plane   |
   |                            | 12.4.4. See                                            |
   |                            | :doc:`/development/modules/panelgraphics_primitives`.  |
   +----------------------------+--------------------------------------------------------+
   |PI_PGRetained.py            | Records one drawing motif into a retained drawing, then|
   |                            | replays that single handle across several tiles under  |
   |                            | different transforms. A reference tile draws the same  |
   |                            | motif in immediate mode, so replay can be compared     |
   |                            | against it directly. Uses the classic                  |
   |                            | :func:`xp.beginRetainedDrawing` /                      |
   |                            | :func:`xp.endRetainedDrawing` / :func:`xp.drawRetained`|
   |                            | / :func:`xp.destroyRetainedDrawing` functions (see also|
   |                            | the :func:`xp.retainedDrawing` context manager).       |
   |                            | Requires XPLM440 / X-Plane 12.4.4. See                 |
   |                            | :doc:`/development/modules/panelgraphics_retained`.    |
   +----------------------------+--------------------------------------------------------+
   |PI_PGScissor.py             | Minimal panel-graphics scissor (clip rectangle)        |
   |                            | example: fills a window red, then fills it green again |
   |                            | inside a scissor, so only the clip rectangle changes   |
   |                            | color. Uses :func:`xp.transformContext`,               |
   |                            | :func:`xp.scissorContext` and :func:`xp.scissorSet`.   |
   |                            | Requires XPLM440 / X-Plane 12.4.4 See                  |
   |                            | :doc:`/development/modules/panelgraphics_state`.       |
   +----------------------------+--------------------------------------------------------+
   |PI_PGStencil.py             | Exercises the stencil-mask API in one window. Two      |
   |                            | overlapping squares are stamped into independent bit   |
   |                            | channels and a five-pointed star into a third; four    |
   |                            | full-window polygons are then drawn, so only the       |
   |                            | stencil test confines each -- giving A-only, B-only,   |
   |                            | overlap and outside regions, with the star knocked out |
   |                            | of all four. Uses :func:`xp.beginSetupStencilMask`,    |
   |                            | :func:`xp.endSetupStencilMask`,                        |
   |                            | :func:`xp.useStencilMask`, :func:`xp.clearStencilMask`.|
   |                            | Requires XPLM440 / X-Plane 12.4.4. See                 |
   |                            | :doc:`/development/modules/stencilmask`.               |
   +----------------------------+--------------------------------------------------------+
   |PI_PGSVTFeatures.py         | Interactive explorer for the SVT (synthetic vision)    |
   |                            | display: one pop-out avionics device drawing a single  |
   |                            | SVT view. SPACE cycles the nine feature flags, TAB and |
   |                            | the arrow keys walk the dataOverride fields so you can |
   |                            | see each one take effect. Uses                         |
   |                            | :func:`xp.createSVTDisplay`,                           |
   |                            | :func:`xp.svtDisplayDrawIn`,                           |
   |                            | :func:`xp.destroySVTDisplay`. Requires XPLM440 /       |
   |                            | X-Plane 12.4.4 See                                     |
   |                            | :doc:`/development/modules/panelgraphics_svt`.         |
   +----------------------------+--------------------------------------------------------+
   |PI_PGTextureAtlas.py        | Packs seven procedurally generated images and one of   |
   |                            | X-Plane's own PNGs into a single texture atlas, then   |
   |                            | draws each entry through drawAt, drawIn, drawStretched,|
   |                            | drawScaled (rotating) and drawMesh in a labelled grid. |
   |                            | Uses :func:`xp.createTextureAtlas`,                    |
   |                            | :func:`xp.textureAtlasAddImage` and variants,          |
   |                            | :func:`xp.textureAtlasBake`,                           |
   |                            | :func:`xp.destroyTextureAtlas`. Requires XPLM440 /     |
   |                            | X-Plane 12.4.4. See                                    |
   |                            | :doc:`/development/modules/panelgraphics_textures`.    |
   +----------------------------+--------------------------------------------------------+
   |PI_PGTouch.py               | Interactive touch-zone example: each frame it registers|
   |                            | an Identifier zone over a drawn button, colors the     |
   |                            | button by the returned held-state, and shows the fields|
   |                            | of each touch event (identifier, status, position,     |
   |                            | delta, button, refCon) on screen as they arrive. Uses  |
   |                            | :func:`xp.accumulateTouchZone` and                     |
   |                            | :func:`xp.windowSetTouchEventHandler`. Requires XPLM440|
   |                            | / X-Plane 12.4.4. See                                  |
   |                            | :doc:`/development/modules/panelgraphics_touch`.       |
   +----------------------------+--------------------------------------------------------+

 
.. toctree::
   :hidden:

   avnwx-tracker
