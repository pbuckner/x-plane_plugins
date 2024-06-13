xpgl Images
===========

To use::

  from XPPython3 import xpgl

.. py:module:: xpgl
    :noindex:

Images refer to files loaded from disk such as PNG, GIF or JPEG. In most cases, you'll
:py:func:`loadImage` during plugin startup, and then :py:func:`drawTexture` during your
draw callback.

This requires the use of the python `Pillow` and `numpy` modules, and will generate
an error if not already installed.

.. py:function:: loadImage(filename, x=0, y=0, width=0, height=0) -> int

  :param str filename: Path to file, relative to X-Plane (e.g., "Resources/plugins/PythonPlugins/myfile.png")
  :param float x:
  :param float y: Optional offset *into* the image from lower-left corner.
  :param float width:
  :param float height: Optional width and height of *part* of the image to be extracted
  :return: internal integer representing texture

  In the simplest form, ``loadImage(filename)``, an image file is loaded into a texture, and an
  internal ID is returned. This ID *is not* an OpenGL texture ID, but represent a part of (perhaps
  *all* of) a texture.

  Optionally, you can provide data to extract a portion of the file's image. Specify the offset into
  the image (x, y), and the (width, height) of the portion to extract.

  If the file is already loaded as a texture, that texture will be reused, saving GPU space. A separate
  internal ID will be returned representing the different portion of the texture to be displayed.

  (Unlike SASL, we do not currently support a "search path" for resources.)

  For performance reasons, images should *not* be loaded during a draw callback. For example, use the XPluginEnable() callback.

  To load::

    filename = "Resources/bitmaps/cockpit/radios/transponder/transponder_HM-1.png"
    Data['fullID'] = xpgl.loadImage(filename)
    Data['dials'] = []
    for i in range(5):
        Data['dials'].append(xpgl.loadImage(filename, x=5 + i * 27, width=21, height=22))

  In the above example code, we've loaded the single file which contains five images for a transponder dial. First we load the full
  image and save its ID as ``Data['fullID']``. Then we load each *individual dial* with a separate ID. The underlying
  OpenGL Texture (containing all of ``transponder_HM-1.png``) is loaded only once.

  To display the full image, and separately display each individual dial, just reference the proper ID (note that
  if you provide a value for *color*, that color us used to replace white in the image.)::

    xpgl.drawRectangle(0, 0, 640, 480, color=Colors['light gray'])
    xpgl.drawTexture(Data['fullID'], 100, 100, width=135, height=27)
    for i in range(5):
        xpgl.drawTexture(Data['dials'][i], 100 + i * (22 + 10), 200 + (i * 5), width=22, height=27, color=Colors['green'])
    
  .. image:: /images/xpgl_loadImage.png
             :width: 50%
  
.. py:function:: drawTexture(texture_id, x, y, width, height, color=None) -> None
                 
  :param int texture_id: return from :py:func:`loadImage` for image to be displayed
  :param float x:
  :param float y: (x, y) location of the lower left corner of the texture
  :param float width:
  :param float height: Width and height of image to be displayed
  :param RGBColor color: Optional color to use to "add" to the image
  :return: None                       

  Draw the texture, with its lower-left corner placed at the (x, y) location. Width and height are
  *required*, Width and height can be different from the original image's width and height resulting
  in scaling or distorting the image.

  Color (normally) should not be provided. If present, that color will be blended with
  the original image.
  
  Assume the same ``load()`` as used with :py:func:`loadImage` example above, here
  are some different ways of displaying the first dial::

    xpgl.drawRectangle(0, 0, 640, 480, color=Colors['light gray'])
                                                      
    w = 22
    h = 27
    xpgl.drawTexture(Data['dials'][0], 100, 200, w,     h)
    xpgl.drawTexture(Data['dials'][0], 150, 200, w,     h, color=Colors['green'])
    xpgl.drawTexture(Data['dials'][0], 200, 200, w * 2, h * 2)
    xpgl.drawTexture(Data['dials'][0], 250, 200, w * 2, h)
    xpgl.drawTexture(Data['dials'][0], 300, 200, w / 2, h / 2)

  .. image:: /images/xpgl_drawTexture.png
             :width: 50%


.. py:function:: drawRotatedTexture(texture_id, angle, x, y, width, height, color=None) -> None:

  :param int texture_id: return from :py:func:`loadImage` for image to be displayed
  :param float angle: Degrees angle of rotation (counter-clockwise)                         
  :param float x:
  :param float y: (x, y) location of the lower left corner of the texture
  :param float width:
  :param float height: Width and height of image to be displayed
  :param RGBColor color: Optional color to use to "add" to the image
  :return: None                       

  Similar to :py:func:`drawTexture`, but first rotates the image *around its lower-left corner*.
  Negative angle results in clockwise rotation. ::
   
    # in load()
    texture = xpgl.loadImage("Resources/bitmaps/cockpit/clocks/chrono_HM.png")

    # in draw()
    xpgl.drawRotatedTexture(texture, 0, 300, 300, width=110, height=110)
    xpgl.drawRotatedTexture(texture, 45, 300, 300, width=110, height=110)
    xpgl.drawRotatedTexture(texture, 90, 300, 300, width=110, height=110)
    xpgl.drawRotatedTexture(texture, -45, 300, 300, width=110, height=110)
  
  .. image:: /images/xpgl_drawRotatedTexture.png
             :width: 50%

.. py:function:: drawRotatedTextureCenter(texture_id, angle, x, y, width, height, color=None) -> None:

  :param int texture_id: return from :py:func:`loadImage` for image to be displayed
  :param float angle: Degrees angle of rotation (counter-clockwise)                         
  :param float x:
  :param float y: (x, y) location of the lower left corner of the texture
  :param float width:
  :param float height: Width and height of image to be displayed
  :param RGBColor color: Optional color to use to "add" in the image
  :return: None                       

  Same as :py:func:`drawRotatedTexture`, but calculates, and rotates around the *center* of the image.
  Negative angle results in clockwise rotation. ::
   
    # in load()
    texture = xpgl.loadImage("Resources/bitmaps/cockpit/supplement/gmeter_linear-1.png")

    # in draw()
    w = 25
    h = 511
    xpgl.drawRotatedTextureCenter(texture,   0, 300, (screen.height - h) / 2, w, h)
    xpgl.drawRotatedTextureCenter(texture,  45, 300, (screen.height - h) / 2, w, h)
    xpgl.drawRotatedTextureCenter(texture,  90, 300, (screen.height - h) / 2, w, h)
    xpgl.drawRotatedTextureCenter(texture, -45, 300, (screen.height - h) / 2, w, h)
  
  .. image:: /images/xpgl_drawRotatedTextureCenter.png
             :width: 50%

