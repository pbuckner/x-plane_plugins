Encryption Details
------------------

The simple example (:doc:`simple`) assumes a single file, key co-located with source, and one version of python. You have options
which make this more powerful & more complicated.

* :doc:`encrypting`: You'll need to encrypt *for each* version of python. You *may* use different keys for each file, or each version
  of file.


* :doc:`loading`: How you specify your encrypted module(s) within ``update_keys()`` changes which key is used for which module.
  The simple example is probably the best, but if you need something more complicated, we support that also.

* :doc:`keydelivery`: Don't want to include the key in your delivered source? Here are some ideas.

.. toctree::

   encrypting
   loading
   keydelivery
   existing
   troubleshooting
