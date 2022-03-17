Encryption Key Delivery
-----------------------

For encryption to work, your users will have to have a key...

Include it in source
====================

If it's non-commercial, and the reason you're doing this is simply to obfuscate code, *include the key in your
un-encrypted code*.
We'll decrypt the file(s) on the fly, but never re-create the source code: The sensitive parts
of your plugin will be usable, though unreadable, without needing to distribute the key out-of-band.

Remember XPPython3 still needs an unencrypted PI\_\*.py file with a defined PythonInterface class in order to
start the plugin.

Read it from a config file
==========================

Get the key the user somehow and have them add it to a configuration file. Perhaps a simple text file
either under your module directory, or in standard ``<X-Plane>/Output/preferences`` folder.
The un-encrypted portion of your plugin can read the key, call ``update_keys()`` and then
import the encrypted portions.

Email it
========

If you're selling the product, you can send the key to your user(s) through email. Or,
have the user enter their email in your plugin (a non-encrypted part), and have your un-encrypted
python code contact your server to return the key if the email matches someone in your database.
Lots of possibilities.

Look it up on your Server
=========================

It's all just python, so you could add just add lookup code to your ``PythonInterface().XPluginStart()``
function: It could read a local file with username / password information, and exchange that information with your server
to retrieve the decryption keys (Note: the decryption keys will be the same for all users!). On success, you update
the loader dictionary using ``XPPython3.xpyce.update_keys()``.

Then, your XPluginStart or XPluginEnable can import the encrypted module, something like::

  class PythonInterface:
     def XPluginStart(self):
         try:
             user_credentials = readCredentialsFromFile()
             keys = getKeysFromServer(user_credentials)
             XPPython3.xpyce.update_keys(keys)
             mod = importlib.import_module('.myplugin.main')
         except:
             mod = None
         return 'Name', 'Signature', 'Description'

     def XPluginEnable(self):
         if not mod:
            promptUserForCredentials():
         else:
            mod.doStuff()

Fortunately, you can test all this *without encryption* by simply not deleting your \*.py module (``myplugin/compute.py`` in this
example). Once you have the credentials part working, delete the python file and the ``importlib.import_module()`` code
will look for the encrypted version.
