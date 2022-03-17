Convert an Existing Plugin to Encrypted
---------------------------------------

Assume you have an existing (python3) plugin: ``PI_MySecret.py``, and you want to convert it.

1. Create subdirectory and place your original code there:

  .. code-block:: console

     $ pwd
     <XP>/Resources/plugins/PythonPlugins
     $ mkdir mysecret
     $ mv PI_MySecret.py mysecret

2. Create a new "shell" plugin PI_MySecret.py under PythonPlugins, which looks similar to::

    from .mysecret import PI_MySecret

    class PythonInterface(PI_MySecret.PythonInterface):
        pass

   Seriously, it's just four lines long (before adding the keys).
   
3. Test and convince yourself this new plugin is identical to your old plugin. (We've not done any encryption yet).

4. Generate keys
   Remember to generate keys for ``mysecret/PI_MySecret.py``, not for your new shell ``PI_MySecret.py``

  .. code-block:: console

   $ cd mysecret
   $ python3.6  ../../XPPython3/xpyce_compile.py PI_MySecret.py 
   PI_MySecret.cpython-36.xpyce: Abcd__
   $ KEY=Abcd__

   $ python3.7  ../../XPPython3/xpyce_compile.py PI_MySecret.py -k $KEY
   PI_MySecret.cpython-37.xpyce: Abcd__
   $ python3.8  ../../XPPython3/xpyce_compile.py PI_MySecret.py -k $KEY
   PI_MySecret.cpython-38.xpyce: Abcd__
   $ python3.9  ../../XPPython3/xpyce_compile.py PI_MySecret.py -k $KEY
   PI_MySecret.cpython-39.xpyce: Abcd__
   $ python3.10 ../../XPPython3/xpyce_compile.py PI_MySecret.py -k $KEY
   PI_MySecret.cpython-310.xpyce: Abcd__
 
5. Add the key to your shell PI_MySecret.py file::

     from XPPython3.xpyce import update_keys
     
     keys = {'mysecret': 'Abcd__'}
     update_keys(keys)
     
     from .mysecret import PI_MySecret
     
     class PythonInterface(PI_MySecret.PythonInterface):
         pass

6. Test again.

   Note that *we still load the* ``mysecret/PI_MySecret.py`` source file. This will always be true: if the \*.py file
   exists, we use it. If the \*.py file does not exist, we look for the \*.xpyce.

7. Move / Remove the \*.py and test again.

   .. code-block:: console

     $ mv mysecret/PI_MySecret.py mysecret/PI_MySecret.py-disabled

8. Test again, this time without the available source file.

9. Done. Ship the code as::

                 ├─── PI_MySecret.py
                 └─── mysecret/
                      ├─── PI_MySecret.cpython-36.xpyce
                      ├─── PI_MySecret.cpython-37.xpyce
                      ├─── PI_MySecret.cpython-38.xpyce
                      ├─── PI_MySecret.cpython-39.xpyce
                      └─── PI_MySecret.cpython-310.xpyce
