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
|:doc:`planes`           |* Get/Set users's aircraft and initial location.                     |
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
|:doc:`weather`          |* Get METAR for given airport                                        |
|                        |                                                                     |
|                        |* Get Weather Information for given location (latitude, longitude,   |
|                        |  altitude).                                                         |
+------------------------+---------------------------------------------------------------------+

X-Planes's Widget modules:

+------------------------+---------------------------------------------------------------------+
|:doc:`widgets`          |Create, destroy and otherwise manipuated pre-defined ("standard")    |
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
   weather
   uigraphics
   widgets
   widgetdefs
   standardwidgets
   widgetutils
   utilities
   xp
   xp_imgui
   python
