Encrypted Python (xpyce)
========================

XPPython3 plugin loads both regular python files (\*.py) and specially encrypted python files (\*.xpyce).

 I am indebted to Soroco Americas Private Limited (https://blog.soroco.com) and https://github.com/soroco/pyce
 for the initial code and idea. I've modified it slightly to support more python versions and work better
 within X-Plane. You can read their code and blog for background.

Normally, python files are distributed as readable text source files. This is a wonderful feature,
allowing users to actually see what the code is going to do, and allows them to make small modifications
to the plugin to better suit their needs.

But, sometimes there is a need for encryption.

* You have a proprietary algorithm, the details of which you don't want to divulge.
* You have complex code you don't want the user to muck-around with as it makes your
  support efforts difficult.
* You need to "lock" the code, making it unusable in some cases.

How xpyce works
---------------

Instead of distributing \*.py files, you can distribute encrypted \*.xpyce files. For each
original \*.py file, you'll create a compiled \.pyc file and then encrypt it, resulting in a
private decryption key *and* a \*.xpyce file. (:doc:`encrypt/encrypting`).

Distribute the \*.xpyce file(s) and get the decryption key to the user somehow (:doc:`encrypt/keydelivery`),
and with that information, XPPython3 will load the module.

Your (non-encrypted) python will update XPPython3 with the decryption keys at runtime (:doc:`encrypt/loading`),
and when XPPython3 attempts to load
a module it will do the normal search for the appropriate \*.py file & failing that, will look for a relevant \*.xpyce
file. If the \*.xpyce file is found, XPPython3 will see if it knows a decryption key for that file and then attempts to
decrypt and load the compiled byte-code.

Let me say that again: if the \*.py file is found, it is loaded. This simplifies plugin testing, as you can do all your
testing with the source code & then encrypt as a final step and remove the source python files from you package.

If everything is done correctly, there is a very small performance impact on load and zero impact on run time.

See :doc:`encrypt/simple` to learn more.


Details
-------

.. toctree::

   encrypt/simple
   encrypt/details
   
