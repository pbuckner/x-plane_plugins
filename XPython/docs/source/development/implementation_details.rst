Implementation Details
======================

* :doc:`import`. Information about python packages and proper way to import.
    
* :doc:`plugins`: Though most plugins are Global Plugins, you can code Aircraft and Scenery plugins.

* Using python **threading**: This works without issue, but with a major caveat. Laminar *requires* all
  calls into the SDK to be done on the main thread. That means you'll need to make all your calls on the
  main Python thread. You can coordinate between thread activity and the main thread by using shared data.
  Be mindful of the normal multi-threading issues: you may need to use ``threading.Lock``, ``queue.Queue``, or
  other synchronization techniques.

* Using python **multiprocessing or subprocess?** See :doc:`multiprocessing` for hints and an example.

* :doc:`callbacks`
  
.. toctree::
   :hidden:

   import
   plugins
   multiprocessing
   callbacks

