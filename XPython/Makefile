.PHONY : clean darwin lin windows sdk

PLUGIN_PATH=Resources/plugins/XPPython3
XP_SDK=../SDK

#########
# COMMON opts
CFLAGS=-Wall -Wextra -g -fpic -fPIC -fvisibility=hidden
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
  PY_CFLAGS = -Wno-unused-result -Wsign-compare -Wunreachable-code -fno-common -DNDEBUG -fwrapv -O3
  PY_LDFLAGS = 
  CFLAGS+= ${PY_CFLAGS} ${XPLM_CPPFLAGS} -DIBM=1 -DAPL=0 -DLIN=0 -I/c/msys64/mingw64/include -I/usr/include
  CFLAGS36=-I${PYDEV36}/include $(CFLAGS) -DPYTHONVERSION=\"3.6\"
  CFLAGS37=-I${PYDEV37}/include $(CFLAGS) -DPYTHONVERSION=\"3.7\"
  CFLAGS38=-I${PYDEV38}/include $(CFLAGS) -DPYTHONVERSION=\"3.8\"
  LDFLAGS+= -shared -static-libgcc -static-libstdc++ -static -lpthread  ${PY_LDFLAGS} -L/c/msys64/mingw64/lib -ldl -lregex -ltre -lintl -liconv $(XP_SDK)/Libraries/Win/XPLM_64.lib $(XP_SDK)/Libraries/Win/XPWidgets_64.lib
  LDFLAGS36=$(LDFLAGS) -L. -lpython36.dll
  LDFLAGS37=$(LDFLAGS) -L. -lpython37.dll
  LDFLAGS38=$(LDFLAGS) -L. -lpython38.dll
  PLUGIN_DIR=win_x64
  LD=g++
endif

ifeq ($(TARGET), Linux)
  # apt-get install python3.6 python3.6-dev python3.7 python3.7-dev python3.8 python3.8-dev
  CFLAGS +=  ${XPLM_CPPFLAGS} -DIBM=0 -DAPL=0 -DLIN=1
  CFLAGS36= $(CFLAGS) $(shell python3.6-config --cflags) -DPYTHONVERSION=\"3.6\"
  CFLAGS37= $(CFLAGS) $(shell python3.7-config --cflags) -DPYTHONVERSION=\"3.7\"
  CFLAGS38= $(CFLAGS) $(shell python3.8-config --cflags) -DPYTHONVERSION=\"3.8\"
  LDFLAGS +=  -shared -fPIC -fpic -ldl -g -fvisibility=hidden -Wl,--export-dynamic
  # For python3.6, python3.7, --ldflags includes the python library, for 3.8 it does not (why?)
  LDFLAGS36 = $(LDFLAGS) $(shell python3.6-config --ldflags)
  LDFLAGS37 = $(LDFLAGS) $(shell python3.7-config --ldflags)
  LDFLAGS38 = $(LDFLAGS) $(shell python3.8-config --ldflags) -lpython3.8
  PLUGIN_DIR=lin_x64
  LD=cc
endif

ifeq ($(TARGET), Darwin)
  # brew install pyenv pyenv-virtualenv pyenv-virtualenvwrapper
  # pyenv install 3.6-dev
  # pyenv install 3.7-dev
  # pyenv install 3.8-dev
  # PY_CFLAGS = $(shell /Users/pbuck/bin/python3-config --cflags)
  # PY_LDFLAGS = $(shell /Users/pbuck/bin/python3-config --ldflags)
  CFLAGS += ${XPLM_CPPFLAGS} -DIBM=0 -DAPL=1 -DLIN=0 -arch x86_64
  CFLAGS36=$(CFLAGS) $(shell /Library/Frameworks/Python.framework/Versions/3.6/bin/python3-config --cflags) -DPYTHONVERSION=\"3.6\"
  CFLAGS37=$(CFLAGS) $(shell /Library/Frameworks/Python.framework/Versions/3.7/bin/python3-config --cflags) -DPYTHONVERSION=\"3.7\"
  CFLAGS38=$(CFLAGS) $(shell /Library/Frameworks/Python.framework/Versions/3.8/bin/python3-config --cflags) -DPYTHONVERSION=\"3.8\"
  LDFLAGS += -shared -fPIC -fpic -ldl -g -fvisibility=hidden -undefined dynamic_lookup
  LDFLAGS36 = $(LDFLAGS) $(shell /Library/Frameworks/Python.framework/Versions/3.6/bin/python3-config --ldflags)
  LDFLAGS37 = $(LDFLAGS) $(shell /Library/Frameworks/Python.framework/Versions/3.7/bin/python3-config --ldflags)
  LDFLAGS38 = $(LDFLAGS) $(shell /Library/Frameworks/Python.framework/Versions/3.8/bin/python3-config --ldflags) -lpython3.8
  PLUGIN_DIR=mac_x64
  LD=cc
endif

#########
# Build targets
windows:
	cd win && $(MAKE)
	cd ..

darwin:
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

XPPython36.xpl: $(subst XXX,36,$(PLUGIN_OBJ))
	$(LD) $^ $(LDFLAGS36) -o $@ 

XPPython37.xpl: $(subst XXX,37,$(PLUGIN_OBJ))
	$(LD) $^ $(LDFLAGS37) -o $@ 

XPPython38.xpl: $(subst XXX,38,$(PLUGIN_OBJ))
	$(LD) $^ $(LDFLAGS38) -o $@ 


StandardPythonUtils = Resources/plugins/XPPython3/xp.py Resources/plugins/XPPython3/I_PI_Updater.py Resources/plugins/XPPython3/scriptupdate.py Resources/plugins/XPPython3/scriptconfig.py

Resources/plugins/xppython36.zip : win/XPPython36.xpl lin/XPPython36.xpl mac/XPPython36.xpl $(StandardPythonUtils)
	rm -f $@
	rm -f Resources/plugins/XPPython3/*/*.xpl
	mkdir -p Resources/plugins/XPPython3/win_x64
	mkdir -p Resources/plugins/XPPython3/lin_x64
	mkdir -p Resources/plugins/XPPython3/mac_x64
	cp win/XPPython36.xpl Resources/plugins/XPPython3/win_x64/XPPython3.xpl
	cp mac/XPPython36.xpl Resources/plugins/XPPython3/mac_x64/XPPython3.xpl
	cp lin/XPPython36.xpl Resources/plugins/XPPython3/lin_x64/XPPython3.xpl
	cd Resources/plugins; $(ZIP) xppython36.zip -R XPPython3  XPPython3/\*_x64/\*3.xpl XPPython3/\*.py \*.md  XPPython3/xpyce/\*.py

Resources/plugins/xppython37.zip : win/XPPython37.xpl lin/XPPython37.xpl mac/XPPython37.xpl $(StandardPythonUtils)
	rm -f $@
	rm -f Resources/plugins/XPPython3/*/*.xpl
	mkdir -p Resources/plugins/XPPython3/win_x64
	mkdir -p Resources/plugins/XPPython3/lin_x64
	mkdir -p Resources/plugins/XPPython3/mac_x64
	cp win/XPPython37.xpl Resources/plugins/XPPython3/win_x64/XPPython3.xpl
	cp mac/XPPython37.xpl Resources/plugins/XPPython3/mac_x64/XPPython3.xpl
	cp lin/XPPython37.xpl Resources/plugins/XPPython3/lin_x64/XPPython3.xpl
	cd Resources/plugins; $(ZIP) xppython37.zip -R XPPython3  XPPython3/\*_x64/\*3.xpl XPPython3/\*.py \*.md  XPPython3/xpyce/\*.py

Resources/plugins/xppython38.zip : win/XPPython38.xpl lin/XPPython38.xpl mac/XPPython38.xpl $(StandardPythonUtils)
	rm -f $@
	rm -f Resources/plugins/XPPython3/*/*.xpl
	mkdir -p Resources/plugins/XPPython3/win_x64
	mkdir -p Resources/plugins/XPPython3/lin_x64
	mkdir -p Resources/plugins/XPPython3/mac_x64
	cp win/XPPython38.xpl Resources/plugins/XPPython3/win_x64/XPPython3.xpl
	cp mac/XPPython38.xpl Resources/plugins/XPPython3/mac_x64/XPPython3.xpl
	cp lin/XPPython38.xpl Resources/plugins/XPPython3/lin_x64/XPPython3.xpl
	cd Resources/plugins; $(ZIP) xppython38.zip -R XPPython3  XPPython3/\*_x64/\*3.xpl XPPython3/\*.py \*.md XPPython3/xpyce/\*.py


install_beta :  Resources/plugins/xppython36.zip Resources/plugins/xppython37.zip Resources/plugins/xppython38.zip
	cp Resources/plugins/xppython36.zip Resources/plugins/xppython36b.zip
	cp Resources/plugins/xppython37.zip Resources/plugins/xppython37b.zip
	cp Resources/plugins/xppython38.zip Resources/plugins/xppython38b.zip
	scp Resources/plugins/xppython3?b.zip pbuck@avnwx.com:/var/www/production/data/x-plane
	ssh pbuck@avnwx.com 'cd /var/www/production/data/x-plane; ./updatemd5 xppython36b.zip; ./updatemd5 xppython37b.zip; ./updatemd5 xppython38b.zip'

install_remote :  Resources/plugins/xppython36.zip Resources/plugins/xppython37.zip Resources/plugins/xppython38.zip
	scp Resources/plugins/xppython3?.zip pbuck@avnwx.com:/var/www/production/data/x-plane
	ssh pbuck@avnwx.com 'cd /var/www/production/data/x-plane; ./updatemd5 xppython36.zip; ./updatemd5 xppython37.zip; ./updatemd5 xppython38.zip'

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


