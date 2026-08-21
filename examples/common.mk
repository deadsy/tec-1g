TOP = $(shell realpath ../..)

TARGET = out

STACK ?= 0x7ffe
CODE ?= 0x4000
DATA ?= 0x6000

OBJS = entry.rel main.rel

LIBS = tec1.lib

CFLAGS = -mz80 \
	--Werror \
	--std-c99 \
	-I $(TOP)/include \
	-DHW_TEC_1G

LFLAGS = -mz80 \
	--no-std-crt0 \
	--code-loc $(CODE) \
	--data-loc $(DATA) \
	-L $(TOP)/lib

.PHONY: all
all: $(OBJS)
	sdcc $(LFLAGS) $(OBJS) $(LIBS) -o $(TARGET).ihx

%.rel : %.s
	sdasz80 -o $<

%.rel : %.c
	sdcc $(CFLAGS) -c $<

.PHONY: clean
clean:
	-rm $(TARGET).lk
	-rm $(TARGET).map
	-rm $(TARGET).noi
	-rm *.rel
	-rm *.lst
	-rm *.sym
	-rm *.asm
