XPLMDefs
========
.. py:module:: XPLMDefs
.. py:currentmodule:: xp

To use:
::

   import xp


.. _XPLMPluginID:

XPLMPluginID
------------

 Each plug-in is identified by a unique integer ID.  This ID can be used to
 disable or enable a plug-in, or discover what plug-in is 'running' at the
 time.  A plug-in ID is unique within the currently running instance of
 X-Plane unless plug-ins are reloaded.  Plug-ins may receive a different
 unique ID each time they are loaded.

 For persistent identification of plug-ins, use :py:func:`findPluginBySignature`.

 .. py:data:: NO_PLUGIN_ID
    :value: -1

    No plugin.
    `Official SDK <https://developer.x-plane.com/sdk/XPLMDefs/#XPLM_NO_PLUGIN_ID>`__: :index:`XPLM_NO_PLUGIN_ID`

 .. py:data:: PLUGIN_XPLANE
    :value: 0

    X-Plane itself.      `Official SDK <https://developer.x-plane.com/sdk/XPLMDefs/#XPLM_PLUGIN_XPLANE>`__: :index:`XPLM_PLUGIN_XPLANE`
 
 .. py:data:: kVersion
    :value: 400

    The current XPLM SDK revision for X-Plane 11.55 is 303. X-Plane 12 is 400, X-Plane 12.3 is 421.
    `Official SDK <https://developer.x-plane.com/sdk/XPLMDefs/#kXPLM_Version>`__: :index:`kXPLM_Version`


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

 .. py:data:: ShiftFlag
   :value: 1

   The shift key is down.       `Official SDK <https://developer.x-plane.com/sdk/XPLMDefs/#xplm_ShiftFlag>`__: :index:`xplm_ShiftFlag`

 .. py:data:: OptionAltFlag
   :value: 2

   The option or alt key is down.       `Official SDK <https://developer.x-plane.com/sdk/XPLMDefs/#xplm_OptionAltFlag>`__: :index:`xplm_OptionAltFlag`

 .. py:data:: ControlFlag
   :value: 4

   The control key is down. `Official SDK <https://developer.x-plane.com/sdk/XPLMDefs/#xplm_ControlFlag>`__: :index:`xplm_ControlFlag`

 .. py:data:: DownFlag
   :value: 8

   The key is being pressed down. `Official SDK <https://developer.x-plane.com/sdk/XPLMDefs/#xplm_DownFlag>`__: :index:`xplm_DownFlag`

 .. py:data:: UpFlag
   :value: 16

   The key is being released. `Official SDK <https://developer.x-plane.com/sdk/XPLMDefs/#xplm_UpFlag>`__: :index:`xplm_UpFlag`



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

.. table::
 :align: left

 ========================= ========================
 Value                     SDK Value
 ========================= ========================
 .. py:data:: KEY_RETURN   :index:`XPLM_KEY_RETURN`
 .. py:data:: KEY_ESCAPE   :index:`XPLM_KEY_ESCAPE`
 .. py:data:: KEY_TAB      :index:`XPLM_KEY_TAB`
 .. py:data:: KEY_DELETE   :index:`XPLM_KEY_DELETE`
 .. py:data:: KEY_LEFT     :index:`XPLM_KEY_LEFT`
 .. py:data:: KEY_RIGHT    :index:`XPLM_KEY_RIGHT`
 .. py:data:: KEY_UP       :index:`XPLM_KEY_UP`
 .. py:data:: KEY_DOWN     :index:`XPLM_KEY_DOWN`
 .. py:data:: KEY_0        :index:`XPLM_KEY_0`
 .. py:data:: KEY_1        :index:`XPLM_KEY_1`
 .. py:data:: KEY_2        :index:`XPLM_KEY_2`
 .. py:data:: KEY_3        :index:`XPLM_KEY_3`
 .. py:data:: KEY_4        :index:`XPLM_KEY_4`
 .. py:data:: KEY_5        :index:`XPLM_KEY_5`
 .. py:data:: KEY_6        :index:`XPLM_KEY_6`
 .. py:data:: KEY_7        :index:`XPLM_KEY_7`
 .. py:data:: KEY_8        :index:`XPLM_KEY_8`
 .. py:data:: KEY_9        :index:`XPLM_KEY_9`
 .. py:data:: KEY_DECIMAL  :index:`XPLM_KEY_DECIMAL`
 ========================= ========================


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

.. table::
 :align: left

 ========================= =============================
 Value                     SDK Value
 ========================= =============================
 .. py:data:: VK_BACK      :index:`XPLM_VK_BACK`
 .. py:data:: VK_TAB       :index:`XPLM_VK_TAB`
 .. py:data:: VK_CLEAR     :index:`XPLM_VK_CLEAR`
 .. py:data:: VK_RETURN    :index:`XPLM_VK_RETURN`
 .. py:data:: VK_ESCAPE    :index:`XPLM_VK_ESCAPE`
 .. py:data:: VK_SPACE     :index:`XPLM_VK_SPACE`
 .. py:data:: VK_PRIOR     :index:`XPLM_VK_PRIOR`
 .. py:data:: VK_NEXT      :index:`XPLM_VK_NEXT`
 .. py:data:: VK_END       :index:`XPLM_VK_END`
 .. py:data:: VK_HOME      :index:`XPLM_VK_HOME`
 .. py:data:: VK_LEFT      :index:`XPLM_VK_LEFT`
 .. py:data:: VK_UP        :index:`XPLM_VK_UP`
 .. py:data:: VK_RIGHT     :index:`XPLM_VK_RIGHT`
 .. py:data:: VK_DOWN      :index:`XPLM_VK_DOWN`
 .. py:data:: VK_SELECT    :index:`XPLM_VK_SELECT`
 .. py:data:: VK_PRINT     :index:`XPLM_VK_PRINT`
 .. py:data:: VK_EXECUTE   :index:`XPLM_VK_EXECUTE`
 .. py:data:: VK_SNAPSHOT  :index:`XPLM_VK_SNAPSHOT`
 .. py:data:: VK_INSERT    :index:`XPLM_VK_INSERT`
 .. py:data:: VK_DELETE    :index:`XPLM_VK_DELETE`
 .. py:data:: VK_HELP      :index:`XPLM_VK_HELP`
 ========================= =============================

XPLM_VK_0 thru XPLM_VK_9 are the same as ASCII '0' thru '9' (= 0x30 - 0x39)

.. table::
 :align: left

 ================== ======================
 Value              SDK Value
 ================== ======================
 .. py:data:: VK_0      :index:`XPLM_VK_0`
 .. py:data:: VK_1      :index:`XPLM_VK_1`
 .. py:data:: VK_2      :index:`XPLM_VK_2`
 .. py:data:: VK_3      :index:`XPLM_VK_3`
 .. py:data:: VK_4      :index:`XPLM_VK_4`
 .. py:data:: VK_5      :index:`XPLM_VK_5`
 .. py:data:: VK_6      :index:`XPLM_VK_6`
 .. py:data:: VK_7      :index:`XPLM_VK_7`
 .. py:data:: VK_8      :index:`XPLM_VK_8`
 .. py:data:: VK_9      :index:`XPLM_VK_9`
 ================== ======================

XPLM_VK_A thru XPLM_VK_Z are the same as ASCII 'A' thru 'Z' (= 0x41 - 0x5A)

.. table::
 :align: left

 ========================= ===========================
 Value                     SDK Value
 ========================= ===========================
 .. py:data:: VK_A         :index:`XPLM_VK_A`
 .. py:data:: VK_B         :index:`XPLM_VK_B`
 .. py:data:: VK_C         :index:`XPLM_VK_C`
 .. py:data:: VK_D         :index:`XPLM_VK_D`
 .. py:data:: VK_E         :index:`XPLM_VK_E`
 .. py:data:: VK_F         :index:`XPLM_VK_F`
 .. py:data:: VK_G         :index:`XPLM_VK_G`
 .. py:data:: VK_H         :index:`XPLM_VK_H`
 .. py:data:: VK_I         :index:`XPLM_VK_I`
 .. py:data:: VK_J         :index:`XPLM_VK_J`
 .. py:data:: VK_K         :index:`XPLM_VK_K`
 .. py:data:: VK_L         :index:`XPLM_VK_L`
 .. py:data:: VK_M         :index:`XPLM_VK_M`
 .. py:data:: VK_N         :index:`XPLM_VK_N`
 .. py:data:: VK_O         :index:`XPLM_VK_O`
 .. py:data:: VK_P         :index:`XPLM_VK_P`
 .. py:data:: VK_Q         :index:`XPLM_VK_Q`
 .. py:data:: VK_R         :index:`XPLM_VK_R`
 .. py:data:: VK_S         :index:`XPLM_VK_S`
 .. py:data:: VK_T         :index:`XPLM_VK_T`
 .. py:data:: VK_U         :index:`XPLM_VK_U`
 .. py:data:: VK_V         :index:`XPLM_VK_V`
 .. py:data:: VK_W         :index:`XPLM_VK_W`
 .. py:data:: VK_X         :index:`XPLM_VK_X`
 .. py:data:: VK_Y         :index:`XPLM_VK_Y`
 .. py:data:: VK_Z         :index:`XPLM_VK_Z`
 .. py:data:: VK_NUMPAD0   :index:`XPLM_VK_NUMPAD0`
 .. py:data:: VK_NUMPAD1   :index:`XPLM_VK_NUMPAD1`
 .. py:data:: VK_NUMPAD2   :index:`XPLM_VK_NUMPAD2`
 .. py:data:: VK_NUMPAD3   :index:`XPLM_VK_NUMPAD3`
 .. py:data:: VK_NUMPAD4   :index:`XPLM_VK_NUMPAD4`
 .. py:data:: VK_NUMPAD5   :index:`XPLM_VK_NUMPAD5`
 .. py:data:: VK_NUMPAD6   :index:`XPLM_VK_NUMPAD6`
 .. py:data:: VK_NUMPAD7   :index:`XPLM_VK_NUMPAD7`
 .. py:data:: VK_NUMPAD8   :index:`XPLM_VK_NUMPAD8`
 .. py:data:: VK_NUMPAD9   :index:`XPLM_VK_NUMPAD9`
 .. py:data:: VK_MULTIPLY  :index:`XPLM_VK_MULTIPLY`
 .. py:data:: VK_ADD       :index:`XPLM_VK_ADD`
 .. py:data:: VK_SEPARATOR :index:`XPLM_VK_SEPARATOR`
 .. py:data:: VK_SUBTRACT  :index:`XPLM_VK_SUBTRACT`
 .. py:data:: VK_DECIMAL   :index:`XPLM_VK_DECIMAL`
 .. py:data:: VK_DIVIDE    :index:`XPLM_VK_DIVIDE`
 .. py:data:: VK_F1        :index:`XPLM_VK_F1`
 .. py:data:: VK_F2        :index:`XPLM_VK_F2`
 .. py:data:: VK_F3        :index:`XPLM_VK_F3`
 .. py:data:: VK_F4        :index:`XPLM_VK_F4`
 .. py:data:: VK_F5        :index:`XPLM_VK_F5`
 .. py:data:: VK_F6        :index:`XPLM_VK_F6`
 .. py:data:: VK_F7        :index:`XPLM_VK_F7`
 .. py:data:: VK_F8        :index:`XPLM_VK_F8`
 .. py:data:: VK_F9        :index:`XPLM_VK_F9`
 .. py:data:: VK_F10       :index:`XPLM_VK_F10`
 .. py:data:: VK_F11       :index:`XPLM_VK_F11`
 .. py:data:: VK_F12       :index:`XPLM_VK_F12`
 .. py:data:: VK_F13       :index:`XPLM_VK_F13`
 .. py:data:: VK_F14       :index:`XPLM_VK_F14`
 .. py:data:: VK_F15       :index:`XPLM_VK_F15`
 .. py:data:: VK_F16       :index:`XPLM_VK_F16`
 .. py:data:: VK_F17       :index:`XPLM_VK_F17`
 .. py:data:: VK_F18       :index:`XPLM_VK_F18`
 .. py:data:: VK_F19       :index:`XPLM_VK_F19`
 .. py:data:: VK_F20       :index:`XPLM_VK_F20`
 .. py:data:: VK_F21       :index:`XPLM_VK_F21`
 .. py:data:: VK_F22       :index:`XPLM_VK_F22`
 .. py:data:: VK_F23       :index:`XPLM_VK_F23`
 .. py:data:: VK_F24       :index:`XPLM_VK_F24`
 ========================= ===========================

The following definitions are extended and are not based on the Microsoft
key set.

.. table::
 :align: left

 ========================== ===========================
 Value                      SDK Value
 ========================== ===========================
 .. py:data:: VK_EQUAL      :index:`XPLM_VK_EQUAL`
 .. py:data:: VK_MINUS      :index:`XPLM_VK_MINUS`
 .. py:data:: VK_RBRACE     :index:`XPLM_VK_RBRACE`
 .. py:data:: VK_LBRACE     :index:`XPLM_VK_LBRACE`
 .. py:data:: VK_QUOTE      :index:`XPLM_VK_QUOTE`
 .. py:data:: VK_SEMICOLON  :index:`XPLM_VK_SEMICOLON`
 .. py:data:: VK_BACKSLASH  :index:`XPLM_VK_BACKSLASH`
 .. py:data:: VK_COMMA      :index:`XPLM_VK_COMMA`
 .. py:data:: VK_SLASH      :index:`XPLM_VK_SLASH`
 .. py:data:: VK_PERIOD     :index:`XPLM_VK_PERIOD`
 .. py:data:: VK_BACKQUOTE  :index:`XPLM_VK_BACKQUOTE`
 .. py:data:: VK_ENTER      :index:`XPLM_VK_ENTER`
 .. py:data:: VK_NUMPAD_ENT :index:`XPLM_VK_NUMPAD_ENT`
 .. py:data:: VK_NUMPAD_EQ  :index:`XPLM_VK_NUMPAD_EQ`
 ========================== ===========================
