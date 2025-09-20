Callbacks and Refcons
---------------------

Many functions include a "reference constant" (refCon) as an input parameter.
In every case, this reference constanct can be any python object (int, float, dict,
etc.) and will be passed to your callback function. If the object is mutable
(e.g., dict, list), you'll be able to change its value within, or outside of the
callback and that new value will be maintained.

For mutable objects, remember you need to keep the same object, that is its ``id()``
must not change.

* That means for ``dict`` objects, you can::

    >>> refCon = {}
    >>> id(refCon)
    15739916928
    >>> refCon['a'] = 1
    >>> refCon['b'] = 2
    >>> refCon.update({'c': 3})
    >>> refCon.pop('b')
    2
    >>> refCon.clear()
    >>> id(refCon)
    15739916928
    
  But, a new assignment changes it:
  
    >>> refCon = {'z': 26}
    >>> id(refCon)
    5116507392
  
* Similarly for lists:
  
    >>> refCon = []
    >>> id(refCon)
    15529489344
    >>> refCon.append('world')
    >>> refCon.append('hello')
    >>> refCon.reverse()
    >>> refCon.remove('hello')
    >>> refCon.insert(0, 'Hello')
    >>> id(refCon)
    15529489344
  
  But, a new assignment changes it:
  
    >>> refCon = ['new string']
    >>> id(refCon)
    1650739251
  
Passing an int, float or string variable as a reference constant *does not* allow
you to change it later, because any change to the variable results is a new ``id()``

  >>> refCon = 4
  >>> id(refCon)
  529489344
  >>> refCon += 1
  >>> id(refCon)
  881650739

In a few cases, the reference constant MUST BE an object which remains in scope
because the same object MUST BE used during a subsequent lookup. For example,

* WORKS (the same object)::

   self.refCon = {'a': 1}
   XPLMDisplay.XPLMRegisterDrawCallback(..., self.refCon)
   ...
   XPLMDisplay.XPLMUnregisterDrawCallback(..., self.refCon)

* DOES NOT WORK (equivalent, but not the same object)::

   XPLMDisplay.XPLMRegisterDrawCallback(..., {'a': 1})
   ...
   XPLMDisplay.XPLMUnregisterDrawCallback(..., {'a': 1})

If no subsequent lookup is required, then there is no requirement to have
the object stay in scope.

.. Warning:: If the object needs to be in scope and it's not, the *sim will crash*, and
             in the most ugly and un-helpful way. [#f1]_

**RefCon REQUIRES SAME OBJECT**::

 XPLMDisplay: XPLMRegisterDrawCallback + XPLMUnregisterDrawCallback
 XPLMDisplay: XPLMRegisterKeySniffer + XPLMUnregisterKeySniffer
 XPLMProcessing: XPLMRegisterFlightLoopCallback + XPLMUnregisterFlightLoopCallback
 XPLMUtilities: XPLMRegisterCommandHandler + XPLMUnregisterCommandHandler      


**RefCon CAN BE ANYTHING**::

 XPLMDataAccess: XPLMRegisterDataAccessor
 XPLMDisplay: XPLMRegisterHotKey
 XPLMDisplay: XPLMCreateWindowEX (mousewheel, mouseclick, rightclick, cursor functions)
 XPLMDisplay: XPLMSetWindowRefCon
 XPLMMap: XPLMRegisterMapCreationHook
 XPLMPlanes: XPLMPlanesAvailable
 XPLMPlanes: XPLMAcquirePlanes
 XPLMScenery: XPLMLoadObjectAsync


.. rubric:: Footnotes

.. [#f1] My consulting rates for these bugs is $500/hour.

