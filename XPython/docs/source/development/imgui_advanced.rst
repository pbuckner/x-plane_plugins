ImGui Advanced
--------------

All of the xp / imgui interface is implemented in python code in the provided ``XPPython3/xp_imgui`` module. If
you're curious how this works, check out files under XPPython3:

 * **xp_imgui/window.py**: which provides the window manipulation code, and
 * **xp_imgui/xprenderer.py**: which provides the interface between XPlane's OpenGL and the imgui renderer

`You should not have to modify either of these files!` They are provides by XPPython3 and
are automatically updated each time XPPython3 is updated (and your changes will be lost).
If you see the need to make a modification, please contact me (support [at] xppython3.org) so
I can make the improvement available to everyone.


