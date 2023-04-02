XPLMPlugin
==========
.. py:module:: XPLMPlugin
.. py:currentmodule:: xp
                      
To use::
  
  import xp

Finding Plugins
---------------

These APIs allow you to find another plugin or yourself, or iterate across
all plugins.  For example, if you wrote an FMS plugin that needed to talk
to an autopilot plugin, you could use these APIs to locate the autopilot
plugin.

.. note:: These interfaces are used to communicate with non-XPPython3 plugins.

.. py:function:: getMyID()

 Return the integer plugin ID of the calling plugin.  Call this to
 get your own ID.

 .. note::

    This is the plugin ID of the XPPython3 plugin because, from the
    perspective of X-Plane, all python plugins are running within
    a single plugin.

 >>> xp.getMyID()
 3

 `Official SDK <https://developer.x-plane.com/sdk/XPLMPlugin/#XPLMGetMyID>`__ :index:`XPLMGetMyID`


.. py:function::  countPlugins()

 Return the total number of (non-python) plugins that are loaded, both
 disabled and enabled. This includes this ``XPPython3`` plugin (which is written in C)
 but none of the python plugins.

 >>> xp.countPlugins()
 5

 `Official SDK <https://developer.x-plane.com/sdk/XPLMPlugin/#XPLMCountPlugins>`__ :index:`XPLMCountPlugins`
 
.. py:function:: getNthPlugin(index)

 Return the ID of a (non-python) plugin by *index*.  Index is 0 based from 0
 to :py:func:`countPlugins`-1, inclusive. Plugins may be returned in any arbitrary
 order.

 >>> xp.getNthPlugin(0)
 0

 `Official SDK <https://developer.x-plane.com/sdk/XPLMPlugin/#XPLMGetNthPlugin>`__ :index:`XPLMGetNthPlugin`

.. py:function::  findPluginByPath(path)

 Returns the pluginID of the plugin whose file exists at the provided *path*.
 Path **must** be an absolute path.

 XPLM_NO_PLUGIN_ID (``-1``) is returned if the
 path does not point to a currently loaded plug-in.

 >>> xp.findPluginByPath('XPPython3.xpl')
 -1
 >>> xp.findPluginByPath('Resources/plugins/XPPython3/mac_x64/XPPython3.xpl')
 -1
 >>> xp.findPluginByPath('/Volumes/Red1/X-Plane/Resources/plugins/XPPython3/mac_x64/XPPython3.xpl')
 3

 `Official SDK <https://developer.x-plane.com/sdk/XPLMPlugin/#XPLMFindPluginByPath>`__ :index:`XPLMFindPluginByPath`
 
.. py:function:: findPluginBySignature(signature)

 Return the  pluginID of the (non-python) plugin whose signature matches *signature*, or
 :py:data:`XPLM_NO_PLUGIN_ID` if no running plug-in has this
 signature.

 Signatures are the best way to identify another plug-in as they
 are independent of the file system path of a plugin or the human-readable
 plugin name, and should be unique for all plug-ins.  Use this routine to
 locate another plugin that your plugin inter-operates with.

 >>> xp.findPluginBySignature('com.x-plane.xlua.1.0.0r1')
 4
 >>> xp.findPluginBySignature('xppython.main')
 3
 >>> xp.findPluginBySignature('xplanesdk.examples.DataRefEditor')
 -1

 `Official SDK <https://developer.x-plane.com/sdk/XPLMPlugin/#XPLMFindPluginBySignature>`__ :index:`XPLMFindPluginBySignature`
 
.. py:function:: getPluginInfo(pluginID)

 Return information about a plugin given its *pluginID* (Not its index!).

     .. py:data:: PluginInfo
    
     Object returned by :py:func:`getPluginInfo` containing
     information about a plugin. It has the following string attributes:
    
     | .name
     | .filePath
     | .signature
     | .description
    
 Conveniently, we also provide a ``str()`` representation for the information
 
 >>> info = xp.getPluginInfo(3)
 >>> info
 <xppython3.PluginInfo object at 0x75d67040f340>
 >>> info.signature
 'xppython3.main'
 >>> print(info)
 XPPython3: 'xppython3.main'
   /Volumes/Red1/X-Plane/Resources/plugins/XPPython3/mac_x64/XPPython3.xpl
   ---
   X-Plane interface for Python3              

 `Official SDK <https://developer.x-plane.com/sdk/XPLMPlugin/#XPLMGetPluginInfo>`__ :index:`XPLMGetPluginInfo`


.. py:function::  isPluginEnabled(pluginID)

 Return 1 if specified *pluginID* is enabled, 0 otherwise.

 >>> xp.isPluginEnabled(3)
 1
 >>> xp.isPluginEnabled(1000)
 0

 `Official SDK <https://developer.x-plane.com/sdk/XPLMPlugin/#XPLMIsPluginEnabled>`__ :index:`XPLMIsPluginEnabled`


.. py:function:: enablePlugin(pluginID)

 Enable (non-python) *pluginID*. Return 1 if already enabled, or was able to be enabled,
 0 otherwise.

 Plugins may fail to enable (for example, if resources cannot be acquired)
 by returning 0 from their ``XPluginEnable`` callback.

 >>> xp.enablePlugin(3)
 1

 `Official SDK <https://developer.x-plane.com/sdk/XPLMPlugin/#XPLMEnablePlugin>`__ :index:`XPLMEnablePlugin`
 
.. py:function:: disablePlugin(pluginID)

 This routine disables (non-python) *pluginID*. No error is returned.

 >>> xp.disablePlugin(4)
 >>> xp.disablePlugin(10000)

 `Official SDK <https://developer.x-plane.com/sdk/XPLMPlugin/#XPLMDisablePlugin>`__ :index:`XPLMDisablePlugin`
 
.. py:function:: reloadPlugins()

 .. warning:: :py:func:`reloadPlugins` triggers a python bug in Python 3.9.8, which causes the sim to crash.
        The bug has been fixed in Python 3.9.9 and 3.10.
    
 This routine reloads **all** plugins.  Once this routine is called and you
 return from the callback you were within (e.g. a menu select callback) you
 will receive your ``XPluginDisable`` and ``XPluginStop`` callbacks,
 then the start process happens as if the sim was starting up.

 .. note:: This reloads **all plugins** not just XPPython3 plugins.

 .. warning:: Many (most?) plugins don't clean up after themselves, so reloading
    may result in an unstable state.

    This includes plugins which use modules which include shared libraries
    such as ``numpy`` and ``OpenGL``:
    These libraries don't always reload correctly, and may cause an immediate crash.

    They work great on initial import, and it's okay to have multiple plugins import
    the same modules -- *reload* is the problem.

 >>> xp.reloadPlugins()
    
 `Official SDK <https://developer.x-plane.com/sdk/XPLMPlugin/#XPLMReloadPlugins>`__ :index:`XPLMReloadPlugins`
 
.. _Inter-plugin Messaging:

Inter-plugin Messaging
----------------------

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

You receive messages as calls to your plugin's
``PythonInterface.XPluginReceiveMessage(inFromWho, inMessage, inParam)`` method.

 | **inFromWho**: integer plugin ID (or 0)
 | **inMessage**: integer message ID, one of those below, or a plugin-defined notification
 | **inParam**: integer or string, but its meaning (and use) depends on the particular message, described below.

Common use for these messages is to detect when the user has changed aircraft (MSG_PLANE_LOADED) or location (MSG_AIRPORT_LOADED),
or entered/existed VR (MSG_ENTERED_VR, MSG_EXITED_VR).

The following messages are sent to your plugin by X-Plane.

On startup, you'll see::

  106 PLANE_UNLOADED  (even though no plane has been loaded yet)
  102 PLANE_LOADED (for user aircraft)
  108 LIVERY_LOADED (for user aircraft)
  104 SCENERY_LOADED
  103 AIRPORT_LOADED
  102 PLANE_LOADED (AI aircraft)
  108 LIVERY_LOADED (AI aircraft)


.. py:data:: MSG_PLANE_CRASHED
 :value: 101

 This message is sent to your plugin whenever the user's plane crashes.

.. py:data:: MSG_PLANE_LOADED
 :value: 102

 This message is sent to your plugin whenever a new plane is loaded. The
 parameter is the number of the plane being loaded; 0 indicates the user's
 plane.

.. py:data:: MSG_AIRPORT_LOADED
 :value: 103

 This messages is called whenever the user's plane is positioned at a new
 airport.
  
.. py:data:: MSG_SCENERY_LOADED
 :value: 104

 This message is sent whenever new scenery is loaded.  (Laminar documentation
 says, "Use datarefs to determine the new scenery files that were loaded." But I've
 not found any datarefs to help with that.)

.. py:data:: MSG_AIRPLANE_COUNT_CHANGED
 :value: 105

 This message is sent whenever the user adjusts the number of X-Plane
 aircraft models. You must use :py:func:`countPlanes` to find out how many planes
 are now available. This message will only be sent in XP7 and higher
 because in XP6 the number of aircraft is not user-adjustable.

.. py:data:: MSG_PLANE_UNLOADED
 :value: 106

 This message is sent to your plugin whenever a plane is unloaded. The
 parameter is the number of the plane being unloaded; 0 indicates the user's
 plane. The parameter is of type int, passed as the value of the pointer.
 (That is: the parameter is an int, not a pointer to an int.)

.. py:data:: MSG_WILL_WRITE_PREFS
 :value: 107

 This message is sent to your plugin right before X-Plane writes its
 preferences file. You can use this for two purposes: to write your own
 preferences, and to modify any datarefs to influence preferences output.
 For example, if your plugin temporarily modifies saved preferences, you can
 put them back to their default values here to avoid having the tweaks be
 persisted if your plugin is not loaded on the next invocation of X-Plane.

.. py:data:: MSG_LIVERY_LOADED
 :value: 108

 This message is sent to your plugin right after a livery is loaded for an
 airplane. You can use this to check the new livery (via datarefs) and
 react accordingly. The parameter is of type int, passed as the value of a
 pointer and represents the aircraft plane number - 0 is the user's plane.

.. py:data:: MSG_ENTERED_VR
 :value: 109

 Sent to your plugin right before X-Plane enters virtual reality mode (at
 which time any windows that are not positioned in VR mode will no longer be
 visible to the user).

.. py:data:: MSG_EXITING_VR
 :value: 110

 Sent to your plugin right before X-Plane leaves virtual reality mode (at
 which time you may want to clean up windows that are positioned in VR
 mode).

.. py:data:: MSG_RELEASE_PLANES
 :value: 111

 Sent to your plugin if another plugin wants to take over AI planes. If you
 are a synthetic traffic provider,  that probably means a plugin for an
 online network has connected and wants to supply aircraft flown by real
 humans and you should cease to provide synthetic traffic. If however you
 are providing online traffic from real humans,  you probably don't want to
 disconnect, in which case you just ignore this message. The sender is the
 plugin ID of the plugin asking for control of the planes now. You can use
 it to find out who is requesting and whether you should yield to them.
 Synthetic traffic providers should always yield to online networks. The
 parameter is unused and should be ignored.

.. py:data:: MSG_FMOD_BANK_LOADED
  :value: 112

  Sent to your plugin after FMOD sound banks are loaded. The parameter is the
  XPLMBankID enum in XPLMSound.h, 0 for the master bank, and 1 for the radio bank.

.. py:data:: MSG_FMOD_BANK_UNLOADING
  :value: 113

  Sent to your plugin before FMOD banks are unloaded. Any associated resources
  should be cleaned up at this point. The parameter is the XPLMBankID enum
  in XPLMSound.h, 0 for the master bank, and 1 for the radio bank.

.. py:data:: MSG_DATAREFS_ADDED
  :value: 114             

  Sent to your plugin per-frame (at most) when/if datarefs are added. It will include the new data ref
  total count so that your plugin can keep a local cache of the total, see what's
  changed and know which ones to inquire about if it cares. Normally this
  is enabled/disabled through the use of the ``XPLM_WANTS_DATAREF_NOTIFICATIONS`` feature,
  but as XPPython3 requires this to be enabled, you'll always get these messages.

  .. Warning:: This appears to be broken in 12.04r3. You'll get the message, but the value
               of ``param`` is not the new count of dataRefs: it appears to be a pointer to the
               value, rather than the value itself. Bug XPD-13931
               filed with Laminar 11-March-2023.

  .. Note:: To work around XPD-13931, we'll automatically convert the pointer value to the
            actual count, the the value you receive with ``param`` is the count of the
            current dataRefs.
  
.. py:function:: sendMessageToPlugin(pluginID, message, param=None)

 Send plugin *message* (as opposed to, say, a Widget message) to another *pluginID*.  Pass
 :data:`NO_PLUGIN_ID` to broadcast to all plug-ins.  Messages sent
 to the XPPython3 plugin will be forward to **all** python plugins. Only enabled plugins with
 a message receive function receive the message.

 Plugins can define their own *message* values.

 *param* is message-dependent and should be None, a string, or an integer.

 For example, DataRefEditor plugin supports a message `0x1000000`, with
 a string parameter, allowing you to notify it of a dataRef your plugin creates. (See also :doc:`dataaccess`.)
 
 >>> dre = xp.findPluginBySignature('xplanesdk.examples.DataRefEditor')
 >>> xp.sendMessageToPlugin(dre, 0x01000000, 'myplugin/dataRef1')

 `Official SDK <https://developer.x-plane.com/sdk/XPLMPlugin/#XPLMSendMessageToPlugin>`__ :index:`XPLMSendMessageToPlugin`

Plugin Features API
-------------------

The plugin features API allows your plugin to "sign up" for additional
capabilities and plugin system features that are normally disabled for
backward compatibility.  This allows advanced plugins to "opt-in" to new
behavior.

Each feature is defined by a permanent string name.  The feature string
names will vary with the particular  installation of X-Plane, so plugins
should not expect a feature to be guaranteed present.

.. py:function:: hasFeature(feature)

 This returns 1 if the given installation of X-Plane supports a feature, or
 0 if it does not. Note that *feature* is a string, not an enumeration.

 See
 `Official SDK <https://developer.x-plane.com/sdk/XPLMPlugin/#Plugin%20Features%20API>`__ :index:`Plugin Features API`
 for X-Plane features.

 >>> xp.hasFeature('XPLM_WANTS_REFLECTIONS')
 1
 
 `Official SDK <https://developer.x-plane.com/sdk/XPLMPlugin/#XPLMHasFeature>`__ :index:`XPLMHasFeature`
 
.. py:function:: isFeatureEnabled(feature)

 This returns 1 if a (string) feature is currently enabled for your plugin, or 0 if
 it is not enabled (or feature does not exist).

 >>> xp.isFeatureEnabled('XPLM_USE_NATIVE_PATHS')
 1
 >>> xp.isFeatureEnabled('INCREASE_FPS_MAGICALLY')
 0

 `Official SDK <https://developer.x-plane.com/sdk/XPLMPlugin/#XPLMIsFeatureEnabled>`__ :index:`XPLMIsFeatureEnabled`

.. py:function::  enableFeature(feature, enable=1)

 This routine enables or disables a *feature* for your plugin.  This will
 change the running behavior of X-Plane and your plugin in some way,
 depending on the feature.

 .. note:: Because this is per-C plugin, not per-python plugin, some
           features have been hard-coded such that you cannot change their
           value. Changing for *your* python plugin would change it for *all*
           python plugins. A runtime exception will be raised.

           This is a conscious limitation of the XPPython3 plugin.

 >>> xp.enableFeature('XPLM_USE_NATIVE_PATHS')
 1
 >>> xp.enableFeature('XPLM_USE_NATIVE_PATHS', enable=0)
 RuntimeError: An XPPython3 plugin is attempting to disable XPLM_USE_NATIVE_PATHS or XPLM_USER_NATIVE_WIDGET_WINDOWS feature, not allowed

 `Official SDK <https://developer.x-plane.com/sdk/XPLMPlugin/#XPLMEnableFeature>`__ :index:`XPLMEnableFeature`

.. py:function:: enumerateFeatures(enumerator, refCon)

  This routine immediately calls your *enumerator* callback once for each feature that this
  running version of X-Plane supports. Use this routine to determine all of
  the features that X-Plane can support.

  Note the callback is synchronous, so you can use the results immediately.

  For example:

  >>> feature_names = []
  >>> def enumerator(name, refCon):
  ...    refCon.append(name)
  ...
  >>> xp.enumerateFeatures(enumerator, refCon=feature_names)
  >>> print(f"Supported Features: {feature_names}")
  Supported Features: ['XPLM_WANTS_REFLECTIONS', 'XPLM_USE_NATIVE_PATHS', 'XPLM_USE_NATIVE_WIDGET_WINDOWS']

  `Official SDK <https://developer.x-plane.com/sdk/XPLMPlugin/#XPLMEnumerateFeatures>`__ :index:`XPLMEnumerateFeatures`

    
Constants
---------

.. py:data:: XPLMPluginID

   Integer plugin ID.

   `Official SDK <https://developer.x-plane.com/sdk/XPLMPlugin/#XPLMPluginID>`__ :index:`XPLMPluginID`
