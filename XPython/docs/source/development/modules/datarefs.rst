Easy Datarefs
=============

.. py:module:: utils.datarefs


To use:
::

  from XPPython3.utils import datarefs

-or-

..  code::

  from XPPython3.utils.datarefs import find_dataref, create_dataref

To make creating and using datarefs easier, XPPython3 includes a module which
mimics the way Xlua creates and define datarefs. While originally this was
included to make it easier for Aircraft designers to transition from Xlua to
python, there is no requirement to use other xlua-related code or restrict
use to Aircraft plugins. (This module can be used with or without other python-xlua
inspired modules).

There are two functions:

 * :py:func:`find_dataref`

 * :py:func:`create_dataref`


Both functions return a :py:class:`DataRef` instance.

.. py:class:: DataRef

  Result of :py:func:`find_dataref` and :py:func:`create_dataref`. You should
  not create an instance of this class yourself.

  Useful properties of DataRef are:

  .. py:property:: .value

     Gets or sets the current datatype-specific value of the dataref. This may be
     a string, an int or float array, an int or float.

     For gets, the data is obtained from
     X-Plane on access. Cache this value if you want to access it multiple
     times during the same frame.

     For sets, the data is immediately updated in X-Plane.::

       >>> ref = datarefs.create_dataref('xppython/test/number', 'number')
       >>> ref.value
       0
       >>> ref.value = 45.6
       >>> ref.value
       45.6

     Note that for
     array datatypes, we update `only` those elements in the slice.::

       >>> ref = datarefs.create_dataref('xppython/test/short_array', 'array[5]')
       >>> ref.value
       [0, 0, 0, 0, 0]
       >>> ref.value[2] = 45
       >>> ref.value
       [0, 0, 45, 0, 0]
       >>> ref.value[2:5] = [1, 2, 3]
       >>> ref.value
       [0, 0, 1, 2, 3]

     It is an error to not provide enough elements for the slice.::

       >>> ref.value[2:5] = [1, 2]
       RuntimeError: Not enough elements(2) in value to set dataref slice (3)

     If you provide too many elements, we'll truncate the list and accept the result.::

       >>> ref.value[2:5] = [4, 3, 2, 1]
       >>> ref.value
       [0, 0, 4, 3, 2]

     Arrays call also be referenced (set or get) `without` the ``.value`` property, as long
     as you use an index or slice.::

       >>> ref[0:]
       [0, 0, 4, 3, 2]
       >>> ref[2:5] = [1, 2, 3]
       >>> ref.value
       [0, 0, 1, 2, 3]

     Of course setting ``ref`` without an index or slice simply updates the python
     variable to `become` an array/list: Negating the dataref.::

       >>> ref = [1, 2, 3, 4]
       >>> ref.value
       AttributeError: 'list' object has no attribute 'value'
     
     We treat ``ref.value`` without an index or slice similarly, but allow you
     to specify any number of elements, truncating if you provide too many::

       >>> ref.value
       [0, 0, 4, 3, 2]
       >>> ref.value = [45]
       >>> ref.value
       [45, 0, 4, 3, 2]
       >>> ref.value = [10, 9, 8, 7, 6, 5, 4, 3, 2, 1]
       >>> ref.value
       [10, 9, 8, 7, 6]
     
     For ``string`` types, we treat slice a little differently, because ``.value`` is a simple
     python string which does not permit item assignment::

       >>> ref.value
       Hello World
       >>> ref.value[6:]
       World
       >>> ref.value[6:] = "Universe"
       TypeError: 'str' object does not support item assignment
       >>> ref.value = "Hello Universe"
       >>> ref.value
       Hello Universe

     Unlike array types, if you provide fewer elements to ``.value`` than the dataref size for strings
     we'll zero-fill:

       >>> ref.value = "Hello"
       >>> ref.value
       Hello

     Note that ``string`` datarefs can also be used without ``.value``, if you provide an index or slice::

       >>> ref[0]
       H
       >>> ref[0] = 'h'
       >>> ref.value
       hello

     In this manner, we will zero-fill `only` to the width provided by in the slice. This
     also allows you set string values `within` a longer dataref such as ``sim/cockpit2/tcas/targets/icao_type``.::

       >>> ref = datarefs.find_dataref('sim/cockpit2/tcas/targets/icao_type')
       >>> ref.value
       'C172'
       >>> ref[0:8]
       'C172'
       >>> ref[8:16]
       ''
       >>> ref[8:16] = 'BE35'
       >>> ref[8:16]
       'BE35'

     The above example will zero-fill the 8-element slice, and apply the 4-character string only to the
     first four elements starting at index 8. As before, if you provide more values than are required for
     the slice, we'll truncate without error. If you provide too few, we'll zero-fill.
       

  .. py:property:: .bytes

     For `non-string` datarefs, this is the same as the ``value`` property, which may be
     an int, float, array of ints or floats. For ``string``
     datarefs, this returns an array of (python) bytes. For example::

       >>> ref = datarefs.create_dataref('xppython3/test/string', 'string')
       >>> ref.value = "Hello World"
       >>> ref.value
       Hello World
       >>> ref.bytes
       [72, 101, 108, 108, 111, 32, 87, 111, 114, 108, 100, 0, 0, 0, ..., 0]

     This property is not commonly used.

  .. py:property:: .name

     The string name of the dataref::

       >>> ref.datarefs.create_datarefs('xppython3/test/string', 'string')
       >>> ref.name
       'xppython3/test/string'

  .. py:property:: .types

     The X-Plane data type, a list containing one or more of the strings
     'int', 'float', 'double', 'float_array', 'int_array', 'data'. Note that ``string``
     types are implemented using X-Plane ``data`` datarefs.

  .. py:property:: .dref

     Reference to the internal X-Plane XPLMDataRef. This would be the result
     of :func:`xp.findDataRef` and can be used with :func:`xp.getDatai` and related
     data access functions.

  .. Note::
    For array types, you can drop '.value' for accessing
    indexed data: e.g., ``inverter_on[0]`` is similar to ``inverter_on.value[0]``:

    * ``inverter_on[0]`` queries X-Plane for the single valued item

    * ``inverter_on.value[0]`` retrieves from X-Plane the list of values, and
      then python returns the ``[0]`` list element.


Functions
---------

.. py:function:: find_dataref(name: str)

  :param str name: Name of existing dataref to find
  :return: :py:class:`DataRef`                   

  Use ``find_dataref`` to retrieve and store the accessor which provides
  the interface to a dataref (either one you created, or an existing dataref)::
  
    >>> num_batteries = find_dataref("sim/aircraft/electrical/num_batteries")
    >>> inverter_on = find_dataref("sim/cockpit2/electrical/inverter_on")
    >>> fuel_pump_on_zero = find_dataref("sim/cockpit/engine/fuel_pump_on[0]")
    >>> print(fuel_pump_on_zero)
    DataRef: 'sim/cockpit/engine/fuel_pump_on[0] [int_array]'
  
  There's no need to determine the datatype as we'll figure it out.
  The result of ``find_dataref()`` is a handle, that is, the data `has not been retrieved yet`.
  
  If the dataref is not found, a ValueError exception is raised.

  To get the current value, simple access the ``.value`` attribute (see :class:`DataRef` above for details).
  
    >>> if num_batteries.value > 1:
    ...     do_something()
    ...
  
  If the dataref is an array type, you'll (probably) want to index it::

    >>> if inverter_on.value[0] and inverter_on.value[1]:
    ...    do_something()
    ... elif inverter_on.value[0]:
    ...    do_something_else()
    ...

  You can `find` a specific array index, and use it as a single-value dataref::

    >>> fuel_pump_on_zero = find_dataref("sim/cockpit/engine/fuel_pump_on[0]")
    >>> fuel_pump_on_zero.value
    1
    >>> fuel_pump_on_zero.value = 0

  Note, however, if you created the dataref as read-only, you can change its
  value `only` if you access it through the non-specific index dataref (e.g., ``ref1`` below).::

    >>> ref1 = create_dataref('xppython3/test/array', 'array[16]', callback=False)
    >>> ref2 = find_dataref('xppython3/test/array[4]')
    >>> ref1[4] = 47
    >>> ref2.value
    47
    >>> ref2.value = 48
    >>> ref1[4] == 47
    True

  Attempting to set it through the specific-index version (``ref2``) will be
  silently ignored.
    

.. py:function::  create_dataref(name:str, dataRefType:str="number"|"array"|"string", callback:None|False|Callable=None)
                  
  :param str name: Name of dataref to be created
  :param str dataRefType: See table for possible values
  :param Callable callback: ``None`` or function to be called whenever dataref is written to, ``False`` to make readonly
  :return: :py:class:`DataRef`                   

  The ``dataRefType`` is a string which defaults to "``number``" if not provided.:
  
  .. table::
     :align: left

     +----------------+------------------------------------------------+
     | ``'number'``   |creates int/float/double type                   |
     +----------------+------------------------------------------------+
     | ``'array[x]'`` |creates int_array/float_array of dimension `x`  |
     |                |(x must be a number)                            |
     +----------------+------------------------------------------------+
     | ``'string'``:  |creates data                                    |
     +----------------+------------------------------------------------+
     
  For ``callback``:
  
  * If a function, that function will be called, with no parameters, after the dataref is written to.
  * If ``None`` (the default), dataref is writable, but no callback: data is simply stored and retrieved.
  * If ``False``, dataref is read-only to all plugins `except` the plugin which created it.
  
  For example, to create a numeric dataref which will store a numeric value::
  
    >>> dr_engaged = create_dataref('test/gadget/engaged')
    >>> print(dr_engaged.value)
    0
    >>> dr_engaged.value = 42
    >>> print(dr_engaged.value)
    42

  For numeric datarefs, We've actually created a dataref within X-Plane which supports integers
  and floats, so other plugins can use either to access the current value. To the creating plugin (i.e., `your`
  plugin) we'll return an int if you set and int, a float if you set a float. If you set a non-integer float
  and another plugin accesses it using :func:`xp.getDatai`, we'll return an integer representation of your float.
  
  To create an array of 6 (numeric) elements, provide the dimension::
  
    >>> dr_tank_capacity = create_dataref('test/fuel_tank/capacity', 'array[6]')
    >>> print(dr_tank_capacity.value)
    [0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
    >>> print(dr_tank_capacity[3] = 42
    >>> print(dr_tank_capacity.value)
    [0.0, 0.0, 0.0, 42.0, 0.0, 0.0]
    >>> print(dr_tank_capacity[3].value)
    42
    >>> print(dr_tank_capacity.value[3])
    42    
  
  Strings are easy, and we automatically create a string of maximum 512 characters::
  
    >>> dr_n_number = create_dataref('test/n_number', 'string')
    >>> print(dr_n_number.value)
    ''
    >>> dr_n_number.value = 'N20113'
    >>> print(dr_n_number.value)
    'N20113'
  
  
  As with other callbacks within XPPython3, if your dataref callback is
  a method of PythonInterface, it should also have ``self`` parameter, which
  will be available within you callback. For example::
  
    class PythonInterface(EasyPython):
        def __init__(self):
            super().__init__()
            self.name = "foobar"
  
        def dataref_callback(self):
            print("someone the dataref created in {self.name}")
  
        def onStart(self):
            datarefs.create_dataref("test/fuel_tank/capacity', 'array[6]', self.dataref_callback)
  
  You will not get any indication `within the callback` as to what was changed. You could
  determine this information by storing previous value within the class (e.g., ``self.capacity_previous``)
  and then making the comparison within the callback, as you'll have access to `self`.
  
