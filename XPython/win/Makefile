VPATH=..
TARGET=Windows
#
# We want to link with python3.dll, which is available for
# python37 and python38. WE point to python3.dll, installed version
# of python3.dll points to python37.dll or python38.dll, so WE don't
# have to change on python minor version change.
#
# To do this, we need to make sure we use only symbols available in python3 for
# our plugin -- we'll get link error if we try to use something not
# otherwise available. If at all possile, re-write to get use the "older" interface
# For example, originally we used PyUnicode_AsUTF8, but that's not in python3,
# so some re-write to use PyUnicode_AsUTF8String instead. Similarly,
# don't use Py_DecodeLocale as that required PyMem_RawFree, and instead
# use Py_DecodeLocaleString -- the changes generally require us to (also)
# PY_DECREF.
#    gendef - /c/Program\ Files/Python38/python3.dll > ./python3.def
#    dlltool -D /c/Program\ Files/Python38/python3.dll -d ./python3.def -l libpython3.dll.a
# This part of the build has been added to ../Makefile
#
# On the user side, they'll need to install
# 64-bit Python (3.7 or 3.8)
# a) "for all users"
# b) "set environment path"
# Note that by default, installation appears to favor 32-bit

TARGET=Windows
PYDEV312=/c/Program\ Files/Python312

all : XPPython3.xpl libpython3.dll.a libpython312.dll.a
	@echo $^

# Note if you add a new version of python, you may need to manually call make libpython<>.dll.a as
# I don't appear to have dependencies working properly

libpython312.dll.a : $(PYDEV312)/python312.dll
	gendef - "$^" > python312.def
	dlltool -D "$^" -d python312.def -l $@

include ../Makefile

