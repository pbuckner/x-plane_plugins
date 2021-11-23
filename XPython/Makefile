.PHONY : clean darwin lin windows sdk

PLUGIN_PATH=Resources/plugins/XPPython3
XP_SDK=../SDK
VERSIONS= 36 37 38 39 310
XPPYTHONVERSION=3.1.0a1
BETA=$(shell echo $(XPPYTHONVERSION) | grep -ce '[a-z]')
#########
# COMMON opts
CFLAGS=-Wall -Wextra -g -fpic -fPIC -fvisibility=hidden -DXPPYTHON3VERSION=\"$(XPPYTHONVERSION)\"
XPLM_CPPFLAGS = -DXPLM200 -DXPLM210 -DXPLM300 -DXPLM301 -DXPLM302 -DXPLM303 \
                -I$(XP_SDK)/CHeaders -I$(XP_SDK)/CHeaders/XPLM -I$(XP_SDK)/CHeaders/Widgets
ZIP=zip

##########
# PYTHON opts

ifeq ($(TARGET), Windows)
  # Do to the way we build for windows against 'python3.dll', the resulting binaries (3.6, 3.7, 3.8) are identical
  # except for the embedded 'version' value
  PYDEV36=/c/Program\ Files/Python36
  PYDEV37=/c/Program\ Files/Python37
  PYDEV38=/c/Program\ Files/Python38
  PYDEV39=/c/Program\ Files/Python39
  PYDEV310=/c/Program\ Files/Python310
  PY_CFLAGS = -Wno-unused-result -Wno-pointer-to-int-cast -Wno-cast-function-type -Wsign-compare -Wunreachable-code -fno-common -DNDEBUG -fwrapv -O3 -D_GNU_SOURCE
  PY_LDFLAGS = 
  CFLAGS+= ${PY_CFLAGS} ${XPLM_CPPFLAGS} -DIBM=1 -DAPL=0 -DLIN=0 -I/c/msys64/mingw64/include -I/usr/include
  CFLAGS36=-I${PYDEV36}/include $(CFLAGS) -DPYTHONVERSION=\"3.6\"
  CFLAGS37=-I${PYDEV37}/include $(CFLAGS) -DPYTHONVERSION=\"3.7\"
  CFLAGS38=-I${PYDEV38}/include $(CFLAGS) -DPYTHONVERSION=\"3.8\"
  CFLAGS39=-I${PYDEV39}/include $(CFLAGS) -DPYTHONVERSION=\"3.9\"
  CFLAGS310=-I${PYDEV310}/include $(CFLAGS) -DPYTHONVERSION=\"3.10\"
  LDFLAGS+= -shared -static-libgcc -static-libstdc++ -static -lpthread  ${PY_LDFLAGS} -L/c/msys64/mingw64/lib -ldl -lregex -ltre -lintl -liconv $(XP_SDK)/Libraries/Win/XPLM_64.lib $(XP_SDK)/Libraries/Win/XPWidgets_64.lib
  LDFLAGS36=$(LDFLAGS) -L. -lpython36.dll
  LDFLAGS37=$(LDFLAGS) -L. -lpython37.dll
  LDFLAGS38=$(LDFLAGS) -L. -lpython38.dll
  LDFLAGS39=$(LDFLAGS) -L. -lpython39.dll
  LDFLAGS310=$(LDFLAGS) -L. -lpython310.dll
  PLUGIN_DIR=win_x64
  LD=g++
endif

ifeq ($(TARGET), Linux)
  # apt-get install python3.6 python3.6-dev python3.7 python3.7-dev python3.8 python3.8-dev
  CFLAGS +=  ${XPLM_CPPFLAGS} -DIBM=0 -DAPL=0 -DLIN=1
  CFLAGS36= $(CFLAGS) $(shell python3.6-config --cflags) -DPYTHONVERSION=\"3.6\"
  CFLAGS37= $(CFLAGS) $(shell python3.7-config --cflags) -DPYTHONVERSION=\"3.7\"
  CFLAGS38= $(CFLAGS) $(shell python3.8-config --cflags) -DPYTHONVERSION=\"3.8\"
  CFLAGS39= $(CFLAGS) $(shell python3.9-config --cflags) -DPYTHONVERSION=\"3.9\"
  CFLAGS310= $(CFLAGS) $(shell python3.10-config --cflags) -DPYTHONVERSION=\"3.10\"
  LDFLAGS +=  -shared -fPIC -fpic -ldl -g -fvisibility=hidden -Wl,--export-dynamic
  # For python3.6, python3.7, --ldflags includes the python library, for 3.8 it does not (why?)
  LDFLAGS36 = $(LDFLAGS) $(shell python3.6-config --ldflags)
  LDFLAGS37 = $(LDFLAGS) $(shell python3.7-config --ldflags)
  LDFLAGS38 = $(LDFLAGS) $(shell python3.8-config --ldflags) -lpython3.8
  LDFLAGS39 = $(LDFLAGS) $(shell python3.9-config --ldflags) -lpython3.9
  LDFLAGS310 = $(LDFLAGS) $(shell python3.10-config --ldflags) -lpython3.10
  PLUGIN_DIR=lin_x64
  LD=cc
endif

ifeq ($(TARGET), Darwin)
  CFLAGS += ${XPLM_CPPFLAGS} -DIBM=0 -DAPL=1 -DLIN=0 -arch x86_64
  CFLAGS36=$(CFLAGS) $(shell /Library/Frameworks/Python.framework/Versions/3.6/bin/python3-config --cflags) -DPYTHONVERSION=\"3.6\"
  CFLAGS37=$(CFLAGS) $(shell /Library/Frameworks/Python.framework/Versions/3.7/bin/python3-config --cflags) -DPYTHONVERSION=\"3.7\"
  CFLAGS38=$(CFLAGS) $(shell /Library/Frameworks/Python.framework/Versions/3.8/bin/python3-config --cflags) -DPYTHONVERSION=\"3.8\"
  CFLAGS39=$(CFLAGS) $(shell /Library/Frameworks/Python.framework/Versions/3.9/bin/python3-config --cflags) -DPYTHONVERSION=\"3.9\"
  CFLAGS310=$(CFLAGS) $(shell /Library/Frameworks/Python.framework/Versions/3.10/bin/python3-config --cflags) -DPYTHONVERSION=\"3.10\"
  LDFLAGS += -shared -fPIC -fpic -ldl -g -fvisibility=hidden -undefined dynamic_lookup
  LDFLAGS36 = $(LDFLAGS) $(shell /Library/Frameworks/Python.framework/Versions/3.6/bin/python3-config --ldflags)
  LDFLAGS37 = $(LDFLAGS) $(shell /Library/Frameworks/Python.framework/Versions/3.7/bin/python3-config --ldflags)
  LDFLAGS38 = $(LDFLAGS) $(shell /Library/Frameworks/Python.framework/Versions/3.8/bin/python3-config --ldflags) -lpython3.8
  LDFLAGS39 = $(LDFLAGS) $(shell /Library/Frameworks/Python.framework/Versions/3.9/bin/python3-config --ldflags) -lpython3.9
  LDFLAGS310 = $(LDFLAGS) $(shell /Library/Frameworks/Python.framework/Versions/3.10/bin/python3-config --ldflags) -lpython3.10
  PLUGIN_DIR=mac_x64
  LD=cc
endif

#########
# Build targets
Windows:
	cd win && $(MAKE)
	cd ..

Darwin:
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
	trackMetricsXXX.o

%36.o	: %.c
	$(CC) -c $(CFLAGS36) $< -o $@

%37.o	: %.c
	$(CC) -c $(CFLAGS37) $< -o $@

%38.o	: %.c
	$(CC) -c $(CFLAGS38) $< -o $@

%39.o	: %.c
	$(CC) -c $(CFLAGS39) $< -o $@

%310.o	: %.c
	$(CC) -c $(CFLAGS310) $< -o $@

XPPython36.xpl: $(subst XXX,36,$(PLUGIN_OBJ))
	$(LD) $^ $(LDFLAGS36) -o $@ 

XPPython37.xpl: $(subst XXX,37,$(PLUGIN_OBJ))
	$(LD) $^ $(LDFLAGS37) -o $@ 

XPPython38.xpl: $(subst XXX,38,$(PLUGIN_OBJ))
	$(LD) $^ $(LDFLAGS38) -o $@ 

XPPython39.xpl: $(subst XXX,39,$(PLUGIN_OBJ))
	$(LD) $^ $(LDFLAGS39) -o $@ 

XPPython310.xpl: $(subst XXX,310,$(PLUGIN_OBJ))
	$(LD) $^ $(LDFLAGS310) -o $@ 


StandardPythonUtils = Resources/plugins/XPPython3/xp.py Resources/plugins/XPPython3/xp_imgui/ Resources/plugins/XPPython3/I_PI_Updater.py Resources/plugins/XPPython3/scriptupdate.py Resources/plugins/XPPython3/scriptconfig.py Resources/plugins/XPPython3/I_PI_FirstTime.py Resources/plugins/XPPython3/zip_download.py Resources/plugins/XPPython3/imgui/core.cp310-win_amd64.pyd

Resources/plugins/xppython3%.zip: win/XPPython3%.xpl lin/XPPython3%.xpl mac/XPPython3%.xpl $(StandardPythonUtils)
	# remove old
	rm -f $@
	rm -f Resources/plugins/XPPython3/*/*.xpl
	# make staging directories
	mkdir -p Resources/plugins/XPPython3/win_x64
	mkdir -p Resources/plugins/XPPython3/lin_x64
	mkdir -p Resources/plugins/XPPython3/mac_x64
	# copy the correct plugin into the correct x-plane subdirectory
	cp $(filter win/%, $^) Resources/plugins/XPPython3/win_x64/XPPython3.xpl
	cp $(filter mac/%, $^) Resources/plugins/XPPython3/mac_x64/XPPython3.xpl
	cp $(filter lin/%, $^) Resources/plugins/XPPython3/lin_x64/XPPython3.xpl
	# 1) Create full zip file (e.g., xppython3X.zip)
	# 2) Update that file with imgui stuff
	# 3) for win, mac, lin
	#    a) duplicate zip
	#    b) delete from zip 'other' architecture file to reduce size
	# result is four files for each version:
	#     xppython36.zip
	#     xppython36-win32.zip
	#     xppython36-linux.zip 
	#     xppython36-darwin.zip 
	cd Resources/plugins; \
		$(ZIP) $(notdir $@) -R XPPython3  XPPython3/\*_x64/\*3.xpl XPPython3/\*.py \*.md  XPPython3/xpyce/\*.py XPPython3/xp_imgui/\*.py; \
		v=`echo $(notdir $@) | sed  's/xppython\([0-9]*\).zip/\1/'`; \
		$(ZIP) $(notdir $@) -u XPPython3/imgui/*.py XPPython3/imgui/integrations/*.py XPPython3/imgui/core*$$v* ; \
		$(ZIP) $(notdir $@) -u XPPython3/utils/*.py ; \
		cp $(notdir $@) $(basename $(notdir $@))-win32.zip; \
		$(ZIP) -d $(basename $(notdir $@))-win32.zip XPPython3/lin_\* XPPython3/mac_\* XPPython3/imgui/core.cpython\*; \
		cp $(notdir $@) $(basename $(notdir $@))-darwin.zip; \
		$(ZIP) -d $(basename $(notdir $@))-darwin.zip XPPython3/lin_\* XPPython3/win_\* \*.pyd \*-gnu.so; \
		cp $(notdir $@) $(basename $(notdir $@))-linux.zip; \
		$(ZIP) -d $(basename $(notdir $@))-linux.zip XPPython3/mac_\* XPPython3/win_\* \*.pyd \*-darwin.so;

	rm -r Resources/plugins/XPPython3/*_x64


zipfiles=$(foreach version,$(VERSIONS), Resources/plugins/xppython$(version).zip) $(foreach version,$(VERSIONS), Resources/plugins/xppython$(version)-win32.zip) $(foreach version,$(VERSIONS), Resources/plugins/xppython$(version)-darwin.zip) $(foreach version,$(VERSIONS), Resources/plugins/xppython$(version)-linux.zip)

install_beta : $(zipfiles)
	# dendencies are xppython3?.zip and xppython3?-(win32|linux|darwin).zip
	# 1) rename each to beta naming xppython3?b.zip and xppython3?-(win32|linux|darwin)b.zip
	for f in $^; do cp $$f `echo "$$f" | sed 's/\.zip/b.zip/g'`; done
	# 2) upload them to server
	scp Resources/plugins/xppython3?{,-darwin,-linux,-win32}b.zip pbuck@avnwx.com:/var/www/production/data/x-plane
	scp Resources/plugins/xppython310{,-darwin,-linux,-win32}b.zip pbuck@avnwx.com:/var/www/production/data/x-plane
	# 3) On the server, execute updatemd5.py <>.zip <version>
	#    ... the server code will manage -win32, -darwin and -linux variants
	ssh pbuck@avnwx.com 'cd /var/www/production/data/x-plane; \
		./updatemd5.py xppython36b.zip "$(XPPYTHONVERSION) - for Python 3.6"; \
		./updatemd5.py xppython37b.zip "$(XPPYTHONVERSION) - for Python 3.7"; \
		./updatemd5.py xppython38b.zip "$(XPPYTHONVERSION) - for Python 3.8"; \
		./updatemd5.py xppython39b.zip "$(XPPYTHONVERSION) - for Python 3.9"; \
		./updatemd5.py xppython310b.zip "$(XPPYTHONVERSION) - for Python 3.10"'

install_remote : $(zipfiles)
	@if [ "$(BETA)" -gt "0" ]; then \
		echo "Current version is BETA: $(XPPYTHONVERSION). Not installing"; \
		exit 1; \
	fi
	scp Resources/plugins/xppython3?{,-darwin,-linux,-win32}.zip pbuck@avnwx.com:/var/www/production/data/x-plane
	scp Resources/plugins/xppython310{,-darwin,-linux,-win32}.zip pbuck@avnwx.com:/var/www/production/data/x-plane
	ssh pbuck@avnwx.com 'cd /var/www/production/data/x-plane; \
		./updatemd5.py xppython36.zip "$(XPPYTHONVERSION) - for Python 3.6"; \
		./updatemd5.py xppython37.zip "$(XPPYTHONVERSION) - for Python 3.7"; \
		./updatemd5.py xppython38.zip "$(XPPYTHONVERSION) - for Python 3.8"; \
		./updatemd5.py xppython39.zip "$(XPPYTHONVERSION) - for Python 3.9"; \
		./updatemd5.py xppython310.zip "$(XPPYTHONVERSION) - for Python 3.10"'

install_sdk : sdk
	$(ZIP) -R examples examples/\*.py
	$(ZIP) -R stubs stubs/\*.py

SUBDIRS = . mac lin win
SUBDIRSCLEAN=$(addsuffix .clean,$(SUBDIRS))

clean : $(SUBDIRSCLEAN)

clean_current_directory:
	rm -f *.so *.o main_??? xppython3?.xpl default.prof* coverage.txt *.log *~ a.out xppython3?.zip

%.clean: %
	$(MAKE) -C $< clean_current_directory
