XPLMDataAccess
==============
.. py:module:: XPLMDataAccess

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

Functions
---------

.. py:function:: XPLMFindDataRef(inDataRefName: str) -> int

    Given a string that names the data ref, this routine looks up the
    actual opaque integer XPLMDataRef that you use to read and write the data. The
    string names for datarefs are published on the X-Plane SDK web site. (https://developer.x-plane.com/datarefs/).

    This function returns handle to the dataref, or None if the data ref
    cannot be found.

    .. NOTE:: this function is relatively expensive; save the XPLMDataRef this
     function returns for future use. Do not look up your data ref by string
     every time you need to read or write it.

.. py:function:: XPLMCanWriteDataRef(inDataRef: int) -> bool:

    Given a data ref, this routine returns True if you can successfully set the
    data, False otherwise. Some datarefs are read-only.

.. py:function:: XPLMIsDataRefGood(inDataRef: int) -> bool:

    .. Warning:: This function is deprecated and should not be used. Datarefs are
     valid until plugins are reloaded or the sim quits. Plugins sharing datarefs
     should support these semantics by not unregistering datarefs during
     operation. (You should however unregister datarefs when your plugin is
     unloaded/stopped, as part of general resource cleanup.)

    This function returns whether a data ref is still valid. If it returns
    False, you should refind the data ref from its original string. Calling an
    accessor function on a bad data ref will return a default value, typically
    0 or 0-length data.

.. py:function:: XPLMGetDataRefTypes(inDataRef: int) -> int:

    This routine returns the XPLMDataTypeID of the data ref for accessor use. If a data
    ref is available in multiple data types, they will all be returned. (bitwise OR'd together).

     * :py:data:`xplmType_Unknown`
     * :py:data:`xplmType_Int`
     * :py:data:`xplmType_Float`
     * :py:data:`xplmType_Double`
     * :py:data:`xplmType_IntArray`
     * :py:data:`xplmType_FloatArray`
     * :py:data:`xplmType_Data`


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
                 XPLMGetDataf(inDataref: int) -> float
                 XPLMGetDatad(inDataref: int) -> float

    Read a data ref (as retrieved using :py:func:`XPLMFindDataRef`) and return its value.
    The return value is the dataref value or 0 if the dataref is NULL or the plugin is
    disabled.

Set Scalar
++++++++++

.. py:function:: XPLMSetDatai(inDataref: int, value: int) -> None
                 XPLMSetDataf(inDataref: int, value: float) -> None
                 XPLMSetDatad(inDataref: int, value: float) -> None

    Write a new value to a data ref. This
    routine is a no-op if the plugin publishing the dataref is disabled, the
    dataref is invalid, or the dataref is not writable.
                 
Get Array
+++++++++

.. py:function:: XPLMGetDatavi(inDataRef: int, outValues: list, inOffset: int, inMax: int) -> int
                 XPLMGetDatavf(inDataRef: int, outValues: list, inOffset: int, inMax: int) -> int

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
                 XPLMSetDatavf(inDataref: int, inValues: list, inOffset: int, inCount: int) -> None

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
                 XPLMSetDatab(inDataref: int, inValue: list, inOffset: int, inCount: int) -> None

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

    A Python2 versus Python3 difference is where the dataref refers to a string. Recall
    that strings in python2 are bytes and in python3 are unicode. Take, for example,
    dataref ``sim/aircraft/view/acf_descip``. While it certainly appears to be a string
    description of the user's aircraft, it is actually a sequence of bytes. You should
    convert it to a bytearray, stripping off trailing ``\x00``, and then decode it from UTF-8
    into unicode.

      >>> description = []
      >>> XPMGetDatab(XPLMFindDataRef("sim/aircraft/view/acf_descrip"), description, 0, 40)
      >>> print(description)
      [67, 101, 115, 115, 110, 97, 32, 49, 55, 50, 32, 83, 80, 32, 83, 107, 121, 104, 97, 119,\
      107, 32, 45, 32, 49, 56, 48, 72, 80, 32, 45, 32, 71, 49, 48, 48, 48, 0, 0, 0]
      >>> print(bytearray(description))
      bytearray(b'Cessna 172 SP Skyhawk - 180HP - G1000\x00\x00\x00')
      >>> print(bytearray([x for x in description if x]).decode('utf-8'))
      'Cessna 172 SP Skyhawk - 180HP - G1000'


Callbacks: Publishing Your Plugin's Data
****************************************
These functions allow you to create data references that other plug-ins can
access via the above data access APIs. Data references published by other
plugins operate the same as ones published by X-Plane in all manners except
that your data reference will not be available to other plugins if/when
your plugin is disabled.

You share data by registering data provider callback functions using :func:`XPLMRegisterDataAccessor`.
When a
plug-in requests your data (e.g., with :func:`XPLMGetDatai`),
these callbacks are then called. You provide
one callback to return the value when a plugin 'reads' it and another to
change the value when a plugin 'writes' it.

Important: you must pick a prefix for your datarefs other than ``sim/`` -
this prefix is reserved for X-Plane. The X-Plane SDK website contains a
registry where authors can select a unique first word for dataref names, to
prevent dataref collisions between plugins. (Yea... that's what Laminar says, but
I've never found the "registry": Pick a prefix to match your company / domainname + plugin name.)

Registration
++++++++++++
.. py:function:: XPLMRegisterDataAccessor() -> int

 This routine creates a new item of data that can be read and written. Pass
 in the data's full name for searching, the type(s) of the data for
 accessing, and whether the data can be written to. For each data type you
 support, pass in a read accessor function and a write accessor function if
 necessary. All parameters are required input, pass None for data types you do
 not support or write accessors if you are read-only.

 You are returned a data accessor reference for the new item of data created. You can use
 this data ref to unregister your data later or read or write from it.

 :param str inDataName: name of data item, e.g., 'my_plugin/data1'
 :param int inDataType: supported data types, e.g., ``xp.Type_Float | xp.Type_Double``
 :param int inIsWritable: 1 == writable
 :param GetDataRefi_f inReadInt_f: your callback when someone requests to read int
 :param SetDataRefi_f inWriteInt_f: your callback when someone requests to write int
 :param GetDataReff_f inReadFloat_f: ^ to read float
 :param SetDataReff_f inWriteFloat_f: ^ to write float
 :param GetDataRefd_f inReadDouble_f: ^ to read double
 :param SetDataRefd_f inWriteDouble_f: ^ to write double
 :param GetDataRefvi_f inReadIntArray_f: ^ to read int array
 :param SetDataRefvi_f inWriteIntArray_f: ^ to write int array
 :param GetDataRefvf_f inReadFloatArray_f: ^ to read float array
 :param SetDataRefvf_f inWriteFloatArray_f: ^ to write float array
 :param GetDataRefb_f inReadData_f: ^ to read data
 :param SetDataRefb_f inWriteData_f: ^ to write data
 :param object inReadRefcon: reference constant included with read functions
 :param object inWriteRefcon: referenc constant included with write functions
 :return: Data Accessor Reference (int). This is not the same as a dataRef (as returned by :py:func:`XPLMFindDataRef`.)
          It should only be used with :py:func:`XPLMUnregisterDataAccessor`.

 Data access callbacks:

 ===================== ==================== ==============================
 | inReadInt_f         inWriteInt_f         XPLMGetDatai_f/XPLMSetDatai_f
 | inReadFloat_f       inWriteFloat_f       XPLMGetDataf_f/XPLMSetDataf_f
 | inReadDouble_f      inWriteDouble_f      XPLMGetDatad_f/XPLMSetDatad_f
 | inReadIntArray_f    inWriteIntArray_f    XPLMGetDatavi_f/XPLMSetDatavi_f
 | inReadFloatArray_f  inWriteFloatArray_f  XPLMGetDatavf_f/XPLMSetDatavf_f
 | inReadData_f        inWriteData_f        XPLMGetDatab_f/XPLMSetDatab_f
 ===================== ==================== ==============================

 For example, to define a dataref ``myPlugin/dataItem``, which can be
 accessed as either an Integer or Float, use something like the following. Note
 we provide callbacks for `reading` as an integer or float, but the user can
 only read (not write) as a float (no idea if this might ever make sense in the real world.)

 ::
      
      XPLMRegisterDataAccessor('myPlugin/dataItem1', xplmType_Int | xplmType_Float,
                               MyReadIntCallback, MyWriteIntCallback,
                               MyReadFloatCallback, None,
                               None, None,
                               None, None,
                               None, None,
                               None, None,
                               0, 0)

.. py:function:: XPLMUnregisterDataAccessor(inDataRef: int) -> None:

    Use this routine to unregister any data accessors you may have registered.
    You unregister a data ref by the XPLMDataRef you get back from
    registration. Once you unregister a data ref, your function pointer will
    not be called anymore.

    For maximum compatibility, do not unregister your data accessors until
    final shutdown (when your XPluginStop routine is called). This allows other
    plugins to find your data reference once and use it for their entire time
    of operation.



Callbacks
+++++++++

.. py:function:: XPLMGetDatai_f(inRefcon: object) -> int
                 XPLMGetDataf_f(inRefcon: object) -> float
                 XPLMGetDatad_f(inRefcon: object) -> double

 Callback you provide to allow others to read your data ref. Note that
 you're passed the reference constant (originally provided with :func:`XPLMRegisterDataAccessor`)
 not the data ref: If you use the same callback function for more than one dataref
 you can use the reference constant to identify which one is being requested.

 ::

    def MyReadIntCallback(self, refCon):
        if refCon == 'param1':
            return int(self.param1)
        elif refCon == 'param2':
            return int(self.param2)
        raise ValueError("Unknown parameter: {}".format(refCon))
         

.. py:function:: XPLMSetDatai_f(inRefcon: object, inValue: int) -> None
                 XPLMSetDataf_f(inRefcon: object, inValue: float) -> None
                 XPLMSetDatad_f(inRefcon: object, inValue: double) -> None

 Callback you provide to allow others to set your data ref.

 ::

    def MyWriteIntCallback(self, refCon, value):
        if refCon == 'param1':
            self.param1 = int(value)
        elif refCon == 'param2':
            self.param2 = int(value)
        else:
            raise ValueError("Unknown parameter: {}".format(refCon))

    
.. py:function:: XPLMGetDatavi_f(inRefcon: object, outValues: list, inOffset: int, inMax: int) -> int
                 XPLMGetDatavf_f(inRefcon: object, outValues: list, inOffset: int, inMax: int) -> int

 Callback you provide to allow others to read your vector dataref.
 The callback semantics is the same as :func:`XPLMGetDatavi` and :func:`XPLMGetDatavf` (those routines
 just forward the request to your callback). If outValues is None, return the size of the array,
 ignoring inOffset and inMax.

 ::

      def MyReadIntVCallback(self, refCon, out, offset, maximum):
          if refCon == 'array1':
               if out is None:
                   return len(self.myarray1)
               out.extend(self.myarray1[offset:offset + maximum])
               return len(out)
          if refCon == 'array1':
               if out is None:
                   return len(self.myarray2)
               out.extend(self.myarray2[offset:offset + maximum])
               return len(out)
          raise ValueError("Unknown refCon: {}".format(refCon))

 .. note:: the use of ``extend()`` rather than simple assigment (e.g., ``out = self.myArray[offset: offset + maximum]``).
    The ``out`` parameter should be either a list (``[]``) or None. If it's a list, we use it
    to return the actual values (rather than merely returning the length of the data.) Because the calling
    function needs the value, we cannot change the python ``id()`` of the object. Simple assignment changes
    the id, so the calling function never gets the updated value. Using ``extend()`` (or ``append()``) will
    maintain the id of the ``out`` parameter, allowing the calling function to retrieve the values.
    

.. py:function:: XPLMSetDatavi_f(inRefCon: object, inValues: list, inOffset: int, inCount: int) -> None
                 XPLMSetDatavf_f(inRefCon: object, inValues: list, inOffset: int, inCount: int) -> None

 Callback you provide to allow other to write your vector dataref.
 The callback semantics is the same as :func:`XPLMSetDatavi` and :func:`XPLMSetDatavf` (those routines
 just forward the request to your callback). Values passed in are written into the
 dataref starting at inOffset. Up to inCount values are written; however if the values work write
 "off the end" of the dataref array, then fewer values are written. (This comment is more
 relevant for fixed-length C-language datastructures, so your python implementation can do
 whatever it likes.)

 ::

      def MyWriteIntVCallback(self, refCon, values, offset, count):
          # Note, if offset is larger than current array, we merely append, which may
          # not be as expected:
          #   '12345'   f('abc',  1, 3)  -> '1abc5'
          #   '12345'   f('abc',  2, 1)  -> '12a45'
          #   '12345'   f('abc',  5, 3)  -> '12345abc'
          #   '12345'   f('abc', 15, 3)  -> '12345abc'
          
          if refCon == 'array1':
              new_value = self.array1[:offset] + values[:count] + self.array1[offset + count:]
              self.array1 = new_value
          elif refCon == 'array2':
              new_value = self.array2[:offset] + values[:count] + self.array2[offset + count:]
              self.array2 = new_value
          else:
              raise ValueError("Unknown refCon: {}".format(refCon))

 
.. py:function:: XPLMGetDatab_f(inRefCon: object, outValues: list, inOffset: int, inMaxLength: int) -> int
                 XPLMSetDatab_f(inRefCon: object, inValues: list, inOffset: int, inMaxLength: int) -> None

 Callback you provide to read/write arbitrary data.
 The callback semantics are the same as :func:`XPLMGetDatab` and :func:`XPLMSetDatab`
 
Interfacing with DataRefEditor
******************************

The third-party `DataRefEditor plugin <http://www.xsquawkbox.net/xpsdk/mediawiki/DataRefEditor>`_
allows you to test your datarefs.

1. Create you datarefs in your XPluginStart function. (This is the recommended practice).
   
2. Register them in your XPluginEnable function::

     dre = xp.findPluginBySignature('xplanesdk.examples.DataRefEditor')
     xp.sendMessageToPlugin(dre, 0x01000000, 'myplugin/dataRef1')
     xp.sendMessageToPlugin(dre, 0x01000000, 'myplugin/dataRef2')

This way your datarefs will appear in the DataRefEditor.
     

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
  data reference; you must use the XPLMGet... and XPLMSet... APIs. With an
  owned data reference, the one owning data reference can manipulate the data
  reference's memory in any way it sees fit.

Shared data references solve two problems: if you need to have a data
reference used by several plugins but do not know which plugins will be
installed, or if all plugins sharing data need to be notified when that
data is changed, use shared data references.

.. py:function:: XPLMShareData(inDataName: str, inDataType: int, inDataChanged_f: DataChanged_f, inRefCon: object) -> int

 This routine connects a plug-in to shared data, creating the shared data if
 necessary. inDataName is a standard path for the data ref, and inDataType
 specifies the data type. This function will create the data if it does not
 exist. If the data already exists but the type does not match, an error is
 returned, so it is important that plug-in authors collaborate to establish
 public standards for shared data.

 If a notificationFunc is passed in and is not None, that notification
 function will be called whenever the data is modified. The notification
 refcon will be passed to it. This allows your plug-in to know which shared
 data was changed if multiple shared data are handled by one callback, or if
 the plug-in does not use global variables.
 
 A one is returned for successfully creating or finding the shared data; a
 zero if the data already exists but is of the wrong type.
                 

.. py:function:: XPLMUnshareData(inDataName: str, inDataType: int, inDataChanged_f: DataChanged_f, inRefCon: object) -> int

 This routine removes your notification function for shared data. Call it
 when done with the data to stop receiving change notifications. Arguments
 must match XPLMShareData. The actual memory will not necessarily be freed,
 since other plug-ins could be using it. Returns 0 if dataref is not found

.. py:function:: XPLMDataChanged_f(inRefCon: object) -> None

 Callback you provide to :func:`XPLMShareData` and :func:`XPLMUnshareData` which
 allows you to be notified when someone changes the shared dataref. Note, you
 do not get a callback for non-shared datarefs -- if you own the dataref, you'll
 know it's been changed because someone called your function to change it.)

 The reference constant you provide in XPLMShareData will be returned to you
 in the callback: that's one way you can tell which shared dataref was changed.

 ::

    def dataChanged(self, refCon):
        if refCon == 'param1':
           new_value = XPLMGetDatai(param1DataRefID)
           print("Someone changed param1 to {}".format(new_value))


Types
--------------------

.. data:: XPLMDataTypeID
   :annotation: bitfield used to identify the type of data

   .. py:data:: xplmType_Unknown
                :value: 0
   .. py:data:: xplmType_Int
             :value: 1
   .. py:data:: xpmlType_Float
                :value: 2
   .. py:data:: xpmlType_Double
                :value: 4
   .. py:data:: xpmlType_FloatArray
                :value: 8
   .. py:data:: xpmlType_IntArray
                :value: 16
   .. py:data:: xpmlType_Data
                :value: 32
