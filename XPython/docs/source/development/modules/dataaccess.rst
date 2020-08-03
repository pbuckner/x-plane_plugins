XPLMDataAccess
==============

To use:
::

   import XPLMDataAccess

Theory Of Operation
-------------------

The data access API gives you a generic, flexible, high performance way to
read and write data to and from X-Plane and other plug-ins. For example,
this API allows you to read and set the nav radios, get the plane location,
determine the current effective graphics frame rate, etc.

The data access APIs are the way that you read and write data from the sim
as well as other plugins.

The API works using opaque data references. A data reference is a source of
data; you do not know where it comes from, but once you have it you can
read the data quickly and possibly write it. To get a data reference, you
look it up.

Data references are identified by verbose string names
(e.g., :code:`sim/cockpit/radios/nav1_freq_hz`). The actual numeric value of the data
reference is implementation defined and is likely to change each time the
simulator is run (or the plugin that provides the datareference is
reloaded).

The task of looking up a data reference is relatively expensive; look up
your data references once based on verbose strings, and save the opaque
data reference value for the duration of your plugin's operation. Reading
and writing data references is relatively fast (the cost is equivalent to
two function calls through function pointers).

This allows data access to be high performance, while leaving in
abstraction; since data references are opaque and are searched for, the
underlying data access system can be rebuilt.

.. Note:: You must know the correct data type to read and write.
 APIs are provided for reading and writing data in a number of ways. You can
 also double check the data type for a data ref. Note that automatic
 conversion is not done for you.

.. Note:: On sharing data with other plugins: the load order of
 plugins is not guaranteed. To make sure that every plugin publishing data
 has published their data references before other plugins try to subscribe,
 publish your data references in your start routine but resolve them the
 first time your 'enable' routine is called, or the first time they are
 needed in code.

X-Plane publishes well over 1000 datarefs; a complete list may be found in
the reference section of the SDK online documentation (from the SDK home
page, choose Documentation https://developer.x-plane.com/datarefs/).

.. automodule:: XPLMDataAccess

Functions
---------

.. autofunction:: XPLMFindDataRef
.. autofunction:: XPLMGetDataRefTypes
.. autofunction:: XPLMCanWriteDataRef
.. autofunction:: XPLMIsDataRefGood

Data Accessors
**************

These routines read and write the data references. For each supported data
type there is a reader and a writer.

If the data ref is invalid or the plugin that provides it is disabled or
there is a type mismatch, the functions that read data will return 0 as a
default value or not modify the passed in memory. The plugins that write
data will not write under these circumstances or if the data ref is
read-only.

.. NOTE:: to keep the overhead of reading datarefs low, these
 routines do not do full validation of a dataref; passing a junk value for a
 dataref can result in crashing the sim.

For array-style datarefs, you specify the number of items to read/write and
the offset into the array; the actual number of items read or written is
returned. This may be less to prevent an array-out-of-bounds error.

Dataref types are:

 * Integer (:py:func:`XPLMGetDatai`, :py:func:`XPLMSetDatai`)

 * Float (:py:func:`XPLMGetDataf`, :py:func:`XPLMSetDataf`)

 * Double (:py:func:`XPLMGetDatad`, :py:func:`XPLMSetDatad`): Python sets and gets using floats, but you'll
   need to use these routines if the dataref is stored as a double.
 
 * Integer Array (:py:func:`XPLMGetDatavi`, :py:func:`XPLMSetDatavi`)

 * Float Array (:py:func:`XPLMGetDatavf`, :py:func:`XPLMSetDatavf`)

 * Bytes (:py:func:`XPLMGetDatab`, :py:func:`XPLMSetDatab`)

Get Scalar
++++++++++

.. py:function:: XPLMGetDatai(inDataref: int) -> int
.. py:function:: XPLMGetDataf(inDataref: int) -> float
.. py:function:: XPLMGetDatad(inDataref: int) -> float

    Read a data ref (as retrieved using :py:func:`XPLMFindDataRef`) and return its value.
    The return value is the dataref value or 0 if the dataref is NULL or the plugin is
    disabled.

Set Scalar
++++++++++

.. py:function:: XPLMSetDatai(inDataref: int, value: int) -> None
.. py:function:: XPLMSetDataf(inDataref: int, value: float) -> None
.. py:function:: XPLMSetDatad(inDataref: int, value: float) -> None

    Write a new value to a data ref. This
    routine is a no-op if the plugin publishing the dataref is disabled, the
    dataref is invalid, or the dataref is not writable.
                 
Get Array
+++++++++

.. py:function:: XPLMGetDatavi(inDataRef: int, outValues: list, inOffset: int, inMax: int) -> int
.. py:function:: XPLMGetDatavf(inDataRef: int, outValues: list, inOffset: int, inMax: int) -> int

 Read a part of an array dataref. If you pass None for outVaules,
 the routine will return the size of the array, ignoring inOffset and inMax.

 If outValues is list, then up to inMax values are copied from the
 dataref into outValues, starting at inOffset in the dataref. If inMax +
 inOffset is larger than the size of the dataref, less than inMax values
 will be copied. The number of values copied is returned.

 .. Note:: The semantics of array datarefs are entirely implemented by the
  plugin (or X-Plane) that provides the dataref, not the SDK itself; the
  above description is how these datarefs are intended to work, but a rogue
  plugin may have different behavior.

Set Array
+++++++++

.. py:function:: XPLMSetDatavi(inDataref: int, inValues: list, inOffset: int, inCount: int) -> None
.. py:function:: XPLMSetDatavf(inDataref: int, inValues: list, inOffset: int, inCount: int) -> None

 Write part or all of an array dataref. The
 values passed by inValues are written into the dataref starting at
 inOffset. Up to inCount values are written; however if the values would
 write "off the end" of the dataref array, then fewer values are written.

 .. Note:: The semantics of array datarefs are entirely implemented by the
    plugin (or X-Plane) that provides the dataref, not the SDK itself; the
    above description is how these datarefs are intended to work, but a rogue
    plugin may have different behavior.

Get/Set Bytes
++++++++++++++++++
.. py:function:: XPLMGetDatab(inDataref: int, outValue: list, inOffset: int, inCount: int) -> None
.. py:function:: XPLMSetDatab(inDataref: int, inValue: list, inOffset: int, inCount: int) -> None

    Read/Write a part of a byte array dataref.

    For Get, if you pass None for outValue, the
    routine will return the size of the array, ignoring inOffset and inMax.
    If outValue is list, then up to inMax values are copied from the
    dataref into outValues, starting at inOffset in the dataref. If inMax +
    inOffset is larger than the size of the dataref, less than inMax values
    will be copied. The number of values copied is returned.

    For Set, write part or all of a byte array dataref. The values passed by inValue
    are written into the dataref starting at inOffset. Up to inCount values are
    written; however if the values would write "off the end" of the dataref
    array, then fewer values are written.

    .. Note:: The semantics of array datarefs are entirely implemented by the
      plugin (or X-Plane) that provides the dataref, not the SDK itself; the
      above description is how these datarefs are intended to work, but a rogue
      plugin may have different behavior.


Callbacks: Publishing Your Plugin's Data
****************************************
These functions allow you to create data references that other plug-ins can
access via the above data access APIs. Data references published by other
plugins operate the same as ones published by X-Plane in all manners except
that your data reference will not be available to other plugins if/when
your plugin is disabled.

You share data by registering data provider callback functions. When a
plug-in requests your data, these callbacks are then called. You provide
one callback to return the value when a plugin 'reads' it and another to
change the value when a plugin 'writes' it.

Important: you must pick a prefix for your datarefs other than "sim/" -
this prefix is reserved for X-Plane. The X-Plane SDK website contains a
registry where authors can select a unique first word for dataref names, to
prevent dataref collisions between plugins.

Registration
++++++++++++
.. autofunction:: XPLMRegisterDataAccessor
.. autofunction:: XPLMUnregisterDataAccessor

Callbacks
+++++++++
.. autofunction:: XPLMDataChanged_f
.. autofunction:: XPLMGetDatab_f
.. autofunction:: XPLMSetDatab_f
.. autofunction:: XPLMGetDatad_f
.. autofunction:: XPLMSetDatad_f
.. autofunction:: XPLMGetDataf_f
.. autofunction:: XPLMSetDataf_f
.. autofunction:: XPLMGetDatai_f
.. autofunction:: XPLMSetDatai_f
.. autofunction:: XPLMGetDatavf_f
.. autofunction:: XPLMSetDatavf_f
.. autofunction:: XPLMGetDatavi_f
.. autofunction:: XPLMSetDatavi_f


Sharing Data Between Multiple Plugins
*************************************

The data reference registration APIs from the previous section allow a
plugin to publish data in a one-owner manner; the plugin that publishes the
data reference owns the real memory that the data ref uses. This is
satisfactory for most cases, but there are also cases where plugnis need to
share actual data.

With a shared data reference, no one plugin owns the actual memory for the
data reference; the plugin SDK allocates that for you. When the first
plugin asks to 'share' the data, the memory is allocated. When the data is
changed, every plugin that is sharing the data is notified.

Shared data references differ from the 'owned' data references from the
previous section in a few ways:

- With shared data references, any plugin can create the data reference;
  with owned plugins one plugin must create the data reference and others
  subscribe. (This can be a problem if you don't know which set of plugins
  will be present).

- With shared data references, every plugin that is sharing the data is
  notified when the data is changed. With owned data references, only the one
  owner is notified when the data is changed.

- With shared data references, you cannot access the physical memory of the
  data reference; you must use the XPLMGet... and XPLMSet... APIs. With an
  owned data reference, the one owning data reference can manipulate the data
  reference's memory in any way it sees fit.

Shared data references solve two problems: if you need to have a data
reference used by several plugins but do not know which plugins will be
installed, or if all plugins sharing data need to be notified when that
data is changed, use shared data references.

.. autofunction:: XPLMShareData
.. autofunction:: XPLMUnshareData

Types
--------------------

.. data:: XPLMDataTypeID
   :annotation: bitfield used to identify the type of data

   .. data:: xplmType_Unknown = 0
   .. data:: xplmType_Int = 1
   .. data:: xpmlType_Float = 2
   .. data:: xpmlType_Double = 4         
   .. data:: xpmlType_FloatArray = 8          
   .. data:: xpmlType_IntArray = 16          
   .. data:: xpmlType_Data = 32          
