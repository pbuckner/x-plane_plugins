# pylint: disable = unused-argument, redefined-builtin
# (a stub's parameters are never used -- there is no body; 'map' is the C
#  keyword name for the mapDisplay* functions and must not be renamed)
from typing import Any, Callable, Optional, Sequence, Union
from XPPython3.xp_typing import (XPLMAvionicsID, XPLMCommandRef, XPLMEGPWSStyle,
                                 XPLMFontHandle, XPLMMapDisplayRef, XPLMRetainedDrawing,
                                 XPLMSVTDisplayRef, XPLMTexture, XPLMTextureAtlasRef,
                                 XPLMWindowID)
SVT_Terrain: int
SVT_Runways: int
SVT_Obstacles: int
SVT_FlightPath: int
SVT_Traffic: int
SVT_AirportSigns: int
SVT_ILSHoops: int
SVT_HorizonHeading: int
SVT_All: int
Map_Nexrad: int
Map_IR: int
Map_Topo: int
Map_Terrain: int
Map_Water: int
Map_EGPWS: int
Map_raw_elev: int
Map_safe_taxi: int
EGPWS_Style_Blocky: XPLMEGPWSStyle
EGPWS_Style_Smooth: XPLMEGPWSStyle
Texture_WeatherRadar1: int
Texture_WeatherRadar2: int
TouchZone_Nothing: int
TouchZone_Command: int
TouchZone_Identifier: int
LineCapButt: int = 0
LineCapRound: int
LineCapSquare: int
CharSetDigits: int
CharSetASCII: int
CharSetUnicode: int
JustLeft: int
JustCenter: int
JustRight: int


def makeColor(red: float, green: float, blue: float, alpha: float) -> int:
    """
    Pack four float color components (each 0.0-1.0, clamped) into a single
    packed color value (ABGR) for use with the panel graphics routines.
    """
    ...


def setLineCap(lineCap: int = LineCapButt) -> None:
    """
    Set how subsequent lines are capped at their start and end points.

    lineCap is LineCapButt (straight edge at the endpoint), LineCapRound
    (half circle centered on the endpoint), or LineCapSquare (half square
    centered on the endpoint). Called with no argument, it restores the
    default LineCapButt, which is also in effect at the start of each
    drawing callback.
    """
    ...


def lines(color: int, vertices: Sequence[tuple[float, float]]) -> None:
    """
    Draw disconnected line segments. Each consecutive pair of vertices forms
    one segment.
    """
    ...


def linesWithWidth(color: int, lineWidth: float, vertices: Sequence[tuple[float, float]]) -> None:
    """
    Draw disconnected line segments with the given line width.
    """
    ...


def linesStipple(color: int, vertices: Sequence[tuple[float, float]], dashLength: float, lineWidth: float) -> None:
    """
    Draw disconnected dashed line segments.
    """
    ...


def linesc(vertices: Sequence[tuple[float, float, int]]) -> None:
    """
    Draw disconnected line segments with per-vertex colors.
    """
    ...


def linescWithWidth(lineWidth: float, vertices: Sequence[tuple[float, float, int]]) -> None:
    """
    Draw disconnected line segments with per-vertex colors and line width.
    """
    ...


def lineStrip(color: int, vertices: Sequence[tuple[float, float]]) -> None:
    """
    Draw a connected line strip; the last vertex is not closed back.
    """
    ...


def lineStripWithWidth(color: int, lineWidth: float, vertices: Sequence[tuple[float, float]]) -> None:
    """
    Draw a connected line strip with the given line width.
    """
    ...


def lineStripc(vertices: Sequence[tuple[float, float, int]]) -> None:
    """
    Draw a connected line strip with per-vertex colors.
    """
    ...


def lineStripcWithWidth(lineWidth: float, vertices: Sequence[tuple[float, float, int]]) -> None:
    """
    Draw a connected line strip with per-vertex colors and line width.
    """
    ...


def lineStripStipple(color: int, vertices: Sequence[tuple[float, float]], dashLength: float, lineWidth: float) -> None:
    """
    Draw a connected dashed line strip.
    """
    ...


def lineLoop(color: int, vertices: Sequence[tuple[float, float]]) -> None:
    """
    Draw a closed line loop; the last vertex connects back to the first.
    """
    ...


def lineLoopWithWidth(color: int, lineWidth: float, vertices: Sequence[tuple[float, float]]) -> None:
    """
    Draw a closed line loop with the given line width.
    """
    ...


def lineLoopc(vertices: Sequence[tuple[float, float, int]]) -> None:
    """
    Draw a closed line loop with per-vertex colors.
    """
    ...


def lineLoopcWithWidth(lineWidth: float, vertices: Sequence[tuple[float, float, int]]) -> None:
    """
    Draw a closed line loop with per-vertex colors and line width.
    """
    ...


def lineLoopStipple(color: int, vertices: Sequence[tuple[float, float]], dashLength: float, lineWidth: float) -> None:
    """
    Draw a closed dashed line loop.
    """
    ...


def polygon(color: int, vertices: Sequence[tuple[float, float]]) -> None:
    """
    Draw a filled convex polygon (at least 3 vertices).
    """
    ...


def polygonc(vertices: Sequence[tuple[float, float, int]]) -> None:
    """
    Draw a filled convex polygon with per-vertex colors.
    """
    ...


def quadstrip(color: int, vertices: Sequence[tuple[float, float]]) -> None:
    """
    Draw a series of connected filled quadrilaterals (count even, >= 4).
    """
    ...


def quadstripc(vertices: Sequence[tuple[float, float, int]]) -> None:
    """
    Draw a quad strip with per-vertex colors.
    """
    ...


def transformPush() -> None:
    """
    Save the current transformation matrix onto the transform stack.
    """
    ...


def transformPop() -> None:
    """
    Restore the transformation matrix from the top of the transform stack.
    """
    ...


def transformTranslate(dx: float, dy: float) -> None:
    """
    Offset all subsequent drawing by (dx, dy) pixels.
    """
    ...


def transformRotate(centerX: float, centerY: float, angle: float) -> None:
    """
    Rotate subsequent drawing around (centerX, centerY) by angle degrees.
    """
    ...


def transformScale(scaleX: float, scaleY: float) -> None:
    """
    Scale subsequent drawing by (scaleX, scaleY) relative to the origin.
    """
    ...


def scissorPush() -> None:
    """
    Save the current scissor rectangle onto the scissor stack.
    """
    ...


def scissorPop() -> None:
    """
    Restore the scissor rectangle from the top of the scissor stack.
    """
    ...


def scissorSet(left: int, top: int, right: int, bottom: int) -> None:
    """
    Set an absolute scissor rectangle; only pixels inside are drawn.
    """
    ...


def scissorIntersect(left: int, top: int, right: int, bottom: int) -> None:
    """
    Intersect the current scissor box with the absolute rectangle given by edges (left, top, right, bottom) in panel pixels; the result is their overlap (drawable area only shrinks).
    """
    ...


def beginSetupStencilMask(bits: int, mask: int) -> None:
    """
    Begin stencil mask setup; subsequent drawing writes to the stencil buffer.
    """
    ...


def endSetupStencilMask() -> None:
    """
    End stencil mask setup; subsequent drawing renders to the screen again.
    """
    ...


def useStencilMask(bits: int, mask: int) -> None:
    """
    Activate stencil testing for subsequent drawing.
    """
    ...


def clearStencilMask() -> None:
    """
    Clear the stencil buffer and disable stencil testing.
    """
    ...

# ---- Synthetic Vision (SVT) display (XPLM440) ----


def createSVTDisplay(pilotIndex: int = 0, pixelsPerDegree: float = 14.0) -> XPLMSVTDisplayRef:
    """
    Create a Synthetic Vision (SVT) display that renders a 3-D perspective view
    of terrain, runways, and optional overlays into an avionics panel.
    pilotIndex is 0 for pilot-side AHRS, 1 for copilot. pixelsPerDegree is the
    vertical scale of the 3-d view at the center of the display; must be > 0,
    the G1000 PFD uses 14. Which layers are drawn is chosen per draw call.
    Draw it with svtDisplayDrawIn() and free it with destroySVTDisplay().
    """
    ...


def destroySVTDisplay(svt: XPLMSVTDisplayRef) -> None:
    """
    Destroy an SVT display created with createSVTDisplay() and free its resources.
    """
    ...


def svtDisplayDrawIn(svt: XPLMSVTDisplayRef, features: int, left: int, top: int, right: int, bottom: int,
                     dataOverrides: Optional[Sequence[float]] = None) -> None:
    """
    Render the SVT display into the panel within (left, top, right, bottom). Must
    be called from an avionics drawing callback; does nothing until terrain tiles
    finish loading. features is a bitwise OR of SVT_* flags for this draw.
    dataOverrides is None for live sim state, or a sequence of 9 values:
    (pitchDeg, rollDeg, headingMagDeg, magVarDeg, indicatedAltFt, baroSettingInHg,
    hsiSource, hdefDots, vdefDots).
    """
    ...

# ---- base map display (XPLM440) ----


def createMapDisplay(pilotIndex: int = 0) -> XPLMMapDisplayRef:
    """
    Create a base map display (ND/MFD background) that renders terrain, topography,
    water, weather and taxi layouts. pilotIndex is 0 for pilot-side GPS position,
    1 for copilot. Draw it with mapDisplayDrawIn() and free it with destroyMapDisplay().
    """
    ...


def destroyMapDisplay(map: XPLMMapDisplayRef) -> None:
    """
    Destroy a map display created with createMapDisplay() and free its resources.
    """
    ...


def mapDisplayDrawIn(map: XPLMMapDisplayRef, layers: int, left: int, top: int, right: int, bottom: int,
                     dataOverrides: Optional[Sequence[float]] = None) -> None:
    """
    Render the base map into the panel within (left, top, right, bottom). Must be
    called from an avionics drawing callback; does nothing until terrain tiles
    finish loading. layers is a bitwise OR of Map_* flags (some are mutually
    exclusive -- e.g. Map_Nexrad with Map_EGPWS or Map_IR). dataOverrides is None
    for live sim state, or a sequence of 12 or 15 values: (datLat, datLon, ctrX,
    ctrY, roseRadius, mapRange, orientation, terrainWarn, terrainCaution, acfAlt,
    gearDown, trueRotation[, nearestRwyElev, egpwsBrightness, egpwsStyle]). The last
    three (EGPWS) are XPLM440 additions; omit them (pass 12) to leave them at 0.
    """
    ...


def mapDisplayProject(map: XPLMMapDisplayRef, layers: int, left: int, top: int, right: int, bottom: int,
                      latitude: float, longitude: float,
                      dataOverrides: Optional[Sequence[float]] = None) -> tuple[float, float] | None:
    """
    Convert a latitude/longitude into an (x, y) position in panel coordinates, for
    the map described by these arguments. Inverse of mapDisplayUnproject(). Pass the
    SAME arguments you draw the map with and you get the projection that draw call
    produces. Need not be called from a drawing callback. Returns None if terrain
    tiles have not loaded or the point is not on this map.
    """
    ...


def mapDisplayUnproject(map: XPLMMapDisplayRef, layers: int, left: int, top: int, right: int, bottom: int,
                        x: float, y: float,
                        dataOverrides: Optional[Sequence[float]] = None) -> tuple[float, float] | None:
    """
    Convert an (x, y) position in panel coordinates back into a (latitude, longitude),
    for the map described by these arguments. Inverse of mapDisplayProject(). Need not
    be called from a drawing callback. Returns None if terrain tiles have not loaded or
    the point does not correspond to anywhere on the earth.
    """
    ...


def mapDisplayScaleMeter(map: XPLMMapDisplayRef, layers: int, left: int, top: int, right: int, bottom: int,
                         x: float, y: float,
                         dataOverrides: Optional[Sequence[float]] = None) -> float:
    """
    Return how many pixels correspond to one meter at (x, y) on the map described by
    these arguments. Use it to size symbols and range rings so they stay correct as the
    range changes. Returns 0.0 if terrain tiles have not loaded yet.
    """
    ...


def mapDisplayGetNorthHeading(map: XPLMMapDisplayRef, layers: int, left: int, top: int, right: int, bottom: int,
                              x: float, y: float,
                              dataOverrides: Optional[Sequence[float]] = None) -> float:
    """
    Return the heading, in degrees clockwise from straight up on the display, at which
    true north lies at (x, y). ADD it to a true heading to get the angle to draw that
    heading at. Accounts for the map's own rotation and for projection convergence.
    Returns 0.0 if terrain tiles have not loaded yet.
    """
    ...


def mapDisplayGetTerrainAltitudes(map: XPLMMapDisplayRef) -> tuple[float, float] | None:
    """
    Return (minAltitude, maxAltitude) -- the lowest and highest altitude (feet)
    shown on the map's EGPWS terrain display. Altitudes are only available if the
    map was drawn with the Map_EGPWS layer; if not, returns None. Must be called
    from an avionics drawing callback.
    """
    ...

# ---- texture atlas (XPLM440) ----


def createTextureAtlas() -> XPLMTextureAtlasRef:
    """
    Create a new, empty texture atlas. Add images with textureAtlasAddImageFile() /
    textureAtlasAddImage() and their *Set() variants, then call textureAtlasBake()
    before drawing. Destroy with destroyTextureAtlas() when done.
    """
    ...


def destroyTextureAtlas(atlas: XPLMTextureAtlasRef) -> None:
    """
    Destroy a texture atlas created with createTextureAtlas() and free all
    associated GPU and CPU resources.
    """
    ...


def textureAtlasAddImageFile(atlas: XPLMTextureAtlasRef, imageFilePath: str) -> int:
    """
    Load a PNG file and add it to the atlas as a single image. Call before
    textureAtlasBake(). Returns the zero-based image index assigned.
    """
    ...


def textureAtlasAddImageFileSet(atlas: XPLMTextureAtlasRef, imageFilePath: str, cellsX: int, cellsY: int) -> int:
    """
    Load a PNG file and subdivide it into a cellsX x cellsY grid, adding each cell
    as a separate image (useful for sprite sheets). Returns the index of the first
    (top-left) cell; cell (x, y) is index + y * cellsX + x.
    """
    ...


def textureAtlasAddImage(atlas: XPLMTextureAtlasRef, image: bytes, width: int, height: int) -> int:
    """
    Add a single image from raw RGBA pixel data (4 bytes/pixel, rows top to bottom)
    to the atlas. image must hold at least width * height * 4 bytes. Returns the
    zero-based image index assigned.
    """
    ...


def textureAtlasAddImageSet(atlas: XPLMTextureAtlasRef, image: bytes, width: int, height: int, cellsX: int, cellsY: int) -> int:
    """
    Add raw RGBA pixel data (4 bytes/pixel, rows top to bottom) to the atlas,
    subdividing it into a cellsX x cellsY grid; each cell becomes a separate image.
    image must hold at least width * height * 4 bytes. Returns the index of the
    first cell; cell (x, y) is index + y * cellsX + x.
    """
    ...


def textureAtlasBake(atlas: XPLMTextureAtlasRef) -> None:
    """
    Pack all previously added images into a GPU texture. Must be called after
    adding all images and before any draw calls. Once baked, no more images may
    be added.
    """
    ...


def textureAtlasGetImageWidth(atlas: XPLMTextureAtlasRef, imageIndex: int) -> int:
    """
    Return the width in pixels of a single image (or cell) in the atlas.
    """
    ...


def textureAtlasGetImageHeight(atlas: XPLMTextureAtlasRef, imageIndex: int) -> int:
    """
    Return the height in pixels of a single image (or cell) in the atlas.
    """
    ...


def textureAtlasDrawAt(atlas: XPLMTextureAtlasRef, imageIndex: int, tintColor: int, x: float, y: float) -> None:
    """
    Draw an atlas image at its native resolution with its top-left corner at (x, y).
    tintColor is multiplied with the texture; use makeColor(1, 1, 1, 1) for no tint.
    """
    ...


def textureAtlasDrawIn(atlas: XPLMTextureAtlasRef, imageIndex: int, tintColor: int, left: float, top: float, right: float, bottom: float) -> None:
    """
    Draw an atlas image scaled to fill the rectangle (left, top, right, bottom),
    stretching or compressing to match. tintColor is multiplied with the texture.
    """
    ...


def textureAtlasDrawStretched(atlas: XPLMTextureAtlasRef, imageIndex: int, tintColor: int, left: float, top: float, right: float, bottom: float) -> None:
    """
    Draw an atlas image into the rectangle (left, top, right, bottom) using 9-slice
    scaling: the four corners keep their native size, edges stretch along one axis,
    and the center stretches in both. Preserves borders when scaling UI elements.
    tintColor is multiplied with the texture.
    """
    ...


def textureAtlasDrawScaled(atlas: XPLMTextureAtlasRef, imageIndex: int, tintColor: int, xPanel: float, yPanel: float, xAtlas: float, yAtlas: float, xScale: float, yScale: float, rotateCW: float) -> None:
    """
    Draw an atlas image with arbitrary scaling, rotation, and positioning. The
    atlas-space pivot (xAtlas, yAtlas), in pixels from the image's bottom-left, is
    aligned to the panel-space point (xPanel, yPanel), then scaled by (xScale,
    yScale) and rotated rotateCW degrees clockwise about that pivot. tintColor is
    multiplied with the texture.
    """
    ...


def textureAtlasDrawMesh(atlas: XPLMTextureAtlasRef, imageIndex: int, tintColor: int, vertices: Sequence[tuple[float, float, float, float]]) -> None:
    """
    Draw an atlas image onto an arbitrary triangle-strip mesh. Each vertex is an
    (x, y, s, t) tuple: (x, y) panel-space position in pixels, (s, t) normalized
    texture coordinate (0.0-1.0) within the image. At least 3 vertices are required.
    tintColor is multiplied with the texture.
    """
    ...

# ---- texture source (stock simulator textures, XPLM440) ----


def textureSourceDrawIn(tex: int, tintColor: int, left: int, top: int, right: int, bottom: int) -> None:
    """
    Draw a stock simulator texture source (e.g. Texture_WeatherRadar1), scaled to
    fill the rectangle (left, top, right, bottom) in panel coordinates. Unlike a
    texture atlas, a texture source is a live texture the simulator renders each
    frame. tintColor is multiplied with the texture; use makeColor(1, 1, 1, 1) for
    no tinting. If the aircraft lacks the requested hardware, the call is skipped.
    """
    ...


def textureSourceDrawMesh(tex: int, tintColor: int, vertices: Sequence[tuple[float, float, float, float]]) -> None:
    """
    Draw a stock simulator texture source onto an arbitrary triangle-strip mesh.
    Each vertex is an (x, y, s, t) tuple: (x, y) panel-space position in pixels,
    (s, t) normalized texture coordinate (0.0-1.0) within the source. At least 3
    vertices are required. tintColor is multiplied with the texture.
    """
    ...

# ---- Dear ImGui-style textured draw calls (XPLM440) ----


def createTexture(rgba: bytes, width: int, height: int) -> XPLMTexture:
    """
    Upload an RGBA8 image (4 bytes/pixel, rows top to bottom; rgba must hold at
    least width * height * 4 bytes) to the GPU and return an opaque texture handle
    for use as the tex of a draw call passed to drawCalls(). Free it with
    destroyTexture(). The sampler is bilinear, clamp-to-edge, no mipmaps.
    """
    ...


def destroyTexture(tex: XPLMTexture) -> None:
    """
    Free a texture created with createTexture(). The handle must not be used after
    this call. It is safe to create and destroy textures every frame.
    """
    ...


def drawCalls(vertices: Union[bytes, Sequence[tuple[float, ...]]],
              indices: Union[bytes, Sequence[int]], drawCalls: Sequence,
              indexSize: int = 2) -> None:
    """
    Render textured indexed-triangle draw calls matching Dear ImGui's ImDrawData
    layout. vertices is a bytes buffer of 20-byte vertices: pos.x, pos.y, uv.x,
    uv.y as float32, then RGBA8 packed as a little-endian uint32. indices is a
    bytes buffer of uint16 indices.

    For convenience, vertices may instead be a sequence of (x, y, u, v, color)
    tuples -- color being a makeColor() value -- and indices a sequence of ints;
    we pack them for you. color may be omitted -- (x, y, u, v) takes opaque white,
    that is, no tint -- but x, y, u and v are always required. Pass bytes when you
    already have them (an ImGui frame, or a mesh you build once and reuse):
    packing here costs a copy per frame.

    indexSize is the width in bytes of each index in the indices BUFFER: 2 (the
    default, what the SDK takes) or 4. Pass 4 when Dear ImGui was built with 32-bit
    ImDrawIdx -- we narrow to uint16 here, which is far cheaper than doing it in
    Python. An index above 65535 raises ValueError; split the mesh across several
    draw calls using vtx_offset. indexSize is ignored when indices is a sequence of
    ints rather than a buffer.

    drawCalls is a sequence of (tex, scissors,
    idx_offset, element_count, vtx_offset): tex is a createTexture() handle or None;
    scissors is (left, top, right, bottom) floats in window-local top-left coords;
    element_count must be a multiple of 3. Call only from a panel-graphics window
    draw callback; the host flips Y for you.
    """
    ...

# ---- touch zones (XPLM440) ----


def accumulateTouchZone(type: int, left: int, top: int, right: int, bottom: int, command: Optional[XPLMCommandRef] = None, identifier: int = 0) -> int:
    """
    Register an interactive touch zone for the current frame. Call every frame from
    your avionics drawing callback for each region. type is one of TouchZone_Nothing,
    TouchZone_Command (fires command on press/release), or TouchZone_Identifier
    (delivers events to the handler set with avionicsSetTouchEventHandler /
    windowSetTouchEventHandler, tagged with identifier). Returns True while the zone
    is being pressed/held. Zones registered later win overlaps.
    """
    ...


def avionicsSetTouchEventHandler(avionic: XPLMAvionicsID, handler: Optional[Callable], refCon: Any = None) -> None:
    """
    Register handler to receive touch events for TouchZone_Identifier zones on the
    given avionics device. handler is called as handler(identifier, status, x, y,
    dx, dy, button, refcon). Pass handler=None to remove the current handler.
    """
    ...


def windowSetTouchEventHandler(window: XPLMWindowID, handler: Optional[Callable], refCon: Any = None) -> None:
    """
    Register handler to receive touch events for TouchZone_Identifier zones on the
    given window. handler is called as handler(identifier, status, x, y, dx, dy,
    button, refcon). Pass handler=None to remove the current handler.
    """
    ...

# ---- constants (XPLM440) ----


# ---- fonts ----


def createFont(charset: int) -> XPLMFontHandle:
    """
    Create a new font handle for the given character set (CharSetDigits,
    CharSetASCII, CharSetUnicode). Add faces with fontAddFace() before drawing.
    """
    ...


def destroyFont(font: XPLMFontHandle) -> None:
    """
    Destroy a font handle created with createFont() and free its resources.
    """
    ...


def fontAddFace(font: XPLMFontHandle, ttf_path: str) -> int:
    """
    Add a TrueType (.ttf/.otf) face to a font handle. Multiple faces provide
    fallback glyphs, searched in the order added.
    """
    ...


def fontGetMetrics(font: XPLMFontHandle, fontSize: float) -> tuple[float, float, float]:
    """
    Return (lineHeight, lineAscent, lineDescent) in pixels for the font at size.
    """
    ...


def fontMeasureString(font: XPLMFontHandle, fontSize: float, string: str) -> float:
    """
    Return the width in pixels the string would occupy if drawn (not drawn).
    """
    ...


def fontGetLineCount(font: XPLMFontHandle, fontSize: float, string: str, width: float) -> int:
    """
    Return how many lines the string would occupy if word-wrapped to width.
    """
    ...


def fontFitForward(font: XPLMFontHandle, fontSize: float, string: str, width: float) -> int:
    """
    Return the number of characters from the start of the string that fit within
    width, measured left to right.
    """
    ...


def fontFitReverse(font: XPLMFontHandle, fontSize: float, string: str, width: float) -> int:
    """
    Return the number of leading characters to skip so the remaining tail
    (string[index:]) fits within width, measured right to left. 0 = whole string
    fits; len(string) = nothing fits.
    """
    ...


def fontDrawString(font: XPLMFontHandle, color: int, fontSize: float, x: float, y: float, string: str, justification: int) -> None:
    """
    Draw a string at (x, y) baseline anchor with the given font, size, packed
    color, and justification (JustLeft, JustCenter, JustRight).
    """
    ...


def fontDrawStringFixedSpacing(font: XPLMFontHandle, color: int, fontSize: float, x: float, y: float, string: str, fixedSpacing: int, justification: int) -> None:
    """
    Draw a string using fixed per-character spacing (pixels) instead of the font's
    natural proportional spacing.
    """
    ...


def fontDrawStringWordWrapped(font: XPLMFontHandle, color: int, fontSize: float, x: float, y: float, string: str, wrapWidth: int, justification: int) -> None:
    """
    Draw a string with automatic word wrapping at wrapWidth pixels.
    """
    ...


def fontDrawStringRotated(font: XPLMFontHandle, color: int, fontSize: float, x: float, y: float, string: str, angle: float, justification: int) -> None:
    """
    Draw a string rotated by angle degrees (positive clockwise) around the (x, y)
    anchor point.
    """
    ...


def beginRetainedDrawing() -> None:
    """
    Begin recording panel-graphics commands into a retained drawing. All
    panel-graphics calls made until endRetainedDrawing() are captured instead
    of being drawn immediately. Recording sessions must not be nested.
    """
    ...


def endRetainedDrawing() -> XPLMRetainedDrawing:
    """
    End the recording started by beginRetainedDrawing() and return an opaque
    handle to the captured commands. Replay it with drawRetained() and free it
    with destroyRetainedDrawing(). If a font or texture atlas used during
    recording is destroyed, you must destroy the retained drawing as well --
    replaying it afterwards references invalid resources.
    """
    ...


def drawRetained(drawing: XPLMRetainedDrawing) -> None:
    """
    Replay a retained drawing captured with endRetainedDrawing(). May be
    called any number of times per frame and across frames to redraw the same
    content cheaply.
    """
    ...


def destroyRetainedDrawing(drawing: XPLMRetainedDrawing) -> None:
    """
    Destroy a retained drawing captured with endRetainedDrawing() and free its
    resources. The handle must not be used after this call.
    """
    ...
