Mini Python Interpreter
=======================

One useful demo is the `Mini Python Interpreter <https://github.com/pbuckner/x-plane_plugins/raw/master/XPython/demos/PI_MiniPython.py>`_.   You'll need to download a copy from github. Also make sure
you get a copy of `XPListBox.py <https://github.com/pbuckner/x-plane_plugins/raw/master/XPython/demos/XPListBox.py>`_
which implements a scroll listbox widget. Drop both files into your Resource/plugins/PythonPlugins folder.

.. note:: It's not a full python interpreter: it can only do single line python input, but it is pretty useful.

MiniPython will create a menu item which toggles the display of the interpreter. When displayed it looks
like

.. image:: /images/mini-python.jpg

Type a one line command or expression in the upper field and press <Return> or click the "Do" button and
your code will be executed, with the results being displayed in the lower scrolling window. Pressing "Help" will
open a browser window to documentation.

We'll store your locals, so you can type::

  a = 15

Followed by::

  a + 6

and we'll print::

  21

You'll also have access to ``self`` which will be set to the MiniPython interpreter's PythonInterface object.

Since my keyboard is always typing the wrong thing, this TexField widget has some extra features:
Typing UpArrow or Ctrl-P will display the previous command you've typed. Repeating that key will
traverse history. DownArrow or Ctrl-N will display the next command in your history.

Other editing keystrokes are available, and listed in the popup window.

Some useful things to do:

* **Explore the SDK**: Not sure how something works? Type it into the interpreter::

    >>> import xp
    >>> xp.getSystemPath()
    '/Volumes/Disk1/X-Plane/'

* **Execute X-Plane Commands**::

    >>> pause_cmd = xp.findCommand('sim/operation/pause_toggle')
    >>> cmd
    <capsule object "XPLMCommandRef" at 0x7566f330ad>
    >>> xp.commandOnce(pause_cmd)

* **Manipulate X-Plane DataRefs**::

    >>> time = xp.findDataRef('sim/time/zulu_time_sec')
    >>> time
    <capsule object "datarefRef" at 0x7fc44329920>
    >>> xp.getDataf(time)
    44768.0
    >>> xp.getDataf(time)
    44771.2
    >>> xp.setDataf(time, 100)
    >>> xp.getDataf(time)
    102.1

    
* **Create Callback**: You can't easily create multiline programs using MiniPython, but you can
  create a one-line lambda function which you can use to test the execution of callbacks::

    >>> cb = lambda a, b: print("in callback")
    >>> xp.createMenu('New Menu, None, 0, cb, [])
    <capsule object "XPLMMenuIDRef" at 0x75334211d>

* **Debug your Plugin**: Note that you'll need access to *your* plugin from the interpreter, so do this:

  * In your plugin, add a global to hold your plugin's instance (and/or other useful parameters)::

      MyPlugin = None

      class PythonInterface:
          def XPluginStart(self):
              global MyPlugin
              MyPlugin = self
              ....

    And set that global's value.

  * Then, within MiniPython, import your plugin, and you'll have access::

      >>> import PI_MyPlugin
      >>> myPlugin = PI_MyPlugin.MyPlugin

    From there, you'll be able to access your full plugin instance.

We don't yet provide online help, so ``help(xp.createWidgets)`` isn't useful, but stay tuned!    
