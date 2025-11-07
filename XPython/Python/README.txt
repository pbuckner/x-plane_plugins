This is Python3.12.1

For MacOS
--------
1) Installed Python framework, Python Documentation and "Install or upgrade pip". Did not
   install GUI Applications, UNIX command-line tools, Shell profile updater.

2) Also installed certificates, requests, opengl, imgui manually doing:
     python3.12 -E -s -m pip install --upgrade certifi pyopengl requests imgui

3) I've removed includes/, all __pycache__/ and __mypy__/ directories, tcl/tk stuff

4) Copy from default install area into xppython/Python/mac
    cp -r /Library/Frameworks/Python.framework/Versions/3.12 ~/xppython/Python/mac/python3.12

Resulting xppython/Python/mac/python3.12 has python for delivery. However, need to change
some dynamic linkage paths and replace signatures with adhoc. (You can view current
informaiton using otool -l <file>

  Python/mac/python3.12/Resources/Python.app/Contents/MacOS/Python
     install_name_tool -change ... @executable_path/../../../../lib/libpython3.12.dylib Python
     codesign --remove-signature ^$
     codesign -s - ^$
  Python/mac/python3.12/lib/libpython3.12.dylib
     codesign --remove-signature ^$
     codesign -s - ^$
  Python/mac/python3.12/lib/python3.12/lib-dynlib/_ssl.cpython-312-darwin.so
     install_name_tool -change ... @loader_path/../../libssl.dylib $^
     install_name_tool -change ... @loader_path/../../libcrypto.dylib $^
     codesign --remove-signature ^$
     codesign -s - ^$
  Python/mac/python3.12/lib/python3.12/lib-dynlib/_hashlib.cpython-312-darwin.so
     install_name_tool -change ... @loader_path/../../libcrypto.dylib $^
     codesign --remove-signature ^$
     codesign -s - ^$

  Python/mac/python3.12/lib/python3.12/site-packages/imgui/core.cpython-312-darwin.so
     codesign --remove-signature ^$
     codesign -s - ^$
  Python/mac/python3.12/lib/python3.12/site-packages/imgui/internal.cpython-312-darwin.so
     codesign --remove-signature ^$
     codesign -s - ^$

  vvvv not done, should I?
  Python/mac/python3.12/lib/python3.12/site-packages/charset_normalizer/md.cpython-312-darwin.so
  Python/mac/python3.12/lib/python3.12/site-packages/charset_normalizer/md__mypyc.cpython-312-darwin.so
  

For Linux (to be updated... likely will require pathclef changes similar to Mac?)
---------
copy pvenv.cfg
copy python3.12._pth (different from the mac one)
under lib, link libpython3.12.so -> libpython3.12.so.1 -> libpython3.12.so.1.0
under bin, just use python3.12

cd Resources/plugins/XPPython3/lin_x64
patchelf --replace-needed \
    libpython3.12.so.1.0 \
    Resources/plugins/XPPython3/lin_x64/python3.12/lib/libpython3.12.so.1.0 \
    XPPython3.xpl
  [ DONE in Makefile ]
  
Need to add pip doing get-pip.py, which created dist-packages under
python3.12/bin/local/lib/python3.12, so
that needed to be moved to lib/python3.12/dist-packages instead, and pip binaries from
python3.12/bin/local/bin to just python3.12/bin

For Windows (to be updated)
-----------
Download an extract all of the embedded windows python (64) into the
  ~/xp12/Resources/plugins/XPPython3/win_x64
... the same folder as which holds the XPL
ADD pip
1) wget copy of 'get-pip.py'
2) execute as pythonw.exe -s get-pip.py
   this will create site-packages folder (and Script folder) under win_x64, and get pi
3) ... could manually add more packages.
