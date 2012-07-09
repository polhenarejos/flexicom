DEBUG=1
CMDLINE=1

###############################

CC=cl
LINK=link
RC=rc
MT=mt
LIBT=lib
RM=@del /F /Q
COPY=@copy /Y

## Script for detecting W64 system or not. It sets IS64 to 1 if is x64 or 0 otherwise
!if [tools\Is64.cmd >Makefile.auto]
!error *** Could not create makefile.auto
!endif
!include makefile.auto
!if $(IS64) == 1
PREF=x64
!else
PREF=x32
!endif

## QT
QT_DIR=deps\Qt
QT_INC_DIR=$(QT_DIR)\include
QT_LIB_DIR=$(QT_DIR)\lib\$(PREF)
QT_BIN_DIR=$(QT_DIR)\bin\$(PREF)
MOC=deps\Qt\bin\$(PREF)\moc.exe
## QWT
QWT_DIR=deps\Qwt
QWT_INC_DIR=$(QWT_DIR)\include
QWT_LIB_DIR=$(QWT_DIR)\lib\$(PREF)
QWT_BIN_DIR=$(QWT_DIR)\bin\$(PREF)
## Boost
BOOST_DIR=deps\boost
BOOST_INC_DIR=$(BOOST_DIR)\include
BOOST_LIB_DIR=$(BOOST_DIR)\lib\$(PREF)
BOOST_BIN_DIR=$(BOOST_DIR)\bin\$(PREF)
## Gnuradio
GR_DIR=deps\gnuradio
GR_INC_DIR=$(GR_DIR)\include
GR_LIB_DIR=$(GR_DIR)\lib\$(PREF)
GR_BIN_DIR=$(GR_DIR)\bin\$(PREF)
## UHD
UHD_DIR=deps\UHD
UHD_INC_DIR=$(UHD_DIR)\include
UHD_LIB_DIR=$(UHD_DIR)\lib\$(PREF)
UHD_BIN_DIR=$(UHD_DIR)\bin\$(PREF)
## CPPUNIT
CPPUNIT_DIR=deps\cppunit
CPPUNIT_INC_DIR=$(CPPUNIT_DIR)\include
CPPUNIT_LIB_DIR=$(CPPUNIT_DIR)\lib\$(PREF)
## NaturalDocs
DOC_DIR=deps\doxygen\bin\$(PREF)
DOC_BIN=$(DOC_DIR)\doxygen

###############################

OUT_DIR=out
OBJ_DIR=$(OUT_DIR)
SRC_DIR=src
INC_DIR=include
MOD_DIR=modules
LAYOUT_DIR=$(SRC_DIR)/layouts
LAYOUT_INC=/I $(LAYOUT_DIR)/80211b /I $(LAYOUT_DIR)/VLC /I $(INC_DIR)/$(MOD_DIR)
TARGET=flexicom
TEST_DIR=test

QT_CORE_DIR=$(QT_INC_DIR)\QtCore
QT_GUI_DIR=$(QT_INC_DIR)\QtGui
QT_INC=/I $(QT_CORE_DIR) /I $(QT_GUI_DIR) /I $(QT_INC_DIR)
!if $(DEBUG) == 1
QT_LIB=/LIBPATH:$(QT_LIB_DIR) QtCore4.lib QtGui4.lib 
!else
QT_LIB=/LIBPATH:$(QT_LIB_DIR) QtCore4.lib QtGui4.lib 
!endif
QWT_INC=/I $(QWT_INC_DIR)
QWT_LIB=/LIBPATH:$(QWT_LIB_DIR) qwt.lib
!if $(CMDLINE) == 0
CMDLINE_LFLAG=/subsystem:windows
!else
CMDLINE_CFLAG=/D CMDLINE
!endif
BOOST_INC=/I $(BOOST_INC_DIR)
BOOST_LIB=/LIBPATH:$(BOOST_LIB_DIR)
GR_INC=/I $(GR_INC_DIR) /I $(GR_INC_DIR)/gnuradio /I $(GR_INC_DIR)/volk /D LV_HAVE_SSE2 /D LV_HAVE_GENERIC
GR_LIB=/LIBPATH:$(GR_LIB_DIR) gnuradio-core.lib gnuradio-uhd.lib
UHD_INC=/I $(UHD_INC_DIR)
UHD_LIB=/LIBPATH:$(UHD_LIB_DIR) uhd.lib
CPPUNIT_INC=/I $(CPPUNIT_INC_DIR)
CPPUNIT_LIB=/LIBPATH:$(CPPUNIT_LIB_DIR) cppunit.lib


INC_FILES = /I $(INC_DIR) $(QT_INC) $(QWT_INC) $(BOOST_INC) $(LAYOUT_INC) $(GR_INC) $(UHD_INC)
EXECFLAGS=$(DBGCFLAG) $(CFLAGS) $(INC_FILES) /Fo$(OBJ_DIR)/
CFLAGS=/J /D NOMINMAX /D _WIN /D _WINDOWS /D _CRT_NONSTDC_NO_DEPRECATE /D _CRT_SECURE_NO_DEPRECATE /nologo /EHsc /c $(CMDLINE_CFLAG) 
#user32.lib kernel32.lib ws2_32.lib winspool.lib shell32.lib gdi32.lib imm32.lib winmm.lib advapi32.lib
LFLAGS=$(DBGLFLAG) /LIBPATH:$(OBJ_DIR) user32.lib kernel32.lib /nologo $(CMDLINE_LFLAG) $(QT_LIB) $(QWT_LIB) $(BOOST_LIB) $(GR_LIB) $(UHD_LIB) \
	/INCREMENTAL:NO /MANIFEST 

!if $(IS64) == 1
CFLAGS = $(CFLAGS) /D _WIN64
LFLAGS = $(LFLAGS) /MACHINE:X64
!else
CFLAGS = $(CFLAGS) /D _WIN32 /arch:SSE2 /D _USE_32BIT_TIME_T 
!endif

!if $(DEBUG) == 1
DBGCFLAG=/MD /Zi /Od
DBGLFLAG=/debug
!else
DBGCFLAG=/MD /Zi /Ox /W1
DBGLFLAG=/debug
!endif

OBJ_FILES=$(OBJ_DIR)/MainWindow.obj $(OBJ_DIR)/MainWindow_moc.obj

MOD_FILES=$(OBJ_DIR)/QtBlock.obj $(OBJ_DIR)/QtBlock_moc.obj $(OBJ_DIR)/bbMatlab.obj 

LAYOUTS=$(OBJ_DIR)/Layout80211b.obj $(OBJ_DIR)/Layout80211b_moc.obj $(OBJ_DIR)/LayoutVLC.obj $(OBJ_DIR)/LayoutVLC_moc.obj \
		$(OBJ_DIR)/Rx80211b.obj $(OBJ_DIR)/RxVLC.obj $(OBJ_DIR)/TxVLC.obj \
		$(OBJ_DIR)/BBN_Slicer.obj $(OBJ_DIR)/BBN_DPSKDemod.obj $(OBJ_DIR)/BBN_PLCP.obj \
        $(OBJ_DIR)/bbManchesterEnc.obj $(OBJ_DIR)/bbManchesterDec.obj $(OBJ_DIR)/bb4b6bEnc.obj  $(OBJ_DIR)/vlc_reed_solomon.obj\
        $(OBJ_DIR)/bbRSEnc.obj $(OBJ_DIR)/bbVLCInterleaver.obj $(OBJ_DIR)/bbVLC_info_assembler.obj $(OBJ_DIR)/bbVLC_Frame_Generator.obj \
        $(OBJ_DIR)/vlc_convolutional_coding.obj $(OBJ_DIR)/bbCCEnc.obj $(OBJ_DIR)/PHY_I_modulator.obj $(OBJ_DIR)/vlc_crc.obj \
        $(OBJ_DIR)/bbPHR_generation.obj $(OBJ_DIR)/bbPSDU_generation.obj 
        
TEST_FILES=$(OBJ_DIR)/test.obj $(OBJ_DIR)/test_example.obj

all: exe install_deps
	$(RM) Makefile.auto
	$(MT) /nologo -outputresource:"$(TARGET).exe;1" -manifest $(TARGET).exe.manifest
	$(RM) $(TARGET).exe.manifest $(TARGET).map $(TARGET).exp

install_deps:
		$(COPY) $(QT_BIN_DIR)\QtGui4.dll QtGui4.dll	>nul
		$(COPY) $(QT_BIN_DIR)\QtCore4.dll QtCore4.dll >nul
		$(COPY) $(GR_BIN_DIR)\gnuradio-core.dll gnuradio-core.dll >nul
		$(COPY) $(GR_BIN_DIR)\gnuradio-uhd.dll gnuradio-uhd.dll >nul
		$(COPY) $(GR_BIN_DIR)\gruel.dll gruel.dll >nul
		$(COPY) $(GR_BIN_DIR)\volk.dll volk.dll >nul
		$(COPY) $(GR_BIN_DIR)\libfftw3f-3.dll libfftw3f-3.dll >nul
		$(COPY) $(UHD_BIN_DIR)\uhd.dll uhd.dll >nul
		$(COPY) $(QWT_BIN_DIR)\qwt.dll qwt.dll >nul
		
exe: objs $(OBJ_DIR)/main.obj
	$(LINK) $(LFLAGS) $(OBJ_FILES) $(MOD_FILES) $(LAYOUTS) $(OBJ_DIR)/main.obj /MAP /OUT:$(TARGET).exe
	
objs: $(OBJ_FILES) $(MOD_FILES) $(LAYOUTS)

clean:
	$(RM) "$(OBJ_DIR)\*.obj" 
	
{$(SRC_DIR)}.cc{$(OBJ_DIR)}.obj:
	@if not exist $(OUT_DIR) mkdir $(OUT_DIR)
	$(CC) $(EXECFLAGS) /Fd$(OBJ_DIR) $<
	
$(OBJ_DIR)/MainWindow_moc.obj: $(INC_DIR)/MainWindow.h
	$(MOC) $(INC_DIR)/MainWindow.h -o $(SRC_DIR)/MainWindow_moc.cc
	$(CC) $(EXECFLAGS) /Fo$(OBJ_DIR)/ /Fd$(OBJ_DIR) $(SRC_DIR)/MainWindow_moc.cc
	
#Modules
{$(SRC_DIR)/$(MOD_DIR)}.cc{$(OBJ_DIR)}.obj:
	$(CC) $(EXECFLAGS) /Fd$(OBJ_DIR) $<
$(OBJ_DIR)/QtBlock_moc.obj: $(INC_DIR)/$(MOD_DIR)/QtBlock.h
	$(MOC) $(INC_DIR)/$(MOD_DIR)/QtBlock.h -o $(SRC_DIR)/$(MOD_DIR)/QtBlock_moc.cc
	$(CC) $(EXECFLAGS) /Fo$(OBJ_DIR)/ /Fd$(OBJ_DIR) $(SRC_DIR)/$(MOD_DIR)/QtBlock_moc.cc

#Layouts
{$(LAYOUT_DIR)/80211b}.cc{$(OBJ_DIR)}.obj:
	$(CC) $(EXECFLAGS) /Fd$(OBJ_DIR) $<
$(OBJ_DIR)/Layout80211b_moc.obj: $(LAYOUT_DIR)/80211b/Layout80211b.h
	$(MOC) $(LAYOUT_DIR)/80211b/Layout80211b.h -o $(LAYOUT_DIR)/80211b/Layout80211b_moc.cc
	$(CC) $(EXECFLAGS) /Fo$(OBJ_DIR)/ /Fd$(OBJ_DIR) $(LAYOUT_DIR)/80211b/Layout80211b_moc.cc
	
{$(LAYOUT_DIR)/VLC}.cc{$(OBJ_DIR)}.obj:
	$(CC) $(EXECFLAGS) /Fd$(OBJ_DIR) $<
$(OBJ_DIR)/LayoutVLC_moc.obj: $(LAYOUT_DIR)/VLC/LayoutVLC.h
	$(MOC) $(LAYOUT_DIR)/VLC/LayoutVLC.h -o $(LAYOUT_DIR)/VLC/LayoutVLC_moc.cc
	$(CC) $(EXECFLAGS) /Fo$(OBJ_DIR)/ /Fd$(OBJ_DIR) $(LAYOUT_DIR)/VLC/LayoutVLC_moc.cc

#Test Suite
test: test_files
	$(LINK) $(LFLAGS) $(TEST_FILES) $(OBJ_FILES) $(MOD_FILES) $(LAYOUTS) $(CPPUNIT_LIB) /MAP /OUT:test_$(TARGET).exe
	test_$(TARGET).exe
	
test_files: $(TEST_FILES) objs

{$(TEST_DIR)}.cc{$(OBJ_DIR)}.obj:
	$(CC) $(EXECFLAGS) $(CPPUNIT_INC) /Fd$(OBJ_DIR) $<
	
#Doc suite
doc: docdep
	$(DOC_BIN) doc\doxy.conf
	
docdep: