Preferences
===========

The **Preferences** popup displays user-settable preferences which relate to the XPPython3 plugin (as
opposed to preferences which may relate to any particular python plugin.)

.. image:: /images/prefs.png
           
* **Documentation**: opens a browser to this page.

* **Save**: records your changes to disk and closes the window:
  you will need to restart X-Plane to have them take effect.

* **Cancel**: discards any changes and closes the window.

There are a few very important settings you should consider, each relates to Internet access
and possible collection of usage data. None of these options are required nor is there
any problem enabling them yet running X-Plane without a network connection.

* **Check for updates**: When set, XPPython3 will contact our server on startup and get
  information about the latest stable (and beta) versions available to download. No
  data is recorded on the server and no personally identifying information is sent, if
  this is the only preference enabled. Disabling this preference will disable
  all status query & usage collection for XPPython3.

  I suggest you leave this enabled to allow XPPython3 to indicate if/when a new version
  is available.

* **Collect XPPython3 Usage Statistics**: When set (and "Check for Updates" is set),
  XPPython3 will send anonymous information
  to our server where we record the fact that XPPython3 plugin is being started & which version
  of X-Plane (e.g., XP11 or XP12) and which variant (Linux, Mac, PC). This information
  is summarized in `current usage graphs <https://maps.avnwx.com/x-plane/stats>`_.

  I hope you leave this preference enabled as it allows me to gauge usage: otherwise I
  have no idea if anyone is using XPPython3. As a free-ware developer, positive feedback
  keeps me going.

* **Collect Python Plugin Statistics**: When set,
  XPPython3 will send anonymous information to our server with the name and description of each python
  plugin in use. Individual plugin developers can disable collection *for their plugin*, by setting
  the value ``self.xp2_disable_usage_stats = False`` on their ``PythonInterface`` instance.

  Every enabled XPPython3 python plugin will be recorded *on program exit*. This information is also
  summarized in `current usage graphs <https://maps.avnwx.com/x-plane/stats>`_. (Note that
  the extent and display of per-plugin usage summarization is under development.)

  I hope you leave this preference enabled as well. Over the years I've been surprised by
  the range of features implemented through python plugins and this is an effort to
  recognized those plugins which are most popular.

.. note:: Information is stored anonymously on secure servers physically in the United States,
  and is accessible *only* by me. I don't have, nor do I care to have, your name, email, phone
  number or pet's name. Other than what is viewable by the above graphs,
  no information is sold or shared with anyone else.

When we say the data is collected "anonymously", here's what we mean. As with *every*
interaction on the Internet, the server will receive your IP address. Other than to communicate
back to XPPython3 (with current version information) this information is ignored, and not
recorded. In an attempt to record *unique* information, XPPython3 creates and sends a UUID
with the data collection messages. This allows us distinguish between new & returning
users. It *does not* tell us anything about you, nor is there any way for us to identify
you from the UUID sent: If you like, you can change the UUID and we'll never know.

Data for preferences (including the generated UUID) are stored in the
text file :code:`<XP>/Output/preferences/xppython3.ini`. (See also :doc:`/development/xppython3.ini`.)

For the most part, changes to Preferences will take effect on next start of X-Plane.
