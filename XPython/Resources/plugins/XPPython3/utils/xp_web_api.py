import re
import queue
import time
import os
import requests
import threading
import multiprocessing as mp
try:
    from XPPython3 import xp
except ModuleNotFoundError:
    pass


# Status
# This spawns mp correctly, gets info from the webserver
# Info is put in queue with "lives" because q is from getCommandsMP,
# which lives because it's run in a separate global thread created in getCommands,
# which doesn't die/join.
#
# In an attempt to get this stuff actually displayed in XPListbox, getCommands()
# is give 'cmd', which it will use to add a lit to XPListbox.
# Problem is, mywidget.add() calls xp.setWidgetDescriptor() and xp.setWidgetProperty
# BUT it's not on the main thread, so we're hosed.
#
# Need to figure a way to have getCommandsMP() pass up data which can be read
# by XPListbox & have ListBox add those lines "at next opportunity" on the main thread.
# ... maybe instead of add, have a Queue.queue owned by XPListbox, and which is
# what getCommandsMP uses. XPListbox, then continuously removes things from this queue
# when ever there is something there... Could replace existing 'add', or at least
# it's use in PI_MiniPython.do() to place these in the queue?


# "LongLivedThread" is a thread which has to remain:
# a) separate from X-Plane name thread
# b) alive until (at least) the multiprocessing thread _it_ spawns, is complete
# Since there's no good way to know when the mp_thread is finished, it's
# not really possible to determine when LongLivedThread should be join'd
# with the main thread. So, we don't -- except if there's a second request
# we'll join the first and create anew.

LongLivedThread: threading.Thread = None


def getCommands(mainthread_q, regex):
    # getCommands() executes on the main X-Plane thread.
    # It spawns LongLivedThread, which will manage the work,
    # and then getCommands() returns.
    #
    # We pass a queue object, which will get whatever output generated,
    # and we pass 'regex', which will be used further down to reduce
    # data returned.

    global LongLivedThread
    if LongLivedThread is not None and LongLivedThread.is_alive():
        LongLivedThread.join()
    LongLivedThread = threading.Thread(target=getCommandsMP, name="LongLived", args=(mainthread_q, regex))
    LongLivedThread.start()


def getCommandsMP(mainthread_q, regex):
    # This is started on the LongLivedThread
    # it, in turn, spawns a multiprocessor thread ("Process")
    # because we need to separate this out of python/GIL
    # LongLivedThread lives waiting on "Process" thread,
    # reading data from "Process" until it receives a sentinal ("ALL DONE")
    # at which point, 'LongLivedThread' joins with 'Process', effectively
    # ending 'Process' thread.
    #
    # "Process" runs get(), and does HTTPD GET on a webserver and passes
    # _another_ queue object to get(). This get() process will put 'ALL DONE'
    # in the queue when get() completes (successfully or otherwise).
    #
    # getCommandMP(), running on LongLivedThread continously reads output
    # from mpQueue, looking for 'ALL DONE'. When found, it terminates the
    # "Process" thread
    # and writes the result to a mainthread_q
    #

    ctx = mp.get_context('spawn')
    mp_queue: mp.Queue = ctx.Queue()
    ProcessThread = ctx.Process(target=get, name="Process", args=(mp_queue, regex))
    ProcessThread.start()

    time.sleep(5)  # seems we need to not rush things...

    max_wait = 60
    waited = 0
    while True:
        if waited > max_wait:
            print("max wait")
            break
        try:
            data = mp_queue.get(True, 1)  # read from spawned get() proces
            waited = 0
            if data == 'ALL DONE':
                break
            else:
                mainthread_q.put(data)  # write to queue in Main Thread
        except queue.Empty:
            waited += 1
        except Exception as e:
            print(f"Other exception {e}")
            break
    ProcessThread.join()
    mainthread_q.put("[complete]")


def get(mp_queue=None, regex=None):
    PORT = 8086
    VERSION = 'v2'
    URL = f'http://localhost:{PORT}/api/{VERSION}'
    HEADERS = {'Content-Type': 'application/json', 'Accept': 'application/json,text/plain,*/*'}
    try:
        res = requests.get(f"{URL}/commands?fields=name,description", headers=HEADERS, timeout=10)
    except requests.ReadTimeout:
        print("read timeout")
        mp_queue.put("ALL DONE")
        return

    try:
        data_list = []
        for x in res.json()['data']:
            if regex is None or re.search(regex, x['name'], flags=re.IGNORECASE):
                data_list.append(f"{x['name']}    {x['description']}")

        data_list = sorted(data_list)
        for x in data_list:
            mp_queue.put(x)
        mp_queue.put('ALL DONE')
        return
    except queue.Full:
        print("queue is full")
