Troubleshooting
---------------

First, make sure your plugin works without encryption: make your python source files available.

You can see details during module loading by enabling ``debug`` in your preference file. See :doc:`/development/xppython3.ini`.

If you plugin fails only with encryption enabled, here are some likely errors (these will appear in your XPPython3Log.txt file regardless
of ``debug`` status):

----

``KeyError: Cannot find decryption_key for module '<module_name>', known modules are: ['<module>', '<module>']``

  We were able to find an xpyce module (*<module_name>*) during import, but could not match that module name with an entry in list of keys.
  The list of "known" modules is provided.

  Most likely, you either forgot to add your key using ``XPPython3.xpyce.update_keys({mod: key})`` or the module you
  updated with is incorrect (spelling error, or wrong package name.)

----

``ValueError: Poorly formed Fernet key: '<key>'``

   The module was found, but the key (*<key>*) associated with the module is not a valid Fernet key. Most likely, this is
   a typo when entering the key value.
   
----

``ValueError: Invalid decryption key: '<key>'``

   The module was found, but the key (*<key>*) associated with the module did not decrypt the file. Most likely, this is
   due to registering the wrong key for the module -- perhaps a mis-match between the encrypted file and the key provided.
