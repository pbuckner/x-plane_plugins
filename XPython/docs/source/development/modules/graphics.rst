XPLMGraphics
============
.. py:module:: XPLMGraphics

To use::

  import XPLMGraphics


A few notes on coordinate systems:

X-Plane uses three kinds of coordinates.  Global coordinates are specified
as latitude, longitude and elevation.  This coordinate system never changes
but is not very precise.

OpenGL (or 'local') coordinates are cartesian and shift with the plane.
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

Functions
---------

.. py:function:: XPLMSetGraphicsState(...) -> None:

 XPLMSetGraphicsState changes OpenGL's graphics state

 :param inEnableFog: enables or disables fog, equivalent to: ``glEnable(GL_FOG);``
 :type inEnableFong: int
 :param inNumberTexUnits: enables or disables a number of multitexturing units. If
   the number is 0, 2d texturing is disabled entirely, as in
   ``glDisable(GL_TEXTURE_2D);``  Otherwise, 2d texturing is enabled, and  a
   number of multitexturing units are enabled sequentially, starting  with
   unit 0, e.g. ``glActiveTextureARB(GL_TEXTURE0_ARB);  glEnable(GL_TEXTURE_2D);``
 :type inNumberTexUnits: int
 :param inEnableLighting: enables or disables OpenGL lighting, e.g.
   ``glEnable(GL_LIGHTING); glEnable(GL_LIGHT0);``
 :type inEnableLighting: int
 :param inEnableAlphaTesting: enables or disables the alpha test per pixel, e.g.
   ``glEnable(GL_ALPHA_TEST);``
 :type inEnableAlphaTesting: int
 :param inEnableAlphaBlending: enables or disables alpha blending per pixel, e.g.
   ``glEnable(GL_BLEND);``
 :type inEnableAlphaBlending: int
 :param inEnableDepthTesting: enables per pixel depth testing, as in
   ``glEnable(GL_DEPTH_TEST);``
 :type inEnableDepthTesting: int
 :param inEnableDepthWriting: enables writing back of depth information to the
   depth bufffer, as in ``glDepthMask(GL_TRUE);``
 :type inEnableDepthWriting: int

 The purpose of this function is to change OpenGL state while keeping
 X-Plane aware of the state changes; this keeps X-Plane from getting
 surprised by OGL state changes, and prevents X-Plane and plug-ins from
 having to set all state before all draws; :py:func:`XPLMSetGraphicsState` internally
 skips calls to change state that is already properly enabled.

 X-Plane does not have a 'default' OGL state to plug-ins; plug-ins should
 totally set OGL state before drawing.  Use :py:func:`XPLMSetGraphicsState` instead of
 any of the above OpenGL calls.

 .. warning:: Any routine that performs drawing (e.g. :py:func:`XPLMDrawString` or widget
   code) may change X-Plane's state.  Always set state before drawing after
   unknown code has executed.


.. py:function:: XPLMBindTexture2d(textureID, textureUnit) -> None:

 Changes currently bound texture

 :param int textureID: Texture object to be bound.
 :param int textureUnit: Zero-based texture unit.

 XPLMBindTexture2d changes what texture is bound to the 2d texturing target.
 This routine caches the current 2d texture across all texturing units in
 the sim and plug-ins, preventing extraneous binding.  For example, consider
 several plug-ins running in series; if they all use the 'general interface'
 bitmap to do UI, calling this function will skip the rebinding of the
 general interface texture on all but the first plug-in, which can provide
 better frame rate son some graphics cards.

 textureID is the ID of the texture object to bind; textureUnit is a
 zero-based  texture unit (e.g. 0 for the first one), up to a maximum of 4
 units.  (This number may increase in future versions of x-plane.)

 Use this routine instead of ``glBindTexture(GL_TEXTURE_2D, ....);``


.. py:function:: XPLMGenerateTextureNumbers(count) -> list:

 Generate number of textures for a plugin.

 :param int count: number of texture numbers to generate 
 :return: list of texture numbers
 :rtype: list of ints

 This routine generates unused texture numbers (ints) that a plug-in can use to
 safely bind textures.

 Use this routine instead of ``glGenTextures;``
 glGenTextures will allocate texture numbers in ranges that X-Plane reserves
 for its own use but does not always use; for example, it might provide an
 ID within the range of textures reserved for terrain...loading a new .env
 file as the plane flies might then cause X-Plane to use this texture ID.
 X-Plane will then  overwrite the plug-ins texture.

 This routine returns
 texture IDs that are out of X-Plane's usage range.


.. py:function:: XPLMWorldToLocal(latitude, longitude, altitude) -> (x, y, z):

 Convert Lat/Lon/Alt to local scene coordinates (x, y, z)

 :param float inLatitude: decimal degrees
 :param float inLongitude: decimal degrees
 :param float inAltitude: meters MSL (mean sea level)
 :return: (x, y, z) in meters, in the local OpenGL coordinate system                         
 :rtype: (float, float, float)                          

.. py:function:: XPLMLocalToWorld(x, y, z) -> (latitude, longitude, altitude):

 Convert local scene coordinates (x, y z) to Lat/Lon/Alt

 :param int x:
 :param int y:
 :param int z: position in local coordinates
 :return: (latitude, longitude, altitude)
 :rtype: (float, float, float)         

 This routine translates a local coordinate triplet back into latitude,
 longitude, and altitude.  Latitude and longitude are in decimal degrees,
 and altitude is in meters MSL (mean sea level).  The XYZ coordinates are in
 meters in the local OpenGL coordinate system.

 .. note:: World coordinates are less precise than local coordinates; you should
   try to avoid round tripping from local to world and back.


.. py:function:: XPLMDrawTranslucentDarkBox(left, top, right, bottom) -> None:

 Draw translucent dark box

 :param int left:
 :param int top:
 :param int right:   
 :param int bottom: outline of box to be drawn

 This routine draws a translucent dark box, partially obscuring parts of the
 screen but making text easy to read.  This is the same graphics primitive
 used by X-Plane to show text files and ATC info.


.. py:function:: XPLMDrawString(rgb, x, y, value, wordWrapWidth, fontID) -> None:

 Draw a string

 :param rgb: (r, g, b) each 0.0 -> 1.0
 :type rgb:  (float, float, float)
 :param int x: horizontal pixel offset                     
 :param int y: vertical pixel offset
 :param str value: string to be displayed
 :param int wordWrapWidth: width of "box" in which to wrap the text, in pixels. Can be None -- don't wrap.
 :param int fontID: :ref:`XPLMFontID`

 This routine draws a string in a given font (:py:data:`XPLMFontID`).
 Pass in the (x, y) lower left pixel
 that the character is to be drawn onto. Also pass the string and font ID.
 The color to draw in is specified as a tuple of three floating point values,
 representing RGB intensities from 0.0 to 1.0.

.. py:function:: XPLMDrawNumber(rgb, x, y, value, digits, decimals, showSign, fontID) -> None:

 Draw a number

 :param rgb: (r, g, b) each 0.0 -> 1.0
 :type rgb:  (float, float, float)
 :param int x: horizontal pixel offset                     
 :param int y: vertical pixel offset
 :param float value: value to be displayed
 :param int digits: number of integer digits to display (i.e., left of decimal point)
 :param int decimals: number of decimal digits to display (i.e., right of decimal point)                   
 :param int showSign: 1= show sign 
 :param int fontID: :ref:`XPLMFontID`

 This routine draws a number similar to the digit editing fields in
 PlaneMaker and data output display in X-Plane.  Pass in a color (r, g, b), a
 position (x, y), a floating point value, and formatting info.  Specify how many
 integer and how many decimal digits to show and whether to show a sign, as
 well as the font (:data:`XPLMFontID`).



.. py:function::  XPLMGetFontDimensions(fontID) -> (width, height, digitsOnly):

 Retrieve a font info

 :param int fontID: :ref:`XPLMFontID`
 :return: (width, height, digitsOnly)                    
 :rtype: (int, int, 1= font has only digits)

 This routine returns the width and height of a character in a given font.
 It also tells you if the font only supports numeric digits.  
 Note that for a proportional font the width will be an arbitrary, hopefully average width.

.. py:function:: XPLMMeasureString(fontID, string) -> width:

 Return a width of a given string in a given font.

 :param int fontID: :ref:`XPLMFontID`
 :param str string: String to be measured
 :return: width in fractional pixels
 :rtype: float         

 This routine returns the width in pixels of a string using a given font.
 The full length of the string is measured: if you need to measure
 a substring, pass only that substring to this function.
 The return value is floating point; it is
 possible that future font drawing may allow for fractional pixels.

 .. note:: A common use for this function is to determine the size of
           a widget in which to display the string. Note that widget sizes
           **require** integers, so you should cast or round the results of this function
           prior to use.


Constants
---------

.. _XPLMFontID:

XPLMFontID
**********

X-Plane features some fixed-character fonts.  Each font may have its own
metrics.

.. data:: xplmFont_Basic

 Mono-spaced font for user interface.  Available in all versions of the SDK.

.. data:: xplmFont_Proportional

 Proportional UI font.

