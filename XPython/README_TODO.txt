# TODO
* Currently, always use instance method for callbacks (self.foo)
  a) can I use @classmethod
  b) @staticmethod
  c) standalone functions?
  
* Currently, we use "global" values for refcon, since we do rich compare (don't we?)
  can instead we just use regular values?, Strings, ints, floats,.. Perhaps even arrays or dicts,
  as long as they'd match?
     >>> a = {'a': 1, 'b': 2}
     >>> b = {'b': 2, 'a': 1}
     >>> a == b
     True
  
## CODE
Support (some?) of the Sandy Barbour convenience functions

These appear to be 'internal', yet available interfaces from Python2 -- I don't see usefulness of these
  XPLMPlugin.PI_RemoveEnumerateFeatures
  XPLMScenery.PI_RemoveLookupObjects
  XPLMScenery.PI_RemoveObjectLoadedCallback
  XPLMUtilities.PI_RemoveErrorCallback

## DOCUMENTATION

### API

Support PythonScriptMessaging functions
* number = PI_CountScripts()
* scriptID = PI_GetNthScript(index)
* scriptID = PI_FindScriptBySignature(signature)
* PI_GetScriptInfo(scriptID, &name, &signature, &description)
* boolean = PI_IsScriptEnabled(scriptID)
* boolean = PI_EnableScript(scriptID)
* PI_DisableScript(scriptID)
* PI_SendMessageToScript(<self>, scriptID | None, message, param)

