Python SDK Modules
==================

We've attempted to fully document the API here, noting changes from the official C-Language SDK.
(Don't forget to use the `Search`
field in the header of each of these documentation web pages to find what you're looking for!)

*Most* of the APIs in this documentation include example source code which can be cut and pasted into the :doc:`/development/debugger`.

Additionally:

* :doc:`/changelog`. Lists recent changes to this SDK.

* `X-Plane Developer Documentation @ developer.x-plane.com <https://developer.x-plane.com/sdk/plugin-sdk-documents/>`_.  The official X-Plane SDK documentation is for C-language, and most of that has been translated to python and documented with XPPython3. However, when in doubt, check the Laminar docs.

Note we use a Simplified Python Interface as described in :doc:`xp`. This makes **all** interfaces available from::

  import xp

You'll find it much faster and easier to write new
code using this interface and it won't wear out your X-P-L-M keys. You can use the older style, but you'll have to import
each module separately::
  
  import XPLMMenus

Not sure which modules contain what you want? Here's an overview arranged by features:

* Accessing **dataRefs** (internal X-Plane data), and creating your own dataRefs. :doc:`dataaccess`

  * with a simpler lua-like access using easy datarefs :doc:`datarefs`

* **Command** creation and execution, also allows you to augment (or replace) existing defined commands :doc:`utilities`
  
  * with a simpler lua-like access using easy commands :doc:`commands`

* Creating **Menus** to execute code or existing commands :doc:`menus`

* Creating **flight loops**, which can execute your code at specific points in time, including every frame. :doc:`processing`

  * **Timers**\: really just simple flight loops :doc:`timers`

* Drawing:

  * Drawing within a **window** :doc:`display_window`

  * Drawing within an **avionics device window** :doc:`display_avionics`

  * Drawing on the **screen** using old-style, and largely deprecated draw callbacks :doc:`display_direct`

  * Using pre-defined 3d object for **Instance** drawing: :doc:`instance`. And access to existing library of objects :doc:`scenery`.
    
  * **OpenGL** interactions, state, textures (including weather radar): :doc:`graphics`
    
  * **Python OpenGL** drawing primitives (lines, polygons, text, transformations, etc.: :doc:`xpgl`
    
  * Simple (non-OpenGL) drawing primitives :doc:`graphics`, and graphical elements :doc:`uigraphics`

  * **Widgets** Old style buttons, textboxes, checkboxes, etc. useful for basic user interaction. :doc:`widgets`, doc:`widgetutils`, doc:`standardwidgets`
    
  * **IMGUI** integration for newer-style buttons, etc. :doc:`xp_imgui`
    
  * **Cursor** manipulation :doc:`cursor`
    
* Querying and controlling the **weather** :doc:`weather`
  
* Accessing the **NavAid** database :doc:`navigation`

* Interacting with **Flight Management Systems** (FMS) and flight plans :doc:`navigation`  

* **Replay and situation files** loading and saving :doc:`utilities`

* **Aircraft**: setting and moving the user plane, changing aircraft type, disabling AI aircraft. :doc:`planes`

* Drawing on the pop-up **Map**. Adding labels and icons. :doc:`map`

* Controlling the **Camera**, changing the position and location of the camera :doc:`camera`

* Keyboard shortcuts:

  * **Hot Keys**: Higher level keystroke intercept to execute code. Hot keys "listen" for a specific
    keystroke, and are *only* called when that stroke is made. :doc:`display_hotkeys`.

  * **Key Sniffing**: Intercept a keystroke and execute code. Lowest level and can "consume" the key blocking
    other usage. Sniffer are called on *every* keystroke and are passed the current keystroke, which
    they can opt to ignore: :doc:`display_sniffer`

* Coordinate systems:

  * OpenGL ("local") and Lat/Lng ("world") :doc:`graphics`

  * Elapsed time, and cycle number :doc:`processing`

  * Mesh probes (vertical distance to terrain) :doc:`scenery`

  * Magnetic variation :doc:`scenery`

* FMOD **Sound**, playing audio clips and positioning them in 3d space :doc:`sound`, or simple "speak string" for alerts :doc:`utilities`.
  
* Working with other **plugins**, messages your plugin will receive (PLANE_LOADED, PLANE_CRASHED, etc.) and finding
  and messaging other plugins : :doc:`plugin`

* **System information**: paths, version, language :doc:`utilities`

* **Python** infrastructure: version, executable, paths. :doc:`python`.

  * Python package management using **pip** :doc:`xp_pip`.

  * Performance **Statistics**\: :doc:`python`

  * Access to internal XPPython Datastructures to aid debugging: :doc:`python`

* **Logging** to Log.txt and to XPPython3Log.txt :doc:`python`.

For compatibility with Laminar documentation, this documentation splits the SDK along the same divisions:

+------------------------+---------------------------------------------------------------------+
|:doc:`camera`           |Control camera angle.                                                |
+------------------------+---------------------------------------------------------------------+
|:doc:`dataaccess`       |Read/Write DataRefs, also create your own.                           |
+------------------------+---------------------------------------------------------------------+
|:doc:`defs`             |Enumerations for Keycodes, primarily.                                |
+------------------------+---------------------------------------------------------------------+
|:doc:`display`          |* Direct drawing, using "draw callbacks".                            |
|                        |                                                                     |
|                        |* Avionics drawing, using "avionics callbacks".                      |
|                        |                                                                     |
|                        |* Window creation and interaction (visibility, geometry, drawing,    |
|                        |  clicks, key handling).                                             |
|                        |                                                                     |
|                        |* Monitor / Screen size query functions.                             |
|                        |                                                                     |
|                        |* Key-sniffing and hot keys.                                         |
+------------------------+---------------------------------------------------------------------+
|:doc:`graphics`         |* Basic OpenGL interface.                                            |
|                        |                                                                     |
|                        |* Local / World Coordinate system conversion.                        |
|                        |                                                                     |
|                        |* Basic drawing functions (drawString, measureString), of limited    |
|                        |  utility.                                                           |
+------------------------+---------------------------------------------------------------------+
|:doc:`instance`         |Preferred drawing routines using "objects" loaded by                 |
|                        |:doc:`scenery`. Create, destroy and set position of instances.       |
+------------------------+---------------------------------------------------------------------+
|:doc:`map`              |Create new layer and draw on X-Plane map.                            |
+------------------------+---------------------------------------------------------------------+
|:doc:`menus`            |Create menus & menu items and provide handlers.                      |
+------------------------+---------------------------------------------------------------------+
|:doc:`navigation`       |* Query NavAid database.                                             |
|                        |                                                                     |
|                        |* Get/Set Flight Management Systems (FMS) entries. (This includes    |
|                        |  basic systems such as G530 and more advanced systems found in      |
|                        |  airliners.)                                                        |
+------------------------+---------------------------------------------------------------------+
|:doc:`planes`           |* Get/Set user's aircraft and initial location.                      |
|                        |                                                                     |
|                        |* Disable AI aircraft.                                               |
+------------------------+---------------------------------------------------------------------+
|:doc:`plugin`           |* Find, enable / disable other plugins.                              |
|                        |                                                                     |
|                        |* Send messages to other plugins                                     |
+------------------------+---------------------------------------------------------------------+
|:doc:`processing`       |* Get current elapsed time and cycle number.                         |
|                        |                                                                     |
|                        |* Create, schedule, and destroy Flight Loops.                        |
+------------------------+---------------------------------------------------------------------+
|:doc:`scenery`          |* Terrain testing (height, slope, relative movement) using probes.   |
|                        |                                                                     |
|                        |* Magnetic variation (declination) at a point.                       |
|                        |                                                                     |
|                        |* Lookup and loading of scenery objects. (Used with :doc:`instance`.)|
+------------------------+---------------------------------------------------------------------+
|:doc:`uigraphics`       |* Draw Windows, to be executed within :doc:`display` draw callbacks. |
|                        |                                                                     |
|                        |* Draw pre-defined decorative elements such as Checkbox, Waypoint,   |
|                        |  CoolingTower: you'll have to manage clicks if you want interaction.|
|                        |                                                                     |
|                        |* Draw Tracks ("scroll bars"). Again, you have to manage interaction.|
+------------------------+---------------------------------------------------------------------+
|:doc:`utilities`        |* Execute X-Plane Commands, also create your own.                    |
|                        |                                                                     |
|                        |* Save & Load situations and replays.                                |
|                        |                                                                     |
|                        |* Speak String, reload scenery, get virtual key description and      |
|                        |  current language.                                                  |
|                        |                                                                     |
|                        |* Get paths, directory contents utilities.                           |
|                        |                                                                     |
|                        |* Find internal X-Plane function symbol, for access to code not      |
|                        |  directly supported by the SDK.                                     |
+------------------------+---------------------------------------------------------------------+
|:doc:`sound`            |Play sound, at a specific location, in a particular direction, with  |
|                        |fading. (Uses FMOD).                                                 |
|                        |                                                                     |
|                        |                                                                     |
+------------------------+---------------------------------------------------------------------+
|:doc:`weather`          |* Get METAR for given airport                                        |
|                        |                                                                     |
|                        |* Get Weather Information for given location (latitude, longitude,   |
|                        |  altitude).                                                         |
+------------------------+---------------------------------------------------------------------+

X-Planes's Widget modules:

+------------------------+---------------------------------------------------------------------+
|:doc:`widgets`          |Create, destroy and otherwise manipulated pre-defined ("standard")   |
|                        |widgets. Send (widget) messages, and handle messages through         |
|                        |callbacks.                                                           |
+------------------------+---------------------------------------------------------------------+
|:doc:`standardwidgets`  |Pre-defined widgets, with their properties and messages. Including   |
|                        |                                                                     |
|                        | | Window                                                            |
|                        | | Button                                                            |
|                        | | Text Field                                                        |
|                        | | Scroll Bar                                                        |
|                        | | Caption                                                           |
|                        | | Progress Indicator                                                |
+------------------------+---------------------------------------------------------------------+
|:doc:`widgetdefs`       |Enumerations related to widgets -- mostly message and property IDs.  |
+------------------------+---------------------------------------------------------------------+
|:doc:`widgetutils`      |Mostly useless functions.                                            |
+------------------------+---------------------------------------------------------------------+

Some "helper" pure-python modules, built on top of the others, making basic functionality easier
to implement.

+-------------------------+---------------------------------------------------------------------+
|:doc:`timers`            |Create and schedule one-shot and interval timers. Easier than working|
|                         |with flight loops.                                                   |
|                         |                                                                     |
+-------------------------+---------------------------------------------------------------------+
|:doc:`commands`          |Create and execute X-Plane commands. Alter existing commands.        |
|                         |                                                                     |
|                         |                                                                     |
+-------------------------+---------------------------------------------------------------------+
|:doc:`datarefs`          |Create and access datarefs.                                          |
|                         |                                                                     |
|                         |                                                                     |
+-------------------------+---------------------------------------------------------------------+
|:doc:`easy_python`       |Optional parent class for required PythonInterface class. Provides a |
|                         |number of well-defined callback points, making it easier for you to  |
|                         |code a plugin.                                                       |
+-------------------------+---------------------------------------------------------------------+
|:doc:`xlua_variables`    |Convenience code with pre-defined global parameters, also commonly   |
|                         |used in xlua programs.                                               |
|                         |                                                                     |
+-------------------------+---------------------------------------------------------------------+

More pure-python modules useful for graphics:

+------------------------+---------------------------------------------------------------------+
|:doc:`xpgl`             |Convenience wrapper around common OpenGL routines allowing you to    |
|                        |quickly draw shapes, load images, and fonts.                         |
|                        |                                                                     |
+------------------------+---------------------------------------------------------------------+

And three XPPython-specific additions (also accessible with simple ``import xp``.)

+------------------------+---------------------------------------------------------------------+
|:doc:`xp`               |The only python modules you really need. This page also describes the|
|                        |pythonic, simplified API mapping with optional and keyword           |
|                        |parameters.                                                          |
+------------------------+---------------------------------------------------------------------+
|:doc:`xp_imgui`         |API to the Dear ImGui graphics library, with examples.               |
+------------------------+---------------------------------------------------------------------+
|:doc:`python`           |* Logging functions                                                  |
|                        |                                                                     |
|                        |* Access internal XPPython3 data structures, perhaps useful for      |
|                        |  debugging new plugins.                                             |
+------------------------+---------------------------------------------------------------------+

.. toctree::
   :hidden:

   camera
   dataaccess
   defs
   display
   graphics
   instance
   map
   menus
   navigation
   planes
   plugin
   processing
   scenery
   sound
   weather
   uigraphics
   widgets
   widgetdefs
   standardwidgets
   widgetutils
   utilities
   cursor
   xp
   xp_imgui
   xp_pip
   python
   timers
   commands
   datarefs
   easy_python
   xlua_variables
   xpgl
