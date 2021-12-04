:orphan:

Mac Quarantine By Hand
----------------------

Removing the quarantine ``xattr`` isn't hard to do, if you're comfortable with the command line:

#. Open Terminal app, and go to the folder <X-Plane>/Resources/plugins/XPPython3/mac_x64::

      $ cd /Users/pbuck/X-Plane/Resources/plugins/XPPython3/mac_x64

   (I don't know where you installed X-Plane, so the top folders may be different)

#. Verify quarantine information for XPPython3.xpl::

      $ xattr -l XPPython3.xpl
      com.apple.quarantine: 0083;619aec20;Safari;47F5FBD6-CEA4-425C-93A3-1856380C71D5

   The code after "com.apple.quarantine" will be different but it's existence indicates
   the file *is subject to quarantine*.

#. Remove the quarantine for XPPython3.xpl::

      $ xattr -d com.apple.quarantine  XPPython3.xpl

That's it: restart X-Plane and you should not see the popup! (These steps will work for other plugins as well.)

Note that some plugins may include other files also subject to quarantine (XPPython3 certainly does!)
-- shared libraries or executable binaries.
Adapt the above instructions to remove quarantine xattr for those files.
   
