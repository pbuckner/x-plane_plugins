#define _GNU_SOURCE 1
#include <Python.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdbool.h>
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMGraphics.h>
#include "utils.h"


static PyObject *XPLMSetGraphicsStateFun(PyObject *self, PyObject *args)
{
  (void) self;

  int inEnableFog;
  int inNumberTexUnits;
  int inEnableLighting;
  int inEnableAlphaTesting;
  int inEnableAlphaBlending;
  int inEnableDepthTesting;
  int inEnableDepthWriting;

  if(!PyArg_ParseTuple(args, "iiiiiii", &inEnableFog, &inNumberTexUnits, &inEnableLighting, &inEnableAlphaTesting,
                                        &inEnableAlphaBlending, &inEnableDepthTesting, &inEnableDepthWriting)){
    return NULL;
  }

  XPLMSetGraphicsState(inEnableFog, inNumberTexUnits, inEnableLighting,
                       inEnableAlphaTesting, inEnableAlphaBlending,
                       inEnableDepthTesting, inEnableDepthWriting);
  Py_RETURN_NONE;
} 

static PyObject *XPLMBindTexture2dFun(PyObject *self, PyObject *args)
{
  (void) self;
  int inTextureNum;
  int inTextureUnit;

  if(!PyArg_ParseTuple(args, "ii", &inTextureNum, &inTextureUnit)){
    return NULL;
  }

  XPLMBindTexture2d(inTextureNum, inTextureUnit); 
  Py_RETURN_NONE;
} 

static PyObject *XPLMGenerateTextureNumbersFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *outTextureIDs;
  int inCount;

  if(!PyArg_ParseTuple(args, "i", &inCount)){
    return NULL;
  }

  int *array = (int *)malloc(sizeof(int) * inCount);
  if(!array){
    PyErr_SetString(PyExc_RuntimeError , "Can't malloc outTextureIDs.");
    return NULL;
  }
  
  XPLMGenerateTextureNumbers(array, inCount);
  int i;
  outTextureIDs = PyList_New(0);
  for(i = 0; i < inCount; ++i){
    PyObject *tmp = PyLong_FromLong(array[i]);
    if(PyList_Append(outTextureIDs, tmp)){
      printf("Problem appending item!\n");
    }
    Py_DECREF(tmp);
  }
  free(array);
  return outTextureIDs;
} 

#if defined(XPLM_DEPRECATED)
static PyObject *XPLMGetTextureFun(PyObject *self, PyObject *args)
{
  (void) self;
  int inTexture;
  if(!PyArg_ParseTuple(args, "i", &inTexture)){
    return NULL;
  }

  int tex = XPLMGetTexture(inTexture);
  return PyLong_FromLong(tex);
}
#endif

static PyObject *XPLMWorldToLocalFun(PyObject *self, PyObject *args)
{
  (void) self;
  double inLatitude;
  double inLongitude;
  double inAltitude;
  double outX, outY, outZ;
  if(!PyArg_ParseTuple(args, "ddd", &inLatitude, &inLongitude, &inAltitude)){
    return NULL;
  }
  XPLMWorldToLocal(inLatitude, inLongitude, inAltitude, &outX, &outY, &outZ);
  
  PyObject *res = PyTuple_New(3);
  PyTuple_SetItem(res, 0, PyFloat_FromDouble(outX));
  PyTuple_SetItem(res, 1, PyFloat_FromDouble(outY));
  PyTuple_SetItem(res, 2, PyFloat_FromDouble(outZ));
  return res;
}

static PyObject *XPLMLocalToWorldFun(PyObject *self, PyObject *args)
{
  (void) self;
  double inX;
  double inY;
  double inZ;
  double outLatitude, outLongitude, outAltitude;
  if(!PyArg_ParseTuple(args, "ddd", &inX, &inY, &inZ)){
    return NULL;
  }
  XPLMLocalToWorld(inX, inY, inZ, &outLatitude, &outLongitude, &outAltitude);
  
  PyObject *res = PyTuple_New(3);
  PyTuple_SetItem(res, 0, PyFloat_FromDouble(outLatitude));
  PyTuple_SetItem(res, 1, PyFloat_FromDouble(outLongitude));
  PyTuple_SetItem(res, 2, PyFloat_FromDouble(outAltitude));
  return res;
}

static PyObject *XPLMDrawTranslucentDarkBoxFun(PyObject *self, PyObject *args)
{
  (void) self;
  int inLeft;
  int inTop;
  int inRight;
  int inBottom;

  if(!PyArg_ParseTuple(args, "iiii", &inLeft, &inTop, &inRight, &inBottom)){
    return NULL;
  }
  XPLMDrawTranslucentDarkBox(inLeft, inTop, inRight, inBottom);
  Py_RETURN_NONE;
}

static PyObject *XPLMDrawStringFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *rgbList;
  int inXOffset;
  int inYOffset;
  const char *inCharC;
  char *inChar;
  PyObject *wordWrapWidthObj;
  int wordWrapWidth;
  int *inWordWrapWidth = NULL;
  int inFontID;

  if(!PyArg_ParseTuple(args, "OiisOi", &rgbList, &inXOffset, &inYOffset, &inCharC, &wordWrapWidthObj, &inFontID)){
    return NULL;
  }
  if(PySequence_Size(rgbList) != 3){
    PyErr_SetString(PyExc_TypeError , "inColourRGB must have 3 items");
    return NULL;
  }
  PyObject *rgbTuple = PySequence_Tuple(rgbList);
  float inColorRGB[3];
  inColorRGB[0] = getFloatFromTuple(rgbTuple, 0);
  inColorRGB[1] = getFloatFromTuple(rgbTuple, 1);
  inColorRGB[2] = getFloatFromTuple(rgbTuple, 2);
  Py_DECREF(rgbTuple);
  if(wordWrapWidthObj != Py_None){
    wordWrapWidth = PyLong_AsLong(wordWrapWidthObj);
    if(wordWrapWidth != 0){
      inWordWrapWidth = &wordWrapWidth;
    }
  }
  inChar = strdup(inCharC);
  XPLMDrawString(inColorRGB, inXOffset, inYOffset, inChar, inWordWrapWidth, inFontID);
  free(inChar);
  Py_RETURN_NONE;
}

static PyObject *XPLMDrawNumberFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *rgbList;
  int inXOffset;
  int inYOffset;
  double inValue;
  int inDigits;
  int inDecimals;
  int inShowSign;
  int inFontID;

  if(!PyArg_ParseTuple(args, "Oiidiiii", &rgbList, &inXOffset, &inYOffset, &inValue,
                       &inDigits, &inDecimals, &inShowSign, &inFontID)){
    return NULL;
  }
  if(PySequence_Size(rgbList) != 3){
    PyErr_SetString(PyExc_TypeError , "inColourRGB must have 3 items");
    return NULL;
  }
  PyObject *rgbTuple = PySequence_Tuple(rgbList);
  float inColorRGB[3];
  inColorRGB[0] = getFloatFromTuple(rgbTuple, 0);
  inColorRGB[1] = getFloatFromTuple(rgbTuple, 1);
  inColorRGB[2] = getFloatFromTuple(rgbTuple, 2);
  Py_DECREF(rgbTuple);

  XPLMDrawNumber(inColorRGB, inXOffset, inYOffset, inValue, inDigits, inDecimals, inShowSign, inFontID);

  Py_RETURN_NONE;
}

static PyObject *XPLMGetFontDimensionsFun(PyObject *self, PyObject *args)
{
  (void) self;
  int inFontID;
  if(!PyArg_ParseTuple(args, "i", &inFontID)) {
    return NULL;
  }

  int outCharWidth, outCharHeight, outDigitsOnly;
  XPLMGetFontDimensions(inFontID, &outCharWidth, &outCharHeight, &outDigitsOnly);
  return Py_BuildValue("(iii)", outCharWidth, outCharHeight, outDigitsOnly);
}

static PyObject *XPLMMeasureStringFun(PyObject *self, PyObject *args)
{
  (void) self;
  int inFontID;
  char *inChar;
  int inNumChars;

  if(!PyArg_ParseTuple(args, "is", &inFontID, &inChar)) {
    return NULL;
  }
  inNumChars = strlen(inChar);
  return PyFloat_FromDouble(XPLMMeasureString(inFontID, inChar, inNumChars));
}


static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  Py_RETURN_NONE;
}

static PyMethodDef XPLMGraphicsMethods[] = {
  {"XPLMSetGraphicsState", XPLMSetGraphicsStateFun, METH_VARARGS, "Sets state of the graphics pipeline."},
  {"XPLMBindTexture2d", XPLMBindTexture2dFun, METH_VARARGS, "Bind a 2D texture."},
  {"XPLMGenerateTextureNumbers", XPLMGenerateTextureNumbersFun, METH_VARARGS, "Generates number of texture IDs."},
#if defined(XPLM_DEPRECATED)
  {"XPLMGetTexture", XPLMGetTextureFun, METH_VARARGS, "DEPRECATED"},
#endif
  {"XPLMWorldToLocal", XPLMWorldToLocalFun, METH_VARARGS, "Transform world coordinates to local."},
  {"XPLMLocalToWorld", XPLMLocalToWorldFun, METH_VARARGS, "Transform local coordinates to world."},
  {"XPLMDrawTranslucentDarkBox", XPLMDrawTranslucentDarkBoxFun, METH_VARARGS, "Draw translucent window."},
  {"XPLMDrawString", XPLMDrawStringFun, METH_VARARGS, "Draw string."},
  {"XPLMDrawNumber", XPLMDrawNumberFun, METH_VARARGS, "Draw number."},
  {"XPLMGetFontDimensions", XPLMGetFontDimensionsFun, METH_VARARGS, "Get fond dimmensions."},
  {"XPLMMeasureString", XPLMMeasureStringFun, METH_VARARGS, "Measure a string."},
  {"cleanup", cleanup, METH_VARARGS, ""},
  {NULL, NULL, 0, NULL}
};

static struct PyModuleDef XPLMGraphicsModule = {
  PyModuleDef_HEAD_INIT,
  "XPLMGraphics",
  NULL,
  -1,
  XPLMGraphicsMethods,
  NULL,
  NULL,
  NULL,
  NULL
};

PyMODINIT_FUNC
PyInit_XPLMGraphics(void)
{
  PyObject *mod = PyModule_Create(&XPLMGraphicsModule);
  if(mod){
     /* The bitmap that contains window outlines, button outlines, fonts, etc.      */
    PyModule_AddIntConstant(mod, "xplm_Tex_GeneralInterface", xplm_Tex_GeneralInterface);
     /* Mono-spaced font for user interface.  Available in all versions of the SDK. */
    PyModule_AddIntConstant(mod, "xplmFont_Basic", xplmFont_Basic);
     /* Proportional UI font.                                                       */
    PyModule_AddIntConstant(mod, "xplmFont_Proportional", xplmFont_Proportional);
  }

  return mod;
}


