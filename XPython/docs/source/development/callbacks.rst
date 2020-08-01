Callbacks and Refcons
---------------------

Many functions include a "reference constant" (refCon) as an input parameter.
In every case, this reference constanct can be any python object (int, float, dict,
etc.) and will be passed to your callback function. If the object is mutable
(e.g., dict, list), you'll be able to change its value within, or outside of the
callback and that new value will be maintained.

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

