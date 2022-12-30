:orphan:
   
Mac Quarantine
--------------

Your newly downloaded plugin is subject to Apple's quarantine on recent versions of macOS. You will **not** be
able to execute the plugin until you first remove the quarantine. We've included a simple script
which will fix that for you:

#. Double-click on the AppleScript application "Authorize XPL" included in the XPPython3 folder [#F1]_:

    .. image:: /images/authorize.png

#. *It* is also subject to quarantine, so you'll see a popup [#F2]_:

    .. image:: /images/cannot_verify_XPL.png
           
#. Select "Cancel", and open "System Preferences", and "Security & Privacy":

    .. image:: /images/system_preferences.png

#. Note **Authorize XPL** is listed as "blocked from use". Select "Open Anyway".

    .. image:: /images/open_anyway.png

    (If "Open" is not selectable, you may need to click on the Lock Icon in the lower
    left, in order to make changes.)
    
#. Clicking "Open Anyway" runs the script which will popup similar to:

    .. image:: /images/authorize_popup.png

    Assuming the final part of the name is "XPPython3", press "Okay" and
    the plugin and associated files will be authorized.

    You can run the script before or after you've moved XPPython3 folder
    under X-Plane Resources/plugins hierarchy.

    It will popup "Success" when completed. [#F3]_

Now, continue with :ref:`Installation<for-mac>`.

----

.. [#F1] If you're curious about what the *Authorize XPL* script actually does, you can open it
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
