"""Graphics routines for X-Plane and OpenGL.

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


 XPLM Texture IDs name well-known textures in the sim for you to use. This
 allows you to recycle textures from X-Plane, saving VRAM.

   xplm_Tex_GeneralInterface
     - The bitmap that contains window outlines, button outlines, fonts, etc.

   xplm_Tex_AircraftPaint
     - The exterior paint for the user's aircraft (daytime).

   xplm_Tex_AircraftLiteMap
     - The exterior light map for the user's aircraft.



 X-Plane features some fixed-character fonts.  Each font may have its own
 metrics.

 WARNING: Some of these fonts are no longer supported or may have changed
 geometries. For maximum copmatibility, see the comments below.

 Note: X-Plane 7 supports proportional-spaced fonts.  Since no measuring
 routine is available yet, the SDK will normally draw using a fixed-width
 font.  You can use a dataref to enable proportional font drawing on XP7 if
 you want to.

   xplmFont_Basic
     - Mono-spaced font for user interface.  Available in all versions of the SDK.

   xplmFont_Proportional
     - Proportional UI font.

"""
###############################################################################


def XPLMSetGraphicsState(inEnableFog, inNumberTexUnits, inEnableLighting,
                         inEnableAlphaTesting, inEnableAlphaBlending,
                         inEnableDepthTesting, inEnableDepthWriting):
    """XPLMSetGraphicsState changes OpenGL's graphics state

      inEnableFog           - integer
      inNumberTexUnits      - integer
      inEnableLighting      - integer
      inEnableAlphaTesting  - integer
      inEnableAlphaBlending - integer
      inEnableDepthTesting  - integer
      inEnableDepthWriting  - integer

   XPLMSetGraphicsState changes OpenGL's graphics state in a number of ways:

   inEnableFog - enables or disables fog, equivalent to: glEnable(GL_FOG);

   inNumberTexUnits - enables or disables a number of multitexturing units. If
   the number is 0, 2d texturing is disabled entirely, as in
   glDisable(GL_TEXTURE_2D);  Otherwise, 2d texturing is enabled, and  a
   number of multitexturing units are enabled sequentially, starting  with
   unit 0, e.g. glActiveTextureARB(GL_TEXTURE0_ARB);  glEnable
   (GL_TEXTURE_2D);

   inEnableLighting - enables or disables OpenGL lighting, e.g.
   glEnable(GL_LIGHTING); glEnable(GL_LIGHT0);

   inEnableAlphaTesting - enables or disables the alpha test per pixel, e.g.
   glEnable(GL_ALPHA_TEST);

   inEnableAlphaBlending - enables or disables alpha blending per pixel, e.g.
   glEnable(GL_BLEND);

   inEnableDepthTesting - enables per pixel depth testing, as in
   glEnable(GL_DEPTH_TEST);

   inEnableDepthWriting - enables writing back of depth information to the
   depth bufffer, as in glDepthMask(GL_TRUE);

   The purpose of this function is to change OpenGL state while keeping
   X-Plane aware of the state changes; this keeps X-Plane from getting
   surprised by OGL state changes, and prevents X-Plane and plug-ins from
   having to set all state before all draws; XPLMSetGraphicsState internally
   skips calls to change state that is already properly enabled.

   X-Plane does not have a 'default' OGL state to plug-ins; plug-ins should
   totally set OGL state before drawing.  Use XPLMSetGraphicsState instead of
   any of the above OpenGL calls.

   WARNING: Any routine that performs drawing (e.g. XPLMDrawString or widget
   code) may change X-Plane's state.  Always set state before drawing after
   unknown code has executed.
    """


###############################################################################
def XPLMBindTexture2d(inTextureNum, inTextureUnit):
    """Changes currently bound texture

      inTextureNum  - integer
      inTextureUnit - integer

   XPLMBindTexture2d changes what texture is bound to the 2d texturing target.
   This routine caches the current 2d texture across all texturing units in
   the sim and plug-ins, preventing extraneous binding.  For example, consider
   several plug-ins running in series; if they all use the 'general interface'
   bitmap to do UI, calling this function will skip the rebinding of the
   general interface texture on all but the first plug-in, which can provide
   better frame rate son some graphics cards.

   inTextureID is the ID of the texture object to bind; inTextureUnit is a
   zero-based  texture unit (e.g. 0 for the first one), up to a maximum of 4
   units.  (This number may increase in future versions of x-plane.)

   Use this routine instead of glBindTexture(GL_TEXTURE_2D, ....);
   """


###############################################################################
def XPLMGenerateTextureNumbers(inCount):
    """Generate number of textures for a plugin.

      outTextureIDs - list
      inCount       - integer

   This routine generates unused texture numbers that a plug-in can use to
   safely bind textures. Use this routine instead of glGenTextures;
   glGenTextures will allocate texture numbers in ranges that X-Plane reserves
   for its own use but does not always use; for example, it might provide an
   ID within the range of textures reserved for terrain...loading a new .env
   file as the plane flies might then cause X-Plane to use this texture ID.
   X-Plane will then  overwrite the plug-ins texture.  This routine returns
   texture IDs that are out of X-Plane's usage range.
   """
    return list  # ints


###############################################################################
def XPLMWorldToLocal(inLatitude, inLongitude, inAltitude):
    """Convert Lat/Lon/Alt to local scene coordinates

   inLatitude  - float
   inLongitude - float
   inAltitude  - float

   This routine translates coordinates from latitude, longitude, and altitude
   to local scene coordinates. Latitude and longitude are in decimal degrees,
   and altitude is in meters MSL (mean sea level).  The XYZ coordinates are in
   meters in the local OpenGL coordinate system.
    """
    return (float, float, float)  # (outX, outY, outZ)


###############################################################################
def XPLMLocalToWorld(inX, inY, inZ):
    """Convert local scene coordinates to Lat/Lon/Alt

   inX - float
   inY - float
   inZ - float

   This routine translates a local coordinate triplet back into latitude,
   longitude, and altitude.  Latitude and longitude are in decimal degrees,
   and altitude is in meters MSL (mean sea level).  The XYZ coordinates are in
   meters in the local OpenGL coordinate system.

   NOTE: world coordinates are less precise than local coordinates; you should
   try to avoid round tripping from local to world and back.
    """
    return (float, float, float)  # (outLatitude, outLongitude, outAltitude)


###############################################################################
def XPLMDrawTranslucentDarkBox(inLeft, inTop, inRight, inBottom):
    """Draw translucent dark box

      inLeft   - integer
      inTop    - integer
      inRight  - integer
      inBottom - integer

   This routine draws a translucent dark box, partially obscuring parts of the
   screen but making text easy to read.  This is the same graphics primitive
   used by X-Plane to show text files and ATC info.
    """


###############################################################################
def XPLMDrawString(inColorRGB, inXoffset, inYOffset, inChar, inWordWrapWidth,
                   inFontID):
    """Draw a string

   inColorRGB      - list of three floats
   inXOffset       - integer
   inYOffset       - integer
   inChar          - string
   inWordWrapWidth - integer
   inFontID        - integer

   This routine draws a string in a given font.  Pass in the lower left pixel
   that the character is to be drawn onto. Also pass the character and font ID.
   The color to draw in is specified as a tuple of three floating point values,
   representing RGB intensities from 0.0 to 1.0.
    """


###############################################################################
def XPLMDrawNumber(inColorRGB, inXOffset, inYOffset, inValue, inDigits,
                   inDecimals, inShowSign, inFontID):
    """Draw a number

   inColorRGB      - list of three floats
   inXOffset       - integer
   inYOffset       - integer
   inValue         - float
   inDigits        - integer
   inDecimals      - integer
   inShowSign      - integer
   inFontID        - integer

   This routine draws a number similar to the digit editing fields in
   PlaneMaker and data output display in X-Plane.  Pass in a color, a
   position, a floating point value, and formatting info.  Specify how many
   integer and how many decimal digits to show and whether to show a sign, as
   well as a character set.
    """


###############################################################################
def XPLMGetFontDimensions(inFontID):
    """Retrieve a font info

   inFontID      - integer

   This routine returns the width and height of a character in a given font.
   It also tells you if the font only supports numeric digits.  Pass None if
   you don't need a given field.  Note that for a proportional font the width
   will be an arbitrary, hopefully average width.
   """
    return (int, int, int)  # (width, height, digitsOnly)


###############################################################################
def XPLMMeasureString(inFontID, inChar, inNumChars):
    """Return a width of a given string in a given font

      inFontID   - integer
      inChar     - string
      inNumChars - integer

   This routine returns the width in pixels of a string using a given font.
   The string is passed along with a length; this is used to allow for
   measuring substrings. The return value is floating point; it is
   possible that future font drawing may allow for fractional pixels.
    """
    return float  # width of string in (fractional) pixels

###############################################################################
# XPLM Texture IDs name well-known textures in the sim for you to use. This
# allows you to recycle textures from X-Plane, saving VRAM.
# The bitmap that contains window outlines, button outlines, fonts, etc.
xplm_Tex_GeneralInterface = 0

# The exterior paint for the user's aircraft (daytime).
xplm_Tex_AircraftPaint = 1

# The exterior light map for the user's aircraft.
xplm_Tex_AircraftLiteMap = 2


###############################################################################
# X-Plane features some fixed-character fonts.  Each font may have its own
# metrics.
#
# WARNING: Some of these fonts are no longer supported or may have changed
# geometries. For maximum copmatibility, see the comments below.
#
# Note: X-Plane 7 supports proportional-spaced fonts.  Since no measuring
# routine is available yet, the SDK will normally draw using a fixed-width
# font.  You can use a dataref to enable proportional font drawing on XP7 if
# you want to.

# Mono-spaced font for user interface.  Available in all versions of the SDK.
xplmFont_Basic = 0

# Proportional UI font.
xplmFont_Proportional = 18
