:orphan:
   
XPPythonGetDicts()
------------------

.. Warning:: Notes in progress. This should give you an idea of what these internal dicts are & how they're used.

* commandCallbacks and commandRefcons

  Key: integer index  

  Value: tuple, ("<PluginFile.py>", <XPLMCommandRef capsule>, <command handler python method>, inBefore=0/1, <refCon object>)  

  Purpose:  
    Rather than providing X-Plane your command handler directly, we provide X-Plane information to call
    XPPython3, and then WE form the python call to your command handler. To do this
    we store information about your callback in `commandCallback` and `commandRefcons`, and substitute
    and internal callback function and a serial integer as the refCon X-Plane will see.

    So your python :code:`XPLMRegisterCommandHandler(inCommand, inHandler, inBefore, inRefcon)`
    becomes C-code similar to::

      ++idx
      commandCallback[<idx>] = (<plugin>, inCommand, inHandler, inBefore, inRefcon)
      commandRefcons[<idx>] =  inCommand
      XPLMRegisterCommandHandler(inCommand, internalCommandCallback, inBefore, <idx>)

    On command execution, X-Plane calls our callback:  
      :code:`internalCommandCallback(inCommand, inPhase, <idx>)`
    We lookup <idx> in commandCallbacks and call your:  
      :code:`inHandler(inCommand, inPhase, inRefcon)`

    On XPLMUnregisterCommandHandler(inCommand, inHandler, inBefore, inRefcon)
    We need to convert back to what we registered as the command handler, so we need
    to get the <idx>, which is from commandRefcons[inCommand]  

       :code:`XPLMUnregisterCommandHandler(inCommand, internalCommandCallback, inBefore, <idx>)`

* commandRefcons

  Key: inCommand  

  Value: <index> into commandCallbacks  

  Purpose: Used with commandCallbacks (see above)

* menus

  Key: integer index  

  Value: tuple, (<plugin>, Display String, <XPLMMenuIDRef>parent, menuItemNumber, <menu handler python method>, <refCon>)  

  Purpose:  

    Similar to commandCallbacks (described above), XPPython interecepts calls to menus.

* menuRefs

  Key: <XPLMMenuIDRef> 

  Value: integer index into menus[] dict

* fl (flightloop)

  Key: integer index

  Value: tuple, (<plugin>, <callback python method> <interval>, <refcon>)

  Purpose:

    Similar to commandCallbacks (described above), XPPython intercepts flightLoopCallbacks)

* flRev

  key: tuple: (<plugin>, <callback>, <refconAddr>)

  Value: integer index into fl[] dict

