.PHONY : clean darwin lin windows

PLUGIN_PATH=Resources/plugins/XPPython3
SERVER_SDK=400
XP_SDK=../SDK411
VERSIONS= 312
# python versions are like 4.1.1 -> 4.1.2a1 -> 4.1.2a2 -> 4.1.2b1 -> 4.1.2 (letter is [ab], cannot end with letter)
XPPYTHONVERSION=4.4.2a10
BETA=$(shell echo $(XPPYTHONVERSION) | grep -ce 'b')
ALPHA=$(shell echo $(XPPYTHONVERSION) | grep -ce 'a')
FMOD=../FMOD
ZIP=zip
zipfiles = \
	Resources/plugins/xp3upgrade-win32.zip \
	Resources/plugins/xp3upgrade-darwin.zip \
	Resources/plugins/xp3upgrade-linux.zip \
	Resources/plugins/xp3-win32.zip \
	Resources/plugins/xp3-darwin.zip \
	Resources/plugins/xp3-linux.zip

#########
# COMMON opts
# eventually....:   -DPy_LIMITED_API=0x030a0000
CFLAGS=-Wall -Wextra -g -fpic -fPIC -fvisibility=hidden -DXPPYTHON3VERSION=\"$(XPPYTHONVERSION)\"
XPLM_CPPFLAGS = -DXPLM200 -DXPLM210 -DXPLM300 -DXPLM301 -DXPLM302 -DXPLM303 -DXPLM400 -DXPLM410 -DXPLM411 \
                -I$(XP_SDK)/CHeaders -I$(XP_SDK)/CHeaders/XPLM -I$(XP_SDK)/CHeaders/Widgets -I$(FMOD)/api/core/inc -I$(FMOD)/api/studio/inc

# "ALPHA" make errCheck() function execute. Otherwise it is removed during compilation and optimization is enabled
ifeq ("$(ALPHA)", "1")
  CFLAGS+= -DERRCHECK=1
  OPTIMIZATION= -O0
else
  CFLAGS+= -DERRCHECK=0
  OPTIMIZATION= -O3
endif

##########
# PYTHON opts

ifeq ($(TARGET), Windows)
  # Do to the way we build for windows against 'python3.dll', the resulting binaries (3.6, 3.7, 3.8) are identical
  # except for the embedded 'version' value
  PYDEV312=/c/Program\ Files/Python312
  PY_CFLAGS = -Wno-unused-result -Wno-pointer-to-int-cast -Wno-cast-function-type -Wsign-compare -Wunreachable-code -fno-common -DNDEBUG -fwrapv -O3 -D_GNU_SOURCE
  PY_LDFLAGS = 
  CFLAGS+= ${PY_CFLAGS} ${XPLM_CPPFLAGS} -DIBM=1 -DAPL=0 -DLIN=0 -I/c/msys64/mingw64/include -I/usr/include
  CFLAGS312=-I${PYDEV312}/include $(CFLAGS) -DPYTHONVERSION=\"3.12\"
  LDFLAGS+= -shared -static-libgcc -static-libstdc++ -static -lpthread  ${PY_LDFLAGS} -L/c/msys64/mingw64/lib -ldl -lregex -ltre -lintl -liconv $(XP_SDK)/Libraries/Win/XPLM_64.lib $(XP_SDK)/Libraries/Win/XPWidgets_64.lib
  LDFLAGS312=$(LDFLAGS) -L. -lpython312.dll
  PLUGIN_DIR=win_x64
  LD=g++
endif

ifeq ($(TARGET), Linux)
  # apt-get install python3.6 python3.6-dev python3.7 python3.7-dev python3.8 python3.8-dev
  CFLAGS +=  ${XPLM_CPPFLAGS} -DIBM=0 -DAPL=0 -DLIN=1
  CFLAGS312= $(CFLAGS) $(shell python3.12-config --cflags) -DPYTHONVERSION=\"3.12\"
  LDFLAGS +=  -shared -fPIC -fpic -ldl -lbsd -g -fvisibility=hidden -Wl,--export-dynamic
  LDFLAGS312 = $(LDFLAGS) $(shell python3.12-config --ldflags) -lpython3.12
  PLUGIN_DIR=lin_x64
  LD=cc
endif

ifeq ($(TARGET), Darwin)
  CFLAGS += ${XPLM_CPPFLAGS} -DIBM=0 -DAPL=1 -DLIN=0
  PYTHON_CONFIG_FLAGS = -I/Library/Frameworks/Python.framework/Versions/3.12/include/python3.12 -I/Library/Frameworks/Python.framework/Versions/3.12/include/python3.12 -fno-strict-overflow -Wsign-compare -Wunreachable-code -fno-common -dynamic -DNDEBUG -g -Wall -arch arm64 -arch x86_64 -g
CFLAGS312=$(CFLAGS) $(PYTHON_CONFIG_FLAGS) -DPYTHONVERSION=\"3.12\"
  CFLAGS312+=$(OPTIMIZATION)
  # LDFLAGS += -shared -fPIC -fpic -ldl -g -fvisibility=hidden -undefined dynamic_lookup
  LDFLAGS += -shared -fPIC -fpic -ldl -g -fvisibility=hidden 
  LDFLAGS312 = $(LDFLAGS) $(shell /Library/Frameworks/Python.framework/Versions/3.12/bin/python3-config --ldflags) -lpython3.12 -F $(XP_SDK)/Libraries/Mac -framework XPLM -framework XPWidgets -framework AppKit
  PLUGIN_DIR=mac_x64
  LD=cc
endif

#########
# Build targets
Windows windows:
	cd win && $(MAKE)
	cd ..

Darwin darwin:
	cd mac && $(MAKE)
	cd ..

linux:
	cd lin && $(MAKE)
	cd ..

PLUGIN_OBJ = plugin.o defs.o display.o utils.o graphics.o data_access.o utilities.o scenery.o menus.o \
	navigation.o plugins.o planes.o processing.o camera.o widget_defs.o widgets.o \
	standard_widgets.o uigraphics.o widgetutils.o instance.o map.o plugin_dl.o sb.o utils.o xppython.o \
	sound.o \
	weather.o weatherinfo_type.o weatherinfoclouds_type.o weatherinfowinds_type.o \
	hotkeyinfo_type.o datarefinfo_type.o probeinfo_type.o plugininfo_type.o navaidinfo_type.o fmsentryinfo_type.o \
	trackmetrics_type.o manage_instance.o manage_instances.o logwriter.o load_modules.o ini_file.o cursor.o

cursor.o : cursor.c
ifeq ($(TARGET),Darwin)
	$(CC) -x objective-c -target arm64-apple-macos11 -c $(CFLAGS312) $< -o $@.arm
	$(CC) -x objective-c -target x86_64-apple-macos10.12 -c $(CFLAGS312)  $< -o $@.x86
	touch $@
else
	$(CC) -c $(CFLAGS312) $< -o $@
endif

%.o	: %.c
ifeq ($(TARGET),Darwin)
	$(CC) -target arm64-apple-macos11 -c $(CFLAGS312) $< -o $@.arm
	$(CC) -target x86_64-apple-macos10.12 -c $(CFLAGS312) $< -o $@.x86
	touch $@
else
	$(CC) -c $(CFLAGS312) $< -o $@
endif

# Modifying *.xpl to "find" python shared object:
# If using @rpath rather than @executable, you cannot use symlinks for *.xpl, as rpath @loader
# is based off of the original_location of xpl. (so you'd have to have python3.12/lib... link _in_ same
# directory as original *.xpl
# I'm keeping with @executable because a) it works; b) it works with symlinks. Here is MAC code
# for using rpath:
# install_name_tool \
# 	-change /Library/Frameworks/Python.framework/Versions/3.12/Python  \
# 	@rpath/libpython3.12.dylib \
# 	XPPython3.xpl
# install_name_tool \
# 	-add_rpath @loader_path/python3.12/lib XPPython3.xpl

XPPython3.xpl: $(PLUGIN_OBJ)
ifeq ($(TARGET), Darwin)
	# $(LD) -headerpad_max_install_names -target arm64-apple-macos11 $(^:.o=.o.arm) $(LDFLAGS312) -o $@.arm
	# $(LD) -headerpad_max_install_names -target x86_64-apple-macos10.12 $(^:.o=.o.x86) $(LDFLAGS312) -o $@.x86
	$(LD) -target arm64-apple-macos11 $(^:.o=.o.arm) $(LDFLAGS312) -o $@.arm
	$(LD) -target x86_64-apple-macos10.12 $(^:.o=.o.x86) $(LDFLAGS312) -o $@.x86
	lipo -create -output $@ $@.arm $@.x86
	install_name_tool \
		-change /Library/Frameworks/Python.framework/Versions/3.12/Python  \
		@executable_path/../../../Resources/plugins/XPPython3/mac_x64/python3.12/lib/libpython3.12.dylib \
		XPPython3.xpl
	codesign --remove-signature XPPython3.xpl
	codesign -s - XPPython3.xpl
else ifeq ($(TARGET), Linux)
	$(LD) $^ $(LDFLAGS312) -o $@
	patchelf \
		--replace-needed libpython3.12.so.1.0 \
		Resources/plugins/XPPython3/lin_x64/python3.12/lib/libpython3.12.so \
		XPPython3.xpl
else
	$(LD) $^ $(LDFLAGS312) -o $@
endif


StandardPythonUtils = Resources/plugins/XPPython3/xp.py Resources/plugins/XPPython3/xp_imgui/ Resources/plugins/XPPython3/I_PI_Updater.py Resources/plugins/XPPython3/scriptupdate.py Resources/plugins/XPPython3/scriptconfig.py Resources/plugins/XPPython3/I_PI_FirstTime.py Resources/plugins/XPPython3/zip_download.py

Resources/plugins/xp3-darwin.zip: win/XPPython3.xpl lin/XPPython3.xpl mac/XPPython3.xpl $(StandardPythonUtils)
	# remove old
	rm -fr Resources/plugins/xp.zip $(zipfiles)
	rm -ff Resources/plugins/XPPython3/*/*.xpl
	echo "XPPython3 v$(XPPYTHONVERSION) for Python 3.12" > Resources/plugins/XPPython3/version.txt
	# make staging directories
	mkdir -p Resources/plugins/XPPython3/win_x64
	mkdir -p Resources/plugins/XPPython3/lin_x64
	mkdir -p Resources/plugins/XPPython3/mac_x64
	# copy the correct plugin into the correct x-plane subdirectory
	cp $(filter win/%, $^) Resources/plugins/XPPython3/win_x64/XPPython3.xpl
	cp $(filter mac/%, $^) Resources/plugins/XPPython3/mac_x64/XPPython3.xpl
	cp $(filter lin/%, $^) Resources/plugins/XPPython3/lin_x64/XPPython3.xpl
	# 1) Create full zip file (e.g., xp3X.zip)
	# 2) for win, mac, lin
	#    a) duplicate zip
	#    b) delete from zip 'other' architecture file to reduce size
	# result is three files for each version:
	#     xp3-win32.zip
	#     xp3-linux.zip 
	#     xp3-darwin.zip 
	cd Resources/plugins; \
		$(ZIP) xp.zip -q -R XPPython3  XPPython3/*_x64/*3.xpl; \
		$(ZIP) xp.zip -q -u XPPython3/version.txt ; \
		$(ZIP) xp.zip -q -u XPPython3/.firstTimeComplete; \
		$(ZIP) xp.zip -q -u XPPython3/*.png; \
		$(ZIP) xp.zip -q -u XPPython3/*.md; \
		$(ZIP) xp.zip -q -u XPPython3/*.py ; \
		$(ZIP) xp.zip -q -u XPPython3/*/*.py ; \
		$(ZIP) xp.zip -q -u XPPython3/*.pyi ; \
		$(ZIP) xp.zip -q -u XPPython3/cursors/*.cur ; \
		$(ZIP) xp.zip -q -u XPPython3/cursors/*.png ; \
		$(ZIP) xp.zip -q -u XPPython3/cursors/*.xcf ; \
		$(ZIP) xp.zip -q -u -r XPPython3/Drop\ Q*.app;
	# That's the same as the 'upgrade' -- python-less -- version
	cp Resources/plugins/xp.zip Resources/plugins/xp3upgrade-win32.zip
	cp Resources/plugins/xp.zip Resources/plugins/xp3upgrade-darwin.zip
	cp Resources/plugins/xp.zip Resources/plugins/xp3upgrade-linux.zip
	# Delete "other" architectures from each zip
	$(ZIP) -d -q Resources/plugins/xp3upgrade-win32.zip  XPPython3/lin_\* XPPython3/mac_\* XPPython3/Drop\ \* XPPython3/Authori\*
	$(ZIP) -d -q Resources/plugins/xp3upgrade-darwin.zip XPPython3/lin_\* XPPython3/win_\*
	$(ZIP) -d -q Resources/plugins/xp3upgrade-linux.zip  XPPython3/mac_\* XPPython3/win_\* XPPython3/Drop\ \* XPPython3/Authori\*


	# Copy "Python3.12" distro
	cp Resources/plugins/xp3upgrade-win32.zip Resources/plugins/xp3-win32.zip
	cp Resources/plugins/xp3upgrade-darwin.zip Resources/plugins/xp3-darwin.zip
	cp Resources/plugins/xp3upgrade-linux.zip Resources/plugins/xp3-linux.zip

	rm -rf Resource/plugins/XPPython3/*/python3.12
	cp -r Python/mac/ Resources/plugins/XPPython3/mac_x64/
	cp -r Python/lin/ Resources/plugins/XPPython3/lin_x64/
	cp -r Python/win/ Resources/plugins/XPPython3/win_x64/

	cd Resources/plugins; $(ZIP) xp3-darwin.zip -q -u -r XPPython3/mac_x64/python3.12
	cd Resources/plugins; $(ZIP) xp3-linux.zip  -q -u -r XPPython3/lin_x64/python3.12
	cd Resources/plugins; $(ZIP) xp3-win32.zip  -q -u -r XPPython3/win_x64
	#
	sleep 30
	rm -fr Resources/plugins/XPPython3/mac_x64
	rm -fr Resources/plugins/XPPython3/lin_x64
	rm -fr Resources/plugins/XPPython3/win_x64


install_beta : $(zipfiles)
	# Beta or Alpha installed this way
	# dependencies are xp3.zip and xp3-(win32|linux|darwin).zip
	# 1) rename each to beta naming xp3b.zip and xp-(win32|linux|darwin)b.zip
	for f in $(filter %.zip, $^); do cp $$f `echo "$$f" | sed 's/\.zip/b.zip/g'`; done
	# 2) upload them to server
	scp Resources/plugins/xp3{,upgrade}{-darwin,-linux,-win32}b.zip pbuck@avnwx.com:/var/www/production/data/x-plane
	# 3) On the server, execute updatemd5.py <>.zip <version>
	#    ... the server code will manage -win32, -darwin and -linux variants
	ssh pbuck@avnwx.com 'cd /var/www/production/data/x-plane; \
		./db_updatemd5.py xp3b.zip $(SERVER_SDK) "$(XPPYTHONVERSION)"'

install_remote : $(zipfiles)
	@if [ "$(BETA)" -gt "0" -o "$(ALPHA)" -gt "0"]; then \
		echo "Current version is BETA: $(XPPYTHONVERSION). Not installing"; \
		exit 1; \
	fi
	scp Resources/plugins/xp3{,upgrade}{-darwin,-linux,-win32}.zip pbuck@avnwx.com:/var/www/production/data/x-plane
	ssh pbuck@avnwx.com 'cd /var/www/production/data/x-plane; \
		./db_updatemd5.py xp3.zip $(SERVER_SDK) "$(XPPYTHONVERSION)"'

SUBDIRS = . mac lin win
SUBDIRSCLEAN=$(addsuffix .clean,$(SUBDIRS))

clean : $(SUBDIRSCLEAN)

clean_current_directory:
	rm -f *.so *.o main_??? XPPython3.xpl default.prof* coverage.txt *.log *~ a.out xppython3.zip *.arm *.x86

%.clean: %
	$(MAKE) -C $< clean_current_directory
