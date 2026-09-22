#define _GNU_SOURCE 1
#define PY_SSIZE_T_CLEAN   /* required for 'y#' (bytes+length) formats below */
#include <Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdint>
#include <cstring>
#include <limits>
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMPanelGraphics.h>
#include "utils.h"
#include "panel_graphics.h"
#include "plugin_dl.h"
#include "capsules.h"

static XPLMTextureVertex_t *parseTextureVertices(PyObject *verticesObj, Py_ssize_t *outCount)
{
  *outCount = 0;
  PyObject *seq = PySequence_Fast(verticesObj, "vertices must be a sequence of (x, y, s, t) tuples");
  if(!seq){
    return nullptr;
  }
  Py_ssize_t count = PySequence_Fast_GET_SIZE(seq);
  XPLMTextureVertex_t *vertices = (XPLMTextureVertex_t *)malloc((count ? count : 1) * sizeof(XPLMTextureVertex_t));
  if(!vertices){
    Py_DECREF(seq);
    PyErr_NoMemory();
    return nullptr;
  }
  for(Py_ssize_t i = 0; i < count; i++){
    PyObject *itemSeq = PySequence_Fast(PySequence_Fast_GET_ITEM(seq, i), "each vertex must be an (x, y, s, t) tuple");
    if(!itemSeq || PySequence_Fast_GET_SIZE(itemSeq) < 4){
      Py_XDECREF(itemSeq);
      free(vertices);
      Py_DECREF(seq);
      PyErr_SetString(PyExc_ValueError, "each vertex must be an (x, y, s, t) tuple");
      return nullptr;
    }
    vertices[i].x = (float)PyFloat_AsDouble(PySequence_Fast_GET_ITEM(itemSeq, 0));
    vertices[i].y = (float)PyFloat_AsDouble(PySequence_Fast_GET_ITEM(itemSeq, 1));
    vertices[i].s = (float)PyFloat_AsDouble(PySequence_Fast_GET_ITEM(itemSeq, 2));
    vertices[i].t = (float)PyFloat_AsDouble(PySequence_Fast_GET_ITEM(itemSeq, 3));
    Py_DECREF(itemSeq);
  }
  Py_DECREF(seq);
  if(PyErr_Occurred()){
    free(vertices);
    return nullptr;
  }
  *outCount = count;
  return vertices;
}


/* ---- Mesh buffers for drawCalls() ----

   vertices and indices may each arrive either already packed as a bytes-like
   object (what ImGui hands you, and what the SDK wants) or as a plain Python
   sequence we pack here. MeshBuffer holds whichever form we ended up with:
   a borrowed buffer view over the caller's bytes, or an array we malloc'd.
   Its destructor releases the right one on every exit path.                    */

struct MeshBuffer {
  Py_buffer view;
  bool haveView;
  void *owned;
  Py_ssize_t count;        /* vertices, or indices -- not bytes */
  const void *data;

  MeshBuffer(): view(), haveView(false), owned(nullptr), count(0), data(nullptr) {}
  ~MeshBuffer(){
    if(haveView){
      PyBuffer_Release(&view);
    }
    free(owned);
  }
  MeshBuffer(const MeshBuffer &) = delete;
  MeshBuffer &operator=(const MeshBuffer &) = delete;
};

/* True if obj exposes a simple C-contiguous read buffer (bytes, bytearray,
   memoryview, array.array...). Clears the TypeError otherwise: not being a
   buffer is not an error here, it just means "try the sequence path".          */
static bool takeSimpleBuffer(PyObject *obj, MeshBuffer *out)
{
  if(PyObject_GetBuffer(obj, &out->view, PyBUF_SIMPLE) != 0){
    PyErr_Clear();
    return false;
  }
  out->haveView = true;
  return true;
}

/* vertices as a sequence of (x, y, u, v, color) -- color being the packed
   uint32 from makeColor(), which occupies the 5th float slot as raw bits.
   color alone is optional: (x, y, u, v) takes opaque white.                    */
static bool packMeshVertices(PyObject *verticesObj, MeshBuffer *out)
{
  if(takeSimpleBuffer(verticesObj, out)){
    /* we have a buffer, not python sequence */
    if(out->view.len % 20 != 0){
      PyErr_SetString(PyExc_ValueError, "drawCalls: vertices length must be a multiple of 20 (5 floats/vertex).");
      return false;
    }
    out->data = out->view.buf;
    out->count = out->view.len / 20;
    return true;
  }

  /* we have a sequence, not a buffer */
  PyObject *seq = PySequence_Fast(verticesObj, "drawCalls: vertices must be bytes or a sequence of "
                                               "(x, y, u, v, color) tuples");
  if(!seq){
    return false;
  }
  Py_ssize_t count = PySequence_Fast_GET_SIZE(seq);
  float *packed = (float *)malloc((count ? count : 1) * 5 * sizeof(float));
  if(!packed){
    Py_DECREF(seq);
    PyErr_NoMemory();
    return false;
  }
  out->owned = packed;
  for(Py_ssize_t i = 0; i < count; i++){
    PyObject *itemSeq = PySequence_Fast(PySequence_Fast_GET_ITEM(seq, i),
                                        "drawCalls: each vertex must be an (x, y, u, v[, color]) tuple");
    Py_ssize_t fields = itemSeq ? PySequence_Fast_GET_SIZE(itemSeq) : 0;
    if(!itemSeq || (fields != 5 && fields != 4)){
      Py_XDECREF(itemSeq);
      Py_DECREF(seq);
      PyErr_SetString(PyExc_ValueError, "drawCalls: each vertex must be (x, y, u, v, color) "
                      "or (x, y, u, v), which takes opaque white.");
      return false;
    }
    packed[i * 5 + 0] = (float)PyFloat_AsDouble(PySequence_Fast_GET_ITEM(itemSeq, 0));
    packed[i * 5 + 1] = (float)PyFloat_AsDouble(PySequence_Fast_GET_ITEM(itemSeq, 1));
    packed[i * 5 + 2] = (float)PyFloat_AsDouble(PySequence_Fast_GET_ITEM(itemSeq, 2));
    packed[i * 5 + 3] = (float)PyFloat_AsDouble(PySequence_Fast_GET_ITEM(itemSeq, 3));
    /* The color slot is 32 raw bits, not a number to convert, memcpy it into float packed[] */
    uint32_t color = 0xFFFFFFFF;   /* opaque white: no tint */
    if(fields == 5){
      color = (uint32_t)PyLong_AsUnsignedLongMask(PySequence_Fast_GET_ITEM(itemSeq, 4));
    }
    memcpy(&packed[i * 5 + 4], &color, sizeof(uint32_t));
    Py_DECREF(itemSeq);
    if(PyErr_Occurred()){
      Py_DECREF(seq);
      return false;
    }
  }
  Py_DECREF(seq);
  out->data = packed;
  out->count = count;
  return true;
}

/* indices as a sequence of ints, packed to the uint16 the SDK requires.        */
static bool packMeshIndices(PyObject *indicesObj, MeshBuffer *out, int indexSize)
{
  if(takeSimpleBuffer(indicesObj, out)){
    /* we have a buffer, not a python sequence.
       If indexSize is 4, we need to reduce it to 2-byte size */
    if(out->view.len % indexSize != 0){
      PyErr_Format(PyExc_ValueError, "drawCalls: indices length must be a multiple of %d (indexSize).",
                   indexSize);
      return false;
    }
    Py_ssize_t count = out->view.len / indexSize;
    if(indexSize == 2){
      /* Already the layout XPLMDrawCalls wants -- use it in place, no copy. */
      out->data = out->view.buf;
      out->count = count;
      return true;
    }
    /* indexSize == 4: Dear ImGui built with 32-bit ImDrawIdx. SDK requires 16-bit*/
    uint16_t *packed = (uint16_t *)malloc((count ? count : 1) * sizeof(uint16_t));
    if(!packed){
      PyErr_NoMemory();
      return false;
    }
    out->owned = packed;
    const uint32_t *wide = (const uint32_t *)out->view.buf;
    for(Py_ssize_t i = 0; i < count; i++){
      if(wide[i] > 65535){
        PyErr_Format(PyExc_ValueError,
                     "drawCalls: index %u at position %zd exceeds uint16 (0..65535). Split the "
                     "mesh across several draw calls using vtx_offset.", wide[i], i);
        return false;
      }
      packed[i] = (uint16_t)wide[i];
    }
    out->data = packed;
    out->count = count;
    return true;
  }

  /* we have a sequence, not a buffer. Pack into 16-bit unsigned integers */
  PyObject *seq = PySequence_Fast(indicesObj, "drawCalls: indices must be bytes or a sequence of ints");
  if(!seq){
    return false;
  }
  Py_ssize_t count = PySequence_Fast_GET_SIZE(seq);
  uint16_t *packed = (uint16_t *)malloc((count ? count : 1) * sizeof(uint16_t));
  if(!packed){
    Py_DECREF(seq);
    PyErr_NoMemory();
    return false;
  }
  out->owned = packed;
  for(Py_ssize_t i = 0; i < count; i++){
    long index = PyLong_AsLong(PySequence_Fast_GET_ITEM(seq, i));
    if(index == -1 && PyErr_Occurred()){
      Py_DECREF(seq);
      return false;
    }
    if(index < 0 || index > 65535){
      Py_DECREF(seq);
      PyErr_Format(PyExc_ValueError, "drawCalls: index %ld at position %zd is out of range for uint16 (0..65535).",
                   index, i);
      return false;
    }
    packed[i] = (uint16_t)index;
  }
  Py_DECREF(seq);
  out->data = packed;
  out->count = count;
  return true;
}


/* ---- Texture atlas ---- */

My_DOCSTR(_createTextureAtlas__doc__, "createTextureAtlas",
          "",
          "",
          "XPLMTextureAtlasRef",
          "Create a new, empty texture atlas. Add images with addImageFile() /\n"
          "addImage() and their *Set() variants, then call textureAtlasBake()\n"
          "before drawing. Destroy with destroyTextureAtlas() when done.");
static PyObject *XPLMCreateTextureAtlasFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {nullptr};
  (void) self;
  if(!XPLMCreateTextureAtlas_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMCreateTextureAtlas is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "", keywords)){
    return nullptr;
  }
  XPLMTextureAtlasRef atlas = XPLMCreateTextureAtlas_ptr();
  if(!atlas){
    PyErr_SetString(PyExc_RuntimeError , "XPLMCreateTextureAtlas failed.");
    return nullptr;
  }
  return makeCapsule(atlas, ATLAS_CAPSULE);
}

My_DOCSTR(_destroyTextureAtlas__doc__, "destroyTextureAtlas",
          "atlas",
          "atlas:XPLMTextureAtlasRef",
          "None",
          "Destroy a texture atlas created with createTextureAtlas() and free all\n"
          "associated GPU and CPU resources.");
static PyObject *XPLMDestroyTextureAtlasFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("atlas"), nullptr};
  (void) self;
  PyObject *atlasCapsule;
  if(!XPLMDestroyTextureAtlas_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMDestroyTextureAtlas is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &atlasCapsule)){
    return nullptr;
  }
  XPLMTextureAtlasRef atlas = getVoidPtr(atlasCapsule, ATLAS_CAPSULE);
  XPLMDestroyTextureAtlas_ptr(atlas);
  deleteCapsule(atlasCapsule);
  Py_RETURN_NONE;
}

My_DOCSTR(_textureAtlasAddImageFile__doc__, "textureAtlasAddImageFile",
          "atlas, imageFilePath",
          "atlas:XPLMTextureAtlasRef, imageFilePath:str",
          "int",
          "Load a PNG file and add it to the atlas as a single image. Call before\n"
          "textureAtlasBake(). Returns the zero-based image index assigned.");
static PyObject *XPLMTextureAtlasAddImageFileFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("atlas"), CHAR("imageFilePath"), nullptr};
  (void) self;
  PyObject *atlasCapsule;
  const char *imageFilePath;
  if(!XPLMTextureAtlasAddImageFile_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMTextureAtlasAddImageFile is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Os", keywords, &atlasCapsule, &imageFilePath)){
    return nullptr;
  }
  XPLMTextureAtlasRef atlas = getVoidPtr(atlasCapsule, ATLAS_CAPSULE);
  return PyLong_FromLong(XPLMTextureAtlasAddImageFile_ptr(atlas, imageFilePath));
}

My_DOCSTR(_textureAtlasAddImageFileSet__doc__, "textureAtlasAddImageFileSet",
          "atlas, imageFilePath, cellsX, cellsY",
          "atlas:XPLMTextureAtlasRef, imageFilePath:str, cellsX:int, cellsY:int",
          "int",
          "Load a PNG file and subdivide it into a cellsX x cellsY grid, adding each\n"
          "cell as a separate image (useful for sprite sheets). Returns the index of\n"
          "the first (top-left) cell; cell (x, y) is index + y * cellsX + x.");
static PyObject *XPLMTextureAtlasAddImageFileSetFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("atlas"), CHAR("imageFilePath"), CHAR("cellsX"), CHAR("cellsY"), nullptr};
  (void) self;
  PyObject *atlasCapsule;
  const char *imageFilePath;
  int cellsX, cellsY;
  if(!XPLMTextureAtlasAddImageFileSet_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMTextureAtlasAddImageFileSet is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Osii", keywords, &atlasCapsule, &imageFilePath, &cellsX, &cellsY)){
    return nullptr;
  }
  XPLMTextureAtlasRef atlas = getVoidPtr(atlasCapsule, ATLAS_CAPSULE);
  return PyLong_FromLong(XPLMTextureAtlasAddImageFileSet_ptr(atlas, imageFilePath, cellsX, cellsY));
}

My_DOCSTR(_textureAtlasAddImage__doc__, "textureAtlasAddImage",
          "atlas, image, width, height",
          "atlas:XPLMTextureAtlasRef, image:bytes, width:int, height:int",
          "int",
          "Add a single image from raw RGBA pixel data (4 bytes/pixel, rows top to\n"
          "bottom) to the atlas. image must hold at least width * height * 4 bytes.\n"
          "Returns the zero-based image index assigned.");
static PyObject *XPLMTextureAtlasAddImageFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("atlas"), CHAR("image"), CHAR("width"), CHAR("height"), nullptr};
  (void) self;
  PyObject *atlasCapsule;
  const unsigned char *image;
  Py_ssize_t imageLen;
  int width, height;
  if(!XPLMTextureAtlasAddImage_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMTextureAtlasAddImage is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Oy#ii", keywords, &atlasCapsule, &image, &imageLen, &width, &height)){
    return nullptr;
  }
  if(width < 0 || height < 0 || imageLen < (Py_ssize_t)width * height * 4){
    PyErr_SetString(PyExc_ValueError, "image must contain at least width * height * 4 bytes of RGBA data.");
    return nullptr;
  }
  XPLMTextureAtlasRef atlas = getVoidPtr(atlasCapsule, ATLAS_CAPSULE);
  return PyLong_FromLong(XPLMTextureAtlasAddImage_ptr(atlas, image, width, height));
}

My_DOCSTR(_textureAtlasAddImageSet__doc__, "textureAtlasAddImageSet",
          "atlas, image, width, height, cellsX, cellsY",
          "atlas:XPLMTextureAtlasRef, image:bytes, width:int, height:int, cellsX:int, cellsY:int",
          "int",
          "Add raw RGBA pixel data (4 bytes/pixel, rows top to bottom) to the atlas,\n"
          "subdividing it into a cellsX x cellsY grid; each cell becomes a separate\n"
          "image. image must hold at least width * height * 4 bytes. Returns the index\n"
          "of the first cell; cell (x, y) is index + y * cellsX + x.");
static PyObject *XPLMTextureAtlasAddImageSetFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("atlas"), CHAR("image"), CHAR("width"), CHAR("height"), CHAR("cellsX"), CHAR("cellsY"), nullptr};
  (void) self;
  PyObject *atlasCapsule;
  const unsigned char *image;
  Py_ssize_t imageLen;
  int width, height, cellsX, cellsY;
  if(!XPLMTextureAtlasAddImageSet_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMTextureAtlasAddImageSet is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "Oy#iiii", keywords, &atlasCapsule, &image, &imageLen, &width, &height, &cellsX, &cellsY)){
    return nullptr;
  }
  if(width < 0 || height < 0 || imageLen < (Py_ssize_t)width * height * 4){
    PyErr_SetString(PyExc_ValueError, "image must contain at least width * height * 4 bytes of RGBA data.");
    return nullptr;
  }
  XPLMTextureAtlasRef atlas = getVoidPtr(atlasCapsule, ATLAS_CAPSULE);
  return PyLong_FromLong(XPLMTextureAtlasAddImageSet_ptr(atlas, image, width, height, cellsX, cellsY));
}

My_DOCSTR(_textureAtlasBake__doc__, "textureAtlasBake",
          "atlas",
          "atlas:XPLMTextureAtlasRef",
          "None",
          "Pack all previously added images into a GPU texture. Must be called after\n"
          "adding all images and before any draw calls. Once baked, no more images\n"
          "may be added.");
static PyObject *XPLMTextureAtlasBakeFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("atlas"), nullptr};
  (void) self;
  PyObject *atlasCapsule;
  if(!XPLMTextureAtlasBake_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMTextureAtlasBake is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &atlasCapsule)){
    return nullptr;
  }
  XPLMTextureAtlasRef atlas = getVoidPtr(atlasCapsule, ATLAS_CAPSULE);
  XPLMTextureAtlasBake_ptr(atlas);
  Py_RETURN_NONE;
}

My_DOCSTR(_textureAtlasGetImageWidth__doc__, "textureAtlasGetImageWidth",
          "atlas, imageIndex=0",
          "atlas:XPLMTextureAtlasRef, imageIndex:int",
          "int",
          "Return the width in pixels of a single image (or cell) in the atlas.");
static PyObject *XPLMTextureAtlasGetImageWidthFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("atlas"), CHAR("imageIndex"), nullptr};
  (void) self;
  PyObject *atlasCapsule;
  int imageIndex = 0;
  if(!XPLMTextureAtlasGetImageWidth_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMTextureAtlasGetImageWidth is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|i", keywords, &atlasCapsule, &imageIndex)){
    return nullptr;
  }
  XPLMTextureAtlasRef atlas = getVoidPtr(atlasCapsule, ATLAS_CAPSULE);
  return PyLong_FromLong(XPLMTextureAtlasGetImageWidth_ptr(atlas, imageIndex));
}

My_DOCSTR(_textureAtlasGetImageHeight__doc__, "textureAtlasGetImageHeight",
          "atlas, imageIndex=0",
          "atlas:XPLMTextureAtlasRef, imageIndex:int",
          "int",
          "Return the height in pixels of a single image (or cell) in the atlas.");
static PyObject *XPLMTextureAtlasGetImageHeightFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("atlas"), CHAR("imageIndex"), nullptr};
  (void) self;
  PyObject *atlasCapsule;
  int imageIndex=0;
  if(!XPLMTextureAtlasGetImageHeight_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMTextureAtlasGetImageHeight is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|i", keywords, &atlasCapsule, &imageIndex)){
    return nullptr;
  }
  XPLMTextureAtlasRef atlas = getVoidPtr(atlasCapsule, ATLAS_CAPSULE);
  return PyLong_FromLong(XPLMTextureAtlasGetImageHeight_ptr(atlas, imageIndex));
}

My_DOCSTR(_textureAtlasDrawAt__doc__, "textureAtlasDrawAt",
          "atlas, imageIndex=0, tintColor=White, x=NaN, y=NaN",
          "atlas:XPLMTextureAtlasRef, imageIndex:int, tintColor:int, x:float, y:float",
          "None",
          "Draw an atlas image at its native resolution with its top-left corner at\n"
          "(x, y). If (x, y) are not specified, draw image lower left corner at (0, 0).\n"
          "tintColor is multiplied with the texture; use makeColor(1, 1, 1, 1)\n"
          "for no tinting.");
static PyObject *XPLMTextureAtlasDrawAtFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("atlas"), CHAR("imageIndex"), CHAR("tintColor"), CHAR("x"), CHAR("y"), nullptr};
  (void) self;
  PyObject *atlasCapsule;
  int imageIndex=0;
  unsigned long tintColor = 0xffffffff;
  float x=NAN, y=NAN;
  if(!XPLMTextureAtlasDrawAt_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMTextureAtlasDrawAt is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|ikff", keywords, &atlasCapsule, &imageIndex, &tintColor, &x, &y)){
    return nullptr;
  }
  XPLMTextureAtlasRef atlas = getVoidPtr(atlasCapsule, ATLAS_CAPSULE);
  if (std::isnan(x)) {x = 0;}
  if (std::isnan(y)) {y = (float)XPLMTextureAtlasGetImageHeight_ptr(atlas, imageIndex);}
  XPLMTextureAtlasDrawAt_ptr(atlas, imageIndex, (uint32_t)tintColor, x, y);
  Py_RETURN_NONE;
}

My_DOCSTR(_textureAtlasDrawIn__doc__, "textureAtlasDrawIn",
          "atlas, imageIndex=0, tintColor=White, left=0, top=NaN, right=NaN, bottom=0",
          "atlas:XPLMTextureAtlasRef, imageIndex:int, tintColor:int, left:float, top:float, right:float, bottom:float",
          "None",
          "Draw an atlas image scaled to fill the rectangle (left, top, right,\n"
          "bottom), If top and right are not specified, use the image's native height and width\n"
          "Image is stretched or compressed to match. tintColor is multiplied\n"
          "with the texture.");
static PyObject *XPLMTextureAtlasDrawInFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("atlas"), CHAR("imageIndex"), CHAR("tintColor"), CHAR("left"), CHAR("top"), CHAR("right"), CHAR("bottom"), nullptr};
  (void) self;
  PyObject *atlasCapsule;
  int imageIndex = 0;
  unsigned long tintColor = 0xffffffff;
  float left=0, top=NAN, right=NAN, bottom=0;
  if(!XPLMTextureAtlasDrawIn_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMTextureAtlasDrawIn is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|ikffff", keywords, &atlasCapsule, &imageIndex, &tintColor, &left, &top, &right, &bottom)){
    return nullptr;
  }
  XPLMTextureAtlasRef atlas = getVoidPtr(atlasCapsule, ATLAS_CAPSULE);
  if (std::isnan(top)) {top = XPLMTextureAtlasGetImageHeight_ptr(atlas, imageIndex);}
  if (std::isnan(right)) {right = XPLMTextureAtlasGetImageWidth_ptr(atlas, imageIndex);}
  XPLMTextureAtlasDrawIn_ptr(atlas, imageIndex, (uint32_t)tintColor, left, top, right, bottom);
  Py_RETURN_NONE;
}

My_DOCSTR(_textureAtlasDrawStretched__doc__, "textureAtlasDrawStretched",
          "atlas, imageIndex=0, tintColor=White, left=0, top=NaN, right=NaN, bottom=0",
          "atlas:XPLMTextureAtlasRef, imageIndex:int, tintColor:int, left:float, top:float, right:float, bottom:float",
          "None",
          "Draw an atlas image into the rectangle (left, top, right, bottom) using\n"
          "9-slice scaling: the four corners keep their native size, edges stretch\n"
          "along one axis, and the center stretches in both. Preserves borders when\n"
          "scaling UI elements. If top and right are not specified, native image size\n"
          "is used. tintColor is multiplied with the texture.");
static PyObject *XPLMTextureAtlasDrawStretchedFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("atlas"), CHAR("imageIndex"), CHAR("tintColor"), CHAR("left"), CHAR("top"), CHAR("right"), CHAR("bottom"), nullptr};
  (void) self;
  PyObject *atlasCapsule;
  int imageIndex=0;
  unsigned long tintColor=0xffffffff;
  float left=0, top=NAN, right=NAN, bottom=0;
  if(!XPLMTextureAtlasDrawStretched_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMTextureAtlasDrawStretched is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|ikffff", keywords, &atlasCapsule, &imageIndex, &tintColor, &left, &top, &right, &bottom)){
    return nullptr;
  }
  XPLMTextureAtlasRef atlas = getVoidPtr(atlasCapsule, ATLAS_CAPSULE);
  if (std::isnan(top)) {top = XPLMTextureAtlasGetImageHeight_ptr(atlas, imageIndex);}
  if (std::isnan(right)) {right = XPLMTextureAtlasGetImageWidth_ptr(atlas, imageIndex);}
  XPLMTextureAtlasDrawStretched_ptr(atlas, imageIndex, (uint32_t)tintColor, left, top, right, bottom);
  Py_RETURN_NONE;
}

My_DOCSTR(_textureAtlasDrawScaled__doc__, "textureAtlasDrawScaled",
          "atlas, imageIndex=0, tintColor=White, xPanel=NaN, yPanel=NaN, xAtlas=NaN, yAtlas=NaN, xScale=1, yScale=1, rotateCW=0",
          "atlas:XPLMTextureAtlasRef, imageIndex:int, tintColor:int, xPanel:float, yPanel:float, xAtlas:float, yAtlas:float, xScale:float, yScale:float, rotateCW:float",
          "None",
          "Draw an atlas image with arbitrary scaling, rotation, and positioning. The\n"
          "atlas-space pivot (xAtlas, yAtlas), in pixels from the image's bottom-left,\n"
          "(image center, if not specified),\n"
          "is aligned to the panel-space point (xPanel, yPanel), \n"
          "then scaled by (xScale, yScale) and rotated rotateCW degrees clockwise \n"
          "about that pivot.\n"
          "tintColor is multiplied with the texture.");
static PyObject *XPLMTextureAtlasDrawScaledFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("atlas"), CHAR("imageIndex"), CHAR("tintColor"), CHAR("xPanel"), CHAR("yPanel"),
                             CHAR("xAtlas"), CHAR("yAtlas"), CHAR("xScale"), CHAR("yScale"), CHAR("rotateCW"), nullptr};
  (void) self;
  PyObject *atlasCapsule;
  int imageIndex = 0;
  unsigned long tintColor = 0xffffffff;
  float xPanel=NAN, yPanel=NAN, xAtlas=NAN, yAtlas=NAN, xScale=1, yScale=1, rotateCW=0;
  if(!XPLMTextureAtlasDrawScaled_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMTextureAtlasDrawScaled is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|ikfffffff", keywords, &atlasCapsule, &imageIndex, &tintColor,
                                  &xPanel, &yPanel, &xAtlas, &yAtlas, &xScale, &yScale, &rotateCW)){
    return nullptr;
  }
  XPLMTextureAtlasRef atlas = getVoidPtr(atlasCapsule, ATLAS_CAPSULE);
  if (std::isnan(xPanel)) xPanel = XPLMTextureAtlasGetImageWidth_ptr(atlas, imageIndex) / 2.0;
  if (std::isnan(yPanel)) yPanel = XPLMTextureAtlasGetImageHeight_ptr(atlas, imageIndex) / 2.0;
  if (std::isnan(xAtlas)) xAtlas = XPLMTextureAtlasGetImageWidth_ptr(atlas, imageIndex) / 2.0;
  if (std::isnan(yAtlas)) yAtlas = XPLMTextureAtlasGetImageHeight_ptr(atlas, imageIndex) / 2.0;
  XPLMTextureAtlasDrawScaled_ptr(atlas, imageIndex, (uint32_t)tintColor, xPanel, yPanel, xAtlas, yAtlas, xScale, yScale, rotateCW);
  Py_RETURN_NONE;
}

My_DOCSTR(_textureAtlasDrawMesh__doc__, "textureAtlasDrawMesh",
          "atlas, imageIndex=0, tintColor=White, vertices=None",
          "atlas:XPLMTextureAtlasRef, imageIndex:int, tintColor:int, vertices:Optional[Sequence[tuple[float, float, float, float]]]",
          "None",
          "Draw an atlas image onto an arbitrary triangle-strip mesh. Each vertex is\n"
          "an (x, y, s, t) tuple: (x, y) panel-space position in pixels, (s, t)\n"
          "normalized texture coordinate (0.0-1.0) within the image. At least 3\n"
          "vertices are required. tintColor is multiplied with the texture.\n"
          "\n"
          "Omit vertices (or pass None) to draw the whole image at its native size\n"
          "with its lower-left corner at the panel origin -- useful as a starting\n"
          "point, and typically combined with a transform to place it. The atlas\n"
          "must already be baked, as the default is measured from the image.");
static PyObject *XPLMTextureAtlasDrawMeshFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("atlas"), CHAR("imageIndex"), CHAR("tintColor"), CHAR("vertices"), nullptr};
  (void) self;
  PyObject *atlasCapsule;
  int imageIndex = 0;
  unsigned long tintColor = 0xffffffff;
  PyObject *verticesObj = Py_None;
  if(!XPLMTextureAtlasDrawMesh_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMTextureAtlasDrawMesh is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O|ikO", keywords, &atlasCapsule, &imageIndex, &tintColor, &verticesObj)){
    return nullptr;
  }
  XPLMTextureAtlasRef atlas = getVoidPtr(atlasCapsule, ATLAS_CAPSULE);

  Py_ssize_t count = 0;
  XPLMTextureVertex_t defaultVerts[4];
  XPLMTextureVertex_t *v;

  if(verticesObj == Py_None){
    /* Default mesh: the whole image at native size, lower-left at the panel
       origin, as a four-vertex triangle strip. Note the size queries require a
       baked atlas -- querying an unbaked one currently takes the sim down. */
    if(!XPLMTextureAtlasGetImageWidth_ptr || !XPLMTextureAtlasGetImageHeight_ptr){
      PyErr_SetString(PyExc_RuntimeError, "XPLMTextureAtlasGetImageWidth/Height are available only in XPLM440 and up.");
      return nullptr;
    }
    float width = (float)XPLMTextureAtlasGetImageWidth_ptr(atlas, imageIndex);
    float height = (float)XPLMTextureAtlasGetImageHeight_ptr(atlas, imageIndex);
    defaultVerts[0] = {0.0f,  0.0f,   0.0f, 0.0f};
    defaultVerts[1] = {0.0f,  height, 0.0f, 1.0f};
    defaultVerts[2] = {width, 0.0f,   1.0f, 0.0f};
    defaultVerts[3] = {width, height, 1.0f, 1.0f};
    v = defaultVerts;
    count = 4;
  } else {
    v = parseTextureVertices(verticesObj, &count);
    if(!v){
      return nullptr;
    }
  }
  XPLMTextureAtlasDrawMesh_ptr(atlas, imageIndex, (uint32_t)tintColor, v, (int)count);
  if(v != defaultVerts){
    free(v);
  }
  Py_RETURN_NONE;
}

/* ---- Texture source (stock simulator textures, e.g. weather radar) ---- */

My_DOCSTR(_textureSourceDrawIn__doc__, "textureSourceDrawIn",
          "tex=WeatherRadar1, tintColor=White, left=0, top=0, right=0, bottom=0",
          "tex:int, tintColor:int, left:int, top:int, right:int, bottom:int",
          "None",
          "Draw a stock simulator texture source (e.g. Texture_WeatherRadar1),\n"
          "scaled to fill the rectangle (left, top, right, bottom) in panel\n"
          "coordinates. Unlike a texture atlas, a texture source is a live texture\n"
          "the simulator renders each frame. tintColor is multiplied with the\n"
          "texture; use makeColor(1, 1, 1, 1) for no tinting. If the aircraft lacks\n"
          "the requested hardware, the call is silently skipped.");
static PyObject *XPLMTextureSourceDrawInFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("tex"), CHAR("tintColor"), CHAR("left"), CHAR("top"), CHAR("right"), CHAR("bottom"), nullptr};
  (void) self;
  int tex = xplm_Texture_WeatherRadar1;
  unsigned long tintColor = 0xffffffff;
  int left=0, top=0, right=0, bottom=0;
  if(!XPLMTextureSourceDrawIn_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMTextureSourceDrawIn is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|ikiiii", keywords, &tex, &tintColor, &left, &top, &right, &bottom)){
    return nullptr;
  }
  XPLMTextureSourceDrawIn_ptr((XPLMTextureSource)tex, (uint32_t)tintColor, left, top, right, bottom);
  Py_RETURN_NONE;
}

My_DOCSTR(_textureSourceDrawMesh__doc__, "textureSourceDrawMesh",
          "tex=WeatherRadar1, tintColor=White, vertices=None",
          "tex:int, tintColor:int, vertices:Sequence[tuple[float, float, float, float]]",
          "None",
          "Draw a stock simulator texture source onto an arbitrary triangle-strip\n"
          "mesh. Each vertex is an (x, y, s, t) tuple: (x, y) panel-space position\n"
          "in pixels, (s, t) normalized texture coordinate (0.0-1.0) within the\n"
          "source. At least 3 vertices are required. tintColor is multiplied with\n"
          "the texture.");
static PyObject *XPLMTextureSourceDrawMeshFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("tex"), CHAR("tintColor"), CHAR("vertices"), nullptr};
  (void) self;
  int tex = xplm_Texture_WeatherRadar1;
  unsigned long tintColor = 0xffffffff;
  PyObject *verticesObj = Py_None;
  if(!XPLMTextureSourceDrawMesh_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMTextureSourceDrawMesh is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|ikO", keywords, &tex, &tintColor, &verticesObj)){
    return nullptr;
  }
  Py_ssize_t count;
  if (verticesObj == Py_None) {
    PyErr_SetString(PyExc_ValueError, "textureSourceDrawMesh vertices not provided");
    return nullptr;
  }
  XPLMTextureVertex_t *v = parseTextureVertices(verticesObj, &count);
  if(!v){
    PyErr_SetString(PyExc_ValueError, "Unable to parse textureSourceDrawMesh vertices");
    return nullptr;
  }
  XPLMTextureSourceDrawMesh_ptr((XPLMTextureSource)tex, (uint32_t)tintColor, v, (int)count);
  free(v);
  Py_RETURN_NONE;
}

/* ---- ImGui-style textured mesh drawing ----------------------------------- */

My_DOCSTR(_createTexture__doc__, "createTexture",
          "rgba, width, height",
          "rgba:bytes, width:int, height:int",
          "XPLMTexture",
          "Upload an RGBA8 image (4 bytes/pixel, rows top to bottom; rgba must hold at\n"
          "least width * height * 4 bytes) to the GPU and return an opaque texture\n"
          "handle for use as the tex of a draw call passed to drawCalls(). Free it with\n"
          "destroyTexture(). The sampler is bilinear, clamp-to-edge, no mipmaps.");
static PyObject *XPLMCreateTextureFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("rgba"), CHAR("width"), CHAR("height"), nullptr};
  (void) self;
  const unsigned char *rgba;
  Py_ssize_t rgbaLen;
  int width, height;
  if(!XPLMCreateTexture_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMCreateTexture is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "y#ii", keywords, &rgba, &rgbaLen, &width, &height)){
    return nullptr;
  }
  if(width < 0 || height < 0 || rgbaLen < (Py_ssize_t)width * height * 4){
    PyErr_SetString(PyExc_ValueError, "createTexture: rgba must hold at least width * height * 4 bytes.");
    return nullptr;
  }
  void *tex = XPLMCreateTexture_ptr(rgba, width, height);
  if(!tex){
    PyErr_SetString(PyExc_RuntimeError , "XPLMCreateTexture failed.");
    return nullptr;
  }
  return makeCapsule(tex, TEXTURE_CAPSULE);
}

My_DOCSTR(_destroyTexture__doc__, "destroyTexture",
          "tex",
          "tex:XPLMTexture",
          "None",
          "Free a texture created with createTexture(). The handle must not be used\n"
          "after this call. It is safe to create and destroy textures every frame.");
static PyObject *XPLMDestroyTextureFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("tex"), nullptr};
  (void) self;
  PyObject *texCapsule;
  if(!XPLMDestroyTexture_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMDestroyTexture is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &texCapsule)){
    return nullptr;
  }
  void *tex = getVoidPtr(texCapsule, TEXTURE_CAPSULE);
  if(!tex && PyErr_Occurred()){
    return nullptr;
  }
  XPLMDestroyTexture_ptr(tex);
  deleteCapsule(texCapsule);
  Py_RETURN_NONE;
}

My_DOCSTR(_drawCalls__doc__, "drawCalls",
          "vertices, indices, drawCalls, indexSize=2",
          "vertices:Union[bytes, Sequence[tuple[float, ...]]], "
          "indices:Union[bytes, Sequence[int]], drawCalls:Sequence, indexSize:int=2",
          "None",
          "Render textured indexed-triangle draw calls matching Dear ImGui's ImDrawData\n"
          "layout. vertices is a bytes buffer of 20-byte vertices: pos.x, pos.y, uv.x,\n"
          "uv.y as float32, then RGBA8 packed as a little-endian uint32 (exactly\n"
          "ImDrawVert; colors are pre-multiplied alpha). indices is a bytes buffer of\n"
          "uint16 indices.\n"
          "\n"
          "For convenience, vertices may instead be a sequence of (x, y, u, v, color)\n"
          "tuples -- color being a makeColor() value -- and indices a sequence of ints;\n"
          "we pack them for you. color may be omitted -- (x, y, u, v) takes opaque\n"
          "white, that is, no tint\n"
          "Pass bytes when you already have them (an ImGui frame, or a mesh you build\n"
          "once and reuse): packing here costs a copy per frame.\n"
          "\n"
          "indexSize is the width in bytes of each index in the indices BUFFER: 2 (the\n"
          "default, what the SDK takes) or 4. Pass 4 when Dear ImGui was built with\n"
          "32-bit ImDrawIdx -- we narrow to uint16 here.\n"
          "An index above 65535 raises ValueError; split the mesh across\n"
          "several draw calls using vtx_offset. indexSize is ignored when indices is a\n"
          "sequence of ints rather than a buffer.\n"
          "\n"
          "drawCalls is a sequence of (tex, scissors, idx_offset,\n"
          "element_count, vtx_offset): tex is a createTexture() handle or None;\n"
          "scissors is (left, top, right, bottom) floats in window-local top-left\n"
          "coords; element_count must be a multiple of 3. The mesh is uploaded once and\n"
          "one GPU dispatch is issued per call. Call only from a panel-graphics window\n"
          "draw callback; the host flips Y for you.");
static PyObject *XPLMDrawCallsFun(PyObject *self, PyObject *args, PyObject *kwargs)
{
  static char *keywords[] = {CHAR("vertices"), CHAR("indices"), CHAR("drawCalls"),
                             CHAR("indexSize"), nullptr};
  (void) self;
  PyObject *verticesObj, *indicesObj, *drawCallsSeq;
  int indexSize = 2;
  MeshBuffer vertices, indices;
  if(!XPLMDrawCalls_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMDrawCalls is available only in XPLM440 and up.");
    return nullptr;
  }
  if(!PyArg_ParseTupleAndKeywords(args, kwargs, "OOO|i", keywords, &verticesObj, &indicesObj,
                                  &drawCallsSeq, &indexSize)){
    return nullptr;
  }

  /* handle vertices and indices */
  if(indexSize != 2 && indexSize != 4){
    PyErr_Format(PyExc_ValueError, "drawCalls: indexSize must be 2 or 4, not %d.", indexSize);
    return nullptr;
  }
  if(!packMeshVertices(verticesObj, &vertices) || !packMeshIndices(indicesObj, &indices, indexSize)){
    return nullptr;
  }

  /* handle drawCalls */
  PyObject *seq = PySequence_Tuple(drawCallsSeq);
  if(!seq){
    PyErr_SetString(PyExc_TypeError, "drawCalls: drawCalls must be a sequence of draw-call tuples.");
    return nullptr;
  }
  Py_ssize_t nCalls = PyTuple_Size(seq);
  XPLMDrawCall_t *calls = nullptr;
  if(nCalls > 0){
    calls = (XPLMDrawCall_t *)malloc(sizeof(XPLMDrawCall_t) * nCalls);
    if(!calls){
      Py_DECREF(seq);
      return PyErr_NoMemory();
    }
  }
  for(Py_ssize_t i = 0; i < nCalls; i++){
    PyObject *fields = PySequence_Tuple(PyTuple_GetItem(seq, i));
    if(!fields || PyTuple_Size(fields) != 5){
      Py_XDECREF(fields); free(calls); Py_DECREF(seq);
      PyErr_SetString(PyExc_ValueError, "drawCalls: each draw call must be "
                      "(tex, scissors, idx_offset, element_count, vtx_offset).");
      return nullptr;
    }
    PyObject *texObj = PyTuple_GetItem(fields, 0);
    void *tex = nullptr;
    if(texObj && texObj != Py_None){
      tex = getVoidPtr(texObj, TEXTURE_CAPSULE);
      if(!tex && PyErr_Occurred()){
        Py_DECREF(fields); free(calls); Py_DECREF(seq);
        return nullptr;
      }
    }
    PyObject *scSeq = PySequence_Tuple(PyTuple_GetItem(fields, 1));
    if(!scSeq || PyTuple_Size(scSeq) != 4){
      Py_XDECREF(scSeq); Py_DECREF(fields); free(calls); Py_DECREF(seq);
      PyErr_SetString(PyExc_ValueError, "drawCalls: scissors must be a sequence of 4 floats "
                      "(left, top, right, bottom).");
      return nullptr;
    }
    calls[i].tex_ref = tex;
    calls[i].scissors[0] = (float)PyFloat_AsDouble(PyTuple_GetItem(scSeq, 0));
    calls[i].scissors[1] = (float)PyFloat_AsDouble(PyTuple_GetItem(scSeq, 1));
    calls[i].scissors[2] = (float)PyFloat_AsDouble(PyTuple_GetItem(scSeq, 2));
    calls[i].scissors[3] = (float)PyFloat_AsDouble(PyTuple_GetItem(scSeq, 3));
    Py_DECREF(scSeq);
    calls[i].idx_offset    = (int)PyLong_AsLong(PyTuple_GetItem(fields, 2));
    calls[i].element_count = (int)PyLong_AsLong(PyTuple_GetItem(fields, 3));
    calls[i].vtx_offset    = (int)PyLong_AsLong(PyTuple_GetItem(fields, 4));
    Py_DECREF(fields);
    if(PyErr_Occurred()){
      free(calls); Py_DECREF(seq);
      return nullptr;
    }
  }
  Py_DECREF(seq);

  XPLMMesh_t mesh;
  mesh.vertex_count = (int)vertices.count;
  mesh.vertices = (const float *)vertices.data;
  mesh.index_count = (int)indices.count;
  mesh.indices = (const uint16_t *)indices.data;

  XPLMDrawCalls_ptr(&mesh, (int)nCalls, calls);
  free(calls);
  Py_RETURN_NONE;
}


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
PyMethodDef panelGraphicsTextureMethods[] = {
  {"createTextureAtlas", (PyCFunction)XPLMCreateTextureAtlasFun, METH_VARARGS | METH_KEYWORDS, _createTextureAtlas__doc__},
  {"XPLMCreateTextureAtlas", (PyCFunction)XPLMCreateTextureAtlasFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"destroyTextureAtlas", (PyCFunction)XPLMDestroyTextureAtlasFun, METH_VARARGS | METH_KEYWORDS, _destroyTextureAtlas__doc__},
  {"XPLMDestroyTextureAtlas", (PyCFunction)XPLMDestroyTextureAtlasFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"textureAtlasAddImageFile", (PyCFunction)XPLMTextureAtlasAddImageFileFun, METH_VARARGS | METH_KEYWORDS, _textureAtlasAddImageFile__doc__},
  {"XPLMTextureAtlasAddImageFile", (PyCFunction)XPLMTextureAtlasAddImageFileFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"textureAtlasAddImageFileSet", (PyCFunction)XPLMTextureAtlasAddImageFileSetFun, METH_VARARGS | METH_KEYWORDS, _textureAtlasAddImageFileSet__doc__},
  {"XPLMTextureAtlasAddImageFileSet", (PyCFunction)XPLMTextureAtlasAddImageFileSetFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"textureAtlasAddImage", (PyCFunction)XPLMTextureAtlasAddImageFun, METH_VARARGS | METH_KEYWORDS, _textureAtlasAddImage__doc__},
  {"XPLMTextureAtlasAddImage", (PyCFunction)XPLMTextureAtlasAddImageFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"textureAtlasAddImageSet", (PyCFunction)XPLMTextureAtlasAddImageSetFun, METH_VARARGS | METH_KEYWORDS, _textureAtlasAddImageSet__doc__},
  {"XPLMTextureAtlasAddImageSet", (PyCFunction)XPLMTextureAtlasAddImageSetFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"textureAtlasBake", (PyCFunction)XPLMTextureAtlasBakeFun, METH_VARARGS | METH_KEYWORDS, _textureAtlasBake__doc__},
  {"XPLMTextureAtlasBake", (PyCFunction)XPLMTextureAtlasBakeFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"textureAtlasGetImageWidth", (PyCFunction)XPLMTextureAtlasGetImageWidthFun, METH_VARARGS | METH_KEYWORDS, _textureAtlasGetImageWidth__doc__},
  {"XPLMTextureAtlasGetImageWidth", (PyCFunction)XPLMTextureAtlasGetImageWidthFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"textureAtlasGetImageHeight", (PyCFunction)XPLMTextureAtlasGetImageHeightFun, METH_VARARGS | METH_KEYWORDS, _textureAtlasGetImageHeight__doc__},
  {"XPLMTextureAtlasGetImageHeight", (PyCFunction)XPLMTextureAtlasGetImageHeightFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"textureAtlasDrawAt", (PyCFunction)XPLMTextureAtlasDrawAtFun, METH_VARARGS | METH_KEYWORDS, _textureAtlasDrawAt__doc__},
  {"XPLMTextureAtlasDrawAt", (PyCFunction)XPLMTextureAtlasDrawAtFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"textureAtlasDrawIn", (PyCFunction)XPLMTextureAtlasDrawInFun, METH_VARARGS | METH_KEYWORDS, _textureAtlasDrawIn__doc__},
  {"XPLMTextureAtlasDrawIn", (PyCFunction)XPLMTextureAtlasDrawInFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"textureAtlasDrawStretched", (PyCFunction)XPLMTextureAtlasDrawStretchedFun, METH_VARARGS | METH_KEYWORDS, _textureAtlasDrawStretched__doc__},
  {"XPLMTextureAtlasDrawStretched", (PyCFunction)XPLMTextureAtlasDrawStretchedFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"textureAtlasDrawScaled", (PyCFunction)XPLMTextureAtlasDrawScaledFun, METH_VARARGS | METH_KEYWORDS, _textureAtlasDrawScaled__doc__},
  {"XPLMTextureAtlasDrawScaled", (PyCFunction)XPLMTextureAtlasDrawScaledFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"textureAtlasDrawMesh", (PyCFunction)XPLMTextureAtlasDrawMeshFun, METH_VARARGS | METH_KEYWORDS, _textureAtlasDrawMesh__doc__},
  {"XPLMTextureAtlasDrawMesh", (PyCFunction)XPLMTextureAtlasDrawMeshFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"textureSourceDrawIn", (PyCFunction)XPLMTextureSourceDrawInFun, METH_VARARGS | METH_KEYWORDS, _textureSourceDrawIn__doc__},
  {"XPLMTextureSourceDrawIn", (PyCFunction)XPLMTextureSourceDrawInFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"textureSourceDrawMesh", (PyCFunction)XPLMTextureSourceDrawMeshFun, METH_VARARGS | METH_KEYWORDS, _textureSourceDrawMesh__doc__},
  {"XPLMTextureSourceDrawMesh", (PyCFunction)XPLMTextureSourceDrawMeshFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"createTexture", (PyCFunction)XPLMCreateTextureFun, METH_VARARGS | METH_KEYWORDS, _createTexture__doc__},
  {"XPLMCreateTexture", (PyCFunction)XPLMCreateTextureFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"destroyTexture", (PyCFunction)XPLMDestroyTextureFun, METH_VARARGS | METH_KEYWORDS, _destroyTexture__doc__},
  {"XPLMDestroyTexture", (PyCFunction)XPLMDestroyTextureFun, METH_VARARGS | METH_KEYWORDS, ""},
  {"drawCalls", (PyCFunction)XPLMDrawCallsFun, METH_VARARGS | METH_KEYWORDS, _drawCalls__doc__},
  {"XPLMDrawCalls", (PyCFunction)XPLMDrawCallsFun, METH_VARARGS | METH_KEYWORDS, ""},
  {nullptr, nullptr, 0, nullptr}
};
#pragma GCC diagnostic pop
