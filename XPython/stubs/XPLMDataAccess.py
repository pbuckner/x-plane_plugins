# XPLMDataTypeID bitfield is used to identify the type of data:
xplmType_Unknown = 0  #: Data of a type the current XPLM doesn't do.
xplmType_Int = 1  #: A single 4-byte integer, native endian.
xplmType_Float = 2  #: A single 4-byte float, native endian.
xplmType_Double = 4  #: A single 8-byte double, native endian.
xplmType_FloatArray = 8  #: An array of 4-byte floats, native endian.
xplmType_IntArray = 16  #: An array of 4-byte integers, native endian.
xplmType_Data = 32  # A: variable block of data.
"""


PUBLISHING YOUR PLUGINS DATA
****************************
SHARING DATA BETWEEN MULTIPLE PLUGINS
*************************************
"""


def XPLMFindDataRef(inDataRefName: str) -> int:
    """
    Given a string that names the data ref, this routine looks up the
    actual opaque integer XPLMDataRef that you use to read and write the data. The
    string names for datarefs are published on the X-Plane SDK web site. (https://developer.x-plane.com/datarefs/).

    This function returns handle to the dataref, or None if the data ref
    cannot be found.

    .. NOTE:: this function is relatively expensive; save the XPLMDataRef this
     function returns for future use. Do not look up your data ref by string
     every time you need to read or write it.
    """
    return int


def XPLMCanWriteDataRef(inDataRef: int) -> bool:
    """
    Given a data ref, this routine returns True if you can successfully set the
    data, False otherwise. Some datarefs are read-only.
    """
    return bool


def XPLMIsDataRefGood(inDataRef: int) -> bool:
    """
    .. Warning:: This function is deprecated and should not be used. Datarefs are
     valid until plugins are reloaded or the sim quits. Plugins sharing datarefs
     should support these semantics by not unregistering datarefs during
     operation. (You should however unregister datarefs when your plugin is
     unloaded, as part of general resource cleanup.)

    This function returns whether a data ref is still valid. If it returns
    False, you should refind the data ref from its original string. Calling an
    accessor function on a bad data ref will return a default value, typically
    0 or 0-length data.
    """
    return bool


def XPLMGetDataRefTypes(inDataRef: int) -> int:
    """
    This routine returns the XPLMDataTypeID of the data ref for accessor use. If a data
    ref is available in multiple data types, they will all be returned. (bitwise OR'd together).

     * xplmType_Unknown
     * xplmType_Int
     * xplmType_Float
     * xplmType_Double
     * xplmType_IntArray
     * xplmType_FloatArray
     * xplmType_Data

    """
    return int


def XPLMGetDatai(inDataRef):
    """
    Read an integer data ref and return its value. The return value is the
    dataref value or 0 if the dataref is NULL or the plugin is
    disabled.
    """
    return int


def XPLMSetDatai(inDataRef, inValue):
    """
    Write a new value to an integer data ref. This routine is a no-op if the
    plugin publishing the dataref is disabled, the dataref is invalid, or the
    dataref is not writable.
    """


def XPLMGetDataf(inDataRef):
    """
    Read a single precision floating point dataref and return its value. The
    return value is the dataref value or 0.0 if the dataref is invalid/NULL or
    the plugin is disabled.
    """
    return float


def XPLMSetDataf(inDataRef, inValue):
    """
    Write a new value to a single precision floating point data ref. This
    routine is a no-op if the plugin publishing the dataref is disabled, the
    dataref is invalid, or the dataref is not writable.
    """


def XPLMGetDatad(inDataRef):
    """
    Read a double precision floating point dataref and return its value. The
    return value is the dataref value or 0.0 if the dataref is invalid/NULL or
    the plugin is disabled.
    """
    return float


def XPLMSetDatad(inDataRef, inValue):
    """
    Write a new value to a double precision floating point data ref. This
    routine is a no-op if the plugin publishing the dataref is disabled, the
    dataref is invalid, or the dataref is not writable.
    """


def XPLMGetDatavi(inDataRef, outValues, inOffset, inMax):
    """
    Read a part of an integer array dataref. If you pass None for outVaules,
    the routine will return the size of the array, ignoring inOffset and inMax.

    If outValues is list, then up to inMax values are copied from the
    dataref into outValues, starting at inOffset in the dataref. If inMax +
    inOffset is larger than the size of the dataref, less than inMax values
    will be copied. The number of values copied is returned.

    Note: the semantics of array datarefs are entirely implemented by the
    plugin (or X-Plane) that provides the dataref, not the SDK itself; the
    above description is how these datarefs are intended to work, but a rogue
    plugin may have different behavior.
    """
    return int  # of items


def XPLMSetDatavi(inDataRef, inValues, inoffset, inCount):
    """
    Write part or all of an integer array dataref. The values passed by
    inValues are written into the dataref starting at inOffset. Up to inCount
    values are written; however if the values would write "off the end" of the
    dataref array, then fewer values are written.

    Note: the semantics of array datarefs are entirely implemented by the
    plugin (or X-Plane) that provides the dataref, not the SDK itself; the
    above description is how these datarefs are intended to work, but a rogue
    plugin may have different behavior.
    """


def XPLMGetDatavf(inDataRef, outValues, inOffset, inMax):
    """
    Read a part of a single precision floating point array dataref. If you pass
    None for outVaules, the routine will return the size of the array, ignoring
    inOffset and inMax.

    If outValues is list, then up to inMax values are copied from the
    dataref into outValues, starting at inOffset in the dataref. If inMax +
    inOffset is larger than the size of the dataref, less than inMax values
    will be copied. The number of values copied is returned.

    Note: the semantics of array datarefs are entirely implemented by the
    plugin (or X-Plane) that provides the dataref, not the SDK itself; the
    above description is how these datarefs are intended to work, but a rogue
    plugin may have different behavior.
    """
    return int  # of items


def XPLMSetDatavf(inDataRef, inValues, inoffset, inCount):
    """
    Write part or all of a single precision floating point array dataref. The
    values passed by inValues are written into the dataref starting at
    inOffset. Up to inCount values are written; however if the values would
    write "off the end" of the dataref array, then fewer values are written.

    Note: the semantics of array datarefs are entirely implemented by the
    plugin (or X-Plane) that provides the dataref, not the SDK itself; the
    above description is how these datarefs are intended to work, but a rogue
    plugin may have different behavior.
    """


def XPLMGetDatab(inDataRef, outValue, inOffset, inMaxBytes):
    """
    Read a part of a byte array dataref. If you pass None for outVaules, the
    routine will return the size of the array, ignoring inOffset and inMax.

    If outValues is list, then up to inMax values are copied from the
    dataref into outValues, starting at inOffset in the dataref. If inMax +
    inOffset is larger than the size of the dataref, less than inMax values
    will be copied. The number of values copied is returned.

    Note: the semantics of array datarefs are entirely implemented by the
    plugin (or X-Plane) that provides the dataref, not the SDK itself; the
    above description is how these datarefs are intended to work, but a rogue
    plugin may have different behavior.
    """
    return int  # of items


def XPLMSetDatab(inDataRef, inValue, inOffset, inLength):
    """
    Write part or all of a byte array dataref. The values passed by inValues
    are written into the dataref starting at inOffset. Up to inCount values are
    written; however if the values would write "off the end" of the dataref
    array, then fewer values are written.

    Note: the semantics of array datarefs are entirely implemented by the
    plugin (or X-Plane) that provides the dataref, not the SDK itself; the
    above description is how these datarefs are intended to work, but a rogue
    plugin may have different behavior.
    """


def XPLMGetDatai_f(inRefcon):
    """
    Data accessor callback you provide to allow others to read the dataref.
    Return dataref value.
    """
    return int


def XPLMSetDatai_f(inRefcon, inValue):
    """
    Data accessor callback you provide to allow others to write the dataref.
    """


def XPLMGetDataf_f(inRefcon):
    """
    Data accessor callback you provide to allow others to read the dataref.
    Return dataref value.
    """
    return float


def XPLMSetDataf_f(inRefcon, inValue):
    """
    Data accessor callback you provide to allow others to write the dataref.
    """


def XPLMGetDatad_f(inRefcon):
    """
    Data accessor callback you provide to allow others to read the dataref.
    Return dataref value.
    """
    pass


def XPLMSetDatad_f(inRefcon, inValue):
    """
    Data accessor callback you provide to allow others to write the dataref.
    """


def XPLMGetDatavi_f(inRefcon, outValues, inOffset, inMax):
    """
    Data accessor callback you provide to allow others to read the dataref.
    The callback semantics is the same as the XPLMGetDatavi (that routine just
    forwards the request to your callback).
    Don't forget to handle the outBuffer being set to None.
    """
    return int  # of items

def XPLMSetDatavi_f(inRefcon, inValues, inOffset, inCount):
    """
    Data accessor callback you provide to allow others to write the dataref.
    The callback semantics is the same as the XPLMSetDatavi (that routine just
    forwards the request to your callback).
    """


def XPLMGetDatavf_f(inRefcon, outValues, inOffset, inMax):
    """
    Data accessor callback you provide to allow others to read the dataref.
    The callback semantics is the same as the XPLMGetDatavf (that routine just
    forwards the request to your callback).
    Don't forget to handle the outBuffer being set to None.
    """
    return int  # of items


def XPLMSetDatavf_f(inRefcon, inValues, inOffset, inCount):
    """
    Data accessor callback you provide to allow others to write the dataref.
    The callback semantics is the same as the XPLMSetDatavf (that routine just
    forwards the request to your callback).
    """


def XPLMGetDatab_f(inRefcon, outValues, inOffset, inMaxLength):
    """
    Data accessor callback you provide to allow others to read the dataref.
    The callback semantics is the same as the XPLMGetDatab (that routine just
    forwards the request to your callback).
    Don't forget to handle the outBuffer being set to None.
    """
    return int  # of items


def XPLMSetDatab_f(inRefcon, inValue, inOffset, inLength):
    """
    Data accessor callback you provide to allow others to write the dataref.
    The callback semantics is the same as the XPLMSetDatab (that routine just
    forwards the request to your callback).
    """


def XPLMRegisterDataAccessor(inDataName, inDataType, inIsWritable,
                             inReadInt_f, inWriteInt_f,
                             inReadFloat_f, inWriteFloat_f,
                             inReadDouble_f, inWriteDouble_f,
                             inReadIntArray_f, inWriteIntArray_f,
                             inReadFloatArray_f, inWriteFloatArray_f,
                             inReadData_f, inWriteData_f,
                             inReadRefcon, inWriteRefcon):
    """
    This routine creates a new item of data that can be read and written. Pass
    in the data's full name for searching, the type(s) of the data for
    accessing, and whether the data can be written to. For each data type you
    support, pass in a read accessor function and a write accessor function if
    necessary. Pass None for data types you do not support or write accessors
    if you are read-only.

    You are returned a data ref for the new item of data created. You can use
    this data ref to unregister your data later or read or write from it.

    Data access callbacks:

    ===================== ==================== ==============================
    | inReadInt_f         inWriteInt_f         XPLMGetDatai_f/XPLMSetDatai_f
    | inReadFloat_f       inWriteFloat_f       XPLMGetDataf_f/XPLMSetDataf_f
    | inReadDouble_f      inWriteDouble_f      XPLMGetDatad_f/XPLMSetDatad_f
    | inReadIntArray_f    inWriteIntArray_f    XPLMGetDatavi_f/XPLMSetDatavi_f
    | inReadFloatArray_f  inWriteFloatArray_f  XPLMGetDatavf_f/XPLMSetDatavf_f
    | inReadData_f        inWriteData_f        XPLMGetDatab_f/XPLMSetDatab_f
    ===================== ==================== ==============================

    """
    return int  # XPLMDataRef


def XPLMUnregisterDataAccessor(inDataRef):
    """
    Use this routine to unregister any data accessors you may have registered.
    You unregister a data ref by the XPLMDataRef you get back from
    registration. Once you unregister a data ref, your function pointer will
    not be called anymore.

    For maximum compatibility, do not unregister your data accessors until
    final shutdown (when your XPluginStop routine is called). This allows other
    plugins to find your data reference once and use it for their entire time
    of operation.
    """


def XPLMDataChanged_f(inRefcon):
    """
    An XPLMDataChanged_f is a callback that the XPLM calls whenever any other
    plug-in modifies shared data. A refcon you provide is passed back to help
    identify which data is being changed. In response, you may want to call one
    of the XPLMGetDataxxx routines to find the new value of the data.
    """


def XPLMShareData(inDataName, inDataType,
                  inNotificationFunc_f, inNotificationRefcon):
    """
    This routine connects a plug-in to shared data, creating the shared data if
    necessary. inDataName is a standard path for the data ref, and inDataType
    specifies the type. This function will create the data if it does not
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

    inNotificationFunc_f : XPLMDataChanged_f
    """
    return int  # 1 on success, 0 if data already exists but is of the wrong type


def XPLMUnshareData(inDataName, inDataType,
                    inNotificationFunc_f, inNotificationRefcon):
    """
    This routine removes your notification function for shared data. Call it
    when done with the data to stop receiving change notifications. Arguments
    must match XPLMShareData. The actual memory will not necessarily be freed,
    since other plug-ins could be using it.
    """
    return int  # 0 if cannot find dataref
