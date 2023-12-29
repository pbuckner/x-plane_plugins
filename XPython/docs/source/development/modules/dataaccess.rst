XPLMDataAccess
==============
.. py:module:: XPLMDataAccess
.. py:currentmodule:: xp          

To use:
::

   import xp

Theory Of Operation
-------------------

The data access API gives you a generic, flexible, high performance way to
read and write data to and from X-Plane and other plug-ins. For example,
this API allows you to read and set the navigation radios, get the plane location,
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
simulator is run (or the plugin that provides the data reference is
reloaded).

The task of looking up a data reference is relatively expensive; look up
your data references once based on verbose strings, and save the opaque
data reference value for the duration of your plugin's operation. Reading
and writing data references is relatively fast (the cost is equivalent to
two function calls through function pointers).

This allows data access to be high performance, while leaving in
abstraction; since data references are opaque and are searched for, the
underlying data access system can be rebuilt.

.. Note:: You must know the correct data-type to read and write.
 APIs are provided for reading and writing data in a number of ways. You can
 also double check the data type for a data ref using :py:func:`getDataRefTypes`.
 Note that automatic conversion is not done for you.

.. Note:: On sharing data with other plugins: the load order of
 plugins is not guaranteed. To make sure that every plugin publishing data
 has published their data references before other plugins try to subscribe,
 publish your data references in your XPluginStart routine but resolve them
 (i.e, :py:func:`findDataRef`) no sooner than your XPluginEnable
 routine is called, or the first time they are needed in code.

X-Plane publishes thousands of datarefs; a complete list may be found in
the reference section of the SDK online documentation (from the SDK home
page, choose Documentation https://developer.x-plane.com/datarefs/).

The general flow is:

 1. :py:func:`findDataRef`: Lookup the string name and receive a ``dataRef`` handle.

 2. :py:func:`getDatai`: Using the ``dataRef`` handle, retrieve current value. The exact accessor function to use
    depends on the dataType (:py:func:`getDatai` for
    scalar integer dataRefs, :py:func:`getDatavf` for float arrays, etc.)

 3. :py:func:`setDatai`: Using the handle, set to a new value. Again, using slightly different accessors
    depending on the type(s) supported by the dataRef.

You can get some limited information about a particular dataRef:

 * :py:func:`canWriteDataRef`: If the ``dataRef`` advertises it is writable (though it may not be...)

 * :py:func:`getDataRefTypes`: What underlying data types are supported by this ``dataRef``.

 * :py:func:`getDataRefInfo`: Given a dataRef, retrieve the string ``name``, owning plugin, writability, and type.

You can create your own dataRefs, which makes them available to other plugins / external programs.

 1. :py:func:`registerDataAccessor`: Association the string name with a set of callback functions

 2. :py:func:`unregisterDataAccessor`: Remove access to your callbacks.

If you have multiple plugins which all need the same data, but you don't want to require any particular
plugin to own the data, you can create a *Shared DataRef*, where the underlying data is managed by X-Plane
and your only access to it is via ``dataRefs``.

 1. :py:func:`shareData`: Request X-Plane to create shared data, adding a notification callback so you can know
    if the data is changed.

 2. :py:func:`unshareData`: Remove your notification callback. Perhaps removing final reference to the shared data.

You can retrieve all dataRefs by interating through a full list known by X-Plane.

 1. :py:func:`countDataRefs` returns the number of currently registered dataRefs, and

 2. :py:func:`getDataRefsByIndex` returns a list of dataRef based on index number. The use :py:func:`getDataRefInfo` to
    retrieve information about each dataref.

To support discovery of dataRefs registered after your plugin, a new message :py:data:`MSG_DATAREFS_ADDED` will
be sent to your plugin whenever a plugin is registered. If interested, you can query for new information
based on index.

Functions
---------

.. py:function:: findDataRef(name)

    Given a data ref string *name*, return the
    actual opaque integer that you use to read and write the data. The
    string names for datarefs are published on the X-Plane SDK web site. (https://developer.x-plane.com/datarefs/).

    Returns integer (as a python Capsule) or None if the data ref cannot be found.

    .. NOTE:: this function is relatively expensive; save the result. Do not
              repeat a lookup every time you need to read or write it.

    ::

       >>> xp.findDataRef('sim/aircraft/electrical/num_batteries')
       <capsule object "datarefRef" at 0x7fa44b4909c0>
       >>> xp.findDataRef('this/does/not/exist')
       None

    `Official SDK <https://developer.x-plane.com/sdk/XPLMDataAccess/#XPLMFindDataRef>`__: :index:`XPLMFindDataRef`

.. py:function:: canWriteDataRef(dataRef)

    Given a *dataRef* as retrieved by :py:func:findDataRef, return True if you can successfully set the
    data, False otherwise. Some datarefs are read-only.

    ::

       >>> dataRef = xp.findDataRef('sim/aircraft/electrical/num_batteries')
       >>> xp.canWriteDataRef(dataRef)
       False
    
    Note that even if a dataref is marked writable, it may not act writable. This can happen
    for datarefs taht X-Plane writes to on every frame of the simulation. In some cases, the dataref
    is writable by you have to set a separate "override" dataref to 1 to stop X-Plane from writing to it.

    `Official SDK <https://developer.x-plane.com/sdk/XPLMDataAccess/#XPLMCanWriteDataRef>`__: :index:`XPLMCanWriteDataRef`

.. py:function:: isDataRefGood(dataRef)

    .. Warning:: This function is deprecated and should not be used. Datarefs are
     valid until plugins are reloaded or the sim quits. Plugins sharing datarefs
     should support these semantics by not unregistering datarefs during
     operation. (You should however unregister datarefs when your plugin is
     unloaded/stopped, as part of general resource cleanup.)

    This function returns whether a *dataRef* is still valid. If it returns
    False, you should re-find the data ref from its original string. Calling an
    accessor function on a bad data ref will return a default value, typically
    0 or 0-length data.

    `Official SDK <https://developer.x-plane.com/sdk/XPLMDataAccess/#XPLMIsDataRefGood>`__: :index:`XPLMIsDataRefGood`

.. py:function:: getDataRefTypes(dataRef)

    This routine returns the type ID of the *dataRef* for accessor use. If a data
    ref is available in multiple data types, they will all be returned. (bitwise OR'd together).

    .. table::
      :align: left

      ==== ==============
      bit  meaning
      ==== ==============
         0 Type_Unknown
         1 Type_Int
         2 Type_Float
         4 Type_Double
         8 Type_FloatArray
        16 Type_IntArray
        32 Type_Data
      ==== ==============

    ::

       >>> dataRef = xp.findDataRef('sim/flightmodel/position/local_x')
       >>> xp.getDataRefTypes(dataRef)
       6
       >>> bool(xp.getDataRefTypes(dataRef) & xp.Type_Float)
       True
       >>> bool(xp.getDataRefTypes(dataRef) & xp.Type_Int)
       False

    Note that dataRefs which are strings are listed by the SDK as Type_Data. There
    is no way to determine if Type_Data byte arrays are strings or just bytes.

    `Official SDK <https://developer.x-plane.com/sdk/XPLMDataAccess/#XPLMGetDataRefTypes>`__: :index:`XPLMGetDataRefTypes`

.. py:function:: getDataRefInfo(dataRef)
   
  This XP12 function returns a DataRefInfo object for the provided ``dataRef``.
  The object has the following members:

   | **name**: the string name of the dataRef
   | **type**: the OR'd bitfield matching the return from :py:func:`getDataRefType`
   | **writable**: boolen
   | **owner**: pluginID of the owning plugin (or 0 if owned by X-Plane).

  >>> dataRef = xp.getDataRefsByIndex()[0]
  >>> info = xp.getDataRefInfo(dataRef)
  >>> info.name
  'sim/aircraft/gear/acf_gear_retract'
  >>> info.type == xp.getDataRefTypes(dataRef)
  True

  .. Warning::

     As with other dataRef related routines, this does not do validation of the dataRef; passing a junk value may crash the sim.

  `Official SDK <https://developer.x-plane.com/sdk/XPLMDataAccess/#XPLMGetDataRefInfo>`__: :index:`XPLMGetDataRefInfo`
   
.. py:function:: countDataRefs

  Returns the total number of datarefs that have been registered in X-Plane.

    >>> xp.countDataRefs()
    6928

  This is similar to the value you'll receive in your XPluginReceiveMessage routine for the :py:data:`MSG_DATAREFS_ADDED` message.
  
  `Official SDK <https://developer.x-plane.com/sdk/XPLMDataAccess/#XPLMCountDataRefs>`__: :index:`XPLMCountDataRefs`

.. py:function:: getDataRefsByIndex(offset=0, count=1)

  Return list of dataRefs. Each dataRef is similar to what is returned by :py:func:`findDataRef`.
  As a special case, if count is `-1`, a full list is returned starting from ``offset``.

    >>> xp.getDataRefsByIndex(count=3)
    [<capsule object "datarefRef" at 0x7fa44b4909c0>,
     <capsule object "datarefRef" at 0x7fa44b940900>,
     <capsule object "datarefRef" at 0x7fa44b4077c0>]

  .. Warning::

     Requesting dataRefs larger than :py:func:`countDataRefs` will return garbage and should be discarded.
     Using such results may crash the sim.
     
  `Official SDK <https://developer.x-plane.com/sdk/XPLMDataAccess/#XPLMGetDataRefsByIndex>`__: :index:`XPLMGetDataRefsByIndex`

Data Accessors
**************

These routines read and write the data references. For each supported data
type there is a reader and a writer.

If the data ref is invalid or the plugin that provides it is disabled or
there is a type mismatch, the functions that read data will return 0 as a
default value or not modify the passed in memory. The plugins that write
data will not write under these circumstances or if the data ref is
read-only.

.. warning:: to keep the overhead of reading datarefs low, these
 routines do not do full validation of a dataRef; passing a junk value for a
 dataRef can result in crashing the sim.

For array-style datarefs, you specify the number of items to read/write and
the offset into the array; the actual number of items read or written is
returned. This may be less to prevent an array-out-of-bounds error.

DataRefs types are:

.. table::
 :align: left

 ============= ==================== ===================
 Integer       :py:func:`getDatai`  :py:func:`setDatai`
 Float         :py:func:`getDataf`  :py:func:`setDataf`
 Double        :py:func:`getDatad`  :py:func:`setDatad`
 Integer Array :py:func:`getDatavi` :py:func:`setDatavi`
 Float Array   :py:func:`getDatavf` :py:func:`setDatavf`
 Bytes         :py:func:`getDatab`  :py:func:`setDatab`
 Strings       :py:func:`getDatas`  :py:func:`setDatas`
 ============= ==================== ===================

For Doubles, Python sets and gets using floats, but you'll
need to use the ``getDatad`` and ``setDatad`` routines if the dataRef is **stored** as a double.

Get Scalar
++++++++++

.. py:function:: getDatai(dataRef)
                 getDataf(dataRef)
                 getDatad(dataRef)

    Read a dataRef (as retrieved using :py:func:`findDataRef`) and return its value.
    The return value is the dataRef value or 0 if the dataRef is NULL or the plugin is
    disabled.

       >>> dataRef = xp.findDataRef('sim/aircraft/electrical/num_batteries')
       >>> xp.getDatai(dataRef)
       1
       >>> xp.getDataf(dataRef)
       0.0

    Note in the second case above, the specified dataRef does not support a "Float" data type
    and returns 0.0 rather than indicating any error. You've been warned.

    Also, specifying a bad dataRef raises and exception:

      >>> dataRef = xp.findDataRef('does/not/exist')
      >>> xp.getDatai(dataRef)
      TypeError: invalid dataRef

    | `Official SDK <https://developer.x-plane.com/sdk/XPLMDataAccess/#XPLMGetDatai>`__ :index:`XPLMGetDatai`
    | `Official SDK <https://developer.x-plane.com/sdk/XPLMDataAccess/#XPLMGetDataf>`__ :index:`XPLMGetDataf`
    | `Official SDK <https://developer.x-plane.com/sdk/XPLMDataAccess/#XPLMGetDatad>`__ :index:`XPLMGetDatad`

Set Scalar
++++++++++

.. py:function:: setDatai(dataRef, value=0)
                 setDataf(dataRef, value=0.0)
                 setDatad(dataRef, value=0.0)

    Write a new value to a data ref.

    >>> dataRef = xp.findDataRef('sim/aircraft/weight/acf_m_fuel_tot')
    >>> xp.getDataf(dataRef)
    158.757
    >>> xp.setDataf(dataRef, 100.0)
    >>> xp.getDataf(dataRef)
    100.0

    This does nothing
    if the plugin publishing the dataRef is disabled, the
    dataRef is invalid, or the dataRef is not writable.
                 
    >>> dataRef = xp.findDataRef('sim/aircraft/electrical/num_batteries')
    >>> xp.getDatai(dataRef)
    1
    >>> xp.setDatai(dataRef, 10)
    >>> xp.getDatai(dataRef)
    1

    | `Official SDK <https://developer.x-plane.com/sdk/XPLMDataAccess/#XPLMGetDatai>`__ :index:`XPLMGetDatai`
    | `Official SDK <https://developer.x-plane.com/sdk/XPLMDataAccess/#XPLMSetDataf>`__ :index:`XPLMSetDataf`
    | `Official SDK <https://developer.x-plane.com/sdk/XPLMDataAccess/#XPLMSetDatad>`__ :index:`XPLMSetDatad`

Get Array
+++++++++

.. py:function:: getDatavi(dataRef, values=None, offset=0, count=-1)
                 getDatavf(dataRef, values=None, offset=0, count=-1)

 Read a part of an array dataRef. If you pass None for *values* (or don't provide
 the parameter,
 the routine will return the size of the array, ignoring *offset* and *count*.

 >>> dataRef = xp.findDataRef('sim/multiplayer/combat/team_status')
 >>> xp.getDatavi(dataRef)
 20
 >>> values = []
 >>> xp.getDatavi(dataRef, values, count=5)
 5
 >>> values
 [1, 0, 0, 0, 0]

 If *values* is a list, then up to *count* values are copied from the
 dataRef into values, starting at *offset* in the dataRef. If count +
 offset is larger than the size of the dataRef, less than count values
 will be copied. In any case, the number of values copied is returned.

 As a special case, if count is negative, we'll get the size of the array
 and copy all elements into the values list. (We truncate the passed values list prior
 to copying into it, so if you have data there already, it will be over-written.)

 >>> values = ['old', 'data']
 >>> xp.getDatavi(dataRef, values)
 20
 >>> values
 [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]

 You can't simply provide a list as a parameter, as you'll have no way to retrieve
 the results:

 >>> # This doesn't work -- or it "works", but isn't very useful...
 >>> xp.getDatavi(dataRef, [])
 20

 .. Note:: The semantics of array datarefs are entirely implemented by the
  plugin (or X-Plane) that provides the dataRef, not the SDK itself; the
  above description is how these datarefs are intended to work, but a rogue
  plugin may have different behavior.

 | `Official SDK <https://developer.x-plane.com/sdk/XPLMDataAccess/#XPLMGetDatavi>`__ :index:`XPLMGetDatavi`
 | `Official SDK <https://developer.x-plane.com/sdk/XPLMDataAccess/#XPLMGetDatavf>`__ :index:`XPLMGetDatavf`

Set Array
+++++++++

.. py:function:: setDatavi(dataRef, values, offset=0, count=-1)
                 setDatavf(dataRef, values, offset=0, count=-1)

 Write part or all of an array dataRef. *values* is list of integer or float
 values written into the dataRef starting at
 *offset*. Up to *count* values are written; however if the values would
 write "off the end" of the dataRef array, then fewer values are written.

 >>> dataRef = xp.findDataRef('sim/flightmodel/engine/ENGN_thro')
 >>> xp.getDatavf(dataRef)
 8

 (Note for XP11 there are 8 elements, as shown here. XP12 has 16 values.)
 
 >>> values = []
 >>> xp.getDatavf(dataRef, values)
 8
 >>> values
 [0, 0, 0, 0, 0, 0, 0, 0]
 >>> xp.setDatavf(dataRef, [.1, .2, .3, .4, .5, .6, .7, .8, .9, 1.0])
 >>> xp.getDatavf(dataRef, values)
 8
 >>> values
 [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8]

 If count is longer then length of passed values list, an error is
 generated and nothing is written.

  >>> xp.setDatavf(dataRef, [.1, .2, .3, .4, .5, .6, .7, .8, .9, 1.0], count=20)
  RuntimeError: setDatavf list too short for provided count

 If count is negative (or not provided), count is set to length of passed
 values list.
 

 .. Note:: The semantics of array datarefs are entirely implemented by the
    plugin (or X-Plane) that provides the dataRef, not the SDK itself; the
    above description is how these datarefs are intended to work, but a rogue
    plugin may have different behavior.

 | `Official SDK <https://developer.x-plane.com/sdk/XPLMDataAccess/#XPLMSetDatavi>`__ :index:`XPLMSetDatavi`
 | `Official SDK <https://developer.x-plane.com/sdk/XPLMDataAccess/#XPLMSetDatavf>`__ :index:`XPLMSetDatavf`

Get/Set Bytes
++++++++++++++++++
.. py:function:: getDatab(dataRef, values=None, offset=0, count=-1)
                 setDatab(dataRef, values, offset=0, count=-1)

 Read/Write a part of a byte array dataRef. See similar functionality
 :py:func:`getDatavi`, :py:func:`setDatavi`.

 Note that the "data" being copied is an array of **bytes**, which in C and C++
 is a simple concept, but in Python is a bit more convoluted. (Plus Python2 is different
 from Python3 in this regard.)
 
 A Python2 versus Python3 difference is where the dataRef refers to a string. Recall
 that strings in python2 are bytes and in python3 are unicode. Take, for example,
 dataRef ``sim/aircraft/view/acf_descip``. While it certainly appears to be a string
 description of the user's aircraft, it is actually a sequence of bytes. You should
 convert it to a bytearray, stripping off trailing ``\x00``, and then decode it from UTF-8
 into unicode.

   >>> description = []
   >>> xp.getDatab(xp.findDataRef("sim/aircraft/view/acf_descrip"), description)
   260
   >>> description
   [67, 101, 115, 115, 110, 97, 32, 49, 55, 50, 32, 83, 80, 32, 83, 107, 121, 104, 97, 119,\
   107, 32, 45, 32, 49, 56, 48, 72, 80, 32, 45, 32, 71, 49, 48, 48, 48, 0, 0, 0, ...]
   >>> bytearray(description)
   bytearray(b'Cessna 172 SP Skyhawk - 180HP - G1000\x00\x00\x00\x00\x00\x00\x00\x00\x00...')
   >>> bytearray([x for x in description if x]).decode('utf-8')
   'Cessna 172 SP Skyhawk - 180HP - G1000'
 
 Similarly, you need to convert from strings, if you're looking to set values

   >>> description = "My New Cessna"
   >>> xp.setDatab(xp.findDataRef("sim/aircraft/view/acf_descrip"), bytearray(description.encode('utf-8')))

 **OR,** if you're manipulating Strings, use :py:func:`getDatas` and :py:func:`setDatas`.

 | `Official SDK <https://developer.x-plane.com/sdk/XPLMDataAccess/#XPLMGetDatab>`__ :index:`XPLMGetDatab`
 | `Official SDK <https://developer.x-plane.com/sdk/XPLMDataAccess/#XPLMSetDatab>`__ :index:`XPLMSetDatab`

Get/Set Strings
++++++++++++++++++

These string-manipulation dataRef functions are not part of the Laminar API, but one that is supported only by XPPython3. Seems pretty
useful.

.. py:function:: getDatas(dataRef, offset=0, count=-1)

  Given how common it is to have string dataRefs, and how awkward the SDK makes it
  to retrieve string information from X-Plane using Python, XPPython3 offers
  a custom API. It works similar to :py:func:`getDatab`, except it will do
  the string conversion for you, and it returns **the string** rather than a count,
  thereby skipping the need to include a *values* list.

  >>> dataRef = xp.findDataRef("sim/aircraft/view/acf_descrip")
  >>> xp.getDatas(dataRef)
  'Cessna 172 SP Skyhawk - 180HP'

  Specifying a *count*, will return a string of no more than
  that length. (The returned string will be less than count, if
  a null byte is seen in the retrieved dataRef.)

  >>> xp.getDatas(dataRef, count=10)
  'Cessna 172'

  *offset* works in a similar manner

  >>> xp.getDatas(dataRef, count=3, offset=7)
  '172'

.. py:function:: setDatas(dataRef, value, offset=0, count=-1)
                 
  Set a byte-array dataRef to the string *value*. *offset* starts
  writing the string into the dataRef at the given offset.

  >>> xp.setDatas(dataRef, "ZZZZ", offset=2, count=2)
  >>> xp.getDatas(dataRef)
  'CeZZna 172 SP Skyhawk - 180HP'

  *count* reflects the *number of bytes to occupy*. This is slightly
  different from :py:func:`setDatab`. First (like `setDatab`), if count
  is less than `len(value)`, only that many items are copied. However,
  if count is greater than len(value), that many bytes will be
  copied into the dataRef, with zeros padded to fill.

  **Caution:** if count *exceeds* the length of the original C-language dataRef, this
  *will crash the sim*, because it will over-write memory not belonging to the dataRef.
  If the dataRef's accessor is implemented in python, (See :py:func:`MySetDatas`
  below) it *will not crash the sim*, because python will dynamically increase the buffer
  size to accommodate.

  Compare with `getDatab` and `setDatab`. Looking at the first forty bytes of the raw dataRef,
  you'll see final bytes are padded with zeros.

  Continuing from `count=2` example above:
  
  >>> xp.setDatas(dataRef, "ZZZZ", offset=2, count=5)
  >>> xp.getDatas(dataRef)
  'CeZZZZ'
  >>> xp.getDatab(dataRef, description, count=40)
  >>> description
  [67, 101, 90, 90, 90, 90, 0, 49, 55, 50, 32, 83, 80, 32, 83, 107, 121, 104, 97, \
  119, 107, 32, 45, 32, 49, 56, 48, 72, 80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]

  Note that we copied in five bytes [90, 90, 90, 90, 0], so the resulting `getDatas` returns the
  null-terminated string 'CeZZZZ'. The previous data past those written bytes is
  still there (as seen by `getDatab`).

  Now, consider immediately following with:

  >>> xp.setDatas(dataRef, "ZZZZ-", offset=2, count=5)
  >>> xp.getDatas(dataRef)
  'CeZZZZ-172 SP Skyhawk - 180HP'
  >>> xp.getDatab(dataRef, description, count=40)
  >>> description
  [67, 101, 90, 90, 90, 90, 45, 49, 55, 50, 32, 83, 80, 32, 83, 107, 121, 104, 97, \
  119, 107, 32, 45, 32, 49, 56, 48, 72, 80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]

  We copy in five bytes [90, 90, 90, 90, 45] in the same space, which means the first null byte
  is now at the end of the original string.

  If count is not provided, we'll zero fill the full buffer.

  >>> xp.setDatas(dataRef, "ZZZZ", offset=2)
  >>> xp.getDatas(dataRef)
  'CeZZZZ'
  >>> xp.getDatab(dataRef, description, count=40)
  >>> description
  [67, 101, 90, 90, 90, 90, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,\
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
  
  Sometimes it is useful to have multiple strings "within" a dataRef. For example
  dataRef "sim/cockpit2/tcas/targets/flight_id' is 512 byte array, but functionally it
  is a set of 64 seven-byte null terminated strings.

  >>> tcasDataRef = xp.findDataRef('sim/cockpit2/tcas/targets/flight_id')
  >>> xp.getDatab(tcasDataRef, description, count=40)
  40
  >>> description
  [78, 49, 55, 50, 83, 80, 0, 0, \
   78, 49, 55, 50, 83, 80, 0, 0, \
   78, 49, 55, 50, 83, 80, 0, 0, \
   78, 49, 55, 50, 83, 80, 0, 0, \
    0,  0,  0,  0,  0,  0, 0, 0]
  >>> xp.getDatas(tcasDataRef)
  'N172NP'

  `getDatas(dataRef)` gets the first null-terminated entry. To get subsequent entries, you'll
  need to use *offset*.

  To set entries, you need to set them individually, *without zeroing out the remaining buffer*.
  (First, you need to acquire all aircraft and set override, in order to make TCAS dataRef
  writable. *And* the first entry cannot be over-written, so we'll only try to update the
  other slots.)

  >>> xp.acquirePlanes(None, None, None)
  1
  >>> xp.setDatai(xp.findDataRef('sim/operation/override/override_TCAS'), 1)
  >>> xp.setDatas(tcasDataRef, value="ZYXWVU",  offset=8,  count=8)
  >>> xp.setDatas(tcasDataRef, value="1234",    offset=16, count=8)
  >>> xp.setDatas(tcasDataRef, value="0987654", offset=24, count=8)
  >>> xp.getDatab(tcasDataRef, description, count=40)
  40
  >>> description
  [78, 49, 55, 50, 83, 80,  0, 0, \
   90, 89, 88, 87, 86, 85,  0, 0, \
   49, 50, 51, 52,  0,  0,  0, 0, \
   48, 57, 56, 55, 54, 53, 52, 0, \
    0,  0,  0,  0,  0,  0,  0, 0]
  >>> xp.getDatas(tcasDataRef)
  'N172NP'
  >>> xp.getDatas(tcasDataRef, offset=8)
  'ZYXWVU'

  If you'd not specified a *count*, you would zero-fill all entries after
  the one you'd written.
                 

Callbacks: Publishing Your Plugin's Data
****************************************
These functions allow you to create data references that other plug-ins can
access via the above data access APIs. Data references published by other
plugins operate the same as ones published by X-Plane in all manners except
that your data reference will not be available to other plugins if/when
your plugin is disabled.

You share data by registering data provider callback functions using :py:func:`registerDataAccessor`.
When a
plug-in requests your data (e.g., with :py:func:`getDatai`),
the appropriate callback is called. You provide
one callback to return the value when a plugin 'reads' it and another to
change the value when a plugin 'writes' it.

Important: you must pick a prefix for your datarefs other than ``sim/`` -
this prefix is reserved for X-Plane. The X-Plane SDK website contains a
registry where authors can select a unique first word for dataRef names, to
prevent dataRef collisions between plugins. (Yea... that's what Laminar says, but
I've never found the "registry": Pick a prefix to match your company / domainname + plugin name.)

Registration
++++++++++++
.. py:function:: registerDataAccessor(name, dataType=0, writable=-1, ..., readRefCon=None, writeRefCon=None)

 This routine creates a new item of data that can be read and written.

 The *name* needs to be unique and will be available to others. Ideally, start
 it with the name of you plugin or a domain name you own. This will be accessible
 by :py:func:`findDataRef`.

 The *dataType* is constructed by bitwise OR'ing together the types you support, e.g.,
 ``Type_Float | Type_Int``. This will be readable by :py:func:`getDataRefTypes`.
 If not provided (or set to 0 / ``Type_Unknown``),
 we'll calculate the value based on the set of accessor callback you provide
 (e.g., if you only provide a *readInt* callback, we'll set *dataType* to xp.Type_Int.)

 The *writable* parameter provides the value for
 :py:func:`canWriteDataRef`. It defaults to -1 / not-set, and with that value (or
 not provided in the call) we'll calculate the value based on the existence
 of any write* accessor callbacks you provide.     

 You can optionally provide two reference constants, one to be passed to (all)
 read callbacks, and the other to all write callbacks.

 For each data type you
 support, pass a read accessor function and a write accessor function if
 necessary. All callbacks by default are None, so the easiest thing
 to do is pass by keyword parameter the ones you need.

 .. table::
   :align: left

   ============== ====================================================
   Keyword        Callback prototype
   ============== ====================================================
   readInt        ``myRead(readRefCon) -> int``
   writeInt       ``myWrite(writeRefCon, value)``
   readFloat      ``myRead(readRefCon) -> float``
   writeFloat     ``myWrite(writeRefCon, value)``
   readIntArray   ``myRead(readRefCon, values, offset, count) -> int``
   writeIntArray  ``myWrite(writeRefCon, values, offset, count)``
   readFloatArray ``myRead(readRefCon, values, offset, count) -> int``
   writeFloatArry ``myWrite(writeRefCon, values, offset, count)``
   readData       ``myRead(writeRefCon, values, offset, count) -> int``
   writeData      ``myWrite(writeRefCon, values, offset, count)``
   ============== ====================================================

 >>> def my_func(refCon):
 ...     return refCon + 1
 ...
 >>> accessor = xp.registerDataAccessor('myPlugin/foobar', readInt=my_func, readRefCon=41)
 >>> xp.getDatai(accessor)
 42

 Note that you can use lambda expressions, if it meets your needs:

 >>> accessor = xp.registerDataAccessor('myPlugin/foobar', readInt=lambda x: x + 1, readRefCon=41)
 >>> xp.getDatai(accessor)
 42

 You are returned a data accessor reference for the new item of data created. You can use
 this accessor reference to unregister your data later. Though it is not
 the same as a dataRef (as returned by :py:func:`findDataRef`), you can
 still use it to get and set data. The data accessor reference
 should be used with :py:func:`unregisterDataAccessor`.

 For example, to define a dataRef ``myPlugin/dataItem``, which can be
 accessed as either an Integer or Float, use something like the following. Note
 we provide callbacks for `reading` as an integer or float, but the user can
 only read (not write) as a float (no idea if this might ever make sense in the real world.)

 >>> def MyReadInt(refCon):
 ...     return MyGlobalData
 ...
 >>> def MyWriteInt(refCon, value):
 ...     global MyGlobalData
 ...     MyGlobalData = value
 ...
 >>> def MyReadFloat(refCon):
 ...     return MyReadInt(refCon)
 ...
 >>> accessor = xp.registerDataAccessor('myPlugin/dataItem1', \
 ...                                    readInt=MyReadInt, writeInt=MyWriteInt,\
 ...                                    readFloat=MyReadFloat)
 >>> 
 >>> MyGlobalData = 42
 >>> dataRef = xp.findDataRef('myPlugin/dataItem1')
 >>> xp.getDatai(dataRef)
 42
 >>> xp.setDatai(dataRef, 43)
 >>> xp.getDatai(dataRef)
 43
 >>> xp.getDataf(dataRef)
 43.0

 `Official SDK <https://developer.x-plane.com/sdk/XPLMDataAccess/#XPLMRegisterDataAccessor>`__: :index:`XPLMRegisterDataAccessor`

.. py:function:: unregisterDataAccessor(accessor)

    Use this routine to unregister any data accessors you may have registered with
    :py:func:`registerDataAccessor`. Once you unregister a data ref, your function
    pointer will not be called anymore.

    For maximum compatibility, do not unregister your data accessors until
    final shutdown (when your XPluginStop routine is called). This allows other
    plugins to find your data reference once and use it for their entire time
    of operation.

    `Official SDK <https://developer.x-plane.com/sdk/XPLMDataAccess/#XPLMUnregisterDataAccessor>`__: :index:`XPLMUnregisterDataAccessor`

Callbacks
+++++++++

ReadCallbacks are passed the readRefCon provided when registering the dataRef.
WriteCallbacks are passed the writeRefCon provided when registering the dataRef.

Scaler accessors (int, float, double) are pretty simple:

.. py:function:: MyGetDatai(refCon)
                 MyGetDataf(refCon)
                 MyGetDatad(refCon)

and

.. py:function:: MySetDatai(refCon, value)
                 MySetDataf(refCon, value)
                 MySetDatad(refCon, value)


 If you use the same callback function for more than one dataRef
 you can use the reference constant to identify which one is being requested.

 >>> def MyReadInt(refCon):
 ...     if refCon == 'param1':
 ...         return int(self.param1)
 ...     elif refCon == 'param2':
 ...         return int(self.param2)
 ...     raise ValueError("Unknown parameter: {}".format(refCon))
 ...
 >>> def MyWriteInt(refCon, value):
 ...     if refCon == 'param1':
 ...         self.param1 = int(value)
 ...     elif refCon == 'param2':
 ...         self.param2 = int(value)
 ...     else:
 ...         raise ValueError("Unknown parameter: {}".format(refCon))

    
Array accessors should be mindful of the *offset* and *count* values.

.. py:function:: MyGetDatavi(refCon, values, offset, count)
                 MyGetDatavf(refCon, values, offset, count)

 The callback semantics is the same as :py:func:`getDatavi` and :py:func:`getDatavf` (those routines
 just forward the request to your callback). If values is None, return the size of the array,
 ignoring *offset* and *count*.

.. py:function:: MySetDatavi(refCon, values, offset, count)
                 MySetDatavf(refCon, values, offset, count)

 The callback semantics is the same as :func:`setDatavi` and :func:`setDatavf` (those routines
 just forward the request to your callback). Values passed in are written into the
 dataRef starting at *offset*. Up to *count* values are written; however if the values would write
 "off the end" of the dataRef array, then fewer values are written.

 Recall that the return value for the *getData* callbacks is the full length of the supported
 buffer, if input *values* is None, otherwise, it is the number of elements actually written into
 *values*.

 >>> MyArray1 = [1, 3, 5]
 >>> MyArray2 = [2, 4, 6, 8]
 >>> def MyGetDatavi(refCon, values, offset, count):
 ...     if refCon == '1':
 ...         if values is None:
 ...             return len(MyArray1)
 ...         values.extend(MyArray1[offset:offset + count])
 ...         return min(count, len(MyArray1) - offset)
 ...     elif refCon == '2':
 ...         if values is None:
 ...             return len(MyArray2)
 ...         values.extend(MyArray2[offset:offset + count])
 ...         return min(count, len(MyArray2) - offset)
 ...     else:
 ...         raise ValueError("Unknown refCon: {}".format(refCon))
 ...
 >>> def MySetDatavi(refCon, values, offset, count):
 ...     global MyArray1, MyArray2
 ...     if not values:
 ...         print("values not provide")
 ...         pass
 ...     elif refCon == '1':
 ...         MyArray1[offset:offset + count] = values[0:count]
 ...     elif refCon == '2':
 ...         MyArray2[offset:offset + count] = values[0:count]
 ...     else:
 ...         raise ValueError("Unknown refCon: {}".format(refCon))
 ...     return
 ...     
 >>> accessor = xp.registerDataAccessor('myplugin/int_array', \
 ...            readIntArray=MyGetDatavi, \
 ...            writeIntArray=MySetDatavi, \
 ...            readRefCon='1', writeRefCon='1')
 >>>
 >>> values = []
 >>> xp.getDatavi(accessor, values)
 3
 >>> values
 [1, 3, 5]
 >>> xp.setDatavi(accessor, [7, 7, 7], offset=1, count=3)
 >>> xp.getDatavi(accessor, values)
 4
 >>> values
 [1, 7, 7, 7]
 
 
 .. note:: the use of ``extend()`` within ``MyReadIntV()`` rather than simple assignment
    (e.g., ``values = MyArray[offset: offset + count]``).
    The *values* parameter should be either a list (``[]``) or None. If it's a list, we use it
    to return the actual values (rather than merely returning the length of the data.) Because the calling
    function needs the value, we cannot change the python ``id()`` of the object. Simple assignment changes
    the id, so the calling function never gets the updated value. Using ``extend()`` (or ``append()``) will
    maintain the id of the *values* parameter, allowing the calling function to retrieve the information.
    

Data accessors can be implemented similar to the other array accessors.

.. py:function:: MyGetDatab(refCon, values, offset, count)
                 MySetDatab(refCon, values, offset, count)

 The callback semantics are the same as :func:`setDatab` and :func:`getDatab`.

 >>> MyArray1 = [1, 3, 5]
 >>> MyArray2 = [2, 4, 6, 8]
 >>> def MyGetDatavi(refCon, values, offset, count):
 ...     if refCon == '1':
 ...         if values is None:
 ...             return len(MyArray1)
 ...         values.extend(MyArray1[offset:offset + count])
 ...         return min(count, len(MyArray1) - offset)
 ...     elif refCon == '2':
 ...         if values is None:
 ...             return len(MyArray2)
 ...         values.extend(MyArray2[offset:offset + count])
 ...         return min(count, len(MyArray2) - offset)
 ...     else:
 ...         raise ValueError("Unknown refCon: {}".format(refCon))
 ...
 >>> def MySetDatavi(refCon, values, offset, count):
 ...     global MyArray1, MyArray2
 ...     if not values:
 ...         print("values not provide")
 ...         pass
 ...     elif refCon == '1':
 ...         MyArray1[offset:offset + count] = values[0:count]
 ...     elif refCon == '2':
 ...         MyArray2[offset:offset + count] = values[0:count]
 ...     else:
 ...         raise ValueError("Unknown refCon: {}".format(refCon))
 ...     return
 ...     
 >>> accessor = xp.registerDataAccessor('myplugin/data_array', \
 ...            readData=MyGetDatavi, \
 ...            writeData=MySetDatavi, \
 ...            readRefCon='1', writeRefCon='1')
 >>>
 >>> values = []
 >>> xp.getDatab(accessor, values)
 3
 >>> values
 [1, 3, 5]
 >>> xp.setDatab(accessor, [7, 7, 7], offset=1, count=3)
 >>> xp.getDatab(accessor, values)
 4
 >>> values
 [1, 7, 7, 7]

String accessors are really just the same as Data accessors: you'll need to provide (only) Data Accessors
if you want to use String Accessors in your python code. Non-python plugins will still only be able to
use Data Accessors to manipulate strings.

Recall that for strings, we get and set data, but convert the bytes to/from UTF-8 strings. You can use
the above Integer Array accessors (``MyGetDatavi`` and ``MySetDatavi``) and provide them as callback
for your ``registerDataAccessor``::

 >>> accessor = xp.registerDataAccessor('myplugin/data_array', \
 ...            readData=MyGetDatavi, \
 ...            writeData=MySetDatavi, \
 ...            readRefCon='1', writeRefCon='1')
 >>>
 >>> values = []
 >>> xp.getDatab(accessor, values)
 3
 >>> values
 [1, 3, 5]
 >>> xp.setDatab(accessor, [7, 7, 7], offset=1, count=3)
 >>> xp.getDatab(accessor, values)
 4
 >>> values
 [1, 7, 7, 7]

Now, as strings:

 >>> xp.setDatas(accessor, "Hello world")
 >>> xp.getDatas(accessor, values)
 "Hell"

What? Why didn't the full string get set? Simple: recall that if *count* is not specified, we use the *current length*
of the array. Because there were only four values ``[1, 7, 7, 7]`` in the underlying
array, we wrote only up to the end of the array.

Because python memory management is vastly simpler (on the programmer) than C & C++,
we can easily extend it, by providing a *count* at least a long as the string
you're setting. Setting *count* to be longer than the underlying dataRef is
a problem with non-python dataRefs but works fine with python implementations (that
is, where python is used to write the data accessor e.g., ``MySetData()``.)

 >>> xp.setDatas(accessor, "Hello world", count=20)
 >>> xp.getDatas(accessor, values)
 "Hello world"
 >>> xp.getDatas(accessor, values)
 20
 >>> values
 [72, 101, 108, 108, 111, 32, 87, 111, 114, 108, 100, 0, 0, 0, 0, 0, 0, 0, 0, 0]

So you can see in the above example, the "raw" data is still stored as an array of bytes. You
could store the underlying data as a string, but then you'd need to make you data callbacks
a bit smarter as *they* would be responsible for converting to/from the stored strings
and a byte representation. For example

 >>> MyString = "Hello"
 >>> def MyGetData(refCon, values, offset, count):
 ...     array = bytearray(MyString, encoding='utf-8')
 ...     if values is None:
 ...         return len(MyString)
 ...     values.extend(array[offset:offset + count])
 ...     return min(count, len(MyString) - offset)  # number of bytes copied
 ...
 >>> def MySetData(refCon, values, offset, count):
 ...     global MyString
 ...     array = bytearray(MyString, encoding='utf-8')
 ...     array[offset:offset + count] = values[0:count]
 ...     # and finally, update global MyString with string equivalent
 ...     MyString = bytearray([x for x in array if x]).decode('utf-8')
 ...
 >>> a = xp.registerDataAccessor('myplugin/string', readData=MyGetData, writeData=MySetData)
 >>> xp.getDatab(a)
 >>> xp.getDatas(a)


 
Interfacing with DataRefEditor and DataRefTool
**********************************************

The third-party `DataRefEditor plugin <http://www.xsquawkbox.net/xpsdk/mediawiki/DataRefEditor>`_
and `DataRefTool plugin <https://forums.x-plane.org/index.php?/forums/topic/82960-datareftool-is-an-improved-datarefeditor-open-source-better-search-change-detection/>`_
allow you to test your datarefs.

1. Create you dataRefs in your XPluginStart function. (This is the recommended practice for all dataRef creation).
   
2. Register them in your XPluginEnable function (which signature you use depends on which plugin you're
   trying to connect to.)::

     for sig in ('com.leecbaker.datareftool', 'xplanesdk.examples.DataRefEditor'):
         dre = xp.findPluginBySignature(sig)
         if dre != xp.NO_PLUGIN_ID:
             xp.sendMessageToPlugin(dre, 0x01000000, 'myplugin/dataRef1')
             xp.sendMessageToPlugin(dre, 0x01000000, 'myplugin/dataRef2')


This way your datarefs will appear in the DataRef plugin.
     

Sharing Data Between Multiple Plugins
*************************************

The data reference registration APIs from the previous section allow a
plugin to publish data in a one-owner manner; the plugin that publishes the
data reference owns the real memory that the data ref uses. This is
satisfactory for most cases, but there are also cases where plugins need to
share actual data.

With a shared data reference, no one plugin owns the actual memory for the
data reference; the plugin SDK allocates that for you. When the first
plugin asks to 'share' the data, the memory is allocated. When the data is
changed, every plugin that is sharing the data is notified (via callback).

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
  data reference; you must use the ``xp.getData...`` and ``xp.setData...`` APIs. With an
  owned data reference, the one owning data reference can manipulate the data
  reference's memory in any way it sees fit.

Shared data references solve two problems: if you need to have a data
reference used by several plugins but do not know which plugins will be
installed, or if all plugins sharing data need to be notified when that
data is changed, use shared data references.

.. note:: You cannot "share" a built-in dataRef. While you may think
          sharing a built-in would allow you to register a *dataChanged* callback,
          the registration returns successfully,
          but the callback is never invoked.


.. py:function:: shareData(name, dataType, dataChanged=None, refCon=None)

 This routine connects a plug-in to shared data, creating the shared data if
 necessary. *name* is a standard style of path string for the data ref,
 and *dataType* specifies the data type. *dataType* must be a single value,
 not a bitwise OR'd set of values.

 This function will create the data if it does not
 exist. If the data already exists but the type does not match, an error is
 returned, so it is important that plug-in authors collaborate to establish
 public standards for shared data.

 If a notificationFunc *dataChanged* is passed in and is not None, it
 will be called whenever the data is modified. The provided *refCon*
 will be passed to the callback. This allows your plug-in to know which shared
 data was changed if multiple shared data are handled by one callback, or if
 the plug-in does not use global variables.
 
 Returns 1 if shared data is successfully created (or found); a
 0 if the data already exists but is of the wrong type.

 The callback function takes one parameter which is the *refCon* passed with
 ``shareData()`` (which defaults to None).

 >>> def dataChanged(refCon):
 ...   xp.log("Data has changed")
 ...

 While you *can* call ``shareData()`` with an existing built-in X-Plane DataRef,
 you *will not* get a callback on data change. (pity, really.)

 For example, define a simple callback which simply alerts the user. Then
 create a shared data item called ``shared/float``.
 
 >>> def Changed(refCon):
 ...     xp.speakString(f"Data has changed for {refCon}")
 ...
 >>> comment = "My Float"
 >>> xp.shareData('shared/float', xp.Type_Float, Changed, comment)
 1

 Find the ``dataRef`` handle. Note that on initialization the value is 0.0. Set
 it to a new value and observe the alert.

 >>> dataRef = xp.findDataRef('shared/float')
 >>> xp.getDataf(dataRef)
 0.0
 >>> xp.setDataf(dataRef, 1.0)

 When you no longer want to have your callback function called, unshare the
 data. Note you *may* still have access to the data, but it is not guaranteed.
 
 >>> xp.unshareData('shared/float', xp.Type_Float, Changed, comment)
 >>> xp.getDataf(dataRef)        # <-- *might* work, but don't count on it
 1.0
 >>> xp.setDataf(dataRef, 2.0)   # <-- *might* work, but don't count on it
 >>> xp.getDataf(dataRef)        # <-- *might* work, but don't count on it
 2.0
 
 Of course, you don't have to include a callback function: you can call ``shareData``
 to simply create and manage the data reference & then you can poll (using :py:func:`getDataf`)
 to get data updates.

.. py:function:: unshareData(name, dataType, dataChanged=None, refCon=None)

 This routine removes your notification function for shared data. Call it
 when done with the data to stop receiving change notifications. Arguments
 must match your :py:func:`shareData`. The actual memory will not necessarily be freed,
 since other plug-ins could be using it. Returns 0 if dataRef is not found.

 >>> refCon = "hello"
 >>> xp.shareData('shared/example', xp.Type_Int, Changed, refCon)
 1
 >>> xp.unshareData('shared/example', xp.Type_Int, Changed, refCon)
 1

 Note the use of a variable for the *refCon* instead of repeating the string
 ``"hello"`` in the share and unshare calls. The reference constant
 *must be the same* in both calls, not merely point to the same data.
 The exception is you can use integers or None, but you cannot use
 other constants (floats, strings) or structures (dicts, lists, etc.).

 >>> # Integers work
 >>> xp.shareData('shared/int', xp.Type_Int, Changed, 5)
 1
 >>> xp.unshareData('shared/int', xp.Type_Int, Changed, 5)
 1
 >>> # None works
 >>> xp.shareData('shared/none', xp.Type_Int, Changed, None)
 1
 >>> xp.unshareData('shared/none', xp.Type_Int, Changed, None)
 1
 >>> # Floats DO NOT work
 >>> xp.shareData('shared/float', xp.Type_Int, Changed, 1.5)
 1
 >>> xp.unshareData('shared/example', xp.Type_Int, Changed, 1.5)
 0   # <-- was not able to find the shared data ref
 >>> # Strings DO NOT work
 >>> xp.shareData('shared/string', xp.Type_Int, Changed, "hello")
 1
 >>> xp.unshareData('shared/string', xp.Type_Int, Changed, "hello")
 0   # <-- was not able to find the shared data ref

 One quirk in this, is *you can* pass a float or string -- it will be correctly
 passed into your ``dataChanged()`` callback: you just won't be able
 to successfully call ``unshareData()``... which perhaps isn't the greatest
 tragedy.

Types
--------------------

.. data:: XPLMDataTypeID
   :annotation: bitfield used to identify the type of data

   .. table::
      :align: left

      +-------------------------------+-----------------------------+
      | Value                         | SDK Value                   |
      +===============================+=============================+
      |.. py:data:: Type_Unknown      | :index:`xplmType_Unknown`   |
      |              :value: 0        |                             |
      +-------------------------------+-----------------------------+
      | .. py:data:: Type_Int         | :index:`xplmType_Int`       |                    
      |              :value: 1        |                             |
      +-------------------------------+-----------------------------+
      | .. py:data:: Type_Float       | :index:`xplmType_Float`     |                    
      |              :value: 2        |                             |
      +-------------------------------+-----------------------------+
      | .. py:data:: Type_Double      | :index:`xplmType_Double`    |                    
      |              :value: 4        |                             |
      +-------------------------------+-----------------------------+
      | .. py:data:: Type_FloatArray  | :index:`xplmType_FloatArray`|                    
      |              :value: 8        |                             |
      +-------------------------------+-----------------------------+
      | .. py:data:: Type_IntArray    | :index:`xplmType_IntArray`  |                    
      |              :value: 16       |                             |
      +-------------------------------+-----------------------------+
      | .. py:data:: Type_Data        | :index:`xplmType_Data`      |                    
      |              :value: 32       |                             |
      +-------------------------------+-----------------------------+
