.PHONY : clean darwin lin windows sdk

PLUGIN_PATH=Resources/plugins/XPPython3
SDK=400
XP_SDK=../SDK401
VERSIONS= 310 311 312
# python versions are like 4.1.1 -> 4.1.2a1 -> 4.1.2a2 -> 4.1.2b1 -> 4.1.2 (letter is [ab], cannot end with letter)
XPPYTHONVERSION=4.1.2a9
BETA=$(shell echo $(XPPYTHONVERSION) | grep -ce '[a-z]')
FMOD=../FMOD
#########
# COMMON opts
# eventually....:   -DPy_LIMITED_API=0x030a0000
CFLAGS=-Wall -Wextra -g -fpic -fPIC -fvisibility=hidden -DXPPYTHON3VERSION=\"$(XPPYTHONVERSION)\"
XPLM_CPPFLAGS = -DXPLM200 -DXPLM210 -DXPLM300 -DXPLM301 -DXPLM302 -DXPLM303 -DXPLM400 \
                -I$(XP_SDK)/CHeaders -I$(XP_SDK)/CHeaders/XPLM -I$(XP_SDK)/CHeaders/Widgets -I$(FMOD)/api/core/inc -I$(FMOD)/api/studio/inc
# "BETA" make errCheck() function execute. Otherwise it is removed during compilation
ifeq ($(BETA), "0")
  CFLAGS+= -DERRCHECK=0
else
  CFLAGS+= -DERRCHECK=1
endif
ZIP=zip

##########
# PYTHON opts

ifeq ($(TARGET), Windows)
  # Do to the way we build for windows against 'python3.dll', the resulting binaries (3.6, 3.7, 3.8) are identical
  # except for the embedded 'version' value
  PYDEV310=/c/Program\ Files/Python310
  PYDEV311=/c/Program\ Files/Python311
  PYDEV312=/c/Program\ Files/Python312
  PY_CFLAGS = -Wno-unused-result -Wno-pointer-to-int-cast -Wno-cast-function-type -Wsign-compare -Wunreachable-code -fno-common -DNDEBUG -fwrapv -O3 -D_GNU_SOURCE
  PY_LDFLAGS = 
  CFLAGS+= ${PY_CFLAGS} ${XPLM_CPPFLAGS} -DIBM=1 -DAPL=0 -DLIN=0 -I/c/msys64/mingw64/include -I/usr/include
  CFLAGS310=-I${PYDEV310}/include $(CFLAGS) -DPYTHONVERSION=\"3.10\"
  CFLAGS311=-I${PYDEV311}/include $(CFLAGS) -DPYTHONVERSION=\"3.11\"
  CFLAGS312=-I${PYDEV312}/include $(CFLAGS) -DPYTHONVERSION=\"3.12\"
  LDFLAGS+= -shared -static-libgcc -static-libstdc++ -static -lpthread  ${PY_LDFLAGS} -L/c/msys64/mingw64/lib -ldl -lregex -ltre -lintl -liconv $(XP_SDK)/Libraries/Win/XPLM_64.lib $(XP_SDK)/Libraries/Win/XPWidgets_64.lib
  LDFLAGS310=$(LDFLAGS) -L. -lpython310.dll
  LDFLAGS311=$(LDFLAGS) -L. -lpython311.dll
  LDFLAGS312=$(LDFLAGS) -L. -lpython312.dll
  PLUGIN_DIR=win_x64
  LD=g++
endif

ifeq ($(TARGET), Linux)
  # apt-get install python3.6 python3.6-dev python3.7 python3.7-dev python3.8 python3.8-dev
  CFLAGS +=  ${XPLM_CPPFLAGS} -DIBM=0 -DAPL=0 -DLIN=1
  CFLAGS310= $(CFLAGS) $(shell python3.10-config --cflags) -DPYTHONVERSION=\"3.10\"
  CFLAGS311= $(CFLAGS) $(shell python3.11-config --cflags) -DPYTHONVERSION=\"3.11\"
  CFLAGS312= $(CFLAGS) $(shell python3.12-config --cflags) -DPYTHONVERSION=\"3.12\"
  LDFLAGS +=  -shared -fPIC -fpic -ldl -lbsd -g -fvisibility=hidden -Wl,--export-dynamic
  LDFLAGS310 = $(LDFLAGS) $(shell python3.10-config --ldflags) -lpython3.10
  LDFLAGS311 = $(LDFLAGS) $(shell python3.11-config --ldflags) -lpython3.11
  LDFLAGS312 = $(LDFLAGS) $(shell python3.12-config --ldflags) -lpython3.12
  PLUGIN_DIR=lin_x64
  LD=cc
endif

ifeq ($(TARGET), Darwin)
  CFLAGS += ${XPLM_CPPFLAGS} -DIBM=0 -DAPL=1 -DLIN=0
  CFLAGS310=$(CFLAGS) $(shell /Library/Frameworks/Python.framework/Versions/3.10/bin/python3-config --cflags) -DPYTHONVERSION=\"3.10\"
  CFLAGS311=$(CFLAGS) $(shell /Library/Frameworks/Python.framework/Versions/3.11/bin/python3-config --cflags) -DPYTHONVERSION=\"3.11\"
  CFLAGS312=$(CFLAGS) $(shell /Library/Frameworks/Python.framework/Versions/3.12/bin/python3-config --cflags) -DPYTHONVERSION=\"3.12\"
  LDFLAGS += -shared -fPIC -fpic -ldl -g -fvisibility=hidden -undefined dynamic_lookup
  LDFLAGS310 = $(LDFLAGS) $(shell /Library/Frameworks/Python.framework/Versions/3.10/bin/python3-config --ldflags) -lpython3.10
  LDFLAGS311 = $(LDFLAGS) $(shell /Library/Frameworks/Python.framework/Versions/3.11/bin/python3-config --ldflags) -lpython3.11
  LDFLAGS312 = $(LDFLAGS) $(shell /Library/Frameworks/Python.framework/Versions/3.12/bin/python3-config --ldflags) -lpython3.12
  PLUGIN_DIR=mac_x64
  LD=cc
endif

ifneq ($(BETA), 0)
  CFLAGS310 += -O0
  CFLAGS311 += -O0
  CFLAGS312 += -O0
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

%.o  : %.cpp
	$(CXX) -c $(CFLAGS) $< -o $@

PLUGIN_OBJ = pluginXXX.o defsXXX.o displayXXX.o utilsXXX.o graphicsXXX.o data_accessXXX.o utilitiesXXX.o sceneryXXX.o menusXXX.o \
	navigationXXX.o pluginsXXX.o planesXXX.o processingXXX.o cameraXXX.o widget_defsXXX.o widgetsXXX.o \
	standard_widgetsXXX.o uigraphicsXXX.o widgetutilsXXX.o instanceXXX.o mapXXX.o plugin_dlXXX.o sbXXX.o utilsXXX.o xppythonXXX.o \
	soundXXX.o \
	weatherXXX.o weatherinfo_typeXXX.o weatherinfoclouds_typeXXX.o weatherinfowinds_typeXXX.o \
	hotkeyinfo_typeXXX.o datarefinfo_typeXXX.o probeinfo_typeXXX.o plugininfo_typeXXX.o navaidinfo_typeXXX.o fmsentryinfo_typeXXX.o \
	trackmetrics_typeXXX.o manage_instanceXXX.o manage_instancesXXX.o logwriterXXX.o load_modulesXXX.o ini_fileXXX.o

%310.o	: %.c
ifeq ($(TARGET),Darwin)
	$(CC) -target arm64-apple-macos11 -c $(CFLAGS310) $< -o $@.arm
	$(CC) -target x86_64-apple-macos10.12 -c $(CFLAGS310) $< -o $@.x86
	touch $@
else
	$(CC) -c $(CFLAGS310) $< -o $@
endif

%311.o	: %.c
ifeq ($(TARGET),Darwin)
	$(CC) -target arm64-apple-macos11 -c $(CFLAGS311) $< -o $@.arm
	$(CC) -target x86_64-apple-macos10.12 -c $(CFLAGS311) $< -o $@.x86
	touch $@
else
	$(CC) -c $(CFLAGS311) $< -o $@
endif

%312.o	: %.c
ifeq ($(TARGET),Darwin)
	$(CC) -target arm64-apple-macos11 -c $(CFLAGS312) $< -o $@.arm
	$(CC) -target x86_64-apple-macos10.12 -c $(CFLAGS312) $< -o $@.x86
	touch $@
else
	$(CC) -c $(CFLAGS312) $< -o $@
endif

XPPython310.xpl: $(subst XXX,310,$(PLUGIN_OBJ))
ifeq ($(TARGET), Darwin)
	$(LD) -target arm64-apple-macos11 $(^:.o=.o.arm) $(LDFLAGS310) -o $@.arm
	$(LD) -target x86_64-apple-macos10.12 $(^:.o=.o.x86) $(LDFLAGS310) -o $@.x86
	lipo -create -output $@ $@.arm $@.x86
else
	$(LD) $^ $(LDFLAGS310) -o $@
endif

XPPython311.xpl: $(subst XXX,311,$(PLUGIN_OBJ))
ifeq ($(TARGET), Darwin)
	$(LD) -target arm64-apple-macos11 $(^:.o=.o.arm) $(LDFLAGS311) -o $@.arm
	$(LD) -target x86_64-apple-macos10.12 $(^:.o=.o.x86) $(LDFLAGS311) -o $@.x86
	lipo -create -output $@ $@.arm $@.x86
else
	$(LD) $^ $(LDFLAGS311) -o $@
endif

XPPython312.xpl: $(subst XXX,312,$(PLUGIN_OBJ))
ifeq ($(TARGET), Darwin)
	$(LD) -target arm64-apple-macos11 $(^:.o=.o.arm) $(LDFLAGS312) -o $@.arm
	$(LD) -target x86_64-apple-macos10.12 $(^:.o=.o.x86) $(LDFLAGS312) -o $@.x86
	lipo -create -output $@ $@.arm $@.x86
else
	$(LD) $^ $(LDFLAGS312) -o $@
endif


StandardPythonUtils = Resources/plugins/XPPython3/xp.py Resources/plugins/XPPython3/xp_imgui/ Resources/plugins/XPPython3/I_PI_Updater.py Resources/plugins/XPPython3/scriptupdate.py Resources/plugins/XPPython3/scriptconfig.py Resources/plugins/XPPython3/I_PI_FirstTime.py Resources/plugins/XPPython3/zip_download.py

Resources/plugins/xp3%.zip: win/XPPython3%.xpl lin/XPPython3%.xpl mac/XPPython3%.xpl $(StandardPythonUtils)
	# remove old
	rm -f $@
	rm -f Resources/plugins/XPPython3/*/*.xpl
	echo "XPPython3 v$(XPPYTHONVERSION) for $(subst .xpl,,$(subst XPPython3,Python 3.,$(notdir $(filter win/XPP%, $^))))" > Resources/plugins/XPPython3/version.txt
	# make staging directories
	mkdir -p Resources/plugins/XPPython3/win_x64
	mkdir -p Resources/plugins/XPPython3/lin_x64
	mkdir -p Resources/plugins/XPPython3/mac_x64
	# copy the correct plugin into the correct x-plane subdirectory
	cp $(filter win/%, $^) Resources/plugins/XPPython3/win_x64/XPPython3.xpl
	cp $(filter mac/%, $^) Resources/plugins/XPPython3/mac_x64/XPPython3.xpl
	cp $(filter lin/%, $^) Resources/plugins/XPPython3/lin_x64/XPPython3.xpl
	# 1) Create full zip file (e.g., xp3X.zip)
	# 2) Update that file with imgui stuff -- NO LONGER NEEDED
	# 3) for win, mac, lin
	#    a) duplicate zip
	#    b) delete from zip 'other' architecture file to reduce size
	# result is four files for each version:
	#     xp36.zip
	#     xp36-win32.zip
	#     xp36-linux.zip 
	#     xp36-darwin.zip 
	cd Resources/plugins; \
		$(ZIP) $(notdir $@) -R XPPython3  XPPython3/\*_x64/\*3.xpl XPPython3/.firstTimeComplete XPPython3/\*.pyi XPPython3/\*.py \*.md; \
		$(ZIP) $(notdir $@) -u XPPython3/utils/*.py ; \
		$(ZIP) $(notdir $@) -u XPPython3/hooks/*.py ; \
		$(ZIP) $(notdir $@) -u XPPython3/xp_imgui/*.py ; \
		$(ZIP) $(notdir $@) -u XPPython3/ui/*.py ; \
		$(ZIP) $(notdir $@) -u -r XPPython3/Authori*.app ; \
		$(ZIP) $(notdir $@) -u XPPython3/version.txt ; \
		cp $(notdir $@) $(basename $(notdir $@))-win32.zip; \
		$(ZIP) -d $(basename $(notdir $@))-win32.zip XPPython3/lin_\* XPPython3/mac_\*; \
		cp $(notdir $@) $(basename $(notdir $@))-darwin.zip; \
		$(ZIP) -d $(basename $(notdir $@))-darwin.zip XPPython3/lin_\* XPPython3/win_\* \*.pyd \*-gnu.so; \
		cp $(notdir $@) $(basename $(notdir $@))-linux.zip; \
		$(ZIP) -d $(basename $(notdir $@))-linux.zip XPPython3/mac_\* XPPython3/win_\* \*.pyd \*-darwin.so;

	rm -r Resources/plugins/XPPython3/*_x64


zipfiles=$(foreach version,$(VERSIONS), Resources/plugins/xp$(version).zip) $(foreach version,$(VERSIONS), Resources/plugins/xp$(version)-win32.zip) $(foreach version,$(VERSIONS), Resources/plugins/xp$(version)-darwin.zip) $(foreach version,$(VERSIONS), Resources/plugins/xp$(version)-linux.zip)

install_beta : $(zipfiles)
	# dendencies are xp3?.zip and xp3?-(win32|linux|darwin).zip
	# 1) rename each to beta naming xp3?b.zip and xp?-(win32|linux|darwin)b.zip
	for f in $^; do cp $$f `echo "$$f" | sed 's/\.zip/b.zip/g'`; done
	# 2) upload them to server
	scp Resources/plugins/xp31?{,-darwin,-linux,-win32}b.zip pbuck@avnwx.com:/var/www/production/data/x-plane
	# 3) On the server, execute updatemd5.py <>.zip <version>
	#    ... the server code will manage -win32, -darwin and -linux variants
	ssh pbuck@avnwx.com 'cd /var/www/production/data/x-plane; \
		./db_updatemd5.py xp310b.zip $(SDK) "$(XPPYTHONVERSION) - for Python 3.10"; \
		./db_updatemd5.py xp311b.zip $(SDK) "$(XPPYTHONVERSION) - for Python 3.11"; \
		./db_updatemd5.py xp312b.zip $(SDK) "$(XPPYTHONVERSION) - for Python 3.12"'

install_remote : $(zipfiles)
	@if [ "$(BETA)" -gt "0" ]; then \
		echo "Current version is BETA: $(XPPYTHONVERSION). Not installing"; \
		exit 1; \
	fi
	scp Resources/plugins/xp31?{,-darwin,-linux,-win32}.zip pbuck@avnwx.com:/var/www/production/data/x-plane
	ssh pbuck@avnwx.com 'cd /var/www/production/data/x-plane; \
		./db_updatemd5.py xp310.zip $(SDK) "$(XPPYTHONVERSION) - for Python 3.10"; \
		./db_updatemd5.py xp311.zip $(SDK) "$(XPPYTHONVERSION) - for Python 3.11"; \
		./db_updatemd5.py xp312.zip $(SDK) "$(XPPYTHONVERSION) - for Python 3.12"'

install_sdk : sdk
	$(ZIP) -R examples examples/\*.py
	$(ZIP) -R stubs stubs/\*.py

SUBDIRS = . mac lin win
SUBDIRSCLEAN=$(addsuffix .clean,$(SUBDIRS))

clean : $(SUBDIRSCLEAN)

clean_current_directory:
	rm -f *.so *.o main_??? XPPython3??.xpl default.prof* coverage.txt *.log *~ a.out xppython3?.zip *.arm *.x86

%.clean: %
	$(MAKE) -C $< clean_current_directory
