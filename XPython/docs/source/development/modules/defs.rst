XPLMDefs
========
.. py:module:: XPLMDefs

To use:
::

   import XPLMDefs


.. _XPLMPluginID:

XPLMPluginID
------------

 Each plug-in is identified by a unique integer ID.  This ID can be used to
 disable or enable a plug-in, or discover what plug-in is 'running' at the
 time.  A plug-in ID is unique within the currently running instance of
 X-Plane unless plug-ins are reloaded.  Plug-ins may receive a different
 unique ID each time they are loaded.

 For persistent identification of plug-ins, use XPLMFindPluginBySignature in
 XPLMUtiltiies.h

 .. py:data:: XPLM_NO_PLUGIN_ID
    :value: -1

    No plugin (xp.NO_PLUGIN_ID)

 .. py:data:: XPLM_PLUGIN_XPLANE
    :value: 0

    X-Plane itself (xp.PLUGIN_XPLANE)
 
 .. py:data:: kXPLM_Version
    :value: 303

    The current XPLM revision is 303

   


.. _XPLMKeyFlags:

XPLMKeyFlags
------------

These bitfields define modifier keys in a platform independent way. When a
key is pressed, a series of messages are sent to your plugin.  The down
flag is set in the first of these messages, and the up flag in the last.
While the key is held down, messages are sent with neither to indicate that
the key is being held down as a repeated character.

The control flag is mapped to the control flag on Macintosh and PC.
Generally X-Plane uses the control key and not the command key on
Macintosh, providing a consistent interface across platforms that does not
necessarily match the Macintosh user interface guidelines.  There is not
yet a way for plugins to access the Macintosh control keys without using
#ifdefed code.

For ``xp.py``, these are defined without the leading ``xplm_`` e.g., ``xp.ShiftFlag``

 .. py:data:: xplm_ShiftFlag
   :value: 1

   The shift key is down

 .. py:data:: xplm_OptionAltFlag
   :value: 2

   The option or alt key is down

 .. py:data:: xplm_ControlFlag
   :value: 4

   The control key is down

 .. py:data:: xplm_DownFlag
   :value: 8

   The key is being pressed down

 .. py:data:: xplm_UpFlag
   :value: 16

   The key is being released



ASCII Control Key Codes
-----------------------
These definitions define how various control keys are mapped to ASCII key
codes. Not all key presses generate an ASCII value, so plugin code should
be prepared to see null characters come from the keyboard...this usually
represents a key stroke that has no equivalent ASCII, like a page-down
press.  Use virtual key codes to find these key strokes. ASCII key codes
ake into account modifier keys; shift keys will affect capitals and
punctuation; control key combinations may have no vaild ASCII and produce
NULL.  To detect control-key combinations, use virtual key codes, not ASCII
keys.

For ``xp.py``, these are defined without the leading ``XPLM_`` e.g., ``xp.KEY_RETURN``

 .. py:data:: XPLM_KEY_RETURN
    XPLM_KEY_ESCAPE
    XPLM_KEY_TAB
    XPLM_KEY_DELETE
    XPLM_KEY_LEFT
    XPLM_KEY_RIGHT
    XPLM_KEY_UP
    XPLM_KEY_DOWN
    XPLM_KEY_0
    XPLM_KEY_1
    XPLM_KEY_2
    XPLM_KEY_3
    XPLM_KEY_4
    XPLM_KEY_5
    XPLM_KEY_6
    XPLM_KEY_7
    XPLM_KEY_8
    XPLM_KEY_9
    XPLM_KEY_DECIMAL


.. _Virtual Key Codes:

Virtual Key Codes
-----------------

These are cross-platform defines for every distinct keyboard press on the
computer. Every physical key on the keyboard has a virtual key code. So
the "two" key on the top row of the main keyboard has a different code
from the "two" key on the numeric key pad. But the 'w' and 'W' character
are indistinguishable by virtual key code because they are the same
physical key (one with and one without the shift key).

Use virtual key codes to detect keystrokes that do not have ASCII
equivalents, allow the user to map the numeric keypad separately from the
main keyboard, and detect control key and other modifier-key combinations
that generate ASCII control key sequences (many of which are not available
directly via character keys in the SDK).

To assign virtual key codes we started with the Microsoft set but made some
additions and changes. A few differences:

 1. Modifier keys are not available as virtual key codes. You cannot get
    distinct modifier press and release messages. Please do not try to use
    modifier keys as regular keys; doing so will almost certainly interfere
    with users' abilities to use the native x-plane key bindings.

 2. Some keys that do not exist on both Mac and PC keyboards are removed.

 3. Do not assume that the values of these keystrokes are interchangeable
    with MS v-keys.

For ``xp.py``, these are defined without the leading ``XPLM_`` e.g., ``xp.VK_TAB``

 .. py:data:: XPLM_VK_BACK
    XPLM_VK_TAB
    XPLM_VK_CLEAR
    XPLM_VK_RETURN
    XPLM_VK_ESCAPE
    XPLM_VK_SPACE
    XPLM_VK_PRIOR
    XPLM_VK_NEXT
    XPLM_VK_END
    XPLM_VK_HOME
    XPLM_VK_LEFT
    XPLM_VK_UP
    XPLM_VK_RIGHT
    XPLM_VK_DOWN
    XPLM_VK_SELECT
    XPLM_VK_PRINT
    XPLM_VK_EXECUTE
    XPLM_VK_SNAPSHOT
    XPLM_VK_INSERT
    XPLM_VK_DELETE
    XPLM_VK_HELP

XPLM_VK_0 thru XPLM_VK_9 are the same as ASCII '0' thru '9' (= 0x30 - 0x39)

 .. py:data:: XPLM_VK_0
    XPLM_VK_1
    XPLM_VK_2
    XPLM_VK_3
    XPLM_VK_4
    XPLM_VK_5
    XPLM_VK_6
    XPLM_VK_7
    XPLM_VK_8
    XPLM_VK_9

XPLM_VK_A thru XPLM_VK_Z are the same as ASCII 'A' thru 'Z' (= 0x41 - 0x5A)

 .. py:data:: XPLM_VK_A
    XPLM_VK_B
    XPLM_VK_C
    XPLM_VK_D
    XPLM_VK_E
    XPLM_VK_F
    XPLM_VK_G
    XPLM_VK_H
    XPLM_VK_I
    XPLM_VK_J
    XPLM_VK_K
    XPLM_VK_L
    XPLM_VK_M
    XPLM_VK_N
    XPLM_VK_O
    XPLM_VK_P
    XPLM_VK_Q
    XPLM_VK_R
    XPLM_VK_S
    XPLM_VK_T
    XPLM_VK_U
    XPLM_VK_V
    XPLM_VK_W
    XPLM_VK_X
    XPLM_VK_Y
    XPLM_VK_Z
    XPLM_VK_NUMPAD0
    XPLM_VK_NUMPAD1
    XPLM_VK_NUMPAD2
    XPLM_VK_NUMPAD3
    XPLM_VK_NUMPAD4
    XPLM_VK_NUMPAD5
    XPLM_VK_NUMPAD6
    XPLM_VK_NUMPAD7
    XPLM_VK_NUMPAD8
    XPLM_VK_NUMPAD9
    XPLM_VK_MULTIPLY
    XPLM_VK_ADD
    XPLM_VK_SEPARATOR
    XPLM_VK_SUBTRACT
    XPLM_VK_DECIMAL
    XPLM_VK_DIVIDE
    XPLM_VK_F1
    XPLM_VK_F2
    XPLM_VK_F3
    XPLM_VK_F4
    XPLM_VK_F5
    XPLM_VK_F6
    XPLM_VK_F7
    XPLM_VK_F8
    XPLM_VK_F9
    XPLM_VK_F10
    XPLM_VK_F11
    XPLM_VK_F12
    XPLM_VK_F13
    XPLM_VK_F14
    XPLM_VK_F15
    XPLM_VK_F16
    XPLM_VK_F17
    XPLM_VK_F18
    XPLM_VK_F19
    XPLM_VK_F20
    XPLM_VK_F21
    XPLM_VK_F22
    XPLM_VK_F23
    XPLM_VK_F24

The following definitions are extended and are not based on the Microsoft
key set.

 .. py:data:: XPLM_VK_EQUAL
    XPLM_VK_MINUS
    XPLM_VK_RBRACE
    XPLM_VK_LBRACE
    XPLM_VK_QUOTE
    XPLM_VK_SEMICOLON
    XPLM_VK_BACKSLASH
    XPLM_VK_COMMA
    XPLM_VK_SLASH
    XPLM_VK_PERIOD
    XPLM_VK_BACKQUOTE
    XPLM_VK_ENTER
    XPLM_VK_NUMPAD_ENT
    XPLM_VK_NUMPAD_EQ
