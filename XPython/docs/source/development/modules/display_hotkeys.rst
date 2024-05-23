Hot Keys
========
.. py:module:: XPLMDisplay
   :no-index:
      
.. py:currentmodule:: xp

To use::

  import xp

Keystrokes that can be managed by others. These are lower-level than window keyboard handlers.

If you have a sniffer and a hot key, the sniffer is called first (even if it is an "after" sniffer)
and if it consumes the key, the hot key will not be called.

.. py:function:: registerHotKey(vKey, flags, description="", hotKey, refCon=None)

 Register a hotkey.

 *vKey* (:ref:`Virtual Key Codes`) is the hot key to be pressed to activate (this may be changed later by your
 plugin, or some other plugin, using :py:func:`setHotKeyCombination`).

 *flags* are bitwise OR'd values for Shift / Ctrl to be pressed with the hot key. **Note** you need to include
 ``xp.DownFlag`` or ``xp.UpFlag``.
 (:ref:`XPLMKeyFlags`)

 Include a *description* for the hot key, so others (using :py:func:`getHotKeyInfo`) can understand
 the intent of your hot key.
 
 Your *hotKey* callback receives only the *refCon*.

 Registration returns a hotKeyID, which is what you'll use with :py:func:`unRegisterHotKey`.

 During execution, the actual key associated with your hot key
 may change, but you are insulated from this.

 `Official SDK <https://developer.x-plane.com/sdk/XPLMDisplay/#XPLMRegisterHotKey>`__ :index:`XPLMRegisterHotKey`

 >>> def MyHotKey(refCon):
 ...     xp.speakString("You pressed the Hot Key")
 ...
 >>> hotKeyID = xp.registerHotKey(xp.VK_Z, xp.DownFlag, "Speak Hotkey Example", MyHotKey)
 >>>
 >>> xp.unregisterHotKey(hotKeyID)

.. py:function:: unregisterHotKey(hotKeyID)

 Unregister a hotkey using *hotKeyID* you received using :py:func:`registerHotKey`.

 Only your own hotkeys can be unregistered (even though you can get hotKeyIDs of
 other Hot Keys using :py:func:`getNthHotKey`.)

 `Official SDK <https://developer.x-plane.com/sdk/XPLMDisplay/#XPLMUnregisterHotKey>`__ :index:`XPLMUnregisterHotKey`

.. py:function:: countHotKeys()

 Return number of hotkeys defined in the whole sim -- not just those you defined..

 `Official SDK <https://developer.x-plane.com/sdk/XPLMDisplay/#XPLMUnregisterHotKey>`__ :index:`XPLMUnregisterHotKey`

.. py:function:: getNthHotKey(index)

 Returns HotKeyID of Nth hotkey (0-based indexing).

 >>> xp.countHotKeys()
 1
 >>> hotKeyID = xp.getNthHotKey(0)

 `Official SDK <https://developer.x-plane.com/sdk/XPLMDisplay/#XPLMGetNthHotKey>`__ :index:`XPLMGetNthHotKey`

.. py:function:: getHotKeyInfo(hotKeyID)

 Return information about the hotkey as an object with attributes.

   | description: str
   | virtualKey:  int (:ref:`Virtual Key Codes`)
   | flags: int (:ref:`XPLMKeyFlags`)
   | plugin: int (:ref:`XPLMPluginID`)

 >>> i.description
 "Speak Hotkey Example"
 >>> i.virtualKey
 90
 >>> i.flags
 0
 >>> i.plugin
 3

 `Official SDK <https://developer.x-plane.com/sdk/XPLMDisplay/#XPLMGetHotKeyInfo>`__ :index:`XPLMGetHotKeyInfo`

.. note::  **All** python-based hotkeys report the XPPython3 plugin ID: there is a convoluted way
 (from python) to determine which python plugin created a hotkey, but non-python
 plugins will always see all python hot keys as originating with the XPPython3 plugin. 

.. py:function:: setHotKeyCombination(hotKeyID, vKey, flags)

 Remap a hot key's keystroke.

 *hotKeyID* can be either one returned from :py:func:`registerHotKey`, or
 found using :py:func:`getNthHotKey`.

 Set *vKey* and *flags* as you would with :py:func:`registerHotKey`.

 You may remap another plugin's keystrokes. For example, to change the first hot key
 to the same key, but requiring a Shift:

 >>> hotKeyID = xp.getNthHotKey(0)
 >>> info = xp.getHotKeyInfo(hotKeyID)
 >>> xp.setHotKeyCombination(hotKeyID, info.virtualKey, flags=info.flags | xp.ShiftFlag)
 

 `Official SDK <https://developer.x-plane.com/sdk/XPLMDisplay/#XPLMGetHotKeyInfo>`__ :index:`XPLMGetHotKeyInfo`

