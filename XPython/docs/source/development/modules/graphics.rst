XPLMGraphics
============
.. py:module:: XPLMGraphics
.. py:currentmodule:: xp

To use::

  import xp


A few notes on coordinate systems:

X-Plane uses three kinds of coordinates.  Global coordinates are specified
as latitude, longitude and elevation.  This coordinate system never changes
but is not very precise.

OpenGL (or 'local') coordinates are Cartesian and shift with the plane.
They offer more precision and are used for 3-d OpenGL drawing.  The X axis
is aligned east-west with positive X meaning east.  The Y axis is aligned
straight up and down at the point 0,0,0 (but since the earth is round it is
not truly straight up and down at other points).  The Z axis is aligned
north-south at 0, 0, 0 with positive Z pointing south (but since the earth
is round it isn't exactly north-south as you move east or west of 0, 0, 0).
One unit is one meter and the point 0,0,0 is on the surface of the  earth
at sea level for some latitude and longitude picked by the sim such that
the  user's aircraft is reasonably nearby.

Cockpit coordinates are 2d, with the X axis horizontal and the Y axis
vertical. The point 0,0 is the bottom left and 1024,768 is the upper right
of the screen. This is true no matter what resolution the user's monitor is
in; when running in higher resolution, graphics will be scaled.

Use X-Plane's routines to convert between global and local coordinates.  Do
not attempt to do this conversion yourself; the precise 'roundness' of
X-Plane's  physics model may not match your own, and (to make things
weirder) the user can potentially customize the physics of the current
planet.

These functions are divided into three sections:

* :ref:`opengl-functions`\: Allowing you to set up and manage OpenGL state.

   | :py:func:`setGraphicsState`,
   | :py:func:`bindTexture2d`,
   | :py:func:`generateTextureNumbers`

* :ref:`coordinate_functions`\: Allowing you to convert to / from coordinate systems.

   | :py:func:`worldToLocal`,
   | :py:func:`localToWorld`

* :ref:`basic_draw`\: Allowing you to do basic drawing, without knowing OpenGL.

   | :py:func:`drawTranslucentDarkBox`,
   | :py:func:`drawString`,
   | :py:func:`drawNumber`,
   | :py:func:`getFontDimensions`,
   | :py:func:`measureString`

.. _opengl-functions:

OpenGL Functions
----------------

.. py:function:: setGraphicsState(fog=0, numberTexUnits=0, lighting=0, alphaTesting=0, alphaBlending=0, depthTesting=0, depthWriting=0)

 Changes OpenGL's graphics state. 

 The purpose of this function is to change OpenGL state while keeping
 X-Plane aware of the state changes; this keeps X-Plane from getting
 surprised by OGL state changes, and prevents X-Plane and plug-ins from
 having to set all state before all draws; :py:func:`setGraphicsState` internally
 skips calls to change state that is already properly enabled.

 X-Plane does not have a 'default' OGL state to plug-ins; plug-ins should
 totally set OGL state before drawing.  Use :py:func:`setGraphicsState` instead of
 any of the OpenGL ``glEnable`` / ``glDisable`` calls below.

 .. warning:: Any routine that performs drawing (e.g. :py:func:`XPLMDrawString` or widget
   code) may change X-Plane's state.  Always set state before drawing after
   unknown code has executed.

 *fog*:  enables or disables fog, equivalent to: ``glEnable(GL_FOG);``

 *numberTexUnits*: enables or disables a number of multitexturing units. If
 the number is 0, 2d texturing is disabled entirely, as in
 ``glDisable(GL_TEXTURE_2D);``  Otherwise, 2d texturing is enabled, and  a
 number of multitexturing units are enabled sequentially, starting  with
 unit 0, e.g. ``glActiveTextureARB(GL_TEXTURE0_ARB);  glEnable(GL_TEXTURE_2D);``

 *lighting*: enables or disables OpenGL lighting, e.g. ``glEnable(GL_LIGHTING); glEnable(GL_LIGHT0);``

 *alphaTesting*: enables or disables the alpha test per pixel:  ``glEnable(GL_ALPHA_TEST);``

 *alphaBlending*: enables or disables alpha blending per pixel:  ``glEnable(GL_BLEND);``

 *depthTesting*: enables per pixel depth testing, as in   ``glEnable(GL_DEPTH_TEST);``

 *depthWriting*: enables writing back of depth information to the depth bufffer, as in ``glDepthMask(GL_TRUE);``

 X-Plane's lighting and fog environment is significantly more complex than the fixed function pipeline
 can express; do not assume that lighting and fog state is a good approximation for 3-d drawing. Prefer to
 use :mod:`XPLMInstance` API to draw objects. Calls to ``setGraphicsState`` *should have no fog or lighting*.
 
 `Official SDK <https://developer.x-plane.com/sdk/XPLMGraphics/#XPLMSetGraphicsState>`__ :index:`XPLMSetGraphicsState`

.. py:function:: bindTexture2d(textureID, textureUnit)

 Changes currently bound texture.

 This routine caches the current 2d texture across all texturing units in
 the sim and plug-ins, preventing extraneous binding.  For example, consider
 several plug-ins running in series; if they all use the 'general interface'
 bitmap to do UI, calling this function will skip the rebinding of the
 general interface texture on all but the first plug-in, which can provide
 better frame rate son some graphics cards.

 *textureID* is the ID of the texture object to bind; *textureUnit* is a
 zero-based  texture unit (e.g. 0 for the first one), up to a maximum of 4
 units.  (This number may increase in future versions of x-plane.)

 Use this routine instead of ``glBindTexture(GL_TEXTURE_2D, ....);``

 `Official SDK <https://developer.x-plane.com/sdk/XPLMGraphics/#XPLMBindTexture2d>`__ :index:`XPLMBindTexture2d`

.. py:function:: generateTextureNumbers(count)

 Generate number of textures for a plugin.

 This routine generates unused texture numbers (ints) that a plug-in can use to
 safely bind textures.

 Use this routine instead of ``glGenTextures;``
 glGenTextures will allocate texture numbers in ranges that X-Plane reserves
 for its own use but does not always use; for example, it might provide an
 ID within the range of textures reserved for terrain...loading a new .env
 file as the plane flies might then cause X-Plane to use this texture ID.
 X-Plane will then  overwrite the plug-ins texture.

 This routine returns a list of integer
 texture IDs that are out of X-Plane's usage range.

 >>> tex_num = xp.generateTextureNumbers(1)[0]
 >>> xp.bindTexture2d(tex_num, 0)
 >>> GL.glDeleteTextures(1, tex_num)

 See demo code in plugin :doc:`PI_TextureDraw.py </development/samples>`

 `Official SDK <https://developer.x-plane.com/sdk/XPLMGraphics/#XPLMGenerateTextureNumbers>`__ :index:`XPLMGenerateTextureNumbers`

.. py:function:: getTexture(textureID)

 Return the OpenGL texture ID of an X-Plane texture based on a generic identifying code (:ref:`XPLMTextureID`). For example,
 you can get the texture for X-Plane's weather radar using the :data:`Tex_Radar_Pilot`. 
 
.. _coordinate_functions:

Coordinate Conversion Functions
-------------------------------

.. py:function:: worldToLocal(lat, lon, alt=0)

 Convert Lat/Lon/Alt to local scene coordinates (x, y, z)

 Latitude and longitude are in decimal degrees, *alt* is in meters MSL.

 Returns tuple of three floats (x, y, z) in meters, in the local OpenGL coordinate system.

 >>> lat = xp.getDatad(xp.findDataRef('sim/flightmodel/position/latitude'))
 >>> lon = xp.getDatad(xp.findDataRef('sim/flightmodel/position/longitude'))
 >>> alt = xp.getDatad(xp.findDataRef('sim/flightmodel/position/elevation'))
 >>> (lat, lon, alt)
 (47.463586666721014, -122.30775530395267, 122.93034338392317)
 >>> xp.worldToLocal(lat, lon, alt)
 (-23161.567539629053, 79.61748455422719, 4007.734676883242)
 >>> xp.localToWorld(*xp.worldToLocal(lat, lon, alt))
 (47.463586666721014, -122.30775530395267, 122.93034338392317

 `Official SDK <https://developer.x-plane.com/sdk/XPLMGraphics/#XPLMWorldToLocal>`__ :index:`XPLMWorldToLocal`

.. py:function:: localToWorld(x, y, z)

 Convert local scene coordinates (x, y, z) to Lat/Lon/Alt

 This routine translates a local coordinate triplet back into latitude,
 longitude, and altitude.  Latitude and longitude are in decimal degrees,
 and altitude is in meters MSL (mean sea level).  The XYZ coordinates are in
 meters in the local OpenGL coordinate system.

 Returns tuple of three floats (lat, lon, alt).

 .. note:: World coordinates are less precise than local coordinates; you should
   try to avoid round tripping from local to world and back.

 >>> x = xp.getDataf(xp.findDataRef('sim/graphics/view/view_x'))
 >>> y = xp.getDataf(xp.findDataRef('sim/graphics/view/view_y'))
 >>> z = xp.getDataf(xp.findDataRef('sim/graphics/view/view_z'))
 >>> (x, y, z)
 (-23161.3359375, 79.97235870361328, 4007.6552734375)
 >>> xp.localToWorld(x, y, z)
 (47.46358739027689, -122,3977522090232, 123.28432321269065)
 >>> xp.worldToLocal(*xp.localToWorld(x, y, z))
 (-23161.335585060915, 79.9723599828585, 4007.655274833594)
 
 `Official SDK <https://developer.x-plane.com/sdk/XPLMGraphics/#XPLMLocalToWorld>`__ :index:`XPLMLocalToWorld`

.. _basic_draw:

Basic Drawing Functions
-----------------------

Note :py:func:`drawTranslucentDarkBox`, :py:func:`drawString`, and :py:func:`drawNumber` must
be called within a draw callback: they *will not* work otherwise.

.. py:function:: drawTranslucentDarkBox(left, top, right, bottom)

 Draw translucent dark box at location (*left*, *top*, *right*, *bottom*)

 This routine draws a translucent dark box, partially obscuring parts of the
 screen but making text easy to read.  This is the same graphics primitive
 used by X-Plane to show text files and ATC info.

 >>> def MyDraw(phase, after, refCon):
 ...     width, height = xp.getScreenSize()
 ...     xp.drawTranslucentDarkBox(100, height - 100, 300, height - 200)
 ...
 >>> xp.registerDrawCallback(MyDraw)

 .. image:: /images/translucentDarkBox.png
 
 `Official SDK <https://developer.x-plane.com/sdk/XPLMGraphics/#XPLMDrawTranslucentDarkBox>`__ :index:`XPLMDrawTranslucentDarkBox`            


.. py:function:: drawString(rgb=white_tuple, x=0, y=0, \
                 value="", wordWrapWidth=None, fontID=Font_Proportional)

 Draw a string *value* at location (*x*, *y*) using given *fontID* (:ref:`XPLMFontID`).
 *wordWrapWidth* is either
 an integer width, or None: don't word-wrap. *rgb* defaults to the tuple ``(1.0, 1.0, 1.0)``
 which is white.
 
 (*x*, *y*) represents the lower-left pixel of the string to be written.

 >>> def MyDraw(phase, after, refCon):
 ...     xp.drawString((0, 1.0, 0), 10, 10, "Lower left")
 ...
 >>> xp.registerDrawCallback(MyDraw)

 .. image:: /images/drawString.png

 `Official SDK <https://developer.x-plane.com/sdk/XPLMGraphics/#XPLMDrawString>`__ :index:`XPLMDrawString`            

.. py:function:: drawNumber(rgb=white_tuple, x=0, y=0,\
                 value=0.0, digits=-1, decimals=0, showSign=1, fontID=Font_Proportional)

 This routine draws a number similar to the  data output display in X-Plane.
 Pass in a color (r, g, b) as *rgb* tuple,
 which defaults to the tuple ``(1.0, 1.0, 1.0)`` or white.
 Pass position (*x*, *y*), a floating point *value*, and formatting info.  Specify how many
 digits to show left of the decimal point (*digits*) and how many to show to the right (*decimals*)
 and *showSign* whether to show a sign, as well as the font (:data:`XPLMFontID`).

 If *digits* is -1, we'll calculate correct width for digits left of the decimal point.
 Note X-Plane interprets the formatting loosely:

    For value ``150.925``:

 .. table::
    :align: left

    +---------+------------------------------------+
    |         | Decimals  >>                       |
    +---------+----------+------+--------+---------+
    | Digits  | 0        | 1    |   2    | 3       |
    +---------+----------+------+--------+---------+
    |    -1   | 150      | 150.9| 150.93 | 150.925 |
    +---------+----------+------+--------+---------+
    |    0    | 1.5x2    | 150.9| 150.93 | 150.925 |
    +---------+----------+------+--------+---------+
    |    1    | 1.5x2    | 150.9| 150.93 | 150.925 |
    +---------+----------+------+--------+---------+
    |    2    | 1.5x2    | 150.9| 150.93 | 150.925 |
    +---------+----------+------+--------+---------+
    |    3    | 150      | 150.9| 150.93 | 150.925 |
    +---------+----------+------+--------+---------+
    |    4    | 150      | 150.9| 150.93 | 150.925 |
    +---------+----------+------+--------+---------+


 and ``decimals=10, digits=10`` results in displaying ``-1.0737418240``...

 >>> def MyDraw(phase, after, refCon):
 ...     xp.drawNumber((1, .25, .75), 10, 10, 15.65, decimals=1)
 ...
 >>> xp.registerDrawCallback(MyDraw)

 .. image:: /images/drawNumber.png
            
.. py:function::  getFontDimensions(fontID)

 Retrieve a font info, for *fontID*. (:ref:`XPLMFontID`)

 Returns tuple (width, height, digitsOnly) where digitsOnly is 1 if font only supports digits.

 This routine returns the width and height of a character in a given font.
 It also tells you if the font only supports numeric digits.  
 Note that for a proportional font the width will be an arbitrary, hopefully average width.

 >>> xp.getFontDimensions(xp.Font_Basic)
 (6, 10, 0)
 >>> xp.getFontDimensions(xp.Font_Proportional)
 (10, 10, 0)
 
 `Official SDK <https://developer.x-plane.com/sdk/XPLMGraphics/#XPLMGetFontDimensions>`__ :index:`XPLMGetFontDimensions`

.. py:function:: measureString(fontID, string)

 Return a width of a given *string* in a given font *fontID* (:ref:`XPLMFontID`).

 The returned width is *fractional pixels*.
 
 The full length of the string is measured: if you need to measure
 a substring, pass only that substring to this function.
 The return value is floating point; it is
 possible that future font drawing may allow for fractional pixels.

 .. note:: A common use for this function is to determine the size of
      a widget in which to display the string. Note that widget sizes
      **require** integers, so you should cast or round the results of this function
      prior to use.

 >>> xp.measureString(xp.Font_Basic, "Hello World")
 66.0
 >>> xp.measureString(xp.Font_Proportional, "Hello World")
 65.0

 `Official SDK <https://developer.x-plane.com/sdk/XPLMGraphics/#XPLMMeasureString>`__ :index:`XPLMMeasureString`


Constants
---------

.. _XPLMFontID:

XPLMFontID
**********

.. image:: /images/font_example.png

X-Plane features some fixed-character fonts.  Each font may have its own
metrics.

.. data:: Font_Basic
 :value: 0


 Mono-spaced font for user interface.  Available in all versions of the SDK.

 `Official SDK <https://developer.x-plane.com/sdk/XPLMGraphics/#xplm_Font_Basic>`__ :index:`xplm_Font_Basic`         

.. data:: Font_Proportional
 :value: 18

 Proportional UI font.

 `Official SDK <https://developer.x-plane.com/sdk/XPLMGraphics/#xplm_Font_Proportional>`__ :index:`xplm_Font_Proportional`         

.. _XPLMTextureID:

XPLMTextureID
*************

Predefined texture bitmaps for use with :func:`getTexture`.

+-----------------------------------------+---------------------------------------+
|.. data:: Tex_GeneralInterface           | Window outlines, button outlines,     |
| :value: 0                               | fonts, etc.                           |
+-----------------------------------------+---------------------------------------+
|.. data:: Tex_Radar_Pilot                | Weather radar instrument texture as   |
| :value: 3                               | controlled by the pilot-side radar    |
|                                         | controls.                             |
+-----------------------------------------+---------------------------------------+
|.. data:: Tex_Radar_Copilot              | Weather radar instrument texture as   |
| :value: 4                               | controlled by the copilot-side radar  |
|                                         | controls                              |
+-----------------------------------------+---------------------------------------+

 `Official SDK <https://developer.x-plane.com/sdk/XPLMGraphics/#XPLMTextureID>`__ :index:`XPLMTextureID`         
