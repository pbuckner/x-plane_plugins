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

  See example below for use.
  
Functions
---------

.. py:function:: find_dataref(name)

  :param name str: Name of existing dataref to find
  :return: :py:class:`DataRef`                   

  Use ``find_dataref`` to retrieve and store the accessor which provides
  the interface to a dataref::
  
    >>> num_batteries = find_dataref("sim/aircraft/electrical/num_batteries")
    >>> inverter_on = find_dataref("sim/cockpit2/electrical/inverter_on")
    >>> fuel_pump_on_zero = find_dataref("sim/cockpit/engine/fuel_pump_on[0]")
  
  There's no need to determine the datatype as we'll figure it out.
  The result of ``find_dataref()`` is a handle: `data has not been retrieved yet`.
  
  If the dataref is not found, a ValueError exception is raised.
  
  To get the current value, simple access the ``.value`` attribute:
  
    >>> if num_batteries.value > 1:
    ...     do_something()
    ...
  
If the dataref is an array type, you'll (probably) want to index it::

  >>> if inverter_on.value[0] and inverter_on.value[1]:
  ...    do_something()
  ... elif inverter_on.value[0]:
  ...    do_something_else()
  ...

The values `are not cached` and are looked up immediately.
While this is fast, if you're going to access the same dataref
many times within the same frame, it is preferable to assign the
retrieved value to a local variable to avoid redundant lookups.

.. Note::
   For array types, you can drop '.value' for accessing
   indexed data: e.g., ``inverter_on[0]`` is similar to ``inverter_on.value[0]``:

  * ``inverter_on[0]`` queries X-Plane for the single valued item

  * ``inverter_on.value[0]`` retrieves from X-Plane the list of values, and
    then python returns the ``[0]`` list element.

The set the dataref value, simply assign a new value::

  >>> is_paused = find_dataref('sim/time/paused')
  >>> is_paused.value
  0
  >>> sim_speed = find_dataref('sim/time/sim_speed')
  >>> sim_speed.value
  1
  >>> sim_speed.value = 0
  >>> is_paused.value
  1

Data is immediately updated in X-Plane.

If the dataref is an array type, and you specified it
with an index (as we did with ``fuel_pump_on_zero`` above), we'll correctly
set the single value.

Otherwise, you get the full array, with which you can individually
change any element.

  >>> inverter_on.value
  [0, 0]
  >>> inverter_on.value[1] = 1
  >>> inverter_on.value
  [0, 1]

Again, you can forgo the ``.value`` attribute for addressing
individual array elements (but you still need it for the whole
array)::

  >>> inverter_on[1] = 1
  >>> inverter_on.value
  [0, 1]


.. py:function::  create_dataref(name:str, dataRefType:str="number"|"array"|"string", callback:None|Callable=None)
                  
  :param name str: Name of dataref to be created
  :param dataRefType str: See table for possible values
  :param: callback Callable: function to be called whenever dataref is written to                          
  :return: :py:class:`DataRef`                   

  The ``dataRefType`` is a string which defaults to "``number``" if not provided.:
  
  .. table::
     :align: left

     +----------------+------------------------------------------------+
     | ``'number'``   | creates int/float/double type                  |
     +----------------+------------------------------------------------+
     | ``'array[x]'`` | creates int_array/float_array of dimension `x` |
     +----------------+------------------------------------------------+
     | ``'string'``:  | creates data                                   |
     +----------------+------------------------------------------------+
     
  For ``callback``:
  
  * If a function, that function will be called, with no parameters, after the dataref is written to.
  * If None (the default), dataref is writable, but no callback: data is simply stored and retrieved.
  
For example, to create a numeric dataref which will store a numeric value::

  >>> dr_engaged = create_dataref('test/gadget/engaged')
  >>> print(dr_engaged.value)
  0
  >>> dr_engaged.value = 42
  >>> print(dr_engaged.value)
  42

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

Strings are easy::

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
