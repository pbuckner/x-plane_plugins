Key Sniffing
============
.. py:module:: XPLMDisplay
   :no-index:
.. py:currentmodule:: xp
                      
To use::

  import xp
  
Key sniffing provides access to low level access to the keystroke stream. Install a key
sniffer with :py:func:`registerKeySniffer`. Key sniffers can be installed above everything or
right in front of the sim.

.. py:function:: registerKeySniffer(sniffer, before=0, refCon=None)

 :param Callable sniffer: function to call on (every) keypress.
 :param int before: get the keystroke *before* X-Plane, or after X-Plane has processed it.
 :param Any refCon: passed to your function.
 :return: 1 on success

 This routine registers a key *sniffer* callback. You specify whether you want to sniff before
 the window system (*before*\=1), or only sniff keys the window system does not consume (*before*\=0).
 You should ALMOST ALWAYS sniff non-control keys after the window system. When the window
 system consumes a key, it is because the user has “focused” a window. Consuming the key or taking
 action based on the key will produce very weird results.

 A window-based UI should not use this!
 The windowing system provides high-level mediated keyboard access, via the callbacks you attach
 on window creation :py:func:`createWindowEx`. By comparison, the key sniffer provides low level keyboard access.

 Key sniffers are provided to allow libraries to provide non-windowed user interaction. For example,
 the MUI library uses a key sniffer to do pop-up text entry.

 Returns 1 if successful.

 Your *sniffer* callback takes four parameters (key, flags, vKey, refCon). *key* is the key code, this
 is OS dependent. In most cases, you should use the *vKey* (:ref:`Virtual Key Codes`) which, in combination
 with *flags* (:ref:`XPLMKeyFlags`), will indicate which key was pressed and if that included
 Shirt, Control, etc.
 
 Have your *sniffer* callback return 1 to pass the key on to the next sniffer,
 the window manager, X-Plane, or whomever is down stream. Return 0 to consume the key.

 >>> def MySniffer(key, flags, vKey, refCon):
 ...     if vKey == xp.VK_Z and flags & xp.ShiftFlag:
 ...         xp.speakString("You pressed Z")
 ...         return 0
 ...     return 1
 ...
 >>> xp.registerKeySniffer(MySniffer)
 1
 >>> xp.unregisterKeySniffer(MySniffer)
 1

 `Official SDK <https://developer.x-plane.com/sdk/XPLMDisplay/#XPLMRegisterKeySniffer>`__ :index:`XPLMRegisterKeySniffer`

.. py:function:: unregisterKeySniffer(sniffer, before=0, refCon=None)

 :param Callable sniffer: function to call on (every) keypress.
 :param int before: get the keystroke *before* X-Plane, or after X-Plane has processed it.
 :param Any refCon: passed to your function.
 :return: 1 on success, -1 if not found

 This routine unregisters a key sniffer. You must unregister a key sniffer for every time you register
 one with the exact same signature. Returns 1 if successful, -1 if registered sniffer cannot be found.

 `Official SDK <https://developer.x-plane.com/sdk/XPLMDisplay/#XPLMUnregisterKeySniffer>`__ :index:`XPLMUnregisterKeySniffer`

