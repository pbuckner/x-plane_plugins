XPPythonGetDicts()
==================

.. Warning:: Notes in progress. This should give you an idea of what these internal dicts are & how they're used.

In order to fully support the X-Plane SDK, and provide a mapping between the Python and C-based API,
XPPython plugin uses a number of internal (python) dictionaries.

You can access these dictionaries using :py:func:`XPPython.XPPythonGetDicts`.

Technically, they're internal and are subject to change, but they can be particularly useful
during debugging.

The result of :py:func:`XPPython.XPPythonGetDicts` is a dictionary of these dictionaries, similar to::

  {'commandCallbacks':
       {54: ('/Resources/plugins/XPPython3/I_PI_Updater.py',
             <capsule object "XPLMCommandRef" at 0x7ff2bf6a42a0>,
             <bound method PythonInterface.updatePython of <XPPython3.I_PI_Updater.PythonInterface object at 0x7ff312547640>>,
             1,
             ''),
        55: ('/Resources/plugins/XPPython3/I_PI_Updater.py',
             <capsule object "XPLMCommandRef" at 0x7ff2ca63ccc0>,
             <bound method PythonInterface.togglePip of <XPPython3.I_PI_Updater.PythonInterface object at 0x7ff312547640>>,
             1,
             ''),
        59: ('/Resources/plugins/PythonPlugins/PI_SeeAndAvoid.py',
             <capsule object "XPLMCommandRef" at 0x7ff2bff3adb0>,
             <bound method PythonInterface.CommandHandler of <PythonPlugins.PI_SeeAndAvoid.PythonInterface object at 0x7ff2bff3ad30>>,
             0,
             []),
        60: ('/Resources/plugins/PythonPlugins/PI_SeeAndAvoid.py',
             <capsule object "XPLMCommandRef" at 0x7ff2bff3ad80>,
             <bound method PythonInterface.CommandHandler of <PythonPlugins.PI_SeeAndAvoid.PythonInterface object at 0x7ff2bff3ad30>>,
             0,
             []),
  },
   'commandRefcons':
       {140680570225312: 54,
        140680579228848: 58,
        140680579231104: 60,
        140680579231152: 59,
        140680758437808: 57},
   'drawCallbackIDs': {2227346048: 3},
   'drawCallbacks':
       {3: ('/Resources/plugins/PythonPlugins/PI_SeeAndAvoid.py',
            <bound method PythonInterface.drawFlightFollowing of <PythonPlugins.PI_SeeAndAvoid.PythonInterface object at 0x7ff2bff3ad30>>,
            50,
            0,
            0)},
   'errCallbacks': {},
   'hotkeyIDs': {},
   'hotkeys': {},
   'keySniffCallbacks': {},
   'mapCreates': {},
   'mapRefs': {},
   'maps': {},
   'menuPluginIdx':
        {'/Resources/plugins/PythonPlugins/PI_Aircraft.py': [],
         '/Resources/plugins/PythonPlugins/PI_MiniPython.py': [7],
         '/Resources/plugins/PythonPlugins/PI_SeeAndAvoid.py': [8],
         '/Resources/plugins/XPPython3/I_PI_Updater.py': [6]},
   'menuRefs':
        {<capsule object "XPLMMenuIDRef" at 0x7ff2bff3e750>: 8,
         <capsule object "XPLMMenuIDRef" at 0x7ff2ca63cb70>: 7},
   'menus':
        {7: ('/Resources/plugins/XPPython3/I_PI_Updater.py',
             'XPPython3',
             None,
             0,
             <bound method PythonInterface.menuHandler of <XPPython3.I_PI_Updater.PythonInterface object at 0x7ff312547640>>,
             'updatePython'),
          8: ('/Resources/plugins/PythonPlugins/PI_SeeAndAvoid.py',
              'See and Avoid',
              None,
              0,
              <bound method PythonInterface.menuHandler of <PythonPlugins.PI_SeeAndAvoid.PythonInterface object at 0x7ff2bff3ad30>>,
              None)},
   'modules':
        {'PythonPlugins.PI_MiniPython': <PythonPlugins.PI_MiniPython.PythonInterface object at 0x7ff2ca700a90>,
         'PythonPlugins.PI_SeeAndAvoid': <PythonPlugins.PI_SeeAndAvoid.PythonInterface object at 0x7ff2bff3ad30>,
         'PythonPlugins.PI_Aircraft': <PythonPlugins.PI_Aircraft.PythonInterface object at 0x7ff2ca63ce50>,
         'XPPython3.I_PI_Updater': <XPPython3.I_PI_Updater.PythonInterface object at 0x7ff312547640>},
   'plugins':
         {<PythonPlugins.PI_Aircraft.PythonInterface object at 0x7ff2ca63ce50>: ['XPPython Aircraft Plugin driver',
                                                                                 'xppython3.aircraft_plugin',
                                                                                 'XPPython Plugin which enables use of aircraft plugins',
                                                                                 'PythonPlugins.PI_Aircraft',
                                                                                 False],
          <PythonPlugins.PI_MiniPython.PythonInterface object at 0x7ff2ca700a90>: ['Mini Python Interpreter',
                                                                                   'xppython3.minipython',
                                                                                   'For debugging / testing, the provides a mini python interpreter',
                                                                                   'PythonPlugins.PI_MiniPython',
                                                                                   False],
          <PythonPlugins.PI_SeeAndAvoid.PythonInterface object at 0x7ff2bff3ad30>: ['See and Avoid',
                                                                                     'com.avnwx.SeeAndAvoid.p3',
                                                                                     'See and Avoid traffic generator',
                                                                                     'PythonPlugins.PI_SeeAndAvoid',
                                                                                     False],
           <XPPython3.I_PI_Updater.PythonInterface object at 0x7ff312547640>: ['XPPython3 Updater',
                                                                               'com.avnwx.xppython3.updater.3.8',
                                                                               'Automatic updater for XPPython3 plugin',
                                                                               'XPPython3.I_PI_Updater',
                                                                               False]},
   'widgetCallbacks':
      {<capsule object "XPLMWidgetID" at 0x7ff2ca63cc60>: [<bound method PythonInterface.widgetMsgs of <PythonPlugins.PI_MiniPython.PythonInterface object at 0x7ff2ca700a90>>],
       <capsule object "XPLMWidgetID" at 0x7ff2ca63cf90>: [<bound method PythonInterface.textEdit of <PythonPlugins.PI_MiniPython.PythonInterface object at 0x7ff2ca700a90>>],
       <capsule object "XPLMWidgetID" at 0x7ff2caa22750>: [<bound method XPListBox.listBoxProc of <XPListBox.XPListBox object at 0x7ff2caa22730>>]},
   'widgetProperties':
      {(<capsule object "XPLMWidgetID" at 0x7ff2caa22750>, 1002900): 0,
       (<capsule object "XPLMWidgetID" at 0x7ff2caa22750>, 1002906): {'Items': [''],
                                                                      'Lefts': [0],
                                                                     'Rights': [570]},
       (<capsule object "XPLMWidgetID" at 0x7ff2caa22750>, 1002907): 24,
       (<capsule object "XPLMWidgetID" at 0x7ff2caa22750>, 1002908): False,
       (<capsule object "XPLMWidgetID" at 0x7ff2caa22750>, 1002909): 0,
       (<capsule object "XPLMWidgetID" at 0x7ff2caa22750>, 1002910): 1,
       (<capsule object "XPLMWidgetID" at 0x7ff2caa22750>, 1002911): 1,
       (<capsule object "XPLMWidgetID" at 0x7ff2caa22750>, 1002912): 24,
       (<capsule object "XPLMWidgetID" at 0x7ff2caa22750>, 1002913): 0},
   'windows': {}}


.. _modules:

modules
-------

 All loaded plugins, by module.

 :key:

    Module Name  for the plugin.

 :value:

    PythonInterface object (e.g., "self" for each plugins)

.. _plugins:

plugins
-------

 Information about all plugins, by PythonInterface object.
 
 :key:

    PythonInterface object (e.g., "self" for each plugins)

  :value:

    List consisting of the Name, Signature, Description are as provided by the Python Plugin in
    the return from ``XPluginStart()``. The Module is package + module as loaded by
    python. The final boolean indicates if the module has been disabled (either at the request of the
    plugin, or because ``XPluginEnable()`` failed.

.. _commandCallbacks:

commandCallbacks
----------------

  :key:

     integer index  

  :value:

     Tuple, ("<PluginFile.py>", <XPLMCommandRef capsule>, <command handler python method>, inBefore=0/1, <refCon object>)  

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

.. _commandRefcons:

commandRefcons
--------------

 :key:

    inCommand  

 :value:

     <index> into commandCallbacks  

 Purpose: Used with :ref:`commandCallbacks` (see above)

.. _menuPluginIdx:

menuPluginIdx
-------------

 :key:
    <plugin>

 :value: list of integers (possibly empty)

 X-Plane uses an index for menu IDs. Each (C-API) plugin has an independent list starting at zero.
 When a menu item is removed, subsequent menu items have their indices decremented to "fill-in" the
 missing slot.

 With XPPython3, "we" only get a single list from Laminar, and therefore "we" have to track which
 menu item goes with which python plugin. This data structure maintains the mapping.

 Note that this index is relevant only for items added to the main plugin menu (not to sub menus).
 This is because items are numbered from zero, for each menu. Because the main plugin menu
 is shared, we have to fake the zero-based index for each python plugin. Sub menus can
 continue with zero-based index for each of their items & these do not need to be remapped.

    'menuPluginIdx':
        {'/Resources/plugins/PythonPlugins/PI_Aircraft.py': [],
         '/Resources/plugins/PythonPlugins/PI_MiniPython.py': [7, 9],
         '/Resources/plugins/PythonPlugins/PI_SeeAndAvoid.py': [8],
         '/Resources/plugins/XPPython3/I_PI_Updater.py': [6]},

 MiniPython plugin will refer to menu item [0] and [1], and XPPython3 will translate requests
 to X-Plane as [7] and [9] -- assuming MiniPython plugin added to items to the main plugin menu.

.. _menus:

menus
-----

 :key:

    integer index  

 :value:

    tuple, (<plugin>, Display String, <XPLMMenuIDRef>parent, menuItemNumber, <menu handler python method>, <refCon>)  

 Similar to :ref:`commandCallbacks` (described above), XPPython intercepts calls to menus.
 We provide X-Plane with a single custom menu handler for all your menus, and include a unique integer as the menu's reference
 constant. X-Plane will, with the reference constant, and we'll use the reference constant to retrieve your
 menu details from this dictionary, as ``menus[refCon]``.

 The value if the index is meaningless -- it is just a unique value, matching a value in menuRefs.

.. _menuRefs:

menuRefs
--------

 :key:

    <XPLMMenuIDRef> 

 :value:

    integer index into menus[] dict

 Maps from X-Plane XPLMMenuIDRef to a key into the menus[] dict.

.. _fl:

fl
--

 :key:

    integer index

 :value:

     tuple, (<plugin>, <callback python method> <interval>, <refcon>)

 Similar to :ref:`commandCallbacks` (described above), XPPython intercepts flightLoopCallbacks

.. _flRev:

flRev
-----

  :key:

     tuple: (<plugin>, <callback>, <refconAddr>)

  :value:

     integer index into fl[] dict

