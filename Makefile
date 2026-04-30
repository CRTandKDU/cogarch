CPP 		= g++

CFLAGS		= -I./include -I./include/cd -I./include/im
LFLAGS		= -I./lib
LIBS_DIR	= ./lib
LIBS_CD		=  $(LIBS_DIR)/cdcontextplus.dll   $(LIBS_DIR)/cd.dll # $(LIBS_DIR)/cdcairo.dll   $(LIBS_DIR)/cddirect2d.dll  $(LIBS_DIR)/cdgl.dll  $(LIBS_DIR)/cdim.dll  $(LIBS_DIR)/cdlua54.dll  $(LIBS_DIR)/cdluacairo54.dll  $(LIBS_DIR)/cdluacontextplus54.dll  $(LIBS_DIR)/cdluadirect2d54.dll  $(LIBS_DIR)/cdluagl54.dll  $(LIBS_DIR)/cdluaim54.dll  $(LIBS_DIR)/cdluapdf54.dll  $(LIBS_DIR)/cdpdf.dll
LIBS_IUP	= $(LIBS_DIR)/iup.dll $(LIBS_DIR)/iupcd.dll 
LIBS		=  $(LIBS_DIR)/cdcontextplus.dll $(LIBS_DIR)/iupcd.dll $(LIBS_DIR)/iup.dll $(LIBS_DIR)/cd.dll # ./lib/gdi32.dll ./lib/comdlg32.dll ./lib/comctl32.dll ./lib/uuid.dll ./lib/oleaut32.dll ./lib/ole32.dll

APIS_DIR	= C:/cygwin64/home/Moria/nxp
APIS_NXP	= $(APIS_DIR)/sign.o $(APIS_DIR)/rule.o $(APIS_DIR)/hypo.o $(APIS_DIR)/compound.o $(APIS_DIR)/engine.o $(APIS_DIR)/engine_dsl.o $(APIS_DIR)/loadkb.o

DSL_DIR		= C:/cygwin64/home/Moria
DSL_CFLAGS	= -D ENGINE_DSL -D ENGINE_DSL_HOWERJFORTH
DSL_LFLAGS      = $(DSL_DIR)/libcsv/libcsv_la-libcsv.o $(DSL_DIR)/embed-master/util.o -L$(DSL_DIR)/embed-master -lembed # -lm

CFLAGS_NXP	= -I$(APIS_DIR) -I$(DSL_DIR)/libforth -I$(DSL_DIR)/embed-master -I$(DSL_DIR)/libcsv

canvas3: canvas3.c netw.c
	$(CPP) $^ -o canvas3.exe -mwindows $(CFLAGS) $(DSL_CFLAGS) $(CFLAGS_NXP) $(LFLAGS) $(DSL_LFLAGS) $(LIBS) $(APIS_NXP)

canvas2: canvas2.c
	gcc canvas2.c -o canvas2.exe $(CFLAGS) $(LFLAGS) $(LIBS)

canvas1: canvas1.c
	gcc canvas1.c -o canvas1.exe $(CFLAGS) $(LFLAGS) $(LIBS)
