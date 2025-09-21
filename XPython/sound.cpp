#define _GNU_SOURCE 1
#include <Python.h>
#include <fmod.h>
#include <fmod_studio.h>
#include <XPLM/XPLMSound.h>
#include <vector>
#include <string>
#include <unordered_map>
#include "plugin_dl.h"
#include "utils.h"
#include "capsules.h"

static intptr_t callbackCntr;
struct SoundCallbackInfo {
  PyObject *callback;
  PyObject *refCon;
  std::string module_name;
};

static std::unordered_map<intptr_t, SoundCallbackInfo> soundCallbacks;
static void soundCallback(void *inRefcon, FMOD_RESULT status);
  
/* See also https://qa.fmod.com/t/how-to-use-fmod-from-a-python-script/12293/2 */
/* or module pyfmodex ?? */

static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;

  // Clean up sound callbacks
  for (auto& pair : soundCallbacks) {
    Py_DECREF(pair.second.callback);
    Py_DECREF(pair.second.refCon);
  }
  soundCallbacks.clear();

  Py_RETURN_NONE;
}

#if defined (_FMOD_COMMON_H)
My_DOCSTR(_getFMODStudio__doc__, "getFMODStudio",
          "",
          "",
          "FMOD_STUDIO_SYSTEM",
          "Get PyCapsule to FMOD_STUDIO_SYSTEM, allowing you to load/process whatever\n"
          "else you need. You will need to use python ctypes to access. See\n"
          "documentation.");
static PyObject *XPLMGetFMODStudioFun(PyObject *self, PyObject *args)
{
  (void)self;
  (void)args;
  if (!XPLMGetFMODStudio_ptr) {
    PyErr_SetString(PyExc_RuntimeError , "XPLMGetFMODStudio is available only in XPLM400 and up and requires at least X-Plane v12.04.");
    return nullptr;
  }
  FMOD_STUDIO_SYSTEM *ret = XPLMGetFMODStudio_ptr();
  return makeCapsule(ret,"FMOD_STUDIO_SYSTEM");
}
  
My_DOCSTR(_getFMODChannelGroup__doc__, "getFMODChannelGroup",
          "audioType",
          "audioType:XPLMAudioBus",
          "FMOD_CHANNELGROUP",
          "Returns PyCapsule to the FMOD_CHANNELGROUP with the given index.\n"
          "You will need to use python ctypes to access. See documentation.");
static PyObject *XPLMGetFMODChannelGroupFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  (void)self;
  if (!XPLMGetFMODChannelGroup_ptr) {
    PyErr_SetString(PyExc_RuntimeError , "XPLMGetFMODChannelGroup is available only in XPLM400 and up and requires at least X-Plane v12.04.");
    return nullptr;
  }
  static char *keywords[] = {CHAR("audioType"), nullptr};
  XPLMAudioBus audioType;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "i", keywords, &audioType)){
    return nullptr;
  }

  FMOD_CHANNELGROUP *ret = XPLMGetFMODChannelGroup_ptr(audioType);
  return makeCapsule(ret, "FMOD_CHANNELGROUP");
}
#endif

My_DOCSTR(_playPCMOnBus__doc__, "playPCMOnBus",
          "audioBuffer, bufferSize, soundFormat, freqHz, numChannels, loop=0, audioType=8, callback=None, refCon=None",
          "audioBuffer:Any, bufferSize:int, soundFormat:int, freqHz:int, numChannels:int, loop:int=0, "
          "audioType:XPLMAudioBus=AudioUI, callback:Optional[Callable[[Any, int], None]]=None, refCon:Any=None",
          "None | FMODChannel",
          "Play provided data, of length bufferSize on the bus indicatedd by audioType. On\n"
          "completion, or stoppage, invoke (optional) callback with provided refCon.\n"
          " * soundFormat is # bytes per frame 1=8bit, 2=16bit, etc.\n"
          " * freqHz is sample framerate, e.g., 800, 22000, 44100\n"
          " * numChannels is e.g., 1=mono, 2=stereo\n"
          "\n"
          "Return audio FMODChannel on success.");
          
/*
  with open('Resouces/sounds/alert/40ft.wav', 'rb') as fp:
     data = fp.read()
  Other example:
      alert/seatbelt.wav is 22kHz, 16bit, stereo
  ....
  or
  import wave
  w = wave.open('Resources/sounds/alert/seatbelt.wav')
  data = w.readframes(w.getnframes())
  xp.playPCMOnBus(data,
                  (w.getnframes() * w.getsampwidth() * w.getnchannels())
                  w.getsampwidth(),  (I think), as it is "2" for 16bit, which matches the enum.
                  w.getframerate(),
                  w.getnchannels(),
                  0, # don't loop
                  7)
                  
  
  
  xp.playPCMOnBus(data, len(data),
                  1, # PCM8 bit  0=None, 2=PCM16, 3=PCM24, 4=PCM32
                  22000, (Get info-> file tells us 22KHz)
                  1, (mono)
                  0, (don't loop)
                  7)  whatever location #7 is...'Interior'... this is the channel not the "
                      location" (so user controlled volume affects this)
 */




static PyObject *XPLMPlayPCMOnBusFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  (void)self;
  (void)args;
  static char *keywords[] = {CHAR("audioBuffer"), CHAR("bufferSize"), CHAR("soundFormat"), CHAR("freqHz"),
                             CHAR("numChannels"), CHAR("loop"), CHAR("audioType"), CHAR("callback"), CHAR("refCon"), nullptr};
  if (!XPLMPlayPCMOnBus_ptr) {
    PyErr_SetString(PyExc_RuntimeError , "XPLMPlayPCMOnBus is available only in XPLM400 and up and requires at least X-Plane v12.04.");
    return nullptr;
  }

  PyObject *audioBufferObj;
  uint32_t bufferSize;
  int freqHz, numChannels, loop;
  PyObject *callbackObj=Py_None, *inRefConObj=Py_None;
  XPLMAudioBus audioType;
  FMOD_SOUND_FORMAT soundFormat;

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Oiiiiii|OO", keywords,
                                  &audioBufferObj, &bufferSize, &soundFormat,
                                  &freqHz, &numChannels, &loop, &audioType,
                                  &callbackObj, &inRefConObj)) {
    return nullptr;
  }

  if (!PyBytes_Check(audioBufferObj)) {
    pythonLog("Passed in audiobuffer not correct type");
    Py_RETURN_NONE;
  }

  void *audioBuffer = (void *)PyBytes_AsString(audioBufferObj);

  XPLMPCMComplete_f callback = nullptr;
  void *inRefcon = nullptr;
  if (callbackObj != Py_None) {
    /* setup callback, to call "my" function with a unique value as the refcon */
    callback = soundCallback;
    inRefcon = (void *)++callbackCntr;

    Py_INCREF(callbackObj);
    Py_INCREF(inRefConObj);
    soundCallbacks[callbackCntr] = {
      .callback = callbackObj,
      .refCon = inRefConObj,
      .module_name = std::string(CurrentPythonModuleName)
    };
  }
    
  FMOD_CHANNEL *fmod_channel = XPLMPlayPCMOnBus_ptr(audioBuffer,
                                                    bufferSize, soundFormat, freqHz, numChannels, loop, audioType,
                                                    callback, inRefcon);
  return makeCapsule(fmod_channel, "FMOD_CHANNEL");
}

static void soundCallback(void *inRefcon, FMOD_RESULT status)
{
  intptr_t refcon_id = (intptr_t)inRefcon;
  auto it = soundCallbacks.find(refcon_id);
  if (it == soundCallbacks.end()) {
    pythonLog("Couldn't find sound callback with id = %p.", inRefcon);
    return;
  }

  SoundCallbackInfo& info = it->second;
  set_moduleName(info.module_name);

  PyObject *statusObj = PyLong_FromLong(status);
  PyObject_CallFunctionObjArgs(info.callback, info.refCon, statusObj, nullptr);
  Py_DECREF(statusObj);

  // Clean up and remove from map because it'll get called only once by XP!
  Py_DECREF(info.callback);
  Py_DECREF(info.refCon);
  soundCallbacks.erase(it);
}

My_DOCSTR(_stopAudio__doc__, "stopAudio",
          "channel",
          "channel:FMOD_CHANNEL",
          "int",
          "Stop playing an active FMOD channel. If you defined a completion callback,\n"
          "this will be called. After this, the FMOD::Channel* will no longer be valid\n"
          "and must not be used in any future calls\n"
          "\n"
          "Returns FMOD_RESULT, 0= FMOD_OK");
static PyObject *XPLMStopAudioFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  (void)self;
  static char *keywords[] = {CHAR("channel"), nullptr};
  if (!XPLMStopAudio_ptr) {
    PyErr_SetString(PyExc_RuntimeError , "XPLMStopAudio is available only in XPLM400 and up and requires at least X-Plane v12.04.");
    return nullptr;
  }

  PyObject *fmod_channel_obj;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &fmod_channel_obj)){
    return nullptr;
  }
  FMOD_CHANNEL *fmod_channel = (FMOD_CHANNEL *)getVoidPtr(fmod_channel_obj, "FMOD_CHANNEL");
  FMOD_RESULT res = XPLMStopAudio_ptr(fmod_channel);
  return PyLong_FromLong(res);
}

My_DOCSTR(_setAudioPosition__doc__, "setAudioPosition",
          "channel, position, velocity=None",
          "channel:FMOD_CHANNEL, position:Sequence[float], velocity:Optional[Sequence[float]]=None",
          "int",
          "For audio channel, set position (for panning and attenuation) \n"
          "and velocity (for use with doppler).\n"
          " * position is OpenGL position (x, y, z) -- list of three float.\n"
          " * velocity is (meters/second) change in each (x, y, z) direction. None = (0, 0, 0)");
static PyObject *XPLMSetAudioPositionFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  (void)self;
  if (!XPLMSetAudioPosition_ptr) {
    PyErr_SetString(PyExc_RuntimeError , "XPLMSetAudioPosition is available only in XPLM400 and up and requires at least X-Plane v12.04.");
    return nullptr;
  }
  static char *keywords[] = {CHAR("channel"), CHAR("position"), CHAR("velocity"), nullptr};
  PyObject *fmod_channel_obj;
  PyObject *position_obj;
  PyObject *velocity_obj = Py_None;

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OO|O", keywords, &fmod_channel_obj, &position_obj, &velocity_obj)) {
    return nullptr;
  }

  if (!PySequence_Check(position_obj)) {
    pythonLog("Expected List object for position");
    PyErr_SetString(PyExc_ValueError, "XPLMSetAudioPosition 'position' parameter is not a list.");
    return nullptr;
  }
  if (PySequence_Size(position_obj) != 3) {
    pythonLog("Expected position to be of length 3, instead it is %ld", (long)PySequence_Size(position_obj));
    PyErr_SetString(PyExc_ValueError, "XPLMSetAudioPosition 'position' not length 3.");
    return nullptr;
  }
  FMOD_VECTOR position, velocity;
  PyObject *param;
  param = PySequence_GetItem(position_obj, 0);
  position.x = (float)PyFloat_AsDouble(param);
  Py_XDECREF(param);
  param = PySequence_GetItem(position_obj, 1);
  position.y = (float)PyFloat_AsDouble(param);
  Py_XDECREF(param);
  param = PySequence_GetItem(position_obj, 2);
  position.z = (float)PyFloat_AsDouble(param);
  Py_XDECREF(param);
    
  if (velocity_obj != Py_None) {
      param = PySequence_GetItem(velocity_obj, 0);
      velocity.x = (float)PyFloat_AsDouble(param);
      Py_XDECREF(param);
      param = PySequence_GetItem(velocity_obj, 1);
      velocity.y = (float)PyFloat_AsDouble(param);
      Py_XDECREF(param);
      param = PySequence_GetItem(velocity_obj, 2);
      velocity.z = (float)PyFloat_AsDouble(param);
      Py_XDECREF(param);
  } else {
    velocity.x = velocity.y = velocity.z = 0;
  }

  FMOD_CHANNEL *fmod_channel = (FMOD_CHANNEL *)getVoidPtr(fmod_channel_obj, "FMOD_CHANNEL");
  FMOD_RESULT res = XPLMSetAudioPosition_ptr(fmod_channel, &position, &velocity);
  return PyLong_FromLong(res);
}

My_DOCSTR(_setAudioFadeDistance__doc__, "setAudioFadeDistance",
          "channel, min_distance=1.0, max_distance=10000.0",
          "channel:FMOD_CHANNEL, min_distance:float=1.0, max_distance:float=10000.0",
          "int",
          "Sets minimum and maximum distance for the channel.\n"
          "When the listener is in-between the minimum distance and the source, the volume\n"
          "will be at it's maximum. As the listener moves from the minimum distance to the\n"
          "maximum distance, the sound will attenuate. When outside the maximum distance\n"
          "the sound will no longer attenuate.\n"
          "\n"
          "Use minimum distance to give the impression that the sound is loud or soft: Small\n"
          "quiet objects such as a bumblebee, set minimum to 0.1. This would cause it to \n"
          "attenuate quickly and dissapear when only a few meters away. A jumbo jet minimum\n"
          "might be 100 meters, thereby maintaining maximum volume until 100 meters away, with\n"
          "fade out over the next hundred meters.\n"
          "\n"
          "Maximum distance is effectively obsolete unless you need the sound to stop fading\n"
          "at a certain point. Do not adjust this from the default if you don't need to. Do\n"
          "not confuse maximum distance as the point where the sound will fade to zero, this\n"
          "is not the case.");
static PyObject *XPLMSetAudioFadeDistanceFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  (void)self;
  static char *keywords[] = {CHAR("channel"), CHAR("min_distance"), CHAR("max_distance"), nullptr};
  if (!XPLMSetAudioFadeDistance_ptr) {
    PyErr_SetString(PyExc_RuntimeError , "XPLMSetAudioFadeDistance is available only in XPLM400 and up and requires at least X-Plane v12.04.");
    return nullptr;
  }
  float min=1.0;
  float max=10000.0;
  PyObject *fmod_channel_obj;
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|ff", keywords, &fmod_channel_obj, &min, &max)) {
    return nullptr;
  }
  FMOD_CHANNEL *fmod_channel = (FMOD_CHANNEL *)getVoidPtr(fmod_channel_obj, "FMOD_CHANNEL");
  FMOD_RESULT res = XPLMSetAudioFadeDistance_ptr(fmod_channel, min, max);
  return PyLong_FromLong(res);
}

My_DOCSTR(_setAudioVolume__doc__, "setAudioVolume",
          "channel, volume=1.0",
          "channel:FMOD_CHANNEL, volume:float=1.0",
          "int",
          "Set the current volume of an active FMOD channel. This should be used to\n"
          "handle changes in the audio source volume, not for fading with distance.\n"
          "Values from 0.0 to 1.0 are normal, above 1 can be used to artificially amplify\n"
          "a sound.");
static PyObject *XPLMSetAudioVolumeFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  (void)self;
  static char *keywords[] = {CHAR("channel"), CHAR("volume"), nullptr};
  if (!XPLMSetAudioVolume_ptr) {
    PyErr_SetString(PyExc_RuntimeError , "XPLMSetAudioVolume is available only in XPLM400 and up and requires at least X-Plane v12.04.");
    return nullptr;
  }
  PyObject *fmod_channel_obj;
  float volume=1.0;

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|f", keywords, &fmod_channel_obj, &volume)) {
    return nullptr;
  }
  FMOD_CHANNEL *fmod_channel = (FMOD_CHANNEL *)getVoidPtr(fmod_channel_obj, "FMOD_CHANNEL");
  FMOD_RESULT res = XPLMSetAudioVolume_ptr(fmod_channel, volume);
  return PyLong_FromLong(res);
}

My_DOCSTR(_setAudioPitch__doc__, "setAudioPitch",
          "channel, pitch=1.0",
          "channel:FMOD_CHANNEL, pitch:float=1.0",
          "int",
          "Change the current pitch of an active FMOD channel.\n"
          "This is a multiplier to the original channel value *not* a new frequency,"
          "so '0.8' lowers the pitch, '1.2' raises the pitch, and '1.0' sets the"
          "pitch to same as initialized.");
static PyObject *XPLMSetAudioPitchFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  (void)self;
  static char *keywords[] = {CHAR("channel"), CHAR("pitch"), nullptr};
  if (!XPLMSetAudioPitch_ptr) {
    PyErr_SetString(PyExc_RuntimeError , "XPLMSetAudioPitch is available only in XPLM400 and up and requires at least X-Plane v12.04.");
    return nullptr;
  }
  PyObject *fmod_channel_obj;
  float herz = 1.0;

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|f", keywords, &fmod_channel_obj, &herz)) {
    return nullptr;
  }
  FMOD_CHANNEL *fmod_channel = (FMOD_CHANNEL *)getVoidPtr(fmod_channel_obj, "FMOD_CHANNEL");
  FMOD_RESULT res = XPLMSetAudioPitch_ptr(fmod_channel, herz);
  return PyLong_FromLong(res);
}

My_DOCSTR(_setAudioCone__doc__, "setAudioCone",
          "channel, inside_angle=360.0, outside_angle=360.0, outside_volume=1.0, orientation=None",
          "channel:FMOD_CHANNEL, inside_angle:float=360.0, outside_angle:float=360.0, outside_volume:float=1.0, orientation:Optional[tuple[float, float, float]]=None",
          "int",
          "Set a direction code for an active FMOD channel. The orientation vector is in local coordinates.\n"
          "This will set the sound to 3D if it is not already.\n"
          " * inside_angle: degrees, within this angle, sound is at normal volume.\n"
          " * outside_angle: degrees, outside of this angle, sound is at outside volume.\n"
          " * outside_volume: sound volume when greater than outside angle, 0 to 1.0 default=1.0"
          " * Orientation of sound: OpenGL vector (x, y, z). None == (0,0,-1) which is due North");
// https://documentation.help/FMOD-API/FMOD_Sound_Set3DConeSettings.html
static PyObject *XPLMSetAudioConeFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  (void)self;
  static char *keywords[] = {CHAR("channel"), CHAR("inside_angle"), CHAR("outside_angle"), CHAR("outside_volume"), CHAR("orientation"), nullptr};
  if (!XPLMSetAudioCone_ptr) {
    PyErr_SetString(PyExc_RuntimeError , "XPLMSetAudioCone is available only in XPLM400 and up and requires at least X-Plane v12.04.");
    return nullptr;
  }

  PyObject *fmod_channel_obj, *orientation_obj=Py_None;
  float inside_angle=360.0, outside_angle=360.0, outside_volume=1.0;

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|fffO", keywords,
                                   &fmod_channel_obj,
                                   &inside_angle, &outside_angle, &outside_volume,
                                   &orientation_obj)) {
    return nullptr;
  }
  FMOD_CHANNEL *fmod_channel = (FMOD_CHANNEL *)getVoidPtr(fmod_channel_obj, "FMOD_CHANNEL");
  FMOD_VECTOR orientation;
  if (orientation_obj != Py_None) {
    if (!PySequence_Check(orientation_obj)) {
      pythonLog("Expected List object for orientation");
      PyErr_SetString(PyExc_ValueError, "XPLMSetAudioCone 'orientation' parameter is not a list.");
      return nullptr;
    }
    if (PySequence_Size(orientation_obj) != 3) {
      pythonLog("Expected orientation to be of length 3, instead it is %ld", (long)PySequence_Size(orientation_obj));
      PyErr_SetString(PyExc_ValueError, "XPLMSetAudioCone 'orientation' not length 3.");
      return nullptr;
    }
    PyObject *param;
    param = PySequence_GetItem(orientation_obj, 0);
    orientation.x = (float)PyFloat_AsDouble(param);
    Py_XDECREF(param);
    
    param = PySequence_GetItem(orientation_obj, 1);
    orientation.y = (float)PyFloat_AsDouble(param);
    Py_XDECREF(param);
    
    param = PySequence_GetItem(orientation_obj, 2);
    orientation.z = (float)PyFloat_AsDouble(param);
    Py_XDECREF(param);
  } else {
    orientation.x = orientation.y = 0.0;
    orientation.z = -1.0;
  }
      
  FMOD_RESULT res = XPLMSetAudioCone_ptr(fmod_channel, inside_angle, outside_angle, outside_volume, &orientation);
  return PyLong_FromLong(res);
}


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
static PyMethodDef XPLMSoundMethods[] = {
#if defined (_FMOD_COMMON_H)  
  {"getFMODStudio", (PyCFunction)XPLMGetFMODStudioFun, METH_VARARGS, _getFMODStudio__doc__},
  {"XPLMGetFMODStudio", (PyCFunction)XPLMGetFMODStudioFun, METH_VARARGS, ""},
  {"getFMODChannelGroup", (PyCFunction)XPLMGetFMODChannelGroupFun, METH_VARARGS | METH_KEYWORDS, _getFMODChannelGroup__doc__},
  {"XPLMGetFMODChannelGroup", (PyCFunction)XPLMGetFMODChannelGroupFun, METH_VARARGS | METH_KEYWORDS, ""},
#endif
  {"playPCMOnBus", (PyCFunction)XPLMPlayPCMOnBusFun, METH_VARARGS | METH_KEYWORDS, _playPCMOnBus__doc__},
  {"XPLMPlayPCMOnBus", (PyCFunction)XPLMPlayPCMOnBusFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"stopAudio", (PyCFunction)XPLMStopAudioFun, METH_VARARGS | METH_KEYWORDS, _stopAudio__doc__},
  {"XPLMStopAudio", (PyCFunction)XPLMStopAudioFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"setAudioPosition", (PyCFunction)XPLMSetAudioPositionFun, METH_VARARGS | METH_KEYWORDS, _setAudioPosition__doc__},
  {"XPLMSetAudioPosition", (PyCFunction)XPLMSetAudioPositionFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"setAudioFadeDistance", (PyCFunction)XPLMSetAudioFadeDistanceFun, METH_VARARGS | METH_KEYWORDS, _setAudioFadeDistance__doc__},
  {"XPLMSetAudioFadeDistance", (PyCFunction)XPLMSetAudioFadeDistanceFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"setAudioVolume", (PyCFunction)XPLMSetAudioVolumeFun, METH_VARARGS | METH_KEYWORDS, _setAudioVolume__doc__},
  {"XPLMSetAudioVolume", (PyCFunction)XPLMSetAudioVolumeFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"setAudioPitch", (PyCFunction)XPLMSetAudioPitchFun, METH_VARARGS | METH_KEYWORDS, _setAudioPitch__doc__},
  {"XPLMSetAudioPitch", (PyCFunction)XPLMSetAudioPitchFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"setAudioCone", (PyCFunction)XPLMSetAudioConeFun, METH_VARARGS | METH_KEYWORDS, _setAudioCone__doc__},
  {"XPLMSetAudioCone", (PyCFunction)XPLMSetAudioConeFun, METH_VARARGS | METH_KEYWORDS, ""},  
  {"_cleanup", cleanup, METH_VARARGS, ""},
  {nullptr, nullptr, 0, nullptr}
};
#pragma GCC diagnostic pop


static struct PyModuleDef XPLMSoundModule = {
  PyModuleDef_HEAD_INIT,
  "XPLMSound",
  "Laminar documentation: \n"
  "   https://developer.x-plane.com/sdk/XPLMSound/\n"
  "XPPython3 documentation: \n"
  "   https://xppython3.rtfd.io/en/stable/development/modules/sound.html",
  -1,
  XPLMSoundMethods,
  nullptr,
  nullptr,
  nullptr,
  nullptr
};

PyMODINIT_FUNC
PyInit_XPLMSound(void)
{
  // soundCallbacks is now a C++ unordered_map, no need to initialize
  PyObject *mod = PyModule_Create(&XPLMSoundModule);
  if(mod){
    PyModule_AddStringConstant(mod, "__author__", "Peter Buckner (pbuck@xppython3.org)");
#if defined(XPLM400)
    PyModule_AddIntConstant(mod, "AudioRadioCom1", xplm_AudioRadioCom1); // XPLMAudioBus
    PyModule_AddIntConstant(mod, "AudioRadioCom2", xplm_AudioRadioCom2); // XPLMAudioBus
    PyModule_AddIntConstant(mod, "AudioRadioPilot", xplm_AudioRadioPilot); // XPLMAudioBus
    PyModule_AddIntConstant(mod, "AudioRadioCopilot", xplm_AudioRadioCopilot); // XPLMAudioBus
    PyModule_AddIntConstant(mod, "AudioExteriorAircraft", xplm_AudioExteriorAircraft); // XPLMAudioBus
    PyModule_AddIntConstant(mod, "AudioExteriorEnvironment", xplm_AudioExteriorEnvironment); // XPLMAudioBus
    PyModule_AddIntConstant(mod, "AudioExteriorUnprocessed", xplm_AudioExteriorUnprocessed); // XPLMAudioBus
    PyModule_AddIntConstant(mod, "AudioInterior", xplm_AudioInterior); // XPLMAudioBus
    PyModule_AddIntConstant(mod, "AudioUI", xplm_AudioUI); // XPLMAudioBus
    PyModule_AddIntConstant(mod, "AudioGround", xplm_AudioGround); // XPLMAudioBus
    PyModule_AddIntConstant(mod, "Master", xplm_Master); // XPLMAudioBus
    PyModule_AddIntConstant(mod, "MasterBank", xplm_MasterBank); // XPLMBankID
    PyModule_AddIntConstant(mod, "RadioBank", xplm_RadioBank); // XPLMBankID
    PyModule_AddIntConstant(mod, "FMOD_OK", FMOD_OK);
    PyModule_AddIntConstant(mod, "FMOD_SOUND_FORMAT_PCM16", FMOD_SOUND_FORMAT_PCM16);
#else
    PyModule_AddIntConstant(mod, "AudioRadioCom1", -1); // XPLMAudioBus
    PyModule_AddIntConstant(mod, "AudioRadioCom2", -1); // XPLMAudioBus
    PyModule_AddIntConstant(mod, "AudioRadioPilot", -1); // XPLMAudioBus
    PyModule_AddIntConstant(mod, "AudioRadioCopilot", -1); // XPLMAudioBus
    PyModule_AddIntConstant(mod, "AudioExteriorAircraft", -1); // XPLMAudioBus
    PyModule_AddIntConstant(mod, "AudioExteriorEnvironment", -1); // XPLMAudioBus
    PyModule_AddIntConstant(mod, "AudioExteriorUnprocessed", -1); // XPLMAudioBus
    PyModule_AddIntConstant(mod, "AudioInterior", -1); // XPLMAudioBus
    PyModule_AddIntConstant(mod, "AudioUI", -1); // XPLMAudioBus
    PyModule_AddIntConstant(mod, "AudioGround", -1); // XPLMAudioBus
    PyModule_AddIntConstant(mod, "Master", -1); // XPLMAudioBus
    PyModule_AddIntConstant(mod, "MasterBank", -1); // XPLMBankID
    PyModule_AddIntConstant(mod, "RadioBank", -1); // XPLMBankID
    PyModule_AddIntConstant(mod, "FMOD_OK", -1);
    PyModule_AddIntConstant(mod, "FMOD_SOUND_FORMAT_PCM16", -1);
#endif
  }
  return mod;
}

