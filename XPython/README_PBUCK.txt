====== My private notes ===========
Pattern for replacing pluginSelf:

  if(!PyArg_ParseTuple(args, "OiOO", &pluginSelf, &inHowLong, &controlFunc, &refcon)){
     return Null;
  }

==>
  if(!PyArg_ParseTuple(args, "OiOO", &pluginSelf, &inHowLong, &controlFunc, &refcon)){
     PyErr_Clear();
     if(!PyArg_ParseTuple(args, "iOO", &inHowLong, &controlFunc, &refcon))
         return NULL;
     pluginSelf = get_pluginSelf(PyThreadState_GET());
   }
   

