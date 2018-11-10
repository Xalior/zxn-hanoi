NAME := zxn-hanoi

MKDIR := mkdir -p

RM := rm -rf

CP := cp

ZIP := zip -r -q

BUILD_DIR := $(NAME)

LIBSPRITE := zxnext_sprite
LIBSPRITE_INCLUDE := $(LIBSPRITE)/include
LIBSPRITE_LIB := $(LIBSPRITE)/lib/sccz80

LIBLAYER2 := zxnext_layer2
LIBLAYER2_INCLUDE := $(LIBLAYER2)/include
LIBLAYER2_LIB := $(LIBLAYER2)/lib/sccz80

DEBUGFLAGS := --list --c-code-in-asm

all: dirs binaries

dirs:
	$(MKDIR) $(BUILD_DIR)

binaries:
	zcc +zxn -vn -O3 -startup=1 -clib=new -m $(DEBUG) -lm \
	    -L$(LIBSPRITE_LIB) -lzxnext_sprite -I$(LIBSPRITE_INCLUDE) \
	    -L$(LIBLAYER2_LIB) -lzxnext_layer2 -I$(LIBLAYER2_INCLUDE) \
	    -pragma-include:zpragma.inc  -Cz"--clean --pages --fullsize" \
	    main.c -o zxn-hanoi/zxn-hanoi -create-app -subtype=sna
	$(CP) dist/* $(BUILD_DIR)/.

debug: DEBUG = $(DEBUGFLAGS)

debug: all

clean:
	$(RM) $(BUILD_DIR) tmp zcc_opt.def zcc_proj.lst src/*.lis

install:
	$(MKDIR) /Volumes/airflash/$(NAME)
	$(CP) $(BUILD_DIR)/zxn-hanoi.sna /Volumes/airflash/$(NAME)
	$(CP) $(BUILD_DIR)/*.nxi /Volumes/airflash/$(NAME)
	$(CP) $(BUILD_DIR)/*.spr /Volumes/airflash/$(NAME)

emulate:
	mono /u/ZX/CSpect2_1_1/CSpect.exe -sound -r -w4 -zxnext -mmc=$(BUILD_DIR)/ $(BUILD_DIR)/$(NAME).sna