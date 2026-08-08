TOP = $(shell realpath ..)

GIT_HASH := $(shell git rev-parse --short HEAD 2>/dev/null || echo "unknown")

LIB = tec1

OBJS = array88.rel \
	display.rel \
	lcd.rel \
	scroll.rel \
	menu.rel \
	keypad.rel \
	delay.rel

CFLAGS = -mz80 \
	--Werror \
	--std-c99 \
	-I $(TOP)/include \
	-DHW_TEC_1G \
	-DGIT_HASH=\"$(GIT_HASH)\"

.PHONY: all
all: $(OBJS)
	sdar -rc $(LIB).lib $(OBJS)

%.rel : %.s
	sdasz80 -o $<

%.rel : %.c
	sdcc $(CFLAGS) -c $<

.PHONY: clean
clean:
	-rm *.lib
	-rm *.asm
	-rm *.lst
	-rm *.rel
	-rm *.sym
