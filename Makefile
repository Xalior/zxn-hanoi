SDNAME := zxn-hanoi

MKDIR := mkdir -p

RM := rm -rf

CP := cp

ZIP := zip -r -q

INSTALLDIR := $(SDNAME)/HANOI

LIBSPRITE := zxnext_sprite
LIBSPRITE_INCLUDE := $(LIBSPRITE)/include
LIBSPRITE_LIB := $(LIBSPRITE)/lib/sccz80

LIBLAYER2 := zxnext_layer2
LIBLAYER2_INCLUDE := $(LIBLAYER2)/include
LIBLAYER2_LIB := $(LIBLAYER2)/lib/sccz80

DEBUGFLAGS := --list --c-code-in-asm

all: dirs binaries

dirs:
	$(MKDIR) $(INSTALLDIR)

binaries:
	zcc +zxn -vn -O3 -startup=1 -clib=new -m $(DEBUG) -lm \
	    -L$(LIBSPRITE_LIB) -lzxnext_sprite -I$(LIBSPRITE_INCLUDE) \
	    -L$(LIBLAYER2_LIB) -lzxnext_layer2 -I$(LIBLAYER2_INCLUDE) \
	    -pragma-include:zpragma.inc  -Cz"--clean --pages --fullsize" \
	    main.c -o zxn-hanoi/zxn-hanoi -create-app -subtype=sna
	$(CP) zxn-hanoi/zxn-hanoi.sna $(INSTALLDIR)

debug: DEBUG = $(DEBUGFLAGS)

debug: all

clean:
	$(RM) tmp zcc_opt.def zcc_proj.lst src/*.lis

install:
	$(CP) $(INSTALLDIR)/zxn-hanoi.sna /Volumes/airflash/HANOI
	$(CP) $(INSTALLDIR)/*.nxi /Volumes/airflash/HANOI
	$(CP) $(INSTALLDIR)/*.spr /Volumes/airflash/HANOI
