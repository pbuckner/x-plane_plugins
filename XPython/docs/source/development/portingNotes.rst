Porting Notes
=============

When porting from Python2 plugin to Python3 plugin, there three things you need to deal with:

#. Python2 to Python3: the language is slightly different. See standard Python documentation.
#. PythonPlugin (2) to XPPython3: the interface is slightly different. See :doc:`changesfromp2`.
#. X-Plane SDK200 to SDK303: Some new API some old ones have been deprecated. See Laminar documentation.

These notes cover my observations based on ported a few Python2 plugins to XPPython3.

Common Language Differences
---------------------------

Print Statement
+++++++++++++++

Remember it's ``print()`` not ``print`` in Python3.

Unicode
+++++++

If you are sending or receiving data externally, you'll need to decode / encode to UTF. For example

  ============================================= =============================================================
  Python 2                                      Python 3
  ============================================= =============================================================
  ``sock.sendto(msg, ('127.0.0.1, self.port))`` ``sock.sendto(msg.encode('utf-8'), ('127.0.0.1, self.port))``
  ============================================= =============================================================

Similarly, you might need to open files in binary ('wb' or 'rb') mode.

Note that :py:func:`xp.getDatab` returns a bytearray, *not* a string, so if you're expecting a string, you should
convert the result. See note with :py:func:`xp.getDatab`, or use XPPython3-specific string function: :py:func:`xp.getDatas`.

Dictionary Iterators
++++++++++++++++++++

Dictionaries iterate differently in Python 3. Not a big deal, but we seem to do this
a lot in Python.

  ============================================= =============================================================
  Python 2                                      Python 3
  ============================================= =============================================================
  ``for k, v in foo.iteritems():``              ``for k, v in iter(foo.items()):``
  ============================================= =============================================================

Relative Imports
++++++++++++++++

Python2 relative imports differ from Python3. I can't say I understand it but if you're importing
from a local package (say, you deploy your plugin with a subdirectory which includes useful functions)
you may have to change imports

  ================================== ========================
  Python 2                           Python 3
  ================================== ========================
  ``from my_package.foo import Foo`` ``from .foo import Foo``
  ================================== ========================

See also :doc:`import` for more information on XPPython3 ``sys.path`` and packages.
  
Callback Registrations
----------------------

Note that XPPython3 does not require providing ``self`` as first parameter on various registrations.

=================================================== =============================================
PythonInterface 2                                   XPPython3
=================================================== =============================================
``XPLMRegisterFlightLoopCallback(self, func, ...)`` ``XPLMRegisterFlightLoopCallback(func, ...)``
``XPLMAddWidgetCallback(self, window, func)``       ``XPLMAddWidgetCallback(window, func)``
``XPDestroyWidget(self, widgetID, 1)``              ``XPDestroyWidget(widgetID, 1)``
``XPLMCreateMenu(self, "Menu Name", ...)``          ``XPLMCreateMenu("Menu Name", ...)``
``XPLMRegisterDataAccessor(self, dataref, ...)``    ``XPLMRegisterDataAccessor(dataref, ...)``
``XPLMUnRegisterDataAccessor(self, dataref)``       ``XPLMUnRegisterDataAccessor(dataref)``
=================================================== =============================================

Widget Message Handling
-----------------------

In Python2, widget message parameters were passed as raw integers. In XPPython3, the parameters are passed as python objects

For example, ``param1``, when sent with a ``xpMsg_KeyPress`` message, would require you to call plugin-provided ``PI_GetKeyState()``
prior to use.
For XPPython3, the paramter is (already) a tuple ``(key, flags, vkey)``, and no call to ``PI_GetKeyState()`` is needed.
