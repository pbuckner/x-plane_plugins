.. py:currentmodule:: xp               

Javascript Promise
==================

When you add a browser function using one of the AddBrowserFunction python calls
you're adding something which can
be called using javascript from within your browser code. Rather than a simple function, it's a javascript
`Promise <https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Promise>`_.

A Promise is standard javascript, but something you may not be familiar with, so here are a few notes to
get you started.

A Promise is an asynchronous "promise" of a value. That is, it has a value once it has been "fulfilled", but initially
it has a "pending" value.

Accessing the Promise
---------------------

First, your javascript code will need to get the Promise that was added by your python code using ``*AddBrowserFunction()``. The
python might look like::

  xp.windowAddBrowserFunction(windowID, name='Foo', callback=myFunction)

and your javascript might look like:

.. code-block:: javascript

  try {
      var myPromise = xplane.Foo('hello');
  } catch (error) {
      reportError(error);
  }

* ``xplane`` *will exist in your CEF browser*. You don't need to do anything. By invoking browser using the :doc:`modules/display_window_browser` functions,
  this objection will already exist.

* ``xplane.Foo`` will exist only after :py:func:`windowAddBrowserFunction` / :py:func:`avionicsAddBrowserFunction` has been called, using ``Foo`` as the name parameter
  (as illustrated above.) Note that you *must* add the browser function(s) *prior* to setting the *initial* URL for the new window.

* ``xplane.Foo('hello');`` sets up the promise, passing ``'hello'`` as a json string to your python ``myFunction(windowID, jsonStr, refCon)`` callback.
  Any/All parameters provided to ``xplane.Foo()`` will be grouped into a single json string and presented to your callback.

The result of ``xplane.Foo('hello')`` isn't a value, it is a *Promise*, which you'll need to resolve.

Resolving a Promise
-------------------

There are two basic ways of getting the Promise's value. From X-Plane's perspective these ways are equivalent. You can query
the web for pros/cons, but generally it comes down to readability.

First, recall the *result* of your python callback will be a json string. This will be converted into a javascript object
as resolved by the Promise. So, assume your python callback returns something like::

  def myCallback(windowID, jsonStr, refCon):
      return json.dumps({"a": 123, "b": "success"})

On success, your promise will resolve to a javascript Object with two keys: ``{a: 123, b: "success}``.

``.then() / .catch()``
++++++++++++++++++++++

You can retrieve the value using ``.then()``:

.. code-block:: javascript

  myPromise
      .then((result) => {
          // element.textContent = `${JSON.stringify(result, null, 2)}`; // if you want to see the raw result...
          element.textContent = `${result.a} and ${result.b}`;
      })
      .catch(error) => {
          element.textContent = error;
      });

The Promise runs asynchronously, and your ``element.textContent`` will get a value "some time in the future".

``async / await``
+++++++++++++++++

Alternatively, and equivalently, you can wrap your Promise in an async function. I suppose this is the
preferred and more "modern" way:

.. code-block:: javascript

  async function executePromise() {
      try {
          const result await myPromise;
          element.textContent = `${result.a} and ${result.b}`;
      } catch (error) {
          element.textContent = error;
      }
  }

And once wrapped, invoke it directly or (for example) invoke it two seconds later...:

.. code-block:: javascript

  executePromise();
  // setTimeout(executePromise, 2000);
  
Realize, you're *invoking* the Promise: it may *resolve* at some later time. (the ``setTimeout`` example
simply starts the resolution later: you have no control over *when* the result is returned.)
