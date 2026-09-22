.. index: Web Browser, Tasks; Web Browser

Browser Windows
===============

.. py:module:: XPLMDisplay
   :no-index:
      
.. py:currentmodule:: xp


Windows created with :data:`WindowContentTypeBrowser`, new with X-Plane 12.4.4 (SDK 440),
result in a `CEF <https://bitbucket.org/chromiumembedded/cef>`__
web view. A single web page covers the whole window; your *draw* callback(s) are not
used.

You can create a regular display Window with :func:`createWindowEx`, or an avionics device
window with :func:`createAvionicsEx`, and set the content type.

Both Windows, and Avionics Devices use differently named functions, but their functionality is
largely identical. This section covers both types of use.

As an overview, you'll:

.. rst-class:: compact

* Create the window (:func:`createWindowEx` / :func:`createAvionicsEx`)

* Set an initial URL (:func:`windowSetURL` / :func:`avionicsSetURL`)

* Optionally inject javascript into the window (:func:`windowInjectScript` / :func:`avionicsInjectScript`)

* Add browser JavaScript function which can call into X-Plane (:func:`windowAddBrowserFunction` / :func:`avionicsAddBrowserFunction`)

* Refresh the window if desired (:func:`windowRefresh` / :func:`avionicsRefresh`)

The *only* differences between Display Windows and Avionics Devices is:

#. When Display Windows are resized, the browser's viewport is resized without rescaling; when Avionics Devices
   are resized, the viewport does not change: the content is enlarged or shrunk to maintain the same viewport. Here we scale 400x400 to 600x600 using
   :func:`setWindowGeometry` and :func:`setAvionicsGeometry`:

   .. image:: /images/browser_scale.png
              :width: 80%

#. Avionics Devices are subject to cockpit lighting (and power); Display Windows are not. Here is what happens in a powered, and unpowered cockpit:

    .. image:: /images/lit_cockpit.gif
       :width: 300px        

    .. image:: /images/unlit_cockpit.gif
       :width: 300px

   You can fix the lighting (if you want) by providing a :func:`brightness` callback function with :func:`createAvionicsEx`.

First, you need to create the window setting ``contentType=xp.WindowContentTypeBrowser``.

>>> windowID = xp.createWindowEx(contentType=xp.WindowContentTypeBrowser,
...                              visible=1, left=100, right=500, top=500, bottom=100)
...

.. py:function:: windowSetURL(windowID, url) -> None
.. py:function:: avionicsSetURL(avionicsID, url) -> None                

    :param XPLMWindowID windowID:
    :param XPLMAvionicsID avionicsID: Window previously created.
    :param str url: URL to load

    Load a URL into a browser-content-type window. Safe to call immediately after
    creation --- the load is queued until the browser is ready.
    Subsequent calls replace the page. Has no effect on non-browser windows.

    >>> xp.windowSetURL(windowID, "https://www.google.com")

    .. image:: /images/browser_windows_url.png
       :width: 300px        

    The browser also recognizes local files, either included with your plugin on created "on-the-fly".
    One approach would be to include the HTML as part of your plugin, perhaps in ``PythonPlugins/myplugin/html/``.
    Then access this using ``file:`` protocol:

    >>> root = f"{xp.getSystemPath()}Resources/plugins/PythonPlugins/myplugin/html"
    >>> xp.windowSetURL(windowID, f"file://{root}/index.html")

    .. image:: /images/browser_window_seturl.png
               :width: 300px

    `Official SDK <https://developer.x-plane.com/sdk/XPLMDisplay/#XPLMWindowSetURL>`__ :index:`XPLMWindowSetURL`

    `Official SDK <https://developer.x-plane.com/sdk/XPLMDisplay/#XPLMAvionicsSetURL>`__ :index:`XPLMAvionicsSetURL`

.. py:function:: windowRefresh(windowID, ignoreCache=0) -> None
.. py:function:: avionicsRefresh(avionicsID, ignoreCache=0) -> None

    :param XPLMWindowID windowID:
    :param XPLMAvionicsID avionicsID: Window previously created.
    :param int ignoreCache: Pass 1 to bypass the HTTP cache (shift-reload)

    Reload the current URL in a browser-content-type window.

    `Official SDK <https://developer.x-plane.com/sdk/XPLMDisplay/#XPLMWindowRefresh>`__ :index:`XPLMWindowRefresh`

    `Official SDK <https://developer.x-plane.com/sdk/XPLMDisplay/#XPLMAvionicsRefresh>`__ :index:`XPLMAvionicsRefresh`

.. py:function:: windowInjectScript(windowID, script) -> None
.. py:function:: avionicsInjectScript(avionicsID, script) -> None

    :param XPLMWindowID windowID:
    :param XPLMAvionicsID avionicsID: Window previously created.
    :param str script: JavaScript source to execute

    Execute a JavaScript snippet in the browser window's main frame, with access to
    the same ``xplane.*`` namespace exposed to the page. If injected before the
    page has finished loading, it may run against an empty document. (See :func:`browserLoadFinished` callback.)

    One trick may be to load an empty page, then inject the HTML source directly.
    (This is kind of a party-trick, but you may find a use for it.)
    
    >>> html = """<body style="background:white"><label for="fuel">Fuel level:</label>
    ... <meter id="fuel" min="0" max="100" low="33" high="66" optimum="80" value="50">
    ... at 50/100
    ... </meter>
    ... </body>
    ... """
    ...
    >>> import json  # to handle escaping the double quotes
    >>> xp.windowInjectScript(windowID, f'document.documentElement.innerHTML={json.dumps(html)};')

    .. image:: /images/browser_fuel.png
               :width: 300px

    This does let us demonstrate other injections. Now that we know there is a DOM element with ``id="fuel"``:

    >>> script = """document.getElementById("fuel").value = 84;"""
    >>> xp.windowInjectScript(windowID, script)
    
    .. image:: /images/browser_fuel2.png
               :width: 300px

    `Official SDK <https://developer.x-plane.com/sdk/XPLMDisplay/#XPLMWindowInjectScript>`__ :index:`XPLMWindowInjectScript`

    `Official SDK <https://developer.x-plane.com/sdk/XPLMDisplay/#XPLMAvionicsInjectScript>`__ :index:`XPLMAvionicsInjectScript`
    
.. py:function:: windowAddBrowserFunction(windowID, name, function, refCon=None) -> None
.. py:function:: avionicsAddBrowserFunction(avionicsID, name, function, refCon=None) -> None

    :param XPLMWindowID windowID:
    :param XPLMAvionicsID avionicsID: Window previously created.
    :param str name: Name the page invokes as ``xplane.<name>(arg)``
    :param function: Callback ``f(windowID, jsonStr, refCon)``; see below
    :type function: Callable[[:class:`XPLMWindowID`, str, Any], Optional[str]]
    :param refCon: Reference constant passed to *function* (default: None)
    :type refCon: Any

    Register a Python callback that the page can invoke as ``xplane.<name>(arg)``.
    The JavaScript call returns a Promise that resolves to whatever your callback
    returns (as a JSON string). Your callback is passed the JSON-serialized
    argument string; registering the same *name* again replaces the callback.

    .. caution:: You need to add function(s) *before* you navigate to the window. Once the *initial*
                 SetURL is called, you cannot add functions.

                 If you try, you'll get an error callback in XPPython3Log.txt reading similar to: |BR|
                 ``XPLMWindowAddBrowserFunction: 'myCallback' was added after the window navigated (XPLMWindowSetURL)``

                 Your call to :func:`windowAddBrowserFunction` itself will not fail -- it will just be ignored.

    Your callback :attr:`function` is like:

      .. py:function:: browser_callback(windowID, jsonStr, refCon=None) -> str

         :param XPLMWindowID windowID:
         :param XPLMAvionicsID avionicsID: Window previously created.
         :param str jsonStr: JSON string representation of any parameters provided in javascript invocation.
         :param Any refCon: refCon as provided with addBrowserFunction (not with :func:`createWindowEx` or :func:`createAvionicsEx`). This is python, not a JSON string. (Javascript never sees this.)
         :return str: JSON string with results                   

    The following example provides a javascript function ``xplane.getTailnum(args)``. The ``args`` parameter provided by the
    actually javascript call in the browser will be converted to a JSON string and provided to the callback function. (We
    don't actually use args in the example, but you can see how they're transferred from JS to Python.)

    ============================== ================== ===================
    Javascript                     Python
    ------------------------------ --------------------------------------
    As called                      jsonStr            json.loads(jsonStr)
    ============================== ================== ===================
    xplane.getTailnum()            '{}\\n'            {}                 
    xplane.getTailnum('hello')     '"hello"\\n'       "hello"                 
    xplane.getTailnum(42)          '42\\n'            42                 
    xplane.getTailnum({foo: "bar"} '{"foo":"bar"}\\n' {"foo": "bar"}     
    ============================== ================== ===================

    Notice that the callback returns a JSON string.

    First, create the window and callback function:

    >>> import json
    >>> windowID = xp.createWindowEx(contentType=xp.WindowContentTypeBrowser, visible=1, left=100, right=500, top=500, bottom=100)
    >>> dataRef = xp.findDataRef('sim/aircraft/view/acf_tailnum')
    >>> def callback(windowID, jsonStr, refCon):
    ...     xp.log(f"Callback with param {json.loads(jsonStr)}, and {refCon=}")
    ...     return json.dumps({"tailnum": f"{xp.getDatas(dataRef)}"})
    ...

    Second, add the function, providing the ``name`` to be used by JavaScript, and optional ``refCon``. Then
    set the URL:

    >>> xp.windowAddBrowserFunction(windowID, 'getTailnum', callback, refCon=56)
    >>> root = f"{xp.getSystemPath()}Resources/plugins/PythonPlugins/myplugin/html"
    >>> xp.windowSetURL(windowID, f"file://{root}/index.html")

    The callback ``xplane.getTailnum`` is a javascript function (always) taking one parameter and returns a Promise. To execute,
    you can call ``then()`` on the function call, providing code to be executed on successful completion of the Promise:
    
    The relevant JavaScript in the browser might look something like this::

      xplane.getTailnum("hello")
         .then(function(arg) {
             var tailnum = arg.tailnum;
             document.getElementById('who').innerHTML = tailnum;
         });

    `Official SDK <https://developer.x-plane.com/sdk/XPLMDisplay/#XPLMWindowAddBrowserFunction>`__ :index:`XPLMWindowAddBrowserFunction`

    `Official SDK <https://developer.x-plane.com/sdk/XPLMDisplay/#XPLMAvionicsAddBrowserFunction>`__ :index:`XPLMAvionicsAddBrowserFunction`


Browser Page Load Callbacks
---------------------------

The two browser callbacks below are (optionally) passed to :func:`createWindowEx` and :func:`createAvionicsEx` (as
*browserLoadFinished* / *browserLoadError*). They can only be specified at window creation time, there
is no way to register them post-creation.

.. py:function:: browserLoadFinished(avionicsID/windowID, url, refCon) -> None

   :param XPLMAvionicsID / XPLMWindowID windowID: window attempting the load
   :param str url: URL of page being loaded
   :param Any refCon: refCon provided with window creation                

   Called when the device's main frame finishes loading a page (also fires for
   rendered HTTP error pages). This does not fire for AJAX loads or referenced
   javascript or images.

   You cannot call :func:`windowInjectScript` / :func:`avionicsInjectScript`
   until after a successful page load: you
   can use this callback as a way to known when the page load has completed.
   Note that a page that renders an HTTP error (e.g., a server 404 page) also
   "finishes" here.
   
   .. caution:: Because of how CEF initializes, this callback sometimes fires with
       ``url='about:blank'`` before your URL is applied, and may also fire *after* a
       failed navigation (for CEF's substitute error page). Treat
       :func:`browserLoadError` as the authoritative failure signal and ignore calls
       when you get passed ``url='about:blank'``.

.. py:function:: browserLoadError(avionicsID/windowID, url, error, refCon) -> None

   :param XPLMAvionicsID / XPLMWindowID windowID: window attempting the load
   :param str url: URL of page being loaded
   :param str error: Reason for failed load. (May be None)                   
   :param Any refCon: refCon provided with window creation                

   Called when a navigation fails at the network level (bad URL, host unreachable,
   TLS failure, file not found). This is the authoritative signal that navigation
   to *url* failed.

