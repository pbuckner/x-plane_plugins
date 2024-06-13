from OpenGL import error
import OpenGL.GL as GL

# drawBezierLineQ
# drawWideBezierLineQ
# drawBezierLineQAdaptive()
# drawWideBezierLineQAdaptive()
# drawBezierLineC
# drawWideBezierLineC
# drawBezierLineCAdaptive()
# drawWideBezierLineCAdaptive()


def clear() -> None:
    GL.glClearColor(0, 0, 0, 1)
    GL.glClear(GL.GL_COLOR_BUFFER_BIT | GL.GL_DEPTH_BUFFER_BIT)


def report() -> None:
    print(f"Vendor: {GL.glGetString(GL.GL_VENDOR)}")
    print(f"Version: {GL.glGetString(GL.GL_VERSION)}")
    try:
        print(f"Version: {GL.glGetIntegerv(GL.GL_MAJOR_VERSION)} {GL.glGetIntegerv(GL.GL_MINOR_VERSION)}")
    except error.GLError:
        print("Version < 3.0")
    #
    print(f"Extentions: {GL.glGetString(GL.GL_EXTENSIONS)}")
    print(f"GL_ALIASED_LINE_WIDTH_RANGE: {GL.glGetFloatv(GL.GL_ALIASED_LINE_WIDTH_RANGE)}")
    print(f"GL_SMOOTH_LINE_WIDTH_RANGE: {GL.glGetFloatv(GL.GL_SMOOTH_LINE_WIDTH_RANGE)}")
    print(f"GL_SMOOTH_LINE_WIDTH_GRANULARITY: {GL.glGetFloat(GL.GL_SMOOTH_LINE_WIDTH_GRANULARITY)}")

    print(f"GL_TEXTURE_2D: {GL.glGetIntegerv(GL.GL_TEXTURE_2D)}")
    print(f"GL_ALPHA_TEST: {GL.glGetIntegerv(GL.GL_ALPHA_TEST)}")
    print(f"GL_BLEND: {GL.glGetIntegerv(GL.GL_BLEND)}")
    print(f"GL_DEPTH_TEST: {GL.glGetIntegerv(GL.GL_DEPTH_TEST)}")
    print(f"GL_DEPTH_WRITEMASK: {GL.glGetIntegerv(GL.GL_DEPTH_WRITEMASK)}")
