XPLMSound
===========
.. py:module:: XPLMSound
.. py:currentmodule:: xp

To use::

  import xp

This API provides access to the X-Plane Sound (FMOD) and requires at least X-Plane 12.04.

There are two approaches to using FMOD. X-Plane provides a basic interface to a few underlying FMOD
routines.
This allows you to load a sound file and set playing parameters using :py:func:`playPCMOnBus`
or :py:func:`playWaveOnBus`. The result is an FMOD channel, with which you can:

  * Locate the sound in 3d space with :py:func:`setAudioPosition`.

  * Change sound attenuation (fade) based on distance from the source with :py:func:`setAudioFadeDistance`

  * Create a direction cone of sound, outside of which the sound is more quiet (or silent) with :py:func:`setAudioCone`

  * Change source sound pitch :py:func:`setAudioPitch` and volume :py:func:`setAudioPitch`

  * Stop a playing sound :py:func:`stopAudio`

These sound functions are sufficient for basic FMOD behaviors.

An alternative interface provides lower-level
access to FMODStudio and FMODChannelGroups. This requires (from python) the heavy use of ``ctypes`` (See below).


Basic FMOD interface
--------------------

.. py:function:: playPCMOnBus(audioBuffer, bufferSize, soundFormat, freqHz, numChannels, loop=0, audioType=8, callback=None, refCon=None)

  Play an in-memory audio buffer as a given audioType (:ref:`XPLMAudioBus`). The resulting FMOD channel
  is returned. When the sound completes or is stopped by X-Plane, *the channel will go away*. It is up to
  you to invalidate any copy of the channel pointer you have lying around. You can do this
  by listening for the callback which is invoked when the sounds is finished (or stopped). The
  callback is optional, because if you have no intention of interacting with the sounds after it's
  launched, then you don't need to keep the channel pointer at all.
  
  The sound is not started instantly. Instead it will be started the next time X-Plane refreshes the sound system,
  typically at the start of the next frame. This allows you to set the initial position for the sound,
  if required. The callback will be called on the main thread, and will be called only
  once per sound. If the :py:func:`playPCMOnBus` call fails and you provided a callback function, you will get a callback
  with an FMOD status code.

  * `bufferSize` is length in bytes of passed data

  * `soundFormat` is width of sample (e.g., 1 of 8-bit, 2 for 16-bit, etc.) See enum `FMOD_SOUND_FORMAT <https://documentation.help/FMOD-Ex/FMOD_SOUND_FORMAT.html>`__.

  * `freqHz` is sample framerate (800, 22000, 44100, etc.)

  * `numChannels` is number of channels in the sample

  * `loop` indicates sound should loop (0= do not loop)

  * `audioType` selects the audioBus on which to play the audio. Defaults to 8= ``AudioUI``. See :ref:`XPLMAudioBus`

  Optional callback will receive two parameters, it does not need to return a value:

    * **refCon**:

      * reference constant you provided with call to :py:func:`playPCMOnBus` or :py:func:`playWaveOnBus`.

    * **status**:

      * integer status code, FMOD_RESULT. 0= FMOD_OK.  See `FMOD_RESULT <https://documentation.help/FMOD-Studio-API/FMOD_RESULT.html>`__
        for full list of possible codes.

  Note that a really easy way to work with PCM files on python is using the standard ``wave`` module. It can
  read the data and extract the required parameters:

  >>> def callback(refCon, status):
  ...     xp.log(f"Sound '{refCon}' ended, status is {status}")
  ...
  >>> import wave
  >>> w = wave.open('Resources/sounds/alert/seatbelt.wav')
  >>> data = w.readframes(w.getnframes())
  >>> channel = xp.playPCMOnBus(
  ...             data,
  ...             bufferSize=w.getnframes() * w.getsampwidth() * w.getnchannels(),
  ...             soundFormat=w.getsampwidth(),
  ...             freqHz=w.getframerate(),
  ...             numChannels=w.getnchannels(),
  ...             loop=0,
  ...             audioType=7,
  ...             callback=callback,
  ...             refCon="SeatBelt")
  ...
  >>> print(channel)
  <capsule object "FOD_CHANNELRefName" at 0x1e45c5710>
  
  With XPPython3Log.txt getting the entry:
  
    ``[Main] Sound 'SeatBelt' ended, status is 0``
  
  `Official SDK <https://developer.x-plane.com/sdk/XPLMSound/#XPLMPlayPCMOnBus>`__ :index:`XPLMPlayPCMOnBus`

.. py:function:: playWaveOnBus(wav, loop, audioType, callback, refCon)

  This is a pure-python convenience function which takes an opened Wave object and provides
  the parameters similar to what was done in the :py:func:`playPCMOnBus` example.

  >>> w = wave.open('Resources/sounds/alert/seatbelt.wav')
  >>> channel = xp.playWaveOnBus(w, loop=0, audioType=7)
  >>> print(channel)
  <capsule object "FOD_CHANNELRefName" at 0x1e45c5710>
  
.. py:function:: stopAudio(channel)
                 
  Stop playing an active channel (as returned by :py:func:`playPCMOnBus` or :py:func:`playWaveOnBus`.)
  If you defined a completion callback,
  it will be called. Once stopped, the channel is no longer valid and must not be used in any future calls.

  Needless to say, ``stopAudio()`` is crucial if you have a looping sound.

  Returns FMOD_RESULT. 0= FMOD_OK.  See `FMOD_RESULT <https://documentation.help/FMOD-Studio-API/FMOD_RESULT.html>`__

  >>> w = wave.open('Resources/sounds/alert/seatbelt.wav')
  >>> channel = xp.playWaveOnBus(w, loop=1, audioType=7)
  >>> xp.stopAudio(channel)
  0
  
  `Official SDK <https://developer.x-plane.com/sdk/XPLMSound/#XPLMStopAudio>`__ :index:`XPLMStopAudio`

.. py:function:: setAudioPosition(channel, position, velocity=None)

  Move the audio channel to a specific location in local (OpenGL) coordinates. This will set the sound
  to 3D if it is not already. Position is *required* if you want to use :py:func:`setAudioFadeDistance`
  or :py:func:`setAudioCone`.

  The channel parameter is as returned from :py:func:`playPCMOnBus`
  The position parameter is a tuple of three floats (x, y, z), commonly obtained through :py:func:`xp.worldToLocal`
  (See example.)

  The velocity parameter is movement from the position in meters per second. It is also a tuple of three
  floats (x, y, z). Positive-to-negative values correspond to movement along three axes: x-axis is east-west,
  y is up-down, z is south-north. If velocity parameter is None or not provided, the sound is stationary.

  Returns FMOD_RESULT. 0= FMOD_OK.  See `FMOD_RESULT <https://documentation.help/FMOD-Studio-API/FMOD_RESULT.html>`__

  >>> def getCurrentPosition():
  ...    lat = xp.getDatad(xp.findDataRef('sim/flightmodel/position/latitude'))
  ...    lon = xp.getDatad(xp.findDataRef('sim/flightmodel/position/longitude'))
  ...    alt = xp.getDatad(xp.findDataRef('sim/flightmodel/position/elevation'))
  ...    return xp.worldToLocal(lat, lon, alt)
  ...
  >>> position = getCurrentPosition()
  >>> position
  (129, 9, 28)
  >>> w = wave.open('Resources/sounds/alert/seatbelt.wav')
  >>> channel = xp.playWaveOnBus(w, loop=0, audioType=7)
  >>> xp.setAudioPosition(channel, position)
  0
  
  `Official SDK <https://developer.x-plane.com/sdk/XPLMSound/#XPLMSetAudioPosition>`__ :index:`XPLMSetAudioPosition`

  
.. py:function:: setAudioFadeDistance(channel, min_distance=1.0, max_distance=10000.0)
             
  Sets the minimum and maximum fad distances for a given channel. When the listener is
  in-between the minimum distance and the source, the volume will be at it's maximum.
  As the listener movies from the minimum distance to the maximum distance, the sound
  with attenuate. When outside the maximum distance the sound will no longer attenuate.

  Use minimum distance to give the impression that the sound is load or soft: Small
  quite objects such as a bumblebee, set minimum to 0.1. This would cause it to
  attenuate quickly and disappear when only a few meters away. A jumbo jet minimum
  might be 100 meters, thereby maintaining maximum volume until 100 meters away, with
  fade out over the next hundred meters.

  Maximum distance is effectively obsolete unless you need the sound to stop fading
  at a certain point. Do not adjust this from the default if you don't need to. Do not
  confuse maximum distance as the point where the sound will fade to zero: This is not
  the case.

  You can reset 3d sound back to 2d sound by passing negative values for both min and max.

  Returns FMOD_RESULT. 0= FMOD_OK.  See `FMOD_RESULT <https://documentation.help/FMOD-Studio-API/FMOD_RESULT.html>`__

  >>> w = wave.open('Resources/sounds/alert/seatbelt.wav')
  >>> channel = xp.playWaveOnBus(w, loop=1, audioType=7)
  >>> xp.setAudioPosition(channel, position)
  0
  >>> xp.setAudioFadeDistance(channel, .1)
  0
  
  `Official SDK <https://developer.x-plane.com/sdk/XPLMSound/#XPLMSetAudioFadeDistance>`__ :index:`XPLMSetAudioFadeDistance`

.. py:function:: setAudioVolume(channel, volume=1.0)

  Sets channel volume. Volume value represents a multiplier to the source. Values from 0 to 1 reduce source, numbers
  above 1 can be used to artificially amplify sound.

  Returns FMOD_RESULT. 0= FMOD_OK.  See `FMOD_RESULT <https://documentation.help/FMOD-Studio-API/FMOD_RESULT.html>`__

  >>> w = wave.open('Resources/sounds/alert/seatbelt.wav')
  >>> channel = xp.playWaveOnBus(w, loop=1, audioType=7)
  >>> xp.setAudioVolume(channel, .1)
  0
  
  `Official SDK <https://developer.x-plane.com/sdk/XPLMSound/#XPLMSetAudioVolume>`__ :index:`XPLMSetAudioVolume`
  
.. py:function:: setAudioPitch(channel, pitch=1.0)

  Change the current pitch of an active channel. Pitch value of 1 sets it to original source value. Greater than
  one increases the frequency, resulting in a higher pitch. Half the source pitch by setting the value to 0.5.

  Returns FMOD_RESULT. 0= FMOD_OK.  See `FMOD_RESULT <https://documentation.help/FMOD-Studio-API/FMOD_RESULT.html>`__

  >>> w = wave.open('Resources/sounds/alert/seatbelt.wav')
  >>> channel = xp.playWaveOnBus(w, loop=1, audioType=7)
  >>> xp.setAudioPitch(channel, 1.5)
  0
  
  `Official SDK <https://developer.x-plane.com/sdk/XPLMSound/#XPLMSetAudioPitch>`__ :index:`XPLMSetAudioPitch`
  
   
.. py:function:: setAudioCone(channel, inside_angle=360.0, outside_angle=360.0, outside_volume=1.0, orientation=None)

  Set a directional cone for an active channel. The orientation vector is in local coordinates.
  This will set the sound to 3d if it is not already.

  * *inside_angle* in degrees. Within this angle, sound is at normal volume.

  * *outside_angle* in degrees. Outside of this angle, sounds is at ``outside_volume``.

  * *outside_volume*: sound volume when greater than outside angle, 0.0 to 1.0.

  * *orientation*: OpenGL tuple (x, y, z). None is equivalent to (0, 0, -1), which is due North.

  See also `FMOD_SoundSet3DConeSettings <https://documentation.help/FMOD-API/FMOD_Sound_Set3DConeSettings.html>`__.

  Returns FMOD_RESULT. 0= FMOD_OK.  See `FMOD_RESULT <https://documentation.help/FMOD-Studio-API/FMOD_RESULT.html>`__

  The following example sets a cone with the source set by :py:func:`setAudioPosition`. The cone opens to the northeast
  and is 45 degrees wide. Within the cone, the volume is 100% (subject to fade). Outside of the cone, but within 180 degrees,
  the sound is 50% (subject to fade). "Behind" the cone there is no sound.

  >>> w = wave.open('Resources/sounds/alert/seatbelt.wav')
  >>> channel = xp.playWaveOnBus(w, loop=1, audioType=7)
  >>> xp.setAudioPosition(channel, position)
  0
  >>> xp.setAudioCone(channel, 45, 180, .5, (1, 0, -1))
  0
  

  `Official SDK <https://developer.x-plane.com/sdk/XPLMSound/#XPLMSetAudioCone>`__ :index:`XPLMSetAudioCone`

.. _XPLMAudioBus:

XPLMAudioBus
************

This enumeration states the type of audio you wish to play -- that is, the part of
the simulated environment that the audio belongs in. If you use FMOD directly, note that COM1, COM2,
Pilot and GND exist in a different FMOD bank so you may see these channels being unloaded/reloaded
independently of the others.

 +----------------------------------------------------------+------------------------------------------------------------------------------+
 |.. py:data:: AudioRadioCom1                               |Incoming speech on COM1                                                       |
 |  :value: 0                                               |                                                                              |
 +----------------------------------------------------------+------------------------------------------------------------------------------+
 |.. py:data:: AudioRadioCom2                               |Incoming speech on COM2                                                       |
 |  :value: 1                                               |                                                                              |
 +----------------------------------------------------------+------------------------------------------------------------------------------+
 |.. py:data:: AudioRadioPilot                              |Pilot's own speech                                                            |
 |  :value: 2                                               |                                                                              |
 +----------------------------------------------------------+------------------------------------------------------------------------------+
 |.. py:data:: AudioRadioCopilot                            |Copilot's own speech                                                          |
 |  :value: 3                                               |                                                                              |
 +----------------------------------------------------------+------------------------------------------------------------------------------+
 |.. py:data:: AudioExteriorAircraft                        |                                                                              |
 |  :value: 4                                               |                                                                              |
 +----------------------------------------------------------+------------------------------------------------------------------------------+
 |.. py:data:: AudioExteriorEnvironment                     |                                                                              |
 |  :value: 5                                               |                                                                              |
 +----------------------------------------------------------+------------------------------------------------------------------------------+
 |.. py:data:: AudioExteriorUnprocessed                     |                                                                              |
 |  :value: 6                                               |                                                                              |
 +----------------------------------------------------------+------------------------------------------------------------------------------+
 |.. py:data:: AudioInterior                                |                                                                              |
 |  :value: 7                                               |                                                                              |
 +----------------------------------------------------------+------------------------------------------------------------------------------+
 |.. py:data:: AudioUI                                      |                                                                              |
 |  :value: 8                                               |                                                                              |
 +----------------------------------------------------------+------------------------------------------------------------------------------+
 |.. py:data:: AudioGround                                  |Dedicated ground vehicle cable.                                               |
 |  :value: 9                                               |                                                                              |
 +----------------------------------------------------------+------------------------------------------------------------------------------+
 |.. py:data:: Master                                       |Master bus. Not normally to be used directly.                                 |
 |  :value: 10                                              |                                                                              |
 +----------------------------------------------------------+------------------------------------------------------------------------------+
                                                             

 `Official SDK <https://developer.x-plane.com/sdk/XPLMAudioBus/>`__ :index:`XPLMAudioBus`
 
Advanced FMOD interface
-----------------------

X-Plane provides two functions to gain access to the underlying FMOD interface. Use these results
to access additional FMOD functionality not otherwise supported via the interface.

.. note:: This gets you into the murky world of Python ``ctypes``, which is an exceptionally quick
          way to crash the simulator. The trade-off is you can do most anything.
          
.. py:function:: getFMODStudio()

  Retrieve handle (PyCapsule) to FMOD_STUDIO_SYSTEM, allowing you to load/process
  whatever else you need.

  >>> xp.getFMODStudio()
  <capsule object "FMOD_STUDIO_SYSTEM" at 0x122231>

  `Official SDK <https://developer.x-plane.com/sdk/XPLMSound/#XPLMGetFMODStudio>`__ :index:`XPLMGetFMODStudio`

.. py:function:: getFMODChannelGroup(audioType)

  Returns handle (PyCapsule) to the FMOD_CHANNELGROUP with the given index (one of
  :ref:`XPLMAudioBus`.)

  >>> xp.getFMODChannelGroup(xp.AudioUI)
  <capsule object "FMOD_CHANNELGROUP" at 0x1721231>

  `Official SDK <https://developer.x-plane.com/sdk/XPLMSound/#XPLMGetFMODChannelGroup>`__ :index:`XPLMGetFMODChannelGroup`

Python Capsules are opaque handles. Normally this is fine as we handle
translations between capsules and C-language pointers transparent. However,
because you'll want to use the underlying *pointer* to access unsupported FMOD routines
you'll need to extract the pointer from the capsule:

   >>> import ctypes
   >>> def PyCapsule_GetPointer(capsule, name):
   ...    # convenience function to get a void * out of a python capsule
   ...    ctypes.pythonapi.PyCapsule_GetPointer.restype = ctypes.c_void_p
   ...    ctypes.pythonapi.PyCapsule_GetPointer.argtypes = [ctypes.py_object, ctypes.c_char_p]
   ...    # cast it to c_void_p as otherwise it's an 'int'
   ...    return ctypes.c_void_p(ctypes.pythonapi.PyCapsule_GetPointer(capsule, name.encode('utf-8')))
   ...
   >>> PyCapsule_GetPointer(xp.getFMODStudio(), "FMOD_STUDIO_SYSTEM")
   c_void_p(2096927)

Next, you'll need to load the FMOD dynamic libraries (this is basic ``ctypes`` functionality). This
is platform specific *and* you need to load the exact filename. For example:

   >>> import platform
   >>> if platform.system() == 'Darwin':
   ...     studio_dll = ctypes.cdll.LoadLibrary('libfmodstudio.dylib')
   ...     fmod_dll = ctypes.cdll.LoadLibrary('libfmod.dylib')
   ... elif platform.system() == 'Windows':
   ...     studio_dll = ctypes.windll.LoadLibrary('fmodstudio')
   ...     fmod_dll = ctypes.windll.LoadLibrary('fmod')
   ... elif platform.system() == 'Linux':
   ...     studio_dll = ctypes.cdll.LoadLibrary('libfmodstudio.so.13')
   ...     fmod_dll = ctypes.cdll.LoadLibrary('libfmod.so.13')
   ...

Then, you can using ctypes to access other C-language functions, such as FMOD_Studio_System_GetCoreSystem().

   >>> studioObj = xp.getFMODStudio()
   >>> studio_ptr = PyCapsule_GetPointer(studioObj, "FMOD_STUDIO_SYSTEM")
   >>> coreSystem = ctypes.c_void_p()
   >>> studio_dll.FMOD_Studio_System_GetCoreSystem(studio_ptr, ctypes.byref(coreSystem))
   0
   >>> coreSystem
   c_void_p(1404993998824)
   
See the ``PI_FMOD_Advanced.py`` in :doc:`../samples`.
