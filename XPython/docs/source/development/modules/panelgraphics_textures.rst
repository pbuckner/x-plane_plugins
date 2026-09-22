Textures
========

.. py:module:: XPLMPanelGraphics
    :no-index:
.. py:currentmodule:: xp

To use::

  import xp

Texture drawing for the native panel-graphics API (new with SDK 440). Three
related facilities:

.. rst-class:: compact
               
* a **texture atlas** you build from PNG files or raw RGBA, bake once, and draw
  many times (with tinting, 9-slice, arbitrary transform, or onto a mesh);
* **texture sources** --- live stock simulator textures such as the weather
  radar; and
* a **Dear ImGui-style** path (:func:`createTexture` + :func:`drawCalls`) for
  rendering an ImGui-compatible vertex/index buffer.

Draw from a panel-graphics drawing context --- see :doc:`panelgraphics`.
*tintColor* arguments are packed values from :func:`makeColor`; use
``makeColor(1, 1, 1, 1)`` for no tinting.

The atlas creation, addition, baking,  and queries (i.e., height, width) can be done at any time on the main thread: they
do not need to be done within a draw callback (generally, they *should not* be done within a
callback, for performance reasons.)

Drawing (``textureAtlasDraw*()``, ``textureSourceDraw*()``) must be done within a window
or avionics device with :data:`WindowContentTypePanelGraphics` context.

Texture atlas
-------------

See ``samples/PI_PGTextureAtlas.py`` for a working example: it packs seven
generated images and one PNG into one atlas, then draws each of them through
every ``textureAtlasDraw*`` entry point in a labelled grid.

.. py:function:: createTextureAtlas() -> XPLMTextureAtlasRef

    :return: A texture-atlas handle
    :rtype: XPLMTextureAtlasRef capsule

    Create a new, empty texture atlas. You will add images with :func:`textureAtlasAddImageFile`
    / :func:`textureAtlasAddImage` (and their ``*Set`` variants), then call
    :func:`textureAtlasBake` before drawing. Destroy with
    :func:`destroyTextureAtlas` when done.

    >>> atlas = xp.createTextureAtlas()
    >>> atlas
    <capsule object "XPLMTextureAtlasRef" at 0x138807420>

    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMCreateTextureAtlas>`__ :index:`XPLMCreateTextureAtlas`

.. py:function:: destroyTextureAtlas(atlas) -> None

    :param XPLMTextureAtlasRef atlas: Handle from :func:`createTextureAtlas`

    Destroy a texture atlas and free all associated GPU and CPU resources.

    >>> xp.destroyTextureAtlas(atlas)
    
    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMDestroyTextureAtlas>`__ :index:`XPLMDestroyTextureAtlas`

.. py:function:: textureAtlasAddImageFile(atlas, imageFilePath) -> int

    :param XPLMTextureAtlasRef atlas: Handle from :func:`createTextureAtlas`
    :param str imageFilePath: Path to a PNG file
    :return: The zero-based image index assigned, -1 on error

    Load a PNG file and add it to the atlas as a single image. Call before
    :func:`textureAtlasBake`. Returns -1 if file not found or cannot be loaded.

    ``imageFilePath`` can be absolute path, or relative to X-Plane root.      

    >>> xp.textureAtlasAddImageFile(atlas, 'Resources/bitmaps/icons/X-Plane.png')
    0 
    >>> xp.textureAtlasAddImageFile(atlas, '/tmp/this-file-does-not-exist.png')
    -1

    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMTextureAtlasAddImageFile>`__ :index:`XPLMTextureAtlasAddImageFile`

.. py:function:: textureAtlasAddImageFileSet(atlas, imageFilePath, cellsX, cellsY) -> int

    :param XPLMTextureAtlasRef atlas: Handle from :func:`createTextureAtlas`
    :param str imageFilePath: Path to a PNG file
    :param int cellsX: Number of columns
    :param int cellsY: Number of rows
    :return: Index of the *first* (top-left) cell; cell ``(x, y)`` is ``index + y * cellsX + x``

    Load a PNG file and subdivide it into a ``cellsX`` × ``cellsY`` grid, adding
    each cell as a separate image (useful for sprite sheets).

    Here, the original single image contains six different versions of the button. Using this function, we
    can load the single file divided into six different images.

    .. image:: /images/dg_pushbutton.png

    >>> xp.textureAtlasAddImageFileSet(atlas, 'Resources/bitmaps/interface/dg_pushbutton.png', cellsX=1, cellsY=6)
    2

    Image #2 in the atlas is the first (top) button, Image #3 is second from top, etc., through Image #7. If we add another
    image to the atlas, it will get index #8

    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMTextureAtlasAddImageFileSet>`__ :index:`XPLMTextureAtlasAddImageFileSet`

.. py:function:: textureAtlasAddImage(atlas, image, width, height) -> int

    :param XPLMTextureAtlasRef atlas: Handle from :func:`createTextureAtlas`
    :param bytes image: Raw RGBA pixel data (4 bytes/pixel, rows top to bottom)
    :param int width: Image width in pixels
    :param int height: Image height in pixels
    :return: The zero-based image index assigned

    Add a single image from raw RGBA pixel data. *image* must hold at least
    ``width * height * 4`` bytes.

    An easy way to create RGBA pixel data is using python PIL module:

    >>> from PIL import Image, ImageDraw
    >>> img = Image.new("RGBA", (300, 300))
    >>> draw = ImageDraw.Draw(img)
    >>> draw.arc([(0, 150), (150, 300)], 180, 0, fill="magenta", width=20)
    >>> xp.textureAtlasAddImage(atlas, img.tobytes(), 300, 300)
    

    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMTextureAtlasAddImage>`__ :index:`XPLMTextureAtlasAddImage`

.. py:function:: textureAtlasAddImageSet(atlas, image, width, height, cellsX, cellsY) -> int

    :param XPLMTextureAtlasRef atlas: Handle from :func:`createTextureAtlas`
    :param bytes image: Raw RGBA pixel data (4 bytes/pixel, rows top to bottom)
    :param int width: Image width in pixels
    :param int height: Image height in pixels
    :param int cellsX: Number of columns
    :param int cellsY: Number of rows
    :return: Index of the first cell; cell ``(x, y)`` is ``index + y * cellsX + x``

    Add raw RGBA pixel data subdivided into a ``cellsX`` × ``cellsY`` grid; each
    cell becomes a separate image. *image* must hold at least ``width * height * 4``
    bytes.

    This does the same as :func:`textureAtlasAddImageFileSet`, but works off of raw image bytes.
    
    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMTextureAtlasAddImageSet>`__ :index:`XPLMTextureAtlasAddImageSet`

.. py:function:: textureAtlasBake(atlas) -> None

    :param XPLMTextureAtlasRef atlas: Handle from :func:`createTextureAtlas`

    Pack all previously added images into a GPU texture. Must be called after
    adding all images and before any draw calls. *Once baked, no more images may be
    added.*

    >>> xp.textureAtlasBake(atlas)
    
    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMTextureAtlasBake>`__ :index:`XPLMTextureAtlasBake`

.. py:function:: textureAtlasGetImageWidth(atlas, imageIndex) -> int

    :param XPLMTextureAtlasRef atlas: Handle from :func:`createTextureAtlas`
    :param int imageIndex: Image (or cell) index
    :return: Width in pixels, 0 if no image
    :rtype: int

    >>> xp.textureAtlasGetImageWidth(atlas, 0)
    256
    >>> xp.textureAtlasGetImageWidth(atlas, 1)
    256
    >>> xp.textureAtlasGetImageWidth(atlas, 2)
    17
    >>> xp.textureAtlasGetImageWidth(atlas, 1000)
    0
    
    .. warning:: X-Plane will crash if you call this function before calling :func:`textureAtlasBake`.

    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMTextureAtlasGetImageWidth>`__ :index:`XPLMTextureAtlasGetImageWidth`

.. py:function:: textureAtlasGetImageHeight(atlas, imageIndex) -> int

    :param XPLMTextureAtlasRef atlas: Handle from :func:`createTextureAtlas`
    :param int imageIndex: Image (or cell) index
    :return: Height in pixels, 0 if no image
    :rtype: int

    >>> xp.textureAtlasGetImageHeight(atlas, 0)
    256
    >>> xp.textureAtlasGetImageHeight(atlas, 1)
    256
    >>> xp.textureAtlasGetImageHeight(atlas, 2)
    21
    >>> xp.textureAtlasGetImageHeight(atlas, 1000)
    0
    
    .. warning:: X-Plane will crash if you call this function before calling :func:`textureAtlasBake`

    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMTextureAtlasGetImageHeight>`__ :index:`XPLMTextureAtlasGetImageHeight`

.. py:function:: textureAtlasDrawAt(atlas, imageIndex=0, tintColor=White, x=NaN, y=NaN) -> None

    :param XPLMTextureAtlasRef atlas: Handle from :func:`createTextureAtlas`
    :param int imageIndex: Image index
    :param int tintColor: Packed tint color (see :func:`makeColor`). Default is white (no-tint)
    :param float x: X of the top-left corner
    :param float y: Y of the top-left corner

    Draw an atlas image at its native resolution with the image's *top-left* corner at
    ``(x, y)``. If not set (NaN), the image to be drawn with its *lower-left*
    corner at (0, 0): we'll extract the image's height and use that value

    >>> def myScreenDraw(refCon):
    ...     xp.textureAtlasDrawAt(atlas, 0)
    ...
    >>> def myBezelDraw(r, g, b, refCon):
    ...     xp.polygon(xp.makeColor(0, 0, 0, 1), [(0, 0), (200, 0), (200, 100), (0, 100)])
    ...
    >>> avionicsID = xp.createAvionicsEx(screenDraw=myScreenDraw, bezelDraw=myBezelDraw,
    ...                                  screenWidth=200, screenHeight=100, bezelWidth=200, bezelHeight=100,
    ...                                  screenOffsetX=0, screenOffsetY=0,
    ...                                  contentType=xp.WindowContentTypePanelGraphics, windowWithChrome=1)
    ...
    >>> xp.setAvionicsPopupVisible(avionicsID)
    
    .. image:: /images/panel_texture_drawAt.png
               :width: 200px

    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMTextureAtlasDrawAt>`__ :index:`XPLMTextureAtlasDrawAt`

.. py:function:: textureAtlasDrawIn(atlas, imageIndex=0, tintColor=White, left=0, top=NaN, right=NaN, bottom=0) -> None

    :param XPLMTextureAtlasRef atlas: Handle from :func:`createTextureAtlas`
    :param int imageIndex: Image index
    :param int tintColor: Packed tint color
    :param float left: Left edge
    :param float top: Top edge
    :param float right: Right edge
    :param float bottom: Bottom edge

    Draw an atlas image scaled to fill the rectangle, stretching or compressing to
    match. Unset ``top`` and ``right`` (NaN) set values to match height and width of the original image.

    Replacing  ``myScreenDraw()`` in the :func:`textureAtlasDrawAt`:

    >>> def myScreenDraw(refCon):
    ...     xp.textureAtlasDrawIn(atlas, 0)
    ...

    The image at it's native size:

    .. image:: /images/panel_texture_drawAt.png
               :width: 200px

    >>> def myScreenDraw(refCon):
    ...     xp.textureAtlasDrawIn(atlas, 0, left=0, top=100, right=200, bottom=0)
    ...

    The image squished to fit the space:
    
    .. image:: /images/panel_texture_drawIn1.png
               :width: 200px

    >>> def myScreenDraw(refCon):
    ...     height, width = xp.textureAtlasGetImageHeight(atlas, 0), xp.textureAtlasGetImageWidth(atlas, 0)
    ...     right = (100. / height) * width
    ...     xp.textureAtlasDrawIn(atlas, 0, left=0, top=100, right=right, bottom=0)
    ...

    The image scaled to fit:
    
    .. image:: /images/panel_texture_drawIn2.png
               :width: 200px

    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMTextureAtlasDrawIn>`__ :index:`XPLMTextureAtlasDrawIn`

.. py:function:: textureAtlasDrawStretched(atlas, imageIndex=0, tintColor=White, left=0, top=Nan, right=NaN, bottom=0) -> None

    :param XPLMTextureAtlasRef atlas: Handle from :func:`createTextureAtlas`
    :param int imageIndex: Image index
    :param int tintColor: Packed tint color
    :param float left: Left edge
    :param float top: Top edge
    :param float right: Right edge
    :param float bottom: Bottom edge

    Draw an atlas image into the rectangle using 9-slice scaling: the four corners
    keep their native size, edges stretch along one axis, and the center stretches
    in both. Preserves borders when scaling UI elements.

    Unset ``top`` and ``right`` (NaN) set values to match height and width of the original image.

    .. code-block:: 

        ┌─┬─┬─┐          ┌─┬──────┬─┐
        ├─┼─┼─┤          ├─┼──────┼─┤
        ├─┼─┼─┤   ->     │ │      │ │
        └─┴─┴─┘          │ │      │ │
                         ├─┼──────┼─┤
                         └─┴──────┴─┘

    We'll create a larger avionics device (500x500), so we can scale the image:
    
    >>> atlas = xp.createTextureAtlas()
    >>> idx = xp.textureAtlasAddImageFile(atlas, 'Resources/bitmaps/icons/X-Plane.png')
    >>> xp.textureAtlasBake(atlas)
    >>> def myScreenDraw(refCon):
    ...     height, width = xp.textureAtlasGetImageHeight(atlas, 0), xp.textureAtlasGetImageWidth(atlas, 0)
    ...     right = (500. / height) * width
    ...     xp.textureAtlasDrawStretched(atlas, 0, left=0, top=500, right=right, bottom=0)
    ...
    >>> def myBezelDraw(r, g, b, refCon):
    ...     xp.polygon(xp.makeColor(0, 0, 0, 1), [(0, 0), (500, 0), (500, 500), (0, 500)])
    ...
    >>> avionicsID = xp.createAvionicsEx(screenDraw=myScreenDraw, bezelDraw=myBezelDraw,
    ...                                  screenWidth=500, screenHeight=500, bezelWidth=500, bezelHeight=500,
    ...                                  screenOffsetX=0, screenOffsetY=0,
    ...                                  contentType=xp.WindowContentTypePanelGraphics, windowWithChrome=1)
    ...
    >>> xp.setAvionicsPopupVisible(avionicsID)

    Stretching can result in perhaps unexpected results.

    .. image:: /images/panel_texture_drawStretched.png
               :width: 400px

    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMTextureAtlasDrawStretched>`__ :index:`XPLMTextureAtlasDrawStretched`

.. py:function:: textureAtlasDrawScaled(atlas, imageIndex=0, tintColor=White, xPanel=NaN, yPanel=NaN, xAtlas=NaN, yAtlas=NaN, xScale=1, yScale=1, rotateCW=0) -> None

    :param XPLMTextureAtlasRef atlas: Handle from :func:`createTextureAtlas`
    :param int imageIndex: Image index
    :param int tintColor: Packed tint color
    :param float xPanel: Panel-space X the pivot aligns to
    :param float yPanel: Panel-space Y the pivot aligns to
    :param float xAtlas: Pivot X, in pixels from the image's bottom-left
    :param float yAtlas: Pivot Y, in pixels from the image's bottom-left
    :param float xScale: Horizontal scale factor
    :param float yScale: Vertical scale factor
    :param float rotateCW: Rotation in degrees, clockwise about the pivot

    Draw an atlas image with arbitrary scaling, rotation, and positioning. The
    atlas-space pivot ``(xAtlas, yAtlas)`` is aligned to the panel-space point
    ``(xPanel, yPanel)``, then scaled by ``(xScale, yScale)`` and rotated
    *rotateCW* degrees clockwise about that pivot.

    If xPanel, yPanel not specified, we set to center point of the image (*not* the panel -- we don't know the panel size).
    If xAtlas, yAtlas not specified, we set to center point of the image.

    >>> atlas = xp.createTextureAtlas()
    >>> idx = xp.textureAtlasAddImageFile(atlas, 'Resources/bitmaps/icons/X-Plane.png')
    >>> xp.textureAtlasBake(atlas)
    >>> def myScreenDraw(refCon):
    ...     xp.textureAtlasDrawScaled(atlas, rotateCW=xp.getCycleNumber() % 360, xScale=1.5)
    ...
    >>> def myBezelDraw(r, g, b, refCon):
    ...     xp.polygon(xp.makeColor(0, 0, 0, 1), [(0, 0), (500, 0), (500, 500), (0, 500)])
    ...
    >>> avionicsID = xp.createAvionicsEx(screenDraw=myScreenDraw, bezelDraw=myBezelDraw,
    ...                                  screenWidth=500, screenHeight=500, bezelWidth=500, bezelHeight=500,
    ...                                  screenOffsetX=0, screenOffsetY=0,
    ...                                  contentType=xp.WindowContentTypePanelGraphics, windowWithChrome=1)
    ...
    >>> xp.setAvionicsPopupVisible(avionicsID)

    .. image:: /images/panel_texture_scaled.gif
               :width: 300px

    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMTextureAtlasDrawScaled>`__ :index:`XPLMTextureAtlasDrawScaled`

.. py:function:: textureAtlasDrawMesh(atlas, imageIndex=0, tintColor=White, vertices=None) -> None

    :param XPLMTextureAtlasRef atlas: Handle from :func:`createTextureAtlas`
    :param int imageIndex: Image index
    :param int tintColor: Packed tint color
    :param vertices: Sequence of ``(x, y, s, t)`` tuples (at least 3)

    Draw an atlas image onto an arbitrary triangle-strip mesh. Each vertex is an
    ``(x, y, s, t)`` tuple: ``(x, y)`` panel-space position in pixels, ``(s, t)``
    normalized texture coordinate (0.0--1.0) within the image.

    If vertices is not specified, it defaults to an identity mesh, based on the retrieved
    height and width of the image::

        [(    0,      0, 0, 0),
         (    0, height, 0, 1),
         (width,      0, 1, 0),
         (width, height, 1, 1)]

    .. note:: Vertices is a triangle-strip mesh, not a polygon, so their order needs
              to conform to :doc:`strip_order`.

    >>> atlas = xp.createTextureAtlas()
    >>> idx = xp.textureAtlasAddImageFile(atlas, 'Resources/bitmaps/icons/X-Plane.png')
    >>> xp.textureAtlasBake(atlas)
    >>> def myScreenDraw(refCon):
    ...     xp.textureAtlasDrawMesh(atlas, vertices=[(0, 0, 0, 0), (0, 200, 0, 1), (200, 0, 1, 0), (500, 500, 1, 1)])
    ...
    >>> def myBezelDraw(r, g, b, refCon):
    ...     xp.polygon(xp.makeColor(0, 0, 0, 1), [(0, 0), (500, 0), (500, 500), (0, 500)])
    ...
    >>> avionicsID = xp.createAvionicsEx(screenDraw=myScreenDraw, bezelDraw=myBezelDraw,
    ...                                  screenWidth=500, screenHeight=500, bezelWidth=500, bezelHeight=500,
    ...                                  screenOffsetX=0, screenOffsetY=0,
    ...                                  contentType=xp.WindowContentTypePanelGraphics, windowWithChrome=1)
    ...
    >>> xp.setAvionicsPopupVisible(avionicsID)

    .. image:: /images/panel_texture_drawMesh.png
               :width: 300px

    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMTextureAtlasDrawMesh>`__ :index:`XPLMTextureAtlasDrawMesh`

Texture source
--------------

A *texture source* is a live stock simulator texture (e.g. the weather radar)
that X-Plane renders each frame. *tex* is one of the ``Texture_*`` constants.

.. py:function:: textureSourceDrawIn(tex=WeatherRadar1, tintColor=White, left=0, top=0, right=0, bottom=0) -> None

    :param int tex: A ``Texture_*`` source (e.g. :data:`Texture_WeatherRadar1`)
    :param int tintColor: Packed tint color (see :func:`makeColor`)
    :param int left: Left edge in panel coordinates
    :param int top: Top edge
    :param int right: Right edge
    :param int bottom: Bottom edge

    Draw a stock simulator texture source scaled to fill the rectangle. If the
    aircraft lacks the requested hardware, the call is silently skipped.

    >>> def myScreenDraw(refCon):
    ...     xp.textureSourceDrawIn(right=500, top=500)
    ...
    >>> def myBezelDraw(r, g, b, refCon):
    ...     xp.polygon(xp.makeColor(0, 0, 0, 1), [(0, 0), (500, 0), (500, 500), (0, 500)])
    ...
    >>> avionicsID = xp.createAvionicsEx(screenDraw=myScreenDraw, bezelDraw=myBezelDraw,
    ...                                  screenWidth=500, screenHeight=500, bezelWidth=500, bezelHeight=500,
    ...                                  screenOffsetX=0, screenOffsetY=0,
    ...                                  contentType=xp.WindowContentTypePanelGraphics, windowWithChrome=1)
    ...
    >>> xp.setAvionicsPopupVisible(avionicsID)
    
    .. image:: /images/panel_source_draw.png
               :width: 300px

    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMTextureSourceDrawIn>`__ :index:`XPLMTextureSourceDrawIn`

.. py:function:: textureSourceDrawMesh(tex=WeatherRadar1, tintColor=White, vertices=None) -> None

    :param int tex: A ``Texture_*`` source
    :param int tintColor: Packed tint color
    :param vertices: Sequence of ``(x, y, s, t)`` tuples (at least 3)

    Draw a stock simulator texture source onto an arbitrary triangle-strip mesh.
    Each vertex is an ``(x, y, s, t)`` tuple: ``(x, y)`` panel-space position in
    pixels, ``(s, t)`` normalized texture coordinate (0.0--1.0) within the source.

    In this example, we distort the radar to be more fan-shaped, bringing in the lower-left and lower-right
    corners of the image. (The rounded top of the radar image is an artifact of the image itself: we're not
    distorting that part.)
    
    >>> def myScreenDraw(refCon):
    ...     xp.textureSourceDrawMesh(vertices=[(200, 0, 0, 0), (0, 500, 0, 1), (300, 0, 1, 0), (500, 500, 1, 1)]) 
    ...
    >>> def myBezelDraw(r, g, b, refCon):
    ...     xp.polygon(xp.makeColor(0, 0, 0, 1), [(0, 0), (500, 0), (500, 500), (0, 500)])
    ...
    >>> avionicsID = xp.createAvionicsEx(screenDraw=myScreenDraw, bezelDraw=myBezelDraw,
    ...                                  screenWidth=500, screenHeight=500, bezelWidth=500, bezelHeight=500,
    ...                                  screenOffsetX=0, screenOffsetY=0,
    ...                                  contentType=xp.WindowContentTypePanelGraphics, windowWithChrome=1)
    ...
    >>> xp.setAvionicsPopupVisible(avionicsID)
    
    .. image:: /images/panel_source_mesh.png
               :width: 300px

    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMTextureSourceDrawMesh>`__ :index:`XPLMTextureSourceDrawMesh`

Dear ImGui-style draw calls
---------------------------

The C-API added this feature to support Dear ImGui DrawLists, a set of non-interactive graphics.
You do not need ImGui to use it: :func:`drawCalls` takes any mesh you build yourself, and Python's
PIL module can replicate most of what ImGui draw lists provide. For that reason we present the
X-Plane API referencing PIL rather than ImGui. See the next section.

If you *do* want ImGui, :mod:`xp_imgui` supports it in both window content types and on avionics
devices --- see :doc:`/development/imgui_plugin` and :doc:`/development/imgui_avionics`.

.. _pg-pil-draw-calls:

PIL-style draw calls
--------------------

The basic idea is to create static visuals using PIL, and create a texture with the resulting image. This
texture (textures...) are loaded into the GPU for fast execution.

#. Create complex image (``PIL.Image``) outside of a drawing callback

#. Convert that image to a texture using :func:`createTexture`, still outside of a drawing callback

#. Draw one or more of these textures using :func:`drawCalls`, within display window :func:`draw`, or
   avionics :func:`screenDraw` callback.

#. Destroy texture when no longer need using :func:`destroyTexture`.

.. py:function:: createTexture(rgba, width, height) -> XPLMTexture

    :param bytes rgba: RGBA PIL image (4 bytes/pixel, rows top to bottom; at least ``width * height * 4`` bytes)
    :param int width: Image width in pixels
    :param int height: Image height in pixels
    :return: An opaque XPLMTexture handle

    .. warning:: **DO NOT** call this within a callback. You must create textures on the main thread.

    Upload an RGBA image to the GPU and return a handle for use as the *tex* of a
    draw call passed to :func:`drawCalls`. Free it with :func:`destroyTexture`. The
    sampler is bilinear, clamp-to-edge, no mipmaps. It is safe to create and
    destroy textures every frame.

    You'll need to call ``.tobytes()`` on a PIL Image to get the proper data from :data:`rgba` attribute.

    While drawing with PIL, remember the origin (0, 0) is the *upper-left* corner.
    
    >>> from PIL import Image, ImageDraw, ImageFont
    >>> img = Image.new("RGBA", (300, 300))
    >>> font = ImageFont.truetype('Resources/fonts/Inconsolata.ttf')
    >>> draw = ImageDraw.Draw(img)
    >>> draw.arc([(0, 0), (300, 300)], 180, 0, fill="magenta", width=20)
    >>> draw.line([(150, 0), (150, 300)], fill="red", width=5)
    >>> draw.line([(0, 150), (300, 150)], fill="white", width=10)
    >>> draw.text((150, 0), "This side UP", anchor="mt", fill='black')

    At this point, if you're drawing the above outside of X-Plane you can call ``img.show()`` and the following
    image will be displayed:

    .. image:: /images/pil_basic.png
               :align: center
               :width: 200px

    (Again, note ``draw.text()`` location at y=0 puts the text at the TOP of the image.)
    
    Within X-Plane, you can take that same image and convert it to a texture:

    >>> tex = xp.createTexture(img.tobytes(), *img.size)

    .. note:: Note that Python PIL "RGBA" images are identical to ImGui RGBA8 and can be used interchangeably.
              Because our examples only use PIL, we'll refer to the image type as "RGBA".

    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMCreateTexture>`__ :index:`XPLMCreateTexture`

.. py:function:: destroyTexture(tex) -> None

    :param XPLMTexture tex: Handle from :func:`createTexture`

    Free a texture created with :func:`createTexture`. The handle must not be used
    after this call.

    >>> xp.destroyTexture(tex)
    
    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMDestroyTexture>`__ :index:`XPLMDestroyTexture`

.. py:function:: drawCalls(vertices, indices, drawCalls) -> None

    :param bytes vertices: See below
    :param bytes indices: See below
    :param drawCalls: Sequence of ``(tex, scissors, idx_offset, element_count, vtx_offset)``

    This function uploads a mesh to the GPU and dispatches drawing calls. It is specially
    designed to be compatible with ImGui ``ImDrawVert`` formatted data, as well as a simpler
    python signature.

    ``vertices``
    ++++++++++++

      ``vertices`` is sequence of 5-tuples ``[(x, y, u, v, color), ...]``, each representing
      a single vertex.
  
      .. rst-class:: compact
                     
      * ``x, y`` are in panel (window) coordinates with ``(0, 0)``
        at the *upper-left* of the panel.
  
      * ``u, v`` are [0.0 .. 1.0] offsets into the texture to be drawn, with ``(0, 0)``
        also at the *upper-left* of the texture.
  
      * ``color`` is a packed color, the result of :func:`makeColor`. If you only specify
        four elements we'll default the vertex color to 'white' (no-tint)
  
      There can be any number of vertices (at least three), in any order. You'll use
      ``indices`` to specify the draw order. This is *not* a triangle-strip.
  
      Alternatively, ``vertices`` can be a single byte sequence of repeating 20 bytes
      ``struct.pack('<ffffI', x, y, u, v, makeColor(...))``. Join together a set of
      these 20-byte vertices and pass the single bytes value as this attribute. This
      packing matches the ``ImDrawVert`` structure used by ImGui. Internally we
      convert the former style into this packed style, which is required by the C-API.
  
      >>> vertices = [(  0,   0, 0.0, 0.0),  # 0: upper left
      ...             (  0, 300, 0.0, 1.0),  # 1: lower left
      ...             (300, 300, 1.0, 1.0),  # 2: lower right
      ...             (300,   0, 1.0, 0.0)]  # 3: upper right
  
      To draw these vertices *as triangles*, we could specify any of:
  
      >>> indices = [0, 1, 3, 1, 2, 3]
      >>> indices = [1, 2, 3, 0, 1, 3]
      >>> indices = [2, 1, 0, 0, 3, 2]
      ...
  

    ``indices``
    +++++++++++
    
      ``indices`` is a sequence of integers, describing a set of triangles to be drawn. The
      number of elements must be a multiple of three. Each index identifies one
      of the (previously specified) vertices. A triangle will be drawn using
      every three indices. *This is not a triangle strip.* There is no requirement
      ordering requirement in vertices, and no requirement that each vertex is
      used at least once by an index. Indices may repeat the same vertex where it
      makes sense.
  
      Alternatively, ``indices`` can be a single byte sequence of 16 bit unsigned integers.
      (E.g., ``struct.pack(f'<{len(indices)}H', *indices)``). Internally we convert the
      sequence system into this packed style, which is required by the C-API
  
    Render textured indexed-triangle draw calls matching Dear ImGui's
    ``ImDrawData`` layout.

    ``drawCalls``
    +++++++++++++

      Each draw call is a tuple ``(tex, scissors, idx_offset, element_count, vtx_offset)``:

      .. rst-class:: compact
                     
      * *tex*: a :func:`createTexture` handle, or None. If None, then only colored vertices are
        drawn.
      * *scissors*: ``(left, top, right, bottom)`` in window-local top-left coordinates. Always required,
        think of it as the bounding-box for what you're drawing.
      * *idx_offset*: Commonly 0, but this allows you so re-use ``vertices`` and ``indices``, specifying
        a starting idx other than 0 for drawing.
      * *element_count*: (a multiple of 3). Commonly len(indices), but could be less.
      * *vtx_offset*: Commonly 0, but will be added to idx_offset to select a different part of the mesh.

      See more detailed example below covering potential use of ``idx_offset`` and ``vtx_offset``.

    The mesh is uploaded once and one GPU dispatch is issued per call. Call only
    from a panel-graphics window draw callback; the host flips Y for you.

    For Python PIL drawing, note that the drawing origin (0, 0) is the *upper-left* of
    the image.

    To draw the texture we previously created in :func:`createTexture`:

    >>> texture = xp.createTexture(img.tobytes(), *img.size)
    >>> def myScreenDraw(refCon):
    ...     vertices = [(0, 0, 0, 0), (0, 200, 0, 1), (200, 0, 1, 0), (200, 200, 1, 1)]
    ...     indices = [0, 1, 3,  0, 2, 3]     # two triangles
    ...     clip = [0, 0, 200, 200]           # (left, top, right, bottom)
    ...     drawCalls = [(texture, clip, 0, 6, 0), ]
    ...     with xp.transformContext():
    ...         xp.transformTranslate(50, 100)
    ...         xp.drawCalls(vertices, indices, drawCalls)
    ...
    >>> def myBezelDraw(r, g, b, refCon):
    ...     xp.polygon(xp.makeColor(0, 0, 0, 1), [(0, 0), (200, 0), (200, 200), (0, 200)])
    ...
    >>> avionicsID = xp.createAvionicsEx(screenDraw=myScreenDraw, bezelDraw=myBezelDraw,
    ...                                  screenWidth=200, screenHeight=200, bezelWidth=200, bezelHeight=200,
    ...                                  screenOffsetX=0, screenOffsetY=0,
    ...                                  contentType=xp.WindowContentTypePanelGraphics, windowWithChrome=1)
    ...
    >>> xp.setAvionicsPopupVisible(avionicsID)
    
    .. image:: /images/panel_pil.png
               :width: 300px
    
    Reminder: the *panel* (*window*) is still drawn with (0, 0) in the lower left corner. Your textures, and the function
    :func:`drawCalls` (with (0, 0) in the upper left) handle the y-shift properly. But, if you're looking to do a translation transform, the
    translation still works on panel coordinates, so::

      >>> with xp.transformContext():
      ...     xp.transformTranslate(50, 100)
      ...     xp.drawCalls(vertices, indices, drawCalls)

    Yields a shift right and up:

    .. image:: /images/panel_pil_translate.png
                :width: 200px

    Regarding ``drawCalls`` with non-zero ``idx_offset`` and/or non-zero ``vtx_offset``: The key is :func:`drawCalls` uploads the mesh
    only once. A GPU dispatch is issued *once per call*, each call capable of rebinding texture and scissor. The offsets are
    used to allow each drawCall (each dispatch) to select a part of the shared mesh.

    ``idx_offset`` sets the base for the index buffer (``indices``)

    As an example, assume you upload a single mesh which contains three sub-meshes and you have three different textures::

      verts   = quad_a + quad_b + tri_c    # 4 + 4 + 3 = 11 vertices
      indices = [0, 1, 2,  0, 2, 3,        # quad A: two triangles making a quad: idx 0..5
                 4, 5, 6,  4, 6, 7,        # quad B: two triangles making a quad: idx 6..11,
                 8, 9, 10]                 # tri  C: one triangle: idx 12..14
      calls   = [(tex_a, clip,  0, 6, 0),  # tex_a drawn with vertices pointed to by first six indices
                 (tex_b, clip,  6, 6, 0),  # tex_b drawn with vertices pointed to by next six indices
                 (None,  clip, 12, 3, 0)]  # Null texture drawn with vertices pointed to by indices[12:12+3]
      xp.drawCalls(verts, indices, calls)

    One mesh (``verts``) uploaded, with three separate calls.
    
    That explains non-zero ``idx_offset``. You can do something similar with ``vtx_offset``: You can zero-base
    the vertex pool also. Same example as above, but re-write ``indices`` to re-based each set of vertices::

      indices = [0, 1, 2,  0, 2, 3,        # written 0-based...
                 0, 1, 2,  0, 2, 3,        # ...and again, identically
                 0, 1, 2]
      calls = [(tex_a, clip,  0, 6, 0),    # quad A at vertex 0
               (tex_b, clip,  6, 6, 4),    # same indices, +4 → vertices 4..7
               (None,  clip, 12, 3, 8)]

    -or-

    Because the two quads have the same index pattern ``[0, 1, 2,   0, 2, 3]``, you can rewrite calls to reduce
    the size of ``indices``::

      indices = [0, 1, 2,  0, 2, 3,        # any quad
                 0, 1, 2]                  # any triangle
      calls = [(tex_a, clip,  0, 6, 0),    # quad A at vertex 0
               (tex_b, clip,  0, 6, 4),    # same indices, +4 → vertices 4..7
               (None,  clip,  6, 3, 8)]

    The offsets also allow you to have *very large* number of vertices, as these indices are limited to 16-bits and can
    overflow in unusual cases. This design is taken directly from the ImGui implementation, hence the need for indexing
    flexibility.

    .. warning::
       :func:`drawCalls` cannot be called within an active Rotation transformation (:func:`transformRotate`)
             
    `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#XPLMDrawCalls>`__ :index:`XPLMDrawCalls`

    See ``samples/PI_PGDrawCalls.py`` for a working example: it builds a texture
    with PIL, uploads one mesh, and dispatches it textured, untextured and
    scissor-clipped in a labelled grid.

Constants
---------

Stock simulator texture sources for :func:`textureSourceDrawIn` / :func:`textureSourceDrawMesh`.
These images are controlled by the aircraft where the range may be set differently for radar and radar2.

.. py:data:: Texture_WeatherRadar1
   :value: 0

   The first weather-radar texture source.
   `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#xplm_Texture_WeatherRadar1>`__: :index:`xplm_Texture_WeatherRadar1`

    .. image:: /images/panel_source_draw.png
               :width: 300px

.. py:data:: Texture_WeatherRadar2
   :value: 1

   The second weather-radar texture source.
   `Official SDK <https://developer.x-plane.com/sdk/XPLMPanelGraphics/#xplm_Texture_WeatherRadar2>`__: :index:`xplm_Texture_WeatherRadar2`

.. toctree::
   :hidden:

   /development/modules/strip_order

Types
-----

.. py:class:: XPLMTextureAtlasRef

    Opaque capsule representing a texture atlas, as returned by
    :func:`createTextureAtlas`. Release it with :func:`destroyTextureAtlas`.

.. py:class:: XPLMTexture

    Opaque capsule representing a texture, as returned by :func:`createTexture`.
    Release it with :func:`destroyTexture`.
