XPLMPlugin
==========
.. py:module:: XPLMPlugin

To use::
  
  import XPLMPlugin

Finding Plugins
---------------

These APIs allow you to find another plugin or yourself, or iterate across
all plugins.  For example, if you wrote an FMS plugin that needed to talk
to an autopilot plugin, you could use these APIs to locate the autopilot
plugin.

.. py:data:: XPLMPluginID

   Integer plugin ID.

.. py:data:: XPLM_NO_PLUGIN_ID

   Value of :py:data:`XPLMPluginID` when refering to no plugin.             

.. py:function:: XPLMGetMyID(None) -> int:

 :return: :py:data:`XPLMPluginID`                 

 This routine returns the plugin ID of the calling plug-in.  Call this to
 get your own ID.

 .. note:: This is the plugin ID of the XPPython3 plugin because, from the
           perspective of X-Plane, all python plugins are running within
           a single plugin.


.. py:function::  XPLMCountPlugins(None) -> int:

 This routine returns the total number of plug-ins that are loaded, both
 disabled and enabled.

.. py:function:: XPLMGetNthPlugin(index: int) -> int:

 :return: :py:data:`XPLMPluginID`                 

 This routine returns the ID of a plug-in by index.  Index is 0 based from 0
 to :py:func:`XPLMCountPlugins`-1, inclusive. Plugins may be returned in any arbitrary
 order.


.. py:function::  XPLMFindPluginByPath(path: str) -> int:

 :param str path: path of plugin to be found                  
 :return: :py:data:`XPLMPluginID`                 

 This routine returns the plug-in ID of the plug-in whose file exists at the
 passed in absolute file system path.  XPLM_NO_PLUGIN_ID is returned if the
 path does not point to a currently loaded plug-in.


.. py:function:: XPLMFindPluginBySignature(signature: str) -> int:

 :param str signature: Signature of plugin to be found
 :return: :py:data:`XPLMPluginID`                 

 This routine returns the plug-in ID of the plug-in whose signature matches
 what is passed in or :py:data:`XPLM_NO_PLUGIN_ID` if no running plug-in has this
 signature.  Signatures are the best way to identify another plug-in as they
 are independent of the file system path of a plug-in or the human-readable
 plug-in name, and should be unique for all plug-ins.  Use this routine to
 locate another plugin that your plugin interoperates with


.. py:data:: PluginInfo

 Object returned by :py:func:`XPLMGetPluginInfo` containing
 information about a plugin. It has the following string attributes:

 | name
 | filePath
 | signature
 | description


.. py:function:: XPLMGetPluginInfo(pluginID: int) -> pluginInfo:

 :param pluginID: :py:data:`XPLMPluginID`                 

 This routine returns information about a plug-in as a :py:data:`PluginInfo` object.

.. py:function::  XPLMIsPluginEnabled(pluginID: int) -> is_enabled:

 :return: int, 1= is enabled

 Returns whether the specified plug-in is enabled for running.


.. py:function:: XPLMEnablePlugin(pluginID: int) -> success:

 :return: int, 1= successfully enabled

 This routine enables a plug-in if it is not already enabled.  It returns 1
 if the plugin was enabled or successfully enables itself, 0 if it does not.
 Plugins may fail to enable (for example, if resources cannot be acquired)
 by returning 0 from their XPluginEnable callback.


.. py:function:: XPLMDisablePlugin(pluginID: int) -> None:

 This routine disables an enabled plug-in.


.. py:function::  XPLMReloadPlugins(None) -> None:

 This routine reloads all plug-ins.  Once this routine is called and you
 return from the callback you were within (e.g. a menu select callback) you
 will receive your ``XPluginDisable`` and ``XPluginStop`` callbacks,
 then the start process happens as if the sim was starting up.

 .. note:: This reloads **all plugins** not just XPPython3 plugins.

 .. warning:: Many (most?) plugins don't clean up after themselves, so reloading
    may result in an unstable state.
    

Interplugin Messaging
---------------------

Plugin messages are defined as 32-bit integers.  Messages below 0x00FFFFFF
are reserved for X-Plane and the plugin SDK.

Messages have two conceptual uses: notifications and commands.  Commands
are  sent from one plugin to another to induce behavior; notifications are
sent  from one plugin to all others for informational purposes.  It is
important that commands and notifications not have the same values because
this could cause a notification sent by one plugin to accidentally induce a
command in another.

By convention, plugin-defined notifications should have the high bit set
(e.g. be greater or equal to unsigned 0x8000000) while commands should have
this bit be cleared.

The following messages are sent to your plugin by X-Plane.

.. py:data:: XPLM_MSG_PLANE_CRASHED

 This message is sent to your plugin whenever the user's plane crashes.

.. py:data:: XPLM_MSG_PLANE_LOADED

 This message is sent to your plugin whenever a new plane is loaded. The
 parameter is the number of the plane being loaded; 0 indicates the user's
 plane.

.. py:data:: XPLM_MSG_AIRPORT_LOADED

 This messages is called whenever the user's plane is positioned at a new
 airport.
  
.. py:data:: XPLM_MSG_SCENERY_LOADED

 This message is sent whenever new scenery is loaded. Use datarefs to
 determine the new scenery files that were loaded.

.. py:data:: XPLM_MSG_AIRPLANE_COUNT_CHANGED

 This message is sent whenever the user adjusts the number of X-Plane
 aircraft models. You must use XPLMCountPlanes to find out how many planes
 are now available. This message will only be sent in XP7 and higher
 because in XP6 the number of aircraft is not user-adjustable.

.. py:data:: XPLM_MSG_PLANE_UNLOADED

 This message is sent to your plugin whenever a plane is unloaded. The
 parameter is the number of the plane being unloaded; 0 indicates the user's
 plane. The parameter is of type int, passed as the value of the pointer.
 (That is: the parameter is an int, not a pointer to an int.)

.. py:data:: XPLM_MSG_WILL_WRITE_PREFS

 This message is sent to your plugin right before X-Plane writes its
 preferences file. You can use this for two purposes: to write your own
 preferences, and to modify any datarefs to influence preferences output.
 For example, if your plugin temporarily modifies saved preferences, you can
 put them back to their default values here to avoid having the tweaks be
 persisted if your plugin is not loaded on the next invocation of X-Plane.

.. py:data:: XPLM_MSG_LIVERY_LOADED

 This message is sent to your plugin right after a livery is loaded for an
 airplane. You can use this to check the new livery (via datarefs) and
 react accordingly. The parameter is of type int, passed as the value of a
 pointer and represents the aicraft plane number - 0 is the user's plane.

.. py:data:: XPLM_MSG_ENTERED_VR

 Sent to your plugin right before X-Plane enters virtual reality mode (at
 which time any windows that are not positioned in VR mode will no longer be
 visible to the user).

.. py:data:: XPLM_MSG_EXITING_VR = 110

 Sent to your plugin right before X-Plane leaves virtual reality mode (at
 which time you may want to clean up windows that are positioned in VR
 mode).

.. py:function:: XPLMSendMessageToPlugin(pluginID: int, message: int, param: object) -> None:

 :param pluginID: :py:data:`XPLMPluginID`                 
 :param message: One of the above ``XPLM_MSG`` messages
 :param object param: parameter appropriate for the type of message being sent.

 This function sends a message to another plug-in or X-Plane.  Pass
 :py:data:`XPLM_NO_PLUGIN_ID` to broadcast to all plug-ins.  Only enabled plug-ins with
 a message receive function receive the message.
 param should be a string or an integer



Plugin Features API
-------------------

The plugin features API allows your plugin to "sign up" for additional
capabilities and plugin system features that are normally disabled for
backward compatibility.  This allows advanced plugins to "opt-in" to new
behavior.

Each feature is defined by a permanent string name.  The feature string
names will vary with the particular  installation of X-Plane, so plugins
should not expect a feature to be guaranteed present.

.. py:function:: XPLMHasFeature(feature: str) -> feature_supported:

 :return: int, 1= feature is supported

 This returns 1 if the given installation of X-Plane supports a feature, or
 0 if it does not.


.. py:function:: XPLMIsFeatureEnabled(inFeature) -> feature_enabled:

 :return: int, 1= feature is enabled

 This returns 1 if a feature is currently enabled for your plugin, or 0 if
 it is not enabled.  It is an error to call this routine with an unsupported
 feature.

.. py:function::  XPLMEnableFeature(feature: str, enable:int) -> None:

 This routine enables or disables a feature for your plugin.  This will
 change the running behavior of X-Plane and your plugin in some way,
 depending on the feature.


.. py:function:: XPLMFeatureEnumerator_f(feature: str, refCon: object) -> None:

  Callback you provide to :py:func:`XPLMEnumerateFeatures` to get a list of features
  supported by the running version of X-Plane.  This routine is called once
  for each feature.

.. py:function:: XPLMEnumerateFeatures(featureEnumerator: callable, refCon object) -> None:

    This routine calls your :py:func:`XPLMFeatureEnumerator_f` callback once for each feature that this
    running version of X-Plane supports. Use this routine to determine all of
    the features that X-Plane can support.

    Note the callback is synchronous, so you can use the results immediately.

Feature enumeration example::

 feature_names = []
 XPLMEnumerateFeatures(featureEnumerator, feature_names)
 print("Supported features: {}".format(feature_names))

 def featureEnumerator(name, ref):
    ref.append(name)
