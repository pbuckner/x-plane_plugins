#define _GNU_SOURCE 1
#include <Python.h>
#include <sys/time.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMGraphics.h>
#include "utils.h"


My_DOCSTR(_setGraphicsState__doc__, "setGraphicsState", "fog=0, numberTexUnits=0, lighting=0, alphaTesting=0, alphaBlending=0, depthTesting=0, depthWriting=0",
          "Change OpenGL's graphics state.\n"
          "\n"
          "Use instead of any glEnable / glDisable calls.");
static PyObject *XPLMSetGraphicsStateFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"fog", "numberTexUnits", "lighting", "alphaTesting", "alphaBlending", "depthTesting", "depthWriting", NULL};
  (void) self;

  int inEnableFog = 0;
  int inNumberTexUnits = 0;
  int inEnableLighting = 0;
  int inEnableAlphaTesting = 0;
  int inEnableAlphaBlending = 0;
  int inEnableDepthTesting = 0;
  int inEnableDepthWriting = 0;

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|iiiiiii", keywords, &inEnableFog, &inNumberTexUnits, &inEnableLighting, &inEnableAlphaTesting,
                                  &inEnableAlphaBlending, &inEnableDepthTesting, &inEnableDepthWriting)){
    return NULL;
  }

  XPLMSetGraphicsState(inEnableFog, inNumberTexUnits, inEnableLighting,
                       inEnableAlphaTesting, inEnableAlphaBlending,
                       inEnableDepthTesting, inEnableDepthWriting);
  Py_RETURN_NONE;
} 

My_DOCSTR(_bindTexture2d__doc__, "bindTexture2d", "textureID, textureUnit",
          "Changes currently bound OpenGL texture.\n"
          "\n"
          "Use instead of glBindTexture(GL_TEXTURE_2D, ...)");
static PyObject *XPLMBindTexture2dFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"textureID", "textureUnit", NULL};
  (void) self;
  int inTextureNum;
  int inTextureUnit;

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "ii", keywords, &inTextureNum, &inTextureUnit)){
    return NULL;
  }

  XPLMBindTexture2d(inTextureNum, inTextureUnit); 
  Py_RETURN_NONE;
} 

My_DOCSTR(_generateTextureNumbers__doc__, "generateTextureNumbers", "count",
          "Generate number of textures for a plugin.\n"
          "\n"
          "Returns list of numbers.");
static PyObject *XPLMGenerateTextureNumbersFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"idlist", "count", NULL};
  (void) self;
  PyObject *outTextureIds;
  int inCount=1;
  int returnValues = 0;

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Oi", keywords, &outTextureIds, &inCount)){
    PyErr_Clear();
    returnValues = 1;
    static char *nkeywords[] = {"count", NULL};
    if(!PyArg_ParseTupleAndKeywords(args, kwargs, "i", nkeywords, &inCount)){
      return NULL;
    }
  }

  int *array = (int *)malloc(sizeof(int) * inCount);
  if(!array){
    PyErr_SetString(PyExc_RuntimeError , "Can't malloc outTextureIDs.");
    return NULL;
  }
  
  XPLMGenerateTextureNumbers(array, inCount);
  int i;
  if (returnValues) {
    outTextureIds = PyList_New(0);
  }
  for(i = 0; i < inCount; ++i){
    PyObject *tmp = PyLong_FromLong(array[i]);
    if(PyList_Append(outTextureIds, tmp)){
      printf("Problem appending item to generateTextureNumbers idlist!\n");
    }
    Py_DECREF(tmp);
  }
  free(array);
  if (returnValues) {
    return outTextureIds;
  }
  Py_RETURN_NONE;
} 

My_DOCSTR(_worldToLocal__doc__, "worldToLocal", "lat, lon, alt=0",
          "Convert Lat/Lon/Alt to local scene coordinates (x, y, z)\n"
          "\n"
          "Latitude and longitude are decimal degrees, altitude is meters MSL.\n"
          "Returns (x, y, z) in meters, in local OpenGL coordinates.");
static PyObject *XPLMWorldToLocalFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"lat", "lon", "alt", NULL};
  (void) self;
  double inLatitude;
  double inLongitude;
  double inAltitude = 0.0;
  double outX, outY, outZ;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "ddd", keywords, &inLatitude, &inLongitude, &inAltitude)){
    return NULL;
  }
  XPLMWorldToLocal(inLatitude, inLongitude, inAltitude, &outX, &outY, &outZ);
  
  PyObject *res = PyTuple_New(3);
  PyTuple_SetItem(res, 0, PyFloat_FromDouble(outX));
  PyTuple_SetItem(res, 1, PyFloat_FromDouble(outY));
  PyTuple_SetItem(res, 2, PyFloat_FromDouble(outZ));
  return res;
}

My_DOCSTR(_localToWorld__doc__, "localToWorld", "x, y z",
          "Convert local scene coordinates (x, y, z) into (lat, lon, alt)\n"
          "\n"
          "Latitude and longitude are decimal degrees, altitude is meters MSL.");
static PyObject *XPLMLocalToWorldFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"x", "y", "z", NULL};
  (void) self;
  double inX;
  double inY;
  double inZ;
  double outLatitude, outLongitude, outAltitude;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "ddd", keywords, &inX, &inY, &inZ)){
    return NULL;
  }
  XPLMLocalToWorld(inX, inY, inZ, &outLatitude, &outLongitude, &outAltitude);
  
  PyObject *res = PyTuple_New(3);
  PyTuple_SetItem(res, 0, PyFloat_FromDouble(outLatitude));
  PyTuple_SetItem(res, 1, PyFloat_FromDouble(outLongitude));
  PyTuple_SetItem(res, 2, PyFloat_FromDouble(outAltitude));
  return res;
}

My_DOCSTR(_drawTranslucentDarkBox__doc__, "drawTranslucentDarkBox", "left, top, right, bottom",
          "Draw translucent dark box at location.");
static PyObject *XPLMDrawTranslucentDarkBoxFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"left", "top", "right", "bottom", NULL};
  (void) self;
  int inLeft;
  int inTop;
  int inRight;
  int inBottom;

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "iiii", keywords, &inLeft, &inTop, &inRight, &inBottom)){
    return NULL;
  }
  XPLMDrawTranslucentDarkBox(inLeft, inTop, inRight, inBottom);
  Py_RETURN_NONE;
}

My_DOCSTR(_drawString__doc__, "drawString", "rgb=(1., 1., 1.), x=0, y=0, value=\"\", wordWrapWidth=None, fontID=18",
          "Draw a string at location (x, y)\n"
          "\n"
          "Default color is white (1., 1., 1.)");
static PyObject *XPLMDrawStringFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"rgb", "x", "y", "value", "wordWrapWidth", "fontID", NULL};
  (void) self;
  PyObject *rgbList = Py_None;
  int inXOffset = 0;
  int inYOffset = 0;
  const char *inCharC = NULL;
  char *inChar;
  PyObject *wordWrapWidthObj = Py_None;
  int wordWrapWidth;
  int *inWordWrapWidth = NULL;
  int inFontID = xplmFont_Proportional;

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|OiisOi", keywords, &rgbList, &inXOffset, &inYOffset, &inCharC, &wordWrapWidthObj, &inFontID)){
    return NULL;
  }
  float inColorRGB[3];
  if (rgbList == Py_None) {
    inColorRGB[0] = 1.0;
    inColorRGB[1] = 1.0;
    inColorRGB[2] = 1.0;
  } else if(PySequence_Size(rgbList) != 3){
    PyErr_SetString(PyExc_TypeError , "inColourRGB must have 3 items");
    return NULL;
  } else {
    inColorRGB[0] = PyFloat_AsDouble(PySequence_GetItem(rgbList, 0));
    inColorRGB[1] = PyFloat_AsDouble(PySequence_GetItem(rgbList, 1));
    inColorRGB[2] = PyFloat_AsDouble(PySequence_GetItem(rgbList, 2));
  }
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

My_DOCSTR(_drawNumber__doc__, "drawNumber", "rgb=(1., 1., 1.), x=0, y=0, value=0.0, digits=-1, decimals=0, showSign=1, fontID=18",
          "Draw a number at location (x, y)\n"
          "\n"
          "Default color is white (1., 1., 1.)");
static PyObject *XPLMDrawNumberFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"rgb", "x", "y", "value", "digits", "decimals", "showSign", "fontID", NULL};
  (void) self;
  PyObject *rgbList = Py_None;
  int inXOffset = 0;
  int inYOffset = 0;
  double inValue = 0.0;
  int inDigits = -1;
  int inDecimals = 0;
  int inShowSign = 1;
  int inFontID = xplmFont_Proportional;

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|Oiidiiii", keywords, &rgbList, &inXOffset, &inYOffset, &inValue,
                       &inDigits, &inDecimals, &inShowSign, &inFontID)){
    return NULL;
  }
  float inColorRGB[3];
  if (rgbList == Py_None) {
    inColorRGB[0] = 1.0;
    inColorRGB[1] = 1.0;
    inColorRGB[2] = 1.0;
  } else if(PySequence_Size(rgbList) != 3){
    PyErr_SetString(PyExc_TypeError , "inColourRGB must have 3 items");
    return NULL;
  } else {
    inColorRGB[0] = PyFloat_AsDouble(PySequence_GetItem(rgbList, 0));
    inColorRGB[1] = PyFloat_AsDouble(PySequence_GetItem(rgbList, 1));
    inColorRGB[2] = PyFloat_AsDouble(PySequence_GetItem(rgbList, 2));
  }
  if (inDigits < 0) {
    if (inValue == 0.0) {
      inDigits = 1;
    } else {
      inDigits = (int)ceil(log10(inValue > 0.0 ? inValue : -inValue));
    }
  }

  XPLMDrawNumber(inColorRGB, inXOffset, inYOffset, inValue, inDigits, inDecimals, inShowSign, inFontID);

  Py_RETURN_NONE;
}

My_DOCSTR(_getFontDimensions__doc__, "getFontDimensions", "fontID",
          "Get information about font\n"
          "\n"
          "Returns (width, height, digitsOnly). Proportional fonts\n"
          "return hopefully average width.");
static PyObject *XPLMGetFontDimensionsFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"fontID", "width", "height", "digitsOnly", NULL};
  (void) self;
  int inFontID;
  PyObject *outCharWidth, *outCharHeight, *outDigitsOnly;
  int returnValues = 0;
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "iOOO", keywords, &inFontID, &outCharWidth, &outCharHeight, &outDigitsOnly)) {
    PyErr_Clear();
    returnValues = 1;
    static char *nkeywords[] = {"fontID", NULL};
    if(!PyArg_ParseTupleAndKeywords(args, kwargs, "i", nkeywords, &inFontID)) {
      return NULL;
    }
  }

  int charWidth, charHeight, digitsOnly;
  XPLMGetFontDimensions(inFontID, &charWidth, &charHeight, &digitsOnly);
  if (returnValues) {
    return Py_BuildValue("(iii)", charWidth, charHeight, digitsOnly);
  }
  pythonLogWarning("XPLMGetFontDimentions only requires initial fontID parameter");
  if (outCharWidth != Py_None)
    PyList_Append(outCharWidth, PyLong_FromLong(charWidth));
  if (outCharHeight != Py_None)
    PyList_Append(outCharHeight, PyLong_FromLong(charHeight));
  if (outDigitsOnly != Py_None)
    PyList_Append(outDigitsOnly, PyLong_FromLong(digitsOnly));
  Py_RETURN_NONE;
}

My_DOCSTR(_measureString__doc__, "measureString", "fontID, string",
          "Returns floating point width of string, with indicated font.");
static PyObject *XPLMMeasureStringFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {"fontID", "string", "numChars", NULL};
  (void) self;
  int inFontID;
  char *inChar;
  int inNumChars;

  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "isi", keywords, &inFontID, &inChar, &inNumChars)) {
    PyErr_Clear();
    static char *nkeywords[] = {"fontID", "string", NULL};
    if(!PyArg_ParseTupleAndKeywords(args, kwargs, "is", nkeywords, &inFontID, &inChar)) {
      return NULL;
    }
    inNumChars = strlen(inChar);
  } else {
    pythonLogWarning("'numChar' unnecessary as final parameter of XPLMMeasureString");
  }
  return PyFloat_FromDouble(XPLMMeasureString(inFontID, inChar, inNumChars));
}


static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  Py_RETURN_NONE;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
static PyMethodDef XPLMGraphicsMethods[] = {
  {"setGraphicsState", (PyCFunction)XPLMSetGraphicsStateFun, METH_VARARGS | METH_KEYWORDS, _setGraphicsState__doc__},
  {"XPLMSetGraphicsState", (PyCFunction)XPLMSetGraphicsStateFun, METH_VARARGS | METH_KEYWORDS, "Sets state of the graphics pipeline."},
  {"bindTexture2d", (PyCFunction)XPLMBindTexture2dFun, METH_VARARGS | METH_KEYWORDS, _bindTexture2d__doc__},
  {"XPLMBindTexture2d", (PyCFunction)XPLMBindTexture2dFun, METH_VARARGS | METH_KEYWORDS, "Bind a 2D texture."},
  {"generateTextureNumbers", (PyCFunction)XPLMGenerateTextureNumbersFun, METH_VARARGS | METH_KEYWORDS, _generateTextureNumbers__doc__},
  {"XPLMGenerateTextureNumbers", (PyCFunction)XPLMGenerateTextureNumbersFun, METH_VARARGS | METH_KEYWORDS, "Generates number of texture IDs."},
#if defined(XPLM_DEPRECATED)
  {"getTexture", (PyCFunction)XPLMGetTextureFun, METH_VARARGS | METH_KEYWORDS, _getTexture__doc__},
  {"XPLMGetTexture", (PyCFunction)XPLMGetTextureFun, METH_VARARGS | METH_KEYWORDS, "DEPRECATED"},
#endif
  {"worldToLocal", (PyCFunction)XPLMWorldToLocalFun, METH_VARARGS | METH_KEYWORDS, _worldToLocal__doc__},
  {"XPLMWorldToLocal", (PyCFunction)XPLMWorldToLocalFun, METH_VARARGS | METH_KEYWORDS, "Transform world coordinates to local."},
  {"localToWorld", (PyCFunction)XPLMLocalToWorldFun, METH_VARARGS | METH_KEYWORDS, _localToWorld__doc__},
  {"XPLMLocalToWorld", (PyCFunction)XPLMLocalToWorldFun, METH_VARARGS | METH_KEYWORDS, "Transform local coordinates to world."},
  {"drawTranslucentDarkBox", (PyCFunction)XPLMDrawTranslucentDarkBoxFun, METH_VARARGS | METH_KEYWORDS, _drawTranslucentDarkBox__doc__},
  {"XPLMDrawTranslucentDarkBox", (PyCFunction)XPLMDrawTranslucentDarkBoxFun, METH_VARARGS | METH_KEYWORDS, "Draw translucent window."},
  {"drawString", (PyCFunction)XPLMDrawStringFun, METH_VARARGS | METH_KEYWORDS, _drawString__doc__},
  {"XPLMDrawString", (PyCFunction)XPLMDrawStringFun, METH_VARARGS | METH_KEYWORDS, "Draw string."},
  {"drawNumber", (PyCFunction)XPLMDrawNumberFun, METH_VARARGS | METH_KEYWORDS, _drawNumber__doc__},
  {"XPLMDrawNumber", (PyCFunction)XPLMDrawNumberFun, METH_VARARGS | METH_KEYWORDS, "Draw number."},
  {"getFontDimensions", (PyCFunction)XPLMGetFontDimensionsFun, METH_VARARGS | METH_KEYWORDS, _getFontDimensions__doc__},
  {"XPLMGetFontDimensions", (PyCFunction)XPLMGetFontDimensionsFun, METH_VARARGS | METH_KEYWORDS, "Get fond dimmensions."},
  {"measureString", (PyCFunction)XPLMMeasureStringFun, METH_VARARGS | METH_KEYWORDS, _measureString__doc__},
  {"XPLMMeasureString", (PyCFunction)XPLMMeasureStringFun, METH_VARARGS | METH_KEYWORDS, "Measure a string."},
  {"_cleanup", cleanup, METH_VARARGS, ""},
  {NULL, NULL, 0, NULL}
};
#pragma GCC diagnostic pop


static struct PyModuleDef XPLMGraphicsModule = {
  PyModuleDef_HEAD_INIT,
  "XPLMGraphics",
  "Laminar documentation: \n"
  "   https://developer.x-plane.com/sdk/XPLMGraphics/\n"
  "XPPython3 documentation: \n"
  "   https://xppython3.rtfd.io/en/stable/development/modules/graphics.html",
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
    PyModule_AddStringConstant(mod, "__author__", "Peter Buckner (pbuck@avnwx.com)");
    PyModule_AddIntConstant(mod, "xplm_Tex_GeneralInterface", xplm_Tex_GeneralInterface);
    PyModule_AddIntConstant(mod, "xplmFont_Basic", xplmFont_Basic);
    PyModule_AddIntConstant(mod, "xplmFont_Proportional", xplmFont_Proportional);
    PyModule_AddIntConstant(mod, "Tex_GeneralInterface", xplm_Tex_GeneralInterface);
    PyModule_AddIntConstant(mod, "Font_Basic", xplmFont_Basic);
    PyModule_AddIntConstant(mod, "Font_Proportional", xplmFont_Proportional);
  }

  return mod;
}


