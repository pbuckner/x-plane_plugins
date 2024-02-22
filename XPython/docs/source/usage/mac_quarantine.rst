:orphan:
   
Mac Quarantine
--------------

Your newly downloaded plugin is subject to Apple's quarantine on recent versions of macOS. You will **not** be
able to execute the plugin until you first remove the quarantine. We've included a simple script
which will fix that for you:

#. Double-click on the AppleScript application "Drop Quarantine" included in the XPPython3 folder [#F1]_:

    .. image:: /images/authorize.png

#. *It* is also subject to quarantine, so you'll see one of two popups, depending on your version of MacOS [#F2]_:

    .. image:: /images/cannot_verify_XPL.png
       :width: 45%

    .. image:: /images/cannot_open_XPL.png
       :width: 45%               
           
#. Select "Cancel" or "OK", and open "System Preferences", and "Security & Privacy". Again, depending
   on your version of MacOS:

    .. image:: /images/system_preferences.png
       :width: 45%        

    .. image:: /images/system_preferences_new.png
       :width: 45%        

#. Note **Drop Quarantine** is listed as "blocked from use". Scroll down and select "Open Anyway".

     .. image:: /images/open_anyway.png
        :width: 45%

     .. image:: /images/open_anyway_new.png
        :width: 50%

   (If "Open" is not selectable, you may need to click on the Lock Icon in the lower
   left, in order to make changes.)
    
#. Clicking "Open Anyway" runs the script which will popup similar to:

     .. image:: /images/authorize_popup.png

   Assuming the final part of the name is "XPPython3", press "Okay" and
   the plugin and associated files will be authorized.

   You can run the script before or after you've moved XPPython3 folder
   under X-Plane Resources/plugins hierarchy.

   It will popup "Success" when completed. [#F3]_

#. You **may** see yet another popup, before the "Authorizing..." popup: [#F4]_

     .. image:: /images/original_folder.png
       :width: 70%

   If so, move Drop Quarantine from the XPPython3 folder to another location -- any location will work.
   I suggest simply moving it one folder "up" into the Resources/plugins folder & that way it will be
   available for any quarantined plugin you may want to add in the future. Then, double click on it again
   to have it do its work.

Now, continue with :ref:`Installation<for-mac>`.

----

.. [#F1] If you're curious about what the *Drop Quarantine* script actually does, you can open it
         in the Apple Script Editor.

.. [#F2] What? Why bother *authorizing* an application to *authorize* the plugin, isn't that double
         the work? Well, not really. If you're comfortable typing commands into Terminal, you don't
         need an app (or even me to tell you how). If you're not comfortable, then this sequence
         requires the fewest steps, will work for everyone, and actually, you'll be able to
         use the "Authorizing Script" for *any* X-Plane plugin. One-and-done!

.. [#F3] All the script does is execute ``xattr -d com.apple.quarantine`` on any XPL file found under
         the folder where the script is located. It also remove quarantine on shared libraries included
         with XPPython3.

         You can actually use this same script on other (non-python) X-Plane plugins: just move or copy
         the script to the other plugin's folder and execute it again.

         Or, check out :doc:`mac_quarantine_commands` to do it via command line.

.. [#F4] Holy cow! Another popup? Yes. Even though Drop Qurantine is *authorized*, it still thinks
         it (the code) is located in a quarantined folder. Seems like an Apple bug to me. Moving
         the file *anywhere* causes it to reset its location allowing it to work.
