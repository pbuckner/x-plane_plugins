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

 :param int vKey: one of :ref:`Virtual Key Codes`
 :param int flags: bitwise OR of :ref:`XPLMKeyFlags`
 :param str description: text descript of your key, viewable by :func:`getHotKeyInfo`.
 :param Callable hotKey: Function called on keypress.
 :param Any refCon: Reference constant passed to your hotKey function
 :return: XPLMHotKeyID capsule

 Register a hotkey.

 *vKey* (:ref:`Virtual Key Codes`) is the hot key to be pressed to activate (this may be changed later by your
 plugin, or some other plugin, using :py:func:`setHotKeyCombination`).

 *flags* are bitwise OR'd values for Shift / Ctrl to be pressed with the hot key. **Note** you need to include
 ``xp.DownFlag`` or ``xp.UpFlag``.
 (:ref:`XPLMKeyFlags`)

 Include a *description* for the hot key, so others (using :py:func:`getHotKeyInfo`) can understand
 the intent of your hot key.
 
 Your *hotKey* callback receives only the *refCon*.

 Registration returns a hotKeyID, which is what you'll use with :py:func:`unregisterHotKey`.

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

 :param XPLMHotKeyID hotKeyID: value received from :py:func:`registerHotKey`.
 :return: None

 Unregister a hotkey. Raises RuntimeError if hotkey is not registered/found.

 Only your own hotkeys can be unregistered (even though you can get hotKeyIDs of
 other Hot Keys using :py:func:`getNthHotKey`.)

 `Official SDK <https://developer.x-plane.com/sdk/XPLMDisplay/#XPLMUnregisterHotKey>`__ :index:`XPLMUnregisterHotKey`

.. py:function:: countHotKeys()

 :return: integer

 Return number of hotkeys defined in the whole sim -- not just those you defined. When a hot key is unregistered,
 it will reduce the count.

 `Official SDK <https://developer.x-plane.com/sdk/XPLMDisplay/#XPLMUnregisterHotKey>`__ :index:`XPLMUnregisterHotKey`

.. py:function:: getNthHotKey(index)

 :param int index: 0-based index                 
 :return: XPLMHotKeyID capsule of *nth* hotkey in the whole sim.

 Returns HotKeyID of Nth hotkey (0-based indexing).

 >>> xp.countHotKeys()
 1
 >>> hotKeyID = xp.getNthHotKey(0)

 `Official SDK <https://developer.x-plane.com/sdk/XPLMDisplay/#XPLMGetNthHotKey>`__ :index:`XPLMGetNthHotKey`

.. py:function:: getHotKeyInfo(hotKeyID)

 :param XPLMHotKeyID hotKeyID: HotKey to look up.
 :return: HotKeyInfo instance, or ValueError if not found.   

 Return information about the hotkey as a HotKeyInfo object with attributes:

   | description: str
   | virtualKey:  int (:ref:`Virtual Key Codes`)
   | flags: int (:ref:`XPLMKeyFlags`)
   | plugin: int (:ref:`XPLMPluginID`)

 >>> info = xp.getHotKeyInfo(xp.getNthHotKey(0))
 >>> info.description
 "Speak Hotkey Example"
 >>> info.virtualKey
 90
 >>> info.flags
 0
 >>> info.plugin
 3

 `Official SDK <https://developer.x-plane.com/sdk/XPLMDisplay/#XPLMGetHotKeyInfo>`__ :index:`XPLMGetHotKeyInfo`

.. note::  **All** python-based hotkeys report the XPPython3 plugin ID: there is a convoluted way
 (from python) to determine which python plugin created a hotkey, but non-python
 plugins will always see all python hot keys as originating with the XPPython3 plugin. 

.. py:function:: setHotKeyCombination(hotKeyID, vKey, flags)

 :param XPLMHotKeyID hotKeyID: hot key to change
 :param int vKey:
 :param inf flags: new key and flags to replace original definition from :func:`registerHotKey`

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

