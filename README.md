# X-Plane Python3 Plugin "XPPython3"

:warning:_CAUTION_: Work-in-progress:warning:

This is an updated version of Sandy Barbour's excellent Python Interface plugin, allowing you to create plugins for X-Plane using Python. Sandy's was restricted to Python2, this is the update to support python3.

It's called XPPython3:
1. "xpython" already exists (it's an all python version of python)
2. "3" to emphasize it is python3, which is not fully backward compatible with python2.

Key features:
* **X-Plane 11.50** _minimum_: There is no attempt to make this work on older versions of X-Plane. This plugin can co-exist with the older plugin, so if Python2 is still available it can be used with the older plugin. (Python2 is end-of-life.)
* **64-bit only**: X-Plane is now 64-bit only.
* **X-Plane SDK 303** _minimum_: This is base SDK version for 11.50. Vulkan/Metal and OpenGL are supported.
* **Python3 only**: This will not work with python2 programs which are not compatible with Python3. Note that you will have
to use the correct version of this plugin to match the version of python installed on your system. We currently support python3.6, python3.7 and python3.8

## Installation
See [README_INSTALL](XPython/Resources/plugins/XPPython3/README_INSTALL.md) for user installation instructions. This will be part of the user-facing website.

## Development of Plugins
See [README_SDK](XPython/README_SDK.md) for additional information on building your own plugins using Python. We'll include differences between the older Python2 plugin interface and this Python3 version.

## Building
See [README_BUILD](XPython/README_BUILD.md) for build instructions. (You should not have to build this plugin.)

- Linux, tested on Ubuntu18 with gnu compiler,
- Mac, developed and tested on Catalina 10.15 with Xcode compiler (command line only), 
- Windows, tested on Windows 10 with mingw-w64 tools. Might be compilable with VisualStudio, but I don't care.

Ultimately, we'll be providing a set of pre-built binaries for distribution. (Separate user-facing website not set yet.)

