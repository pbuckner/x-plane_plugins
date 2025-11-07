pythonGetDicts()
================

.. Warning:: Notes in progress. This should give you an idea of what these internal dicts are & how they're used.

In order to fully support the X-Plane SDK, and provide a mapping between the Python and C-based API,
XPPython3 plugin uses a number of internal datastructures, some of these are available to user python
plugins (mostly for debugging purposes.)

You can access these dictionaries using :py:func:`xp.pythonGetDicts`.

.. py:function:: pythonGetDicts

   :return: Dictionary of dictionaries.

   Each dictionary is (also) obtainable individually, and detailed below. Note that these functions
   *dynamically* re-build the data when invoked. You will not get pointer to 'live' python object which
   changes. You'll need to call this function (or child functions) again to get updated data.


Many of these internal structures are keyed off a unique integer ID.
Rather than providing X-Plane your callback handlers directly, we provide X-Plane information to call
XPPython3, and then WE form the python call to your command handler. To do this
we store information about your callbacks in these datastructures, and substitute
an internal, generic, callback function and a serial integer as the refCon X-Plane will see.
That way, X-Plane calls our generic handler passing in the ID, we use the ID to figure out
which python callback should be executed and pass in that particular callbacks refCon.

Technically, theses data structures internal and are subject to change, but they can be particularly useful
during debugging.

The result of :py:func:`pythonGetDicts` is a dictionary of these dictionaries.

   >>> xp.pythonGetDicts().keys()
   dict_keys([
   'avionicsCallbacks',
   'capsules',
   'commandCallbacks',
   'dataRefCallbacks',
   'drawCallbacks',
   'errorCallbacks',
   'flightLoopCallbacks',
   'hotKeyCallbacks',
   'keySnifferCallbacks',
   'mapCallbacks',
   'menuCallbacks',
   'plugins',
   'sharedDataRefCallbacks',
   'widgetCallbacks',
   'widgetProperties',
   'windowCallbacks',
   ])


.. _avionicsCallbacks:

.. py:function:: getAvionicsCallbackDict

   Returns a dict of Tuples:

   :key:

      Integer ID used as generic refCon.

   :value:

      0. moduleName: str

      #. deviceID: int

      #. capsule: XPLMAvionicsID

      #. before: Callback
 
      #. after: Callback

      #. refCon: Any

      #. bezel_draw: Callback

      #. draw: Callback

      #. bezel_click: Callback

      #. bezel_rightclick: Callback

      #. bezel_scroll: Callback

      #. bezel_cursor: Callback

      #. screen_touch: Callback

      #. screen_righttouch: Callback

      #. screen_scroll: Callback

      #. screen_cursor: Callback

      #. keyboard: Callback:

      #. brightness: Callback

      #. createFlag: int

   For those with ``createFlag == 1``, these were added using :func:`xp.createAvionicsEx`, and are
   expected to be removed using :func:`xp.destroyAvionics`. These are "Custom" avionics devices.

   For those with ``createFlag == 0``, these were adding using :func:`xp.registerAvionicsCallbacksEx`
   and are expected to be removed using :func:`xp.unregisterAvionicsCallbacks`. There are existing
   avionics devices. For these, ``bezel_draw``, and ``draw`` are not used.

   ::

     {1: ('PythonPlugins.PI_MiniPython', 6, <capsule object "XPLMAvionicsID" at 0x377a721f0>,
          None, <function MyDraw at 0x1260f99e0>, None,
          None, None, None, None, None, None,
          None, None, None, None,
          None, None,
          0)}

.. _capsules:

.. py:function:: getCapsuleDict

   Returns a dict of Tuples:

   :key:

      Derefenced value of capsule. Integer, but value depends on the type of capsule.
      For example, XPWidgetIDs are C-language (void \*) pointer, but XPLMCommandRefs are internal X-Plane codes.

   :value:

      0. context: str, if debug mode is set, this is [module] file:line

      #. capsule: <capsule>

      #. name: Capsule type

   Foobar

   ::

      {33136351168:
          ('[PythonPlugins.PI_MiniPython] Users/pbuck/xp12/Resources/plugins/XPPython3/XPListBox.py:471',
           <capsule object "XPLMDataRef" at 0x127974ff0>,
           'XPLMDataRef'),
       33136351008:
          ('[PythonPlugins.PI_MiniPython] Users/pbuck/xp12/Resources/plugins/XPPython3/XPListBox.py:471',
           <capsule object "XPLMDataRef" at 0x127975050>,
           'XPLMDataRef'),
       33136350848:
          ('[PythonPlugins.PI_MiniPython] Users/pbuck/xp12/Resources/plugins/XPPython3/XPListBox.py:471',
           <capsule object "XPLMDataRef" at 0x127974e40>,
           'XPLMDataRef'),
       3030:
          ('[XPPython3.I_PI_Updater] Users/pbuck/xp12/Resources/plugins/XPPython3/I_PI_Updater.py:89',
          <capsule object "XPLMCommandRef" at 0x127959650>,
          'XPLMCommandRef'),
       33136351328:
         ('[PythonPlugins.PI_MiniPython] Users/pbuck/xp12/Resources/plugins/XPPython3/XPListBox.py:471',
          <capsule object "XPLMDataRef" at 0x127974fc0>,
          'XPLMDataRef'),
       3032:
         ('[XPPython3.I_PI_Updater] Users/pbuck/xp12/Resources/plugins/XPPython3/I_PI_Updater.py:89',
          <capsule object "XPLMCommandRef" at 0x1279595f0>,
          'XPLMCommandRef'),
       3028:
         ('[XPPython3.I_PI_Updater] Users/pbuck/xp12/Resources/plugins/XPPython3/I_PI_Updater.py:108',
          <capsule object "XPLMCommandRef" at 0x1279596e0>, 'XPLMCommandRef')}
                 
.. _commandCallbacks:

.. py:function:: getCommandCallbackDict

   Returns a dict of Tuples:

   :key:

      Integer ID used as generic refCon.

   :value:

      0. moduleName: str

      #. capsule: XPLMCommandRef

      #. callback: Callback

      #. before: int flag

      #. refCon: Any


   One for each command registered using :func:`xp.registerCommandHandler`. Entries are removed
   by using :func:`xp.unregisterCommandHandler`.

   ::

     {54: ('XPPython3/I_PI_Updater',<capsule object "XPLMCommandRef" at 0x7ff2bf6a42a0>,
           <bound method PythonInterface.updatePython of <XPPython3.I_PI_Updater.PythonInterface object at 0x7ff312547640>>,
           1, None),
      55: ('XPPython3/I_PI_Updater', <capsule object "XPLMCommandRef" at 0x7ff2ca63ccc0>,
           <bound method PythonInterface.togglePip of <XPPython3.I_PI_Updater.PythonInterface object at 0x7ff312547640>>,
           1, None),
      59: ('PythonPlugins/PI_SeeAndAvoid', <capsule object "XPLMCommandRef" at 0x7ff2bff3adb0>,
           <bound method PythonInterface.CommandHandler of <PythonPlugins.PI_SeeAndAvoid.PythonInterface object at 0x7ff2bff3ad30>>,
           0, []),
      60: ('PythonPlugins/PI_SeeAndAvoid', <capsule object "XPLMCommandRef" at 0x7ff2bff3ad80>,
           <bound method PythonInterface.CommandHandler of <PythonPlugins.PI_SeeAndAvoid.PythonInterface object at 0x7ff2bff3ad30>>,
           0, [])
      }

.. _drefs:

.. py:function:: getDataRefCallbackDict

  Returns a dict of Tuples

  :key:

    Integer ID used as generic refCon.

  :value:

    0. moduleName:str

    #. dataRef name: str

    #. bitwise: XPLMDataTypeID

    #. is_writable: int

    #. readInt: Callable

    #. writeInt: Callable

    #. readFloat: Callable

    #. writeFloat: Callable

    #. readDouble: Callable

    #. writeDouble: Callable

    #. readIntArray: Callable

    #. writeIntArray: Callable

    #. readFloatArray: Callable

    #. writeFloatArray: Callable

    #. readData: Callable

    #. writeData: Callable

    #. read_refCon: Any

    #. write_refCon: Any

    #. capsule: XPLMDataRef capsule

  All python-defined dataRefs. These are dataRefs created / supported by python. Non-python dataRefs are not
  added to this data structure. That is, dataRefs created by :func:`xp.registerDataAccessor`, not :func:`xp.findDataRef`.

  ::

       {0: ('PythonPlugins.PI_MiniPython', 'myPlugin/foobar', 1, 0,
            <function my_func at 0x38e36d800>, None, None, None,
            None, None, None, None, None, None, None, None,
            41, None, <capsule object "XPLMDataRef" at 0x3bc9939>),
            },
       {1: ('PythonPlugins.PI_MiniPython', 'myPlugin/dataItem1', 3, 1,
            <function MyReadInt at 0x38e36d440>, <function MyWriteInt at 0x38e36d6c0>, <function MyReadFloat at 0x38e36d3a0>, None,
            None, None, None, None, None, None, None, None,
            None, None,  <capsule object "XPLMDataRef" at 0xbc39939>),
            }

.. _drawCallbacks:


.. py:function:: getDrawCallbackDict

   Returns a dict of Tuples

   :key:

      Integer ID used as generic refCon.

   :value:

      0. moduleName: str

      #. callback function: Callable

      #. phase: int

      #. after: int

      #. refCon: Any

   All draw callbacks registers using :func:`xp.registerDrawCallback`. When the callback is unregistered
   using :func:`xp.unregisterDrawCallback` its entry is removed.

   ::

      {1: ('PythonPlugins.PI_MiniPython', <function MyDraw at 0x1261489a0>, 50, 0, None)},
      {2: ('PythonPlugins.PI_MiniPython', <function MyGLDraw at 0x1682419a0>, 50, 0, None)}


.. _errorCallbacks:

.. py:function:: getErrorCallbackDict

  Returns a dict of Tuples:

  :key:

    Python Module Name

  :value:

    0. moduleName:str

    #. callback: Callable

  Each python plugin can define a callback on internal X-Plane error, using :py:func:`xp.setErrorCallback`. Subsequent
  calls overwrite any previous callback. There is no way to remove an error callback.


.. _flightLoopCallbacks:

.. py:function:: getFlightLoopCallbackDict

   Returns a dict of Tuples

   :key:

      Integer ID used as generic refCon.

   :value:

      0. moduleName: str

      #. callback: Callable

      #. refCon Any

      #. flightLoopType (0=old style, 1=new style)

      #. XPLMFlightLoopID Capsule (if new, None otherwise)

   One for each flightLoop (not flightLoop item) created using :func:`xp.createFlightLoop`, removed by :func:`xp.destroyFlightLoop`.

   ::

       {3: ('PythonPlugins.PI_MiniPython', <function MyCallback at 0x1278919e0>, {'data': []},
            1, <capsule object "XPLMFlightLoopID" at 0x3ba275530>),
        2: ('PythonPlugins.PI_MiniPython', <function MyCallback at 0x127891940>, None,
            0, None),
        1: ('XPPython3.I_PI_Updater', <bound method Performance.fLCallback of <XPPython3.updater.performance.Performance object at 0x1276bb350>>, None,
            0, None)}


.. _hotKeyCallbacks:

.. py:function:: getHotKeyCallbackDict

   Returns a dict of Tuples

   :key:

      Integer ID used as generic refCon.

   :value:

      0. moduleName: str

      #. capsule: XPLMHotKeyRef

      #. callback: Callable

      #. refCon: Any


   One for each hotKey registered using :func:`xp.registerHotKey`. Entries are removed
   by using :func:`xp.unregisterHotKey`.

   ::

      {0: ('PythonPlugins.PI_MiniPython', <capsule object "XPLMHotKeyID" at 0x1281ee9a0>,
           <function MyHotKey at 0x1281e3420>, None ),
       1: ('PythonPlugins.PI_MiniPython', <capsule object "XPLMHotKeyID" at 0x181ee9a20>,
           <function MyHotOtherKey at 0x281e34201>, "RefCon" )}
        

.. _keySnifferCallbacks:

.. py:function:: getKeySnifferCallbackDict

   Returns a dict of Tuples

   :key:

      Integer ID used as generic refCon.

   :value:

      0. moduleName: str

      #. callback: Callable

      #. before: int

      #. refCon: Any

   One for each keySniffer registered using :func:`xp.registerKeySniffer`. Entries are removed
   by using :func:`xp.unregisterKeySniffer`.

   ::

       {1: ('PythonPlugins.PI_MiniPython', <function MySniffer at 0x37fdacb80>, 0, None),
        2: ('PythonPlugins.PI_TestSniffer', <function MyTestSniffer at 0x73dfca8b0>, 0, 'Test')}

.. _mapCallbacks:

.. py:function:: getMapCallbackDict

   Returns a dict of Tuples

   :key:

      Integer ID used as generic refCon.

   :value:

      0. moduleName: str

      #. layerType: int

      #. deleted: Callable

      #. prep: Callable

      #. draw: Callable

      #. icon: Callable

      #. label: Callable

      #. showToggle: int

      #. name (of layer): str

      #. refCon: Any

      #. capsule: XPLMMapLyerID

   One for each map layer created using :func:`xp.createMapLayer`. Entries are removed
   by using :func:`xp.destroyMapLayer`.

   ::

      {6: ('PythonPlugins.PI_MiniPython', 1,
           <function deleteLayer at 0x1295c9620>, None, None, None, None,
           1, 'Delete Example',
           None, <capsule object "XPLMMapLayerID" at 0x3c2a6e1f0>),
       5: ('PythonPlugins.PI_MiniPython', 1,
           None, <function prepLayer at 0x1295c9800>, None, None, None,
           1, 'Prep Example',
           None, <capsule object "XPLMMapLayerID" at 0x3c2a6e100>),
       4: ('PythonPlugins.PI_MiniPython', 1,
           None, None, None, None, <function labelLayer at 0x1295c9940>,
           1, 'Label Example',
           None, <capsule object "XPLMMapLayerID" at 0x3c2a6e550>),
       3: ('PythonPlugins.PI_MiniPython', 1,
           None, None, None, <function iconLayer at 0x1295c94e0>, None,
           1, 'Black Star',
           None, <capsule object "XPLMMapLayerID" at 0x3c2a6e5b0>),
       2: ('PythonPlugins.PI_MiniPython', 1,
           None, None, <function drawLayer at 0x1295c9440>, None, None,
           1, 'Green Diagonal',
           None, <capsule object "XPLMMapLayerID" at 0x3c2a6e700>),
       1: ('PythonPlugins.PI_MiniPython', 1,
           None, None, None, None, None,
           1, 'My New Layer',
           None, <capsule object "XPLMMapLayerID" at 0x3aab51f20>)}

.. _menuCallbacks:

.. py:function:: getMenuCallbackDict

   Returns a dict of Tuples

   :key:

      Integer ID used as generic refCon.

   :value:

      0. moduleName: str

      #. Menu Name: str

      #. parentID: int

      #. parent_item: int

      #. callback: Callable

      #. refCon: Any

      #. capsule: XPLMMenuID

   One for each menu (not menu item) created using :func:`xp.createMenu`, removed by :func:`xp.destroyMenu`.

   ::

       {2: ('PythonPlugins.PI_TestFoo', 'Custom Avionics', 0, 0,
            <bound method PythonInterface.menu_handler of <PythonPlugins.PI_TestFoo.PythonInterface object at 0x132e69f70>>,
            None, <capsule object "XPLMMenuID" at 0x130b18900>),
        1: ('XPPython3.I_PI_Updater', 'XPPython3', 0, 0,
           <bound method PythonInterface.menuHandler of <XPPython3.I_PI_Updater.PythonInterface object at 0x12f907290>>,
           None, <capsule object "XPLMMenuID" at 0x13155cf60>)}

.. _plugins:

.. py:function:: getPluginDict

   Returns a dict of Tuples

   :key:

      PythonInterface instance ("self" for each plugin)

   :value:

      0. moduleName: str

      #. plugin name: str

      #. signature: str

      #. description: str

      #. disabled: bool

   Information about all python plugins successfully started. The Module is package + module as loaded by
   python. The final boolean indicates if the module has been disabled (either at the request of the
   plugin, or because ``XPluginEnable()`` failed.


   ::

       {<PythonPlugins.PI_Aircraft.PythonInterface object at 0x7ff2ca63ce50>:
           ('PythonPlugins.PI_Aircraft',
            'XPPython Aircraft Plugin driver',
            'xppython3.aircraft_plugin',
            'XPPython Plugin which enables use of aircraft plugins',
            False),
        <PythonPlugins.PI_MiniPython.PythonInterface object at 0x7ff2ca700a90>:
           ('PythonPlugins.PI_MiniPython',
            'Mini Python Interpreter',
            'xppython3.minipython',
            'For debugging / testing, the provides a mini python interpreter',
             False),
        <PythonPlugins.PI_SeeAndAvoid.PythonInterface object at 0x7ff2bff3ad30>:
           ('PythonPlugins.PI_SeeAndAvoid',
            'See and Avoid',
            'com.avnwx.SeeAndAvoid.p3',
            'See and Avoid traffic generator',
            False),
        <XPPython3.I_PI_Updater.PythonInterface object at 0x7ff312547640>:
           ('XPPython3.I_PI_Updater',
            'XPPython3 Updater',
            'com.avnwx.xppython3.updater.3.8',
            'Automatic updater for XPPython3 plugin',
             False)},


.. _sharedDataRefCallbacks:


.. py:function:: getSharedDataRefCallbackDict
                 
  Returns a dict of Tuples:

  :key:

    Integer ID used as generic refCon.

  :value:

    0. moduleName:str

    #. dataRef name: str

    #. XPLMDataTypeID: int

    #. callback: Callable

    #. refCon: Any

  All shared dataRefs. Entries are added for each call to :func:`xp.shareData` and removed for :func:`xp.unshareData`.

  ::

       {0: ('PythonPlugins.PI_MiniPython', 'shared/float', 2,
            <function Changed at 0x1290e3ec0>, 'My Float'),
            },
       {1: ('PythonPlugins.PI_MiniPython', 'shared/int', 1,
            <function Changed at 0x1290e3ec0>, 'My Int'),
            },

.. _widgetCallbacks:

.. py:function:: getWidgetCallbackDict

   Returns a dict of Tuples

   :key:

      XPWidgetID Python Capsule

   :value:

      0. moduleName: str

      #. List of callbacks

   ::

      {<capsule object "XPWidgetID" at 0x12c488f60>: (
           'PythonPlugins.PI_MiniPython', [
                <bound method PythonInterface.textEdit of <PythonPlugins.PI_MiniPython.PythonInterface object at 0x12c4655e0>>
                ]),
       <capsule object "XPWidgetID" at 0x12c4885a0>: (
           'PythonPlugins.PI_MiniPython', [
                <bound method PythonInterface.widgetMsgs of <PythonPlugins.PI_MiniPython.PythonInterface object at 0x12c4655e0>>
                ]),
       <capsule object "XPWidgetID" at 0x12c488f30>: (
           'PythonPlugins.PI_MiniPython', [
                <bound method XPListBox.listBoxProc of <XPPython3.XPListBox.XPListBox object at 0x12c488a70>>
                ])}

.. _widgetProperties:

.. py:function:: getWidgetPropertiesDict

   Returns a dict of dict

   :key:

      XPWidgetID Python Capsule

   :value:

      * Dict of properties:

        :key:
           
           PropertyID: int

        :value:

           Any

   ::

      {<capsule object "XPWidgetID" at 0x12c488f30>: {
           1002907: 24,
           1002911: 23,
           1002908: False,
           1002901: 0,
           1002913: 0,
           1002910: 46,
           1002912: 24,
           1002900: 27,
           1002909: 0,
           1002906: {'Items': ['',
                               '...     if x < 100 and y < 100:  # only change cursor within the green portion of screen',
                               '... ',
                               '...       xp.setCursor(18)',
                               '...       return xp.CursorCustom',
                               '...     return xp.CursorDefault',
                               '... ',
                               '>>> def myCallback(message, widgetID, param1, param2):',
                               '...     if message == xp.Msg_CursorAdjust:',
                               '... ',
                               '...       xp.setCursor(18)',
                               '...       param2[0] = xp.CursorCustom',
                               '...       return 1',
                               '...     return 0',
                               '... ',
                               '>>> xp.addWidgetCallback(subwidgetID, myCallback)',
                               '>>> xp.destroyWidget(widgetID)',
                               '',
                               '',
                               '>>> xp.getWidgetCallbackDict()'],
                   'Lefts': [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
                   'Rights': [565, 565, 565, 565, 565, 565, 565, 565, 565, 565, 565],
                   'Wraps': [False, False, False, False, False, False, False, True]
                  }
             }
        }


.. _windowCallbacks:

.. py:function:: getWindowCallbackDict

   Returns a dict of Tuples

   :key:

      WindowID Python Capsule

   :value:

      0. moduleName: str

      #. draw: Callable

      #. click: Callable

      #. key: Callable

      #. cursor: Callable

      #. wheel: Callable

      #. rightClick: Callable

      #. refCon: Any

   All windows as created using :func:`xp.createWindowEx`. Each of the callbacks are included (or None if not defined), as well
   as the refCon. Note the key is the XPLMWindowID represented as a python capsule.

   When the window is destroyed using :func:`xp.destroyWindow` its entry is removed.

   ::

      {<capsule object "XPLMWindowID" at 0x1261a9380>: ('PythonPlugins.PI_MiniPython',
                                                        <function MyDraw at 0x12615d9e0>, None, None, None, None, None,
                                                        None),
       <capsule object "XPLMWindowID" at 0x1261a9170>: ('PythonPlugins.PI_MiniPython',
                                                        None, None, None, None, None, None,
                                                        'Hello')}


